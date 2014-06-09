/************************************************************************/
/*									*/
/*  Motif specific document configuration.				*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	<locale.h>

#   include	"appFrame.h"

#   ifdef USE_MOTIF

#   include	<Xm/ToggleB.h>
#   include	<Xm/RowColumn.h>
#   include	<Xm/Protocols.h>
#   include	<Xm/Form.h>
#   include	<Xm/ScrolledW.h>
#   include	<Xm/DrawingA.h>
#   include	<Xm/PanedW.h>
#   include	<Xm/ScrollBar.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Event handler to keep track of resize events for the document.	*/
/*  Both resize events of the shel widget and the document widget are	*/
/*  handeled to keep track of the difference in size.			*/
/*									*/
/*  The difference in size is used to set the maximum size of the	*/
/*  shell widget in the window manager.					*/
/*									*/
/************************************************************************/

static void appDocTopConfigure(	APP_WIDGET		w,
				void *			voided,
				XEvent *		event,
				Boolean *		pRefused )
    {
    EditDocument *	ed= (EditDocument *)voided;

    if  ( event->type == MapNotify && ed->edShellExtraWidth < 0 )
	{
	appDocumentCalculateExtraSize( ed );
	ed->edMapped= 1;
	}

    if  ( event->type == ConfigureNotify )
	{
	XConfigureEvent *	cevent= (XConfigureEvent *)event;

	if  ( ! ed->edMapped && cevent->y < 0 )
	    {
	    XtVaSetValues( ed->edToplevel.atTopWidget,
				    XmNy,		40,
				    NULL );
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make a document window.						*/
/*									*/
/*  1)  Allocate a document.						*/
/*  3)  Make a shell to contain everything.				*/
/*  4)  Make the window pane.						*/
/*									*/
/************************************************************************/

static int appDocMakeMainWindow(	EditDocument *		ed,
					EditApplication *	ea )
    {
    Arg			al[20];
    int			ac;
    Pixmap		pixmap= (Pixmap)0;
    Pixmap		mask= (Pixmap)0;

    Display *		display= XtDisplay( ea->eaToplevel.atTopWidget );

    const char *	fullTitle;
    const char *	fullIconName;

    int			maxWidth= ( 4* ea->eaScreenPixelsWide )/ 5;
    int			maxHeight= ( 4* ea->eaScreenPixelsHigh )/ 5;

    if  ( ea->eaDocumentIcon						&&
	  appGetImagePixmap( ea, ea->eaDocumentIcon, &pixmap, &mask )	)
	{ SDEB(ea->eaDocumentIcon); return -1;	}

    if  ( appFormatDocumentTitle( &fullTitle, &fullIconName, ea, ed->edTitle ) )
	{ SDEB(ed->edTitle); return -1;	}

    ac= 0;
    XtSetArg( al[ac], XmNtitle,			fullTitle ); ac++;
    XtSetArg( al[ac], XmNiconName,		fullIconName ); ac++;
    XtSetArg( al[ac], XmNdeleteResponse,	XmDO_NOTHING ); ac++;
    XtSetArg( al[ac], XmNallowShellResize,	True ); ac++;
    XtSetArg( al[ac], XmNuseAsyncGeometry,	True ); ac++;
    XtSetArg( al[ac], XmNwaitForWm,		False ); ac++; /* LessTif BUG */
    XtSetArg( al[ac], XmNwmTimeout,		0 ); ac++; /* LessTif BUG */
    XtSetArg( al[ac], XmNmappedWhenManaged,	False ); ac++;
    XtSetArg( al[ac], XmNinput,			True ); ac++;
    XtSetArg( al[ac], XmNmaxWidth,		maxWidth ); ac++;
    XtSetArg( al[ac], XmNmaxHeight,		maxHeight ); ac++;

    if  ( pixmap )
	{ XtSetArg( al[ac], XmNiconPixmap,	pixmap ); ac++; }

#   ifdef USE_X11_R5
    ed->edToplevel.atTopWidget= XtAppCreateShell( ea->eaApplicationName, 
					    ea->eaDocumentWidgetName,
					    applicationShellWidgetClass,
					    display, al, ac );
#   else
    ed->edToplevel.atTopWidget= XtAppCreateShell( ea->eaApplicationName, 
					    ea->eaDocumentWidgetName,
					    topLevelShellWidgetClass,
					    display, al, ac );
#   endif

    appSetCloseCallback( ed->edToplevel.atTopWidget, ea,
					appDocFileCloseCallback, (void *)ed );

    ac= 0;
    XtSetArg( al[ac],	XmNsashWidth,		1 ); ac++;
    XtSetArg( al[ac],	XmNsashHeight,		1 ); ac++;
    XtSetArg( al[ac],	XmNseparatorOn,		False ); ac++;
    XtSetArg( al[ac],	XmNmarginWidth,		0 ); ac++;
    XtSetArg( al[ac],	XmNmarginHeight,	0 ); ac++;
    XtSetArg( al[ac],	XmNspacing,		0 ); ac++;

    ed->edMainWindow= XmCreatePanedWindow(
			ed->edToplevel.atTopWidget, WIDGET_NAME, al, ac );

    appGuiInsertMenubarInColumn( &(ed->edMenuBar), ed->edMainWindow );

    appDocFillMenu( ed );

    return 0;
    }

static int appDocMakeScrolledWindow(	EditDocument *		ed,
					EditApplication *	ea )
    {
    Arg			al[20];
    int			ac= 0;
    
    ac= 0;
    XtSetArg( al[ac], XmNscrollingPolicy,	XmAPPLICATION_DEFINED ); ac++;
    XtSetArg( al[ac], XmNscrollBarDisplayPolicy, XmSTATIC ); ac++;
    XtSetArg( al[ac], XmNhighlightThickness,	0 ); ac++;
    XtSetArg( al[ac], XmNspacing,		0 ); ac++;

    ed->edScrolledWindow=
	    XmCreateScrolledWindow( ed->edMainWindow, WIDGET_NAME, al, ac );

    ac= 0;
    XtSetArg( al[ac], XmNorientation,		XmVERTICAL ); ac++;
    XtSetArg( al[ac], XmNprocessingDirection,	XmMAX_ON_BOTTOM ); ac++;
    XtSetArg( al[ac], XmNtraversalOn,		False ); ac++;
    XtSetArg( al[ac], XmNnavigationType,	XmNONE ); ac++;
    ed->edVerticalScrollbar=
		XmCreateScrollBar( ed->edScrolledWindow, WIDGET_NAME, al, ac );

    XtAddCallback( ed->edVerticalScrollbar, XmNvalueChangedCallback,
			    ea->eaDocVerticalScrollbarCallback, (void *)ed );
    XtAddCallback( ed->edVerticalScrollbar, XmNdragCallback,
			    ea->eaDocVerticalScrollbarCallback, (void *)ed );

    ac= 0;
    XtSetArg( al[ac], XmNorientation,		XmHORIZONTAL ); ac++;
    XtSetArg( al[ac], XmNprocessingDirection,	XmMAX_ON_RIGHT ); ac++;
    XtSetArg( al[ac], XmNtraversalOn,		False ); ac++;
    XtSetArg( al[ac], XmNnavigationType,	XmNONE ); ac++;
    ed->edHorizontalScrollbar=
		XmCreateScrollBar( ed->edScrolledWindow, WIDGET_NAME, al, ac );

    XtAddCallback( ed->edHorizontalScrollbar, XmNvalueChangedCallback,
			    appDocHorizontalScrollbarCallback, (void *)ed );
    XtAddCallback( ed->edHorizontalScrollbar, XmNdragCallback,
			    appDocHorizontalScrollbarCallback, (void *)ed );

    appMotifTurnOfSashTraversal( ed->edMainWindow );

    return 0;
    }

int appMakeDocumentWidget(	EditApplication *	ea,
				EditDocument *		ed )
    {
    Arg			al[20];
    int			ac= 0;

    APP_WIDGET		form;

    Display *		display= XtDisplay( ea->eaToplevel.atTopWidget );
    int			screen= DefaultScreen( display );
    Pixel		blackPixel= BlackPixel( display, screen );

    appDocumentRulerWidth( ea, ed );

    /*  2  */
    form= XmCreateForm( ed->edScrolledWindow, WIDGET_NAME, al, ac );

    /*  3  */
    ac= 0;
    XtSetArg( al[ac],	XmNforeground,		blackPixel ); ac++;
    XtSetArg( al[ac],	XmNtraversalOn,		False ); ac++;
    XtSetArg( al[ac],	XmNleftAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac],	XmNleftOffset,		0 ); ac++;
    XtSetArg( al[ac],	XmNtopAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac],	XmNtopOffset,		0 ); ac++;
    XtSetArg( al[ac],	XmNrightAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac],	XmNrightOffset,		0 ); ac++;
    XtSetArg( al[ac],	XmNbottomAttachment,	XmATTACH_OPPOSITE_FORM ); ac++;
    XtSetArg( al[ac],	XmNbottomOffset,	-ed->edTopRulerHeightPixels ); ac++;

    ed->edTopRulerWidget= XmCreateDrawingArea( form, WIDGET_NAME, al, ac );

    /*  4  */
    ac= 0;
    XtSetArg( al[ac],	XmNforeground,		blackPixel ); ac++;
    XtSetArg( al[ac],	XmNtraversalOn,		False ); ac++;
    XtSetArg( al[ac],	XmNleftAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac],	XmNleftOffset,		0 ); ac++;
    XtSetArg( al[ac],	XmNtopAttachment,	XmATTACH_OPPOSITE_FORM ); ac++;
    XtSetArg( al[ac],	XmNtopOffset,		-ed->edBottomRulerHeightPixels ); ac++;
    XtSetArg( al[ac],	XmNrightAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac],	XmNrightOffset,		0 ); ac++;
    XtSetArg( al[ac],	XmNbottomAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac],	XmNbottomOffset,	0 ); ac++;

    ed->edBottomRulerWidget= XmCreateDrawingArea( form, WIDGET_NAME, al, ac );

    /*  5  */
    ac= 0;
    XtSetArg( al[ac],	XmNforeground,		blackPixel ); ac++;
    XtSetArg( al[ac],	XmNtraversalOn,		False ); ac++;

    XtSetArg( al[ac],	XmNleftAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac],	XmNleftOffset,		0 ); ac++;

    XtSetArg( al[ac],	XmNtopAttachment,	XmATTACH_WIDGET ); ac++;
    XtSetArg( al[ac],	XmNtopWidget,		ed->edTopRulerWidget ); ac++;
    XtSetArg( al[ac],	XmNtopOffset,		0 ); ac++;

    XtSetArg( al[ac],	XmNrightAttachment,	XmATTACH_OPPOSITE_FORM ); ac++;
    XtSetArg( al[ac],	XmNrightOffset,		-ed->edLeftRulerWidthPixels ); ac++;

    XtSetArg( al[ac],	XmNbottomAttachment,	XmATTACH_WIDGET ); ac++;
    XtSetArg( al[ac],	XmNbottomWidget,	ed->edBottomRulerWidget ); ac++;
    XtSetArg( al[ac],	XmNbottomOffset,	0 ); ac++;

    ed->edLeftRulerWidget= XmCreateDrawingArea( form, WIDGET_NAME, al, ac );

    /*  6  */
    ac= 0;
    XtSetArg( al[ac],	XmNforeground,		blackPixel ); ac++;
    XtSetArg( al[ac],	XmNtraversalOn,		False ); ac++;
    XtSetArg( al[ac],	XmNleftAttachment,	XmATTACH_OPPOSITE_FORM ); ac++;
    XtSetArg( al[ac],	XmNleftOffset,		-ed->edRightRulerWidthPixels ); ac++;
    XtSetArg( al[ac],	XmNtopAttachment,	XmATTACH_WIDGET ); ac++;
    XtSetArg( al[ac],	XmNtopWidget,		ed->edTopRulerWidget ); ac++;
    XtSetArg( al[ac],	XmNtopOffset,		0 ); ac++;
    XtSetArg( al[ac],	XmNrightAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac],	XmNrightOffset,		0 ); ac++;
    XtSetArg( al[ac],	XmNbottomAttachment,	XmATTACH_WIDGET ); ac++;
    XtSetArg( al[ac],	XmNbottomWidget,	ed->edBottomRulerWidget ); ac++;
    XtSetArg( al[ac],	XmNbottomOffset,	0 ); ac++;

    ed->edRightRulerWidget= XmCreateDrawingArea( form, WIDGET_NAME, al, ac );

    /*  7  */
    ac= 0;
    XtSetArg( al[ac],	XmNtraversalOn,		True ); ac++;

    /* NO: Postpone
    XtSetArg( al[ac],	XmNbackground,		whitePixel ); ac++;
    */
    XtSetArg( al[ac],	XmNleftAttachment,	XmATTACH_WIDGET ); ac++;
    XtSetArg( al[ac],	XmNleftWidget,		ed->edLeftRulerWidget ); ac++;
    XtSetArg( al[ac],	XmNleftOffset,		0 ); ac++;
    XtSetArg( al[ac],	XmNtopAttachment,	XmATTACH_WIDGET ); ac++;
    XtSetArg( al[ac],	XmNtopWidget,		ed->edTopRulerWidget ); ac++;
    XtSetArg( al[ac],	XmNtopOffset,		0 ); ac++;
    XtSetArg( al[ac],	XmNrightAttachment,	XmATTACH_WIDGET ); ac++;
    XtSetArg( al[ac],	XmNrightWidget,		ed->edRightRulerWidget ); ac++;
    XtSetArg( al[ac],	XmNrightOffset,		0 ); ac++;
    XtSetArg( al[ac],	XmNbottomAttachment,	XmATTACH_WIDGET ); ac++;
    XtSetArg( al[ac],	XmNbottomWidget,	ed->edBottomRulerWidget ); ac++;
    XtSetArg( al[ac],	XmNbottomOffset,	0 ); ac++;

    ed->edDocumentWidget= XmCreateDrawingArea( form, WIDGET_NAME, al, ac );

    XtAddCallback( ed->edDocumentWidget,
		XmNdestroyCallback, appDestroyEditDocument, (void *)ed );

    XtManageChild( ed->edTopRulerWidget );
    XtManageChild( ed->edLeftRulerWidget );
    XtManageChild( ed->edRightRulerWidget );
    XtManageChild( ed->edBottomRulerWidget );
    XtManageChild( ed->edDocumentWidget );
    XtManageChild( form );

    return 0;
    }

static int appDocMakeToolbar(	EditDocument *		ed,
				EditApplication *	ea )
    {
#   if 0
    Arg			al[20];
    int			ac= 0;

    Display *		display= ea->eaDisplay;
    int			screen= DefaultScreen( ea->eaDisplay );

    int			verPixPerCM;

    verPixPerCM= ( 10* DisplayHeight( display, screen ) )/
					DisplayHeightMM( display, screen );
    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNheight,		verPixPerCM ); ac++;

    ed->edToolbar= XmCreateForm( ed->edMainWindow, WIDGET_NAME, al, ac );

#   endif
    return 0;
    }

int appFinishDocumentWindow(	EditDocument *		ed )
    {
    EditApplication *	ea= ed->edApplication;

    /*  3  */
    if  ( appDocMakeMainWindow( ed, ea )		||
	  appDocMakeToolbar( ed, ea )			||
	  appDocMakeScrolledWindow( ed, ea )		||
	  (*ea->eaMakeDocumentWidget)( ea, ed )		)
	{ LDEB(1); return -1; }

    XtAddEventHandler( ed->edDocumentWidget, StructureNotifyMask, False,
					appDocConfigure, (void *)ed );
    XtAddEventHandler( ed->edToplevel.atTopWidget, StructureNotifyMask, False,
					appDocTopConfigure, (void *)ed );

    /*  4  */
    XtAddEventHandler( ed->edDocumentWidget,
		    PropertyChangeMask, True, appDocCopyPasteHandler, ed );

    XtManageChild( ed->edVerticalScrollbar );
    XtManageChild( ed->edHorizontalScrollbar );

    XtManageChild( ed->edDocumentWidget );
    XtManageChild( ed->edScrolledWindow );

    if  ( ed->edToolbar )
	{ XtManageChild( ed->edToolbar ); }

    XtManageChild( ed->edMainWindow );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Prevent the Shell that contains a document from being resized	*/
/*  beyond normal limits.						*/
/*									*/
/************************************************************************/

void appSetShellConstraints(	EditDocument *		ed )
    {
    int			wide;
    int			high;

    Arg			al[20];
    int			ac= 0;

    AppDrawingData *	add= &(ed->edDrawingData);

    appDrawGetSizeOfWidget( &wide, &high, ed->edToplevel.atTopWidget );

    if  ( ed->edShellExtraWidth >= 0 )
	{
	XtWidgetGeometry	preferred;

	int			maxWidth;

	maxWidth= add->addBackRect.drX1- add->addBackRect.drX0;
	maxWidth += ed->edShellExtraWidth;

	XtQueryGeometry( ed->edMenuBar, (XtWidgetGeometry *)0, &preferred );
	if  ( maxWidth < preferred.width+ preferred.border_width )
	    { maxWidth=  preferred.width+ preferred.border_width;	}

	XtSetArg( al[ac],	XmNmaxWidth,	maxWidth ); ac++;

	if  ( wide > maxWidth )
	    {
	    XtSetArg( al[ac],	XmNwidth,	maxWidth ); ac++;

	    ed->edVisibleRect.drX1 += maxWidth- wide;
	    }
	}

    if  ( ed->edShellExtraHeight >= 0 )
	{
	int	maxHeight;

	maxHeight= add->addBackRect.drY1- add->addBackRect.drY0;

	if  ( maxHeight < add->addPaperRect.drY1- add->addPaperRect.drY0 )
	    { maxHeight=  add->addPaperRect.drY1- add->addPaperRect.drY0; }

	maxHeight += ed->edShellExtraHeight;

	XtSetArg( al[ac],	XmNmaxHeight,	maxHeight ); ac++;

	if  ( high > maxHeight )
	    {
	    XtSetArg( al[ac],	XmNheight,	maxHeight ); ac++;

	    ed->edVisibleRect.drY1 += maxHeight- high;
	    }
	}

    if  ( ac > 0 )
	{ XtSetValues( ed->edToplevel.atTopWidget, al, ac );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Finish the setup of the document after it is realised.		*/
/*									*/
/************************************************************************/

static XIMStyle	appXimPreferences[]=
    {
    XIMPreeditCallbacks,
    XIMPreeditPosition,
    XIMPreeditArea,
    XIMPreeditNothing,
    XIMStatusCallbacks,
    XIMStatusArea,
    XIMStatusNothing,
    };

int appFinishDocumentSetup(	EditApplication *	ea,
				EditDocument *		ed )
    {
    AppDrawingData *	add= &(ed->edDrawingData);

    appSetDrawingDataForWidget( ed->edDocumentWidget,
					    ea->eaMagnification, add );

    appDrawSetRedrawHandler( ed->edDocumentWidget,
					appDocExposeHandler, (void *)ed );

    if  ( ea->eaObserveFocus )
	{
	appGuiSetFocusChangeHandler( ed->edDocumentWidget,
					    ea->eaObserveFocus, (void *)ed );
	}

    if  ( ea->eaDocumentMouseHandler )
	{
	appDrawSetButtonPressHandler( ed->edDocumentWidget,
				ea->eaDocumentMouseHandler, (void *)ed );
	}

    if  ( ea->eaDocumentKeyboardHandler )
	{
	appDrawSetKeyboardHandler( ed->edDocumentWidget,
			    ea->eaDocumentKeyboardHandler, (void *)ed );
	}

    if  ( ea->eaDocumentCursor )
	{
	XDefineCursor( XtDisplay( ed->edDocumentWidget ),
		    XtWindow( ed->edDocumentWidget ), ea->eaDocumentCursor );
	}

    if  ( ea->eaInputMethod )
	{
	XIMStyles *	availableStyles;
	XIMStyle	supportedStyles;
	XIMStyle	likedStyle;
	int		style;

	XGetIMValues( ea->eaInputMethod,
		XNQueryInputStyle, &availableStyles,
		NULL );

	supportedStyles=	XIMPreeditArea		|
				XIMPreeditNothing	|
				XIMPreeditNone		|
				XIMStatusArea		|
				XIMStatusNothing	|
				XIMStatusNone		;

	likedStyle= 0;
	for ( style= 0; style < availableStyles->count_styles; style++ )
	    {
	    XIMStyle	foundStyle= availableStyles->supported_styles[style];
	    unsigned	j;

	    if  ( ( supportedStyles & foundStyle ) != foundStyle )
		{ continue;	}

	    if  ( ! likedStyle )
		{ likedStyle= foundStyle; continue;	}

	    for ( j= 0; j < sizeof(appXimPreferences)/sizeof(XIMStyle); j++ )
		{
		if  (   ( foundStyle & appXimPreferences[j] )	&&
		      ! ( likedStyle & appXimPreferences[j] )	)
		    { likedStyle= foundStyle; break;	}
		}
	    }

	if  ( likedStyle )
	    {
	    ed->edInputContext= XCreateIC( ea->eaInputMethod,
		XNClientWindow,		XtWindow( ed->edDocumentWidget ),
		XNInputStyle,		likedStyle,
		/*
		XNPeeditAttributes,	attributeList,
		XNStatusAttributes,	attributeList,
		*/
		NULL );
	    }
	else{ ed->edInputContext= (XIC)0;	}
	}
    else{ ed->edInputContext= (XIC)0;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Management of the 'Windows' menu options.				*/
/*									*/
/************************************************************************/

void appSetWindowsItemState(	APP_WIDGET	menu,
				EditDocument *	ed,
				int		changed )
    {
    WidgetList		children;
    Cardinal		childCount;

    int			j;

    XtVaGetValues( menu,
			XmNchildren,		&children,
			XmNnumChildren,		&childCount,
			NULL );

    for ( j= childCount- 1; j >= 0; j-- )
	{
	void *	voided= (void *)0;

	XtVaGetValues( children[j],
			XmNuserData,	&voided,
			NULL );

	if  ( voided == (void *)ed )
	    { appGuiSetToggleItemState( children[j], changed ); }
	}

    return;
    }

void appRemoveWindowsOption(	APP_WIDGET		menu,
				EditDocument *		oldEd )
    {
    WidgetList		children;
    Cardinal		childCount;

    int			i;

    XtVaGetValues( menu,
			XmNchildren,		&children,
			XmNnumChildren,		&childCount,
			NULL );

    for ( i= childCount- 1; i >= 0; i-- )
	{
	void *	voided= (void *)0;

	XtVaGetValues( children[i],
			XmNuserData,	&voided,
			NULL );

	if  ( voided == (void *)oldEd )
	    { XtDestroyWidget( children[i] );	}
	}

    return;
    }

void appRenameWindowsOption(		APP_WIDGET		menu,
					EditDocument *		ed,
					const char *		title )
    {
    WidgetList		children;
    Cardinal		childCount;

    int			i;

    XtVaGetValues( menu,
			XmNchildren,		&children,
			XmNnumChildren,		&childCount,
			NULL );

    for ( i= childCount- 1; i >= 0; i-- )
	{
	void *	voided= (void *)0;

	XtVaGetValues( children[i],
			XmNuserData,	&voided,
			NULL );

	if  ( voided == (void *)ed )
	    { appGuiSetToggleItemLabel( children[i], title );	}
	}
    }

#   endif
