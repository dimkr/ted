/************************************************************************/
/*									*/
/*  Ted: Handle user input.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedApp.h"
#   include	"tedRuler.h"

#   include	<appDebugon.h>

# ifdef USE_MOTIF

/************************************************************************/
/*									*/
/*  Blinking cursor: MOTIF specific code.				*/
/*									*/
/************************************************************************/

#   define	TED_BLINK_VISIBLE	(800L)
#   define	TED_BLINK_INVISIBLE	(400L)

static void tedHideIBar(	void *		voided,
				XtIntervalId *	intervalId );

static void tedShowIBar(	void *		voided,
				XtIntervalId *	intervalId )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    AppDrawingData *		add= &(ed->edDrawingData);

    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    td->tdShowIBarId= (XtIntervalId)0;

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return;	}

    tedDrawIBar( &(sg.sgBegin), ox, oy, add );

    td->tdHideIBarId= XtAppAddTimeOut( ea->eaContext, TED_BLINK_VISIBLE,
						tedHideIBar, (void *)ed );
    }

static void tedHideIBar(	void *		voided,
				XtIntervalId *	intervalId )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    td->tdHideIBarId= (XtIntervalId)0;

    tedUndrawIBar( ed );

    td->tdShowIBarId= XtAppAddTimeOut( ea->eaContext, TED_BLINK_INVISIBLE,
						tedShowIBar, (void *)ed );
    }

void tedStartCursorBlink(	EditDocument *	ed )
    {
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    tedStopCursorBlink( ed );

    td->tdHideIBarId= XtAppAddTimeOut( ea->eaContext, TED_BLINK_VISIBLE,
						tedHideIBar, (void *)ed );
    }

void tedStopCursorBlink(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    if  ( td->tdHideIBarId )
	{ XtRemoveTimeOut( td->tdHideIBarId ); }

    if  ( td->tdShowIBarId )
	{
	AppDrawingData *	add= &(ed->edDrawingData);

	int			ox= ed->edVisibleRect.drX0;
	int			oy= ed->edVisibleRect.drY0;

	DocumentSelection	ds;
	SelectionGeometry	sg;
	SelectionDescription	sd;

	XtRemoveTimeOut( td->tdShowIBarId );

	if  ( ! tedGetSelection( &ds, &sg, &sd, td ) )
	    {
	    tedDrawIBar( &(sg.sgBegin), ox, oy, add );
	    }
	}

    td->tdHideIBarId= (XtIntervalId)0;
    td->tdShowIBarId= (XtIntervalId)0;

    return;
    }

void tedCleanCursorBlink(	TedDocument *	td )
    {
    if  ( td->tdHideIBarId )
	{ XtRemoveTimeOut( td->tdHideIBarId ); }
    if  ( td->tdShowIBarId )
	{ XtRemoveTimeOut( td->tdShowIBarId ); }

    td->tdHideIBarId= (XtIntervalId)0;
    td->tdShowIBarId= (XtIntervalId)0;
    }

#   endif

