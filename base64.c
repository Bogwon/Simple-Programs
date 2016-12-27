// base64 codec
// Convert binary files to ASCII (and vice versa) for transmission through
// text channels, like email.
// Similar to uuencode / uudecode, but slightly different character set.
// Set DECODE_TABLE to 1 to use table driven decoding.
// Set DECODE_TABLE to 0 to use character typing procedure for decoding.
// Decode procedure probably won't work with EBCDIC.
// Will table driven decoding work with EBCDIC?
// Does anyone even use EBCDIC anymore?
//
// Charles Pergiel 26 October 2008
// Derived from a version found on http://www.fourmilab.ch/

#define	DECODE_TABLE	1

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE			1
#define FALSE			0
 
typedef unsigned char byte;

int		errcheck		= TRUE;
char*	encode_table	=
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";



int encode_file(FILE* fi, FILE* fo)
{
	#define LINELEN		 72

	int		c;
	int		n;
	int		hiteof		= FALSE;
	int		linelength	= 0;
	char*	eol			= "\n";
	byte	igroup	[3];
	byte	ogroup	[4];

	while (!hiteof)
	{
		igroup[0] = 0;
		igroup[1] = 0;
		igroup[2] = 0;
		for (n=0; n<3; n++)
		{
			c = fgetc(fi);
			if (c==EOF)
			{
				hiteof= TRUE;
				break;
			}
			igroup[n]= (byte)c;
		}

		if (n> 0)
		{
			ogroup[0]= encode_table[						   igroup[0]>>2	];
			ogroup[1]= encode_table[((igroup[0] &    3)<<4) | (igroup[1]>>4)];
			ogroup[2]= encode_table[((igroup[1] & 0x0F)<<2) | (igroup[2]>>6)];
			ogroup[3]= encode_table[  igroup[2] & 0x3F						];

			if (n<3)
			{
				ogroup[3]= '=';
				if (n<2)
					ogroup[2]= '=';
			}

			if (linelength>=LINELEN)
			{
				if (fputs(eol,fo)==EOF)
					return 1;

				linelength= 0;
			}
			if (fwrite(ogroup, sizeof(ogroup), 1, fo) != 1)
				return 1;

			linelength += sizeof(ogroup);
		}
	}

	if (fputs(eol,fo)==EOF)
		return 1;
		
	return 0;
}


#if !DECODE_TABLE
char decode_char(int cx)
{
	if (isupper(cx))	return cx - 'A';
	if (islower(cx))	return cx - 'a' + 26;
	if (isdigit(cx))	return cx - '0' + 52;
	if (cx == '+')		return 62;	// (26 letters x 2) + 10 digits
	if (cx == '/')		return 63;
	if (cx == '=')		return 0;
						return cx | (char)0x80;
}
#endif



int decode_file(FILE* fi, FILE* fo)
{
	int		c;
	int		i;
	int		write_count	= 3;
	byte	a[4];
	byte	b[4];
	byte	o[3];

#if DECODE_TABLE
	#define	decode_char(cx)	decode_table[cx]

	char	decode_table[0x100];
	int		limit;
	int		index;

	limit = strlen(encode_table);
	memset(decode_table, 0x80, sizeof(decode_table));
	for (index=0; index<limit; index++)
		decode_table[ encode_table[ index ]] = index;
	decode_table[ '=' ] = 0;
#endif

	while (TRUE)
	{
		for (i= 0;i<4;i++)
		{
			while (1)	// read characters from file until we get a good one.
			{
				c = fgetc(fi);

				if (c==EOF)
				{
					if (i==0)
						return 0;
						
					if (errcheck && (i> 0))
						fprintf(stderr, "Input file incomplete.\n");

					return 1;
				}

				if (isspace(c))		// Ignore spaces, carriage returns
					continue;		//  & line feeds

				if (c <= 0x20)		// as well as any other control characters
				{
					if (errcheck) fprintf(stderr, 
					"Control character '%c' (0x%02x) in input file.\n", c, c);
					continue;
				}

				break;
			}

			a[i]= (byte)c;
			c = decode_char((char)c);
			if (c & 0x80)
			{
				if (errcheck) fprintf(stderr, 
					"Illegal character '%c' (0x%02x) in input file.\n", c, c);
				i--;
				continue;
			}
			b[i] = c;
		}

		o[0]= (b[0]<<2)|(b[1]>>4);			// Pack (4) 6-bit values
		o[1]= (b[1]<<4)|(b[2]>>2);			// into (3) 8-bit bytes.
		o[2]= (b[2]<<6)| b[3];

		if (a[2] == '=')	write_count = 1;	else	// Test for end of
		if (a[3] == '=')	write_count = 2;	else	// input file
							write_count = 3;

		if (fwrite(o, write_count, 1, fo) != 1)			// Write decoded data
			return 1;

		if (write_count < 3)
			return 0;
	}
}



void usage(void)
{
	printf(
		"\nUsage:   base64 [options] [infile [outfile]]"
		"\n  base64 encodes binary files as text for transmission over text channels."
		"\n  base64 can also decode these encoded files to restore the original binary."
		"\n  If files are not specified, stdin & stdout are used."
		"\n  Options may appear anywhere on the command line."
		"\n  Encode is the default mode of operation."
		"\n"
		"\nOptions:"
		"\n        -d, --decode      Decode base64 encoded file"
		"\n        -e, --encode      Encode file into base64"
		"\n        -n, --noerrcheck  Do not post error messages when decoding"
		"\n        -u, --help        Print this message"
		"\n"
		"\nReturns 0 if all is well,"
		"\n        1 on file I/O error," 
		"\n        2 error opening file,"
		"\n        3 invalid command line."
		"\n"
		"\n");
}



int main(int argc, char** argv)
{
	int		ix;
	int		f			= 0;
	int		decoding	= FALSE;
	FILE*	fi;
	FILE*	fo;
	
	fi = stdin;
	fo = stdout;

	for (ix=1; ix<argc; ix++)
	{
		if (argv[ix][0] == '-')
		{
			if (!strcmp(argv[ix], "-d"			)) { decoding = 1; continue; }
			if (!strcmp(argv[ix], "-e"			)) { decoding = 0; continue; }
			if (!strcmp(argv[ix], "-n"			)) { errcheck = 0; continue; }
			if (!strcmp(argv[ix], "-u"			)) { usage();	   return 0; }

			if (!strcmp(argv[ix], "--decode"	)) { decoding = 1; continue; }
			if (!strcmp(argv[ix], "--encode"	)) { decoding = 0; continue; }
			if (!strcmp(argv[ix], "--noerrcheck")) { errcheck = 0; continue; }
			if (!strcmp(argv[ix], "--help"		)) { usage();	   return 0; }
			
			printf("\nInvalid option: %s\n", argv[ix]);{usage();   return 3; }
		}

		switch (f)
		{
		case 0:
				fi = fopen(argv[ix], "rb");
				if (fi == NULL)
				{
					fprintf(stderr,"Cannot open input file %s\n", argv[ix]);
					return 2;
				}
				f++;
				break;

		case 1:
				fo = fopen(argv[ix], "wb");
				if (fo == NULL)
				{
					fprintf(stderr,"Cannot open output file %s\n", argv[ix]);
					return 2;
				}
				f++;
				break;

		default:
				fprintf(stderr,"Too many file names specified.\n");
				usage();
				return 3;
		}
	}

	if (decoding)
		return decode_file(fi, fo);
	
	return encode_file(fi, fo);
}

