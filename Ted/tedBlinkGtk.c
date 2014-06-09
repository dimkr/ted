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

# ifdef USE_GTK

/************************************************************************/
/*									*/
/*  Blinking cursor: GTK specific code.					*/
/*									*/
/*  Thanks to Andrea Frome who contributed the original code.		*/
/*									*/
/************************************************************************/

#   define	TED_BLINK_VISIBLE	(800L)
#   define	TED_BLINK_INVISIBLE	(400L)

static int tedHideIBar(	void *		voided );

static int tedShowIBar(	void *		voided )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    AppDrawingData *		add= &(ed->edDrawingData);

    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    td->tdShowIBarId= (guint)0;

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return 0;	}

    tedDrawIBar( &(sg.sgBegin), ox, oy, add );

    td->tdHideIBarId= gtk_timeout_add( TED_BLINK_VISIBLE,
						tedHideIBar, (void *)ed );
    return 0;
    }

static int tedHideIBar(	void *		voided )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    td->tdHideIBarId= (guint)0;

    tedUndrawIBar( ed );

    td->tdShowIBarId= gtk_timeout_add( TED_BLINK_INVISIBLE,
						tedShowIBar, (void *)ed );
    return 0;
    }

void tedStartCursorBlink(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    tedStopCursorBlink( ed );

    td->tdHideIBarId= gtk_timeout_add( TED_BLINK_VISIBLE,
						tedHideIBar, (void *)ed );
    }

void tedStopCursorBlink(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    if  ( td->tdHideIBarId )
	{ gtk_timeout_remove( td->tdHideIBarId ); }

    if  ( td->tdShowIBarId )
	{
	AppDrawingData *	add= &(ed->edDrawingData);

	int			ox= ed->edVisibleRect.drX0;
	int			oy= ed->edVisibleRect.drY0;

	DocumentSelection	ds;
	SelectionGeometry	sg;
	SelectionDescription	sd;

	gtk_timeout_remove( td->tdShowIBarId );

	if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	    { LDEB(1); return;	}

	tedDrawIBar( &(sg.sgBegin), ox, oy, add );
	}

    td->tdHideIBarId= (guint)0;
    td->tdShowIBarId= (guint)0;

    return;
    }

void tedCleanCursorBlink(	TedDocument *	td )
    {
    if  ( td->tdHideIBarId )
	{ gtk_timeout_remove( td->tdHideIBarId ); }
    if  ( td->tdShowIBarId )
	{ gtk_timeout_remove( td->tdShowIBarId ); }

    td->tdHideIBarId= (guint)0;
    td->tdShowIBarId= (guint)0;
    }

#   endif

