// Domino puzzle from Stu Savory (http://www.savory.de/blog_feb_20.htm#20200203)
// Arrange 8 dominoes in a hollow square so the sums of top and bottom rows and
// the left and right columns all have the same value.
// February 2020 Pergelator

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define dimensionof(p)  (sizeof(p)/sizeof(p[0]))
int		ECHO = 1;
#define echo(  p, q, s) if (ECHO ) fprintf(stdout, #p ": %" #q s, p)
#define EOL     "\n"

// For amusement only
int	 counter	 = 0;
int	 skipped	 = 0;
int  solved		 = 0;
int  failed		 = 0;

// Essential data
#define	PERMUTE_LIMIT	dimensionof(tiles)
int		mask;				// available positions in array permute
char	permute[9];			// tile placement order
int 	tiles[8][2] =		// our dominos
{
	4, 4,				// this is our only double tile
	6, 3,
	4, 6,
	6, 2,
	4, 5,
	5, 1,
	5, 6,
	5, 3,
};
int   block  [5][5];		// rows by columns

// There are four sides to the block. The center nine cells are not used.
// Each side has two and a half tiles. We number the tiles in a clockwise manner.
// Tile 7 occupies the top two cells of the leftmost column.
// Tile 0 is the leftmost horizontal tile in the top row.

// Tiles can have two orientations: 0 or 1, controlled by bitmask 'thisend'.
// When a tile is placed in the arrangement, if a tile has normal orientation,
// the number of dots are placed in normal order,
// i.e. tiles[i][0] is used first and then tiles[i][1].
// Otherwise dots are applied in reverse order.

int show_board(void)
{
	char* board[] =						// for displaying the block
	{
			"╔═══╦═══╤═══╦═══╤═══╗",
			"║ X ║ X │ X ║ X │ X ║",	// X's are replaced by the number of dots
			"╟───╬═══╧═══╩═══╬═══╣",
			"║ X ║ X   X   X ║ X ║",
			"╠═══╣           ╟───╢",
			"║ X ║ X   X   X ║ X ║",
			"╟───╢           ╠═══╣",
			"║ X ║ X   X   X ║ X ║",
			"╠═══╬═══╦═══╤═══╬───╢",
			"║ X │ X ║ X │ X ║ X ║",
			"╚═══╧═══╩═══╧═══╩═══╝",
	};

	char line[64];	// longest lines: (5 cells x 4 spaces) x 1 for the last char) x 3 bytes for each character)
					//  + 1 for terminating zero = 64 bytes.

	assert( ( strlen(board[0]) + 1 ) <= sizeof(line) );
	for (int i=0; i<dimensionof(board); i++)
	{
		strcpy(line, board[i]);
		if (i & 1)
		{
			char* p = line;
			int   k = (i >> 1);
			for (int j=0; j<5; j++)
			{
				p = strchr(p, 'X');			// Replace X's with digits
				assert(p!=NULL);
				*p = block[k][j] + '0';
			}
		}
		printf("%s" EOL, line);
	}
}		

void init_block(void)		// Do this so the inner 9 cells print as blanks
{
	for (int i=0; i<dimensionof(block); i++)
		for (int j=0; j<dimensionof(block[0]); j++)
			block[i][j] = -16;			// adding '0' (0x30) gives us a space
}

int sum_pips(void)
{
	int	rc[2] = { 0, 4 };
	int ix = 0;
	int	sum[4];

	counter++;
	for (int i=0; i<2; i++)
	{
		sum[ix] = 0;
		for (int k=0; k<dimensionof(block[0]); k++)
			sum[ix] += block[ k ][ rc[i] ];
		ix++;

		sum[ix] = 0;
		for (int k=0; k<dimensionof(block[0]); k++)
			sum[ix] += block[ rc[i] ][ k ];
		ix++;
	}
	for (int i=1; i<4; i++)
		if (sum[0] != sum[i])		// FAIL
		{
			failed++;
			return -1;
		}

	solved++;

	echo(counter, d, "  ");
	echo(skipped, d, "  ");
	echo(solved,  d, "  ");
	echo(permute, s, "  ");
	printf("Sum: %d" EOL, sum[0]);
	show_board();
	return sum[0];	
}

int fill_block(int swapmask)
{
// Fill in block using order specifed by 'permute'. Do this the same way for every permutation.

/*	Initial placement of tiles on board (block).
	First number is index into permute,
	second number specifies which end of the tile.

		╔═════╦═════╤═════╦═════╤═════╗
		║ 7,1 ║ 0,0 │ 0,1 ║ 1,0 │ 1,1 ║
		╟─────╬═════╧═════╩═════╬═════╣
		║ 7,0 ║                 ║ 2,0 ║
		╠═════╣                 ╟─────╢
		║ 6,1 ║                 ║ 2,1 ║
		╟─────╢                 ╠═════╣
		║ 6,0 ║                 ║ 3,0 ║
		╠═════╬═════╦═════╤═════╬─────╢
		║ 5,1 │ 5,0 ║ 4,1 │ 4,0 ║ 3,1 ║
		╚═════╧═════╩═════╧═════╩═════╝

*/
// The orientation of the tile is only relevant in the corner positions.
// There are only four corners, so we only need four bits in our swap mask.
// Corner postions use odd indices from 'permute'.
// Dividing the index (n) by two gets us the number
// of bits to shift to get the correct mask bit.

#define	first(n)	tiles[ permute[n] - '0' ][ ((swapmask >> (n >> 1)) & 1)     ];
#define	second(n)	tiles[ permute[n] - '0' ][ ((swapmask >> (n >> 1)) & 1) ^ 1 ];
#define	any0(n)		tiles[ permute[n] - '0' ][					0				];
#define	any1(n)		tiles[ permute[n] - '0' ][					1				];

// row x column
	block[0][0] = second(7);
	block[0][1] =   any0(0);	// order irrelevent
	block[0][2] =   any1(0);	// order irrelevent
	block[0][3] =  first(1);
	block[0][4] = second(1);

	block[1][0] =  first(7);
//	block[1][1]
//	block[1][2]
//	block[1][3]
	block[1][4] =   any0(2);	// order irrelevent

	block[2][0] =   any1(6);	// order irrelevent
//	block[2][1]
//	block[2][2]
//	block[2][3]
	block[2][4] =   any1(2);	// order irrelevent

	block[3][0] =   any0(6);	// order irrelevent
//	block[3][1]
//	block[3][2]
//	block[3][3]
	block[3][4] =  first(3);

	block[4][0] = second(5);
	block[4][1] =  first(5);
	block[4][2] =   any1(4);	// order irrelevent
	block[4][3] =   any0(4);	// order irrelevent
	block[4][4] = second(3);

	return 1;
}

int double_reversi(void)		// are any of the corner tiles doubles?
{
	int doublemask	= 0;
	int bit			= 1;					// start with low order bit
	for (int i=1; i<8; i+=2)				// for all corner positions
	{
		int k = permute[i] - '0';			// get tile number
		if	(tiles[k][0] == tiles[k][1])	// and the assigned tile is a double
			doublemask |= bit;

		bit <<= 1;
	}
	return doublemask;
}

void permute_xor(int x)		// x is tile number
{
	char bmask[9];

	if (x >= PERMUTE_LIMIT)
	{
		assert(mask==0);
		permute[PERMUTE_LIMIT] = 0;		// terminate string
		int doublemask = double_reversi();
		for (int swapmask=0; swapmask<0x10; swapmask++)
		{
			if (doublemask & swapmask)
			{
				skipped++;
				continue;
			}

			fill_block(swapmask);
			sum_pips();
		}
	}
	else
	{
		int bit = 1;
		for (int i=0; i<PERMUTE_LIMIT; i++)
		{		
			if (mask & bit)				// this slot is available
			{
				mask ^= bit;			// mark slot as used
				permute[i]	= x + '0';
				permute_xor(x+1);
				mask ^= bit;			// mark slot as free
			}
			bit <<= 1;
		}
	}
}

void main1(int n)
{
	#define PERMUTE_MASK	((1 << PERMUTE_LIMIT ) - 1)		// gives us a binary value compose of eight 1's.

	permute[n] = '0';
	mask = PERMUTE_MASK ^ (1 << n);		// Mark position as filled
	echo(mask, x, EOL);
	permute_xor( 1);

	printf("   Total solutions found: %9d  " EOL, solved);
	printf("          Total failures: %9d  " EOL, failed);
	printf("Total permutations tried: %9d  " EOL, counter);
	assert(counter==(solved+failed));
}

int main(int argc, char** argv)
{
	printf("Puzzle Initial Arrangement" EOL);
	strcpy(permute, "01234567");
	init_block();
	fill_block(0);
	show_board();

/* Since the block is square, any solution can be rotated to any of four positions.
   We only need to find one of these four solutions. We do this by ensuring tile 0 
   is placed in the first side. First, by placing in the first position, and then 
   in the second position.
*/
	main1(0);
	main1(1);
	return 0;
}

