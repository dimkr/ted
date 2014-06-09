#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<appWinMeta.h>
#   include	<utilPs.h>
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

static void appMetaCleanObjectX11(	DeviceContextX11 *	dcx,
					MetaFileObject *	mfo )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    AppDrawingData *	add= &(dc->dcDrawingData);
    PatternBrush *	pb;
    LogicalBrush *	lb;

    switch( mfo->mfoType )
	{
	case MFtypePATTERNBRUSH:
	    pb= &(mfo->mfoPatternBrush);
	    if  ( pb->pbTilePixmap )
		{
		appDrawFreePixmap( add, pb->pbTilePixmap );
		pb->pbTilePixmap= (APP_BITMAP_IMAGE)0;
		}
	    break;

	case MFtypeBRUSH:
	    lb= &(mfo->mfoLogicalBrush);
	    if  ( lb->lbTilePixmap )
		{
		appDrawFreePixmap( add, lb->lbTilePixmap );
		lb->lbTilePixmap= (APP_BITMAP_IMAGE)0;
		}
	    break;

	default:
	    break;
	}

    return;
    }

static void appMetaCleanDeviceContextX11(	DeviceContextX11 *	dcx )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    int			ob;

    for ( ob= 0; ob < dc->dcObjectCount; ob++ )
	{ appMetaCleanObjectX11( dcx, &(dc->dcObjects[ob]) );	}

    appMetaCleanDeviceContext( dc );

    return;
    }

static int appMetaInitDeviceContextX11(	DeviceContextX11 *	dcx,
					const PostScriptFontList * psfl,
					int			objectCount,
					int			mapMode,
					int			xWinExt,
					int			yWinExt,
					int			twipsWide,
					int			twipsHigh,
					int			pixelsWide,
					int			pixelsHigh )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);

    if  ( appMetaInitDeviceContext( dc, psfl, objectCount, mapMode,
						    xWinExt, yWinExt,
						    pixelsWide, pixelsHigh,
						    twipsWide, twipsHigh ) )
	{ LDEB(objectCount); return -1;	}

    dcx->dcxPhysicalFont= (DrawScreenFont *)0;

    dcx->dcxPixelsWide= pixelsWide;
    dcx->dcxPixelsHigh= pixelsHigh;

    return 0;
    }

static int appMetaSetDeviceDefaultsX11(	DeviceContextX11 *	dcx,
					AppColors *		ac )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    AppDrawingData *	add= &(dc->dcDrawingData);

    appDrawSetForegroundBlack( add );

    if  ( appColorRgb( &dcx->dcxPenColor, ac,
		    dc->dcPen.lpColor.rgb8Red,
		    dc->dcPen.lpColor.rgb8Green,
		    dc->dcPen.lpColor.rgb8Blue ) )
	{ LDEB(1); return -1;	}

    if  ( appColorRgb( &dcx->dcxBrushColor, ac,
		    dc->dcBrush.lbColor.rgb8Red,
		    dc->dcBrush.lbColor.rgb8Green,
		    dc->dcBrush.lbColor.rgb8Blue ) )
	{ LDEB(1); return -1;	}

    if  ( appColorRgb( &dcx->dcxBkColor, ac,
		    dc->dcBkColor.rgb8Red,
		    dc->dcBkColor.rgb8Green,
		    dc->dcBkColor.rgb8Blue ) )
	{ LDEB(1); return -1;	}

    if  ( appColorRgb( &dcx->dcxTextColor, ac,
		    dc->dcTextColor.rgb8Red,
		    dc->dcTextColor.rgb8Green,
		    dc->dcTextColor.rgb8Blue ) )
	{ LDEB(1); return -1;	}


    return 0;
    }

static int appMetaSelectBrushObjectX11(	DeviceContextX11 *	dcx,
					AppColors *		ac,
					int			ob )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    AppDrawingData *	add= &(dc->dcDrawingData);
    LogicalBrush *	lb= &(dc->dcObjects[ob].mfoLogicalBrush);

    WMFDEB(appDebug( "SelectObject( ob= %d ) ", ob ));
    WMFDEB(appDebug( "BRUSH" ));

    switch( lb->lbStyle )
	{
	case BS_SOLID:
	    dcx->dcxDeviceContext.dcFillInsides= 1;

	    WMFDEB(appDebug( " SOLID r= %d, g= %d, b=%d\n",
					    lb->lbColor.rgb8Red,
					    lb->lbColor.rgb8Green,
					    lb->lbColor.rgb8Blue ));

	    if  ( appColorRgb( &dcx->dcxBrushColor, ac,
					    lb->lbColor.rgb8Red,
					    lb->lbColor.rgb8Green,
					    lb->lbColor.rgb8Blue ) )
		{ LDEB(1);  return -1;	}

#	    ifdef USE_MOTIF
	    XSetFillStyle( add->addDisplay, add->addGc, FillSolid );
#	    endif
#	    ifdef USE_GTK
	    gdk_gc_set_fill( add->addGc, GDK_SOLID );
#	    endif

	    dcx->dcxFillTiled= 0;
	    dcx->dcxFillHatched= 0;
	    appDrawSetForegroundColor( add, &(dcx->dcxBrushColor) );
	    break;

	case BS_HOLLOW:
#	    ifdef USE_MOTIF
	    XSetFillStyle( add->addDisplay, add->addGc, FillSolid );
#	    endif
#	    ifdef USE_GTK
	    gdk_gc_set_fill( add->addGc, GDK_SOLID );
#	    endif

	    dcx->dcxFillTiled= 0;
	    dcx->dcxFillHatched= 0;
	    dcx->dcxDeviceContext.dcFillInsides= 0;

	    WMFDEB(appDebug( " HOLLOW\n" ));
	    break;

	case BS_HATCHED:
	case BS_PATTERN:
	    WMFDEB(appDebug( "\n" ));
	    if  ( ! lb->lbTilePixmap )
		{
		int		wide= 8;
		int		high= 8;

		AppDrawingData	addPat;

		appInitDrawingData( &addPat );

		if  ( appColorRgb( &(dcx->dcxBrushColor), ac,
						lb->lbColor.rgb8Red,
						lb->lbColor.rgb8Green,
						lb->lbColor.rgb8Blue ) )
		    { LDEB(1);  return -1;	}

		lb->lbTilePixmap= appMakePixmap( add, wide, high );

		appCloneDrawingEnvironment( &addPat,
					add, 1.0, 0.05, lb->lbTilePixmap );

		appDrawSetForegroundWhite( &addPat );
		appDrawFillRectangle( &addPat, 0, 0, wide, high );

		appDrawSetForegroundColor( &addPat, &(dcx->dcxBrushColor) );

		switch( lb->lbHatch )
		    {
		    case HS_HORIZONTAL:
			appDrawDrawLine( &addPat, 0, 1, wide- 1, 1 );
			break;

		    case HS_VERTICAL:
			appDrawDrawLine( &addPat, 1, 0, 1, high- 1 );
			break;

		    case HS_FDIAGONAL:
			appDrawDrawLine( &addPat, 0, 0, wide- 1, high- 1 );
			break;

		    case HS_BDIAGONAL:
			appDrawDrawLine( &addPat, 0, high- 1, wide- 1, 0 );
			break;

		    case HS_CROSS:
			appDrawDrawLine( &addPat, 0, 1, wide- 1, 1 );
			appDrawDrawLine( &addPat, 1, 0, 1, high- 1 );
			break;

		    case HS_DIAGCROSS:
			appDrawDrawLine( &addPat, 0, 0, wide- 1, high- 1 );
			appDrawDrawLine( &addPat, 0, high- 1, wide- 1, 0 );
			break;

		    default:
			LDEB(lb->lbHatch);
			break;
		    }

		appCleanDrawingData( &addPat );
		}

#	    ifdef USE_MOTIF
	    XSetTile( add->addDisplay, add->addGc, lb->lbTilePixmap );
#	    endif
#	    ifdef USE_GTK
	    gdk_gc_set_tile( add->addGc, lb->lbTilePixmap );
#	    endif

	    dcx->dcxDeviceContext.dcFillInsides= 1;
	    dcx->dcxFillTiled= 1;
	    dcx->dcxFillHatched= 1;
	    break;

	default:
	    LDEB(lb->lbStyle); 
	    dcx->dcxDeviceContext.dcFillInsides= 0;
	    break;
	}

    dcx->dcxDeviceContext.dcBrush= *lb;

    return 0;
    }

static int appMetaSelectPatternBrushObjectX11(	DeviceContextX11 *	dcx,
						AppColors *		ac,
						int			ob )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    AppDrawingData *	add= &(dc->dcDrawingData);
    PatternBrush *	pb= &(dc->dcObjects[ob].mfoPatternBrush);
    AppBitmapImage *	abi= &(pb->pbAbi);

    WMFDEB(appDebug( "SelectObject( ob= %d ) ", ob ));
    WMFDEB(appDebug( "PATTERNBRUSH\n" ));

    if  ( ! pb->pbTilePixmap )
	{
	const int		dstXExt= abi->abiBitmap.bdPixelsWide;
	const int		dstYExt= abi->abiBitmap.bdPixelsHigh;

	if  ( appImgMakePixmap( add, &(pb->pbTilePixmap),
					    dstXExt, dstYExt, ac, abi ) )
	    { LDEB(1); return -1;	}
	}

#   ifdef USE_MOTIF
    XSetTile( add->addDisplay, add->addGc, pb->pbTilePixmap );
#   endif
#   ifdef USE_GTK
    gdk_gc_set_tile( add->addGc, pb->pbTilePixmap );
#   endif

    dc->dcFillInsides= 1;
    dcx->dcxFillTiled= 1;
    dcx->dcxFillHatched= 0;

    return 0;
    }

static int appMetaSelectFontObjectX11(	DeviceContextX11 *	dcx,
					AppColors *		ac,
					int			ob )
    {
    DeviceContext *		dc= &(dcx->dcxDeviceContext);
    AppDrawingData *		add= &(dc->dcDrawingData);
    LogicalFont *		lf= &(dc->dcObjects[ob].mfoLogicalFont);

    if  ( lf->lfPrivateFont < 0 )
	{
	const TextAttribute *	ta= &(lf->lfTextAttribute);

	lf->lfPrivateFont= appOpenScreenFont( add, &(dc->x_dcFontList), ta );

	if  ( lf->lfPrivateFont < 0 )
	    { SLDEB(lf->lfFaceName,lf->lfPrivateFont); return 0;	}
	}

    if  ( lf->lfPrivateFont >= 0 )
	{
	dcx->dcxPhysicalFont=
			add->addScreenFontList.apflFonts+ lf->lfPrivateFont;

	appDrawSetFont( add, dcx->dcxPhysicalFont->apfFontStruct );
	}
    else{ dcx->dcxPhysicalFont= (DrawScreenFont *)0;	}

    dc->dcFont= *lf;

    return 0;
    }

void appMetaX11SetWindowsLineStyle(	AppDrawingData *	add,
					int *			pDraw,
					int			ps,
					int			lpW,
					int			width )
    {
    int		drawBorders= 0;

    static unsigned char	dash[]= { 3, 2 };
    static unsigned char	dot[]= { 1, 2 };
    static unsigned char	dashdot[]= { 2, 2, 1, 2 };
    static unsigned char	dashdotdot[]= { 2, 2, 1, 2, 1, 2 };

    switch( ps )
	{
	case PS_SOLID:
	    WMFDEB(appDebug( "SOLID\n" ));
	  solid:
	    drawBorders= 1;

	    appDrawSetLineAttributes( add,
			width, LINEstyleSOLID, LINEcapBUTT, LINEjoinMITER,
			(const unsigned char *)0, 0 );
	    break;

	case PS_INSIDEFRAME:
	    WMFDEB(appDebug( "INSIDEFRAME\n" ));

	    drawBorders= 1;

	    appDrawSetLineAttributes( add,
			width, LINEstyleSOLID, LINEcapBUTT, LINEjoinMITER,
			(const unsigned char *)0, 0 );
	    break;

	case PS_DASH:
	    WMFDEB(appDebug( "DASH\n" ));

	    if  ( lpW > 1 )
		{ goto solid;	}

	    drawBorders= 1;

	    appDrawSetLineAttributes( add,
			1, LINEstyleON_OFF_DASH, LINEcapBUTT, LINEjoinMITER,
			dash, sizeof( dash ) );
	    break;

	case PS_DOT:
	    WMFDEB(appDebug( "DOT\n" ));

	    if  ( lpW > 1 )
		{ goto solid;	}

	    drawBorders= 1;

	    appDrawSetLineAttributes( add,
			1, LINEstyleON_OFF_DASH, LINEcapBUTT, LINEjoinMITER,
			dot, sizeof( dot ) );
	    break;

	case PS_DASHDOT:
	    WMFDEB(appDebug( "DASHDOT\n" ));

	    if  ( lpW > 1 )
		{ goto solid;	}

	    drawBorders= 1;

	    appDrawSetLineAttributes( add,
			1, LINEstyleON_OFF_DASH, LINEcapBUTT, LINEjoinMITER,
			dashdot, sizeof( dashdot ) );
	    break;

	case PS_DASHDOTDOT:
	    WMFDEB(appDebug( "DASHDOTDOT\n" ));

	    if  ( lpW > 1 )
		{ goto solid;	}

	    drawBorders= 1;

	    appDrawSetLineAttributes( add,
			1, LINEstyleON_OFF_DASH, LINEcapBUTT, LINEjoinMITER,
			dashdotdot, sizeof( dashdotdot ) );
	    break;

	case PS_NULL:
	    WMFDEB(appDebug( "NULL\n" ));
	    drawBorders= 0;
	    break;

	default:
	    WMFDEB(appDebug( "????\n" ));
	    drawBorders= 0;
	    break;
	}


    *pDraw= drawBorders;
    return;
    }

static int appMetaSelectPenObjectX11(	DeviceContextX11 *	dcx,
					AppColors *		ac,
					int			ob )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    AppDrawingData *	add= &(dc->dcDrawingData);
    LogicalPen *	lp= &(dc->dcObjects[ob].mfoLogicalPen);

    WMFDEB(appDebug( "SelectObject( ob= %d ) PEN Width= %d ",
							ob, lp->lpWidth ));

    appMetaX11SetWindowsLineStyle( add,
			    &(dcx->dcxDeviceContext.dcDrawBorders),
			    lp->lpStyle, lp->lpWidth,
			    DCX_wViewport( lp->lpWidth, dcx ) );

    if  ( dcx->dcxDeviceContext.dcDrawBorders )
	{
	if  ( appColorRgb( &dcx->dcxPenColor, ac,
					    lp->lpColor.rgb8Red,
					    lp->lpColor.rgb8Green,
					    lp->lpColor.rgb8Blue ) )
		{ LDEB(1);  return -1;	}

	appDrawSetForegroundColor( add, &(dcx->dcxPenColor) );
	}

    dcx->dcxDeviceContext.dcPen= *lp;

    return 0;
    }

static int appMetaGetCountAndPointsX11(	DeviceContextX11 *	dcx,
					int *			pCount,
					SimpleInputStream *	sis,
					int			recordSize )
    {
    int				count;

    count= sioEndianGetLeInt16( sis );

    if  ( 2* count != recordSize- 4 )
	{ LLDEB(2*count,recordSize-4); return -1;	}

    if  ( appWinMetaGetPoints( &(dcx->dcxDeviceContext), count, sis ) )
	{ LDEB(count); return -1;	}

    *pCount= count; return 0;
    }

static int appMetaSelectObjectX11(	DeviceContextX11 *	dcx,
					AppColors *		ac,
					int			recordSize,
					SimpleInputStream *	sis )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);

    int			ob;

    ob= sioEndianGetLeInt16( sis );

    if  ( ob < 0 || ob >= dc->dcObjectCount )
	{ LLDEB(ob,dc->dcObjectCount); return -1;	}

    switch( dc->dcObjects[ob].mfoType )
	{
	case MFtypePEN:
	    if  ( appMetaSelectPenObjectX11( dcx, ac, ob ) )
		{ LDEB(ob); return -1;	}
	    break;
	case MFtypeBRUSH:
	    if  ( appMetaSelectBrushObjectX11( dcx, ac, ob ) )
		{ LDEB(ob); return -1;	}
	    break;
	case MFtypeFONT:
	    WMFDEB(appDebug( "SelectObject( ob= %d ) ", ob ));
	    WMFDEB(appDebug( "FONT\n" ));
	    if  ( appMetaSelectFontObjectX11( dcx, ac, ob ) )
		{ LDEB(ob); return -1;	}
	    break;
	case MFtypePATTERNBRUSH:
	    if  ( appMetaSelectPatternBrushObjectX11( dcx, ac, ob ) )
		{ LDEB(ob); return -1;	}
	    break;
	default:
	    WMFDEB(appDebug( "SelectObject( ob= %d ) ", ob ));
	    WMFDEB(appDebug( "type= %d\n", dc->dcObjects[ob].mfoType ));
	    break;
	}

    return 0;
    }

/************************************************************************/

static int appMetaBitmapImageX11(	AppBitmapImage **	pAbi,
					SimpleInputStream *	sis,
					int			expectBytes,
					DeviceContextX11 *	dcx,
					AppColors *		ac,
					int			srcX,
					int			srcY,
					int			srcXExt,
					int			srcYExt,
					int			dstX,
					int			dstY,
					int			dstXExt,
					int			dstYExt )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    AppDrawingData *	add= &(dc->dcDrawingData);

    AppBitmapImage *	abi;
    BitmapDescription *	bd;

    int			pixelsWide;
    int			pixelsHigh;

    APP_IMAGE *		xim= (APP_IMAGE *)0;

    if  ( expectBytes < 40 )
	{
	int	done= 0;

	/* LDEB(expectBytes); */

	while( done < expectBytes )
	    { sioInGetCharacter( sis ); done++;	}

	return 0;
	}

    if  ( srcX != 0 || srcY != 0 )
	{ LLDEB(srcX,srcY); 	}

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

    dstXExt= DC_wViewport( dstXExt, dc );
    dstYExt= DC_hViewport( dstYExt, dc );

    dstX= DCX_xViewport( dstX, dcx );
    dstY= DCX_yViewport( dstY, dcx );

    if  ( dstXExt < 0 )
	{ dstXExt= -dstXExt;	}
    if  ( dstYExt < 0 )
	{ dstYExt= -dstYExt;	}

    pixelsWide= dstXExt;
    pixelsHigh= dstYExt;

    if  ( appImgMakeImage( add, &xim, pixelsWide, pixelsHigh, ac, abi ) )
	{ LDEB(1); appCleanBitmapImage( abi ); free( abi ); return -1; }

#   ifdef USE_MOTIF
    XPutImage( add->addDisplay, add->addDrawable, add->addGc,
					    xim, srcX, srcY, dstX, dstY,
					    pixelsWide, pixelsHigh );
#   ifdef USE_MECHECK
    if  ( xim->data )
	{ free( xim->data ); xim->data= (char *)0; }
#   endif

    XDestroyImage( xim );
#   endif

#   ifdef USE_GTK
    gdk_draw_image( add->addDrawable, add->addGc,
					    xim, srcX, srcY, dstX, dstY,
					    pixelsWide, pixelsHigh );
    gdk_image_destroy( xim );
#   endif

    if  ( dstX == 0				&&
	  dstY == 0				&&
	  dstXExt == dcx->dcxPixelsWide		&&
	  dstYExt == dcx->dcxPixelsHigh		)
	{
	bd= &(abi->abiBitmap);
	bd->bdUnit= BMunINCH;
	bd->bdXResolution= ( 72* 20* bd->bdPixelsWide )/ dc->dcTwipsWide;
	bd->bdYResolution= ( 72* 20* bd->bdPixelsHigh )/ dc->dcTwipsHigh;

	if  ( *pAbi )
	    { appCleanBitmapImage( *pAbi ); free( *pAbi ); }

	*pAbi= abi;
	}
    else{
	/*
	LLDEB(dstX,dstY);
	LLDEB(dstXExt,dcx->dcxPixelsWide);
	LLDEB(dstYExt,dcx->dcxPixelsHigh);
	*/
	appCleanBitmapImage( abi ); free( abi );
	}

    return 0;
    }

static int appMeta_RoundRectX11(	SimpleInputStream *	sis,
					int			recordSize,
					DeviceContextX11 *	dcx )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    AppDrawingData *	add= &(dc->dcDrawingData);

    int		y0;
    int		x0;
    int		y1;
    int		x1;

    int		h;
    int		w;

    h= sioEndianGetLeInt16( sis );
    w= sioEndianGetLeInt16( sis );

    y1= sioEndianGetLeInt16( sis );
    x1= sioEndianGetLeInt16( sis );
    y0= sioEndianGetLeInt16( sis );
    x0= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("RoundRect(%d..%d,%d..%d, w= %d,h= %d\n",
					    x0,x1,y0,y1, w, h ));

    y0= DCX_yViewport( y0, dcx );
    x0= DCX_xViewport( x0, dcx );
    y1= DCX_yViewport( y1, dcx );
    x1= DCX_xViewport( x1, dcx );

    h= DCX_hViewport( h, dcx );
    w= DCX_wViewport( w, dcx );

    if  ( appMetafileDrawRoundRectX11( add, x0, y0, x1, y1, w, h,
				dc->dcFillInsides, dc->dcDrawBorders,
				&(dcx->dcxBrushColor), &(dcx->dcxPenColor) ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static void appMetaGetArcX11(	SimpleInputStream *	sis,
				DeviceContextX11 *	dcx,
				char *			fun,
				int *			pY0,
				int *			pX0,
				int *			pY1,
				int *			pX1,
				int *			pa1,
				int *			pa2 )

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
    y0= DCX_yViewport( y0, dcx );
    x0= DCX_xViewport( x0, dcx );
    y1= DCX_yViewport( y1, dcx );
    x1= DCX_xViewport( x1, dcx );

    ys= DCX_yViewport( ys, dcx );
    xs= DCX_xViewport( xs, dcx );
    ye= DCX_yViewport( ye, dcx );
    xe= DCX_xViewport( xe, dcx );

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

    as= ( 180* as )/ M_PI;
    ae= ( 180* ae )/ M_PI;

    if  ( ae < as )
	{ ae += 360;	}

    as= 64* as;
    ae= 64* ae;

    ae -= as;

    *pY0= y0;
    *pX0= x0;
    *pY1= y1;
    *pX1= x1;
    *pa1= (int)as;
    *pa2= (int)ae;

    return;
    }

static int appMeta_ArcX11(	SimpleInputStream *	sis,
				int			recordSize,
				DeviceContextX11 *	dcx )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    AppDrawingData *	add= &(dc->dcDrawingData);

    int			y0;
    int			x0;
    int			y1;
    int			x1;

    int			a1;
    int			a2;

    appMetaGetArcX11( sis, dcx, "Arc", &y0, &x0, &y1, &x1, &a1, &a2 );

    if  ( dc->dcDrawBorders )
	{
	appDrawSetForegroundColor( add, &(dcx->dcxPenColor) );

	appDrawDrawArc( add, x0, y0, x1- x0, y1- y0, a1, a2 );
	}

    return 0;
    }

static int appMeta_ChordX11(	SimpleInputStream *	sis,
				int			recordSize,
				DeviceContextX11 *	dcx )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    AppDrawingData *	add= &(dc->dcDrawingData);

    int			y0;
    int			x0;
    int			y1;
    int			x1;

    int			a1;
    int			a2;

    appMetaGetArcX11( sis, dcx, "Chord", &y0, &x0, &y1, &x1, &a1, &a2 );

    if  ( dc->dcDrawBorders )
	{
	appDrawSetForegroundColor( add, &(dcx->dcxPenColor) );

	appDrawDrawArc( add, x0, y0, x1- x0, y1- y0, a1, a2 );
	}

    return 0;
    }

static int appMeta_PieX11(	SimpleInputStream *	sis,
				int			recordSize,
				DeviceContextX11 *	dcx )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    AppDrawingData *	add= &(dc->dcDrawingData);

    int			y0;
    int			x0;
    int			y1;
    int			x1;

    int			a1;
    int			a2;

    appMetaGetArcX11( sis, dcx, "Pie", &y0, &x0, &y1, &x1, &a1, &a2 );

    if  ( dc->dcFillInsides )
	{
	appDrawSetForegroundColor( add, &(dcx->dcxBrushColor) );

	appDrawFillArc( add, x0, y0, x1- x0, y1- y0, a1, a2 );
	}

    if  ( dc->dcDrawBorders )
	{
	appDrawSetForegroundColor( add, &(dcx->dcxPenColor) );

	appDrawDrawArc( add, x0, y0, x1- x0, y1- y0, a1, a2 );
	}

    return 0;
    }

static int appMeta_EllipseX11(	SimpleInputStream *	sis,
				int			recordSize,
				DeviceContextX11 *	dcx )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    AppDrawingData *	add= &(dc->dcDrawingData);

    int			y0;
    int			x0;
    int			y1;
    int			x1;

    int			swap;

    y1= sioEndianGetLeInt16( sis );
    x1= sioEndianGetLeInt16( sis );
    y0= sioEndianGetLeInt16( sis );
    x0= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("Ellipse(%d..%d,%d..%d)\n", x0,x1,y0,y1));

    x0= DCX_xViewport( x0, dcx );
    y0= DCX_yViewport( y0, dcx );
    x1= DCX_xViewport( x1, dcx );
    y1= DCX_yViewport( y1, dcx );

    if  ( x1 < x0 )
	{ swap= x0; x0= x1; x1= swap; }
    if  ( y1 < y0 )
	{ swap= y0; y0= y1; y1= swap; }

    if  ( dc->dcFillInsides )
	{
	appDrawSetForegroundColor( add, &(dcx->dcxBrushColor) );

	appDrawFillArc( add, x0, y0, x1- x0, y1- y0, 64* 0, 64* 360 );
	}

    if  ( dc->dcDrawBorders )
	{
	appDrawSetForegroundColor( add, &(dcx->dcxPenColor) );

	appDrawDrawArc( add, x0, y0, x1- x0, y1- y0, 64* 0, 64* 360 );
	}

    return 0;
    }

static void appMeta_SetFillX11(		DeviceContextX11 *	dcx,
					int			before )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    AppDrawingData *	add= &(dc->dcDrawingData);

    if  ( before )
	{ appDrawSetForegroundColor( add, &(dcx->dcxBrushColor) );	}

    if  ( dcx->dcxFillTiled && before )
	{
#	ifdef USE_MOTIF
	XSetFillStyle( add->addDisplay, add->addGc, FillTiled );
#	endif
#	ifdef USE_GTK
	gdk_gc_set_fill( add->addGc, GDK_TILED );
#	endif
	}

    if  ( dcx->dcxFillTiled && ! before )
	{
#	ifdef USE_MOTIF
	XSetFillStyle( add->addDisplay, add->addGc, FillSolid );
#	endif
#	ifdef USE_GTK
	gdk_gc_set_fill( add->addGc, GDK_SOLID );
#	endif
	}

    return;
    }

static int appMeta_RectangleX11(	SimpleInputStream *	sis,
					DeviceContextX11 *	dcx )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    AppDrawingData *	add= &(dc->dcDrawingData);
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

    x0= DCX_xViewport( x0, dcx );
    y0= DCX_yViewport( y0, dcx );
    x1= DCX_xViewport( x1, dcx );
    y1= DCX_yViewport( y1, dcx );

    if  ( x1 < x0 )
	{ swap= x0; x0= x1; x1= swap; }
    if  ( y1 < y0 )
	{ swap= y0; y0= y1; y1= swap; }

    if  ( dc->dcFillInsides )
	{
	appMeta_SetFillX11( dcx, 1 );

	appDrawFillRectangle( add, x0, y0, x1- x0, y1- y0 );

	appMeta_SetFillX11( dcx, 0 );
	}

    if  ( lp->lpStyle == PS_INSIDEFRAME )
	{ x1 -= 2; y1 -= 2; }

    if  ( dc->dcDrawBorders )
	{
	appDrawSetForegroundColor( add, &(dcx->dcxPenColor) );

	appDrawDrawRectangle( add, x0, y0, x1- x0, y1- y0 );
	}

    return 0;
    }

static int appMeta_PolyLineX11(	SimpleInputStream *	sis,
				int			recordSize,
				DeviceContextX11 *	dcx )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    AppDrawingData *	add= &(dc->dcDrawingData);

    int			count;

    if  ( appMetaGetCountAndPointsX11( dcx, &count, sis, recordSize ) )
	{ LDEB(recordSize); return -1;	}

    WMFDEB(appDebug("PolyLine( count=%d, ... )\n", count ));

    appDrawSetForegroundColor( add, &(dcx->dcxPenColor) );

    appDrawDrawLines( add, dc->dcPoints, count );

    return 0;
    }

static int appMeta_PolygonX11(	SimpleInputStream *	sis,
				int			recordSize,
				DeviceContextX11 *	dcx )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    AppDrawingData *	add= &(dc->dcDrawingData);

    int			count;

    if  ( appMetaGetCountAndPointsX11( dcx, &count, sis, recordSize ) )
	{ LDEB(recordSize); return -1;	}

    WMFDEB(appDebug("Polygon( count=%d, ... ) Fill= %d Borders= %d\n",
			    count, dc->dcFillInsides, dc->dcDrawBorders ));

#   if 1
    {
    int	i;

    for ( i= 0; i < count; i++ )
	{
	WMFDEB(appDebug(" %3d: [%3d,%3d]\n",
			   i,  dc->dcPoints[i].x, dc->dcPoints[i].y ));
	}
    }
#   endif

    if  ( dc->dcFillInsides )
	{
	appMeta_SetFillX11( dcx, 1 );

	appDrawFillPolygon( add, dc->dcPoints, count );

	appMeta_SetFillX11( dcx, 0 );
	}

    if  ( dc->dcDrawBorders )
	{
	appDrawSetForegroundColor( add, &(dcx->dcxPenColor) );

	appDrawDrawLines( add, dc->dcPoints, count+ 1 );
	}

    return 0;
    }

static void appMetaDrawStringX11(	int			x0,
					int			y0,
					int			count,
					DeviceContextX11 *	dcx )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    AppDrawingData *	add= &(dc->dcDrawingData);
    DrawScreenFont *	apf= dcx->dcxPhysicalFont;

    int			wide;
    int			fontAscent;
    int			fontDescent;

    if  ( ! apf )
	{ XDEB(apf); return;	}

    appDrawSetForegroundColor( add, &(dcx->dcxTextColor) );

    appDrawTextExtents( &wide, &fontAscent, &fontDescent,
		add, apf->apfFontStruct, (const char *)dc->dcString, count );

    switch( dc->dcTextAlignment & 0x01 )
	{
	case TA_NOUPDATECP:
	    break;

	case TA_UPDATECP:
	    x0= DCX_xViewport( dc->dcX, dcx );
	    y0= DCX_yViewport( dc->dcY, dcx );
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
	    x0 -= wide;
	    break;

	case TA_CENTER:
	    x0 -= wide/ 2;
	    break;

	default:
	    XDEB(dc->dcTextAlignment & 0x06);
	    break;
	}

    switch( dc->dcTextAlignment & 0x18 )
	{
	case TA_TOP:
	    y0 += (int)( apf->apfFontStruct->ascent );
	    break;

	case TA_BOTTOM:
	    y0 -= (int)( apf->apfFontStruct->descent );
	    break;

	case TA_BASELINE:
	    break;

	default:
	    WMFDEB(appDebug(" 0x%x", dc->dcTextAlignment & 0x18 ));
	    break;
	}

    appDrawDrawString( add, x0, y0, (char *)dc->dcString, count );

    if  ( dc->dcFont.lfTextAttribute.taTextIsUnderlined )
	{
	appDrawFillRectangle( add,
				x0, y0+ apf->apfUnderLinePositionPixels,
				wide, apf->apfUnderlineThicknessPixels );
	}

    switch( dc->dcTextAlignment & 0x01 )
	{
	case TA_NOUPDATECP:
	    break;

	case TA_UPDATECP:
	    /*
	    XDEB(dc->dcTextAlignment & 0x01 );
	    */
	    break;

	default:
	    XDEB(dc->dcTextAlignment & 0x01 );
	    break;
	}

    return;
    }

static int appMeta_ExtTextOutX11(	SimpleInputStream *	sis,
					int			recordSize,
					DeviceContextX11 *	dcx )
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
			    &x1, &y1, &w1, &h1, &(dcx->dcxDeviceContext) ) )
	{ LDEB(1); return -1;	}

    x0= DCX_xViewport( x0, dcx );
    y0= DCX_yViewport( y0, dcx );

    x1= DCX_xViewport( x1, dcx );
    y1= DCX_yViewport( y1, dcx );

    w1= DCX_wViewport( w1, dcx );
    h1= DCX_hViewport( h1, dcx );

    appMetaDrawStringX11( x0, y0, count, dcx );

    return 0;
    }

static int appMeta_TextOutX11(	SimpleInputStream *	sis,
				int			recordSize,
				DeviceContextX11 *	dcx )
    {
    int		x0;
    int		y0;
    int		count;

    if  ( appMeta_TextOut( sis, recordSize,
				&x0, &y0, &count, &(dcx->dcxDeviceContext) ) )
	{ LDEB(1); return -1;	}

    x0= DCX_xViewport( x0, dcx );
    y0= DCX_yViewport( y0, dcx );

    appMetaDrawStringX11( x0, y0, count, dcx );

    return 0;
    }

static int appMeta_LineToX11(	SimpleInputStream *	sis,
				int			recordSize,
				DeviceContextX11 *	dcx )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    AppDrawingData *	add= &(dc->dcDrawingData);

    int			x0= dc->dcX;
    int			y0= dc->dcY;
    int			y1;
    int			x1;

    y1= sioEndianGetLeInt16( sis );
    x1= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("LineTo( x:%d->%d, y:%d->%d )\n", x0, x1, y0, y1 ));

    dc->dcX= x1;
    dc->dcY= y1;

    x0= DCX_xViewport( x0, dcx );
    y0= DCX_yViewport( y0, dcx );
    x1= DCX_xViewport( x1, dcx );
    y1= DCX_yViewport( y1, dcx );

    appDrawSetForegroundColor( add, &(dcx->dcxPenColor) );

    appDrawDrawLine( add, x0, y0, x1, y1 );

    return 0;
    }

static int appMeta_PatBltX11(	SimpleInputStream *	sis,
				int			recordSize,
				DeviceContextX11 *	dcx )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    AppDrawingData *	add= &(dc->dcDrawingData);

    long		rop;

    int			h0;
    int			w0;
    int			y0;
    int			x0;

    int			x1;
    int			y1;

    int			swap;

    rop= sioEndianGetLeInt32( sis );
    h0= sioEndianGetLeInt16( sis );
    w0= sioEndianGetLeInt16( sis );
    y0= sioEndianGetLeInt16( sis );
    x0= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug("PatBlt([%d+%d,%d+%d], rop=0x%lx )\n", x0,w0,y0,h0, rop ));

    x1= x0+ w0;
    y1= y0+ h0;

    x0= DCX_xViewport( x0, dcx );
    y0= DCX_yViewport( y0, dcx );

    x1= DCX_xViewport( x1, dcx );
    y1= DCX_yViewport( y1, dcx );

    if  ( x1 < x0 )
	{ swap= x0; x0= x1; x1= swap; }
    if  ( y1 < y0 )
	{ swap= y0; y0= y1; y1= swap; }

    if  ( dc->dcFillInsides )
	{
	appDrawSetForegroundColor( add, &(dcx->dcxBrushColor) );

	appDrawFillRectangle( add, x0, y0, x1- x0, y1- y0 );
	}

    return 0;
    }

static int appMeta_SetPolyFillModeX11(	SimpleInputStream *	sis,
					int			recordSize,
					DeviceContextX11 *	dcx )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    AppDrawingData *	add= &(dc->dcDrawingData);
    int			mode;

    if  ( recordSize != 4 )
	{ LDEB(recordSize); return -1;	}

    mode= sioEndianGetLeInt16( sis );

    switch( mode )
	{
	case ALTERNATE:
	    WMFDEB(appDebug("SetPolyFillMode( ALTERNATE= %d )\n", mode ));

#	    ifdef USE_MOTIF
	    XSetFillRule( add->addDisplay, add->addGc, EvenOddRule );
#	    endif
#	    ifdef USE_GTK
	    appDrawGtkSetXFillRule( add->addGc, GDK_EVEN_ODD_RULE );
#	    endif
	    break;

	case WINDING:
	    WMFDEB(appDebug("SetPolyFillMode( WINDING= %d )\n", mode ));

#	    ifdef USE_MOTIF
	    XSetFillRule( add->addDisplay, add->addGc, WindingRule );
#	    endif
#	    ifdef USE_GTK
	    appDrawGtkSetXFillRule( add->addGc, GDK_WINDING_RULE );
#	    endif
	    break;

	default:
	    WMFDEB(appDebug("SetPolyFillMode( %d ) ??\n", mode ));
	    break;
	}

    return 0;
    }

static int appMeta_PolyPolygonX11(	SimpleInputStream *	sis,
					int			recordSize,
					DeviceContextX11 *	dcx )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    AppDrawingData *	add= &(dc->dcDrawingData);

    int			count;
    int			i;

    int *		counts;

    if  ( appMeta_GetCounts( sis, recordSize, &count, &counts,
						&(dcx->dcxDeviceContext ) ) )
	{ LDEB(1); return -1;	}

    WMFDEB(appDebug("PolyPolygon( count=%d, ... )\n", count ));

    for ( i= 0; i < count; i++ )
	{
	if  ( appWinMetaGetPoints( dc, counts[i], sis ) )
	    { LDEB(counts[i]); return -1;	}

#	if 1
	if  ( dc->dcFillInsides )
	    {
	    appDrawSetForegroundColor( add, &(dcx->dcxBrushColor) );

	    appDrawFillPolygon( add, dc->dcPoints, counts[i] );
	    }
#	endif

	if  ( dc->dcDrawBorders )
	    {
	    appDrawSetForegroundColor( add, &(dcx->dcxPenColor) );

	    appDrawDrawLines( add, dc->dcPoints, counts[i]+ 1 );
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Fill the background of the pixmap.					*/
/*									*/
/************************************************************************/

static void appMetaFillBackgroundX11(	DeviceContextX11 *	dcx )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    AppDrawingData *	add= &(dc->dcDrawingData);

    appDrawSetForegroundWhite( add );
    appDrawFillRectangle( add, 0, 0, dcx->dcxPixelsWide, dcx->dcxPixelsHigh );

    return;
    }

/************************************************************************/
/*									*/
/*  Play the metafile records in a metafile.				*/
/*									*/
/************************************************************************/

static int appWinMetaPlayRecords(	SimpleInputStream *	sis,
					void **			pPrivate,
					AppColors *		ac,
					DeviceContextX11 *	dcx )
    {
    DeviceContext *	dc= &(dcx->dcxDeviceContext);
    AppBitmapImage *	abi= (AppBitmapImage *)0;

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

	int		ob;

	recordSize= sioEndianGetLeInt32( sis );
	function= sioEndianGetLeInt16( sis );

	if  ( recordSize < 0 || function < 0 )
	    { LLDEB(recordSize,function); return -1;	}

	switch( function )
	    {
	    case WINMETA_OffsetWindowOrg:
		if  ( appMetaOffsetWindowOrg( dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_SetWindowOrg:
		if  ( appMetaSetWindowOrg( dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_SetWindowExt:
		if  ( appMetaSetWindowExt( dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_SetViewportOrg:
		if  ( appMetaSetViewportOrg( dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_SetViewportExt:
		if  ( appMetaSetViewportExt( dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_SaveDC:
		if  ( appMetaSaveDC( dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_RestoreDC:
		if  ( appMetaRestoreDC( dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_OffsetClipRgn:
		if  ( appMetaOffsetClipRgn( dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_SelectClipRgn:
		if  ( appMetaSelectClipRgn( dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_ExcludeClipRect:
		if  ( appMetaExcludeClipRect( dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_IntersectClipRect:
		if  ( appMetaIntersectClipRect( dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_SetMapperFlags:
		rop= sioEndianGetLeInt32( sis );
		WMFDEB(appDebug("WINMETA_SetMapperFlags(0x%lx)\n",rop));
		continue;

	    case WINMETA_BitBlt:
		rop= sioEndianGetLeInt32( sis );
		/*  source	*/
		y0= sioEndianGetLeInt16( sis );
		x0= sioEndianGetLeInt16( sis );
		/*  destination	*/
		h1= sioEndianGetLeInt16( sis );
		w1= sioEndianGetLeInt16( sis );
		y1= sioEndianGetLeInt16( sis );
		x1= sioEndianGetLeInt16( sis );

		h0= h1;
		w0= w1;

		WMFDEB(appDebug("BitBlt([%d+%d,%d+%d]->[%d+%d,%d+%d],..)\n",
				x0,w0,y0,h0, x1,w1,y1,h1 ));

		if  ( appMetaBitmapImageX11( &abi, sis, 2*(recordSize-3-2-6*1),
			    dcx, ac, x0, y0, w0, h0, x1, y1, w1, h1 ) )
		    { LDEB(1); return -1; }

		continue;

	    case WINMETA_StretchBlt:
		rop= sioEndianGetLeInt32( sis );
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

		WMFDEB(appDebug("StretchBlt([%d+%d,%d+%d]->[%d+%d,%d+%d],..)\n",
				x0,w0,y0,h0, x1,w1,y1,h1 ));

		if  ( appMetaBitmapImageX11( &abi, sis, 2*(recordSize-3-2-8*1),
			    dcx, ac, x0, y0, w0, h0, x1, y1, w1, h1 ) )
		    { LDEB(1); return -1; }

		continue;

	    case WINMETA_StretchDIBits:
		rop= sioEndianGetLeInt32( sis );
		(void) sioEndianGetLeInt16( sis );
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

		if  ( appMetaBitmapImageX11( &abi, sis, 2*(recordSize-3-2-9*1),
			    dcx, ac, x0, y0, w0, h0, x1, y1, w1, h1 ) )
		    { LDEB(1); return -1; }

		continue;

	    case WINMETA_SelectObject:
		if  ( appMetaSelectObjectX11( dcx, ac, recordSize, sis ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case WINMETA_CreateBrushIndirect:
		if  ( appMetaCreateBrushIndirect( dc, recordSize, sis ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case WINMETA_CreatePenIndirect:
		if  ( appMetaCreatePenIndirect( dc, recordSize, sis ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case WINMETA_CreatePatternBrush:
		if  ( appMetaCreatePatternBrush( dc, recordSize, sis ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case WINMETA_CreatePalette:
		if  ( appMetaCreatePalette( dc, recordSize, sis ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case WINMETA_CreateFontIndirect:
		if  ( appMetaCreateFontIndirect( dc, recordSize, sis ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_DeleteObject:
		ob= sioEndianGetLeInt16( sis );

		if  ( ob < 0 || ob >= dc->dcObjectCount )
		    { LLDEB(ob,dc->dcObjectCount); return -1;	}

		appMetaCleanObjectX11( dcx, &(dc->dcObjects[ob]) );
		appMetaCleanObject( &(dc->dcObjects[ob]) );

		dc->dcObjects[ob].mfoType= MFtypeFREE;

		continue;

	    case WINMETA_SetPolyFillMode:
		if  ( appMeta_SetPolyFillModeX11( sis, recordSize, dcx ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case WINMETA_SetBkColor:
		if  ( appMetaSetBkColor( &(dcx->dcxDeviceContext),
							recordSize, sis ) )
		    { LDEB(1); return -1;	}

		if  ( appColorRgb( &dcx->dcxBkColor, ac,
			    dcx->dcxDeviceContext.dcBkColor.rgb8Red,
			    dcx->dcxDeviceContext.dcBkColor.rgb8Green,
			    dcx->dcxDeviceContext.dcBkColor.rgb8Blue ) )
		    { LDEB(1);  return -1;	}

		continue;

	    case WINMETA_SetTextColor:
		if  ( appMetaSetTextColor( &(dcx->dcxDeviceContext),
							recordSize, sis ) )
		    { LDEB(1); return -1;	}

		if  ( appColorRgb( &dcx->dcxTextColor, ac,
			    dcx->dcxDeviceContext.dcTextColor.rgb8Red,
			    dcx->dcxDeviceContext.dcTextColor.rgb8Green,
			    dcx->dcxDeviceContext.dcTextColor.rgb8Blue ) )
		    { LDEB(1);  return -1;	}

		continue;

	    case WINMETA_SetBkMode:
		WMFDEB(appDebug("SetBkMode(...)\n"));
		if  ( recordSize == 4 )
		    {
		    dcx->dcxDeviceContext.dcBkMode= sioEndianGetLeInt16( sis );
		    continue;
		    }
		if  ( recordSize == 5 )
		    {
		    dcx->dcxDeviceContext.dcBkMode= sioEndianGetLeInt32( sis );
		    continue;
		    }
		XLDEB(function,recordSize); return -1;

	    case WINMETA_SetTextCharExtra:
		WMFDEB(appDebug("SetTextCharExtra(...)\n"));
		dcx->dcxDeviceContext.dcExtraTextSpacing=
						sioEndianGetLeInt16( sis );
		continue;

	    case WINMETA_SetTextAlign:
		if  ( appMetaSetTextAlign( dc, recordSize, sis ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case WINMETA_SetTextJustification:
		WMFDEB(appDebug("SetTextJustification(...)\n"));

		dcx->dcxDeviceContext.dcJustificationAmount=
						sioEndianGetLeInt16( sis );
		dcx->dcxDeviceContext.dcJustificationSpaces=
						sioEndianGetLeInt16( sis );
		continue;

	    case WINMETA_SetStretchBltMode:
		switch( recordSize )
		    {
		    case 4:
			dcx->dcxDeviceContext.dcStretchBltMode=
						    sioEndianGetLeInt16( sis );
			break;
		    case 5:
			dcx->dcxDeviceContext.dcStretchBltMode=
						    sioEndianGetLeInt32( sis );
			break;
		    default:
			LDEB(recordSize); return -1;
		    }
		WMFDEB(appDebug("SetStretchBltMode(%ld)\n",
				    dcx->dcxDeviceContext.dcStretchBltMode));
		continue;

	    case WINMETA_PolyLine:
		if  ( appMeta_PolyLineX11( sis, recordSize, dcx ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_Polygon:
		if  ( appMeta_PolygonX11( sis, recordSize, dcx ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_Rectangle:
		if  ( appMeta_RectangleX11( sis, dcx ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_LineTo:
		if  ( appMeta_LineToX11( sis, recordSize, dcx ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_MoveTo:
		y0= sioEndianGetLeInt16( sis );
		x0= sioEndianGetLeInt16( sis );

		WMFDEB(appDebug("MoveTo( x=%d, y=%d )\n", x0, y0 ));

		dcx->dcxDeviceContext.dcX= x0;
		dcx->dcxDeviceContext.dcY= y0;

		continue;

	    case WINMETA_ExtTextOut:
		if  ( appMeta_ExtTextOutX11( sis, recordSize, dcx ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case WINMETA_TextOut:
		if  ( appMeta_TextOutX11( sis, recordSize, dcx ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case WINMETA_Escape:
		WMFDEB(appDebug("Escape(...)\n"));
		goto skipArguments;

	    case WINMETA_PatBlt:
		if  ( appMeta_PatBltX11( sis, recordSize, dcx ) )
		    { LDEB(recordSize); return -1;	}
		continue;

	    case WINMETA_RoundRect:
		if  ( appMeta_RoundRectX11( sis, recordSize, dcx ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_Pie:
		if  ( appMeta_PieX11( sis, recordSize, dcx ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_Arc:
		if  ( appMeta_ArcX11( sis, recordSize, dcx ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_Chord:
		if  ( appMeta_ChordX11( sis, recordSize, dcx ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_Ellipse:
		if  ( appMeta_EllipseX11( sis, recordSize, dcx ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_SetMapMode:
		appMetaSetMapMode( dc, recordSize, sis );
		continue;

	    case WINMETA_SetROP2:
		if  ( recordSize == 5 )
		    {
		    dcx->dcxDeviceContext.dcROP2= sioEndianGetLeInt32( sis );
		    WMFDEB(appDebug("SetROP2(%ld)\n",
					(long)dcx->dcxDeviceContext.dcROP2 ));
		    continue;
		    }
		if  ( recordSize == 4 )
		    {
		    dcx->dcxDeviceContext.dcROP2= sioEndianGetLeInt16( sis );
		    WMFDEB(appDebug("SetROP2(%ld)\n",
					(long)dcx->dcxDeviceContext.dcROP2 ));
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
		if  ( appMeta_PolyPolygonX11( sis, recordSize, dcx ) )
		    { LDEB(1); return -1;	}
		continue;

	    case WINMETA_ScaleViewportExt:
		appMetaScaleViewportExt( dc, recordSize, sis );
		continue;

	    case WINMETA_ScaleWindowExt:
		appMetaScaleWindowExt( dc, recordSize, sis );
		continue;

	    case 0:
		if  ( recordSize != 3 )
		    { XLDEB(function,recordSize); return -1;	}
		WMFDEB(appDebug("READY\n"));
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

    *pPrivate= (void *)abi;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Play the contents of a 'wmf' original windows metafile.		*/
/*									*/
/************************************************************************/

int appMetaPlayWmfX11(	SimpleInputStream *	sis,
			void **			pPrivate,
			AppColors *		ac,
			AppDrawingData *	parent_add,
			APP_WINDOW		pixmap,
			int			mapMode,
			int			xWinExt,
			int			yWinExt,
			int			pixelsWide,
			int			pixelsHigh,
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

    double		magnification;
    double		pixelsPerTwip;

    DeviceContextX11	dcx;

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

    appMetaInitDeviceContextX11( &dcx, parent_add->addPostScriptFontList,
						    objectCount, mapMode,
						    xWinExt, yWinExt,
						    twipsWide, twipsHigh,
						    pixelsWide, pixelsHigh );

    pixelsPerTwip= sqrt( (double)( pixelsWide* pixelsHigh )/
						( twipsWide* twipsHigh ) );
    magnification= pixelsPerTwip/ parent_add->addMagnifiedPixelsPerTwip;

    appCloneDrawingEnvironment( &(dcx.dcxDeviceContext.dcDrawingData),
			parent_add, magnification, pixelsPerTwip, pixmap );

    if  ( appMetaSetDeviceDefaultsX11( &dcx, ac ) )
	{ LDEB(1); return -1;	}

    dcx.dcxFillTiled= 0;
    dcx.dcxFillHatched= 0;

    appMetaFillBackgroundX11( &dcx );

    WMFDEB(appDebug(
		"X11 WMF winExt: [%dx%d], pix: [%dx%d], twips: [%dx%d]\n",
						    xWinExt, yWinExt,
						    pixelsWide, pixelsHigh,
						    twipsWide, twipsHigh ));

    if  ( appWinMetaPlayRecords( sis, pPrivate, ac, &dcx ) )
	{
	LDEB(1);
	appMetaCleanDeviceContextX11( &dcx );
	return -1;
	}

    appMetaCleanDeviceContextX11( &dcx );

    return 0;
    }

int appMetaPlayEmfX11(	SimpleInputStream *	sis,
			void **			pPrivate,
			AppColors *		ac,
			AppDrawingData *	parent_add,
			APP_BITMAP_IMAGE	pixmap,
			int			mapMode,
			int			xExt,
			int			yExt,
			int			pixelsWide,
			int			pixelsHigh,
			int			twipsWide,
			int			twipsHigh )
    {
    unsigned long	recordType;
    unsigned long	recordSize;
    long		boundsLeft;
    long		boundsRight;
    long		boundsTop;
    long		boundsBottom;
    long		frameLeft;
    long		frameRight;
    long		frameTop;
    long		frameBottom;
    unsigned long	signature;
    unsigned long	version;
    unsigned long	size;
    unsigned long	numOfRecords;
    unsigned int	numOfHandles;
    unsigned int	reserved;
    unsigned long	sizeOfDescrip;
    unsigned long	offsOfDescrip;
    unsigned long	numPalEntries;
    long		widthDevPixels;
    long		heightDevPixels;
    long		widthDevMM;
    long		heightDevMM;

    int			done= 0;

    /*
    if  ( sioInGetCharacter( sis ) == EOF )
	{ return 0;	}
    sioInUngetLastRead( sis );
    */

    recordType= sioEndianGetLeUint32( sis ); done += 4;
    recordSize= sioEndianGetLeUint32( sis ); done += 4;

    boundsLeft= sioEndianGetLeInt32( sis ); done += 4;
    boundsRight= sioEndianGetLeInt32( sis ); done += 4;
    boundsTop= sioEndianGetLeInt32( sis ); done += 4;
    boundsBottom= sioEndianGetLeInt32( sis ); done += 4;

    frameLeft= sioEndianGetLeInt32( sis ); done += 4;
    frameRight= sioEndianGetLeInt32( sis ); done += 4;
    frameTop= sioEndianGetLeInt32( sis ); done += 4;
    frameBottom= sioEndianGetLeInt32( sis ); done += 4;

    signature= sioEndianGetLeUint32( sis ); done += 4;
    version= sioEndianGetLeUint32( sis ); done += 4;
    size= sioEndianGetLeUint32( sis ); done += 4;
    numOfRecords= sioEndianGetLeUint32( sis ); done += 4;

    numOfHandles= sioEndianGetLeUint16( sis ); done += 2;
    reserved= sioEndianGetLeUint16( sis ); done += 2;

    sizeOfDescrip= sioEndianGetLeUint32( sis ); done += 4;
    offsOfDescrip= sioEndianGetLeUint32( sis ); done += 4;
    numPalEntries= sioEndianGetLeUint32( sis ); done += 4;

    widthDevPixels= sioEndianGetLeInt32( sis ); done += 4;
    heightDevPixels= sioEndianGetLeInt32( sis ); done += 4;
    widthDevMM= sioEndianGetLeInt32( sis ); done += 4;
    heightDevMM= sioEndianGetLeInt32( sis ); done += 4;

    /*********/

    if  ( recordType != 1 )
	{ LLDEB(recordType,1); return -1;	}
    if  ( signature != 0x464D4520 )
	{ XXDEB(signature,0x464D4520); return -1;	}
    if  ( version != 0x00000100 )
	{ XXDEB(version,0x00000100);	}

    while ( done < recordSize )
	{ sioInGetCharacter( sis ); done++;	}

    /*********/

    LDEB(1); return -1;
    }

