// random number test
// main program shuffles a list and then runs the tester procedure on the results

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define	dimensionof(p)	(sizeof(p)/sizeof(p[0]))
#define	EOL				"\n"
#define	echo(p, q, s)	fprintf(stderr, #p ": %" #q s, p)

#define	 LIST_SIZE	10000

int m[LIST_SIZE];
int n[LIST_SIZE];
int* a = m;
int* b = n;

void show(int* a, int n)
{
	int k = 0;
	for (int i=0; i<n; i++)
	{
		for (int j=0; j<n; j++)
			printf("%10d", a[k++]);
		printf(EOL);
	}
}

// ======================= required by tester =======================
#define	IUNI	next()

int	nexti = 0;

int next()
{
	if (nexti >= LIST_SIZE)
		nexti = 0;
	return b[nexti++];
}

int ktbl[ 36];
int	gcd	[100];

void tester(int n)
{
	int mark = 1000;
	int	u;
	int v;
	int w;
// ===================== begin code from jstatsoft =====================
// www.jstatsoft.org/v07/i03/paper
// The following C segment generates n (usually 10 7 ) 32-bit pairs u and v, then increments the gcd and k tables
// for each pair; (IUNI is a #define that provides the random 32-bit integer):

	for (int i=1; i<=n; i++)
	{
		int k = 0;
		do
		{
			u=IUNI; 
			v=IUNI;
		} while (u==0 || v==0); //get non-zero u and v

		do
		{
			w=u%v;
			u=v;
			v=w;
			k++;
		} while(v>0);

		if(u>100)
			u=100; 
		gcd[u]++;

		if(k<3)
			k=3; 

		if(k>35) 
			k=35; 

		ktbl[k]++;
// ===================== end code from jstatsoft =====================

		mark--;
		if (mark==0)
		{
			mark = 1000;
			printf(".");
		}
	}
}

int main(int argc, char** argv)
{
	int count = 1;
	if (argc > 1)
		count = atoi(argv[1]);

	echo(LIST_SIZE, d, "  ");
	echo(count,		d,  EOL);
	for (int i=0; i<LIST_SIZE; i++)
		m[i] = i;

//	srand(time(NULL));
	for (int j=0; j<count; j++)
	{
		int up	 = random() % LIST_SIZE;
		int down = up - 1;
		int odd	 = 1;
		for (int i=0; i<LIST_SIZE; i++)
		{
			if (odd)
			{
				if (up >= LIST_SIZE)
					up = 0;
				b[i] = a[up++];
			}
			else
			{
				if (down < 0)
					down = LIST_SIZE - 1;
				b[i] = a[down--];
			}
			odd = (odd + 1) & 1;
		}
		tester(LIST_SIZE);

		int* x = a;		// Swap input and output buffers
			 a = b;
			 b = x;
	}
	printf(EOL);
	show(ktbl, 6);
	show(gcd, 10);

	return 0;
}

