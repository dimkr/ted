#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"appFrame.h"
#   include	"appSystem.h"
#   include	<appGeoString.h>

#   ifdef USE_GTK

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Hide the popup of this drawn popdown.				*/
/*									*/
/************************************************************************/

static void appDrawnPulldownPopdown(	APP_WIDGET	w )
    {
    if  ( GTK_WIDGET_HAS_GRAB( w ) )
	{
	gtk_grab_remove( w );
	gdk_pointer_ungrab( GDK_CURRENT_TIME );
	}

    gtk_widget_hide( w );

    return;
    }

/************************************************************************/
/*									*/
/*  Display the popup that belongs to this drawn popdown.		*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( appDrawnPulldownPulldown, w, voidadp, mouseEvent )
    {
    AppDrawnPulldown *	adp= (AppDrawnPulldown *)voidadp;
    GdkEventButton *	bevent= &(mouseEvent->button);

    gint		screenX;
    gint		screenY;

    int			mouseX;
    int			mouseY;

    int			button;
    int			upDown;
    int			seq;
    unsigned int	keyState= 0;

    if  ( bevent->button != 1 )
	{ return;	}

    /******/

    if  ( appGetCoordinatesFromMouseButtonEvent( &mouseX, &mouseY,
					    &button, &upDown, &seq, &keyState,
					    w, mouseEvent ) )
	{ appDrawnPulldownPopdown( w ); return;	}

    screenX= bevent->x_root;
    screenY= bevent->y_root;

    /*
    adp->adpFirstDownTime= bevent->time;
    */
    adp->adpMouseX= screenX;
    adp->adpMouseY= screenY;

    /******/

    gdk_window_get_origin( adp->adpInplaceDrawing->window, &screenX, &screenY );

#   if GTK_MAJOR_VERSION < 2
    gtk_widget_set_uposition( adp->adpPulldownShell, screenX, screenY );
#   else
    gtk_window_move( GTK_WINDOW( adp->adpPulldownShell ), screenX, screenY );
#   endif

    /* code borrowed from gtkcombo.c: */

    gtk_widget_show( adp->adpPulldownShell );
    gtk_widget_grab_focus( adp->adpPulldownShell );
    gtk_grab_add( adp->adpPulldownShell );

    gdk_pointer_grab( adp->adpPulldownShell->window, TRUE,
				    GDK_BUTTON_PRESS_MASK | 
				    GDK_BUTTON_RELEASE_MASK,
				    NULL, NULL, GDK_CURRENT_TIME );

    return;
    }

static APP_EVENT_HANDLER_H( appDrawnPulldownMouseUpDown, w, voidadp, mouseEvent )
    {
    AppDrawnPulldown *	adp= (AppDrawnPulldown *)voidadp;
    GdkEventButton *	bevent= &(mouseEvent->button);

    int			screenX;
    int			screenY;

    gint		wide;
    gint		high;

    int			mouseX;
    int			mouseY;

    int			button;
    int			upDown;
    int			seq;
    unsigned int	keyState= 0;

    /**/
    if  ( appGetCoordinatesFromMouseButtonEvent( &mouseX, &mouseY,
					    &button, &upDown, &seq, &keyState,
					    w, mouseEvent ) )
	{ appDrawnPulldownPopdown( w ); return;	}

    screenX= bevent->x_root;
    screenY= bevent->y_root;

    /*
    if  ( mouseEvent->xbutton.time == adp->adpFirstDownTime )
	{ return;	}
    */

    if  ( upDown < 0			&&
	  adp->adpMouseX == screenX	&&
	  adp->adpMouseY == screenY	)
	{ return;	}

#   if GTK_MAJOR_VERSION < 2
    wide= w->allocation.width;
    high= w->allocation.height;
#   else
    gtk_window_get_size( GTK_WINDOW( w ), &wide, &high );
#   endif

    /**/
    if  ( mouseX < 0		||
	  mouseY < 0		||
	  mouseX >= wide	||
	  mouseY >= high	)
	{ goto ready;	}

    if  ( adp->adpClickHandler )
	{
	(*adp->adpClickHandler)( w, mouseEvent, adp->adpThrough );
	}

  ready:
    appDrawnPulldownPopdown( w );


    return;
    }

/************************************************************************/
/*									*/
/*  Finish the drawn pulldown. Attach the pulldown to the in place	*/
/*  widget and do administration.					*/
/*									*/
/************************************************************************/

static void appFinishDrawnPulldown(	AppDrawnPulldown *	adp,
					APP_EVENT_HANDLER_T	redrawInplace,
					APP_EVENT_HANDLER_T	redrawPulldown,
					APP_EVENT_HANDLER_T	clickedPulldown,
					void *			through )
    {
    adp->adpClickHandler= clickedPulldown;
    adp->adpThrough= through;

    /******/

    if  ( redrawInplace )
	{
	appDrawSetRedrawHandler( adp->adpInplaceDrawing, 
						redrawInplace, through );
	}

    /*
    XtAddCallback( adp->adpInplaceDrawing, XmNresizeCallback,
				    appGuiSendExposeForResize, (void *)0 );
    */

    /******/

    adp->adpPulldownShell= gtk_window_new( GTK_WINDOW_POPUP );

    adp->adpPulldownDrawing= gtk_drawing_area_new();

    gtk_container_add( GTK_CONTAINER( adp->adpPulldownShell ),
						    adp->adpPulldownDrawing );

    gtk_drawing_area_size( GTK_DRAWING_AREA( adp->adpPulldownDrawing ),
								100, 100 );

    if  ( redrawPulldown )
	{
	appDrawSetRedrawHandler( adp->adpPulldownDrawing,
						redrawPulldown, through );
	}

    /*
    XtAddCallback( adp->adpPulldownDrawing, XmNresizeCallback,
				    appGuiSendExposeForResize, (void *)0 );
    */

    /******/
    gtk_widget_add_events( adp->adpPulldownShell, 
			    GDK_BUTTON_RELEASE_MASK|GDK_BUTTON_PRESS_MASK );

    gtk_signal_connect( GTK_OBJECT( adp->adpPulldownShell ),
				"button_press_event",
				(GtkSignalFunc)appDrawnPulldownMouseUpDown,
				(void *)adp );
    gtk_signal_connect( GTK_OBJECT( adp->adpPulldownShell ),
				"button_release_event",
				(GtkSignalFunc)appDrawnPulldownMouseUpDown,
				(void *)adp );

    gtk_widget_add_events( adp->adpInplaceDrawing, GDK_BUTTON_PRESS_MASK );
    gtk_signal_connect( GTK_OBJECT( adp->adpInplaceDrawing ),
				"button_press_event",
				(GtkSignalFunc)appDrawnPulldownPulldown,
				(void *)adp );

    /******/

    gtk_widget_show( adp->adpPulldownDrawing );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a pulldown where the values are drawings.. Not Text.		*/
/*									*/
/************************************************************************/

void appMakeDrawnPulldownInColumn(	AppDrawnPulldown *	adp,
					APP_EVENT_HANDLER_T	redrawInplace,
					APP_EVENT_HANDLER_T	redrawPulldown,
					APP_EVENT_HANDLER_T	clickedPulldown,
					APP_WIDGET		column,
					void *			through )
    {
    gboolean	expand= FALSE;
    gboolean	fill= TRUE;

    adp->adpInplaceDrawing= gtk_drawing_area_new();

    gtk_box_pack_start( GTK_BOX( column ), adp->adpInplaceDrawing,
							    expand, fill, 0 );

    gtk_widget_show( adp->adpInplaceDrawing );

    appFinishDrawnPulldown( adp,
		    redrawInplace, redrawPulldown, clickedPulldown, through );

    return;
    }

void appMakeDrawnPulldownInRow(		AppDrawnPulldown *	adp,
					APP_EVENT_HANDLER_T	redrawInplace,
					APP_EVENT_HANDLER_T	redrawPulldown,
					APP_EVENT_HANDLER_T	clickedPulldown,
					APP_WIDGET		row,
					int			column,
					int			colspan,
					void *			through )
    {
    adp->adpInplaceDrawing= gtk_drawing_area_new();

    gtk_table_attach( GTK_TABLE( row ),
			adp->adpInplaceDrawing,
			column, column+ colspan,
			0, 1,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			0, 0 );
			/*
			ROW_XPADDING_GTK, ROW_YPADDING_GTK );
			*/

    gtk_widget_show( adp->adpInplaceDrawing );

    appFinishDrawnPulldown( adp,
		    redrawInplace, redrawPulldown, clickedPulldown, through );

    return;
    }

void appGuiEnableDrawnPulldown(	AppDrawnPulldown *	adp,
				int			sensitive )
    {
    gtk_widget_set_sensitive( adp->adpInplaceDrawing, sensitive != 0 );
    }

void appInitDrawnPulldown(		AppDrawnPulldown *		adp )
    {
    adp->adpPulldownShell= (APP_WIDGET)0;
    adp->adpPulldownDrawing= (APP_WIDGET)0;
    adp->adpInplaceDrawing= (APP_WIDGET)0;
    adp->adpClickHandler= (APP_EVENT_HANDLER_T)0;
    adp->adpThrough= (void *)0;
    }

void appGuiSetDrawnPulldownHeight(	AppDrawnPulldown *	adp,
					int			height )
    {
    gint	wide;
    gint	high;

#   if GTK_MAJOR_VERSION < 2
    wide= adp->adpPulldownShell->allocation.width;
    high= adp->adpPulldownShell->allocation.height;
#   else
    gtk_window_get_size( GTK_WINDOW( adp->adpPulldownShell ), &wide, &high );
#   endif

    high= height;
    wide= adp->adpInplaceDrawing->allocation.width;

    gtk_window_set_default_size( GTK_WINDOW( adp->adpPulldownShell ),
							    wide, high );
#   if GTK_MAJOR_VERSION < 2
    gtk_widget_set_usize( adp->adpPulldownShell, wide, high );
#   else
    gtk_window_resize( GTK_WINDOW( adp->adpPulldownShell ), wide, high );
#   endif

    return;
    }

void appGuiSetDrawnPulldownStrips(	AppDrawnPulldown *	adp,
					int			strips )
    {
    int		inplaceHeight;

    inplaceHeight= adp->adpInplaceDrawing->allocation.height;

    appGuiSetDrawnPulldownHeight( adp, strips* inplaceHeight );

    return;
    }

int appGuiDrawnPulldownGetStrip(
				int *				pStrip,
				const AppDrawnPulldown *	adp,
				const APP_WIDGET		w,
				const APP_EVENT *		mouseEvent )
    {
    int			mouseX;
    int			mouseY;

    int			button;
    int			upDown;
    int			seq;
    unsigned int	keyState= 0;

    gint		inplaceHeight;

    inplaceHeight= adp->adpInplaceDrawing->allocation.height;

    if  ( appGetCoordinatesFromMouseButtonEvent( &mouseX, &mouseY,
					    &button, &upDown, &seq, &keyState,
					    w, mouseEvent ) )
	{ return -1;	}

    *pStrip= mouseY/ inplaceHeight;
    return 0;
    }

#   endif
