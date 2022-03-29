char* title = "Egyptian Fractions";
// cd Computer/Mine
// cc EgyptianFractions.c -lm

// https://www.youtube.com/watch?v=vVEcqzjykHU

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define Asize   		1000000
#define	EOL				"\n"
#define	ECHO			1
#define	echo(p, q, s)	if (ECHO)	fprintf(stderr, #p ": %" #q s, p)
#define	dimensionof(p)	(sizeof(p)/sizeof(p[0]))

typedef struct
{
	int		flag;
	int		numerator;
	int		denominator;
	double	value;
}	fraction_s;
fraction_s	fraction_t;

int	mod(int a, int n)
{
	return a - n * floor(a / n);
}

int	modx(int a, int n)
{
	int r = a % n;
	if (r < 0)	return n + r;
	else		return r;

/*	if (a<0)
		return a + (n * ((-a / n) + 1));
	else
		return a - (n *  ( a / n)	  );		// integer division truncates
*/
}

int	showmod(int a, int n)
{
	return printf("%2d mod %d = %d" EOL, a, n, mod(a, n));
}

int	showmodx(int a, int n)
{
	return printf("%2d mod %d = %d" EOL, a, n, modx(a, n));
}

float fraction(char symbol, int n, int d)
{
	echo(n, d, "  ");
	echo(d, d, EOL );

//	if (n==1)
//	{
//		printf("%c (%d / %d) " EOL, symbol, n, d);
//		return 1;
//	}

	int d1 = ceil(d / n);
	fprintf(stderr, "%c (1 / %d) " , symbol, d1);

	float frac = (float)n / (float)d;
	if (n==1)
	{
		fprintf(stderr, EOL);
		return frac;
	}


	int n1 = modx(d, n);
	d1 *= d;

	return frac + fraction('+', n1, d1);
}

int main(int argc, char** argv)
{
// mod(a, n) = a - n * floor(a / n)

	int a = 11;
	int n = 7;

	showmod(a, n);
	showmodx(a, n);
	showmod(-a, n);
	showmodx(-a, n);

	if (argc!=3)
	{
		printf("%s requires two parameters: the numerator and the denominator." EOL, title);
		return -1;
	}

	echo(argc, d, EOL);
	if (ECHO)
		for (int i=0; i<argc; i++)
			fprintf(stderr, "arg[%d]: %s" EOL, i, argv[i]);

	int	num = atoi(argv[1]);
	int den = atoi(argv[2]);

	if (num>den)
	{
		printf("Numerator (%d) must NOT be larger than the denominator (%d)." EOL, num, den);
		return -2;
	}

	printf("%d / %d ", num, den);

	float frac = (float)num / (float)den;
	float sum = fraction('=', num, den);
	return fprintf(stderr, "Fraction: %f  Sum: %f  Difference: %f" EOL, 
							frac, sum, frac - sum);
}

