#   include	"bitmapConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	"bmintern.h"
#   include	<utilEndian.h>
#   include	<appDebugon.h>

typedef unsigned long CARD32;
typedef unsigned short CARD16;
typedef unsigned char CARD8;

# define B32 /*nothing*/
# define B16 /*nothing*/
# define B8 /*nothing*/

#define StaticGray		0
#define GrayScale		1
#define StaticColor		2
#define PseudoColor		3
#define TrueColor		4
#define DirectColor		5

#define LSBFirst		0
#define MSBFirst		1

#   include	"XWDFile.h"

/************************************************************************/
/*  Implementing routines.						*/
/************************************************************************/
static int bmReadX11wd(	FILE *			f,
			XWDFileHeader *		xh,
			unsigned char **	pBuf,
			BitmapDescription *	bd );

/************************************************************************/
/*  Read an XWD file.							*/
/************************************************************************/
int bmReadXwdFile(	const char *		filename,
			unsigned char **	pBuf,
			BitmapDescription *	bd,
			int *			pPrivateFormat,
			double *		pCompressionFactor	)
    {
    FILE *		f;
    int			rval= 0;
    int			privateFormat= -1;

    XWDFileHeader	x11Header;

    f= fopen( filename, "rb" );
    if  ( ! f )
	{ SXDEB(filename,f); return -1;	}

    /***************/
    x11Header.header_size= utilGetBeInt32( f );
    x11Header.file_version= utilGetBeInt32( f );
    x11Header.pixmap_format= utilGetBeInt32( f );
    x11Header.pixmap_depth= utilGetBeInt32( f );
    x11Header.pixmap_width= utilGetBeInt32( f );
    x11Header.pixmap_height= utilGetBeInt32( f );
    x11Header.xoffset= utilGetBeInt32( f );
    x11Header.byte_order= utilGetBeInt32( f );
    x11Header.bitmap_unit= utilGetBeInt32( f );
    x11Header.bitmap_bit_order= utilGetBeInt32( f );
    x11Header.bitmap_pad= utilGetBeInt32( f );
    x11Header.bits_per_pixel= utilGetBeInt32( f );
    x11Header.bytes_per_line= utilGetBeInt32( f );
    x11Header.visual_class= utilGetBeInt32( f );
    x11Header.red_mask= utilGetBeInt32( f );
    x11Header.green_mask= utilGetBeInt32( f );
    x11Header.blue_mask= utilGetBeInt32( f );
    x11Header.bits_per_rgb= utilGetBeInt32( f );
    x11Header.colormap_entries= utilGetBeInt32( f );
    x11Header.ncolors= utilGetBeInt32( f );
    x11Header.window_width= utilGetBeInt32( f );
    x11Header.window_height= utilGetBeInt32( f );
    x11Header.window_x= utilGetBeInt32( f );
    x11Header.window_y= utilGetBeInt32( f );
    x11Header.window_bdrwidth= utilGetBeInt32( f );
    /***************/

    switch( x11Header.file_version )
	{
	case XWD_FILE_VERSION:
	    privateFormat= 11;
	    rval= bmReadX11wd( f, &x11Header, pBuf, bd );
	    break;
	case XWD_FILE_VERSION << 24:
	    XDEB(x11Header.file_version); fclose( f ); return -1;
	default:
	    XDEB(x11Header.file_version); fclose( f ); return -1;
	}

    fclose( f );

    if  ( ! rval )
	{ *pPrivateFormat= privateFormat;	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Read an X11 window dump file.					*/
/*									*/
/************************************************************************/

static int bmReadX11wd(	FILE *			f,
			XWDFileHeader *		xh,
			unsigned char **	pBuf,
			BitmapDescription *	bd )
    {
    int			rval= 0;

    unsigned short	one= 1;
    int			byte_order= *(unsigned char *)&one?LSBFirst:MSBFirst;

    RGB8Color *		pal= (RGB8Color *)0;

    unsigned short *	ps;
    unsigned char *	pu;
    unsigned long *	remap;

    int			row;
    int			col;

    switch( xh->pixmap_format )
	{
	case 0:
	    bd->bdBitsPerSample= 1;
	    bd->bdSamplesPerPixel= 1;
	    bd->bdColorEncoding= BMcoBLACKWHITE;
	    break;
	case 1:
	    bd->bdBitsPerSample= xh->pixmap_depth;
	    bd->bdSamplesPerPixel= 1;
	    bd->bdColorEncoding= BMcoBLACKWHITE;
	    break;
	case 2:
	    if  ( xh->colormap_entries == 2 )
		{
		bd->bdBitsPerSample= xh->pixmap_depth;
		bd->bdSamplesPerPixel= 1;
		bd->bdColorEncoding= BMcoBLACKWHITE;
		}
	    else{
		if  ( xh->colormap_entries > 256 )
		    {
		    LLDEB(xh->pixmap_format,xh->colormap_entries);
		    return -1;
		    }
		bd->bdBitsPerSample= xh->pixmap_depth;
		bd->bdSamplesPerPixel= 3;
		bd->bdColorEncoding= BMcoRGB8PALETTE;
		}
	    break;
	default:
	    LDEB(xh->pixmap_format); return -1;
	}

    /*  skip name */
    if  ( xh->header_size > (unsigned long)ftell( f ) )
	{ fseek( f, (long)(xh->header_size - ftell( f ) ), SEEK_CUR ); }

    bd->bdPixelsWide= xh->pixmap_width;
    bd->bdPixelsHigh= xh->pixmap_height;

    if  ( xh->visual_class == TrueColor		||
	  xh->visual_class == DirectColor	)
	{
	XWDColor *		colors= (XWDColor *)0;

	if  ( xh->ncolors > 0 )
	    {
	    unsigned int	cp;

	    colors= (XWDColor *)malloc( xh->ncolors* sizeof(XWDColor) );
	    if  ( ! colors )
		{ LXDEB(xh->ncolors,colors); return -1;	}

	    for ( cp= 0; cp < xh->ncolors; cp++ )
		{
		colors[cp].pixel= utilGetBeInt32( f );
		colors[cp].red= utilGetBeInt16( f );
		colors[cp].green= utilGetBeInt16( f );
		colors[cp].blue= utilGetBeInt16( f );
		colors[cp].flags= getc( f );
		colors[cp].pad= getc( f );
		}
	    }

	if  ( xh->bits_per_pixel == 16 )
	    {
	    unsigned long	rm, gm, bm;
	    int			rs= 0, gs= 0, bs= 0;

	    rm= xh->red_mask;
	    while( ! ( rm & 1 ) )
		{ rm >>= 1; rs++; }

	    gm= xh->green_mask;
	    while( ! ( gm & 1 ) )
		{ gm >>= 1; gs++; }

	    bm= xh->blue_mask;
	    while( ! ( bm & 1 ) )
		{ bm >>= 1; bs++; }

	    pal= (RGB8Color *)malloc(
			( 1 << xh->bits_per_pixel ) * sizeof(RGB8Color) );
	    if  ( ! pal )
		{ LLDEB(xh->ncolors,pal); return -1;	}

	    if  ( xh->ncolors > 0 )
		{
		for ( col= 0; col < 1 << xh->bits_per_pixel; col++ )
		    {
		    pal[col].rgb8Red=	colors[ ( col >> rs ) & rm ].red/ 256;
		    pal[col].rgb8Green=	colors[ ( col >> gs ) & gm ].green/ 256;
		    pal[col].rgb8Blue=	colors[ ( col >> bs ) & bm ].blue/ 256;
		    pal[col].rgb8Alpha= 255;
		    }
		}
	    else{
		for ( col= 0; col < 1 << xh->bits_per_pixel; col++ )
		    {
		    pal[col].rgb8Red=	( 256* ( ( col >> rs ) & rm ) )/ rm;
		    pal[col].rgb8Green=	( 256* ( ( col >> gs ) & gm ) )/ gm;
		    pal[col].rgb8Blue=	( 256* ( ( col >> bs ) & bm ) )/ bm;
		    pal[col].rgb8Alpha= 255;
		    }
		}

	    if  ( colors )
		{ free( colors );	}
	    }
	}
    else{
	if  ( xh->ncolors > 0 )
	    {
	    XWDColor		color;
	    unsigned int	cp;

	    pal= (RGB8Color *)malloc( xh->ncolors* sizeof(RGB8Color) );
	    if  ( ! pal )
		{ LLDEB(xh->ncolors,pal); return -1;	}

	    for ( cp= 0; cp < xh->ncolors; cp++ )
		{
		color.pixel= utilGetBeInt32( f );
		color.red= utilGetBeInt16( f );
		color.green= utilGetBeInt16( f );
		color.blue= utilGetBeInt16( f );
		color.flags= getc( f );
		color.pad= getc( f );

		pal[cp].rgb8Red=	color.red/256;
		pal[cp].rgb8Green=	color.green/256;
		pal[cp].rgb8Blue=	color.blue/256;
		pal[cp].rgb8Alpha=	255;
		}
	    }
	}

    switch( xh->bits_per_pixel )
	{
	case 1:
	    bd->bdBufferLength=
			    xh->bytes_per_line* xh->pixmap_height;
	    bd->bdBitsPerSample= 1;
	    bd->bdSamplesPerPixel= 1;
	    bd->bdColorEncoding= BMcoBLACKWHITE;
	    bd->bdBytesPerRow= xh->bytes_per_line;
	    bd->bdBitsPerPixel= xh->bits_per_pixel;

	    bd->bdUnit= BMunPIXEL;
	    bd->bdXResolution= 1;
	    bd->bdYResolution= 1;

	    *pBuf= (unsigned char *)malloc( bd->bdBufferLength );
	    if  ( ! *pBuf )
		{ LLDEB(bd->bdBufferLength,*pBuf); rval= -1; }
	    else{
		if  ( fread( *pBuf, 1, bd->bdBufferLength, f ) !=
							bd->bdBufferLength )
		    { LDEB(bd->bdBufferLength); free( *pBuf); rval= -1; }
		}
	    if  ( pal )
		{ free( pal );	}
	    break;
	case	8:
	    bd->bdBufferLength= xh->bytes_per_line* xh->pixmap_height;
	    bd->bdBitsPerSample= 8;
	    bd->bdSamplesPerPixel= 3;
	    bd->bdColorCount= xh->ncolors;
	    bd->bdColorEncoding= BMcoRGB8PALETTE;
	    bd->bdBytesPerRow= xh->bytes_per_line;
	    bd->bdBitsPerPixel= xh->bits_per_pixel;

	    bd->bdUnit= BMunPIXEL;
	    bd->bdXResolution= 1;
	    bd->bdYResolution= 1;

	    *pBuf= (unsigned char *)malloc( bd->bdBufferLength );
	    if  ( ! *pBuf )
		{ LLDEB(bd->bdBufferLength,*pBuf); rval= -1; }
	    else{
		if  ( fread( *pBuf, 1, bd->bdBufferLength, f ) !=
							bd->bdBufferLength )
		    { LDEB(bd->bdBufferLength); free( *pBuf); rval= -1; }
		}

	    bd->bdRGB8Palette= pal;
	    break;
	case	16:
	    bd->bdBufferLength= xh->bytes_per_line* xh->pixmap_height;
	    bd->bdBitsPerSample= 8;
	    bd->bdSamplesPerPixel= 3;
	    bd->bdColorCount= 1 << xh->bits_per_pixel;
	    bd->bdColorEncoding= BMcoRGB8PALETTE;
	    bd->bdBytesPerRow= xh->bytes_per_line;
	    bd->bdBitsPerPixel= xh->bits_per_pixel;

	    bd->bdUnit= BMunPIXEL;
	    bd->bdXResolution= 1;
	    bd->bdYResolution= 1;

	    *pBuf= (unsigned char *)malloc( bd->bdBufferLength );
	    if  ( ! *pBuf )
		{ LLDEB(bd->bdBufferLength,*pBuf); rval= -1; }
	    else{
		if  ( fread( *pBuf, 1, bd->bdBufferLength, f ) !=
							bd->bdBufferLength )
		    { LDEB(bd->bdBufferLength); free( *pBuf); rval= -1; }
		}

	    remap= (unsigned long *)malloc(
			( 1 << xh->bits_per_pixel )* sizeof(unsigned long) );
	    if  ( ! remap )
		{ XDEB(remap); return -1;	}

	    for ( col= 0; col < ( 1 << xh->bits_per_pixel ); col++ )
		{ remap[col]= 0; }

	    if  ( (int)byte_order != (int)xh->byte_order )
		{
		for ( row= 0; row < bd->bdPixelsHigh; row++ )
		    {
		    pu= *pBuf+ row* bd->bdBytesPerRow;
		    ps= (unsigned short *) ( *pBuf+ row* xh->bytes_per_line );

		    for ( col= 0; col < bd->bdPixelsWide; ps++, pu += 2, col++ )
			{
			unsigned char c;
			c= pu[0]; pu[0]= pu[1]; pu[1]= c;
			remap[*ps]= 1;
			}
		    }
		}
	    else{
		for ( row= 0; row < bd->bdPixelsHigh; row++ )
		    {
		    pu= *pBuf+ row* bd->bdBytesPerRow;
		    ps= (unsigned short *) ( *pBuf+ row* xh->bytes_per_line );

		    for ( col= 0; col < bd->bdPixelsWide; ps++, pu += 2, col++ )
			{ remap[*ps]= 1; }
		    }
		}

	    row= 0;
	    for ( col= 0; col < ( 1 << xh->bits_per_pixel ); col++ )
		{
		if  ( remap[col] )
		    { pal[row]= pal[col]; remap[col]= row++; }
		}

	    bd->bdColorCount= row;

	    if  ( row < 256 )
		{
		bd->bdBufferLength /= 2;
		bd->bdBitsPerSample= 8;
		bd->bdSamplesPerPixel= 3;
		bd->bdBytesPerRow /= 2;
		bd->bdBitsPerPixel= 8;

		for ( row= 0; row < bd->bdPixelsHigh; row++ )
		    {
		    pu= *pBuf+ row* bd->bdBytesPerRow;
		    ps= (unsigned short *) ( *pBuf+ row* xh->bytes_per_line );

		    for ( col= 0; col < bd->bdPixelsWide; ps++, pu++, col++ )
			{ *pu= remap[*ps]; }
		    }
		}
	    else{
		for ( row= 0; row < bd->bdPixelsHigh; row++ )
		    {
		    ps= (unsigned short *)*pBuf+ row* xh->bytes_per_line;

		    for ( col= 0; col < bd->bdPixelsWide; ps++, pu++, col++ )
			{ *ps= remap[*ps]; }
		    }
		}

	    free( remap );

	    bd->bdRGB8Palette= pal;
	    break;
	case	32:
	default:
	    LLDEB(xh->bits_per_pixel,xh->bitmap_unit);
	    rval= -1;
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Write an XWD file.							*/
/*									*/
/************************************************************************/

int bmWriteXwdFile(	const char *			filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    { LDEB(-1); return -1; }
