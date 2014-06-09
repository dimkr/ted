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

#   ifdef USE_GTK

/************************************************************************/
/*									*/
/*  Collect exposures for optimised drawing.				*/
/*									*/
/*  Thanks to Andrea Frome who is the original author of this code.	*/
/*									*/
/************************************************************************/

#   define	LOG_EXPOSES	0

void appCollectExposures(	DocumentRectangle *	drClip,
				AppDrawingData *	add,
				int			ox,
				int			oy,
				APP_EVENT *		event )
    {
    GdkRectangle		gRect;

    drClip->drX0= ox+ event->expose.area.x;
    drClip->drY0= oy+ event->expose.area.y;
    drClip->drX1= ox+ event->expose.area.x+ event->expose.area.width;
    drClip->drY1= oy+ event->expose.area.y+ event->expose.area.height;

#   if LOG_EXPOSES
    appDebug( "======: [%4d+%4d]x[%4d+%4d]\n",
		    drClip->drX0,
		    drClip->drX1- drClip->drX0+ 1,
		    drClip->drY0,
		    drClip->drY1- drClip->drY0+ 1 );
#   endif

    while( TRUE )
	{
	GdkEvent *		nxEvent;
	DocumentRectangle	drMore;

	nxEvent= gdk_event_peek();
	if  ( ! nxEvent )
	    { break;	}

	if  ( nxEvent->type != GDK_EXPOSE		||
	      nxEvent->any.window != add->addDrawable	)
	    { gdk_event_free( nxEvent ); break;	}

	gdk_event_free( nxEvent );

	nxEvent= gdk_event_get();
	if  ( ! nxEvent )
	    { XDEB(nxEvent); break;	}

	drMore.drX0= ox+ nxEvent->expose.area.x;
	drMore.drY0= oy+ nxEvent->expose.area.y;
	drMore.drX1= ox+ nxEvent->expose.area.x+ nxEvent->expose.area.width;
	drMore.drY1= oy+ nxEvent->expose.area.y+ nxEvent->expose.area.height;

	gdk_event_free( nxEvent );

#	if LOG_EXPOSES
	appDebug( "++++++: [%4d+%4d]x[%4d+%4d]\n",
			drMore.drX0,
			drMore.drX1- drMore.drX0+ 1,
			drMore.drY0,
			drMore.drY1- drMore.drY0+ 1 );
#	endif

	geoUnionRectangle( drClip, drClip, &drMore );

#	if LOG_EXPOSES
	appDebug( "......: [%4d+%4d]x[%4d+%4d]\n",
			drClip->drX0,
			drClip->drX1- drClip->drX0+ 1,
			drClip->drY0,
			drClip->drY1- drClip->drY0+ 1 );
#	endif
	}

    gRect.x= drClip->drX0- ox;
    gRect.y= drClip->drY0- oy;
    gRect.width= drClip->drX1- drClip->drX0+ 1;
    gRect.height= drClip->drY1- drClip->drY0+ 1;

    gdk_gc_set_clip_rectangle( add->addGc, &gRect );

    return;
    }

void appExposeRectangle(	const AppDrawingData *	add,
				int			x,
				int			y,
				int			wide,
				int			high )
    {
    if  ( wide == 0 || high == 0 )
	{
	gdk_window_clear_area_e( add->addDrawable, x, y, wide, high );
	}
    else{
#	if GTK_MAJOR_VERSION < 2
	gdk_window_clear_area_e( add->addDrawable, x, y, wide, high );
#	else

	GdkRectangle	rect;

	rect.x= x;
	rect.y= y;
	rect.width= wide;
	rect.height= high;

	gdk_window_invalidate_rect( add->addDrawable, &rect, TRUE );

	/*
	gtk_widget_queue_draw_area ( add->addDrawable, x, y, wide, high ) ?
	*/

#	endif
	}

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
    add->addDrawable= (GdkWindow *)0;
    add->addGc= (GdkGC *)0;
    add->addCurrentFont= (GdkFont *)0;

    add->addPostScriptFontList= (const PostScriptFontList *)0;
    appInitFontList( &(add->addScreenFontList) );
    }

void appCleanDrawingData(	AppDrawingData *	add )
    {
    if  ( add->addGc )
	{ gdk_gc_unref( add->addGc );	}

    appCleanFontList( add, &(add->addScreenFontList) );

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
    if  ( w )
	{ add->addForScreenDrawing= 1; }

    add->addMagnification= magnification;
    add->addMagnifiedPixelsPerTwip= xfac;
    add->addScreenPixelsPerMM= screenPixelsPerMM;

    add->addPostScriptFontList= psfl;

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

    add->addDrawable= drawable;

    add->addColorMap= parent_add->addColorMap;
    add->addGc= gdk_gc_new( add->addDrawable );

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
    
    double		f1;
    double		f2;

    GtkStyle *		gs;

    if  ( ! GTK_WIDGET_REALIZED( w ) )
	{ gtk_widget_realize( w );	}

    add->addDrawable= w->window;
    add->addColorMap= gtk_widget_get_colormap( w );
    add->addGc= gdk_gc_new( add->addDrawable );

    horPixPerMM= (double)gdk_screen_width()/ (double)gdk_screen_width_mm();
    verPixPerMM= (double)gdk_screen_height()/ (double)gdk_screen_height_mm();

    xfac=  ( 25.4/ ( 20.0* 72.0 ) )* horPixPerMM;
    yfac=  ( 25.4/ ( 20.0* 72.0 ) )* verPixPerMM;

    xfac *= magnification;
    yfac *= magnification;

    add->addMagnification= magnification;
    add->addMagnifiedPixelsPerTwip= sqrt( xfac* yfac );
    add->addScreenPixelsPerMM= sqrt( horPixPerMM* verPixPerMM );

    gs= gtk_widget_get_style( w );

    add->addForeColor= gs->fg[GTK_STATE_NORMAL];
    add->addBackColor= gs->bg[GTK_STATE_NORMAL];

    add->addTopColor= gs->bg[GTK_STATE_NORMAL];
    add->addBottomColor= gs->bg[GTK_STATE_NORMAL];

# define sq(x) ((1.0*x)*(1.0*x))

    f1= 0;
    f1 += sq( add->addTopColor.red );
    f1 += sq( add->addTopColor.green );
    f1 += sq( add->addTopColor.blue );
    f1= sqrt( f1 )/ sqrt( 3* sq( 256* 256 ) );
    if  ( f1 < 0.001 )
	{ f2= 1.0;		}
    else{ f2= sqrt( f1 )/ f1;	}

    add->addTopColor.red *= f2;
    add->addTopColor.green *= f2;
    add->addTopColor.blue *= f2;
    if  ( ! gdk_color_alloc( add->addColorMap, &(add->addTopColor) ) )
	{ LDEB(1);	}

    f1= 0;
    f1 += sq( add->addBottomColor.red );
    f1 += sq( add->addBottomColor.green );
    f1 += sq( add->addBottomColor.blue );
    f1= sqrt( f1 )/ sqrt( 3* sq( 256* 256 ) );
    if  ( f1 < 0.001 )
	{ f2= 1.0;				}
    else{ f2= ( 1.0- sqrt( 1.0- f1 ) )/ f1;	}

    add->addBottomColor.red *= f2;
    add->addBottomColor.green *= f2;
    add->addBottomColor.blue *= f2;
    if  ( ! gdk_color_alloc( add->addColorMap, &(add->addBottomColor) ) )
	{ LDEB(1);	}

    return 0;
    }

int appDrawBlackColor(	AppDrawingData *	add,
			APP_COLOR_RGB *		xc )
    {
    if  ( ! gdk_color_black( add->addColorMap, xc ) )
	{ return -1;	}

    return 0;
    }

int appDrawWhiteColor(	AppDrawingData *	add,
			APP_COLOR_RGB *		xc )
    {
    if  ( ! gdk_color_white( add->addColorMap, xc ) )
	{ return -1;	}

    return 0;
    }

void appDrawSetForegroundBlack(	AppDrawingData *	add )
    {
    APP_COLOR_RGB	xc;

    appDrawBlackColor( add, &xc );
    appDrawSetForegroundColor( add, &xc );
    }

void appDrawSetForegroundWhite(	AppDrawingData *	add )
    {
    APP_COLOR_RGB	xc;

    appDrawWhiteColor( add, &xc );
    appDrawSetForegroundColor( add, &xc );
    }

void appDrawSetBackgroundWhite(	AppDrawingData *	add )
    {
    APP_COLOR_RGB	xc;

    appDrawWhiteColor( add, &xc );
    appDrawSetBackgroundColor( add, &xc );
    }

void appDrawSetForegroundColor(	AppDrawingData *	add,
				APP_COLOR_RGB *		xc )
    { gdk_gc_set_foreground( add->addGc, xc ); }

void appDrawSetBackgroundColor(	AppDrawingData *	add,
				APP_COLOR_RGB *		xc )
    { gdk_gc_set_background( add->addGc, xc ); }

void appDrawFillRectangle(	AppDrawingData *	add,
				int			x,
				int			y,
				int			wide,
				int			high )
    {
    gdk_draw_rectangle( add->addDrawable, add->addGc, TRUE, x, y, wide, high );
    }

void appDrawDrawRectangle(	AppDrawingData *	add,
				int			x,
				int			y,
				int			wide,
				int			high )
    {
    gdk_draw_rectangle( add->addDrawable, add->addGc, FALSE,
							    x, y, wide, high );
    }

APP_FONT * appDrawOpenFont(	AppDrawingData *	add,
				const char *		name )
    {
    return gdk_font_load( name );
    }

void appDrawSetFont(		AppDrawingData *	add,
				APP_FONT *		xfs )
    {
    add->addCurrentFont= xfs;

    gdk_gc_set_font( add->addGc, xfs );
    }

void appDrawDrawString(		AppDrawingData *	add,
				int			x,
				int			y,
				const char *		s,
				int			len )
    {
    if  ( ! add->addCurrentFont )
	{ XDEB(add->addCurrentFont); return;	}

    gdk_draw_text( add->addDrawable, add->addCurrentFont, add->addGc,
							    x, y, s, len );
    }

void appDrawDrawLine(		AppDrawingData *	add,
				int			x0,
				int			y0,
				int			x1,
				int			y1 )
    {
    gdk_draw_line( add->addDrawable, add->addGc, x0, y0, x1, y1 );
    }

void appDrawMoveArea(		AppDrawingData *	add,
				int			src_x,
				int			src_y,
				int			wide,
				int			high,
				int			dest_x,
				int			dest_y )
    {
    gdk_window_copy_area( add->addDrawable, add->addGc, dest_x, dest_y,
					    add->addDrawable, src_x, src_y,
					    wide, high );
    }

void appDrawFreeFont(		AppDrawingData *	add,
				APP_FONT *		font )
    {
    if  ( add->addCurrentFont == font )
	{ add->addCurrentFont= (APP_FONT *)0;	}

    gdk_font_unref( font );

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
    if  ( l > 0 )
	{ *pWidth= gdk_text_width( xfs, s, l );	}
    else{ *pWidth= 0;				}

    *pFontAscent= xfs->ascent;
    *pFontDescent= xfs->descent;

    return;
    }

void appDrawNoClipping(		AppDrawingData *	add )
    {
    gdk_gc_set_clip_rectangle( add->addGc, (GdkRectangle *)0 );

    return;
    }

extern void appDrawSetClipRect(	AppDrawingData *		add,
				const DocumentRectangle *	drClip )
    {
    GdkRectangle		gRect;

    gRect.x= drClip->drX0;
    gRect.y= drClip->drY0;
    gRect.width= drClip->drX1- drClip->drX0+ 1;
    gRect.height= drClip->drY1- drClip->drY0+ 1;

    gdk_gc_set_clip_rectangle( add->addGc, &gRect );

    return;
    }

void appDrawFillPolygon(		AppDrawingData *	add,
					const APP_POINT *	points,
					int			count )
    {
    gdk_draw_polygon( add->addDrawable, add->addGc, TRUE,
						(APP_POINT *)points, count );
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
	gdk_draw_line( add->addDrawable, add->addGc,
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
    gdk_draw_pixmap( add->addDrawable, add->addGc, pixmap,
				    xSrc, ySrc, xDest, yDest, wide, high );

    return;
    }

int appDrawGetSizeOfWidget(		int *		pWide,
					int *		pHigh,
					APP_WIDGET	w )
    {
    gint		wide;
    gint		high;

    wide= w->allocation.width;
    high= w->allocation.height;

    *pWide= wide; *pHigh= high; return 0;
    }


void appDrawFreePixmap(	AppDrawingData *	add,
			APP_BITMAP_IMAGE	pixmap )
    {
    gdk_pixmap_unref( pixmap );
    }

APP_BITMAP_MASK appMakeBitmap(	AppDrawingData *	add,
				int			wide,
				int			high )
    {
    return gdk_pixmap_new( add->addDrawable, wide, high, 1 );
    }

APP_BITMAP_IMAGE appMakePixmap(	AppDrawingData *	add,
				int			wide,
				int			high )
    {
    int			depth= gdk_visual_get_system()->depth;
    APP_BITMAP_IMAGE	rval;
    APP_COLOR_RGB	xc;

    rval= gdk_pixmap_new( add->addDrawable, wide, high, depth );

    if  ( ! rval || ! gdk_color_white( add->addColorMap, &xc ) )
	{ XDEB(rval);	}
    else{
	gdk_gc_set_foreground( add->addGc, &xc );
	gdk_draw_rectangle( rval, add->addGc, TRUE, 0, 0, wide, high );
	}

    return rval;
    }

void appDrawDrawSegments(	AppDrawingData *	add,
				APP_SEGMENT *		segments,
				int			count )
    {
    gdk_draw_segments( add->addDrawable, add->addGc, segments, count );
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
    gdk_draw_arc( add->addDrawable, add->addGc, FALSE,
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
    gdk_draw_arc( add->addDrawable, add->addGc, TRUE,
					x, y, wide, high, alpha0, alpha_step );
    }

void appDrawDrawArcs(		AppDrawingData *	add,
				const XArc *		arcs,
				int			count )
    {
    int		i;

    for ( i= 0; i < count; arcs++, i++ )
	{
	gdk_draw_arc( add->addDrawable, add->addGc, FALSE,
					    arcs->x, arcs->y,
					    arcs->width, arcs->height,
					    arcs->angle1, arcs->angle2 );
	}

    return;
    }

void appDrawFillArcs(		AppDrawingData *	add,
				const XArc *		arcs,
				int			count )
    {
    int		i;

    for ( i= 0; i < count; arcs++, i++ )
	{
	gdk_draw_arc( add->addDrawable, add->addGc, TRUE,
					    arcs->x, arcs->y,
					    arcs->width, arcs->height,
					    arcs->angle1, arcs->angle2 );
	}

    return;
    }

int appDrawSetLineAttributes(	AppDrawingData *	add,
				int			lineWidth,
				int			lineStyle,
				int			capStyle,
				int			joinStyle,
				const unsigned char *	dashList,
				int			dashCount )
    {
    gdk_gc_set_line_attributes( add->addGc,
				lineWidth, lineStyle, capStyle, joinStyle );

    if  ( lineStyle != LINEstyleSOLID )
	{
	if  ( ! dashList || dashCount == 0 )
	    { XLDEB(dashList,dashCount);	}
	else{
	    gdk_gc_set_dashes( add->addGc, 0,
				    (unsigned char *)dashList, dashCount );
	    }
	}

    return 0;
    }

#   endif
