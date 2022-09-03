char* title = "Numbrix";
char* help	= "Program to solve Numbrix puzzles found in the Parade Sunday "
				"newspaper supplement.";

/*
A Numbrix puzzle consists of a nine by nine grid that is to be filled with the 
numbers one through 81 in sequence. Each consequecutive number must be 
adjacent to the previous number, either horizontally or vertically. Diagonal 
moves are not permitted. Some of the values are given. The task is to fill in 
the rest of the board following the rules.

We have two arrays:
	- board		our working copy of the board
	- available	contains one slot for each of the 81 possible values. When a 
				square is assigned a value, the slot corresponding to that 
				value is cleared.
*/

#include <stdio.h>

#define	dimensionof(p)	(sizeof(p)/sizeof(p[0]))
#define	EOL				"\n"						// End Of Line

#define	DIMENSION		9
#define	LIMIT			(DIMENSION * DIMENSION)

int		calls = 0;
int		available	[LIMIT + 1];
int		board		[DIMENSION][DIMENSION] = 

{
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	 0,	 0,	 1,	 0,	63,	 0,	45,	 0,	 0,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	 0,	 0,	77,	 0,	 0,	 0,	23,	 0,	 0,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	 0,	 0,	 9,	 0,	19,	 0,	31,	 0,	 0,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0
};
/* 
{
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,		// Blank for future trials
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0
};
{
	73,	 0,	 0,	 0,	81,	 0,	 0,	 0,	35,	// Added this one August 14, 2022
	 0,	69,	 0,	 0,	80,	 0,	 0,	33,	 0,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	61,	64,	 0,	 0,	 0,	 0,	 0,	26,	27,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	 0,	51,	 0,	 0,	 2,	 0,	 0,	15,	 0,
	53,	 0,	 0,	 0,	 1,	 0,	 0,	 0,	17
};
{
	75,	 0,	79,	 0,	81,	 0,	 7,	 0,	11,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	73,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	13,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	63,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	19,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	59,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	25,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	55,	 0,	53,	 0,	49,	 0,	33,	 0,	27
};

{
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	 0,	 1,	 0,	15,	 0,	71,	 0,	77,	 0,
	 0,	 0,	11,	 0,	19,	 0,	75,	 0,	 0,
	 0,	 9,	 0,	 0,	 0,	 0,	 0,	67,	 0,
	 0,	 0,	25,	 0,	 0,	 0,	51,	 0,	 0,
	 0,	29,	 0,	 0,	 0,	 0,	 0,	63,	 0,
	 0,	 0,	31,	 0,	43,	 0,	53,	 0,	 0,
	 0,	37,	 0,	41,	 0,	47,	 0,	59,	 0,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0
};
{
//	18,	 0,	13,	 0,	19,	 0,	25,	 0,	27,		// NO SOLUTION !?!
	11,	 0,	13,	 0,	19,	 0,	25,	 0,	27,		// Corrected On-line
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	 7,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	33,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	81,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	49,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	79,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	53,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	75,	 0,	73,	 0,	63,	 0,	59,	 0,	55
};
Test Puzzle #1
{
	31,	 0,	29,	 0,	23,	 0,	 5,	 0,	 7,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	35,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 9,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	69,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	15,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	71,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	47,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	79,	 0,	77,	 0,	61,	 0,	55,	 0,	49
};
// Test Puzzle #2
{
	 0,	 0,	47,	 0,	 0,	 0,	 5,	 0,	 0,
	 0,	41,	 0,	43,	 0,	 3,	 0,	 9,	 0,
	51,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	11,
	 0,	53,	 0,	 0,	 0,	 0,	 0,	17,	 0,
	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	 0,
	 0,	59,	 0,	 0,	 0,	 0,	 0,	15,	 0,
	75,	 0,	 0,	 0,	 0,	 0,	 0,	 0,	23,
	 0,	79,	 0,	71,	 0,	31,	 0,	27,	 0,
	 0,	 0,	81,	 0,	 0,	 0,	29,	 0,	 0
};
*/


void show_puzzle(void)
{
	int		ix,		iy;
	char*	breakline = "+----+----+----+----+----+----+----+----+----+" EOL;
	
	printf("%s", breakline);
	for (ix=0; ix<DIMENSION; ix++)
	{
		for (iy=0; iy<DIMENSION; iy++)
			printf("| %2d ", board[ix][iy]);
		printf("|");
		printf(EOL);
		printf("%s", breakline);
	}
}



void initialize(void)
{
	int		ix,	iy;

// Set up board and master list based on given values

	for (ix=1; ix<dimensionof(available); ix++)
		available[ix] = 1;

	for (iy=0; iy<DIMENSION; iy++)
	{
		for (ix=0; ix<DIMENSION; ix++)
		if (board[ix][iy])
			available[ board[ix][iy] ] = 0;
	}
}



int follow(int x, int y, int number)
{
	int			flag = 0;

	calls++;
	if (number > LIMIT)
		return 1;

	if (available[number] == 0)
	{
		if (board[x][y] != number)
			return 0;
	}
	else
	{
		if (board[x][y] != 0)
			return 0;

		board[x][y]			= number;
		available[number]	= 0;
		flag				= 1;
	}

	if ((y<(DIMENSION-1))	&& follow(x,	 y + 1, number + 1))	return 1;
	if ((y>		0		)	&& follow(x,	 y - 1, number + 1))	return 1;
	if ((x<(DIMENSION-1))	&& follow(x + 1, y,		number + 1))	return 1;
	if ((x>		0		)	&& follow(x - 1, y,		number + 1))	return 1;
	if (flag)
	{
		board[x][y]			= 0;
		available[number]	= 1;
	}
	return 0;
}	
	


int main(int argc, char** argv)
{
	int		x,	y;
	
	printf("%s", title);
	printf(EOL);
	initialize();
	show_puzzle();

	for (x=0; x<DIMENSION; x++)
	{
		for (y=0; y<DIMENSION; y++)
		{
			if (follow(x, y, 1))
				break;
		}
	}
	printf(EOL);
	show_puzzle();
	printf("Number of calls to tracking procedure: %d" EOL, calls);
	printf("Press Enter to exit." EOL);
	getchar();
	return 0;
}

