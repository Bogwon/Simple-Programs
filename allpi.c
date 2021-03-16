char* title = "Demonstrate various methods of computing pi.";

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

#define	USE_TRIG	1

#define	WiPi	3.14159265358979323846264338327950288419716939937510
// Pi according to Wikipedia


double	PI;



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

	printf("%s" EOL EOL, title_classic);
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

	printf("%s" EOL EOL, title_Ramanujan);

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

	printf("%s" EOL EOL, title_BBP);

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
		if ((pi-PI)==0)
			break;

		divisor *= 16;
	}
}



// Tangent Method



void tangent_method(void)
{
	int		k		= 1;
	int		n		= 2;	// number of chords for a half circle
	int		count	= 0;	// so we don't go wild
	double	a		= 1;	// one half the length of the tangent
	double	pi;

	double	x		= 0;	// distance from circle to end of current tangent

	printf("%s" EOL EOL, title_tangent);
	printf("Step    tangent       altitude               pi              difference" EOL);

	while (1)
	{
		n *= 2;
		pi = a * n;
		printf(FORMAT_STRING, k++, a * 2, x, pi, pi-PI);
		if (pi<=PI)
			break;

		if ((++count % 20)==0)
			Continue("continue");

		x = sqrt(a * a + 1) - 1;
		a =	((a * a) - (x * x)) / (2 * a);
	}
}



// Chord Method



void chord_method(void)
{
	int		k		= 1;
	int		n		= 1;	// number of chords for a half circle
	int		count	= 0;	// so we don't go wild
	double	a		= 1;	// altitude from midpoint of chord to circle
	double	pi;

	double	h;				// hypotenuse or chord

	printf("%s" EOL EOL, title_chord);
	printf("Step    atlitude       chord                 pi              difference" EOL);

	h		= sqrt(2);
	while (1)
	{
		n *= 2;
		pi = h * n;
		printf(FORMAT_STRING, k++, a, h, pi, pi-PI);
		if (pi>=PI)
			break;

		if ((++count % 20)==0)
			Continue("continue");

		a = 1 - sqrt(   1	 - (h * h / 4));
		h =		sqrt((a * a) + (h * h / 4));
	}
}



int main(void)
{
	printf("%s" EOL, title);
	printf("For comparison, we use the value for pi " EOL);

#if USE_TRIG
	PI		= acos(-1);
	printf("we get from the trig function acos:" FORMAT EOL EOL, PI);
#else
	PI		= WiPi;
	printf("we got from Wikipedia:"				 FORMAT EOL EOL, PI);
#endif

	classic_method();	Continue("continue");
	Ramanujan_method();	Continue("continue");
	BBP_method();		Continue("continue");
	tangent_method();	Continue("continue");
	chord_method();

// We ask for a key press in case the program has been invoked automatically
// instead of the command being entered in a DOS box or terminal window.

	Continue("Exit");
	return 0;
}

