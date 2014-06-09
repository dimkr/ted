#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"appFrame.h"
#   include	"appSystem.h"
#   include	<appGeoString.h>

#   ifdef USE_GTK

#   include	<gdk/gdkkeysyms.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Make a dialog with a vertical organisation.				*/
/*									*/
/************************************************************************/

void appMakeVerticalDialog(	AppDialog *		ad,
				APP_WIDGET *		pColumn,
				EditApplication *	ea,
				APP_CLOSE_CALLBACK_T	closeCallback,
				APP_DESTROY_CALLBACK_T	destroyCallback,
				void *			through,
				char *			widgetName )
    {
    APP_WIDGET		shell;
    APP_WIDGET		column;

#   if  GTK_MAJOR_VERSION < 2
    const gboolean	homogeneous= FALSE;
    const gint		spacing= COLUMN_SPACING_GTK;
#   endif

#   if GTK_MAJOR_VERSION < 2
    shell= gtk_window_new( GTK_WINDOW_DIALOG );
#   else
    shell= gtk_dialog_new();
#   endif

    gtk_window_set_position( GTK_WINDOW( shell ), GTK_WIN_POS_CENTER );

    if  ( closeCallback )
	{
	gtk_signal_connect( GTK_OBJECT( shell ),
					    "delete_event",
					    (GtkSignalFunc)closeCallback,
					    through );
	}

    if  ( destroyCallback )
	{
	gtk_signal_connect( GTK_OBJECT( shell ),
					    "destroy_event",
					    (GtkSignalFunc)destroyCallback,
					    through );
	}

    gtk_container_set_border_width( GTK_CONTAINER( shell ), WINDOW_BORDER_GTK );

#   if  GTK_MAJOR_VERSION < 2
    column= gtk_vbox_new( homogeneous, spacing );

    gtk_container_add( GTK_CONTAINER( shell ), column );

    gtk_widget_show( column );
#   else
    column= gtk_bin_get_child( GTK_BIN( shell ) );
#   endif

    ad->adAccelGroup= gtk_accel_group_new();
#   if GTK_MAJOR_VERSION < 2
    gtk_accel_group_attach( ad->adAccelGroup, GTK_OBJECT( shell ) );
#   else
    gtk_window_add_accel_group( GTK_WINDOW( shell ), ad->adAccelGroup );
#   endif

    ad->adTopWidget= shell; *pColumn= column;
    return;
    }

static void appGuiGtkDialogBreak(	GtkWidget *		w,
					GdkEvent *		event,
					void *			voidad )
    {
    AppDialog *		ad= (AppDialog *)voidad;

    ad->adCloseId= 0;

    gtk_main_quit();

    return;
    }

void appGuiRunDialog(			AppDialog *		ad,
					int			initial,
					EditApplication *	ea )
    {
    gtk_window_set_modal( GTK_WINDOW( ad->adTopWidget ), TRUE );

    ad->adResponse= initial;

    ad->adCloseId= gtk_signal_connect( GTK_OBJECT( ad->adTopWidget ),
					"delete_event",
					(GtkSignalFunc)appGuiGtkDialogBreak,
					(void *)ad );

    gtk_main();

    if  ( ad->adCloseId )
	{
	gtk_signal_disconnect( GTK_OBJECT( ad->adTopWidget ), ad->adCloseId );
	ad->adCloseId= 0;
	}

    gtk_window_set_modal( GTK_WINDOW( ad->adTopWidget ), FALSE );

    return;
    }

void appGuiBreakDialog(			AppDialog *		ad,
					int			response )
    {
    ad->adResponse= response;
    gtk_main_quit();
    }

void appGuiSetDefaultButtonForDialog(	AppDialog *		ad,
					APP_WIDGET		button )
    {
    unsigned int	keyMask= 0;
    unsigned int	keyCode= GDK_Return;

    gtk_widget_add_accelerator( button, "clicked", ad->adAccelGroup,
					keyCode, keyMask, GTK_ACCEL_VISIBLE );
    }

void appGuiSetCancelButtonForDialog(	AppDialog *		ad,
					APP_WIDGET		button )
    {
    unsigned int	keyMask= 0;
    unsigned int	keyCode= GDK_Escape;

    gtk_widget_add_accelerator( button, "clicked", ad->adAccelGroup,
					keyCode, keyMask, GTK_ACCEL_VISIBLE );
    }

void appGuiShowDialog(			AppDialog *		ad,
					APP_WIDGET		relative )
    {
    gint		x;
    gint		y;
    gint		wide;
    gint		high;

    if  ( relative )
	{
	gtk_window_set_transient_for(
		    GTK_WINDOW( ad->adTopWidget ), GTK_WINDOW( relative ) );

	gtk_window_set_position( GTK_WINDOW( ad->adTopWidget ),
							GTK_WIN_POS_NONE );

	gtk_widget_realize( ad->adTopWidget );

	gdk_window_get_position( relative->window, &x, &y );
	gdk_window_get_size( relative->window, &wide, &high );

	gdk_window_move( ad->adTopWidget->window, x+ wide/5, y+ high/ 5 );
	}
    else{
	gtk_window_set_position( GTK_WINDOW( ad->adTopWidget ),
							GTK_WIN_POS_CENTER );
	}

    gtk_widget_show( ad->adTopWidget );
    }

void appGuiHideDialog(			AppDialog *		ad )
    {
    gtk_widget_hide( ad->adTopWidget );

    gtk_window_set_transient_for(
			    GTK_WINDOW( ad->adTopWidget ), (GtkWindow *)0 );
    }

#   endif
