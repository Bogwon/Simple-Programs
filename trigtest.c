/* https://community.intel.com/t5/Intel-C-Compiler/What-is-wrong-in-fsin-opcode/m-p/926872

Trig Test

$ cc trigtest.c -lm -DTEST=1
$ time ./a.out
2.356194

real	0m1.046s
user	0m1.044s
sys	0m0.000s


$ cc trigtest.c -lm -DTEST=2
$ time ./a.out
3.141593

real	0m8.666s
user	0m8.640s
sys	0m0.000s

*/

#ifndef TEST
	#error Define TEST as 1 or 2 (-DTEST=1 or -DTEST=2)
#elif TEST!=1 && TEST!=2
	#error Define TEST as 1 or 2 (-DTEST=1 or -DTEST=2)
#endif

#include <stdio.h>
#include <math.h>

int main()
{
	double x=1;
	double y;
	for(int i=1;i<20000000;i++)
	{
		y=sin(x);
#if TEST==1
		y+=cos(x);
#endif
		x+=y;
	}
	printf("%f\n", x);
	return 0;
}

