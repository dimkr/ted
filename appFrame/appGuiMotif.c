#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"appFrame.h"
#   include	"appSystem.h"
#   include	<appGeoString.h>

#   ifdef USE_MOTIF

#   include	<X11/Xatom.h>
#   include	<Xm/RowColumn.h>
#   include	<Xm/PushB.h>
#   include	<Xm/PushBG.h>
#   include	<Xm/Form.h>
#   include	<Xm/Label.h>
#   include	<Xm/ToggleB.h>
#   include	<Xm/PanedW.h>
#   include	<Xm/MwmUtil.h>
#   include	<Xm/Protocols.h>
#   include	<Xm/DialogS.h>
#   include	<Xm/DrawnB.h>
#   include	<Xm/Separator.h>
#   include	<Xm/List.h>
#   include	<Xm/ScrollBar.h>
#   include	<Xm/SashP.h>
#   include	<Xm/DrawingA.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Tame the sashes in an XmPanedWindow.				*/
/*									*/
/************************************************************************/

void appMotifTurnOfSashTraversal(	Widget		column )
    {
    WidgetList		children;
    Cardinal		childCount;
    int			i;

    XtVaGetValues( column,
			XmNchildren,		&children,
			XmNnumChildren,		&childCount,
			NULL );

    if  ( childCount == 0 )
	{ return;	}

    for ( i= 0; i < childCount; children++, i++ )
	{
	if  ( XmIsSash( children[0] ) )
	    {
	    XtVaSetValues(	children[0],
				    XmNtraversalOn,	False,
				    XmNsensitive,	False,
				    NULL );
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Destroy all children of a widget.					*/
/*									*/
/*  1)  Unmanaging should be done by the Xt toolkit but is not always	*/
/*	done correctly. Doing it here does no harm anyway.		*/
/*									*/
/************************************************************************/

void appEmptyParentWidget(	Widget		parent )
    {
    WidgetList		children;
    WidgetList		save;
    Cardinal		childCount;

    XtVaGetValues( parent,
			XmNchildren,		&children,
			XmNnumChildren,		&childCount,
			NULL );

    if  ( childCount == 0 )
	{ return;	}

    /*  1  */
    XtUnmanageChildren( children, childCount );

    save= (WidgetList)malloc( childCount* sizeof(Widget) );
    if  ( save )
	{
	int	i;

	for ( i= childCount- 1; i >= 0; i-- )
	    { save[i]= children[i];	}

	for ( i= childCount- 1; i >= 0; i-- )
	    { XtDestroyWidget( save[i] );	}

	free( save );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Use the text of an option and the name of the application as a	*/
/*  widget title.							*/
/*									*/
/************************************************************************/

void appSetShellTitle(	APP_WIDGET		shell,
			Widget			option,
			const char *		applicationName )
    {
    char *	title;

    if  ( option )
	{
	char *		s;

	s= appGetTextFromMenuOption( option );

	title= (char *)malloc( strlen( applicationName )+ 1+ strlen( s )+ 1 );
	sprintf( title, "%s %s", applicationName, s );

	appFreeTextFromMenuOption( s );
	}
    else{ title= strdup( applicationName );	}

    XtVaSetValues( shell,
		    XmNtitle, title,
		    NULL );

    free( title );
    }

void appGuiLowerShellWidget(	APP_WIDGET	shell )
    {
    XLowerWindow( XtDisplay( shell ), XtWindow( shell ) );

    return;
    }

/************************************************************************/
/*									*/
/*  1)  Remember te initial size of a shell as its minimum size.	*/
/*									*/
/*  2)  Remember te initial size of a shell as its minimum and its	*/
/*	maximum size. This event handler can be used to prevent the	*/
/*	user from resizing a dialog. E.G. because KDE does not listen	*/
/*	to the window manager hints about this. On the other hand some	*/
/*	window managers (mwm on vms!) cause this event handler to be	*/
/*	called too early. That results in dialogs that are MUCH too	*/
/*	small.								*/
/*									*/
/************************************************************************/

/*  1  */
void appSetSizeAsMinimum(	Widget			w,
				void *			through,
				XEvent *		event,
				Boolean *		pRefused )
    {
    XConfigureEvent *		cevent= &(event->xconfigure);

    if  ( cevent->type != ConfigureNotify )
	{ return;	}

    XtVaSetValues( w,	XmNminWidth,	cevent->width,
			XmNminHeight,	cevent->height,
			NULL );

    XtRemoveEventHandler( w, StructureNotifyMask, False,
					    appSetSizeAsMinimum, through );

    *pRefused= 1;

    return;
    }

/*  2  */
static void appFixSize(	Widget			w,
			void *			through,
			XEvent *		event,
			Boolean *		pRefused )
    {
    XConfigureEvent *		cevent= &(event->xconfigure);

    if  ( cevent->type != ConfigureNotify )
	{ return;	}

    XtVaSetValues( w,	XmNminWidth,	cevent->width,
			XmNmaxWidth,	cevent->width,
			XmNminHeight,	cevent->height,
			XmNmaxHeight,	cevent->height,
			NULL );

    XtRemoveEventHandler( w, StructureNotifyMask, False,
					    appFixSize, through );

    *pRefused= 1;

    return;
    }

/************************************************************************/
/*									*/
/*  Names of the X11 events.						*/
/*									*/
/************************************************************************/

char * APP_X11EventNames[]=
    {
    "0", "1", "KeyPress", "KeyRelease", "ButtonPress", "ButtonRelease",
    "MotionNotify", "EnterNotify", "LeaveNotify", "FocusIn", "FocusOut",
    "KeymapNotify", "Expose", "GraphicsExpose", "NoExpose", "VisibilityNotify",
    "CreateNotify", "DestroyNotify", "UnmapNotify", "MapNotify", "MapRequest",
    "ReparentNotify", "ConfigureNotify", "ConfigureRequest", "GravityNotify",
    "ResizeRequest", "CirculateNotify", "CirculateRequest", "PropertyNotify",
    "SelectionClear", "SelectionRequest", "SelectionNotify", "ColormapNotify",
    "ClientMessage", "MappingNotify"
    };


/************************************************************************/
/*									*/
/*  Make a in a form containing a row of buttons.			*/
/*									*/
/************************************************************************/

void appMakeButtonInRow(	APP_WIDGET *		pButton,
				APP_WIDGET		row,
				const char *		text,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through,
				int			position,
				int			showAsDefault )
    {
    Arg		al[20];
    int		ac= 0;

    XmString	labelString;
    Widget	button;

    labelString= XmStringCreateLocalized( (char *)text );

    ac= 0;
    XtSetArg( al[ac], XmNlabelString,		labelString ); ac++;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNbottomAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNleftPosition,		position ); ac++;
    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNrightPosition,		position+ 1 ); ac++;

    XtSetArg( al[ac], XmNtopOffset,		0 ); ac++;
    XtSetArg( al[ac], XmNbottomOffset,		0 ); ac++;
    XtSetArg( al[ac], XmNleftOffset,		0 ); ac++;
    XtSetArg( al[ac], XmNrightOffset,		0 ); ac++;

    if  ( showAsDefault )
	{
	XtSetArg( al[ac], XmNdefaultButtonShadowThickness, 1 ); ac++;
	XtSetArg( al[ac], XmNshowAsDefault, showAsDefault != 0 ); ac++;
	}
    else{
	XtSetArg( al[ac], XmNdefaultButtonShadowThickness, 0 ); ac++;
	}

    button= XmCreatePushButton( row, WIDGET_NAME, al, ac );

    XmStringFree( labelString );

    if  ( callback )
	{ XtAddCallback( button, XmNactivateCallback, callback, through ); }

    XtManageChild( button );

    if  ( showAsDefault )
	{
	XtVaSetValues( row,
			XmNdefaultButton,	button,
			NULL );
	}

    *pButton= button; return;
    }

APP_WIDGET appMakeToggleInRow(	APP_WIDGET		row,
				const char *		text,
				APP_TOGGLE_CALLBACK_T	callback,
				void *			through,
				int			position )
    {
    Arg		al[20];
    int		ac= 0;

    XmString	labelString;
    Widget	toggle;

    labelString= XmStringCreateLocalized( (char *)text );

    ac= 0;
    XtSetArg( al[ac], XmNlabelString,		labelString ); ac++;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNbottomAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNleftPosition,		position ); ac++;
    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNrightPosition,		position+ 1 ); ac++;
 
    XtSetArg( al[ac], XmNtopOffset,		0 ); ac++;
    XtSetArg( al[ac], XmNbottomOffset,		0 ); ac++;
    XtSetArg( al[ac], XmNleftOffset,		0 ); ac++;
    XtSetArg( al[ac], XmNrightOffset,		0 ); ac++;

    XtSetArg( al[ac], XmNrecomputeSize,		True ); ac++;

    XtSetArg( al[ac], XmNmarginTop,		TBrmargT ); ac++;

    XtSetArg( al[ac], XmNalignment,		XmALIGNMENT_BEGINNING ); ac++;

    toggle= XmCreateToggleButton( row, WIDGET_NAME, al, ac );

    if  ( callback )
	{ XtAddCallback( toggle, XmNvalueChangedCallback, callback, through ); }

    XmStringFree( labelString );

    XtManageChild( toggle );

    return toggle;
    }

APP_WIDGET appMakeRowInColumn(	APP_WIDGET	column,
				int		columnCount,
				int		heightResizable )
    {
    Arg			al[20];
    int			ac= 0;

    Widget		row;

    ac= 0;
    XtSetArg( al[ac],	XmNfractionBase,	columnCount ); ac++;
    XtSetArg( al[ac],	XmNskipAdjust,		heightResizable == 0 ); ac++;

    XtSetArg( al[ac],	XmNallowResize,		True ); ac++;

    row= XmCreateForm( column, WIDGET_NAME, al, ac );

    XtManageChild( row );

    appMotifTurnOfSashTraversal( column );

    return row;
    }

/************************************************************************/
/*									*/
/*  Make a column inside a row.						*/
/*									*/
/************************************************************************/

void appMakeColumnInRow(	APP_WIDGET *	pColumn,
				APP_WIDGET	row,
				int		position,
				int		colspan )
    {
    Arg			al[20];
    int			ac= 0;

    Widget		column;

    ac= 0;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNbottomAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNleftPosition,		position ); ac++;
    XtSetArg( al[ac], XmNrightPosition,		position+ colspan ); ac++;

    XtSetArg( al[ac], XmNsashWidth,		1 ); ac++;
    XtSetArg( al[ac], XmNsashHeight,		1 ); ac++;
    XtSetArg( al[ac], XmNseparatorOn,		False ); ac++;
    XtSetArg( al[ac], XmNmarginWidth,		PWmargW ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,		PWmargH ); ac++;
    XtSetArg( al[ac], XmNspacing,		PWspacing ); ac++;
    XtSetArg( al[ac], XmNallowResize,		True ); ac++;

    column= XmCreatePanedWindow( row, WIDGET_NAME, al, ac );

    XtManageChild( column );

    *pColumn= column; return;
    }

void appMakeImageInColumn(	APP_WIDGET *		pLabel,
				APP_WIDGET		column,
				APP_BITMAP_IMAGE	pixmap,
				APP_BITMAP_MASK		mask )
    {
    Widget		label;

    Arg			al[20];
    int			ac= 0;

    ac= 0;
    XtSetArg( al[ac], XmNlabelType,		XmPIXMAP ); ac++;
    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    XtSetArg( al[ac], XmNrecomputeSize,		True ); ac++;
    XtSetArg( al[ac], XmNallowResize,		True ); ac++;
    XtSetArg( al[ac], XmNalignment,		XmALIGNMENT_BEGINNING ); ac++;

    XtSetArg( al[ac], XmNlabelPixmap,		pixmap ); ac++;

    label= XmCreateLabel( column, WIDGET_NAME, al, ac );

    XtManageChild( label );

    appMotifTurnOfSashTraversal( column );

    *pLabel= label; return;
    }

void appMakeColumnToggle(	Widget *		pToggle,
				Widget			column,
				XtCallbackProc		callback,
				void *			through,
				const char *		labelText,
				int			set )
    {
    Widget		toggle;

    XmString		labelString= (XmString)0;

    Arg			al[20];
    int			ac= 0;

    if  ( labelText )
	{ labelString= XmStringCreateLocalized( (char *)labelText ); }

    ac= 0;
    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    XtSetArg( al[ac], XmNrecomputeSize,		True ); ac++;
    XtSetArg( al[ac], XmNalignment,		XmALIGNMENT_BEGINNING ); ac++;
    XtSetArg( al[ac], XmNset,			set ); ac++;

    XtSetArg( al[ac], XmNmarginTop,		TBcmargT ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,		TBcmargH ); ac++;

    if  ( labelString )
	{ XtSetArg( al[ac], XmNlabelString,	labelString ); ac++; }

    toggle= XmCreateToggleButton( column, WIDGET_NAME, al, ac );

    if  ( callback )
	{ XtAddCallback( toggle, XmNvalueChangedCallback, callback, through ); }

    if  ( labelString )
	{ XmStringFree( labelString );	}

    XtManageChild( toggle );

    *pToggle= toggle;
    }

void appMakeColumnDrawing(	Widget *		pButton,
				Widget			column,
				XtCallbackProc		exposeCallback,
				XtCallbackProc		pushedCallback,
				void *			through,
				int			width )
    {
    Widget		button;

    Arg			al[20];
    int			ac= 0;

    XmString		labelString= XmStringCreateLocalized( " " );

    ac= 0;
    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    XtSetArg( al[ac], XmNrecomputeSize,		True ); ac++;
    XtSetArg( al[ac], XmNwidth,			width ); ac++;

    XtSetArg( al[ac], XmNmarginTop,		1 ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,		4 ); ac++;

    XtSetArg( al[ac], XmNlabelString,		labelString ); ac++;

    button= XmCreateDrawnButton( column, WIDGET_NAME, al, ac );

    if  ( exposeCallback )
	{
	XtAddCallback( button, XmNexposeCallback, exposeCallback, through );
	}

    if  ( pushedCallback )
	{
	XtAddCallback( button, XmNactivateCallback, pushedCallback, through );
	}

    XtManageChild( button );

    XmStringFree( labelString );

    *pButton= button;
    }

/************************************************************************/
/*									*/
/*  Make a tool with a vertical organisation.				*/
/*									*/
/************************************************************************/

void appMakeVerticalTool(	APP_WIDGET *		pShell,
				APP_WIDGET *		pPaned,
				EditApplication *	ea,
				APP_BITMAP_IMAGE	iconPixmap,
				APP_BITMAP_MASK		iconMask,
				const char *		widgetName,
				int			userResizable,
				APP_WIDGET		option,
				APP_CLOSE_CALLBACK_T	closeCallback,
				void *			through )
    {
    Widget		shell;
    Widget		paned;

    Arg			al[20];
    int			ac= 0;

    MwmHints		hints;

    hints.flags= MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;

    hints.functions=	MWM_FUNC_MOVE		|
			MWM_FUNC_MINIMIZE	|
			MWM_FUNC_CLOSE		;
    hints.decorations=	MWM_DECOR_BORDER	|
			MWM_DECOR_TITLE		|
			MWM_DECOR_MENU		|
			MWM_DECOR_MINIMIZE	;

    if  ( userResizable )
	{
	hints.functions |=	MWM_FUNC_RESIZE;
	hints.decorations |=	MWM_DECOR_RESIZEH;
	}

    ac=0;
    XtSetArg( al[ac], XmNdeleteResponse,	XmDO_NOTHING ); ac++;
    XtSetArg( al[ac], XmNinput,			True ); ac++;
    XtSetArg( al[ac], XmNallowShellResize,	True ); ac++;
    XtSetArg( al[ac], XmNuseAsyncGeometry,	True ); ac++;
    XtSetArg( al[ac], XmNwaitForWm,		False ); ac++; /* LessTif BUG */
    XtSetArg( al[ac], XmNwmTimeout,		0 ); ac++; /* LessTif BUG */
    XtSetArg( al[ac], XmNmwmDecorations,	hints.decorations ); ac++;
    XtSetArg( al[ac], XmNmwmFunctions,		hints.functions ); ac++;

    if  ( iconPixmap )
	{ XtSetArg( al[ac], XmNiconPixmap,	iconPixmap ); ac++; }

#   ifdef USE_X11_R5
    shell= XtAppCreateShell( ea->eaApplicationName, widgetName,
			    applicationShellWidgetClass,
			    XtDisplay( ea->eaToplevel.atTopWidget ), al, ac );
#   else
    shell= XtAppCreateShell( ea->eaApplicationName, widgetName,
			    topLevelShellWidgetClass,
			    XtDisplay( ea->eaToplevel.atTopWidget ), al, ac );
#   endif

    XtAddEventHandler( shell, StructureNotifyMask, False,
					    appSetSizeAsMinimum, through );

    appSetShellTitle( shell, option, ea->eaApplicationName );

#   if 0
    See above.
    if  ( ! userResizable )
	{
	XtAddEventHandler( shell, StructureNotifyMask, False,
						    appFixSize, through );
	}
#   endif

    appSetCloseCallback( shell, ea, closeCallback, through );

    ac= 0;
    XtSetArg( al[ac], XmNsashWidth,		1 ); ac++;
    XtSetArg( al[ac], XmNsashHeight,		1 ); ac++;
    XtSetArg( al[ac], XmNseparatorOn,		False ); ac++;
    XtSetArg( al[ac], XmNmarginWidth,		PWmargW ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,		PWmargH ); ac++;
    XtSetArg( al[ac], XmNspacing,		PWspacing ); ac++;
    paned= XmCreatePanedWindow( shell, WIDGET_NAME, al, ac );

    XtManageChild( paned );

    *pShell= shell, *pPaned= paned; return;
    }

/************************************************************************/
/*									*/
/*  For debugging purposes: Give widgets a name of the form file(line)	*/
/*  for where they are made.						*/
/*									*/
/************************************************************************/

char * appWidgetName(	char *	file,
			int	line )
    {
    static char **	names;
    static int		nameCount;

    char **		freshNames;
    char *		freshName;

    int			i;

    freshNames= (char **)realloc( names, ( nameCount+ 1 )* sizeof(char *) );
    if  ( ! freshNames )
	{ XDEB(freshNames); return file;	}
    names= freshNames;

    freshName= (char *)malloc( strlen( file )+ 11 );
    if  ( ! freshName )
	{ XDEB(freshName); return file;	}

    sprintf( freshName, "%s(%d)", file, line );

    for ( i= 0; i < nameCount; freshNames++, i++ )
	{
	if  ( ! strcmp( freshNames[0], freshName ) )
	    { free( freshName ); return freshNames[0];	}
	}

    names[nameCount++]= freshName;

    return freshName;
    }

/************************************************************************/
/*									*/
/*  Retrieve configurable resource values from the GUI environment.	*/
/*									*/
/************************************************************************/

void appGuiGetResourceValues(	int *				pGotResources,
				EditApplication *		ea,
				void *				pValues,
				AppConfigurableResource *	acr,
				int				acrCount )
    {
    if  ( *pGotResources )
	{ LDEB(*pGotResources); }

    if  ( ! *pGotResources )
	{ appSetResourceDefaults( ea, acr, acrCount );	}

    if  ( ea->eaToplevel.atTopWidget )
	{
	XtGetApplicationResources( ea->eaToplevel.atTopWidget, pValues,
						    acr, acrCount, NULL, 0 );
	}
    else{
	char *		values= (char *)pValues;
	int		i;

	for ( i= 0; i < acrCount; acr++, i++ )
	    {
	    *((const char **)(values+acr->acrStructOffset))=
						    acr->acrDefaultValue;
	    }
	}

    *pGotResources= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  Callbacks for session management.					*/
/*									*/
/************************************************************************/

#   ifndef USE_X11_R5

/************************************************************************/
/*									*/
/*  Session management support.						*/
/*									*/
/*  0)  Choose a short ID, otherwise xsm crashes at starup.		*/
/*  00) Do not save a discard command: Setting it here crashes xsm.	*/
/*  A)  When the shutdown was canceled, or we happen not to be allowed	*/
/*	to interact with the user, tell the session manager that we	*/
/*	have not saved our state and return the token to tell the	*/
/*	session manager that we are done.				*/
/*  1)  When the session is shut down, or if user interaction is	*/
/*	allowed later on, turn off the whole application for the	*/
/*	moment.								*/
/*	NOTE that the comment in the example on p 35 of the X11R6	*/
/*	X11R6 programmers supplement is misleading.			*/
/*  2)  When no interaction is allowed, we can only just save our	*/
/*	state.								*/
/*  3)  If there are unsaved documents tell the session manager that	*/
/*	we want to give the user her word. The callback is removed by	*/
/*	Xt.								*/
/*  z)  For one reason or another, we survived session management. Turn	*/
/*	on the application widgets again.				*/
/*									*/
/************************************************************************/

static void appSessionFreeCommand(	char **		command )
    {
    char **	cmd= command;

    while( *cmd )
	{ free( *cmd ); cmd++; }

    free( command );

    return;
    }

static char ** appSessionAllocateCommand(	int		argc,
						const char *	argv0 )
    {
    char **	rval= malloc( ( argc+ 1 )* sizeof(char *) );

    if  ( ! rval )
	{ XDEB(rval); return rval;	}

    rval[0]= strdup( argv0 );
    if  ( ! rval[0] )
	{ free( rval ); return (char **)0;	}

    rval[1]= (char *)0;

    return rval;
    }

static int appSessionSaveState(	EditApplication *	ea,
				int			saveChanged,
				int			interactive )
    {
    char **	restartCommand= (char **)0;
    char **	discardCommand= (char **)0;

    char **	restartTarget= (char **)0;
    char **	discardTarget= (char **)0;

    char	currentDirectory[1000+1];
    char	saveDirectory[1000+1];
    int		dirlen;

    char	scratch[15];
    char *	id;
    int		idlen;

    char *	sessionID= (char *)0;
    char *	smSaveDir= getenv( "SM_SAVE_DIR" );

    int		rval= 0;
    int		changedCount;
    int		restartCount;

    int		i;

    if  ( appCurrentDirectory( currentDirectory,
					sizeof(currentDirectory)- 1 ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( smSaveDir )
	{
	dirlen= strlen( smSaveDir );
	if  ( dirlen < 1 || dirlen >= sizeof(saveDirectory) )
	    { LDEB(dirlen); rval= -1; goto ready;	}

	strcpy( saveDirectory, smSaveDir );
	}
    else{
	dirlen= appHomeDirectory( saveDirectory,
		sizeof(saveDirectory)- 3- strlen( ea->eaApplicationName ) );
	if  ( dirlen < 0 )
	    { LDEB(dirlen); rval= -1; goto ready;	}

	strcat( saveDirectory, "/." );
	strcat( saveDirectory, ea->eaApplicationName );

	dirlen= strlen( saveDirectory );
	}

    if  ( appTestDirectory( saveDirectory )	&&
	  appMakeDirectory( saveDirectory )	)
	{ SDEB(saveDirectory); rval= -1; goto ready;	}

    XtVaGetValues( ea->eaToplevel.atTopWidget,
			XtNsessionID,	&sessionID,
			NULL );

    restartCommand= appSessionAllocateCommand(
				    3+ 4* ea->eaOpenCount+ 1, ea->eaArgv[0] );
    if  ( ! restartCommand )
	{ XDEB(restartCommand); rval= -1; goto ready;	}
    restartTarget= restartCommand+ 1;

    discardCommand= appSessionAllocateCommand(
				    3+ 2* ea->eaOpenCount+ 2, ea->eaArgv[0] );
    if  ( ! discardCommand )
	{ XDEB(discardCommand); rval= -1; goto ready;	}
    discardTarget= discardCommand+ 1;

    if  ( sessionID )
	{
	*restartTarget= strdup( "-xtsessionID" );
	if  ( ! *restartTarget )
	    { XDEB(*restartTarget); rval= -1; goto ready;	}
	*(++restartTarget)= (char *)0;

	*restartTarget= strdup( sessionID );
	if  ( ! *restartTarget )
	    { XDEB(*restartTarget); rval= -1; goto ready;	}
	*(++restartTarget)= (char *)0;

	id= sessionID; idlen= strlen( id );
	}

    /*  0 !! */
    sprintf( scratch, "%08lx", appGetTimestamp() );
    id= scratch; idlen= strlen( id );

    changedCount= 0;
    restartCount= 0;
    for ( i= 0; i < ea->eaOpenCount; i++ )
	{
	EditDocument *	ed= ea->eaOpenDocuments[i];

	if  ( ed->edCheckpointFilename )
	    {
	    if  ( appRemoveFile( ed->edCheckpointFilename ) )
		{ SDEB(ed->edCheckpointFilename);	}

	    free( ed->edCheckpointFilename );
	    ed->edCheckpointFilename= (char *)0;
	    }

	if  ( saveChanged					&&
	      ( ed->edHasBeenChanged || ! ed->edFilename )	)
	    {
	    int			size;
	    const char *	ext;
	    char *		name;

	    changedCount++;
	    restartCount++;

	    if  ( ed->edFilename )
		{
		*restartTarget= strdup( "++Upd" );
		if  ( ! *restartTarget )
		    { XDEB(*restartTarget); rval= -1; goto ready;	}
		*(++restartTarget)= (char *)0;
		}
	    else{
		*restartTarget= strdup( "++New" );
		if  ( ! *restartTarget )
		    { XDEB(*restartTarget); rval= -1; goto ready;	}
		*(++restartTarget)= (char *)0;
		}

	    *discardTarget= strdup( "++Old" );
	    if  ( ! *discardTarget )
		{ XDEB(*discardTarget); rval= -1; goto ready;	}
	    *(++discardTarget)= (char *)0;

	    size= dirlen+ 1+ idlen+ 20;
	    ext= ea->eaFileExtensions[ed->edFormat].afeExtension;
	    if  ( ext )
		{
		name= malloc( size+ 1+ strlen( ext ) );
		if  ( ! name )
		    { XDEB(name); rval= -1; goto ready;	}
		sprintf( name, "%s/%s-%d.%s", saveDirectory, id, i, ext );

		}
	    else{
		name= malloc( size+ 1 );
		if  ( ! name )
		    { XDEB(name); rval= -1; goto ready;	}
		sprintf( name, "%s/%s-%d", saveDirectory, id, i );
		}
	    ed->edCheckpointFilename= name;

	    appDocSaveDocumentByName( ed, (Widget)0, interactive,
							ed->edFormat, name );

	    *discardTarget= strdup( name );
	    if  ( ! *discardTarget )
		{ XDEB(*discardTarget); rval= -1; goto ready;	}
	    *(++discardTarget)= (char *)0;

	    *restartTarget= strdup( name );
	    if  ( ! *restartTarget )
		{ XDEB(*restartTarget); rval= -1; goto ready;	}
	    *(++restartTarget)= (char *)0;

	    *restartTarget= strdup( ed->edTitle?ed->edTitle:"?" );
	    if  ( ! *restartTarget )
		{ XDEB(*restartTarget); rval= -1; goto ready;	}
	    *(++restartTarget)= (char *)0;

	    if  ( ed->edFilename )
		{
		*restartTarget= strdup( ed->edFilename );
		if  ( ! *restartTarget )
		    { XDEB(*restartTarget); rval= -1; goto ready;	}
		*(++restartTarget)= (char *)0;
		}
	    }
	else{
	    if  ( ed->edFilename )
		{
		restartCount++;

		*restartTarget= strdup( ed->edFilename );
		if  ( ! *restartTarget )
		    { XDEB(*restartTarget); rval= -1; goto ready;	}
		*(++restartTarget)= (char *)0;
		}
	    }
	}

    *discardTarget= strdup( "++Exit" );
    if  ( ! *discardTarget )
	{ XDEB(*discardTarget); rval= -1; goto ready;	}
    *(++discardTarget)= (char *)0;

    XtVaSetValues( ea->eaToplevel.atTopWidget,
		    XtNcurrentDirectory,	currentDirectory,
		    XtNrestartCommand,		restartCommand,
		    /* 00
		    XtNdiscardCommand,		discardCommand,
		    */
		    NULL );

  ready:
    if  ( restartCommand )
	{ appSessionFreeCommand( restartCommand );	}
    if  ( discardCommand )
	{ appSessionFreeCommand( discardCommand );	}

    return rval;
    }

static void appSessionInteractCallBack(	Widget			w,
					void *			voidea,
					void *			voidxct )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    XtCheckpointToken		xct= (XtCheckpointToken)voidxct;
    Widget			relative= ea->eaToplevel.atTopWidget;

    int				rcc;
    const int			interactive= 1;

    /*  A  */
    if  ( xct->cancel_shutdown				||
	  xct->interact_style == SmInteractStyleNone	)
	{
	xct->save_success= False;
	XtSessionReturnToken( xct );
	return;
	}

    if  ( xct->interact_style == SmInteractStyleAny	&&
	  xct->shutdown					)
	{
	int	changedCount;

	changedCount= appCountChangedDocuments( ea );

	if  ( changedCount > 1 )
	    {
	    rcc= appRunReallyQuitDialog( (Widget)0, relative, ea );

	    switch( rcc )
		{
		case AQDrespCANCEL:
		    xct->request_cancel= True;
		    xct->save_success= False;

		    XtSessionReturnToken( xct );
		    return;

		case AQDrespNO:
		    {
		    const int	saveChanged= 0;

		    if  ( appSessionSaveState( ea, saveChanged, interactive ) )
			{ LDEB(1); xct->save_success= False;	}
		    else{ xct->save_success= True;		}

		    xct->request_cancel= False;

		    XtSessionReturnToken( xct );
		    if  ( xct->shutdown )
			{ appExitApplication( ea ); LDEB(1);	}
		    return;
		    }

		default:
		    LDEB(rcc); return;

		case AQDrespYES:
		    break;
		}
	    }

	appAskCloseDocuments( (Widget)0, ea );

	changedCount= appCountChangedDocuments( ea );
	if  ( changedCount > 0 )
	    {
	    xct->request_cancel= True;
	    xct->save_success= False;
	    XtSessionReturnToken( xct );
	    return;
	    }
	else{
	    const int	saveChanged= 0;

	    if  ( appSessionSaveState( ea, saveChanged, interactive ) )
		{ LDEB(1); xct->save_success= False;	}
	    else{ xct->save_success= True;		}

	    xct->request_cancel= False;

	    XtSessionReturnToken( xct );
	    if  ( xct->shutdown )
		{ appExitApplication( ea ); LDEB(1);	}
	    return;
	    }
	}

    {
    const int	saveChanged= 1;

    if  ( appSessionSaveState( ea, saveChanged, interactive ) )
	{ LDEB(1); xct->save_success= False;	}
    else{ xct->save_success= True;		}

    xct->request_cancel= False;

    XtSessionReturnToken( xct );

    /* NO!
    if  ( xct->shutdown )
	{ appExitApplication( ea ); LDEB(1);	}
    */
    }
    }


static void appSessionMakeSensitive(	EditApplication *	ea,
					int			sensitive )
    {
    int		i;

    XtSetSensitive( ea->eaToplevel.atTopWidget, sensitive );

    for ( i= 0; i < ea->eaOpenCount; i++ )
	{
	EditDocument *	ed= ea->eaOpenDocuments[i];

	XtSetSensitive( ed->edToplevel.atTopWidget, sensitive );
	}

    return;
    }

static void appSessionSaveCallBack(	Widget			w,
					void *			voidea,
					void *			voidxct )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    XtCheckpointToken		xct= (XtCheckpointToken)voidxct;

    /*  1  */
    if  ( xct->shutdown					||
	  xct->interact_style != SmInteractStyleNone	)
	{ appSessionMakeSensitive( ea, False );	}

    /*  2  */
    if  ( xct->interact_style == SmInteractStyleNone )
	{
	const int	saveChanged= 1;
	const int	interactive= 0;

	if  ( appSessionSaveState( ea, saveChanged, interactive ) )
	    { LDEB(1); xct->save_success= False;	}
	else{ xct->save_success= True;			}

	return;
	}

    /*  3  */
    if  ( xct->interact_style == SmInteractStyleAny )
	{ xct->interact_dialog_type= SmDialogNormal;	}
    else{ xct->interact_dialog_type= SmDialogError;		}

    XtAddCallback( ea->eaToplevel.atTopWidget, XtNinteractCallback,
					appSessionInteractCallBack, voidea );

    return;
    }

static void appSessionDieCallBack(	Widget			w,
					void *			voidea,
					void *			vnull )
    {
    EditApplication *		ea= (EditApplication *)voidea;

    appExitApplication( ea );

    return;
    }

static void appSessionCancelCallBack(	Widget			w,
					void *			voidea,
					void *			vnull )
    {
    EditApplication *		ea= (EditApplication *)voidea;

    /*  z  */
    appSessionMakeSensitive( ea, True );

    return;
    }

static void appSessionSaveCompleteCallBack( Widget		w,
					void *			voidea,
					void *			vnull )
    {
    EditApplication *		ea= (EditApplication *)voidea;

    /*  z  */
    appSessionMakeSensitive( ea, True );

    return;
    }

#   endif

/************************************************************************/
/*									*/
/*  Make the initial connection to the gui environment.			*/
/*									*/
/************************************************************************/

static int appXErrorHandler( Display * display, XErrorEvent * ev )
    {
    char	text[250+1];
    static int	count= 0;

    XGetErrorText( display, ev->error_code, text, sizeof(text)- 1 );

    appDebug( "%3d: %s\n", count, text );

    if  ( count++ >= 100 )
	{ exit( 1 ); }

    return 0;
    }

int appGuiInitApplication(	EditApplication *	ea,
				int *			pArgc,
				char ***		pArgv )
    {
    int			argc= *pArgc;
    char **		argv= *pArgv;

    MwmHints		hints;

    Arg			al[20];
    int			ac= 0;

#   ifndef USE_X11_R5
    char *		cloneCommand[2];
#   endif

    if  ( ! XSupportsLocale() )
	{ SLDEB(getenv("LANG"),XSupportsLocale()); }

#   if 0

    This is actually counter productive. It is already done by Motif/Xt.
    The document that states XtSetLanguageProc() is indespensable
    refers to a lower level of software. At the application level,
    we can trust the libraries to call XtSetLanguageProc().

    XtSetLanguageProc( NULL, NULL, NULL );

#   endif

    /*  1  */
    hints.flags= MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;
    hints.functions=	MWM_FUNC_MOVE		|
			MWM_FUNC_MINIMIZE	|
			MWM_FUNC_CLOSE		;
    hints.decorations=	MWM_DECOR_BORDER	|
			MWM_DECOR_TITLE		|
			MWM_DECOR_MENU		|
			MWM_DECOR_MINIMIZE	;
    ac= 0;
    XtSetArg( al[ac], XmNinput,			True ); ac++;
    XtSetArg( al[ac], XmNallowShellResize,	True ); ac++;
    XtSetArg( al[ac], XmNuseAsyncGeometry,	True ); ac++;
    XtSetArg( al[ac], XmNwaitForWm,		False ); ac++; /* LessTif BUG */
    XtSetArg( al[ac], XmNwmTimeout,		0 ); ac++; /* LessTif BUG */
    XtSetArg( al[ac], XmNmwmDecorations,	hints.decorations ); ac++;
    XtSetArg( al[ac], XmNmwmFunctions,		hints.functions ); ac++;

#   ifdef USE_X11_R5
    /* applicationShellWidgetClass, */
    ea->eaToplevel.atTopWidget= XtAppInitialize( &(ea->eaContext),
					ea->eaApplicationName,
					NULL, 0, &argc, argv, NULL, al, ac );
#   else
    cloneCommand[0]= argv[0];
    cloneCommand[1]= (char *)0;
    XtSetArg( al[ac], XtNcloneCommand,		cloneCommand ); ac++;

    ea->eaToplevel.atTopWidget= XtOpenApplication( &(ea->eaContext),
					ea->eaApplicationName,
					NULL, 0, &argc, argv, NULL,
					sessionShellWidgetClass,
					al, ac );

    XtAddCallback( ea->eaToplevel.atTopWidget, XtNsaveCallback,
				appSessionSaveCallBack, (void *)ea );
    XtAddCallback( ea->eaToplevel.atTopWidget, XtNcancelCallback,
				appSessionCancelCallBack, (void *)ea );
    XtAddCallback( ea->eaToplevel.atTopWidget, XtNsaveCompleteCallback,
				appSessionSaveCompleteCallBack, (void *)ea );
    XtAddCallback( ea->eaToplevel.atTopWidget, XtNdieCallback,
				appSessionDieCallBack, (void *)ea );
#   endif

    /*  2  */
    ea->eaScreenPixelsWide=
		    DisplayWidth( XtDisplay( ea->eaToplevel.atTopWidget ),
		    DefaultScreen( XtDisplay( ea->eaToplevel.atTopWidget ) ) );
    ea->eaScreenPixelsHigh=
		    DisplayHeight( XtDisplay( ea->eaToplevel.atTopWidget ),
		    DefaultScreen( XtDisplay( ea->eaToplevel.atTopWidget ) ) );

    ea->eaInputMethod= XOpenIM( XtDisplay( ea->eaToplevel.atTopWidget ),
							NULL, NULL, NULL );
    ea->eaDocumentCursor= (Cursor)0;

    /*  3  */
    ea->eaCloseAtom= XmInternAtom( XtDisplay( ea->eaToplevel.atTopWidget ),
						"WM_DELETE_WINDOW", False );

    ea->eaArgc= argc;
    ea->eaArgv= argv;

    appSetCloseCallback( ea->eaToplevel.atTopWidget, ea,
						appAppWmClose, (void *)ea );

    XtRealizeWidget( ea->eaToplevel.atTopWidget );

    *pArgc= argc;
    *pArgv= argv;

    if  ( getenv( "CATCH_X_ERRORS" ) )
	{ XSetErrorHandler( appXErrorHandler );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a column in a top level widget (Window)			*/
/*									*/
/************************************************************************/

void appGuiInsertColumnInWindow(	APP_WIDGET *	pColumn,
					APP_WIDGET	parent )
    {
    APP_WIDGET		column;

    Arg			al[20];
    int			ac= 0;

    ac= 0;
    XtSetArg( al[ac],	XmNsashWidth,		1 ); ac++;
    XtSetArg( al[ac],	XmNsashHeight,		1 ); ac++;
    XtSetArg( al[ac],	XmNseparatorOn,		False ); ac++;
    XtSetArg( al[ac],	XmNmarginWidth,		0 ); ac++;
    XtSetArg( al[ac],	XmNmarginHeight,	0 ); ac++;
    XtSetArg( al[ac],	XmNspacing,		0 ); ac++;

    column= XmCreatePanedWindow( parent, WIDGET_NAME, al, ac );

    XtManageChild( column );

    *pColumn= column; return;
    }

/************************************************************************/
/*									*/
/*  Insert a menubar as the first row of a column.			*/
/*									*/
/************************************************************************/

void appGuiInsertMenubarInColumn(	APP_WIDGET *	pMenubar,
					APP_WIDGET	parent )
    {
    Widget		menuBar;

    Arg			al[20];
    int			ac= 0;

    ac= 0;
    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    XtSetArg( al[ac], XmNallowResize,		True ); ac++;
    XtSetArg( al[ac], XmNtraversalOn,		False ); ac++;
    XtSetArg( al[ac], XmNresizeHeight,		True ); ac++;

    menuBar= XmCreateMenuBar( parent, WIDGET_NAME, al, ac );

    XtManageChild( menuBar );

    *pMenubar= menuBar; return;
    }

/************************************************************************/
/*									*/
/*  Insert a separator as a row of a column.				*/
/*									*/
/************************************************************************/

void appGuiInsertSeparatorInColumn(	APP_WIDGET *	pSeparator,
					APP_WIDGET	column )
    {
    Arg			al[20];
    int			ac= 0;

    *pSeparator= XmCreateSeparator( column, WIDGET_NAME, al, ac );

    XtManageChild( *pSeparator );

    return;
    }

/************************************************************************/
/*									*/
/*  Change the label/state of a toggle button.				*/
/*									*/
/************************************************************************/

void appGuiSetToggleLabel(	APP_WIDGET		toggle,
				const char *		text )
    {
    XmString	labelString;

    labelString= XmStringCreateLocalized( (char *)text );
    XtVaSetValues( toggle,
			XmNlabelString,	labelString,
			NULL );

    XmStringFree( labelString );
    }

void appGuiSetToggleState(	APP_WIDGET		toggle,
				int			set )
    {
    XmToggleButtonSetState( toggle, set != 0, False );
    }

int appGuiGetToggleState(	APP_WIDGET		toggle )
    {
    return XmToggleButtonGetState( toggle ) != 0;
    }

int appGuiGetToggleStateFromCallbackMotif( void *	voidcbs )
    {
    XmToggleButtonCallbackStruct *	cbs;

    cbs= (XmToggleButtonCallbackStruct *)voidcbs;

    return cbs->set;
    }

/************************************************************************/
/*									*/
/*  Enable/Dispable a widget.						*/
/*									*/
/*  NOTE that for text edit boxes there is a different routine.		*/
/*									*/
/************************************************************************/

void appGuiEnableWidget(	APP_WIDGET		w,
				int			on_off )
    {
    XtSetSensitive( w, on_off != 0 );
    }

void appGuiSetWidgetVisible(	APP_WIDGET		w,
				int			on_off )
    {
    XtVaSetValues( w,	XmNmappedWhenManaged,	on_off != 0,
			NULL );
    }

void appShowShellWidget(		APP_WIDGET		shell )
    {
    XtRealizeWidget( shell );

    XtVaSetValues( shell, XmNinitialState, NormalState, NULL );
    XtMapWidget( shell );
    XRaiseWindow( XtDisplay( shell ), XtWindow( shell ) );
    }

void appHideShellWidget(		APP_WIDGET		shell )
    {
    if  ( XtIsRealized( shell ) )
	{ XtUnmapWidget( shell );	}
    }

void appDestroyShellWidget(		APP_WIDGET		shell )
    {
    XtDestroyWidget( shell );
    }

void appIconifyShellWidget(		APP_WIDGET		shell )
    {
    Window		win= XtWindow( shell );
    Display *		display= XtDisplay( shell );

    XtVaSetValues( shell,
		    XmNinitialState,	IconicState,
		    NULL );

    XIconifyWindow( display, win, DefaultScreen( display ) );
    }

void appGuiChangeButtonText(	APP_WIDGET	button,
				const char *	label )
    {
    appGuiChangeLabelText( button, label );
    }

void appGuiGetScrollbarValues(		int *		pValue,
					int *		pSliderSize,
					APP_WIDGET	scrollbar )
    {
    XtVaGetValues( scrollbar,
				    XmNvalue,		pValue,
				    XmNsliderSize,	pSliderSize,
				    NULL );
    return;
    }

void appGuiSetScrollbarValues(		APP_WIDGET	scrollbar,
					int		value,
					int		sliderSize )
    {
    const Boolean		notify= False;

    XmScrollBarSetValues( scrollbar, value, sliderSize, 0,0, notify );

    return;
    }

int appGuiGetScrollbarValueFromCallback( APP_WIDGET		scrollbar,
					void *			voidcbs )
    {
    XmScrollBarCallbackStruct *	cbs= (XmScrollBarCallbackStruct *)voidcbs;

    return cbs->value;
    }

void appGuiSetShellTitle(		APP_WIDGET		shell,
					const char *		fullTitle )
    {
    XtVaSetValues( shell,
			XmNtitle,		fullTitle,
			NULL );

    return;
    }

void appGuiSetIconTitle(		APP_WIDGET		shell,
					const char *		fullIconName )
    {
    XtVaSetValues( shell,
			XmNiconName,		fullIconName,
			NULL );

    return;
    }

/************************************************************************/
/*									*/
/*  Make drawing areas. Typically these are small drawings so it does	*/
/*  no harm to redraw the whole drawing area after a resize.		*/
/*									*/
/************************************************************************/

static void appGuiSendExposeForResize(	APP_WIDGET		w,
					void *			through,
					void *			voidcbs )
    {
    if  ( XtIsRealized( w ) )
	{
	XClearArea( XtDisplay( w ), XtWindow( w ), 0, 0, 0, 0, True );
	}
    }

void appGuiMakeDrawingAreaInColumn( APP_WIDGET *		pDrawing,
				    APP_WIDGET			column,
				    int				wide,
				    int				high,
				    int				heightResizable,
				    APP_EVENT_HANDLER_T		redraw,
				    void *			through )
    {
    APP_WIDGET	drawing;

    Arg		al[20];
    int		ac;

    /*
    XtSetArg( al[ac],	XmNsensitive,		False ); ac++;
    */

    ac= 0;
    if  ( high > 0 )
	{
	XtSetArg( al[ac], XmNheight,		high ); ac++;
	}

    if  ( high <= 0 || heightResizable )
	{ XtSetArg( al[ac], XmNskipAdjust,	False ); ac++;	}
    else{ XtSetArg( al[ac], XmNskipAdjust,	True ); ac++;	}

    if  ( wide > 0 )
	{
	XtSetArg( al[ac], XmNwidth,		wide ); ac++;
	}
    XtSetArg( al[ac],	XmNtraversalOn,		False ); ac++;
    XtSetArg( al[ac],	XmNnavigationType,	XmNONE ); ac++;

    drawing= XmCreateDrawingArea( column, WIDGET_NAME, al, ac );

    if  ( redraw )
	{
	XtAddEventHandler( drawing, ExposureMask, False, redraw, through );
	}

    XtAddCallback( drawing, XmNresizeCallback, appGuiSendExposeForResize,
								(void *)0 );

    XtManageChild( drawing );

    *pDrawing= drawing;
    }

/************************************************************************/
/*									*/
/*  Calculate magnification factors.					*/
/*									*/
/************************************************************************/

void appGetFactors(	EditApplication *	ea,
			double *		pHorPixPerMM,
			double *		pVerPixPerMM,
			double *		pXfac,
			double *		pYfac )
    {
    Display *		display= XtDisplay( ea->eaToplevel.atTopWidget );
    int			screen= DefaultScreen( display );

    double		horPixPerMM;
    double		verPixPerMM;
    double		xfac;
    double		yfac;

    horPixPerMM= ( (double)DisplayWidth( display, screen ) )/
					DisplayWidthMM( display, screen );
    verPixPerMM= ( (double)DisplayHeight( display, screen ) )/
					DisplayHeightMM( display, screen );

    xfac=  ( 25.4/ ( 20.0* 72.0 ) )* horPixPerMM;
    yfac=  ( 25.4/ ( 20.0* 72.0 ) )* verPixPerMM;

    *pHorPixPerMM= horPixPerMM;
    *pVerPixPerMM= verPixPerMM;

    *pXfac= ea->eaMagnification* xfac;
    *pYfac= ea->eaMagnification* yfac;

    return;
    }

/************************************************************************/
/*									*/
/*  Run a loop while the left mouse button is down.			*/
/*									*/
/************************************************************************/

typedef struct DragLoop
    {
    XtAppContext		dlContext;

    XtEventHandler		dlUpHandler;
    XtEventHandler		dlMoveHandler;
    int				dlTimerInterval;
    XtTimerCallbackProc		dlTimerHandler;
    void *			dlThrough;

    int				dlHalted;
    XtIntervalId		dlIntervalId;
    } DragLoop;

static void appDragTick(	void *		vdl,
				XtIntervalId *	xii )
    {
    DragLoop *		dl= (DragLoop *)vdl;

    (*dl->dlTimerHandler)( dl->dlThrough, xii );

    if  ( dl->dlTimerHandler && dl->dlTimerInterval > 0 )
	{
	dl->dlIntervalId= XtAppAddTimeOut( dl->dlContext,
			    dl->dlTimerInterval, appDragTick, (void *)dl );
	}
    return;
    }

static void appDragMouseUp(		APP_WIDGET		w,
					void *			vdl,
					APP_EVENT *		event,
					Boolean *		pRefused )
    {
    DragLoop *		dl= (DragLoop *)vdl;

    if  ( dl->dlUpHandler )
	{ (*dl->dlUpHandler)( w, dl->dlThrough, event, pRefused );	}

    dl->dlHalted= 1;
    }

static void appDragMouseMove(		APP_WIDGET		w,
					void *			vdl,
					APP_EVENT *		event,
					Boolean *		pRefused )
    {
    DragLoop *		dl= (DragLoop *)vdl;

    if  ( dl->dlMoveHandler )
	{ (*dl->dlMoveHandler)( w, dl->dlThrough, event, pRefused );	}
    }

void appRunDragLoop(	APP_WIDGET		w,
			EditApplication *	ea,
			APP_EVENT *		downEvent,
			APP_EVENT_HANDLER_T	upHandler,
			APP_EVENT_HANDLER_T	moveHandler,
			int			timerInterval,
			APP_TIMER_CALLBACK	timerHandler,
			void *			through )
    {
    DragLoop		dl;

    dl.dlContext= ea->eaContext;

    dl.dlUpHandler= upHandler;
    dl.dlMoveHandler= moveHandler;
    dl.dlTimerInterval= timerInterval;
    dl.dlTimerHandler= timerHandler;
    dl.dlThrough= through;

    dl.dlHalted= 0;
    dl.dlIntervalId= (XtIntervalId)0;

    if  ( dl.dlTimerHandler && dl.dlTimerInterval > 0 )
	{
	dl.dlIntervalId= XtAppAddTimeOut( dl.dlContext,
				dl.dlTimerInterval, appDragTick, (void *)&dl );
	}

    XtInsertEventHandler( w, ButtonReleaseMask, False,
				    appDragMouseUp, (void *)&dl, XtListHead );
    XtInsertEventHandler( w, Button1MotionMask, False,
				    appDragMouseMove, (void *)&dl, XtListHead );

    while( ! dl.dlHalted )
	{ XtAppProcessEvent( dl.dlContext, XtIMAll );	}

    if  ( dl.dlIntervalId )
	{ XtRemoveTimeOut( dl.dlIntervalId );	}

    XtRemoveEventHandler( w, ButtonReleaseMask, False,
					    appDragMouseUp, (void *)&dl );
    XtRemoveEventHandler( w, Button1MotionMask, False,
					    appDragMouseMove, (void *)&dl );
    return;
    }

void appGuiSetFocusChangeHandler(	APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through )
    {
    XtAddEventHandler( w, FocusChangeMask, False, handler, through );
    }

void appSetDestroyCallback(	APP_WIDGET		shell,
				APP_DESTROY_CALLBACK_T	destroyCallback,
				void *			through )
    {
    if  ( destroyCallback )
	{
	XtAddCallback( shell, XmNdestroyCallback,
						destroyCallback, through );
	}
    }

void appSetCloseCallback(	APP_WIDGET		shell,
				EditApplication *	ea,
				APP_CLOSE_CALLBACK_T	closeCallback,
				void *			through )
    {
    if  ( closeCallback && ea->eaCloseAtom > 0 )
	{
	XtVaSetValues( shell,
			    XmNdeleteResponse,		XmDO_NOTHING,
			    NULL );

	XmAddWMProtocolCallback( shell, ea->eaCloseAtom,
						    closeCallback, through );
	}
    }

#   endif
