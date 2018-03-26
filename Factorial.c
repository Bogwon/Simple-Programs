// Factorial.c
// Chuck Pergiel, March 25, 2018
// From a question on Quora:	http://qr.ae/TU1pZ8
// compile command: cc Factorial.c -lgmp -lm
// -lgmp is for the GNU Multi-Precision library
// -lm is for the math library

//	 Number		Elapsed time	Digits
//	   100,000	 1.5 seconds	  456,574
//	 1,000,000	 3   minutes	5,565,709
//	10,000,000	 7	 hours

#include <gmp.h>		// GNU Multi-Precision arithmetic library
#include <math.h>		// Only used to estimate the number of digits in the result
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define	dimensionof(x)		(sizeof(x)/sizeof(x[0]))
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

char* comify(int n)		// print a number with commas
{
	static char buffer[16];
			int i	  = sizeof(buffer) - 1;
			int comma = 0;

	while (1)
	{
		i -= 4;
		sprintf(&buffer[i], "%03d", n % 1000);
		if (comma)					// append comma if needed
			buffer[i+3] = ',';		// replaces terminating zero

		comma = 1;
		n /= 1000;
		if (n==0)
			break;
	}
	while (buffer[i]=='0')
		i++;				// erase leading zeros

	return &buffer[i];		
}

int	 progress_digits = 10;
void progress_format(char* number)
{
	progress_digits  = strlen(number);
	int temp = progress_digits/3;
	if ((progress_digits%3)==0)
		progress_digits--;
	progress_digits += temp;
}

void progress(clock_t start, int mode)
{
	#define	interval	100000
	static	int		i		 = 1;
	static	int		counter	 = interval;	

	if ((counter==0) || (mode==1))
	{
		clock_t et = 1000 * (clock() - start) / CLOCKS_PER_SEC;		// ticks per millisecond
		printf("Multiplies: %*s   ", progress_digits, comify((i*interval) - counter));
		printf("Time (seconds): %.1f" EOL, (float)et/1000);
		counter = interval;
		i++;
	}
	counter--;
}

int main(int argc, char** argv)
{
	char* number = "12345";		// Default value in case no argument is given

	if (argc>1)
	{
		if (isdigit(argv[1][0]))
			number = argv[1];
		else
		{
			printf(	"%s computes the factorial of a number and writes the result to a file. "	EOL
					"The name of the file is made by tacking !.txt onto the number. "			EOL
					"For example, the factorial of 123 is written to 123!.txt"					EOL,
					argv[0]);

			exit(-1);
		}
	}

	progress_format(number);
	unsigned int dog0	= atoi(number);
			 int digits = findDigits(dog0);

	printf("Estimated number of digits in %s! ", comify(dog0)	);
	printf("is %s" EOL,							 comify(digits)	);

	mpz_t		dog2;
	mpz_init2(	dog2, digits);
	mpz_add_ui(	dog2, dog2, 1);			// Set product to 1

	clock_t start	 = clock();

	while (dog0)
	{
		mpz_mul_ui(dog2, dog2, dog0--);
		progress(start, 0);
	}

	progress(start, 1);

	char buffer[100];
	strcpy(buffer, number);
	strcat(buffer, "!.txt");
	FILE*	outfile = fopen(buffer, "w");
	int count =	gmp_fprintf(outfile, "%Zd", dog2);
	fclose(outfile);

	printf("%s! has been written to file %s" EOL, comify(atoi(number)), buffer);
	printf("%s! has ",							  comify(atoi(number)));
	printf("%s digits."				 		 EOL, comify(count));
	return 0;
}

