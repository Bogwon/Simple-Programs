// Permute
// Special for question on Quora - http://qr.ae/TUTJVh
// Generate a 6 digit number using digits 0..9
// 7 must show up once
// 3 digits must be even numbers, 3 digits must be odd numbers.
// no repetition, each digit may appear in the number only once.

// Chuck Pergiel, May 2018
// Compile command:				  cc permute.c

#include <stdio.h>

#define dimensionof(p)  (sizeof(p)/sizeof(p[0]))
int		ECHO = 1;
#define echo(  p, q, s) if (ECHO ) fprintf(stderr, #p ": %" #q s, p)
#define EOL     "\n"

#define	MAX	6

char	number	[MAX+1];
int 	count	= 0;

char	donut	[10];
int		even	= 0;
int		odd		= 0;

int fill(int i)
{
	if (i==MAX)
	{
		if (donut[7]==0)
			return 0;

		count++;
		echo(count, d, "  ");
		echo(number, s, EOL);
		return 1;
	}

	int total = 0;
	int start = 0;
	if (i==0) start = 1;			// Cannot start with 0 if we want a six digit number.
	for (int j=start; j<10; j++)
	{
		if (donut[j]==1)	continue;
		if (j & 1)	{	if (odd		>= 3)	continue;	odd++;	}
		else		{	if (even	>= 3)	continue;	even++;	}
		donut[j] = 1;

			number[i] = j + '0';
			total += fill(i + 1);

		donut[j] = 0;
		if (j & 1)	odd--;
		else		even--;
	}
	return total;
}

int main(int argc, char** argv)
{
	int result = fill(0);
	echo(count, d, EOL);
	return result;
}
