/************************************************************************/
/*									*/
/*  Make XImages and/or Pixmaps from a bitmap.				*/
/*									*/
/*  GTK specific code.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	"appImage.h"

#   include	<appDebugon.h>

#   ifdef USE_GTK

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
    GdkVisual *			vis= gdk_visual_get_system();
    int				depth= vis->depth;

    const BitmapDescription *	bdIn= &(abi->abiBitmap);
    const unsigned char *	bufferIn= abi->abiBuffer;
    unsigned char *		bufferOut;
    BitmapDescription		bdOut;

    APP_IMAGE *			xim;

    int				col;


    int				bitmapUnit= 0;
    int				swapBitmapBytes= 0;
    int				swapBitmapBits= 0;
    const int			dither= 1;

    bmInitDescription ( &bdOut );

    bdOut.bdPixelsWide= toWide;
    bdOut.bdPixelsHigh= toHigh;
    bdOut.bdHasAlpha= 0;
    bdOut.bdXResolution= 1;
    bdOut.bdYResolution= 1;
    bdOut.bdUnit= BMunPIXEL;

    if  ( ac->acVisualClass == GDK_VISUAL_TRUE_COLOR	||
	  ac->acVisualClass == GDK_VISUAL_DIRECT_COLOR	)
	{
	xim= gdk_image_new( GDK_IMAGE_FASTEST, vis, toWide, toHigh );
	if  ( ! xim )
	    { LDEB(xim); return -1;	}
	bufferOut= xim->mem;

	bdOut.bdBytesPerRow= xim->bpl;
	bdOut.bdBufferLength= toHigh* bdOut.bdBytesPerRow;
	
	bdOut.bdBitsPerSample= depth/ 3;
	bdOut.bdSamplesPerPixel= 3;
	bdOut.bdBitsPerPixel= 8* xim->bpp;
	bdOut.bdColorEncoding= BMcoRGB;
	bdOut.bdColorCount= 0;
	}
    else{
	switch( depth )
	    {
#	    if  GTK_MAJOR_VERSION < 2
	    case 1:
		{
		int		pad= 8;
		unsigned int	one= 1;

		pad= 8;
		bdOut.bdBytesPerRow= ( toWide+ pad- 1 )/ pad;
		bdOut.bdBytesPerRow *= (pad/8);

		bdOut.bdBufferLength= toHigh* bdOut.bdBytesPerRow+ toWide+ 32;

		bufferOut= (unsigned char *)malloc( bdOut.bdBufferLength );
		if  ( ! bufferOut )
		    { LLDEB(toWide,toHigh); return -1;	}

		xim= gdk_image_new_bitmap( vis, (gpointer *)bufferOut,
							    toWide, toHigh );

		if  ( ! xim )
		    { LDEB(xim); free( bufferOut ); return -1;	}

		if  ( *((unsigned char *)&one)	)
		    { swapBitmapBytes= 1;	}
		else{ swapBitmapBytes= 0;	}

		bitmapUnit= 8;

		bdOut.bdBitsPerSample= 1;
		bdOut.bdSamplesPerPixel= 1;
		bdOut.bdBitsPerPixel= 1;
		bdOut.bdColorEncoding= BMcoBLACKWHITE;
		bdOut.bdColorCount= 0;

		}

		break;
#	    endif

	    case 8:
	    case 16:
		xim= gdk_image_new( GDK_IMAGE_FASTEST, vis, toWide, toHigh );
		if  ( ! xim )
		    { LDEB(xim); return -1;	}
		bufferOut= xim->mem;

		bdOut.bdBytesPerRow= xim->bpl;
		bdOut.bdBufferLength= toHigh* bdOut.bdBytesPerRow;
		
		bdOut.bdBitsPerSample= 8;
		bdOut.bdSamplesPerPixel= 3;
		bdOut.bdBitsPerPixel= depth;
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
	int			r, g, b;
	APP_COLOR_RGB		xc;

	r= ( col & 0x30 ) << 2;
	g= ( col & 0x0c ) << 4;
	b= ( col & 0x03 ) << 6;

	if  ( appColorRgb( &xc, ac, r, g, b ) )
	    { LDEB(col); return -1; }
	}

    if  ( bdOut.bdBytesPerRow != xim->bpl )
	{ LLDEB(bdOut.bdBytesPerRow,xim->bpl);	}

    if  ( bmFillImage( &(ac->acAllocator),
				bitmapUnit, swapBitmapBytes, swapBitmapBits,
				dither, bufferOut, bufferIn, &bdOut, bdIn ) )
	{ LDEB(1); gdk_image_destroy( xim ); return -1; }

    *pPimage= xim; return 0;
    }

/************************************************************************/
/*									*/
/*  Convert a BitmapDescription to an APP_IMAGE.			*/
/*									*/
/************************************************************************/

int appImgMakePixmap(	AppDrawingData *		add,
			APP_BITMAP_IMAGE *		pPixmap,
			int				toWide,
			int				toHigh,
			AppColors *			ac,
			const AppBitmapImage *		abi )
    {
    APP_IMAGE *		xim;

    GdkWindow *		win= add->addDrawable;
    int			depth= ac->acAllocator.caDepth;

    if  ( appImgMakeImage( add, &xim, toWide, toHigh, ac, abi ) )
	{ LLDEB(toWide,toHigh); return -1;	}

    /*  5  */
    *pPixmap= gdk_pixmap_new( win, toWide, toHigh, depth );

    if  ( ! *pPixmap )
	{ XDEB(*pPixmap); return -1;	}

    gdk_draw_image( *pPixmap, add->addGc, xim, 0, 0, 0, 0, toWide, toHigh );

    gdk_image_destroy( xim );

    return 0;
    }

#   endif /* USE_GTK */
