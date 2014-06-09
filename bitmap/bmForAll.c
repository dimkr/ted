#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	<string.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Call a function for all pixels that have value '1' in a bitmap	*/
/*  image. [Or '0'.. when invertMaskIn == 0xff.].			*/
/*									*/
/*  Used inside morphology code and to extract features from the	*/
/*  result.								*/
/*									*/
/*  The behavior with ( invertMaskIn != 0xff && invertMaskIn != 0x00 )	*/
/*  is completely predictable, buf not what this routine was meant for.	*/
/*									*/
/*  1)  Determine the mask to ignore the trailing bits in the input	*/
/*	image.								*/
/*									*/
/************************************************************************/

void bmForAll1Pixels(		const BitmapDescription *	bdIn,
				const unsigned char *		bufIn,
				unsigned char			invertMaskIn,
				void *				through,
				BM_PIX_FUN			pixFun )
    {
    int			rowIn;
    unsigned char	lastMaskIn;

    int			col;

    if  ( bdIn->bdBitsPerPixel != 1 )
	{ LLDEB(bdIn->bdBitsPerPixel,bdIn->bdBitsPerPixel!=1);	}

    /*  1  */
    lastMaskIn= 0xff;
    lastMaskIn >>= ( bdIn->bdPixelsWide % 8 );
    lastMaskIn <<= ( bdIn->bdPixelsWide % 8 );

    for ( rowIn= 0; rowIn < bdIn->bdPixelsHigh; rowIn++ )
	{
	const unsigned char *	rowBufIn= bufIn+ rowIn* bdIn->bdBytesPerRow;

	for ( col= 0; col < bdIn->bdPixelsWide; rowBufIn++, col += 8 )
	    {
	    unsigned char	val= ( rowBufIn[0] ^ invertMaskIn );

	    unsigned char	mask= 0x80;
	    int		pix;

	    if  ( col+ 8 >= bdIn->bdPixelsWide )
		{ val &= lastMaskIn;	}
	    if  ( ! val )
		{ continue;	}

	    for ( pix= 0; pix < 8; mask >>= 1, pix++ )
		{
		int		colIn= col+ pix;

		if  ( ! ( val & mask ) )
		    { continue;	}

		(*pixFun)( through, rowIn, colIn );
		}
	    }
	}

    return;
    }

