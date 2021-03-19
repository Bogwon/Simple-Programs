#define title			"Demonstrate various methods of computing pi."

// April 2018
// Embellished March 2021

#define title_classic	"Method 1: Classic Method."
#define title_Ramanujan	"Method 2: Use a formula devised by Srinivasa Ramanujan."
#define title_BBP		"Method 3: Use the new Bailey-Borwein-Plouffe formula."
#define title_tangent	"Method 4: Add the lengths of successively smaller tangents."
#define title_chord		"Method 5: Add the lengths of successively smaller chords, starting with %d chords."

#include <math.h>
#include <stdio.h>

#define	EOL				"\n"
#define	FORMAT			"%21.17f"
#define	FORMAT_STRING	"%2d "		  "%14e %14e "	FORMAT FORMAT EOL
#define	FORMAT_STRING2	"%2d  %16s  " FORMAT FORMAT FORMAT FORMAT EOL
#define	FORMAT_STRING3			 "%20s %14e %14e "	FORMAT FORMAT EOL


#define	USE_TRIG	0

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


// Classic method
/*
1,557,000,000,000  1557000000001.00000000000000000  0.78539816339745105  3.14159265358980422  0.00000000000001110
1,558,000,000,000  1558000000001.00000000000000000  0.78539816339745072  3.14159265358980289  0.00000000000000977
1,559,000,000,000  1559000000001.00000000000000000  0.78539816339745039  3.14159265358980155  0.00000000000000844
1,560,000,000,000  1560000000001.00000000000000000  0.78539816339745028  3.14159265358980111  0.00000000000000799
1,561,000,000,000  1561000000001.00000000000000000  0.78539816339745006  3.14159265358980022  0.00000000000000711
1,562,000,000,000  1562000000001.00000000000000000  0.78539816339744983  3.14159265358979933  0.00000000000000622
1,563,000,000,000  1563000000001.00000000000000000  0.78539816339744961  3.14159265358979845  0.00000000000000533
1,564,000,000,000  1564000000001.00000000000000000  0.78539816339744928  3.14159265358979711  0.00000000000000400
1,565,000,000,000  1565000000001.00000000000000000  0.78539816339744883  3.14159265358979534  0.00000000000000222
1,566,000,000,000  1566000000001.00000000000000000  0.78539816339744861  3.14159265358979445  0.00000000000000133
1,567,000,000,000  1567000000001.00000000000000000  0.78539816339744861  3.14159265358979445  0.00000000000000133
1,568,000,000,000  1568000000001.00000000000000000  0.78539816339744839  3.14159265358979356  0.00000000000000044
1,569,000,000,000  1569000000001.00000000000000000  0.78539816339744817  3.14159265358979267 -0.00000000000000044
1,570,000,000,000  1570000000001.00000000000000000  0.78539816339744795  3.14159265358979178 -0.00000000000000133
1,571,000,000,000  1571000000001.00000000000000000  0.78539816339744783  3.14159265358979134 -0.00000000000000178
1,572,000,000,000  1572000000001.00000000000000000  0.78539816339744761  3.14159265358979045 -0.00000000000000266
1,573,000,000,000  1573000000001.00000000000000000  0.78539816339744717  3.14159265358978868 -0.00000000000000444
1,574,000,000,000  1574000000001.00000000000000000  0.78539816339744684  3.14159265358978734 -0.00000000000000577
1,575,000,000,000  1575000000001.00000000000000000  0.78539816339744661  3.14159265358978645 -0.00000000000000666
1,576,000,000,000  1576000000001.00000000000000000  0.78539816339744639  3.14159265358978557 -0.00000000000000755
1,577,000,000,000  1577000000001.00000000000000000  0.78539816339744617  3.14159265358978468 -0.00000000000000844
1,578,000,000,000  1578000000001.00000000000000000  0.78539816339744584  3.14159265358978335 -0.00000000000000977
1,579,000,000,000  1579000000001.00000000000000000  0.78539816339744561  3.14159265358978246 -0.00000000000001066
 . . .
4,041,000,000,000  4041000000001.00000000000000000  0.78539816339724589  3.14159265358898354 -0.00000000000080957

*/

void classic_method(void)
{
//	pi = 4 * (z - (z^3)/3 + (z^5)/3 - (z^7)/7 + . . .)
//	z = 1

	long	n;
	int		m	= 1;
	double	pi	= 0;
	double	x	= 0;
	double	divisor;

	printf(EOL EOL title_classic EOL EOL);
	printf("        Step               n           divisor              pi                difference" EOL);
//			123456789-123456789- 123456789-1234 123456789-1234 123456789-123456789-1 123456789-123456789-1

	double last = 10;
	double diff = 0;

	for (n=0; diff!=last; n+=2)
	{
		divisor = n +1;
		x += m / divisor;
		pi = 4 * x;

		last = diff;
		diff = fabs(pi - PI);

		if ((n % 100000)==0)		// one hundred thousand
			printf(FORMAT_STRING3, commafy(n/2), divisor, x, pi, pi - PI);
		
		m *= -1;
	}
	printf(FORMAT_STRING3, commafy(n/2), divisor, x, pi, pi - PI);
}



// Bailey-Borwein-Plouffe method


void BBP_method(void)
{
	int		n;
	double	divisor	= 1;
	double	pi		= 0;
	double	x;

	printf(EOL EOL title_classic EOL EOL);
	printf(" n      divisor       increment              pi              difference" EOL);

// We can only make 10 iterations before the divisor grows so big that the
// additions effectively go to zero.

	for (int n=0; pi!=PI; n+=8)
	{
		x	=	( 4.0/(n + 1)
				- 2.0/(n + 4)
				- 1.0/(n + 5)
				- 1.0/(n + 6)) / divisor;
		pi += x;

		printf(FORMAT_STRING, n, divisor, x, pi, pi - PI);

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
	double	pi = 0;
	double	x  = 0;
	double	y;

	printf(EOL EOL title_Ramanujan EOL EOL);

	f = 2 * sqrt(2) / 9801;
	printf("Step  step value         sum                 pi              difference" EOL);
	for (int k=0; pi!=PI; k++)
	{
		y = (factorial(4 * k) * (1103 + (26390 * k))) / (pow(factorial(k), 4) * pow(396, 4 * k));
		x += y;
		pi = 1/(f * x);
		printf(FORMAT_STRING, k + 1, y, x, pi, pi - PI);
	}
}



// Tangent Method



void tangent_method(void)
{
	long	n		= 2;	// number of tangents for a half circle
	double	a		= 1;	// one half the length of the tangent
	double	pi;
	double	x		= 1;	// distance along radius from circle to end of current tangent

	printf(EOL EOL title_tangent EOL EOL);
	printf("Step   Tangents            tangent               altitude                 pi                difference" EOL);
		// "2d ------16s-------  123456789-123456789-1 123456789-123456789-1 123456789-123456789-1 123456789-123456789-1 EOL

	for (int k=1; x!=0; k++)
	{
		pi = 2 * a * n;
		x = sqrt(a * a + 1) - 1;
		printf(FORMAT_STRING2, k, commafy(n), a * 2, x, pi, pi-PI);
		a =	((a * a) - (x * x)) / (2 * a);
		n *= 2;
	}
}



// Chord Method



void chord_method(int n)
{
	double	c;				// length of chord
	double	a;				// altitude from midpoint of chord to center
	double	b;				// distance from midpoint of chord to rim
	double	pi = 0;

	printf(EOL EOL title_chord EOL EOL, n);
	printf("Step    Chords             altitude               chord                   pi               difference"      EOL);
		// "2d ------16s-------  123456789-123456789-1 123456789-123456789-1 123456789-123456789-1 123456789-123456789-1 EOL

	if (n==4)
		c = sqrt(2);	// chord
	else
		c = 1;			// n = 6
	
	n /= 2;		// number of chords for a half circle

	for (int k=1; pi<PI; k++)
	{
		a = sqrt(   1	 - (c * c / 4));
		pi = c * n;
		printf(FORMAT_STRING2, k, commafy(n), a, c, pi, pi-PI);
		b = 1 - a;
		c =	sqrt((b * b) + (c * c / 4));
		n *= 2;
	}
}



int main(void)
{
	printf(title EOL);
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
	tangent_method();
	chord_method(4);
	chord_method(6);

	return 0;
}

