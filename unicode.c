// generate box drawing characters from unicode number
// https://www.fileformat.info/info/unicode/utf8.htm
// https://www.fileformat.info/info/unicode/block/box_drawing/list.htm
// https://pergelator.blogspot.com/2020/02/curses.html

#include <stdio.h>
#include <string.h>

#define EOL     "\n"

/*	Binary format of bytes in Unicode sequence
														Number of 			Maximum Expressible 
	1st Byte	2nd Byte	3rd Byte	4th Byte		Free Bits			Unicode Value
	0xxxxxxx												   7			007F hex (		127)
	110xxxxx	10xxxxxx							(5+6)    =11			07FF hex (	   2047)
	1110xxxx	10xxxxxx	10xxxxxx				(4+6+6)  =16			FFFF hex (	  65535)
	11110xxx	10xxxxxx	10xxxxxx	10xxxxxx	(3+6+6+6)=21		  10FFFF hex (1,114,111)
*/

int unicode_count(char* ux)		// For valid unicode start bytes, returns length of unicode sequence
{											// For continuation byte, returns -1;
	if ((*ux & 0x80)==   0)	return 1;		// For all other cases, returns 0;
	if ((*ux & 0xC0)==0x80) return -1;
	if ((*ux & 0xE0)==0xC0)	return 2;
	if ((*ux & 0xF0)==0xE0)	return 3;
	if ((*ux & 0xF8)==0xF0)	return 4;
	return 0;
}

int unicode_encode(int n, char* ptr)	// takes the unicode and encodes it in a character string
{										// terminates string with a zero
	int count = 1;						// returns the number of bytes used, not including the zero
	if (n>0x10ffff)	return -1;
	if (n>  0xffff) count = 4;	else
	if (n>   0x7ff)	count = 3;	else
	if (n>    0x7f)	count = 2;

	if (count==1)
		ptr[0] = n;
	else
	{
		for (int i=(count-1); i>0; i--)
		{
			ptr[i] = 0x80 | (0x3f & n);
			n>>=6;
		}
		int mask = (0xff >> (1 + count));
		int code = (0xff << (8 - count)) & 0xff;
		ptr[0] = code | (mask & n);
	}
	ptr[count] = 0;
	return count;
}

int main(void)
{
	int i=0x2500;
	for (int j=0; j<8; j++)				// column headers
		printf("code count char  ");
	printf(EOL);
	for (int j=0; j<16; j++)
	{
		int k = i;					// save starting value
		for (int j=0; j<8; j++)		
		{
			char box[5];
			int count = unicode_encode(i, box);
			if (unicode_count(box)!=count)
				printf("Error - count does not match. count1: %d  count2: %d" EOL, count, unicode_count(box));

			printf("%4x  ", i);
			printf(" %d   ", count);
			printf(" %s    ", box);
			i +=0x10;
		}
		printf(EOL);
		i = k + 1;		// increment for next row
	}
	return 0;
}
