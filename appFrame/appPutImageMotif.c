/************************************************************************/
/*									*/
/*  Make XImages and/or Pixmaps from a bitmap.				*/
/*									*/
/*  MOTIF/X11 specific code.						*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	"appImage.h"

#   include	<appDebugon.h>

#   ifdef USE_MOTIF

extern void _XInitImageFuncPtrs( XImage * xim );

/************************************************************************/
/*									*/
/*  Make a pixmap from a picture.					*/
/*									*/
/*  2)  Make sure that at least the 222 colors are available.		*/
/*  2a) Allocate an array of XColors and initialise it.			*/
/*  3)  Fill dither tables.						*/
/*  4)  Fill an XImage, the way depends on the bitmap.			*/
/*  5)  Store it in a Pixmap.						*/
/*									*/
/************************************************************************/

int appImgMakeImage(	AppDrawingData *		add,
			APP_IMAGE **			pPimage,
			int				toWide,
			int				toHigh,
			AppColors *			ac,
			const AppBitmapImage *		abi )
    {
    Display *			display= add->addDisplay;
    int				screen= add->addScreen;
    int				depth= DefaultDepth( display, screen );

    const BitmapDescription *	bdIn= &(abi->abiBitmap);
    const unsigned char *	bufferIn= abi->abiBuffer;
    BitmapDescription		bdOut;

    APP_IMAGE *			xim;

    Visual *			vis;

    int				pad= 8;
    int				col;

    unsigned int		one= 1;

    int				bitmapUnit= 0;
    int				swapBitmapBytes= 0;
    int				swapBitmapBits= 0;
    const int			dither= 1;

    vis= DefaultVisual( display, screen );

    bmInitDescription ( &bdOut );

    bdOut.bdPixelsWide= toWide;
    bdOut.bdPixelsHigh= toHigh;
    bdOut.bdHasAlpha= 0;
    bdOut.bdXResolution= 1;
    bdOut.bdYResolution= 1;
    bdOut.bdUnit= BMunPIXEL;

    if  ( ac->acVisualClass ==	TrueColor	||
	  ac->acVisualClass ==	DirectColor	)
	{
	bdOut.bdBytesPerRow= toWide;
	if  ( depth > 8 )
	    { bdOut.bdBytesPerRow *= 2;	}
	if  ( depth > 16 )
	    { bdOut.bdBytesPerRow *= 2;	}

	xim= XCreateImage( display, vis, depth, ZPixmap, 0, (char *)0,
						    toWide, toHigh, pad, 0 );

	if  ( ! xim )
	    { LDEB(xim); return -1;	}

	bdOut.bdBytesPerRow= xim->bytes_per_line;
	bdOut.bdBufferLength= toHigh* bdOut.bdBytesPerRow;
	bdOut.bdBitsPerSample= depth/ 3;
	bdOut.bdSamplesPerPixel= 3;
	bdOut.bdBitsPerPixel= xim->bits_per_pixel;
	bdOut.bdColorEncoding= BMcoRGB;
	bdOut.bdColorCount= 0;
	}
    else{
	switch( depth )
	    {
	    case 1:
		xim= XCreateImage( display, vis, depth, XYPixmap, 0,
					(char *)0, toWide, toHigh, pad, 0 );

		if  ( ! xim )
		    { LDEB(xim); return -1;	}

		if  ( xim->bitmap_unit > 32 )
		    { LDEB(xim->bitmap_unit); return -1;	}

		bitmapUnit= xim->bitmap_unit;
		if  ( xim->byte_order == MSBFirst )
		    {
		    if  ( *((unsigned char *)&one)	)
			{ swapBitmapBytes= 1;	}
		    else{ swapBitmapBytes= 0;	}
		    }
		else{
		    if  ( *((unsigned char *)&one)	)
			{ swapBitmapBytes= 0;	}
		    else{ swapBitmapBytes= 1;	}
		    }

		/*  ?  */
		if  ( xim->bitmap_bit_order == MSBFirst )
		    { swapBitmapBits= 0;	}
		else{ swapBitmapBits= 1;	}

		bdOut.bdBytesPerRow= xim->bytes_per_line;
		bdOut.bdBufferLength= toHigh* bdOut.bdBytesPerRow;
		bdOut.bdBitsPerSample= 1;
		bdOut.bdSamplesPerPixel= 1;
		bdOut.bdBitsPerPixel= 1;
		bdOut.bdColorEncoding= BMcoBLACKWHITE;
		bdOut.bdColorCount= 0;

		break;

	    case 8:
		xim= XCreateImage( display, vis, depth, ZPixmap, 0,
						    (char *)0,
						    toWide, toHigh, pad, 0 );

		if  ( ! xim )
		    { LDEB(xim); return -1;	}

		bdOut.bdBytesPerRow= xim->bytes_per_line;
		bdOut.bdBufferLength= toHigh* bdOut.bdBytesPerRow;
		bdOut.bdBitsPerSample= 8;
		bdOut.bdSamplesPerPixel= 3;
		bdOut.bdBitsPerPixel= 8;
		bdOut.bdColorEncoding= BMcoRGB8PALETTE;
		bdOut.bdColorCount= 0;

		break;
	    case 16:
		bdOut.bdBytesPerRow= 4* ( ( 2* toWide+ 3 )/ 4 );
		bdOut.bdBufferLength= toHigh* bdOut.bdBytesPerRow;

		xim= XCreateImage( display, vis, depth, ZPixmap, 0,
						    (char *)0,
						    toWide, toHigh, pad, 0 );

		if  ( ! xim )
		    { LDEB(xim); return -1;	}

		bdOut.bdBytesPerRow= xim->bytes_per_line;
		bdOut.bdBufferLength= toHigh* bdOut.bdBytesPerRow;
		bdOut.bdBitsPerSample= 8;
		bdOut.bdSamplesPerPixel= 3;
		bdOut.bdBitsPerPixel= 16;
		bdOut.bdColorEncoding= BMcoRGB8PALETTE;
		bdOut.bdColorCount= 0;

		break;

	    case 32:
	    case 24:
	    default:
		LDEB(depth); return -1;
	    }
	}

    /*  2  */
    for ( col= 0; col < 64; col++ )
	{
	int		r, g, b;
	XColor		xc;

	r= ( col & 0x30 ) << 2;
	g= ( col & 0x0c ) << 4;
	b= ( col & 0x03 ) << 6;

	if  ( appColorRgb( &xc, ac, r, g, b ) )
	    { LDEB(col); return -1; }
	}

    if  ( *((unsigned char *)&one) )
	{
	if  ( xim->byte_order != LSBFirst )
	    {
	    xim->byte_order= LSBFirst;
	    _XInitImageFuncPtrs( xim );
	    }
	}
    else{
	if  ( xim->byte_order != MSBFirst )
	    {
	    xim->byte_order= MSBFirst;
	    _XInitImageFuncPtrs( xim );
	    }
	}

    xim->data= malloc( bdOut.bdBufferLength );
    if  ( ! xim->data )
	{
	LXDEB(bdOut.bdBufferLength,xim->data);
	XDestroyImage( xim ); return -1;
	}

    if  ( bmFillImage( &(ac->acAllocator),
			bitmapUnit, swapBitmapBytes, swapBitmapBits,
			dither,
			(unsigned char *)xim->data, bufferIn, &bdOut, bdIn ) )
	{
	LDEB(1);

#	ifdef USE_MECHECK
	if  ( xim->data )
	    { free( xim->data ); xim->data= (char *)0; }
#	endif

	XDestroyImage( xim );
	return -1;
	}

    *pPimage= xim; return 0;
    }

/************************************************************************/
/*									*/
/*  Convert a BitmapDescription to an APP_IMAGE.			*/
/*									*/
/************************************************************************/

int appImgMakePixmap(	AppDrawingData *		add,
			Pixmap *			pPixmap,
			int				toWide,
			int				toHigh,
			AppColors *			ac,
			const AppBitmapImage *		abi )
    {
    APP_IMAGE *		xim= (APP_IMAGE *)0;

    Display *		display= add->addDisplay;
    Window		win= add->addDrawable;
    int			screen= DefaultScreen( display );
    int			depth= DefaultDepth( display, screen );

    if  ( appImgMakeImage( add, &xim, toWide, toHigh, ac, abi ) )
	{ LLDEB(toWide,toHigh); return -1;	}

    /*  5  */
    *pPixmap= XCreatePixmap( display, win, toWide, toHigh, depth );

    if  ( ! *pPixmap )
	{ XDEB(*pPixmap); return -1;	}

    XPutImage( display, *pPixmap, add->addGc, xim, 0, 0, 0, 0, toWide, toHigh );

#   ifdef USE_MECHECK
    if  ( xim->data )
	{ free( xim->data ); xim->data= (char *)0; }
#   endif

    XDestroyImage( xim );

    return 0;
    }

#   endif /* USE_MOTIF */
