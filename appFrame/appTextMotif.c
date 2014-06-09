#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"appFrame.h"
#   include	"appSystem.h"
#   include	<appGeoString.h>

#   ifdef USE_MOTIF

#   include	<X11/Xatom.h>
#   include	<Xm/Text.h>
#   include	<Xm/PanedW.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Windows style cut/copy/paste behaviour.				*/
/*									*/
/************************************************************************/

static char *	APP_PasteTranslationString=
    "Ctrl <Key>v: copy-primary()\n"	/*  PASTE	*/
    "Ctrl <Key>c: copy-clipboard()\n"	/*  COPY	*/
    "Ctrl <Key>x: cut-clipboard()\n"	/*  CUT		*/
    ;

static XtTranslations APP_PasteTranslations= (XtTranslations)0;

/************************************************************************/
/*									*/
/*  Make a text box widget in a row.					*/
/*									*/
/************************************************************************/

void appMakeTextInRow(		Widget *		pText,
				Widget			row,
				int			column,
				int			colspan,
				int			textColumns,
				int			textEnabled )
    {
    Display *		display= XtDisplay( row );
    int			screen= DefaultScreen( display );
    Pixel		whitePixel= WhitePixel( display, screen );
    Pixel		blackPixel= BlackPixel( display, screen );

    Widget		text;

    Arg			al[20];
    int			ac= 0;

    if  ( ! APP_PasteTranslations )
	{
	APP_PasteTranslations=
		    XtParseTranslationTable( APP_PasteTranslationString );
	}

    ac= 0;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNtopOffset,		0 ); ac++;

    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNleftPosition,		column ); ac++;

    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNrightPosition,		column+ colspan ); ac++;

    if  ( textColumns > 0 )
	{ XtSetArg( al[ac], XmNcolumns,		textColumns ); ac++;	}

    XtSetArg( al[ac], XmNmarginHeight,		TXmargH ); ac++;

    if  ( textEnabled )
	{
	XtSetArg( al[ac], XmNbackground,		whitePixel ); ac++;
	XtSetArg( al[ac], XmNforeground,		blackPixel ); ac++;
	}
    else{
	XtSetArg( al[ac], XmNeditable,			False ); ac++;
	XtSetArg( al[ac], XmNtraversalOn,		False ); ac++;
	XtSetArg( al[ac], XmNcursorPositionVisible,	False ); ac++;
	}

    text= XmCreateText( row, WIDGET_NAME, al, ac );

    if  ( APP_PasteTranslations )
	{ XtOverrideTranslations( text, APP_PasteTranslations ); }

    XtManageChild( text );

    *pText= text;
    }

/************************************************************************/
/*									*/
/*  Make a text widget that is contained in a column of widgets		*/
/*									*/
/************************************************************************/

void appMakeTextInColumn(	Widget *	pText,
				Widget		column,
				int		textColumns,
				int		textEnabled )
    {
    Display *		display= XtDisplay( column );
    int			screen= DefaultScreen( display );
    Pixel		whitePixel= WhitePixel( display, screen );
    Pixel		blackPixel= BlackPixel( display, screen );

    Widget		text;

    Arg			al[20];
    int			ac= 0;

    if  ( ! APP_PasteTranslations )
	{
	APP_PasteTranslations=
		    XtParseTranslationTable( APP_PasteTranslationString );
	}

    ac= 0;
    XtSetArg( al[ac], XmNnavigationType,	XmTAB_GROUP ); ac++;
    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;

    XtSetArg( al[ac], XmNmarginHeight,		TXmargH ); ac++;

    if  ( textColumns > 0 )
	{ XtSetArg( al[ac], XmNcolumns,		textColumns ); ac++;	}

    if  ( textEnabled )
	{
	XtSetArg( al[ac], XmNbackground,		whitePixel ); ac++;
	XtSetArg( al[ac], XmNforeground,		blackPixel ); ac++;
	}
    else{
	XtSetArg( al[ac], XmNeditable,			False ); ac++;
	XtSetArg( al[ac], XmNtraversalOn,		False ); ac++;
	XtSetArg( al[ac], XmNcursorPositionVisible,	False ); ac++;
	}

    text= XmCreateText( column, WIDGET_NAME, al, ac );

    if  ( APP_PasteTranslations )
	{ XtOverrideTranslations( text, APP_PasteTranslations ); }

    XtManageChild( text );

    appMotifTurnOfSashTraversal( column );

    *pText= text;
    }

void appRefuseTextValue(	Widget		w )
    {
    char *	s;

    s= appGetStringFromTextWidget( w );
    XmTextSetSelection( w, 0, strlen( s ),
			    XtLastTimestampProcessed( XtDisplay( w ) ) );
    appFreeStringFromTextWidget( s );
    XmProcessTraversal( w, XmTRAVERSE_CURRENT );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert an integer in a text widget.					*/
/*									*/
/************************************************************************/

void appStringToTextWidget(		Widget		w,
					const char *	s )
    {
    XmTextSetString( w, (char *)s );

    return;
    }

/************************************************************************/
/*									*/
/*  Turn a text widget on or off.					*/
/*									*/
/************************************************************************/

void appEnableText(		Widget		text,
				int		enabled )
    {
    if  ( enabled )
	{
	Display *	display= XtDisplay( text );
	int		screen= DefaultScreen( display );

	XtVaSetValues( text,
		    XmNbackground,		WhitePixel( display, screen ),
		    XmNforeground,		BlackPixel( display, screen ),
		    XmNeditable,		True,
		    XmNtraversalOn,		True,
		    XmNcursorPositionVisible,	True,
		    NULL );
	}
    else{
	Pixel		background;
	Pixel		foreground;

	XtVaGetValues( XtParent( text ),
			XmNbackground,			&background,
			XmNforeground,			&foreground,
			NULL );

	XtVaSetValues( text,
			    XmNbackground,		background,
			    XmNforeground,		foreground,
			    XmNeditable,		False,
			    XmNtraversalOn,		False,
			    XmNcursorPositionVisible,	False,
			    NULL );
	}
    }

/************************************************************************/
/*									*/
/*  1)  Retrieve strings from text widgets.				*/
/*  2)  Free the result obtained in this way.				*/
/*									*/
/************************************************************************/

/*  1  */
char *	appGetStringFromTextWidget(	Widget		w )
    { return XmTextGetString( w );	}

/*  2  */
void appFreeStringFromTextWidget(	char *		s )
    { XtFree( s );	}

void appGuiSetTypingCallbackForText(	Widget		text,
					XtCallbackProc	callBack,
					void *		through )
    {
    XtAddCallback( text, XmNvalueChangedCallback, callBack, through );

    return;
    }

void appGuiSetGotValueCallbackForText(	Widget		text,
					XtCallbackProc	callBack,
					void *		through )
    {
    XtAddCallback( text, XmNactivateCallback, callBack, through );

    return;
    }

#   endif
