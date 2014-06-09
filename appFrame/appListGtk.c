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
/*  List specific functionality.					*/
/*									*/
/************************************************************************/

void appGuiMakeListInColumn(	APP_WIDGET *		pList,
				APP_WIDGET		column,
				int			visibleItems,
				APP_LIST_CALLBACK_T	callback,
				void *			through )
    {
    APP_WIDGET		list;
    APP_WIDGET		scrolled_window;

    gboolean		expand= TRUE;
    gboolean		fill= TRUE;

    scrolled_window= gtk_scrolled_window_new(
				(GtkAdjustment *)0, (GtkAdjustment *)0 );

    gtk_box_pack_start( GTK_BOX( column ), scrolled_window, expand, fill, 0 );

    gtk_widget_show( scrolled_window );

    gtk_scrolled_window_set_placement( GTK_SCROLLED_WINDOW( scrolled_window ),
						    GTK_CORNER_BOTTOM_LEFT );
    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( scrolled_window ),
						    GTK_POLICY_NEVER,
						    GTK_POLICY_AUTOMATIC );

    if  ( visibleItems > 0 )
	{
	GtkStyle *	gs= gtk_widget_get_style( scrolled_window );
	GdkFont *	gf;

#	if GTK_MAJOR_VERSION < 2
	gf= gs->font;
#	else
	gf= gtk_style_get_font( gs );
#	endif

	gtk_widget_set_usize( scrolled_window, -1, ( 110* visibleItems*
					( gf->ascent+ gf->descent ) )/ 100 );
	}

    list= gtk_list_new();

    gtk_scrolled_window_add_with_viewport(
			    GTK_SCROLLED_WINDOW( scrolled_window ), list );

    gtk_list_set_selection_mode( GTK_LIST( list ), GTK_SELECTION_SINGLE );

    gtk_container_set_focus_vadjustment( GTK_CONTAINER( list ),
			    gtk_scrolled_window_get_vadjustment(
				    GTK_SCROLLED_WINDOW( scrolled_window ) ) );

    if  ( callback )
	{
	gtk_signal_connect( GTK_OBJECT( list ), "select_child",
					    (GtkSignalFunc)callback, through );
	}

    gtk_widget_show( list );

    *pList= list; return;
    }

void appGuiEmptyListWidget(	APP_WIDGET		list )
    {
    gtk_list_unselect_all( GTK_LIST( list ) );
    gtk_list_clear_items( GTK_LIST( list ), 0, -1 );

    return;
    }

void appGuiAddValueToListWidget(	APP_WIDGET	list,
					const char *	value )
    {
    GList *	glist= (GList *)0;
    GtkWidget *	item= gtk_list_item_new_with_label( value );

    glist= g_list_append( glist, item );

    gtk_list_append_items( GTK_LIST( list ), glist );

    gtk_widget_show( item );

    /*  No:
    g_list_free( glist );
    */

    return;
    }

void appGuiSelectPositionInListWidget(		APP_WIDGET	list,
						int		position )
    {
    GtkWidget *		scrolled_window;

    gtk_list_select_item( GTK_LIST( list ), position );

    scrolled_window= gtk_widget_get_ancestor( list, GTK_TYPE_SCROLLED_WINDOW );

    if  ( ! scrolled_window )
	{ XDEB(scrolled_window); return;	}
    else{
	GtkAdjustment *	ga;

	GList *		glf;
	GList *		gl;

	int		childCount= 0;
	GtkWidget *	selectedItem= (GtkWidget *)0;

	ga= gtk_scrolled_window_get_vadjustment(
				    GTK_SCROLLED_WINDOW( scrolled_window ) );

	gl= glf= gtk_container_children( GTK_CONTAINER( list ) );

	while( gl )
	    {
	    if  ( childCount == position )
		{ selectedItem= (GtkWidget *)gl->data; }
	    childCount++; gl= gl->next;
	    }

	if  ( glf )
	    { g_list_free( glf );	}

	if  ( ! selectedItem )
	    { return;	}

	gtk_container_set_focus_child( GTK_CONTAINER( list ), selectedItem );
	}

    return;
    }

void appGuiRemoveSelectionFromListWidget(	APP_WIDGET	list )
    {
    gtk_list_unselect_all( GTK_LIST( list ) );

    return;
    }

int appGuiGetPositionFromListCallback(		APP_LIST	list,
						void *		voiditem )
    {
    GtkWidget *	item= (GtkWidget *)voiditem;


    return gtk_list_child_position( GTK_LIST( list ), item );
    }

char * appGuiGetStringFromListCallback(	APP_LIST		list,
					APP_LIST_CHOICE *	item )
    {
    static char	slabel[]= "<< ? >>";
    char *	label= slabel;

    if  ( ! appGuiGtkGetChildLabel( &label, item ) )
	{ SDEB(label);	}

    return label;
    }

void appFreeStringFromListCallback(	char *		s )
    { return;	}

#   endif
