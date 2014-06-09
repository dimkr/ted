/************************************************************************/
/*									*/
/*  A Page Layout tool.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<limits.h>

#   include	<utilPropMask.h>
#   include	<appFrame.h>
#   include	"appMarginTool.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Show the size of the paper in the Text.				*/
/*									*/
/************************************************************************/

void appMarginToolShowMargins(	AppMarginTool *			amt,
				int				unitInt,
				const DocumentGeometry *	dg )
    {
    char			scratch[50];

    appGeoLengthToString( scratch, dg->dgLeftMarginTwips, unitInt );
    appStringToTextWidget( amt->amtLeftMarginText, scratch );

    appGeoLengthToString( scratch, dg->dgTopMarginTwips, unitInt );
    appStringToTextWidget( amt->amtTopMarginText, scratch );

    appGeoLengthToString( scratch, dg->dgRightMarginTwips, unitInt );
    appStringToTextWidget( amt->amtRightMarginText, scratch );

    appGeoLengthToString( scratch, dg->dgBottomMarginTwips, unitInt );
    appStringToTextWidget( amt->amtBottomMarginText, scratch );

    return;
    }

/************************************************************************/
/*									*/
/*  Get one/all margins from the text widgets.				*/
/*									*/
/************************************************************************/

int appMarginToolGetMargins(	PropertyMask *		pUpdMask,
				const PropertyMask *	chgMask,
				int			unitType,
				AppMarginTool *		amt,
				DocumentGeometry *	dg )
    {
    int			changed;

    const int		minValue= 1;
    const int		adaptToMin= 0;
    const int		maxValue= INT_MAX;
    const int		adaptToMax= 0;

    PropertyMask	updMask;

    PROPmaskCLEAR( &updMask );

    if  ( PROPmaskISSET( chgMask, DGpropLEFT_MARGIN ) )
	{
	if  ( appGetLengthFromTextWidget( amt->amtLeftMarginText,
				&(dg->dgLeftMarginTwips), &changed, unitType,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	    { return -1;	}
	if  ( changed )
	    { PROPmaskADD( &updMask, DGpropLEFT_MARGIN );	}
	}

    if  ( PROPmaskISSET( chgMask, DGpropTOP_MARGIN ) )
	{
	if  ( appGetLengthFromTextWidget( amt->amtTopMarginText,
				&(dg->dgTopMarginTwips), &changed, unitType,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	    { return -1;	}
	if  ( changed )
	    { PROPmaskADD( &updMask, DGpropTOP_MARGIN );	}
	}

    if  ( PROPmaskISSET( chgMask, DGpropRIGHT_MARGIN ) )
	{
	if  ( appGetLengthFromTextWidget( amt->amtRightMarginText,
				&(dg->dgRightMarginTwips), &changed, unitType,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	    { return -1;	}
	if  ( changed )
	    { PROPmaskADD( &updMask, DGpropRIGHT_MARGIN );	}
	}

    if  ( PROPmaskISSET( chgMask, DGpropBOTTOM_MARGIN ) )
	{
	if  ( appGetLengthFromTextWidget( amt->amtBottomMarginText,
				&(dg->dgBottomMarginTwips), &changed, unitType,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	    { return -1;	}
	if  ( changed )
	    { PROPmaskADD( &updMask, DGpropBOTTOM_MARGIN );	}
	}

    *pUpdMask= updMask; return 0;
    }

/************************************************************************/
/*									*/
/*  Check margins.							*/
/*									*/
/************************************************************************/

int appMarginToolCheckMargins(	const DocumentGeometry * dg )
    {
    if  ( 1.2* ( dg->dgLeftMarginTwips+ dg->dgRightMarginTwips ) >= 
						    dg->dgPageWideTwips )
	{ return -1;	}

    if  ( 1.2* ( dg->dgTopMarginTwips+ dg->dgBottomMarginTwips ) >= 
						    dg->dgPageWideTwips )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make the frame for the margins.					*/
/*									*/
/************************************************************************/

void appMakeMarginToolWidgets(	APP_WIDGET			parent,
				const MarginToolResources *	mtr,
				AppMarginTool *			amt,
				APP_TXACTIVATE_CALLBACK_T	gotValue,
				void *				through )
    {
    APP_WIDGET	label;
    APP_WIDGET	row;

    const int	textWidth= 6;
    const int	textEnabled= 1;
    const int	colspan= 1;

    appMakeRowFrameInColumn( &(amt->amtMarginFrame), &(amt->amtInsideWidget),
					    parent, 2, mtr->mtrTitleText );

    appMakeColumnInRow( &(amt->amtLeftColumn), amt->amtInsideWidget,
								0, colspan );
    appMakeColumnInRow( &(amt->amtRightColumn), amt->amtInsideWidget,
								1, colspan );

    /**********************/

    appMakeLabelAndTextRow( &row, &label, &(amt->amtLeftMarginText),
				amt->amtLeftColumn, mtr->mtrLeftMarginText,
				textWidth, textEnabled );
    appMakeLabelAndTextRow( &row, &label, &(amt->amtRightMarginText),
				amt->amtLeftColumn, mtr->mtrRightMarginText,
				textWidth, textEnabled );

    appMakeLabelAndTextRow( &row, &label, &(amt->amtTopMarginText),
				amt->amtRightColumn, mtr->mtrTopMarginText,
				textWidth, textEnabled );
    appMakeLabelAndTextRow( &row, &label, &(amt->amtBottomMarginText),
				amt->amtRightColumn, mtr->mtrBottomMarginText,
				textWidth, textEnabled );

    if  ( gotValue )
	{
	appGuiSetGotValueCallbackForText( amt->amtLeftMarginText,
							gotValue, through );

	appGuiSetGotValueCallbackForText( amt->amtTopMarginText,
							gotValue, through );

	appGuiSetGotValueCallbackForText( amt->amtRightMarginText,
							gotValue, through );

	appGuiSetGotValueCallbackForText( amt->amtBottomMarginText,
							gotValue, through );
	}

    return;
    }
