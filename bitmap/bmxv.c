#   include	"bitmapConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	"bmintern.h"
#   include	<sioEndian.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Read/Write the 'xv' Copy/Paste format.				*/
/*									*/
/************************************************************************/

int bmXvReadPaste(	BitmapDescription *	bd,
			unsigned char **	pBuffer,
			SimpleInputStream *	sis )
    {
    long		len;
    int			wide;
    int			high;

    int			is24bit;
    int			hasAlpha;

    int			alpha;
    int			redAlpha;
    int			greenAlpha;
    int			blueAlpha;

    int			i;

    RGB8Color *		palette= (RGB8Color *)0;
    unsigned char *	buffer;
    unsigned char *	to;

    len= sioEndianGetLeInt32( sis );
    if  ( len < 0 )
	{ LDEB(len); return -1;	}

    wide= sioEndianGetLeInt16( sis );
    high= sioEndianGetLeInt16( sis );

    is24bit= sioInGetCharacter( sis );

    hasAlpha= sioInGetCharacter( sis );
    alpha= sioInGetCharacter( sis );
    redAlpha= sioInGetCharacter( sis );
    greenAlpha= sioInGetCharacter( sis );
    blueAlpha= sioInGetCharacter( sis );

    if  ( ! is24bit )
	{
	palette= (RGB8Color *)malloc( 256* sizeof( RGB8Color ) );
	if  ( ! palette )
	    { XDEB(palette); return -1;	}

	for ( i= 0; i < 256; i++ )
	    {
	    palette[i].rgb8Red= sioInGetCharacter( sis );
	    palette[i].rgb8Green= sioInGetCharacter( sis );
	    palette[i].rgb8Blue= sioInGetCharacter( sis );
	    palette[i].rgb8Alpha= 255;
	    }
	}

    if  ( is24bit )
	{
	if  ( hasAlpha )
	    {
	    buffer= (unsigned char *)malloc( 4* high* wide );
	    if  ( ! buffer )
		{ LXDEB(wide*high,buffer); return -1;	}

	    bd->bdSamplesPerPixel= 4;
	    bd->bdBytesPerRow= 4* wide;
	    bd->bdBitsPerPixel= 32;
	    }
	else{
	    buffer= (unsigned char *)malloc( 3* high* wide );
	    if  ( ! buffer )
		{ LXDEB(wide*high,buffer); return -1;	}

	    bd->bdSamplesPerPixel= 3;
	    bd->bdBytesPerRow= 3* wide;
	    bd->bdBitsPerPixel= 24;
	    }

	bd->bdColorEncoding= BMcoRGB;
	}
    else{
	if  ( hasAlpha )
	    {
	    buffer= (unsigned char *)malloc( 2* high* wide );
	    if  ( ! buffer )
		{ LXDEB(wide*high,buffer); return -1;	}

	    bd->bdSamplesPerPixel= 4;
	    bd->bdBytesPerRow= 2* wide;
	    bd->bdBitsPerPixel= 16;
	    }
	else{
	    buffer= (unsigned char *)malloc( 1* high* wide );
	    if  ( ! buffer )
		{ LXDEB(wide*high,buffer); return -1;	}

	    bd->bdSamplesPerPixel= 3;
	    bd->bdBytesPerRow= wide;
	    bd->bdBitsPerPixel= 8;
	    }

	bd->bdColorCount= 256;
	bd->bdColorEncoding= BMcoRGB8PALETTE;
	bd->bdRGB8Palette= palette;
	}

    bd->bdPixelsWide= wide;
    bd->bdPixelsHigh= high;
    bd->bdBitsPerSample= 8;
    bd->bdBufferLength= bd->bdPixelsHigh* bd->bdBytesPerRow;

    bd->bdXResolution= 72;
    bd->bdYResolution= 72;
    bd->bdUnit= BMunINCH;

    bd->bdHasAlpha= hasAlpha != 0;

    to= buffer;
    if  ( is24bit )
	{
	if  ( hasAlpha )
	    {
	    for ( i= 0; i < high* wide; i++ )
		{
		*(to++)= sioInGetCharacter( sis );
		*(to++)= sioInGetCharacter( sis );
		*(to++)= sioInGetCharacter( sis );
		*(to  )=	to[-2] != redAlpha	||
				to[-1] != greenAlpha	||
				to[ 0] != blueAlpha	;
		to++;
		}
	    }
	else{
	    for ( i= 0; i < high* wide; i++ )
		{
		*(to++)= sioInGetCharacter( sis );
		*(to++)= sioInGetCharacter( sis );
		*(to++)= sioInGetCharacter( sis );
		}
	    }
	}
    else{
	if  ( hasAlpha )
	    {
	    for ( i= 0; i < high* wide; i++ )
		{
		*(to++)= sioInGetCharacter( sis );
		*(to  )= to[0] != alpha; to++;
		}
	    }
	else{
	    for ( i= 0; i < high* wide; i++ )
		{
		*(to++)= sioInGetCharacter( sis );
		}
	    }
	}

    *pBuffer= buffer;

    return len;
    }

static void bmXvWriteHeader(	long				len,
				int				is24bit,
				const BitmapDescription *	bd,
				SimpleOutputStream *		sos )
    {
    sioEndianPutLeInt32( len, sos );

    sioEndianPutLeInt16( bd->bdPixelsWide, sos );
    sioEndianPutLeInt16( bd->bdPixelsHigh, sos );

    sioOutPutCharacter( is24bit, sos );

    if  (  bd->bdHasAlpha )
	{ sioOutPutCharacter( 1, sos );	}
    else{ sioOutPutCharacter( 0, sos );	}
    sioOutPutCharacter( 255, sos );
    sioOutPutCharacter( 255, sos );
    sioOutPutCharacter( 255, sos );
    sioOutPutCharacter( 255, sos );

    return;
    }

static void bmXvWritePalette(	RGB8Color *		palette,
				int			colorCount,
				SimpleOutputStream *	sos )
    {
    int		i;

    for ( i= 0; i < colorCount; i++ )
	{
	sioOutPutCharacter( palette->rgb8Red, sos );
	sioOutPutCharacter( palette->rgb8Green, sos );
	sioOutPutCharacter( palette->rgb8Blue, sos );

	palette++;
	}

    for ( i= colorCount; i < 256; i++ )
	{
	sioOutPutCharacter( 0, sos );
	sioOutPutCharacter( 0, sos );
	sioOutPutCharacter( 0, sos );

	palette++;
	}
    }

static int bmXvWrite8bits(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				unsigned char *			line,
				SimpleOutputStream *		sos )
    {
    const unsigned char *	from;
    int				row;

    from= buffer;
    if  ( bd->bdBitsPerPixel < 8 )
	{
	for ( row= 0; row < bd->bdPixelsHigh; row++ )
	    {
	    const int	removeAlpha= 0;
	    const int	transpColor= 0;

	    bmInflateTo8bit( line, from, bd, transpColor, removeAlpha );

	    if  ( sioOutWriteBytes( sos, line, bd->bdPixelsWide ) !=
							    bd->bdPixelsWide )
		{ LDEB(bd->bdPixelsWide); return -1;	}

	    from += bd->bdBytesPerRow;
	    }
	}
    else{
	for ( row= 0; row < bd->bdPixelsHigh; row++ )
	    {
	    if  ( sioOutWriteBytes( sos, from, bd->bdPixelsWide ) !=
							    bd->bdPixelsWide )
		{ LDEB(bd->bdPixelsWide); return -1;	}

	    from += bd->bdBytesPerRow;
	    }
	}

    return 0;
    }

static int bmXvWrite24bits(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos )
    {
    const unsigned char *	from;
    int				row;

    from= buffer;
    for ( row= 0; row < bd->bdPixelsHigh; row++ )
	{
	if  ( sioOutWriteBytes( sos, from, 3* bd->bdPixelsWide ) !=
							    3* bd->bdPixelsWide )
	    { LDEB(bd->bdPixelsWide); return -1;	}

	from += bd->bdBytesPerRow;
	}

    return 0;
    }

int bmXvWritePaste(	const BitmapDescription *	bd,
			const unsigned char *		buffer,
			SimpleOutputStream *		sos )
    {
    long			len;
    RGB8Color			palette[256];
    int				colorCount;

    unsigned char *	line= (unsigned char *)0;

    if  ( bd->bdHasAlpha )
	{ LDEB(bd->bdHasAlpha); return -1;	}

    len= 4+ 2* 2+ 1+ 5;

    switch( bd->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:

	    if  ( bd->bdBitsPerPixel < 8 )
		{
		line= (unsigned char *)malloc( bd->bdPixelsWide+ 7 );
		if  ( ! line )
		    { LXDEB(bd->bdPixelsWide,line); return -1;	}
		}

	    if  ( bmMakeGrayPalette( bd, &colorCount, palette ) )
		{ LDEB(bd->bdBitsPerPixel); return -1;	}

	    len += 3* 256;
	    len += bd->bdPixelsHigh* bd->bdPixelsWide;

	    bmXvWriteHeader( len, 0, bd, sos );

	    bmXvWritePalette( palette, colorCount, sos );

	    if  ( bmXvWrite8bits( bd, buffer, line, sos ) )
		{ LDEB(bd->bdPixelsWide); return -1;	}

	    break;

	case BMcoRGB8PALETTE:
	    if  ( bd->bdColorCount > 256 )
		{ LDEB(bd->bdColorCount); return -1;	}

	    if  ( bd->bdBitsPerPixel < 8 )
		{
		line= (unsigned char *)malloc( bd->bdPixelsWide+ 7 );
		if  ( ! line )
		    { LXDEB(bd->bdPixelsWide,line); return -1;	}
		}

	    len += 3* 256;
	    len += bd->bdPixelsHigh* bd->bdPixelsWide;

	    bmXvWriteHeader( len, 0, bd, sos );

	    bmXvWritePalette( bd->bdRGB8Palette, bd->bdColorCount, sos );

	    if  ( bmXvWrite8bits( bd, buffer, line, sos ) )
		{ LDEB(bd->bdPixelsWide); return -1;	}

	    break;

	case BMcoRGB:
	    len += 3* bd->bdPixelsHigh* bd->bdPixelsWide;

	    bmXvWriteHeader( len, 1, bd, sos );

	    if  ( bmXvWrite24bits( bd, buffer, sos ) )
		{ LDEB(bd->bdPixelsWide); return -1;	}

	    break;

	default:
	    LDEB(bd->bdColorEncoding); return -1;
	}

    if  ( line )
	{ free( line );	}

    return len;
    }
