/************************************************************************/
/*									*/
/*  Application, Main module.						*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	<locale.h>
#   include	<signal.h>

#   include	<appSystem.h>

#   include	<appFrame.h>
#   include	"appUnit.h"
#   include	"appPaper.h"
#   include	"appGeoString.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Hide the application window if shown as an 'about' window.		*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( appAppFileHide, option, voidea, call_data )
    {
    EditApplication *		ea= (EditApplication *)voidea;

    ea->eaMainVisibleAsAbout= 0;

    if  ( ea->eaVisibleDocumentCount > 0 )
	{ appHideShellWidget( ea->eaToplevel.atTopWidget ); }

    return;
    }

/************************************************************************/
/*									*/
/*  Minimise the application.						*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( appAppFileMini, option, voidea, call_data )
    {
    EditApplication *		ea= (EditApplication *)voidea;

    appIconifyShellWidget( ea->eaToplevel.atTopWidget );

    return;
    }

/************************************************************************/
/*									*/
/*  Callback that is invoked when the application is closed through	*/
/*  the window manager.							*/
/*									*/
/************************************************************************/

APP_CLOSE_CALLBACK_H( appAppWmClose, w, voidea )
    {
    EditApplication *		ea= (EditApplication *)voidea;

    if  ( ea->eaMainVisibleAsAbout )
	{
	ea->eaMainVisibleAsAbout= 0;

	if  ( ea->eaVisibleDocumentCount > 0 )
	    { appHideShellWidget( ea->eaToplevel.atTopWidget ); }
	}
    else{
	appQuitApplication( (APP_WIDGET)0,
				    ea->eaToplevel.atTopWidget, ea );
	}
    }

/************************************************************************/
/*									*/
/*  Quit the application.						*/
/*									*/
/*  1)  Close the documents in reverse order to avoid the subscript of	*/
/*	unhandeled documents to change.					*/
/*									*/
/************************************************************************/

int appCountChangedDocuments(	EditApplication *	ea )
    {
    int		i;
    int		changedCount= 0;

    for ( i= 0; i < ea->eaOpenCount; i++ )
	{
	EditDocument *	ed= ea->eaOpenDocuments[i];

	if  ( ed->edHasBeenChanged )
	    { changedCount++;	}
	}

    return changedCount;
    }

void appExitApplication(	EditApplication *	ea )
    {
    appDiscardImagePixmaps( ea );

    appDestroyShellWidget( ea->eaToplevel.atTopWidget );

    exit( 0 ); LDEB(1); return;
    }

void appAskCloseDocuments(	APP_WIDGET		option,
				EditApplication *	ea )
    {
    int		i;

    /*  1  */
    for ( i= ea->eaOpenCount- 1; i >= 0; i-- )
	{
	EditDocument *	ed= ea->eaOpenDocuments[i];

	if  ( ed->edHasBeenChanged )
	    {
	    appMakeDocVisible( ea, ed );
	    appRunReallyCloseDialog( option, ed );
	    }
	}

    return;
    }

void appQuitApplication(	APP_WIDGET		option,
				APP_WIDGET		relative,
				EditApplication *	ea )
    {
    int				rcc;
    int				changedCount= 0;

    changedCount= appCountChangedDocuments( ea );

    if  ( changedCount == 0 )
	{ appExitApplication( ea ); LDEB(1); return;	}

    if  ( changedCount > 1 )
	{
	rcc= appRunReallyQuitDialog( option, relative, ea );

	switch( rcc )
	    {
	    case AQDrespCANCEL:
		return;
	    case AQDrespNO:
		appExitApplication( ea );
		LDEB(1); return;

	    default:
		LDEB(rcc); return;
	    case AQDrespYES:
		break;
	    }
	}

    appAskCloseDocuments( option, ea );

    changedCount= appCountChangedDocuments( ea );
    if  ( changedCount == 0 )
	{ appExitApplication( ea ); LDEB(1); return;	}

    return;
    }

APP_MENU_CALLBACK_H( appAppFileQuit, option, voidea, call_data )
    {
    EditApplication *	ea= (EditApplication *)voidea;
    APP_WIDGET		woption= (APP_WIDGET)option;

    appQuitApplication( woption, ea->eaToplevel.atTopWidget, ea );
    }

/************************************************************************/
/*									*/
/*  Open a new document.						*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( appAppFileNew, option, voidea, call_data )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    const char *		filename= (const char *)0;

    if  ( appNewDocument( ea, filename ) )
	{ SDEB(filename); }

    return;
    }

/************************************************************************/
/*									*/
/*  Make a document the current Document.				*/
/*									*/
/************************************************************************/

static void appHighlightCurrentDocument( EditApplication *	ea,
					EditDocument *		ed,
					int			current )
    {
    appGuiEnableWidget( ed->edMenuBar, current != 0 );
    return;
    }

void appSetCurrentDocument(	EditApplication *	ea,
				EditDocument *		ed	)
    {
    if  ( ea->eaCurrentDocument )
	{ appHighlightCurrentDocument( ea, ea->eaCurrentDocument, 0 ); }

    ea->eaCurrentDocument= ed;

    appHighlightCurrentDocument( ea, ea->eaCurrentDocument, 1 );
    }

/************************************************************************/
/*									*/
/*  Remember whether a document has been changed.			*/
/*									*/
/************************************************************************/

void appDocumentChanged(	EditDocument *		ed,
				int			changed	)
    {
    EditApplication *	ea= ed->edApplication;

    changed= changed != 0;

    if  ( ed->edHasBeenChanged != changed )
	{
	int		i;

	for ( i= 0; i < ea->eaOpenCount; i++ )
	    {
	    if  ( ea->eaOpenDocuments[i] == ed )
		{ continue;	}

	    appSetWindowsItemState( ea->eaOpenDocuments[i]->edWindowMenu,
								ed, changed );
	    }

	appSetWindowsItemState( ea->eaWinMenu, ed, changed );

	ed->edHasBeenChanged= changed;
	}
    }

/************************************************************************/
/*									*/
/*  Count the number of visible documents.				*/
/*									*/
/************************************************************************/

void appDocVisible(	EditApplication *	ea,
			EditDocument *		ed,
			int			visible	)
    {
    int	from;

    if  ( visible )
	{ from= ea->eaVisibleDocumentCount++;	}
    else{ from= ea->eaVisibleDocumentCount--;	}

    if  ( ea->eaAppFileHideOption )
	{
	appGuiEnableWidget( ea->eaAppFileHideOption,
					ea->eaVisibleDocumentCount > 0 );
	}

    if  ( ea->eaVisibleDocumentCount == 0 )
	{ appShowShellWidget( ea->eaToplevel.atTopWidget ); }
    else{
	if  ( ! ea->eaMainVisibleAsAbout )
	    { appHideShellWidget( ea->eaToplevel.atTopWidget );	}
	}

    if  ( ea->eaVisibleDocumentCountChanged )
	{
	(*ea->eaVisibleDocumentCountChanged)
				    ( ea, from, ea->eaVisibleDocumentCount );
	}
    }

/************************************************************************/
/*									*/
/*  Remove a reference to a document.					*/
/*									*/
/************************************************************************/

void appRemoveDocument(	EditApplication *	ea,
			EditDocument *		ed )
    {
    int		i;
    int		count= 0;

    for ( i= 0; i < ea->eaOpenCount; i++ )
	{
	if  ( ea->eaOpenDocuments[i] == ed )
	    { continue;	}

	appRemoveWindowsOption( ea->eaOpenDocuments[i]->edWindowMenu, ed );
	ea->eaOpenDocuments[count++]= ea->eaOpenDocuments[i];
	}

    appRemoveWindowsOption( ea->eaWinMenu, ed );

    ea->eaOpenCount= count;

    if  ( ea->eaCurrentDocument == ed )
	{ ea->eaCurrentDocument= (EditDocument *)0;	}

    if  ( ed->edIsVisible )
	{
	ed->edIsVisible= 0;
	appDocVisible( ea, ed, ed->edIsVisible );
	}

    return;
    }

void appRenameDocumentOptions(	EditApplication *	ea,
				EditDocument *		ed,
				char *			title )
    {
    int		i;

    for ( i= 0; i < ea->eaOpenCount; i++ )
	{
	if  ( ea->eaOpenDocuments[i] == ed )
	    { continue;	}

	appRenameWindowsOption( ea->eaOpenDocuments[i]->edWindowMenu,
								ed, title );
	}

    appRenameWindowsOption( ea->eaWinMenu, ed, title );

    return;
    }

/************************************************************************/
/*									*/
/*  Remember a reference to a document.					*/
/*									*/
/************************************************************************/

static void appAppSetWindowsOption(	APP_WIDGET		menu,
					EditDocument *		ed,
					char *			label )
    {
    APP_WIDGET		windowsOption;
    AppMenuItem		ami;

    if  ( ed->edHasBeenChanged )
	{ ami.amiItemType= ITEMtyTOGGLE_ON;	}
    else{ ami.amiItemType= ITEMtyTOGGLE_OFF;	}

    ami.amiItemText= label;
    ami.amiKey= (char *)0;
    ami.amiKeyText= (char *)0;
    ami.amiCallback= (APP_MENU_CALLBACK_T)appDocToFront;

    windowsOption= appSetToggleMenuItem( menu, &(ed->edToplevel),
							&ami, (void *)ed );
    }

void appSetDocument(	EditApplication *	ea,
			EditDocument *		newEd )
    {
    EditDocument **	fresh;
    int			i;

    fresh= (EditDocument **)realloc( ea->eaOpenDocuments,
			( ea->eaOpenCount+ 1 )* sizeof(EditDocument *) );
    if  ( ! fresh )
	{ XDEB(fresh); return;	}

    fresh[ea->eaOpenCount]= newEd;
    ea->eaOpenDocuments= fresh;

    for ( i= 0; i < ea->eaOpenCount; i++ )
	{
	appAppSetWindowsOption( ea->eaOpenDocuments[i]->edWindowMenu,
							newEd, newEd->edTitle );

	appAppSetWindowsOption( newEd->edWindowMenu,
					    ea->eaOpenDocuments[i],
					    ea->eaOpenDocuments[i]->edTitle );
	}

    appAppSetWindowsOption( ea->eaWinMenu, newEd, newEd->edTitle );

    ea->eaOpenCount++;

    return;
    }

/************************************************************************/
/*									*/
/*  Generic editor type application main().				*/
/*									*/
/*  1)  Make an application shell.					*/
/*  1b) Obtain resource values.						*/
/*  2)  Remember X11 properties.					*/
/*  3)  Remember Window manager properties.				*/
/*									*/
/************************************************************************/

static AppConfigurableResource	APP_ApplicationResourceTable[]=
{
    APP_RESOURCE( "unit",
		offsetof(EditApplication,eaUnitString),
		"inch" ),
    APP_RESOURCE( "paper",
		offsetof(EditApplication,eaPaperString),
		"letter" ),
    APP_RESOURCE( "leftMargin",
		offsetof(EditApplication,eaLeftMarginString),
		"90pt" ),
    APP_RESOURCE( "rightMargin",
		offsetof(EditApplication,eaRightMarginString),
		"90pt" ),
    APP_RESOURCE( "topMargin",
		offsetof(EditApplication,eaTopMarginString),
		"72pt" ),
    APP_RESOURCE( "bottomMargin",
		offsetof(EditApplication,eaBottomMarginString),
		"72pt" ),
    APP_RESOURCE( "magnification",
		offsetof(EditApplication,eaMagnificationString),
		"120%" ),

    APP_RESOURCE( "supportXvCopyPaste",
		offsetof(EditApplication,eaSupportXvCopyPasteString),
		(char *)0 ),

    APP_RESOURCE( "hideSaveToOption",
		offsetof(EditApplication,eaHideSaveToOptionString),
		(char *)0 ),

    APP_RESOURCE( "usePostScriptFilters",
		offsetof(EditApplication,eaUsePostScriptFiltersString),
		(char *)0 ),
    APP_RESOURCE( "usePostScriptIndexedImages",
		offsetof(EditApplication,eaUsePostScriptIndexedImagesString),
		(char *)0 ),

    APP_RESOURCE( "skipEmptyPages",
		offsetof(EditApplication,eaSkipEmptyPagesString),
		(char *)0 ),
    APP_RESOURCE( "skipBlankPages",
		offsetof(EditApplication,eaSkipBlankPagesString),
		(char *)0 ),
    APP_RESOURCE( "omitHeadersOnEmptyPages",
		offsetof(EditApplication,eaOmitHeadersOnEmptyPagesString),
		(char *)0 ),

    APP_RESOURCE( "author",
		offsetof(EditApplication,eaAuthor),
		(char *)0 ),

    APP_RESOURCE( "pageNumberFormat",
		offsetof(EditApplication,eaPageNumberFormat),
		"Page %d" ),

    APP_RESOURCE( "afmDirectory",
		offsetof(EditApplication,eaAfmDirectory),
		AFMDIR ),
    APP_RESOURCE( "fontDirectory",
		offsetof(EditApplication,eaFontDirectory),
		(char *)0 ),
    APP_RESOURCE( "ghostscriptFontmap",
		offsetof(EditApplication,eaGhostscriptFontmap),
		(char *)0 ),
    APP_RESOURCE( "ghostscriptFontToXmapping",
		offsetof(EditApplication,eaGhostscriptFontToXmapping),
		(char *)0 ),
    APP_RESOURCE( "defaultFont",
		offsetof(EditApplication,eaDefaultFont),
		"Helvetica,,,10" ),
    APP_RESOURCE( "faxCommand",
		offsetof(EditApplication,eaFaxCommand),
		(char *)0 ),
    APP_RESOURCE( "customPrintCommand",
		offsetof(EditApplication,eaCustomPrintCommand),
		(char *)0 ),
    APP_RESOURCE( "customPrinterName",
		offsetof(EditApplication,eaCustomPrinterName),
		(char *)0 ),
    APP_RESOURCE( "customPrintCommand2",
		offsetof(EditApplication,eaCustomPrintCommand2),
		(char *)0 ),
    APP_RESOURCE( "customPrinterName2",
		offsetof(EditApplication,eaCustomPrinterName2),
		(char *)0 ),

    APP_RESOURCE( "rulerFont",
		offsetof(EditApplication,eaRulerFont),
		"-*-helvetica-medium-r-*-*-%d-*-*-*-*-*-iso8859-1" ),
    APP_RESOURCE( "printDialogFont",
		offsetof(EditApplication,eaPrintDialogFont),
		"-*-lucidatypewriter-bold-r-*-*-%d-*-*-*-m-*-iso8859-1" ),
};

/************************************************************************/
/*									*/
/*  Initialize geometry related settings for the application.		*/
/*									*/
/************************************************************************/

static void appInitializeGeometrySettings(	EditApplication *	ea )
    {
    int				paperFormat;

    ea->eaUnitInt= appUnitTypeInt( ea->eaUnitString );
    if  ( ea->eaUnitInt < 0 )
	{ SDEB(ea->eaUnitString); ea->eaUnitInt= UNITtyINCH; }

    ea->eaMagnification= 1.2;
    if  ( ea->eaMagnificationString )
	{
	double	mag= 0.0;
	char	c;
	int	res;

	res= sscanf( ea->eaMagnificationString, "%lg%c", &mag, &c );
	if  ( res == 1 )
	    { ea->eaMagnification= mag;		}
	if  ( res == 2 && c == '%' )
	    { ea->eaMagnification= mag/100.0;	}

	if  ( mag < 0.1				||
	      ea->eaMagnification < 0.1		||
	      ea->eaMagnification > 10.0	)
	    {
	    SFDEB(ea->eaMagnificationString,mag);
	    ea->eaMagnification= 1.2;
	    }
	}

    if  ( appPaperFormatFromString( &paperFormat,
			    &(ea->eaDefaultDocumentGeometry.dgPageWideTwips),
			    &(ea->eaDefaultDocumentGeometry.dgPageHighTwips),
			    ea->eaUnitInt, ea->eaPaperString ) )
	{ SDEB(ea->eaPaperString);	}

    if  ( ea->eaLeftMarginString )
	{
	if  ( appGeoLengthFromString( ea->eaLeftMarginString, ea->eaUnitInt,
			&ea->eaDefaultDocumentGeometry.dgLeftMarginTwips ) )
	    { SDEB(ea->eaLeftMarginString);	}
	}

    if  ( ea->eaRightMarginString )
	{
	if  ( appGeoLengthFromString( ea->eaRightMarginString, ea->eaUnitInt,
			&ea->eaDefaultDocumentGeometry.dgRightMarginTwips ) )
	    { SDEB(ea->eaRightMarginString);	}
	}

    if  ( ea->eaTopMarginString )
	{
	if  ( appGeoLengthFromString( ea->eaTopMarginString, ea->eaUnitInt,
			&ea->eaDefaultDocumentGeometry.dgTopMarginTwips ) )
	    { SDEB(ea->eaTopMarginString);	}
	}

    if  ( ea->eaBottomMarginString )
	{
	if  ( appGeoLengthFromString( ea->eaBottomMarginString, ea->eaUnitInt,
			&ea->eaDefaultDocumentGeometry.dgBottomMarginTwips ) )
	    { SDEB(ea->eaBottomMarginString);	}
	}

    ea->eaSupportXvCopyPaste= 0;
    if  ( ea->eaSupportXvCopyPasteString			&&
	  ! strcmp( ea->eaSupportXvCopyPasteString, "1" )	)
	{ ea->eaSupportXvCopyPaste= 1;	}

    ea->eaHideSaveToOption= 0;
    if  ( ea->eaHideSaveToOptionString			&&
	  ! strcmp( ea->eaHideSaveToOptionString, "1" )	)
	{ ea->eaHideSaveToOption= 1;	}

    ea->eaUsePostScriptFilters= 0;
    if  ( ea->eaUsePostScriptFiltersString			&&
	  ! strcmp( ea->eaUsePostScriptFiltersString, "1" )	)
	{ ea->eaUsePostScriptFilters= 1;	}

    ea->eaUsePostScriptIndexedImages= 0;
    if  ( ea->eaUsePostScriptIndexedImagesString			&&
	  ! strcmp( ea->eaUsePostScriptIndexedImagesString, "1" )	)
	{ ea->eaUsePostScriptIndexedImages= 1;	}

    ea->eaSkipEmptyPages= 0;
    if  ( ea->eaSkipEmptyPagesString			&&
	  ! strcmp( ea->eaSkipEmptyPagesString, "1" )	)
	{ ea->eaSkipEmptyPages= 1;	}

    ea->eaSkipBlankPages= 0;
    if  ( ea->eaSkipBlankPagesString			&&
	  ! strcmp( ea->eaSkipBlankPagesString, "1" )	)
	{ ea->eaSkipBlankPages= 1;	}

    ea->eaOmitHeadersOnEmptyPages= 0;
    if  ( ea->eaOmitHeadersOnEmptyPagesString			&&
	  ! strcmp( ea->eaOmitHeadersOnEmptyPagesString, "1" )	)
	{ ea->eaOmitHeadersOnEmptyPages= 1;	}

    if  ( ea->eaToplevel.atTopWidget )
	{
	double			horPixPerMM;
	double			verPixPerMM;
	double			xfac;
	double			yfac;

	appGetFactors( ea, &horPixPerMM, &verPixPerMM, &xfac, &yfac );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Fill the application main widget.					*/
/*									*/
/************************************************************************/

static int appFinishApplicationWindow(	EditApplication *	ea )
    {
    APP_BITMAP_IMAGE		pixmap= (APP_BITMAP_IMAGE)0;
    APP_BITMAP_MASK		mask= (APP_BITMAP_MASK)0;

    APP_WIDGET			pictureLabel= (APP_WIDGET)0;
    APP_WIDGET			label;

    appGuiInsertColumnInWindow( &(ea->eaMainWindow),
					    ea->eaToplevel.atTopWidget );

    appGuiInsertMenubarInColumn( &(ea->eaMenuBar), ea->eaMainWindow );

    ea->eaFileMenu= appMakeMenu( &(ea->eaFileMenuButton),
			&(ea->eaToplevel), ea, ea->eaMenuBar,
			*(ea->eaAppFileMenuText), 0,
			ea->eaAppFileMenuItems, ea->eaAppFileMenuItemCount,
			(void *)ea );

    if  ( ea->eaAppFileHideOption == (APP_WIDGET)0 )
	{
	int			i;
	const AppMenuItem *	ami= ea->eaAppFileMenuItems;

	for ( i= 0; i < ea->eaAppFileMenuItemCount; ami++, i++ )
	    {
	    if  ( ami->amiCallback == appAppFileHide )
		{ ea->eaAppFileHideOption= ami->amiOptionWidget;	}
	    }
	}


    ea->eaWinMenu= appMakeMenu( &(ea->eaWinMenuButton),
			&(ea->eaToplevel), ea, ea->eaMenuBar,
			*(ea->eaAppWinMenuText), 0,
			ea->eaAppWinMenuItems, ea->eaAppWinMenuItemCount,
			(void *)ea );

    if  ( ea->eaMakePrivateApplicationMenus )
	{ (*ea->eaMakePrivateApplicationMenus)( ea, ea->eaMenuBar ); }

    if  ( ea->eaAppHelpMenuItems )
	{
	ea->eaHelpMenu= appMakeMenu( &(ea->eaHelpMenuButton),
			&(ea->eaToplevel), ea, ea->eaMenuBar,
			*(ea->eaAppHelpMenuText), 1,
			ea->eaAppHelpMenuItems, ea->eaAppHelpMenuItemCount,
			(void *)ea );
	}

#	ifdef USE_MOTIF
	/*  Work around BUG in LessTif	*/
	{
	Dimension		high;

	XtVaGetValues( ea->eaMenuBar,
				XmNheight,	&high,
				NULL );
	if  ( high < 8 )
	    {
	    XtWidgetGeometry	preferred;

	    XtQueryGeometry( ea->eaMenuBar, (XtWidgetGeometry *)0, &preferred );

	    high= preferred.height+ preferred.border_width;
	    XtVaSetValues( ea->eaMenuBar,
				XmNheight,	high,
				NULL );
	    }
	}
#	endif

    if  ( ea->eaMainPicture )
	{
	APP_BITMAP_IMAGE	labelPixmap;
	APP_BITMAP_MASK		labelMask;

	if  ( ! appGetImagePixmap( ea, ea->eaMainPicture,
						&labelPixmap, &labelMask ) )
	    {
	    appMakeImageInColumn( &pictureLabel, ea->eaMainWindow,
						    labelPixmap, labelMask );
	    }
	}

    if  ( ea->eaNameAndVersion )
	{
	char *		ident= (char *)0;


	if  ( ea->eaPlatformCompiled )
	    {
	    ident= malloc( strlen( ea->eaNameAndVersion )+ 3+
					strlen( ea->eaPlatformCompiled )+ 1 );
	    if  ( ! ident )
		{ XDEB(ident); }
	    else{
		sprintf( ident, "%s - %s",
				ea->eaNameAndVersion, ea->eaPlatformCompiled );
		}
	    }
	else{
	    ident= strdup( ea->eaNameAndVersion );
	    }

	if  ( ! ident )
	    { XDEB(ident);	}
	else{
	    appMakeLabelInColumn( &label, ea->eaMainWindow, ident );

	    free( ident );
	    }
	}

    if  ( ea->eaReference )
	{
	appMakeLabelInColumn( &label, ea->eaMainWindow,
						    ea->eaReference );
	}

    if  ( ea->eaMainIcon						&&
	  appGetImagePixmap( ea, ea->eaMainIcon, &pixmap, &mask )	)
	{ SDEB(ea->eaMainIcon); return -1;	}

    if  ( pixmap )
	{
#	ifdef USE_MOTIF
	XtVaSetValues( ea->eaToplevel.atTopWidget,
				    XmNiconPixmap,	pixmap,
				    NULL );
#	endif
#	ifdef USE_GTK
	gdk_window_set_icon( ea->eaToplevel.atTopWidget->window,
				    ea->eaToplevel.atTopWidget->window,
				    pixmap, mask );
#	endif
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Resolve application resources.					*/
/*									*/
/*  9)  Theoretically a program could have more than one application	*/
/*	object. This has never beem tested. The use of a single		*/
/*	table and different flags to check for reuse here is one of the	*/
/*	things to fix.							*/
/*									*/
/************************************************************************/

void appGetApplicationResourceValues(	EditApplication *	ea )
    {
    /*  b  */
    if  ( ! ea->eaGotResourceTable )
	{
	appGuiGetResourceValues( &(ea->eaGotResourceTable), ea,
					    ea->eaResourceData,
					    ea->eaResourceTable,
					    ea->eaResourceCount );
	}

    if  ( ! ea->eaGotFileMessageResourceTable )
	{
	appGuiGetResourceValues( &(ea->eaGotFileMessageResourceTable), ea,
					&(ea->eaFileMessageResources),
					ea->eaFileMessageResourceTable,
					ea->eaFileMessageResourceCount );
	}

    /*  9  */
    if  ( ! ea->eaGotApplicationResources )
	{
	appGuiGetResourceValues( &(ea->eaGotApplicationResources), ea,
				(void *)ea,
				APP_ApplicationResourceTable,
				sizeof(APP_ApplicationResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    appInitializeGeometrySettings( ea );

    return;
    }

/************************************************************************/
/*									*/
/*  Collect a list of printers.						*/
/*									*/
/************************************************************************/

int appGetPrintDestinations(		EditApplication *	ea )
    {
    if  ( ea->eaPrintDestinationsCollected )
	{ return 0;	}

    ea->eaPrintDestinationsCollected= 1;

    if  ( utilPrinterGetPrinters( &(ea->eaPrintDestinationCount),
				    &(ea->eaDefaultPrintDestination),
				    &(ea->eaPrintDestinations),
				    ea->eaCustomPrintCommand,
				    ea->eaCustomPrinterName,
				    ea->eaCustomPrintCommand2,
				    ea->eaCustomPrinterName2 ) )
	{ LDEB(1); return -1; 	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Handle Special Calls.						*/
/*									*/
/*  1)  It is not necessary to initialize TedApplication to convert	*/
/*	files. Intercept conversion calls to prevent the program from	*/
/*	becoming interactive. NOTE that the calls differ from the calls	*/
/*	that initialize the application: They begin with -- instead of	*/
/*	++ . To override configurable options, the ++Something calls	*/
/*	should be used.							*/
/*									*/
/************************************************************************/

static int appMainHandleSpecialCall(	EditApplication *	ea,
					const char *		prefix,
					int			getResources,
					int			argc,
					char *			argv[] )
    {
    int			prefixLength= strlen( prefix );
    const char *	command;

    if  ( argc < 2 )
	{ return 0;	}

    if  ( strncmp( argv[1], prefix, prefixLength ) )
	{ return 0;	}

    command= argv[1]+ prefixLength;

    /*  1  */
    if  ( argc == 2				&&
	  ! strcmp( command, "version" )	)
	{
	printf( "%s\n", ea->eaNameAndVersion );
	return 2;
	}

    if  ( argc == 2			&&
	  ! strcmp( command, "platform" )	)
	{
	printf( "%s\n", ea->eaPlatformCompiled );
	return 2;
	}

    if  ( argc == 2			&&
	  ! strcmp( command, "build" )	)
	{
	printf( "%s\n", ea->eaHostDateCompiled );
	return 2;
	}

    if  ( argc == 2				&&
	  ! strcmp( command, "fullVersion" )	)
	{
	printf( "%s %s %s\n",
		    ea->eaNameAndVersion,
		    ea->eaPlatformCompiled,
		    ea->eaHostDateCompiled );

	return 2;
	}

    if  ( argc == 4				&&
	  ! strcmp( command, "saveTo" )	)
	{
	if  ( getResources )
	    { appGetApplicationResourceValues( ea );	}

	if  ( appFileConvert( ea, argv[2], argv[3] ) )
	    { SSSDEB(argv[1],argv[2],argv[3]); return -1; }

	return 4;
	}

    /**  print to file **/

    if  ( argc == 4				&&
	  ! strcmp( command, "printToFile" )	)
	{
	if  ( getResources )
	    { appGetApplicationResourceValues( ea );	}

	if  ( appPrintToFile( ea, argv[2], argv[3], (const char *)0 ) )
	    { SSSDEB(argv[1],argv[2],argv[3]); return -1; }

	return 4;
	}

    if  ( argc == 5					&&
	  ! strcmp( command, "printToFilePaper" )	)
	{
	if  ( getResources )
	    { appGetApplicationResourceValues( ea );	}

	if  ( appPrintToFile( ea, argv[2], argv[3], argv[4] ) )
	    { SSSDEB(argv[1],argv[2],argv[3]); return -1; }

	return 5;
	}

    /**  print to printer **/

    if  ( argc == 3			&&
	  ! strcmp( command, "print" )	)
	{
	if  ( getResources )
	    { appGetApplicationResourceValues( ea );	}

	if  ( appPrintToPrinter( ea, argv[2],
					(const char *)0, (const char *)0 ) )
	    { SSDEB(argv[1],argv[2]); return -1; }

	return 3;
	}

    if  ( argc == 4				&&
	  ! strcmp( command, "printPaper" )	)
	{
	if  ( getResources )
	    { appGetApplicationResourceValues( ea );	}

	if  ( appPrintToPrinter( ea, argv[2], (const char *)0, argv[3] ) )
	    { SSDEB(argv[1],argv[2]); return -1; }

	return 4;
	}

    if  ( argc == 4				&&
	  ! strcmp( command, "printToPrinter" )	)
	{
	if  ( getResources )
	    { appGetApplicationResourceValues( ea );	}

	if  ( appPrintToPrinter( ea, argv[2], argv[3], (const char *)0 ) )
	    { SSDEB(argv[1],argv[2]); return -1; }

	return 4;
	}

    if  ( argc == 5					&&
	  ! strcmp( command, "printToPrinterPaper" )	)
	{
	if  ( getResources )
	    { appGetApplicationResourceValues( ea );	}

	if  ( appPrintToPrinter( ea, argv[2], argv[3], argv[4] ) )
	    { SSDEB(argv[1],argv[2]); return -1; }

	return 5;
	}

    /**  ----- **/

    return 0;
    }

/************************************************************************/
/*									*/
/*  Use the contents of /etc/papersize as the default papersize.	*/
/*									*/
/************************************************************************/

static void appDefaultPapersize(	EditApplication *	ea )
    {
    const char *	sizeFile= "/etc/papersize";

    if  ( ! appTestFileExists( sizeFile ) )
	{
	FILE *	f;

	f= fopen( sizeFile, "r" );
	if  ( ! f )
	    { SXDEB(sizeFile,f);	}
	else{
	    char	buf[50];

	    if  ( fgets( buf, sizeof(buf)-1, f ) )
		{
		int		l;

		buf[sizeof(buf)-1]= '\0';
		l= strlen( buf );
		if  ( l > 0 && buf[l-1] == '\n' )
		    { buf[l-1]= '\0'; l--; }

		if  ( l > 0 )
		    {
		    if  ( appSetSystemProperty( ea, "paper", buf ) )
			{ SDEB(buf);	}
		    }
		}
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Generic main() procedure.						*/
/*									*/
/*  1)  Connect to the GUI system.					*/
/*  2)  Retrieve resource values and derive some application wide	*/
/*	settings from them.						*/
/*  3)  Do not die too easily.						*/
/*	And try to be immune for shells that make you sensitive for	*/
/*	signals that an X11 application does not want to know about.	*/
/*									*/
/************************************************************************/

int appMain(	EditApplication *	ea,
		int			argc,
		char *			argv[] )
    {
    int			argTo;
    int			arg;
    int			res;

    setlocale( LC_ALL, "" );

    appDefaultPapersize( ea );

    res= appReadUserProperties( ea );
    if  ( res )
	{ LDEB(res); return 1;	}

    res= appReadSystemProperties( ea );
    if  ( res )
	{ LDEB(res); return 1;	}

    argTo= 1;
    for ( arg= 1; arg < argc; arg++ )
	{
	if  ( arg+ 3 <= argc				&&
	      ! strcmp( argv[arg], "--setProperty" )	)
	    {
	    if  ( appSetUserProperty( ea, argv[arg+ 1], argv[arg+ 2] ) )
		{
		SSDEB(argv[arg+ 1], argv[arg+ 2]);
		return 1;
		}

	    arg += 2;
	    continue;
	    }
	argv[argTo++]= argv[arg];
	}
    argc= argTo;

    res= appMainHandleSpecialCall( ea, "--", 1, argc, argv );
    if  ( res < 0 )
	{ LDEB(res); return 1;	}
    if  ( res > 0 )
	{ return 0;		}

    /*  1  */
    if  ( appGuiInitApplication( ea, &argc, &argv ) )
	{ LDEB(1); return 1;	}

    utilInitDocumentGeometry( &(ea->eaDefaultDocumentGeometry) );

    /*  b  */
    appGetApplicationResourceValues( ea );

    if  ( appFinishApplicationWindow( ea ) )
	{ LDEB(1); return -1;	}

    appAllocateCopyPasteTargetAtoms( ea );

#   ifdef USE_MOTIF
    if  ( ea->eaAppSelectionTypeCount > 0 )
	{
	XtAddEventHandler( ea->eaToplevel.atTopWidget,
			PropertyChangeMask, True, appAppGotPasteCall, ea );
	}
#   endif

    res= appMainHandleSpecialCall( ea, "++", 0, argc, argv );
    if  ( res < 0 )
	{ LDEB(res); return 1;	}
    if  ( res > 0 )
	{ return 0;		}

    /*  3  */
    (void) signal( SIGHUP, SIG_IGN );
    (void) signal( SIGINT, SIG_IGN );
#   ifdef SIGWINCH
    (void) signal( SIGWINCH, SIG_IGN );
#   endif

    for ( arg= 1; arg < argc; arg++ )
	{
	const int			read_only= 0;
	AppFileMessageResources *	afmr= &(ea->eaFileMessageResources);
	static char			absolute[1000+1];

	EditDocument *			ed;

	if  ( ! strcmp( argv[arg], "++Upd" ) )
	    {
	    if  ( arg+ 3 >= argc )
		{ LLDEB(arg,argc); arg= argc- 1; continue;	}

	    ed= appOpenDocument( ea, ea->eaToplevel.atTopWidget, (APP_WIDGET)0,
						read_only, argv[arg+ 1] );

	    if  ( ed )
		{
		appSetDocumentTitle( ed, argv[arg+ 2] );
		appSetDocumentFilename( ed, argv[arg+ 3] );

		appDocumentChanged( ed, 1 );
		}

	    if  ( appRemoveFile( argv[arg+ 1] ) )
		{ SDEB(argv[arg+ 1]);	}

	    arg += 3; continue;
	    }

	if  ( ! strcmp( argv[arg], "++New" ) )
	    {
	    if  ( arg+ 2 >= argc )
		{ LLDEB(arg,argc); arg= argc- 1; continue;	}

	    ed= appOpenDocument( ea, ea->eaToplevel.atTopWidget, (APP_WIDGET)0,
						read_only, argv[arg+ 1] );

	    if  ( ed )
		{
		appSetDocumentTitle( ed, argv[arg+ 2] );
		appSetDocumentFilename( ed, (char *)0 );

		appDocumentChanged( ed, 1 );
		}

	    if  ( appRemoveFile( argv[arg+ 1] ) )
		{ SDEB(argv[arg+ 1]);	}

	    arg += 2; continue;
	    }

	if  ( ! strcmp( argv[arg], "++Old" ) )
	    {
	    if  ( arg+ 1 >= argc )
		{ LLDEB(arg,argc); arg= argc- 1; continue;	}

	    if  ( appRemoveFile( argv[arg+ 1] ) )
		{ SDEB(argv[arg+ 1]);	}

	    arg += 1; continue;
	    }

	if  ( ! strcmp( argv[arg], "++Exit" ) )
	    { continue;	}

	if  ( appTestFileExists( argv[arg] ) )
	    {
	    if  ( ea->eaCreateNewFromCommand )
		{
		int	res;

		res= appQuestionRunSubjectYesNoDialog( ea,
				    ea->eaToplevel.atTopWidget, (APP_WIDGET)0,
				    argv[arg], afmr->afmrMakeItQuestion,
		    (char *)0, (char *)0 );

		if  ( res == AQDrespYES )
		    {
		    if  ( appNewDocument( ea, argv[arg] ) )
			{ SDEB(argv[arg]);	}
		    }
		}
	    else{
		appQuestionRunSubjectErrorDialog( ea,
				    ea->eaToplevel.atTopWidget, (APP_WIDGET)0,
				    argv[arg], afmr->afmrNoSuchFileMessage );
		}
	    }
	else{
	    const char * const	fileRelativeTo= (const char *)0;
	    int			relativeIsFile= 1;

	    if  ( appAbsoluteName( absolute, 1000,
			    argv[arg], relativeIsFile, fileRelativeTo ) < 0 )
		{
		SDEB(argv[arg]);
		ed= appOpenDocument( ea, ea->eaToplevel.atTopWidget,
					(APP_WIDGET)0, read_only, argv[arg] );
		}
	    else{
		ed= appOpenDocument( ea, ea->eaToplevel.atTopWidget,
					(APP_WIDGET)0, read_only, absolute );
		}
	    }
	}

    if  ( ea->eaVisibleDocumentCount > 0 )
	{ appHideShellWidget( ea->eaToplevel.atTopWidget );	}
    else{ appShowShellWidget( ea->eaToplevel.atTopWidget );	}

#   ifdef USE_MOTIF
    XtAppMainLoop( ea->eaContext );
#   endif

#   ifdef USE_GTK
    gtk_main();
#   endif

    return 0;
    }
