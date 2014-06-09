#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	<appDebugon.h>

#   ifdef USE_GTK

#   include	"appFrame.h"
#   include	<appGeoString.h>

/************************************************************************/
/*									*/
/*  Retrieve configurable resource values from the GUI environment.	*/
/*									*/
/*  1)  This should be done wih something like the GNU message catalog	*/
/*	system. For the moment, just install the default values.	*/
/*									*/
/************************************************************************/

void appGuiGetResourceValues(	int *				pGotResources,
				EditApplication *		ea,
				void *				pValues,
				AppConfigurableResource *	acrList,
				int				acrCount )
    {
    AppConfigurableResource *	acr;
    char *			values= (char *)pValues;
    int				i;

    if  ( *pGotResources )
	{ LDEB(*pGotResources); return;	}

    if  ( ! *pGotResources )
	{ appSetResourceDefaults( ea, acrList, acrCount );	}

    acr= acrList;
    for ( i= 0; i < acrCount; acr++, i++ )
	{
	*((const char **)(values+acr->acrStructOffset))= acr->acrDefaultValue;
	}

    if  ( ea->eaToplevel.atTopWidget )
	{ appGuiGetResourceValuesGtkX( ea, pValues, acrList, acrCount ); }

    *pGotResources= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  Destroy all children of a widget.					*/
/*									*/
/************************************************************************/

void appEmptyParentWidget(	APP_WIDGET		parent )
    {
    gtk_container_foreach( GTK_CONTAINER( parent ),
				    (GtkCallback)gtk_widget_destroy, NULL );
    }

/************************************************************************/
/*									*/
/*  Make the initial connection to the gui environment.			*/
/*									*/
/************************************************************************/

int appGuiInitApplication(	EditApplication *	ea,
				int *			pArgc,
				char ***		pArgv )
    {
    int			argc= *pArgc;
    char **		argv= *pArgv;

    GtkWidget *		gWindow;
    GtkAccelGroup *	gAccelGroup;

    gtk_set_locale();

    gtk_init( &argc, &argv );

    gWindow= gtk_window_new( GTK_WINDOW_TOPLEVEL );
    ea->eaToplevel.atTopWidget= gWindow;

    /* user can't minimize or maximize */
    gtk_window_set_policy (GTK_WINDOW (gWindow), FALSE, FALSE, FALSE);
    gtk_window_set_position (GTK_WINDOW (gWindow), GTK_WIN_POS_CENTER);

    appSetCloseCallback( gWindow, ea, appAppWmClose, (void*)ea );

    /* 
     * Facilities for making menu option accelerators.
     */
    gAccelGroup = gtk_accel_group_new();
    ea->eaToplevel.atAccelGroup= gAccelGroup;

#   if GTK_MAJOR_VERSION < 2
    gtk_accel_group_attach (gAccelGroup, GTK_OBJECT (gWindow));
#   else
    gtk_window_add_accel_group( GTK_WINDOW( gWindow ), gAccelGroup );
#   endif

    /* 
     * Do this here so that we don't get an error when
     * we create the pixmap using the window.
     */
    gtk_widget_realize( gWindow );

    ea->eaScreenPixelsWide= gdk_screen_width();
    ea->eaScreenPixelsHigh= gdk_screen_height();

    *pArgc= argc;
    *pArgv= argv;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a column in a top level widget (Window)			*/
/*									*/
/************************************************************************/

void appGuiInsertColumnInWindow(	APP_WIDGET *	pColumn,
					APP_WIDGET	shell )
    {
    GtkWidget *		column;

    column= gtk_vbox_new( FALSE, COLUMN_SPACING_GTK );

    gtk_widget_show( column );

    gtk_container_add( GTK_CONTAINER( shell ), column );

    *pColumn= column; return;
    }

/************************************************************************/
/*									*/
/*  Insert a menubar as the first row of a column.			*/
/*									*/
/************************************************************************/

void appGuiInsertMenubarInColumn(	APP_WIDGET *	pMenubar,
					APP_WIDGET	column )
    {
    GtkWidget *		menubar= gtk_menu_bar_new();

    gtk_widget_show( menubar );

    gtk_box_pack_start( GTK_BOX( column ), menubar, FALSE, TRUE, 0 );

    *pMenubar= menubar;
    return;
    }

/************************************************************************/
/*									*/
/*  Calculate magnification factors.					*/
/*									*/
/************************************************************************/

void appGetFactors(	EditApplication *	ea,
			double *		pHorPixPerMM,
			double *		pVerPixPerMM,
			double *		pXfac,
			double *		pYfac )
    {
    double		horPixPerMM;
    double		verPixPerMM;
    double		xfac;
    double		yfac;

    horPixPerMM= (double)gdk_screen_width()/ (double)gdk_screen_width_mm();
    verPixPerMM= (double)gdk_screen_height()/ (double)gdk_screen_height_mm();

    xfac=  ( 25.4/ ( 20.0* 72.0 ) )* horPixPerMM;
    yfac=  ( 25.4/ ( 20.0* 72.0 ) )* verPixPerMM;

    *pHorPixPerMM= horPixPerMM;
    *pVerPixPerMM= verPixPerMM;

    *pXfac= ea->eaMagnification* xfac;
    *pYfac= ea->eaMagnification* yfac;

    return;
    }

/************************************************************************/
/*									*/
/*  Enable/Dispable a widget.						*/
/*									*/
/*  NOTE that for text edit boxes there is a different routine.		*/
/*									*/
/************************************************************************/

void appGuiEnableWidget(	APP_WIDGET		w,
				int			on_off )
    {
    gtk_widget_set_sensitive( w, on_off != 0 );
    }


void appGuiSetWidgetVisible(	APP_WIDGET		w,
				int			on_off )
    {
    if  ( on_off )
	{ gtk_widget_show( w );	}
    else{ gtk_widget_hide( w );	}
    }

/************************************************************************/
/*									*/
/*  Use the text of an option and the name of the application as a	*/
/*  widget title.							*/
/*									*/
/************************************************************************/

void appSetShellTitle(	APP_WIDGET		shell,
			APP_WIDGET		option,
			const char *		applicationName )
    {
    char *	title;

    if  ( option )
	{
	char *		s;

	s= appGetTextFromMenuOption( option );

	title= (char *)malloc( strlen( applicationName )+ 1+ strlen( s )+ 1 );
	sprintf( title, "%s %s", applicationName, s );

	appFreeTextFromMenuOption( s );
	}
    else{ title= strdup( applicationName );	}

    gtk_window_set_title( GTK_WINDOW( shell ), title );

    free( title );
    }

void appGuiSetShellTitle(	APP_WIDGET		shell,
				const char *		fullTitle )
    {
    gtk_window_set_title( GTK_WINDOW( shell ), fullTitle );

    return;
    }

void appGuiSetIconTitle(	APP_WIDGET		shell,
				const char *		fullIconName )
    {
    gdk_window_set_icon_name( shell->window, fullIconName );
    return;
    }

void appGuiLowerShellWidget(	APP_WIDGET	shell )
    {
    gdk_window_lower( shell->window );

    return;
    }

/************************************************************************/
/*									*/
/*  Change the label/state of a toggle button.				*/
/*									*/
/************************************************************************/

void appGuiSetToggleLabel(	APP_WIDGET		toggle,
				const char *		text )
    {
    if  ( ! appGuiGtkSetChildLabel( toggle, text ) )
	{ SDEB(text);	}
    }

void appGuiSetToggleState(	APP_WIDGET		toggle,
				int			set )
    {
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( toggle ), set != 0 );
    }

int appGuiGetToggleState(	APP_WIDGET		toggle )
    {
    return gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( toggle ) ) != 0;
    }

int appGuiGetToggleStateFromCallbackGtk( GtkToggleButton *	toggle )
    {
    return gtk_toggle_button_get_active( toggle ) != 0;
    }

APP_WIDGET appMakeToggleInRow(	APP_WIDGET		row,
				const char *		text,
				APP_TOGGLE_CALLBACK_T	callback,
				void *			through,
				int			position )
    {
    GtkWidget *	toggle= gtk_check_button_new_with_label( text );

    gtk_table_attach( GTK_TABLE( row ),
			toggle,
			position, position+ 1,
			0, 1,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			ROW_XPADDING_GTK, ROW_YPADDING_GTK );

    gtk_widget_show( toggle );

    gtk_signal_connect( GTK_OBJECT( toggle ), "toggled",
					    (GtkSignalFunc)callback, through );

    return toggle;
    }

/************************************************************************/
/*									*/
/*  Make a row in a column: Rows are implemented as..			*/
/*									*/
/************************************************************************/

APP_WIDGET appMakeRowInColumn(	APP_WIDGET	column,
				int		columnCount,
				int		heightResizable )
    {
    gboolean	expand= FALSE;
    gboolean	fill= TRUE;

    GtkWidget *	row= gtk_table_new( 1, columnCount, TRUE );

    if  ( heightResizable )
	{ expand= TRUE;	}

    gtk_box_pack_start( GTK_BOX( column ), row, expand, fill, 0 );

    gtk_widget_show( row );

    return row;
    }

void appMakeButtonInRow(	APP_WIDGET *		pButton,
				APP_WIDGET		row,
				const char *		text,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through,
				int			position,
				int			showAsDefault )
    {
    GtkWidget *		button= gtk_button_new_with_label( text );

    gtk_table_attach( GTK_TABLE( row ),
			button,
			position, position+ 1,
			0, 1,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			ROW_XPADDING_GTK, ROW_YPADDING_GTK );

    gtk_widget_show( button );

    gtk_signal_connect( GTK_OBJECT( button ), "clicked",
					(GtkSignalFunc)callback, through );

    *pButton= button;

    return;
    }

void appGuiSetScrollbarValues(		APP_WIDGET	scrollbar,
					int		value,
					int		sliderSize )
    {
    GtkAdjustment *	adjustment;

    adjustment= gtk_range_get_adjustment( GTK_RANGE( scrollbar ) );

    adjustment->value= value;
    adjustment->page_size= sliderSize;

    gtk_adjustment_changed( adjustment );

    return;
    }

void appGuiGetScrollbarValues(		int *		pValue,
					int *		pSliderSize,
					APP_WIDGET	scrollbar )
    {
    GtkAdjustment *	adjustment;

    adjustment= gtk_range_get_adjustment( GTK_RANGE( scrollbar ) );

    *pValue= adjustment->value;
    *pSliderSize= adjustment->page_size;

    return;
    }

void appGuiChangeButtonText(	APP_WIDGET	button,
				const char *	label )
    {
    if  ( ! appGuiGtkSetChildLabel( button, label ) )
	{ SDEB(label);	}
    }

void appShowShellWidget(		APP_WIDGET		shell )
    {
    gtk_widget_show( shell );
    gdk_window_raise( shell->window );
    }

void appHideShellWidget(		APP_WIDGET		shell )
    {
    gtk_widget_hide( shell );
    }

void appDestroyShellWidget(		APP_WIDGET		shell )
    {
    gtk_widget_destroy( shell );
    }

/************************************************************************/
/*									*/
/*  Insert a separator as a row of a column.				*/
/*									*/
/************************************************************************/

void appGuiInsertSeparatorInColumn(	APP_WIDGET *	pSeparator,
					APP_WIDGET	column )
    {
    GtkWidget *		separator= gtk_hseparator_new();

    gtk_box_pack_start( GTK_BOX( column ), separator, FALSE, TRUE, 0 );

    gtk_widget_show( separator );

    *pSeparator= separator;

    return;
    }

void appMakeColumnInRow(	APP_WIDGET *	pColumn,
				APP_WIDGET	row,
				int		position,
				int		colspan )
    {
    GtkWidget *		column= gtk_vbox_new( FALSE, COLUMN_SPACING_GTK );

    gtk_table_attach( GTK_TABLE( row ),
			column,
			position, position+ colspan,
			0, 1,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			ROW_XPADDING_GTK, ROW_YPADDING_GTK );

    gtk_widget_show( column );

    *pColumn= column;
    }

void appGuiMakeDrawingAreaInColumn( APP_WIDGET *		pDrawing,
				    APP_WIDGET			column,
				    int				wide,
				    int				high,
				    int				heightResizable,
				    APP_EVENT_HANDLER_T		redraw,
				    void *			through )
    {
    GtkWidget *	drawing= gtk_drawing_area_new();
    gboolean	expand= FALSE;
    gboolean	fill= TRUE;

    if  ( wide < 0 )
	{ wide= 0;	}
    if  ( high < 0 )
	{ high= 0;	}

    if  ( high <= 0 || heightResizable )
	{ expand= TRUE;	}

    gtk_box_pack_start( GTK_BOX( column ), drawing, expand, fill, 0 );

    gtk_drawing_area_size( GTK_DRAWING_AREA( drawing ), wide, high );

    gtk_widget_show( drawing );

    *pDrawing= drawing;

    if  ( redraw )
	{ appDrawSetRedrawHandler( drawing, redraw, through );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make a tool with a vertical organisation.				*/
/*									*/
/************************************************************************/

void appMakeVerticalTool(	APP_WIDGET *		pShell,
				APP_WIDGET *		pColumn,
				EditApplication *	ea,
				APP_BITMAP_IMAGE	iconPixmap,
				APP_BITMAP_MASK		iconMask,
				const char *		widgetName,
				int			userResizable,
				APP_WIDGET		option,
				APP_CLOSE_CALLBACK_T	closeCallback,
				void *			through )
    {
    GtkWidget *		shell;
    GtkWidget *		column;

    shell= gtk_window_new( GTK_WINDOW_TOPLEVEL );

    appSetCloseCallback( shell, ea, closeCallback, through );

    appSetShellTitle( shell, option, ea->eaApplicationName );

    column= gtk_vbox_new( FALSE, COLUMN_SPACING_GTK );

    gtk_container_set_border_width( GTK_CONTAINER( shell ), WINDOW_BORDER_GTK );
    gtk_container_add( GTK_CONTAINER( shell ), column );

    gtk_widget_show( column );

    *pShell= shell; *pColumn= column; return;
    }

/************************************************************************/
/*									*/
/*  Insert a frame in a column. The frame is to be a column itself.	*/
/*									*/
/************************************************************************/

static void appGuiGtkMakeFrameInColumn(	APP_WIDGET *		pFrame,
					APP_WIDGET *		pTable,
					APP_WIDGET		parent,
					APP_WIDGET		child,
					const char *		title )
    {
    GtkWidget *		frame;
    GtkWidget *		table;

    frame= gtk_frame_new( title );

    gtk_frame_set_shadow_type( GTK_FRAME( frame ), GTK_SHADOW_ETCHED_IN );

    gtk_box_pack_start( GTK_BOX( parent ), frame, FALSE, TRUE, 0 );

    gtk_widget_show( frame );

    table= gtk_table_new( 1, 1, TRUE );

    gtk_container_add( GTK_CONTAINER( frame ), table );

    gtk_widget_show( table );

    gtk_table_attach( GTK_TABLE( table ),
			child,
			0, 1,
			0, 1,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			FRAME_BORDER_GTK, FRAME_BORDER_GTK );

    gtk_widget_show( child );

    *pFrame= frame; *pTable= table; return;
    }

void appMakeColumnFrameInColumn(	APP_WIDGET *		pFrame,
					APP_WIDGET *		pPaned,
					APP_WIDGET		parent,
					const char *		title )
    {
    GtkWidget *		frame;
    GtkWidget *		table;
    GtkWidget *		paned;

    paned= gtk_vbox_new( FALSE, COLUMN_SPACING_GTK );

    appGuiGtkMakeFrameInColumn( &frame, &table, parent, paned, title );

    *pFrame= frame; *pPaned= paned; return;
    }

/************************************************************************/
/*									*/
/*  Set the title of a frame.						*/
/*									*/
/************************************************************************/

int appGuiSetFrameTitle(		APP_WIDGET		frame,
					const unsigned char *	title )
    {
    gtk_frame_set_label( GTK_FRAME( frame ), title );
    }

/************************************************************************/
/*									*/
/*  Insert a frame in a column. The frame is to be a row itself.	*/
/*									*/
/************************************************************************/

void appMakeRowFrameInColumn(	APP_WIDGET *		pFrame,
				APP_WIDGET *		pRow,
				APP_WIDGET		parent,
				int			columnCount,
				const char *		title )
    {
    GtkWidget *		frame;
    GtkWidget *		table;
    GtkWidget *		row;

    row= gtk_table_new( 1, columnCount, TRUE );

    appGuiGtkMakeFrameInColumn( &frame, &table, parent, row, title );

    *pFrame= frame; *pRow= row; return;
    }

int appGuiGetScrollbarValueFromCallback( APP_WIDGET		scrollbar,
					void *			voidcbs )
    {
    GtkAdjustment *	adjustment= (GtkAdjustment *)scrollbar;

    return adjustment->value;
    }

void appMakeImageInColumn(	APP_WIDGET *		pImage,
				APP_WIDGET		column,
				APP_BITMAP_IMAGE	pixmap,
				APP_BITMAP_MASK		mask )
    {
    GtkWidget *		image= gtk_pixmap_new( pixmap, mask );

    gtk_box_pack_start( GTK_BOX( column ), image, FALSE, TRUE, 0 );

    gtk_widget_show( image );

    *pImage= image; return;
    }

/************************************************************************/
/*									*/
/*  Run a loop while the left mouse button is down.			*/
/*									*/
/************************************************************************/

typedef struct DragLoop
    {
    APP_EVENT_HANDLER_T		dlUpHandler;
    APP_EVENT_HANDLER_T		dlMoveHandler;
    int				dlTimerInterval;
    GtkFunction			dlTimerHandler;
    void *			dlThrough;

    guint			dlIntervalId;

    guint			dlUpId;
    guint			dlDownId;
    guint			dlMoveId;
    } DragLoop;

static int appDragTick(	void *		vdl )
    {
    DragLoop *		dl= (DragLoop *)vdl;

    (*dl->dlTimerHandler)( dl->dlThrough );

    if  ( dl->dlTimerHandler && dl->dlTimerInterval > 0 )
	{
	dl->dlIntervalId= gtk_timeout_add( dl->dlTimerInterval,
						    appDragTick, (void *)dl );
	}

    return 0;
    }

static void appDragMouseUp(		APP_WIDGET		w,
					APP_EVENT *		event,
					void *			vdl )
    {
    DragLoop *		dl= (DragLoop *)vdl;

    if  ( dl->dlIntervalId )
	{
	gtk_timeout_remove( dl->dlIntervalId );
	dl->dlIntervalId= 0;
	}

    if  ( dl->dlUpHandler )
	{ (*dl->dlUpHandler)( w, event, dl->dlThrough );	}

    gtk_main_quit();
    }

static void appDragMouseMove(		APP_WIDGET		w,
					APP_EVENT *		event,
					void *			vdl )
    {
    DragLoop *		dl= (DragLoop *)vdl;

    if  ( dl->dlMoveHandler )
	{ (*dl->dlMoveHandler)( w, event, dl->dlThrough );	}
    }

void appRunDragLoop(	APP_WIDGET		w,
			EditApplication *	ea,
			APP_EVENT *		downEvent,
			APP_EVENT_HANDLER_T	upHandler,
			APP_EVENT_HANDLER_T	moveHandler,
			int			timerInterval,
			APP_TIMER_CALLBACK	timerHandler,
			void *			through )
    {
    DragLoop		dl;

    dl.dlUpHandler= upHandler;
    dl.dlMoveHandler= moveHandler;
    dl.dlTimerInterval= timerInterval;
    dl.dlTimerHandler= timerHandler;
    dl.dlThrough= through;

    dl.dlIntervalId= 0;

    gtk_widget_add_events( w,
			GDK_BUTTON_RELEASE_MASK | GDK_BUTTON1_MOTION_MASK );

    dl.dlUpId= gtk_signal_connect( GTK_OBJECT( w ),
					    "button_release_event",
					    (GtkSignalFunc)appDragMouseUp,
					    (void *)&dl );
    dl.dlDownId= gtk_signal_connect( GTK_OBJECT( w ),
					    "button_press_event",
					    (GtkSignalFunc)appDragMouseUp,
					    (void *)&dl );
    dl.dlMoveId= gtk_signal_connect( GTK_OBJECT( w ),
					    "motion_notify_event",
					    (GtkSignalFunc)appDragMouseMove,
					    (void *)&dl );

    if  ( dl.dlTimerHandler && dl.dlTimerInterval > 0 )
	{
	dl.dlIntervalId= gtk_timeout_add( dl.dlTimerInterval,
						appDragTick, (void *)&dl );
	}

    /* Almost hangs X
    gdk_pointer_grab( w->window, FALSE, GDK_BUTTON1_MOTION_MASK,
					    (GdkWindow *)0,
					    (GdkCursor *)0,
					    GDK_CURRENT_TIME );
    */

    gtk_main();

    /*
    gdk_pointer_ungrab( GDK_CURRENT_TIME );
    */

    if  ( dl.dlIntervalId )
	{ gtk_timeout_remove( dl.dlIntervalId );	}

    gtk_signal_disconnect( GTK_OBJECT( w ), dl.dlUpId );
    gtk_signal_disconnect( GTK_OBJECT( w ), dl.dlDownId );
    gtk_signal_disconnect( GTK_OBJECT( w ), dl.dlMoveId );

    return;
    }

void appGuiGetStringFromKeyboardEvent(	APP_INPUT_CONTEXT	ic,
					APP_WIDGET		w,
					APP_EVENT *		event,
					int *			pGotString,
					int *			pGotKey,
					unsigned int *		pState,
					unsigned char *		buf,
					int			capacity,
					APP_KEY_VALUE *		pKey )
    {
    GdkEventKey *	keyEvent= &(event->key);
    int			got;

    got= keyEvent->length;
    if  ( got > capacity )
	{ got=  capacity;	}

    strncpy( buf, keyEvent->string, got );

    *pGotString= got;
    *pGotKey= 1;
    *pState= keyEvent->state;
    *pKey= keyEvent->keyval;

    return;
    }

void appGuiSetFocusChangeHandler(	APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through )
    {
    gtk_widget_add_events( w, GDK_FOCUS_CHANGE_MASK );

    gtk_signal_connect( GTK_OBJECT( w ),
				    "focus_in_event",
				    (GtkSignalFunc)handler, through );

    gtk_signal_connect( GTK_OBJECT( w ), "focus_out_event",
				    (GtkSignalFunc)handler, through );
    }

void appSetDestroyCallback(	APP_WIDGET		shell,
				APP_DESTROY_CALLBACK_T	destroyCallback,
				void *			through )
    {
    if  ( destroyCallback )
	{
	/*
	gtk_signal_connect( GTK_OBJECT( shell ), "destroy_event",
				    (GtkSignalFunc)destroyCallback, through );
	gtk_signal_connect( GTK_OBJECT( shell ), "destroy-event",
				    (GtkSignalFunc)destroyCallback, through );
	*/
	gtk_signal_connect( GTK_OBJECT( shell ), "destroy",
				    (GtkSignalFunc)destroyCallback, through );
	}
    }

void appSetCloseCallback(	APP_WIDGET		shell,
				EditApplication *	ea,
				APP_CLOSE_CALLBACK_T	closeCallback,
				void *			through )
    {
    if  ( closeCallback )
	{
	gtk_signal_connect( GTK_OBJECT( shell ), "delete_event",
				    (GtkSignalFunc)closeCallback, through );
	}
    }

int appGuiGtkSetChildLabel(	GtkWidget *		w,
				const char *		s )
    {
    int		rval= 0;

    if  ( GTK_IS_LABEL( w ) )
	{ gtk_label_set_text( GTK_LABEL( w ), s ); return 1;	}

    if  ( GTK_IS_CONTAINER( w ) )
	{
	GList *	glf;
	GList *	gl;

	gl= glf= gtk_container_children( GTK_CONTAINER( w ) );

	while( gl )
	    {
	    if  ( appGuiGtkSetChildLabel( (GtkWidget *)gl->data, s ) )
		{ rval= 1; break;	}

	    gl= gl->next;
	    }

	if  ( glf )
	    { g_list_free( glf );	}
	}

    return rval;
    }

int appGuiGtkGetChildLabel(	char **			pLabel,
				GtkWidget *		w )
    {
    int		rval= 0;

    if  ( GTK_IS_LABEL( w ) )
	{ gtk_label_get( GTK_LABEL( w ), pLabel ); return 1;	}

    if  ( GTK_IS_CONTAINER( w ) )
	{
	GList *	glf;
	GList *	gl;

	gl= glf= gtk_container_children( GTK_CONTAINER( w ) );

	while( gl )
	    {
	    if  ( appGuiGtkGetChildLabel( pLabel, (GtkWidget *)gl->data ) )
		{ rval= 1; break;	}

	    gl= gl->next;
	    }

	if  ( glf )
	    { g_list_free( glf );	}
	}

    return rval;
    }

#   endif
