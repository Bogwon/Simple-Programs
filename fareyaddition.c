// fareyaddition.c
// Chuck Pergiel, May 2018
// Compile command: cc fareyaddition.c
// Inspired by Numberphile video https://www.youtube.com/watch?v=0hlvhQZIOQw&feature=youtu.be
// Control-C handler from https://stackoverflow.com/questions/17766550/ctrl-c-interrupt-event-handling-in-linux

#include <math.h>		// fabs
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <signal.h>							// handle control-C
volatile sig_atomic_t flag = 0;
void my_function(int sig) { flag = 1; }		// can be called asynchronously

#define	ONEzillion		10							// Bigger than the biggest value we expect
#define indexof(p, b)	((p-b)/sizeof(b[0]))
#define dimensionof(p)  (sizeof(p)/sizeof(p[0]))
int		ECHO = 1;
#define echo(  p, q, s) if (ECHO ) fprintf(stderr, #p ": %" #q s, p)
#define EOL     "\n"

struct	fraction_s
{
	double	value;
	int		numerator;
	int		denominator;
	double	upless;
	double	upmore;
};
typedef struct	fraction_s	fraction_t;

int fcmp(const void* p1, const void* p2)	// numeric value compare for qsort and bsearch
{
	if (((fraction_t*)p1)->value > ((fraction_t*)p2)->value)	return  1;
	if (((fraction_t*)p1)->value < ((fraction_t*)p2)->value)	return -1;
	return 0;
}

// Save this procedure. It took an inordinate amount of screwing around to get it working.
int gcf(int m, int n)	// greatest common factor
{
	if ((m==0) || (n==0))
	{
		if ((m==0) && (n==0)) return 1;
		if (m==0) return n;
		return m;
	}

	while(m!=n)
    {
        if(m > n)
            m -= n;
        else
            n -= m;
    }
	return m;
}

void show(fraction_t* a, int count)		// If you want see
{
	for (int k=0; k<count; k++)
	{
		echo(k,					3d, "  ");
		echo(a[k].numerator,	 d, "  ");
		echo(a[k].denominator,	 d, "  ");
		echo(a[k].value,		 f, EOL);
	}
}

float verify(fraction_t* c1, fraction_t* c2)
{
// Given two fractions, verify that two circles,
// - tangent to the number line,
// - with the center directly above the point designated by the fraction,
// - with a diameter based on the fraction,
// are tangent to each other
// lowercase letters indicate smaller circle, uppercase are for larger one

	fraction_t* f = c1;		// Presume c1 denominator is larger, so f1 will be the smaller circle
	fraction_t* F = c2;
	if (f->denominator < F->denominator)
	{
		F = c1;				// if denominator is smaller, circle will be larger
		f = c2;
	}

	float r = 1/(float)(2 * f->denominator * f->denominator);	// radius of smaller circle
	float R = 1/(float)(2 * F->denominator * F->denominator);	// radius of larger circle

	float dV = fabs(f->value - F->value);			// distance between values on number line
	float dR = R - r;								// difference between radii
	float dC = R + r;								// distance between centers
	return ((dV * dV) + (dR * dR)) - (dC * dC);		// Should be zero
}

int main(int argc, char** argv)
{
  signal(SIGINT, my_function);	// Register Control-C handler

//	echo(sizeof(fraction_t), ld, EOL);
	int q = 234;
	if (argc>1)
		q = atoi(argv[1]);				

// Assigning 1180 to q works when simply declaring the array a, 1181 gives a segmentation fault.

	int			kcount	= (q + 1) * q/2;
	fraction_t*	a		= malloc(		kcount * sizeof(fraction_t)	);
	if (a==NULL)
	{
		echo(kcount, d, " is too big. Not able to allocate enough memory." EOL);
		exit(0);
	}
						  memset(a, 0,	kcount * sizeof(fraction_t)	);

// Generate fractions, one denominator at a time

	printf("Denominator    New Fractions    Total Fractions" EOL);
	a[0].numerator		= 0;	// End points of our number line
	a[0].denominator	= 1;
	a[0].value			= 0;	// zero
	a[1].numerator		= 1;
	a[1].denominator	= 1;
	a[1].value			= 1;	// and one
	int 	k		= 2;
	int		ecount	= 0;
	int error_count = 0;
	float	maxdiff = 0;
	for (int i=1; i<=q; i++)	// denominator
	{
		int start = k;
		for (int j=1; j<i; j++)		// numerator
		{
			int factor = gcf(j, i);
			a[k].numerator		= j / factor;
			a[k].denominator	= i / factor;
			a[k].value			= (float)a[k].numerator / (float)a[k].denominator;
			k++;	// Cannot increment index in assignment, bad things happen.
		}
		qsort(a, k, sizeof(a[0]), fcmp);	// put them in order

		int limit = k;
		for (int i=1; i<limit; i++)		// Compress. Eliminate duplicate entries.
		{
			if (a[i].value == 0)
			{
				a[i].value = ONEzillion;
				k--;
			}
			else
				if (a[i].value==a[i+1].value)
				{
					a[i].value = ONEzillion;
					k--;
				}
		}
		qsort(a, limit, sizeof(a[0]), fcmp);	// Move duplicates to top of list
		
		if (i>10)	printf("\r");						// After the first 10, overwrite the last line
		printf("%10d    %10d    %10d", i, k-start, k);
		if (i<=10)	printf(EOL);						// Print first 10 fractions on their own line

		for (int j=1; j<k-1; j++)			// Verify fractions are Farey sum of parents
		{
			if (a[j].value != ((float)(a[j-1].numerator + a[j+1].numerator) / (float)(a[j-1].denominator + a[j+1].denominator)))
			{
				echo(j, d, " Farey sum fail" EOL);
				show(&a[j-1], 3);				
				error_count++;
			}
		}

		for (int j=1; j<k-1; j++)			// Verify circles.
		{
			if	(	(a[j].upless == 0)
				&&	(a[j].upmore == 0)
				)
			{
				float	result1 = verify(&a[j-1], &a[j  ]);
				float	result2 = verify(&a[j  ], &a[j+1]);
				float	diff	= fabs(result1-result2);
				if (diff != 0)
				{
					ecount++;
					if (maxdiff < diff)
						maxdiff = diff;
#define	RCH	.0000001

					if (diff > RCH)
					{
						echo(j, d, " index   ");
						echo(diff, .12f, " difference" EOL);
						show(&a[j-1], 3);
					}
				}
			}
		}
	    if(flag)
			break;
	}
	printf(EOL);
	ECHO = 1;
	echo(k,				d, " = Number of generated fractions"	EOL	);
	echo(kcount,		d, " = Calculated number"				EOL	);
	echo(error_count, 	d, " = Farey sum errors"				EOL	);
	echo(ecount,		d, " = Ford circle discrepencies"		EOL	);
	echo(maxdiff,	 .12f, " = Largest circle discrepency"		EOL	);	//  0.000000037253
	return error_count;
}

