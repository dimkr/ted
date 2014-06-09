#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Make images with standard content.					*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Make a completely transparent image					*/
/*									*/
/************************************************************************/

int bmTransparentImage(		BitmapDescription *		bdOut,
				unsigned char **		pBufOut,
				int				colorEncoding,
				int				wide,
				int				high )
    {
    BitmapDescription		bd;
    unsigned char *		buffer;

    switch( colorEncoding )
	{
	case BMcoRGB8PALETTE:
	    bd.bdPixelsWide= wide;
	    bd.bdPixelsHigh= high;
	    bd.bdHasAlpha= 1;
	    bd.bdUnit= BMunINCH;
	    bd.bdXResolution= 72;
	    bd.bdYResolution= 72;

	    bd.bdBitsPerSample= 8;
	    bd.bdSamplesPerPixel= 3;

	    if  ( bd.bdHasAlpha )
		{ bd.bdBitsPerPixel= 8;	}
	    else{ bd.bdBitsPerPixel= 4;	}

	    bd.bdColorEncoding= BMcoRGB8PALETTE;
	    bd.bdColorCount= 3;
	    bd.bdRGB8Palette= (RGB8Color *)0;

	    if  ( bmCalculateSizes( &bd ) )
		{ LDEB(1); return -1;	}

	    buffer= (unsigned char *)malloc( bd.bdBufferLength );
	    if  ( ! buffer )
		{ LXDEB(bd.bdBufferLength,buffer); return -1; }

	    bd.bdRGB8Palette= (RGB8Color *)
				    malloc( 256* sizeof( RGB8Color ) );
	    if  ( ! bd.bdRGB8Palette )
		{
		LLDEB(256,bd.bdRGB8Palette);
		free( buffer ); return -1;
		}

	    /*  transparent */
	    bd.bdRGB8Palette[0].rgb8Red= 255;
	    bd.bdRGB8Palette[0].rgb8Green= 255;
	    bd.bdRGB8Palette[0].rgb8Blue= 255;
	    bd.bdRGB8Palette[0].rgb8Alpha= 0;

	    /*  white */
	    bd.bdRGB8Palette[1].rgb8Red= 255;
	    bd.bdRGB8Palette[1].rgb8Green= 255;
	    bd.bdRGB8Palette[1].rgb8Blue= 255;
	    bd.bdRGB8Palette[1].rgb8Alpha= 255;

	    /*  black */
	    bd.bdRGB8Palette[2].rgb8Red= 0;
	    bd.bdRGB8Palette[2].rgb8Green= 0;
	    bd.bdRGB8Palette[2].rgb8Blue= 0;
	    bd.bdRGB8Palette[2].rgb8Alpha= 255;

	    memset( buffer, 0, bd.bdBufferLength );

	    break;

	default:
	    LDEB(colorEncoding); return -1;
	}

    *bdOut= bd; *pBufOut= buffer;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Make an image containing just one RGB color.			*/
/*									*/
/************************************************************************/

int bmRGBImage(			BitmapDescription *		bdOut,
				unsigned char **		pBufOut,
				int				colorEncoding,
				int				r,
				int				g,
				int				b,
				int				wide,
				int				high )
    {
    BitmapDescription		bd;
    unsigned char *		buffer;

    switch( colorEncoding )
	{
	case BMcoRGB8PALETTE:

	    if  ( r < 0 || r > 255 )
		{ LDEB(r); return -1;	}
	    if  ( g < 0 || g > 255 )
		{ LDEB(g); return -1;	}
	    if  ( b < 0 || b > 255 )
		{ LDEB(b); return -1;	}

	    bd.bdPixelsWide= wide;
	    bd.bdPixelsHigh= high;
	    bd.bdHasAlpha= 0;
	    bd.bdUnit= BMunINCH;
	    bd.bdXResolution= 72;
	    bd.bdYResolution= 72;

	    bd.bdBitsPerSample= 8;
	    bd.bdSamplesPerPixel= 3;

	    if  ( bd.bdHasAlpha )
		{ bd.bdBitsPerPixel= 8;	}
	    else{ bd.bdBitsPerPixel= 4;	}

	    bd.bdColorEncoding= BMcoRGB8PALETTE;
	    bd.bdColorCount= 3;
	    bd.bdRGB8Palette= (RGB8Color *)0;

	    if  ( bmCalculateSizes( &bd ) )
		{ LDEB(1); return -1;	}

	    buffer= (unsigned char *)malloc( bd.bdBufferLength );
	    if  ( ! buffer )
		{ LXDEB(bd.bdBufferLength,buffer); return -1; }

	    bd.bdRGB8Palette= (RGB8Color *)
				    malloc( 256* sizeof( RGB8Color ) );
	    if  ( ! bd.bdRGB8Palette )
		{
		LLDEB(256,bd.bdRGB8Palette);
		free( buffer ); return -1;
		}

	    /*  color */
	    bd.bdRGB8Palette[0].rgb8Red= r;
	    bd.bdRGB8Palette[0].rgb8Green= g;
	    bd.bdRGB8Palette[0].rgb8Blue= b;
	    bd.bdRGB8Palette[0].rgb8Alpha= 255;

	    /*  white */
	    bd.bdRGB8Palette[1].rgb8Red= 255;
	    bd.bdRGB8Palette[1].rgb8Green= 255;
	    bd.bdRGB8Palette[1].rgb8Blue= 255;
	    bd.bdRGB8Palette[1].rgb8Alpha= 255;

	    /*  black */
	    bd.bdRGB8Palette[2].rgb8Red= 0;
	    bd.bdRGB8Palette[2].rgb8Green= 0;
	    bd.bdRGB8Palette[2].rgb8Blue= 0;
	    bd.bdRGB8Palette[2].rgb8Alpha= 255;

	    memset( buffer, 0, bd.bdBufferLength );

	    break;

	default:
	    LDEB(colorEncoding); return -1;
	}

    *bdOut= bd; *pBufOut= buffer;
    return 0;
    }
