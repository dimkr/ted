/************************************************************************/
/*									*/
/*  Run file choosers for edit applications.				*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<locale.h>

#   include	<appSystem.h>

#   include	<appFrame.h>
#   include	<appDebugon.h>

#   include	"appFileChooser.h"

#   ifdef USE_MOTIF

#   include	<Xm/Text.h>
#   include	<Xm/TextF.h>
#   include	<Xm/FileSB.h>
#   include	<Xm/Form.h>
#   include	<Xm/MwmUtil.h>
#   include	<Xm/RowColumn.h>

/************************************************************************/
/*									*/
/*  Cleanup when a chooser is destroyed.				*/
/*									*/
/************************************************************************/

static void appFreeAci(		Widget		fileChooser,
				XtPointer	voidaci,
				XtPointer	voidcbs	 )
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
    aci->aciOption= (Widget)0;
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
				void *		voidaci,
				void *		voidcbs )
    {
    AppChooserInformation *	aci= (AppChooserInformation *)voidaci;

    appGuiBreakDialog( &(aci->aciDialog), ACIrespCANCEL );

    return;
    }

/************************************************************************/
/*									*/
/*  Retrieve the file name from a chooser.				*/
/*									*/
/************************************************************************/

static int appChooserGetFilename(	void *			voidcbs,
					AppChooserInformation *	aci )
    {
    XmFileSelectionBoxCallbackStruct *	cbs;

    char *				filename;

    cbs= (XmFileSelectionBoxCallbackStruct *)voidcbs;

    if  ( ! XmStringGetLtoR( cbs->value, XmFONTLIST_DEFAULT_TAG, &filename ) )
	{
	LDEB(1);
	appGuiBreakDialog( &(aci->aciDialog), ACIrespFAILURE );
	return -1;
	}

    if  ( appChooserSaveFilename( aci, filename ) )
	{ XtFree( filename ); return -1;	}

    XtFree( filename );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set the directory for a chooser, based on the file name.		*/
/*  Set a file name derived from the previous name and an extension.	*/
/*									*/
/************************************************************************/

static void appChooserSetText(		Widget  text,
					char *	s )
    {
    if  ( XmIsText( text ) )
	{
	XmTextSetString( text, s );
	XmTextSetInsertionPosition( text, strlen( s ) );

	return;
	}

    if  ( XmIsTextField( text ) )
	{
	XmTextFieldSetString( text, s );
	XmTextFieldSetInsertionPosition( text, strlen( s ) );

	return;
	}

    SDEB( (char *)text );
    }

static void appChooserSetDirectory(	char *			filename,
					AppChooserInformation *	aci )
    {
    char *	slash= strrchr( filename, '/' );

    if  ( slash )
	{
	XmString	directoryString;

	*slash= '\0';
	directoryString= XmStringCreateLocalized( filename );
	*slash= '/';

	XtVaSetValues( aci->aciDialog.adDialog,
				XmNdirectory,		directoryString,
				NULL );

	XmStringFree( directoryString );
	}

    return;
    }

static void appChooserSetFileExtension(	char *	filename,
					char *	extension,
					Widget	text )
    {
    const char *	ext;

    ext= appFileExtensionOfName( filename );

    if  ( ext )
	{
	int	l;
	char *	s;

	l= strlen( filename )+ strlen( extension )+ 2;
	s= (char *)malloc( l );
	strcpy( s, filename );
	strcpy( s+ ( ext- filename ), extension );

	appChooserSetText( text, s );

	free( s );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Callback from the file chooser to open a file.			*/
/*									*/
/************************************************************************/

static void appOpenChooserOk(	Widget		fileChooser,
				XtPointer	voidaci,
				XtPointer	voidcbs	 )
    {
    AppChooserInformation *	aci= (AppChooserInformation *)voidaci;
    Widget			option= aci->aciOption;

    if  ( appChooserGetFilename( voidcbs, aci ) )
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
/*  The file selection box was toold to filter via the normal motif	*/
/*  mechanisms: Adapt the OptionMenu with extensions to the filter.	*/
/*									*/
/************************************************************************/

static void appFileFilterActivated(	Widget		w,
					XtPointer	voidaci,
					XtPointer	voidpbcs )
    {
    AppChooserInformation *	aci= (AppChooserInformation *)voidaci;

    Widget text= XmFileSelectionBoxGetChild( aci->aciDialog.adDialog,
							XmDIALOG_FILTER_TEXT );

    char *		filter= XmTextGetString( text );
    const char *	extS;


    if  ( ! filter )
	{ return;	}

    extS= appFileExtensionOfName( filter );
    if  ( extS )
	{
	int				i;
	const AppFileExtension *	afe= aci->aciExtensions;

	Widget				current= (Widget)0;
	WidgetList			children;
	Cardinal			childCount= 0;

	XtVaGetValues( aci->aciFilterOptionmenu.aomPulldown,
			XmNmenuHistory,		&current,
			XmNchildren,		&children,
			XmNnumChildren,		&childCount,
			NULL );

	for ( i= 0; i < childCount; i++ )
	    {
	    if  ( children[i] == current )
		{ break;	}
	    }

	if  ( i >= childCount				||
	      i >= aci->aciExtensionCount		||
	      strcmp( extS, afe[i].afeExtension )	)
	    {
	    for ( i= 0; i < aci->aciExtensionCount; afe++, i++ )
		{
		if  ( i >= childCount )
		    { continue;	}
		if  ( ! XtIsManaged( children[i] ) )
		    { continue;	}
		if  ( ! XtIsSensitive( children[i] ) )
		    { continue;	}

		if  ( ! strcmp( extS, afe->afeExtension )	)
		    { break;	}
		}

	    if  ( i < aci->aciExtensionCount )
		{ appSetOptionmenu( &(aci->aciFilterOptionmenu), i );	}
	    }
	}

    XtFree( filter );

    return;
    }

/************************************************************************/
/*									*/
/*  Add a pulldown with filters to a file chooser.			*/
/*									*/
/************************************************************************/

static void appFileFilterChosen(	Widget		w,
					XtPointer	voidaci,
					XtPointer	voidpbcs )
    {
    AppChooserInformation *	aci= (AppChooserInformation *)voidaci;
    const AppFileExtension *	afe= aci->aciExtensions;
    Widget			chooser= aci->aciDialog.adDialog;
    Widget			text;

    int				previousExtension= aci->aciFormat;
    short			extensionChosen= -1;

    char *			filter;
    XmString			filterString;

    char *			fileSelected;

    text= XmFileSelectionBoxGetChild( chooser, XmDIALOG_TEXT );
    fileSelected= appGetStringFromTextWidget( text );

    XtVaGetValues( w,	XmNpositionIndex,	&extensionChosen,
			NULL );

    if  ( extensionChosen < 0 )
	{ LDEB(extensionChosen); return;	}

    filter= afe[extensionChosen].afeFilter;
    filterString= XmStringCreateLocalized( filter );

    XtVaSetValues( chooser,
			XmNpattern,		filterString,
			NULL );

    if  ( fileSelected )
	{
	const char *	extS;

	extS= appFileExtensionOfName( fileSelected );

	if  ( extS							&&
	      previousExtension >= 0					&&
	      afe[previousExtension].afeExtension			&&
	      afe[extensionChosen].afeExtension				&&
	      ! strcmp( extS, afe[previousExtension].afeExtension )	)
	    {
	    char *	ext= afe[extensionChosen].afeExtension;

	    appChooserSetFileExtension( fileSelected, ext, text );
	    }
	else{ appChooserSetText( text, fileSelected ); }

	appFreeStringFromTextWidget( fileSelected );
	}

    XmStringFree( filterString );

    aci->aciFormat= extensionChosen;

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a filter dropdown in a file chooser.				*/
/*									*/
/************************************************************************/

static void appChooserMakeOptionmenu(	AppOptionmenu *	aom,
					Widget		parentForm )
    {
    Arg				al[20];
    int				ac= 0;

    appInitOptionmenu( aom );

    ac= 0;
    XtSetArg( al[ac], XmNresizeWidth,		False ); ac++;
    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNbottomAttachment,	XmATTACH_FORM ); ac++;

    aom->aomPulldown= XmCreatePulldownMenu( parentForm, WIDGET_NAME, al, ac );

    ac= 0;
    XtSetArg( al[ac], XmNsubMenuId,		aom->aomPulldown ); ac++;

    XtSetArg( al[ac], XmNmarginHeight,		0 ); ac++;
    XtSetArg( al[ac], XmNmarginWidth,		0 ); ac++;
    XtSetArg( al[ac], XmNspacing,		0 ); ac++;

    XtSetArg( al[ac], XmNtopAttachment,		XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNleftAttachment,	XmATTACH_FORM ); ac++;
    XtSetArg( al[ac], XmNrightAttachment,	XmATTACH_FORM ); ac++;

    XtSetArg( al[ac], XmNtopOffset,		0 ); ac++;
    XtSetArg( al[ac], XmNleftOffset,		5 ); ac++;
    XtSetArg( al[ac], XmNrightOffset,		5 ); ac++;

    aom->aomInplace= XmCreateOptionMenu( parentForm, WIDGET_NAME, al, ac );

    appFinishOptionmenuMotif( aom->aomInplace, aom->aomPulldown );

    XtVaSetValues( aom->aomPulldown,
			    XmNresizeWidth,	False,
			    NULL );
    XtVaSetValues( aom->aomInplace,
			    XmNresizeWidth,	False,
			    NULL );

    XtManageChild( aom->aomInplace );

    return;
    }

static void appFileMakeFilter(	AppChooserInformation *	aci )
    {
    Arg				al[20];
    int				ac= 0;

    Widget			child;

    ac= 0;
    XtSetArg( al[ac], XmNresizePolicy,	XmRESIZE_NONE );
    child= XmCreateForm( aci->aciDialog.adDialog, WIDGET_NAME, al, ac );

    appChooserMakeOptionmenu( &(aci->aciFilterOptionmenu), child );

    XtManageChild( child );

    return;
    }

static void appFileFillFilter(	EditApplication *	ea,
				AppChooserInformation *	aci,
				AppFileExtension *	extensions,
				int			extensionCount,
				char *			defaultFilter )
    {
    AppConfigurableResource *	acr;
    AppConfigurableResource *	acri;
    AppFileExtension *		afe;

    int				i;

    int				selected= -1;
    char *			child0Filter= (char *)0;
    int				ignored= 0;

    acr= (AppConfigurableResource *)
		    malloc( extensionCount* sizeof(AppConfigurableResource) );
    if  ( ! acr )
	{ LXDEB(extensionCount,acr); return;	}

    afe= extensions;
    acri= acr;
    for ( i= 0; i < extensionCount; afe++, acri++, i++ )
	{
	APP_SET_RESOURCE( acri, afe->afeId,
			i* sizeof(AppFileExtension)+
				    offsetof(AppFileExtension,afeDescription),
			afe->afeDescription );
	}

    appGuiGetResourceValues( &ignored, ea, extensions, acr, extensionCount );

    free( acr );

    appEmptyOptionmenu( &(aci->aciFilterOptionmenu) );

    afe= extensions;
    for ( i= 0; i < extensionCount; afe++, i++ )
	{
	Widget	fresh;

	fresh= appAddItemToOptionmenu( &(aci->aciFilterOptionmenu),
		    afe->afeDescription, appFileFilterChosen, (void *)aci );

	if  ( selected < 0 )
	    {
	    if  ( defaultFilter )
		{
		if  ( ! strcmp( afe->afeFilter, defaultFilter ) )
		    {
		    selected= i;
		    child0Filter= afe->afeFilter;
		    }
		}
	    else{
		selected= i;
		child0Filter= afe->afeFilter;
		}
	    }
	}

    if  ( selected >= 0 )
	{
	XmString	filterString= (XmString)0;

	appSetOptionmenu( &(aci->aciFilterOptionmenu), selected );

	if  ( child0Filter )
	    {
	    filterString= XmStringCreateLocalized( child0Filter );

	    XtVaSetValues( aci->aciDialog.adDialog,
					XmNpattern,	filterString,
					NULL );

	    XmStringFree( filterString );
	    }
	}

    aci->aciExtensions= extensions;
    aci->aciExtensionCount= extensionCount;
    }

/************************************************************************/
/*									*/
/*  Make a chooser.							*/
/*									*/
/************************************************************************/

static int appMakeFileChooser( AppChooserInformation **	pAci,
				Widget				relative,
				EditApplication *		ea,
				int				withFilter,
				const char *			defaultFilter,
				Widget				option )
    {
    Display *			display= XtDisplay( ea->eaToplevel.atTopWidget );
    int				screen= DefaultScreen( display );
    Widget			text;

    Arg				al[20];
    int				ac= 0;

    XmString			filterString= (XmString)0;

    Widget			helpButton;
    Widget			filterButton;
    AppChooserInformation *	aci;

    MwmHints			hints;

    aci= (AppChooserInformation *)malloc( sizeof(AppChooserInformation) );
    if  ( ! aci )
	{ XDEB(aci); return -1;	}

    appInitAci( aci );

    appFileChooserGetTexts( ea, aci );

    hints.flags= MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;
    hints.functions=	MWM_FUNC_MOVE		|
			MWM_FUNC_RESIZE		;
    hints.decorations=	MWM_DECOR_BORDER	|
			MWM_DECOR_RESIZEH	|
			MWM_DECOR_TITLE		|
			MWM_DECOR_MENU		;

    ac= 0;
    XtSetArg( al[ac], XmNmwmDecorations,	hints.decorations ); ac++;
    XtSetArg( al[ac], XmNmwmFunctions,		hints.functions ); ac++;
    XtSetArg( al[ac], XmNdeleteResponse,	XmDO_NOTHING ); ac++;
    XtSetArg( al[ac], XmNallowShellResize,	True ); ac++;
    XtSetArg( al[ac], XmNuseAsyncGeometry,	True ); ac++;
    XtSetArg( al[ac], XmNwaitForWm,		False ); ac++; /* LessTif BUG */
    XtSetArg( al[ac], XmNwmTimeout,		0 ); ac++; /* LessTif BUG */

    XtSetArg( al[ac], XmNchildPlacement,XmPLACE_ABOVE_SELECTION ); ac++;
#   if	XmVersion >= 2000 && ! defined( LESSTIF_VERSION )
    XtSetArg( al[ac], XmNfileFilterStyle,
					XmFILTER_HIDDEN_FILES ); ac++;
#   endif

    if  ( defaultFilter )
	{
	filterString= XmStringCreateLocalized( (char *)defaultFilter );
	XtSetArg( al[ac], XmNpattern, filterString ); ac++;
	}

    aci->aciDialog.adDialog= XmCreateFileSelectionDialog(
						ea->eaToplevel.atTopWidget,
						WIDGET_NAME, al, ac );
    aci->aciDialog.adTopWidget= XtParent( aci->aciDialog.adDialog );

    if  ( filterString )
	{ XmStringFree( filterString );	}

    helpButton= XmFileSelectionBoxGetChild( aci->aciDialog.adDialog,
						XmDIALOG_HELP_BUTTON );
    XtUnmanageChild( helpButton );

    text= XmFileSelectionBoxGetChild( aci->aciDialog.adDialog,
							XmDIALOG_FILTER_TEXT );
    XtVaSetValues( text,
			XmNbackground,	WhitePixel( display, screen ),
			XmNforeground,	BlackPixel( display, screen ),
			NULL );

    /*  Not necessary
    if  ( withFilter )
	{
	XtAddCallback( text, XmNactivateCallback,
				    appFileFilterActivated, (void *)aci );
	}
    */

    filterButton= XmFileSelectionBoxGetChild( aci->aciDialog.adDialog,
						    XmDIALOG_APPLY_BUTTON );

    if  ( withFilter )
	{
	XtAddCallback( filterButton, XmNactivateCallback,
				    appFileFilterActivated, (void *)aci );
	}

    text= XmFileSelectionBoxGetChild( aci->aciDialog.adDialog, XmDIALOG_TEXT );
    XtVaSetValues( text,
			XmNbackground,	WhitePixel( display, screen ),
			XmNforeground,	BlackPixel( display, screen ),
			NULL );

    appSetShellTitle( aci->aciDialog.adTopWidget, option,
						    ea->eaApplicationName );

    XtAddEventHandler( aci->aciDialog.adTopWidget, StructureNotifyMask, False,
					appSetSizeAsMinimum, (void *)0 );

    XtAddCallback( aci->aciDialog.adDialog,
		    XmNcancelCallback, appChooserCancel, (XtPointer)aci );

    XtAddCallback( aci->aciDialog.adDialog,
		    XmNdestroyCallback, appFreeAci, (void *)aci );

    aci->aciApplication= ea;

    *pAci= aci;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Run some kind of a file chooser.					*/
/*									*/
/*  1)  Cope with Motif's silly behavior.				*/
/*									*/
/************************************************************************/

static void appRunFileChooser(	EditApplication *		ea,
				AppChooserInformation *		aci,
				Widget				relative )
    {
    aci->aciFilename= (char *)0;

    appGuiShowDialog( &(aci->aciDialog), relative );

    /*  1  */
    if  ( aci->aciFilterOptionmenu.aomInplace )
	{
	XtVaSetValues( XtParent( aci->aciFilterOptionmenu.aomInplace ),
				XmNresizePolicy,	XmRESIZE_NONE,
				NULL );
	}

    appGuiRunDialog( &(aci->aciDialog), ACIrespNONE, ea );

    appGuiHideDialog( &(aci->aciDialog) );

    return;
    }

/************************************************************************/
/*									*/
/*  Open a document.							*/
/*									*/
/************************************************************************/

void appRunOpenChooser( Widget			option,
			Widget			relative,
			int			extensionCount,
			AppFileExtension *	extensions,
			char *			defaultFilter,
			void *			through,
			APP_OPEN_DOCUMENT	openDocument,
			EditApplication *	ea )
    {
    AppChooserInformation *	aci= (AppChooserInformation *)0;
    const int			withFilter= extensionCount > 0;

    if  ( appMakeFileChooser( &aci, relative, ea, withFilter, defaultFilter, option ) )
	{ LDEB(1); return;	}

    XtAddCallback( aci->aciDialog.adDialog,
			XmNokCallback, appOpenChooserOk, (XtPointer)aci );

    if  ( withFilter )
	{
	appFileMakeFilter( aci );

	appGuiShowDialog( &(aci->aciDialog), relative );

	appFileFillFilter( ea, aci,
				extensions, extensionCount, defaultFilter );
	}

    aci->aciOption= option;
    aci->aciThrough= through;
    aci->aciOpenDocument= openDocument;
    aci->aciFilename= (char *)0;

    if  ( withFilter )
	{
	WidgetList		children;
	Cardinal		childCount= 0;
	AppFileExtension *	afe;

	unsigned		i;

	XtVaGetValues( aci->aciFilterOptionmenu.aomPulldown,
			    XmNchildren,		&children,
			    XmNnumChildren,		&childCount,
			    NULL );

	afe= extensions;
	for ( i= 0; i < childCount; afe++, i++ )
	    {
	    if  ( ! ( afe->afeUseFlags & APPFILE_CAN_OPEN )	||
		  ( afe->afeUseFlags & APPFILE_IS_BASIC_TYPE )	||
		  ( afe->afeUseFlags & APPFILE_HIDE_OPEN )	)
		{ XtUnmanageChild( children[i] ); continue;	}

	    XtManageChild( children[i] );
	    }
	}

    appRunFileChooser( ea, aci, relative );

    XtDestroyWidget( aci->aciDialog.adTopWidget );

    return;
    }

static int appChooserOpenDocument(	void *		through,
					Widget		relative,
					Widget		option,
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

    appRunOpenChooser( option, ea->eaToplevel.atTopWidget,
			ea->eaFileExtensionCount, ea->eaFileExtensions,
			ea->eaDefaultFileFilter,
			(void *)ea, appChooserOpenDocument, ea );
    }

void appDocFileOpen(	APP_WIDGET	option,
			void *		voided,
			void *		call_data )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;

    appRunOpenChooser( option, ed->edToplevel.atTopWidget,
			ea->eaFileExtensionCount, ea->eaFileExtensions,
			ea->eaDefaultFileFilter,
			(void *)ea, appChooserOpenDocument, ea );
    }

static void appSaveOkPushed(		Widget		fileChooser,
					XtPointer	voidaci,
					XtPointer	voidcbs	 )
    {
    AppChooserInformation *		aci= (AppChooserInformation *)voidaci;
    int					response;

    if  ( appChooserGetFilename( voidcbs, aci ) )
	{ return;	}

    response= appFileChooserTestNameForSave( aci, aci->aciFilename );

    switch( response )
	{
	case ACIrespSAVE:
	    appGuiBreakDialog( &(aci->aciDialog), ACIrespSAVE );
	    return;

	case ACIrespCANCEL:
	    appGuiBreakDialog( &(aci->aciDialog), ACIrespCANCEL );
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

/************************************************************************/
/*									*/
/*  Run Filechooser for save.						*/
/*									*/
/************************************************************************/

static int appSaveChooserFilterExtensions( EditApplication *	ea,
					const EditDocument *	ed,
					unsigned int		useFlags,
					AppChooserInformation *	aci )
    {
    WidgetList		children;
    Cardinal		childCount= 0;
    AppFileExtension *	afe;

    int			choice= -1;
    int			i;

    XtVaGetValues( aci->aciFilterOptionmenu.aomPulldown,
			XmNchildren,		&children,
			XmNnumChildren,		&childCount,
			NULL );

    afe= ea->eaFileExtensions;
    for ( i= 0; i < (int)childCount; afe++, i++ )
	{
	if  ( appDocumentTestCanSave( ea, afe, ed->edPrivateData,
							useFlags, i ) )
	    { XtUnmanageChild( children[i] ); continue;	}

	XtManageChild( children[i] );

	if  ( i == ed->edFormat || choice < 0 )
	    { choice= i;	}
	}

    if  ( choice >= 0 )
	{
	XmString	filterString;

	filterString= XmStringCreateLocalized(
				ea->eaFileExtensions[choice].afeFilter );

	appSetOptionmenu( &(aci->aciFilterOptionmenu), choice );

	XtVaSetValues( aci->aciDialog.adDialog,
			XmNpattern,			filterString,
			NULL );

	XmStringFree( filterString );

	if  ( ed->edFilename )
	    {
	    Widget	text;

	    text= XmFileSelectionBoxGetChild( aci->aciDialog.adDialog,
							    XmDIALOG_TEXT );

	    if  ( choice == ed->edFormat )
		{ appChooserSetText( text, ed->edFilename ); }
	    else{
		char *	ext= ea->eaFileExtensions[choice].afeExtension;

		if  ( ext )
		    {
		    appChooserSetFileExtension( ed->edFilename,
							    ext, text );
		    }
		}
	    }

	aci->aciFormat= choice;
	}

    return 0;
    }

int appRunSaveChooser(		APP_WIDGET		option,
				APP_WIDGET		relative,
				EditApplication *	ea,
				const EditDocument *	ed,
				unsigned int		useFlags,
				int *			pFormat,
				char **			pFilename )
    {
    AppChooserInformation *	aci= (AppChooserInformation *)0;
    const int			withFilter= ea->eaFileExtensionCount > 0;
    int				response;

    if  ( appMakeFileChooser( &aci, relative, ea, withFilter,
					    ea->eaDefaultFileFilter, option ) )
	{ return ACIrespFAILURE;	}

    XtAddCallback( aci->aciDialog.adDialog,
			    XmNokCallback, appSaveOkPushed, (XtPointer)aci );

    if  ( withFilter )
	{
	appFileMakeFilter( aci );

	appGuiShowDialog( &(aci->aciDialog), relative );

	appFileFillFilter( ea, aci,
			    ea->eaFileExtensions, ea->eaFileExtensionCount,
			    ea->eaDefaultFileFilter );
	}

    aci->aciOption= option;

    if  ( ed->edFilename )
	{ appChooserSetDirectory( ed->edFilename, aci ); }

    if  ( ea->eaFileExtensionCount > 0					&&
	  appSaveChooserFilterExtensions( ea, ed, useFlags, aci )	)
	{ LDEB(1); return ACIrespFAILURE;	}

    appRunFileChooser( ea, aci, relative );

    response= aci->aciDialog.adResponse;

    if  ( response == ACIrespSAVE )
	{
	*pFilename= aci->aciFilename;
	*pFormat= aci->aciFormat;
	}

    XtDestroyWidget( aci->aciDialog.adTopWidget );

    return response;
    }

/************************************************************************/
/*									*/
/*  Run Filechooser for print to file.					*/
/*									*/
/************************************************************************/

int appRunPrintToFileChooser(		Widget			option,
					Widget			panel,
					EditApplication *	ea,
					const EditDocument *	ed,
					char **			pFilename )
    {
    AppChooserInformation *	aci= (AppChooserInformation *)0;
    const int			withFilter= 0;
    int				response;

    if  ( appMakeFileChooser( &aci, panel, ea, withFilter, "*.ps", option ) )
	{ LDEB(1); return ACIrespFAILURE;	}

    XtAddCallback( aci->aciDialog.adDialog,
				XmNokCallback, appSaveOkPushed, (void *)aci );

    aci->aciExtension= "ps";
    aci->aciOption= option;
    aci->aciFilename= (char *)0;

    if  ( ed->edFilename )
	{
	Widget	text;

	text= XmFileSelectionBoxGetChild(
				    aci->aciDialog.adDialog, XmDIALOG_TEXT );

	appChooserSetDirectory( ed->edFilename, aci );
	appChooserSetFileExtension( ed->edFilename, "ps", text );
	}

    appRunFileChooser( ea, aci, panel );

    response= aci->aciDialog.adResponse;

    if  ( response == ACIrespSAVE )
	{ *pFilename= aci->aciFilename; }

    XtDestroyWidget( aci->aciDialog.adTopWidget );

    return response;
    }

#   endif
