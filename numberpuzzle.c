/*	numberpuzzle.c
	Chuck Pergiel
	August 4, 2021

Program to solve jigsaw puzzle composed the decimal digits.

						 <->
						^   ^
						|   |
						v   v
						 <->
						^   ^
						|   |
						v   v
						 <->

	Crude illustration of seven-segment display

In this puzzle, numeric digits are modeled after the seven segment displays used in some 
electronic displays. In this puzzle, not all of the segments are complete. Each full segment is a 
short bar with a triangular point on each end. We will use three characters to represent each 
segment. Each full segment will have all three characters set. Incomplete segments will only have 
one or two bits set.

Each digit is modeled in a 7 by 11 grid. The 11 vertical spaces are for 2 segments of 3 characters 
each plus 3 horizontal bars, each 1 character high, plus a blank row at the top and bottom to 
accomodate the odd bits in some digits when they are rotated.

			 ((2 x 3) + 3 + 2) = 11

Each row has two spaces for the vertical bars and 3 characters for the horizontal bars, and then 
one extra space at each end to accomodate filler bits.

				2 + 3 + 2 = 7
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define dimensionof(p)	(sizeof(p)/sizeof(p[0]))
#define	EOL		"\n"

// Allocate a 12 x 12 space for each digit so we can accomodate digits in either vertical or horizontal orientation.
// (11 spaces for characters and one space for the trailing zero used to terminate C strings.)

char	bufferz	[ 2 * 4 * 10 * 12 * 12];
char*	rotatez	[ 2][ 4][ 10][ 11];
char*	digits			[ 10][ 11] =
{
	"       ",
	"  000  ",
	" 0   0 ",
	" 0   0 ",
	" 0   0 ",
	"      0",
	"     0 ",
	" 0   0 ",
	" 0   0 ",
	"  000  ",
	"       ",	"       ",
				"       ",
				"     1 ",
				"     1 ",
				"     1 ",
				"      1",
				"     1 ",
				"     1 ",
				"     1 ",
				"       ",
				"       ",	"       ",
							"  222  ",
							"     2 ",
							"     2 ",
							"     2 ",
							"  222  ",
							" 2     ",
							" 2     ",
							" 2     ",
							"  222  ",
							"       ",	"       ",
										"  333  ",
										"     3 ",
										"     3 ",
										"     3 ",
										"  333  ",
										"     3 ",
										"     3 ",
										"     3 ",
										"  333  ",
										"       ",	"       ",
													"       ",
													" 4   4 ",
													" 4   4 ",
													" 4   4 ",
													"  444  ",
													"     4 ",
													"     4 ",
													"     4 ",
													"       ",
													"       ",
	"       ",
	"  555  ",
	" 5     ",
	" 5     ",
	" 5     ",
	"  555  ",
	"     5 ",
	"     5 ",
	"     5 ",
	"  555  ",
	"       ",	"       ",
				"  666  ",
				" 6     ",
				" 6     ",
				" 6     ",
				"  666  ",
				" 6   6 ",
				" 6   6 ",
				" 6   6 ",
				"  666  ",
				"       ",	"       ",
							"  777  ",
							"     7 ",
							"     7 ",
							"     7 ",
							"      7 ",
							"     7 ",
							"     7 ",
							"     7 ",
							"       ",
							"       ",	"       ",
										"  888  ",
										" 8   8 ",
										" 8   8 ",
										" 8   8 ",
										"  888  ",
										" 8   8 ",
										" 8   8 ",
										" 8   8 ",
										"  888  ",
										"       ",	"       ",
													"  999  ",
													" 9   9 ",
													" 9   9 ",
													" 9   9 ",
													"  999  ",
													"     9 ",
													"     9 ",
													"     9 ",
													"  999  ",
													"       "
};
	char* field[] =
{
	" |   |   |   |   |   | ",
	"-x---x---x---x---x---x-",
	" |   |   |   |   |   | ",
	" |   |   |   |   |   | ",
	" |   |   |   |   |   | ",
	"-x---x---x---x---x---x-",
	" |   |   |   |   |   | ",
	" |   |   |   |   |   | ",
	" |   |   |   |   |   | ",
	"-x---x---x---x---x---x-",
	" |   |   |   |   |   | ",
	" |   |   |   |   |   | ",
	" |   |   |   |   |   | ",
	"-x---x---x---x---x---x-",
	" |   |   |   |   |   | ",
	" |   |   |   |   |   | ",
	" |   |   |   |   |   | ",
	"-x---x---x---x---x---x-",
	" |   |   |   |   |   | "
};

int		mirror[10]	= { 0 };
int		rotate[10]	= { 0 };
int		solutions	= 0;
long	count1		= 0;		// progress display
long	count2		= 0;		// progress display
#define	REFILL		100000		// one hundred thousand

int print10(char* label, int* values)
{
	int count = 0;
	count += printf(EOL "%s :", label);
	for (int i=0; i<10; i++)
		count += printf("%2d", values[i]);

	return count;
}

int show_field(char** fieldx)
{
	int count = 0;
	count += print10("Mirror: ", mirror);
	count += print10("Rotate: ", rotate);
//	count += printf(EOL "Mirror: %d   Rotate: %d   Solutions: %d", mirror, rotate, solutions);
	count += printf(EOL "Solutions: %d", solutions);
	for (int i=0; i<dimensionof(field); i++)
		count += printf(EOL "%s", fieldx[i]);

	count += printf(EOL);
	count += printf(EOL);
	return count;
}

// Place pieces in first available location. If we fail, back up one step and move piece to next available location. Repeat until complete.
// Digits are 11 characters tall by  7 character  wide.
// Field is   19    rows    tall by 23 characters wide

int test_placement(char** fieldx, char** digitx, int x, int y)
{
	int width	=	   strlen( digitx[0]	);
	int height	= dimensionof( digits[0]	);
	if (width  == dimensionof( digits[0]	))	// if width = original height
		height	=	   strlen( digits[0][0]	);	// digit has been rotated

	for (int i=0; i<width; i++)
		for (int j=0; j<height; j++)
			if	(	isdigit( digitx[j][i] )
				&&	(fieldx[y+j][x+i]!='-')
				&&	(fieldx[y+j][x+i]!='|')
				)
				return 0;

	for (int i=0; i<width; i++)
		for (int j=0; j<height; j++)
			if (isdigit( digitx[j][i] ))
				fieldx[y+j][x+i] = digitx[j][i];
	
	return 1;
}

int down_one(int digit, char** field1)
{
	if (--count1<0)
	{
		fprintf(stderr, "*");		// progress indicator
		count1 = REFILL;
		count2++;
	}

	char	buffer[ dimensionof(field) * ( sizeof(field[0]) + strlen(field[0]) + 1 )];			// Make copy of field1
	memcpy(	buffer, field1, sizeof(buffer));

			// digit:	0	1	2	3	4	5	6	7	8	9
	int	flimit[] = {	2,	1,	2,	1,	2,	2,	2,	2,	1,	2	}; // mirror (2) or not (1)

	for (mirror[digit]=0; mirror[digit]<flimit[digit]; mirror[digit]++)		// mirror images (flipped)
	{
				// digit:	0	1	2	3	4	5	6	7	8	9
		int	klimit[] = {	4,	4,	2,	4,	4,	2,	4,	4,	2,	4	}; // number of useful rotations

		for (rotate[digit]=0; rotate[digit]<klimit[digit]; rotate[digit]++)		// rotations
		{
			int	xl[]	= { 5, 4, 5, 4 };	// positions in field
			int	yl[]	= { 3, 4, 3, 4 };	// indices 0 and 3 are for vertical orientaion, 1 & 4 are for horizontal
			int	ilimit	= xl[ rotate[digit] ];
			int	jlimit	= yl[ rotate[digit] ];
			if (digit == 0)
			{
				ilimit = (ilimit / 2) + (ilimit & 1);	// round up
				jlimit = (jlimit / 2) + (jlimit & 1);
			}

			for (int i=0; i<ilimit; i++)		// horizontal positions
			{
				int x = i * 4;

				for (int j=0; j<jlimit; j++)		// vertical positions
				{
					int y = j * 4;
			
					if (test_placement(field1, rotatez[ mirror[digit] ][ rotate[digit] ][digit], x, y))
					{
						if (digit==9)
						{
							solutions++;				// We found a solution
							show_field(field1);
							return 1;
						}
						down_one(digit + 1, field1);
						memcpy(field1, buffer, sizeof(buffer));		// Restore field1
					}
				}
			}
		}
	}
	return 0;
}

char** flip_digit(char** flipped, char** digitx, int lx, int ly)
{
	for (int j=0; j<ly; j++)		// create mirror image
		for (int i=0; i<lx; i++)
			flipped[j][lx-1-i] = digitx[j][i];

	return flipped;
}

char** rotate_digit(char** rotate, char** digitx, int lx, int ly)
{	// translate character array from 7 across by 9 tall to 9 across by 7 tall
	// or vice versa, depending on lx and ly

	for (int j=0; j<ly; j++)
		for (int i=0; i<lx; i++)
			rotate[i][ly-1-j] = digitx[j][i];

	return rotate;
}

int	lx[] = {  7, 11,  7, 11 };
int ly[] = { 11,  7, 11,  7 };

int rotate_all(void)
{
	memset(bufferz,		 0, sizeof(bufferz));

#define A	 dimensionof(rotatez		 )		// 2	mirror images
#define B	 dimensionof(rotatez[0]		 )		// 4	rotated positions
#define C	 dimensionof(rotatez[0][0]	 )		// 10	number of characters
#define D	 dimensionof(rotatez[0][0][0])		// 11	height of character
#define	E	(dimensionof(rotatez[0][0][0]) + 1)	// 12	width of rotated character

	for (int f=0; f<A; f++)
		for (int k=0; k<B; k++)							// fill rotatez with pointers into bufferz
			for (int i=0; i<C; i++)
				for (int j=0; j<D; j++)
					rotatez[f][k][i][j] = &bufferz[(f * B * C * D * E) + (k * C * D * E) + (i * D * E) + (j * E)];

	for (int i=0; i<dimensionof(digits); i++)			// fill first rank of rotatez[0] with copy of digits
		for (int j=0; j<dimensionof(digits[0]); j++)
			strcpy(rotatez[0][0][i][j], digits[i][j]);

	for (int i=0; i<dimensionof(digits); i++)			// fill first rank of rotatez[1] with mirror images of digits
		flip_digit(rotatez[1][0][i], rotatez[0][0][i], lx[0], ly[0]);

	for (int f=0; f<2; f++)
		for (int k=0; k<3; k++)			// fill successive ranks of rotatez with successively rotated versions of digits
			for (int i=0; i<10; i++)
				rotate_digit(rotatez[f][k+1][i], rotatez[f][k][i], lx[k], ly[k]);
}

char* commafy(char* s, long n)	// prints long int directly into given buffer, left to right
{
	char*	ptr	= s;	// save 
	long	x	= n;
	long	d	= 1;	// divisor
	while (x>=1000)
	{
		x /= 1000;
		d *= 1000;
	}

	ptr+= sprintf(ptr, "%ld", x);
	while (d > 1)
	{
		n %= d;
		d /= 1000;
		x  = n / d;
		ptr+= sprintf(ptr, ",%03ld", x);
	}
	return s;	
}

int main(int argc, char** argv)
{
	rotate_all();

	struct
	{
		char*	p[ dimensionof(field) ];
		char	s[ dimensionof(field) ][ strlen(field[0])+1 ];
	}	f;

	for (int i=0; i<dimensionof(field); i++)	// Make writable copy of field
	{
		f.p[i] = f.s[i];
		strcpy(f.p[i], field[i]);
	}

	down_one(0, (char**)&f);

	printf(EOL "solutions: %d", solutions);

	char buffer[32];
	printf( EOL "calls: %s" EOL, commafy(buffer, (count2 * REFILL) + REFILL - count1));
	return 0;
}
