#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<string.h>

#   include	<appDebugon.h>

static unsigned char BMXBMflip[256];

static void bmXbmSetFlip( void )
    {
    int	i;

    for ( i= 0; i < 256; i++ )
	{
	BMXBMflip[i]=	( ( i & 0x80 ) >> 7 )	|
			( ( i & 0x40 ) >> 5 )   |
			( ( i & 0x20 ) >> 3 )   |
			( ( i & 0x10 ) >> 1 )   |
			( ( i & 0x08 ) << 1 )   |
			( ( i & 0x04 ) << 3 )   |
			( ( i & 0x02 ) << 5 )   |
			( ( i & 0x01 ) << 7 )   ;
	}
    }

/************************************************************************/
/*  Read an X bitmap file.						*/
/*  Routine stolen from the X11R6 file 'lib/X11/RdBitF.c'		*/
/************************************************************************/

/*  Fake X headers */
#define Bool unsigned char
#define False 0
#define True 1
#define BitmapOpenFailed -1
#define BitmapFileInvalid -1
#define BitmapNoMemory -1
#define BitmapSuccess 0
#define Xfree(p) free(p)
#define Xmalloc(n) malloc(n)

#define MAX_SIZE 255

/* shared data for the image read/parse logic */
static short hexTable[256];		/* conversion value */
static Bool initialized = False;	/* easier to fill in at run time */


/*
 *	Table index for the hex values. Initialized once, first time.
 *	Used for translation value or delimiter significance lookup.
 */
static void initHexTable( void )
{
    /*
     * We build the table at run time for several reasons:
     *
     *     1.  portable to non-ASCII machines.
     *     2.  still reentrant since we set the init flag after setting table.
     *     3.  easier to extend.
     *     4.  less prone to bugs.
     */
    hexTable['0'] = 0;	hexTable['1'] = 1;
    hexTable['2'] = 2;	hexTable['3'] = 3;
    hexTable['4'] = 4;	hexTable['5'] = 5;
    hexTable['6'] = 6;	hexTable['7'] = 7;
    hexTable['8'] = 8;	hexTable['9'] = 9;
    hexTable['A'] = 10;	hexTable['B'] = 11;
    hexTable['C'] = 12;	hexTable['D'] = 13;
    hexTable['E'] = 14;	hexTable['F'] = 15;
    hexTable['a'] = 10;	hexTable['b'] = 11;
    hexTable['c'] = 12;	hexTable['d'] = 13;
    hexTable['e'] = 14;	hexTable['f'] = 15;

    /* delimiters of significance are flagged w/ negative value */
    hexTable[' '] = -1;	hexTable[','] = -1;
    hexTable['}'] = -1;	hexTable['\n'] = -1;
    hexTable['\t'] = -1;
	
    initialized = True;
}

/*
 *	read next hex value in the input stream, return -1 if EOF
 */
static int NextInt ( FILE *fstream )
{
    int	ch;
    int	value = 0;
    int gotone = 0;
    int done = 0;
    
    /* loop, accumulate hex value until find delimiter  */
    /* skip any initial delimiters found in read stream */

    while (!done) {
	ch = getc(fstream);
	if (ch == EOF) {
	    value	= -1;
	    done++;
	} else {
	    /* trim high bits, check type and accumulate */
	    ch &= 0xff;
	    if (isascii(ch) && isxdigit(ch)) {
		value = (value << 4) + hexTable[ch];
		gotone++;
	    } else if ((hexTable[ch]) < 0 && gotone)
	      done++;
	}
    }
    return value;
}

static int XReadBitmapFileData (
    const char *filename,
    unsigned int *width,                /* RETURNED */
    unsigned int *height,               /* RETURNED */
    unsigned char **data,               /* RETURNED */
    int *x_hot,                         /* RETURNED */
    int *y_hot)                         /* RETURNED */
{
    FILE *fstream;			/* handle on file  */
    unsigned char *bits = NULL;		/* working variable */
    char line[MAX_SIZE];		/* input line from file */
    int size;				/* number of bytes of data */
    char name_and_type[MAX_SIZE];	/* an input line */
    char *type;				/* for parsing */
    int value;				/* from an input line */
    int version10p;			/* boolean, old format */
    int padding;			/* to handle alignment */
    int bytes_per_line;			/* per scanline of data */
    unsigned int ww = 0;		/* width */
    unsigned int hh = 0;		/* height */
    int hx = -1;			/* x hotspot */
    int hy = -1;			/* y hotspot */

    /* first time initialization */
    if (initialized == False) initHexTable();

    if (!(fstream = fopen(filename, "r")))
	{ SDEB(filename); return BitmapOpenFailed; }

    /* error cleanup and return macro	*/
#define	RETURN(code) \
{ if (bits) Xfree ((char *)bits); fclose (fstream); LDEB(1); return code; }

    while (fgets(line, MAX_SIZE, fstream)) {
	if (strlen(line) == MAX_SIZE-1)
	    RETURN (BitmapFileInvalid);
	if (sscanf(line,"#define %s %d",name_and_type,&value) == 2) {
	    if (!(type = strrchr(name_and_type, '_')))
	      type = name_and_type;
	    else
	      type++;

	    if (!strcmp("width", type))
	      ww = (unsigned int) value;
	    if (!strcmp("height", type))
	      hh = (unsigned int) value;
	    if (!strcmp("hot", type)) {
		if (type-- == name_and_type || type-- == name_and_type)
		  continue;
		if (!strcmp("x_hot", type))
		  hx = value;
		if (!strcmp("y_hot", type))
		  hy = value;
	    }
	    continue;
	}
    
	if (sscanf(line, "static short %s = {", name_and_type) == 1)
/*}*/
	  version10p = 1;
	else if (sscanf(line,"static unsigned char %s = {",name_and_type) == 1)
/*}*/
	  version10p = 0;
	else if (sscanf(line, "static char %s = {", name_and_type) == 1)
/*}*/
	  version10p = 0;
	else
	  continue;

	if (!(type = strrchr(name_and_type, '_')))
	  type = name_and_type;
	else
	  type++;

	if (strcmp("bits[]", type))
	  continue;
    
	if (!ww || !hh)
	  { LLDEB(ww,hh); RETURN (BitmapFileInvalid); }

	if ((ww % 16) && ((ww % 16) < 9) && version10p)
	  padding = 1;
	else
	  padding = 0;

	bytes_per_line = (ww+7)/8 + padding;

	size = bytes_per_line * hh;
	bits = (unsigned char *) Xmalloc ((unsigned int) size);
	if (!bits) 
	  { LDEB(bits); RETURN (BitmapNoMemory); }

	if (version10p) {
	    unsigned char *ptr;
	    int bytes;

	    for (bytes=0, ptr=bits; bytes<size; (bytes += 2)) {
		if ((value = NextInt(fstream)) < 0)
		  { LDEB(value); RETURN (BitmapFileInvalid); }
		*(ptr++) = value;
		if (!padding || ((bytes+2) % bytes_per_line))
		  *(ptr++) = value >> 8;
	    }
	} else {
	    unsigned char *ptr;
	    int bytes;

	    for (bytes=0, ptr=bits; bytes<size; bytes++, ptr++) {
		if ((value = NextInt(fstream)) < 0) 
		  { LDEB(value); RETURN (BitmapFileInvalid); }
		*ptr=value;
	    }
	}
    }					/* end while */

    fclose(fstream);
    if (!bits)
	{ XDEB(bits); return (BitmapFileInvalid); }

    *data = bits;
    *width = ww;
    *height = hh;
    if (x_hot) *x_hot = hx;
    if (y_hot) *y_hot = hy;

    return (BitmapSuccess);
}

/************************************************************************/
/*  Write an X bitmap file.						*/
/*  Part of this routine is stolen from: the X11R6 file			*/
/*  'lib/X11/WrBitF.c							*/
/************************************************************************/

#define BYTES_PER_OUTPUT_LINE 12

int bmWriteXbmFile(	const char *			filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    {
    FILE *			stream;
    int				x_hot= -1;
    int				y_hot= -1;
    const char *		name= "IMAGE";
    const unsigned char *	data= buffer;
    int				width= bd->bdPixelsWide;
    int				height= bd->bdPixelsHigh;
    int				size= bd->bdBufferLength;

    int				byte;
    int				c;
    const unsigned char *	ptr;

    stream= fopen( filename, "w" );
    if  ( ! stream )
	{ SDEB(filename); return -1;	}

    bmXbmSetFlip();

/* #>>>>>>>>>>>>>>>>>>> begin FROM X*/

  /* Write out standard header */
  fprintf(stream, "#define %s_width %d\n", name, width);
  fprintf(stream, "#define %s_height %d\n", name, height);
  if (x_hot != -1) {
    fprintf(stream, "#define %s_x_hot %d\n", name, x_hot);
    fprintf(stream, "#define %s_y_hot %d\n", name, y_hot);
  }

  /* Print out the data itself */
  fprintf(stream, "static unsigned char %s_bits[] = {", name);
  for (byte=0, ptr=data; byte<size; byte++, ptr++) {
    if (!byte)
      fprintf(stream, "\n   ");
    else if (!(byte % BYTES_PER_OUTPUT_LINE))
      fprintf(stream, ",\n   ");
    else
      fprintf(stream, ", ");
    c = *ptr;
    if (c<0)
      c += 256;
    fprintf(stream, "0x%02x", BMXBMflip[c]);
  }
  fprintf(stream, "};\n");

  fclose(stream);
  return(BitmapSuccess);
}
/* #<<<<<<<<<<<<<<<<<<< end   FROM X*/

int bmCanWriteXbmFile(	const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    {
    if  ( privateFormat != 11 )
	{ return -1;	}

    if  ( bd->bdBitsPerPixel != 1 )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read an XBM bitmap file.						*/
/*									*/
/************************************************************************/

int bmReadXbmFile(	const char *		filename,
			unsigned char **	pBuffer,
			BitmapDescription *	bd,
			int *			pPrivateFormat,
			double *		pCompressionFactor	)
    {
    unsigned int	width;
    unsigned int	height;
    int			x_hot;
    int			y_hot;
    unsigned char *	s;

    if  ( XReadBitmapFileData (	filename,
				&width,
				&height,
				pBuffer,
				&x_hot, &y_hot	) )
	{  SDEB(filename); return -1;	}

    bd->bdBytesPerRow= ( width+ 7 )/8;
    bd->bdBufferLength= height* bd->bdBytesPerRow;
    bd->bdPixelsWide= width;
    bd->bdPixelsHigh= height;
    bd->bdBitsPerSample= 1;
    bd->bdSamplesPerPixel= 1;
    bd->bdBitsPerPixel= 1;
    bd->bdXResolution= 1;
    bd->bdYResolution= 1;
    bd->bdUnit= BMunPIXEL;
    bd->bdColorEncoding= BMcoBLACKWHITE;
    bd->bdHasAlpha= 0;
    bd->bdColorCount= 0;

    *pPrivateFormat= 11;

    bmXbmSetFlip();
    s= *pBuffer;
    for ( width = 0; width < bd->bdBufferLength; width++ )
	{ *s= BMXBMflip[*s]; s++; }

    return 0;
    }
