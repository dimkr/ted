/************************************************************************/
/*									*/
/*  Header file for internal use by the FileChooser implementation.	*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  For opening/saving documents.					*/
/*									*/
/************************************************************************/

#   define	ACIrespNONE	0
#   define	ACIrespSAVE	1
#   define	ACIrespCANCEL	2
#   define	ACIrespOPEN	3
#   define	ACIrespFAILURE	4

typedef struct AppFileChooserResources
    {
    char *			acrNoFilenameMessage;
    char *			acrIsDirecoryMessage;
    char *			acrNotWritableMessage;
    char *			acrNotReadableMessage;
    char *			acrOverwriteMessage;
    char *			acrNoSuchDirMessage;
    } AppFileChooserResources;

typedef struct AppChooserInformation
    {
    AppDialog				aciDialog;

    char *				aciFilename;
    void *				aciThrough;
    APP_OPEN_DOCUMENT			aciOpenDocument;
    const AppFileExtension *		aciExtensions;
    int					aciExtensionCount;
    AppOptionmenu			aciFilterOptionmenu;
    EditApplication *			aciApplication;
    APP_WIDGET				aciOption;
    int					aciFormat;
    char *				aciExtension;

    const AppFileChooserResources *	aciResources;
    } AppChooserInformation;

/************************************************************************/
/*									*/
/*  Procedure declarations.						*/
/*									*/
/************************************************************************/

extern void appFileChooserGetTexts(	EditApplication *		ea,
					AppChooserInformation *		aci );

extern int appFileChooserTestNameForOpen(
				const AppChooserInformation *	aci,
				const char *			filename );

extern int appFileChooserTestNameForSave(
				const AppChooserInformation *	aci,
				const char *			filename );

extern int appChooserSaveFilename(	AppChooserInformation *	aci,
					const char *		filename );

extern int appRunSaveChooser(		APP_WIDGET		option,
					APP_WIDGET		relative,
					EditApplication *	ea,
					const EditDocument *	ed,
					unsigned int		useFlags,
					int *			pFormat,
					char **			pFilename );

