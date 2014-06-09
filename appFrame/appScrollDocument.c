/************************************************************************/
/*									*/
/*  Document window application independent scrolling functionality.	*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	<appSystem.h>
#   include	"appFrame.h"

#   include	<appDebugon.h>

#   define	WHEEL_STEP	10
#   define	BAR_STEP	20

/************************************************************************/
/*									*/
/*  Callbacks for the scrollbars, and the size admin  of a document.	*/
/*									*/
/************************************************************************/

static void appDocScrollVertically(	EditDocument *		ed,
					int			scrolledY )
    {
    EditApplication *		ea= ed->edApplication;
    AppDrawingData *		add= &(ed->edDrawingData);

    int				ox;
    int				oy;
    int				high;
    int				wide;

    DocumentRectangle		drClip;
    DocumentRectangle		drScreen;

    appDrawNoClipping( add );

    drClip= add->addBackRect;

    high= ed->edVisibleRect.drY1- ed->edVisibleRect.drY0+ 1;
    wide= ed->edVisibleRect.drX1- ed->edVisibleRect.drX0+ 1;

    if  ( scrolledY > 0 )
	{
	if  ( high > scrolledY )
	    {
	    appDrawMoveArea( add, 0, scrolledY, wide, high- scrolledY, 0, 0 );

	    drClip.drY0= ed->edVisibleRect.drY1- scrolledY;
	    drClip.drY1= ed->edVisibleRect.drY1;
	    }
	else{
	    drClip.drY0= ed->edVisibleRect.drY0;
	    drClip.drY1= ed->edVisibleRect.drY1;
	    }
	}
    else{
	if  ( high+ scrolledY > 0 )
	    {
	    appDrawMoveArea( add, 0, 0, wide, high+ scrolledY, 0, -scrolledY );

	    drClip.drY0= ed->edVisibleRect.drY0;
	    drClip.drY1= ed->edVisibleRect.drY0- scrolledY;
	    }
	else{
	    drClip.drY0= ed->edVisibleRect.drY0;
	    drClip.drY1= ed->edVisibleRect.drY1;
	    }
	}

    ox= ed->edVisibleRect.drX0;
    oy= ed->edVisibleRect.drY0;

    drScreen.drX0= drClip.drX0- ox;
    drScreen.drY0= drClip.drY0- oy;
    drScreen.drX1= drClip.drX1- ox;
    drScreen.drY1= drClip.drY1- oy;

    appDrawSetClipRect( add, &drScreen );

    (*ea->eaDrawRectangle)( ed->edDocumentWidget, ed, &drClip, ox, oy );

    if  ( ed->edLeftRuler && ea->eaScrollVerticalRuler )
	{
	(*ea->eaScrollVerticalRuler)
			( ed->edLeftRuler, ed->edLeftRulerWidget, scrolledY );
	}

    return;
    }

static void appDocScrollHorizontally(	EditDocument *		ed,
					int			scrolledX )
    {
    EditApplication *		ea= ed->edApplication;
    AppDrawingData *		add= &(ed->edDrawingData);

    int				ox;
    int				oy;
    int				high;
    int				wide;

    DocumentRectangle		drClip;
    DocumentRectangle		drScreen;

    appDrawNoClipping( add );

    drClip= add->addBackRect;

    high= ed->edVisibleRect.drY1- ed->edVisibleRect.drY0+ 1;
    wide= ed->edVisibleRect.drX1- ed->edVisibleRect.drX0+ 1;

    if  ( scrolledX > 0 )
	{
	if  ( wide > scrolledX )
	    {
	    appDrawMoveArea( add, scrolledX, 0, wide- scrolledX, high, 0, 0 );

	    drClip.drX0= ed->edVisibleRect.drX1- scrolledX;
	    drClip.drX1= ed->edVisibleRect.drX1;
	    }
	else{
	    drClip.drX0= ed->edVisibleRect.drX0;
	    drClip.drX1= ed->edVisibleRect.drX1;
	    }
	}
    else{
	if  ( wide+ scrolledX > 0 )
	    {
	    appDrawMoveArea( add, 0, 0, wide+ scrolledX, high, -scrolledX, 0 );

	    drClip.drX0= ed->edVisibleRect.drX0;
	    drClip.drX1= ed->edVisibleRect.drX0- scrolledX;
	    }
	else{
	    drClip.drX0= ed->edVisibleRect.drX0;
	    drClip.drX1= ed->edVisibleRect.drX1;
	    }
	}

    ox= ed->edVisibleRect.drX0;
    oy= ed->edVisibleRect.drY0;

    drScreen.drX0= drClip.drX0- ox;
    drScreen.drY0= drClip.drY0- oy;
    drScreen.drX1= drClip.drX1- ox;
    drScreen.drY1= drClip.drY1- oy;

    appDrawSetClipRect( add, &drScreen );

    (*ea->eaDrawRectangle)( ed->edDocumentWidget, ed, &drClip, ox, oy );

    if  ( ed->edTopRuler && ea->eaScrollHorizontalRuler )
	{
	(*ea->eaScrollHorizontalRuler)
			( ed->edTopRuler, ed->edTopRulerWidget, scrolledX );
	}

    return;
    }

void appDocVerticalScrollbarCallback(	APP_WIDGET	w,
					void *		voided,
					void *		voidscbs )
    {
    EditDocument *		ed= (EditDocument *)voided;

    int				scrolledY;

    scrolledY= appGuiGetScrollbarValueFromCallback( w, voidscbs )-
						    ed->edVisibleRect.drY0;

    ed->edVisibleRect.drY0 += scrolledY;
    ed->edVisibleRect.drY1 += scrolledY;

    if  ( scrolledY == 0 )
	{ return;	}

    appDocScrollVertically( ed, scrolledY );

    return;
    }


void appDocHorizontalScrollbarCallback(	APP_WIDGET	w,
					void *		voided,
					void *		voidscbs )
    {
    EditDocument *		ed= (EditDocument *)voided;

    int				scrolledX;

    scrolledX= appGuiGetScrollbarValueFromCallback( w, voidscbs )-
						    ed->edVisibleRect.drX0;


    ed->edVisibleRect.drX0 += scrolledX;
    ed->edVisibleRect.drX1 += scrolledX;

    if  ( scrolledX == 0 )
	{ return;	}

    appDocScrollHorizontally( ed, scrolledX );

    return;
    }

/************************************************************************/
/*									*/
/*  Scroll the selection into view.					*/
/*									*/
/*  NOTE: sliderSize is passed to XmScrollBarSetValues() because of a	*/
/*	bug in lesstif Release 0.87.0. (Jan 1999).			*/
/*									*/
/************************************************************************/

void appScrollToRectangle(	EditDocument *		ed,
				int			x0,
				int			y0,
				int			x1,
				int			y1,
				int *			pScrolledX,
				int *			pScrolledY )
    {
    const AppDrawingData *	add= &(ed->edDrawingData);

    int				sliderSize;
	
    int				oox= ed->edVisibleRect.drX0;
    int				ooy= ed->edVisibleRect.drY0;
    int				nox;
    int				noy;

    int				set;

    nox= oox;
    noy= ooy;

    appGuiGetScrollbarValues( &noy, &sliderSize, ed->edVerticalScrollbar );

    if  ( y1 > noy+ sliderSize )
	{
	set= y1- sliderSize;

	if  ( noy != set )
	    {
	    appGuiSetScrollbarValues( ed->edVerticalScrollbar,
							set, sliderSize );
	    noy= set;
	    }
	}

    if  ( y0 < noy )
	{
	set= y0;

	if  ( noy != set )
	    {
	    appGuiSetScrollbarValues( ed->edVerticalScrollbar,
							set, sliderSize );
	    noy= set;
	    }
	}

    if  ( noy+ sliderSize > add->addBackRect.drY1 )
	{
	set= add->addBackRect.drY1- sliderSize+ 1;
	if  ( set < 0 )
	    { set= 0;	}

	if  ( noy != set )
	    {
	    appGuiSetScrollbarValues( ed->edVerticalScrollbar,
							set, sliderSize );
	    noy= set;
	    }
	}

    appGuiGetScrollbarValues( &nox, &sliderSize, ed->edHorizontalScrollbar );

    if  ( x1 > nox+ sliderSize )
	{
	set= x1- sliderSize;
	if  ( nox != set )
	    {
	    appGuiSetScrollbarValues( ed->edHorizontalScrollbar,
							set, sliderSize );
	    nox= set;
	    }
	}

    if  ( x0 < nox )
	{
	set= x0;
	if  ( nox != set )
	    {
	    appGuiSetScrollbarValues( ed->edHorizontalScrollbar,
							set, sliderSize );
	    nox= set;
	    }
	}

    if  ( nox+ sliderSize > add->addBackRect.drX1 )
	{
	set= add->addBackRect.drX1- sliderSize+ 1;
	if  ( set < 0 )
	    { set= 0;	}

	if  ( nox != set )
	    {
	    appGuiSetScrollbarValues( ed->edHorizontalScrollbar,
							set, sliderSize );
	    nox= set;
	    }
	}

    *pScrolledX= nox- oox;
    *pScrolledY= noy- ooy;

    ed->edVisibleRect.drX0 += *pScrolledX;
    ed->edVisibleRect.drY0 += *pScrolledY;
    ed->edVisibleRect.drX1 += *pScrolledX;
    ed->edVisibleRect.drY1 += *pScrolledY;

    if  ( *pScrolledX != 0 )
	{ appDocScrollHorizontally( ed, *pScrolledX );	}
    if  ( *pScrolledY != 0 )
	{ appDocScrollVertically( ed, *pScrolledY );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Prevent the Shell that contains a document from being resized	*/
/*  beyond normal limits.						*/
/*									*/
/************************************************************************/

static void appFileAdaptHorizontalRulerRange(	EditDocument *	ed,
						int		width )
    {
    EditApplication *		ea= ed->edApplication;

    if  ( ed->edTopRuler && ea->eaSetHorizontalRulerRange )
	{
	int	leftRulerWide= 0;
	int	leftRulerHigh= 0;

	if  ( ed->edLeftRulerWidget )
	    {
	    appDrawGetSizeOfWidget( &leftRulerWide, &leftRulerHigh,
						    ed->edLeftRulerWidget );
	    }

	(*ea->eaSetHorizontalRulerRange)( ed->edTopRuler,
					ed->edTopRulerWidget,
					ed->edVisibleRect.drX0,
					ed->edVisibleRect.drX1,
					width+ leftRulerWide );
	}

    return;
    }

static void appFileAdaptVerticalRulerRange(	EditDocument *	ed,
						int		height )
    {
    EditApplication *		ea= ed->edApplication;

    if  ( ed->edLeftRuler && ea->eaSetVerticalRulerRange )
	{
	(*ea->eaSetVerticalRulerRange)( ed->edLeftRuler,
					ed->edLeftRulerWidget,
					ed->edVisibleRect.drY0,
					ed->edVisibleRect.drY1,
					height );
	}

    return;
    }

void appAdaptToDocumentSize(	EditDocument *	ed,
				int		width,
				int		height )
    {
    AppDrawingData *	add= &(ed->edDrawingData);
    int			changed= 0;
    int			d;

    if  ( ed->edVisibleRect.drY1 != ed->edVisibleRect.drY0+ height- 1 )
	{
	ed->edVisibleRect.drY1= ed->edVisibleRect.drY0+ height- 1;
	changed= 1;

	if  ( ed->edVisibleRect.drY1 > add->addBackRect.drY1 )
	    {
	    d= ed->edVisibleRect.drY1- add->addBackRect.drY1;

	    if  ( d > ed->edVisibleRect.drY0 )
		{ d= ed->edVisibleRect.drY0;	}

	    ed->edVisibleRect.drY0 -= d;
	    ed->edVisibleRect.drY1 -= d;
	    }

	appFileAdaptVerticalRulerRange( ed, height );

	appExposeRectangle( add, 0, 0, 0, 0 );
	}

    if  ( ed->edVisibleRect.drX1 != ed->edVisibleRect.drX0+ width+ 1 )
	{
	ed->edVisibleRect.drX1= ed->edVisibleRect.drX0+ width+ 1;
	changed= 1;

	if  ( ed->edVisibleRect.drX1 > add->addBackRect.drX1 )
	    {
	    d= ed->edVisibleRect.drX1- add->addBackRect.drX1;

	    if  ( d > ed->edVisibleRect.drX0 )
		{ d= ed->edVisibleRect.drX0;	}

	    ed->edVisibleRect.drX0 -= d;
	    ed->edVisibleRect.drX1 -= d;
	    }

	appFileAdaptHorizontalRulerRange( ed, width );

	appExposeRectangle( add, 0, 0, 0, 0 );
	}

    if  ( changed )
	{ appDocSetScrollbarValues( ed );	}

    return;
    }

void appDocumentRulerWidth(	EditApplication *	ea,
				EditDocument *		ed )
    {
    int			mult;

    double		horPixPerMM;
    double		verPixPerMM;
    double		xfac;
    double		yfac;

    appGetFactors( ea, &horPixPerMM, &verPixPerMM, &xfac, &yfac );

    /*  1  */
    ed->edLeftRulerWidthPixels=
			(int)( ea->eaLeftRulerWidthMM* horPixPerMM );
    ed->edTopRulerHeightPixels=
			(int)( ea->eaTopRulerHeightMM* verPixPerMM );
    ed->edRightRulerWidthPixels=
			(int)( ea->eaRightRulerWidthMM * horPixPerMM );
    ed->edBottomRulerHeightPixels=
			(int)( ea->eaBottomRulerHeightMM* verPixPerMM );

    mult= ea->eaLeftRulerWidthMultiple;
    if  ( mult )
	{
	ed->edLeftRulerWidthPixels=
		    mult* ( ( ed->edLeftRulerWidthPixels+ mult- 1 )/ mult );
	}

    mult= ea->eaTopRulerHeightMultiple;
    if  ( mult )
	{
	ed->edTopRulerHeightPixels=
		    mult* ( ( ed->edTopRulerHeightPixels+ mult- 1 )/ mult );
	}

    mult= ea->eaRightRulerWidthMultiple;
    if  ( mult )
	{
	ed->edRightRulerWidthPixels=
		    mult* ( ( ed->edRightRulerWidthPixels+ mult- 1 )/ mult );
	}

    mult= ea->eaBottomRulerHeightMultiple;
    if  ( mult )
	{
	ed->edBottomRulerHeightPixels=
		    mult* ( ( ed->edBottomRulerHeightPixels+ mult- 1 )/ mult );
	}

    return;
    }

void appDocumentCalculateExtraSize(	EditDocument *	ed )
    {
    int	shellWide;
    int	shellHigh;

    int	docWide;
    int	docHigh;

    appDrawGetSizeOfWidget( &shellWide, &shellHigh,
					    ed->edToplevel.atTopWidget );

    appDrawGetSizeOfWidget( &docWide, &docHigh,
					    ed->edDocumentWidget );

    ed->edShellExtraWidth= shellWide- docWide;
    ed->edShellExtraHeight= shellHigh- docHigh;

    appSetShellConstraints( ed );

    appAdaptToDocumentSize( ed, docWide, docHigh );
    }

APP_EVENT_HANDLER_H( appDocConfigure, w, voided, event )
    {
    EditDocument *	ed= (EditDocument *)voided;

    int			wide;
    int			high;

    if  ( appDrawGetSizeFromConfigureEvent( &wide, &high, w, event ) )
	{ return;	}

    appAdaptToDocumentSize( ed, wide, high );

    return;
    }

void appMouseWheelDown(		EditDocument *	ed )
    {
    AppDrawingData *	add= &(ed->edDrawingData);

    int			ooy= ed->edVisibleRect.drY0;
    int			noy= ed->edVisibleRect.drY0;
    int			scrolledY;

    int			sliderSize;

    sliderSize= ed->edVisibleRect.drY1- ed->edVisibleRect.drY0+ 1;
    noy += ( sliderSize+ WHEEL_STEP- 1 )/ WHEEL_STEP;

    if  ( noy > add->addBackRect.drY1- sliderSize )
	{ noy=  add->addBackRect.drY1- sliderSize;	}

    scrolledY= noy- ooy;

    if  ( scrolledY == 0 )
	{ return;	}

    ed->edVisibleRect.drY0 += scrolledY;
    ed->edVisibleRect.drY1 += scrolledY;

    appDocScrollVertically( ed, scrolledY );

    appGuiSetScrollbarValues( ed->edVerticalScrollbar, noy, sliderSize );
    }

void appMouseWheelUp(		EditDocument *	ed )
    {
    int			ooy= ed->edVisibleRect.drY0;
    int			noy= ed->edVisibleRect.drY0;
    int			scrolledY;

    int			sliderSize;

    sliderSize= ed->edVisibleRect.drY1- ed->edVisibleRect.drY0+ 1;
    noy -= ( sliderSize+ WHEEL_STEP- 1 )/ WHEEL_STEP;

    if  ( noy < 0 )
	{ noy=  0;	}

    scrolledY= noy- ooy;

    if  ( scrolledY == 0 )
	{ return;	}

    ed->edVisibleRect.drY0 += scrolledY;
    ed->edVisibleRect.drY1 += scrolledY;

    appDocScrollVertically( ed, scrolledY );

    appGuiSetScrollbarValues( ed->edVerticalScrollbar, noy, sliderSize );
    }

void appDocSetScrollbarValues(	EditDocument *	ed )
    {
    AppDrawingData *	add= &(ed->edDrawingData);

    int			sliderSize;
    int			maximum;
    int			minimum;
    int			value;

    sliderSize= ed->edVisibleRect.drY1- ed->edVisibleRect.drY0+ 1;
    minimum= add->addBackRect.drY0;
    value= ed->edVisibleRect.drY0;

    if  ( sliderSize >= add->addBackRect.drY1+ 1 )
	{ maximum= ed->edVisibleRect.drY1+ 1;	}
    else{ maximum= add->addBackRect.drY1+ 1;	}

    if  ( value+ sliderSize > maximum )
	{ value= maximum- sliderSize; }

#   ifdef USE_MOTIF
    XtVaSetValues( ed->edVerticalScrollbar,
		XmNminimum,		minimum,
		XmNmaximum,		maximum,
		XmNvalue,		value,
		XmNsliderSize,		sliderSize,
		XmNpageIncrement,	( 9* sliderSize+ 9 )/10,
		XmNincrement,		( sliderSize+ BAR_STEP- 1 )/ BAR_STEP,
		NULL );
#   endif

#   ifdef USE_GTK
    ed->edVerticalAdjustment->lower= minimum;
    ed->edVerticalAdjustment->upper= maximum;
    ed->edVerticalAdjustment->value= value;
    ed->edVerticalAdjustment->page_size= sliderSize;
    ed->edVerticalAdjustment->page_increment= ( 9* sliderSize+ 9 )/10;
    ed->edVerticalAdjustment->step_increment=
				    ( sliderSize+ BAR_STEP- 1 )/ BAR_STEP;

    gtk_adjustment_changed( ed->edVerticalAdjustment );
#   endif

    sliderSize= ed->edVisibleRect.drX1- ed->edVisibleRect.drX0+ 1;
    minimum= add->addBackRect.drX0;
    value= ed->edVisibleRect.drX0;

    if  ( sliderSize >= add->addBackRect.drX1+ 1 )
	{ maximum= ed->edVisibleRect.drX1+ 1;	}
    else{ maximum= add->addBackRect.drX1+ 1;	}

#   ifdef USE_MOTIF
    XtVaSetValues( ed->edHorizontalScrollbar,
			XmNminimum,	minimum,
			XmNmaximum,	maximum,
			XmNvalue,	value,
			XmNsliderSize,	sliderSize,
			NULL );
#   endif

#   ifdef USE_GTK
    ed->edHorizontalAdjustment->lower= minimum;
    ed->edHorizontalAdjustment->upper= maximum;
    ed->edHorizontalAdjustment->value= value;
    ed->edHorizontalAdjustment->page_size= sliderSize;

    gtk_adjustment_changed( ed->edHorizontalAdjustment );
#   endif

    return;
    }

