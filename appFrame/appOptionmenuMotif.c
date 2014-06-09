#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"appFrame.h"
#   include	"appSystem.h"
#   include	<appGeoString.h>

#   ifdef USE_MOTIF

#   include	<Xm/RowColumn.h>
#   include	<Xm/PushB.h>
#   include	<Xm/PushBG.h>
#   include	<Xm/PanedW.h>

#   include	<appDebugon.h>

#   define	RECLAIM_FOCUS	0

/************************************************************************/
/*									*/
/*  Get around Motif's ridiculous resize behavior.			*/
/*									*/
/************************************************************************/

void appOptionmenuSetWidthMotif(	Widget		menu,
					int		newWidth )
    {
    Dimension		parentMarginWidth= 0;

    Widget		pulldown;
    Widget		buttonGadget;

    XtWidgetGeometry	xwg;

    /*
    SDEB(XtClass(menu)->core_class.class_name);
    */

    XtVaGetValues( menu,
			XmNsubMenuId,		&pulldown,
			NULL );

    XtVaGetValues( XtParent( pulldown ),
			XmNmarginWidth,		&parentMarginWidth,
			NULL );

    if  ( newWidth < 2* parentMarginWidth )
	{ LLDEB(newWidth,parentMarginWidth); return;	}

    XtVaSetValues( pulldown,
			XmNwidth,		newWidth-
						2* parentMarginWidth,
			NULL );

    buttonGadget= XmOptionButtonGadget( menu );

    XtQueryGeometry( buttonGadget, NULL, &xwg );

    XtResizeWidget( buttonGadget, newWidth, xwg.height, xwg.border_width );

    return;
    }

void appOptionmenuRefreshWidth(	AppOptionmenu *		aom )
    {
    Dimension		width;

    XtVaGetValues( aom->aomInplace,
			    XmNwidth,		&width,
			    NULL );

    appOptionmenuSetWidthMotif( aom->aomInplace, width );

    return;
    }

/************************************************************************/
/*									*/
/*  Reclaim focus that drifted away.					*/
/*  This is to work around a bug in the Xfce window manager (Oct 2004).	*/
/*									*/
/************************************************************************/

# if RECLAIM_FOCUS

static void appRefocusOptionmenuMotif(	Widget			w,
					void *			voidaom,
					void *			pbcs )
    {
    AppOptionmenu *	aom= (AppOptionmenu *)voidaom;

    if  ( XtDisplay( aom->aomInplace ) )
	{
	XSetInputFocus( XtDisplay( aom->aomInplace ),
						XtWindow( aom->aomInplace ),
						RevertToNone, CurrentTime );
	}
    }

# endif

/************************************************************************/
/*									*/
/*  Make a pulldown that is usable as a child of a form.		*/
/*									*/
/************************************************************************/

static void appOptionMenuConfigureMotif(	Widget		w,
						void *		through,
						XEvent *	event,
						Boolean *	pRefused )
    {
    XConfigureEvent *		cevent= &(event->xconfigure);

    if  ( cevent->type != ConfigureNotify )
	{ return;	}

    appOptionmenuSetWidthMotif( w, cevent->width );

    *pRefused= 1;

    return;
    }

void appFinishOptionmenuMotif(	Widget		menu,
				Widget		pulldown )
    {
    Widget			labelGadget;
    Widget			buttonGadget;

    XtAddEventHandler( menu, StructureNotifyMask, False,
				    appOptionMenuConfigureMotif, (void *)0 );

    labelGadget= XmOptionLabelGadget( menu );
    buttonGadget= XmOptionButtonGadget( menu );

    XtUnmanageChild( labelGadget );
    XtManageChild( buttonGadget );

    XtVaSetValues( buttonGadget,
			XmNalignment,		XmALIGNMENT_BEGINNING,
			XmNmarginHeight,	1,
			NULL );

    return;
    }

void appMakeOptionmenuInColumn(		AppOptionmenu *	aom,
					Widget		column )
    {
    Widget			pulldown;
    Widget			menu;

    Arg				al[20];
    int				ac= 0;

    appInitOptionmenu( aom );

    ac= 0;
    /*
    No! is counter productive..
    XtSetArg( al[ac], XmNresizeWidth,		False ); ac++;
    */
    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    /*
    Does not make any difference..
    XtSetArg( al[ac], XmNallowShellResize,	False ); ac++;
    */

    pulldown= XmCreatePulldownMenu( column, WIDGET_NAME, al, ac );

    ac= 0;
    XtSetArg( al[ac], XmNsubMenuId,		pulldown ); ac++;

    XtSetArg( al[ac], XmNmarginHeight,		0 ); ac++;
    XtSetArg( al[ac], XmNmarginWidth,		0 ); ac++;
    XtSetArg( al[ac], XmNspacing,		0 ); ac++;
    XtSetArg( al[ac], XmNentryBorder,		0 ); ac++;
    XtSetArg( al[ac], XmNskipAdjust,		True ); ac++;
    XtSetArg( al[ac], XmNallowResize,		True ); ac++;
    XtSetArg( al[ac], XmNresizeHeight,		True ); ac++;

    menu= XmCreateOptionMenu( column, WIDGET_NAME, al, ac );

    appFinishOptionmenuMotif( menu, pulldown );

    XtManageChild( menu );

    appMotifTurnOfSashTraversal( column );

    aom->aomPulldown= pulldown; aom->aomInplace= menu; return;
    }

void appMakeOptionmenuInRow(		AppOptionmenu *	aom,
					Widget		row,
					int		column,
					int		colspan )
    {
    Widget			pulldown;
    Widget			menu;

    Arg				al[20];
    int				ac= 0;

    appInitOptionmenu( aom );

    ac= 0;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNtopOffset,		0 ); ac++;

    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNleftPosition,		column ); ac++;

    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNrightPosition,		column+ colspan ); ac++;

    /*
    No! is counter productive..
    XtSetArg( al[ac], XmNresizeWidth,		False ); ac++;
    */

    pulldown= XmCreatePulldownMenu( row, WIDGET_NAME, al, ac );

    ac= 0;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNtopOffset,		0 ); ac++;

    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNleftPosition,		column ); ac++;

    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_POSITION ); ac++;
    XtSetArg( al[ac], XmNrightPosition,		column+ colspan ); ac++;

    XtSetArg( al[ac], XmNsubMenuId,		pulldown ); ac++;

    XtSetArg( al[ac], XmNmarginHeight,		0 ); ac++;
    XtSetArg( al[ac], XmNmarginWidth,		0 ); ac++;
    XtSetArg( al[ac], XmNspacing,		0 ); ac++;
    XtSetArg( al[ac], XmNentryBorder,		0 ); ac++;

    menu= XmCreateOptionMenu( row, WIDGET_NAME, al, ac );

    appFinishOptionmenuMotif( menu, pulldown );

    XtManageChild( menu );

    aom->aomPulldown= pulldown; aom->aomInplace= menu; return;
    }

/************************************************************************/
/*									*/
/*  Add an option to a pulldown.					*/
/*									*/
/************************************************************************/

APP_WIDGET appAddItemToOptionmenu(	AppOptionmenu *		aom,
					const char *		label,
					XtCallbackProc		callBack,
					void *			target )
    {
    Widget		fresh;

    WidgetList		children;
    Cardinal		childCount= 0;

    Arg			al[20];
    int			ac= 0;

    XtVaGetValues( aom->aomPulldown,
		    XmNchildren,	&children,
		    XmNnumChildren,	&childCount,
		    NULL );

    XtSetArg( al[ac], XmNmarginHeight,		1 ); ac++;

    if  ( aom->aomOptionsVisible >= childCount )
	{
	fresh= XmCreatePushButtonGadget( aom->aomPulldown, (char *)label,
						    al,	ac );
	}
    else{
	XmString	labelString;

	fresh= children[aom->aomOptionsVisible];

	labelString= XmStringCreateLocalized( (char *)label );

	XtVaSetValues( fresh,
			XmNlabelString,	labelString,
			NULL );

	XmStringFree( labelString );
	}

    if  ( callBack )
	{ XtAddCallback( fresh, XmNactivateCallback, callBack, target ); }

#   if RECLAIM_FOCUS
    XtAddCallback( fresh, XmNactivateCallback,
				    appRefocusOptionmenuMotif, (void *)aom );
#   endif

    XtManageChild( fresh );

    aom->aomOptionsVisible++;

    return fresh;
    }

/************************************************************************/
/*									*/
/*  Select a particular option in an option menu.			*/
/*									*/
/************************************************************************/

void appSetOptionmenu(	AppOptionmenu *	aom,
			int		num )
    {
    WidgetList		children;
    Cardinal		childCount= 0;

    XtVaGetValues( aom->aomPulldown,
		    XmNchildren,	&children,
		    XmNnumChildren,	&childCount,
		    NULL );

    if  ( num >= 0 && num < (int)childCount )
	{
	XtVaSetValues( aom->aomInplace,
			XmNmenuHistory,	children[num],
			NULL );
	}
    else{
	XtVaSetValues( aom->aomInplace,
			XmNmenuHistory,	(Widget)0,
			NULL );
	}

    return;
    }

void appEmptyOptionmenu(	AppOptionmenu *		aom )
    {
    WidgetList		children;
    Cardinal		childCount;
    int			child;

    XtVaSetValues( aom->aomInplace,
		    XmNmenuHistory,	(Widget)0,
		    NULL );

    /*  Does not work!
    appEmptyParentWidget( aom->aomPulldown );
    */

    XtVaGetValues( aom->aomPulldown,
			XmNchildren,		&children,
			XmNnumChildren,		&childCount,
			NULL );

    if  ( childCount == 0 )
	{ return;	}

    if  ( aom->aomOptionsVisible > childCount )
	{
	LLDEB(aom->aomOptionsVisible,childCount);
	aom->aomOptionsVisible= childCount;
	}

    for ( child= 0; child < aom->aomOptionsVisible; child++ )
	{
	XtRemoveAllCallbacks( children[child], XmNactivateCallback );

#	if 0
	Count callbacks to make sure that only one exists:

	XtCallbackList	callBacks= (XtCallbackList)0;

	XtVaGetValues( children[child],
			XmNactivateCallback,	&callBacks,
			NULL );
	if  ( callBacks )
	    {
	    int		n= 0;

	    while( callBacks[n].callback && callBacks[n].closure )
		{ n++;	}
	    LLDEB(child,n);
	    }
#	endif
	}

    /*  1  */
    XtUnmanageChildren( children, childCount );
    aom->aomOptionsVisible= 0;
    }

void appGuiEnableOptionmenu(	AppOptionmenu *		aom,
				int			sensitive )
    {
    XtSetSensitive( aom->aomInplace, sensitive != 0 );
    }

void appInitOptionmenu(		AppOptionmenu *		aom )
    {
    aom->aomPulldown= (APP_WIDGET)0;
    aom->aomInplace= (APP_WIDGET)0;

    aom->aomOptionsVisible= 0;
    }

int appGuiGetOptionmenuItemIndexMotif(	AppOptionmenu *	aom,
					Widget		w )
    {
    short		pos= -1;

    XtVaGetValues( w,	XmNpositionIndex,	&pos,
			NULL );
    return pos;
    }

void appOptionmenuItemSetVisibility(	APP_WIDGET	w,
					int		visible )
    {
    visible= ( visible != 0 );

    if  ( XtIsManaged( w ) == visible )
	{ return;	}

    if  ( visible )
	{ XtManageChild( w );	}
    else{ XtUnmanageChild( w );	}

    return;
    }

#   endif
