// xfontlol.c

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/cursorfont.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define dimensionof(p)  (sizeof(p)/sizeof(p[0]))
#define EOL				"\n"
#define	EXPLAIN			0
#define	SHOW_ERROR		1
#define	SHOW_INFO		1
#define	SHOW_STRINGS	1
#define	SHOW_PAIRS		0
#define	DEBUG			0

	char*	program_name;
	char**	fonts;
Display	*	dpy;

Display* Open_Display(char *display_name)	// Called only by main
{											// Only need this to get access to fonts
	Display *d;

	d = XOpenDisplay(display_name);
	if (d == NULL)
	{
		if (SHOW_ERROR) fprintf (stderr, "%s:  unable to open display '%s'" EOL,
			 program_name, XDisplayName (display_name));
		exit(-1);
	}
	return d;
}

char** get_list(int* available, XFontStruct** infox)	// Called only by main
{
	int		nnames	= INT_MAX;
	char*	pattern	= "*";
    char**	fonts;

	fonts = XListFontsWithInfo(dpy, pattern, nnames, available, infox  );
    if (EXPLAIN && (fonts == NULL))
    	fprintf(stderr, "%s: pattern \"%s\" unmatched" EOL, program_name, pattern);

    return fonts;
}

char* known_property(char* atom)	// called only by PrintProperty
{
    char* atom_names[] = {        /* values are atoms */
    "FOUNDRY",                    /* font name components (see section 3.2 of the XLFD) */
    "FAMILY_NAME",
    "WEIGHT_NAME",
    "SLANT",
    "SETWIDTH_NAME",
    "ADD_STYLE_NAME",
    "SPACING",
    "CHARSET_REGISTRY",
    "CHARSET_ENCODING",
    "FONT",                         /* other standard X font properties (see section 3.2 of the XLFD) */
    "FACE_NAME",
    "FULL_NAME",                    /* deprecated */
    "COPYRIGHT",
    "NOTICE",
    "FONT_TYPE",
    "FONT_VERSION",
    "RASTERIZER_NAME",
    "RASTERIZER_VERSION",
    "_ADOBE_POSTSCRIPT_FONTNAME",   /* other registered font properties (see the X.org Registry, sec. 15) */
    "CHARSET_COLLECTIONS",          /*     unregistered font properties */
    "CLASSIFICATION",
    "DEVICE_FONT_NAME",
    "FONTNAME_REGISTRY",
    "MONOSPACED",
    "QUALITY",
    "RELATIVE_SET",
    "STYLE"
    };

    for (int i = 0; i<dimensionof(atom_names); i++)
    {
        if (strcmp(atom_names[i], atom) == 0)
        	return atom_names[i];
    }
    return NULL;
}

static
int IgnoreError(Display *disp, XErrorEvent *event)
{
    return 0;
}

// Each font has a list of associated properties.
// Each property has a value.
// Property and value are identified by strings.
// All strings are stored in 'master', an array of characters.
// A pointer to each string in master is recorded in 'list', an array of data
// structures.
// 'pair' is another array of data structures.
// Each structure in 'pair' contains two pointers into 'master',
// one for the property name and one for the value.
// 'list' is maintained simply for quick lookup of a string. Since it gets
// reordered every time a new string is added, it does us no good to record
// pointers (or indexes) into list.

#define	BLOCK_SIZE	100000
#define	MASTER_SIZE	1000000

char	master[MASTER_SIZE];					// string storage
char*	master_next  =  master;
char*	master_limit = &master[MASTER_SIZE];

#define	LIST_SIZE	BLOCK_SIZE
int		list_next	= 0;

enum ptypes { NONE, VALUE, PROPERTY };

struct item_types								// one for each string
{
	char*	s;
	int		type;
	int		count;
}	list[LIST_SIZE];
typedef struct item_types	item_type;

struct pairs									// one for each pair (property, value)
{
	char*	pp;
	char*	vp;
	int		count;
};
typedef struct pairs	pair_t;
pair_t	pair[BLOCK_SIZE];
int		pair_next = 0;

int item_strcmp1(const void* p1, const void* p2)
{
	return strcmp((char*)p1, ((item_type*)p2)->s);
}

int item_strcmp2(const void* p1, const void* p2)
{
	return strcmp(((item_type*)p1)->s, ((item_type*)p2)->s);
}

int item_countcmp(const void* p1, const void* p2)	// Ascending numeric order
{
	return ((item_type*)p2)->count - ((item_type*)p1)->count;
}

char* save_string(char* s, int type)
{
	item_type*	result;

	result = bsearch(s, list, list_next,  sizeof(item_type), item_strcmp1);
	if (result)
	{
		result->type |= type;
		result->count++;
		return result->s;
	}

	if (list_next>=LIST_SIZE)
		return NULL;							// No room in the index

	int count = strlen(s) + 1;
	if ((master_next + count) >= master_limit)
		return NULL;							// No room at the inn

	strcpy(master_next, s);						// Saving . . .
	list[list_next  ].s		= master_next;
	list[list_next  ].type	= type;
	list[list_next++].count	= 1;
	master_next			   += count;
			 qsort(		list, list_next,  sizeof(item_type), item_strcmp2);
	result = bsearch(s, list, list_next,  sizeof(item_type), item_strcmp1);
	if (result)
		return result->s;

	return NULL;
}

int pair_cmp1(const void* p1, const void* p2)
{
// The pointers pp & vp point into 'master'. Once a string is entered in
// 'master' it does not move, so a pointer to that string will not change
// either. Pointers into 'list' become useless every time 'list' is sorted,
// which happens every time a new string is added to 'master'.

	pair_t*	pa		= (pair_t*)p1;
	pair_t*	pb		= (pair_t*)p2;

	int result = strcmp(pa->pp, pb->pp);	// first test property name
	if (result)
		return result;

	return		 strcmp(pa->vp, pb->vp);		// then test value name
}

int show_pairs(int count)
{
	int	i;

	printf(EOL EOL "Count Property Value" EOL);
	for (i=0; i<count; i++)
		printf("%4d %100s %s\n", pair[i].count, pair[i].pp, pair[i].vp);
	return i;
}

int save_pair(char* p, char* v)
{
	if (pair_next >= BLOCK_SIZE)
		return -1;

	pair[pair_next].pp		= save_string(p, PROPERTY);
	pair[pair_next].vp		= save_string(v, VALUE);

	pair_t*	result = (pair_t*)bsearch(&pair[pair_next], pair, pair_next,  sizeof(pair_t), pair_cmp1);

	if (result)				// pair exists
		result->count++;
	else					// new pair
	{
		pair[pair_next++].count	= 1;
		qsort(pair, pair_next,  sizeof(pair_t), pair_cmp1);
	}
	return 0;
}

int save_pair_n(char* p, int v)
{
	char	buffer[32];		// We only need 12 spaces. Allow 32 for safety.
							// 10 decimal digits in a 32 bit integer,
							// +1 for a minus sign and
							// +1 for the terminating zero.

	sprintf(buffer, "%d", v);
	return save_pair(p, buffer);
}

static
void PrintProperty(XFontProp* prop)		// called only by do_query_font
{
    char*   atom;
    char*   value;

    XErrorHandler oldhandler = XSetErrorHandler(IgnoreError);

    atom = XGetAtomName(dpy, prop->name);
    if (atom)
    {
		value = XGetAtomName(dpy, prop->card32);
		if	(	(		value == NULL)
			||	(strlen(value)==	0)
			)
		{
			if (SHOW_ERROR) fprintf(stderr, "%ld (expected string value)" EOL, prop->card32);
			save_pair_n(atom, prop->card32);
		}
		else
			save_pair(atom, value);

		if (value)
			XFree (value);

		XFree (atom);
    }
    XSetErrorHandler (oldhandler);
}


char* non_cell_character(XFontStruct* fs, XCharStruct* cs)	// Called only by ComputeFontType
{
	if	(	(cs->lbearing > cs->rbearing)
		&&	(	(	(cs->width < 0)
					&&	(	(cs->width		> cs->lbearing	)
						||	(cs->rbearing	>		0		)
						)
				)
			||	(	(cs->width >= 0)
					&&	(	(cs->width		< cs->rbearing	)
						||	(cs->lbearing	< 		0		)
						)
				)
			)
		)
		return "ink outside bounding box";

	if	(cs->descent > fs->descent	)
		return "characters not all same descent";

	if	(cs->ascent  > fs->ascent)
		return "characters not all same ascent";

	return NULL;
}

static
char* ComputeFontType(XFontStruct* fs, char** eptr)		// Called only by do_query_font
{
            int     i;
            char*   reason;
    XCharStruct *   cs;

    if (fs->min_bounds.width != fs->max_bounds.width)
    {
        *eptr = " (min and max widths not equal)";
        return "Proportional";
    }

    if (fs->per_char)		// Check width of all characters,
    {
        for (	cs = fs->per_char,
        		i  = fs->min_char_or_byte2; i <= fs->max_char_or_byte2;	i++,
        																cs++)
        {
            if (cs->width == 0)
            	continue;

            if (cs->width != fs->max_bounds.width)                // this shouldn't happen since we checked above
            {
                *eptr = " (characters not all the same width)";
                return "Proportional";
            }
        }
    }

    Atom awatom = XInternAtom (dpy, "AVERAGE_WIDTH", False);
    if (awatom)
    {
        for (i = 0; i < fs->n_properties; i++)
        {
            if  (   (fs->properties[i].name     ==      awatom                )
                &&  (fs->properties[i].card32   != (fs->max_bounds.width * 10))
                )
            {
                *eptr = "font width not equal to AVERAGE_WIDTH";
                return "Monospaced";
            }
        }
    }

    if (fs->per_char)
    {
        for (	cs = fs->per_char,
        		i  = fs->min_char_or_byte2; i <= fs->max_char_or_byte2;	i++,
        																cs++)
        {
            reason = non_cell_character(fs, cs);
            if (reason)
            {
            	*eptr = reason;
                return "Monospaced";
            }
        }
    }

    *eptr = NULL;
    return "Character Cell";
}


static
void do_query_font (Display *dpy, char *name)	// Called only by show_fonts
{
    		int 	i;
    		char* 	eptr = NULL;
    XFontStruct *	info = XLoadQueryFont (dpy, name);

    if (!info)
    {
    	if (SHOW_ERROR) fprintf (stderr, "%s:  unable to get info about font \"%s\"\n", program_name, name);
        return;
    }

    if (SHOW_INFO)
    {
		printf("name:  %s"			EOL, name 									? name			  : "(nil)"			);
		printf("  direction:\t\t%s" EOL, (info->direction == FontLeftToRight)	? "left to right" : "right to left"	);

		printf("  font type:\t\t%s", ComputeFontType(info, &eptr));
		if (EXPLAIN && eptr)
			printf("%s", eptr);
		printf(EOL);

		printf("  properties:\t\t%d" EOL, info->n_properties);
    }
    for (i=0; 		  i<info->n_properties; i++)
        PrintProperty( &info->properties[i]);

    if (SHOW_INFO)
    	printf(EOL);

    XFreeFontInfo(NULL, info, 1);
}

static
void show_fonts(int font_cnt, XFontStruct* info)	// Called only by main
{
    int i;

    if (font_cnt == 0)
        return;

    if (SHOW_INFO)
    {
		XFontStruct *pfi;
		char        *string;

		printf("DIR  ");
		printf("NAME");
		printf(EOL);

		for (i=0; i<font_cnt; i++)
		{
			pfi = &info[i];
			if (!pfi) {
				if (SHOW_ERROR)
					fprintf(stderr, "%s:  no font information for font \"%s\".\n",
							program_name,
							fonts[i] ?
							fonts[i] : "");
				continue;
			}
			switch(pfi->direction) {
				case FontLeftToRight: string = "-->"; break;		// direction
				case FontRightToLeft: string = "<--"; break;
				default:              string = "???"; break;
			}
			printf("%-4s", string);
			printf("%s" EOL, fonts[i]);
	    }
    }

	for (i=0; i<font_cnt; i++)
		do_query_font(dpy, fonts[i]);

	return;
}

int main(int argc, char** argv)
{
			int		count;
	XFontStruct  *	info = NULL;         // wrong?

	program_name = argv[0];

	dpy = Open_Display(NULL);

	fonts = get_list(&count, &info);
	show_fonts(		  count,  info);

	if (SHOW_STRINGS)
	{
		qsort(list, list_next,  sizeof(item_type), item_countcmp);
		printf(EOL EOL "Type Count Item" EOL);
		for (int i=0; i<list_next; i++)
			printf("%4d %5d %s" EOL, list[i].type, list[i].count, list[i].s);
	}

	if (SHOW_PAIRS)
		for (int i=0; i<pair_next; i++)
		{
			printf("i: %4d  %4d", i, pair[i].count);
			printf(" %08lx", (unsigned long)pair[i].pp);
			printf(" %08lx", (unsigned long)pair[i].vp);
			printf(EOL);
		}

	show_pairs(pair_next);

	return 0;
}

