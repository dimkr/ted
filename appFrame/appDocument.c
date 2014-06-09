/************************************************************************/
/*									*/
/*  Document window application independent functionality.		*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	<appSystem.h>
#   include	<utilFontmap.h>
#   include	<utilT1Entries.h>
#   include	"appFrame.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Callback for the file menu.						*/
/*									*/
/************************************************************************/

int appDocSaveDocumentByName(		EditDocument *		ed,
					APP_WIDGET		option,
					int			interactive,
					int			format,
					const char *		filename )
    {
    EditApplication *		ea= ed->edApplication;

    if  ( (*ea->eaSaveDocument)( ed->edPrivateData, format,
			    ea->eaNameAndVersion, ed->edTitle, filename ) )
	{
	if  ( interactive )
	    {
	    appReportSaveFailure( ea, option,
				    ed->edToplevel.atTopWidget, filename );
	    }

	return -1;
	}

    return 0;
    }

APP_MENU_CALLBACK_H( appDocFileSave, option, voided, call_data )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;
    APP_WIDGET			woption= (APP_WIDGET)option;

    int				interactive= 1;

    if  ( ! ea->eaSaveDocument )
	{ XDEB(ea->eaSaveDocument); return;	}

    if  ( ! ed->edFilename		||
	  ed->edFileReadOnly		||
	  ed->edFormat < 0		)
	{
#	ifdef USE_MOTIF
	appDocFileSaveAs( option, voided, (void *)0 ); return;
#	endif
#	ifdef USE_GTK
	appDocFileSaveAs( option, voided ); return;
#	endif
	XDEB(ed->edFilename); return;
	}

    if  ( appDocSaveDocumentByName( ed, woption, interactive,
					    ed->edFormat, ed->edFilename ) )
	{
	appReportSaveFailure( ea, woption,
				ed->edToplevel.atTopWidget, ed->edFilename );
	}
    else{ appDocumentChanged( ed, 0 ); }

    return;
    }

/************************************************************************/
/*									*/
/*  'Close' callback for the file menu and for the window manager menu	*/
/*									*/
/*  1)  As 'mwm' sometimes sends a 'Save yourself' when 'Close' is	*/
/*	selected from the window manager menu, deactivate the protocol.	*/
/*	B.T.W. This does not help.					*/
/*									*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( appDocFileClose, option, voided, call_data )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;
    APP_WIDGET		woption= (APP_WIDGET)option;

    if  ( ed->edHasBeenChanged )
	{ appRunReallyCloseDialog( woption, ed ); return; }

    appCloseDocument( ea, ed );

    return;
    }

APP_CLOSE_CALLBACK_H( appDocFileCloseCallback, w, voided )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;

    if  ( ed->edHasBeenChanged )
	{ appRunReallyCloseDialog( (APP_WIDGET)0, ed ); return; }

    appCloseDocument( ea, ed );
    }

APP_MENU_CALLBACK_H( appDocFileNew, option, voided, call_data )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;
    const char * const	filename= (const char *)0;

    if  ( appNewDocument( ea, filename ) )
	{ SDEB(filename); }

    return;
    }

APP_MENU_CALLBACK_H( appDocFileQuit, option, voided, call_data )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;
    APP_WIDGET		woption= (APP_WIDGET)option;

    appQuitApplication( woption, ed->edToplevel.atTopWidget, ea );

    return;
    }

APP_MENU_CALLBACK_H( appDocFileMini, option, voided, call_data )
    {
    EditDocument *		ed= (EditDocument *)voided;

    appIconifyShellWidget( ed->edToplevel.atTopWidget );

    return;
    }

APP_MENU_CALLBACK_H( appDocFileHide, option, voided, call_data )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;

    if  ( ed->edIsVisible )
	{
	appHideShellWidget( ed->edToplevel.atTopWidget );

	ed->edIsVisible= 0;

	appDocVisible( ea, ed, ed->edIsVisible );
	}

    return;
    }

void appReportSaveFailure(	EditApplication *	ea,
				APP_WIDGET		option,
				APP_WIDGET		relative,
				const char *		filename )
    {
    AppFileMessageResources *	afmr= &(ea->eaFileMessageResources);

    if  ( appTestFileWritable( filename ) )
	{
	appQuestionRunSubjectErrorDialog( ea, relative, option,
					filename, afmr->afmrFileNotWritable );
	}
    else{
	appQuestionRunSubjectErrorDialog( ea, relative, option,
					filename, afmr->afmrFileNotWritable );
	}
    }

/************************************************************************/
/*									*/
/*  The user selected the 'about' option.				*/
/*									*/
/************************************************************************/

void appDocAbout(	APP_WIDGET	option,
			void *		voided,
			void *		call_data )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;

    ea->eaMainVisibleAsAbout= 1;

    appShowShellWidget( ea->eaToplevel.atTopWidget );

    return;
    }

/************************************************************************/
/*									*/
/*  Callback for the item in the files menu.				*/
/*									*/
/************************************************************************/

void appMakeDocVisible(	EditApplication *	ea,
			EditDocument *		ed )
    {
    appShowShellWidget( ed->edToplevel.atTopWidget );

    if  ( ! ed->edIsVisible )
	{
	ed->edIsVisible= 1;

	appDocVisible( ea, ed, ed->edIsVisible );
	}
    }

void appDocToFront(	APP_WIDGET	option,
			void *		voided,
			void *		call_data )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;

    appMakeDocVisible( ea, ed );

    appGuiSetToggleItemState( option, ed->edHasBeenChanged );

    return;
    }

/************************************************************************/
/*									*/
/*  Callbacks for the edit menu.					*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( appDocEditCopy, option, voided, call_data )
    {
    EditDocument *			ed= (EditDocument *)voided;
    EditApplication *			ea= ed->edApplication;

    if  ( ! ea->eaDocCopy )
	{ SXDEB(ed->edFilename,ea->eaDocCopy); return;	}

    (*ea->eaDocCopy)( ed ); return;
    }

APP_MENU_CALLBACK_H( appDocEditCut, option, voided, call_data )
    {
    EditDocument *			ed= (EditDocument *)voided;
    EditApplication *			ea= ed->edApplication;

    if  ( ! ea->eaDocCut )
	{ SXDEB(ed->edFilename,ea->eaDocCut); return;	}

    (*ea->eaDocCut)( ed ); return;
    }

APP_MENU_CALLBACK_H( appDocEditPaste, option, voided, call_data )
    {
    EditDocument *			ed= (EditDocument *)voided;

    appDocAskForPaste( ed, "PRIMARY" );
    }

APP_MENU_CALLBACK_H( appDocEditSelAll, option, voided, call_data )
    {
    EditDocument *			ed= (EditDocument *)voided;
    EditApplication *			ea= ed->edApplication;

    if  ( ! ea->eaDocSelAll )
	{ SXDEB(ed->edFilename,ea->eaDocSelAll); return;	}

    ea->eaDocSelAll( ed ); return;
    }

/************************************************************************/
/*									*/
/*  Close a a document.							*/
/*									*/
/************************************************************************/

void appCloseDocument(	EditApplication *	ea,
			EditDocument *		ed )
    {
    appRemoveDocument( ea, ed );

    /* No! is done by the widget destroy callback
    if  ( ed->edPrivateData )
	{
	(*ea->eaFreeDocument)( ed->edPrivateData, ed->edFormat,
						    &(ed->edDrawingData) );
	ed->edPrivateData= (void *)0;
	}
    */

    appDestroyShellWidget( ed->edToplevel.atTopWidget );

    /* No! is done by the widget destroy callback
    appFreeDocument( ea, ed );
    */

    return;
    }

int appSetDocumentFilename(	EditDocument *		ed,
				const char *		filename )
    {
    char *			s= (char *)0;

    if  ( filename )
	{
	s= strdup( filename );
	if  ( ! s )
	    { XDEB(s); return -1;	}
	}

    if  ( ed->edFilename )
	{ free( ed->edFilename );	}
    ed->edFilename= s;

    return 0;
    }

int appFormatDocumentTitle(	const char **		pWindowTitle,
				const char **		pIconTitle,
				EditApplication *	ea,
				const char *		title )
    {
    const char *		baseName;

    int				len;
    static char *		fullTitle;
    static char *		fullIconName;

    baseName= strrchr( title, '/' );
    if  ( baseName )
	{ baseName++;		}
    else{ baseName= title;	}

    len= strlen( ea->eaApplicationName )+ 2+ strlen( title )+ 1;
    fullTitle= (char *)realloc( fullTitle, len );
    if  ( ! fullTitle )
	{ LXDEB(len,fullTitle); return -1;	}

    len= strlen( ea->eaApplicationName )+ 2+ strlen( baseName )+ 1;
    fullIconName= (char *)realloc( fullIconName, len );
    if  ( ! fullIconName )
	{ LXDEB(len,fullIconName); return -1;	}

    sprintf( fullTitle, "%s: %s", ea->eaApplicationName, title );
    sprintf( fullIconName, "%s: %s", ea->eaApplicationName, baseName );

    *pWindowTitle= fullTitle; *pIconTitle= fullIconName; return 0;
    }

int appSetDocumentTitle(	EditDocument *		ed,
				const char *		title )
    {
    EditApplication *		ea= ed->edApplication;

    char *			s;

    const char *		fullTitle;
    const char *		fullIconName;

    s= strdup( title );
    if  ( ! s )
	{ XDEB(s); return -1;	}

    if  ( ed->edTitle )
	{ free( ed->edTitle );	}
    ed->edTitle= s;

    if  ( appFormatDocumentTitle( &fullTitle, &fullIconName, ea, ed->edTitle ) )
	{ SDEB(ed->edTitle); return -1;	}

    appGuiSetShellTitle( ed->edToplevel.atTopWidget, fullTitle );
    appGuiSetIconTitle( ed->edToplevel.atTopWidget, fullIconName );

    appRenameDocumentOptions( ed->edApplication, ed, ed->edTitle );

    return 0;
    }


/************************************************************************/
/*									*/
/*  Free all resources allocated for a document.			*/
/*									*/
/************************************************************************/

void appFreeDocument(		EditApplication *	ea,
				EditDocument *		ed )
    {
    if  ( ed->edLeftRuler )
	{ (*ea->eaFreeLeftRuler)( ed->edLeftRuler ); }
    if  ( ed->edTopRuler )
	{ (*ea->eaFreeTopRuler)( ed->edTopRuler ); }
    if  ( ed->edRightRuler )
	{ (*ea->eaFreeRightRuler)( ed->edRightRuler ); }
    if  ( ed->edBottomRuler )
	{ (*ea->eaFreeBottomRuler)( ed->edBottomRuler ); }

    if  ( ed->edFilename )
	{ free( ed->edFilename );	}
    if  ( ed->edTitle )
	{ free( ed->edTitle );	}

    if  ( ed->edCheckpointFilename )
	{
	if  ( appRemoveFile( ed->edCheckpointFilename ) )
	    { SDEB(ed->edCheckpointFilename);	}

	free( ed->edCheckpointFilename );
	}

#   ifdef USE_MOTIF
    if  ( ed->edInputContext )
	{ XDestroyIC( ed->edInputContext ); }
#   endif

    appCleanDrawingData( &(ed->edDrawingData) );
    appCleanColors( &(ed->edColors) );

    free( ed );

    return;
    }

void appInitEditDocument(	EditApplication *	ea,
				EditDocument *		ed )
    {
    ed->edApplication= ea;

    ed->edFilename= (char *)0;
    ed->edTitle= (char *)0;
    ed->edCheckpointFilename= (char *)0;
    ed->edFormat= -1;
    ed->edFileReadOnly= 0;

    ed->edMenuBar= (APP_WIDGET)0;
    ed->edMainWindow= (APP_WIDGET)0;
	ed->edFileMenu= (APP_WIDGET)0;
	ed->edFileMenuButton= (APP_WIDGET)0;
	ed->edEditMenu= (APP_WIDGET)0;
	ed->edEditMenuButton= (APP_WIDGET)0;
	ed->edWindowMenu= (APP_WIDGET)0;
	ed->edWindowMenuButton= (APP_WIDGET)0;
	ed->edHelpMenu= (APP_WIDGET)0;
	ed->edHelpMenuButton= (APP_WIDGET)0;

    ed->edToolbar= (APP_WIDGET)0;
    ed->edScrolledWindow= (APP_WIDGET)0;
	ed->edVerticalScrollbar= (APP_WIDGET)0;
	ed->edHorizontalScrollbar= (APP_WIDGET)0;
	ed->edDocumentWidget= (APP_WIDGET)0;

	ed->edLeftRulerWidget= (APP_WIDGET)0;
	ed->edTopRulerWidget= (APP_WIDGET)0;
	ed->edRightRulerWidget= (APP_WIDGET)0;
	ed->edBottomRulerWidget= (APP_WIDGET)0;

	ed->edLeftRuler= (void *)0;
	ed->edTopRuler= (void *)0;
	ed->edRightRuler= (void *)0;
	ed->edBottomRuler= (void *)0;

#   ifdef USE_MOTIF
    ed->edInputContext= (XIC)0;
#   endif

    ed->edLeftRulerWidthPixels= 0;
    ed->edTopRulerHeightPixels= 0;
    ed->edRightRulerWidthPixels= 0;
    ed->edBottomRulerHeightPixels= 0;

    ed->edHasBeenChanged= 0;
    ed->edIsReadonly= 0;
    ed->edIsVisible= 0;

    ed->edShellExtraWidth= -1;
    ed->edShellExtraHeight= -1;

    appInitColors( &(ed->edColors) );
    appInitDrawingData( &(ed->edDrawingData) );

    ed->edMapped= 0;
    ed->edNotYetDrawn= 1;

    return;
    }

int appMakeDocumentWindow(	EditDocument **		pEd,
				EditApplication *	ea,
				int			read_only,
				const char *		title,
				const char *		filename )
    {
    char			scratch[100];

    EditDocument *		ed;

    if  ( ! title )
	{
	static int			count= 1;

	AppFileMessageResources *	afmr= &(ea->eaFileMessageResources);

	sprintf( scratch, afmr->afmrNamelessTitleFormat, count++ );
	title= scratch;
	}

    /*  1  */
    ed= (EditDocument *)malloc( sizeof(EditDocument) );
    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    appInitEditDocument( ea, ed );

    ea->eaNextDocumentId++; /* Never 0 */
    ed->edDocumentId= ea->eaNextDocumentId;

    if  ( filename )
	{ ed->edFilename= strdup( filename );	}
    else{ ed->edFilename= (char *)0;		}

    ed->edIsReadonly= read_only;
    ed->edTitle= strdup( title );

    if  ( ea->eaMakePrivateData	)
	{
	ed->edPrivateData= (*ea->eaMakePrivateData)();
	if  ( ! ed->edPrivateData )
	    { XDEB(ed->edPrivateData); appFreeDocument( ea, ed ); return -1; }
	}

    if  ( appFinishDocumentWindow( ed ) )
	{ appFreeDocument( ea, ed ); return -1; }

    *pEd= ed; return 0;
    }

int appSetupDocument(	EditApplication *	ea,
			EditDocument *		ed )
    {
    AppDrawingData *		add= &(ed->edDrawingData);

    double			screenPixelsPerMM;
    double			verPixPerMM;
    double			xfac;
    double			yfac;

    int				wide;
    int				high;

    appGetFactors( ea, &screenPixelsPerMM, &verPixPerMM, &xfac, &yfac );

    appSetDrawingEnvironment( add, ea->eaMagnification, xfac,
			screenPixelsPerMM,
			&(ea->eaPostScriptFontList),
			ea->eaToplevel.atTopWidget );

    add->addPageGapPixels= (int)( ea->eaPageGapMM* verPixPerMM );

    if  ( (*ea->eaLayoutDocument)( ed->edPrivateData, ed->edFormat,
					&(ed->edDrawingData),
					&(ea->eaDefaultDocumentGeometry) ) )
	{ SDEB(ed->edFilename); return -1; }

    appSetShellConstraints( ed );

    wide= add->addPaperRect.drX1- add->addPaperRect.drX0;
    high= add->addPaperRect.drY1- add->addPaperRect.drY0;

    if  ( wide > ( 4* ea->eaScreenPixelsWide )/ 5 )
	{ wide=( 4* ea->eaScreenPixelsWide )/ 5; }
    if  ( high > ( 4* ea->eaScreenPixelsHigh )/ 5 )
	{ high=( 4* ea->eaScreenPixelsHigh )/ 5; }

#   ifdef USE_MOTIF
    XtVaSetValues( ed->edDocumentWidget,
			    XmNborderWidth,		0,
			    XmNshadowThickness,		0,
			    XmNwidth,			wide,
			    XmNheight,			high,
			    NULL );
#   endif

#   ifdef USE_GTK
    gtk_drawing_area_size( GTK_DRAWING_AREA( ed->edTopRulerWidget ),
					ed->edLeftRulerWidthPixels+
					wide+
					ed->edRightRulerWidthPixels,
					ed->edTopRulerHeightPixels );
    gtk_drawing_area_size( GTK_DRAWING_AREA( ed->edLeftRulerWidget ),
					ed->edLeftRulerWidthPixels, high );


    gtk_drawing_area_size( GTK_DRAWING_AREA( ed->edDocumentWidget ),
								wide, high );

    gtk_drawing_area_size( GTK_DRAWING_AREA( ed->edRightRulerWidget ),
					ed->edRightRulerWidthPixels, high );
    gtk_drawing_area_size( GTK_DRAWING_AREA( ed->edBottomRulerWidget ),
					ed->edLeftRulerWidthPixels+
					wide+
					ed->edRightRulerWidthPixels,
					ed->edBottomRulerHeightPixels );

    gtk_object_set_user_data( GTK_OBJECT( ed->edDocumentWidget ), (void *)ed );
#   endif

    ed->edVisibleRect.drX0= add->addPaperRect.drX0;
    ed->edVisibleRect.drY0= add->addPaperRect.drY0;
    ed->edVisibleRect.drX1= add->addPaperRect.drX1;
    ed->edVisibleRect.drY1= add->addPaperRect.drY1;

    if  ( ea->eaSetTopRuler		&&
	  (*ea->eaSetTopRuler)( ed )	)
	{ SDEB(ed->edFilename);	}

    if  ( ea->eaSetLeftRuler		&&
	  (*ea->eaSetLeftRuler)( ed )	)
	{ SDEB(ed->edFilename);	}

    appDocSetScrollbarValues( ed );

#   ifdef USE_MOTIF
    XtRealizeWidget( ed->edToplevel.atTopWidget );
#   endif

#   ifdef USE_GTK
    gtk_widget_realize( ed->edToplevel.atTopWidget );
#   endif

    if  ( ea->eaFinishDocumentSetup && (*ea->eaFinishDocumentSetup)( ed ) )
	{ SDEB(ed->edFilename); return -1; }

    appShowShellWidget( ed->edToplevel.atTopWidget );

    ed->edIsVisible= 1;
    ed->edHasBeenChanged= 0;

    appDocVisible( ea, ed, ed->edIsVisible );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Fill the menu of a document window.					*/
/*									*/
/************************************************************************/

void appDocFillMenu(	EditDocument *		ed )
    {
    EditApplication *	ea= ed->edApplication;

    ed->edFileMenu= appMakeMenu( &(ed->edFileMenuButton),
		    &(ed->edToplevel), ea, ed->edMenuBar,
		    *(ea->eaDocFileMenuText), 0,
		    ea->eaDocFileMenuItems, ea->eaDocFileMenuItemCount,
		    (void *)ed );

    ed->edEditMenu= appMakeMenu( &(ed->edEditMenuButton),
		    &(ed->edToplevel), ea, ed->edMenuBar,
		    *(ea->eaDocEditMenuText), 0,
		    ea->eaDocEditMenuItems, ea->eaDocEditMenuItemCount,
		    (void *)ed );

    if  ( ea->eaMakePrivateDocumentMenus )
	{ (*ea->eaMakePrivateDocumentMenus)( ea, ed, ed->edMenuBar ); }

    ed->edWindowMenu= appMakeMenu( &(ed->edWindowMenuButton),
		    &(ed->edToplevel), ea, ed->edMenuBar,
		    *(ea->eaDocWindowMenuText), 0,
		    ea->eaDocWindowMenuItems, ea->eaDocWindowMenuItemCount,
		    (void *)ed );

    if  ( ea->eaDocHelpMenuItems )
	{
	ed->edHelpMenu= appMakeMenu( &(ed->edHelpMenuButton),
		    &(ed->edToplevel), ea, ed->edMenuBar,
		    *(ea->eaDocHelpMenuText), 1,
		    ea->eaDocHelpMenuItems, ea->eaDocHelpMenuItemCount,
		    (void *)ed );
	}

    }

/************************************************************************/
/*									*/
/*  Open a a document.							*/
/*									*/
/*  1)  Allocate a document.						*/
/*  3)  Make a shell to contain everything.				*/
/*  4)  Make the window pane.						*/
/*									*/
/************************************************************************/

EditDocument * appOpenDocument(	EditApplication *	ea,
				APP_WIDGET		relative,
				APP_WIDGET		option,
				int			read_only,
				const char *		filename )
    {
    EditDocument *		ed;
    int				fileReadonly= 0;
    AppFileMessageResources *	afmr= &(ea->eaFileMessageResources);

    if  ( read_only )
	{ fileReadonly= 1;	}
    else{
	if  ( appTestFileReadable( filename ) )
	    {
	    int	resp;

	    resp= appQuestionRunSubjectOkCancelDialog( ea,
					relative, option,
					filename, afmr->afmrFileReadOnly,
					(char *)0, (char *)0 );

	    if  ( resp != AQDrespOK )
		{ return (EditDocument *)0;	}

	    fileReadonly= 1;
	    }
	}

    /*  1,3,4  */
    if  ( appMakeDocumentWindow( &ed, ea, read_only, filename, filename ) )
	{ SDEB(filename); return (EditDocument *)0;	}

    ed->edFileReadOnly= fileReadonly;

    if  ( (*ea->eaOpenDocument)( ea, ed->edPrivateData, &(ed->edFormat),
						relative, option, filename ) )
	{ return (EditDocument *)0; }

    if  ( appSetupDocument( ea, ed ) )
	{ SDEB(filename); return (EditDocument *)0; }

    appSetDocument( ea, ed );

    return ed;
    }

int appNewDocument(	EditApplication *	ea,
			const char *		filename )
    {
    const int			read_only= 0;
    const char *		title= filename;

    EditDocument *		ed;

    /*  1,3,4  */
    if  ( appMakeDocumentWindow( &ed, ea, read_only, title, filename ) )
	{ SDEB(filename); return -1;	}

    if  ( (*ea->eaNewDocument)( ea, ed, filename ) )
	{ SDEB(title); return -1; }

    if  ( appSetupDocument( ea, ed ) )
	{ SDEB(title); return -1; }

    appSetDocument( ea, ed );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find out whether a document can be saved in a certain format.		*/
/*									*/
/************************************************************************/

int appDocumentTestCanSave(	EditApplication *		ea,
				const AppFileExtension *	afe,
				void *				privateData,
				unsigned int			useFlags,
				int				format )
    {
    if  ( ( afe->afeUseFlags & useFlags ) != useFlags )
	{ return -1;	}

    if  ( ea->eaCanSaveDocument					&&
	  (*ea->eaCanSaveDocument)( privateData, format )	)
	{ return -1;	}

    return 0;
    }

int appDocumentGetSaveFormat(	EditApplication *		ea,
				const char *			filename,
				void *				privateData,
				unsigned int			useFlags,
				int				format )
    {
    const char *	ext;
    AppFileExtension *	afe;
    int			i;

    ext= appFileExtensionOfName( filename );

    if  ( format >= 0 && format < ea->eaFileExtensionCount )
	{
	afe= ea->eaFileExtensions+ format;

	if  ( ! appDocumentTestCanSave( ea, afe, privateData,
							useFlags, format ) )
	    {
	    if  ( ! ext					||
		  ! afe->afeExtension			||
		  ! strcmp( ext, afe->afeExtension )	)
		{ return format;	}
	    }
	}

    afe= ea->eaFileExtensions;
    for ( i= 0; i < ea->eaFileExtensionCount; afe++, i++ )
	{
	if  ( appDocumentTestCanSave( ea, afe, privateData, useFlags, i ) )
	    { continue;	}

	if  ( ! ext					||
	      ! afe->afeExtension			||
	      ! strcmp( ext, afe->afeExtension )	)
	    { return i;	}
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  File Conversion entry point. ea is supposed to be initialized.	*/
/*  Depending on the application this routine may or mar not work with	*/
/*  an EditApplication struct that is not initialized.			*/
/*									*/
/************************************************************************/

int appFileConvert(	EditApplication *	ea,
			const char *		fromName,
			const char *		toName )
    {
    int			interactive= ( ea->eaToplevel.atTopWidget != NULL );

    void *		privateData= (void *)0;
    int			fromFormat= -1;
    int			toFormat;

    if  ( ! strcmp( fromName, toName ) )
	{ SSDEB(fromName,toName); return -1;	}

    if  ( ea->eaMakePrivateData	)
	{
	privateData= (*ea->eaMakePrivateData)();
	if  ( ! privateData )
	    { XDEB(privateData); return -1; }
	}

    if  ( (*ea->eaOpenDocument)( ea, privateData, &fromFormat,
						ea->eaToplevel.atTopWidget,
						(APP_WIDGET)0, fromName ) )
	{ SDEB(fromName); return -1; }

    toFormat= appDocumentGetSaveFormat( ea, toName, privateData,
						APPFILE_CAN_SAVE, fromFormat );

    if  ( toFormat >= 0 )
	{
	if  ( (*ea->eaSaveDocument)( privateData, toFormat,
				ea->eaNameAndVersion, toName, toName ) )
	    {
	    if  ( interactive )
		{
		appReportSaveFailure( ea, (APP_WIDGET)0,
					ea->eaToplevel.atTopWidget, toName );
		}

	    (*ea->eaFreeDocument)( privateData, fromFormat,
						    (AppDrawingData *)0 );
	    return -1;
	    }

	(*ea->eaFreeDocument)( privateData, fromFormat,
						    (AppDrawingData *)0 );
	return 0;
	}

    SSLDEB(fromName,toName,toFormat);
    (*ea->eaFreeDocument)( privateData, fromFormat, (AppDrawingData *)0 );
    return -1;
    }


/************************************************************************/
/*									*/
/*  Destroy callback on a document widget.				*/
/*									*/
/************************************************************************/

APP_DESTROY_CALLBACK_H( appDestroyEditDocument, w, voided )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;

    if  ( ed->edPrivateData )
	{
	(*ea->eaFreeDocument)( ed->edPrivateData, ed->edFormat,
						    &(ed->edDrawingData) );
	ed->edPrivateData= (void *)0;
	}

    appFreeDocument( ea, ed );

    return;
    }

/************************************************************************/
/*									*/
/*  Redraw a rectangle in the document widget.				*/
/*									*/
/************************************************************************/

APP_EVENT_HANDLER_H( appDocExposeHandler, w, voided, exposeEvent )
    {
    EditDocument *		ed= (EditDocument *)voided;
    AppDrawingData *		add= &(ed->edDrawingData);
    EditApplication *		ea= ed->edApplication;

    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    DocumentRectangle		drClip;

    if  ( ed->edNotYetDrawn				&&
	  ea->eaDocumentFirstVisible			)
	{ (*ea->eaDocumentFirstVisible)( ed );	}

    ed->edNotYetDrawn= 0;

    /*  1  */
    appCollectExposures( &drClip, add, ox, oy, exposeEvent );

    /*  2,3,4  */
    (*ea->eaDrawRectangle)( w, ed, &drClip, ox, oy );

    appDrawNoClipping( add );

    return;
    }

/************************************************************************/
/*									*/
/*  Cause a rectangle to be redrawn.					*/
/*									*/
/*  Cause the smallest rectangle that contains the selection to be	*/
/*  redrawn.								*/
/*									*/
/************************************************************************/

void appDocExposeRectangle(	const EditDocument *		ed,
				const DocumentRectangle *	drChanged,
				int				scrolledX,
				int				scrolledY )
    {
    DocumentRectangle		drExpose;
    DocumentRectangle		drScrolled;

    const AppDrawingData *	add= &(ed->edDrawingData);
    const DocumentRectangle *	drVisible= &(ed->edVisibleRect);

    if  ( ! drChanged )
	{ appExposeRectangle( add, 0, 0, 0, 0 ); return; }

    drScrolled= *drChanged;
    drScrolled.drX0 -= scrolledX;
    drScrolled.drX1 -= scrolledX;
    drScrolled.drY0 -= scrolledY;
    drScrolled.drY1 -= scrolledY;

    if  ( scrolledX != 0 )
	{
	drScrolled.drY0= drVisible->drY0;
	drScrolled.drY1= drVisible->drY1;
	}

    if  ( scrolledY != 0 )
	{
	drScrolled.drX0= drVisible->drX0;
	drScrolled.drX1= drVisible->drX1;
	}

    geoUnionRectangle( &drScrolled, &drScrolled, drChanged );

    drExpose.drX0= drVisible->drX0;
    drExpose.drY0= drVisible->drY0;
    drExpose.drX1= drVisible->drX1;
    drExpose.drY1= drVisible->drY1;

    if  ( geoIntersectRectangle( &drExpose, &drExpose, &drScrolled ) )
	{
	int	ox= drVisible->drX0;
	int	oy= drVisible->drY0;

#	if LOG_REDRAWS
	docLogRectangle( "CLEAR!", &drExpose );
#	endif

	appExposeRectangle( add,
				    drExpose.drX0- ox,
				    drExpose.drY0- oy,
				    drExpose.drX1- drExpose.drX0+ 1,
				    drExpose.drY1- drExpose.drY0+ 1 );
	}
    }

int appPostScriptFontCatalog(		EditApplication *	ea )
    {
    const int		mapNames= 1;

    if  ( ea->eaPostScriptFontList.psflFamilyCount > 0 )
	{ return 0;	}

    if  ( psFontCatalog( &(ea->eaPostScriptFontList), ea->eaAfmDirectory ) )
	{ SDEB(ea->eaAfmDirectory); return -1;	}

    if  ( ea->eaGhostscriptFontmap			&&
	  ! ea->eaGhostscriptMappingsRead		)
	{
	if  ( utilFontmapReadMap( ea->eaGhostscriptFontmap ) )
	    { SDEB(ea->eaGhostscriptFontmap); return -1;	}

	if  ( utilSetT1EntriesForFonts( ea->eaGhostscriptFontToXmapping,
				    mapNames, &(ea->eaPostScriptFontList) ) )
	    { SDEB(ea->eaGhostscriptFontToXmapping); return -1; }

	ea->eaGhostscriptMappingsRead= 1;
	}

    return 0;
    }
