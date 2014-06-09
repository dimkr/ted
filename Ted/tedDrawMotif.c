/************************************************************************/
/*									*/
/*  Ted, Screen drawing and forcing drawing through			*/
/*  appExposeRectangle().						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	"tedApp.h"
#   include	"docDraw.h"

#   include	<appDebugon.h>

#   ifdef USE_MOTIF

#   include	<X11/cursorfont.h>

/************************************************************************/
/*									*/
/*  Create the windows that are used to reshape selected objects.	*/
/*									*/
/************************************************************************/

void tedSetObjectWindows(	EditDocument *			ed,
				const PositionGeometry *	pg,
				const InsertedObject *		io,
				int				ox,
				int				oy )
    {
    AppDrawingData *		add= &(ed->edDrawingData);
    Display *			display= add->addDisplay;
    Window			win= add->addDrawable;

    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    XSetWindowAttributes	xswa;
    int				i;

    DocumentRectangle		drObj;
    APP_POINT			xp[RESIZE_COUNT];

    static const int		font_cursors[RESIZE_COUNT]=
				    {
				    XC_bottom_left_corner,
				    XC_bottom_side,
				    XC_bottom_right_corner,

				    XC_left_side,
				    XC_right_side,

				    XC_top_left_corner,
				    XC_top_side,
				    XC_top_right_corner,
				    };

    static Cursor		moveCursor;
    static Cursor		cursors[RESIZE_COUNT];

    if  ( ! moveCursor )
	{
	moveCursor= XCreateFontCursor( display, XC_fleur );

	for ( i= 0; i < RESIZE_COUNT; i++ )
	    { cursors[i]= XCreateFontCursor( display, font_cursors[i] ); }
	}

    tedGetObjectRectangle( &drObj, xp, io, pg, td );
    for ( i= 0; i < RESIZE_COUNT; i++ )
	{
	xp[i].x -= drObj.drX0;
	xp[i].y -= drObj.drY0;
	}

    if  ( ! td->tdObjectWindow )
	{
	xswa.cursor= moveCursor;
	td->tdObjectWindow= XCreateWindow( display, win,
				drObj.drX0- ox, drObj.drY0- oy,
				drObj.drX1- drObj.drX0+ 1,
				drObj.drY1- drObj.drY0+ 1,
				0, CopyFromParent, InputOnly, CopyFromParent,
				0L, &xswa );

	for ( i= 0; i < RESIZE_COUNT; i++ )
	    {
	    xswa.cursor= cursors[i];

	    td->tdObjectResizeWindows[i]=
			XCreateWindow( display, td->tdObjectWindow,
			    xp[i].x, xp[i].y,
			    RESIZE_BLOCK, RESIZE_BLOCK,
			    0, CopyFromParent, CopyFromParent, CopyFromParent,
			    CWCursor, &xswa );
	    }

	for ( i= 0; i < RESIZE_COUNT; i++ )
	    { XMapRaised( display, td->tdObjectResizeWindows[i] ); }
	}
    else{
	XMoveResizeWindow( display, td->tdObjectWindow,
				drObj.drX0- ox, drObj.drY0- oy,
				drObj.drX1- drObj.drX0+ 1,
				drObj.drY1- drObj.drY0+ 1 );

	for ( i= 0; i < RESIZE_COUNT; i++ )
	    {
	    XMoveWindow( display, td->tdObjectResizeWindows[i],
							xp[i].x, xp[i].y );
	    }
	}

    XMapRaised( display, td->tdObjectWindow );
    }

void tedHideObjectWindows(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    if  ( ! td->tdObjectWindow )
	{ XDEB(td->tdObjectWindow); return;	}

    XUnmapWindow( XtDisplay( ed->edDocumentWidget ), td->tdObjectWindow );
    }

#   endif
