/************************************************************************/
/*  Editor, File actions.						*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	<appFrame.h>

#   include	<appDebugon.h>

#   ifdef	USE_GTK

# define ADEB(ev,a) SDEB(((a)==0?"None":gdk_atom_name((a))))

/************************************************************************/
/*									*/
/*  Look for a selection type.						*/
/*									*/
/************************************************************************/

static int appGetResponseType(	AppSelectionType **		pAst,
				AppSelectionTargetType **	pAstt,
				int *				pTtargetFound,
				AppSelectionType *		ast,
				int				astCount,
				APP_ATOM			selection,
				APP_ATOM			target )
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
/*  Respond to selection events.					*/
/*									*/
/************************************************************************/

APP_EVENT_HANDLER_H( appAppGotPasteCall, w, voided, event )
    {
LDEB(1); return;
    }

int appDocOwnSelection(		EditDocument *			ed,
				const char *			selection,
				AppSelectionTargetType * 	targets,
				int				targetCount )
    {
    int				i;
    AppSelectionType *		ast;

    EditApplication *		ea= ed->edApplication;

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
	if  ( ! targets[i].asttTargetAtom )
	    {
	    targets[i].asttTargetAtom= gdk_atom_intern(
					    targets[i].asttTargetString, 0 );

	    if  ( ! targets[i].asttTargetAtom )
		{
		SDEB(targets[i].asttTargetString);
		XDEB(targets[i].asttTargetAtom);
		return -1;
		}
	    }

	gtk_selection_add_target( ed->edDocumentWidget,
						ast->astSelectionAtom,
						targets[i].asttTargetAtom,
						0 );
	}

    ed->edTargetTypes= targets;
    ed->edTargetTypeCount= targetCount;

    if  ( ! gtk_selection_owner_set( ed->edDocumentWidget,
						    ast->astSelectionAtom,
						    GDK_CURRENT_TIME ) )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Paste loop:								*/
/*									*/
/************************************************************************/

static int appAskForPaste(	APP_WIDGET		w,
				EditApplication *	ea,
				const char *		selection )
    {
    int				i;
    AppSelectionType *	ast;

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

    if  ( ! gtk_selection_convert( w,
		/*  selection	*/  ast->astSelectionAtom,
		/*  target	*/  ast->astTargetTypes[0].asttTargetAtom,
				    GDK_CURRENT_TIME ) )
	{ SDEB(selection); return -1;	}

    gtk_main();

    /*  1  */
    if  ( ea->eaGotPaste < 0 )
	{ return -1;	}

    return 0;
    }

static void appDocGotPasteReply(	GtkWidget *		w,
					GtkSelectionData *	gsd,
					guint			time,
					void *			voided )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;

    AppSelectionType *		ast;
    AppSelectionTargetType *	astt;
    int				targetFound;

    if  ( appGetResponseType( &ast, &astt, &targetFound,
					    ea->eaDocSelectionTypes,
					    ea->eaDocSelectionTypeCount,
					    gsd->selection, gsd->target ) )
	{ LDEB(1); return;	}

    if  ( ! gsd->type )
	{
	ea->eaGotPaste= -1;

	if  ( targetFound < ast->astTargetTypeCount- 1 )
	    {
	    if  ( ! gtk_selection_convert( w,
			/*  selection	*/  gsd->selection,
			/*  target	*/  astt[1].asttTargetAtom,
					    time ) )
		{ LDEB(1); return;	}

	    ea->eaGotPaste= 0;
	    }
	}
    else{
	(*astt->asttUsePaste)( w, gsd, time, voided );
	ea->eaGotPaste= 1;
	gtk_main_quit();
	}
    }

static void appAppGotPaste(	GtkWidget *		w,
				GtkSelectionData *	gsd,
				guint			time,
				void *			voidea )
    {
    EditApplication *	ea= (EditApplication *)voidea;
LDEB(1);
ea->eaGotPaste= 1;
    gtk_main_quit();
    }

int appDocAskForPaste(		EditDocument *		ed,
				const char *		selection )
    {
    int		rval;
    guint	id;

    id= gtk_signal_connect( GTK_OBJECT( ed->edDocumentWidget ),
					"selection_received",
					(GtkSignalFunc)appDocGotPasteReply,
					(void *)ed );

    rval= appAskForPaste( ed->edDocumentWidget,
					ed->edApplication, selection );

    gtk_signal_disconnect( GTK_OBJECT( ed->edDocumentWidget ), id );

    return rval;
    }

/*  2  */
int appAppAskForPaste(		EditApplication *	ea,
				const char *		selection )
    {
    int		rval;
    guint	id;

    id= gtk_signal_connect( GTK_OBJECT( ea->eaToplevel.atTopWidget ),
						"selection_received",
						(GtkSignalFunc)appAppGotPaste,
						(void *)ea );

    rval= appAskForPaste( ea->eaToplevel.atTopWidget, ea, selection );

    gtk_signal_disconnect( GTK_OBJECT( ea->eaToplevel.atTopWidget ), id );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Deliver the data that you previously copied to someone who wants	*/
/*  to paste it.							*/
/*									*/
/************************************************************************/

APP_GIVE_COPY( appDocReplyToCopyRequest, w, gsd, voided )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;

    AppSelectionType *		ast;
    AppSelectionTargetType *	astt;
    int				targetFound;

    if  ( appGetResponseType( &ast, &astt, &targetFound,
					    ea->eaDocSelectionTypes,
					    ea->eaDocSelectionTypeCount,
					    gsd->selection, gsd->target ) )
	{ LDEB(1); return;	}

    if  ( ! astt->asttGiveCopy )
	{ XDEB(astt->asttGiveCopy); return;	}

    (*astt->asttGiveCopy)( w, gsd, info, time, voided );

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
    GdkEventSelection *		clrEvent= &(event->selection);
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
	{ ADEB(ev,clrEvent->selection); return; }

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
    switch( event->type )
	{
#	if 0
	case GDK_SELECTION_NOTIFY:
	    appDocGotPasteReply( w, voided, event );
	    break;
#	endif

	case GDK_SELECTION_CLEAR:
	    appDocForgetCopiedSelection( w, voided, event );
	    break;

	default:
	    LDEB(event->type);
	    break;
	}

    return;
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

    ast= ea->eaDocSelectionTypes;
    for ( i= 0; i < ea->eaDocSelectionTypeCount; ast++, i++ )
	{
	if  ( ! ast->astSelectionAtom )
	    {
	    ast->astSelectionAtom= gdk_atom_intern(
					    ast->astSelectionString, 0 );
	    if  ( ! ast->astSelectionAtom )
		{ SLDEB(ast->astSelectionString, ast->astSelectionAtom); }
	    }
	astt= ast->astTargetTypes;
	for ( j= 0; j < ast->astTargetTypeCount; astt++, j++ )
	    {
	    if  ( ! astt->asttTargetAtom )
		{
		astt->asttTargetAtom= gdk_atom_intern(
					    astt->asttTargetString, 0 );
		if  ( ! astt->asttTargetAtom )
		    { SLDEB(astt->asttTargetString, astt->asttTargetAtom); }
		}
	    }
	}

    ast= ea->eaAppSelectionTypes;
    for ( i= 0; i < ea->eaAppSelectionTypeCount; ast++, i++ )
	{
	if  ( ! ast->astSelectionAtom )
	    {
	    ast->astSelectionAtom= gdk_atom_intern(
					    ast->astSelectionString, 0 );
	    if  ( ! ast->astSelectionAtom )
		{ SLDEB(ast->astSelectionString, ast->astSelectionAtom); }
	    }
	astt= ast->astTargetTypes;
	for ( j= 0; j < ast->astTargetTypeCount; astt++, j++ )
	    {
	    if  ( ! astt->asttTargetAtom )
		{
		astt->asttTargetAtom= gdk_atom_intern(
					    astt->asttTargetString, 0 );
		if  ( ! astt->asttTargetAtom )
		    { SLDEB(astt->asttTargetString, astt->asttTargetAtom); }
		}
	    }
	}

    return;
    }


#   endif
