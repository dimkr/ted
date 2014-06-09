#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	"bmgetrow.h"
#   include	<string.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Collect rows of data for image transformations.			*/
/*									*/
/************************************************************************/

static void bmGetBytePixels(	unsigned char *	to,
				unsigned char	from,
				int		bitsPerPixel )
    {
    unsigned	mask;
    int		shift;

    mask= ( 1 << bitsPerPixel )- 1;

    for ( shift= 8- bitsPerPixel; shift >= 0; to++, shift -= bitsPerPixel )
	{ *to= ( from >> shift ) & mask;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Fill a Depth 8 image from rgb8 palette data.			*/
/*									*/
/************************************************************************/

static void bmGetPaletteSourceRow(	ColorValue *			cv,
					int				col0Out,
					const unsigned char *		from,
					int				col0In,
					int				colPIn,
					const BitmapDescription *	bdIn )
    {
    int			col;
    unsigned		mask;

    int			pos;
    int			past;
    int			pixelsPerByte;
    unsigned char	scratch[8];

    cv += col0Out;

    switch( bdIn->bdBitsPerPixel )
	{
	case 8:
	    from += col0In;

	    for ( col= col0In; col < colPIn; from++, cv++, col++ )
		{
		cv->cvR += bdIn->bdRGB8Palette[*from].rgb8Red;
		cv->cvG += bdIn->bdRGB8Palette[*from].rgb8Green;
		cv->cvB += bdIn->bdRGB8Palette[*from].rgb8Blue;
		cv->cvN++;
		}
	    return;

	case 4: case 2: case 1:
	    mask= ( 1 << bdIn->bdBitsPerPixel )- 1;

	    pixelsPerByte= 8/ bdIn->bdBitsPerPixel;

	    from += col0In/ pixelsPerByte;

	    if  ( col0In % pixelsPerByte )
		{
		past= col0In+ pixelsPerByte- 1;
		past /= pixelsPerByte;
		past *= pixelsPerByte;

		bmGetBytePixels( scratch, *from, bdIn->bdBitsPerPixel );

		if  ( past > colPIn )
		    { past=  colPIn;	}

		pos= past- col0In;
		for ( col= col0In; col < past; pos++, cv++, col++ )
		    {
		    cv->cvR += bdIn->bdRGB8Palette[scratch[pos]].rgb8Red;
		    cv->cvG += bdIn->bdRGB8Palette[scratch[pos]].rgb8Green;
		    cv->cvB += bdIn->bdRGB8Palette[scratch[pos]].rgb8Blue;
		    cv->cvN++;
		    }

		col0In= past; from++;
		}

	    for ( col= col0In; col+ pixelsPerByte- 1 < colPIn; from++,
							col += pixelsPerByte )
		{
		int	shift= 8- bdIn->bdBitsPerPixel;
		int	pix;

		for ( pix= 0; pix < pixelsPerByte;
				    cv++, shift -= bdIn->bdBitsPerPixel, pix++ )
		    {
		    int		val= ( *from >> shift ) & mask;

		    cv->cvR += bdIn->bdRGB8Palette[val].rgb8Red;
		    cv->cvG += bdIn->bdRGB8Palette[val].rgb8Green;
		    cv->cvB += bdIn->bdRGB8Palette[val].rgb8Blue;
		    cv->cvN++;
		    }
		}

	    if  ( col < colPIn )
		{
		bmGetBytePixels( scratch, *from, bdIn->bdBitsPerPixel );

		pos= 0;
		for ( ; col < colPIn; pos++, cv++, col++ )
		    {
		    cv->cvR += bdIn->bdRGB8Palette[scratch[pos]].rgb8Red;
		    cv->cvG += bdIn->bdRGB8Palette[scratch[pos]].rgb8Green;
		    cv->cvB += bdIn->bdRGB8Palette[scratch[pos]].rgb8Blue;
		    cv->cvN++;
		    }
		}

	    return;

	case 16:
	    {
	    const BmUint16 *	psh= (const BmUint16 *)from;

	    psh += col0In;

	    for ( col= col0In; col < colPIn; psh++, cv++, col++ )
		{
		cv->cvR += bdIn->bdRGB8Palette[*psh].rgb8Red;
		cv->cvG += bdIn->bdRGB8Palette[*psh].rgb8Green;
		cv->cvB += bdIn->bdRGB8Palette[*psh].rgb8Blue;
		cv->cvN++;
		}

	    return;
	    }

	case 32:
	    {
	    const BmUint32 *	plo= (const BmUint32 *)from;

	    plo += col0In;

	    for ( col= col0In; col < colPIn; plo++, cv++, col++ )
		{
		cv->cvR += bdIn->bdRGB8Palette[*plo].rgb8Red;
		cv->cvG += bdIn->bdRGB8Palette[*plo].rgb8Green;
		cv->cvB += bdIn->bdRGB8Palette[*plo].rgb8Blue;
		cv->cvN++;
		}

	    return;
	    }

	default:
	    LDEB(bdIn->bdBitsPerPixel); return;
	}
    }

/************************************************************************/
/*									*/
/*  Fill a Depth 8 image from rgb8 palette data with an alpha channel.	*/
/*									*/
/************************************************************************/

static void bmGetPaletteSourceRowAlpha(	ColorValue *			cv,
					int				col0Out,
					const unsigned char *		from,
					int				col0In,
					int				colPIn,
					const BitmapDescription *	bdIn )
    {
    int		col;
    unsigned	mask;

    cv += col0Out;

    switch( bdIn->bdBitsPerPixel )
	{
	case 32:
	    {
	    const BmUint16 *	psh= (const BmUint16 *)from;

	    psh += 2* col0In;

	    for ( col= col0In; col < colPIn; psh += 2, cv++, col++ )
		{
		cv->cvR += bdIn->bdRGB8Palette[*psh].rgb8Red;
		cv->cvG += bdIn->bdRGB8Palette[*psh].rgb8Green;
		cv->cvB += bdIn->bdRGB8Palette[*psh].rgb8Blue;
		cv->cvN++;
		}
	    }
	return;

	case 16:
	    from += 2* col0In;

	    for ( col= col0In; col < colPIn; from += 2, cv++, col++ )
		{
		if  ( from[1] )
		    {
		    cv->cvR += bdIn->bdRGB8Palette[*from].rgb8Red;
		    cv->cvG += bdIn->bdRGB8Palette[*from].rgb8Green;
		    cv->cvB += bdIn->bdRGB8Palette[*from].rgb8Blue;
		    }
		else{
		    /* why not white?*/
		    cv->cvR += 255;
		    cv->cvG += 255;
		    cv->cvB += 255;
		    }
		cv->cvN++;
		}
	    return;

	case 8:
	    mask= 0x0f;

	    from += col0In;

	    for ( col= col0In; col < colPIn; from++, cv++, col++ )
		{
		if  ( *from & 0x0f )
		    {
		    int		val= ( *from >> 4 ) & mask;

		    cv->cvR += bdIn->bdRGB8Palette[val].rgb8Red;
		    cv->cvG += bdIn->bdRGB8Palette[val].rgb8Green;
		    cv->cvB += bdIn->bdRGB8Palette[val].rgb8Blue;
		    }
		else{
		    /* why not white?*/
		    cv->cvR += 255;
		    cv->cvG += 255;
		    cv->cvB += 255;
		    }
		cv->cvN++;
		}
	    return;

	default:
	    LDEB(bdIn->bdBitsPerPixel); return;
	}
    }

static void bmGetBlackWhite124SourceRow(ColorValue *			cv,
					int				col0Out,
					const unsigned char *		from,
					int				col0In,
					int				colPIn,
					const BitmapDescription *	bdIn )
    {
    int			col;
    unsigned		mask;

    int			pos;
    int			past;
    int			pixelsPerByte;
    unsigned char	scratch[8];

    cv += col0Out;

    mask= ( 1 << bdIn->bdBitsPerPixel )- 1;

    pixelsPerByte= 8/ bdIn->bdBitsPerPixel;
    from += col0In/ pixelsPerByte;

    if  ( col0In % pixelsPerByte )
	{
	past= col0In+ pixelsPerByte- 1;
	past /= pixelsPerByte;
	past *= pixelsPerByte;

	bmGetBytePixels( scratch, *from, bdIn->bdBitsPerPixel );

	if  ( past > colPIn )
	    { past=  colPIn;	}

	pos= past- col0In;
	for ( col= col0In; col < past; pos++, cv++, col++ )
	    {
	    unsigned int	val;

	    val= ( 255* scratch[pos] )/ mask;
	    val= 255- val;

	    cv->cvR += val;
	    cv->cvG += val;
	    cv->cvB += val;
	    cv->cvN++;
	    }

	col0In= past; from++;
	}

    for ( col= col0In; col+ pixelsPerByte- 1 < colPIn; from++,
							col += pixelsPerByte )
	{
	int	shift= 8- bdIn->bdBitsPerPixel;
	int	pix;

	for ( pix= 0; pix < pixelsPerByte;
				cv++, shift -= bdIn->bdBitsPerPixel, pix++ )
	    {
	    int		val= ( *from >> shift ) & mask;

	    val= ( 255* val )/ mask;
	    val= 255- val;

	    cv->cvR += val;
	    cv->cvG += val;
	    cv->cvB += val;
	    cv->cvN++;
	    }
	}

    if  ( col < colPIn )
	{
	bmGetBytePixels( scratch, *from, bdIn->bdBitsPerPixel );

	pos= 0;
	for ( ; col < colPIn; pos++, cv++, col++ )
	    {
	    unsigned int	val;

	    val= ( 255* scratch[pos] )/ mask;
	    val= 255- val;

	    cv->cvR += val;
	    cv->cvG += val;
	    cv->cvB += val;
	    cv->cvN++;
	    }
	}

    return;
    }

static void bmGetWhiteBlack124SourceRow(ColorValue *			cv,
					int				col0Out,
					const unsigned char *		from,
					int				col0In,
					int				colPIn,
					const BitmapDescription *	bdIn )
    {
    int			col;
    unsigned		mask;

    int			pos;
    int			past;
    int			pixelsPerByte;
    unsigned char	scratch[8];

    cv += col0Out;

    mask= ( 1 << bdIn->bdBitsPerPixel )- 1;

    pixelsPerByte= 8/ bdIn->bdBitsPerPixel;
    from += col0In/ pixelsPerByte;

    if  ( col0In % pixelsPerByte )
	{
	past= col0In+ pixelsPerByte- 1;
	past /= pixelsPerByte;
	past *= pixelsPerByte;

	bmGetBytePixels( scratch, *from, bdIn->bdBitsPerPixel );

	if  ( past > colPIn )
	    { past=  colPIn;	}

	pos= past- col0In;
	for ( col= col0In; col < past; pos++, cv++, col++ )
	    {
	    unsigned int	val;

	    val= ( 255* scratch[pos] )/ mask;

	    cv->cvR += val;
	    cv->cvG += val;
	    cv->cvB += val;
	    cv->cvN++;
	    }

	col0In= past; from++;
	}

    for ( col= col0In; col+ pixelsPerByte- 1 < colPIn; from++,
							col += pixelsPerByte )
	{
	int	shift= 8- bdIn->bdBitsPerPixel;
	int	pix;

	for ( pix= 0; pix < pixelsPerByte;
				cv++, shift -= bdIn->bdBitsPerPixel, pix++ )
	    {
	    int		val= ( *from >> shift ) & mask;

	    val= ( 255* val )/ mask;

	    cv->cvR += val;
	    cv->cvG += val;
	    cv->cvB += val;
	    cv->cvN++;
	    }
	}

    if  ( col < colPIn )
	{
	bmGetBytePixels( scratch, *from, bdIn->bdBitsPerPixel );

	pos= 0;
	for ( ; col < colPIn; pos++, cv++, col++ )
	    {
	    unsigned int	val;

	    val= ( 255* scratch[pos] )/ mask;

	    cv->cvR += val;
	    cv->cvG += val;
	    cv->cvB += val;
	    cv->cvN++;
	    }
	}

    return;
    }

static void bmGetBlackWhite8SourceRow(	ColorValue *			cv,
					int				col0Out,
					const unsigned char *		from,
					int				col0In,
					int				colPIn,
					const BitmapDescription *	bdIn )
    {
    int			col;

    cv += col0Out;
    from += col0In;

    for ( col= col0In; col < colPIn; cv++, from++, col++ )
	{
	int		val= 255- *from;

	cv->cvR += val;
	cv->cvG += val;
	cv->cvB += val;
	cv->cvN++;
	}
    }

static void bmGetWhiteBlack8SourceRow(	ColorValue *			cv,
					int				col0Out,
					const unsigned char *		from,
					int				col0In,
					int				colPIn,
					const BitmapDescription *	bdIn )
    {
    int			col;

    cv += col0Out;
    from += col0In;

    for ( col= col0In; col < colPIn; cv++, from++, col++ )
	{
	cv->cvR += *from;
	cv->cvG += *from;
	cv->cvB += *from;
	cv->cvN++;
	}
    }

static void bmGetWhiteBlack16ASourceRow(ColorValue *			cv,
					int				col0Out,
					const unsigned char *		from,
					int				col0In,
					int				colPIn,
					const BitmapDescription *	bdIn )
    {
    int			col;

    cv += col0Out;
    from += col0In;

    for ( col= col0In; col < colPIn; cv++, from += 2, col++ )
	{
	cv->cvR += *from;
	cv->cvG += *from;
	cv->cvB += *from;
	cv->cvN++;
	}
    }

/************************************************************************/
/*									*/
/*  Add data from one row of a 24 bits image to an acumulator array.	*/
/*									*/
/************************************************************************/

static void bmGetRGB24SourceRow(	ColorValue *			cv,
					int				col0Out,
					const unsigned char *		from,
					int				col0In,
					int				colPIn,
					const BitmapDescription *	bdIn )
    {
    int		col;

    cv += col0Out;
    from += 3* col0In;

    for ( col= col0In; col < colPIn; cv++, col++ )
	{
	cv->cvR += *(from++);
	cv->cvG += *(from++);
	cv->cvB += *(from++);
	cv->cvN++;
	}

    return;
    }

static void bmGetRGBA32SourceRow(	ColorValue *			cv,
					int				col0Out,
					const unsigned char *		from,
					int				col0In,
					int				colPIn,
					const BitmapDescription *	bdIn )
    {
    int		col;

    cv += col0Out;
    from += 3* col0In;

    for ( col= col0In; col < colPIn; cv++, col++ )
	{
	if  ( from[3] )
	    {
	    cv->cvR += *(from++);
	    cv->cvG += *(from++);
	    cv->cvB += *(from++);
	    from++;
	    }
	else{
	    /*why not white*/
	    cv->cvR += 255;
	    cv->cvG += 255;
	    cv->cvB += 255;
	    from += 4;
	    }
	cv->cvN++;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Add data from one row of a 48 bits image to an acumulator array.	*/
/*									*/
/************************************************************************/

static void bmGetRGB48SourceRow(	ColorValue *			cv,
					int				col0Out,
					const unsigned char *		ucFrom,
					int				col0In,
					int				colPIn,
					const BitmapDescription *	bdIn )
    {
    int			col;
    const BmUint16 *	from= (const BmUint16 *)ucFrom;

    cv += col0Out;
    from += 3* col0In;

    for ( col= col0In; col < colPIn; cv++, col++ )
	{
	cv->cvR += *(from++)/ 256;
	cv->cvG += *(from++)/ 256;
	cv->cvB += *(from++)/ 256;
	cv->cvN++;
	}

    return;
    }

static void bmGetRGBA64SourceRow(	ColorValue *			cv,
					int				col0Out,
					const unsigned char *		ucFrom,
					int				col0In,
					int				colPIn,
					const BitmapDescription *	bdIn )
    {
    int			col;
    const BmUint16 *	from= (const BmUint16 *)ucFrom;

    cv += col0Out;
    from += 3* col0In;

    for ( col= col0In; col < colPIn; cv++, col++ )
	{
	cv->cvR += *(from++)/ 256;
	cv->cvG += *(from++)/ 256;
	cv->cvB += *(from++)/ 256;
	from++;
	cv->cvN++;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Initialize a row of color values.					*/
/*									*/
/************************************************************************/

void bmInitColorRow(	ColorValue *		cv,
			int			wide )
    {
    int			col;

    for ( col= 0; col < wide; cv++, col++ )
	{
	cv->cvR= 0L;
	cv->cvG= 0L;
	cv->cvB= 0L;
	cv->cvN= 0;
	}
    }

/************************************************************************/
/*									*/
/*  Determine how to retrieve pixels from an input image.		*/
/*									*/
/************************************************************************/

int bmGetGetRow(	GetSourceRow *			pGetRow,
			const BitmapDescription *	bdIn )
    {
    GetSourceRow	getRow= (GetSourceRow)0;

    /*  4  */
    switch( bdIn->bdColorEncoding )
	{
	case BMcoRGB8PALETTE:
	    if  ( bdIn->bdHasAlpha )
		{
		getRow= bmGetPaletteSourceRowAlpha;
		break;
		}
	    else{
		getRow= bmGetPaletteSourceRow;
		break;
		}
	    break;

	case BMcoBLACKWHITE:
	    switch( bdIn->bdBitsPerPixel )
		{
		case 1:
		case 2:
		case 4:
		    getRow= bmGetBlackWhite124SourceRow;
		    break;

		case 8:
		    getRow= bmGetBlackWhite8SourceRow;
		    break;

		default:
		    LLDEB(bdIn->bdColorEncoding,bdIn->bdBitsPerPixel);
		    return -1;
		}
	    break;

	case BMcoWHITEBLACK:
	    switch( bdIn->bdBitsPerPixel )
		{
		case 1:
		case 2:
		case 4:
		    getRow= bmGetWhiteBlack124SourceRow;
		    break;

		case 8:
		    getRow= bmGetWhiteBlack8SourceRow;
		    break;

		case 16:
		    if  ( bdIn->bdHasAlpha )
			{ getRow= bmGetWhiteBlack16ASourceRow;	}
		    else{
			LLDEB(bdIn->bdBitsPerPixel,bdIn->bdHasAlpha);
			return -1;
			}
		    break;

		default:
		    LLDEB(bdIn->bdColorEncoding,bdIn->bdBitsPerPixel);
		    return -1;
		}
	    break;

	case BMcoRGB:
	    switch( bdIn->bdBitsPerSample )
		{
		case 8:
		    if  ( bdIn->bdHasAlpha )
			{ getRow= bmGetRGBA32SourceRow;	}
		    else{ getRow= bmGetRGB24SourceRow;	}
		    break;

		case 16:
		    if  ( bdIn->bdHasAlpha )
			{ getRow= bmGetRGBA64SourceRow;	}
		    else{ getRow= bmGetRGB48SourceRow;	}
		    break;

		default:
		    LLDEB(bdIn->bdColorEncoding,bdIn->bdBitsPerSample);
		    return -1;
		}
	    break;
	default:
	    LDEB(bdIn->bdColorEncoding); return -1;
	}

    *pGetRow= getRow;
    return 0;
    }
