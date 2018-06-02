// fareyaddition.c
// Chuck Pergiel, May 2018
// Compile command: cc fareyaddition.c
// Inspired by Numberphile video https://www.youtube.com/watch?v=0hlvhQZIOQw&feature=youtu.be

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define	ONEzillion		10							// Bigger than the biggest value we expect
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

void show(fraction_t* a, int count)
{
	for (int k=0; k<count; k++)
	{
		echo(k,					3d, "  ");
		echo(a[k].numerator,	 d, "  ");
		echo(a[k].denominator,	 d, "  ");
		echo(a[k].value,		 f, "  ");
		echo(a[k].upless,		 f, "  ");
		echo(a[k].upmore,		 f, EOL);
	}
}

float verify(fraction_t* c1, fraction_t* c2)
{
// Given two fractions, verify that two circles,
// - tangent to the number line,
// - with the center directly above the point designated by the fraction,
// - with a diameter based on the fraction,
// are tangent to each other 

	fraction_t* f = c1; 
	fraction_t* F = c2;
	if (f->denominator < F->denominator)
	{
		F = c1;
		f = c2;
	}

	float r = 1/(float)(2 * f->denominator * f->denominator);	// radius of smaller circle
	float R = 1/(float)(2 * F->denominator * F->denominator);	// radius of larger circle

	float dV = abs(f->value - F->value);			// distance between values on number line
	float dR = R - r;								// difference between radii
	float dC = R + r;								// distance between centers
	return ((dV * dV) + (dR * dR)) - (dC * dC);		// Should be zero
//	return sqrt((dV * dV) + (dR * dR)) - (r + R);	// Should be zero
}

int main(int argc, char** argv)
{
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
						  memset(a, 0,	kcount * sizeof(fraction_t)	);	// function level variables are NOT initialized to zero.

// Generate fractions, one denominator at a time

	a[0].numerator		= 0;	// End points of our number line
	a[0].denominator	= 1;
	a[0].value			= 0;	// zero
	a[1].numerator		= 1;
	a[1].denominator	= 1;
	a[1].value			= 1;	// and one
	int k = 2;
	for (int i=1; i<=q; i++)	// denominator
	{
		for (int j=1; j<i; j++)		// numerator
		{
			int factor = gcf(j, i);
			a[k].numerator		= j / factor;
			a[k].denominator	= i / factor;
			a[k].value			= (float)a[k].numerator / (float)a[k].denominator;
			k++;	// Cannot increment index in assignment, bad things happen.
		}
		qsort(a, k, sizeof(a[0]), fcmp);	// put them in order
		echo(k, d, " before compression  ");
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
		qsort(a, limit, sizeof(a[0]), fcmp);	// Cast out duplicates

		echo(k, d, " = Number of elements after deleting duplicates" EOL);

		for (int j=1; j<k-1; j++)			// Copy values from parents. Needed to verify circles.
		{									// We'll use these to locate the actual fractions later
			if (a[j].upless == 0)
				a[j].upless =  a[j - 1].value;
			if (a[j].upmore == 0)
				a[j].upmore =  a[j + 1].value;
		}	
	}

	float maxdiff = 0;
	for (int j=1; j<k-1; j++)		// Verify that adjacent circles are tangent to each other
	{
		fraction_t	less;		 less.value = a[j].upless;
		fraction_t	more;		 more.value = a[j].upmore;
		fraction_t*	p = bsearch(&less, a, k, sizeof(a[0]), fcmp);
		fraction_t*	q = bsearch(&more, a, k, sizeof(a[0]), fcmp);
		float	result1 = verify(p, &a[j]);
		float	result2 = verify(q, &a[j]);
		float diff = fabs(result1-result2);
		if (diff!=0)
		{
			if (maxdiff < diff)
				maxdiff = diff;

#define	RCH .0001

			if (diff > RCH)
			{
				echo(j, d, "  ");
				echo(result1, f, "  ");
				echo(result2, f, EOL);
			}
		}
	}
	echo(maxdiff, f, EOL);
	echo(k, d, " = Count of elements in array.  ");
	echo(kcount, d, " = Calculated number" EOL);

	show(a, k);

	int error_count = 0;			// Verify that each fraction is a Farey sum of it's immediate neighbors
	for (int i=0; i<k-2; i++)
	{
		fraction_t	sum;
		sum.numerator	= a[i].numerator	+ a[i+2].numerator;
		sum.denominator = a[i].denominator	+ a[i+2].denominator;
		sum.value		=  (float)sum.numerator / (float)sum.denominator;
		if (sum.value != a[i+1].value)
		{
			echo(i,	d, EOL);
			show(&a[i],	3);
			error_count++;
		}
	}
	echo(error_count, d, EOL);
	return error_count;
}

