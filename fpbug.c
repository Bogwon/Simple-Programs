/* https://community.intel.com/t5/Software-Archive/gcc-5-1-mfpmath-387-Floating-point-exception/m-p/1015475

$  cc -fcilkplus                    fpbug.c -lcilkrts -lm	f(1)+g(2)=0.850648
$  cc -fcilkplus -Wall              fpbug.c -lcilkrts -lm	f(1)+g(2)=0.850648
$  cc -fcilkplus -Wall -mfpmath=387 fpbug.c -lcilkrts -lm	Floating point exception

$  gcc-5.1/bin/gcc -fcilkplus -Wall -mfpmath=387 fpbug.c -o fpbug-gcc510-387 -lcilkrts -lm	$ ./fpbug-gcc510-387	Floating point exception
$  gcc-5.1/bin/gcc -fcilkplus -Wall -mfpmath=sse fpbug.c -o fpbug-gcc510-sse -lcilkrts -lm	$ ./fpbug-gcc510-sse	f(1)+g(2)=0.850648
$ cilk-4.8/bin/gcc -fcilkplus -Wall -mfpmath=387 fpbug.c -o fpbug-cilk48-387 -lcilkrts -lm	$ ./fpbug-cilk48-387	f(1)+g(2)=0.850648
$ cilk-4.8/bin/gcc -fcilkplus -Wall -mfpmath=sse fpbug.c -o fpbug-cilk48-sse -lcilkrts -lm	$ ./fpbug-cilk48-sse	f(1)+g(2)=0.850648
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <cilk/cilk.h>

const struct timespec onesec={1,0};

double f(double x){
    nanosleep(&onesec,0);
    return 2*x*sin(x);
}
double g(double x){
    nanosleep(&onesec,0);
    return x*cos(x);
}

int main(){
    double a,b;
    a=cilk_spawn f(1);
    b=cilk_spawn g(2);
    cilk_sync;
    printf("f(1)+g(2)=%g\n",a+b);
    return 0;
}
