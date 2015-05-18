char* title     = "Permute";
char* help[]    =
{
    "Generates all permutations of N digits, i.e. from 0 to N-1.",
    "Accepts three parameters: ",
    "    -quiet, so the permuations are not displayed, just the count.",
    "    N, a number from 1 to 20.",
    "    M=n, Method, where n equals 1 thru 4 or A for all.",
    "         Method 1 is swaps digits.",
    "         Method 2 uses an integer flag.",
    "         Method 3 uses a bit flag.",
    "         Method 4 is a special case for the game 0hh1.",
    "The default value of N is 4.",
    "The default method is swap (1).",
    "Lowercase letters a thru k are used as symbols for 10 thru 20.",
    "Running time for the first three methods with N=16 is about a day on a Zotac Zbox.",
    "N! (factorial) permuations are generated. The count is stored in a 64 bit integer.",
    "20 is the largest number that we can compute the factorial for and store.",
    "Charles Pergiel, May 2015."
};

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EOL "\n"
#define MAX 20
#define dimensionof(p)  (sizeof(p)/sizeof(p[0]))
#define putline(p)      fprintf(stderr, "%s" EOL, p)
#define symbol(i)       (i<=9)?(i + '0'):(i + 'a' - 10)

char    show    =  1;
int64_t count   =  0;
int     N       =  4;
char    p   [MAX + 1];
int     mask    = -1;       // used only by xor
int     mint[MAX + 1];      // used only by flag
int     Blue;               // for 0hh1 only
int     Red;                // Number of spaces of these colors left

int64_t factorial(int n)
{
    int64_t x = 1;
    
    while (n)   x *= n--;
    return x;
}

// permute_swap from: http://www.cs.utexas.edu/users/djimenez/utsa/cs3343/lecture25.html
void swap(char* v, char i, char j)
{
    char    t;
    
    t    = v[i];
    v[i] = v[j];
    v[j] = t;
}


int permute_swap(int x)
{
    if (x==N)
    {
        p[x] = 0;
        if (show)
            printf("%s" EOL, p);
        count++;
        return 0;
    }

    permute_swap(x+1);
    for (int j=x+1; j<N; j++)
    {
        swap(p, x, j);
        permute_swap(x+1);
        swap(p, x, j);
    }
    return 0;
}

int permute_xor(int x)
{
    if (x==N)
    {
        p[x] = 0;
        if (show)
            printf("%s" EOL, p);
        count++;
        return 0;
    }

    int bit = 1;
    for (int i=0; i<N; i++)
    {
        if (mask & bit)
        {
            mask ^= bit;
            p[x]    = symbol(i);
            permute_xor(x+1);
            mask ^= bit;
        }
        bit <<= 1;
    }
    return 0;
}

int permute_flag(int x)
{
    if (x==N)
    {
        p[x] = 0;
        if (show)
            printf("%s" EOL, p);
        count++;
        return 0;
    }

    for (int i=0; i<N; i++)
    {
        if (mint[i])
        {
            mint[i] = 0;
            p[x]    = symbol(i);
            permute_flag(x+1);
            mint[i] = 1;
        }
    }
    return 0;
}

int permute_0hh1(int x, int blue, int red)
{
    #define BLUE    '1'
    #define RED     '-'

    if (blue && Blue)
    {
        Blue--;
        p[x] = BLUE;
    }
    else
    if (red && Red)
    {
        Red--;
        p[x] = RED;
    }
    else
        return -1;
        
    x++;
    
    if (x==N)
    {
        p[x] = 0;
        if (show)
            printf("%s" EOL, p);
        count++;
    }
    else
    if (blue==2)
    {
        permute_0hh1(x, 0, 1);
    }
    else
    if (red==2)
    {
        permute_0hh1(x, 1, 0);
    }
    else
    if (blue==1)
    {
        permute_0hh1(x, 2, 0);
        permute_0hh1(x, 0, 1);
    }
    else
    if (red==1)
    {
        permute_0hh1(x, 0, 2);
        permute_0hh1(x, 1, 0);
    }

    x--;
    if (p[x]==RED)      Red++;
    else
    if (p[x]==BLUE)     Blue++;
    return 0;
}

int main(int argc, char** argv)
{
    int     helpx   = 0;
    char    modec   = '1';
    char*   method;
    int     mode;

    putline(title);

    for (int i=1; i<argc; i++)
    {
        if (!strcmp(argv[i], "-quiet"))
            show = 0;
        else 
            if  (isdigit(*argv[i]))
            {
                N = atoi(argv[i]);

                if (N > MAX)    N = MAX;
                else
                if (N <   1)    N =   1;
            }
        else
            if  (   (argv[i][0]=='M')
                &&  (argv[i][1]=='=')
                )
                modec = argv[i][2];
        else
            helpx = 1;
    }
    if  (   (modec >= '1')
        &&  (modec <= '4')
        )
        mode = 1 << (modec - '1');
    else
    {
        if (modec=='A')
            mode = 0xf;
        else
            helpx = 1;
    }
    if (helpx)
    {
        for (int i=0; i<dimensionof(help); i++)
            putline(help[i]);
        return -1;
    }
    fprintf(stderr, "N set to %d" EOL, N);
    fprintf(stderr, "Method set to %c" EOL, modec);

    for (int mx=1; mx<=8; mx<<=1)
    {
        count = 0;
        if (mode & mx & 1)  {
                                method = "swap";
                                for (int i=0; i<N; i++)
                                    p[i] = symbol(i);
                                permute_swap(0);
                            }
        if (mode & mx & 2)  {   
                                method = "flag";
                                for (int i=0; i<N; i++)
                                    mint[i] = 1;
                                permute_flag(0);
                            }
        if (mode & mx & 4)  {   
                                method = "xor";
                                permute_xor( 0);
                            }
        if (mode & mx & 8)  {   method = "0hh1";    
                                if (N & 1)
                                    N--;
                                Blue = N / 2;
                                Red  = N / 2;
                                permute_0hh1(0, 1, 0);
                                permute_0hh1(0, 0, 1);
                            }
                        
        if (mode & mx)
        {
            fprintf(stderr, "Method: %s"    EOL, method);   
            fprintf(stderr, "     N: %d"    EOL, N);
            fprintf(stderr, "    N!: %lld"  EOL, factorial(N));
            fprintf(stderr, " count: %lld"  EOL, count);
            fprintf(stderr, "  diff: %lld"  EOL, count - factorial(N));
        }
    }
    return 0;
}
