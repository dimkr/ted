/************************************************************************/
/*									*/
/*  Run file choosers for edit applications.				*/
/*									*/
/*  GTK specific code. This code is based on original code by Andrea	*/
/*  Frome.								*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<locale.h>

#   include	<appSystem.h>

#   include	<appFrame.h>
#   include	<appDebugon.h>

#   include	"appFileChooser.h"

#   ifdef USE_GTK

/************************************************************************/
/*									*/
/*  Cleanup when a chooser is destroyed.				*/
/*									*/
/************************************************************************/

static void appChooserDestroyed(APP_WIDGET	fileChooser,
				GdkEvent *	event,
				void *		voidaci )
    { free( voidaci );	}

static void appInitAci(		AppChooserInformation *	aci )
    {
    appInitOptionmenu( &(aci->aciFilterOptionmenu) );

    aci->aciFilename= (char *)0;
    aci->aciThrough= (void *)0;
    aci->aciOpenDocument= (APP_OPEN_DOCUMENT)0;
    aci->aciExtensions= (AppFileExtension *)0;
    aci->aciExtensionCount= 0;
    aci->aciApplication= (EditApplication *)0;
    aci->aciOption= (APP_WIDGET)0;
    aci->aciFormat= -1;
    aci->aciExtension= (char *)0;

    aci->aciResources= (const AppFileChooserResources *)0;
    }

/************************************************************************/
/*									*/
/*  Callback for cancel buttons on choosers.				*/
/*									*/
/************************************************************************/

static void appChooserCancel(	APP_WIDGET	fileChooser,
				void *		voidaci )
    {
    AppChooserInformation *	aci= (AppChooserInformation *)voidaci;

    appGuiBreakDialog( &(aci->aciDialog), ACIrespCANCEL );
    return;
    }

static int appChooserClosed(	GtkWidget *	chooser,
				GdkEvent *	event,
				void *		voidaci )
    {
    gtk_widget_destroy( chooser );

    return FALSE;
    }

/************************************************************************/
/*									*/
/*  Retrieve the file name from a chooser.				*/
/*									*/
/************************************************************************/

static int appChooserGetFilename(	AppChooserInformation *	aci )
    {
    const char *	filename;
    GtkFileSelection *	gfs= GTK_FILE_SELECTION( aci->aciDialog.adTopWidget );

    filename= gtk_file_selection_get_filename( gfs );
    if  ( ! filename )
	{
	XDEB(filename);
	appGuiBreakDialog( &(aci->aciDialog), ACIrespFAILURE );
	return -1;
	}

    if  ( appChooserSaveFilename( aci, filename ) )
	{
	gtk_main_quit();
	return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Callback from the file chooser to open a file.			*/
/*									*/
/************************************************************************/

static void appOpenChooserOk(	GtkWidget *	okButton,
				void *		voidaci )
    {
    AppChooserInformation *	aci= (AppChooserInformation *)voidaci;
    APP_WIDGET			option= aci->aciOption;

    if  ( appChooserGetFilename( aci ) )
	{ return;	}

    if  ( appFileChooserTestNameForOpen( aci, aci->aciFilename ) )
	{
	free( aci->aciFilename ); aci->aciFilename= (char *)0;
	return;
	}

    if  ( (*aci->aciOpenDocument)( aci->aciThrough,
					    aci->aciDialog.adTopWidget, option,
					    aci->aciFilename ) )
	{ free( aci->aciFilename ); aci->aciFilename= (char *)0; return; }

    free( aci->aciFilename ); aci->aciFilename= (char *)0;

    appGuiBreakDialog( &(aci->aciDialog), ACIrespOPEN );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a chooser.							*/
/*									*/
/************************************************************************/

static int appMakeFileChooser(	AppChooserInformation **	pAci,
				APP_WIDGET			relative,
				EditApplication *		ea,
				const char *			defaultFilter,
				APP_WIDGET			option )
    {
    AppChooserInformation *	aci;

    GtkFileSelection *		gfs;

    aci= (AppChooserInformation *)malloc( sizeof(AppChooserInformation) );
    if  ( ! aci )
	{ XDEB(aci); return -1;	}

    appInitAci( aci );

    appFileChooserGetTexts( ea, aci );

    aci->aciDialog.adTopWidget= gtk_file_selection_new( "" );
    gfs= GTK_FILE_SELECTION( aci->aciDialog.adTopWidget );

    gtk_file_selection_hide_fileop_buttons( gfs );

    appSetShellTitle( aci->aciDialog.adTopWidget, option,
						    ea->eaApplicationName );

    gtk_signal_connect( GTK_OBJECT( gfs->cancel_button ), "clicked",
			GTK_SIGNAL_FUNC( appChooserCancel ), (void *)aci );
    gtk_signal_connect( GTK_OBJECT( gfs ), "delete_event",
			GTK_SIGNAL_FUNC( appChooserClosed ), (void *)aci );
    gtk_signal_connect( GTK_OBJECT( gfs ), "destroy_event",
			GTK_SIGNAL_FUNC( appChooserDestroyed ), (void *)aci );

    aci->aciApplication= ea;

    *pAci= aci;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Run some kind of a file chooser.					*/
/*									*/
/************************************************************************/

static void appRunFileChooser(	EditApplication *		ea,
				AppChooserInformation *		aci,
				APP_WIDGET			relative )
    {
    aci->aciFilename= (char *)0;

    appGuiShowDialog( &(aci->aciDialog), relative );

    appGuiRunDialog( &(aci->aciDialog), ACIrespNONE, ea );

    appGuiHideDialog( &(aci->aciDialog) );

    return;
    }

/************************************************************************/
/*									*/
/*  Open a document.							*/
/*									*/
/************************************************************************/

void appRunOpenChooser( APP_WIDGET		option,
			APP_WIDGET		relative,
			int			extensionCount,
			AppFileExtension *	extensions,
			char *			defaultFilter,
			void *			through,
			APP_OPEN_DOCUMENT	openDocument,
			EditApplication *	ea )
    {
    GtkFileSelection *		gfs;
    AppChooserInformation *	aci= (AppChooserInformation *)0;

    if  ( appMakeFileChooser( &aci, relative, ea, defaultFilter, option ) )
	{ LDEB(1); return;	}

    gfs= GTK_FILE_SELECTION( aci->aciDialog.adTopWidget );

    gtk_signal_connect( GTK_OBJECT( gfs->ok_button ), "clicked",
			GTK_SIGNAL_FUNC( appOpenChooserOk ), (void *)aci );

    aci->aciOption= option;
    aci->aciThrough= through;
    aci->aciOpenDocument= openDocument;
    aci->aciFilename= (char *)0;

    appRunFileChooser( ea, aci, relative );

    gtk_widget_destroy( aci->aciDialog.adTopWidget );

    return;
    }

static int appChooserOpenDocument(	void *		through,
					APP_WIDGET	relative,
					APP_WIDGET	option,
					const char *	filename )
    {
    const int	read_only= 0;

    if  ( ! appOpenDocument( (EditApplication *)through,
				    relative, option, read_only, filename ) )
	{ return -1;	}

    return 0;
    }

APP_MENU_CALLBACK_H( appAppFileOpen, option, voidea, call_data )
    {
    EditApplication *	ea= (EditApplication *)voidea;

    appRunOpenChooser( GTK_WIDGET( option ), ea->eaToplevel.atTopWidget,
			ea->eaFileExtensionCount, ea->eaFileExtensions,
			ea->eaDefaultFileFilter,
			(void *)ea, appChooserOpenDocument, ea );
    }

APP_MENU_CALLBACK_H( appDocFileOpen, option, voided, call_data )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;

    appRunOpenChooser( GTK_WIDGET( option ), ed->edToplevel.atTopWidget,
			ea->eaFileExtensionCount, ea->eaFileExtensions,
			ea->eaDefaultFileFilter,
			(void *)ea, appChooserOpenDocument, ea );
    }

static void appSaveOkPushed(		APP_WIDGET	fileChooser,
					void *		voidaci )
    {
    AppChooserInformation *		aci= (AppChooserInformation *)voidaci;
    int					response;

    if  ( appChooserGetFilename( aci ) )
	{
	gtk_main_quit();
	return;
	}

    response= appFileChooserTestNameForSave( aci, aci->aciFilename );

    switch( response )
	{
	case ACIrespSAVE:
	    appGuiBreakDialog( &(aci->aciDialog), ACIrespSAVE );
	    return;

	case ACIrespNONE:
	    free( aci->aciFilename ); aci->aciFilename= (char *)0;
	    return;

	default:
	    LDEB(response);
	    free( aci->aciFilename ); aci->aciFilename= (char *)0;
	    appGuiBreakDialog( &(aci->aciDialog), ACIrespFAILURE );
	    return;
	}

    return;
    }

int appRunSaveChooser(		APP_WIDGET		option,
				APP_WIDGET		relative,
				EditApplication *	ea,
				const EditDocument *	ed,
				unsigned int		useFlags,
				int *			pFormat,
				char **			pFilename )
    {
    GtkFileSelection *		gfs;
    AppChooserInformation *	aci= (AppChooserInformation *)0;

    if  ( appMakeFileChooser( &aci, relative, ea,
					    ea->eaDefaultFileFilter, option ) )
	{ return ACIrespFAILURE;	}

    gfs= GTK_FILE_SELECTION( aci->aciDialog.adTopWidget );

    gtk_signal_connect( GTK_OBJECT( gfs->ok_button ), "clicked",
			GTK_SIGNAL_FUNC( appSaveOkPushed ), (void *)aci );

    aci->aciOption= option;

    appRunFileChooser( ea, aci, relative );

    if  ( aci->aciDialog.adResponse == ACIrespSAVE )
	{
	if  ( aci->aciFormat < 0 )
	    {
	    aci->aciFormat= appDocumentGetSaveFormat( ea, aci->aciFilename,
				    ed->edPrivateData, useFlags, ed->edFormat );
	    }

	*pFilename= aci->aciFilename;
	*pFormat= aci->aciFormat;
	}

    gtk_widget_destroy( aci->aciDialog.adTopWidget );

    return aci->aciDialog.adResponse;
    }

/************************************************************************/
/*									*/
/*  Run Filechooser for print to file.					*/
/*									*/
/************************************************************************/

int appRunPrintToFileChooser(		APP_WIDGET		option,
					APP_WIDGET		panel,
					EditApplication *	ea,
					const EditDocument *	ed,
					char **			pFilename )
    {
    GtkFileSelection *		gfs;
    AppChooserInformation *	aci= (AppChooserInformation *)0;
    int				response;

    if  ( appMakeFileChooser( &aci, panel, ea, "*.ps", option ) )
	{ LDEB(1); return ACIrespFAILURE;	}

    gfs= GTK_FILE_SELECTION( aci->aciDialog.adTopWidget );

    gtk_signal_connect( GTK_OBJECT( gfs->ok_button ), "clicked",
			GTK_SIGNAL_FUNC( appSaveOkPushed ), (void *)aci );

    aci->aciExtension= "ps";
    aci->aciOption= option;
    aci->aciFilename= (char *)0;

    appRunFileChooser( ea, aci, panel );

    response= aci->aciDialog.adResponse;

    if  ( response == ACIrespSAVE )
	{ *pFilename= aci->aciFilename; }

    gtk_widget_destroy( aci->aciDialog.adTopWidget );

    return response;
    }

#   endif
