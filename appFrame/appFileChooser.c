/************************************************************************/
/*									*/
/*  Run file choosers for edit applications.				*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	<locale.h>

#   include	<appSystem.h>
#   include	<sioStdio.h>

#   include	<appFrame.h>
#   include	<appDebugon.h>

#   include	"appFileChooser.h"

/************************************************************************/
/*									*/
/*  Error messages.							*/
/*									*/
/************************************************************************/

static AppConfigurableResource APP_ChooserResourceTable[]=
    {
	APP_RESOURCE( "chooserNoFilename",
		    offsetof(AppFileChooserResources,acrNoFilenameMessage),
		    "Please Give a Name" ),
	APP_RESOURCE( "chooserIsDirectory",
		    offsetof(AppFileChooserResources,acrIsDirecoryMessage),
		    "Is a directory" ),
	APP_RESOURCE( "chooserNotWritable",
		    offsetof(AppFileChooserResources,acrNotWritableMessage),
		    "You have no permission to write this file." ),
	APP_RESOURCE( "chooserNotReadable",
		    offsetof(AppFileChooserResources,acrNotReadableMessage),
		    "You have no permission to read this file." ),
	APP_RESOURCE( "chooserOverwrite",
		    offsetof(AppFileChooserResources,acrOverwriteMessage),
		    "Do you want to overwrite this file?" ),
	APP_RESOURCE( "chooserNoSuchDir",
		    offsetof(AppFileChooserResources,acrNoSuchDirMessage),
		    "This directory does not exist." ),
    };

/************************************************************************/
/*									*/
/*  Retrieve the configurable texts of a file chooser.			*/
/*									*/
/************************************************************************/

void appFileChooserGetTexts(	EditApplication *		ea,
				AppChooserInformation *		aci )
    {
    static int				gotResources= 0;
    static AppFileChooserResources	APP_ChooserResourceValues;

    if  ( ! gotResources )
	{
	appGuiGetResourceValues( &gotResources, ea,
				    (void *)&APP_ChooserResourceValues,
				    APP_ChooserResourceTable,
				    sizeof(APP_ChooserResourceTable)/
				    sizeof(AppConfigurableResource) );
	}

    aci->aciResources= &APP_ChooserResourceValues;

    return;
    }

/************************************************************************/
/*									*/
/*  Some tests on the name of a file that is to be opened.		*/
/*									*/
/************************************************************************/

int appFileChooserTestNameForOpen(
				const AppChooserInformation *	aci,
				const char *			filename  )
    {
    EditApplication *		ea= aci->aciApplication;
    APP_WIDGET			option= aci->aciOption;

    int				fileExists= 0;
    int				fileReadable= 0;
    int				isDir= 0;

    const AppFileChooserResources *	acr= aci->aciResources;

    fileExists= appTestFileExists( filename ) == 0;

    if  ( fileExists )
	{ fileReadable= appTestFileReadable( filename ) == 0;	}
    else{ isDir= appTestDirectory( filename ) == 0;		}

    if  ( isDir )
	{
	appQuestionRunSubjectErrorDialog( ea, aci->aciDialog.adTopWidget,
				option, filename, acr->acrIsDirecoryMessage );
	return -1;
	}

    if  ( ! fileExists )
	{
	AppFileMessageResources *	afmr= &(ea->eaFileMessageResources);

	appQuestionRunSubjectErrorDialog( ea, aci->aciDialog.adTopWidget,
			    option, filename, afmr->afmrNoSuchFileMessage );

	return -1;
	}

    if  ( ! fileReadable )
	{
	appQuestionRunSubjectErrorDialog( ea, aci->aciDialog.adTopWidget,
			    option, filename, acr->acrNotReadableMessage );
	return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save and partially validate the file name of a file chooser.	*/
/*									*/
/************************************************************************/

int appChooserSaveFilename(		AppChooserInformation *	aci,
					const char *		filename )
    {
    EditApplication *			ea= aci->aciApplication;
    APP_WIDGET				option= aci->aciOption;

    char *				extension= (char *)0;
    const char *			slash;
    const char *			relative;

    int					fileNameLength;

    char *				fresh;

    const AppFileChooserResources *	acr= aci->aciResources;

    fileNameLength= strlen( filename );
    fresh= malloc( fileNameLength+ 11 );
    if  ( ! fresh )
	{
	LXDEB(fileNameLength,fresh);
	appGuiBreakDialog( &(aci->aciDialog), ACIrespFAILURE );
	return -1;
	}
    strcpy( fresh, filename );

    if  ( aci->aciFilename )
	{ free( aci->aciFilename ); aci->aciFilename= (char *)0;	}
    aci->aciFilename= fresh;

    if  ( aci->aciExtensionCount > 0 )
	{
	if  ( aci->aciFormat >= 0 && aci->aciFormat < aci->aciExtensionCount )
	    {
	    const AppFileExtension *	afe= aci->aciExtensions+ aci->aciFormat;

	    extension= afe->afeExtension;
	    }
	}
    else{ extension= aci->aciExtension;	}

    slash= strrchr( filename, '/' );
    if  ( ! slash )
	{ relative= filename;		}
    else{ relative= slash+ 1;		}

    if  ( ! relative[0] )
	{
	appQuestionRunErrorDialog( ea, aci->aciDialog.adTopWidget, option,
						acr->acrNoFilenameMessage );

	free( aci->aciFilename ); aci->aciFilename= (char *)0;
	/* NO: just stay in the loop.
	appGuiBreakDialog( &(aci->aciDialog, ACIrespFAILURE );
	*/
	return -1;
	}

    if  ( extension && extension[0] )
	{
	char *	dot;

	dot= strrchr( relative, '.' );

	if  ( ! dot				||
	      strcmp( dot+ 1, extension )	)
	    {
	    int		le;

	    le= strlen( extension );

	    fresh= malloc( fileNameLength+ le+ 2+ 11 );
	    if  ( ! fresh )
		{
		LXDEB(fileNameLength,fresh);
		appGuiBreakDialog( &(aci->aciDialog), ACIrespFAILURE );
		return -1;
		}

	    strcpy( fresh, filename );

	    if  ( aci->aciFilename )
		{ free( aci->aciFilename ); aci->aciFilename= (char *)0; }
	    aci->aciFilename= fresh;

	    if  ( dot && ! dot[1] )
		{
		strcpy( fresh+ fileNameLength,    extension );
		}
	    else{
		strcpy( fresh+ fileNameLength   , "." );
		strcpy( fresh+ fileNameLength+ 1, extension );
		}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Some tests on the name for a file to save a document to.		*/
/*									*/
/************************************************************************/

int appFileChooserTestNameForSave(
				const AppChooserInformation *	aci,
				const char *			filename  )
    {
    EditApplication *		ea= aci->aciApplication;
    APP_WIDGET			option= aci->aciOption;

    int				isDir= 0;
    int				fileExists= 0;
    int				dirExists= 0;
    int				fileWritable= 0;

    const AppFileChooserResources *	acr= aci->aciResources;

    fileExists= appTestFileExists( filename ) == 0;

    if  ( fileExists )
	{ fileWritable= appTestFileWritable( filename ) == 0;	}
    else{ isDir= appTestDirectory( filename ) == 0;		}

    if  ( ! fileExists && ! isDir )
	{
	char *	scratch;
	char *	slash;

	scratch= (char *)malloc( strlen( filename )+ 11 );
	if  ( ! scratch )
	    { XDEB(scratch); return ACIrespFAILURE;	}

	strcpy( scratch, filename );
	slash= strrchr( scratch, '/' );
	if  ( slash )
	    {
	    *slash= '\0';
	    dirExists= appTestDirectory( scratch ) == 0;
	    }

	free( scratch );
	}

    if  ( isDir )
	{
	appQuestionRunSubjectErrorDialog( ea, aci->aciDialog.adTopWidget,
				option, filename, acr->acrIsDirecoryMessage );
	return ACIrespNONE;
	}

    if  ( fileExists )
	{
	if  ( ! fileWritable )
	    {
	    appQuestionRunSubjectErrorDialog(
			ea, aci->aciDialog.adTopWidget, option,
			filename, acr->acrNotWritableMessage );
	    return ACIrespNONE;
	    }
	else{
	    int		rcc;

	    rcc= appQuestionRunSubjectYesNoCancelDialog(
			ea, aci->aciDialog.adTopWidget, option,
			filename, acr->acrOverwriteMessage,
			(char *)0, (char *)0, (char *)0 );

	    switch( rcc )
		{
		case AQDrespYES:
		    return ACIrespSAVE;
		case AQDrespNO:
		    return ACIrespNONE;
		default:
		    LDEB(rcc);
		    /*FALLTHROUGH*/
		case AQDrespCANCEL:
		    return ACIrespCANCEL;
		}
	    }
	}
    else{
	if  ( ! dirExists )
	    {
	    char *	slash= strrchr( filename+ 1, '/' );

	    if  ( slash )
		{
		*slash= '\0';

		appQuestionRunSubjectErrorDialog(
			    ea, aci->aciDialog.adTopWidget, option,
			    filename, acr->acrNoSuchDirMessage );
		return ACIrespNONE;
		}
	    }
	}

    return ACIrespSAVE;
    }

void appDocPrintToFile(	APP_WIDGET			option,
			APP_WIDGET			panel,
			EditDocument *			ed,
			const PrintGeometry *		pg )
    {
    EditApplication *		ea= ed->edApplication;
    int				response;
    char *			filename;

    SimpleOutputStream *	sos;

    PrintJob			pj;

    if  ( ! ea->eaPrintDocument )
	{ XDEB(ea->eaPrintDocument); return;	}

    appPrintJobForEditDocument( &pj, ed );

    response= appRunPrintToFileChooser( option, panel, ea, ed, &filename );

    switch( response )
	{
	case ACIrespCANCEL:
	    break;

	case ACIrespSAVE:
	    sos= sioOutStdioOpen( filename );

	    if  ( ! sos )
		{ SXDEB(filename,sos); free( filename ); return;	}

	    appCallPrintFunction( sos, &pj, pg );

	    sioOutClose( sos );

	    free( filename );
	    break;

	default:
	    LDEB(response); break;
	}
    }

/************************************************************************/
/*									*/
/*  Callbacks from the file menu: Save a document and remember name and	*/
/*  type=format of the output file.					*/
/*									*/
/************************************************************************/

extern APP_MENU_CALLBACK_H( appDocFileSaveAs, option, voided, call_data )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;
    const int		interactive= 1;

    int			response;
    int			format;
    char *		filename;

    if  ( ! ea->eaSaveDocument )
	{ XDEB(ea->eaSaveDocument); return;	}

    response= appRunSaveChooser( option, ed->edToplevel.atTopWidget, ea, ed,
				    APPFILE_CAN_SAVE, &format, &filename );

    switch( response )
	{
	case ACIrespCANCEL:
	    break;

	case ACIrespSAVE:
	    if  ( appDocSaveDocumentByName( ed, option, interactive,
							format, filename ) )
		{ free( filename ); return;	}

	    if  ( format >= 0				&&
		  format < ea->eaFileExtensionCount	)
		{
		const AppFileExtension *	afe;

		afe= &(ea->eaFileExtensions[format]);

		if  ( afe->afeUseFlags & APPFILE_CAN_OPEN )
		    {
		    appDocumentChanged( ed, 0 );

		    if  ( appSetDocumentFilename( ed, filename ) )
			{ SDEB(filename);	}
		    if  ( appSetDocumentTitle( ed, filename ) )
			{ SDEB(filename);	}

		    ed->edFileReadOnly= 0;
		    ed->edFormat= format;
		    }
		}

	    free( filename );
	    break;

	default:
	    LDEB(response); break;
	}
    }

