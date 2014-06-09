#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Replace 'White' in a bitmap with 'Transparent'			*/
/*									*/
/************************************************************************/

static int bmSetAlphaBytes(	unsigned char *			b1,
				unsigned char *			b2,
				int				bitsPerPixel,
				unsigned int			t )
    {
    unsigned int	i;
    unsigned int	m;

    switch( bitsPerPixel )
	{
	case 1:
	    m= 0x01;

	    for ( i= 0; i < 256; i++ )
		{
		b1[i]= b2[i]= 0;

		b1[i] |=	( i & 0x80 )     	|
				( i & 0x40 ) >> 1	|
				( i & 0x20 ) >> 2	|
				( i & 0x10 ) >> 3	;

		b1[i] |=	( m* ( ( ( i & 0x80 ) >> 7 ) != t ) ) << 6 |
				( m* ( ( ( i & 0x40 ) >> 6 ) != t ) ) << 4 |
				( m* ( ( ( i & 0x20 ) >> 5 ) != t ) ) << 2 |
				( m* ( ( ( i & 0x10 ) >> 4 ) != t ) ) << 0 ;

		b2[i] |=	( i & 0x08 ) << 4	|
				( i & 0x04 ) << 3	|
				( i & 0x02 ) << 2	|
				( i & 0x01 ) << 1	;

		b2[i] |=	( m* ( ( ( i & 0x08 ) >> 3 ) != t ) ) << 6 |
				( m* ( ( ( i & 0x04 ) >> 2 ) != t ) ) << 4 |
				( m* ( ( ( i & 0x02 ) >> 1 ) != t ) ) << 2 |
				( m* ( ( ( i & 0x01 ) >> 0 ) != t ) ) << 0 ;
		}
	    break;

	case 2:
	    m= 0x03;

	    for ( i= 0; i < 256; i++ )
		{
		b1[i]= b2[i]= 0;

		b1[i] |=	( i & 0xc0 )     	|
				( i & 0x30 ) >> 2	;

		b1[i] |=	( m* ( ( ( i & 0xc0 ) >> 6 ) != t ) ) << 4 |
				( m* ( ( ( i & 0x60 ) >> 4 ) != t ) ) << 0 ;

		b2[i] |=	( i & 0x0c ) << 4	|
				( i & 0x03 ) << 2	;

		b2[i] |=	( m* ( ( ( i & 0x0c ) >> 2 ) != t ) ) << 4 |
				( m* ( ( ( i & 0x03 ) >> 0 ) != t ) ) << 0 ;
		}
	    break;

	case 4:
	    m= 0x0f;

	    for ( i= 0; i < 256; i++ )
		{
		b1[i]= b2[i]= 0;

		b1[i] |=	( i & 0xf0 )     	;

		b1[i] |=	( m* ( ( ( i & 0xf0 ) >> 4 ) != t ) ) << 0 ;

		b2[i] |=	( i & 0x0f ) << 4	;

		b2[i] |=	( m* ( ( ( i & 0x0f ) >> 0 ) != t ) ) << 0 ;
		}
	    break;

	case 8:
	    for ( i= 0; i < 256; i++ )
		{
		b1[i]=	i;
		b2[i]=	( i == t )? 0: 255;
		}
	    break;

	default:
	    LDEB(bitsPerPixel); return -1;
	    break;
	}

    return 0;
    }

int bmWhiteToTransparent(	BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				int				ignoredInt )
    {
    BitmapDescription		bd= *bdIn;
    unsigned char *		bufOut;
    int				row, col;
    const unsigned char *	from;
    unsigned char *		to;

    unsigned char		b1[256];
    unsigned char		b2[256];

    unsigned int		white= -1;

    bmInitDescription( &bd );
    if  ( bmCopyDescription( &bd, bdIn ) )
	{ LDEB(1); return -1;	}

    switch( bdIn->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	    if  ( bdIn->bdSamplesPerPixel != 1 )
		{ LDEB(bdIn->bdSamplesPerPixel); return -1;	}
	    bd.bdSamplesPerPixel= 2;
	    bd.bdHasAlpha= 1;

	    white= 0;
	    if  ( bmSetAlphaBytes( b1, b2, bdIn->bdBitsPerSample, white ) )
		{ LDEB(bdIn->bdBitsPerSample); return -1;	}

	    bd.bdBitsPerPixel= bd.bdSamplesPerPixel* bd.bdBitsPerSample;
	    break;

	case BMcoWHITEBLACK:
	    if  ( bdIn->bdSamplesPerPixel != 1 )
		{ LDEB(bdIn->bdSamplesPerPixel); return -1;	}
	    bd.bdSamplesPerPixel= 2;
	    bd.bdHasAlpha= 1;

	    white= ( 1 << bdIn->bdBitsPerPixel )- 1;
	    if  ( bmSetAlphaBytes( b1, b2, bdIn->bdBitsPerSample, white ) )
		{ LDEB(bdIn->bdBitsPerSample); return -1;	}

	    bd.bdBitsPerPixel= bd.bdSamplesPerPixel* bd.bdBitsPerSample;
	    break;

	case BMcoRGB8PALETTE:
	    if  ( bdIn->bdSamplesPerPixel != 3 )
		{ LDEB(bdIn->bdSamplesPerPixel); return -1;	}
	    bd.bdSamplesPerPixel= 4;
	    bd.bdHasAlpha= 1;

	    white= bd.bdColorCount;
	    for ( row= 0; row < bd.bdColorCount; row++ )
		{
		if  ( bd.bdRGB8Palette[row].rgb8Red == 255	&&
		      bd.bdRGB8Palette[row].rgb8Green == 255	&&
		      bd.bdRGB8Palette[row].rgb8Blue == 255	)
		    {
		    bd.bdRGB8Palette[row].rgb8Alpha= 0;
		    break;
		    }
		}

	    white= row;
	    if  ( bmSetAlphaBytes( b1, b2, bdIn->bdBitsPerSample, white ) )
		{ LDEB(bdIn->bdBitsPerSample); return -1;	}

	    bd.bdBitsPerPixel= 2* bdIn->bdBitsPerPixel;
	    break;

	case BMcoRGB:
	    if  ( bdIn->bdSamplesPerPixel != 3 )
		{ LDEB(bdIn->bdSamplesPerPixel); return -1;	}
	    bd.bdSamplesPerPixel= 4;
	    bd.bdHasAlpha= 1;
	    break;

	default:
	    LDEB(bdIn->bdColorEncoding); return -1;
	}

    if  ( bmCalculateSizes( &bd ) )
	{ LDEB(1); return -1;	}
    bufOut= malloc( bd.bdBufferLength );
    if  ( ! bufOut )
	{ LLDEB(bd.bdBufferLength,bufOut); return -1;	}

    switch( bdIn->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	case BMcoRGB8PALETTE:
	    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
		{
		from= bufIn + row* bdIn->bdBytesPerRow;
		to  = bufOut+ row* bd.   bdBytesPerRow;

		for ( col= 0; col < bdIn->bdBytesPerRow; col++ )
		    {
		    *(to++)= b1[*from];
		    *(to++)= b2[*from];
		    from++;
		    }
		}
	    break;

	case BMcoRGB:
	    LDEB(bdIn->bdColorEncoding); return -1;
	    break;

	default:
	    LDEB(bdIn->bdColorEncoding); return -1;
	}

    *bdOut= bd; *pBufOut= bufOut;

    return 0;
    }

