/************************************************************************/
/*									*/
/*  Rulers, Ted specific functionality.					*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appFrame.h>
#   include	<appMetricRuler.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Allow a user to change the selected rectangle.			*/
/*									*/
/*  Move the vertical hair on the Document.				*/
/*									*/
/*  NOTE: Exposures and Drawing go to the document widget, NOT the	*/
/*  ruler.								*/
/*									*/
/************************************************************************/

void appHorMetricRulerMoveHair(		void *		voided,
					int		from,
					int		to,
					int		asUpper	)
    {
    EditDocument *		ed= (EditDocument *)voided;
    AppDrawingData *		add= &(ed->edDrawingData);
    int				ox= ed->edVisibleRect.drX0;

    int				high;

    high= ed->edVisibleRect.drY1- ed->edVisibleRect.drY0;

    if  ( from >= 0 )
	{
	if  ( asUpper )
	    { appExposeRectangle( add, from- ox, 0, 2, high );		}
	else{ appExposeRectangle( add, from- ox- 2, 0, 2, high );	}
	}

    if  ( to >= 0 )
	{
	appDrawSetForegroundBlack( add );

	if  ( asUpper )
	    {
	    appDrawFillRectangle( add, to- ox, 0, 1, high );
	    appDrawSetForegroundWhite( add );
	    appDrawFillRectangle( add, to- ox+ 1, 0, 1, high );
	    }
	else{
	    appDrawFillRectangle( add, to- ox- 1, 0, 1, high );
	    appDrawSetForegroundWhite( add );
	    appDrawFillRectangle( add, to- ox- 2, 0, 1, high );
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Allow a user to change the selected rectangle.			*/
/*									*/
/*  Move the horizontal hair on the Document.				*/
/*									*/
/*  NOTE: Exposures and Drawing go to the document widget, NOT the	*/
/*  ruler.								*/
/*									*/
/************************************************************************/

void appVertMetricRulerMoveHair(	void *		voided,
					int		from,
					int		to,
					int		asUpper	)
    {
    EditDocument *		ed= (EditDocument *)voided;
    AppDrawingData *		add= &(ed->edDrawingData);
    int				oy= ed->edVisibleRect.drY0;

    int				wide;

    wide= ed->edVisibleRect.drX1- ed->edVisibleRect.drX0;

    if  ( from >= 0 )
	{
	if  ( asUpper )
	    { appExposeRectangle( add, 0, from- oy, wide, 2 );		}
	else{ appExposeRectangle( add, 0, from- oy- 2, wide, 2 );	}
	}

    if  ( to >= 0 )
	{
	appDrawSetForegroundBlack( add );

	if  ( asUpper )
	    {
	    appDrawFillRectangle( add, 0, to- oy, wide, 1 );
	    appDrawSetForegroundWhite( add );
	    appDrawFillRectangle( add, 0, to- oy+ 1, wide, 1 );
	    }
	else{
	    appDrawFillRectangle( add, 0, to- oy- 1, wide, 1 );
	    appDrawSetForegroundWhite( add );
	    appDrawFillRectangle( add, 0, to- oy- 2, wide, 1 );
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a selection rectangle over the document. Strictly spoken this	*/
/*  has nothing to do with metric rulers as drawing is done on the	*/
/*  document widget.							*/
/*									*/
/************************************************************************/

void appDrawRectangleSelection(	DocumentRectangle *	drClip,
				DocumentRectangle *	drSelected,
				AppDrawingData *	add,
				AppColors *		ac,
				int			ox,
				int			oy )
    {
    if  ( drSelected->drX0 < 0	&&
	  drSelected->drX1 < 0	&&
	  drSelected->drY0 < 0	&&
	  drSelected->drY1 < 0	)
	{ return;	}

    appDrawSetForegroundBlack( add );

    if  ( drSelected->drX0 >= 0 )
	{
	appDrawFillRectangle( add,
			drSelected->drX0- ox- 1,
			add->addBackRect.drY0- oy,
			1,
			add->addBackRect.drY1- add->addBackRect.drY0 );
	}

    if  ( drSelected->drX1 >= 0 )
	{
	appDrawFillRectangle( add,
			drSelected->drX1- ox,
			add->addBackRect.drY0- oy,
			1,
			add->addBackRect.drY1- add->addBackRect.drY0 );
	}

    if  ( drSelected->drY0 >= 0 )
	{
	appDrawFillRectangle( add,
			add->addBackRect.drX0- ox,
			drSelected->drY0- oy- 1,
			add->addBackRect.drX1- add->addBackRect.drX0,
			1 );
	}

    if  ( drSelected->drY1 >= 0 )
	{
	appDrawFillRectangle( add,
			add->addBackRect.drX0- ox,
			drSelected->drY1- oy,
			add->addBackRect.drX1- add->addBackRect.drX0,
			1 );
	}

    appDrawSetForegroundWhite( add );

    if  ( drSelected->drX0 >= 0 )
	{
	appDrawFillRectangle( add,
			drSelected->drX0- ox- 2,
			add->addBackRect.drY0- oy,
			1,
			add->addBackRect.drY1- add->addBackRect.drY0 );
	}

    if  ( drSelected->drX1 >= 0 )
	{
	appDrawFillRectangle( add,
			drSelected->drX1- ox+ 1,
			add->addBackRect.drY0- oy,
			1,
			add->addBackRect.drY1- add->addBackRect.drY0 );
	}

    if  ( drSelected->drY0 >= 0 )
	{
	appDrawFillRectangle( add,
			add->addBackRect.drX0- ox,
			drSelected->drY0- oy- 2,
			add->addBackRect.drX1- add->addBackRect.drX0,
			1 );
	}

    if  ( drSelected->drY1 >= 0 )
	{
	appDrawFillRectangle( add,
			add->addBackRect.drX0- ox,
			drSelected->drY1+ 1- oy,
			add->addBackRect.drX1- add->addBackRect.drX0,
			1 );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Remove the rectangle selection and redraw the appropriate		*/
/*  rectangles.								*/
/*									*/
/************************************************************************/

void appRemoveRectangleSelection( DocumentRectangle *	drVisible,
				DocumentRectangle *	drSelected,
				AppDrawingData *	add )
    {
    int			ox= drVisible->drX0;
    int			oy= drVisible->drY0;

    int			wide= drVisible->drX1- drVisible->drX0;
    int			high= drVisible->drY1- drVisible->drY0;

    if  ( drSelected->drX0 >= 0 )
	{
	appExposeRectangle( add, drSelected->drX0- ox- 2, 0, 2, high );

	drSelected->drX0= -1;
	}

    if  ( drSelected->drX1 >= 0 )
	{
	appExposeRectangle( add, drSelected->drX1- ox, 0, 2, high );

	drSelected->drX1= -1;
	}

    if  ( drSelected->drY0 >= 0 )
	{
	appExposeRectangle( add, 0, drSelected->drY0- oy- 2, wide, 2 );

	drSelected->drY0= -1;
	}

    if  ( drSelected->drY1 >= 0 )
	{
	appExposeRectangle( add, 0, drSelected->drY1- oy, wide, 2 );

	drSelected->drY1= -1;
	}

    return;
    }

void appSetHorRectangleSelection( DocumentRectangle *	drVisible,
				DocumentRectangle *	drSelected,
				int			x0Screen,
				int			x1Screen,
				AppDrawingData *	add )
    {
    int			ox= drVisible->drX0;
    int			high= drVisible->drY1- drVisible->drY0;

    if  ( drSelected->drX0 != x0Screen )
	{
	if  ( drSelected->drX0 >= 0 )
	    {
	    appExposeRectangle( add, drSelected->drX0- ox- 2, 0, 2, high );
	    }

	if  ( x0Screen >= 0 )
	    {
	    appExposeRectangle( add, x0Screen- ox- 2, 0, 2, high );
	    }

	drSelected->drX0= x0Screen;
	}

    if  ( drSelected->drX1 != x1Screen )
	{
	if  ( drSelected->drX1 >= 0 )
	    {
	    appExposeRectangle( add, drSelected->drX1- ox, 0, 2, high );
	    }

	if  ( x1Screen >= 0 )
	    {
	    appExposeRectangle( add, x1Screen- ox, 0, 2, high );
	    }

	drSelected->drX1= x1Screen;
	}

    return;
    }


void appSetVertRectangleSelection( DocumentRectangle *	drVisible,
				DocumentRectangle *	drSelected,
				int			y0Screen,
				int			y1Screen,
				AppDrawingData *	add )
    {
    int			oy= drVisible->drY0;
    int			wide= drVisible->drX1- drVisible->drX0;

    if  ( drSelected->drY0 != y0Screen )
	{
	if  ( drSelected->drY0 >= 0 )
	    {
	    appExposeRectangle( add, 0, drSelected->drY0- oy- 2, wide, 2 );
	    }

	if  ( y0Screen >= 0 )
	    {
	    appExposeRectangle( add, 0, y0Screen- oy- 2, wide, 2 );
	    }

	drSelected->drY0= y0Screen;
	}

    if  ( drSelected->drY1 != y1Screen )
	{
	if  ( drSelected->drY1 >= 0 )
	    {
	    appExposeRectangle( add, 0, drSelected->drY1- oy, wide, 2 );
	    }

	if  ( y1Screen >= 0 )
	    {
	    appExposeRectangle( add, 0, y1Screen- oy, wide, 2 );
	    }

	drSelected->drY1= y1Screen;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make the ruler administration for the top ruler widget of a		*/
/*  document.								*/
/*									*/
/************************************************************************/

void appDocSetMetricTopRuler(	EditDocument *		ed,
				int			topRulerHeight,
				double			magnification,
				int			unitInt,
				APP_EVENT_HANDLER_T	mouseDown )
    {
    EditApplication *		ea= ed->edApplication;
    AppDrawingData *		add= &(ed->edDrawingData);

    ed->edTopRuler= appMakeMetricRuler(

		    topRulerHeight,		/*  sizeAcross		*/
		    add->addMagnifiedPixelsPerTwip,
		    magnification,		/*  magnification	*/

		    ed->edLeftRulerWidthPixels,	/*  minUnused		*/
		    ed->edRightRulerWidthPixels,/*  maxUnused		*/

		    add->addDocRect.drX0,	/*  documentC0		*/
		    add->addDocRect.drX1,	/*  documentC1		*/
		    ed->edVisibleRect.drX0,	/*  visisbleC0		*/
		    ed->edVisibleRect.drX1,	/*  visisbleC1		*/
		    add->addPaperRect.drX1,	/*  rulerC1		*/

		    ea->eaRulerFont,
		    unitInt );			/*  whatUnit		*/

    appDrawSetConfigureHandler( ed->edTopRulerWidget,
			appHorizontalRulerConfigure, (void *)ed->edTopRuler );

    appDrawSetRedrawHandler( ed->edTopRulerWidget,
		    appRedrawHorizontalMetricRuler, (void *)ed->edTopRuler );

    appDrawSetButtonPressHandler( ed->edTopRulerWidget,
						mouseDown, (void *)ed );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the ruler administration for the left ruler widget of a	*/
/*  document.								*/
/*									*/
/************************************************************************/

void appDocSetMetricLeftRuler(	EditDocument *		ed,
				int			leftRulerWidth,
				double			magnification,
				int			unitInt,
				APP_EVENT_HANDLER_T	mouseDown )
    {
    EditApplication *		ea= ed->edApplication;
    AppDrawingData *		add= &(ed->edDrawingData);

    ed->edLeftRuler= appMakeMetricRuler(
		    leftRulerWidth,		/*  sizeAcross		*/
		    add->addMagnifiedPixelsPerTwip,
		    magnification,		/*  magnification	*/

		    0, 0,			/*  {min,max}Unused	*/

		    add->addDocRect.drY0,	/*  documentC0		*/
		    add->addDocRect.drY1,	/*  documentC1		*/
		    ed->edVisibleRect.drY0,	/*  visisbleC0		*/
		    ed->edVisibleRect.drY1,	/*  visisbleC1		*/
		    add->addPaperRect.drY1,	/*  rulerC1		*/

		    ea->eaRulerFont,
		    unitInt );			/*  whatUnit		*/

    appDrawSetConfigureHandler( ed->edLeftRulerWidget,
			appVerticalRulerConfigure, (void *)ed->edLeftRuler );

    appDrawSetRedrawHandler( ed->edLeftRulerWidget,
			    appRedrawVerticalMetricRuler, ed->edLeftRuler );

    appDrawSetButtonPressHandler( ed->edLeftRulerWidget,
						    mouseDown, (void *)ed );

    return;
    }
