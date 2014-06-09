/************************************************************************/
/*									*/
/*  Drawing primitives for X11/Motif.					*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<geo2DInteger.h>
#   include	"appDraw.h"

#   include	<appDebugon.h>

#   ifdef USE_MOTIF

/************************************************************************/
/*									*/
/*  Collect exposures for optimised drawing.				*/
/*									*/
/************************************************************************/

#   define	LOG_EXPOSES	0

void appCollectExposures(	DocumentRectangle *	drClip,
				AppDrawingData *	add,
				int			ox,
				int			oy,
				XEvent *		event )
    {
    Region		region;

    region= XCreateRegion();
    XtAddExposureToRegion( event, region );

    drClip->drX0= ox+ event->xexpose.x;
    drClip->drY0= oy+ event->xexpose.y;
    drClip->drX1= ox+ event->xexpose.x+ event->xexpose.width;
    drClip->drY1= oy+ event->xexpose.y+ event->xexpose.height;

#   if LOG_EXPOSES
    appDebug( "E=====: [%4d+%4d]x[%4d+%4d]\n",
		    drClip->drX0,
		    drClip->drX1- drClip->drX0+ 1,
		    drClip->drY0,
		    drClip->drY1- drClip->drY0+ 1 );
#   endif

    while( QLength( add->addDisplay ) > 0 )
	{
	XEvent			nextEvent;
	DocumentRectangle	drMore;

	XPeekEvent( add->addDisplay, &nextEvent );

	if  ( nextEvent.type != Expose && nextEvent.type != GraphicsExpose )
	    { break;	}
	if  ( nextEvent.xexpose.window != add->addDrawable )
	    { break;	}

	XNextEvent( add->addDisplay, &nextEvent );
	XtAddExposureToRegion( &nextEvent, region );
	drMore.drX0= ox+ nextEvent.xexpose.x;
	drMore.drY0= oy+ nextEvent.xexpose.y;
	drMore.drX1= ox+ nextEvent.xexpose.x+ nextEvent.xexpose.width;
	drMore.drY1= oy+ nextEvent.xexpose.y+ nextEvent.xexpose.height;

#	if LOG_EXPOSES
	appDebug( "E+++++: [%4d+%4d]x[%4d+%4d]\n",
			drMore.drX0,
			drMore.drX1- drMore.drX0+ 1,
			drMore.drY0,
			drMore.drY1- drMore.drY0+ 1 );
#	endif

	geoUnionRectangle( drClip, drClip, &drMore );

#	if LOG_EXPOSES
	appDebug( "E.....: [%4d+%4d]x[%4d+%4d]\n",
			drClip->drX0,
			drClip->drX1- drClip->drX0+ 1,
			drClip->drY0,
			drClip->drY1- drClip->drY0+ 1 );
#	endif
	}

    XSetRegion( add->addDisplay, add->addGc, region );

    XDestroyRegion( region );

    return;
    }

void appExposeRectangle(	const AppDrawingData *	add,
				int			x,
				int			y,
				int			wide,
				int			high )
    {
    XEvent	xev;

    if  ( ! add->addDrawable )
	{ XDEB(add->addDrawable); return; }

    if  ( wide == 0 || high == 0 )
	{
	XClearArea( add->addDisplay, add->addDrawable,
						x, y, wide, high, True );
	return;
	}

    xev.type= Expose;
    xev.xexpose.serial= 0;
    xev.xexpose.send_event= True;
    xev.xexpose.display= add->addDisplay;
    xev.xexpose.window= add->addDrawable;
    xev.xexpose.x= x;
    xev.xexpose.y= y;
    xev.xexpose.width= wide;
    xev.xexpose.height= high;
    xev.xexpose.count= 0;

    if  ( XSendEvent( add->addDisplay, add->addDrawable,
					0, ExposureMask, &xev ) != Success )
	{ /*LDEB(1);*/	}

    return;
    }

/************************************************************************/
/*									*/
/*  Initialise drawing data.						*/
/*									*/
/************************************************************************/

void appInitDrawingData(	AppDrawingData *	add )
    {
    add->addForScreenDrawing= 0;

    docInitRectangle( &(add->addBackRect) );
    docInitRectangle( &(add->addPaperRect) );
    docInitRectangle( &(add->addDocRect) );

    add->addBottomMarginPixels= 0;
    add->addPageGapPixels= 0;
    add->addPageStepPixels= 0;

    add->addMagnification= 1.0;
    add->addMagnifiedPixelsPerTwip= 0.0;
    add->addScreenPixelsPerMM= 0.0;
    add->addDisplay= (Display *)0;
    add->addDrawable= None;
    add->addGc= None;
    add->addScreen= -1;

    add->addPostScriptFontList= (const PostScriptFontList *)0;
    appInitFontList( &(add->addScreenFontList) );
    }

void appCleanDrawingData(	AppDrawingData *	add )
    {
    if  ( add->addGc )
	{ XFreeGC( add->addDisplay, add->addGc );	}

    if  ( add->addDisplay )
	{ appCleanFontList( add, &(add->addScreenFontList) ); }

    return ;
    }

void appSetDrawingEnvironment(
			AppDrawingData *		add,
			double				magnification,
			double				xfac,
			double				screenPixelsPerMM,
			const PostScriptFontList *	psfl,
			APP_WIDGET			w )
    {
    Display *	display= XtDisplay( w );

    if  ( w )
	{ add->addForScreenDrawing= 1; }

    add->addMagnification= magnification;
    add->addMagnifiedPixelsPerTwip= xfac;
    add->addScreenPixelsPerMM= screenPixelsPerMM;

    add->addPostScriptFontList= psfl;

    add->addDisplay= display;
    add->addScreen= DefaultScreen( display );

    return;
    }

void appCloneDrawingEnvironment(	AppDrawingData *	add,
					const AppDrawingData *	parent_add,
					double			magnification,
					double			xfac,
					APP_WINDOW		drawable )
    {
    add->addMagnification= magnification;
    add->addMagnifiedPixelsPerTwip= xfac;
    add->addScreenPixelsPerMM= parent_add->addScreenPixelsPerMM;

    add->addPostScriptFontList= parent_add->addPostScriptFontList;

    add->addDisplay= parent_add->addDisplay;
    add->addScreen= parent_add->addScreen;
    add->addDrawable= drawable;

    if  ( add->addDisplay )
	{
	add->addGc= XCreateGC( add->addDisplay, add->addDrawable,
							0L, (XGCValues *)0 );
	}

    add->addForeColor= parent_add->addForeColor;
    add->addBackColor= parent_add->addBackColor;
    add->addTopColor= parent_add->addTopColor;
    add->addBottomColor= parent_add->addBottomColor;

    return;
    }


int appSetDrawingDataForWidget(	APP_WIDGET		w,
				double			magnification,
				AppDrawingData *	add )
    {
    double		horPixPerMM;
    double		verPixPerMM;
    double		xfac;
    double		yfac;

    Colormap		cmap;

    add->addDisplay= XtDisplay( w );
    add->addScreen= DefaultScreen( add->addDisplay );
    add->addDrawable= XtWindow( w );
    add->addGc= XCreateGC( add->addDisplay, add->addDrawable, 0, NULL );

    horPixPerMM= ( (double)DisplayWidth( add->addDisplay, add->addScreen ) )/
			    DisplayWidthMM( add->addDisplay, add->addScreen );
    verPixPerMM= ( (double)DisplayHeight( add->addDisplay, add->addScreen ) )/
			    DisplayHeightMM( add->addDisplay, add->addScreen );

    xfac=  ( 25.4/ ( 20.0* 72.0 ) )* horPixPerMM;
    yfac=  ( 25.4/ ( 20.0* 72.0 ) )* verPixPerMM;

    xfac *= magnification;
    yfac *= magnification;

    add->addMagnification= magnification;
    add->addMagnifiedPixelsPerTwip= sqrt( xfac* yfac );
    add->addScreenPixelsPerMM= sqrt( horPixPerMM* verPixPerMM );

    XtVaGetValues( w,
			XmNforeground,		&(add->addForeColor.pixel),
			XmNbackground,		&(add->addBackColor.pixel),
			XmNtopShadowColor,	&(add->addTopColor.pixel),
			XmNbottomShadowColor,	&(add->addBottomColor.pixel),
			NULL );

    cmap= DefaultColormap( add->addDisplay, add->addScreen );

    XQueryColor( add->addDisplay, cmap, &(add->addForeColor) );
    XQueryColor( add->addDisplay, cmap, &(add->addBackColor) );
    XQueryColor( add->addDisplay, cmap, &(add->addTopColor) );
    XQueryColor( add->addDisplay, cmap, &(add->addBottomColor) );

    return 0;
    }

int appDrawWhiteColor(		AppDrawingData *		add,
				XColor *			xc )
    {
    xc->red= xc->green= xc->blue= 0xffff;
    xc->pixel= WhitePixel( add->addDisplay, add->addScreen );
    return 0;
    }

int appDrawBlackColor(		AppDrawingData *		add,
				XColor *			xc )
    {
    xc->red= xc->green= xc->blue= 0x0000;
    xc->pixel= BlackPixel( add->addDisplay, add->addScreen );
    return 0;
    }

void appDrawSetForegroundBlack(	AppDrawingData *	add )
    {
    XSetForeground( add->addDisplay, add->addGc,
			    BlackPixel( add->addDisplay, add->addScreen ) );
    }

void appDrawSetForegroundWhite(	AppDrawingData *	add )
    {
    XSetForeground( add->addDisplay, add->addGc,
			    WhitePixel( add->addDisplay, add->addScreen ) );
    }

void appDrawSetBackgroundWhite(	AppDrawingData *	add )
    {
    XSetBackground( add->addDisplay, add->addGc,
			    WhitePixel( add->addDisplay, add->addScreen ) );
    }

void appDrawSetForegroundColor(	AppDrawingData *	add,
				XColor *		xc )
    { XSetForeground( add->addDisplay, add->addGc, xc->pixel ); }

void appDrawSetBackgroundColor(	AppDrawingData *	add,
				XColor *		xc )
    { XSetBackground( add->addDisplay, add->addGc, xc->pixel ); }

void appDrawFillRectangle(	AppDrawingData *	add,
				int			x,
				int			y,
				int			wide,
				int			high )
    {
    XFillRectangle( add->addDisplay, add->addDrawable, add->addGc,
							    x, y, wide, high );
    }

void appDrawDrawRectangle(	AppDrawingData *	add,
				int			x,
				int			y,
				int			wide,
				int			high )
    {
    XDrawRectangle( add->addDisplay, add->addDrawable, add->addGc,
							    x, y, wide, high );
    }

APP_FONT * appDrawOpenFont(	AppDrawingData *	add,
				const char *		name )
    {
    return XLoadQueryFont( add->addDisplay, name );
    }

void appDrawSetFont(		AppDrawingData *	add,
				APP_FONT *		xfs )
    {
    XSetFont( add->addDisplay, add->addGc, xfs->fid );
    }

void appDrawDrawString(		AppDrawingData *	add,
				int			x,
				int			y,
				const char *		s,
				int			len )
    {
    XDrawString( add->addDisplay, add->addDrawable, add->addGc,
							    x, y, s, len );
    }

void appDrawDrawLine(		AppDrawingData *	add,
				int			x0,
				int			y0,
				int			x1,
				int			y1 )
    {
    XDrawLine( add->addDisplay, add->addDrawable, add->addGc, x0, y0, x1, y1 );
    }

void appDrawMoveArea(		AppDrawingData *	add,
				int			src_x,
				int			src_y,
				int			wide,
				int			high,
				int			dest_x,
				int			dest_y )
    {
    XCopyArea( add->addDisplay, add->addDrawable, add->addDrawable, add->addGc,
				    src_x, src_y, wide, high, dest_x, dest_y );
    }

void appDrawFreeFont(		AppDrawingData *	add,
				APP_FONT *		font )
    {
    XFreeFont( add->addDisplay, font );

    return;
    }

void appDrawTextExtents(	int *			pWidth,
				int *			pFontAscent,
				int *			pFontDescent,
				const AppDrawingData *	add,
				APP_FONT *		xfs,
				const char *		s,
				int			l )
    {
    *pWidth= XTextWidth( xfs, s, l );
    *pFontAscent= xfs->ascent;
    *pFontDescent= xfs->descent;

    return;
    }

void appDrawNoClipping(		AppDrawingData *	add )
    {
#   if LOG_EXPOSES
    appDebug( "C=====: OFF\n" );
#   endif

    XSetClipMask( add->addDisplay, add->addGc, None );

    return;
    }

extern void appDrawSetClipRect(	AppDrawingData *		add,
				const DocumentRectangle *	drClip )
    {
    XRectangle		xRect;

    xRect.x= drClip->drX0;
    xRect.y= drClip->drY0;
    xRect.width= drClip->drX1- drClip->drX0+ 1;
    xRect.height= drClip->drY1- drClip->drY0+ 1;

#   if LOG_EXPOSES
    appDebug( "C=====: [%4d+%4d]x[%4d+%4d]\n",
		    drClip->drX0,
		    drClip->drX1- drClip->drX0+ 1,
		    drClip->drY0,
		    drClip->drY1- drClip->drY0+ 1 );
#   endif

    XSetClipRectangles( add->addDisplay, add->addGc,
						0, 0, &xRect, 1, Unsorted );

    return;
    }

void appDrawFillPolygon(		AppDrawingData *	add,
					const APP_POINT *	points,
					int			count )
    {
    XFillPolygon( add->addDisplay, add->addDrawable, add->addGc,
			(APP_POINT *)points, count, Complex, CoordModeOrigin );
    }

void appDrawDrawLines(			AppDrawingData *	add,
					const APP_POINT *	points,
					int			count )
    {
    int		i;

    /*
    XDrawLines( add->addDisplay, add->addDrawable, add->addGc,
				(APP_POINT *)points, count, CoordModeOrigin );
    */

    count--;

    for ( i= 0; i < count; points++, i++ )
	{
	XDrawLine( add->addDisplay, add->addDrawable, add->addGc,
						    points[0].x, points[0].y,
						    points[1].x, points[1].y );
	}

    return;
    }

void appDrawDrawPixmap(			AppDrawingData *	add,
					APP_BITMAP_IMAGE	pixmap,
					int			xSrc,
					int			ySrc,
					int			wide,
					int			high,
					int			xDest,
					int			yDest )
    {
    XCopyArea( add->addDisplay, pixmap, add->addDrawable, add->addGc,
				    xSrc, ySrc, wide, high, xDest, yDest );

    return;
    }

int appDrawGetSizeOfWidget(		int *		pWide,
					int *		pHigh,
					APP_WIDGET	w )
    {
    Dimension		wide;
    Dimension		high;

    XtVaGetValues( w,	XmNwidth,	&wide,
			XmNheight,	&high,
			NULL );

    *pWide= wide; *pHigh= high; return 0;
    }

void appDrawFreePixmap(	AppDrawingData *	add,
			APP_BITMAP_IMAGE	pixmap )
    {
    XFreePixmap( add->addDisplay, pixmap );
    }

APP_BITMAP_MASK appMakeBitmap(	AppDrawingData *	add,
				int			wide,
				int			high )
    {
    return XCreatePixmap( add->addDisplay, add->addDrawable,
						    wide, high, 1 );
    }

APP_BITMAP_IMAGE appMakePixmap(	AppDrawingData *	add,
				int			wide,
				int			high )
    {
    int			depth= DefaultDepth( add->addDisplay, add->addScreen );
    APP_BITMAP_IMAGE	rval;

    rval= XCreatePixmap( add->addDisplay, add->addDrawable,
						    wide, high, depth );
    if  ( ! rval )
	{ XDEB(rval);	}

    XSetForeground( add->addDisplay, add->addGc,
			    WhitePixel( add->addDisplay, add->addScreen ) );

    XFillRectangle( add->addDisplay, rval, add->addGc, 0, 0, wide, high );

    return rval;
    }

void appDrawDrawSegments(	AppDrawingData *	add,
				APP_SEGMENT *		segments,
				int			count )
    {
    XDrawSegments( add->addDisplay, add->addDrawable, add->addGc,
							    segments, count );
    }

/************************************************************************/
/*									*/
/*  Draw an arc. Angles are in X11 style:				*/
/*									*/
/*  Unit:		1/64 degree.					*/
/*  Origin:		At the right of the circle.			*/
/*  Orientation:	Counterclockwise.				*/
/*									*/
/************************************************************************/

void appDrawDrawArc(		AppDrawingData *	add,
				int			x,
				int			y,
				int			wide,
				int			high,
				int			alpha0,
				int			alpha_step )
    {
    XDrawArc( add->addDisplay, add->addDrawable, add->addGc,
					x, y, wide, high, alpha0, alpha_step );
    }

void appDrawFillArc(		AppDrawingData *	add,
				int			x,
				int			y,
				int			wide,
				int			high,
				int			alpha0,
				int			alpha_step )
    {
    XFillArc( add->addDisplay, add->addDrawable, add->addGc,
					x, y, wide, high, alpha0, alpha_step );
    }

void appDrawDrawArcs(		AppDrawingData *	add,
				const XArc *		arcs,
				int			count )
    {
    XDrawArcs( add->addDisplay, add->addDrawable, add->addGc, (XArc *)arcs, count );
    }

void appDrawFillArcs(		AppDrawingData *	add,
				const XArc *		arcs,
				int			count )
    {
    XFillArcs( add->addDisplay, add->addDrawable, add->addGc, (XArc *)arcs, count );
    }

int appDrawSetLineAttributes(	AppDrawingData *	add,
				int			lineWidth,
				int			lineStyle,
				int			capStyle,
				int			joinStyle,
				const unsigned char *	dashList,
				int			dashCount )
    {
    XSetLineAttributes( add->addDisplay, add->addGc,
				lineWidth, lineStyle, capStyle, joinStyle );

    if  ( lineStyle != LINEstyleSOLID )
	{
	if  ( ! dashList || dashCount == 0 )
	    { XLDEB(dashList,dashCount);	}
	else{
	    XSetDashes( add->addDisplay, add->addGc, 0,
						(char *)dashList, dashCount );
	    }
	}

    return 0;
    }

#   endif
