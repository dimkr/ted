#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<appMacPict.h>
#   include	<utilPs.h>
#   include	<sioEndian.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<appDebugon.h>

# if 0
#    define	PICTDEB(x)	(x)
# else
#    define	PICTDEB(x)	/*nothing*/
# endif

/************************************************************************/
/*									*/
/*  Set the current color (to draw)					*/
/*									*/
/************************************************************************/

static int appMacPictSetColorX11(	MacpictDevice *		md,
					AppColors *		ac,
					RGB8Color *		rgb8 )
    {
    if  ( bmRGB8ColorsDiffer( &(md->mdColorSet), rgb8 ) )
	{
	AppDrawingData *	add= &(md->mdDrawingData);

	appColorRgb( &(md->mdDeviceColorSet), ac,
			    rgb8->rgb8Red, rgb8->rgb8Green, rgb8->rgb8Blue );

	appDrawSetForegroundColor( add, &(md->mdDeviceColorSet) );

	md->mdColorSet= *rgb8;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Fill the background of the pixmap.					*/
/*									*/
/************************************************************************/

static void appMacPictFillBackgroundX11(	MacpictDevice *	md )
    {
    AppDrawingData *	add= &(md->mdDrawingData);

    appDrawSetForegroundWhite( add );

    appDrawFillRectangle( add, 0, 0, md->mdOutWide, md->mdOutHigh );

    return;
    }

/************************************************************************/
/*									*/
/*  Set a (bitmap) pattern to draw with.				*/
/*									*/
/*  1)  Nothing to be done, but not for solid.				*/
/*  2)  Discard previous pattern.					*/
/*  3)  Solid pattern.							*/
/*  4)  1=BLACK= (foreground), 0=WHITE= (background)			*/
/*  5)  Make pixmap.							*/
/*									*/
/************************************************************************/

static int appMacPictSetStippleX11(	MacpictDevice *		md,
					AppColors *		ac,
					unsigned char		pattern[8],
					int			which )
    {
    AppDrawingData *	add= &(md->mdDrawingData);

    /*  1  */
    if  ( which != STIPPLE_SOLID && md->mdStippleSet == which )
	{ return 0;	}

    /*  2  */
    if  ( md->mdTilePixmap )
	{
	appDrawFreePixmap( add, md->mdTilePixmap );
	md->mdTilePixmap= (APP_BITMAP_IMAGE)0;
	}

    /*  3  */
    if  ( which == STIPPLE_SOLID )
	{
#	ifdef USE_MOTIF
	XSetFillStyle( add->addDisplay, add->addGc, FillSolid );
#	endif
#	ifdef USE_GTK
	gdk_gc_set_fill( add->addGc, GDK_SOLID );
#	endif

	/*  4  */
	if  ( pattern[0] )
	    {
	    if  ( appMacPictSetColorX11( md, ac, &(md->mdForeColor) ) )
		{ LDEB(1); return -1;	}
	    }
	else{
	    if  ( appMacPictSetColorX11( md, ac, &(md->mdBackColor) ) )
		{ LDEB(1); return -1;	}
	    }

	md->mdStippleSet= which;
	return 0;
	}

    /*  1  */
    if  ( md->mdStippleSet == which )
	{ return 0;	}

    /*  5  */
    {
    AppBitmapImage	abi;

    const int		dstXExt= 8;
    const int		dstYExt= 8;

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

    if  ( appImgMakePixmap( add, &(md->mdTilePixmap),
					dstXExt, dstYExt, ac, &abi ) )
	{ LDEB(1); return -1;	}

    abi.abiBuffer= (unsigned char *)0;
    appCleanBitmapImage( &abi );
    }

#   ifdef USE_MOTIF
    XSetTile( add->addDisplay, add->addGc, md->mdTilePixmap );
#   endif
#   ifdef USE_GTK
    gdk_gc_set_tile( add->addGc, md->mdTilePixmap );
#   endif

#   ifdef USE_MOTIF
    XSetFillStyle( add->addDisplay, add->addGc, FillTiled );
#   endif
#   ifdef USE_GTK
    gdk_gc_set_fill( add->addGc, GDK_TILED );
#   endif

    md->mdStippleSet= which;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Use the pen/fill pattern for drawing.				*/
/*									*/
/************************************************************************/

static int appMacPictSetPenX11(		MacpictDevice *		md,
					AppColors *		ac )
    {
    if  ( md->mdPenIsSolid )
	{
	if  ( appMacPictSetStippleX11( md, ac,
				md->mdPenPattern, STIPPLE_SOLID ) )
	    { LDEB(STIPPLE_SOLID); return -1;	}
	}
    else{
	if  ( appMacPictSetStippleX11( md, ac,
				md->mdPenPattern, STIPPLE_PEN ) )
	    { LDEB(STIPPLE_PEN); return -1;	}
	}

    return 0;
    }

static int appMacPictSetFillX11(	MacpictDevice *		md,
					AppColors *		ac )
    {
    if  ( md->mdFillIsSolid )
	{
	if  ( appMacPictSetStippleX11( md, ac,
				md->mdFillPattern, STIPPLE_SOLID ) )
	    { LDEB(STIPPLE_SOLID); return -1;	}
	}
    else{
	if  ( appMacPictSetStippleX11( md, ac,
				md->mdFillPattern, STIPPLE_FILL ) )
	    { LDEB(STIPPLE_FILL); return -1;	}
	}

    return 0;
    }

static int appMacPictSetBackX11(	MacpictDevice *		md,
					AppColors *		ac )
    {
    if  ( md->mdBackIsSolid )
	{
	if  ( appMacPictSetStippleX11( md, ac,
				md->mdBackPattern, STIPPLE_SOLID ) )
	    { LDEB(STIPPLE_SOLID); return -1;	}
	}
    else{
	if  ( appMacPictSetStippleX11( md, ac,
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

static int appMacPictDrawLineX11(	MacpictDevice *		md,
					AppColors *		ac,
					int			x0,
					int			y0,
					int			x1,
					int			y1 )
    {
    AppDrawingData *	add= &(md->mdDrawingData);

    if  ( x1 == x0 && y1 == y0 )
	{ return 0;	}

    if  ( appMacPictSetPenX11( md, ac ) )
	{ LDEB(1); return -1;	}

    x0= MD_X( x0, md );
    y0= MD_Y( y0, md );
    x1= MD_X( x1, md );
    y1= MD_Y( y1, md );

    appDrawDrawLine( add, x0, y0, x1, y1 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a string.							*/
/*									*/
/************************************************************************/

static int appMacPictDrawStringX11(	MacpictDevice *		md,
					AppColors *		ac,
					int			x,
					int			y,
					int			count )
    {
    AppDrawingData *		add= &(md->mdDrawingData);

    while( count > 0 && isspace( md->mdTextString[count-1] ) )
	{ count--;	}

    if  ( count == 0 )
	{ return 0;	}

    if  ( appMacPictSetPenX11( md, ac ) )
	{ LDEB(1); return -1;	}

    x= MD_X( x, md );
    y= MD_Y( y, md );

    if  ( md->mdPrivateFont < 0 )
	{
	if  ( md->mdTextAttribute.taFontNumber < 0 )
	    { LDEB(md->mdTextAttribute.taFontNumber);	}
	else{
	    md->mdPrivateFont= appOpenScreenFont( add,
				&(md->mdFontList), &(md->mdTextAttribute) );

	    if  ( md->mdPrivateFont < 0 )
		{ LDEB(md->mdPrivateFont);	}
	    else{
		DrawScreenFont *	apf;

		apf= add->addScreenFontList.apflFonts+ md->mdPrivateFont;

		appDrawSetFont( add, apf->apfFontStruct );
		}
	    }
	}

    appDrawDrawString( add, x, y, (char *)md->mdTextString, count );

    return 0;
    }

/************************************************************************/
/*									*/
/*  FrameRect/PaintRect variants.					*/
/*									*/
/************************************************************************/

static int appMacPictFrameRectX11(	MacpictDevice *		md,
					AppColors *		ac )
    {
    AppDrawingData *	add= &(md->mdDrawingData);

    int x0= MD_X( md->mdRectX0, md );
    int y0= MD_Y( md->mdRectY0, md );
    int x1= MD_X( md->mdRectX1, md );
    int y1= MD_Y( md->mdRectY1, md );

    if  ( appMacPictSetPenX11( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawDrawRectangle( add, x0, y0, x1- x0, y1- y0 );

    return 0;
    }

static int appMacPictPaintRectX11(	MacpictDevice *		md,
					AppColors *		ac )
    {
    AppDrawingData *	add= &(md->mdDrawingData);

    int x0= MD_X( md->mdRectX0, md );
    int y0= MD_Y( md->mdRectY0, md );
    int x1= MD_X( md->mdRectX1, md );
    int y1= MD_Y( md->mdRectY1, md );

    if  ( appMacPictSetPenX11( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawFillRectangle( add, x0, y0, x1- x0, y1- y0 );

    if  ( appMacPictSetPenX11( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawDrawRectangle( add, x0, y0, x1- x0, y1- y0 );

    return 0;
    }

static int appMacPictEraseRectX11(	MacpictDevice *		md,
					AppColors *		ac )
    {
    AppDrawingData *	add= &(md->mdDrawingData);

    int x0= MD_X( md->mdRectX0, md );
    int y0= MD_Y( md->mdRectY0, md );
    int x1= MD_X( md->mdRectX1, md );
    int y1= MD_Y( md->mdRectY1, md );

    if  ( appMacPictSetBackX11( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawFillRectangle( add, x0, y0, x1- x0, y1- y0 );

    return 0;
    }

static int appMacPictFillRectX11(	MacpictDevice *		md,
					AppColors *		ac )
    {
    AppDrawingData *	add= &(md->mdDrawingData);

    int x0= MD_X( md->mdRectX0, md );
    int y0= MD_Y( md->mdRectY0, md );
    int x1= MD_X( md->mdRectX1, md );
    int y1= MD_Y( md->mdRectY1, md );

    if  ( appMacPictSetFillX11( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawFillRectangle( add, x0, y0, x1- x0, y1- y0 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  PaintRRect/FrameRRect variants.					*/
/*									*/
/************************************************************************/

static int appMacPictFrameRRectX11(	MacpictDevice *		md,
					AppColors *		ac )
    {
    AppDrawingData *	add= &(md->mdDrawingData);

    const int		fill= 0;
    const int		borders= 1;

    int x0= MD_X( md->mdRRectX0, md );
    int y0= MD_Y( md->mdRRectY0, md );
    int x1= MD_X( md->mdRRectX1, md );
    int y1= MD_Y( md->mdRRectY1, md );

    int h= MD_H( md->mdRRectOvalHigh, md );
    int w= MD_W( md->mdRRectOvalWide, md );

    if  ( appMacPictSetPenX11( md, ac ) )
	{ LDEB(1); return -1;	}

    if  ( appMetafileDrawRoundRectX11( add, x0, y0, x1, y1, w, h,
						    fill, borders,
						    &(md->mdDeviceColorSet),
						    &(md->mdDeviceColorSet) ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static int appMacPictPaintRRectX11(	MacpictDevice *		md,
					AppColors *		ac )
    {
    AppDrawingData *	add= &(md->mdDrawingData);

    const int		fill= 1;
    const int		borders= 1;

    int x0= MD_X( md->mdRRectX0, md );
    int y0= MD_Y( md->mdRRectY0, md );
    int x1= MD_X( md->mdRRectX1, md );
    int y1= MD_Y( md->mdRRectY1, md );

    int h= MD_H( md->mdRRectOvalHigh, md );
    int w= MD_W( md->mdRRectOvalWide, md );

    if  ( appMacPictSetPenX11( md, ac ) )
	{ LDEB(1); return -1;	}

    if  ( appMetafileDrawRoundRectX11( add, x0, y0, x1, y1, w, h,
						    fill, borders,
						    &(md->mdDeviceColorSet),
						    &(md->mdDeviceColorSet) ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static int appMacPictEraseRRectX11(	MacpictDevice *		md,
					AppColors *		ac )
    {
    AppDrawingData *	add= &(md->mdDrawingData);

    const int		fill= 1;
    const int		borders= 1;

    int x0= MD_X( md->mdRRectX0, md );
    int y0= MD_Y( md->mdRRectY0, md );
    int x1= MD_X( md->mdRRectX1, md );
    int y1= MD_Y( md->mdRRectY1, md );

    int h= MD_H( md->mdRRectOvalHigh, md );
    int w= MD_W( md->mdRRectOvalWide, md );

    if  ( appMacPictSetBackX11( md, ac ) )
	{ LDEB(1); return -1;	}

    if  ( appMetafileDrawRoundRectX11( add, x0, y0, x1, y1, w, h,
						    fill, borders,
						    &(md->mdDeviceColorSet),
						    &(md->mdDeviceColorSet) ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static int appMacPictFillRRectX11(	MacpictDevice *		md,
					AppColors *		ac )
    {
    AppDrawingData *	add= &(md->mdDrawingData);

    const int		fill= 1;
    const int		borders= 0;

    int x0= MD_X( md->mdRRectX0, md );
    int y0= MD_Y( md->mdRRectY0, md );
    int x1= MD_X( md->mdRRectX1, md );
    int y1= MD_Y( md->mdRRectY1, md );

    int h= MD_H( md->mdRRectOvalHigh, md );
    int w= MD_W( md->mdRRectOvalWide, md );

    if  ( appMacPictSetPenX11( md, ac ) )
	{ LDEB(1); return -1;	}

    if  ( appMetafileDrawRoundRectX11( add, x0, y0, x1, y1, w, h,
						    fill, borders,
						    &(md->mdDeviceColorSet),
						    &(md->mdDeviceColorSet) ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  PaintPoly/FramePoly variants.					*/
/*									*/
/************************************************************************/

static int appMacPictPaintPolyX11(	MacpictDevice *		md,
					AppColors *		ac )
    {
    AppDrawingData *	add= &(md->mdDrawingData);

    if  ( appMacPictSetPenX11( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawFillPolygon( add, md->mdPolyPoints, md->mdPolyPointCount );

    if  ( appMacPictSetPenX11( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawDrawLines( add, md->mdPolyPoints, md->mdPolyPointCount );

    return 0;
    }

static int appMacPictFramePolyX11(	MacpictDevice *		md,
					AppColors *		ac )
    {
    AppDrawingData *	add= &(md->mdDrawingData);

    if  ( appMacPictSetPenX11( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawDrawLines( add, md->mdPolyPoints, md->mdPolyPointCount );

    return 0;
    }

/************************************************************************/
/*									*/
/*  PaintOval/FrameOval variants.					*/
/*									*/
/************************************************************************/

static int appMacPictPaintOvalX11(	MacpictDevice *		md,
					AppColors *		ac )
    {
    AppDrawingData *	add= &(md->mdDrawingData);
    int			swap;

    int x0= MD_X( md->mdOvalX0, md );
    int y0= MD_Y( md->mdOvalY0, md );
    int x1= MD_X( md->mdOvalX1, md );
    int y1= MD_Y( md->mdOvalY1, md );

    if  ( x1 < x0 )
	{ swap= x0; x0= x1; x1= swap; }
    if  ( y1 < y0 )
	{ swap= y0; y0= y1; y1= swap; }

    if  ( appMacPictSetPenX11( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawFillArc( add, x0, y0, x1- x0, y1- y0, 64* 0, 64* 360 );

    if  ( appMacPictSetPenX11( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawDrawArc( add, x0, y0, x1- x0, y1- y0, 64* 0, 64* 360 );

    return 0;
    }

static int appMacPictFrameOvalX11(	MacpictDevice *		md,
					AppColors *		ac )
    {
    AppDrawingData *	add= &(md->mdDrawingData);
    int			swap;

    int x0= MD_X( md->mdOvalX0, md );
    int y0= MD_Y( md->mdOvalY0, md );
    int x1= MD_X( md->mdOvalX1, md );
    int y1= MD_Y( md->mdOvalY1, md );

    if  ( x1 < x0 )
	{ swap= x0; x0= x1; x1= swap; }
    if  ( y1 < y0 )
	{ swap= y0; y0= y1; y1= swap; }

    if  ( appMacPictSetPenX11( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawDrawArc( add, x0, y0, x1- x0, y1- y0, 64* 0, 64* 360 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  PaintArc/FrameArc variants.						*/
/*									*/
/************************************************************************/

static int appMacPictFrameArcX11(	MacpictDevice *		md,
					AppColors *		ac )
    {
    AppDrawingData *	add= &(md->mdDrawingData);
    int			swap;

    int			x0= MD_X( md->mdArcX0, md );
    int			y0= MD_Y( md->mdArcY0, md );
    int			x1= MD_X( md->mdArcX1, md );
    int			y1= MD_Y( md->mdArcY1, md );

    if  ( x1 < x0 )
	{ swap= x0; x0= x1; x1= swap; }
    if  ( y1 < y0 )
	{ swap= y0; y0= y1; y1= swap; }

    if  ( appMacPictSetPenX11( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawDrawArc( add, x0, y0, x1- x0, y1- y0,
			    64* ( 90- md->mdArcA0 ), 64* ( -md->mdArcA1 ) );

    return 0;
    }

static int appMacPictPaintArcX11(	MacpictDevice *		md,
					AppColors *		ac )
    {
    AppDrawingData *	add= &(md->mdDrawingData);
    int			swap;

    int			x0= MD_X( md->mdArcX0, md );
    int			y0= MD_Y( md->mdArcY0, md );
    int			x1= MD_X( md->mdArcX1, md );
    int			y1= MD_Y( md->mdArcY1, md );

    if  ( x1 < x0 )
	{ swap= x0; x0= x1; x1= swap; }
    if  ( y1 < y0 )
	{ swap= y0; y0= y1; y1= swap; }

    if  ( appMacPictSetPenX11( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawFillArc( add, x0, y0, x1- x0, y1- y0,
			    64* ( 90- md->mdArcA0 ), 64* ( -md->mdArcA1 ) );

    if  ( appMacPictSetPenX11( md, ac ) )
	{ LDEB(1); return -1;	}

    appDrawDrawArc( add, x0, y0, x1- x0, y1- y0,
			    64* ( 90- md->mdArcA0 ), 64* ( -md->mdArcA1 ) );

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

static int appMacPictDrawImageX11(	MacpictDevice *		md,
					AppColors *		ac,
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

    /*  12  */
    {
    int			pixelsWide= x1Dest- x0Dest+ 1;
    int			pixelsHigh= y1Dest- y0Dest+ 1;

    int			srcX= x0Source;
    int			srcY= y0Source;
    int			dstX= MD_X( x0Dest, md );
    int			dstY= MD_Y( y0Dest, md );

    AppDrawingData *	add= &(md->mdDrawingData);
    APP_IMAGE *		xim;

    pixelsWide= MD_W( pixelsWide, md );
    pixelsHigh= MD_H( pixelsHigh, md );

    if  ( appImgMakeImage( add, &xim, pixelsWide, pixelsHigh, ac, &abi ) )
	{ LDEB(1); rval= -1; goto ready; }

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
    }

  ready:

    /*  13  */
    appCleanBitmapImage( &abi );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Play a macpict metafile.						*/
/*									*/
/************************************************************************/

int appMacPictPlayFileX11( SimpleInputStream *	sis,
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
    int			c;
    int			i;

    int			rval= 0;

    double		magnification;
    double		pixelsPerTwip;

    MacpictDevice	md;


    appMacPictInitDeviceHeader( &md, pixelsWide, pixelsHigh,
						    twipsWide, twipsHigh );

    if  ( appMacPictGetDeviceHeader( &md, sis ) )
	{ LDEB(1); return -1;	}

    md.mdPostScriptFontList= parent_add->addPostScriptFontList;

    PICTDEB(appDebug( "\n" ));
    PICTDEB(appDebug( "MACPICT X11( %d .. %d x %d .. %d -> %d x %d ) VERSION %d\n",
		    md.mdInX0, md.mdInX1, md.mdInY0, md.mdInY1,
		    md.mdOutWide, md.mdOutHigh, md.mdVersion ));

    pixelsPerTwip= sqrt( (double)( pixelsWide* pixelsHigh )/
						( twipsWide* twipsHigh ) );
    magnification= pixelsPerTwip/ parent_add->addMagnifiedPixelsPerTwip;

    appCloneDrawingEnvironment( &(md.mdDrawingData),
			parent_add, magnification, pixelsPerTwip, pixmap );

    appMacPictFillBackgroundX11( &md );

    appColorRgb( &(md.mdDeviceColorSet), ac, 0, 0, 0 );
    appDrawSetForegroundBlack( &(md.mdDrawingData) );

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

		if  ( appMacPictFrameOvalX11( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FrameSameOval:
		{
		PICTDEB(appDebug( "FrameSameOval()\n" ));

		if  ( appMacPictFrameOvalX11( &md, ac ) )
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

		if  ( appMacPictPaintOvalX11( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PaintSameOval:
		{
		PICTDEB(appDebug( "PaintSameOval()\n" ));

		if  ( appMacPictPaintOvalX11( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PaintPoly:
		{
		if  ( appMacPictGetPoly( &md, sis ) )
		    { rval= -1; goto ready;	}

		PICTDEB(appDebug( "PaintPoly(,%d)\n", md.mdPolyPointCount ));

		if  ( appMacPictPaintPolyX11( &md, ac ) )
		    { LDEB(1); return -1;	}
		}
		continue;

	    case MACPICT_FramePoly:
		{
		if  ( appMacPictGetPoly( &md, sis ) )
		    { rval= -1; goto ready;	}

		PICTDEB(appDebug( "FramePoly(,%d)\n", md.mdPolyPointCount ));

		if  ( appMacPictFramePolyX11( &md, ac ) )
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

		if  ( appMacPictDrawLineX11( &md, ac, x0, y0, x1, y1 ) )
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

		if  ( appMacPictDrawLineX11( &md, ac, x0, y0, x1, y1 ) )
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

		if  ( appMacPictDrawLineX11( &md, ac, x0, y0, x1, y1 ) )
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

		if  ( appMacPictDrawLineX11( &md, ac, x0, y0, x1, y1 ) )
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

		if  ( appMacPictFrameRRectX11( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FrameSameRRect:
		{
		PICTDEB(appDebug( "FrameSameRRect()\n" ));

		if  ( appMacPictFrameRRectX11( &md, ac ) )
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

		if  ( appMacPictPaintRRectX11( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PaintSameRRect:
		{
		PICTDEB(appDebug( "PaintSameRRect()\n" ));

		if  ( appMacPictPaintRRectX11( &md, ac ) )
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

		if  ( appMacPictFillRRectX11( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FillSameRRect:
		{
		PICTDEB(appDebug( "FillSameRRect()\n" ));

		if  ( appMacPictFillRRectX11( &md, ac ) )
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

		if  ( appMacPictEraseRRectX11( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_EraseSameRRect:
		{
		PICTDEB(appDebug( "EraseSameRRect()\n" ));

		if  ( appMacPictEraseRRectX11( &md, ac ) )
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

		if  ( appMacPictFrameRectX11( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FrameSameRect:
		{
		PICTDEB(appDebug( "FrameSameRect()\n" ));

		if  ( appMacPictFrameRectX11( &md, ac ) )
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

		if  ( appMacPictPaintRectX11( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_PaintSameRect:
		{
		PICTDEB(appDebug( "PaintSameRect()\n" ));

		if  ( appMacPictPaintRectX11( &md, ac ) )
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

		if  ( appMacPictFillRectX11( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_FillSameRect:
		{
		PICTDEB(appDebug( "FillSameRect()\n" ));

		if  ( appMacPictFillRectX11( &md, ac ) )
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

		if  ( appMacPictEraseRectX11( &md, ac ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_EraseSameRect:
		{
		PICTDEB(appDebug( "EraseSameRect()\n" ));

		if  ( appMacPictEraseRectX11( &md, ac ) )
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

		if  ( appMacPictFrameArcX11( &md, ac ) )
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

		if  ( appMacPictPaintArcX11( &md, ac ) )
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

		if  ( appMacPictDrawStringX11( &md, ac,
					    md.mdPenX, md.mdPenY, count ) )
		    { LDEB(count); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_DVText:
		{
		int	count;

		if  ( appMacPictGetDVText( &count, &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( appMacPictDrawStringX11( &md, ac,
					    md.mdPenX, md.mdPenY, count ) )
		    { LDEB(count); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_DHDVText:
		{
		int	count;

		if  ( appMacPictGetDHDVText( &count, &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( appMacPictDrawStringX11( &md, ac,
					    md.mdPenX, md.mdPenY, count ) )
		    { LDEB(count); rval= -1; goto ready;	}
		}
		continue;

	    case MACPICT_LongText:
		{
		int	count;

		if  ( appMacPictGetLongText( &count, &md, sis ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( appMacPictDrawStringX11( &md, ac,
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

		if  ( appMacPictDrawImageX11( &md, ac, sis,
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

		if  ( appMacPictDrawImageX11( &md, ac, sis,
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

		if  ( appMacPictDrawImageX11( &md, ac, sis,
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

		if  ( appMacPictDrawImageX11( &md, ac, sis,
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

		if  ( appMacPictDrawImageX11( &md, ac, sis,
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
