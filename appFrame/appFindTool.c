/************************************************************************/
/*									*/
/*  A find/replace tool.						*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>

#   include	"appFrame.h"

#   ifdef USE_MOTIF
#   include	<X11/keysym.h>
#   endif

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Represents a find tool.						*/
/*									*/
/************************************************************************/

#   define	FILEL	400

typedef struct AppFindToolResources
    {
    char *	aftrFindTitle;
    char *	aftrFindNext;

    char *	aftrUseRegex;

    char *	aftrFindPrevious;

    char *	aftrReplaceTitle;
    char *	aftrReplaceFound;
    char *	aftrReplaceNext;

    char *	aftrLower;
    char *	aftrClose;
    } AppFindToolResources;

typedef struct AppFindTool
    {
    APP_WIDGET			aftTopWidget;
    APP_WIDGET			aftMainWidget;

    APP_WIDGET			aftPatternText;
    APP_WIDGET			aftReplaceText;
    APP_WIDGET			aftReplaceFrame;

    APP_WIDGET			aftReplaceButton;
    APP_WIDGET			aftReplaceNextButton;
    APP_WIDGET			aftFindNextButton;
    APP_WIDGET			aftFindPrevButton;

    APP_WIDGET			aftRegexToggle;

    APP_WIDGET			aftLowerButton;
    APP_WIDGET			aftCloseButton;

    AppToolDestroy		aftDestroy;
    FindToolFind		aftFindNext;
    FindToolFind		aftFindPrev;
    FindToolReplace		aftReplace;
    FindToolSetPattern		aftSetPattern;

    void *			aftTarget;

    int				aftVisible;
    int				aftLastFindResult;
    int				aftUseRegex;
    } AppFindTool;

/************************************************************************/
/*									*/
/*  Turn buttons on and off depending on the situation.			*/
/*									*/
/************************************************************************/

static void appFindToolReflectPatternText(	AppFindTool *	aft )
    {
    int		sensitive;
    char *	pattern;

    pattern= appGetStringFromTextWidget( aft->aftPatternText );

    sensitive= pattern[0] != '\0';

    appFreeStringFromTextWidget( pattern );

    appGuiEnableWidget( aft->aftFindNextButton, sensitive );
    appGuiEnableWidget( aft->aftFindPrevButton, sensitive );

    return;
    }

static void appFindToolReflectReplaceText(	AppFindTool *	aft )
    {
    int		sensitive;
    char *	replacement;

    replacement= appGetStringFromTextWidget( aft->aftReplaceText );

    sensitive= replacement[0] != '\0' && aft->aftLastFindResult == 0;

    appFreeStringFromTextWidget( replacement );

    appGuiEnableWidget( aft->aftReplaceButton, sensitive );
    appGuiEnableWidget( aft->aftReplaceNextButton, sensitive );

    return;
    }

static void appFindToolSetFindResult(	AppFindTool *	aft,
					int		result )
    {
    int		sensitive;

    aft->aftLastFindResult= result;

    if  ( result )
	{ sensitive= 0;	}
    else{
	char *	replacement;

	appGuiEnableWidget( aft->aftReplaceFrame, 1 );

	replacement= appGetStringFromTextWidget( aft->aftReplaceText );

	sensitive= replacement[0] != '\0';

	appFreeStringFromTextWidget( replacement );
	}

    appGuiEnableWidget( aft->aftReplaceButton, sensitive );
    appGuiEnableWidget( aft->aftReplaceNextButton, sensitive );

    return;
    }

static APP_TXTYPING_CALLBACK_H( appFindToolPatternChanged, w, voidaft )
    {
    AppFindTool *	aft= (AppFindTool *)voidaft;

    appFindToolReflectPatternText( aft );

    return;
    }

static APP_TXTYPING_CALLBACK_H( appFindToolReplacementChanged, w, voidaft )
    {
    AppFindTool *	aft= (AppFindTool *)voidaft;

    appFindToolReflectReplaceText( aft );

    return;
    }

/************************************************************************/
/*									*/
/*  'Find' button has been pushed.					*/
/*									*/
/************************************************************************/

static int appFindToolSetProgram(	AppFindTool *	aft )
    {
    char *		pattern;
    int			res;

    if  ( ! aft->aftSetPattern )
	{ XDEB(aft->aftSetPattern); return -1;	}

    pattern= appGetStringFromTextWidget( aft->aftPatternText );

    res= (*aft->aftSetPattern)( aft->aftTarget,
				(unsigned char *)pattern, aft->aftUseRegex );

    appFreeStringFromTextWidget( pattern );

    return res;
    }

static void appFindToolFindNext(	AppFindTool *	aft )
    {
    int			result;

    if  ( ! aft->aftFindNext )
	{
	XDEB(aft->aftFindNext);
	appFindToolSetFindResult( aft, -1 ); return;
	}

    if  ( appFindToolSetProgram( aft ) )
	{ appFindToolSetFindResult( aft, -1 ); return; }

    result= (*aft->aftFindNext)( aft->aftTarget );
    appFindToolSetFindResult( aft, result );

    return;
    }

static APP_BUTTON_CALLBACK_H( appFindToolFindNextPushed, w, voidaft )
    {
    AppFindTool *	aft= (AppFindTool *)voidaft;

    appFindToolFindNext( aft );

    return;
    }

static APP_BUTTON_CALLBACK_H( appFindToolFindPrev, w, voidaft )
    {
    AppFindTool *	aft= (AppFindTool *)voidaft;
    int			result;

    if  ( ! aft->aftFindPrev )
	{
	XDEB(aft->aftFindPrev);
	appFindToolSetFindResult( aft, -1 ); return;
	}

    if  ( appFindToolSetProgram( aft ) )
	{ appFindToolSetFindResult( aft, -1 ); return; }

    result= (*aft->aftFindPrev)( aft->aftTarget );
    appFindToolSetFindResult( aft, result );

    return;
    }

/************************************************************************/
/*									*/
/*  'Replace' button has been pushed.					*/
/*									*/
/************************************************************************/

static void appFindToolReplaceSelection( AppFindTool *	aft )
    {
    char *		replacement;

    if  ( ! aft->aftReplace )
	{ XDEB(aft->aftReplace); return;	}

    replacement= appGetStringFromTextWidget( aft->aftReplaceText );

    (*aft->aftReplace)( aft->aftTarget, (const unsigned char *)replacement );

    appFreeStringFromTextWidget( replacement );

    appGuiEnableWidget( aft->aftReplaceButton, 0 );

    return;
    }

static APP_BUTTON_CALLBACK_H( appFindToolReplaceSelectionPushed, w, voidaft )
    {
    AppFindTool *	aft= (AppFindTool *)voidaft;

    appFindToolReplaceSelection( aft );
    }

static APP_BUTTON_CALLBACK_H( appFindToolReplaceNext, w, voidaft )
    {
    AppFindTool *	aft= (AppFindTool *)voidaft;

    appFindToolReplaceSelection( aft );

    appFindToolFindNext( aft );

    return;
    }

static APP_TOGGLE_CALLBACK_H( appFindRegexToggled, w, voidaft, voidtbcs )
    {
    AppFindTool *	aft= (AppFindTool *)voidaft;
    int			set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    aft->aftUseRegex= ( set != 0 );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a row of buttons for the Find Tool.				*/
/*									*/
/************************************************************************/

static void appFindToolMakeButtonRow(	APP_WIDGET *		pRow,
					APP_WIDGET		parent,
					APP_WIDGET *		pLeftButton,
					APP_WIDGET *		pRightButton,
					char *			leftLabel,
					char *			rightLabel,
					APP_BUTTON_CALLBACK_T	leftCallback,
					APP_BUTTON_CALLBACK_T	rightCallback,
					AppFindTool *		aft )
    {
    APP_WIDGET		row;

    APP_WIDGET		leftButton;
    APP_WIDGET		rightButton;

    const int		heightResizable= 0;
    const int		showAsDefault= 0;

    row= appMakeRowInColumn( parent, 2, heightResizable );

    appMakeButtonInRow( &(leftButton), row, leftLabel,
			    leftCallback, (void *)aft, 0, showAsDefault );
    appMakeButtonInRow( &(rightButton), row, rightLabel,
			    rightCallback, (void *)aft, 1, showAsDefault );

#   ifdef USE_MOTIF
    XtVaSetValues( XtParent( parent ),
			XmNdefaultButton,	leftButton,
			NULL );
#   endif


    *pLeftButton= leftButton, *pRightButton= rightButton; return;
    }

/************************************************************************/
/*									*/
/*  Make the button part of the find Tool.				*/
/*									*/
/************************************************************************/

static APP_WIDGET appFindMakeFindFrame(	APP_WIDGET		parent,
					AppFindToolResources *	aftr,
					AppFindTool *		aft 	)
    {
    APP_WIDGET		frame;
    APP_WIDGET		paned;
    APP_WIDGET		row;

    const int		textEnabled= 1;
    const int		heightResizable= 0;

    appMakeColumnFrameInColumn( &frame, &paned, parent, aftr->aftrFindTitle );

    appMakeTextInColumn( &(aft->aftPatternText), paned, 0, textEnabled );

    appGuiSetTypingCallbackForText( aft->aftPatternText,
				    appFindToolPatternChanged, (void *)aft );

#   ifdef USE_MOTIF
    XtVaSetValues( XtParent( paned ),
			XmNinitialFocus,	aft->aftPatternText,
			NULL );
#   endif

    /***************/

    row= appMakeRowInColumn( paned, 1, heightResizable );
    aft->aftRegexToggle=
		    appMakeToggleInRow( row, aftr->aftrUseRegex,
					appFindRegexToggled, (void *)aft, 0 );

    /***************/

    appFindToolMakeButtonRow( &row, paned,
		&(aft->aftFindNextButton), &(aft->aftFindPrevButton),
		aftr->aftrFindNext, aftr->aftrFindPrevious,
		appFindToolFindNextPushed, appFindToolFindPrev,
		aft );

    /***************/

    return frame;
    }

/************************************************************************/
/*									*/
/*  Make the replace part of the find tool.				*/
/*									*/
/************************************************************************/

static APP_WIDGET appFindMakeReplaceFrame( APP_WIDGET		parent,
					AppFindToolResources *	aftr,
					AppFindTool *		aft )
    {
    APP_WIDGET		row;
    APP_WIDGET		frame;
    APP_WIDGET		paned;

    const int		textEnabled= 1;

    appMakeColumnFrameInColumn( &frame, &paned,
					    parent, aftr->aftrReplaceTitle );
    aft->aftReplaceFrame= frame;

    appMakeTextInColumn( &(aft->aftReplaceText), paned, 0, textEnabled );

    appGuiSetTypingCallbackForText( aft->aftReplaceText,
				appFindToolReplacementChanged, (void *)aft );

#   ifdef USE_MOTIF
    XtVaSetValues( XtParent( paned ),
			XmNinitialFocus,	aft->aftReplaceText,
			NULL );
#   endif

    /***************/

    appFindToolMakeButtonRow( &row, paned,
		&(aft->aftReplaceButton), &(aft->aftReplaceNextButton),
		aftr->aftrReplaceFound, aftr->aftrReplaceNext,
		appFindToolReplaceSelectionPushed, appFindToolReplaceNext,
		aft );

    /***************/

    return frame;
    }

/************************************************************************/
/*  A find tool must be destroyed.					*/
/************************************************************************/

static void appDestroyFindTool(	AppFindTool *	aft )
    {
    if  ( aft->aftDestroy )
	{ (*aft->aftDestroy)( aft->aftTarget );	}

    appDestroyShellWidget( aft->aftTopWidget );

    free( aft );
    }

static APP_CLOSE_CALLBACK_H( appFindToolCloseCallback, w, voidaft )
    {
    AppFindTool *	aft= (AppFindTool *)voidaft;

    appDestroyFindTool( aft );

    return;
    }

static APP_BUTTON_CALLBACK_H( appLowerFindTool, w, voidaft )
    {
    AppFindTool *	aft= (AppFindTool *)voidaft;
    APP_WIDGET		shell= aft->aftTopWidget;

    appGuiLowerShellWidget( shell );

    return;
    }

static APP_BUTTON_CALLBACK_H( appCloseFindTool, w, voidaft )
    {
    AppFindTool *	aft= (AppFindTool *)voidaft;

    appDestroyFindTool( aft );

    return;
    }

#   ifdef USE_MOTIF
static void appFindToolKey(		APP_WIDGET		w,
					void *			voidaft,
					XEvent *		event,
					Boolean *		pRefused )
    {
    if  ( event->type == KeyRelease )
	{
	XKeyEvent *		kevent= &(event->xkey);
	AppFindTool *		aft= (AppFindTool *)voidaft;

	if  ( XKeysymToKeycode( XtDisplay(w), XK_Escape ) == kevent->keycode )
	    { appDestroyFindTool( aft ); *pRefused= 0; return; }

	if  ( XKeysymToKeycode( XtDisplay(w), XK_F3 ) == kevent->keycode )
	    { appFindToolFindNext( aft ); *pRefused= 0; return; }
	}

    *pRefused= 1; return;
    }
#   endif

/************************************************************************/
/*									*/
/*  make a find tool.							*/
/*									*/
/************************************************************************/

#   ifdef USE_MOTIF
static void appFindToolConfigure(	APP_WIDGET		w,
					void *			voidaft,
					XEvent *		event,
					Boolean *		pRefused )
    {
    XConfigureEvent *		cevent= &(event->xconfigure);

    if  ( cevent->type != ConfigureNotify )
	{ return;	}

    XtVaSetValues( w,	XmNminHeight,	cevent->height,
			XmNmaxHeight,	cevent->height,
			XmNminWidth,	cevent->width,
			NULL );

    XtRemoveEventHandler( w, StructureNotifyMask, False,
					    appFindToolConfigure, voidaft );

    *pRefused= 1;
    return;
    }
#   endif

#   ifdef USE_MOTIF
static void appFindToolVisible(		APP_WIDGET		w,
					void *			voidaft,
					XEvent *		event,
					Boolean *		pRefused )
    {
    AppFindTool *	aft= (AppFindTool *)voidaft;

    if  ( event->type == MapNotify )
	{
	XSetInputFocus( XtDisplay( w ), XtWindow( w ),
						RevertToNone, CurrentTime );
	aft->aftVisible= 1;
	}

    if  ( event->type == UnmapNotify )
	{ aft->aftVisible= 0; }

    *pRefused= 1;
    return;
    }
#   endif

static AppConfigurableResource APP_FindToolResourceTable[]=
    {
	APP_RESOURCE( "findToolFindTitle",
		    offsetof(AppFindToolResources,aftrFindTitle),
		    "Find" ),

	APP_RESOURCE( "findToolUseRegex",
		    offsetof(AppFindToolResources,aftrUseRegex),
		    "Find Regular Expression" ),

	APP_RESOURCE( "findToolFindNext",
		    offsetof(AppFindToolResources,aftrFindNext),
		    "Find" ),
	APP_RESOURCE( "findToolFindPrevious",
		    offsetof(AppFindToolResources,aftrFindPrevious),
		    "Previous" ),

	APP_RESOURCE( "findToolReplaceTitle",
		    offsetof(AppFindToolResources,aftrReplaceTitle),
		    "Replace" ),

	APP_RESOURCE( "findToolReplaceFound",
		    offsetof(AppFindToolResources,aftrReplaceFound),
		    "Replace" ),
	APP_RESOURCE( "findToolReplaceNext",
		    offsetof(AppFindToolResources,aftrReplaceNext),
		    "Replace, Next" ),

	APP_RESOURCE( "findToolLower",
		    offsetof(AppFindToolResources,aftrLower),
		    "Lower" ),
	APP_RESOURCE( "findToolClose",
		    offsetof(AppFindToolResources,aftrClose),
		    "Close" ),
    };

void * appMakeFindTool(		APP_WIDGET		findOption,
				EditApplication *	ea,
				const char *		widgetName,
				APP_BITMAP_IMAGE	iconPixmap,
				APP_BITMAP_MASK		iconMask,
				AppToolDestroy		destroy,
				FindToolFind		findNext,
				FindToolFind		findPrev,
				FindToolReplace		replace,
				FindToolSetPattern	setPattern,
				void *			target )
    {
    AppFindTool *	aft;
    
    APP_WIDGET		findFrame;
    APP_WIDGET		replaceFrame;
    APP_WIDGET		buttonRow;

    const int		userResizable= 1;
    
    static AppFindToolResources	aftr;
    static int			gotResources;

    if  ( ! gotResources )
	{
	appGuiGetResourceValues( &gotResources, ea, (void *)&aftr,
					APP_FindToolResourceTable,
					sizeof(APP_FindToolResourceTable)/
					sizeof(AppConfigurableResource) );

	gotResources= 1;
	}

    aft= (AppFindTool *)malloc( sizeof(AppFindTool) );
    if  ( ! aft )
	{ XDEB(aft); return (void *)0;	}

    aft->aftDestroy= destroy;
    aft->aftFindNext= findNext;
    aft->aftFindPrev= findPrev;
    aft->aftSetPattern= setPattern;
    aft->aftReplace= replace;
    aft->aftTarget= target;
    aft->aftVisible= 0;
    aft->aftUseRegex= 0;
    
    appMakeVerticalTool( &(aft->aftTopWidget), &(aft->aftMainWidget), ea,
			    iconPixmap, iconMask,
			    widgetName, userResizable, findOption,
			    appFindToolCloseCallback, (void *)aft );

#   ifdef USE_MOTIF
    XtVaSetValues(	aft->aftTopWidget,
				XmNkeyboardFocusPolicy,	XmEXPLICIT,
				NULL );

    XtAddEventHandler( aft->aftTopWidget, StructureNotifyMask, False,
					appFindToolConfigure, (void *)aft );
    XtAddEventHandler( aft->aftTopWidget, StructureNotifyMask, False,
					appFindToolVisible, (void *)aft );
#   endif

    findFrame= appFindMakeFindFrame( aft->aftMainWidget, &aftr, aft );

    replaceFrame= appFindMakeReplaceFrame( aft->aftMainWidget,
							&aftr, aft );

    appFindToolMakeButtonRow( &buttonRow, aft->aftMainWidget,
		&(aft->aftLowerButton), &(aft->aftCloseButton),
		aftr.aftrLower, aftr.aftrClose,
		appLowerFindTool, appCloseFindTool,
		aft );

#   ifdef USE_MOTIF
    XtInsertEventHandler( aft->aftPatternText, KeyReleaseMask, False,
				appFindToolKey, (void *)aft, XtListHead );
    XtInsertEventHandler( aft->aftReplaceText, KeyReleaseMask, False,
				appFindToolKey, (void *)aft, XtListHead );

    XtInsertEventHandler( aft->aftReplaceButton, KeyReleaseMask, False,
				appFindToolKey, (void *)aft, XtListHead );
    XtInsertEventHandler( aft->aftReplaceNextButton, KeyReleaseMask, False,
				appFindToolKey, (void *)aft, XtListHead );

    XtInsertEventHandler( aft->aftFindNextButton, KeyReleaseMask, False,
				appFindToolKey, (void *)aft, XtListHead );
    XtInsertEventHandler( aft->aftFindPrevButton, KeyReleaseMask, False,
				appFindToolKey, (void *)aft, XtListHead );
#   endif

    return (void *)aft;
    }

/************************************************************************/
/*									*/
/*  Draw a find tool to front.						*/
/*									*/
/************************************************************************/

void appShowFindTool(		APP_WIDGET	relative,
				void *		voidaft	)
    {
    AppFindTool *		aft= (AppFindTool *)voidaft;

    if  ( ! aft->aftVisible )
	{
#	ifdef USE_MOTIF
	Dimension			x;
	Dimension			y;
	Dimension			width;
	Dimension			height;

	XtVaGetValues( relative,
			    XmNx,		&x,
			    XmNy,		&y,
			    XmNwidth,		&width,
			    XmNheight,		&height,
			    NULL );

	XtVaSetValues( aft->aftTopWidget,
			    XmNx,		x+ width/10,
			    XmNy,		y+ height/10,
			    NULL );
#	endif

	appShowShellWidget( aft->aftTopWidget );
	}
    else{
	appShowShellWidget( aft->aftTopWidget );

#	ifdef USE_MOTIF
	{
	Window		win=  XtWindow( aft->aftTopWidget );
	Display *	display= XtDisplay( aft->aftTopWidget );

	XSetInputFocus( display, win, RevertToNone, CurrentTime );
	}
#	endif

	}

    appFindToolSetFindResult( aft, -1 );
    appFindToolReflectPatternText( aft );
    appFindToolReflectReplaceText( aft );

#   ifdef USE_MOTIF
    XmProcessTraversal( aft->aftPatternText, XmTRAVERSE_CURRENT );
#   endif
    }

void appEnableFindTool(		void *	voidaft,
				int	enabled )
    {
    AppFindTool *	aft= (AppFindTool *)voidaft;

    appGuiEnableWidget( aft->aftMainWidget, enabled != 0 );

    return;
    }

void appFindToolEnableReplace(	void *	voidaft,
				int	enabled )
    {
    AppFindTool *	aft= (AppFindTool *)voidaft;

    appGuiEnableWidget( aft->aftReplaceFrame, enabled );

    return;
    }

void appFindToolSetPattern(	void *			voidaft,
				const unsigned char *	pattern,
				int			useRegex )
    {
    AppFindTool *	aft= (AppFindTool *)voidaft;

    aft->aftUseRegex= useRegex;

    appStringToTextWidget( aft->aftPatternText, (const char *)pattern );

    appGuiSetToggleState( aft->aftRegexToggle, aft->aftUseRegex );

    appFindToolReflectPatternText( aft );

    return;
    }

