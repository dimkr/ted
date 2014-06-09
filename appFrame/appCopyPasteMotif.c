/************************************************************************/
/*									*/
/*  Application framework: Copy/Paste logic. Motif/X11 specific code.	*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	<appFrame.h>

#   include	<appDebugon.h>

#   ifdef	USE_MOTIF

/************************************************************************/
/*									*/
/*  Respond to selection events.					*/
/*									*/
/************************************************************************/
# define ADEB(ev,a) SDEB(((a)==None?"None":XGetAtomName((ev)->display,(a))))

static Atom	XA_MULTIPLE= None;
static Atom	XA_TARGETS= None;

static int appGetResponseType(	AppSelectionType **		pAst,
				AppSelectionTargetType **	pAstt,
				int *				pTtargetFound,
				AppSelectionType *		ast,
				int				astCount,
				Atom				selection,
				Atom				target )
    {
    int				i;

    AppSelectionTargetType *	astt;

    for ( i= 0; i < astCount; ast++, i++ )
	{
	if  ( ast->astSelectionAtom == selection )
	    { break;	}
	}

    if  ( i >= astCount )
	{ return -1; }

    astt= ast->astTargetTypes;
    for ( i= 0; i < ast->astTargetTypeCount; astt++, i++ )
	{
	if  ( astt->asttTargetAtom == target )
	    { break;	}
	}

    if  ( i >= ast->astTargetTypeCount )
	{ return -1; }

    *pAst= ast; *pTtargetFound= i; *pAstt= astt; return 0;
    }

/************************************************************************/
/*									*/
/*  You got the paste you asked for.					*/
/*									*/
/************************************************************************/

static void appDocGotPasteReply(	Widget			w,
					void *			voided,
					XEvent *		event,
					Boolean *		pRefused )
    {
    XSelectionEvent *		selEvent= (XSelectionEvent *)event;
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;

    AppSelectionType *		ast;
    AppSelectionTargetType *	astt;
    int				targetFound;

    Display *			display= XtDisplay( w );

    /*
    appDebug( "PASTE " );
    appDebug( "selection= %s target= %s, property= %s\n",
			selEvent->selection == None ? "None" :
			    XGetAtomName( display, selEvent->selection ),
			selEvent->target == None ? "None" :
			    XGetAtomName( display, selEvent->target ),
			selEvent->property == None ? "None" :
			    XGetAtomName( display, selEvent->property ) );
    */

    if  ( appGetResponseType( &ast, &astt, &targetFound,
				    ea->eaDocSelectionTypes,
				    ea->eaDocSelectionTypeCount,
				    selEvent->selection, selEvent->target ) )
	{ LDEB(1); return;	}

    if  ( selEvent->property == None )
	{
	ea->eaGotPaste= -1;

	if  ( targetFound < ast->astTargetTypeCount- 1 )
	    {
	    Window		win= XtWindow( w );

	    XConvertSelection( display, selEvent->selection,
			    astt[1].asttTargetAtom,
			    selEvent->selection, win, selEvent->time );

	    ea->eaGotPaste= 0;
	    }
	}
    else{
	(*astt->asttUsePaste)( w, voided, event, pRefused );
	ea->eaGotPaste= 1;
	}

    return;
    }

static void appAppUsePaste(		Widget			w,
					void *			voidea,
					XEvent *		event,
					Boolean *		pRefused )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    XSelectionEvent *		selEvent= (XSelectionEvent *)event;

    AppSelectionType *		ast;
    AppSelectionTargetType *	astt;
    int				targetFound;

    Display *			display= XtDisplay( w );

    /*
    appDebug( "PASTE " );
    appDebug( "selection= %s target= %s, property= %s\n",
			selEvent->selection == None ? "None" :
			    XGetAtomName( display, selEvent->selection ),
			selEvent->target == None ? "None" :
			    XGetAtomName( display, selEvent->target ),
			selEvent->property == None ? "None" :
			    XGetAtomName( display, selEvent->property ) );
    */

    if  ( appGetResponseType( &ast, &astt, &targetFound,
				    ea->eaAppSelectionTypes,
				    ea->eaAppSelectionTypeCount,
				    selEvent->selection, selEvent->target ) )
	{ LDEB(1); return;	}

    if  ( selEvent->property == None )
	{
	ea->eaGotPaste= -1;

	if  ( targetFound < ast->astTargetTypeCount- 1 )
	    {
	    Window		win= XtWindow( w );

	    XConvertSelection( display, selEvent->selection,
			    astt[1].asttTargetAtom,
			    selEvent->selection, win, selEvent->time );

	    ea->eaGotPaste= 0;
	    }
	}
    else{
	(*astt->asttUsePaste)( w, voidea, event, pRefused );
	ea->eaGotPaste= 1;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Deliver the data that you previously copied to someone who wants	*/
/*  to paste it.							*/
/*									*/
/************************************************************************/

APP_GIVE_COPY( appDocReplyToCopyRequest, w, event, voided )
    {
    XSelectionRequestEvent *	reqEvent= &(event->xselectionrequest);
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;

    AppSelectionType *		ast;
    AppSelectionTargetType *	astt;
    int				targetFound;

    Display *			display= XtDisplay( w );

    XEvent			response;

    /*
    appDebug( "COPY  " );
    appDebug( "selection= %s target= %s, property= %s\n",
			    reqEvent->selection == None ? "None" :
				XGetAtomName( display, reqEvent->selection ),
			    reqEvent->target == None ? "None" :
				XGetAtomName( display, reqEvent->target ),
			    reqEvent->property == None ? "None" :
				XGetAtomName( display, reqEvent->property ) );
    */

    response.type= SelectionNotify;
    response.xselection.display= reqEvent->display;
    response.xselection.requestor= reqEvent->requestor;
    response.xselection.selection= reqEvent->selection;
    response.xselection.target= reqEvent->target;
    response.xselection.property= reqEvent->property;
    response.xselection.time= reqEvent->time;

    if  ( response.xselection.target == XA_TARGETS )
	{
	static Atom *		atoms;
	Atom *			fresh;
	int			i;

	if  ( ed->edTargetTypeCount < 1 )
	    { LDEB(ed->edTargetTypeCount); goto refuse;	}

	fresh= (Atom *)realloc( atoms, ed->edTargetTypeCount* sizeof(Atom) );
	if  ( ! fresh )
	    { XDEB(fresh); goto refuse;	}
	atoms= fresh;

	for ( i= 0; i < ed->edTargetTypeCount; i++ )
	    { atoms[i]= ed->edTargetTypes[i].asttTargetAtom; }

	XChangeProperty( display,
			response.xselection.requestor,
			response.xselection.property,
			response.xselection.target,
			8* sizeof(Atom), PropModeReplace,
			(unsigned char *)atoms,
			ed->edTargetTypeCount );

	XSendEvent( display, response.xselection.requestor, False, 0L,
								&response );
	return;
	}

    if  ( response.xselection.target == XA_MULTIPLE )
	{
#	define		FST		( 2* 10L )

	Atom *		atomPairs;

	int		ret;
	int		i;

	unsigned long	itemsReturned;
	unsigned long	itemsLeft;
	Atom		typeFound;
	int		formatFound;

	ret= XGetWindowProperty( display, reqEvent->requestor,
		reqEvent->property,
		0L, FST, False, AnyPropertyType,
		&typeFound, &formatFound,
		&itemsReturned, &itemsLeft, (unsigned char **)&atomPairs );

	if  ( ret != Success )
	    { LLDEB(ret,Success); goto refuse;	}

	if  ( itemsLeft > 0 )
	    {
	    XFree( atomPairs );

	    ret= XGetWindowProperty( display, reqEvent->requestor,
		    reqEvent->property,
		    0L, FST+ itemsLeft, False, AnyPropertyType,
		    &typeFound, &formatFound,
		    &itemsReturned, &itemsLeft, (unsigned char **)&atomPairs );
	    }

	if  ( ret != Success )
	    { LLDEB(ret,Success); goto refuse;	}
	if  ( itemsReturned == 0 || itemsReturned % 2 )
	    { LDEB(itemsReturned); goto refuse;	}
	if  ( formatFound != 32 )
	    { LLDEB(typeFound,formatFound); goto refuse;	}

	for ( i= 0; i < (int)itemsReturned; i += 2 )
	    {
	    XEvent	evPaste;

	    evPaste= response;

	    if  ( appGetResponseType( &ast, &astt, &targetFound,
					ea->eaDocSelectionTypes,
					ea->eaDocSelectionTypeCount,
					reqEvent->selection, atomPairs[i] ) )
		{ LDEB(1); goto refuse;	}

	    if  ( ! astt->asttGiveCopy )
		{ XDEB(astt->asttGiveCopy); goto refuse;	}

	    evPaste.xselection.requestor= response.xselection.requestor;
	    evPaste.xselection.property= atomPairs[i+1];
	    evPaste.xselection.target= atomPairs[i];

	    (*astt->asttGiveCopy)( w, ed, &evPaste, pRefused );
	    }

	XFree( atomPairs );
	XSendEvent( display, response.xselection.requestor, False, 0L,
								&response );
	return;
	}

    /*  ICCM L.2.2; ICCM L.2.6.2  */
    if  ( response.xselection.property == None )
	{ response.xselection.property= response.xselection.target;	}

    if  ( appGetResponseType( &ast, &astt, &targetFound,
				    ea->eaDocSelectionTypes,
				    ea->eaDocSelectionTypeCount,
				    reqEvent->selection, reqEvent->target ) )
	{ goto refuse;	}

    if  ( ! astt->asttGiveCopy )
	{ XDEB(astt->asttGiveCopy); goto refuse;	}

    (*astt->asttGiveCopy)( w, voided, &response, pRefused );

    XSendEvent( display, response.xselection.requestor,
						    False, 0L, &response );
    return;

  refuse:
    /*
    SDEB(XGetAtomName(display,reqEvent->selection));
    SDEB(XGetAtomName(display,reqEvent->target));
    */

    response.xselection.property= None;
    XSendEvent( display, reqEvent->requestor,
				    False, PropertyChangeMask, &response );

    return;
    }

/************************************************************************/
/*									*/
/*  You can forget the selection you just copied because no one is	*/
/*  going to ask for it anymore.					*/
/*									*/
/************************************************************************/

static void appDocForgetCopiedSelection(	APP_WIDGET		w,
						void *			voided,
						APP_EVENT *		event )
    {
    XSelectionClearEvent *	clrEvent= &(event->xselectionclear);
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;

    int				i;
    AppSelectionType *		ast;

    ast= ea->eaDocSelectionTypes;
    for ( i= 0; i < ea->eaDocSelectionTypeCount; ast++, i++ )
	{
	if  ( ast->astSelectionAtom == clrEvent->selection )
	    { break;	}
	}

    if  ( i >= ea->eaDocSelectionTypeCount )
	{ ADEB(clrEvent,clrEvent->selection); return; }

    if  ( ast->astForgetCopy )
	{ (*ast->astForgetCopy)( w, voided, event );	}

    return;
    }

/************************************************************************/
/*									*/
/*  CopyPaste event handler.						*/
/*									*/
/************************************************************************/

APP_EVENT_HANDLER_H( appDocCopyPasteHandler, w, voided, event )
    {
    if  ( XA_MULTIPLE == None )
	{ XA_MULTIPLE= XInternAtom( XtDisplay( w ), "MULTIPLE", False ); }
    if  ( XA_TARGETS == None )
	{ XA_TARGETS= XInternAtom( XtDisplay( w ), "TARGETS", False ); }

    switch( event->type )
	{
	case SelectionNotify:
	    appDocGotPasteReply( w, voided, event, pRefused );
	    *pRefused= False;
	    break;

	case SelectionRequest:
	    appDocReplyToCopyRequest( w, voided, event, pRefused );
	    *pRefused= False;
	    break;

	case SelectionClear:
	    appDocForgetCopiedSelection( w, voided, event );
	    *pRefused= False;
	    break;

	case PropertyNotify:
	    *pRefused= True;
	    break;

	case GraphicsExpose:
	    appDocExposeHandler( w, voided, event, pRefused );
	    *pRefused= False;
	    break;

	case NoExpose:
	    break;

	default:
	    {
	    EditDocument *	ed= (EditDocument *)voided;

	    appDebug( "SELECTION \"%s\": %s\n",
			    ed->edTitle, APP_X11EventNames[event->type] );
	    *pRefused= True;
	    }
	    break;
	}

    return;
    }

void appAppGotPasteCall(	Widget		w,
				void *		voidea,
				XEvent *	event,
				Boolean *	pRefused )
    {
    EditApplication *	ea= (EditApplication *)voidea;

    if  ( XA_MULTIPLE == None )
	{ XA_MULTIPLE= XInternAtom( XtDisplay( w ), "MULTIPLE", False ); }
    if  ( XA_TARGETS == None )
	{ XA_TARGETS= XInternAtom( XtDisplay( w ), "TARGETS", False ); }

    switch( event->type )
	{
	case SelectionNotify:
	    appAppUsePaste( w, voidea, event, pRefused );
	    *pRefused= False;
	    break;
	case PropertyNotify:
	    *pRefused= True;
	    break;
	case SelectionClear:
	case NoExpose:
	case GraphicsExpose:
	case SelectionRequest:
	default:
	    appDebug( "SELECTION \"%s\": %s\n",
		    ea->eaApplicationName, APP_X11EventNames[event->type] );
	    *pRefused= True;
	    break;
	}

    return;
    }

int appDocOwnSelection(		EditDocument *			ed,
				const char *			selection,
				AppSelectionTargetType * 	targets,
				int				targetCount )
    {
    int				i;
    AppSelectionType *		ast;

    EditApplication *		ea= ed->edApplication;
    Display *			display= XtDisplay( ed->edDocumentWidget );
    Window			win= XtWindow( ed->edDocumentWidget );

    ast= ea->eaDocSelectionTypes;
    for ( i= 0; i < ea->eaDocSelectionTypeCount; ast++, i++ )
	{
	if  ( ! strcmp( ast->astSelectionString, selection ) )
	    { break;	}
	}

    if  ( i >= ea->eaDocSelectionTypeCount )
	{ SDEB(selection); return -1; }

    if  ( ast->astTargetTypeCount == 0 )
	{ SLDEB(selection,ast->astTargetTypeCount); return -1;	}

    for ( i= 0; i < targetCount; i++ )
	{
	if  ( targets[i].asttTargetAtom == None )
	    {
	    targets[i].asttTargetAtom=
				XInternAtom( XtDisplay( ed->edDocumentWidget ),
				targets[i].asttTargetString, False );

	    if  ( ! targets[i].asttTargetAtom )
		{
		SDEB(targets[i].asttTargetString);
		XDEB(targets[i].asttTargetAtom);
		return -1;
		}
	    }
	}

    ed->edTargetTypes= targets;
    ed->edTargetTypeCount= targetCount;

    XSetSelectionOwner( display, ast->astSelectionAtom, win, CurrentTime );

    if  ( XGetSelectionOwner( display, ast->astSelectionAtom ) != win )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Ask for a paste.							*/
/*									*/
/*  NOTE that the selection and property names are identical.		*/
/*									*/
/*  1)  Some software does not support the normal X11 Copy/Paste	*/
/*	mechanism. (Guess why) This should be handeled by the		*/
/*	application.							*/
/*									*/
/************************************************************************/

static int appAskForPaste(	Widget			w,
				EditApplication *	ea,
				const char *		selection )
    {
    int				i;
    AppSelectionType *	ast;

    Display *			display= XtDisplay( w );
    Window			win= XtWindow( w );

    ea->eaGotPaste= 0;

    ast= ea->eaDocSelectionTypes;
    for ( i= 0; i < ea->eaDocSelectionTypeCount; ast++, i++ )
	{
	if  ( ! strcmp( ast->astSelectionString, selection ) )
	    { break;	}
	}

    if  ( i >= ea->eaDocSelectionTypeCount )
	{ SDEB(selection); return -1; }

    if  ( ast->astTargetTypeCount == 0 )
	{ SLDEB(selection,ast->astTargetTypeCount); return -1;	}

    XConvertSelection( display,
		/*  selection	*/  ast->astSelectionAtom,
		/*  target	*/  ast->astTargetTypes[0].asttTargetAtom,
		/*  property	*/  ast->astSelectionAtom,
				    win, CurrentTime );

    while( ! ea->eaGotPaste )
	{
	XEvent		event;

	XtAppNextEvent( ea->eaContext, &event );
	XtDispatchEvent( &event );
	}

    /*  1  */
    if  ( ea->eaGotPaste < 0 )
	{ return -1;	}

    return 0;
    }
				
/************************************************************************/
/*									*/
/*  Ask for a paste of a particular selection.				*/
/*									*/
/*  1)  Paste into a document.						*/
/*  2)  Paste into the application.					*/
/*									*/
/************************************************************************/

/*  1  */
int appDocAskForPaste(		EditDocument *		ed,
				const char *		selection )
    {
    return appAskForPaste( ed->edDocumentWidget,
				    ed->edApplication, selection );
    }
				
/*  2  */
int appAppAskForPaste(		EditApplication *	ea,
				const char *		selection )
    {
    return appAskForPaste( ea->eaToplevel.atTopWidget, ea, selection );
    }

/************************************************************************/
/*									*/
/*  Allocate atoms for the different selection types.			*/
/*									*/
/************************************************************************/

void appAllocateCopyPasteTargetAtoms(	EditApplication *	ea )
    {
    AppSelectionType *		ast;
    int				i;

    int				j;
    AppSelectionTargetType *	astt;

    Display *			display;

    display= XtDisplay( ea->eaToplevel.atTopWidget );

    ast= ea->eaDocSelectionTypes;
    for ( i= 0; i < ea->eaDocSelectionTypeCount; ast++, i++ )
	{
	if  ( ast->astSelectionAtom == None )
	    {
	    ast->astSelectionAtom= XInternAtom( display,
					    ast->astSelectionString, 0 );
	    if  ( ast->astSelectionAtom == None )
		{ SLDEB(ast->astSelectionString, ast->astSelectionAtom); }
	    }
	astt= ast->astTargetTypes;
	for ( j= 0; j < ast->astTargetTypeCount; astt++, j++ )
	    {
	    if  ( astt->asttTargetAtom == None )
		{
		astt->asttTargetAtom= XInternAtom( display,
					    astt->asttTargetString, 0 );
		if  ( astt->asttTargetAtom == None )
		    { SLDEB(astt->asttTargetString, astt->asttTargetAtom); }
		}
	    }
	}

    ast= ea->eaAppSelectionTypes;
    for ( i= 0; i < ea->eaAppSelectionTypeCount; ast++, i++ )
	{
	if  ( ast->astSelectionAtom == None )
	    {
	    ast->astSelectionAtom= XInternAtom( display,
					    ast->astSelectionString, 0 );
	    if  ( ast->astSelectionAtom == None )
		{ SLDEB(ast->astSelectionString, ast->astSelectionAtom); }
	    }
	astt= ast->astTargetTypes;
	for ( j= 0; j < ast->astTargetTypeCount; astt++, j++ )
	    {
	    if  ( astt->asttTargetAtom == None )
		{
		astt->asttTargetAtom= XInternAtom( display,
					    astt->asttTargetString, 0 );
		if  ( astt->asttTargetAtom == None )
		    { SLDEB(astt->asttTargetString, astt->asttTargetAtom); }
		}
	    }
	}

    return;
    }

void appCopyPixmapValue(	APP_SELECTION_EVENT *	event,
				APP_BITMAP_IMAGE	pixmapCopied )
    {
    XSelectionEvent *	selEvent= &(event->xselection);

    XChangeProperty( selEvent->display,
		    selEvent->requestor, selEvent->property, selEvent->target,
		    8* sizeof(Pixmap), PropModeReplace,
		    (unsigned char *)&pixmapCopied,
		    1 );
    }

#   endif
