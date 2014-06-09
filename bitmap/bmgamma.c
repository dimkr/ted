#   include	"bitmapConfig.h"

#   include	"bmintern.h"

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Simple gamma correction of images.					*/
/*									*/
/*  1)  Check input.							*/
/*  2)  Determine the maximum value of an individual component.		*/
/*  3)  Fill a gamma correction array for correcting the individual	*/
/*	samples. The order of the values in the array makes it possible	*/
/*	to map the values in the bitmap without knowing the exact color	*/
/*	encoding scheme in the image.					*/
/*  4)  Make an exact copy of the input image.				*/
/*  5)  And map the component values inside the image to the mapped	*/
/*	values.								*/
/*  6)  Copy the result to the output destination.			*/
/*									*/
/************************************************************************/

#   define	MAX_COMPONENTS	4

int bmGammaCorrect(	BitmapDescription *		bdOut,
			const BitmapDescription *	bdIn,
			unsigned char **		pBufOut,
			const unsigned char *		bufIn,
			double				gammaValue )
    {
    int			map[256*MAX_COMPONENTS];
    double		gammas[MAX_COMPONENTS];
    int *		mp;
    int			mxv;

    int			i;
    int			j;

    unsigned char *	bufOut;

    BitmapDescription	bd;

    bmInitDescription( &bd );

    /*  1  */
    if  ( gammaValue <= 0.0001 )
	{ FDEB(gammaValue); return -1;	}
    if  ( bdIn->bdBitsPerSample > 8 )
	{ LDEB(bdIn->bdBitsPerSample); return -1;	}

    for ( j= 0; j < bdIn->bdSamplesPerPixel; j++ )
	{ gammas[j]= gammaValue;	}

    /*  2  */
    mxv= 1;
    for ( i= 0; i < bdIn->bdBitsPerSample; i++ )
	{ mxv *= 2;	}
    mxv--;

    /*  3  */
    mp= map;
    for ( i= 0; i <= mxv; i++ )
	{
	for ( j= 0; j < bdIn->bdSamplesPerPixel; j++ )
	    { *(mp++)= mxv* pow( (double)i/(double)mxv, gammas[j] );	}
	}

    /*  4  */
    bufOut= malloc( bdIn->bdBufferLength+ bdIn->bdSamplesPerPixel- 1 );
    if  ( ! bufOut )
	{ LXDEB(bdIn->bdBufferLength,bufOut); return -1; }

    if  ( bmCopyDescription( &bd, bdIn ) )
	{ LDEB(1); free( bufOut ); return -1;	}

    memcpy( bufOut, bufIn, bdIn->bdBufferLength );

    /*  5  */
    if  ( bmMapImageColors( &bd, map, bufOut ) )
	{
	LDEB(1);

	bmCleanDescription( &bd );
	free( bufOut );

	return -1;
	}

    /*  6  */
    if  ( bmCopyDescription( bdOut, &bd ) )
	{
	LDEB(1);

	bmCleanDescription( &bd );
	free( bufOut );

	return -1;

	}

    *pBufOut= bufOut; return 0;
    }

