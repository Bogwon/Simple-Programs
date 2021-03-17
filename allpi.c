char* title = "Demonstrate various methods of computing pi.";

// April 2018
// Embellished March 2021

char* title_classic		= "Method 1: Classic Method.";
char* title_Ramanujan	= "Method 2: Use a formula devised by Srinivasa Ramanujan.";
char* title_BBP			= "Method 3: Use the new Bailey-Borwein-Plouffe formula.";
char* title_tangent		= "Method 4: Add the lengths of successively smaller tangents.";
char* title_chord		= "Method 5: Add the lengths of successively smaller chords.";

#include <math.h>
#include <stdio.h>

#define	EOL				"\n"
#define	FORMAT			"%21.17f"
#define	FORMAT_STRING	"%2d  %14e %14e " FORMAT FORMAT EOL
#define	FORMAT_STRING2	"%12s  " FORMAT FORMAT FORMAT FORMAT EOL


#define	USE_TRIG	1

#define	WiPi	3.14159265358979323846264338327950288419716939937510
// Pi according to Wikipedia

int		ECHO = 1;
#define echo(  p, q, s) if (ECHO ) fprintf(stderr, #p ": %" #q s, p)	// for debugging
			// p is the parameter you want to print
			// q is the format letter - d, s, f, etc.
			// s is the white space at the end

double	PI;

char* commafy(long n)
{
	static char buffer[64];

	int i = sizeof(buffer);
	buffer[--i] = 0;		// terminating zero
	int count = 0;
	while (1)
	{
		buffer[--i] = (n % 10) + '0';
		n /= 10;		
		if (n == 0)
			break;
		count++;
//		echo(count, d, "  ");		// example use of echo
//		echo(	i,  d, "  ");
//		echo(	n,  d, EOL);
		if (count==3)
		{
			buffer[--i] = ',';
			count = 0;
		}
	}
	return &buffer[i];
}

void Continue(char* action)
{
	printf("Press the Enter key to %s." EOL, action);
	getchar();
}



void classic_method(void)
{
	int		k;
	int		n;
	double	divisor	= 1;
	double	pi		= 0;
	double	x;

	printf(EOL EOL "%s" EOL EOL, title_classic);
	printf("Step    divisor       increment              pi              difference" EOL);

// We can only make 10 iterations before the divisor grows so big that the
// additions effectively go to zero.

	for (k=0; k<16; k++)
	{
		n	= 8 * k;
		x	=	( 4.0/(n + 1)
				- 2.0/(n + 4)
				- 1.0/(n + 5)
				- 1.0/(n + 6)) / divisor;
		pi += x;

		printf(FORMAT_STRING, k + 1, divisor, x, pi, pi - PI);
		if ((pi-PI)==0)
			break;

		divisor *= 16;;
	}
}



/* Method 2: formula devised by Srinivasa Ramanujan

	1/pi = ((2 x 2^0.5) / 9801)
		 times the sum of (as k goes from zero to infinity)
		 (4k)! x (1103 + 26390k) / (k!)^4 x 396^(4k)
*/



double factorial(int k)
{
	double	f;

	f = 1;
	while (k)
		f *= k--;

	return f;
}



void Ramanujan_method(void)
{
	double	f;
	double	pi;
	double	x = 0;
	double	y;
	int		k = 0;

	printf(EOL EOL "%s" EOL EOL, title_Ramanujan);

	f = 2 * sqrt(2) / 9801;
	printf("Step  step value         sum                 pi              difference" EOL);
	while (1)
	{
		y = (factorial(4 * k) * (1103 + (26390 * k))) / (pow(factorial(k), 4) * pow(396, 4 * k));
		x += y;
		pi = 1/(f * x);
		printf(FORMAT_STRING, k + 1, y, x, pi, pi - PI);
		if (pi==PI)
			break;

		k++;
		if ((k%10)==0)
			Continue("continue");
	}
}



// Bailey-Borwein-Plouffe method


void BBP_method(void)
{
	int		k;
	int		n;
	double	divisor	= 1;
	double	pi		= 0;
	double	x;

	printf(EOL EOL "%s" EOL EOL, title_BBP);

// We can only make 10 iterations before the divisor grows so big that the
// additions effectively go to zero.

	printf("Step    divisor       increment              pi              difference" EOL);
	for (k=0; k<16; k++)
	{
		n	= 8 * k;
		x	=	( 4.0/(n + 1)
				- 2.0/(n + 4)
				- 1.0/(n + 5)
				- 1.0/(n + 6));
		
		x /= divisor;

		pi += x;
		printf(FORMAT_STRING, k + 1, divisor, x, pi, pi - PI);
		if (pi==PI)		// at some point x gets so small that it no longer has any impact on pi
			break;		// due to the format of floating point numbers.

		divisor *= 16;
	}
}



// Tangent Method



void tangent_method(void)
{
	long	n		= 2;	// number of tangents for a half circle
	double	a		= 1;	// one half the length of the tangent
	double	pi;
	double	x		= 0;	// distance along radius from circle to end of current tangent

	printf(EOL EOL "%s" EOL EOL, title_tangent);
	printf("  Tangents          tangent               altitude                 pi                difference" EOL);
		// "----12s-----  123456789-123456789-1 123456789-123456789-1 123456789-123456789-1 123456789-123456789-1 EOL

	while (1)
	{
		pi = 2 * a * n;
		x = sqrt(a * a + 1) - 1;
		printf(FORMAT_STRING2, commafy(n), a * 2, x, pi, pi-PI);
		if (x==0)
			break;

		a =	((a * a) - (x * x)) / (2 * a);
		n *= 2;
	}
}



// Chord Method



void chord_method(void)
{
	int		n		= 2;		// number of chords for a half circle
	double	c		= sqrt(2);	// chord
	double	r		= 1.0;		// radius
	double	a;					// altitude from midpoint of chord to center
	double	b;					// distance from midpoint of chord to rim
	double	pi;

	printf(EOL EOL "%s" EOL EOL, title_chord);
	printf("   Chords           altitude               chord                   pi               difference"      EOL);
		// "----12s-----  123456789-123456789-1 123456789-123456789-1 123456789-123456789-1 123456789-123456789-1 EOL

	while (1)
	{
		a = sqrt(   1	 - (c * c / 4));
		pi = c * n;
		printf(FORMAT_STRING2, commafy(n), a, c, pi, pi-PI);
		if (pi>=PI)
			break;		// due to the inherent limits of floating point, eventually our calculated value will exceed the limit

		b = r - a;
		c =	sqrt((b * b) + (c * c / 4));
		n *= 2;
	}
}



int main(void)
{
	printf("%s" EOL, title);
	printf("For comparison, we use the value for pi " EOL);

#if USE_TRIG
	PI		= acos(-1);
	printf("we get from the trig function acos:" FORMAT EOL, PI);
#else
	PI		= WiPi;
	printf("we got from Wikipedia:"				 FORMAT EOL, PI);
#endif

	classic_method();
	Ramanujan_method();
	BBP_method();
	tangent_method();
	chord_method();

	return 0;
}

