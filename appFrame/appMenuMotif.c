/************************************************************************/
/*									*/
/*  Menu building MOTIF specific code.					*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	"appFrame.h"
#   include	<appDebugon.h>

#   ifdef USE_MOTIF

#   include	<Xm/PushB.h>
#   include	<Xm/Separator.h>
#   include	<Xm/ToggleB.h>
#   include	<Xm/RowColumn.h>
#   include	<Xm/CascadeB.h>

APP_WIDGET appSetMenuItem(	APP_WIDGET		menu,
				AppToplevel *		at,
				AppMenuItem *		ami,
				void *			target )
    {
    Widget		rval;

    XmString		labelString;
    XmString		acceleratorString= (XmString)0;

    Arg			al[20];
    int			ac= 0;

    labelString= XmStringCreateLocalized( ami->amiItemText );

    XtSetArg( al[ac], XmNlabelString, labelString ); ac++;

    if  ( ami->amiKey )
	{ XtSetArg( al[ac], XmNaccelerator, ami->amiKey ); ac++; }

    if  ( ami->amiKeyText )
	{
	acceleratorString= XmStringCreateLocalized( ami->amiKeyText );

	XtSetArg( al[ac], XmNacceleratorText, acceleratorString ); ac++;
	}

    XtSetArg( al[ac], XmNmarginHeight, 1 ); ac++;

    rval= XmCreatePushButton( menu, WIDGET_NAME, al, ac );

    XmStringFree( labelString );
    if  ( acceleratorString )
	{ XmStringFree( acceleratorString );	}
    
    if  ( ami->amiCallback )
	{ XtAddCallback( rval, XmNactivateCallback, ami->amiCallback, target );}

    XtManageChild( rval );

    return rval;
    }	    

APP_WIDGET appSetMenuSeparator(	APP_WIDGET		menu,
				AppToplevel *		at,
				AppMenuItem *		ami,
				void *			target )
    {
    Widget		rval;

    Arg			al[20];
    int			ac= 0;

    rval= XmCreateSeparator( menu, WIDGET_NAME, al, ac );

    XtManageChild( rval );

    return rval;
    }	    

APP_WIDGET appSetToggleMenuItem(	APP_WIDGET		menu,
					AppToplevel *		at,
					AppMenuItem *		ami,
					void *			target )
    {
    Widget		rval;

    XmString		labelString;
    XmString		acceleratorString= (XmString)0;

    Arg			al[20];
    int			ac= 0;

    labelString= XmStringCreateLocalized( ami->amiItemText );

    XtSetArg( al[ac], XmNlabelString,		labelString ); ac++;
    XtSetArg( al[ac], XmNvisibleWhenOff,	True ); ac++;
    XtSetArg( al[ac], XmNuserData,		target ); ac++;

    if  ( ami->amiItemType == ITEMtyTOGGLE_ON )
	{
#	if XmVersion >= 2000
	XtSetArg( al[ac], XmNset,		XmSET ); ac++;
#	else
	XtSetArg( al[ac], XmNset,		True ); ac++;
#	endif
	}

    if  ( ami->amiKey )
	{ XtSetArg( al[ac], XmNaccelerator, ami->amiKey ); ac++; }

    if  ( ami->amiKeyText )
	{
	acceleratorString=
		    XmStringCreateLocalized( ami->amiKeyText );

	XtSetArg( al[ac], XmNacceleratorText, acceleratorString ); ac++;
	}

    XtSetArg( al[ac], XmNmarginHeight, 1 ); ac++;

    rval= XmCreateToggleButton( menu, WIDGET_NAME, al, ac );

    XmStringFree( labelString );
    if  ( acceleratorString )
	{ XmStringFree( acceleratorString );	}

    XtAddCallback( rval, XmNvalueChangedCallback, ami->amiCallback, target );

    XtManageChild( rval );

    return rval;
    }	    

APP_WIDGET appMakeMenuInParent(	APP_WIDGET *		pButton,
				AppToplevel *		at,
				APP_WIDGET		menuBar,
				const char *		itemText,
				int			isHelp )
    {
    APP_WIDGET			menu;
    APP_WIDGET			button;

    XmString			labelString;

    Arg				al[20];
    int				ac= 0;

    labelString= XmStringCreateLocalized( (char *)itemText );

    ac= 0;
    menu= XmCreatePulldownMenu( menuBar, WIDGET_NAME, al, ac );
    if  ( ! menu )
	{ XDEB(menu); return menu;	}

    ac= 0;
    XtSetArg( al[ac], XmNlabelString,		labelString ); ac++;
    XtSetArg( al[ac], XmNsubMenuId,		menu ); ac++;

    button= XmCreateCascadeButton( menuBar, WIDGET_NAME, al, ac );
    XtManageChild( button );

    if  ( isHelp )
	{
	XtVaSetValues( menuBar,
			    XmNmenuHelpWidget,	button,
			    NULL );
	}

    XmStringFree( labelString );

    *pButton= button;

    return menu;
    }

void appGuiSetToggleItemState(	APP_WIDGET		toggle,
				int			set )
    {
    XmToggleButtonSetState( toggle, set != 0, False );
    }

void appGuiSetToggleItemLabel(	APP_WIDGET		toggle,
				const char *		label )
    {
    XmString	labelString;

    labelString= XmStringCreateLocalized( (char *)label );
    XtVaSetValues( toggle,
			XmNlabelString,	labelString,
			NULL );

    XmStringFree( labelString );
    }

int appGuiGetMenuToggleStateFromCallback( APP_WIDGET		toggle,
					void *			voidcbs )
    {
    XmToggleButtonCallbackStruct *	cbs;

    cbs= (XmToggleButtonCallbackStruct *)voidcbs;

    return cbs->set;
    }

/************************************************************************/
/*									*/
/*  1)  Retrieve the text from menu options.				*/
/*  2)  Free the result obtained in this way.				*/
/*									*/
/************************************************************************/

/*  1  */
char *	appGetTextFromMenuOption(	Widget		w )
    {
    XmString	labelString= (XmString)0;
    char *	s;

    XtVaGetValues( w,
		    XmNlabelString,		&labelString,
		    NULL );

    XmStringGetLtoR( labelString, XmSTRING_DEFAULT_CHARSET, &s );

    XmStringFree( labelString );

    return s;
    }

/*  2  */
void appFreeTextFromMenuOption(	char *		s )
    { XtFree( s );	}

#   endif

