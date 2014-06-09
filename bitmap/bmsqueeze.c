#   include	"bitmapConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	"bmintern.h"
#   include	<sioHex.h>
#   include	<sioEndian.h>
#   include	<sioStdio.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Reduce the number of bits per pixel either by realizing that the	*/
/*  actual number of colors can be representing in fewer bits, or by	*/
/*  changing from a black/white palette to monochrome.			*/
/*									*/
/*  This will make it possible to save it in monochrome formats such	*/
/*  the TIFF fax formats.						*/
/*									*/
/************************************************************************/

static void bmSqueeze_8_4(	const unsigned char *	fr,
				unsigned char *		to,
				int			wide )
    {
    int			col;
    unsigned char	s[8];

    for ( col= 0; col+ 1 < wide; fr += 2, col += 2, to++ )
	{
	*to=	( ( fr[0] & 0x0f ) << 4 )	|
		( ( fr[1] & 0x0f )      )	;
	}

    if  ( col < wide )
	{
	memset( s, 0, 8 );
	memcpy( s, fr, wide- col ); fr= s;

	*to=	( ( fr[0] & 0x0f ) << 4 )	|
		( ( fr[1] & 0x0f )      )	;
	}
    }

static void bmSqueeze_8_2(	const unsigned char *	fr,
				unsigned char *		to,
				int			wide )
    {
    int			col;
    unsigned char	s[8];

    for ( col= 0; col+ 3 < wide;
				fr += 4, col += 4, to++ )
	{
	*to=	( ( fr[0] & 0x03 ) << 6 )	|
		( ( fr[1] & 0x03 ) << 4 )	|
		( ( fr[2] & 0x03 ) << 2 )	|
		( ( fr[3] & 0x03 )      )	;
	}

    if  ( col < wide )
	{
	memset( s, 0, 8 );
	memcpy( s, fr, wide- col ); fr= s;

	*to=	( ( fr[0] & 0x03 ) << 6 )	|
		( ( fr[1] & 0x03 ) << 4 )	|
		( ( fr[2] & 0x03 ) << 2 )	|
		( ( fr[3] & 0x03 )      )	;
	}
    }

static void bmSqueeze_8_1(	const unsigned char *	fr,
				unsigned char *		to,
				int			wide )
    {
    int			col;
    unsigned char	s[8];

    for ( col= 0; col+ 7 < wide; fr += 8, col += 8, to++ )
	{
	*to=	( ( fr[0] & 0x01 ) << 7 )	|
		( ( fr[1] & 0x01 ) << 6 )	|
		( ( fr[2] & 0x01 ) << 5 )	|
		( ( fr[3] & 0x01 ) << 4 )	|
		( ( fr[4] & 0x01 ) << 3 )	|
		( ( fr[5] & 0x01 ) << 2 )	|
		( ( fr[6] & 0x01 ) << 1 )	|
		( ( fr[7] & 0x01 )      )	;
	}

    if  ( col < wide )
	{
	memset( s, 0, 8 );
	memcpy( s, fr, wide- col ); fr= s;

	*to=	( ( fr[0] & 0x01 ) << 7 )	|
		( ( fr[1] & 0x01 ) << 6 )	|
		( ( fr[2] & 0x01 ) << 5 )	|
		( ( fr[3] & 0x01 ) << 4 )	|
		( ( fr[4] & 0x01 ) << 3 )	|
		( ( fr[5] & 0x01 ) << 2 )	|
		( ( fr[6] & 0x01 ) << 1 )	|
		( ( fr[7] & 0x01 )      )	;
	}
    }

static void bmSqueeze_4_2(	const unsigned char *	fr,
				unsigned char *		to,
				int			wide )
    {
    int			col;
    unsigned char	s[8];

    for ( col= 0; col+ 3 < wide; fr += 2, col += 4, to++ )
	{
	*to=	( ( fr[0] & 0x30 ) << 2 )	|
		( ( fr[0] & 0x03 ) << 6 )	|
		( ( fr[1] & 0x30 ) << 2 )	|
		( ( fr[1] & 0x03 )      )	;
	}

    if  ( col < wide )
	{
	memset( s, 0, 8 );
	memcpy( s, fr, ( wide- col+ 1 )/2 );
	fr= s;

	*to=	( ( fr[0] & 0x30 ) << 2 )	|
		( ( fr[0] & 0x03 ) << 6 )	|
		( ( fr[1] & 0x30 ) << 2 )	|
		( ( fr[1] & 0x03 )      )	;
	}
    }

static void bmSqueeze_4_1(	const unsigned char *	fr,
				unsigned char *		to,
				int			wide )
    {
    int			col;
    unsigned char	s[8];

    for ( col= 0; col+ 7 < wide; fr += 4, col += 8, to++ )
	{
	*to=	( ( fr[0] & 0x10 ) << 3 )	|
		( ( fr[0] & 0x01 ) << 6 )	|
		( ( fr[1] & 0x10 ) << 1 )	|
		( ( fr[1] & 0x01 ) << 4 )	|
		( ( fr[2] & 0x10 ) >> 1 )	|
		( ( fr[2] & 0x01 ) << 2 )	|
		( ( fr[3] & 0x10 ) >> 3 )	|
		( ( fr[3] & 0x01 )      )	;
	}

    if  ( col < wide )
	{
	memset( s, 0, 8 );
	memcpy( s, fr, ( wide- col+ 1 )/2 );
	fr= s;

	*to=	( ( fr[0] & 0x10 ) << 3 )	|
		( ( fr[0] & 0x01 ) << 6 )	|
		( ( fr[1] & 0x10 ) << 1 )	|
		( ( fr[1] & 0x01 ) << 4 )	|
		( ( fr[2] & 0x10 ) >> 1 )	|
		( ( fr[2] & 0x01 ) << 2 )	|
		( ( fr[3] & 0x10 ) >> 3 )	|
		( ( fr[3] & 0x01 )      )	;
	}
    }

static void bmSqueeze_2_1(	const unsigned char *	fr,
				unsigned char *		to,
				int			wide )
    {
    int			col;
    unsigned char	s[8];

    for ( col= 0; col+ 7 < wide; fr += 2, col += 8, to++ )
	{
	*to=	( ( fr[0] & 0x40 ) << 1 )	|
		( ( fr[0] & 0x10 ) << 2 )	|
		( ( fr[0] & 0x04 ) << 3 )	|
		( ( fr[0] & 0x01 ) << 4 )	|
		( ( fr[1] & 0x40 ) << 3 )	|
		( ( fr[1] & 0x10 ) << 2 )	|
		( ( fr[1] & 0x04 ) << 1 )	|
		( ( fr[1] & 0x01 )      )	;
	}

    if  ( col < wide )
	{
	memset( s, 0, 8 );
	memcpy( s, fr, ( wide- col+ 3 )/4 );
	fr= s;

	*to=	( ( fr[0] & 0x40 ) << 1 )	|
		( ( fr[0] & 0x10 ) << 2 )	|
		( ( fr[0] & 0x04 ) << 3 )	|
		( ( fr[0] & 0x01 ) << 4 )	|
		( ( fr[1] & 0x40 ) << 3 )	|
		( ( fr[1] & 0x10 ) << 2 )	|
		( ( fr[1] & 0x04 ) << 1 )	|
		( ( fr[1] & 0x01 )      )	;
	}
    }

static int bmSqueezeBuffer(	const BitmapDescription *	bd,
				int				bitsPerPixel,
				int				bytesPerRow,
				unsigned char *			buffer )
    {
    int				row;

    const unsigned char *	fr;
    unsigned char *		to;

    switch( bd->bdBitsPerPixel )
	{
	case 8:
	    switch( bitsPerPixel )
		{
		case 8:
		    return 0;
		case 4:
		    for ( row= 0; row < bd->bdPixelsHigh; row++ )
			{
			fr= buffer+ row* bd->bdBytesPerRow;
			to= buffer+ row* bytesPerRow;

			bmSqueeze_8_4( fr, to, bd->bdPixelsWide );
			}
		    return 0;
		case 2:
		    for ( row= 0; row < bd->bdPixelsHigh; row++ )
			{
			fr= buffer+ row* bd->bdBytesPerRow;
			to= buffer+ row* bytesPerRow;

			bmSqueeze_8_2( fr, to, bd->bdPixelsWide );
			}
		    return 0;
		case 1:
		    for ( row= 0; row < bd->bdPixelsHigh; row++ )
			{
			fr= buffer+ row* bd->bdBytesPerRow;
			to= buffer+ row* bytesPerRow;

			bmSqueeze_8_1( fr, to, bd->bdPixelsWide );
			}
		    return 0;
		default:
		    LLDEB(bd->bdBitsPerPixel,bitsPerPixel); return -1;
		}
	case 4:
	    switch( bitsPerPixel )
		{
		case 4:
		    return 0;
		case 2:
		    for ( row= 0; row < bd->bdPixelsHigh; row++ )
			{
			fr= buffer+ row* bd->bdBytesPerRow;
			to= buffer+ row* bytesPerRow;

			bmSqueeze_4_2( fr, to, bd->bdPixelsWide );
			}
		    return 0;
		case 1:
		    for ( row= 0; row < bd->bdPixelsHigh; row++ )
			{
			fr= buffer+ row* bd->bdBytesPerRow;
			to= buffer+ row* bytesPerRow;

			bmSqueeze_4_1( fr, to, bd->bdPixelsWide );
			}
		    return 0;
		default:
		    LLDEB(bd->bdBitsPerPixel,bitsPerPixel); return -1;
		}
	case 2:
	    switch( bitsPerPixel )
		{
		case 2:
		    return 0;
		case 1:
		    for ( row= 0; row < bd->bdPixelsHigh; row++ )
			{
			fr= buffer+ row* bd->bdBytesPerRow;
			to= buffer+ row* bytesPerRow;

			bmSqueeze_2_1( fr, to, bd->bdPixelsWide );
			}
		    return 0;
		default:
		    LLDEB(bd->bdBitsPerPixel,bitsPerPixel); return -1;
		}
	case 1:
	    switch( bitsPerPixel )
		{
		case 1:
		    return 0;
		default:
		    LLDEB(bd->bdBitsPerPixel,bitsPerPixel); return -1;
		}
	    break;
	default:
	    LLDEB(bd->bdBitsPerPixel,bitsPerPixel); return -1;
	}
    }

int bmMakeMonochrome(		BitmapDescription *	bd,
				RGB8Color *		palette,
				unsigned char *		buffer )
    {
    int		hasAlpha= bd->bdHasAlpha != 0;
    int		bitsPerPixel;
    int		bytesPerRow;

    if  ( bd->bdColorCount == 2 )
	{
	bitsPerPixel= 1+ hasAlpha;
	bytesPerRow= ( bd->bdPixelsWide* bitsPerPixel+ 7 )/ 8;

	if  ( palette[0].rgb8Red	== 0	&&
	      palette[0].rgb8Green	== 0	&&
	      palette[0].rgb8Blue	== 0	&&
	      palette[1].rgb8Red	== 255	&&
	      palette[1].rgb8Green	== 255	&&
	      palette[1].rgb8Blue	== 255	)
	    {
	    if  ( bmSqueezeBuffer( bd, bitsPerPixel, bytesPerRow, buffer ) )
		{ LDEB(1); return -1;	}

	    free( (char *)palette );
	    bd->bdBitsPerSample= 1;
	    bd->bdColorEncoding= BMcoWHITEBLACK;
	    bd->bdSamplesPerPixel= 1+ hasAlpha;
	    bd->bdBitsPerPixel= bitsPerPixel;
	    bd->bdBytesPerRow= bytesPerRow;
	    bd->bdBufferLength= bd->bdPixelsHigh* bytesPerRow;
	    return 0;
	    }

	if  ( palette[0].rgb8Red	== 255	&&
	      palette[0].rgb8Green	== 255	&&
	      palette[0].rgb8Blue	== 255	&&
	      palette[1].rgb8Red	== 0	&&
	      palette[1].rgb8Green	== 0	&&
	      palette[1].rgb8Blue	== 0	)
	    {
	    if  ( bmSqueezeBuffer( bd, bitsPerPixel, bytesPerRow, buffer ) )
		{ LDEB(1); return -1;	}

	    free( (char *)palette );
	    bd->bdBitsPerSample= 1;
	    bd->bdColorEncoding= BMcoBLACKWHITE;
	    bd->bdSamplesPerPixel= 1+ hasAlpha;
	    bd->bdBitsPerPixel= bitsPerPixel;
	    bd->bdBytesPerRow= bytesPerRow;
	    bd->bdBufferLength= bd->bdPixelsHigh* bytesPerRow;
	    return 0;
	    }

	return -1;
	}

    return -1;
    }

