#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	<appFrame.h>
#   include	<appMetricRuler.h>

#   include	<appDebugon.h>

#   define	BACK_MARG(s)	((s)/2)

#   define	TAG_TOP(s)	((s)/3)
#   define	TAG_BOTTOM(s)	(s)
#   define	TAG_HEIGHT(s)	(TAG_BOTTOM(s)-TAG_TOP(s))

#   define	FONT_HEIGHT(s)	(5*(s)/9)
#   define	FONT_BOTTOM(s)	(5*(s)/9)

#   define	TICK_TOP(s)	((2*(s))/3)
#   define	TICK_BOTTOM(s)	(s)
#   define	TICK_HEIGHT(s)	(TICK_BOTTOM(s)-TICK_TOP(s))

#   define	HAIR_THICK(s)	(2)

typedef struct MetricRuler
    {
    RulerData		mrRulerData;

    int			mrV0Pixels;		/*  hairs lower value	*/
    int			mrV1Pixels;		/*  hairs upper value	*/
    double		mrMagnification;
    } MetricRuler;

typedef struct MetricRulerDrag
    {
    int *		mrdValuePointer;
    int *		mrdChangedPointer;
    MetricRuler *	mrdMr;
    APP_WIDGET		mrdWidget;
    int			mrdOc;
    void *		mrdThrough;
    DragHair		mrdDragHair;
    int			mrdAsUpper;
    } MetricRulerDrag;

void * appMakeMetricRuler(	int			sizeAcross,
				double			magnifiedPixelsPerTwip,
				double			magnification,

				int			minUnused,
				int			maxUnused,

				int			documentC0,
				int			documentC1,
				int			visibleC0,
				int			visibleC1,
				int			rulerC1,

				const char *		fontName,
				int			whatUnit )
    {
    MetricRuler *	mr;
    RulerData *		rd;

    mr= malloc( sizeof( MetricRuler ) );
    if  ( ! mr )
	{ XDEB(mr); return (void *)0;	}
    rd= &(mr->mrRulerData);

    appInitRulerData( &(mr->mrRulerData) );

    mr->mrMagnification= magnification;

    rd->rdUnit= whatUnit;
    rd->rdFontName= fontName;
    appRulerCalculateIncrements( rd, magnifiedPixelsPerTwip, magnification );

    rd->rdMinUsed= minUnused;
    rd->rdMaxUsed= 0;
    rd->rdExtraAfterMaxUsed= maxUnused;

    rd->rdDocumentC0= documentC0;
    rd->rdDocumentC1= documentC1;
    rd->rdVisibleC0= visibleC0;
    rd->rdVisibleC1= visibleC1;
    rd->rdRulerC1= rulerC1;

    rd->rdSizeAcross= sizeAcross;
    rd->rdSizeAlong= 0;

    mr->mrV0Pixels= -1;
    mr->mrV1Pixels= -1;

    return (void *)mr;
    }

void appFreeMetricRuler(	void *		voidmr )
    {
    MetricRuler *		mr= (MetricRuler *)voidmr;

    appCleanRulerData( &(mr->mrRulerData) );

    free( mr );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a horizontal ruler.						*/
/*									*/
/************************************************************************/

static void appDrawHorizontalRuler(	APP_WIDGET		w,
					MetricRuler *		mr,
					DocumentRectangle *	drClip,
					int			ox )
    {
    RulerData *		rd= &(mr->mrRulerData);
    int			sizeAcross= rd->rdSizeAcross;

    int			tick;
    double		units;

    int			tagY= TAG_TOP(sizeAcross);
    int			tagH= TAG_HEIGHT(sizeAcross);

    int			tickY= TICK_TOP(sizeAcross);
    int			tickH= TICK_HEIGHT(sizeAcross);

    int			fontBottom= FONT_BOTTOM(sizeAcross);

    int			x0;
    int			x1;

    AppDrawingData *	add= &(rd->rdDrawingData);

    if  ( rd->rdSizeAlong == 0 )
	{
	int	high;

	appDrawGetSizeOfWidget( &(rd->rdSizeAlong), &high, w );

	rd->rdMaxUsed= rd->rdSizeAlong- rd->rdExtraAfterMaxUsed;
	}

    appRulerDrawBackground( rd,
		    drClip->drX0- ox, drClip->drY0,
		    drClip->drX1- drClip->drX0, drClip->drY1- drClip->drY0 );

    if  ( mr->mrV0Pixels >= 0 || mr->mrV1Pixels >= 0 )
	{
	DocumentRectangle	drSel;

	drSel.drX0= 0;
	drSel.drX1= rd->rdMaxUsed;
	if  ( mr->mrV0Pixels >= 0 )
	    { drSel.drX0= mr->mrV0Pixels;	}
	if  ( mr->mrV1Pixels >= 0 )
	    { drSel.drX1= mr->mrV1Pixels;	}
	drSel.drY0= 0;
	drSel.drY1= sizeAcross;

	if  ( geoIntersectRectangle( &drSel, &drSel, drClip ) )
	    {
	    appDrawSetForegroundColor( add, &(add->addBottomColor) );

	    appDrawFillRectangle( add,
		    drSel.drX0- ox, drSel.drY0,
		    drSel.drX1- drSel.drX0, drSel.drY1- drSel.drY0 );
	    }
	}

    appDrawSetForegroundColor( add, &(add->addForeColor) );

    x0= 0- ox;
    x1= rd->rdRulerC1- ox;

    if  ( x0 < rd->rdMinUsed )
	{ x0=  rd->rdMinUsed;	}
    if  ( x1 > rd->rdMaxUsed )
	{ x1=  rd->rdMaxUsed;	}

    if  ( x1 >= x0 )
	{
	appDrawFillRectangle( add, x0, sizeAcross- 1, x1- x0, 1 );
	}

    units= 0; tick= 0;
    for (;;)
	{
	int	twips= units* rd->rdTwipsPerUnit;
	int	iitem= rd->rdMagnifiedPixelsPerTwip* twips;

	if  ( iitem > drClip->drX1 )
	    { break;	}

	if  ( iitem- ox <  rd->rdMinUsed	||
	      iitem- ox >= rd->rdMaxUsed	)
	    { units += rd->rdUnitsPerTick; tick++; continue; }

	if  ( iitem != 0 && tick % rd->rdTicksPerText == 0 )
	    {
	    char	scratch[30];
	    int		len;
	    int		wide;

	    appRulerTagText( scratch, &wide, &len, rd, units );

	    if  ( iitem- ox- wide/2 >= rd->rdMinUsed	&&
		  iitem- ox+ wide/2 <  rd->rdMaxUsed	)
		{
		appDrawDrawString( add,
			    iitem- ox- wide/2, fontBottom, scratch, len );

		appDrawFillRectangle( add,
		    iitem- ox, fontBottom+ 2, 1, sizeAcross- fontBottom- 2 );
		}
	    else{
		appDrawFillRectangle( add, iitem- ox, tagY, 1, tagH );
		}
	    }
	else{
	    if  ( tick % rd->rdTicksPerTag == 0 )
		{ appDrawFillRectangle( add, iitem- ox, tagY, 1,  tagH );  }
	    else{ appDrawFillRectangle( add, iitem- ox, tickY, 1, tickH ); }
	    }

	units += rd->rdUnitsPerTick; tick++; continue;
	}

    /* Already done:
    XSetForeground( display, rd->rdGc, mr->trForePixel );
    */

    if  ( mr->mrV0Pixels >= 0				&&
	  mr->mrV0Pixels- ox- 2 >= rd->rdMinUsed		&&
	  mr->mrV0Pixels- ox <  rd->rdMaxUsed		)
	{
	appDrawFillRectangle( add, mr->mrV0Pixels- ox- 1, 0, 1, sizeAcross );
	}

    if  ( mr->mrV1Pixels >= 0				&&
	  mr->mrV1Pixels- ox >= rd->rdMinUsed		&&
	  mr->mrV1Pixels- ox+ 2 <  rd->rdMaxUsed	)
	{
	appDrawFillRectangle( add, mr->mrV1Pixels- ox, 0, 1, sizeAcross );
	}

    if  ( mr->mrV0Pixels >= 0 || mr->mrV1Pixels >= 0 )
	{
	appDrawSetForegroundWhite( add );

	if  ( mr->mrV0Pixels >= 0			&&
	      mr->mrV0Pixels- ox- 2 >= rd->rdMinUsed	&&
	      mr->mrV0Pixels- ox <  rd->rdMaxUsed	)
	    {
	    appDrawFillRectangle( add,
			    mr->mrV0Pixels- ox- 2, 0, 1, sizeAcross );
	    }

	if  ( mr->mrV1Pixels >= 0			&&
	      mr->mrV1Pixels- ox >= rd->rdMinUsed	&&
	      mr->mrV1Pixels- ox+ 2 <  rd->rdMaxUsed	)
	    {
	    appDrawFillRectangle( add,
			    mr->mrV1Pixels- ox+ 1, 0, 1, sizeAcross );
	    }
	}

    appDrawNoClipping( add );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a vertical ruler.						*/
/*									*/
/************************************************************************/

static void appDrawVerticalRuler(	APP_WIDGET		w,
					MetricRuler *		mr,
					DocumentRectangle *	drClip,
					int			oy )
    {
    RulerData *		rd= &(mr->mrRulerData);
    int			sizeAcross= rd->rdSizeAcross;

    int			tick;
    double		units;

    int			tagX= TAG_TOP(sizeAcross);
    int			tagW= TAG_HEIGHT(sizeAcross);

    int			tickX= TICK_TOP(sizeAcross);
    int			tickW= TICK_HEIGHT(sizeAcross);

    int			fontBottom= FONT_BOTTOM(sizeAcross);

    int			y0;
    int			y1;

    AppDrawingData *	add= &(rd->rdDrawingData);

    /*  2  */
    appDrawSetForegroundColor( add, &(add->addBackColor) );

    if  ( rd->rdSizeAlong == 0 )
	{
	int	wide;

	appDrawGetSizeOfWidget( &wide, &(rd->rdSizeAlong), w );

	rd->rdMaxUsed= rd->rdSizeAlong- rd->rdExtraAfterMaxUsed;
	}

    appDrawFillRectangle( add,
		    drClip->drX0, drClip->drY0- oy,
		    drClip->drX1- drClip->drX0, drClip->drY1- drClip->drY0 );

    if  ( mr->mrV0Pixels >= 0 || mr->mrV1Pixels >= 0 )
	{
	DocumentRectangle	drSel;

	drSel.drX0= 0;
	drSel.drX1= sizeAcross;
	drSel.drY0= 0;
	drSel.drY1= rd->rdMaxUsed;
	if  ( mr->mrV0Pixels >= 0 )
	    { drSel.drY0= mr->mrV0Pixels;	}
	if  ( mr->mrV1Pixels >= 0 )
	    { drSel.drY1= mr->mrV1Pixels;	}

	if  ( geoIntersectRectangle( &drSel, &drSel, drClip ) )
	    {
	    appDrawSetForegroundColor( add, &(add->addBottomColor) );

	    appDrawFillRectangle( add,
		    drSel.drX0, drSel.drY0- oy,
		    drSel.drX1- drSel.drX0, drSel.drY1- drSel.drY0 );
	    }
	}

    appDrawSetForegroundColor( add, &(add->addForeColor) );

    y0= 0- oy;
    y1= rd->rdRulerC1- oy;

    if  ( y0 < rd->rdMinUsed )
	{ y0=  rd->rdMinUsed;	}
    if  ( y1 > rd->rdMaxUsed )
	{ y1=  rd->rdMaxUsed;	}

    if  ( y1 >= y0 )
	{
	appDrawFillRectangle( add, sizeAcross- 1, y0, 1, y1- y0 );
	}

    units= 0; tick= 0;
    for (;;)
	{
	int	twips= units* rd->rdTwipsPerUnit;
	int	iitem= rd->rdMagnifiedPixelsPerTwip* twips;

	if  ( iitem > drClip->drY1 )
	    { break;	}

	if  ( iitem- oy <  rd->rdMinUsed	||
	      iitem- oy >= rd->rdMaxUsed	)
	    { units += rd->rdUnitsPerTick; tick++; continue; }

	if  ( iitem != 0 && tick % rd->rdTicksPerText == 0 )
	    {
	    char	scratch[30];
	    int		len;
	    int		wide;

	    int		ascent;
	    int		descent;

	    appRulerTagText( scratch, &wide, &len, rd, units );

	    ascent= FONT_HEIGHT(sizeAcross)/ 2;
	    descent= FONT_HEIGHT(sizeAcross)/ 2;

	    if  ( iitem- oy- descent >= rd->rdMinUsed	&&
		  iitem- oy+ ascent  <  rd->rdMaxUsed	)
		{
		appRulerDrawVerticalString( rd,
			    fontBottom, iitem+ wide/2- oy, scratch, len );

		appDrawFillRectangle( add,
				fontBottom+ 2, iitem- oy,
				sizeAcross- fontBottom- 2, 1 );
		}
	    else{
		appDrawFillRectangle( add, tagX, iitem- oy, tagW, 1 );
		}
	    }
	else{
	    if  ( tick % rd->rdTicksPerTag == 0 )
		{
		appDrawFillRectangle( add, tagX, iitem- oy, tagW, 1 );
		}
	    else{
		appDrawFillRectangle( add, tickX, iitem- oy, tickW, 1 );
		}
	    }

	units += rd->rdUnitsPerTick; tick++; continue;
	}

    /* Already done:
    XSetForeground( display, rd->rdGc, mr->trForePixel );
    */

    if  ( mr->mrV0Pixels >= 0				&&
	  mr->mrV0Pixels- oy- 2 >= rd->rdMinUsed		&&
	  mr->mrV0Pixels- oy <  rd->rdMaxUsed		)
	{
	appDrawFillRectangle( add, 0, mr->mrV0Pixels- oy- 1, sizeAcross, 1 );
	}

    if  ( mr->mrV1Pixels >= 0				&&
	  mr->mrV1Pixels- oy >= rd->rdMinUsed		&&
	  mr->mrV1Pixels- oy+ 2 <  rd->rdMaxUsed	)
	{
	appDrawFillRectangle( add, 0, mr->mrV1Pixels- oy, sizeAcross, 1 );
	}

    if  ( mr->mrV0Pixels >= 0 || mr->mrV1Pixels >= 0 )
	{
	appDrawSetForegroundWhite( add );

	if  ( mr->mrV0Pixels >= 0			&&
	      mr->mrV0Pixels- oy- 2 >= rd->rdMinUsed	&&
	      mr->mrV0Pixels- oy <  rd->rdMaxUsed	)
	    {
	    appDrawFillRectangle( add,
				    0, mr->mrV0Pixels- oy- 2, sizeAcross, 1 );
	    }

	if  ( mr->mrV1Pixels >= 0			&&
	      mr->mrV1Pixels- oy >= rd->rdMinUsed	&&
	      mr->mrV1Pixels- oy+ 2 <  rd->rdMaxUsed	)
	    {
	    appDrawFillRectangle( add,
				    0, mr->mrV1Pixels- oy+ 1, sizeAcross, 1 );
	    }
	}

    appDrawNoClipping( add );

    return;
    }

APP_EVENT_HANDLER_H( appRedrawHorizontalMetricRuler, w, voidmr, event )
    {
    MetricRuler *	mr= (MetricRuler *)voidmr;
    RulerData *		rd= &(mr->mrRulerData);
    AppDrawingData *	add= &(rd->rdDrawingData);

    int			ox= rd->rdVisibleC0- rd->rdMinUsed;
    int			oy= 0;

    DocumentRectangle	drClip;

    if  ( ! rd->rdDrawingDataAllocated )
	{
	const int	vertical= 0;

	appRulerAllocateDrawingData( rd, vertical,
		    FONT_HEIGHT(rd->rdSizeAcross), mr->mrMagnification, w );
	}

    appCollectExposures( &drClip, add, ox, oy, event );

    appDrawHorizontalRuler( w, mr, &drClip, ox );
    }

APP_EVENT_HANDLER_H( appRedrawVerticalMetricRuler, w, voidmr, event )
    {
    MetricRuler *	mr= (MetricRuler *)voidmr;
    RulerData *		rd= &(mr->mrRulerData);
    AppDrawingData *	add= &(rd->rdDrawingData);

    int			ox= 0;
    int			oy= rd->rdVisibleC0- rd->rdMinUsed;

    DocumentRectangle	drClip;

    if  ( ! rd->rdDrawingDataAllocated )
	{
	const int	vertical= 1;

	appRulerAllocateDrawingData( rd, vertical,
		    FONT_HEIGHT(rd->rdSizeAcross), mr->mrMagnification, w );
	}

    appCollectExposures( &drClip, add, ox, oy, event );

    appDrawVerticalRuler( w, mr, &drClip, oy );
    }

APP_EVENT_HANDLER_H( appHorizontalRulerConfigure, w, voidmr, event )
    {
    MetricRuler *	mr= (MetricRuler *)voidmr;
    RulerData *		rd= &(mr->mrRulerData);
    int			wide;
    int			high;

    if  ( appDrawGetSizeFromConfigureEvent( &wide, &high, w, event ) )
	{ return;	}

    rd->rdSizeAlong= wide;
    rd->rdMaxUsed= wide- rd->rdExtraAfterMaxUsed;

    return;
    }

APP_EVENT_HANDLER_H( appVerticalRulerConfigure, w, voidmr, event )
    {
    MetricRuler *	mr= (MetricRuler *)voidmr;
    RulerData *		rd= &(mr->mrRulerData);
    int			wide;
    int			high;

    if  ( appDrawGetSizeFromConfigureEvent( &wide, &high, w, event ) )
	{ return;	}

    rd->rdSizeAlong= high;
    rd->rdMaxUsed= high- rd->rdExtraAfterMaxUsed;

    return;
    }

/************************************************************************/
/*									*/
/*  Scroll the ruler.							*/
/*									*/
/************************************************************************/

void appScrollHorMetricRuler(	void *			voidmr,
				APP_WIDGET		w,
				int			d )
    {
    MetricRuler *		mr= (MetricRuler *)voidmr;
    RulerData *			rd= &(mr->mrRulerData);

    DocumentRectangle		drClip;

    int				ox;

    appScrollHorizontalRuler( rd, &drClip, d );

    ox= rd->rdVisibleC0- rd->rdMinUsed;

    appDrawHorizontalRuler( w, mr, &drClip, ox );
    }

void appScrollVertMetricRuler(	void *			voidmr,
				APP_WIDGET		w,
				int			d )
    {
    MetricRuler *		mr= (MetricRuler *)voidmr;
    RulerData *			rd= &(mr->mrRulerData);

    DocumentRectangle		drClip;

    int				oy;

    appScrollVerticalRuler( rd, &drClip, d );

    oy= rd->rdVisibleC0- rd->rdMinUsed;

    appDrawVerticalRuler( w, mr, &drClip, oy );
    }

void appSetMetricRulerRange(	void *			voidmr,
				APP_WIDGET		w,
				int			docVisV0,
				int			docVisV1,
				int			docBackV1 )
    {
    MetricRuler *	mr= (MetricRuler *)voidmr;
    RulerData *		rd= &(mr->mrRulerData);
    AppDrawingData *	add= &(rd->rdDrawingData);

    rd->rdVisibleC0= docVisV0;
    rd->rdVisibleC1= docVisV1;
    rd->rdMaxUsed= docBackV1;

#   ifdef USE_MOTIF
    if  ( XtIsRealized( w ) )
#   endif
#   ifdef USE_GTK
    if  ( GTK_WIDGET_REALIZED( w ) )
#   endif
	{
	if  ( ! rd->rdDrawingDataAllocated				&&
	      appSetDrawingDataForWidget( w, mr->mrMagnification, add )	)
	    { LDEB(1); return;	}

	appExposeRectangle( add, 0, 0, 0, 0 );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Let the user drag one of the controls on the top ruler.		*/
/*									*/
/*  1)  Draw a 'hair' over the document that corresponds to the		*/
/*	position of the control.					*/
/*  2)  Until the mouse button is released, observe events.		*/
/*  3)  Merge as many mouse motion events as possible.			*/
/*  4)  If the event is a mouse motion or a button release, redraw the	*/
/*	old an new position of the control on the ruler. (Assume that	*/
/*	the buttons are wider than the tab marks.			*/
/*  5)  Let the document move the hair.					*/
/*  6)  When the mouse button is released, the loop ends.		*/
/*  7)  Let the toolkit dispatch all events that are not relevant for	*/
/*	this routine.							*/
/*									*/
/************************************************************************/

static void appHorizontalRulerSetValue(	MetricRulerDrag *	mrd,
					int			newValue )
    {
    MetricRuler *		mr= mrd->mrdMr;
    RulerData *			rd= &(mr->mrRulerData);
    int				oldValue= *(mrd->mrdValuePointer);

    DocumentRectangle		drClip;

    if  ( newValue == oldValue )
	{ return;	}

    *(mrd->mrdValuePointer)= newValue;
    *(mrd->mrdChangedPointer)= 1;

    if  ( oldValue < newValue )
	{
	drClip.drX0= oldValue- rd->rdSizeAcross;
	drClip.drX1= newValue+ rd->rdSizeAcross;
	drClip.drY0= 0;
	drClip.drY1= rd->rdSizeAcross;
	}
    else{
	drClip.drX0= newValue- rd->rdSizeAcross;
	drClip.drX1= oldValue+ rd->rdSizeAcross;
	drClip.drY0= 0;
	drClip.drY1= rd->rdSizeAcross;
	}

    appDrawHorizontalRuler( mrd->mrdWidget, mr, &drClip, mrd->mrdOc );

    /*  5  */
    (*mrd->mrdDragHair)( mrd->mrdThrough, oldValue, newValue, mrd->mrdAsUpper );

    return;
    }

static APP_EVENT_HANDLER_H( appHorizontalRulerMouseMove, w, vmrd, event )
    {
    MetricRulerDrag *		mrd= (MetricRulerDrag *)vmrd;

    int				x;
    int				y;

    if  ( appGetCoordinatesFromMouseMoveEvent( &x, &y, w, event ) )
	{ return;	}

    appHorizontalRulerSetValue( mrd, x+ mrd->mrdOc );

    return;
    }

static APP_EVENT_HANDLER_H( appHorizontalRulerMouseUp, w, vmrd, event )
    {
    MetricRulerDrag *		mrd= (MetricRulerDrag *)vmrd;

    int				x;
    int				y;

    int				button;
    int				upDown;
    int				seq;
    unsigned int		keyState;

    if  ( appGetCoordinatesFromMouseButtonEvent(
			&x, &y, &button, &upDown, &seq, &keyState, w, event ) )
	{ return;	}

    appHorizontalRulerSetValue( mrd, x+ mrd->mrdOc );

    (*mrd->mrdDragHair)( mrd->mrdThrough,
			    *(mrd->mrdValuePointer), -1, mrd->mrdAsUpper );
    return;
    }

static void appHorizontalRulerDragItem(	int *			pValue,
					int *			pChanged,
					APP_WIDGET		w,
					EditApplication *	ea,
					APP_EVENT *		downEvent,
					MetricRuler *		mr,
					int			ox,
					void *			voided,
					DragHair		dragHair,
					int			asUpper )
    {
    MetricRulerDrag		mrd;

    mrd.mrdValuePointer= pValue;
    mrd.mrdChangedPointer= pChanged;
    mrd.mrdMr= mr;
    mrd.mrdWidget= w;
    mrd.mrdOc= ox;
    mrd.mrdThrough= voided;
    mrd.mrdDragHair= dragHair;
    mrd.mrdAsUpper= asUpper;

    /*  1  */
    (*mrd.mrdDragHair)( mrd.mrdThrough,
				-1, *(mrd.mrdValuePointer), mrd.mrdAsUpper );

    appRunDragLoop( w, ea, downEvent,
				appHorizontalRulerMouseUp,
				appHorizontalRulerMouseMove,
				0, (APP_TIMER_CALLBACK)0,
				(void *)&mrd );
    }

/************************************************************************/
/*									*/
/*  The user clicked on the top ruler, make her happy.			*/
/*									*/
/*  1)  Only accept 'left mouse down' events.				*/
/*									*/
/************************************************************************/

void     appHorizontalRulerTrackMouse(	int *			pX0pixels,
					int *			pX1pixels,
					int *			pChanged,
					APP_WIDGET		w,
					EditApplication *	ea,
					APP_EVENT *		downEvent,
					void *			voidmr,
					void *			voided,
					DragHair		dragHair )
    {
    MetricRuler  *	mr= (MetricRuler *)voidmr;
    RulerData *		rd= &(mr->mrRulerData);
    AppDrawingData *	add= &(rd->rdDrawingData);

    int			x;
    int			y;
    int			ox= rd->rdVisibleC0- rd->rdMinUsed;

    int			changed= 0;
    int			asUpper;

    int			xp= 2;

    int			button;
    int			upDown;
    int			seq;
    unsigned int	keyState;

    if  ( appGetCoordinatesFromMouseButtonEvent(
		    &x, &y, &button, &upDown, &seq, &keyState, w, downEvent ) )
	{ return;	}

    if  ( upDown < 1 || button != 1 || seq > 1 )
	{ return;	}

    if  ( mr->mrV0Pixels >= 0				&&
	  x+ ox <  mr->mrV0Pixels + xp			&&
	  x+ ox >= mr->mrV0Pixels- HAIR_THICK( sizeAcross ) -xp	)
	{
	asUpper= 0;

	appHorizontalRulerDragItem( &mr->mrV0Pixels, &changed,
		    w, ea, downEvent, mr, ox, voided, dragHair, asUpper );
	goto ready;
	}

    if  ( mr->mrV1Pixels >= 0					&&
	  x+ ox >= mr->mrV1Pixels- xp				&&
	  x+ ox <  mr->mrV1Pixels+ HAIR_THICK( sizeAcross )+ xp	)
	{
	asUpper= 1;

	appHorizontalRulerDragItem( &mr->mrV1Pixels, &changed,
		    w, ea, downEvent, mr, ox, voided, dragHair, asUpper );

	goto ready;
	}

    if  ( mr->mrV0Pixels >= 0 )
	{
	if  ( mr->mrV1Pixels >= 0 )
	    { return;	}

	if  ( x+ ox < mr->mrV1Pixels )
	    {
	    asUpper= 0;
	    mr->mrV1Pixels= mr->mrV0Pixels;

	    mr->mrV0Pixels= x+ ox;

	    changed= 1;
	    (*dragHair)( voided, -1, x+ ox, asUpper );

	    appHorizontalRulerDragItem( &mr->mrV0Pixels, &changed,
		    w, ea, downEvent, mr, ox, voided, dragHair, asUpper );

	    goto ready;
	    }
	else{
	    asUpper= 1;
	    mr->mrV1Pixels= x+ ox;

	    changed= 1;
	    (*dragHair)( voided, -1, x+ ox, asUpper );

	    appHorizontalRulerDragItem( &mr->mrV1Pixels, &changed,
		    w, ea, downEvent, mr, ox, voided, dragHair, asUpper );

	    goto ready;
	    }
	}

    if  ( mr->mrV1Pixels >= 0 )
	{
	if  ( x+ ox > mr->mrV1Pixels )
	    {
	    asUpper= 1;
	    mr->mrV0Pixels= mr->mrV1Pixels;

	    mr->mrV1Pixels= x+ ox;

	    changed= 1;
	    (*dragHair)( voided, -1, x+ ox, asUpper );

	    appHorizontalRulerDragItem( &mr->mrV1Pixels, &changed,
		    w, ea, downEvent, mr, ox, voided, dragHair, asUpper );

	    goto ready;
	    }
	else{
	    asUpper= 0;
	    mr->mrV0Pixels= x+ ox;

	    changed= 1;
	    (*dragHair)( voided, -1, x+ ox, asUpper );

	    appHorizontalRulerDragItem( &mr->mrV0Pixels, &changed,
		    w, ea, downEvent, mr, ox, voided, dragHair, asUpper );

	    goto ready;
	    }
	}

    if  ( x+ ox <= ( rd->rdMinUsed+ rd->rdMaxUsed )/ 2 )
	{
	asUpper= 0;
	mr->mrV0Pixels= x+ ox;

	changed= 1;
	(*dragHair)( voided, -1, x+ ox, asUpper );

	appHorizontalRulerDragItem( &mr->mrV0Pixels, &changed,
		    w, ea, downEvent, mr, ox, voided, dragHair, asUpper );
	}
    else{
	asUpper= 1;
	mr->mrV1Pixels= x+ ox;

	changed= 1;
	(*dragHair)( voided, -1, x+ ox, asUpper );

	appHorizontalRulerDragItem( &mr->mrV1Pixels, &changed,
		    w, ea, downEvent, mr, ox, voided, dragHair, asUpper );
	}

  ready:
    if  ( mr->mrV0Pixels >= 0			&&
	  mr->mrV1Pixels >= 0			&&
	  mr->mrV0Pixels > mr->mrV1Pixels	)
	{
	int		swap;

	changed= 1;

	(*dragHair)( voided, mr->mrV1Pixels, -1, 1 );
	(*dragHair)( voided, mr->mrV0Pixels, -1, 0 );

	swap= mr->mrV1Pixels;
	mr->mrV1Pixels= mr->mrV0Pixels;
	mr->mrV0Pixels= swap;

	(*dragHair)( voided, -1, mr->mrV1Pixels, 1 );
	(*dragHair)( voided, -1, mr->mrV0Pixels, 0 );
	}

    if  ( changed )
	{
	*pChanged= 1;
	appExposeRectangle( add, 0, 0, 0, 0 );
	}

    *pX0pixels= mr->mrV0Pixels;
    *pX1pixels= mr->mrV1Pixels;

    return;
    }

/************************************************************************/
/*									*/
/*  Let the user drag one of the controls on the left ruler.		*/
/*									*/
/*  1)  Draw a 'hair' over the document that corresponds to the		*/
/*	position of the control.					*/
/*  2)  Until the mouse button is released, observe events.		*/
/*  3)  Merge as many mouse motion events as possible.			*/
/*  4)  If the event is a mouse motion or a button release, redraw the	*/
/*	old an new position of the control on the ruler. (Assume that	*/
/*	the buttons are wider than the tab marks.			*/
/*  5)  Let the document move the hair.					*/
/*  6)  When the mouse button is released, the loop ends.		*/
/*  7)  Let the toolkit dispatch all events that are not relevant for	*/
/*	this routine.							*/
/*									*/
/************************************************************************/

static void appVerticalRulerSetValue(	MetricRulerDrag *	mrd,
					int			newValue )
    {
    MetricRuler *		mr= mrd->mrdMr;
    RulerData *			rd= &(mr->mrRulerData);
    int				oldValue= *(mrd->mrdValuePointer);

    DocumentRectangle		drClip;

    if  ( newValue < rd->rdDocumentC0 )
	{ newValue= rd->rdDocumentC0;	}
    if  ( newValue > rd->rdDocumentC1 )
	{ newValue= rd->rdDocumentC1;	}

    if  ( newValue == oldValue )
	{ return;	}

    *(mrd->mrdValuePointer)= newValue;
    *(mrd->mrdChangedPointer)= 1;

    if  ( oldValue < newValue )
	{
	drClip.drX0= 0;
	drClip.drX1= rd->rdSizeAcross;
	drClip.drY0= oldValue- rd->rdSizeAcross;
	drClip.drY1= newValue+ rd->rdSizeAcross;
	}
    else{
	drClip.drX0= 0;
	drClip.drX1= rd->rdSizeAcross;
	drClip.drY0= newValue- rd->rdSizeAcross;
	drClip.drY1= oldValue+ rd->rdSizeAcross;
	}

    appDrawVerticalRuler( mrd->mrdWidget, mr, &drClip, mrd->mrdOc );

    /*  5  */
    (*mrd->mrdDragHair)( mrd->mrdThrough, oldValue, newValue, mrd->mrdAsUpper );

    return;
    }

static APP_EVENT_HANDLER_H( appVerticalRulerMouseMove, w, vmrd, event )
    {
    MetricRulerDrag *		mrd= (MetricRulerDrag *)vmrd;

    int				x;
    int				y;

    if  ( appGetCoordinatesFromMouseMoveEvent( &x, &y, w, event ) )
	{ return;	}

    appVerticalRulerSetValue( mrd, y+ mrd->mrdOc );

    return;
    }

static APP_EVENT_HANDLER_H( appVerticalRulerMouseUp, w, vmrd, event )
    {
    MetricRulerDrag *	mrd= (MetricRulerDrag *)vmrd;

    int			x;
    int			y;

    int			button;
    int			upDown;
    int			seq;
    unsigned int	keyState;

    if  ( appGetCoordinatesFromMouseButtonEvent(
			&x, &y, &button, &upDown, &seq, &keyState, w, event ) )
	{ return;	}

    appVerticalRulerSetValue( mrd, y+ mrd->mrdOc );

    (*mrd->mrdDragHair)( mrd->mrdThrough,
			    *(mrd->mrdValuePointer), -1, mrd->mrdAsUpper );

    return;
    }

static void appVerticalRulerDragItem(	int *			pValue,
					int *			pChanged,
					APP_WIDGET		w,
					EditApplication *	ea,
					APP_EVENT *		downEvent,
					MetricRuler *		mr,
					int			oy,
					void *			voided,
					DragHair		dragHair,
					int			asUpper )
    {
    MetricRulerDrag		mrd;

    mrd.mrdValuePointer= pValue;
    mrd.mrdChangedPointer= pChanged;
    mrd.mrdMr= mr;
    mrd.mrdWidget= w;
    mrd.mrdOc= oy;
    mrd.mrdThrough= voided;
    mrd.mrdDragHair= dragHair;
    mrd.mrdAsUpper= asUpper;

    /*  1  */
    (*mrd.mrdDragHair)( mrd.mrdThrough,
				-1, *(mrd.mrdValuePointer), mrd.mrdAsUpper );

    appRunDragLoop( w, ea, downEvent,
				appVerticalRulerMouseUp,
				appVerticalRulerMouseMove,
				0, (APP_TIMER_CALLBACK)0,
				(void *)&mrd );
    }

/************************************************************************/
/*									*/
/*  The user clicked on the left ruler, make her happy.			*/
/*									*/
/*  1)  Only accept 'left mouse down' events.				*/
/*									*/
/************************************************************************/

void     appVerticalRulerTrackMouse(	int *			pY0pixels,
					int *			pY1pixels,
					int *			pChanged,
					APP_WIDGET		w,
					EditApplication *	ea,
					APP_EVENT *		downEvent,
					void *			voidmr,
					void *			voided,
					DragHair		dragHair )
    {
    MetricRuler  *	mr= (MetricRuler *)voidmr;
    RulerData *		rd= &(mr->mrRulerData);
    AppDrawingData *	add= &(rd->rdDrawingData);

    int			x;
    int			y;
    int			oy= rd->rdVisibleC0- rd->rdMinUsed;

    int			changed= 0;
    int			asUpper;

    int			xp= 2;

    int			button;
    int			upDown;
    int			seq;
    unsigned int	keyState;

    if  ( appGetCoordinatesFromMouseButtonEvent(
		    &x, &y, &button, &upDown, &seq, &keyState, w, downEvent ) )
	{ return;	}

    if  ( upDown < 1 || button != 1 || seq > 1 )
	{ return;	}

    if  ( mr->mrV0Pixels >= 0					&&
	  y+ oy <  mr->mrV0Pixels + xp			&&
	  y+ oy >= mr->mrV0Pixels- HAIR_THICK( sizeAcross ) -xp	)
	{
	asUpper= 0;

	appVerticalRulerDragItem( &(mr->mrV0Pixels), &changed,
		    w, ea, downEvent, mr, oy, voided, dragHair, asUpper );

	goto ready;
	}

    if  ( mr->mrV1Pixels >= 0					&&
	  y+ oy >= mr->mrV1Pixels- xp			&&
	  y+ oy <  mr->mrV1Pixels+ HAIR_THICK( sizeAcross )+ xp	)
	{
	asUpper= 1;

	appVerticalRulerDragItem( &mr->mrV1Pixels, &changed,
		    w, ea, downEvent, mr, oy, voided, dragHair, asUpper );

	goto ready;
	}

    if  ( mr->mrV0Pixels >= 0 )
	{
	if  ( mr->mrV1Pixels >= 0 )
	    { return;	}

	if  ( y+ oy < mr->mrV1Pixels )
	    {
	    asUpper= 0;
	    mr->mrV1Pixels= mr->mrV0Pixels;

	    mr->mrV0Pixels= y+ oy;

	    changed= 1;
	    (*dragHair)( voided, -1, y+ oy, asUpper );

	    appVerticalRulerDragItem( &mr->mrV0Pixels, &changed,
		    w, ea, downEvent, mr, oy, voided, dragHair, asUpper );

	    goto ready;
	    }
	else{
	    asUpper= 1;
	    mr->mrV1Pixels= y+ oy;

	    changed= 1;
	    (*dragHair)( voided, -1, y+ oy, asUpper );

	    appVerticalRulerDragItem( &mr->mrV1Pixels, &changed,
		    w, ea, downEvent, mr, oy, voided, dragHair, asUpper );

	    goto ready;
	    }
	}

    if  ( mr->mrV1Pixels >= 0 )
	{
	if  ( y+ oy > mr->mrV1Pixels )
	    {
	    asUpper= 1;
	    mr->mrV0Pixels= mr->mrV1Pixels;

	    mr->mrV1Pixels= y+ oy;

	    changed= 1;
	    (*dragHair)( voided, -1, y+ oy, asUpper );

	    appVerticalRulerDragItem( &mr->mrV1Pixels, &changed,
		    w, ea, downEvent, mr, oy, voided, dragHair, asUpper );

	    goto ready;
	    }
	else{
	    asUpper= 0;
	    mr->mrV0Pixels= y+ oy;

	    changed= 1;
	    (*dragHair)( voided, -1, y+ oy, asUpper );

	    appVerticalRulerDragItem( &mr->mrV0Pixels, &changed,
		    w, ea, downEvent, mr, oy, voided, dragHair, asUpper );

	    goto ready;
	    }
	}

    if  ( y+ oy <= ( rd->rdMinUsed+ rd->rdMaxUsed )/ 2 )
	{
	asUpper= 0;
	mr->mrV0Pixels= y+ oy;

	changed= 1;
	(*dragHair)( voided, -1, y+ oy, asUpper );

	appVerticalRulerDragItem( &mr->mrV0Pixels, &changed,
		    w, ea, downEvent, mr, oy, voided, dragHair, asUpper );

	goto ready;
	}
    else{
	asUpper= 1;
	mr->mrV1Pixels= y+ oy;

	changed= 1;
	(*dragHair)( voided, -1, y+ oy, asUpper );

	appVerticalRulerDragItem( &mr->mrV1Pixels, &changed,
		    w, ea, downEvent, mr, oy, voided, dragHair, asUpper );

	goto ready;
	}

  ready:
    if  ( mr->mrV0Pixels >= 0			&&
	  mr->mrV1Pixels >= 0			&&
	  mr->mrV0Pixels > mr->mrV1Pixels	)
	{
	int		swap;

	changed= 1;

	(*dragHair)( voided, mr->mrV1Pixels, -1, 1 );
	(*dragHair)( voided, mr->mrV0Pixels, -1, 0 );

	swap= mr->mrV1Pixels;
	mr->mrV1Pixels= mr->mrV0Pixels;
	mr->mrV0Pixels= swap;

	(*dragHair)( voided, -1, mr->mrV1Pixels, 1 );
	(*dragHair)( voided, -1, mr->mrV0Pixels, 0 );
	}

    if  ( changed )
	{
	*pChanged= 1;
	appExposeRectangle( add, 0, 0, 0, 0 );
	}

    *pY0pixels= mr->mrV0Pixels;
    *pY1pixels= mr->mrV1Pixels;

    return;
    }

/************************************************************************/
/*									*/
/*  Set the selection of a horizontal/vertical ruler.			*/
/*									*/
/************************************************************************/

void appSetMetricRulerValues(	APP_WIDGET	w,
				void *		voidmr,
				int		v0,
				int		v1 )
    {
    MetricRuler  *	mr= (MetricRuler *)voidmr;
    RulerData *		rd= &(mr->mrRulerData);
    AppDrawingData *	add= &(rd->rdDrawingData);

    int			changed= 0;

    if  ( v0 != mr->mrV0Pixels )
	{ mr->mrV0Pixels= v0; changed= 1; }

    if  ( v1 != mr->mrV1Pixels )
	{ mr->mrV1Pixels= v1; changed= 1; }

    if  ( changed )
	{ appExposeRectangle( add, 0, 0, 0, 0 );	}
    }

void appSetMetricRulerMagnification(	APP_WIDGET	w,
					void *		voidmr,
					double		magnifiedPixelsPerTwip,
					double		magnification )
    {
    MetricRuler  *		mr= (MetricRuler *)voidmr;
    RulerData *			rd= &(mr->mrRulerData);
    AppDrawingData *		add= &(rd->rdDrawingData);

    appRulerCalculateIncrements( rd, magnifiedPixelsPerTwip, magnification );

    appExposeRectangle( add, 0, 0, 0, 0 );

    return;
    }

void appAdaptMetricRuler(	APP_WIDGET	w,
				void *		voidmr,
				int		documentC1,
				int		rulerC1 )
    {
    MetricRuler  *	mr= (MetricRuler *)voidmr;
    RulerData *		rd= &(mr->mrRulerData);
    AppDrawingData *	add= &(rd->rdDrawingData);

    rd->rdDocumentC1= documentC1;

    if  ( rd->rdRulerC1 != rulerC1 )
	{
	rd->rdRulerC1= rulerC1;

	appExposeRectangle( add, 0, 0, 0, 0 );
	}
    }

