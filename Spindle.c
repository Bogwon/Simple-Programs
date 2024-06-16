// Compute the volume of a spindle given its length and diameter

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define var float
#define EOL "\n"
#define echo(p, q, s)   fprintf(stderr, #p ": %" #q s, p)

int main(int argc, char** argv)
{
    echo(argc, d, EOL);
    
    var Length  =   70;
    var Beam    =    8;
    var Steps   = 1000;
    echo(argc,d,"  ");
    if (argc>1) Length  = atof(argv[1]);    echo(Length,f, "  ");
    if (argc>2) Beam    = atof(argv[2]);    echo(Beam,  f, "  ");
    if (argc>3) Steps   = atof(argv[3]);    echo(Steps, f, "  ");
    
//     Radius^2         = (Length/2)^2 + (Radius - (Beam/2))^2
//     Radius^2         = (Length/2)^2 + Radius^2 - 2*Radius*(Beam/2) + (Beam^2/4)
//        0             = (Length/2)^2 - 2*Radius*(Beam/2) + (Beam^2/4)
//   2*Radius*(Beam/2)  = (Length/2)^2  + (Beam^2/4)
//   2*Radius           = ((Length/2)^2  + (Beam^2/4)) / (Beam/2)
    var Radius  = (pow(Length/2,2)  + (pow(Beam, 2)/4)) / Beam;
    echo(Radius, f, EOL);
    
    var step    = Length / Steps;
    var start   =   step / 2;
    var volume  = 0;
    var pi      = acos(-1);
    for (var x=start; x<Length; x+=step)
    {
        var radius = sqrt(pow(Radius, 2)- pow((Length/2)-x, 2)) - Radius + (Beam/2);
//      echo(radius, f, EOL);
        volume += pi * pow(radius, 2) * step;
    }
    echo(volume, f, EOL);
    return 0;
}
