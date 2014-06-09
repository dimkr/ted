/************************************************************************/
/*									*/
/*  Widget to select a height on the Format Tool.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>
#   include	<limits.h>

#   include	<appGeoString.h>
#   include	<appUnit.h>

#   include	"tedApp.h"
#   include	"tedFormatTool.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Refresh a height chooser.						*/
/*									*/
/************************************************************************/

void tedFormatRefreshHeightChooser(	HeightChooser *	hc,
					int		height )
    {
    char		scratch[50];

    if  ( height == 0 )
	{
	appEnableText( hc->hcText, 0 );

	hc->hcHow= HC_FREE;
	appStringToTextWidget( hc->hcText, "" );
	}
    else{
	appEnableText( hc->hcText, 1 );

	if  ( height > 0 )
	    {
	    hc->hcHow= HC_AT_LEAST;
	    appGeoLengthToString( scratch,  height, UNITtyPOINTS );
	    }
	else{
	    hc->hcHow= HC_EXACTLY;
	    appGeoLengthToString( scratch, -height, UNITtyPOINTS );
	    }

	appStringToTextWidget( hc->hcText, scratch );
	}

    appSetOptionmenu( &(hc->hcOptionmenu), hc->hcHow );
    }

int tedFormatToolGetHeight(	int *		pHeight,
				HeightChooser *	hc )
    {
    int			sign;

    int			height;
    int			changed;

    const int		minValue= 1;
    const int		adaptToMin= 0;
    const int		maxValue= INT_MAX;
    const int		adaptToMax= 0;

    switch( hc->hcHow )
	{
	case HC_FREE:
	    sign= height= 0;
	    break;
	case HC_AT_LEAST:
	    sign=  1;
	    break;
	case HC_EXACTLY:
	    sign= -1;
	    break;
	default:
	    LDEB(hc->hcHow); return -1;
	}

    height= *pHeight;
    height *= sign;

    if  ( sign != 0 )
	{
	if  ( appGetLengthFromTextWidget( hc->hcText,
				&height, &changed, UNITtyPOINTS,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	    { return -1; }

	height *= sign;
	}

    *pHeight= height;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Callback for a height chooser.					*/
/*									*/
/************************************************************************/

void tedFormatHeightChosen(	int			how,
				HeightChooser *		hc,
				int			defaultValue )
    {
    char		scratch[50];
    int			val;

    switch( how )
	{
	case HC_FREE:
	    appEnableText( hc->hcText, 0 );
	    appStringToTextWidget( hc->hcText, "" );
	    hc->hcHow= how;
	    return;

	case HC_AT_LEAST:
	    appEnableText( hc->hcText, 1 );
	    val= defaultValue;
	    if  ( val < 0 )
		{ val= -val;	}
	    appGeoLengthToString( scratch, val, UNITtyPOINTS );
	    appStringToTextWidget( hc->hcText, scratch );
	    hc->hcHow= how;
	    return;

	case HC_EXACTLY:
	    appEnableText( hc->hcText, 1 );
	    val= defaultValue;
	    appGeoLengthToString( scratch, val, UNITtyPOINTS );
	    appStringToTextWidget( hc->hcText, scratch );
	    hc->hcHow= how;
	    return;

	default:
	    LDEB(how); return;
	}

    LDEB(1);
    }

void tedFormatMakeHeightRow(	APP_WIDGET *			pRow,
				void *				through,
				APP_WIDGET			parent,
				HeightChooser *			hc,
				APP_TXACTIVATE_CALLBACK_T	callback )
    {
    APP_WIDGET	row;
    APP_WIDGET	text;

    const int	menuColumn= 0;
    const int	menuColspan= 1;
    const int	textColumn= menuColumn+ menuColspan;
    const int	textColspan= 1;
    const int	heightResizable= 0;

    row= appMakeRowInColumn( parent, 2, heightResizable );

    appMakeOptionmenuInRow( &(hc->hcOptionmenu), row, menuColumn, menuColspan );

    appMakeTextInRow( &text, row, textColumn, textColspan, 10, 0 );

    if  ( callback )
	{ appGuiSetGotValueCallbackForText( text, callback, through ); }

    *pRow= row;
    hc->hcText= text;

    return;
    }

/************************************************************************/
/*									*/
/*  Fill the menu in a height chooser.					*/
/*									*/
/************************************************************************/

void tedFormatFillHeightChooser(	HeightChooser *		hc,
					APP_OITEM_CALLBACK_T	callback,
					void *			voidtft,
					const char *		freeText,
					const char *		atLeastText,
					const char *		exactlyText )
    {
    appEmptyOptionmenu( &(hc->hcOptionmenu) );

    hc->hcMenuItems[HC_FREE]= appAddItemToOptionmenu( &(hc->hcOptionmenu),
				    freeText, callback, voidtft );

    hc->hcMenuItems[HC_AT_LEAST]= appAddItemToOptionmenu( &(hc->hcOptionmenu),
				    atLeastText, callback, voidtft );

    hc->hcMenuItems[HC_EXACTLY]= appAddItemToOptionmenu( &(hc->hcOptionmenu),
				    exactlyText, callback, voidtft );

    appSetOptionmenu( &(hc->hcOptionmenu), HC_FREE );
    hc->hcHow= HC_FREE;

    appOptionmenuRefreshWidth( &(hc->hcOptionmenu) );
    }

/************************************************************************/
/*									*/
/*  Initialize.								*/
/*									*/
/************************************************************************/

void tedInitHeightChooser(	HeightChooser *		hc )
    {
    int		i;

    appInitOptionmenu( &(hc->hcOptionmenu) );
    hc->hcText= (APP_WIDGET)0;

    for ( i= 0; i < HC__COUNT; i++ )
	{ hc->hcMenuItems[i]= (APP_WIDGET)0;	}

    hc->hcHow= HC_FREE;
    }

