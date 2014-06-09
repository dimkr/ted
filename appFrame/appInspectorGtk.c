#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"appFrame.h"
#   include	"appSystem.h"
#   include	<appGeoString.h>

#   ifdef USE_GTK

#   include	<appDebugon.h>

void appInspectorMakePageParent(	AppInspector *	ai )
    {
    GtkWidget *		notebook;

    notebook= gtk_notebook_new();

    gtk_notebook_set_show_tabs( GTK_NOTEBOOK( notebook ), FALSE );
    gtk_notebook_set_show_border( GTK_NOTEBOOK( notebook ), FALSE );

    gtk_widget_show( notebook );

    gtk_box_pack_start( GTK_BOX( ai->aiPaned ), notebook, FALSE, TRUE, 0 );

    ai->aiPageParent= notebook;

    return;
    }

void appInspectorChoosePage(	AppInspector *		ai,
				int			andMenu,
				int			subject )
    {
    gtk_notebook_set_page( GTK_NOTEBOOK( ai->aiPageParent ), subject );

    if  ( andMenu )
	{ appSetOptionmenu( &(ai->aiSubjectOptionmenu), subject );	}

    return;
    }

static APP_OITEM_CALLBACK_H( appInspectorPageChosen, w, vai )
    {
    AppInspector *	ai= (AppInspector *)vai;

    int			subject;

    const int		andMenu= 0;

    subject= appGuiGetOptionmenuItemIndex( &(ai->aiSubjectOptionmenu), w );
    if  ( subject >= ai->aiSubjectCount )
	{ LDEB(ai->aiSubjectCount); return;	}

    if  ( ai->aiNotifySubject )
	{ (*ai->aiNotifySubject)( ai, ai->aiCurrentSubject, subject ); }

    appInspectorChoosePage( ai, andMenu, subject );

    ai->aiCurrentSubject= subject;

    return;
    }

void appMakeVerticalInspectorPage(	APP_WIDGET *	pPage,
					APP_WIDGET *	pMenuitem,
					AppInspector *	ai,
					const char *	label )
    {
    GtkWidget *		item;
    GtkWidget *		page;

    page= gtk_vbox_new( FALSE, COLUMN_SPACING_GTK );

    gtk_notebook_append_page( GTK_NOTEBOOK( ai->aiPageParent ),
						    page, (GtkWidget *)0 );

    gtk_widget_show( page );

    item= appAddItemToOptionmenu( &(ai->aiSubjectOptionmenu), label,
				    appInspectorPageChosen, (void *)ai );

    *pPage= page; *pMenuitem= item; return;
    }

void appInspectorEnablePage(	AppInspector *	ai,
				int		subject,
				int		enabled )
    {
    GtkWidget *		page;

    page= gtk_notebook_get_nth_page( GTK_NOTEBOOK( ai->aiPageParent ),
								subject );

    gtk_widget_set_sensitive( page, enabled != 0 );

    gtk_widget_set_sensitive( ai->aiSubjects[subject].isMenuitem,
							    enabled != 0 );

    return;
    }

#   endif /*  USE_GTK	*/
