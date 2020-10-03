char* title = "Cube Puzzle Solver";
// The cube is 4 units on a side so it has 64 spots. There are ten puzzle 
// pieces. They are all one unit thick. Each piece is defined as an ASCII 
// string of 16 charcters. Places occupied by the piece are indicated with a 
// capital letter. Spaces are used for unoccupied spots. We use a 64 bit 
// unsigned integer to indicate which places in the cube are filled. We use a 
// 64 character ASCII string to show where a particular piece can been placed.
// We use that string to generate a list of 64 bit integers, one for each 
// possible placement of a piece in the cube. Then we go through this list 
// and'ing and or'ing these integers to find a combination that will fill all 
// the spots without impinging on any other pieces. When we have that, we use 
// the current indices of all our permutations to mark the places in the 64 
// character ASCI string, which we then use to display the solution.

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define	dimensionof(p)	(sizeof(p)/sizeof(p[0]))
#define EOL "\n"

#define ulong unsigned long

char*	pieces[10] =	// commas separate invidual pieces
{
	"  AA"				// quoted strings on succesive lines without commas are concatenated into a single string
	"  A "
	"  AA"
	"   A",	"  BB"
			"  B "
			"  BB"
			"  BB",	"  CC"
					"  C "
					"  CC"
					"  C ",	"  D "
							"  D "
							"  DD"
							"  DD",	"  E "
									"  EE"
									"  EE"
									"  EE",
	"    "
	" FF "
	" FF "
	" FFF",	"    "
			"  G "
			" GG "
			" GGG",	"    "
					" HH "
					"  HH"
					"  HH",	"    "
							" III"
							"  I "
							"  II",	"    "
									" JJJ"
									" JJ "
									"  JJ"
};

void shiftleft(char* b, char* piece)	// shift piece one place to left inside the 4 x 4 frame
{
	int one		= 1;	// indexes to column one in piece
	int	two		= 5;
	int three	= 9;
	int four	= 13;
						// copy three characters, pad the line to make four characters

	for (int i=0; i<3; i++)	*b++ = piece[  one++];		*b++ = ' ';
	for (int i=0; i<3; i++)	*b++ = piece[  two++];		*b++ = ' ';
	for (int i=0; i<3; i++)	*b++ = piece[three++];		*b++ = ' ';
	for (int i=0; i<3; i++)	*b++ = piece[ four++];		*b++ = ' ';
	*b = 0;
}

void shiftup(char* b, char* piece)	// shift piece one place up inside frame
{
	piece += 4;					// skip first row which is all blanks
	for (int i=0; i<12; i++)
		*b++ = *piece++;
	for (int i=0; i<4; i++)		// fill bottom row with blanks
		*b++ = ' ';

	*b = 0;
}

void rotate(char* b, char* piece)	// 90 degree anti-clockwise
{
	int one		= 3;
	int	two		= 7;
	int three	= 11;
	int four	= 15;
	for (int i=0; i<4; i++)
	{
		*b++ = piece[  one--];
		*b++ = piece[  two--];
		*b++ = piece[three--];
		*b++ = piece[ four--];
	}
	*b = 0;
}

void reverse(char* b, char* piece)		// left to right
{
	int one		= 3;
	int	two		= 7;
	int three	= 11;
	int four	= 15;
	for (int i=0; i<4; i++)	*b++ = piece[  one--];
	for (int i=0; i<4; i++)	*b++ = piece[  two--];
	for (int i=0; i<4; i++)	*b++ = piece[three--];
	for (int i=0; i<4; i++)	*b++ = piece[ four--];
	*b = 0;
}

ulong mangle(char* piece, int rotation, int flip, int axis, int level, int vertical, int horizontal)
{
	char frame[2][17];
	strcpy(frame[0], piece);
	int a = 0;
	int b = 1;
	for (int i=0; i<horizontal; i++)
	{
		shiftleft(frame[b], frame[a]);
		a = (a + 1) & 1;
		b = (b + 1) & 1;
	}
	for (int i=0; i<vertical; i++)
	{
		shiftup(frame[b], frame[a]);
		a = (a + 1) & 1;
		b = (b + 1) & 1;
	}
	for (int i=0; i<rotation; i++)
	{
		rotate(frame[b], frame[a]);
		a = (a + 1) & 1;
		b = (b + 1) & 1;
	}
	for (int i=0; i<flip; i++)
	{
		reverse(frame[b], frame[a]);
		a = (a + 1) & 1;
		b = (b + 1) & 1;
	}

	char buffer[65];
	for (int i=0; i<64; i++)
		buffer[i] = ' ';
	switch(axis)
	{
	case 0:	strncpy(&buffer[ level * 16		], frame[a], 16);
	break;

	case 1:	strncpy(&buffer[  0 + (level* 4)], &frame[a][ 0], 4);
			strncpy(&buffer[ 16 + (level* 4)], &frame[a][ 4], 4);
			strncpy(&buffer[ 32 + (level* 4)], &frame[a][ 8], 4);
			strncpy(&buffer[ 48 + (level* 4)], &frame[a][12], 4);
	break;

	case 2:		// orthogonal
			for (int i=0; i<16; i++)
				buffer[(i * 4) + level] = frame[a][i];
	break;
	}
	buffer[64] = 0;
	ulong	mask	= 1;
	ulong	result	= 0;
	for (int i=0; i<64; i++)
	{
		if (isalpha(buffer[i]))
			result |= mask;
		mask <<= 1;
	}
	return result;
}

int showframes(int n, char** pptr)
{
	char*	boxtop  = "    ╔═╤═╤═╤═╗";
	char*	boxwall	= "    ║ │ │ │ ║";
	char*	boxbot	= "    ╚═╧═╧═╧═╝";
	char*	vbar[]	= { "│", "│", "│", "║" };
	int 	count	= 0;

	for (int i=0; i<n; i++)
		count += printf("%s", boxtop);
	count += printf(EOL);

	for (int j=0; j<4; j++)			// rows
	{
		for (int i=0; i<n; i++)		// frames
		{
			printf("    ║");
			for (int k=0; k<4; k++)					// characters
				count += printf("%c%s", *pptr[i]++, vbar[k]);
		}
		count += printf(EOL);
	}
	for (int i=0; i<n; i++)
		count += printf("%s", boxbot);
	count += printf(EOL);

	return count;
}

int showCube(char* Acube)
{
	char*	pptr[4] = { Acube, &Acube[16], &Acube[32], &Acube[48] };
	return showframes( 4, pptr);
}

#define	ZILLION	1000000						// one million
int		limit0 = 12;	// piece 0 can be shifted to 3 positions in any of the 4 layers.
						// All other positions are just rotations or mirror images.
int		limit1;			// 96 * 3 = 288
int		limit2;			// 96 * 4 = 384
ulong	mask[10][400];
char	solution[65];
long	counter = ZILLION;
int		millions = 0;

void place(int px, int i)
{
	//				 0123456789abcdef
	ulong onebit = 0x8000000000000000;
	for (int j=0; j<64; j++)
	{
		if (mask[px][i] & onebit)
			solution[j] = 'A' + px;
		onebit >>= 1;
	}
}

ulong plunge(int px, ulong maskx)
{
	if (px>9)
		return 1;

	counter--;
	if (counter<=0)
	{
		millions++;
		fprintf(stderr, ".");
		counter = ZILLION;
	}

	int limit;
	if (px<5)	limit = limit1;
	else		limit = limit2;
	for (int i=0; i<limit; i++)
		if ((maskx & mask[px][i])==0)
		{
			if (plunge(px+1, maskx | mask[px][i]))
			{
				place(px, i);
				return 1;
			}
		}

	return 0;
}

int main(void) 
{
	printf("%s" EOL, title);
	assert(sizeof(ulong)>=8);

	int	hlimit	= 3;		// first group of five pieces are 4 units tall and 2 units wide
	int	vlimit	= 1;		// so the can be shifted up to two places horizontally
	int index;
	for (int i=0; i<dimensionof(pieces); i++)
	{
		if (i==5)		// two groups of 5 pieces
		{
			vlimit = 2;		// second group of five pieces are 3 units tall by 3 units wide
			hlimit = 2;		// they can shifted one unit up and one unit across
			limit1 = index + 1;
		}
		for (int vertical=0; vertical<vlimit; vertical++)
			for (int horizontal=0; horizontal<hlimit; horizontal++)
				for (int rotation=0; rotation<4; rotation++)
					for (int flip=0; flip<2; flip++)
						for (int axis=0; axis<3; axis++)
							for (int layer=0; layer<4; layer++)
							{
								index	= rotation	* ( 2 * 3 * vlimit*	hlimit * 4)
										+ flip		* (		3 * vlimit*	hlimit * 4)
										+ axis		* (			vlimit*	hlimit * 4)
										+ vertical	* (					hlimit * 4)
										+ horizontal* (							 4)
										+ layer;
								mask[i][index] = mangle(pieces[i], rotation, flip, axis, layer, vertical, horizontal);
							}

	}
	limit2 = index + 1;

	printf(EOL "Pieces:" EOL);
	showframes(5, pieces);
	showframes(5, &pieces[5]);

	int Solutions = 0;
	for (int i=0; i<limit0; i++)
	{
		fprintf(stderr, EOL "Zillions: ");
		if (plunge(1, mask[0][i]))
		{
			place(0, i);
			printf(EOL "Comparisons: %ld" EOL, (millions * ZILLION) + (ZILLION - counter));
			printf(EOL "Solution:" EOL);
			solution[64] = 0;
			showCube(solution);
			Solutions++;
		}
	}
	printf("Solutions: %d" EOL, Solutions);

	return 0;
}
