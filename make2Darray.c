// make2Darray.c
// Dynamic 2d array allocation demo
// Chuck Pergiel
// March 22, 2018

#include <stdio.h>
#include <stdlib.h>

#define ECHO            1
#define echo(p, q, s)   if (ECHO) fprintf(stderr, #p ": %" #q s, p)
#define EOL "\n"

typedef	int	array_t;

array_t** make2Darray(int rows, int columns)
{
	int offset(int index, int rows, int columns)
	{
		return		(			rows * sizeof(array_t*))
		 		  +	(index * columns * sizeof(array_t ));
	}

	char*	p = (char*)malloc(offset(rows, rows, columns));
	array_t**	q = (array_t**)p;
	for (int i=0; i<rows; i++)
		q[i] = (array_t*)&p[offset(i, rows, columns)];

	return q;
}

int main(int argc, char** argv)
{
	echo(sizeof(int**), ld, EOL);

	int	   rows = 10;
	int columns = 12;
	if (argc > 1)	   rows = atoi(argv[1]);
	if (argc > 2)	columns = atoi(argv[2]);

	array_t** a = make2Darray(rows, columns);
	int n = 1;
	for (int i=0; i<rows; i++)
	{
		for (int j=0; j<columns; j++)
			a[i][j] = n++;
	}

	for (int i=0; i<rows; i++)
	{
		for (int j=0; j<columns; j++)
			printf("%4d", a[i][j]);
		printf(EOL);
	}
}
