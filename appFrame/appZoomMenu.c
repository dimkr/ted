/************************************************************************/
/*									*/
/*  Manage a zoom menu.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<appFrame.h>
#   include	<appZoomMenu.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Normalize a zoom factor.						*/
/*									*/
/************************************************************************/

static double appZoomFactor(	int	logs2mag )
    {
    if  ( logs2mag == 0 )
	{ return 1.0;	}

    if  ( logs2mag > 0 )
	{ return	pow( sqrt( 2.0 ),  (double)logs2mag );	}
    else{ return 1.0/	pow( sqrt( 2.0 ), -(double)logs2mag );	}
    }

/************************************************************************/
/*									*/
/*  Adapt the zoom menu to the current factor.				*/
/*									*/
/************************************************************************/

void appUpdateZoomMenu(	APP_WIDGET	menu,
			APP_WIDGET *	options,
			int		optionCount,
			int		logs2mag )
    {
    int				i;

    for ( i= 0; i < optionCount; i++ )
	{
	char		scratch[40];

	sprintf( scratch, "%6.0f %%",
		    100.0 * appZoomFactor( logs2mag+ i - optionCount/ 2 ) );

	appGuiSetToggleItemLabel( options[i], scratch );

	appGuiSetToggleItemState( options[i], i == optionCount/ 2 );
	}
    }

/************************************************************************/
/*									*/
/*  The user has chosen another magnification:				*/
/*									*/
/************************************************************************/

void appZoomChangeFactor(	APP_WIDGET		option,
				APP_WIDGET		menu,
				APP_WIDGET *		options,
				int			optionCount,
				int *			pLogSqrt2Magnification,
				double *		pMagnification,
				DocumentRectangle *	drSelected )

    {
    short			num= -1;
    double			magStep;

    for ( num= 0; num < optionCount; num++ )
	{
	if  ( options[num] == option )
	    { break;	}
	}

    if  ( num < 0 || num >= optionCount )
	{ LDEB(num); return;	}

    if  ( num == optionCount/2 )
	{ return;	}

    *pLogSqrt2Magnification += num- optionCount/2;
    magStep= appZoomFactor( *pLogSqrt2Magnification )/ *pMagnification;
    *pMagnification= appZoomFactor( *pLogSqrt2Magnification );

    appUpdateZoomMenu( menu, options, optionCount, *pLogSqrt2Magnification );

    if  ( drSelected )
	{
	if  ( drSelected->drX0 >= 0 )
	    { drSelected->drX0 *= magStep;	}
	if  ( drSelected->drX1 >= 0 )
	    { drSelected->drX1 *= magStep;	}
	if  ( drSelected->drY0 >= 0 )
	    { drSelected->drY0 *= magStep;	}
	if  ( drSelected->drY1 >= 0 )
	    { drSelected->drY1 *= magStep;	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make a zoom menu.							*/
/*									*/
/************************************************************************/

void appMakeZoomMenu(		EditApplication *	ea,
				EditDocument *		ed,
				APP_WIDGET *		pZoomMenu,
				APP_WIDGET		menubar,
				int			logSqrt2Magnification,
				const char *		zoomText,
				AppMenuItem *		optionItems,
				APP_WIDGET *		options,
				int			optionCount )
    {
    int			i;
    APP_WIDGET		menu;

    APP_WIDGET		button;

    for ( i= 1; i < optionCount; i++ )
	{
	optionItems[i]= optionItems[0];
	optionItems[i].amiItemType= ITEMtyTOGGLE_OFF;
	}
    optionItems[optionCount/2].amiItemType= ITEMtyTOGGLE_ON;

    menu= appMakeMenu( &button, &(ed->edToplevel), ea, menubar,
			zoomText, 0, optionItems, optionCount, (void *)ed );

    for ( i= 0; i < optionCount; i++ )
	{ options[i]= optionItems[i].amiOptionWidget;	}
    *pZoomMenu= menu;

    appUpdateZoomMenu( menu, options, optionCount, logSqrt2Magnification );

    return;
    }
