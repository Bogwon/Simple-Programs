char* help = "Compute the volume of a spindle given its length and diameter.";

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

//  #define var float
#define var double

#define EOL "\n"
#define show(p, q, s)              printf(             "%" #q s, p)
#define echo(p, q, s)   if (ECHO) fprintf(stderr, #p ": %" #q s, p)

var pi;         // = acos(-1);
int ECHO = 0;   // debug

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

char* commafyd(double n)	// prints number n into static buffer, with commas
{
	static	char	buffer[64];		// buffer for commafy
			char*	ptr	= buffer;	// save 
            char    separator = '.';
            
    strcpy(buffer, commafy(trunc(n)));
    ptr = strlen(buffer) + buffer;
    for (int i=0; i<2; i++)
    {
        n -= trunc(n);
        n *= 1000;
        ptr += sprintf(ptr, "%c%03d", separator, (int)trunc(n));
        separator = ',';
    }
    return buffer;	
}

char* commafydx(var v)	// prints number n into static buffer, with commas
{
	static	char	buffer[64];		// buffer for commafy

    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "%f", v);
    char* ptr = strchr(buffer, '.') - 3;
    if (ptr==NULL)
        return buffer;
        
    while (ptr > buffer)
    {
        int n = strlen(ptr);
        memmove(ptr+1, ptr, n);
        *ptr = ',';
        ptr -= 3;
    }
    ptr = strchr(buffer, '.');
    int n = (strlen(ptr+1) / 3) - 1;
    for (int i=0; i<n; i++)
    {
        ptr += 4;
        memmove(ptr+1, ptr, strlen(ptr));
        *ptr = ',';
    }    
    return buffer;	
}

var spindle_volume(var Radius, var Length, var Beam, int Steps)
{
    var step    = Length / Steps;
    var start   =   step / 2;
    var volume  = 0;
    for (var x=start; x<Length; x+=step)
    {
        var radius = sqrt(pow(Radius, 2)- pow((Length/2)-x, 2)) - Radius + (Beam/2);
        echo(radius, f, EOL);
        volume += pi * pow(radius, 2) * step;
    }
    return volume;
}

void spindle_volume_time(var Radius, var Length, var Beam, int Steps)
{
    clock_t Tstart = clock();
    var volume = spindle_volume(Radius, Length, Beam, Steps);
    clock_t Tend = clock();
    printf("%12s  ", commafy(Tend-Tstart));
    printf("%12s  ", commafy(Steps));
    printf("%s" EOL, commafyd(volume));
}

int main(int argc, char** argv)
{
    echo(argc, d, EOL);
    
    if (argv[1] && !isdigit(*argv[1]))
    {
        printf("%s %s Optional parameters: Length Beam Steps" EOL, argv[0], help);
        return -1;
    }
    
    var Length  =   70;
    var Beam    =    8;
    int Steps   =    0;

    if (argc>1) Length  = atof(argv[1]);    echo(Length,f, "  ");
    if (argc>2) Beam    = atof(argv[2]);    echo(Beam,  f, "  ");
    if (argc>3) Steps   = atoi(argv[3]);    echo(Steps, d, "  ");
    
//     Radius^2         = (Length/2)^2 + (Radius - (Beam/2))^2
//     Radius^2         = (Length/2)^2 + Radius^2 - 2*Radius*(Beam/2) + (Beam^2/4)
//        0             = (Length/2)^2 - 2*Radius*(Beam/2) + (Beam^2/4)
//   2*Radius*(Beam/2)  = (Length/2)^2  + (Beam^2/4)
//   2*Radius           = ((Length/2)^2  + (Beam^2/4)) / (Beam/2)

    var Radius  = (pow(Length/2,2)  + (pow(Beam, 2)/4)) / Beam;
    echo(Radius, f, EOL);
    
    pi      = acos(-1);
    
    printf("  CPU ticks       Slices        Volume" EOL);
    
    if (Steps!=0)
        spindle_volume_time(Radius, Length, Beam, Steps);
    else
        for (var i=0; i<9; i++)
            spindle_volume_time(Radius, Length, Beam, pow(10,i));

    return 0;
}