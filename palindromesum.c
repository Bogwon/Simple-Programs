// palindromesum.c
// Chuck Pergiel, February 2020
// Compile command: cc palindromesum.c
// Inspired by website where a guy claims he can find three palindromes to add up to any integer
// https://somethingorotherwhatever.com/sum-of-3-palindromes/

/*
Okay, I've done the first part. We can find the largest palindrome that is less than or equal to any integer.
We use the first half of the given number to make a palindrome. If the palindrome is too large, we take the first half of the number, subtract one, and then make a palindrome from that.

Finding the largest palindrome and then subtracting from the original number and then repeating that once again will not necessarily give us three palindromes that will add up to the given number, so we will have to work a little harder.

If the last step does not give us a solution, we go back to the previous step and find the next smaller palindrome and then repeat the process until either we find a solution, or our palindrome is smaller than the remainder.
*/
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define dimensionof(p)  (sizeof(p)/sizeof(p[0]))
int		ECHO = 0;
#define echo(  p, q, s) if (ECHO ) fprintf(stderr, #p ": %" #q s, p)
#define EOL     "\n"

#define	LONGsize 24						// buffer size for ASCII version of long integer
#define	BIGint	int

int ispalindromeS(char* s)
{
	int count = strlen(s);
	int last  = count - 1;

	for (int i=0; i<count/2; i++)
		if (s[i] != s[last--])
			return 0;

	return 1;
}

int ispalindromeN(long n)
{
	char 	buffer[LONGsize];

	sprintf(buffer, "%ld", n);
	return ispalindromeS(buffer);
}

int middle_minus_one(char* s1)		// reduce middle digit of s1 by one
{									// digit just to left of middle in even length string
	int full =  strlen(s1);
	int half = (strlen(s1) + 1) / 2;	// include middle character in odd length string
	int count = full - half;			// last half of string
	long minus = 1;
	while (count--)
		minus *= 10;
	long n1 = atol(s1) - minus;
	return sprintf(s1, "%ld", n1);
}

long makeone(char* gs)		// make the largest palindromic number no larger than the given string
{							// gs=given string
	long gi = atol(gs);		// gi=given integer
	char as[LONGsize];		// as=working string
	strcpy(as, gs);
	int	count = strlen(as);
	while (1)
	{
		int last  = count - 1;			// index of last character
		int limit = count / 2;			// ignore middle character in odd length strings
		for (int i=0; i<limit; i++)		// make palindrome from the first half of given number
			as[last--] = as[ i ];

		long ai = atol(as);
		if (ai <= gi)
			return ai;

		int count2 = middle_minus_one(as);			// palindrome is too big
		if (count2 < count)					// Number got shorter
		{
			echo(count2, d, "  ");
			for (int i=0; i<count2; i++)	// Replace with a string of all 9's
				as[i] = '9';
			as[count2] = 0;
			count = count2;
		}
	}
	return 0;	// never happens
}

long	n[4];			// 0 is the original, 1, 2 & 3 are the 3 palindromes

int decomp(char* s0)
{
	long	nx;				// a temporary working variable
	char	s1[LONGsize];	// ASCII version of a long integer
	char	s2[LONGsize];	// ASCII version of a long integer

	n[0] = atol(s0);
	strcpy(s1, s0);
	while (1)
	{
		n[1] = makeone(s1);
		nx   = n[0] - n[1];
		sprintf(s2, "%ld",	nx);
		while (1)
		{
			n[2] = makeone(s2);
			n[3] = nx - n[2];
			
			if (ispalindromeN(n[3]))
				return 0;

			if (n[3]>n[2])
				break;

			middle_minus_one(s2);
			echo(s2, s, EOL);
		}
		middle_minus_one(s1);
		echo(s1, s, EOL);
	}
	return 1;
}

#define	ONEzillion		10000000	// Ten million
long	counters[11] = { 0 };
int	dcount = ONEzillion;

int counter(long n)
{
	int count = 0;
	if (n)
	{
		char buffer[32];
		count = sprintf(buffer, "%ld", n);
		if (count > dimensionof(counters)-1)
			count = dimensionof(counters)-1;
	}	

	counters[count]++;

	if (dcount)
		dcount--;
	else
	{
		for (int i=0; i<dimensionof(counters); i++)
			printf("%ld  ", counters[i]);
		printf(EOL);
		dcount = ONEzillion;
	}

	return 0;
}

int show(char* s)
{
	echo(s, s, "  ");
	int result = decomp(s);

	counter(n[3]);
	
	if (result)	printf("ERROR" EOL);

	return result;
}

int isnumber(char* s)
{
	while (*s)
		if (isdigit(*s))
			s++;
		else
			return 0;
	return 1;
}

int main(int argc, char** argv)
{
	if (argc>1)		// look for command line options
		for (int i=1; i<argc; i++)
		{
			if (!isnumber(argv[i]))
			{
				ECHO = 1;
				fprintf(stderr, "ECHO is turned on." EOL);
				break;
			}
		}

	echo(sizeof(long), ld, EOL);

	if (argc>1)
	{
		int count = 0;
		for (int i=1; i<argc; i++)		// decompose numeric arguments
		{
			echo(	  i,  d, "  ");
			echo(argv[i], s, EOL );
			if (isnumber(argv[i]))
			{
				show(argv[i]);
				count++;
			}
		}

		if (count)
			return count;
	}

//	long i = 0;							// no numeric arguments, run wild
//	while (i<20000)

	BIGint i = 0;							// no numeric arguments, run wild
	while (i>=0)
	{
		char	s[LONGsize];
		sprintf(s, "%d", i);
		if (show(s))
			break;
		i++;
	}
	return i;
}

