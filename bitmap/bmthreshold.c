#   include	"bitmapConfig.h"

#   include	<bmGrayHisto.h>

#   include	"bmintern.h"
#   include	<string.h>
#   include	<appDebugon.h>

#   include	<math.h>

/************************************************************************/
/*									*/
/*  Thresholding implementations.					*/
/*									*/
/************************************************************************/

static int bmToThresholdAllocateColor(	AllocatorColor *	ac,
					ColorAllocator *	ca,
					unsigned int		r,
					unsigned int		g,
					unsigned int		b )
    {
    ThresholderHistogram *	th= (ThresholderHistogram *)ca->caSystemPrivate;
    long			i;

    if  ( r > th->thThreshold )
	{ i= 1;	}
    else{ i= 0;	}

    ac->acRed= 257* r;
    ac->acGreen= 257* g;
    ac->acBlue= 257* b;
    ac->acColorNumber= i;

    return 0;
    }

static void bmThresholdFillHistogram8(
				ThresholderHistogram *		th,
				const BitmapDescription *	bdIn,
				const unsigned char *		bufferIn )
    {
    int		row;
    int		col;

    for ( col= 0; col < 256; col++ )
	{ th->thHistogram[col]= 0;	}

    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
	{
	const unsigned char *	from= bufferIn+ row* bdIn->bdBytesPerRow;

	for ( col= 0; col < bdIn->bdPixelsWide; from++, col++ )
	    {
	    if  ( bdIn->bdColorEncoding == BMcoWHITEBLACK )
		{ th->thHistogram[     *from]++; }
	    else{ th->thHistogram[255- *from]++; }
	    }
	}

    th->thHistogramSize= 256;
    th->thPixelCount= bdIn->bdPixelsHigh* bdIn->bdPixelsWide;
    th->thThreshold= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Thresholding an image.						*/
/*									*/
/*  1)  Initialize data structures.					*/
/*  2)  Only operate on monochrome images.				*/
/*  3)  At this point histogram code only works for 8 bit/pixel images.	*/
/*  4)  Determine threshold value.					*/
/*  5)  Derive properties of output bitmap from input.			*/
/*  6)  Allocate buffer for output image.				*/
/*  7)  Actual thresholding implementaed as a bitmat format		*/
/*	modification.							*/
/*									*/
/************************************************************************/

int bmThreshold(	BitmapDescription *		bdOut,
			const BitmapDescription *	bdIn,
			unsigned char **		pBufOut,
			const unsigned char *		bufferIn,
			int				ignoredInt )
    {
    int				rval= 0;

    BitmapDescription		bd;
    int				bytesPerRow;

    unsigned char *		buffer= (unsigned char *)0;

    ColorAllocator		ca;
    ThresholderHistogram	th;

    int				bitmapUnit= 0;
    int				swapBitmapBytes= 0;
    int				swapBitmapBits= 0;
    const int			dither= 0;

    /*  1  */
    bmInitColorAllocator( &ca );
    ca.caSystemAllocator= bmToThresholdAllocateColor;
    ca.caSystemPrivate= &th;
    ca.caAllocationType= CA_ALLOCATOR;

    /*  2  */
    switch( bdIn->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	    break;

	case BMcoRGB8PALETTE:
	case BMcoRGB:
	default:
	    LDEB(bdIn->bdColorEncoding);
	    rval= -1; goto ready;
	}

    /*  3  */
    switch( bdIn->bdBitsPerSample )
	{
	case 8:
	    bmThresholdFillHistogram8( &th, bdIn, bufferIn );
	    break;

	default:
	    LDEB(bdIn->bdBitsPerSample);
	    rval= -1; goto ready;
	}

    /*  4  */
    if  ( 0 )
	{
	bmThresholdMean( &th );
	bmThresholdQuantile( &th, 0.5 );
	bmThreshold2Peaks( &th );
	bmThresholdRidler( &th );
	bmThresholdVariance( &th );
	bmThresholdMinimumError( &th );
	bmThresholdKapur( &th );
	bmThresholdJohannsen( &th );
	}
    else{
	bmThresholdJohannsen( &th );
	}

    /*  5  */
    bmInitDescription( &bd );
    bmCopyDescription( &bd, bdIn );

    bd.bdBitsPerSample= 1;
    bd.bdSamplesPerPixel= 1;
    bd.bdBitsPerPixel= 1;

    bd.bdColorEncoding= BMcoWHITEBLACK;

    bytesPerRow= ( bd.bdPixelsWide+ 7 )/ 8;
    bd.bdBytesPerRow= bytesPerRow;
    bd.bdBufferLength= bd.bdPixelsHigh* bytesPerRow;

    /*  6  */
    buffer= (unsigned char *)malloc( bd.bdBufferLength );
    if  ( ! buffer )
	{ LLDEB(bd.bdBufferLength,buffer); rval= -1; goto ready; }

    /*  7  */
    if  ( bmFillImage( &ca, bitmapUnit, swapBitmapBytes, swapBitmapBits,
				dither, buffer, bufferIn, &bd, bdIn ) )
	{ LDEB(1); rval= -1; goto ready;	}

    *pBufOut= buffer;
    *bdOut= bd;

  ready:

    if  ( rval && buffer )
	{ free( buffer );	}
    if  ( rval )
	{ bmCleanDescription( &bd );	}

    return rval;
    }
