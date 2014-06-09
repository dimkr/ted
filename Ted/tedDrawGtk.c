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

#   ifdef USE_GTK

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
    GdkWindow *			win= add->addDrawable;

    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    GdkWindowAttr		xswa;
    int				i;
    gint			attributesMask;

    DocumentRectangle		drObj;
    APP_POINT			xp[RESIZE_COUNT];

    static const int		font_cursors[RESIZE_COUNT]=
				    {
				    GDK_BOTTOM_LEFT_CORNER,
				    GDK_BOTTOM_SIDE,
				    GDK_BOTTOM_RIGHT_CORNER,

				    GDK_LEFT_SIDE,
				    GDK_RIGHT_SIDE,

				    GDK_TOP_LEFT_CORNER,
				    GDK_TOP_SIDE,
				    GDK_TOP_RIGHT_CORNER,
				    };

    static GdkCursor *		moveCursor;
    static GdkCursor *		cursors[RESIZE_COUNT];

    if  ( ! moveCursor )
	{
	moveCursor= gdk_cursor_new( GDK_FLEUR );

	for ( i= 0; i < RESIZE_COUNT; i++ )
	    { cursors[i]= gdk_cursor_new( font_cursors[i] ); }
	}

    xswa.wclass= GDK_INPUT_ONLY;
    xswa.window_type= GDK_WINDOW_CHILD;
    attributesMask= GDK_WA_X | GDK_WA_Y;

    tedGetObjectRectangle( &drObj, xp, io, pg, td );
    for ( i= 0; i < RESIZE_COUNT; i++ )
	{
	xp[i].x -= drObj.drX0;
	xp[i].y -= drObj.drY0;
	}

    if  ( ! td->tdObjectWindow )
	{
	xswa.cursor= moveCursor;
	xswa.x= drObj.drX0- ox;
	xswa.y= drObj.drY0- oy;
	xswa.width= drObj.drX1- drObj.drX0+ 1;
	xswa.height= drObj.drY1- drObj.drY0+ 1;
	td->tdObjectWindow= gdk_window_new( win, &xswa, attributesMask );

	for ( i= 0; i < RESIZE_COUNT; i++ )
	    {
	    xswa.cursor= cursors[i];
	    xswa.x= xp[i].x;
	    xswa.y= xp[i].y;
	    xswa.width= RESIZE_BLOCK;
	    xswa.height= RESIZE_BLOCK;
	    td->tdObjectResizeWindows[i]= gdk_window_new( td->tdObjectWindow,
					 &xswa, attributesMask|GDK_WA_CURSOR );
	    }

	for ( i= 0; i < RESIZE_COUNT; i++ )
	    { gdk_window_show( td->tdObjectResizeWindows[i] );	}
	}
    else{
	gdk_window_move_resize( td->tdObjectWindow,
				drObj.drX0- ox, drObj.drY0- oy,
				drObj.drX1- drObj.drX0+ 1,
				drObj.drY1- drObj.drY0+ 1 );

	for ( i= 0; i < RESIZE_COUNT; i++ )
	    {
	    gdk_window_move( td->tdObjectResizeWindows[i],
							xp[i].x, xp[i].y );
	    }
	}

    gdk_window_show( td->tdObjectWindow );
    }

void tedHideObjectWindows(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    if  ( ! td->tdObjectWindow )
	{ XDEB(td->tdObjectWindow); return;	}

    gdk_window_hide( td->tdObjectWindow );
    }
#   endif
