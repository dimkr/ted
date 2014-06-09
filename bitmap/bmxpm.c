#   include	"bitmapConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	<ctype.h>

#   include	<X11/xpm.h>

#   include	<appDebugon.h>

#   include	"bmintern.h"

/************************************************************************/
/*  Characters approximately in gray scale order.			*/
/************************************************************************/

static char	BMXPMBytes[]=
    " "
    "."
    "-,`'"
    "_~:;"
    "*^"
    "!=+<>?1ijl|/"
    "(){}"
    "[]7"
    "35"
    "IJLT"
    "CF"
    "fcrst"
    "Eea2SZ$G"
    "69"
    "K4AP"
    "ovxyz"
    "nuw"
    "UYVXOkhQ"
    "Bmg8D"
    "bdpq"
    "%&0N"
    "@"
    "RWHM"
    "#"
    ;

/************************************************************************/
/*									*/
/*  Translate an XPM color to BMcoRGB8PALETTE format; update mapping.	*/
/*									*/
/*  1)  Translate transparent to white.					*/
/*									*/
/************************************************************************/

static int bmXpmPaletteColor(	const char *	c_color,
				int *		pTransparent,
				RGB8Color *	rgb8 )
    {
    char	ch;
    unsigned	r, g, b;

    FILE *	f;

    /*  1  */
    if  ( ! c_color				||
	  ! strcmp( c_color, "None" )		||
	  ! strcmp( c_color, "none" )		||
	  ! strcmp( c_color, "Transparent" )	||
	  ! strcmp( c_color, "transparent" )	)
	{
	*pTransparent= 1;
	rgb8->rgb8Red= 255;
	rgb8->rgb8Green= 255;
	rgb8->rgb8Blue= 255;
	rgb8->rgb8Alpha= 0;

	return 0;
	}

    if  ( sscanf( c_color, "#%4x%4x%4x%c", &r, &g, &b, &ch ) == 3 )
	{
	*pTransparent= 0;
	rgb8->rgb8Red= r/ 256;
	rgb8->rgb8Green= g/ 256;
	rgb8->rgb8Blue= b/ 256;
	rgb8->rgb8Alpha= 255;
	
	return 0;
	}

    if  ( sscanf( c_color, "#%2x%2x%2x%c", &r, &g, &b, &ch ) == 3 )
	{
	*pTransparent= 0;
	rgb8->rgb8Red= r;
	rgb8->rgb8Green= g;
	rgb8->rgb8Blue= b;
	rgb8->rgb8Alpha= 255;
	
	return 0;
	}

    if  ( sscanf( c_color, "#%1x%1x%1x%c", &r, &g, &b, &ch ) == 3 )
	{
	*pTransparent= 0;
	rgb8->rgb8Red= r* 16;
	rgb8->rgb8Green= g* 16;
	rgb8->rgb8Blue= b* 16;
	rgb8->rgb8Alpha= 255;
	
	return 0;
	}

    f= fopen( "/usr/lib/X11/rgb.txt", "r" );
    if  ( f )
	{
	char	line[100];
	char	color[100];

	while( fgets( line, 100, f ) )
	    {
	    if  ( sscanf( line, "%u %u %u %s", &r, &g, &b, color ) == 4 )
		{
		const char *	s1= color;
		const char *	s2= c_color;

		for (;;)
		    {
		    char	c1;
		    char	c2;

		    while( *s1 == ' ' )	{ s1++;	}
		    while( *s2 == ' ' )	{ s2++;	}

		    c1= *s1; c2= *s2;

		    if  ( ! c1 && ! c2 )
			{
			*pTransparent= 0;
			rgb8->rgb8Red= r;
			rgb8->rgb8Green= g;
			rgb8->rgb8Blue= b;
			rgb8->rgb8Alpha= 255;

			fclose( f ); return 0;
			}

		    if  ( isupper( c1 ) )	{ c1= tolower( c1 );	}
		    if  ( isupper( c2 ) )	{ c2= tolower( c2 );	}

		    if  ( c1 != c2 )
			{ break;	}

		    s1++; s2++;
		    }
		}
	    }

	fclose( f );
	}

    SDEB(c_color); return -1;
    }

/************************************************************************/
/*									*/
/*  Read an XPM file.							*/
/*									*/
/************************************************************************/

int bmReadXpmFile(	const char *		filename,
			unsigned char **	pBuffer,
			BitmapDescription *	bd,
			int *			pPrivateFormat,
			double *		pCompressionFactor )
    {
    int			ret;
    unsigned int	row;
    unsigned int	col;
    unsigned char *	buffer;
    
    XpmImage		image;

    int			bitsPerPixel= 8;

    ret= XpmReadFileToXpmImage( (char *)filename, &image, (XpmInfo *)0 );

    if  ( ret != XpmSuccess )
	{ SSDEB(filename,XpmGetErrorString(ret)); return -1;	}

    if  ( image.ncolors > 256* 256 )
	{ LDEB(image.ncolors); XpmFreeXpmImage( &image ); return -1;	}

    bd->bdPixelsWide= image.width;
    bd->bdPixelsHigh= image.height;

    if  ( image.ncolors > 256 && image.ncolors < 256* 256 )
	{ bitsPerPixel= 16;	}

    switch( bitsPerPixel )
	{
	case 8:
	case 16:
	    bd->bdBitsPerSample= 8;
	    bd->bdSamplesPerPixel= 3;
	    bd->bdBitsPerPixel= bitsPerPixel;
	    bd->bdBytesPerRow= ( bd->bdBitsPerPixel+ 7 )/8* bd->bdPixelsWide;
	    bd->bdBufferLength= bd->bdPixelsHigh* bd->bdBytesPerRow;
	    bd->bdColorEncoding= BMcoRGB8PALETTE;
	    bd->bdHasAlpha= 0;
	    bd->bdColorCount= image.ncolors;
	    bd->bdRGB8Palette= (RGB8Color *)
			    malloc( bd->bdColorCount* sizeof( RGB8Color ) );
	    if  ( ! bd->bdRGB8Palette )
		{
		LXDEB(bd->bdColorCount,bd->bdRGB8Palette);
		XpmFreeXpmImage( &image );
		return -1;
		}

	    for ( col= 0; col < image.ncolors; col++ )
		{
		int		transparent= 0;

		if  ( bmXpmPaletteColor( image.colorTable[col].c_color,
						&transparent,
						bd->bdRGB8Palette+ col ) )
		    {
		    SSDEB(image.colorTable[col].string,
					    image.colorTable[col].c_color);
		    XpmFreeXpmImage( &image );
		    return -1;
		    }

		if  ( transparent && ! bd->bdHasAlpha )
		    {
		    bd->bdHasAlpha= 1;
		    bd->bdBitsPerPixel= 2* bitsPerPixel;
		    }
		}

	    if  ( bmCalculateSizes( bd ) )
		{
		LXDEB(bd->bdColorCount,bd->bdRGB8Palette);
		XpmFreeXpmImage( &image );
		return -1;
		}

	    buffer= (unsigned char *)malloc( bd->bdBufferLength );
	    if  ( ! buffer )
		{
		LXDEB(bd->bdBufferLength,buffer);
		XpmFreeXpmImage( &image );
		return -1;
		}
	    break;

	default:
	    LDEB(bitsPerPixel);
	    LLDEB(image.cpp,image.ncolors);
	    XpmFreeXpmImage( &image );
	    return -1;
	}

    bd->bdXResolution= bd->bdYResolution= 1;
    bd->bdUnit= BMunPIXEL;

    switch( bd->bdBitsPerPixel )
	{
	case 8:
	    if  ( ! bd->bdHasAlpha )
		{
		unsigned int *	from= image.data;

		for ( row= 0; row < bd->bdPixelsHigh; row++ )
		    {
		    unsigned char *	to= buffer+ row* bd->bdBytesPerRow;

		    for ( col= 0; col < bd->bdPixelsWide; to++, from++, col++ )
			{ *to= *from; }
		    }
		}
	    else{
		LDEB(bd->bdHasAlpha);
		free( buffer );
		XpmFreeXpmImage( &image );
		return -1;
		}

	    break;

	case 16:
	    if  ( ! bd->bdHasAlpha )
		{
		const unsigned int *	from;
		BmUint16 *		to;

		from= image.data;
		to= (BmUint16 *)buffer;

		for ( col= 0; col < bd->bdBufferLength; col++ )
		    { *(to++)= *(from++);	}
		}
	    else{
		const unsigned int *	from;
		unsigned char *		to;

		from= image.data;
		to= buffer;

		for ( col= 0; col < bd->bdBufferLength; col += 2 )
		    {
		    int	c= *(from++);

		    *(to++)= c;
		    *(to++)= bd->bdRGB8Palette[c].rgb8Alpha;
		    }
		}
	    break;

	case 32:
	    if  ( ! bd->bdHasAlpha )
		{
		LDEB(bd->bdHasAlpha);
		free( buffer );
		XpmFreeXpmImage( &image );
		return -1;
		}
	    else{
		const unsigned int *	from;
		unsigned char *		to;

		/* expand palette and alpha */
		from= image.data;
		to= buffer;

		for ( col= 0; col < bd->bdBufferLength; col += 4 )
		    {
		    int	c= *(from++);

		    *(to++)= bd->bdRGB8Palette[c].rgb8Red;
		    *(to++)= bd->bdRGB8Palette[c].rgb8Green;
		    *(to++)= bd->bdRGB8Palette[c].rgb8Blue;
		    *(to++)= bd->bdRGB8Palette[c].rgb8Alpha;
		    }

		if  ( bd->bdRGB8Palette )
		    {
		    free( bd->bdRGB8Palette );
		    bd->bdRGB8Palette= (RGB8Color *)0;
		    }

		bd->bdColorEncoding= BMcoRGB;
		bd->bdHasAlpha= 1;
		bd->bdBitsPerSample= 8;
		bd->bdSamplesPerPixel= 4;
		bd->bdBitsPerPixel= 32;
		}
	    break;

	default:
	    free( buffer );
	    XpmFreeXpmImage( &image );
	    LLDEB(bd->bdBitsPerPixel,image.cpp); return -1;
	}

    XpmFreeXpmImage( &image );

    *pBuffer= buffer;
    *pPrivateFormat= 0;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write an XPM file.							*/
/*									*/
/************************************************************************/

static void bmXpmClean(		XpmImage *	xpmi )
    {
    unsigned		col;

    if  ( xpmi->data )
	{ free( xpmi->data );	}

    for ( col= 0; col < xpmi->ncolors; col++ )
	{
	if  ( xpmi->colorTable[col].string )
	    { free( xpmi->colorTable[col].string );	}
	if  ( xpmi->colorTable[col].symbolic )
	    { free( xpmi->colorTable[col].symbolic );	}
	if  ( xpmi->colorTable[col].m_color )
	    { free( xpmi->colorTable[col].m_color );	}
	if  ( xpmi->colorTable[col].g4_color )
	    { free( xpmi->colorTable[col].g4_color );	}
	if  ( xpmi->colorTable[col].g_color )
	    { free( xpmi->colorTable[col].g_color );	}
	if  ( xpmi->colorTable[col].c_color )
	    { free( xpmi->colorTable[col].c_color );	}
	}

    if  ( xpmi->colorTable )
	{ free( xpmi->colorTable );	}

    return;
    }

static int bmXpmAllocColors(	XpmImage *	xpmi,
				int		ncolors )
    {
    int		col;

    xpmi->colorTable= (XpmColor *)malloc( xpmi->ncolors* sizeof(XpmColor) );
    if  ( ! xpmi->colorTable )
	{ LXDEB(ncolors,xpmi->colorTable); return -1;	}

    for ( col= 0; col < ncolors; col++ )
	{
	xpmi->colorTable[col].string= (char *)0;
	xpmi->colorTable[col].symbolic= (char *)0;
	xpmi->colorTable[col].m_color= (char *)0;
	xpmi->colorTable[col].g4_color= (char *)0;
	xpmi->colorTable[col].g_color= (char *)0;
	xpmi->colorTable[col].c_color= (char *)0;
	}

    xpmi->ncolors= ncolors;

    return 0;
    }

static int bmXpmSetColor(	XpmColor *	xpmc,
				int		charactersPerPixel,
				char *		bytes,
				int		r,
				int		g,
				int		b )
    {
    xpmc->c_color= (char *)malloc( 14 );
    if  ( ! xpmc->c_color )
	{ XDEB(xpmc->c_color); return -1;	}

    xpmc->string= (char *)malloc( charactersPerPixel+ 1 );
    if  ( ! xpmc->string )
	{ XDEB(xpmc->string); return -1;	}

    sprintf( xpmc->c_color, "#%04x%04x%04x", 257* r, 257* g, 257* b );

    memcpy( xpmc->string, bytes, charactersPerPixel );
    xpmc->string[charactersPerPixel]= '\0';

    return 0;
    }

int bmWriteXpmFile(	const char *			filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    {
    int				ret;
    unsigned int		col;
    int				row;
    XpmImage			image;

    int				charactersPerPixel= 1;

    unsigned int *		to;
    const unsigned char *	from;

    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE )
	{
	if  ( bd->bdColorCount >= sizeof(BMXPMBytes) )
	    { LLDEB(bd->bdColorCount,sizeof(BMXPMBytes)); return -1; }
	}
    else{
	if  ( bd->bdColorEncoding == BMcoBLACKWHITE	||
	      bd->bdColorEncoding == BMcoWHITEBLACK	)
	    {
	    if  ( bd->bdBitsPerPixel > 4 )
		{ LDEB(bd->bdBitsPerPixel); return -1; }
	    }
	else{ LDEB(bd->bdColorEncoding); return -1;	}
	}

    image.width= bd->bdPixelsWide;
    image.height= bd->bdPixelsHigh;
    image.cpp= 1;
    image.ncolors= 0;
    image.colorTable= (XpmColor *)0;
    image.data= (unsigned int *)0;

    switch( bd->bdColorEncoding )
	{
	unsigned int	cp;

	case BMcoRGB8PALETTE:
	    image.cpp= charactersPerPixel;
	    image.ncolors= bd->bdColorCount;
	    ret= image.width* image.height* sizeof( unsigned int );
	    image.data= (unsigned int *)malloc( ret );
	    if  ( ! image.data )
		{ LXDEB(ret,image.data); return -1;	}

	    if  ( bmXpmAllocColors( &image, image.ncolors ) )
		{ LDEB(bd->bdColorCount); bmXpmClean( &image ); return -1; }

	    for ( cp= 0; cp < image.ncolors; cp++ )
		{
		if  ( bmXpmSetColor( image.colorTable+ cp, charactersPerPixel,
					BMXPMBytes+ cp,
					bd->bdRGB8Palette[cp].rgb8Red,
					bd->bdRGB8Palette[cp].rgb8Green,
					bd->bdRGB8Palette[cp].rgb8Blue ) )
		    { XDEB(image.colorTable[cp].c_color); return -1;	}
		}

	    break;
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	    image.cpp= charactersPerPixel;
	    image.ncolors= 1 << bd->bdBitsPerPixel;
	    ret= image.width* image.height* sizeof( unsigned int );
	    image.data= (unsigned int *)
				    malloc( ret+ 7* sizeof( unsigned int ) );
	    if  ( ! image.data )
		{ LXDEB(ret,image.data); return -1;	}

	    if  ( bmXpmAllocColors( &image, image.ncolors ) )
		{ LDEB(bd->bdColorCount); bmXpmClean( &image ); return -1; }

	    for ( cp= 0; cp < image.ncolors; cp++ )
		{
		int	gray= ( 255* cp )/ ( image.ncolors- 1 );

		if  ( bd->bdColorEncoding == BMcoBLACKWHITE )
		    { gray= 255- gray;	}

		if  ( bmXpmSetColor( image.colorTable+ cp, charactersPerPixel,
					BMXPMBytes+ cp,
					gray, gray, gray ) )
		    { XDEB(image.colorTable[cp].c_color); return -1;	}
		}

	    break;
	default:
	    LDEB(bd->bdColorEncoding); bmXpmClean( &image ); return -1;
	}

    switch( bd->bdBitsPerPixel )
	{
	case 1:
	    to= image.data;
	    for ( row= 0; row < bd->bdPixelsHigh; row++ )
		{
		from= buffer+ row* bd->bdBytesPerRow;
		to= image.data+ row* charactersPerPixel* image.width;

		for ( col= 0; col < bd->bdBytesPerRow; from++, col++ )
		    {
		    *(to++)= ( *from & 0x80 ) != 0;
		    *(to++)= ( *from & 0x40 ) != 0;
		    *(to++)= ( *from & 0x20 ) != 0;
		    *(to++)= ( *from & 0x10 ) != 0;
		    *(to++)= ( *from & 0x08 ) != 0;
		    *(to++)= ( *from & 0x04 ) != 0;
		    *(to++)= ( *from & 0x02 ) != 0;
		    *(to++)= ( *from & 0x01 ) != 0;
		    }
		}
	    break;
	case 2:
	    to= image.data;
	    for ( row= 0; row < bd->bdPixelsHigh; row++ )
		{
		from= buffer+ row* bd->bdBytesPerRow;
		to= image.data+ row* charactersPerPixel* image.width;

		for ( col= 0; col < bd->bdBytesPerRow; from++, col++ )
		    {
		    *(to++)= ( *from & 0xc0 ) >> 6;
		    *(to++)= ( *from & 0x30 ) >> 4;
		    *(to++)= ( *from & 0x0c ) >> 2;
		    *(to++)= ( *from & 0x03 );
		    }
		}
	    break;
	case 4:
	    to= image.data;
	    for ( row= 0; row < bd->bdPixelsHigh; row++ )
		{
		from= buffer+ row* bd->bdBytesPerRow;

		for ( col= 0; col < bd->bdPixelsWide- 1; from++, col += 2 )
		    {
		    *(to++)= ( *from & 0xf0 ) >> 4;
		    *(to++)= ( *from & 0x0f );
		    }

		if  ( col < bd->bdPixelsWide )
		    { *(to++)= ( *from & 0xf0 ) >> 4; }
		}
	    break;
	case 8:
	    to= image.data;
	    for ( row= 0; row < bd->bdPixelsHigh; row++ )
		{
		from= buffer+ row* bd->bdBytesPerRow;

		for ( col= 0; col < bd->bdPixelsWide; col++ )
		    { *(to++)= *(from++); }
		}
	    break;
	default:
	    LDEB(bd->bdBitsPerPixel); return -1;
	}


    ret= XpmWriteFileFromXpmImage( (char *)filename, &image, (XpmInfo *)0 );
    if  ( ret != XpmSuccess )
	{ SSDEB(filename,XpmGetErrorString(ret)); ret= -1;	}
    else{ ret= 0;	}

    bmXpmClean( &image );

    return ret;
    }

int bmCanWriteXpmFile( const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    {
    if  ( bd->bdColorEncoding == BMcoBLACKWHITE	||
	  bd->bdColorEncoding == BMcoWHITEBLACK	)
	{
	if  ( bd->bdBitsPerPixel <= 4 )
	    { return 0; }
	}

    if  ( bd->bdColorEncoding != BMcoRGB8PALETTE )
	{ return -1;	}

    if  ( bd->bdColorCount >= sizeof(BMXPMBytes) )
	{ LLDEB(bd->bdColorCount,sizeof(BMXPMBytes)); return -1; }

    return 0;
    }
