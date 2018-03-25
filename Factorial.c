// From question on Quora:	http://qr.ae/TU1pZ8
// compile command: cc bigadd.c -lgmp
// -lm is for the math library

//	 Number		Elapsed time	Digits
//	   100,000	  0m  1.420s
//	   100,000	  0m  1.444s	  456,574	latest version
//	 1,000,000	  1m 29.968s
//	 1,000,000	  2m 55.056s	5,565,709	Mar 23 11:25 1000000!.txt
//	10,000,000	413m 13.856s

#define	PREALLOCATE	0	// preallocate storage for our really big number
#define	UINT		0	// use a regular unsigned interger instead of GMP number for our multiplier

#include <gmp.h>		// GNU Multi-Precision arithmetic library
#include <math.h>		// Only used to estimate the number of digits in the result
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define ECHO            1
#define echo(p, q, s)   if (ECHO) fprintf(stderr, #p ": %" #q s, p)
#define EOL "\n"

// https://www.geeksforgeeks.org/count-digits-factorial-set-2/
// Returns the number of digits present in n!
// Since the result can be large, long long is used as return type
long long findDigits(int n)
{
	if (n <  0)	return 0;    // we don't do negative numbers
	if (n <= 1)	return 1;    // base case

	double x = ((n*log10(n/M_E) + log10(2*M_PI*n) / 2.0));    // Use Kamenetsky formula to calculate the number of digits
	return floor(x)+1;
}

int computePU(unsigned int n);
int computeAU(unsigned int n);
int computePG(unsigned int n);
int computeAG(unsigned int n);

void  progress(clock_t start, int mode)
{
	#define	interval	100000
	static	int		i		 = 1;
	static	int		counter	 = interval;	

	if ((counter==0) || (mode==1))
	{
		clock_t et = (clock() - start) / CLOCKS_PER_SEC;
		echo(i,	  3d, "  ");
		echo(et, 10ld, EOL);
		counter = interval;
		i++;
	}
	counter--;
}

int main(int argc, char** argv)
{
	char* number = "12345";		// Default value in case no argument is given
	if (argc>1)
		if (isdigit(argv[1][0]))
			number = argv[1];
		else
		{
			printf("%s computes the factorial of a number and writes the result to a file. " EOL
					"The name of the file is made by tacking !.txt onto the number. "
					"For example, the factorial of 123 is written to 123!.txt", argv[0]);
			exit(-1);
		}

	unsigned int dog0 = atoi(number);

#if PREALLOCATE		// Product
	mpz_t	dog2;	mpz_init2(	dog2, findDigits(dog0));
#else
	mpz_t	dog2;	mpz_init(	dog2);
#endif

	echo(findDigits(dog0), lld, "  ");
	echo(			number,  s, "  ");

	mpz_add_ui(dog2, dog2, 1);			// Set product to 1

	clock_t start	 = clock();

#if UINT
	while (dog0)
	{
		mpz_mul_ui(dog2, dog2, dog0--);
		progress(start, 0);
	}
#else
	mpz_t	dog1;	mpz_init(	dog1);						// Input
	gmp_sscanf(number, "%Zd",	dog1);
	gmp_printf("dog1: %Zd" EOL, dog1);
	while (mpz_cmp_ui(dog1, (unsigned int)0))
	{
		mpz_mul(dog2, dog2, dog1);
		mpz_sub_ui(dog1, dog1, 1);
		progress(start, 0);
	}
#endif
	progress(start, 1);
	char buffer[100];
	strcpy(buffer, number);
	FILE*	outfile = fopen(strcat(buffer, "!.txt"), "w");
	gmp_fprintf(outfile, "%Zd", dog2);
	fclose(outfile);

	return 0;
}

