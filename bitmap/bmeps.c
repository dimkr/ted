#   include	"bitmapConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	<sioStdio.h>
#   include	<sioHex.h>
#   include	<sioBase85.h>
#   include	<sioFlate.h>
#   include	<sioLzw.h>

#   include	"bmintern.h"

#   include	<utilPs.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<appDebugon.h>

#   define	MAX_BPS		12

/************************************************************************/
/*									*/
/*  Determine how to print.						*/
/*									*/
/*  Note that bmPsSetBitmapPrinter() has a knowledge of the limitations	*/
/*  of bmPsWriteBitmapData().						*/
/*									*/
/************************************************************************/

int bmPsOpenBitmapPrinter(	BitmapPrinter *			bp,
				SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				useFilters,
				int				indexedImages )
    {
    bp->bpOutput= (SimpleOutputStream *)0;
    bp->bpHexed= (SimpleOutputStream *)0;
    bp->bpBase85= (SimpleOutputStream *)0;
    bp->bpFlate= (SimpleOutputStream *)0;

    if  ( useFilters )
	{
	const int	gzipEmbedded= 0;

	bp->bpBase85= sioOutBase85Open( sos );

	bp->bpFlate= sioOutFlateOpen( bp->bpBase85, gzipEmbedded );
	bp->bpOutput= bp->bpFlate;
	}
    else{
	const int	wide= 72;
	const int	lastNL= 1;

	bp->bpHexed= sioOutHexOpenFolded( sos, wide, lastNL );
	bp->bpOutput= bp->bpHexed;
	}

    bp->bpUseFilters= useFilters;
    bp->bpIndexedImages= indexedImages;

    return 0;
    }

void bmCloseBitmapPrinter(	BitmapPrinter *		bp )
    {
    if  ( bp->bpFlate )
	{ sioOutClose( bp->bpFlate );	}

    if  ( bp->bpBase85 )
	{ sioOutClose( bp->bpBase85 );	}

    if  ( bp->bpHexed )
	{ sioOutClose( bp->bpHexed );	}

    return;
    }

/************************************************************************/

/************************************************************************/
/*									*/
/*  We do our best to print anything.					*/
/*									*/
/************************************************************************/

int bmCanWriteEpsFile(	const BitmapDescription *	bd,
			int				PrivateFormat,
			double				CompressionFactor )
    {
    switch( bd->bdBitsPerSample )
	{
	case 1: case 2: case 4: case 8: case 16:
	    return 0;
	default:
	    /* LDEB(bd->bdBitsPerSample); */ return -1;	
	}
    }

/************************************************************************/
/*									*/
/*  Print Black/White/Color data in hexadecimal format.			*/
/*									*/
/*  1)  The caller of bmPsWriteShortRow() promises to pass even numbers	*/
/*	of shorts to the routine. So there is no issue about odd	*/
/*	counts.								*/
/*									*/
/************************************************************************/

static void bmPsWriteShortRow(	SimpleOutputStream *	sos,
				int			invert,
				int			count,
				const BmUint16 *	from )
    {
    int			col;
    int			val0;
    int			val1;

    if  ( invert )
	{
	/*  1  */
	for ( col= 0; col+ 1 < count; col += 2 )
	    {
	    val0= ~*(from++);
	    val1= ~*(from++);

	    val1= val1 >> 4;

	    sioOutPutCharacter( ( val0 >> 8 ), sos );
	    sioOutPutCharacter(
			( val0 & 0xf0 ) | ( ( val1 & 0xf00 ) >> 8 ), sos );
	    sioOutPutCharacter( ( val1 & 0xff ), sos );
	    }

	if  ( col < count )
	    {
	    val0= ~*(from++);

	    sioOutPutCharacter( ( val0 >> 8 ), sos );
	    sioOutPutCharacter( ( val0 & 0xf0 ), sos );
	    }
	}
    else{
	/*  1  */
	for ( col= 0; col+ 1 < count; col += 2 )
	    {
	    val0= *(from++);
	    val1= *(from++);

	    val1= val1 >> 4;

	    sioOutPutCharacter( ( val0 >> 8 ), sos );
	    sioOutPutCharacter(
			( val0 & 0xf0 ) | ( ( val1 & 0xf00 ) >> 8 ), sos );
	    sioOutPutCharacter( ( val1 & 0xff ), sos );
	    }

	if  ( col < count )
	    {
	    val0= *(from++);

	    sioOutPutCharacter( ( val0 >> 8 ), sos );
	    sioOutPutCharacter( ( val0 & 0xf0 ), sos );
	    }
	}

    return;
    }

static void bmPsWriteByteRow0(	SimpleOutputStream *	sos,
				int			invert,
				int			bytesPerRow,
				const unsigned char *	from )
    {
    int		col;

    if  ( invert )
	{
	for ( col= 0; col < bytesPerRow; col++ )
	    {
	    sioOutPutCharacter( ( ~*(from) ) & 0xff, sos );
	    from++;
	    }
	}
    else{
	for ( col= 0; col < bytesPerRow; col++ )
	    {
	    sioOutPutCharacter( *(from) & 0xff, sos );
	    from++;
	    }
	}

    return;
    }

static void bmPsWriteByteRowX(	SimpleOutputStream *	sos,
				int			invert,
				int			originBit,
				int			rightShift,
				int			bytesPerRow,
				const unsigned char *	from )
    {
    int				col;
    unsigned char		val;

    if  ( invert )
	{
	for ( col= 1; col < bytesPerRow; col++ )
	    {
	    val=
		( ~( ( from[0] << originBit )	|
		     ( from[1] >> rightShift )	) ) &0xff;

	    sioOutPutCharacter( val, sos );
	    from++;
	    }

	val= ( ~( from[0] << originBit ) ) & 0xff;
	sioOutPutCharacter( val, sos );
	}
    else{
	for ( col= 1; col < bytesPerRow; col++ )
	    {
	    val=	( from[0] << originBit )	|
			( from[1] >> rightShift )	;

	    sioOutPutCharacter( val, sos );

	    from++;
	    }

	val= from[0] << originBit;
	sioOutPutCharacter( val, sos );
	}

    return;
    }

static void bmPsWritePal8Row(	SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				bytesPerRow,
				const unsigned char *		from )
    {
    int		col;

    for ( col= 0; col < bytesPerRow; col++ )
	{
	sioOutPutCharacter( bd->bdRGB8Palette[*from].rgb8Red, sos );
	sioOutPutCharacter( bd->bdRGB8Palette[*from].rgb8Green, sos );
	sioOutPutCharacter( bd->bdRGB8Palette[*from].rgb8Blue, sos );

	from++;
	}

    return;
    }

static void bmPsWritePal4Row0(	SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				selWidePix,
				const unsigned char *		from )
    {
    int		col;
    int		val;

    for ( col= 0; col+ 1 < selWidePix; col += 2 )
	{
	val= ( *from >> 4 ) & 0x0f;

	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Red, sos );
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Green, sos );
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Blue, sos );

	val= *from & 0x0f;

	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Red, sos );
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Green, sos );
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Blue, sos );

	from++;
	}

    if  ( selWidePix % 2 )
	{
	val= ( *from >> 4 ) & 0x0f;

	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Red, sos );
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Green, sos );
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Blue, sos );
	}

    return;
    }

static void bmPsWritePal4RowX(	SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				bytesPerRow,
				const unsigned char *		from )
    {
    int		col;
    int		val;

    val= *from & 0x0f;

    sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Red, sos );
    sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Green, sos );
    sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Blue, sos );

    from++;

    for ( col= 1; col < bytesPerRow; col++ )
	{
	val= ( *from >> 4 ) & 0x0f;

	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Red, sos);
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Green, sos);
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Blue, sos);

	val= *from & 0x0f;

	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Red, sos);
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Green, sos);
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Blue, sos);

	from++;
	}

    val= ( *from >> 4 ) & 0x0f;

    sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Red, sos);
    sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Green, sos);
    sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Blue, sos);

    return;
    }

static void bmPsWritePal2Row0(	SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				selWidePix,
				const unsigned char *		from )
    {
    int		col;
    int		val;

    for ( col= 0; col+ 3 < selWidePix; col += 4 )
	{
	val= ( *from >> 6 ) & 0x03;

	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Red, sos );
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Green, sos );
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Blue, sos );

	val= ( *from >> 4 ) & 0x03;

	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Red, sos );
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Green, sos );
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Blue, sos );

	val= ( *from >> 2 ) & 0x03;

	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Red, sos );
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Green, sos );
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Blue, sos );

	val= ( *from >> 0 ) & 0x03;

	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Red, sos );
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Green, sos );
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Blue, sos );

	from++;
	}

    if  ( selWidePix % 4 > 0 )
	{
	val= ( *from >> 6 ) & 0x03;

	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Red, sos );
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Green, sos );
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Blue, sos );
	}

    if  ( selWidePix % 4 > 1 )
	{
	val= ( *from >> 4 ) & 0x03;

	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Red, sos );
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Green, sos );
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Blue, sos );
	}

    if  ( selWidePix % 4 > 2 )
	{
	val= ( *from >> 2 ) & 0x03;

	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Red, sos );
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Green, sos );
	sioOutPutCharacter( bd->bdRGB8Palette[val].rgb8Blue, sos );
	}

    return;
    }

int bmPsWriteBitmapData(	    BitmapPrinter *		bp,
				    int				selX0Pix,
				    int				selY0Pix,
				    int				selWidePix,
				    int				selHighPix,
				    const BitmapDescription *	bd,
				    const unsigned char *	inputBuffer )
    {
    SimpleOutputStream *	sos= bp->bpOutput;
    int				indexedImages= bp->bpIndexedImages;
    int				directPixels;

    int				row;

    int				originBit;
    int				originByte;
    int				rightShift;

    int				bytesPerRow;

    const unsigned char *	from;

    directPixels= 0;
    if  ( bd->bdColorEncoding == BMcoWHITEBLACK )
	{ directPixels= 1;	}
    if  ( bd->bdColorEncoding == BMcoBLACKWHITE )
	{ directPixels= 1;	}
    if  ( bd->bdColorEncoding == BMcoRGB && ! bd->bdHasAlpha )
	{ directPixels= 1;	}
    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE && indexedImages )
	{ directPixels= 1;	}
    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE && bd->bdBitsPerPixel == 1 )
	{ directPixels= 1;	}

    if  ( bd->bdBitsPerSample == 16 && directPixels )
	{
	int		invert= bd->bdColorEncoding == BMcoBLACKWHITE;
	int		shortsPerRow;
	int		shortsPerCall;

	originByte= 2* selX0Pix;
	shortsPerRow= ( selWidePix* bd->bdBitsPerPixel+ 15 )/ 16;

	shortsPerCall= 2* ( shortsPerRow/ 2 );

	for ( row= 0; row < selHighPix; row++ )
	    {
	    from= inputBuffer+
			( row+ selY0Pix )* bd->bdBytesPerRow+ originByte;

	    bmPsWriteShortRow( sos, invert, shortsPerRow,
						    (const BmUint16 *)from );
	    }

	return 0;
	}

    originBit=   ( selX0Pix*   bd->bdBitsPerPixel )% 8;
    originByte=  ( selX0Pix*   bd->bdBitsPerPixel )/ 8;
    bytesPerRow= ( selWidePix* bd->bdBitsPerPixel+ 7 )/ 8;
    rightShift= 8- originBit;

    if  ( originBit == 0 && directPixels )
	{
	int	invert= bd->bdColorEncoding == BMcoBLACKWHITE;

	for ( row= 0; row < selHighPix; row++ )
	    {
	    from= inputBuffer+
			( row+ selY0Pix )* bd->bdBytesPerRow+ originByte;

	    bmPsWriteByteRow0( sos, invert, bytesPerRow, from );
	    }

	return 0;
	}

    if  ( bd->bdColorEncoding == BMcoRGB	&&
	  bd->bdHasAlpha			)
	{
	const int	invert= 0;
	unsigned char *	scratch;

	scratch= malloc( bd->bdBytesPerRow );
	if  ( ! scratch )
	    { LXDEB(bd->bdBytesPerRow,scratch); return -1;	}

	if  ( bd->bdBitsPerSample == 8 )
	    {
	    for ( row= 0; row < selHighPix; row++ )
		{
		int		col;
		unsigned char *	to= scratch;

		from= inputBuffer+
			    ( row+ selY0Pix )* bd->bdBytesPerRow+ originByte;

		for ( col= 0; col < selWidePix; col++ )
		    {
		    if  ( from[3] < 127 )
			{
			*(to++)= 0xff;
			*(to++)= 0xff;
			*(to++)= 0xff;
			from += 4;
			}
		    else{
			*(to++)= *(from++);
			*(to++)= *(from++);
			*(to++)= *(from++);
			from++;
			}
		    }

		bmPsWriteByteRow0( sos, invert, 3* selWidePix, scratch );
		}
	    }
	else{
	    LDEB(bd->bdBitsPerSample); free( scratch ); return -1;
	    }

	free( scratch );
	return 0;
	}

    if  ( bd->bdColorEncoding == BMcoWHITEBLACK	||
	  bd->bdColorEncoding == BMcoBLACKWHITE )
	{
	const int	invert= bd->bdColorEncoding == BMcoBLACKWHITE;

	for ( row= 0; row < selHighPix; row++ )
	    {
	    from= inputBuffer+
			( row+ selY0Pix )* bd->bdBytesPerRow+ originByte;

	    bmPsWriteByteRowX( sos, invert,
				originBit, rightShift, bytesPerRow, from );
	    }

	return 0;
	}

    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE	&&
	  ! directPixels				&&
	  bd->bdBitsPerPixel == 8			)
	{
	for ( row= 0; row < selHighPix; row++ )
	    {
	    from= inputBuffer+
			( row+ selY0Pix )* bd->bdBytesPerRow+ originByte;

	    bmPsWritePal8Row( sos, bd, bytesPerRow, from );
	    }

	return 0;
	}

    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE	&&
	  ! directPixels				&&
	  bd->bdBitsPerPixel == 4			&&
	  originBit == 0				)
	{
	for ( row= 0; row < selHighPix; row++ )
	    {
	    from= inputBuffer+
			( row+ selY0Pix )* bd->bdBytesPerRow+ originByte;

	    bmPsWritePal4Row0( sos, bd, selWidePix, from );
	    }

	return 0;
	}

    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE	&&
	  ! directPixels				&&
	  bd->bdBitsPerPixel == 4			&&
	  originBit == 4				)
	{
	for ( row= 0; row < selHighPix; row++ )
	    {
	    from= inputBuffer+
			( row+ selY0Pix )* bd->bdBytesPerRow+ originByte;

	    bmPsWritePal4RowX( sos, bd, bytesPerRow, from );
	    }

	return 0;
	}

    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE	&&
	  ! directPixels				&&
	  bd->bdBitsPerPixel == 2			&&
	  originBit == 0				)
	{
	for ( row= 0; row < selHighPix; row++ )
	    {
	    from= inputBuffer+
			( row+ selY0Pix )* bd->bdBytesPerRow+ originByte;

	    bmPsWritePal2Row0( sos, bd, selWidePix, from );
	    }

	return 0;
	}

    LDEB(directPixels);
    LDEB(bd->bdBitsPerPixel);
    LLDEB(originBit,bd->bdColorEncoding); return -1;
    }

/************************************************************************/
/*									*/
/*  Emit code for the emulation of color images on gray scale printers.	*/
/*									*/
/*  I do not understand this either but it works.			*/
/*									*/
/************************************************************************/

static void bmEpsColorEmulation( SimpleOutputStream *	sos )
    {
    sioOutPutString( "\
	/bwproc {\n\
	    rgbproc\n\
	    dup length 3 idiv string 0 3 0\n\
	    5 -1 roll {\n\
		add 2 1 roll 1 sub dup 0 eq {\n\
		    pop 3 idiv\n\
		    3 -1 roll\n\
		    dup 4 -1 roll\n\
		    dup 3 1 roll\n\
		    5 -1 roll put\n\
		    1 add 3 0\n\
		} { 2 1 roll } ifelse\n\
	    } forall\n\
	    pop pop pop\n\
	} def\n\
	/colorimage where {pop} {\n\
	    /colorimage {pop pop /rgbproc exch def {bwproc} image} bind def\n\
	} ifelse\n", sos );
    }

/************************************************************************/
/*									*/
/*  Return the size of a PostScript string that can hold one row of	*/
/*  data for the bitmap.						*/
/*									*/
/************************************************************************/

int bmPsRowStringSize(		const BitmapDescription *	bd,
				int				pixelsWide,
				int				indexedImages )
    {
    switch( bd->bdColorEncoding )
	{
	case	BMcoWHITEBLACK:
	case	BMcoBLACKWHITE:
	case	BMcoRGB:

	    if  ( bd->bdBitsPerSample == 16 )
		{
		int bpp= MAX_BPS* bd->bdSamplesPerPixel;

		return ( pixelsWide* bpp+ 7 )/ 8;
		}
	    else{
		return ( pixelsWide* bd->bdBitsPerPixel+ 7 )/ 8;
		}

	case	BMcoRGB8PALETTE:

	    if  ( bd->bdBitsPerPixel == 1 )
		{ return ( pixelsWide+ 7 )/ 8;				}

	    if  ( indexedImages )
		{ return ( pixelsWide* bd->bdBitsPerPixel+ 7 )/ 8;	}
	    else{ return ( pixelsWide* 24+ 7 )/ 8;			}

	default:

	    LDEB(bd->bdColorEncoding);
	    return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Print one Encapsulated postsctipt page.				*/
/*									*/
/************************************************************************/

int bmPsPrintBitmap(	SimpleOutputStream *		sos,
			int				level,
			double				xscale,
			double				yscale,
			int				ox,
			int				oy,
			int				selX0Pix,
			int				selY0Pix,
			int				selWidePix,
			int				selHighPix,
			int				useFilters,
			int				indexedImages,
			const BitmapDescription *	bd,
			const unsigned char *		buffer	)
    {
    switch( bd->bdUnit )
	{
	case BMunM:
	    xscale= ( xscale* POINTS_PER_M* selWidePix )/ bd->bdXResolution;
	    yscale= ( yscale* POINTS_PER_M* selHighPix )/ bd->bdYResolution;
	    break;

	case BMunINCH:
	    xscale= ( xscale* 72.0* selWidePix )/ bd->bdXResolution;
	    yscale= ( yscale* 72.0* selHighPix )/ bd->bdYResolution;
	    break;

	case BMunPOINT:
	    xscale= ( xscale* 1.0* selWidePix )/ bd->bdXResolution;
	    yscale= ( yscale* 1.0* selHighPix )/ bd->bdYResolution;
	    break;

	case BMunPIXEL:
	    xscale= ( xscale* 1.0* selWidePix );
	    yscale= ( yscale* 1.0* selHighPix );
	    break;

	default:
	    LDEB(bd->bdUnit);
	    xscale= ( xscale* 1.0* selWidePix );
	    yscale= ( yscale* 1.0* selHighPix );
	    break;
	}

    return bmPsPrintBitmapImage( sos, level, xscale, yscale, ox, oy,
				    selX0Pix, selY0Pix, selWidePix, selHighPix,
				    useFilters, indexedImages,
				    bd, buffer );
    }

/************************************************************************/
/*									*/
/*  Write image instructions.						*/
/*									*/
/************************************************************************/

static void bmPsWriteMonoImageInstructions(
				SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				selWidePix,
				int				selHighPix,
				const char *			source )
    {
    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE	&&
	  bd->bdColorCount == 2				)
	{
	const RGB8Color *	rgb8= &(bd->bdRGB8Palette[1]);

	if  ( rgb8->rgb8Red == rgb8->rgb8Green	&&
	      rgb8->rgb8Red == rgb8->rgb8Blue	)
	    {
	    sioOutPrintf( sos, "%g setgray\n",
					rgb8->rgb8Red/255.0 );
	    }
	else{
	    sioOutPrintf( sos, "%g %g %g setrgbcolor\n",
					rgb8->rgb8Red/255.0,
					rgb8->rgb8Green/255.0,
					rgb8->rgb8Blue/255.0 );
	    }
	}
    else{
	sioOutPrintf( sos, "0 setgray\n" );
	}

    sioOutPrintf( sos, "%u %u", selWidePix, selHighPix );

    sioOutPrintf( sos, " false\n" );

    sioOutPrintf( sos, "[ %u 0 0 -%u 0 %u ]\n",
				selWidePix, selHighPix, selHighPix );

    sioOutPrintf( sos, "%s\n", source );

    sioOutPrintf( sos, "imagemask\n" );

    return;
    }

static void bmPsWriteGrayImageInstructions(
				SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				selWidePix,
				int				selHighPix,
				const char *			source )
    {
    int		bitsPerSample= bd->bdBitsPerSample;

    if  ( bitsPerSample > MAX_BPS )
	{ bitsPerSample= MAX_BPS;	}

    sioOutPrintf( sos, "%u %u", selWidePix, selHighPix );

    sioOutPrintf( sos, " %d\n", bitsPerSample );

    sioOutPrintf( sos, "[ %u 0 0 -%u 0 %u ]\n",
				selWidePix, selHighPix, selHighPix );

    sioOutPrintf( sos, "%s\n", source );

    sioOutPrintf( sos, "image\n" );

    return;
    }

static void bmPsWriteRgbImageInstructions(
				SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				selWidePix,
				int				selHighPix,
				const char *			source )
    {
    int		bitsPerSample= bd->bdBitsPerSample;

    if  ( bitsPerSample > MAX_BPS )
	{ bitsPerSample= MAX_BPS;	}

    bmEpsColorEmulation( sos );

    sioOutPrintf( sos, "%u %u %d\n",
			    selWidePix, selHighPix, bitsPerSample );

    sioOutPrintf( sos, "[ %u 0 0 -%u 0 %u ]\n",
			    selWidePix, selHighPix, selHighPix );

    sioOutPrintf( sos, "%s\n", source );
    sioOutPrintf( sos, "false %d colorimage\n", 3 );

    return;
    }

static void bmPsWriteIndexedImageInstructions(
				SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				selWidePix,
				int				selHighPix,
				const char *			source )
    {
    int		i;

    /*  Done by caller
    sioOutPrintf( sos, "gsave\n" );
    */

    sioOutPrintf( sos, "[ /Indexed /DeviceRGB %d\n",
					    bd->bdColorCount- 1 );
    sioOutPrintf( sos, "<\n" );

    for ( i= 0; i < bd->bdColorCount; i++ )
	{
	sioOutPrintf( sos, " %02x%02x%02x\n", 
				bd->bdRGB8Palette[i].rgb8Red,
				bd->bdRGB8Palette[i].rgb8Green,
				bd->bdRGB8Palette[i].rgb8Blue );
	}

    sioOutPrintf( sos, ">\n" );

    sioOutPrintf( sos, "] setcolorspace\n" );

    sioOutPrintf( sos, "10 dict\n" );
    sioOutPrintf( sos, "  dup /ImageType 1 put\n" );
    sioOutPrintf( sos, "  dup /Width %u put\n",
					selWidePix );
    sioOutPrintf( sos, "  dup /Height %u put\n",
					selHighPix );
    sioOutPrintf( sos, "  dup /ImageMatrix [ %u 0 0 -%u 0 %u ] put\n",
					selWidePix, selHighPix, selHighPix );
    sioOutPrintf( sos, "  dup /BitsPerComponent %d put\n",
					bd->bdBitsPerPixel );
    sioOutPrintf( sos, "  dup /DataSource %s put\n",
					source  );
    sioOutPrintf( sos, "  dup /Decode [ 0 %d ] put\n",
					( 1 << bd->bdBitsPerPixel )- 1  );
    sioOutPrintf( sos, "image\n" );

    /*  Done by caller after image data
    sioOutPrintf( sos, "grestore\n" );
    */

    return;
    }

void bmPsWriteImageInstructions(
				SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				selWidePix,
				int				selHighPix,
				const char *			source,
				int				indexedImages )
    {
    int		bitsPerSample= bd->bdBitsPerSample;

    if  ( bitsPerSample > MAX_BPS )
	{ bitsPerSample= MAX_BPS;	}

    switch( bd->bdColorEncoding )
	{
	case BMcoWHITEBLACK:
	case BMcoBLACKWHITE:

	    if  ( bd->bdBitsPerSample == 1 )
		{
		bmPsWriteMonoImageInstructions( sos, bd,
					    selWidePix, selHighPix, source );
		}
	    else{
		bmPsWriteGrayImageInstructions( sos, bd,
					    selWidePix, selHighPix, source );
		}

	    break;
	
	case BMcoRGB:

	    bmPsWriteRgbImageInstructions( sos, bd,
					    selWidePix, selHighPix, source );
	    break;

	case BMcoRGB8PALETTE:

	    if  ( bd->bdBitsPerPixel == 1 )
		{
		bmPsWriteMonoImageInstructions( sos, bd,
					    selWidePix, selHighPix, source );
		break;
		}

	    if  ( indexedImages )
		{
		bmPsWriteIndexedImageInstructions( sos, bd,
					    selWidePix, selHighPix, source );
		break;
		}

	    bmPsWriteRgbImageInstructions( sos, bd,
					    selWidePix, selHighPix, source );
	    break;

	default:
	    LDEB(bd->bdColorEncoding);
	}

    return;
    }

int bmPsPrintBitmapImage(	SimpleOutputStream *		sos,
				int				level,
				double				xscale,
				double				yscale,
				int				ox,
				int				oy,
				int				selX0Pix,
				int				selY0Pix,
				int				selWidePix,
				int				selHighPix,
				int				useFilters,
				int				indexedImages,
				const BitmapDescription *	bd,
				const unsigned char *		buffer )
    {
    BitmapPrinter	bp;

    if  ( bd->bdColorEncoding != BMcoRGB8PALETTE )
	{ indexedImages= 0;	}

    sioOutPrintf( sos, "gsave 10 dict begin %% {> bitmap image\n" );

    if  ( ox != 0 || oy != 0 || xscale != 1.0 || yscale != 1.0 )
	{
	if  ( ox != 0 || oy != 0 )
	    { sioOutPrintf( sos, "%d %d translate ", ox, oy );	}
	if  ( xscale != 1.0 || yscale != 1.0 )
	    { sioOutPrintf( sos, "%f %f scale ", xscale, yscale ); }

	sioOutPrintf( sos, "\n" );
	}

    if  ( useFilters )
	{
#	if	USE_LZW
	bmPsWriteImageInstructions( sos, bd, selWidePix, selHighPix,
		"currentfile /ASCII85Decode filter /LZWDecode filter",
		indexedImages );
#	else
	bmPsWriteImageInstructions( sos, bd, selWidePix, selHighPix,
		"currentfile /ASCII85Decode filter /FlateDecode filter",
		indexedImages );
#	endif
	}
    else{
	int bytesPerRow= bmPsRowStringSize( bd, selWidePix, indexedImages );
	if  ( bytesPerRow < 0 )
	    { LDEB(bytesPerRow); return -1;	}

	sioOutPrintf( sos, "/line %d string def\n", bytesPerRow );

	bmPsWriteImageInstructions( sos, bd, selWidePix, selHighPix,
			    "{ currentfile line readhexstring pop } bind",
			    indexedImages );
	}

    bmPsOpenBitmapPrinter( &bp, sos, bd, useFilters, indexedImages );

    if  ( bmPsWriteBitmapData( &bp,
		selX0Pix, selY0Pix, selWidePix, selHighPix, bd, buffer ) )
	{ LDEB(1); return -1;	}

    bmCloseBitmapPrinter( &bp );

    sioOutPrintf( sos, "end grestore %% }< bitmap image\n");

    return 0;
    }

/************************************************************************/
/*									*/
/*  Calculate the page state parameters.				*/
/*									*/
/************************************************************************/

static void bmPsPageExtent(	const BitmapDescription *	bd,
				float *				pPointsWide,
				float *				pPointsHigh,
				int				pixelsWide,
				int				pixelsHigh )
    {
    double	xResolution;
    double	yResolution;

    switch( bd->bdUnit )
	{
	case BMunM:
	    xResolution= bd->bdXResolution/ POINTS_PER_M;
	    yResolution= bd->bdYResolution/ POINTS_PER_M;
	    break;

	case BMunINCH:
	    xResolution= bd->bdXResolution/ 72.0;
	    yResolution= bd->bdYResolution/ 72.0;
	    break;

	case BMunPOINT:
	    xResolution= bd->bdXResolution;
	    yResolution= bd->bdYResolution;
	    break;

	case BMunPIXEL:
	    xResolution= 1.0;
	    yResolution= 1.0;
	    break;

	default:
	    LDEB(bd->bdUnit); return;
	}

    *pPointsWide= pixelsWide/ xResolution;
    *pPointsHigh= pixelsHigh/ yResolution;

    return;
    }

/************************************************************************/
/*									*/
/*  Save a bitmap image to Encapsulated PostScript (EPS) Format.	*/
/*									*/
/*  1)  Only 1,2,4,8,16 bits per sample images are supported.		*/
/*  2)  Open the output file.						*/
/*  3)  Determine the bounding box of the image.			*/
/*  4)  Issue the EPS header comments.					*/
/*  5)  Issue the Image contents.					*/
/*  6)  Include a 'showpage' command.					*/
/*  7)  Finish with a general header.					*/
/*									*/
/************************************************************************/

int bmWriteEpsFile(	const char *			filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    {
    SimpleOutputStream *	sos;
    float			pointsWide;
    float			pointsHigh;

    int				useFilters= 1;
    int				indexedImages= 1;

    if  ( privateFormat < 2 )
	{ useFilters= indexedImages= 0;	}

    /*  1  */
    switch( bd->bdBitsPerSample )
	{
	case 1:
	case 2:
	case 4:
	case 8:
	case 16:
	    break;

	default:
	    LDEB(bd->bdBitsPerSample); return -1;	
	}

    /*  2  */
    sos= sioOutStdioOpen( filename );
    if  ( ! sos )
	{ SXDEB(filename,sos); return -1;	}

    /*  3  */
    bmPsPageExtent( bd, &pointsWide, &pointsHigh,
					bd->bdPixelsWide, bd->bdPixelsHigh );

    /*  4  */
    appPsWriteEpsHeader( sos, 
		    "Scan, Mark de Does, mark@mdedoes.com",
		    filename, 
		    (int)ceil( pointsWide ), (int)ceil( pointsHigh )	);

    /*  5  */
    if  ( bmPsPrintBitmap( sos, privateFormat, 1.0, 1.0, 0, 0,
				0, 0, bd->bdPixelsWide, bd->bdPixelsHigh,
				useFilters, indexedImages, bd, buffer ) )
	{ LDEB(1); sioOutClose( sos ); return -1;	}

    /*  6  */
    sioOutPrintf( sos, "showpage\n");

    /*  7  */
    sioOutPrintf( sos, "%%%%Trailer\n" );
    sioOutPrintf( sos, "%%%%EOF\n" );

    sioOutClose( sos ); return 0;
    }
