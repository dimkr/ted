#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<appWinMeta.h>
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
#    define	WMFDEB(x)	(x)
# else
#    define	WMFDEB(x)	/*nothing*/
# endif

# if 0
#    define	WMFLOG(x)	(x)
# else
#    define	WMFLOG(x)	/*nothing*/
# endif

/************************************************************************/
/*									*/
/*  Play a windows metafile to PostScript.				*/
/*									*/
/************************************************************************/

static void appMetaSetColorPs(		PrintingState *		ps,
					DeviceContext *		dc,
					RGB8Color *		rgb )
    {
    if  ( dc->dcRedSet == rgb->rgb8Red		&&
	  dc->dcGreenSet == rgb->rgb8Green	&&
	  dc->dcBlueSet == rgb->rgb8Blue	)
	{ return;	}

    utilPsSetRgbColor( ps, rgb->rgb8Red/255.0,
				    rgb->rgb8Green/255.0,
				    rgb->rgb8Blue/255.0 );

    dc->dcRedSet= rgb->rgb8Red;
    dc->dcGreenSet= rgb->rgb8Green;
    dc->dcBlueSet= rgb->rgb8Blue;

    return;
    }

static void appMetaSetFontPs(		PrintingState *		ps,
					DeviceContext *		dc,
					TextAttribute		ta )
    {
    if  ( docEqualTextAttributes( &ta, &(dc->dcTextAttributeSet) ) )
	{ return;	}

    utilPsSetFont( ps, "pf", &ta );

    dc->dcTextAttributeSet= ta;
    }

static void appMetaSetPenPs(		PrintingState *		ps,
					DeviceContext *		dc,
					const LogicalPen *	lp )
    {
    SimpleOutputStream *	sos= ps->psSos;

    if  ( dc->dcPenSet.lpStyle == lp->lpStyle		&&
	  dc->dcPenSet.lpWidth == lp->lpWidth		)
	{ return;	}

    switch( lp->lpStyle )
	{
	case PS_SOLID:
	  solid:
	    sioOutPrintf( sos, "%d setlinewidth ",
					DC_wViewport( lp->lpWidth, dc ) );
	    sioOutPrintf( sos, "[] 0 setdash\n" );
	    break;

	case PS_INSIDEFRAME:
	    sioOutPrintf( sos, "%d setlinewidth ",
					DC_wViewport( lp->lpWidth, dc ) );
	    sioOutPrintf( sos, "[] 0 setdash\n" );
	    break;

	case PS_DASH:
	    if  ( lp->lpWidth > 1 )
		{ goto solid;	}

	    sioOutPrintf( sos, "10 setlinewidth " );
	    sioOutPrintf( sos, "[60 40] 0 setdash\n" );
	    break;

	case PS_DOT:
	    if  ( lp->lpWidth > 1 )
		{ goto solid;	}

	    sioOutPrintf( sos, "10 setlinewidth " );
	    sioOutPrintf( sos, "[10 20] 0 setdash\n" );
	    break;

	case PS_DASHDOT:
	    if  ( lp->lpWidth > 1 )
		{ goto solid;	}

	    sioOutPrintf( sos, "10 setlinewidth " );
	    sioOutPrintf( sos, "[60 20 10 20] 0 setdash\n" );
	    break;

	case PS_DASHDOTDOT:
	    if  ( lp->lpWidth > 1 )
		{ goto solid;	}

	    sioOutPrintf( sos, "10 setlinewidth " );
	    sioOutPrintf( sos, "[60 20 10 20 10 20] 0 setdash\n" );

	    break;

	case PS_NULL:
	    break;

	default:
	    WMFDEB(appDebug( "style= %d\n", lp->lpStyle ));
	    WMFLOG(sioOutPrintf( sos, "style= %d\n", lp->lpStyle ));
	    break;
	}

    dc->dcPenSet= *lp;

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a bitmap image in the PostScript output for a metafile.	*/
/*									*/
/************************************************************************/

static int appMetaBitmapImagePs(	SimpleInputStream *	sis,
					PrintingState *		ps,
					int			expectBytes,
					int			useFilters,
					int			indexedImages,
					DeviceContext *		dc,
					int			srcX,
					int			srcY,
					int			srcXExt,
					int			srcYExt,
					int			dstX,
					int			dstY,
					int			dstXExt,
					int			dstYExt )
    {
    SimpleOutputStream *	sos= ps->psSos;
    int				rval= 0;

    AppBitmapImage *		abi;
    BitmapDescription *		bd;
    double			twipsWide;
    double			twipsHigh;

    if  ( srcX != 0 || srcY != 0 )
	{ LLDEB(srcX,srcY); return 0;	}

    if  ( appMetaBitmapImage( &abi, sis, expectBytes ) )
	{ LDEB(expectBytes); return -1;	}

    bd= &(abi->abiBitmap);

    if  ( srcXExt != bd->bdPixelsWide	||
	  srcYExt != bd->bdPixelsHigh	)
	{
	LLDEB(srcXExt,bd->bdPixelsWide);
	LLDEB(srcYExt,bd->bdPixelsHigh);
	appCleanBitmapImage( abi ); free( abi );
	return 0;
	}

    twipsWide= DC_wViewport( dstXExt, dc );
    twipsHigh= DC_hViewport( dstYExt, dc );

    if  ( twipsWide < 0 )
	{ twipsWide= -twipsWide;	}
    if  ( twipsHigh < 0 )
	{ twipsHigh= -twipsHigh;	}

    dstX= DC_xViewport( dstX, dc );
    dstY= DC_yViewport( dstY, dc );

    sioOutPrintf( sos, "gsave 1 setgray\n" );
    sioOutPrintf( sos, "%d %d %g %g rectfill\n", dstX, dstY,
						    twipsWide, twipsHigh );
    sioOutPrintf( sos, "grestore\n" );

    if  ( bmPsPrintBitmapImage( sos, 1,
			    twipsWide, -twipsHigh,
			    dstX, ( dstY+ twipsHigh ), 0, 0,
			    bd->bdPixelsWide, bd->bdPixelsHigh,
			    useFilters, indexedImages, bd, abi->abiBuffer ) )
	{ LDEB(1); rval= -1;	}

    appCleanBitmapImage( abi ); free( abi );

    return rval;
    }

static int appMetaSelectPenObjectPs(	DeviceContext *		dc,
					PrintingState *		ps,
					int			ob )
    {
    LogicalPen *	lp= &(dc->dcObjects[ob].mfoLogicalPen);

    WMFDEB(appDebug( "SelectObject( ob= %d ) PEN Width= %d ",
							ob, lp->lpWidth ));

    WMFLOG(sioOutPrintf( sos, "%% SelectObject( ob= %d ) PEN Width= %d ",
							ob, lp->lpWidth ));

    switch( lp->lpStyle )
	{
	case PS_SOLID:
	    WMFDEB(appDebug( "SOLID\n" ));
	    WMFLOG(sioOutPrintf( sos, "SOLID\n" ));
	  solid:
	    dc->dcDrawBorders= 1;
	    break;

	case PS_INSIDEFRAME:
	    WMFDEB(appDebug( "INSIDEFRAME\n" ));
	    WMFLOG(sioOutPrintf( sos, "INSIDEFRAME\n" ));

	    dc->dcDrawBorders= 1;
	    break;

	case PS_DASH:
	    WMFDEB(appDebug( "DASH\n" ));
	    WMFLOG(sioOutPrintf( sos, "DASH\n" ));

	    if  ( lp->lpWidth > 1 )
		{ goto solid;	}

	    dc->dcDrawBorders= 1;
	    break;

	case PS_DOT:
	    WMFDEB(appDebug( "DOT\n" ));
	    WMFLOG(sioOutPrintf( sos, "DOT\n" ));

	    if  ( lp->lpWidth > 1 )
		{ goto solid;	}

	    dc->dcDrawBorders= 1;
	    break;

	case PS_DASHDOT:
	    WMFDEB(appDebug( "DASHDOT\n" ));
	    WMFLOG(sioOutPrintf( sos, "DASHDOT\n" ));

	    if  ( lp->lpWidth > 1 )
		{ goto solid;	}

	    dc->dcDrawBorders= 1;
	    break;

	case PS_DASHDOTDOT:
	    WMFDEB(appDebug( "DASHDOTDOT\n" ));
	    WMFLOG(sioOutPrintf( sos, "DASHDOTDOT\n" ));

	    if  ( lp->lpWidth > 1 )
		{ goto solid;	}

	    dc->dcDrawBorders= 1;
	    break;

	case PS_NULL:
	    WMFDEB(appDebug( "NULL\n" ));
	    WMFLOG(sioOutPrintf( sos, "NULL\n" ));
	    dc->dcDrawBorders= 0;
	    break;

	default:
	    WMFDEB(appDebug( "style= %d\n", lp->lpStyle ));
	    WMFLOG(sioOutPrintf( sos, "style= %d\n", lp->lpStyle ));
	    dc->dcDrawBorders= 0;
	    break;
	}

    dc->dcPen= *lp;

    return 0;
    }

static int appMetaSelectPatternBrushObjectPs(	DeviceContext *		dc,
						PrintingState *		ps,
						int			ob )
    {
    PatternBrush *	pb= &(dc->dcObjects[ob].mfoPatternBrush);

    if  ( appMetafileStartPatternFillPs( ps, &(pb->pbAbi) ) )
	{ LDEB(1); return -1;	}

    dc->dcFillHatched= 0;
    dc->dcFillPattern= 1;
    dc->dcFillInsides= 1;

    return 0;
    }

static int appMetaSelectBrushObjectPs(	DeviceContext *		dc,
					PrintingState *		ps,
					int			ob )
    {
    LogicalBrush *		lb= &(dc->dcObjects[ob].mfoLogicalBrush);

    WMFDEB(appDebug( "SelectObject( ob= %d ) ", ob ));
    WMFDEB(appDebug( "BRUSH\n" ));

    switch( lb->lbStyle )
	{
	case BS_SOLID:
	    dc->dcFillInsides= 1;
	    dc->dcFillHatched= 0;
	    dc->dcFillPattern= 0;
	    break;

	case BS_HOLLOW:
	    dc->dcFillInsides= 0;
	    dc->dcFillHatched= 0;
	    dc->dcFillPattern= 0;
	    break;

	case BS_HATCHED:
	    dc->dcFillInsides= 1;
	    dc->dcFillHatched= 1;
	    dc->dcFillPattern= 0;
	    break;

	case BS_PATTERN:
	default:
	    dc->dcFillInsides= 1;
	    dc->dcFillHatched= 0;
	    dc->dcFillPattern= 0;
	    LDEB(lb->lbStyle);
	    break;
	}

    dc->dcBrush= *lb;

    return 0;
    }

static int appMetaSelectFontObjectPs( DeviceContext *		dc,
				const PostScriptFontList *	psfl,
				int				ob )
    {
    LogicalFont *		lf= &(dc->dcObjects[ob].mfoLogicalFont);

    DocumentFontList *		dfl= &(dc->x_dcFontList);
    DocumentFont *		df;

    df= dfl->dflFonts+ lf->lfTextAttribute.taFontNumber;

    if  ( utilFindPsFontForDocFont( df, dfl, psfl ) )
	{
	LDEB(lf->lfTextAttribute.taFontNumber);
	dc->dcAfi= (AfmFontInfo *)0;
	dc->dcFontEncoding= -1;
	return -1;
	}

    dc->dcAfi= utilPsGetAfi( &(dc->dcFontEncoding), dfl,
					    psfl, &(lf->lfTextAttribute) );
    if  ( ! dc->dcAfi )
	{ XDEB(dc->dcAfi); return -1;	}

    dc->dcFont= *lf;

    return 0;
    }

static int appMetaSelectObjectPs( DeviceContext *		dc,
				const PostScriptFontList *	psfl,
				PrintingState *			ps,
				int				recordSize,
				SimpleInputStream *		sis )
    {
    int			ob;

    ob= sioEndianGetLeInt16( sis );

    if  ( ob < 0 || ob >= dc->dcObjectCount )
	{ LLDEB(ob,dc->dcObjectCount); return -1;	}

    switch( dc->dcObjects[ob].mfoType )
	{
	case MFtypePEN:
	    if  ( appMetaSelectPenObjectPs( dc, ps, ob ) )
		{ LDEB(ob); return -1;	}
	    break;

	case MFtypeBRUSH:
	    if  ( appMetaSelectBrushObjectPs( dc, ps, ob ) )
		{ LDEB(ob); return -1;	}
	    break;

	case MFtypePATTERNBRUSH:
	    if  ( appMetaSelectPatternBrushObjectPs( dc, ps, ob ) )
		{ LDEB(ob); return -1;	}
	    break;

	case MFtypeFONT:
	    WMFDEB(appDebug( "SelectObject( ob= %d ) ", ob ));
	    WMFDEB(appDebug( "FONT\n" ));
	    if  ( appMetaSelectFontObjectPs( dc, psfl, ob ) )
		{ LDEB(ob); return -1;	}
	    break;

	default:
	    WMFDEB(appDebug( "SelectObject( ob= %d ) ", ob ));
	    WMFDEB(appDebug( "type= %d\n", dc->dcObjects[ob].mfoType ));
	    break;
	}

    return 0;
    }

static int appWinMeta_FillPath(	PrintingState *		ps,
				DeviceContext *		dc )
    {
    SimpleOutputStream *	sos= ps->psSos;

    if  ( dc->dcFillPattern )
	{
	sioOutPrintf( sos, "fill-pattern\n" );
	return 0;
	}

    appMetaSetColorPs( ps, dc, &(dc->dcBrush.lbColor) );

    if  ( dc->dcFillHatched )
	{
	switch( dc->dcBrush.lbHatch )
	    {
	    case HS_HORIZONTAL:
		sioOutPrintf( sos, "fill-horizontal\n" );
		break;
	    case HS_VERTICAL:
		sioOutPrintf( sos, "fill-vertical\n" );
		break;
	    case HS_FDIAGONAL:
		sioOutPrintf( sos, "fill-fdiagonal\n" );
		break;
	    case HS_BDIAGONAL:
		sioOutPrintf( sos, "fill-bdiagonal\n" );
		break;
	    case HS_CROSS:
		sioOutPrintf( sos, "fill-cross\n" );
		break;
	    case HS_DIAGCROSS:
		sioOutPrintf( sos, "fill-diagcross\n" );
		break;
	    default:
		LDEB(dc->dcBrush.lbHatch);
		sioOutPrintf( sos, "%% HATCH %d !!!!!!!!!!!\n",
						    dc->dcBrush.lbHatch );
	    }
	}
    else{ sioOutPrintf( sos, "fill\n" );	}

    return 0;
    }


static int appMeta_PolyLinePs(	SimpleInputStream *	sis,
				PrintingState *		ps,
				int			recordSize,
				DeviceContext *		dc )
    {
    SimpleOutputStream *	sos= ps->psSos;

    int				count;
    int				done;

    int				x0;
    int				y0;

    const char *		command;

    appMetaSetColorPs( ps, dc, &(dc->dcPen.lpColor) );
    appMetaSetPenPs( ps, dc, &(dc->dcPen) );

    count= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("PolyLine( count=%d, ... )\n", count ));

    if  ( 2* count != recordSize- 4 )
	{ LLDEB(2*count,recordSize-4); return -1;	}

    command= "bp"; x0= y0= 0;

    for ( done= 0; done < count; done++ )
	{
	int	x= sioEndianGetLeInt16( sis );
	int	y= sioEndianGetLeInt16( sis );

	x= DC_xViewport( x, dc );
	y= DC_yViewport( y, dc );

	sioOutPrintf( sos, "%d %d %s", x- x0, y- y0, command );

	if  ( done % 8 == 7 )
	    { sioOutPutCharacter( '\n', sos );		}
	else{ sioOutPutCharacter( ' ', sos );		}

	command= "rl"; x0= x; y0= y;
	}

    sioOutPrintf( sos, "stroke\n" );

    return 0;
    }

static void appMetaDrawPolygonPs(	PrintingState *		ps,
					DeviceContext *		dc,
					int			count )
    {
    char *	command;
    int		x0;
    int		y0;


    if  ( dc->dcFillInsides )
	{
	appMetafilePolygonPathPs( ps, dc->dcPoints, count, "closepath\n" );

	appWinMeta_FillPath( ps, dc );
	}

    if  ( dc->dcDrawBorders )
	{
	command= "bp"; x0= 0; y0= 0;

	appMetaSetColorPs( ps, dc, &(dc->dcPen.lpColor) );
	appMetaSetPenPs( ps, dc, &(dc->dcPen) );

	appMetafilePolygonPathPs( ps, dc->dcPoints, count, "stroke\n" );
	}

    return;
    }

static int appMeta_PolygonPs(	SimpleInputStream *	sis,
				PrintingState *		ps,
				int			recordSize,
				DeviceContext *		dc )
    {
    int		count;

    count= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("Polygon( count=%d, ... )\n", count ));
    WMFLOG(sioOutPrintf( ps->psSos, "%% Polygon( count=%d, ... )\n", count ));

    if  ( appWinMetaGetPoints( dc, count, sis ) )
	{ LDEB(count); return -1;	}

    appMetaDrawPolygonPs( ps, dc, count );

    return 0;
    }

static int appMeta_RectanglePs(	SimpleInputStream *	sis,
				PrintingState *		ps,
				int			recordSize,
				DeviceContext *		dc )
    {
    LogicalPen *	lp= &(dc->dcPen);

    int			y0;
    int			x0;
    int			y1;
    int			x1;

    int			swap;

    y1= sioEndianGetLeInt16( sis );
    x1= sioEndianGetLeInt16( sis );
    y0= sioEndianGetLeInt16( sis );
    x0= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("Rectangle(%d..%d,%d..%d)\n", x0,x1,y0,y1));
    WMFLOG(sioOutPrintf( ps->psSos, "%% Rectangle(%d..%d,%d..%d)\n", x0,x1,y0,y1));

    x0= DC_xViewport( x0, dc );
    y0= DC_yViewport( y0, dc );
    x1= DC_xViewport( x1, dc );
    y1= DC_yViewport( y1, dc );

    if  ( x1 < x0 )
	{ swap= x0; x0= x1; x1= swap; }
    if  ( y1 < y0 )
	{ swap= y0; y0= y1; y1= swap; }

    if  ( lp->lpStyle == PS_INSIDEFRAME )
	{
	int	width= 1;

	x0 += ( width+ 1 )/ 2;
	x1 -= ( width+ 1 )/ 2;
	y0 += ( width+ 1 )/ 2;
	y1 -= ( width+ 1 )/ 2;
	}

    if  ( dc->dcFillInsides )
	{
	appMetafileRectPathPs( ps, x0, y0, x1, y1 );
	appWinMeta_FillPath( ps, dc );
	}

    if  ( dc->dcDrawBorders )
	{
	appMetaSetColorPs( ps, dc, &(dc->dcPen.lpColor) );
	appMetaSetPenPs( ps, dc, &(dc->dcPen) );

	appMetafileRectPathPs( ps, x0, y0, x1, y1 );
	sioOutPrintf( ps->psSos, "stroke\n" );
	}

    return 0;
    }

static int appMeta_LineToPs(	SimpleInputStream *	sis,
				PrintingState *		ps,
				int			recordSize,
				DeviceContext *		dc )
    {
    int		x0= dc->dcX;
    int		y0= dc->dcY;
    int		y1;
    int		x1;

    y1= sioEndianGetLeInt16( sis );
    x1= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("LineTo( x:%d-> %d, y:%d-> %d )\n", x0, x1, y0, y1 ));
    WMFLOG(sioOutPrintf( ps->psSos, "%% LineTo( x:%d-> %d, y:%d-> %d )\n", x0, x1, y0, y1 ));

    dc->dcX= x1;
    dc->dcY= y1;

    x0= DC_xViewport( x0, dc );
    y0= DC_yViewport( y0, dc );
    x1= DC_xViewport( x1, dc );
    y1= DC_yViewport( y1, dc );

    if  ( dc->dcDrawBorders )
	{
	appMetaSetColorPs( ps, dc, &(dc->dcPen.lpColor) );
	appMetaSetPenPs( ps, dc, &(dc->dcPen) );

	sioOutPrintf( ps->psSos, "%d %d bp ", x0, y0 );
	sioOutPrintf( ps->psSos, "%d %d rl stroke\n", x1- x0, y1- y0 );
	}

    return 0;
    }

static void appMetaShowStringPs(	PrintingState *		ps,
					int			x0,
					int			y0,
					int			count,
					DeviceContext *		dc )
    {
    AfmFontInfo *	afi= dc->dcAfi;
    int			encoding= dc->dcFontEncoding;
    LogicalFont *	lf= &(dc->dcFont);

    int			width;
    AfmBBox		abb;
    const int		withKerning= 0;
    int			fontSizeTwips;

    if  ( ! afi )
	{ XDEB(afi); return;	}

    fontSizeTwips= 10* lf->lfTextAttribute.taFontSizeHalfPoints;
    width= psCalculateStringExtents( &abb, dc->dcString, count,
				fontSizeTwips, withKerning, encoding, afi );

    switch( dc->dcTextAlignment & 0x01 )
	{
	case TA_NOUPDATECP:
	    break;

	case TA_UPDATECP:
	    x0= DC_xViewport( dc->dcX, dc );
	    y0= DC_yViewport( dc->dcY, dc );
	    break;

	default:
	    XDEB(dc->dcTextAlignment & 0x01 );
	    break;
	}

    switch( dc->dcTextAlignment & 0x06 )
	{
	case TA_LEFT:
	    break;

	case TA_RIGHT:
	    x0 -= width;
	    break;

	case TA_CENTER:
	    x0 -= width/ 2;
	    break;

	default:
	    XDEB(dc->dcTextAlignment & 0x06);
	    break;
	}

    switch( dc->dcTextAlignment & 0x18 )
	{
	case TA_TOP:
	    y0 += ( fontSizeTwips* afi->afiAscender+ 500 )/ 1000;
	    break;

	case TA_BOTTOM:
	    y0 -= ( fontSizeTwips* afi->afiDescender+ 500 )/ 1000;
	    break;

	case TA_BASELINE:
	    break;

	default:
	    WMFDEB(appDebug(" 0x%x", dc->dcTextAlignment & 0x18 ));
	    break;
	}

    appMetaSetFontPs( ps, dc, dc->dcFont.lfTextAttribute );
    appMetaSetColorPs( ps, dc, &(dc->dcTextColor) );

    if  ( dc->dcFont.lfOrientation != 0 )
	{
	sioOutPrintf( ps->psSos, "gsave %d %d translate %g rotate\n",
				x0, y0, -0.1* dc->dcFont.lfOrientation );

	x0= y0= 0;
	}

    utilPsMovePrintStringValue( ps, dc->dcString, count, x0, y0 );

    if  ( dc->dcFont.lfTextAttribute.taTextIsUnderlined )
	{
	int		h;

	y0 -= (int)( ( fontSizeTwips* afi->afiUnderlinePosition+ 500 )/ 1000 );
	h= (int)( ( fontSizeTwips* afi->afiUnderlineThickness+ 500 )/ 1000 );

	utilPsFillRectangle( ps, x0, y0, width, h );
	}

    if  ( dc->dcFont.lfOrientation != 0 )
	{ sioOutPrintf( ps->psSos, "grestore\n" );	}

    return;
    }

static int appMeta_ExtTextOutPs(	SimpleInputStream *	sis,
					PrintingState *		ps,
					int			recordSize,
					DeviceContext *		dc )
    {
    int		x0;
    int		y0;
    int		count;
    int		style;

    int		h1;
    int		w1;
    int		y1;
    int		x1;

    if  ( appMeta_ExtTextOut( sis, recordSize, &x0, &y0, &count, &style,
						&x1, &y1, &w1, &h1, dc ) )
	{ LDEB(1); return -1;	}

    x0= DC_xViewport( x0, dc );
    y0= DC_yViewport( y0, dc );

    x1= DC_xViewport( x1, dc );
    y1= DC_yViewport( y1, dc );

    w1= DC_wViewport( w1, dc );
    h1= DC_hViewport( h1, dc );

    appMetaShowStringPs( ps, x0, y0, count, dc );

    return 0;
    }

static int appMeta_TextOutPs(	SimpleInputStream *	sis,
				PrintingState *		ps,
				int			recordSize,
				DeviceContext *		dc )
    {
    int		x0;
    int		y0;
    int		count;

    if  ( appMeta_TextOut( sis, recordSize, &x0, &y0, &count, dc ) )
	{ LDEB(1); return -1;	}

    x0= DC_xViewport( x0, dc );
    y0= DC_yViewport( y0, dc );

    appMetaShowStringPs( ps, x0, y0, count, dc );

    return 0;
    }

static int appMeta_PatBltPs(	SimpleInputStream *	sis,
				PrintingState *		ps,
				int			recordSize,
				DeviceContext *		dc )
    {
    long	rop;

    int		h0;
    int		w0;
    int		y0;
    int		x0;

    int		x1;
    int		y1;

    int		swap;

    rop= sioEndianGetLeInt32( sis );
    h0= sioEndianGetLeInt16( sis );
    w0= sioEndianGetLeInt16( sis );
    y0= sioEndianGetLeInt16( sis );
    x0= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("PatBlt([%d+%d,%d+%d], rop=0x%lx )\n", x0,w0,y0,h0, rop ));

    x1= x0+ w0;
    y1= y0+ h0;

    x0= DC_xViewport( x0, dc );
    y0= DC_yViewport( y0, dc );

    x1= DC_xViewport( x1, dc );
    y1= DC_yViewport( y1, dc );

    if  ( x1 < x0 )
	{ swap= x0; x0= x1; x1= swap; }
    if  ( y1 < y0 )
	{ swap= y0; y0= y1; y1= swap; }

    if  ( dc->dcFillInsides )
	{
	appMetafileRectPathPs( ps, x0, y0, x1, y1 );
	appWinMeta_FillPath( ps, dc );
	}

    return 0;
    }

static void appMetaGetArcPs(	SimpleInputStream *	sis,
				DeviceContext *		dc,
				char *			fun,
				int *			pY0,
				int *			pX0,
				int *			pY1,
				int *			pX1,
				int *			pYs,
				int *			pXs,
				double *		pas,
				double *		pae )

    {
    int		y0;
    int		x0;
    int		y1;
    int		x1;

    int		ys;
    int		xs;
    int		ye;
    int		xe;

    int		ym;
    int		xm;
    int		h;
    int		w;

    double	as;
    double	ae;

    int		swap;

    ye= sioEndianGetLeInt16( sis );
    xe= sioEndianGetLeInt16( sis );
    ys= sioEndianGetLeInt16( sis );
    xs= sioEndianGetLeInt16( sis );

    y1= sioEndianGetLeInt16( sis );
    x1= sioEndianGetLeInt16( sis );
    y0= sioEndianGetLeInt16( sis );
    x0= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("%s(%d..%d,%d..%d, (%d,%d), (%d,%d)\n", fun,
					    x0,x1,y0,y1, xs,ys,xe,ye));
    y0= DC_yViewport( y0, dc );
    x0= DC_xViewport( x0, dc );
    y1= DC_yViewport( y1, dc );
    x1= DC_xViewport( x1, dc );

    ys= DC_yViewport( ys, dc );
    xs= DC_xViewport( xs, dc );
    ye= DC_yViewport( ye, dc );
    xe= DC_xViewport( xe, dc );

    ym= ( y1+ y0 )/2;
    xm= ( x1+ x0 )/2;

    if  ( x1 < x0 )
	{
	xs= 2* xm- xs;
	xe= 2* xm- xe;
	swap= x0; x0= x1; x1= swap;
	}
    if  ( y1 < y0 )
	{
	ys= 2* ym- ys;
	ye= 2* ym- ye;
	swap= y0; y0= y1; y1= swap;
	}

    h= y1- y0;
    w= x1- x0;

    as= -atan2(	(double) w* ( ys- ym ), (double) h* ( xs- xm ) );
    ae= -atan2(	(double) w* ( ye- ym ), (double) h* ( xe- xm ) );

    xs= (int)( ( ( x1- x0 )/ 2 )* cos( as )+ xm );
    ys= (int)( ( ( x1- x0 )/ 2 )* sin( as ) );

    as= ( 180* as )/ M_PI;
    ae= ( 180* ae )/ M_PI;

    *pY0= y0;
    *pX0= x0;
    *pY1= y1;
    *pX1= x1;
    *pYs= ys;
    *pXs= xs;
    *pas= as;
    *pae= ae;

    return;
    }

static int appMeta_ArcPs(	SimpleInputStream *	sis,
				PrintingState *		ps,
				int			recordSize,
				DeviceContext *		dc )
    {
    int		y0;
    int		x0;
    int		y1;
    int		x1;

    int		ys;
    int		xs;

    double	as;
    double	ae;

    appMetaGetArcPs( sis, dc, "Arc", 
			    &y0, &x0, &y1, &x1, &ys, &xs, &as, &ae );

    if  ( dc->dcDrawBorders )
	{
	appMetaSetColorPs( ps, dc, &(dc->dcPen.lpColor) );
	appMetaSetPenPs( ps, dc, &(dc->dcPen) );

	sioOutPrintf( ps->psSos, "gsave [1 0 0 %g 0 %d] concat\n",
			    (double)(y0- y1)/(double)(x1- x0), ( y0+ y1 )/ 2 );

	sioOutPrintf( ps->psSos, "%d %d bp ", xs, ys );

	sioOutPrintf( ps->psSos, "%d 0 %d %f %f arc stroke\n",
					( x0+ x1 )/ 2, (x1- x0)/2, as, ae );

	sioOutPrintf( ps->psSos, "grestore\n" );
	}

    return 0;
    }

static int appMeta_PiePs(	SimpleInputStream *	sis,
				PrintingState *		ps,
				int			recordSize,
				DeviceContext *		dc )
    {
    int		y0;
    int		x0;
    int		y1;
    int		x1;

    int		ys;
    int		xs;

    double	as;
    double	ae;

    appMetaGetArcPs( sis, dc, "Pie", 
			    &y0, &x0, &y1, &x1, &ys, &xs, &as, &ae );

    sioOutPrintf( ps->psSos, "gsave [1 0 0 %g 0 %d] concat\n",
			    (double)(y0- y1)/(double)(x1- x0), ( y0+ y1 )/ 2 );

    if  ( dc->dcFillInsides )
	{
	sioOutPrintf( ps->psSos, "%d %d bp ", xs, ys );

	sioOutPrintf( ps->psSos, "%d 0 %d %f %f arc\n",
					( x0+ x1 )/ 2, (x1- x0)/2, as, ae );
	sioOutPrintf( ps->psSos, "%d  0 lineto ", ( x0+ x1 )/ 2 );
	sioOutPrintf( ps->psSos, "%d %d lineto closepath\n", xs, ys );

	appWinMeta_FillPath( ps, dc );
	}

    if  ( dc->dcDrawBorders )
	{
	appMetaSetColorPs( ps, dc, &(dc->dcPen.lpColor) );
	appMetaSetPenPs( ps, dc, &(dc->dcPen) );

	sioOutPrintf( ps->psSos, "%d %d bp ", xs, ys );

	sioOutPrintf( ps->psSos, "%d 0 %d %f %f arc ",
					( x0+ x1 )/ 2, (x1- x0)/2, as, ae );
	sioOutPrintf( ps->psSos, "%d  0 lineto ", ( x0+ x1 )/ 2 );
	sioOutPrintf( ps->psSos, "%d %d lineto stroke\n", xs, ys );
	}

    sioOutPrintf( ps->psSos, "grestore\n" );

    return 0;
    }

static int appMeta_ChordPs(	SimpleInputStream *	sis,
				PrintingState *		ps,
				int			recordSize,
				DeviceContext *		dc )
    {
    int		y0;
    int		x0;
    int		y1;
    int		x1;

    int		ys;
    int		xs;

    double	as;
    double	ae;

    appMetaGetArcPs( sis, dc, "Chord", 
			    &y0, &x0, &y1, &x1,
			    &ys, &xs, &as, &ae );

    sioOutPrintf( ps->psSos, "gsave [1 0 0 %g 0 %d] concat\n",
			    (double)(y0- y1)/(double)(x1- x0), ( y0+ y1 )/ 2 );

    if  ( dc->dcDrawBorders )
	{
	appMetaSetColorPs( ps, dc, &(dc->dcPen.lpColor) );
	appMetaSetPenPs( ps, dc, &(dc->dcPen) );

	sioOutPrintf( ps->psSos, "%d %d bp ", xs, ys );

	sioOutPrintf( ps->psSos, "%d 0 %d %f %f arc %d %d lineto stroke\n",
					( x0+ x1 )/ 2, (x1- x0)/2,
					as, ae, xs, ys );
	}

    sioOutPrintf( ps->psSos, "grestore\n" );

    return 0;
    }

static int appMeta_EllipsePs(	SimpleInputStream *	sis,
				PrintingState *		ps,
				DeviceContext *		dc )
    {
    int		y0;
    int		x0;
    int		y1;
    int		x1;

    int		swap;

    y1= sioEndianGetLeInt16( sis );
    x1= sioEndianGetLeInt16( sis );
    y0= sioEndianGetLeInt16( sis );
    x0= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("Ellipse(%d..%d,%d..%d)\n", x0,x1,y0,y1));

    x0= DC_xViewport( x0, dc );
    y0= DC_yViewport( y0, dc );
    x1= DC_xViewport( x1, dc );
    y1= DC_yViewport( y1, dc );

    if  ( x1 < x0 )
	{ swap= x0; x0= x1; x1= swap; }
    if  ( y1 < y0 )
	{ swap= y0; y0= y1; y1= swap; }

    sioOutPrintf( ps->psSos, "gsave [1 0 0 %g 0 %d] concat\n",
			    (double)(y1- y0)/(double)(x1- x0), ( y0+ y1 )/ 2 );

    if  ( dc->dcFillInsides )
	{
	sioOutPrintf( ps->psSos, "%d 0 bp ", x1 );
	sioOutPrintf( ps->psSos, "%d 0 %d 0 360 arc closepath\n",
					    ( x0+ x1 )/ 2, (x1- x0)/2 );

	appWinMeta_FillPath( ps, dc );
	}

    if  ( dc->dcDrawBorders )
	{
	appMetaSetColorPs( ps, dc, &(dc->dcPen.lpColor) );
	appMetaSetPenPs( ps, dc, &(dc->dcPen) );

	sioOutPrintf( ps->psSos, "%d 0 bp ", x1 );
	sioOutPrintf( ps->psSos, "%d 0 %d 0 360 arc stroke\n",
					    ( x0+ x1 )/ 2, (x1- x0)/2 );
	}

    sioOutPrintf( ps->psSos, "grestore\n" );

    return 0;
    }

static int appMeta_RoundRectPs(	SimpleInputStream *	sis,
				PrintingState *		ps,
				int			recordSize,
				DeviceContext *		dc )
    {
    int		y0;
    int		x0;
    int		y1;
    int		x1;

    int		h;
    int		w;

    int		swap;

    int		top;
    int		bottom;
    int		radius;

    h= sioEndianGetLeInt16( sis );
    w= sioEndianGetLeInt16( sis );

    y1= sioEndianGetLeInt16( sis );
    x1= sioEndianGetLeInt16( sis );
    y0= sioEndianGetLeInt16( sis );
    x0= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("RoundRect(%d..%d,%d..%d, w= %d,h= %d\n",
					    x0,x1,y0,y1, w, h ));

    y0= DC_yViewport( y0, dc );
    x0= DC_xViewport( x0, dc );
    y1= DC_yViewport( y1, dc );
    x1= DC_xViewport( x1, dc );

    h= DC_hViewport( h, dc );
    w= DC_wViewport( w, dc );

    if  ( x1 < x0 )
	{ swap= x0; x0= x1; x1= swap; }
    if  ( y1 < y0 )
	{ swap= y0; y0= y1; y1= swap; }

    if  ( h < 0 )
	{ h= -h;	}
    if  ( w < 0 )
	{ w= -w;	}

    top=    w* ( y1- y0 )/ ( 2* h );
    bottom= w* ( y0- y1 )/ ( 2* h );
    radius= w/ 2;

    sioOutPrintf( ps->psSos, "gsave [1 0 0 %g 0 %d] concat\n",
					    (double)h/ w, ( y0+ y1 )/ 2 );

    if  ( dc->dcFillInsides )
	{
	appMetafileRoundRectPathPs( ps, x0, y0, x1, y1, w, h, "closepath\n" );

	appWinMeta_FillPath( ps, dc );
	}

    if  ( dc->dcDrawBorders )
	{
	appMetaSetColorPs( ps, dc, &(dc->dcPen.lpColor) );
	appMetaSetPenPs( ps, dc, &(dc->dcPen) );

	appMetafileRoundRectPathPs( ps, x0, y0, x1, y1, w, h, "stroke\n" );
	}

    sioOutPrintf( ps->psSos, "grestore\n" );

    return 0;
    }

static int appMeta_PolyPolygonPs(	SimpleInputStream *	sis,
					PrintingState *		ps,
					int			recordSize,
					DeviceContext *		dc )
    {
    int			count;
    int			i;

    int *		counts;

    if  ( appMeta_GetCounts( sis, recordSize, &count, &counts, dc ) )
	{ LDEB(1); return -1;	}

    WMFDEB(appDebug("PolyPolygon( count=%d, ... )\n", count ));

    for ( i= 0; i < count; i++ )
	{
	if  ( appWinMetaGetPoints( dc, counts[i], sis ) )
	    { LDEB(counts[i]); return -1;	}

	appMetaDrawPolygonPs( ps, dc, counts[i] );
	}

    return 0;
    }

int appMetaPlayFilePs(	PrintingState *			ps,
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
    int			ob;

    {
    int			fileType;
    int			headerSize;
    int			windowsVersion;
    long		fileSize;
    int			objectCount;
    long		maxRecordSize;
    int			parameterCount;

    DeviceContext	dc;

    if  ( sioInGetCharacter( sis ) == EOF )
	{ return 0;	}
    sioInUngetLastRead( sis );

    fileType= sioEndianGetLeInt16( sis );
    headerSize= sioEndianGetLeInt16( sis );
    windowsVersion= sioEndianGetLeInt16( sis );
    fileSize= sioEndianGetLeInt32( sis );
    objectCount= sioEndianGetLeInt16( sis );
    maxRecordSize= sioEndianGetLeInt32( sis );
    parameterCount= sioEndianGetLeInt16( sis );

    if  ( appMetaInitDeviceContext( &dc, psfl, objectCount, mapMode,
				    xWinExt, yWinExt,
				    twipsWide, twipsHigh,
				    twipsWide, twipsHigh ) )
	{ LDEB(objectCount); return -1;	}

    WMFDEB(appDebug( "PS METAFILE ...\n" ));

    for (;;)
	{
	long		rop;

	int		x0;
	int		y0;
	int		w0;
	int		h0;

	int		x1;
	int		y1;
	int		w1;
	int		h1;

	int		done;
	int		ignore;

	long		recordSize;
	int		function;

	recordSize= sioEndianGetLeInt32( sis );
	function= sioEndianGetLeInt16( sis );

	if  ( recordSize < 0 || function < 0 )
	    { LLDEB(recordSize,function); return -1;	}

	switch( function )
	    {
	    case WINMETA_OffsetWindowOrg:
		if  ( appMetaOffsetWindowOrg( &dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_SetWindowOrg:
		if  ( appMetaSetWindowOrg( &dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_SetWindowExt:
		if  ( appMetaSetWindowExt( &dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_SetViewportOrg:
		if  ( appMetaSetViewportOrg( &dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_SetViewportExt:
		if  ( appMetaSetViewportExt( &dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_SaveDC:
		if  ( appMetaSaveDC( &dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_RestoreDC:
		if  ( appMetaRestoreDC( &dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_OffsetClipRgn:
		if  ( appMetaOffsetClipRgn( &dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_SelectClipRgn:
		if  ( appMetaSelectClipRgn( &dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_ExcludeClipRect:
		if  ( appMetaExcludeClipRect( &dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_IntersectClipRect:
		if  ( appMetaIntersectClipRect( &dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_SetMapperFlags:
		rop= sioEndianGetLeInt32( sis );
		WMFDEB(appDebug("WINMETA_SetMapperFlags(0x%lx)\n",rop));
		continue;

	    case WINMETA_StretchBlt:
		rop= sioEndianGetLeInt32( sis );
		h0= sioEndianGetLeInt16( sis );
		w0= sioEndianGetLeInt16( sis );
		y0= sioEndianGetLeInt16( sis );
		x0= sioEndianGetLeInt16( sis );
		h1= sioEndianGetLeInt16( sis );
		w1= sioEndianGetLeInt16( sis );
		y1= sioEndianGetLeInt16( sis );
		x1= sioEndianGetLeInt16( sis );

		WMFDEB(appDebug("StretchBlt([%d+%d,%d+%d]->[%d+%d,%d+%d],..)\n",
				x0,w0,y0,h0, x1,w1,y1,h1 ));

		if  ( appMetaBitmapImagePs( sis, ps, 2*(recordSize-3-2-8*1),
					useFilters, indexedImages, &dc,
					x0, y0, w0, h0, x1, y1, w1, h1 ) )
		    { LDEB(1); return -1; }

		continue;
	    case WINMETA_StretchDIBits:
		rop= sioEndianGetLeInt32( sis );
		ignore= sioEndianGetLeInt16( sis );
		/*  source	*/
		h0= sioEndianGetLeInt16( sis );
		w0= sioEndianGetLeInt16( sis );
		y0= sioEndianGetLeInt16( sis );
		x0= sioEndianGetLeInt16( sis );
		/*  destination	*/
		h1= sioEndianGetLeInt16( sis );
		w1= sioEndianGetLeInt16( sis );
		y1= sioEndianGetLeInt16( sis );
		x1= sioEndianGetLeInt16( sis );

		WMFDEB(appDebug(
			"StretchDIBits([%d+%d,%d+%d]->[%d+%d,%d+%d],..)\n",
			x0,w0,y0,h0, x1,w1,y1,h1 ));

		if  ( appMetaBitmapImagePs( sis, ps, 2*(recordSize-3-2-9*1),
					    useFilters, indexedImages, &dc,
					    x0, y0, w0, h0, x1, y1, w1, h1 ) )
		    { LDEB(1); return -1; }
		continue;

	    case WINMETA_SelectObject:
		if  ( appMetaSelectObjectPs( &dc, psfl, ps,
							recordSize, sis ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case WINMETA_CreateBrushIndirect:
		if  ( appMetaCreateBrushIndirect( &dc, recordSize, sis ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case WINMETA_CreatePenIndirect:
		if  ( appMetaCreatePenIndirect( &dc, recordSize, sis ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case WINMETA_CreatePatternBrush:
		if  ( appMetaCreatePatternBrush( &dc, recordSize, sis ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case WINMETA_CreatePalette:
		if  ( appMetaCreatePalette( &dc, recordSize, sis ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case WINMETA_CreateFontIndirect:
		if  ( appMetaCreateFontIndirect( &dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_DeleteObject:

		ob= sioEndianGetLeInt16( sis );

		if  ( ob < 0 || ob >= dc.dcObjectCount )
		    { LLDEB(ob,dc.dcObjectCount); return -1;	}

		appMetaCleanObject( &(dc.dcObjects[ob]) );

		dc.dcObjects[ob].mfoType= MFtypeFREE;
		continue;

	    case WINMETA_SetBkColor:
		if  ( appMetaSetBkColor( &dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}

		continue;

	    case WINMETA_SetTextColor:
		if  ( appMetaSetTextColor( &dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}

		continue;

	    case WINMETA_SetBkMode:
		WMFDEB(appDebug("SetBkMode(...)\n"));
		if  ( recordSize == 4 )
		    {
		    dc.dcBkMode= sioEndianGetLeInt16( sis );
		    continue;
		    }
		if  ( recordSize == 5 )
		    {
		    dc.dcBkMode= sioEndianGetLeInt32( sis );
		    continue;
		    }
		XLDEB(function,recordSize); return -1;

	    case WINMETA_SetTextCharExtra:
		WMFDEB(appDebug("SetTextCharExtra(...)\n"));
		dc.dcExtraTextSpacing= sioEndianGetLeInt16( sis );
		continue;

	    case WINMETA_SetTextAlign:
		if  ( appMetaSetTextAlign( &dc, recordSize, sis ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case WINMETA_SetTextJustification:
		WMFDEB(appDebug("SetTextJustification(...)\n"));

		dc.dcJustificationAmount= sioEndianGetLeInt16( sis );
		dc.dcJustificationSpaces= sioEndianGetLeInt16( sis );
		continue;

	    case WINMETA_SetStretchBltMode:
		switch( recordSize )
		    {
		    case 4:
			dc.dcStretchBltMode= sioEndianGetLeInt16( sis );
			break;
		    case 5:
			dc.dcStretchBltMode= sioEndianGetLeInt32( sis );
			break;
		    default:
			LDEB(recordSize); return -1;
		    }

		WMFDEB(appDebug("SetStretchBltMode(%ld)\n",
						    dc.dcStretchBltMode ));
		continue;

	    case WINMETA_PolyLine:
		if  ( appMeta_PolyLinePs( sis, ps, recordSize, &dc ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_Polygon:
		if  ( appMeta_PolygonPs( sis, ps, recordSize, &dc ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_Rectangle:
		if  ( appMeta_RectanglePs( sis, ps, recordSize, &dc ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_LineTo:
		if  ( appMeta_LineToPs( sis, ps, recordSize, &dc ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_MoveTo:
		y0= sioEndianGetLeInt16( sis );
		x0= sioEndianGetLeInt16( sis );

		WMFDEB(appDebug("MoveTo( x=%d, y=%d )\n", x0, y0 ));

		dc.dcX= x0;
		dc.dcY= y0;

		continue;

	    case WINMETA_ExtTextOut:
		if  ( appMeta_ExtTextOutPs( sis, ps, recordSize, &dc ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case WINMETA_TextOut:
		if  ( appMeta_TextOutPs( sis, ps, recordSize, &dc ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case WINMETA_Escape:
		WMFDEB(appDebug("Escape(...)\n"));
		goto skipArguments;

	    case WINMETA_PatBlt:
		if  ( appMeta_PatBltPs( sis, ps, recordSize, &dc ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case WINMETA_SetPolyFillMode:
		WMFDEB(appDebug("SetPolyFillMode(...)\n"));
		goto skipArguments;

	    case WINMETA_Pie:
		if  ( appMeta_PiePs( sis, ps, recordSize, &dc ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_Arc:
		if  ( appMeta_ArcPs( sis, ps, recordSize, &dc ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_Chord:
		if  ( appMeta_ChordPs( sis, ps, recordSize, &dc ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_Ellipse:
		if  ( appMeta_EllipsePs( sis, ps, &dc ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_RoundRect:
		if  ( appMeta_RoundRectPs( sis, ps, recordSize, &dc ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_SetMapMode:
		appMetaSetMapMode( &dc, recordSize, sis );
		continue;

	    case WINMETA_SetROP2:
		if  ( recordSize == 5 )
		    {
		    dc.dcROP2= sioEndianGetLeInt32( sis );
		    WMFDEB(appDebug("SetROP2(%ld)\n", (long)dc.dcROP2 ));
		    continue;
		    }
		if  ( recordSize == 4 )
		    {
		    dc.dcROP2= sioEndianGetLeInt16( sis );
		    WMFDEB(appDebug("SetROP2(%ld)\n", (long)dc.dcROP2 ));
		    continue;
		    }
		XLDEB(function,recordSize); return -1;

	    case WINMETA_SelectPalette:
		ob= sioEndianGetLeInt16( sis );
		WMFDEB(appDebug("SelectPalette(%d)\n", ob ));
		continue;

	    case WINMETA_RealizePalette:
		WMFDEB(appDebug("RealizePalette()\n"));
		continue;

	    case WINMETA_PolyPolygon:
		if  ( appMeta_PolyPolygonPs( sis, ps, recordSize, &dc ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_ScaleViewportExt:
		appMetaScaleViewportExt( &dc, recordSize, sis );
		continue;

	    case WINMETA_ScaleWindowExt:
		appMetaScaleViewportExt( &dc, recordSize, sis );
		continue;

	    case 0:
		if  ( recordSize != 3 )
		    { XLDEB(function,recordSize); return -1;	}
		break;
	    skipArguments:
		for ( done= 3; done < recordSize; done++ )
		    { ignore= sioEndianGetLeInt16( sis ); }
		continue;
	    default:
		XLDEB(function,recordSize);
		/*
		for ( done= 3; done < recordSize; done++ )
		    { ignore= sioEndianGetLeInt16( sis ); }
		continue;
		*/
		return -1;
	    }

	break;
	}

    appMetaCleanDeviceContext( &dc );
    }

    return 0;
    }

/************************************************************************/
/*									*/
/*  List the fonts in a windows metafile.				*/
/*									*/
/*  The geometry parameters are absolutely irrelevant, but required by	*/
/*  appMetaInitDeviceContext().						*/
/*									*/
/************************************************************************/

int appMetaListFontsPs( PostScriptTypeList *	pstl,
			SimpleInputStream *	sis,
			const PostScriptFontList * psfl,
			const char *		prefix,
			int			mapMode,
			int			xWinExt,
			int			yWinExt,
			int			twipsWide,
			int			twipsHigh )
    {
    int			fileType;
    int			headerSize;
    int			windowsVersion;
    long		fileSize;
    int			objectCount;
    long		maxRecordSize;
    int			parameterCount;

    DeviceContext	dc;

    DocumentFontList *	dfl= &(dc.x_dcFontList);

    if  ( sioInGetCharacter( sis ) == EOF )
	{ return 0;	}
    sioInUngetLastRead( sis );

    fileType= sioEndianGetLeInt16( sis );
    headerSize= sioEndianGetLeInt16( sis );
    windowsVersion= sioEndianGetLeInt16( sis );
    fileSize= sioEndianGetLeInt32( sis );
    objectCount= sioEndianGetLeInt16( sis );
    maxRecordSize= sioEndianGetLeInt32( sis );
    parameterCount= sioEndianGetLeInt16( sis );

    if  ( appMetaInitDeviceContext( &dc, psfl, objectCount, mapMode,
						    xWinExt, yWinExt,
						    twipsWide, twipsHigh,
						    twipsWide, twipsHigh ) )
	{ LDEB(objectCount); return -1;	}

    WMFDEB(appDebug( "PS METAFILE FONTS ...\n" ));

    for (;;)
	{
	int			done;
	int			ignore;

	long			recordSize;
	int			function;

	LogicalFont *		lf;

	DocumentFont *		df;
	AfmFontInfo *		afi;
	int			encoding;

	const int		appearsInText= 0;

	MetaFileObject *	mfo;

	recordSize= sioEndianGetLeInt32( sis );
	function= sioEndianGetLeInt16( sis );

	if  ( recordSize < 0 || function < 0 )
	    { LLDEB(recordSize,function); return -1;	}

	switch( function )
	    {
	    case WINMETA_SetWindowExt:
		if  ( appMetaSetWindowExt( &dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_SetViewportExt:
		if  ( appMetaSetViewportExt( &dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_CreateFontIndirect:
		if  ( appMetaCreateFontIndirect( &dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}

		mfo= &(dc.dcObjects[0]);
		lf= &(mfo->mfoLogicalFont);

		df= dc.x_dcFontList.dflFonts+ lf->lfTextAttribute.taFontNumber;

		df= dfl->dflFonts+ lf->lfTextAttribute.taFontNumber;

		if  ( utilFindPsFontForDocFont( df, dfl, psfl ) )
		    { LDEB(lf->lfTextAttribute.taFontNumber); return -1; }

		afi= utilPsGetAfi( &encoding, dfl,
					    psfl, &(lf->lfTextAttribute) );
		if  ( ! afi )
		    { XDEB(afi); return -1;	}

		if  ( utilRememberPostsciptFace( pstl, encoding, afi,
			    &(lf->lfTextAttribute), prefix, appearsInText ) )
		    { SDEB(afi->afiFontName); return -1;	}

		appMetaCleanObject( mfo );
		mfo->mfoType= MFtypeFREE;

		continue;

	    default:
		for ( done= 3; done < recordSize; done++ )
		    { ignore= sioEndianGetLeInt16( sis ); }
		continue;

	    case 0:
		break;
	    }

	break;
	}

    appMetaCleanDeviceContext( &dc );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Include the procedures that are used for WMF rendering in		*/
/*  PostScript.								*/
/*									*/
/*  sed -e 's/^/"/' -e 's/$/",/' file					*/
/*									*/
/************************************************************************/

static const char *	APPMETAPS_FillPrep[]=
{
"/fill-prep",
"    {",
"    pathbbox",
"    clip newpath",
"			% llx_usr  lly_usr  urx_usr  ury_usr",
"    [ 0 0 0 0 0 0 ] currentmatrix",
"    transform",
"    [ 0 0 0 0 0 0 ] defaultmatrix [ 0 0 0 0 0 0 ] invertmatrix",
"    transform",
"			% llx_usr  lly_usr  urx_dev  ury_dev",
"    4 2 roll",
"			% urx_dev  ury_dev  llx_usr  lly_usr",
"    [ 0 0 0 0 0 0 ] currentmatrix",
"    transform",
"    [ 0 0 0 0 0 0 ] defaultmatrix [ 0 0 0 0 0 0 ] invertmatrix",
"    transform",
"			% urx_dev  ury_dev  llx_dev  lly_dev",
"    3 -1 roll",
"			% urx_dev  llx_dev  lly_dev  ury_dev",
"    exch",
"			% urx_dev  llx_dev  ury_dev  lly_dev",
"    2 copy lt { exch } if",
"			% urx_dev  llx_dev  ury=d  lly=d",
"    /fill-high load div floor   /fill-high load mul",
"			% urx_dev  llx_dev  ury=d  LLY_dev",
"    exch",
"    /fill-high load div ceiling /fill-high load mul",
"			% urx_dev  llx_dev  LLY_dev URY_dev",
"    4 2 roll",
"			% LLY_dev URY_dev urx_dev  llx_dev",
"    2 copy lt { exch } if",
"			% LLY_dev URY_dev urx=d  llx=d",
"    /fill-wide load div floor   /fill-wide load mul",
"			% LLY_dev URY_dev urx=d  LLX_dev",
"    exch",
"    /fill-wide load div ceiling /fill-wide load mul",
"			% LLY_dev URY_dev LLX_dev URX_dev",
"    4 2 roll",
"			% LLX_dev URX_dev LLY_dev URY_dev",
"    [ 0 0 0 0 0 0 ] defaultmatrix setmatrix",
"			% LLX_dev URX_dev LLY_dev URY_dev",
"    } bind def",
"",
};

static const char *	APPMETAPS_FillPattern[]=
{
"/fill-pattern",
"    {",
"    gsave fill-prep",
"			% LLX_dev URX_dev LLY_dev URY_dev",
"    /fill-high load exch",
"			% LLX_dev URX_dev LLY_dev <fill-high> URY_dev",
"	{",
"	gsave",
"			% LLX_dev URX_dev y",
"	0 exch translate",
"			% LLX_dev URX_dev",
"	2 copy /fill-wide load exch",
"	    {",
"	    gsave",
"	    0 translate",
"	    fill-cell",
"	    grestore",
"	    } for",
"",
"	grestore",
"	} for",
"",
"    pop pop",
"    grestore",
"    } bind def",
"",
};

static const char *	APPMETAPS_HatchHorizontal[]=
{
"/hatch-horizontal",
"    {",
"			% LLX_dev URX_dev LLY_dev URY_dev",
"    /fill-high load exch",
"			% LLX_dev URX_dev LLY_dev <fill-high> URY_dev",
"	{",
"			% LLX_dev URX_dev y",
"	2 copy moveto",
"			% LLX_dev URX_dev y",
"	3 copy exch pop",
"	lineto pop stroke",
"	} for",
"",
"    pop pop",
"    } bind def",
"",
};

static const char *	APPMETAPS_HatchVertical[]=
{
"/hatch-vertical",
"    {",
"			% LLX_dev URX_dev LLY_dev URY_dev",
"    4 2 roll",
"			% LLY_dev URY_dev LLX_dev URX_dev",
"    /fill-wide load exch",
"			% LLY_dev URY_dev LLX_dev <fill-wide> URX_dev",
"	{",
"			% LLY_dev URY_dev x",
"	2 copy exch moveto",
"			% LLY_dev URY_dev x",
"	3 copy exch pop",
"	exch lineto pop stroke",
"	} for",
"",
"    pop pop",
"    } bind def",
"",
};

static const char *	APPMETAPS_HatchFdiagonal[]=
{
"/hatch-fdiagonal",
"    {",
"			% LLX_dev URX_dev LLY_dev URY_dev",
"    4 2 roll 2 copy exch sub 1 copy",
"			% LLY_dev URY_dev LLX_dev URX_dev <dx> <dx>",
"    4 2 roll",
"			% LLY_dev URY_dev <dx> <dx> LLX_dev URX_dev",
"    6 3 roll",
"			% <dx> LLX_dev URX_dev LLY_dev URY_dev <dx>",
"    3 -1 roll",
"			% <dx> LLX_dev URX_dev URY_dev <dx> LLY_dev",
"    exch sub exch",
"",
"    /fill-high load exch",
"			% <dx> LLX_dev URX_dev <dx> LLY_dev- <fill-high> URY_dev",
"	{",
"			% <dx> LLX_dev URX_dev y",
"	2 copy moveto",
"			% <dx> LLX_dev URX_dev y",
"	4 copy",
"			% <dx> LLX_dev URX_dev y <dx> LLX_dev URX_dev y",
"	exch pop",
"			% <dx> LLX_dev URX_dev y <dx> LLX_dev y",
"	3 -1 roll",
"			% <dx> LLX_dev URX_dev y LLX_dev y <dx>",
"	add",
"			% <dx> LLX_dev URX_dev y LLX_dev y--",
"	lineto pop stroke",
"	} for",
"",
"    pop pop pop",
"    } bind def",
"",
};

static const char *	APPMETAPS_HatchBdiagonal[]=
{
"/hatch-bdiagonal",
"    {",
"			% LLX_dev URX_dev LLY_dev URY_dev",
"    4 2 roll 2 copy exch sub 1 copy",
"			% LLY_dev URY_dev LLX_dev URX_dev <dx> <dx>",
"    4 2 roll",
"			% LLY_dev URY_dev <dx> <dx> LLX_dev URX_dev",
"    6 3 roll",
"			% <dx> LLX_dev URX_dev LLY_dev URY_dev <dx>",
"    add /fill-high load exch",
"			% <dx> LLX_dev URX_dev <dx> LLY_dev <fill-high> URY_dev+",
"	{",
"			% <dx> LLX_dev URX_dev y",
"	2 copy moveto",
"			% <dx> LLX_dev URX_dev y",
"	4 copy",
"			% <dx> LLX_dev URX_dev y <dx> LLX_dev URX_dev y",
"	exch pop",
"			% <dx> LLX_dev URX_dev y <dx> LLX_dev y",
"	3 -1 roll",
"			% <dx> LLX_dev URX_dev y LLX_dev y <dx>",
"	sub",
"			% <dx> LLX_dev URX_dev y LLX_dev y--",
"	lineto pop stroke",
"	} for",
"",
"    pop pop pop",
"    } bind def",
"",
};

static const char *	APPMETAPS_FillHorizontal[]=
{
"/fill-horizontal",
"    {",
"    gsave",
"",
"    0.3 setlinewidth",
"",
"    /fill-wide 4 store",
"    /fill-high 4 store",
"",
"    fill-prep",
"    hatch-horizontal",
"",
"    grestore",
"    } bind def",
"",
};

static const char *	APPMETAPS_FillVertical[]=
{
"/fill-vertical",
"    {",
"    gsave",
"",
"    0.3 setlinewidth",
"",
"    /fill-wide 4 store",
"    /fill-high 4 store",
"",
"    fill-prep",
"    hatch-vertical",
"",
"    grestore",
"    } bind def",
"",
};

static const char *	APPMETAPS_FillCross[]=
{
"/fill-cross",
"    {",
"    gsave",
"",
"    0.3 setlinewidth",
"",
"    /fill-wide 4 store",
"    /fill-high 4 store",
"",
"    fill-prep",
"    4 copy",
"    hatch-horizontal",
"    hatch-vertical",
"",
"    grestore",
"    } bind def",
"",
};

static const char *	APPMETAPS_FillBdiagonal[]=
{
"/fill-bdiagonal",
"    {",
"    gsave",
"",
"    0.3 setlinewidth",
"",
"    /fill-wide 4 store",
"    /fill-high 4 store",
"",
"    fill-prep",
"    hatch-bdiagonal",
"",
"    grestore",
"    } bind def",
"",
};

static const char *	APPMETAPS_FillFdiagonal[]=
{
"/fill-fdiagonal",
"    {",
"    gsave",
"",
"    0.3 setlinewidth",
"",
"    /fill-wide 4 store",
"    /fill-high 4 store",
"",
"    fill-prep",
"    hatch-fdiagonal",
"",
"    grestore",
"    } bind def",
"",
};

static const char *	APPMETAPS_FillDiagcross[]=
{
"/fill-diagcross",
"    {",
"    gsave",
"",
"    0.3 setlinewidth",
"",
"    /fill-wide 4 store",
"    /fill-high 4 store",
"",
"    fill-prep",
"    4 copy",
"    hatch-bdiagonal",
"    hatch-fdiagonal",
"",
"    grestore",
"    } bind def",
"",
};

static const char *	APPMETAPS_Rl[]=
{
"/rl",
"    {",
"    rlineto",
"    } bind def",
"",
};

static const char *	APPMETAPS_Bp[]=
{
"/bp",
"    {",
"    newpath moveto",
"    } bind def",
"",
};

int appMetaDefineProcsetPs(	SimpleOutputStream *		sos )
    {
    sioOutPrintf( sos, "%%%%BeginProcSet AppWmfToPs 1.0 1\n\n" );

    utilPsDefineProcedure( sos, APPMETAPS_Bp,
		    sizeof(APPMETAPS_Bp)/sizeof(const char *) );

    utilPsDefineProcedure( sos, APPMETAPS_Rl,
		    sizeof(APPMETAPS_Rl)/sizeof(const char *) );

    utilPsDefineProcedure( sos, APPMETAPS_FillPrep,
		    sizeof(APPMETAPS_FillPrep)/sizeof(const char *) );

    utilPsDefineProcedure( sos, APPMETAPS_FillPattern,
		    sizeof(APPMETAPS_FillPattern)/sizeof(const char *) );

    utilPsDefineProcedure( sos, APPMETAPS_HatchHorizontal,
		    sizeof(APPMETAPS_HatchHorizontal)/sizeof(const char *) );

    utilPsDefineProcedure( sos, APPMETAPS_HatchVertical,
		    sizeof(APPMETAPS_HatchVertical)/sizeof(const char *) );

    utilPsDefineProcedure( sos, APPMETAPS_HatchFdiagonal,
		    sizeof(APPMETAPS_HatchFdiagonal)/sizeof(const char *) );

    utilPsDefineProcedure( sos, APPMETAPS_HatchBdiagonal,
		    sizeof(APPMETAPS_HatchBdiagonal)/sizeof(const char *) );

    utilPsDefineProcedure( sos, APPMETAPS_FillHorizontal,
		    sizeof(APPMETAPS_FillHorizontal)/sizeof(const char *) );

    utilPsDefineProcedure( sos, APPMETAPS_FillVertical,
		    sizeof(APPMETAPS_FillVertical)/sizeof(const char *) );

    utilPsDefineProcedure( sos, APPMETAPS_FillCross,
		    sizeof(APPMETAPS_FillCross)/sizeof(const char *) );

    utilPsDefineProcedure( sos, APPMETAPS_FillBdiagonal,
		    sizeof(APPMETAPS_FillBdiagonal)/sizeof(const char *) );

    utilPsDefineProcedure( sos, APPMETAPS_FillFdiagonal,
		    sizeof(APPMETAPS_FillFdiagonal)/sizeof(const char *) );

    utilPsDefineProcedure( sos, APPMETAPS_FillDiagcross,
		    sizeof(APPMETAPS_FillDiagcross)/sizeof(const char *) );

    sioOutPrintf( sos, "%%%%EndProcSet\n\n" );
    return 0;
    }

