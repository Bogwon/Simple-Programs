/* Zebra puzzle - https://en.wikipedia.org/wiki/Zebra_Puzzle

Set text editor tab width to 4

Blog post about this program: https://pergelator.blogspot.com/2022/06/zebra-puzzle.html

The following version of the puzzle appeared in Life International in 1962:

There are five houses.
	The Englishman lives in the red house.
	The Spaniard owns the dog.
	Coffee is drunk in the green house.
	The Ukrainian drinks tea.
	The green house is immediately to the right of the ivory house.
	The Old Gold smoker owns snails.
	Kools are smoked in the yellow house.
	Milk is drunk in the middle house.
	The Norwegian lives in the first house.
	The man who smokes Chesterfields lives in the house next to the man with the fox.
	Kools are smoked in the house next to the house where the horse is kept.
	The Lucky Strike smoker drinks orange juice.
	The Japanese smokes Parliaments.
	The Norwegian lives next to the blue house.
Now, who drinks water? Who owns the zebra?
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	dimensionof(p)	(sizeof(p)/sizeof(p[0]))
#define	EOL				"\n"
#define	ECHO			0
#define	echo(p, q, s)	if (ECHO)	fprintf(stdout, #p ": %" #q s, p)

struct
{
	char* category;
	char* list[5];
} items[5] =
{
	{	"Nation",
		{	"Englishman",
			"Spaniard",
			"Ukrainian",
			"Norwegian",
			"Japanese",
		},
	},
	{	"Color",
		{	"red",
			"green",
			"ivory",
			"yellow",
			"blue",
		},
	},
	{	"Animal",
		{	"dog",
			"snails",
			"fox",
			"horse",
			"",
		},
	},
	{	"Drink",
		{	"Coffee",
			"tea",
			"Milk",
			"orange juice",
			"",
		},
	},
	{	"Smoke",
		{	"Old Gold",
			"Chesterfields",
			"Kools",
			"Lucky Strike",
			"Parliaments",
		},
	}
};

char* table[6][5];		// table[0] isn't used. We have five houses numbered 1 thru 5.
char* save1[6][5];		// If there were more than one solution we would need an array of these.

struct
{
	char*	item1;
	char*	item2;
	int		position;	// 0 indicates same house
						// positive integers indicate absolute position
						// -1 means item2 is to the right of item1
						// -3 means item2 may be left or right of item1
						// we could use -2 to indicate to the left, but it isn't needed
} rules[] =
{
	{	"Englishman",		"red",			 0	},	//	 0
	{	"Spaniard",			"dog",			 0	},	//   1
	{	"Coffee",			"green",		 0	},	//	 2
	{	"Ukrainian",		"tea",			 0	},	//	 3
	{	"green",			"ivory",		-1	},	//	 4
	{	"Old Gold",			"snails",		 0	},	//	 5
	{	"Kools",			"yellow",		 0	},	//	 6
	{	"Milk",				"",				 3	},	//	 7
	{	"Norwegian",		"",				 1	},	//	 8
	{	"Chesterfields",	"fox",			-3	},	//	 9
	{	"Kools",			"horse",		-3	},	//	10
	{	"Lucky Strike",		"orange juice",	 0	},	//	11
	{	"Japanese",			"Parliaments",	 0	},	//	12
	{	"Norwegian",		"blue",			-3	},	//	13
};

int show(void)
{
	#define	format	"%-14s"

	int	n = printf(EOL);
	for (int i=0; i<dimensionof(table); i++)
	{
//		n += printf("%d   ", i);
		for (int j=0; j<dimensionof(table[0]); j++)
			n += printf(format, table[i][j]);
		n += printf(EOL);
	}
	memcpy(save1, table, sizeof(table));
	return n;
}

int indexof(char* item)		// return index of category
{
	for (int i=0; i<dimensionof(items); i++)				// categories
		for (int j=0; j<dimensionof(items[0].list); j++)	// items in each category
			if (strcmp(item, items[i].list[j]) == 0)
				return i;

	return -1;	
}

int init(void)
{
	for (int i=0; i<dimensionof(items); i++)
		table[0][i] = items[i].category;

	for (int i=0; i<dimensionof(rules); i++)
		if (rules[i].position > 0)
			table[ rules[i].position ][ indexof( rules[i].item1 ) ] = rules[i].item1;

	return 0;
}


int try(int index)
{
	if (index>=dimensionof(rules))
		return show();

	if (rules[index].position > 0)
		return try(index+1);

	int	flag	= 0;
	int i		= indexof( rules[index].item1 );	// get index of category
	int j		= indexof( rules[index].item2 );
	int	mask	=		  -rules[index].position;	// decipher code
	int result	= 0;

	for (int k=1; k<=2; k++)
	{
		int start	= 1;
		int	limit	= dimensionof(table);
		int	offset	= 0;

		// first we search for item1 and see if we can fit item2

		if (mask)
		{			if ((mask & k) == 1)	{	limit--;	offset = +1;	}
			else	if ((mask & k) == 2)	{	start++;	offset = -1;	}
			else	return 0;					
		}

		for (int p1=start; p1<limit; p1++)						// Look for existing item
		{
			if	   (table[p1	   ][i] == rules[index].item1)
			{
				flag = 1;
				if (table[p1+offset][j] == NULL)				// Found
				{
					table[p1+offset][j]  = rules[index].item2;
					result				 = try(index+1);
					table[p1+offset][j]  = NULL;
				}
				break;	// we found item1, we better not find another one
			}
		}
		if (mask!=3)
			break;
	}
	if (flag) return 0;

	for (int k=1; k<=2; k++)
	{
		int start	= 1;
		int	limit	= dimensionof(table);
		int	offset	= 0;

		// now we search for item2 and see if we can fit item1
		// invert mask values to preserve relationship order from rule

		if (mask)
		{			if ((mask & k) == 2)	{	limit--;	offset = +1;	}
			else	if ((mask & k) == 1)	{	start++;	offset = -1;	}
			else	return 0;					
		}

		for (int p1=start; p1<limit; p1++)
		{
			if	   (table[p1+offset ][i] == rules[index].item2)
			{
				flag = 1;
				if (table[p1		][j] == NULL				 )
				{
					table[p1		][i] = rules[index].item1;
					result				 = try(index+1);
					table[p1		][i] = NULL;
				}
				break;	// we found item2, we better not find another one
			}
		}
		if (mask!=3)
			break;
	}
	if (flag) return 0;

	for (int k=1; k<=2; k++)
	{
		int start	= 1;
		int	limit	= dimensionof(table);
		int	offset	= 0;
		int	flag	= 0;

		// we didn't find either item1 or item2, so we look for a double blank

		if (mask)
		{			if ((mask & k) == 2)	{	limit--;	offset = +1;	}
			else	if ((mask & k) == 1)	{	start++;	offset = -1;	}
			else	return 0;					
		}

		for (int p1=start; p1<limit; p1++)
			if	(	(table[p1		][i] == NULL)	
				&&	(table[p1+offset][j] == NULL)
				)
			{
/*				echo(p1,				 d, "  ");
				echo(i,					 d, "  ");
				echo(j,					 d, "  ");
				echo(	   index,		 d, "  ");
				echo(rules[index].item1, s, "  ");
				echo(rules[index].item2, s, EOL);
*/
				table[ p1		][i] = rules[index].item1;
				table[ p1+offset][j] = rules[index].item2;
				result				 = try(index+1);
				table[ p1		][i] = NULL;
				table[ p1+offset][j] = NULL;
			}

		if (mask!=3)
			break;
	}
	return result;
}

int main(int argc, char** argv)
{
	init();
	if (ECHO) show();
	try(0);

	printf(EOL);
	for (int i=0; i<dimensionof(save1[0]); i++)
	{
		if (save1[i][2]==NULL)
		{
			printf("The %s owns the zebra." EOL, save1[i][0]);
			break;
		}
	}
	for (int i=0; i<dimensionof(save1[0]); i++)
	{
		if (save1[i][3]==NULL)
		{
			printf("The %s drinks water." EOL, save1[i][0]);
			break;
		}
	}
	return 0;
}

