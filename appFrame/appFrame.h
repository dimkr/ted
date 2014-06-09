/************************************************************************/
/*									*/
/*  Basic structures for a Gui document manipulator application.	*/
/*									*/
/************************************************************************/

#   ifndef	APP_FRAME_H
#   define	APP_FRAME_H

#   include	<stdio.h>

#   include	<appGuiBase.h>
#   include	<appGuiResource.h>

#   include	"geo2DInteger.h"
#   include	"utilPs.h"
#   include	<psFont.h>
#   include	"utilPrinter.h"
#   include	"utilMailContent.h"
#   include	<appIcons.h>
#   include	<appTool.h>
#   include	<appDraw.h>
#   include	<appInspector.h>

/************************************************************************/
/*									*/
/*  For configuring texts of 'Really' dialogs.				*/
/*									*/
/************************************************************************/

typedef struct AppFileMessageResources
    {
					/********************************/
					/*  Format for 'New' title	*/
					/********************************/
    char *	afmrNamelessTitleFormat;
					/********************************/
					/*  Really Close? Dialog.	*/
					/********************************/
    char *	afmrReallyCloseQuestion;
    char *	afmrReallyCloseSaveIt;
    char *	afmrReallyCloseDontSave;
    char *	afmrReallyCloseCancel;
					/********************************/
					/*  Really Quit? Dialog.	*/
					/********************************/
    char *	afmrReallyQuitQuestion;
    char *	afmrReallyQuitReview;
    char *	afmrReallyQuitAnyway;
    char *	afmrReallyQuitCancel;
					/********************************/
					/*  Messages about  a file.	*/
					/********************************/
    char *	afmrFileNoAccess;
    char *	afmrFileReadOnly;
    char *	afmrNoSuchFileMessage;
    char *	afmrFileNotWritable;
    char *	afmrMakeItQuestion;
    } AppFileMessageResources;

/************************************************************************/
/*  A menu, menu item in an application.				*/
/************************************************************************/

typedef enum MenuItemType
    {
    ITEMtyOPTION= 0,
    ITEMtyTOGGLE_OFF,
    ITEMtyTOGGLE_ON,
    ITEMtySEPARATOR
    } MenuItemType;

typedef struct AppMenuItem
    {
    char *		amiTextResName;
    char *		amiItemText;

    char *		amiKeyResName;
    char *		amiKey;

    char *		amiKeyTextResName;
    char *		amiKeyText;

    MenuItemType	amiItemType;
    APP_MENU_CALLBACK_T	amiCallback;
    APP_WIDGET		amiOptionWidget;
    } AppMenuItem;

/************************************************************************/
/*									*/
/*  Kinds of files that an application can open.			*/
/*									*/
/************************************************************************/

#   define	APPFILE_CAN_OPEN	0x01
#   define	APPFILE_CAN_SAVE	0x02
#   define	APPFILE_IS_BASIC_TYPE	0x04
#   define	APPFILE_HIDE_OPEN	0x08

typedef struct AppFileExtension
    {
    char *		afeId;
    char *		afeFilter;
    char *		afeDescription;
    char *		afeExtension;
    unsigned int	afeUseFlags;
    } AppFileExtension;

/************************************************************************/
/*									*/
/*  Printing..								*/
/*									*/
/************************************************************************/

typedef struct PrintJob
    {
    void *			pjPrivateData;
    int				pjFormat;
    const char *		pjTitle;
    AppDrawingData *		pjDrawingData;
    struct EditApplication *	pjApplication;
    } PrintJob;

/************************************************************************/
/*									*/
/*  Describes a document generically.					*/
/*									*/
/************************************************************************/

typedef struct EditDocument
    {
    struct EditApplication *	edApplication;

    char *			edFilename;
    char *			edTitle;
    char *			edCheckpointFilename;
    int				edFormat;
    int				edFileReadOnly;
    unsigned int		edDocumentId;

    AppToplevel			edToplevel;

    APP_WIDGET			edMenuBar;
    APP_WIDGET			edMainWindow;
    APP_WIDGET				edFileMenu;
    APP_WIDGET				edFileMenuButton;
    APP_WIDGET				edEditMenu;
    APP_WIDGET				edEditMenuButton;
    APP_WIDGET				edWindowMenu;
    APP_WIDGET				edWindowMenuButton;
    APP_WIDGET				edHelpMenu;
    APP_WIDGET				edHelpMenuButton;

    APP_WIDGET			edToolbar;
    APP_WIDGET			edScrolledWindow;
    APP_WIDGET				edVerticalScrollbar;
    APP_WIDGET				edHorizontalScrollbar;
#   ifdef USE_GTK
    GtkAdjustment *			edVerticalAdjustment;
    GtkAdjustment *			edHorizontalAdjustment;
#   endif
    APP_WIDGET				edDocumentWidget;

    APP_WIDGET				edLeftRulerWidget;
    APP_WIDGET				edTopRulerWidget;
    APP_WIDGET				edRightRulerWidget;
    APP_WIDGET				edBottomRulerWidget;

    void *				edLeftRuler;
    void *				edTopRuler;
    void *				edRightRuler;
    void *				edBottomRuler;

    int					edLeftRulerWidthPixels;
    int					edTopRulerHeightPixels;
    int					edRightRulerWidthPixels;
    int					edBottomRulerHeightPixels;

    int				edShellExtraWidth;
    int				edShellExtraHeight;

    AppColors			edColors;

    APP_INPUT_CONTEXT		edInputContext;

    int				edHasBeenChanged;
    int				edIsReadonly;
    int				edIsVisible;

    AppDrawingData		edDrawingData;
    DocumentRectangle		edVisibleRect;

    void *			edPrivateData;

    struct AppSelectionTargetType *
				edTargetTypes;
    int				edTargetTypeCount;

    int				edMapped;	/*  Struggle with fvwm	*/
    int				edNotYetDrawn;	/*  For FirstVisible	*/
    } EditDocument;

/************************************************************************/
/*									*/
/*  Describes the application.						*/
/*									*/
/*  1)  Descriptive members.						*/
/*  2)  Allocated at run time.						*/
/*									*/
/************************************************************************/

typedef struct EditApplication
    {
    /*  1  */
    char *		eaApplicationName;
    char *		eaNameAndVersion;
    char *		eaReference;
    char *		eaPlatformCompiled;
    char *		eaHostDateCompiled;

    char *		eaDocumentWidgetName;
    char *		eaPageToolName;
    char *		eaPrintDialogName;
    char *		eaMailDialogName;
    char *		eaMessageDialogName;
    char *		eaMainIcon;
    char *		eaMainPicture;
    AppFileExtension *	eaFileExtensions;
    int			eaFileExtensionCount;
    char *		eaDefaultFileFilter;
    char *		eaDocumentIcon;
    void *		eaResourceData;
    AppConfigurableResource *	eaResourceTable;
    int			eaResourceCount;
    AppConfigurableResource *	eaFileMessageResourceTable;
    int			eaFileMessageResourceCount;
    int			eaCreateNewFromCommand;

    int			eaScreenPixelsWide;
    int			eaScreenPixelsHigh;
    float		eaMagnification;

    int			eaLeftRulerWidthMM;
    int			eaTopRulerHeightMM;
    int			eaRightRulerWidthMM;
    int			eaBottomRulerHeightMM;
    int			eaPageGapMM;

    int			eaLeftRulerWidthMultiple;
    int			eaTopRulerHeightMultiple;
    int			eaRightRulerWidthMultiple;
    int			eaBottomRulerHeightMultiple;

    char *		eaUnitString;
    char *		eaPaperString;
    char *		eaLeftMarginString;
    char *		eaTopMarginString;
    char *		eaRightMarginString;
    char *		eaBottomMarginString;
    int			eaUnitInt;
    DocumentGeometry	eaDefaultDocumentGeometry;

    char **		eaAppFileMenuText;
    AppMenuItem *	eaAppFileMenuItems;
    int			eaAppFileMenuItemCount;

    char **		eaAppWinMenuText;
    AppMenuItem *	eaAppWinMenuItems;
    int			eaAppWinMenuItemCount;

    char **		eaDocFileMenuText;
    AppMenuItem *	eaDocFileMenuItems;
    int			eaDocFileMenuItemCount;

    char **		eaDocEditMenuText;
    AppMenuItem *	eaDocEditMenuItems;
    int			eaDocEditMenuItemCount;

    char **		eaDocWindowMenuText;
    AppMenuItem *	eaDocWindowMenuItems;
    int			eaDocWindowMenuItemCount;

    char **		eaDocHelpMenuText;
    AppMenuItem *	eaDocHelpMenuItems;
    int			eaDocHelpMenuItemCount;

    char **		eaAppHelpMenuText;
    AppMenuItem *	eaAppHelpMenuItems;
    int			eaAppHelpMenuItemCount;

    void *		(*eaMakePrivateData)( void );

    int			(*eaMakeDocumentWidget)(
				struct EditApplication *	ea,
				EditDocument *			ed );
    int			(*eaOpenDocument)(
				struct EditApplication *	ea,
				void *				privateData,
				int *				pFormat,
				APP_WIDGET			relative,
				APP_WIDGET			option,
				const char *			filename );
    int			(*eaNewDocument)(
				struct EditApplication *	ea,
				EditDocument *			ed,
				const char *			filename );
    int			(*eaLayoutDocument)(
				void *				privateData,
				int				format,
				AppDrawingData *		add,
				const DocumentGeometry *	defDg );
    int			(*eaFinishDocumentSetup)(
				EditDocument *			ed );
    void		(*eaDocumentFirstVisible)(
				EditDocument *			ed );
    int			(*eaCanSaveDocument)(
				const void *			privateData,
				int				format );
    int			(*eaSaveDocument)(
				const void *			privateData,
				int				format,
				const char *			applicationId,
				const char *			documentTitle,
				const char *			filename );
    void		(*eaFreeDocument)(
				void *				privateData,
				int				format,
				AppDrawingData *		add );
    void		(*eaSuggestNup)(
				PrintGeometry *			pg,
				void *				privateData );
    int			(*eaPrintDocument)(
				SimpleOutputStream *		sos,
				const PrintJob *		pj,
				const PrintGeometry *		pg );
    void		(*eaDrawRectangle)(
				APP_WIDGET			w,
				EditDocument *			ed,
				DocumentRectangle *		drClip,
				int				ox,
				int				oy );

    void		(*eaVisibleDocumentCountChanged)(
				    struct EditApplication *	ea,
				    int				from,
				    int				to );

    void		(*eaMakePrivateApplicationMenus)(
				    struct EditApplication *	ea,
				    APP_WIDGET			menubar );

    void		(*eaMakePrivateDocumentMenus)(
				    struct EditApplication *	ea,
				    EditDocument *		ed,
				    APP_WIDGET			menubar );

    void		(*eaDocCopy)( EditDocument *);
    void		(*eaDocCut)( EditDocument *);
    void		(*eaDocSelAll)( EditDocument * );

    void		(*eaSetPageLayout)(
				    struct EditApplication *	ea,
				    const DocumentGeometry *	dgNew,
				    const PropertyMask *	setMask,
				    int				wholeDocument );

			/************************************************/
			/*  User input on the document widget:		*/
			/************************************************/
    APP_EVENT_HANDLER_T	eaDocumentMouseHandler;
    APP_EVENT_HANDLER_T	eaDocumentKeyboardHandler;
    APP_EVENT_HANDLER_T	eaObserveFocus;

    APP_SCROLLBAR_CALLBACK	eaDocHorizontalScrollbarCallback;
    APP_SCROLLBAR_CALLBACK	eaDocVerticalScrollbarCallback;

    void		(*eaScrollHorizontalRuler) (void *, APP_WIDGET, int );
    void		(*eaSetHorizontalRulerRange)(
						void *		voidtr,
						APP_WIDGET	w,
						int		docVisX0,
						int		docVisX1,
						int		docBackX1 );

    void		(*eaScrollVerticalRuler) (void *, APP_WIDGET, int );
    void		(*eaSetVerticalRulerRange)(
						void *		voidtr,
						APP_WIDGET	w,
						int		docVisY0,
						int		docVisY1,
						int		docBackY1 );

    int			(*eaSetTopRuler)(	EditDocument *	ed );
    int			(*eaSetLeftRuler)(	EditDocument *	ed );

    void		(*eaFreeLeftRuler)(
					void *			leftRuler );
    void		(*eaFreeTopRuler)(
					void *			topRuler );
    void		(*eaFreeRightRuler)(
					void *			rightRuler );
    void		(*eaFreeBottomRuler)(
					void *			bottomRuler );

    struct AppSelectionType *	eaDocSelectionTypes;
    int				eaDocSelectionTypeCount;

    struct AppSelectionType *	eaAppSelectionTypes;
    int				eaAppSelectionTypeCount;

    PrintDestination *	eaPrintDestinations;
    int			eaPrintDestinationCount;
    int			eaDefaultPrintDestination;
    int			eaPrintDestinationsCollected;

    MailContent *	eaMailContents;
    int			eaMailContentCount;
    int			eaDefaultMailContent;

    NamedPicture *	eaNamedPictures;
    int			eaNamedPictureCount;

    APP_ATOM		eaCloseAtom;

    unsigned int	eaNextDocumentId;

    /*  2  */
#   ifdef USE_MOTIF
    XtAppContext		eaContext;
#   endif

    AppToplevel			eaToplevel;
	APP_INPUT_METHOD	eaInputMethod;
	APP_CURSOR		eaDocumentCursor;
    APP_WIDGET			eaMainWindow;
    APP_WIDGET			eaMenuBar;
    APP_WIDGET			eaFileMenu;
    APP_WIDGET			eaFileMenuButton;
    APP_WIDGET			eaWinMenu;
    APP_WIDGET			eaWinMenuButton;
    APP_WIDGET			eaHelpMenu;
    APP_WIDGET			eaHelpMenuButton;

    void *			eaSpellTool;
    void *			eaFindTool;
    void *			eaPageTool;
    void *			eaSymbolPicker;
    void *			eaPrintDialog;

    int				eaArgc;
    char **			eaArgv;

    EditDocument *		eaCurrentDocument;
    EditDocument **		eaOpenDocuments;
    int				eaOpenCount;
    int				eaVisibleDocumentCount;
    int				eaMainVisibleAsAbout;

    char *			eaAfmDirectory;
    char *			eaFontDirectory;
    char *			eaGhostscriptFontmap;
    char *			eaGhostscriptFontToXmapping;
    int				eaGhostscriptMappingsRead;
    char *			eaDefaultFont;
    char *			eaRulerFont;
    char *			eaPrintDialogFont;
    char *			eaFaxCommand;
    char *			eaCustomPrintCommand;
    char *			eaCustomPrinterName;
    char *			eaCustomPrintCommand2;
    char *			eaCustomPrinterName2;
    char *			eaAuthor;
    char *			eaPageNumberFormat;
    char *			eaFocusColor;

    int				eaSupportXvCopyPaste;
    int				eaHideSaveToOption;
    int				eaUsePostScriptFilters;
    int				eaUsePostScriptIndexedImages;
    int				eaSkipEmptyPages;
    int				eaSkipBlankPages;
    int				eaOmitHeadersOnEmptyPages;

    int				eaGotPaste;
    AppFileMessageResources	eaFileMessageResources;
    char *			eaMagnificationString;
    char *			eaSupportXvCopyPasteString;
    char *			eaHideSaveToOptionString;
    char *			eaUsePostScriptFiltersString;
    char *			eaUsePostScriptIndexedImagesString;
    char *			eaSkipEmptyPagesString;
    char *			eaSkipBlankPagesString;
    char *			eaOmitHeadersOnEmptyPagesString;

    PostScriptFontList		eaPostScriptFontList;

    void *			eaSystemProperties;
    void *			eaUserProperties;

    int				eaGotResourceTable;
    int				eaGotFileMessageResourceTable;
    int				eaGotApplicationResources;

    APP_WIDGET			eaAppFileHideOption;
    } EditApplication;

typedef int (*APP_OPEN_DOCUMENT)(	void *		through,
					APP_WIDGET	relative,
					APP_WIDGET	option,
					const char *	filename );

/************************************************************************/
/*									*/
/*  Callback from the Find Tool.					*/
/*									*/
/************************************************************************/

typedef int (*FindToolFind)(		void *			target );
typedef void (*FindToolReplace)(	void *			target,
					const unsigned char *	guess );
typedef int (*FindToolSetPattern)(	void *			voidea,
					const unsigned char *	pattern,
					int			useRegex );

/************************************************************************/
/*									*/
/*  Describes types of content for Copy/Paste.				*/
/*  The order is the order of desirability for the application.		*/
/*									*/
/************************************************************************/

typedef void (*APP_FORGET_COPY)( APP_WIDGET		w,
				void *			through,
				APP_EVENT *		event );

typedef struct AppSelectionTargetType
    {
    char *		asttTargetString;		/*  ContentType	*/
    APP_ATOM		asttTargetAtom;
    APP_PASTE_REPLY	asttUsePaste;
    APP_GIVE_COPY	asttGiveCopy;
    } AppSelectionTargetType;

typedef struct AppSelectionType
    {
    char *			astSelectionString;	/*  What	*/
    APP_ATOM			astSelectionAtom;

    AppSelectionTargetType *	astTargetTypes;
    int				astTargetTypeCount;

    APP_FORGET_COPY		astForgetCopy;
    } AppSelectionType;

/************************************************************************/
/*									*/
/*  Possible responses in one of Question dialogs.			*/
/*									*/
/************************************************************************/

#   define	AQDrespFAILURE		-1
#   define	AQDrespNONE		0
#   define	AQDrespYES		1
#   define	AQDrespNO		2
#   define	AQDrespCANCEL		3
#   define	AQDrespOK		4

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern APP_WIDGET appMakeMenu(		APP_WIDGET *		pButton,
					AppToplevel *		at,
					EditApplication *	ea,
					APP_WIDGET		parent,
					const char *		itemText,
					int			isHelp,
					AppMenuItem *		items,
					int			itemCount,
					void *			through );

extern EditDocument * appOpenDocument(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					int			read_only,
					const char *		filename );

extern int appNewDocument(	EditApplication *	ea,
				const char *		filename );

extern int appMain(	EditApplication *	ea,
			int			argc,
			char *			argv[] );

extern void appSetDocument(	EditApplication *	ea,
				EditDocument *		ed );

extern void appRemoveDocument(	EditApplication *	ea,
				EditDocument *		ed );

extern void appCloseDocument(	EditApplication *	ea,
				EditDocument *		ed );

extern void appSetCurrentDocument(	EditApplication *	ea,
					EditDocument *		ed );

extern void appDocToFront(	APP_WIDGET		option,
				void *			voided,
				void *			call_data );

extern void appDocumentChanged(	EditDocument *		ed,
				int			changed );

extern int appDocSaveDocumentByName(	EditDocument *		ed,
					APP_WIDGET		option,
					int			interactive,
					int			format,
					const char *		filename );

void appQuitApplication(	APP_WIDGET		option,
				APP_WIDGET		relative,
				EditApplication *	ea );

extern APP_MENU_CALLBACK_H( appAppFileQuit, option, voidea, e );

extern void appDocVisible(	EditApplication *	ea,
				EditDocument *		ed,
				int			visible );

extern APP_MENU_CALLBACK_H( appAppFileOpen, option, voidea, e );
extern APP_MENU_CALLBACK_H( appAppFileNew, option, voidea, e );

extern void appDocSetScrollbarValues(	EditDocument *	ed );

extern void appMouseWheelUp(		EditDocument *	ed );
extern void appMouseWheelDown(		EditDocument *	ed );

extern APP_MENU_CALLBACK_H( appDocFileSaveAs, option, voided, call_data );

extern void appSetShellConstraints(	EditDocument *		ed );

extern APP_MENU_CALLBACK_H( appAppFileMini, option, voided, call_data );
extern APP_MENU_CALLBACK_H( appAppFileHide, option, voided, call_data );
extern APP_MENU_CALLBACK_H( appDocFileNew, option, voided, call_data );
extern APP_MENU_CALLBACK_H( appDocFileOpen, option, voided, call_data );
extern APP_MENU_CALLBACK_H( appDocFileSave, option, voided, call_data );
extern APP_MENU_CALLBACK_H( appDocFileMini, option, voided, call_data );
extern APP_MENU_CALLBACK_H( appDocFileHide, option, voided, call_data );
extern APP_MENU_CALLBACK_H( appDocFileClose, option, voided, call_data );
extern APP_MENU_CALLBACK_H( appDocFileQuit, option, voided, call_data );
extern APP_MENU_CALLBACK_H( appDocEditCopy, option, voided, call_data );
extern APP_MENU_CALLBACK_H( appDocEditCut, option, voided, call_data );
extern APP_MENU_CALLBACK_H( appDocEditPaste, option, voided, call_data );
extern APP_MENU_CALLBACK_H( appDocEditSelAll, option, voided, call_data );

extern APP_CLOSE_CALLBACK_H( appDocFileCloseCallback, w, voided );
extern APP_CLOSE_CALLBACK_H( appAppWmClose, w, voidea );

extern void appRunReallyCloseDialog(	APP_WIDGET		option,
					EditDocument *		ed );

extern int appRunReallyQuitDialog(	APP_WIDGET		option,
					APP_WIDGET		relative,
					EditApplication *	ea );

extern void appMakeDocVisible(	EditApplication *	ea,
				EditDocument *		ed );

extern void appShowDefaultsEditor(	APP_WIDGET	prefOption,
					void *		voidea );

extern int appGetImagePixmap(	EditApplication *	ea,
				const char *		name,
				APP_BITMAP_IMAGE *	pImage,
				APP_BITMAP_MASK *	pMask );

extern void appDiscardImagePixmaps(	EditApplication *	ea );

extern void appFreeDocument(		EditApplication *	ea,
					EditDocument *		ed );

extern int appSetDocumentFilename(	EditDocument *		ed,
					const char *		filename );

extern int appSetDocumentTitle(		EditDocument *		ed,
					const char *		title );

extern void appGetFactors(	EditApplication *	ea,
				double *		pHorPixPerMM,
				double *		pVerPixPerMM,
				double *		pXfac,
				double *		pYfac );

extern int appFinishDocumentSetup(	EditApplication *	ea,
					EditDocument *		ed );

extern int appMakeDocumentWindow(	EditDocument **		pEd,
					EditApplication *	ea,
					int			read_only,
					const char *		title,
					const char *		filename );

extern int appSetupDocument(	EditApplication *	ea,
				EditDocument *		ed );

extern void appRenameDocumentOptions(	EditApplication *	ea,
					EditDocument *		ed,
					char *			title );

extern int appQuestionRunSubjectYesNoCancelDialog( EditApplication *	ea,
						APP_WIDGET	relative,
						APP_WIDGET	option,
						const char *	subject,
						const char *	question,
						const char *	yesText,
						const char *	noText,
						const char *	cancelText );

extern int appQuestionRunYesNoCancelDialog(	EditApplication *	ea,
						APP_WIDGET	relative,
						APP_WIDGET	option,
						const char *	question,
						const char *	yesText,
						const char *	noText,
						const char *	cancelText );

extern void appQuestionRunSubjectErrorDialog(	EditApplication *	ea,
						APP_WIDGET	relative,
						APP_WIDGET	option,
						const char *	subject,
						const char *	message );

extern void appQuestionRunErrorDialog(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		message );

extern int appQuestionRunOkCancelDialog(	EditApplication * ea,
						APP_WIDGET	relative,
						APP_WIDGET	option,
						const char *	question,
						const char *	okText,
						const char *	cancelText );

extern int appQuestionRunSubjectOkCancelDialog( EditApplication * ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		subject,
					const char *		question,
					const char *		okText,
					const char *		cancelText );

extern int appQuestionRunSubjectYesNoDialog( EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		subject,
					const char *		message,
					const char *		yesText,
					const char *		noText );

extern void * appMakePageTool(	EditApplication *	ea,
				APP_WIDGET		printOption,
				const char *		pixmapName );

extern void appShowPageTool(		EditApplication *	ea );

extern void appPageToolSetProperties(	void *				voidapt,
					const DocumentGeometry *	dg );

extern void appEnablePageTool(		void *		voidapt,
					int		enabled );

extern void appRunPrintDialog(		EditDocument *		ed,
					const DocumentGeometry * dgDocument,
					int			pageCount,
					int			firstSelected,
					int			lastSelected,
					APP_WIDGET		printOption,
					const char *		pixmapName );

extern int appRunMailDialog(		EditApplication *	ea,
					EditDocument *		ed,
					APP_WIDGET		mailOption,
					const char *		pixmapName,
					const unsigned char *	subject,
					void *			voiddoc );

extern int appPrintDocument(	int				printer,
				const PrintJob *		pj,
				const PrintGeometry *		pg );

extern int appFaxDocument(	EditDocument *			ed,
				const char *			faxNumber,
				const PrintGeometry *		pg );

extern void appDocPrintToFile(	APP_WIDGET			option,
				APP_WIDGET			panel,
				EditDocument *			ed,
				const PrintGeometry *		pg );

extern void appDocAbout(	APP_WIDGET	option,
				void *		voided,
				void *		call_data	 );

extern void appRunOpenChooser(	APP_WIDGET		option,
				APP_WIDGET		relative,
				int			openExtensionCount,
				AppFileExtension *	openExtensions,
				char *			defaultFilter,
				void *			through,
				APP_OPEN_DOCUMENT	openDocument,
				EditApplication *	ea );

extern APP_EVENT_HANDLER_H( appDocCopyPasteHandler, w, voided, event );
extern APP_EVENT_HANDLER_H( appAppGotPasteCall, w, voided, event );

extern int appDocOwnSelection(	EditDocument *			ed,
				const char *			selection,
				AppSelectionTargetType * 	targets,
				int				targetCount );

extern int appDocAskForPaste(	EditDocument *		ed,
				const char *		selection );

extern int appAppAskForPaste(	EditApplication *	ea,
				const char *		selection );

extern void appDocHorizontalScrollbarCallback(	APP_WIDGET	w,
						void *		voided,
						void *		voidscbs );

extern void appDocVerticalScrollbarCallback(	APP_WIDGET	w,
						void *		voided,
						void *		voidscbs );
extern void appMakeButtonInRow(	APP_WIDGET *		pButton,
				APP_WIDGET		buttonRow,
				const char *		text,
				APP_BUTTON_CALLBACK_T	callback,
				void *			through,
				int			position,
				int			showAsDefault );

extern void appMakeLabelInRow(		APP_WIDGET *		pLabel,
					APP_WIDGET		row,
					int			column,
					int			colspan,
					const char *		labelText );

extern void appMakeTextInRow(		APP_WIDGET *		pText,
					APP_WIDGET		row,
					int			column,
					int			colspan,
					int			textColumns,
					int			textEnabled );

extern void appMakeLabelAndTextRow(	APP_WIDGET *	pRow,
					APP_WIDGET *	pLabel,
					APP_WIDGET *	pText,
					APP_WIDGET	parent,
					const char *	labelText,
					int		textColumns,
					int		textEnabled );

extern APP_WIDGET appMakeToggleInRow(	APP_WIDGET		buttonRow,
					const char *		text,
					APP_TOGGLE_CALLBACK_T	callback,
					void *			through,
					int			position );

extern void appMakeToggleAndTextRow(	APP_WIDGET *		pRow,
					APP_WIDGET *		pToggle,
					APP_WIDGET *		pText,
					APP_WIDGET		parent,
					char *			labelText,
					APP_TOGGLE_CALLBACK_T	callback,
					void *			through,
					int			textColumns,
					int			textEnabled );

extern void appReportSaveFailure(	EditApplication *	ea,
					APP_WIDGET		option,
					APP_WIDGET		relative,
					const char *		filename );

extern void appMakeColumnFrameInColumn(	APP_WIDGET *	pFrame,
					APP_WIDGET *	pPaned,
					APP_WIDGET	parent,
					const char *	title );

extern void appMakeLabelInColumn(	APP_WIDGET *	pLabel,
					APP_WIDGET	row,
					const char *	labelText );

extern void appMakeTextInColumn(	APP_WIDGET *	pText,
					APP_WIDGET	column,
					int		textColumns,
					int		textEnabled );

extern void appMakeColumnToggle(	APP_WIDGET *		pToggle,
					APP_WIDGET		column,
					APP_TOGGLE_CALLBACK_T	callback,
					void *			through,
					const char *		labelText,
					int			set );

extern void appMakeColumnDrawing(	APP_WIDGET *		pButton,
					APP_WIDGET		column,
					APP_DRAW_BUTTON_CALLBACK exposeCallback,
					APP_BUTTON_CALLBACK_T	pushedCallback,
					void *			through,
					int			width );

extern void appMakeVerticalDialog(	AppDialog *		ad,
					APP_WIDGET *		pPaned,
					EditApplication *	ea,
					APP_CLOSE_CALLBACK_T	closeCallback,
					APP_DESTROY_CALLBACK_T	destroyCallback,
					void *			through,
					char *			widgetName );

extern char * 	appWidgetName(		char *			file,
					int			line );

extern int appGetLengthFromTextWidget(	APP_WIDGET	w,
					int *		pValue,
					int *		pChanged,
					int		defaultUnit,
					int		minValue,
					int		adaptToMin,
					int		maxValue,
					int		adaptToMax );

extern int appGetIntegerFromTextWidget(	APP_WIDGET	w,
					int *		pValue,
					int		minValue,
					int		adaptToMin,
					int		maxValue,
					int		adaptToMax );

extern int appGetDoubleFromTextWidget(	APP_WIDGET	w,
					double *	pValue,
					double		minValue,
					int		adaptToMin,
					double		maxValue,
					int		adaptToMax );

extern void appEnableText(		APP_WIDGET	text,
					int		enabled );

extern void appRefuseTextValue(		APP_WIDGET	w );

extern void appDrawPageDiagram(	APP_WIDGET			w,
				AppDrawingData *		add,
				double				widgetHighCm,
				const DocumentGeometry *	dg );

extern void appDrawNupDiagram(	APP_WIDGET			w,
				AppDrawingData *		add,
				APP_FONT *			xfs,
				double				widgetHighCm,
				const DocumentGeometry *	dgPage,
				const PrintGeometry *		pg );

extern int appCallPrintFunction( SimpleOutputStream *		sos,
				const PrintJob *		pj,
				const PrintGeometry *		pg );

extern void appStringToTextWidget(	APP_WIDGET		w,
					const char *		s );

extern void appIntegerToTextWidget(	APP_WIDGET		w,
					int			n );

extern void appDoubleToTextWidget(	APP_WIDGET		w,
					double			d );

extern int appImgMakeFileExtensions(	AppFileExtension **	pAfeList,
					int *			pAfeCount );

extern void appInspectorMakeButtonRow(	APP_WIDGET *		pRow,
					APP_WIDGET		parent,
					APP_WIDGET *		pLeftButton,
					APP_WIDGET *		pRightButton,
					const char *		leftLabel,
					const char *		rightLabel,
					APP_BUTTON_CALLBACK_T	leftCallback,
					APP_BUTTON_CALLBACK_T	rightCallback,
					void *			through );

extern void appInspectorMake3ButtonRow( APP_WIDGET *		pRow,
					APP_WIDGET		parent,
					APP_WIDGET *		pLeftButton,
					APP_WIDGET *		pMiddleButton,
					APP_WIDGET *		pRightButton,
					const char *		leftLabel,
					const char *		middleLabel,
					const char *		rightLabel,
					APP_BUTTON_CALLBACK_T	leftCallback,
					APP_BUTTON_CALLBACK_T	middleCallback,
					APP_BUTTON_CALLBACK_T	rightCallback,
					void *			through );

extern void appInspectorMakeToggleRow(	APP_WIDGET *		pRow,
					APP_WIDGET		parent,
					APP_WIDGET *		pLeftToggle,
					APP_WIDGET *		pRightToggle,
					const char *		leftText,
					const char *		rightText,
					APP_TOGGLE_CALLBACK_T	leftCallback,
					APP_TOGGLE_CALLBACK_T	rightCallback,
					void *			through );

extern AppInspector * 	appMakeInspector(
				    EditApplication *		ea,
				    APP_WIDGET			option,
				    const char *		pixmapName,
				    const char *		widgetName,
				    InspectorSubjectResources * isr,
				    int				subjectCount,
				    AppToolDestroy		closeInspector,
				    void *			through );

extern void appInspectorMakeMenuRow(	APP_WIDGET *		pRow,
					AppOptionmenu *		aom,
					APP_WIDGET *		pLabel,
					APP_WIDGET		parent,
					const char *		labelText );

extern void appFillInspectorMenu(	int			count,
					int			current,
					APP_WIDGET *		items,
					char * const *		texts,
					AppOptionmenu *		aom,
					APP_OITEM_CALLBACK_T	callBack,
					void *			target );

extern int appMakeDocumentWidget(	EditApplication *	ea,
					EditDocument *		ed );

extern APP_EVENT_HANDLER_H( appDocExposeHandler, w, d, exposeEvent );

extern int appFileConvert(	EditApplication *	ea,
				const char *		fromName,
				const char *		toName );

extern int appPrintToFile(	EditApplication *	ea,
				const char *		fromName,
				const char *		toName,
				const char *		paperString );

extern int appPrintToPrinter(	EditApplication *	ea,
				const char *		fromName,
				const char *		toName,
				const char *		paperString );

APP_DESTROY_CALLBACK_H( appDestroyEditDocument, w, voided );

extern APP_WIDGET appMakePageDrawing(	APP_WIDGET		parent,
					EditApplication *	ea,
					int			mmHigh,
					APP_EVENT_HANDLER_T	redraw,
					void *			through );

extern void appScrollToRectangle(	EditDocument *		ed,
					int			x0,
					int			y0,
					int			x1,
					int			y1,
					int *			pScrolledX,
					int *			pScrolledY );

extern void appMakeVerticalTool( APP_WIDGET *		pShell,
				APP_WIDGET *		pPaned,
				EditApplication *	ea,
				APP_BITMAP_IMAGE	iconPixmap,
				APP_BITMAP_MASK		iconMask,
				const char *		widgetName,
				int			userResizable,
				APP_WIDGET		option,
				APP_CLOSE_CALLBACK_T	closeCallback,
				void *			through );

extern APP_WIDGET appMakeRowInColumn(	APP_WIDGET	parent,
					int		columnCount,
					int		heightResizable );

extern void * appMakeFindTool(		APP_WIDGET		findOption,
					EditApplication *	ea,
					const char *		widgetName,
					APP_BITMAP_IMAGE	iconPixmap,
					APP_BITMAP_MASK		iconMask,
					AppToolDestroy		destroy,
					FindToolFind		findNext,
					FindToolFind		findPrev,
					FindToolReplace		replace,
					FindToolSetPattern	setPattern,
					void *			target );

extern void appFindToolSetPattern(	void *			voidaft,
					const unsigned char *	pattern,
					int			useRegex );

extern void appShowFindTool(		APP_WIDGET		reference,
					void *			voidaft );

extern void appEnableFindTool(		void *		voidaft,
					int		enabled );

extern void appFindToolEnableReplace(	void *		voidaft,
					int		enabled );

extern void appMakeColumnInRow(		APP_WIDGET *	pColumn,
					APP_WIDGET	row,
					int		position,
					int		colspan );

extern void appMakeRowFrameInColumn(	APP_WIDGET *	pFrame,
					APP_WIDGET *	pRow,
					APP_WIDGET	parent,
					int		columnCount,
					const char *	title );

extern char *	appGetStringFromTextWidget(	APP_WIDGET	w );
extern void	appFreeStringFromTextWidget(	char *		s );

extern char *	appGetTextFromMenuOption(	APP_WIDGET	w );
extern void	appFreeTextFromMenuOption(	char *		s );

extern void appGuiGetResourceValues(
				int *				pGotResources,
				EditApplication *		ea,
				void *				pValues,
				AppConfigurableResource *	acr,
				int				acrCount );

extern void appSetResourceDefaults(
				EditApplication *		ea,
				AppConfigurableResource *	acr,
				int				acrCount );

extern int appCountChangedDocuments(	EditApplication *	ea );

extern int appGuiInitApplication(	EditApplication *	ea,
					int *			pArgc,
					char ***		pArgv );

extern void appExitApplication(	EditApplication *	ea );

extern void appAskCloseDocuments(	APP_WIDGET		option,
					EditApplication *	ea );

extern void appGuiInsertColumnInWindow(	APP_WIDGET *	pColumn,
					APP_WIDGET	parent );

extern void appGuiInsertMenubarInColumn( APP_WIDGET *	pMenubar,
					APP_WIDGET	parent );

extern void appMakeImageInColumn(	APP_WIDGET *		pLabel,
					APP_WIDGET		column,
					APP_BITMAP_IMAGE	pixmap,
					APP_BITMAP_MASK		mask );

extern void appGuiSetToggleLabel(	APP_WIDGET		toggle,
					const char *		text );

extern void appGuiSetToggleState(	APP_WIDGET		toggle,
					int			set );

extern int appGuiGetToggleState(	APP_WIDGET		toggle );

extern void appSetOptionmenu(		AppOptionmenu *		aom,
					int			num );

extern void appGuiEnableWidget(		APP_WIDGET		w,
					int			on_off );

extern void appGuiSetWidgetVisible(	APP_WIDGET		w,
					int			on_off );

extern void appShowShellWidget(		APP_WIDGET		shell );
extern void appHideShellWidget(		APP_WIDGET		shell );
extern void appDestroyShellWidget(	APP_WIDGET		shell );
extern void appIconifyShellWidget(	APP_WIDGET		shell );

extern void appGuiInsertSeparatorInColumn(	APP_WIDGET *	pSeparator,
						APP_WIDGET	parent );

extern void appOptionmenuSetWidthMotif(	APP_WIDGET		w,
					int			newWidth );

extern void appOptionmenuRefreshWidth(	AppOptionmenu *		aom );

extern void appEmptyOptionmenu(		AppOptionmenu *		aom );
extern void appEmptyParentWidget(	APP_WIDGET		parent );

extern void appGuiEnableOptionmenu(	AppOptionmenu *		aom,
					int			sensitive );

extern APP_WIDGET appAddItemToOptionmenu( AppOptionmenu *	aom,
					const char *		label,
					APP_OITEM_CALLBACK_T	callBack,
					void *			target );

extern void appSetShellTitle(		APP_WIDGET		shell,
					APP_WIDGET		option,
					const char *		title );

extern void appGuiLowerShellWidget(	APP_WIDGET		shell );

/************************************************************************/
/*									*/
/*  Event handler to set minimum size.					*/
/*									*/
/************************************************************************/

extern APP_EVENT_HANDLER_H( appSetSizeAsMinimum, w, through, event );

/************************************************************************/
/*									*/
/*  Names of the X11 events.						*/
/*									*/
/************************************************************************/

extern char * APP_X11EventNames[];

extern void appGuiChangeLabelText(	APP_WIDGET	labelWidget,
					const char *	label );

extern void appGuiChangeButtonText(	APP_WIDGET	labelWidget,
					const char *	label );

extern void appInspectorMakePageParent( AppInspector *	ai );

extern void appMakeVerticalInspectorPage(	APP_WIDGET *	pPage,
						APP_WIDGET *	pMenuitem,
						AppInspector *	ai,
						const char *	label );

extern void appInspectorEnablePage(	AppInspector *	ai,
					int		pageNumber,
					int		enabled );

extern void appMakeOptionmenuInColumn(		AppOptionmenu *	aom,
						APP_WIDGET	parent );

extern void appMakeOptionmenuInRow(	AppOptionmenu *	aom,
					APP_WIDGET	row,
					int		column,
					int		colspan );

extern int appGuiGetMenuToggleStateFromCallback(
					APP_WIDGET		toggle,
					void *			voidcbs );

extern void appGuiMakeListInColumn(	APP_WIDGET *		pList,
					APP_WIDGET		column,
					int			visibleItems,
					APP_LIST_CALLBACK_T	callback,
					void *			through );

extern void appGuiSetTypingCallbackForText(
					APP_WIDGET		text,
					APP_TXTYPING_CALLBACK_T	callBack,
					void *			through );

extern void appGuiSetGotValueCallbackForText(
				    APP_WIDGET			text,
				    APP_TXACTIVATE_CALLBACK_T	callBack,
				    void *			through );

extern void appGuiGetScrollbarValues(	int *		pValue,
					int *		pSliderSize,
					APP_WIDGET	scrollbar );

extern void appGuiSetScrollbarValues(	APP_WIDGET	scrollbar,
					int		value,
					int		sliderSize );

extern void appInitEditDocument(	EditApplication *	ea,
					EditDocument *		ed );

extern void appDocumentRulerWidth(	EditApplication *	ea,
					EditDocument *		ed );

extern int appFinishDocumentWindow(	EditDocument *		ed );

extern void appAdaptToDocumentSize(	EditDocument *	ed,
					int		width,
					int		height );

extern int appGuiGetScrollbarValueFromCallback( APP_WIDGET	scrollbar,
					void *			voidcbs );

extern void appGuiSetShellTitle(	APP_WIDGET		shell,
					const char *		title );

extern void appGuiSetIconTitle(		APP_WIDGET		shell,
					const char *		title );

extern void appGuiEmptyListWidget(	APP_WIDGET		list );

extern void appGuiAddValueToListWidget(	APP_WIDGET		list,
					const char *		value );

extern void appGuiSelectPositionInListWidget(	APP_WIDGET	list,
						int		position );

extern void appGuiRemoveSelectionFromListWidget(	APP_WIDGET	list );

extern APP_WIDGET appSetMenuItem(	APP_WIDGET		menu,
					AppToplevel *		at,
					AppMenuItem *		ami,
					void *			target );

extern APP_WIDGET appSetMenuSeparator(	APP_WIDGET		menu,
					AppToplevel *		at,
					AppMenuItem *		ami,
					void *			target );

extern APP_WIDGET appSetToggleMenuItem(	APP_WIDGET		menu,
					AppToplevel *		at,
					AppMenuItem *		ami,
					void *			target );

extern APP_WIDGET appMakeMenuInParent(	APP_WIDGET *		pButton,
					AppToplevel *		at,
					APP_WIDGET		menuBar,
					const char *		itemText,
					int			isHelp );

extern char * appGuiGetStringFromListCallback(
					    APP_LIST		list,
					    APP_LIST_CHOICE *	voidlcs );

extern void appFreeStringFromListCallback(	char *		s );

extern void appGuiRunDialog( 		AppDialog *		ad,
					int			initial,
					EditApplication *	ea );

extern void appGuiBreakDialog( 		AppDialog *		ad,
					int			response );

extern void appGuiSetDefaultButtonForDialog(	AppDialog *	ad,
						APP_WIDGET	button );

extern void appGuiSetCancelButtonForDialog(	AppDialog *	ad,
						APP_WIDGET	button );

extern void appGuiShowDialog(			AppDialog *	ad,
						APP_WIDGET	relative );

extern void appGuiHideDialog(			AppDialog *	ad );

extern void appMotifTurnOfSashTraversal(	APP_WIDGET	column );

extern void appGuiMakeDrawingAreaInColumn( APP_WIDGET *		pDrawing,
				APP_WIDGET			column,
				int				wide,
				int				high,
				int				heightResizable,
				APP_EVENT_HANDLER_T		redraw,
				void *				through );

void appGuiGetStringFromKeyboardEvent(	APP_INPUT_CONTEXT	ic,
					APP_WIDGET		w,
					APP_EVENT *		event,
					int *			pGotString,
					int *			pGotKey,
					unsigned int *		pState,
					unsigned char *		buf,
					int			capacity,
					APP_KEY_VALUE *		pKey );

extern void appDocFillMenu(		EditDocument *		ed );

void appInspectorChoosePage(	AppInspector *		ai,
				int			andMenu,
				int			pageNumber );

extern void appInitOptionmenu(		AppOptionmenu *		aom );

extern void appRunDragLoop(	APP_WIDGET		w,
				EditApplication *	ea,
				APP_EVENT *		downEvent,
				APP_EVENT_HANDLER_T	upHandler,
				APP_EVENT_HANDLER_T	moveHandler,
				int			timerInterval,
				APP_TIMER_CALLBACK	timerHandler,
				void *			through );

extern void appGuiSetFocusChangeHandler( APP_WIDGET		shell,
					APP_EVENT_HANDLER_T	handler,
					void *			through );

extern void appGuiSetToggleItemState(	APP_WIDGET		toggle,
					int			set );

extern void appSetDestroyCallback(	APP_WIDGET		shell,
					APP_DESTROY_CALLBACK_T	destroyCallback,
					void *			through );

extern void appSetCloseCallback(	APP_WIDGET		shell,
					EditApplication *	ea,
					APP_CLOSE_CALLBACK_T	closeCallback,
					void *			through );

extern void appGuiSetToggleItemLabel(	APP_WIDGET		toggle,
					const char *		label );

extern int appGuiGtkGetChildLabel(	char **			pLabel,
					APP_WIDGET		w );

extern int appGuiGtkSetChildLabel(	APP_WIDGET		w,
					const char *		s );

extern void appGuiGetResourceValuesGtkX(
				EditApplication *		ea,
				void *				pValues,
				AppConfigurableResource *	acr,
				int				acrCount );

extern int appFormatDocumentTitle(	const char **		pWindowTitle,
					const char **		pIconTitle,
					EditApplication *	ea,
					const char *		title );

extern APP_EVENT_HANDLER_H( appDocConfigure, w, voided, event );

extern void appDocumentCalculateExtraSize(	EditDocument *	ed );

extern void appSetWindowsItemState(	APP_WIDGET	menu,
					EditDocument *	ed,
					int		changed );

extern void appRemoveWindowsOption(	APP_WIDGET		menu,
					EditDocument *		oldEd );

extern void appRenameWindowsOption(	APP_WIDGET		menu,
					EditDocument *		ed,
					const char *		title );

extern void appAllocateCopyPasteTargetAtoms(	EditApplication *	ea );

extern APP_GIVE_COPY( appDocReplyToCopyRequest, w, gsd, voided );

extern void appCopyPixmapValue(	APP_SELECTION_EVENT *	event,
				APP_BITMAP_IMAGE	pixmapCopied );

extern void appGetApplicationResourceValues(	EditApplication *	ea );

extern void appDocExposeRectangle(
				const EditDocument *		ed,
				const DocumentRectangle *	drChanged,
				int				scrolledX,
				int				scrolledY );

extern void appPrintJobForEditDocument(	PrintJob *		pj,
					EditDocument *		ed );

extern void appApplicationSettingsToPrintGeometry(
					PrintGeometry *		pg,
					EditApplication *	ea );

extern int appRunPrintToFileChooser(	APP_WIDGET		option,
					APP_WIDGET		panel,
					EditApplication *	ea,
					const EditDocument *	ed,
					char **			pFilename );

extern int appDocumentTestCanSave(
				EditApplication *		ea,
				const AppFileExtension *	afe,
				void *				privateData,
				unsigned int			useFlags,
				int				format );

extern int appDocumentGetSaveFormat(
				EditApplication *		ea,
				const char *			filename,
				void *				privateData,
				unsigned int			useFlags,
				int				format );

extern void appMakeDrawnPulldownInColumn(
				AppDrawnPulldown *		adp,
				APP_EVENT_HANDLER_T		redrawInplace,
				APP_EVENT_HANDLER_T		redrawPulldown,
				APP_EVENT_HANDLER_T		clickHandler,
				APP_WIDGET			column,
				void *				through );

extern void appMakeDrawnPulldownInRow(
				AppDrawnPulldown *		adp,
				APP_EVENT_HANDLER_T		redrawInplace,
				APP_EVENT_HANDLER_T		redrawPulldown,
				APP_EVENT_HANDLER_T		clickHandler,
				APP_WIDGET			row,
				int				column,
				int				colspan,
				void *				through );

extern void appGuiEnableDrawnPulldown(	AppDrawnPulldown *	adp,
					int			sensitive );

extern void appInitDrawnPulldown(	AppDrawnPulldown *	adp );

extern void appGuiSetDrawnPulldownHeight(
				    AppDrawnPulldown *	adp,
				    int			height );

extern void appGuiSetDrawnPulldownStrips(
				    AppDrawnPulldown *	adp,
				    int			strips );

extern int appGuiDrawnPulldownGetStrip(
				int *				pStrip,
				const AppDrawnPulldown *	adp,
				const APP_WIDGET		w,
				const APP_EVENT *		mouseEvent );

extern void appDrawnPulldownDrawArrow(	int *			pRestWide,
					int *			pRestHigh,
					APP_WIDGET		w,
					AppDrawingData *	add );

extern APP_FONT * appGuiGetLabelFont(	APP_WIDGET	w );

extern int appGuiGetPositionFromListCallback(	APP_LIST	list,
						void *		voidlcs );

extern void appOptionmenuItemSetVisibility(	APP_WIDGET	w,
						int		visible );

extern int appPostScriptFontCatalog(		EditApplication *	ea );

extern int appGetPrintDestinations(		EditApplication *	ea );

extern int appReadSystemProperties(	EditApplication *	ea );
extern int appReadUserProperties(	EditApplication *	ea );
extern int appSetUserProperty(		EditApplication *	ea,
					const char *		name,
					const char *		value );

extern int appSetSystemProperty(	EditApplication *	ea,
					const char *		name,
					const char *		value );

extern int appGuiSetFrameTitle(		APP_WIDGET		w,
					const unsigned char *	title );

#   endif
