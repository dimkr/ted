/************************************************************************/
/*									*/
/*  Paste images.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	<utilEndian.h>
#   include	<appImage.h>

#   include	<appDebugon.h>

#   ifdef USE_MOTIF

#   include	<X11/Xlib.h>

static int appGetIndexedColors(	XColor **	pColors,
				Display *	display,
				Colormap	cmap,
				int		depth )
    {
    int		count;
    int		col;
    XColor *	xc;

    count= 1 << depth;

    xc= (XColor *)malloc( count* sizeof(XColor) );
    if  ( ! xc )
	{ XDEB(xc); return -1;	}

    for ( col= 0; col < count; col++ )
	{ xc[col].pixel= col; }

    for ( col= 0; col < count; col += 256 )
	{
	int	n= count- col;

	if  ( n > 256 )
	    { n= 256;	}

	XQueryColors( display, cmap, xc+ col, n );
	}

    for ( col= 0; col < count; col++ )
	{ xc[col].pad= 0; }

    *pColors= xc; return count;
    }

static int appGetCalculatedColors(	XColor **		pColors,
					int			depth,
					unsigned long		red_mask,
					unsigned long		green_mask,
					unsigned long		blue_mask )
    {
    int			count;
    unsigned int	col;
    XColor *		xc;

    unsigned int	redShift;
    unsigned long	redMask;
    unsigned int	greenShift;
    unsigned long	greenMask;
    unsigned int	blueShift;
    unsigned long	blueMask;

    unsigned int	r, g, b;

    if  ( red_mask == 0		||
	  green_mask == 0	||
	  blue_mask == 0	)
	{ LLLDEB(red_mask,green_mask,blue_mask); return -1; }

    count= 1 << depth;

    xc= (XColor *)malloc( count* sizeof(XColor) );
    if  ( ! xc )
	{ XDEB(xc); return -1;	}

    for ( col= 0; col < count; col++ )
	{ xc[col].pixel= col; xc[col].pad= 0; }

    redMask= red_mask; redShift= 0;
    while( ! ( redMask & 0x01 ) )
	{ redShift++; redMask= redMask >> 1;	}

    greenMask= green_mask; greenShift= 0;
    while( ! ( greenMask & 0x01 ) )
	{ greenShift++; greenMask= greenMask >> 1;	}

    blueMask= blue_mask; blueShift= 0;
    while( ! ( blueMask & 0x01 ) )
	{ blueShift++; blueMask= blueMask >> 1;	}

    for ( r= 0; r <= redMask; r++ )
	{
	for ( g= 0; g <= greenMask; g++ )
	    {
	    for ( b= 0; b <= blueMask; b++ )
		{
		col=	( r << redShift )	|
			( g << greenShift )	|
			( b << blueShift )	;

		xc[col].red=	( 65535* r )/ redMask;
		xc[col].green=	( 65535* g )/ greenMask;
		xc[col].blue=	( 65535* b )/ blueMask;
		}
	    }
	}

    *pColors= xc; return count;
    }

static int appGetRGB8Description(	BitmapDescription *	bd,
					double			pixelsPerTwip,
					const APP_IMAGE *	xim,
					int			count )
    {
    int		bitsPerPixel;

    bitsPerPixel= xim->bits_per_pixel;

    bd->bdPixelsWide= xim->width;
    bd->bdPixelsHigh= xim->height;

    bd->bdBitsPerSample= 8;
    bd->bdSamplesPerPixel= 3;
    bd->bdBitsPerPixel= bitsPerPixel;

    bd->bdXResolution= (int)( ( 72* 20 )* pixelsPerTwip );
    bd->bdYResolution= (int)( ( 72* 20 )* pixelsPerTwip );
    bd->bdUnit= BMunINCH;

    bd->bdColorEncoding= BMcoRGB8PALETTE;

    bd->bdHasAlpha= 0;

    bd->bdColorCount= 0;
    bd->bdRGB8Palette= (RGB8Color *)malloc( count* sizeof(RGB8Color) );
    if  ( ! bd->bdRGB8Palette )
	{ LXDEB(count,bd->bdRGB8Palette); return -1; }

    bd->bdBytesPerRow= ( bitsPerPixel/ 8 )* bd->bdPixelsWide;
    bd->bdBufferLength= bd->bdPixelsHigh* bd->bdBytesPerRow;

    return 0;
    }

static int appGetRGBDescription(	BitmapDescription *	bd,
					double			pixelsPerTwip,
					const APP_IMAGE *	xim )
    {
    bd->bdPixelsWide= xim->width;
    bd->bdPixelsHigh= xim->height;

    bd->bdBitsPerSample= 8;
    bd->bdSamplesPerPixel= 3;
    bd->bdBitsPerPixel= bd->bdSamplesPerPixel* bd->bdBitsPerSample;

    bd->bdXResolution= (int) ( ( 72* 20 )* pixelsPerTwip );
    bd->bdYResolution= (int) ( ( 72* 20 )* pixelsPerTwip );
    bd->bdUnit= BMunINCH;

    bd->bdColorEncoding= BMcoRGB;

    bd->bdHasAlpha= 0;

    bd->bdColorCount= 0;

    bd->bdBytesPerRow= ( bd->bdBitsPerPixel/ 8 )* bd->bdPixelsWide;
    bd->bdBufferLength= bd->bdPixelsHigh* bd->bdBytesPerRow;

    return 0;
    }

static int appGetRGB8Palette(	XColor *	xc,
				int		count,
				RGB8Color *	palette )
    {
    unsigned int	from;
    unsigned int	to;

    to= 0;
    for ( from= 0; from < count; from++ )
	{
	if  ( ! xc[from].pad )
	    { continue;	}

	palette->rgb8Red= xc[from].red/ 256;
	palette->rgb8Green= xc[from].green/ 256;
	palette->rgb8Blue= xc[from].blue/ 256;

	xc[from].red= to++; palette++;
	}

    return to;
    }

static int appGetImage(	AppBitmapImage *	abi,
			const APP_IMAGE *	xim,
			Display *		display )
    {
    int			bitsPerPixel;
    int			bytesPerLine;
    char *		data;

    int			screen= DefaultScreen( display );
    Colormap		cmap= DefaultColormap( display, screen );
    Visual *		vis= DefaultVisual( display, screen );

    XVisualInfo		vi;
    XVisualInfo *	visuals;

    int			row;
    int			col;
    int			count;
    XColor *		xc= (XColor *)0;

    BitmapDescription	bd;

    unsigned char *	buffer;
    unsigned char *	b;

    double		horPixPerMM;
    double		xfac;

    bitsPerPixel= xim->bits_per_pixel;
    bytesPerLine= xim->bytes_per_line;
    data= xim->data;

    vi.visualid= XVisualIDFromVisual( vis );
    visuals= XGetVisualInfo( display, VisualIDMask, &vi, &count );
    if  ( count != 1 )
	{ LDEB(count); return -1;	}
    vi= *visuals;
    XFree( visuals );

    horPixPerMM= ( (double)DisplayWidth( display, screen ) )/
					DisplayWidthMM( display, screen );

    xfac=  ( 25.4/ ( 20.0* 72.0 ) )* horPixPerMM;

    bmInitDescription( &bd );

    if  ( xim->xoffset )
	{ LDEB( xim->xoffset); return -1;	}

#   ifdef __cplusplus
    switch( vi.c_class )
#   else
    switch( vi.class )
#   endif
	{
	case StaticGray:
	case GrayScale:
#	    ifdef __cplusplus
	    SDEB(APP_VisualClasses[vi.c_class]);
#	    else
	    SDEB(APP_VisualClasses[vi.class]);
#	    endif
	    return -1;
	case StaticColor:
	    count= appGetIndexedColors( &xc, display, cmap, xim->depth );
	    if  ( count < 0 )
		{ LDEB(count); return -1;	}

	    if  ( appGetRGB8Description( &bd, xfac, xim, count ) )
		{ LDEB(count); return -1;	}

	    break;

	case PseudoColor:
	    count= appGetIndexedColors( &xc, display, cmap, xim->depth );
	    if  ( count < 0 )
		{ LDEB(count); return -1;	}

	    if  ( appGetRGB8Description( &bd, xfac, xim, count ) )
		{ LDEB(count); return -1;	}

	    break;

	case TrueColor:
	case DirectColor:
	    if  ( bitsPerPixel <= 16 )
		{
		/*
		count= appGetCalculatedColors( &xc, xim->depth,
			xim->red_mask, xim->green_mask, xim->blue_mask );
		*/
		count= appGetCalculatedColors( &xc, xim->depth,
				vi.red_mask, vi.green_mask, vi.blue_mask );

		if  ( count < 0 )
		    { LDEB(count); return -1;	}

		if  ( appGetRGB8Description( &bd, xfac, xim, count ) )
		    { LDEB(count); return -1;	}
		}
	    else{
		if  ( bitsPerPixel == 32 )
		    {
		    if  ( appGetRGBDescription( &bd, xfac, xim ) )
			{ LDEB(1); return -1;	}

		    break;
		    }

#		ifdef __cplusplus
		SLDEB(APP_VisualClasses[vi.c_class],bitsPerPixel);
#		else
		SLDEB(APP_VisualClasses[vi.class],bitsPerPixel);
#		endif
		break;
		}
	}

    buffer= (unsigned char *)malloc( bd.bdBufferLength );
    if  ( ! buffer )
	{ LXDEB(bd.bdBufferLength,buffer); return -1;	}

    switch( bitsPerPixel )
	{
	unsigned short * psh;

	case 8:
	    for ( row= 0; row < bd.bdPixelsHigh; row++ )
		{
		b= buffer+ row* bd.bdBytesPerRow;

		memcpy( b, data+ row* bytesPerLine, bd.bdBytesPerRow );
		for ( col= 0; col < bd.bdPixelsWide; b++, col++ )
		    { xc[*b].pad= 1; }
		}

	    bd.bdColorCount= appGetRGB8Palette( xc, count, bd.bdRGB8Palette );

	    for ( row= 0; row < bd.bdPixelsHigh; row++ )
		{
		b= buffer+ row* bd.bdBytesPerRow;

		for ( col= 0; col < bd.bdPixelsWide; b++, col++ )
		    { *b= xc[*b].red; }
		}

	    break;

	case 16:
	    if  ( xim->format != ZPixmap )
		{ LDEB(xim->format); return -1;	}

	    for ( row= 0; row < bd.bdPixelsHigh; row++ )
		{
		psh= (unsigned short *)(buffer+ row* bd.bdBytesPerRow);

		memcpy( psh, data+ row* bytesPerLine, bd.bdBytesPerRow );
		for ( col= 0; col < bd.bdPixelsWide; psh++, col++ )
		    { xc[*psh].pad= 1; }
		}

	    bd.bdColorCount= appGetRGB8Palette( xc, count, bd.bdRGB8Palette );

	    if  ( bd.bdColorCount < 256 )
		{
		unsigned char *	pch= buffer;

		for ( row= 0; row < bd.bdPixelsHigh; row++ )
		    {
		    psh= (unsigned short *)(buffer+ row* bd.bdBytesPerRow);

		    for ( col= 0; col < bd.bdPixelsWide;
						    pch++, psh++, col++ )
			{ *pch= xc[*psh].red; }
		    }

		bd.bdBitsPerPixel= 8;
		bd.bdBytesPerRow= bd.bdPixelsWide;
		bd.bdBufferLength= bd.bdPixelsHigh* bd.bdBytesPerRow;
		}
	    else{
		for ( row= 0; row < bd.bdPixelsHigh; row++ )
		    {
		    psh= (unsigned short *)(buffer+ row* bd.bdBytesPerRow);

		    for ( col= 0; col < bd.bdPixelsWide; psh++, col++ )
			{ *psh= xc[*psh].red; }
		    }
		}
	    break;

	case 32:
	    {
	    unsigned int *	from;
	    unsigned char *	to;

	    unsigned int	redShift;
	    unsigned long	redMask;
	    unsigned int	greenShift;
	    unsigned long	greenMask;
	    unsigned int	blueShift;
	    unsigned long	blueMask;

	    if  ( sizeof(unsigned int) != 4 )
		{ LLDEB(sizeof(unsigned int),4); return -1;	}

	    redMask= vi.red_mask; redShift= 0;
	    while( ! ( redMask & 0x01 ) )
		{ redShift++; redMask= redMask >> 1;	}

	    greenMask= vi.green_mask; greenShift= 0;
	    while( ! ( greenMask & 0x01 ) )
		{ greenShift++; greenMask= greenMask >> 1;	}

	    blueMask= vi.blue_mask; blueShift= 0;
	    while( ! ( blueMask & 0x01 ) )
		{ blueShift++; blueMask= blueMask >> 1;	}

	    for ( row= 0; row < bd.bdPixelsHigh; row++ )
		{
		from= (unsigned int *) ( data+ row* bytesPerLine );
		to= buffer+ row* bd.bdBytesPerRow;

		for ( col= 0; col < bd.bdPixelsHigh; from++, col++ )
		    {
		    *(to++)= ( (*from) >> redShift ) & redMask;
		    *(to++)= ( (*from) >> greenShift ) & greenMask;
		    *(to++)= ( (*from) >> blueShift ) & blueMask;
		    }
		}
	    }
	    break;

	case 24:
	default:
	    LDEB(bitsPerPixel); return -1;
	}

    if  ( xc )
	{ free( xc );	}

    abi->abiBuffer= buffer;
    abi->abiBitmap= bd;
    return 0;
    }

int appImgPastePixmap(		AppBitmapImage *	abi,
				APP_WIDGET		w,
				APP_SELECTION_EVENT *	event )
    {
    XSelectionEvent *	selEvent= &(event->xselection);
    int			ret;

    APP_WINDOW		pmap;
    APP_IMAGE *		xim;

    Atom		typeFound;
    int			formatFound;

    unsigned long	itemsReturned;
    unsigned long	itemsLeft;
    unsigned char *	dataReturned;

    Window		wignored;
    int			xignored;
    int			yignored;
    unsigned int	bignored;

    unsigned int	width= 0;
    unsigned int	height= 0;
    unsigned int	depth;

    ret= XGetWindowProperty( selEvent->display,
			    selEvent->requestor, selEvent->property,
			    0, 1, False, AnyPropertyType,
			    &typeFound, &formatFound,
			    &itemsReturned, &itemsLeft, &dataReturned );

    if  ( ret != Success )
	{ LLDEB(ret,Success); return -1;	}
    if  ( itemsReturned == 0 )
	{ LDEB(itemsReturned); return -1;	}
    if  ( formatFound != 32 )
	{ LLDEB(typeFound,formatFound); return -1;	}

    memcpy( &pmap, dataReturned, sizeof(Pixmap) );
    XFree( dataReturned );

    ret= XGetGeometry( selEvent->display, pmap,
			    &wignored, &xignored, &yignored,
			    &width, &height,
			    &bignored,
			    &depth );
    /*
    if  ( ret != Success )
	{ LLDEB(ret,Success); return -1;	}
    */
    xim= XGetImage( selEvent->display, pmap,
				0, 0, width, height, AllPlanes, ZPixmap );

    if  ( ! xim )
	{ XDEB(xim); return -1;	}

    if  ( appGetImage( abi, xim, selEvent->display ) )
	{
	LDEB(depth);

	XDestroyImage( xim );

	return -1;
	}

    XDestroyImage( xim );

    return 0;
    }

#   endif
