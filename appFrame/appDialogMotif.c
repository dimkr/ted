#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"appFrame.h"
#   include	"appSystem.h"
#   include	<appGeoString.h>

#   ifdef USE_MOTIF

#   include	<Xm/Form.h>
#   include	<Xm/PanedW.h>
#   include	<Xm/MwmUtil.h>
#   include	<Xm/Protocols.h>
#   include	<Xm/DialogS.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  1)  Force keyboard focus to a modal dialog.				*/
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
static void appDialogSetFocus(		Widget			w,
					XtPointer		voidqc,
					XEvent *		event,
					Boolean *		pRefused )
    {
    if  ( event->type == MapNotify )
	{
	XSetInputFocus( XtDisplay( w ), XtWindow( w ),
					    RevertToNone, CurrentTime );

	/*
	XtRemoveEventHandler( w, StructureNotifyMask, False,
						appDialogSetFocus, voidqc );
	*/
	}

    *pRefused= 1; return;
    }

/*  2  */
static void appFixDialogSize(	Widget			w,
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
					    appFixDialogSize, through );

    *pRefused= 1;

    return;
    }

/************************************************************************/
/*									*/
/*  Make a dialog with a vertical organisation.				*/
/*									*/
/************************************************************************/

void appMakeVerticalDialog(	AppDialog *		ad,
				Widget *		pPaned,
				EditApplication *	ea,
				APP_CLOSE_CALLBACK_T	closeCallback,
				APP_DESTROY_CALLBACK_T	destroyCallback,
				void *			through,
				char *			widgetName )
    {
    Widget		shell;
    Widget		dialog;
    Widget		paned;

    Arg			al[20];
    int			ac= 0;

    MwmHints		hints;

    hints.flags=	MWM_HINTS_FUNCTIONS	|
			MWM_HINTS_DECORATIONS	;
    hints.functions=	MWM_FUNC_MOVE		|
			MWM_FUNC_MINIMIZE	;
    if  ( closeCallback )
	{ hints.functions |= MWM_FUNC_CLOSE;	}

    hints.decorations=	MWM_DECOR_BORDER	|
			MWM_DECOR_TITLE		|
			MWM_DECOR_MENU		|
			MWM_DECOR_MINIMIZE	;

    ac= 0;
    XtSetArg( al[ac], XmNdeleteResponse,	XmDO_NOTHING ); ac++;
    XtSetArg( al[ac], XmNallowShellResize,	True ); ac++;
    XtSetArg( al[ac], XmNuseAsyncGeometry,	True ); ac++;
    XtSetArg( al[ac], XmNwaitForWm,		False ); ac++; /* LessTif BUG */
    XtSetArg( al[ac], XmNwmTimeout,		0 ); ac++; /* LessTif BUG */
    if  ( hints.flags & MWM_HINTS_FUNCTIONS )
	{ XtSetArg( al[ac], XmNmwmFunctions,	hints.functions ); ac++; }
    if  ( hints.flags & MWM_HINTS_DECORATIONS )
	{ XtSetArg( al[ac], XmNmwmDecorations,	hints.decorations ); ac++; }

    shell= XmCreateDialogShell( ea->eaToplevel.atTopWidget,
							widgetName, al, ac );

    if  ( closeCallback && ea->eaCloseAtom > 0 )
	{
	XmAddWMProtocolCallback( shell, ea->eaCloseAtom,
						    closeCallback, through );
	}

    if  ( destroyCallback )
	{
	XtAddCallback( shell, XmNdestroyCallback,
						destroyCallback, through );
	}

#   if 0
    See Above
    XtAddEventHandler( shell, StructureNotifyMask, False,
						appFixDialogSize, through );
#   endif

    ac= 0;
    XtSetArg( al[ac], XmNmarginWidth,		0 ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,		0 ); ac++;
    XtSetArg( al[ac], XmNallowResize,		True ); ac++;
    XtSetArg( al[ac], XmNallowShellResize,	True ); ac++;
    /* Does not work due to motif bug:
    XtSetArg( al[ac], XmNdialogStyle,		XmDIALOG_APPLICATION_MODAL );
									ac++;
    */

    dialog= XmCreateForm( shell, WIDGET_NAME, al, ac );

    ac= 0;
    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNbottomAttachment,	XmATTACH_FORM ); ac++;

    XtSetArg( al[ac], XmNsashWidth,		1 ); ac++;
    XtSetArg( al[ac], XmNsashHeight,		1 ); ac++;
    XtSetArg( al[ac], XmNseparatorOn,		False ); ac++;
    XtSetArg( al[ac], XmNmarginWidth,		PWmargW ); ac++;
    XtSetArg( al[ac], XmNmarginHeight,		PWmargH ); ac++;
    XtSetArg( al[ac], XmNspacing,		PWspacing ); ac++;
    paned= XmCreatePanedWindow( dialog, WIDGET_NAME, al, ac );

    XtManageChild( paned );

    XtAddEventHandler( shell, StructureNotifyMask, False,
					    appDialogSetFocus, (void *)0 );

    ad->adTopWidget= shell; ad->adDialog= dialog;
    *pPaned= paned;
    return;
    }

void appGuiRunDialog(			AppDialog *		ad,
					int			initial,
					EditApplication *	ea )
    {
#   if 0
    APP_WIDGET		defaultButton= (APP_WIDGET)0;

    XtVaGetValues( qc->qcDialog.adDialog,
			XmNdefaultButton,	&defaultButton,
			NULL );

    if  ( defaultButton )
	{ XmProcessTraversal( defaultButton, XmTRAVERSE_CURRENT ); }
#   endif

    XtAddGrab( ad->adTopWidget, True, False );

    ad->adResponse= initial;
    ad->adContinue= 1;

    while( ad->adContinue			&&
	   XtIsRealized( ad->adTopWidget )	)
	{
	XtAppProcessEvent( ea->eaContext, XtIMAll );
	}

    XtRemoveGrab( ad->adTopWidget );

    return;
    }

void appGuiBreakDialog(			AppDialog *		ad,
					int			response )
    {
    ad->adResponse= response;
    ad->adContinue= 0;
    }

void appGuiSetDefaultButtonForDialog(	AppDialog *		ad,
					APP_WIDGET		button )
    {
    XtVaSetValues( ad->adDialog,
			XmNdefaultButton,	button,
			NULL );
    }

void appGuiSetCancelButtonForDialog(	AppDialog *		ad,
					APP_WIDGET		button )
    {
    XtVaSetValues( ad->adDialog,
			XmNcancelButton,	button,
			NULL );
    }

static void appDialogRelative(	Widget		relative,
				Widget		dialog )
    {
    Dimension		x;
    Dimension		y;
    Dimension		width;
    Dimension		height;

    XtVaGetValues( relative,
			    XmNx,	&x,
			    XmNy,	&y,
			    XmNwidth,	&width,
			    XmNheight,	&height,
			    NULL );

    XtVaSetValues( dialog,
			    XmNx,		x+ width/5,
			    XmNy,		y+ height/ 5,
			    NULL );

    XtVaSetValues( XtParent( dialog ),
			    XmNtransientFor,	relative,
			    XmNx,		x+ width/5,
			    XmNy,		y+ height/ 5,
			    NULL );

    }

static void appRelativeMapCallback(	Widget		w,
					XtPointer	voidRelative,
					XtPointer	voidcbs )
    {
    Widget		relative= (Widget)voidRelative;

    appDialogRelative( relative, w );

    XtRemoveCallback( w, XmNmapCallback, appRelativeMapCallback, voidRelative );
    }

void appGuiShowDialog(			AppDialog *		ad,
					Widget			relative )
    {
    if  ( relative )
	{
	XtAddCallback( ad->adDialog, XmNmapCallback,
				appRelativeMapCallback, (void *)relative );
	}

    XtManageChild( ad->adDialog );

    if  ( relative )
	{ appDialogRelative( relative, ad->adDialog );	}
    }

void appGuiHideDialog(			AppDialog *		ad )
    {
    XtUnmanageChild( ad->adDialog );

    XtVaSetValues( ad->adTopWidget,
			    XmNtransientFor,	(Widget)0,
			    NULL );
    }

#   endif
