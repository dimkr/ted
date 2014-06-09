/************************************************************************/
/*									*/
/*  Gtk specific document configuration.				*/
/*									*/
/*  Thanks to Andrea Frome who contributed the original code.		*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	<locale.h>

#   include	"appFrame.h"

#   include	<appDebugon.h>

#   ifdef USE_GTK

/************************************************************************/
/*									*/
/*  Thanks to Andrea Frome who contributed the original code.		*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Event handler to keep track of resize events for the document.	*/
/*  Both resize events of the shel widget and the document widget are	*/
/*  handeled to keep track of the difference in size.			*/
/*									*/
/*  The difference in size is used to set the maximum size of the	*/
/*  shell widget in the window manager.					*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( appDocTopConfigure, w, voided, event )
    {
    EditDocument *	ed= (EditDocument *)voided;

    if  ( event->type == GDK_MAP && ed->edShellExtraWidth < 0 )
	{
	appDocumentCalculateExtraSize( ed );
	ed->edMapped= 1;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make a document window.						*/
/*									*/
/*  1)  Allocate a document.						*/
/*  3)  Make a shell to contain everything.				*/
/*  4)  Make the window pane.						*/
/*									*/
/************************************************************************/

static int appDocMakeMainWindow(	EditDocument *		ed,
					EditApplication *	ea )
    {
    APP_BITMAP_IMAGE	pixmap= (APP_BITMAP_IMAGE)0;
    APP_BITMAP_MASK	mask= (APP_BITMAP_MASK)0;

    const char *	fullTitle;
    const char *	fullIconName;

    const int		spacing= 0;

    if  ( ea->eaDocumentIcon						&&
	  appGetImagePixmap( ea, ea->eaDocumentIcon, &pixmap, &mask )	)
	{ SDEB(ea->eaDocumentIcon); return -1;	}

    if  ( appFormatDocumentTitle( &fullTitle, &fullIconName, ea, ed->edTitle ) )
	{ SDEB(ed->edTitle); return -1;	}

    ed->edToplevel.atTopWidget= gtk_window_new( GTK_WINDOW_TOPLEVEL );

    gtk_window_set_title( GTK_WINDOW( ed->edToplevel.atTopWidget ), fullTitle );

    gtk_window_set_position( GTK_WINDOW( ed->edToplevel.atTopWidget ),
						GTK_WIN_POS_CENTER );
    gtk_window_set_policy( GTK_WINDOW( ed->edToplevel.atTopWidget ), 
						    TRUE, TRUE, FALSE );

    gtk_widget_realize( ed->edToplevel.atTopWidget );

    gdk_window_set_icon( ed->edToplevel.atTopWidget->window,
					ed->edToplevel.atTopWidget->window,
					pixmap, mask );
    gdk_window_set_icon_name( ed->edToplevel.atTopWidget->window,
								fullIconName );

    appSetCloseCallback( ed->edToplevel.atTopWidget, ea,
					appDocFileCloseCallback, (void *)ed );

    appSetDestroyCallback( ed->edToplevel.atTopWidget,
					appDestroyEditDocument, (void *)ed );

    ed->edMainWindow= gtk_vbox_new( FALSE, spacing );
    gtk_container_add( GTK_CONTAINER( ed->edToplevel.atTopWidget ),
							ed->edMainWindow );
    gtk_widget_show( ed->edMainWindow );

    ed->edToplevel.atAccelGroup= gtk_accel_group_new();

#   if GTK_MAJOR_VERSION < 2
    gtk_accel_group_attach( ed->edToplevel.atAccelGroup,
				    GTK_OBJECT( ed->edToplevel.atTopWidget ) );
#   else
    gtk_window_add_accel_group( GTK_WINDOW( ed->edToplevel.atTopWidget ),
					    ed->edToplevel.atAccelGroup );
#   endif

    ed->edMenuBar= gtk_menu_bar_new();
    gtk_widget_show( ed->edMenuBar );
    gtk_box_pack_start( GTK_BOX( ed->edMainWindow ),
					    ed->edMenuBar, FALSE, TRUE, 0 );

    appDocFillMenu( ed );

    return 0;
    }

static int appDocMakeScrolledWindow(	EditDocument *		ed,
					EditApplication *	ea )
    {
    ed->edScrolledWindow= gtk_table_new( 2, 2, FALSE );

    gtk_widget_show( ed->edScrolledWindow );
    gtk_box_pack_start( GTK_BOX( ed->edMainWindow ),
				    ed->edScrolledWindow, TRUE, TRUE, 0 );

    /*****/

    ed->edVerticalScrollbar= gtk_vscrollbar_new( NULL );
    ed->edVerticalAdjustment=
	    gtk_range_get_adjustment( GTK_RANGE( ed->edVerticalScrollbar ) );

    gtk_table_attach( GTK_TABLE( ed->edScrolledWindow ),
			ed->edVerticalScrollbar,
			1, 2,
			0, 1,
			GTK_FILL,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			0, 0 );

    gtk_widget_show( ed->edVerticalScrollbar );

    /*****/

    ed->edHorizontalScrollbar= gtk_hscrollbar_new( NULL );
    ed->edHorizontalAdjustment=
	    gtk_range_get_adjustment( GTK_RANGE( ed->edHorizontalScrollbar ) );

    gtk_table_attach( GTK_TABLE( ed->edScrolledWindow ),
			ed->edHorizontalScrollbar,
			0, 1,
			1, 2,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			GTK_FILL,
			0, 0 );

    gtk_widget_show( ed->edHorizontalScrollbar );

    return 0;
    }

int appMakeDocumentWidget(	EditApplication *	ea,
				EditDocument *		ed )
    {
    APP_WIDGET		form;

    appDocumentRulerWidth( ea, ed );

    /*  2  */
    form= gtk_table_new( 3, 3, FALSE );

    gtk_table_attach( GTK_TABLE( ed->edScrolledWindow ),
			form,
			0, 1,
			0, 1,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			0, 0 );

    gtk_widget_show( form );

    ed->edTopRulerWidget= gtk_drawing_area_new();

    gtk_table_attach( GTK_TABLE( form ),
			ed->edTopRulerWidget,
			0, 3,
			0, 1,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			GTK_FILL,
			0, 0 );

    gtk_widget_show( ed->edTopRulerWidget );

    ed->edBottomRulerWidget= gtk_drawing_area_new();

    gtk_table_attach( GTK_TABLE( form ),
			ed->edBottomRulerWidget,
			0, 3,
			2, 3,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			GTK_FILL,
			0, 0 );

    gtk_widget_show( ed->edBottomRulerWidget );

    ed->edLeftRulerWidget= gtk_drawing_area_new();

    gtk_table_attach( GTK_TABLE( form ),
			ed->edLeftRulerWidget,
			0, 1,
			1, 2,
			GTK_FILL,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			0, 0 );

    gtk_widget_show( ed->edLeftRulerWidget );

    ed->edRightRulerWidget= gtk_drawing_area_new();

    gtk_table_attach( GTK_TABLE( form ),
			ed->edRightRulerWidget,
			2, 3,
			1, 2,
			GTK_FILL,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			0, 0 );

    gtk_widget_show( ed->edRightRulerWidget );

    ed->edDocumentWidget= gtk_drawing_area_new();

    gtk_table_attach( GTK_TABLE( form ),
			ed->edDocumentWidget,
			1, 2,
			1, 2,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			0, 0 );

    gtk_widget_show( ed->edDocumentWidget );

    GTK_WIDGET_SET_FLAGS( ed->edDocumentWidget, GTK_CAN_FOCUS );
    gtk_window_set_focus( GTK_WINDOW( ed->edToplevel.atTopWidget ),
						    ed->edDocumentWidget );

    return 0;
    }

static int appDocMakeToolbar(	EditDocument *		ed,
				EditApplication *	ea )
    {
#   if 0
    Arg			al[20];
    int			ac= 0;

    Display *		display= ea->eaDisplay;
    int			screen= DefaultScreen( ea->eaDisplay );

    int			verPixPerCM;

    verPixPerCM= ( 10* DisplayHeight( display, screen ) )/
					DisplayHeightMM( display, screen );
    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNheight,		verPixPerCM ); ac++;

    ed->edToolbar= XmCreateForm( ed->edMainWindow, WIDGET_NAME, al, ac );

#   endif
    return 0;
    }

int appFinishDocumentWindow(	EditDocument *		ed )
    {
    EditApplication *	ea= ed->edApplication;

    /*  3  */
    if  ( appDocMakeMainWindow( ed, ea )		||
	  appDocMakeToolbar( ed, ea )			||
	  appDocMakeScrolledWindow( ed, ea )		||
	  (*ea->eaMakeDocumentWidget)( ea, ed )		)
	{ LDEB(1); return -1; }

    gtk_signal_connect( GTK_OBJECT( ed->edDocumentWidget ),
				    "configure_event",
				    (GtkSignalFunc)appDocConfigure,
				    (void *)ed );

    gtk_signal_connect( GTK_OBJECT( ed->edToplevel.atTopWidget ), 
				    "map_event",
				    (GtkSignalFunc)appDocTopConfigure,
				    (void *)ed );

    gtk_signal_connect( GTK_OBJECT( ed->edDocumentWidget ),
				    "selection_clear_event",
				    (GtkSignalFunc)appDocCopyPasteHandler,
				    (void *)ed );

    gtk_signal_connect( GTK_OBJECT( ed->edDocumentWidget ),
				    "selection_get",
				    (GtkSignalFunc)appDocReplyToCopyRequest,
				    (void *)ed );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Prevent the Shell that contains a document from being resized	*/
/*  beyond normal limits.						*/
/*									*/
/************************************************************************/

void appSetShellConstraints(	EditDocument *		ed )
    {
    int			wide;
    int			high;

    gint		flags= 0;
    int			shrink= 0;
    gint		maxWidth;
    gint		maxHeight;

    AppDrawingData *	add= &(ed->edDrawingData);

    appDrawGetSizeOfWidget( &wide, &high, ed->edToplevel.atTopWidget );

    maxWidth= add->addBackRect.drX1- add->addBackRect.drX0;
    maxHeight= add->addBackRect.drY1- add->addBackRect.drY0;

    if  ( ed->edShellExtraWidth >= 0 )
	{
	GtkRequisition		gr;

	maxWidth= add->addBackRect.drX1- add->addBackRect.drX0;
	maxWidth += ed->edShellExtraWidth;

	gr.width= gr.height= 0;
	gtk_widget_get_child_requisition( ed->edMenuBar, &gr );
	if  ( maxWidth < gr.width )
	    { maxWidth=  gr.width;	}

	flags |= GDK_HINT_MAX_SIZE;

	if  ( wide > maxWidth )
	    {
	    wide= maxWidth; shrink= 1;
	    ed->edVisibleRect.drX1 += maxWidth- wide;
	    }
	}

    if  ( ed->edShellExtraHeight >= 0 )
	{
	maxHeight= add->addBackRect.drY1- add->addBackRect.drY0;

	if  ( maxHeight < add->addPaperRect.drY1- add->addPaperRect.drY0 )
	    { maxHeight=  add->addPaperRect.drY1- add->addPaperRect.drY0; }

	maxHeight += ed->edShellExtraHeight;

	flags |= GDK_HINT_MAX_SIZE;

	if  ( high > maxHeight )
	    {
	    high= maxHeight; shrink= 1;
	    ed->edVisibleRect.drY1 += maxHeight- high;
	    }
	}

    if  ( flags )
	{
	gdk_window_set_hints( ed->edToplevel.atTopWidget->window,
							0, 0,
							wide, high,
							maxWidth, maxHeight,
							flags );
	}
    if  ( shrink )
	{
	gdk_window_resize( ed->edToplevel.atTopWidget->window,
							wide, high );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Finish the setup of a document window.				*/
/*									*/
/*  2)  The drawing area does not deliver focus out events so install	*/
/*	the focus event handler on the top level widget.		*/
/*									*/
/************************************************************************/

int appFinishDocumentSetup(	EditApplication *	ea,
				EditDocument *		ed )
    {
    AppDrawingData *	add= &(ed->edDrawingData);

    ed->edInputContext= 0;

    appSetDrawingDataForWidget( ed->edDocumentWidget,
					    ea->eaMagnification, add );

    appDrawSetRedrawHandler( ed->edDocumentWidget,
					    appDocExposeHandler, (void *)ed );

    if  ( ea->eaObserveFocus )
	{
	/*  2  */
#	if 1
	appGuiSetFocusChangeHandler( ed->edToplevel.atTopWidget,
					    ea->eaObserveFocus, (void *)ed );
#	else
	appGuiSetFocusChangeHandler( ed->edDocumentWidget,
					    ea->eaObserveFocus, (void *)ed );
#	endif
	}

    if  ( ea->eaDocumentMouseHandler )
	{
	appDrawSetButtonPressHandler( ed->edDocumentWidget,
				    ea->eaDocumentMouseHandler, (void *)ed );
	}

    if  ( ea->eaDocumentKeyboardHandler )
	{
	appDrawSetKeyboardHandler( ed->edDocumentWidget,
				ea->eaDocumentKeyboardHandler, (void *)ed );
	}

    gtk_signal_connect( GTK_OBJECT( ed->edVerticalAdjustment ),
			    "value_changed", 
			    (GtkSignalFunc)appDocVerticalScrollbarCallback,
			    (void *)ed );

    gtk_signal_connect( GTK_OBJECT( ed->edHorizontalAdjustment ),
			    "value_changed", 
			    (GtkSignalFunc)appDocHorizontalScrollbarCallback,
			    (void *)ed );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Management of the 'Windows' menu options.				*/
/*									*/
/************************************************************************/

void appSetWindowsItemState(	APP_WIDGET	menu,
				EditDocument *	ed,
				int		changed )
    {
    GList *	glf;
    GList *	gl;

    gl= glf= gtk_container_children( GTK_CONTAINER( menu ) );

    while( gl )
	{
	GtkWidget *	child= (GtkWidget *)gl->data;
	void *		voided;
	
	voided= gtk_object_get_user_data( GTK_OBJECT( child ) );

	if  ( voided == (void *)ed )
	    { appGuiSetToggleItemState( child, changed ); }

	gl= gl->next;
	}

    if  ( glf )
	{ g_list_free( glf );	}

    return;
    }

void appRemoveWindowsOption(	APP_WIDGET		menu,
				EditDocument *		oldEd )
    {
    GList *	glf;
    GList *	gl;

    gl= glf= gtk_container_children( GTK_CONTAINER( menu ) );

    while( gl )
	{
	GtkWidget *	child= (GtkWidget *)gl->data;
	void *		voided;
	
	voided= gtk_object_get_user_data( GTK_OBJECT( child ) );

	if  ( voided == (void *)oldEd )
	    { gtk_widget_destroy( child );	}

	gl= gl->next;
	}

    if  ( glf )
	{ g_list_free( glf );	}

    return;
    }

void appRenameWindowsOption(		APP_WIDGET		menu,
					EditDocument *		ed,
					const char *		title )
    {
    GList *	glf;
    GList *	gl;

    gl= glf= gtk_container_children( GTK_CONTAINER( menu ) );

    while( gl )
	{
	GtkWidget *	child= (GtkWidget *)gl->data;
	void *		voided;
	
	voided= gtk_object_get_user_data( GTK_OBJECT( child ) );

	if  ( voided == (void *)ed )
	    { appGuiSetToggleItemLabel( child, title ); }

	gl= gl->next;
	}

    if  ( glf )
	{ g_list_free( glf );	}

    return;
    }

#   endif
