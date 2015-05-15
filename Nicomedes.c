char* title = "Trisect an angle as measured from the %c axis.";

/* Conchoid of Nicomedes
Given the x and y coordinates of a vector that is between 0 and 90 degrees
clockwise from the y axis, find the x and y coordinates of a vector whose 
angle is one third that of the first vector.
*/

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define	DEBUG			0
#define	FROM_X_AXIS		1
#define	debugprint(p)	printf(#p ": %f" EOL, p);
#define	EOL				"\r\n"						// End Of Line

// Module level variables
double	fourHsquared;
double	mx;
double	my;
int		count;

// begin rootfinder.c
#define	NONE			 0
#define	BAD_DATA		-1
#define	NO_CONVERGENCE	-2



double iterate(double	x0, 
			   double	x2, 
			   double	(*f)(double), 
			   double	eps, 
			   int		imax, 
			   int*		error)
{
	double	x1,
			xm, 

			ym,
			y0,  y1,  y2,
				 y10, y20,
					  y21,

			b, c,
			temp;
	double	xmlast = x0;
	int		i;

	*error = NONE;
	y0 = f(x0);		if (y0 ==0.0)		  return x0;
	y2 = f(x2); 	if (y2 ==0.0)		  return x2;

	if ((y2 * y0) > 0.0) { *error = BAD_DATA; return x0; }

	for (i = 0; i < imax; i++) 
	{ 
		x1 = 0.5 * (x2 + x0); 
		y1 = f(x1); 
		if (y1			 == 0.0) return x1; 
		if (fabs(x1 - x0) < eps) return x1; 

		if ((y1 * y0) > 0.0)	// exchange values
		{ 
			temp = x0;	x0 = x2;	x2 = temp; 
			temp = y0;	y0 = y2;	y2 = temp; 
		}

		y10 = y1 - y0; 
		y21 = y2 - y1; 
		y20 = y2 - y0; 

		if ((y2 * y20) < (2.0 * y1 * y10)) 
		{ 
			x2 = x1; 
			y2 = y1; 
		} 
		else 
		{ 
			b  = (x1  - x0 ) /  y10;
			c  = (y10 - y21) / (y21 * y20); 
			xm = x0 - b * y0 * (1.0-c * y1); 
			ym = f(xm); 
			if (ym				 == 0.0)
											return xm; 

			if (fabs(xm - xmlast) < eps)
											return xm; 

			xmlast = xm; 
			if ((ym * y0) < 0.0) 
			{ 
				x2 = xm; 
				y2 = ym; 
			} 
			else 
			{ 
				x0 = xm; 
				y0 = ym; 
				x2 = x1; 
				y2 = y1; 
			} 
		} 
	} 
	*error = NO_CONVERGENCE; 
	return x1; 
}
// end rootfinder.c



double compute(double b)
{
	double	k;

	assert(b>=0);

	count++;
	assert(b<=my);

	k = (my / b) - 1;
	return (k*k *  (mx*mx + b*b)) - fourHsquared;
}



double trisect(double x, double y, double eps)
{
/*
Original vector forms a right triangle with the x axis and a vertical line 
constructed parallel to the y axis and running between the point of the 
vector and the x axis. The sides have lengths x, y & h.

Taking the first third of the angle,
1st triangle has sides a, b, c (a = y).
2nd triangle is similar, so it has sides ka, kb, kc.
Hypotenuse of 2nd triangle is double the length of the original vector: kc = 2h
Also:
  kb + b  =  x
 b(k + 1) =  x
   k + 1  =  x / b
   k      = (x / b) - 1

Caret (^) is used to indicate exponentiation:
         (ka)^2   +    (kb)^2    = (kc)^2
      (k^2 * a^2) + (k^2 * b^2)  = (2h)^2
       k^2 * (a^2 +        b^2)  = 4 * h^2
 ((x / b) - 1)^2 *  (a^2 + b^2)  = 4 * h^2
(((x / b) - 1)^2 *  (a^2 + b^2)) - (4 * h^2) = 0

We are given x and a (a is y), and we can easily compute h,
so we just need to find a value for b that will make this equation true.
*/
//	double	h;	// hypoteneuse
	double	result;
	int		error;

#if FROM_X_AXIS
	mx		= x;
	my		= y;
#else
	mx		= y;
	my		= x;
#endif
	count	= 0;

//	h = sqrt( x*x + y*y);
//	fourHsquared = 4 * h * h;
	fourHsquared = 4 * ( x*x + y*y);

	result = iterate(	my * eps,		//	double	x0,		// not zero
						my / 2,			//	double	x2, 
						compute,		//	function
						eps,			//	double	eps, 
						100,			//	int		imax, 
						&error			//	int*	error
					);

	if (DEBUG || error!=NONE)
	{	
		printf("Error: ");
		if (error==NONE)			puts("NONE");			else
		if (error==BAD_DATA)		puts("BAD_DATA");		else
		if (error==NO_CONVERGENCE)	puts("NO_CONVERGENCE");	else
		printf("%d" EOL, error);
	}
	return result;
}



void getenter(void)
{
	printf("Press ENTER to continue/exit.");
	while (getchar()!=0xa)
		;
}



int main(int argc, char** argv)
{
	int		accuracy	= 6;
	double	angle1;
	double	angle2;
	double	eps;
	double	result;
	double	x;	// horizontal coordinate
	double	y;	// vertical coordinate
	double	PI;

	char*	help = 
		"Program to compute coordinates of a vector that is one third the "		EOL
		"angle of the given vector without using trigonometry. Requires two"	EOL
		"parameters: the X & Y coordinates of the a vector in the first "		EOL		
		"quadrant (between 0 and 90 degrees). Accepts an optional third "		EOL
		"parameter: a positive integer specifying the number of decimal digits"	EOL
		"of accuracy. The default is 6."										EOL;

	PI	= acos(-1);
	printf(title, (FROM_X_AXIS)? 'X' : 'Y');
	printf(EOL);

	if ((argc < 3) || (argc > 4))
	{
		printf(help);
		getenter();
		return -1;
	}
	x = atof(argv[1]);
	y = atof(argv[2]);

	if (argc==4)
		accuracy = atoi(argv[3]);
	if ((accuracy < 0) || (accuracy > 20))
	{
		printf("Specified accuracy (%d) out of range (0..20). Using default of 6."
					EOL, accuracy);
		accuracy = 6;
	}
	printf("Precision set to %d decimal places." EOL, accuracy);
	eps = pow(10, -accuracy);

	printf("Coordinates of original point angle: x: %f    y: %f" EOL, x, y);
	if ((x<=0) || (y<=0))
	{
		puts( "Error: point must be in first quadrant, "
				"i.e. both coordinates must be positive.");
		getenter();
		return -2;
	}
	result = trisect( x, y, eps);


#if FROM_X_AXIS
	angle1 = atan(y/x);
	printf("Coordinates of new point:            x: %f    y: %f" EOL, x, result);
	angle2 = atan(result/x);
#else
	angle1 = atan(x/y);
	printf("Coordinates of new point:            x: %f    y: %f" EOL, result, y);
	angle2 = atan(result/y);
#endif

	printf("Number of calls to compute function: %d" EOL, count);
	printf("Orginal angle: %f degrees." EOL, angle1 * 180 / PI);
	printf("    New angle: %f degrees." EOL, angle2 * 180 / PI);
	printf("Error (difference between original angle and 3 times new angle): %f"
			EOL, angle1 - (angle2 * 3));
	getenter();
	return 0;
}
