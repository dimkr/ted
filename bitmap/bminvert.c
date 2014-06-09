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
/*  Invert the individual components of an image.			*/
/*									*/
/************************************************************************/

#   define	MAX_COMPONENTS	4

int bmInvertImage(	BitmapDescription *		bdOut,
			const BitmapDescription *	bdIn,
			unsigned char **		pBufOut,
			const unsigned char *		bufIn,
			int				ignoredInt )
    {
    int			map[256*MAX_COMPONENTS];
    int *		mp;
    int			mxv;

    int			i;
    int			j;

    unsigned char *	bufOut;

    BitmapDescription	bd;

    bmInitDescription( &bd );

    /*  1  */
    if  ( bdIn->bdBitsPerSample > 8 )
	{ LDEB(bdIn->bdBitsPerSample); return -1;	}

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
	    { *(mp++)= mxv- i;	}
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

