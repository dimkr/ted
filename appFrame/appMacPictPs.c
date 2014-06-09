#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<appMacPict.h>
#   include	<utilPs.h>
#   include	<utilMatchFont.h>
#   include	<sioEndian.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<appDebugon.h>

#   ifndef	M_PI
#	define	M_PI	3.14159265358979323846
#   endif

# if 0
#    define	PICTDEB(x)	(x)
# else
#    define	PICTDEB(x)	/*nothing*/
# endif

# if 0
#    define	PICTLOG(x)	(x)
# else
#    define	PICTLOG(x)	/*nothing*/
# endif

/************************************************************************/
/*									*/
/*  Get information on the current font.				*/
/*									*/
/************************************************************************/

static int appMacPictGetAfi(		MacpictDevice *			md,
					const PostScriptFontList *	psfl )
    {
    DocumentFontList *		dfl= &(md->mdFontList);
    DocumentFont *		df;

    df= dfl->dflFonts+ md->mdTextAttribute.taFontNumber;

    if  ( utilFindPsFontForDocFont( df, dfl, psfl ) )
	{
	LDEB(md->mdTextAttribute.taFontNumber);
	md->mdAfi= (AfmFontInfo *)0;
	md->mdFontEncoding= -1;
	return -1;
	}

    md->mdAfi= utilPsGetAfi( &(md->mdFontEncoding), dfl,
					    psfl, &(md->mdTextAttribute) );
    if  ( ! md->mdAfi )
	{ XDEB(md->mdAfi); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  MACPICT_PackBitsRect						*/
/*									*/
/*  1)  Initialise bitmap.						*/
/*  2)  Get image.							*/
/*  13) Ready: clean image related data.				*/
/*									*/
/************************************************************************/

static int appMacPictSkipImage(		MacpictDevice *		md,
					SimpleInputStream *	sis,
					int			packed,
					int			direct,
					int			clipRegion )
    {
    int			rval= 0;

    int			y0Source;
    int			x0Source;
    int			y1Source;
    int			x1Source;

    int			y0Dest;
    int			x0Dest;
    int			y1Dest;
    int			x1Dest;

    AppBitmapImage	abi;

    int			bytesRead= 0;

    /*  1  */
    appInitBitmapImage( &abi );

    /*  2  */
    if  ( appMacPictGetImage( &abi,
			&y0Source, &x0Source, &y1Source, &x1Source,
			&y0Dest, &x0Dest, &y1Dest, &x1Dest,
			md, sis, packed, direct, clipRegion ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  11  */
    if  ( md->mdVersion > 1 && bytesRead % 2 )
	{ (void)sioInGetCharacter( sis );	}

    /*  13  */
  ready:
    appInitBitmapImage( &abi );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Remember a font.							*/
/*									*/
/************************************************************************/

static int appMacPictRememberFont(	MacpictDevice *		md,
					const PostScriptFontList * psfl,
					const char *		prefix,
					PostScriptTypeList *	pstl )
    {
    if  ( ! md->mdAfi )
	{
	if  ( appMacPictGetAfi( md, psfl ) )
	    { LDEB(1);	}

	if  ( md->mdAfi )
	    {
	    const int		appearsInText= 0;

	    if  ( utilRememberPostsciptFace( pstl,
			    md->mdFontEncoding, md->mdAfi,
			    &(md->mdTextAttribute), prefix, appearsInText ) )
		{ SDEB(md->mdAfi->afiFontName); return -1;	}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  List the fonts in a macintosh pict file.				*/
/*									*/
/*  The geometry parameters are absolutely irrelevant, but required by	*/
/*  appMacPictInitDeviceHeader().					*/
/*									*/
/************************************************************************/

int appMacPictListFontsPs( PostScriptTypeList *	pstl,
			SimpleInputStream *	sis,
			const PostScriptFontList * psfl,
			const char *		prefix,
			int			mapMode,
			int			xWinExt,
			int			yWinExt,
			int			twipsWide,
			int			twipsHigh )
    {
    int			c;
    int			i;

    int			rval= 0;

    MacpictDevice	md;


    appMacPictInitDeviceHeader( &md, twipsWide, twipsHigh,
						    twipsWide, twipsHigh );

    if  ( appMacPictGetDeviceHeader( &md, sis ) )
	{ LDEB(1); return -1;	}

    md.mdPostScriptFontList= psfl;

    PICTDEB(appDebug( "\n" ));
    PICTDEB(appDebug( "MACPICT FONTS( %d .. %d x %d .. %d -> %d x %d ) VERSION %d\n",
		    md.mdInX0, md.mdInX1, md.mdInY0, md.mdInY1,
		    md.mdOutWide, md.mdOutHigh, md.mdVersion ));

    for (;;)
	{
	unsigned int	opcode;
	int		bytes;

	if  ( appMacPictGetOpcode( &opcode, &bytes, &md, sis ) )
	    { LDEB(1); rval= -1; goto ready;	}

	switch( opcode )
	    {
	    case MACPICT_NOP:
		PICTDEB(appDebug( "NOP()\n" ));
		continue;

	    case MACPICT_PnSize:
		{
		int			h;
		int			w;

		h= sioEndianGetBeInt16( sis );
		w= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "PnSize( %d, %d )\n", w, h ));
		}
		continue;

	    case MACPICT_OvSize:
		if  ( appMacPictGetOvSize( &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		continue;

	    case MACPICT_DefHilite:
		PICTDEB(appDebug( "DefHilite()\n" ));
		continue;

	    case MACPICT_Clip:
		{
		int	regionBytes= sioEndianGetBeInt16( sis );
		int	i;

		PICTDEB(appDebug( "Clip( %d )\n", regionBytes/ 2 ));

		for ( i= 2; i < regionBytes; i += 2 )
		    {
		    int		regionCoord;

		    regionCoord= sioEndianGetBeInt16( sis );
		    }
		}
		continue;

	    case MACPICT_PnMode:
		md.mdPenMode= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "PnMode(%d)\n", md.mdPenMode ));

		continue;

	    case MACPICT_PnLocHFrac:
		{
		int	val;

		val= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "PnLocHFrac(%d)\n", val ));

		}
		continue;

	    case MACPICT_RGBFgCol:
		if  ( appMacPictGetColor( &(md.mdForeColor), &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug( "RGBFgCol(%d,%d,%d)\n",
					md.mdForeColor.rgb8Red,
					md.mdForeColor.rgb8Green,
					md.mdForeColor.rgb8Blue ));

		md.mdStippleSet= STIPPLE_UNDEF;

		continue;

	    case MACPICT_RGBBkCol:
		if  ( appMacPictGetColor( &(md.mdBackColor), &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug( "RGBBkCol(%d,%d,%d)\n",
					md.mdBackColor.rgb8Red,
					md.mdBackColor.rgb8Green,
					md.mdBackColor.rgb8Blue ));

		md.mdStippleSet= STIPPLE_UNDEF;

		continue;

	    case MACPICT_PnPat:
		if  ( appMacPictGetPattern( &md, sis, STIPPLE_PEN,
				    &(md.mdPenIsSolid), md.mdPenPattern ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug(
			"PnPat(%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x)\n",
					    md.mdPenPattern[0],
					    md.mdPenPattern[1],
					    md.mdPenPattern[2],
					    md.mdPenPattern[3],
					    md.mdPenPattern[4],
					    md.mdPenPattern[4],
					    md.mdPenPattern[6],
					    md.mdPenPattern[7] ));

		continue;

	    case MACPICT_FillPat:
		if  ( appMacPictGetPattern( &md, sis, STIPPLE_FILL,
				    &(md.mdFillIsSolid), md.mdFillPattern ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug(
			"FillPat(%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x)\n",
					    md.mdFillPattern[0],
					    md.mdFillPattern[1],
					    md.mdFillPattern[2],
					    md.mdFillPattern[3],
					    md.mdFillPattern[4],
					    md.mdFillPattern[4],
					    md.mdFillPattern[6],
					    md.mdFillPattern[7] ));

		continue;

	    case MACPICT_BkPat:
		if  ( appMacPictGetPattern( &md, sis, STIPPLE_BACK,
				    &(md.mdBackIsSolid), md.mdBackPattern ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug(
			"BkPat(%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x)\n",
					    md.mdBackPattern[0],
					    md.mdBackPattern[1],
					    md.mdBackPattern[2],
					    md.mdBackPattern[3],
					    md.mdBackPattern[4],
					    md.mdBackPattern[4],
					    md.mdBackPattern[6],
					    md.mdBackPattern[7] ));

		continue;

	    case MACPICT_TxRatio:
		{
		int			y0;
		int			x0;
		int			y1;
		int			x1;

		y0= sioEndianGetBeInt16( sis );
		x0= sioEndianGetBeInt16( sis );
		y1= sioEndianGetBeInt16( sis );
		x1= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "TxRatio( %d, %d, %d, %d )\n",
							x0, y0, x1, y1 ));
		}
		continue;

	    case MACPICT_FrameOval:
		{
		if  ( appMacPictGetOval( &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FrameSameOval:
		continue;

	    case MACPICT_PaintOval:
		{
		if  ( appMacPictGetOval( &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PaintSameOval:
		continue;

	    case MACPICT_PaintPoly:
		{
		if  ( appMacPictGetPoly( &md, sis ) )
		    { rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FramePoly:
		{
		if  ( appMacPictGetPoly( &md, sis ) )
		    { rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_OpColor:
		{
		md.mdRedMax= sioEndianGetBeUint16( sis );
		md.mdGreenMax= sioEndianGetBeUint16( sis );
		md.mdBlueMax= sioEndianGetBeUint16( sis );
		}
		continue;

	    case MACPICT_ShortLine:
		{
		int			x0;
		int			y0;
		int			x1;
		int			y1;

		if  ( appMacPictGetShortLine( &x0, &y0, &x1, &y1, &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_ShortLineFrom:
		{
		int			y0;
		int			x0;
		int			y1;
		int			x1;

		if  ( appMacPictGetShortLineFrom( &x0, &y0, &x1, &y1,
								&md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_LineFrom:
		{
		int			y0;
		int			x0;
		int			y1;
		int			x1;

		if  ( appMacPictGetLineFrom( &x0, &y0, &x1, &y1, &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_Line:
		{
		int			y0;
		int			x0;
		int			y1;
		int			x1;

		if  ( appMacPictGetLine( &x0, &y0, &x1, &y1, &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FrameRRect:
		{
		md.mdRRectY0= sioEndianGetBeInt16( sis );
		md.mdRRectX0= sioEndianGetBeInt16( sis );
		md.mdRRectY1= sioEndianGetBeInt16( sis );
		md.mdRRectX1= sioEndianGetBeInt16( sis );
		}
		continue;

	    case MACPICT_FrameSameRRect:
		continue;

	    case MACPICT_PaintRRect:
		{
		md.mdRRectY0= sioEndianGetBeInt16( sis );
		md.mdRRectX0= sioEndianGetBeInt16( sis );
		md.mdRRectY1= sioEndianGetBeInt16( sis );
		md.mdRRectX1= sioEndianGetBeInt16( sis );
		}
		continue;

	    case MACPICT_PaintSameRRect:
		continue;

	    case MACPICT_FillRRect:
		{
		md.mdRRectY0= sioEndianGetBeInt16( sis );
		md.mdRRectX0= sioEndianGetBeInt16( sis );
		md.mdRRectY1= sioEndianGetBeInt16( sis );
		md.mdRRectX1= sioEndianGetBeInt16( sis );
		}
		continue;

	    case MACPICT_FillSameRRect:
		continue;

	    case MACPICT_EraseRRect:
		{
		md.mdRRectY0= sioEndianGetBeInt16( sis );
		md.mdRRectX0= sioEndianGetBeInt16( sis );
		md.mdRRectY1= sioEndianGetBeInt16( sis );
		md.mdRRectX1= sioEndianGetBeInt16( sis );
		}
		continue;

	    case MACPICT_EraseSameRRect:
		continue;

	    case MACPICT_FrameRect:
		{
		md.mdRectY0= sioEndianGetBeInt16( sis );
		md.mdRectX0= sioEndianGetBeInt16( sis );
		md.mdRectY1= sioEndianGetBeInt16( sis );
		md.mdRectX1= sioEndianGetBeInt16( sis );
		}
		continue;

	    case MACPICT_FrameSameRect:
		continue;

	    case MACPICT_PaintRect:
		{
		md.mdRectY0= sioEndianGetBeInt16( sis );
		md.mdRectX0= sioEndianGetBeInt16( sis );
		md.mdRectY1= sioEndianGetBeInt16( sis );
		md.mdRectX1= sioEndianGetBeInt16( sis );
		}
		continue;

	    case MACPICT_PaintSameRect:
		continue;

	    case MACPICT_FillRect:
		{
		md.mdRectY0= sioEndianGetBeInt16( sis );
		md.mdRectX0= sioEndianGetBeInt16( sis );
		md.mdRectY1= sioEndianGetBeInt16( sis );
		md.mdRectX1= sioEndianGetBeInt16( sis );
		}
		continue;

	    case MACPICT_FillSameRect:
		continue;

	    case MACPICT_EraseRect:
		{
		md.mdRectY0= sioEndianGetBeInt16( sis );
		md.mdRectX0= sioEndianGetBeInt16( sis );
		md.mdRectY1= sioEndianGetBeInt16( sis );
		md.mdRectX1= sioEndianGetBeInt16( sis );
		}
		continue;

	    case MACPICT_EraseSameRect:
		continue;

	    case MACPICT_FrameArc:
		{
		if  ( appMacPictGetArc( &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PaintArc:
		{
		if  ( appMacPictGetArc( &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FontName:
		if  ( appMacPictGetFontName( &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		continue;

		if  ( appMacPictRememberFont( &md, psfl, prefix, pstl ) )
		    { LDEB(1);	}


	    case MACPICT_TxFont:
		if  ( appMacPictGetTxFont( &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( appMacPictRememberFont( &md, psfl, prefix, pstl ) )
		    { LDEB(1);	}

		continue;

	    case MACPICT_TxFace:
		if  ( appMacPictGetTxFace( &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( appMacPictRememberFont( &md, psfl, prefix, pstl ) )
		    { LDEB(1);	}

		continue;

	    case MACPICT_TxSize:
		if  ( appMacPictGetTxSize( &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		continue;

	    case MACPICT_GlyphState:
		if  ( appMacPictGetGlyphState( &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		continue;

	    case MACPICT_DHText:
		{
		int	count;

		if  ( appMacPictGetDHText( &count, &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_DVText:
		{
		int	count;

		if  ( appMacPictGetDVText( &count, &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_DHDVText:
		{
		int	count;

		if  ( appMacPictGetDHDVText( &count, &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_LongText:
		{
		int	count;

		if  ( appMacPictGetLongText( &count, &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_LongComment:
		{
		int	kind= sioEndianGetBeInt16( sis );
		int	size= sioEndianGetBeInt16( sis );

		kind= kind;

		for ( i= 0; i < size; i++ )
		    { c= sioInGetCharacter( sis );	}

		if  ( md.mdVersion > 1 && size % 2 )
		    { (void)sioInGetCharacter( sis );	}
		}
		continue;

	    case MACPICT_ShortComment:
		{
		int	kind= sioEndianGetBeInt16( sis );

		kind= kind;
		}
		continue;

	    case MACPICT_FrameRgn:
		{
		int	regionBytes= sioEndianGetBeInt16( sis );
		int	i;

		PICTDEB(appDebug( "FrameRgn( %d )\n", regionBytes ));

		for ( i= 2; i < regionBytes; i++ )
		    {
		    int		regionCoord;

		    regionCoord= sioInGetCharacter( sis );
		    }
		}
		continue;

	    case MACPICT_PaintRgn:
		{
		int	regionBytes= sioEndianGetBeInt16( sis );
		int	i;

		PICTDEB(appDebug( "PaintRgn( %d )\n", regionBytes ));

		for ( i= 2; i < regionBytes; i++ )
		    {
		    int		regionCoord;

		    regionCoord= sioInGetCharacter( sis );
		    }
		}
		continue;

	    case MACPICT_BitsRect:
		{
		const int	packed= 0;
		const int	direct= 0;
		const int	clipRegion= 0;

		PICTDEB(appDebug( "BitsRect( .. )\n" ));

		if  ( appMacPictSkipImage( &md, sis,
						packed, direct, clipRegion ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PackBitsRect:
		{
		const int	packed= 1;
		const int	direct= 0;
		const int	clipRegion= 0;

		PICTDEB(appDebug( "PackBitsRect( .. )\n" ));

		if  ( appMacPictSkipImage( &md, sis,
						packed, direct, clipRegion ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PackBitsRgn:
		{
		const int	packed= 1;
		const int	direct= 0;
		const int	clipRegion= 1;

		PICTDEB(appDebug( "PackBitsRgn( .. )\n" ));

		if  ( appMacPictSkipImage( &md, sis,
						packed, direct, clipRegion ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_DirectBitsRect:
		{
		const int	packed= 0;
		const int	direct= 1;
		const int	clipRegion= 0;

		PICTDEB(appDebug( "DirectBitsRect( .. )\n" ));

		if  ( appMacPictSkipImage( &md, sis,
						packed, direct, clipRegion ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_DirectBitsRgn:
		{
		const int	packed= 0;
		const int	direct= 1;
		const int	clipRegion= 1;

		PICTDEB(appDebug( "DirectBitsRgn( .. )\n" ));

		if  ( appMacPictSkipImage( &md, sis,
						packed, direct, clipRegion ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_CompressedQuickTime:
		{
		long	qtbytes= sioEndianGetBeInt32( sis );
		int	i;

		PICTDEB(appDebug( "CompressedQuickTime( %ld )\n",
							    qtbytes ));

		for ( i= 0; i < qtbytes; i++ )
		    {
		    int		qtdata;

		    qtdata= sioInGetCharacter( sis );
		    }

		}
		continue;

	    case MACPICT_UncompressedQuickTime:
		{
		long	qtbytes= sioEndianGetBeInt32( sis );
		int	i;

		PICTDEB(appDebug( "UncompressedQuickTime( %ld )\n",
							    qtbytes ));

		for ( i= 0; i < qtbytes; i++ )
		    {
		    int		qtdata;

		    qtdata= sioInGetCharacter( sis );
		    }

		}
		continue;

	    case 0x66:
		XLDEB(opcode,bytes);
		bytes= 12;
		goto skipBytes;

	    default:
	    skipBytes:
		if  ( bytes >= 0 )
		    {
		    XLDEB(opcode,bytes);

		    for ( i= 0; i < bytes; i++ )
			{ c= sioInGetCharacter( sis );	}

		    continue;
		    }
		XDEB(opcode); rval= -1; goto ready;

	    case MACPICT_OpEndPic:
		PICTDEB(appDebug( "OpEndPic()\n" ));
		break;
	    }

	break;
	}

  ready:

    appMacPictCleanDeviceHeader( &md );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Set the current color (to draw)					*/
/*									*/
/************************************************************************/

static int appMacPictSetColorPs(	MacpictDevice *		md,
					PrintingState *		ps,
					RGB8Color *		rgb8 )
    {
    if  ( bmRGB8ColorsDiffer( &(md->mdColorSet), rgb8 ) )
	{
	utilPsSetRgbColor( ps, rgb8->rgb8Red/ 255.0,
					    rgb8->rgb8Green/ 255.0,
					    rgb8->rgb8Blue/ 255.0 );
	md->mdColorSet= *rgb8;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Actual color/pattern setting.					*/
/*									*/
/************************************************************************/

static int appMacPictSetStipplePs(	MacpictDevice *		md,
					PrintingState *		ps,
					unsigned char		pattern[8],
					int			which )
    {
    /*  1  */
    if  ( which != STIPPLE_SOLID && md->mdStippleSet == which )
	{ return 0;	}

    /*  3  */
    if  ( which == STIPPLE_SOLID )
	{
	/*  4  */
	if  ( pattern[0] )
	    {
	    if  ( appMacPictSetColorPs( md, ps, &(md->mdForeColor) ) )
		{ LDEB(1); return -1;	}
	    }
	else{
	    if  ( appMacPictSetColorPs( md, ps, &(md->mdBackColor) ) )
		{ LDEB(1); return -1;	}
	    }

	md->mdStippleSet= which;
	md->mdFillStippled= 0;
	return 0;
	}

    /*  1  */
    if  ( md->mdStippleSet == which )
	{ return 0;	}

    /*  5  */
    {
    AppBitmapImage	abi;

    appInitBitmapImage( &abi );
    abi.abiBuffer= pattern;

    abi.abiBitmap.bdPixelsWide= 8;
    abi.abiBitmap.bdPixelsHigh= 8;
    abi.abiBitmap.bdColorEncoding= BMcoBLACKWHITE;
    abi.abiBitmap.bdBitsPerSample= 1;
    abi.abiBitmap.bdSamplesPerPixel= 1;
    abi.abiBitmap.bdBitsPerPixel= 1;

    abi.abiBitmap.bdXResolution= 1;
    abi.abiBitmap.bdYResolution= 1;
    abi.abiBitmap.bdUnit= BMunPIXEL;

    bmCalculateSizes( &(abi.abiBitmap) );

    if  ( appMetafileStartPatternFillPs( ps, &abi ) )
	{ LDEB(1); return -1;	}

    abi.abiBuffer= (unsigned char *)0;
    appCleanBitmapImage( &abi );

    md->mdFillStippled= 1;
    }

    return 0;
    }

static int appMacPict_FillPath(	PrintingState *		ps,
				MacpictDevice *		md )
    {
    if  ( md->mdFillStippled )
	{ sioOutPrintf( ps->psSos, "fill-pattern\n" );	}
    else{ sioOutPrintf( ps->psSos, "fill\n" );		}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Use the pen/fill pattern for drawing.				*/
/*									*/
/************************************************************************/

static int appMacPictSetPenPs(		MacpictDevice *		md,
					PrintingState *		ps )
    {
    if  ( md->mdPenIsSolid )
	{
	if  ( appMacPictSetStipplePs( md, ps,
				md->mdPenPattern, STIPPLE_SOLID ) )
	    { LDEB(STIPPLE_SOLID); return -1;	}
	}
    else{
	if  ( appMacPictSetStipplePs( md, ps,
				md->mdPenPattern, STIPPLE_PEN ) )
	    { LDEB(STIPPLE_PEN); return -1;	}
	}

    return 0;
    }

static int appMacPictSetFillPs(	MacpictDevice *		md,
				PrintingState *		ps )
    {
    if  ( md->mdFillIsSolid )
	{
	if  ( appMacPictSetStipplePs( md, ps,
				md->mdFillPattern, STIPPLE_SOLID ) )
	    { LDEB(STIPPLE_SOLID); return -1;	}
	}
    else{
	if  ( appMacPictSetStipplePs( md, ps,
				md->mdFillPattern, STIPPLE_FILL ) )
	    { LDEB(STIPPLE_FILL); return -1;	}
	}

    return 0;
    }

static int appMacPictSetBackPs(	MacpictDevice *		md,
				PrintingState *		ps )
    {
    if  ( md->mdBackIsSolid )
	{
	if  ( appMacPictSetStipplePs( md, ps,
				md->mdBackPattern, STIPPLE_SOLID ) )
	    { LDEB(STIPPLE_SOLID); return -1;	}
	}
    else{
	if  ( appMacPictSetStipplePs( md, ps,
				md->mdBackPattern, STIPPLE_BACK ) )
	    { LDEB(STIPPLE_BACK); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a line.							*/
/*									*/
/************************************************************************/

static int appMacPictDrawLinePs(	MacpictDevice *		md,
					PrintingState *		ps,
					int			x0,
					int			y0,
					int			x1,
					int			y1 )
    {
    if  ( x1 == x0 && y1 == y0 )
	{ return 0;	}

    if  ( appMacPictSetPenPs( md, ps ) )
	{ LDEB(1); return -1;	}

    x0= MD_X( x0, md );
    y0= MD_Y( y0, md );
    x1= MD_X( x1, md );
    y1= MD_Y( y1, md );

    sioOutPrintf( ps->psSos, "%d %d bp ", x0, y0 );
    sioOutPrintf( ps->psSos, "%d %d rl stroke\n", x1- x0, y1- y0 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a string.							*/
/*									*/
/************************************************************************/

static int appMacPictDrawStringPs(	MacpictDevice *		md,
					const PostScriptFontList * pstl,
					PrintingState *		ps,
					int			x,
					int			y,
					int			count )
    {
    int		width;
    AfmBBox	abb;
    const int	withKerning= 0;
    int		fontSizeTwips;

    while( count > 0 && isspace( md->mdTextString[count-1] ) )
	{ count--;	}

    if  ( count == 0 )
	{ return 0;	}

    if  ( appMacPictSetPenPs( md, ps ) )
	{ LDEB(1); return -1;	}

    x= MD_X( x, md );
    y= MD_Y( y, md );

    if  ( ! md->mdAfi )
	{
	if  ( md->mdTextAttribute.taFontNumber < 0 )
	    { LDEB(md->mdTextAttribute.taFontNumber);	}
	else{
	    if  ( appMacPictGetAfi( md, pstl ) )
		{ LDEB(1);						}
	    else{ utilPsSetFont( ps, "pf", &(md->mdTextAttribute) );	}
	    }
	}

    if  ( ! md->mdAfi )
	{ XDEB(md->mdAfi); return 0;	}

    fontSizeTwips= 10* md->mdTextAttribute.taFontSizeHalfPoints;
    width= psCalculateStringExtents( &abb,
				    md->mdTextString, count,
				    fontSizeTwips, withKerning,
				    md->mdFontEncoding, md->mdAfi );

    utilPsMovePrintStringValue( ps, md->mdTextString, count, x, y );

    return 0;
    }


/************************************************************************/
/*									*/
/*  FrameRect/PaintRect variants.					*/
/*									*/
/************************************************************************/

static int appMacPictFrameRectPs(	MacpictDevice *		md,
					PrintingState *		ps )
    {
    int x0= MD_X( md->mdRectX0, md );
    int y0= MD_Y( md->mdRectY0, md );
    int x1= MD_X( md->mdRectX1, md );
    int y1= MD_Y( md->mdRectY1, md );

    if  ( appMacPictSetPenPs( md, ps ) )
	{ LDEB(1); return -1;	}

    appMetafileRectPathPs( ps, x0, y0, x1, y1 );
    sioOutPrintf( ps->psSos, "stroke\n" );

    return 0;
    }

static int appMacPictPaintRectPs(	MacpictDevice *		md,
					PrintingState *		ps )
    {
    int x0= MD_X( md->mdRectX0, md );
    int y0= MD_Y( md->mdRectY0, md );
    int x1= MD_X( md->mdRectX1, md );
    int y1= MD_Y( md->mdRectY1, md );

    if  ( appMacPictSetPenPs( md, ps ) )
	{ LDEB(1); return -1;	}

    appMetafileRectPathPs( ps, x0, y0, x1, y1 );

    appMacPict_FillPath( ps, md );

    return 0;
    }

static int appMacPictFillRectPs(	MacpictDevice *		md,
					PrintingState *		ps )
    {
    int x0= MD_X( md->mdRectX0, md );
    int y0= MD_Y( md->mdRectY0, md );
    int x1= MD_X( md->mdRectX1, md );
    int y1= MD_Y( md->mdRectY1, md );

    if  ( appMacPictSetFillPs( md, ps ) )
	{ LDEB(1); return -1;	}

    appMetafileRectPathPs( ps, x0, y0, x1, y1 );

    appMacPict_FillPath( ps, md );

    return 0;
    }

static int appMacPictEraseRectPs(	MacpictDevice *		md,
					PrintingState *		ps )
    {
    int x0= MD_X( md->mdRectX0, md );
    int y0= MD_Y( md->mdRectY0, md );
    int x1= MD_X( md->mdRectX1, md );
    int y1= MD_Y( md->mdRectY1, md );

    if  ( appMacPictSetBackPs( md, ps ) )
	{ LDEB(1); return -1;	}

    appMetafileRectPathPs( ps, x0, y0, x1, y1 );

    appMacPict_FillPath( ps, md );

    return 0;
    }

/************************************************************************/
/*									*/
/*  PaintRRect/FrameRRect variants.					*/
/*									*/
/************************************************************************/

static int appMacPictFrameRRectPs(	MacpictDevice *		md,
					PrintingState *		ps )
    {
    int		x0= MD_X( md->mdRRectX0, md );
    int		y0= MD_Y( md->mdRRectY0, md );
    int		x1= MD_X( md->mdRRectX1, md );
    int		y1= MD_Y( md->mdRRectY1, md );

    int		h= MD_H( md->mdRRectOvalHigh, md );
    int		w= MD_W( md->mdRRectOvalWide, md );

    if  ( appMacPictSetPenPs( md, ps ) )
	{ LDEB(1); return -1;	}

    if  ( h < 0 )
	{ h= -h;	}
    if  ( w < 0 )
	{ w= -w;	}

    sioOutPrintf( ps->psSos, "gsave [1 0 0 %g 0 %d] concat\n",
					    (double)h/ w, ( y0+ y1 )/ 2 );

    appMetafileRoundRectPathPs( ps, x0, y0, x1, y1, w, h, "stroke\n" );

    sioOutPrintf( ps->psSos, "grestore\n" );

    return 0;
    }

static int appMacPictPaintRRectPs(	MacpictDevice *		md,
					PrintingState *		ps )
    {
    int		x0= MD_X( md->mdRRectX0, md );
    int		y0= MD_Y( md->mdRRectY0, md );
    int		x1= MD_X( md->mdRRectX1, md );
    int		y1= MD_Y( md->mdRRectY1, md );

    int		h= MD_H( md->mdRRectOvalHigh, md );
    int		w= MD_W( md->mdRRectOvalWide, md );

    if  ( appMacPictSetPenPs( md, ps ) )
	{ LDEB(1); return -1;	}

    if  ( h < 0 )
	{ h= -h;	}
    if  ( w < 0 )
	{ w= -w;	}

    sioOutPrintf( ps->psSos, "gsave [1 0 0 %g 0 %d] concat\n",
					    (double)h/ w, ( y0+ y1 )/ 2 );

    appMetafileRoundRectPathPs( ps, x0, y0, x1, y1, w, h, "closepath\n" );

    appMacPict_FillPath( ps, md );

    sioOutPrintf( ps->psSos, "grestore\n" );

    return 0;
    }

static int appMacPictFillRRectPs(	MacpictDevice *		md,
					PrintingState *		ps )
    {
    int		x0= MD_X( md->mdRRectX0, md );
    int		y0= MD_Y( md->mdRRectY0, md );
    int		x1= MD_X( md->mdRRectX1, md );
    int		y1= MD_Y( md->mdRRectY1, md );

    int		h= MD_H( md->mdRRectOvalHigh, md );
    int		w= MD_W( md->mdRRectOvalWide, md );

    if  ( appMacPictSetPenPs( md, ps ) )
	{ LDEB(1); return -1;	}

    if  ( h < 0 )
	{ h= -h;	}
    if  ( w < 0 )
	{ w= -w;	}

    sioOutPrintf( ps->psSos, "gsave [1 0 0 %g 0 %d] concat\n",
					    (double)h/ w, ( y0+ y1 )/ 2 );

    appMetafileRoundRectPathPs( ps, x0, y0, x1, y1, w, h, "closepath\n" );

    appMacPict_FillPath( ps, md );

    sioOutPrintf( ps->psSos, "grestore\n" );

    return 0;
    }

static int appMacPictEraseRRectPs(	MacpictDevice *		md,
					PrintingState *		ps )
    {
    int		x0= MD_X( md->mdRRectX0, md );
    int		y0= MD_Y( md->mdRRectY0, md );
    int		x1= MD_X( md->mdRRectX1, md );
    int		y1= MD_Y( md->mdRRectY1, md );

    int		h= MD_H( md->mdRRectOvalHigh, md );
    int		w= MD_W( md->mdRRectOvalWide, md );

    if  ( appMacPictSetBackPs( md, ps ) )
	{ LDEB(1); return -1;	}

    if  ( h < 0 )
	{ h= -h;	}
    if  ( w < 0 )
	{ w= -w;	}

    sioOutPrintf( ps->psSos, "gsave [1 0 0 %g 0 %d] concat\n",
					    (double)h/ w, ( y0+ y1 )/ 2 );

    appMetafileRoundRectPathPs( ps, x0, y0, x1, y1, w, h, "closepath\n" );

    appMacPict_FillPath( ps, md );

    sioOutPrintf( ps->psSos, "grestore\n" );

    return 0;
    }

/************************************************************************/
/*									*/
/*  PaintPoly/FramePoly variants.					*/
/*									*/
/************************************************************************/

static int appMacPictPaintPolyPs(	MacpictDevice *		md,
					PrintingState *		ps )
    {
    if  ( appMacPictSetPenPs( md, ps ) )
	{ LDEB(1); return -1;	}

    appMetafilePolygonPathPs( ps,
			md->mdPolyPoints, md->mdPolyPointCount,
			"closepath\n" );

    appMacPict_FillPath( ps, md );

    return 0;
    }

static int appMacPictFramePolyPs(	MacpictDevice *		md,
					PrintingState *		ps )
    {
    if  ( appMacPictSetPenPs( md, ps ) )
	{ LDEB(1); return -1;	}

    appMetafilePolygonPathPs( ps,
			md->mdPolyPoints, md->mdPolyPointCount,
			"stroke\n" );

    return 0;
    }

/************************************************************************/
/*									*/
/*  PaintOval/FrameOval variants.					*/
/*									*/
/************************************************************************/

static int appMacPictPaintOvalPs(	MacpictDevice *		md,
					PrintingState *		ps )
    {
    int		swap;

    int		x0= MD_X( md->mdOvalX0, md );
    int		y0= MD_Y( md->mdOvalY0, md );
    int		x1= MD_X( md->mdOvalX1, md );
    int		y1= MD_Y( md->mdOvalY1, md );

    int		xs= x1;
    int		ys= 0;

    double	as= 0.0;
    double	ae= 360.0;

    if  ( x1 < x0 )
	{ swap= x0; x0= x1; x1= swap; }
    if  ( y1 < y0 )
	{ swap= y0; y0= y1; y1= swap; }

    if  ( appMacPictSetPenPs( md, ps ) )
	{ LDEB(1); return -1;	}

    xs= x1;

    sioOutPrintf( ps->psSos, "gsave [1 0 0 %g 0 %d] concat\n",
			(double)(y0- y1)/(double)(x1- x0), ( y0+ y1 )/ 2 );

    sioOutPrintf( ps->psSos, "%d %d bp ", xs, ys );

    sioOutPrintf( ps->psSos, "%d 0 %d %f %f arc stroke\n",
				    ( x0+ x1 )/ 2, (x1- x0)/2, as, ae );

    appMacPict_FillPath( ps, md );

    sioOutPrintf( ps->psSos, "grestore\n" );

    return 0;
    }

static int appMacPictFrameOvalPs(	MacpictDevice *		md,
					PrintingState *		ps )
    {
    int		swap;

    int		x0= MD_X( md->mdOvalX0, md );
    int		y0= MD_Y( md->mdOvalY0, md );
    int		x1= MD_X( md->mdOvalX1, md );
    int		y1= MD_Y( md->mdOvalY1, md );

    int		xs= x1;
    int		ys= 0;

    double	as= 0.0;
    double	ae= 360.0;

    if  ( x1 < x0 )
	{ swap= x0; x0= x1; x1= swap; }
    if  ( y1 < y0 )
	{ swap= y0; y0= y1; y1= swap; }

    if  ( appMacPictSetPenPs( md, ps ) )
	{ LDEB(1); return -1;	}

    xs= x1;

    sioOutPrintf( ps->psSos, "gsave [1 0 0 %g 0 %d] concat\n",
			(double)(y0- y1)/(double)(x1- x0), ( y0+ y1 )/ 2 );

    sioOutPrintf( ps->psSos, "%d %d bp ", xs, ys );

    sioOutPrintf( ps->psSos, "%d 0 %d %f %f arc stroke\n",
				    ( x0+ x1 )/ 2, (x1- x0)/2, as, ae );

    sioOutPrintf( ps->psSos, "grestore\n" );

    return 0;
    }

static int appMacPictFrameArcPs(	MacpictDevice *		md,
					PrintingState *		ps )
    {
    int		swap;

    int		x0= MD_X( md->mdArcX0, md );
    int		y0= MD_Y( md->mdArcY0, md );
    int		x1= MD_X( md->mdArcX1, md );
    int		y1= MD_Y( md->mdArcY1, md );

    int		xs;
    int		ys;

    double	as= 90.0- md->mdArcA0;
    double	ae= 90.0- ( md->mdArcA0+ md->mdArcA1 );
    double	aer;

    if  ( x1 < x0 )
	{ swap= x0; x0= x1; x1= swap; }
    if  ( y1 < y0 )
	{ swap= y0; y0= y1; y1= swap; }

    if  ( appMacPictSetPenPs( md, ps ) )
	{ LDEB(1); return -1;	}

    aer= ( M_PI* ae )/ 180.0;
    xs= (x0+ x1)/ 2+ 0.5* (x1- x0)* cos( aer );
    ys=              0.5* (x1- x0)* sin( aer );

    sioOutPrintf( ps->psSos, "gsave [1 0 0 %g 0 %d] concat\n",
			(double)(y0- y1)/(double)(x1- x0), ( y0+ y1 )/ 2 );

    sioOutPrintf( ps->psSos, "%d %d bp ", xs, ys );

    sioOutPrintf( ps->psSos, "%d 0 %d %f %f arc stroke\n",
				    ( x0+ x1 )/ 2, (x1- x0)/2, ae, as );

    sioOutPrintf( ps->psSos, "grestore\n" );

    return 0;
    }

static int appMacPictPaintArcPs(	MacpictDevice *		md,
					PrintingState *		ps )
    {
    int		swap;

    int		x0= MD_X( md->mdArcX0, md );
    int		y0= MD_Y( md->mdArcY0, md );
    int		x1= MD_X( md->mdArcX1, md );
    int		y1= MD_Y( md->mdArcY1, md );

    int		xs;
    int		ys;

    double	as= 90.0- md->mdArcA0;
    double	ae= 90.0- ( md->mdArcA0+ md->mdArcA1 );
    double	aer;

    if  ( x1 < x0 )
	{ swap= x0; x0= x1; x1= swap; }
    if  ( y1 < y0 )
	{ swap= y0; y0= y1; y1= swap; }

    if  ( appMacPictSetPenPs( md, ps ) )
	{ LDEB(1); return -1;	}

    aer= ( M_PI* ae )/ 180.0;
    xs= (x0+ x1)/ 2+ 0.5* (x1- x0)* cos( aer );
    ys=              0.5* (x1- x0)* sin( aer );

    sioOutPrintf( ps->psSos, "gsave [1 0 0 %g 0 %d] concat\n",
			(double)(y0- y1)/(double)(x1- x0), ( y0+ y1 )/ 2 );

    sioOutPrintf( ps->psSos, "%d %d bp ", xs, ys );

    sioOutPrintf( ps->psSos, "%d 0 %d %f %f arc\n",
				    ( x0+ x1 )/ 2, (x1- x0)/2, ae, as );

    appMacPict_FillPath( ps, md );

    sioOutPrintf( ps->psSos, "grestore\n" );

    return 0;
    }

/************************************************************************/
/*									*/
/*  MACPICT_PackBitsRect						*/
/*									*/
/*  1)  Initialise bitmap.						*/
/*  2)  Get image.							*/
/*  12) Draw image.							*/
/*  13) Ready: clean image related data.				*/
/*									*/
/************************************************************************/

static int appMacPictDrawImagePs(	MacpictDevice *		md,
					PrintingState *		ps,
					SimpleInputStream *	sis,
					int			useFilters,
					int			indexedImages,
					int			packed,
					int			direct,
					int			clipRegion )
    {
    int			rval= 0;

    int			y0Source;
    int			x0Source;
    int			y1Source;
    int			x1Source;

    int			y0Dest;
    int			x0Dest;
    int			y1Dest;
    int			x1Dest;

    AppBitmapImage	abi;
    BitmapDescription *	bd;

    int			bytesRead= 0;

    /*  1  */
    appInitBitmapImage( &abi );

    /*  2  */
    if  ( appMacPictGetImage( &abi,
			&y0Source, &x0Source, &y1Source, &x1Source,
			&y0Dest, &x0Dest, &y1Dest, &x1Dest,
			md, sis, packed, direct, clipRegion ) )
	{ LDEB(1); rval= -1; goto ready;	}

    bd= &(abi.abiBitmap);

    /*  11  */
    if  ( md->mdVersion > 1 && bytesRead % 2 )
	{ (void)sioInGetCharacter( sis );	}

    /*  12  */
    {
    int			pixelsWide= x1Dest- x0Dest;
    int			pixelsHigh= y1Dest- y0Dest;

    double		twipsWide;
    double		twipsHigh;

    int			dstX= MD_X( x0Dest, md );
    int			dstY= MD_Y( y0Dest, md );

    twipsWide= MD_W( pixelsWide, md );
    twipsHigh= MD_H( pixelsHigh, md );

    sioOutPrintf( ps->psSos, "gsave 1 setgray\n" );
    utilPsFillRectangle( ps, dstX, dstY, twipsWide, twipsHigh );
    sioOutPrintf( ps->psSos, "grestore\n" );

    if  ( bmPsPrintBitmapImage( ps->psSos, 1,
			    twipsWide, -twipsHigh,
			    dstX, ( dstY+ twipsHigh ), 0, 0,
			    bd->bdPixelsWide, bd->bdPixelsHigh,
			    useFilters, indexedImages,
			    bd, abi.abiBuffer ) )
	{ LDEB(1); rval= -1;	}
    }

  ready:

    /*  13  */
    appCleanBitmapImage( &abi );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Print a macintosh pict file.					*/
/*									*/
/************************************************************************/

int appMacPictPlayFilePs( PrintingState *		ps,
			SimpleInputStream *		sis,
			const PostScriptFontList *	psfl,
			int				useFilters,
			int				indexedImages,
			int				mapMode,
			int				xWinExt,
			int				yWinExt,
			int				twipsWide,
			int				twipsHigh )
    {
    int			c;
    int			i;

    int			rval= 0;

    MacpictDevice	md;


    appMacPictInitDeviceHeader( &md, twipsWide, twipsHigh,
						    twipsWide, twipsHigh );

    if  ( appMacPictGetDeviceHeader( &md, sis ) )
	{ LDEB(1); return -1;	}

    PICTDEB(appDebug( "\n" ));
    PICTDEB(appDebug( "MACPICT PS( %d .. %d x %d .. %d -> %d x %d ) VERSION %d\n",
		    md.mdInX0, md.mdInX1, md.mdInY0, md.mdInY1,
		    md.mdOutWide, md.mdOutHigh, md.mdVersion ));

    md.mdColorSet.rgb8Red= 0;
    md.mdColorSet.rgb8Green= 0;
    md.mdColorSet.rgb8Blue= 0;
    md.mdPostScriptFontList= psfl;

    sioOutPrintf( ps->psSos, "0 setgray\n" );

    for (;;)
	{
	unsigned int	opcode;
	int		bytes;

	if  ( appMacPictGetOpcode( &opcode, &bytes, &md, sis ) )
	    { LDEB(1); rval= -1; goto ready;	}

	switch( opcode )
	    {
	    case MACPICT_NOP:
		PICTDEB(appDebug( "NOP()\n" ));
		continue;

	    case MACPICT_PnSize:
		{
		int			h;
		int			w;

		h= sioEndianGetBeInt16( sis );
		w= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "PnSize( %d, %d )\n", w, h ));
		}
		continue;

	    case MACPICT_OvSize:
		if  ( appMacPictGetOvSize( &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		continue;

	    case MACPICT_DefHilite:
		PICTDEB(appDebug( "DefHilite()\n" ));
		continue;

	    case MACPICT_Clip:
		{
		int	regionBytes= sioEndianGetBeInt16( sis );
		int	i;

		PICTDEB(appDebug( "Clip( %d )\n", regionBytes/ 2 ));

		for ( i= 2; i < regionBytes; i += 2 )
		    {
		    int		regionCoord;

		    regionCoord= sioEndianGetBeInt16( sis );
		    }
		}
		continue;

	    case MACPICT_PnMode:
		md.mdPenMode= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "PnMode(%d)\n", md.mdPenMode ));

		continue;

	    case MACPICT_PnLocHFrac:
		{
		int	val;

		val= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "PnLocHFrac(%d)\n", val ));

		}
		continue;

	    case MACPICT_RGBFgCol:
		if  ( appMacPictGetColor( &(md.mdForeColor), &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug( "RGBFgCol(%d,%d,%d)\n",
					md.mdForeColor.rgb8Red,
					md.mdForeColor.rgb8Green,
					md.mdForeColor.rgb8Blue ));

		md.mdStippleSet= STIPPLE_UNDEF;

		continue;

	    case MACPICT_RGBBkCol:
		if  ( appMacPictGetColor( &(md.mdBackColor), &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug( "RGBBkCol(%d,%d,%d)\n",
					md.mdBackColor.rgb8Red,
					md.mdBackColor.rgb8Green,
					md.mdBackColor.rgb8Blue ));

		md.mdStippleSet= STIPPLE_UNDEF;

		continue;

	    case MACPICT_PnPat:
		if  ( appMacPictGetPattern( &md, sis, STIPPLE_PEN,
				    &(md.mdPenIsSolid), md.mdPenPattern ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug(
			"PnPat(%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x)\n",
					    md.mdPenPattern[0],
					    md.mdPenPattern[1],
					    md.mdPenPattern[2],
					    md.mdPenPattern[3],
					    md.mdPenPattern[4],
					    md.mdPenPattern[4],
					    md.mdPenPattern[6],
					    md.mdPenPattern[7] ));

		continue;

	    case MACPICT_FillPat:
		if  ( appMacPictGetPattern( &md, sis, STIPPLE_FILL,
				    &(md.mdFillIsSolid), md.mdFillPattern ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug(
			"FillPat(%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x)\n",
					    md.mdFillPattern[0],
					    md.mdFillPattern[1],
					    md.mdFillPattern[2],
					    md.mdFillPattern[3],
					    md.mdFillPattern[4],
					    md.mdFillPattern[4],
					    md.mdFillPattern[6],
					    md.mdFillPattern[7] ));

		continue;

	    case MACPICT_BkPat:
		if  ( appMacPictGetPattern( &md, sis, STIPPLE_BACK,
				    &(md.mdBackIsSolid), md.mdBackPattern ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug(
			"BkPat(%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x)\n",
					    md.mdBackPattern[0],
					    md.mdBackPattern[1],
					    md.mdBackPattern[2],
					    md.mdBackPattern[3],
					    md.mdBackPattern[4],
					    md.mdBackPattern[4],
					    md.mdBackPattern[6],
					    md.mdBackPattern[7] ));

		continue;

	    case MACPICT_TxRatio:
		{
		int			y0;
		int			x0;
		int			y1;
		int			x1;

		y0= sioEndianGetBeInt16( sis );
		x0= sioEndianGetBeInt16( sis );
		y1= sioEndianGetBeInt16( sis );
		x1= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "TxRatio( %d, %d, %d, %d )\n",
							x0, y0, x1, y1 ));
		}
		continue;

	    case MACPICT_FrameOval:
		{
		if  ( appMacPictGetOval( &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug( "FrameOval( %d, %d, %d, %d )\n",
						md.mdOvalX0, md.mdOvalY0,
						md.mdOvalX1, md.mdOvalY1 ));

		if  ( appMacPictFrameOvalPs( &md, ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FrameSameOval:
		{
		PICTDEB(appDebug( "FrameSameOval()\n" ));

		if  ( appMacPictFrameOvalPs( &md, ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PaintOval:
		{
		if  ( appMacPictGetOval( &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug( "PaintOval( %d, %d, %d, %d )\n",
						md.mdOvalX0, md.mdOvalY0,
						md.mdOvalX1, md.mdOvalY1 ));

		if  ( appMacPictPaintOvalPs( &md, ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PaintSameOval:
		{
		PICTDEB(appDebug( "PaintSameOval()\n" ));

		if  ( appMacPictPaintOvalPs( &md, ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PaintPoly:
		{
		if  ( appMacPictGetPoly( &md, sis ) )
		    { rval= -1; goto ready;	}

		PICTDEB(appDebug( "PaintPoly(,%d)\n", md.mdPolyPointCount ));

		if  ( appMacPictPaintPolyPs( &md, ps ) )
		    { LDEB(1); return -1;	}
		}
		continue;

	    case MACPICT_FramePoly:
		{
		if  ( appMacPictGetPoly( &md, sis ) )
		    { rval= -1; goto ready;	}

		PICTDEB(appDebug( "FramePoly(,%d)\n", md.mdPolyPointCount ));

		if  ( appMacPictFramePolyPs( &md, ps ) )
		    { LDEB(1); return -1;	}
		}
		continue;

	    case MACPICT_OpColor:
		{
		md.mdRedMax= sioEndianGetBeUint16( sis );
		md.mdGreenMax= sioEndianGetBeUint16( sis );
		md.mdBlueMax= sioEndianGetBeUint16( sis );

		PICTDEB(appDebug( "OpColor(%u,%u,%u)\n",
				md.mdRedMax, md.mdGreenMax, md.mdBlueMax ));
		}
		continue;

	    case MACPICT_ShortLine:
		{
		int			x0;
		int			y0;
		int			x1;
		int			y1;

		if  ( appMacPictGetShortLine( &x0, &y0, &x1, &y1, &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( appMacPictDrawLinePs( &md, ps, x0, y0, x1, y1 ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_ShortLineFrom:
		{
		int			y0;
		int			x0;
		int			y1;
		int			x1;

		if  ( appMacPictGetShortLineFrom( &x0, &y0, &x1, &y1,
								&md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( appMacPictDrawLinePs( &md, ps, x0, y0, x1, y1 ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_LineFrom:
		{
		int			y0;
		int			x0;
		int			y1;
		int			x1;

		if  ( appMacPictGetLineFrom( &x0, &y0, &x1, &y1, &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( appMacPictDrawLinePs( &md, ps, x0, y0, x1, y1 ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_Line:
		{
		int			y0;
		int			x0;
		int			y1;
		int			x1;

		if  ( appMacPictGetLine( &x0, &y0, &x1, &y1, &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( appMacPictDrawLinePs( &md, ps, x0, y0, x1, y1 ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FrameRRect:
		{
		md.mdRRectY0= sioEndianGetBeInt16( sis );
		md.mdRRectX0= sioEndianGetBeInt16( sis );
		md.mdRRectY1= sioEndianGetBeInt16( sis );
		md.mdRRectX1= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "FrameRRect( %d, %d, %d, %d )\n",
						md.mdRRectX0, md.mdRRectY0,
						md.mdRRectX1, md.mdRRectY1 ));

		if  ( appMacPictFrameRRectPs( &md, ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FrameSameRRect:
		{
		PICTDEB(appDebug( "FrameSameRRect()\n" ));

		if  ( appMacPictFrameRRectPs( &md, ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PaintRRect:
		{
		md.mdRRectY0= sioEndianGetBeInt16( sis );
		md.mdRRectX0= sioEndianGetBeInt16( sis );
		md.mdRRectY1= sioEndianGetBeInt16( sis );
		md.mdRRectX1= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "PaintRRect( %d, %d, %d, %d )\n",
						md.mdRRectX0, md.mdRRectY0,
						md.mdRRectX1, md.mdRRectY1 ));

		if  ( appMacPictPaintRRectPs( &md, ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PaintSameRRect:
		{
		PICTDEB(appDebug( "PaintSameRRect()\n" ));

		if  ( appMacPictPaintRRectPs( &md, ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FillRRect:
		{
		md.mdRRectY0= sioEndianGetBeInt16( sis );
		md.mdRRectX0= sioEndianGetBeInt16( sis );
		md.mdRRectY1= sioEndianGetBeInt16( sis );
		md.mdRRectX1= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "FillRRect( %d, %d, %d, %d )\n",
						md.mdRRectX0, md.mdRRectY0,
						md.mdRRectX1, md.mdRRectY1 ));

		if  ( appMacPictFillRRectPs( &md, ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FillSameRRect:
		{
		PICTDEB(appDebug( "FillSameRRect()\n" ));

		if  ( appMacPictFillRRectPs( &md, ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_EraseRRect:
		{
		md.mdRRectY0= sioEndianGetBeInt16( sis );
		md.mdRRectX0= sioEndianGetBeInt16( sis );
		md.mdRRectY1= sioEndianGetBeInt16( sis );
		md.mdRRectX1= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "EraseRRect( %d, %d, %d, %d )\n",
						md.mdRRectX0, md.mdRRectY0,
						md.mdRRectX1, md.mdRRectY1 ));

		if  ( appMacPictEraseRRectPs( &md, ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_EraseSameRRect:
		{
		PICTDEB(appDebug( "EraseSameRRect()\n" ));

		if  ( appMacPictEraseRRectPs( &md, ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FrameRect:
		{
		md.mdRectY0= sioEndianGetBeInt16( sis );
		md.mdRectX0= sioEndianGetBeInt16( sis );
		md.mdRectY1= sioEndianGetBeInt16( sis );
		md.mdRectX1= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "FrameRect( %d, %d, %d, %d )\n",
						md.mdRectX0, md.mdRectY0,
						md.mdRectX1, md.mdRectY1 ));

		if  ( appMacPictFrameRectPs( &md, ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FrameSameRect:
		{
		PICTDEB(appDebug( "FrameSameRect()\n" ));

		if  ( appMacPictFrameRectPs( &md, ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PaintRect:
		{
		md.mdRectY0= sioEndianGetBeInt16( sis );
		md.mdRectX0= sioEndianGetBeInt16( sis );
		md.mdRectY1= sioEndianGetBeInt16( sis );
		md.mdRectX1= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "PaintRect( %d, %d, %d, %d )\n",
						md.mdRectX0, md.mdRectY0,
						md.mdRectX1, md.mdRectY1 ));

		if  ( appMacPictPaintRectPs( &md, ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PaintSameRect:
		{
		PICTDEB(appDebug( "PaintSameRect()\n" ));

		if  ( appMacPictPaintRectPs( &md, ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FillRect:
		{
		md.mdRectY0= sioEndianGetBeInt16( sis );
		md.mdRectX0= sioEndianGetBeInt16( sis );
		md.mdRectY1= sioEndianGetBeInt16( sis );
		md.mdRectX1= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "FillRect( %d, %d, %d, %d )\n",
						md.mdRectX0, md.mdRectY0,
						md.mdRectX1, md.mdRectY1 ));

		if  ( appMacPictFillRectPs( &md, ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FillSameRect:
		{
		PICTDEB(appDebug( "FillSameRect()\n" ));

		if  ( appMacPictFillRectPs( &md, ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_EraseRect:
		{
		md.mdRectY0= sioEndianGetBeInt16( sis );
		md.mdRectX0= sioEndianGetBeInt16( sis );
		md.mdRectY1= sioEndianGetBeInt16( sis );
		md.mdRectX1= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "EraseRect( %d, %d, %d, %d )\n",
						md.mdRectX0, md.mdRectY0,
						md.mdRectX1, md.mdRectY1 ));

		if  ( appMacPictEraseRectPs( &md, ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_EraseSameRect:
		{
		PICTDEB(appDebug( "EraseSameRect()\n" ));

		if  ( appMacPictEraseRectPs( &md, ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FrameArc:
		{
		if  ( appMacPictGetArc( &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug( "FrameArc( %d, %d, %d, %d, %d, %d )\n",
						    md.mdArcX0, md.mdArcY0,
						    md.mdArcX1, md.mdArcY1,
						    md.mdArcA0, md.mdArcA1 ));

		if  ( appMacPictFrameArcPs( &md, ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PaintArc:
		{
		if  ( appMacPictGetArc( &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		PICTDEB(appDebug( "PaintArc( %d, %d, %d, %d, %d, %d )\n",
						    md.mdArcX0, md.mdArcY0,
						    md.mdArcX1, md.mdArcY1,
						    md.mdArcA0, md.mdArcA1 ));

		if  ( appMacPictPaintArcPs( &md, ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FontName:
		if  ( appMacPictGetFontName( &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		continue;

	    case MACPICT_TxFont:
		if  ( appMacPictGetTxFont( &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		continue;

	    case MACPICT_TxFace:
		if  ( appMacPictGetTxFace( &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		continue;

	    case MACPICT_TxSize:
		if  ( appMacPictGetTxSize( &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		continue;

	    case MACPICT_GlyphState:
		if  ( appMacPictGetGlyphState( &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}
		continue;

	    case MACPICT_DHText:
		{
		int	count;

		if  ( appMacPictGetDHText( &count, &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( appMacPictDrawStringPs( &md, psfl, ps,
					    md.mdPenX, md.mdPenY, count ) )
		    { LDEB(count); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_DVText:
		{
		int	count;

		if  ( appMacPictGetDVText( &count, &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( appMacPictDrawStringPs( &md, psfl, ps,
					    md.mdPenX, md.mdPenY, count ) )
		    { LDEB(count); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_DHDVText:
		{
		int	count;

		if  ( appMacPictGetDHDVText( &count, &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( appMacPictDrawStringPs( &md, psfl, ps,
					    md.mdPenX, md.mdPenY, count ) )
		    { LDEB(count); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_LongText:
		{
		int	count;

		if  ( appMacPictGetLongText( &count, &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( appMacPictDrawStringPs( &md, psfl, ps,
					    md.mdPenX, md.mdPenY, count ) )
		    { LDEB(count); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_LongComment:
		{
		int	kind= sioEndianGetBeInt16( sis );
		int	size= sioEndianGetBeInt16( sis );

		kind= kind;

		switch( kind )
		    {
		    case PICTCMT_TextBegin:
			{
			int	justify;
			int	flip;
			int	angle;
			int	line;
			int	cmnt;
			int	angleFixed;

			if  ( size != 12 )
			    { goto defaultCase;	}

			justify= sioEndianGetBeInt16( sis );
			flip= sioEndianGetBeInt16( sis );
			angle= sioEndianGetBeInt16( sis );
			line= sioEndianGetBeInt16( sis );
			cmnt= sioEndianGetBeInt16( sis );
			angleFixed= sioEndianGetBeInt16( sis );

			PICTDEB(appDebug(
			    "LongComment( %s, j=%d, f=%d, a=%d, af=%d )\n",
				appMacPictCommentName( kind ),
				justify, flip, angle, angleFixed ));
			}
			break;

		    default:
		    defaultCase:
			PICTDEB(appDebug( "LongComment( %d:%s, %d, .. )\n",
				kind, appMacPictCommentName( kind ), size ));
			for ( i= 0; i < size; i++ )
			    { c= sioInGetCharacter( sis );	}
			break;
		    }

		if  ( md.mdVersion > 1 && size % 2 )
		    { (void)sioInGetCharacter( sis );	}
		}
		continue;

	    case MACPICT_ShortComment:
		{
		int	kind= sioEndianGetBeInt16( sis );

		PICTDEB(appDebug( "ShortComment( %d:%s )\n",
				kind, appMacPictCommentName( kind ) ));

		kind= kind;
		}
		continue;

	    case MACPICT_FrameRgn:
		{
		int	regionBytes= sioEndianGetBeInt16( sis );
		int	i;

		PICTDEB(appDebug( "FrameRgn( %d )\n", regionBytes ));

		for ( i= 2; i < regionBytes; i++ )
		    {
		    int		regionCoord;

		    regionCoord= sioInGetCharacter( sis );
		    }
		}
		continue;

	    case MACPICT_PaintRgn:
		{
		int	regionBytes= sioEndianGetBeInt16( sis );
		int	i;

		PICTDEB(appDebug( "PaintRgn( %d )\n", regionBytes ));

		for ( i= 2; i < regionBytes; i++ )
		    {
		    int		regionCoord;

		    regionCoord= sioInGetCharacter( sis );
		    }
		}
		continue;

	    case MACPICT_BitsRect:
		{
		const int	packed= 0;
		const int	direct= 0;
		const int	clipRegion= 0;

		PICTDEB(appDebug( "BitsRect( .. )\n" ));
		PICTLOG(sioOutPrintf( ps->psSos, "%%%%BitsRect( .. )\n" ));

		if  ( appMacPictDrawImagePs( &md, ps, sis,
						useFilters, indexedImages,
						packed, direct, clipRegion ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PackBitsRect:
		{
		const int	packed= 1;
		const int	direct= 0;
		const int	clipRegion= 0;

		PICTDEB(appDebug( "PackBitsRect( .. )\n" ));
		PICTLOG(sioOutPrintf( ps->psSos, "%%%%PackBitsRect( .. )\n" ));

		if  ( appMacPictDrawImagePs( &md, ps, sis,
						useFilters, indexedImages,
						packed, direct, clipRegion ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PackBitsRgn:
		{
		const int	packed= 1;
		const int	direct= 0;
		const int	clipRegion= 1;

		PICTDEB(appDebug( "PackBitsRgn( .. )\n" ));
		PICTLOG(sioOutPrintf( ps->psSos, "%%%%PackBitsRgn( .. )\n" ));

		if  ( appMacPictDrawImagePs( &md, ps, sis,
						useFilters, indexedImages,
						packed, direct, clipRegion ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_DirectBitsRect:
		{
		const int	packed= 0;
		const int	direct= 1;
		const int	clipRegion= 0;

		PICTDEB(appDebug( "DirectBitsRect( .. )\n" ));
		PICTLOG(sioOutPrintf( ps->psSos, "%%%%DirectBitsRect( .. )\n" ));

		if  ( appMacPictDrawImagePs( &md, ps, sis,
						useFilters, indexedImages,
						packed, direct, clipRegion ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_DirectBitsRgn:
		{
		const int	packed= 0;
		const int	direct= 1;
		const int	clipRegion= 1;

		PICTDEB(appDebug( "DirectBitsRgn( .. )\n" ));
		PICTLOG(sioOutPrintf( ps->psSos, "%%%%DirectBitsRgn( .. )\n" ));

		if  ( appMacPictDrawImagePs( &md, ps, sis,
						useFilters, indexedImages,
						packed, direct, clipRegion ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_CompressedQuickTime:
		{
		long	qtbytes= sioEndianGetBeInt32( sis );
		int	i;

		PICTDEB(appDebug( "CompressedQuickTime( %ld )\n",
							    qtbytes ));

		for ( i= 0; i < qtbytes; i++ )
		    {
		    int		qtdata;

		    qtdata= sioInGetCharacter( sis );
		    }

		}
		continue;

	    case MACPICT_UncompressedQuickTime:
		{
		long	qtbytes= sioEndianGetBeInt32( sis );
		int	i;

		PICTDEB(appDebug( "UncompressedQuickTime( %ld )\n",
							    qtbytes ));

		for ( i= 0; i < qtbytes; i++ )
		    {
		    int		qtdata;

		    qtdata= sioInGetCharacter( sis );
		    }

		}
		continue;

	    case 0x66:
		XLDEB(opcode,bytes);
		bytes= 12;
		goto skipBytes;

	    default:
	    skipBytes:
		if  ( bytes >= 0 )
		    {
		    XLDEB(opcode,bytes);

		    for ( i= 0; i < bytes; i++ )
			{ c= sioInGetCharacter( sis );	}

		    continue;
		    }
		XDEB(opcode); rval= -1; goto ready;

	    case MACPICT_OpEndPic:
		PICTDEB(appDebug( "OpEndPic()\n" ));
		break;
	    }

	break;
	}

  ready:

    appMacPictCleanDeviceHeader( &md );

    return rval;
    }

