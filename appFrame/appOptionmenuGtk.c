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
/*  Used to get around Motif's ridiculous resize behavior. Probably	*/
/*  not needed for GTK.							*/
/*									*/
/************************************************************************/

void appOptionmenuRefreshWidth(	AppOptionmenu *		aom )
    { return;	}

void appMakeOptionmenuInColumn(		AppOptionmenu *	aom,
					APP_WIDGET	column )
    {
    APP_WIDGET	pulldown;
    APP_WIDGET	inplace;

    pulldown= gtk_menu_new();

    inplace= gtk_option_menu_new();

    gtk_box_pack_start( GTK_BOX( column ), inplace, FALSE, TRUE, 0 );

    gtk_widget_show( inplace );

    gtk_option_menu_set_menu( GTK_OPTION_MENU( inplace ), pulldown );

    aom->aomPulldown= pulldown; aom->aomInplace= inplace; return;
    }

void appMakeOptionmenuInRow(		AppOptionmenu *	aom,
					APP_WIDGET	row,
					int		column,
					int		colspan )
    {
    APP_WIDGET	pulldown;
    APP_WIDGET	inplace;

    pulldown= gtk_menu_new();

    inplace= gtk_option_menu_new();

    gtk_table_attach( GTK_TABLE( row ),
			inplace,
			column, column+ colspan,
			0, 1,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			ROW_XPADDING_GTK, ROW_YPADDING_GTK );

    gtk_widget_show( inplace );

    gtk_option_menu_set_menu( GTK_OPTION_MENU( inplace ), pulldown );

    aom->aomPulldown= pulldown; aom->aomInplace= inplace; return;
    }

/************************************************************************/
/*									*/
/*  Add an option to a pulldown.					*/
/*									*/
/************************************************************************/

APP_WIDGET appAddItemToOptionmenu(	AppOptionmenu *		aom,
					const char *		label,
					APP_OITEM_CALLBACK_T	callBack,
					void *			target )
    {
    APP_WIDGET	item;

    item= gtk_menu_item_new_with_label( label );

    gtk_menu_append( GTK_MENU( aom->aomPulldown ), item );

    gtk_widget_show( item );

    if  ( callBack )
	{
	gtk_signal_connect( GTK_OBJECT( item ), "activate",
					(GtkSignalFunc)callBack, target );
	}

    return item;
    }

/************************************************************************/
/*									*/
/*  Select a particular option in an option menu.			*/
/*									*/
/************************************************************************/

void appSetOptionmenu(	AppOptionmenu *	aom,
			int		num )
    {
    gtk_option_menu_set_history( GTK_OPTION_MENU( aom->aomInplace ), num );

    return;
    }

void appEmptyOptionmenu(	AppOptionmenu *		aom )
    {
    appEmptyParentWidget( aom->aomPulldown );
    }

void appGuiEnableOptionmenu(	AppOptionmenu *		aom,
				int			sensitive )
    {
    gtk_widget_set_sensitive( aom->aomInplace, sensitive != 0 );
    }

void appInitOptionmenu(		AppOptionmenu *		aom )
    {
    aom->aomPulldown= (APP_WIDGET)0;
    aom->aomInplace= (APP_WIDGET)0;
    }

static int appGuiGetOptionmenuFindItem(	int *		pIdx,
					GtkWidget *	w,
					GtkMenuItem *	it )
    {
    if  ( GTK_IS_CONTAINER( w ) )
	{
	GList *	glf;
	GList *	gl;

	gl= glf= gtk_container_children( GTK_CONTAINER( w ) );

	while( gl )
	    {
	    GtkWidget *	c= (GtkWidget *)gl->data;

	    if  ( GTK_IS_MENU_ITEM( c )		)
		{
		if  ( GTK_MENU_ITEM( c ) == it	)
		    { return *pIdx;	}

		(*pIdx)++;
		}
	    else{
		if  ( GTK_IS_CONTAINER( c ) )
		    {
		    int	rval= appGuiGetOptionmenuFindItem( pIdx, c, it );

		    if  ( rval >= 0 )
			{ return rval;	}
		    }
		}

	    gl= gl->next;
	    }

	if  ( glf )
	    { g_list_free( glf );	}
	}

    return -1;
    }

int appGuiGetOptionmenuItemIndexGtk(	AppOptionmenu *	aom,
					GtkMenuItem *	it )
    {
    int		idx= 0;

    return appGuiGetOptionmenuFindItem( &idx, aom->aomPulldown, it );
    }

void appOptionmenuItemSetVisibility(	APP_WIDGET	w,
					int		visible )
    {
    visible= ( visible != 0 );

    if  ( GTK_WIDGET_VISIBLE( w ) == visible )
	{ return;	}

    if  ( visible )
	{ gtk_widget_show( w );	}
    else{ gtk_widget_hide( w );	}

    return;
    }

#   endif
