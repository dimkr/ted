#   include	"bitmapConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	"bitmap.h"

#   include	<appDebugon.h>

int bmSelect(	BitmapDescription *		bdOut,
		unsigned char **		pBuffer,
		const BitmapDescription *	bdIn,
		const unsigned char *		bufIn,
		int				x0,
		int				y0,
		int				wide,
		int				high )
    {
    BitmapDescription	bd;
    int			bytesPerRow;
    int			originBit;
    int			originByte;
    unsigned char *	buffer;
    int			row;

    if  ( x0+ wide > bdIn->bdPixelsWide	||
	  y0+ high > bdIn->bdPixelsHigh )
	{
	LLLLDEB(x0,y0,wide,high);
	LLDEB(bdIn->bdPixelsWide,bdIn->bdPixelsHigh);
	return -1;
	}

    /************************************************************/
    /*  Derive properties of output bitmap from input.		*/
    /************************************************************/
    bmCopyDescription( &bd, bdIn );
    bd.bdPixelsHigh= high;
    bd.bdPixelsWide= wide;
    bytesPerRow= ( bd.bdBitsPerPixel* bd.bdPixelsWide+ 7 )/ 8;
    bd.bdBytesPerRow= bytesPerRow;
    bd.bdBufferLength= high* bytesPerRow;

    /************************************************************/
    /*  How many bits to shift?					*/
    /************************************************************/
    originBit=  ( x0* bdIn->bdBitsPerPixel )% 8;
    originByte= ( x0* bdIn->bdBitsPerPixel )/ 8;

    /************************************************************/
    /*  Allocate new buffer.					*/
    /************************************************************/
    buffer= (unsigned char *)malloc( bd.bdBufferLength );
    if  ( ! buffer )
	{ LLDEB(bd.bdBufferLength,buffer); return -1;	}

    if  ( originBit == 0 )
	{
	for ( row= 0; row < high; row++ )
	    {
	    memcpy( buffer+ row* bytesPerRow, bufIn+
		    ( row+ y0 )* bdIn->bdBytesPerRow+ originByte, bytesPerRow );
	    }
	}
    else{
	int	rightShift= 8- originBit;

	for ( row= 0; row < high; row++ )
	    {
	    int				byte;
	    unsigned char *		to;
	    const unsigned char *	from;

	    to= buffer+ row* bytesPerRow;
	    from= bufIn+ ( row+ y0 )* bdIn->bdBytesPerRow+ originByte;

	    *to=   *from << originBit; from++;

	    for ( byte= 1; byte < bytesPerRow; byte++ )
		{
		*to |= *from >> rightShift; to++;
		*to=   *from << originBit; from++;
		}

	    *to |= *from >> rightShift;
	    }
	}

    *pBuffer= buffer;
    *bdOut= bd;

    return 0;
    }

static void bmAverageMono8Row(	const BitmapDescription *	bdOut,
				const BitmapDescription *	bdIn,
				unsigned char *			to,
				const unsigned char *		rf,
				int				xpixels,
				int				ypixels )
    {
    int		col;

    int		r;
    int		c;

    for ( col= 0; col < bdIn->bdPixelsWide/ xpixels; rf += xpixels, col++ )
	{
	int				v= 0;
	const int			n= xpixels* ypixels;
	const unsigned char *		cf= rf;

	for ( r= 0; r < ypixels; cf += bdIn->bdBytesPerRow, r++ )
	    {
	    const unsigned char *	f= cf;

	    for ( c= 0; c < xpixels; c++ )
		{ v += *(f++);	}
	    }

	*(to++)= ( v+ n/2 )/ n;
	}

    xpixels= bdIn->bdPixelsWide % xpixels;

    if  ( xpixels > 0 )
	{
	int				v= 0;
	const int			n= ypixels* xpixels;
	const unsigned char *		cf= rf;

	for ( r= 0; r < ypixels; cf += bdIn->bdBytesPerRow, r++ )
	    {
	    const unsigned char *	f= cf;

	    for ( c= 0; c < xpixels; c++ )
		{ v += *(f++);	}
	    }

	*(to++)= ( v+ n/2 )/ n;
	}

    return;
    }

static void bmAverageMono8Image(	const BitmapDescription *	bdOut,
					const BitmapDescription *	bdIn,
					unsigned char *			buffer,
					const unsigned char *		bufIn,
					int				pixels )
    {
    int			row;
    unsigned char *	to= buffer;
    int			ypixels;

    for ( row= 0; row < bdIn->bdPixelsHigh/ pixels; row++ )
	{
	const unsigned char *	rf= bufIn+ pixels* row* bdIn->bdBytesPerRow;

	bmAverageMono8Row( bdOut, bdIn, to, rf, pixels, pixels );

	to += bdOut->bdBytesPerRow;
	}

    ypixels= bdIn->bdPixelsHigh % pixels;

    if  ( ypixels > 0 )
	{
	const unsigned char *	rf= bufIn+ pixels* row* bdIn->bdBytesPerRow;

	bmAverageMono8Row( bdOut, bdIn, to, rf, pixels, ypixels );
	}

    return;
    }

static void bmAverageRgb24Row(	const BitmapDescription *	bdOut,
				const BitmapDescription *	bdIn,
				unsigned char *			to,
				const unsigned char *		rf,
				int				xpixels,
				int				ypixels )
    {
    int		col;

    int		r;
    int		c;

    for ( col= 0; col < bdIn->bdPixelsWide/ xpixels; rf += 3* xpixels, col++ )
	{
	int				rv= 0;
	int				gv= 0;
	int				bv= 0;
	const int			n= xpixels* ypixels;
	const unsigned char *		cf= rf;

	for ( r= 0; r < ypixels; cf += bdIn->bdBytesPerRow, r++ )
	    {
	    const unsigned char *	f= cf;

	    for ( c= 0; c < xpixels; c++ )
		{ rv += *(f++); gv += *(f++); bv += *(f++);	}
	    }

	*(to++)= ( rv+ n/2 )/ n;
	*(to++)= ( gv+ n/2 )/ n;
	*(to++)= ( bv+ n/2 )/ n;
	}

    xpixels= bdIn->bdPixelsWide % xpixels;

    if  ( xpixels > 0 )
	{
	int				rv= 0;
	int				gv= 0;
	int				bv= 0;
	const int			n= ypixels* xpixels;
	const unsigned char *		cf= rf;

	for ( r= 0; r < ypixels; cf += bdIn->bdBytesPerRow, r++ )
	    {
	    const unsigned char *	f= cf;

	    for ( c= 0; c < xpixels; c++ )
		{ rv += *(f++); gv += *(f++); bv += *(f++);	}
	    }

	*(to++)= ( rv+ n/2 )/ n;
	*(to++)= ( gv+ n/2 )/ n;
	*(to++)= ( bv+ n/2 )/ n;
	}

    return;
    }

static void bmAverageRgb24Image(	const BitmapDescription *	bdOut,
					const BitmapDescription *	bdIn,
					unsigned char *			buffer,
					const unsigned char *		bufIn,
					int				pixels )
    {
    int			row;
    unsigned char *	to= buffer;
    int			ypixels;

    for ( row= 0; row < bdIn->bdPixelsHigh/ pixels; row++ )
	{
	const unsigned char *	rf= bufIn+ pixels* row* bdIn->bdBytesPerRow;

	bmAverageRgb24Row( bdOut, bdIn, to, rf, pixels, pixels );

	to += bdOut->bdBytesPerRow;
	}

    ypixels= bdIn->bdPixelsHigh % pixels;

    if  ( ypixels > 0 )
	{
	const unsigned char *	rf= bufIn+ pixels* row* bdIn->bdBytesPerRow;

	bmAverageRgb24Row( bdOut, bdIn, to, rf, pixels, ypixels );
	}

    return;
    }

int bmAverage(		BitmapDescription *		bdOut,
			const BitmapDescription *	bdIn,
			unsigned char **		pBufOut,
			const unsigned char *		bufIn,
			int				pixels )
    {
    BitmapDescription	bd;
    int			bytesPerRow;

    unsigned char *	buffer;

    switch( bdIn->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	case BMcoRGB:
	    break;
	case BMcoRGB8PALETTE:
	default:
	    LDEB(bdIn->bdColorEncoding);
	    return -1;
	}

    switch( bdIn->bdBitsPerSample )
	{
	case 8:
	    break;
	default:
	    LDEB(bdIn->bdBitsPerSample); return -1;
	}

    /************************************************************/
    /*  Derive properties of output bitmap from input.		*/
    /************************************************************/
    bmInitDescription( &bd );
    bmCopyDescription( &bd, bdIn );
    bd.bdPixelsHigh= ( bdIn->bdPixelsHigh+ pixels- 1 )/pixels;
    bd.bdPixelsWide= ( bdIn->bdPixelsWide+ pixels- 1 )/pixels;
    bytesPerRow= ( bd.bdBitsPerPixel* bd.bdPixelsWide+ 7 )/ 8;
    bd.bdBytesPerRow= bytesPerRow;
    bd.bdBufferLength= bd.bdPixelsHigh* bytesPerRow;

    /************************************************************/
    /*  Allocate new buffer.					*/
    /************************************************************/
    buffer= (unsigned char *)malloc( bd.bdBufferLength );
    if  ( ! buffer )
	{
	LLDEB(bd.bdBufferLength,buffer);
	bmCleanDescription( &bd );
	return -1;
	}

    /************************************************************/
    /*  Average pixel values.					*/
    /************************************************************/
    switch( bd.bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	    bmAverageMono8Image( &bd, bdIn, buffer, bufIn, pixels );
	    break;

	case BMcoRGB:
	    bmAverageRgb24Image( &bd, bdIn, buffer, bufIn, pixels );
	    break;

	case BMcoRGB8PALETTE:
	default:
	    LDEB(bd.bdColorEncoding);
	    return -1;
	}

    *pBufOut= buffer;
    *bdOut= bd;

    return 0;
    }
