// Program to factor a non-negative integer

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	EOL	"\n"
#define	dimensionof(p)	(sizeof(p)/sizeof(p[0]))
#define	echo(p, q, s)	if (ECHO) fprintf(stderr, #p ": %" #q s, p)
int ECHO = 0;

char* commafy(long n)	// prints number n into static buffer, with commas
{
	static	char	buffer[64];		// buffer for commafy
			char*	ptr	= buffer;	// save 
			long	x	= n;
			long	d	= 1;

	while (x>=1000)
	{
		x /= 1000;
		d *= 1000;
	}

	ptr+= sprintf(ptr, "%ld", x);
	while (d > 1)
	{
		n -= (x * d);				// or n %= d;
		d /= 1000;
		x  = n / d;
		ptr+= sprintf(ptr, ",%03ld", x);
	}
	return buffer;	
}

typedef struct
{
	int	value;
	int	count;
}	factor_t;

factor_t factors[33];	// We can only factor numbers up to two billion, so this is more than enough space.	

// If the number can be factored, the largest factor will only be the size of the square root of the number
int	primes[20000] = { 2, 3, 5, 7, 11, 13, 17, 0 };
int	nprimes = 7;

/*	Ensure that we have all the prime numbers up to the required limit.
	Look through our list of prime numbers, find the first prime number that is greater than the 
	limit and then choose the previous one.
	If we find one, return the number of primes (index) in the list that we have looked at.
	If we don't reach our limit, generate prime numbers until we do.
*/

int generate_primes(int limit)
{
	echo(nprimes, d, "  ");
	echo(limit, d, EOL);
	int ix = 0;
	while (ix<nprimes)
	{
		if (primes[ix] == limit)	return ix;
		if (primes[ix] >  limit)	return ix-1;
		ix++;
	}

// ix == nprimes

	int	x = primes[ix-1] + 2;	// first number to test = last prime + 2
	echo(x, d, "  ");
	while (x<limit)
	{
		int jx = 0;
		while(jx < nprimes)
		{
			if (x % primes[jx] == 0)	break;
			jx++;
		}
		if (jx >= nprimes) // x is prime
		{
			primes[jx] = x;		// add it to our list of prime numbers
			echo(		jx , d, "  ");
			echo(primes[jx], d,  EOL);
			nprimes++;
		}
		x += 2;		// next possible prime number
	}
	echo(x, d, "  ");
	echo(nprimes, d, EOL);
	return nprimes;
}

int factor(int n, factor_t* plist)
{
	int count	= 0;
	int	np		= generate_primes(sqrt(n));
	int fx		= 0;
	if (n<=1) return 0;

	for (int ix=0; ix<np; ix++)
	{
//			echo(		ix,	 d, "  ");
//			echo(primes[ix], d, EOL);
		if (n % primes[ix] == 0)
		{
			n			   /= primes[ix];
			plist[fx].value = primes[ix];
			plist[fx].count++;
			count++;
			echo(		  n, d,  EOL);
			if (n<=1) break;

			while (n % primes[ix] == 0)
			{
				n /= primes[ix];
				plist[fx].count++;
				count++;
				echo(		  n, d,  EOL);
				if (n<=1) break;
			}
			fx++;
		}
	}
	if (n > 1)
	{
		echo(fx, d, "  ");
		echo( n, d,  EOL);
		plist[fx].value = n;
		plist[fx].count++;
		count++;
	}
	return count;
}

int main(int count, char** p)
{
	if (count<2)
	{
		printf("Program requires one parameter - a non-negative integer." EOL);
		return -1;
	}
	if (count > 2)
	{
		if (strcmp(p[2], "ECHO")==0)
			ECHO = 1;
	}

	char buffer[64];
	int n = atoi(p[1]);

	sprintf(buffer, "%d", n);
	if (strcmp(buffer, p[1]))
	{
		printf("%s is too large. Limit is 2^31 - 1." EOL, p[1]);
		return -2;
	}

	n = factor(n, factors);

	if (ECHO)
	{
		printf("Number of factors: %d" EOL, n);
		printf("     value     count" EOL);
		int i		= 0;
		while(factors[i].count)
		{
			printf("%10d%10d" EOL, factors[i].value, factors[i].count);
			i++;
		}
	}

	int	i		= 0;
	int product = 1;
	int	join	= 0;

	if (n==1)
		printf( "%s is a prime number and cannot be factored." EOL, commafy(factors[i].value));
	else
	{
		while(factors[i].count)
		{
			if (join) printf(" x ");
			join = 1;

			printf( "%s", commafy(factors[i].value));
			if (factors[i].count > 1)
				printf("^%d",	  factors[i].count );

			while(factors[i].count--)
				product  *=  factors[i].value;

			i++;
		}
		printf(" = %s" EOL, commafy(product));
	}

	if (ECHO)
	{
		printf("Original number: %s" EOL, p[1]);
		printf("Difference: %d" EOL, product - atoi(p[1]));
	}

	return 0;
}
