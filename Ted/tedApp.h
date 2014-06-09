#   include	<appFrame.h>
#   include	<appImage.h>

#   include	<utilPropMask.h>
#   include	<utilMemoryBuffer.h>
#   include	<docExpandedTextAttribute.h>

#   include	<geo2DInteger.h>
#   include	<appDraw.h>
#   include	"docBuf.h"
#   include	"docScreenFontList.h"

#   ifndef	TED_APP_H
#   define	TED_APP_H

#   define	RESIZE_BLOCK	10

#   define	RESIZE_BOTTOM_LEFT	0
#   define	RESIZE_BOTTOM_MIDDLE	1
#   define	RESIZE_BOTTOM_RIGHT	2

#   define	RESIZE_MIDDLE_LEFT	3
#   define	RESIZE_MIDDLE_RIGHT	4

#   define	RESIZE_TOP_LEFT		5
#   define	RESIZE_TOP_MIDDLE	6
#   define	RESIZE_TOP_RIGHT	7

#   define	RESIZE_COUNT		8

/************************************************************************/
/*									*/
/*  Private data for a document.					*/
/*									*/
/************************************************************************/

typedef struct TedDocument
    {
    BufferDocument *		tdDocument;
    TextAttribute		tdCurrentTextAttribute;
    int				tdCurrentTextAttributeNumber;
    int				tdCurrentScreenFont;

    DocumentSelection		tdDocumentSelection;
    SelectionGeometry		tdSelectionGeometry;
    SelectionDescription	tdSelectionDescription;

    ScreenFontList		tdScreenFontList;

    APP_WIDGET			tdCopyWidget;
    APP_WIDGET			tdCutWidget;
    APP_WIDGET			tdPasteWidget;

    APP_WIDGET			tdInsertMenu;
    APP_WIDGET			tdInsertMenuButton;
    APP_WIDGET			tdInsPictOption;
    APP_WIDGET			tdInsFileOption;
    APP_WIDGET			tdInsSymbolOption;
    APP_WIDGET			tdInsHyperlinkOption;
    APP_WIDGET			tdInsBookmarkOption;
    APP_WIDGET			tdInsInsertFootnoteOption;
    APP_WIDGET			tdInsInsertEndnoteOption;
    APP_WIDGET			tdInsInsertChftnsepOption;
    APP_WIDGET			tdInsInsertTableOption;
    APP_WIDGET			tdInsInsertPageNumberOption;
    APP_WIDGET			tdInsInsertLineBreakOption;
    APP_WIDGET			tdInsInsertPageBreakOption;
    APP_WIDGET			tdInsInsertSectBreakOption;

    APP_WIDGET			tdTableMenu;
    APP_WIDGET			tdTableMenuButton;
    APP_WIDGET			tdTabInsertTableOption;
    APP_WIDGET			tdTabAddRowOption;
    APP_WIDGET			tdTabAddColumnOption;

    APP_WIDGET			tdFormatMenu;
    APP_WIDGET			tdFormatMenuButton;
    APP_WIDGET			tdFormatOneParaOption;

    APP_WIDGET			tdSelectTableWidget;
    APP_WIDGET			tdSelectRowWidget;
    APP_WIDGET			tdSelectColumnOption;

    APP_WIDGET			tdDeleteTableWidget;
    APP_WIDGET			tdDeleteRowWidget;
    APP_WIDGET			tdDeleteColumnOption;

    APP_WIDGET			tdDrawTableGridOption;

    APP_WIDGET			tdFontMenu;
    APP_WIDGET			tdFontMenuButton;
    APP_WIDGET			tdFontBoldOption;
    APP_WIDGET			tdFontItalicOption;
    APP_WIDGET			tdFontUnderlinedOption;
    APP_WIDGET			tdFontSuperscriptOption;
    APP_WIDGET			tdFontSubscriptOption;

    APP_WIDGET			tdToolsMenu;
    APP_WIDGET			tdToolsMenuButton;
    APP_WIDGET			tdToolsFormatToolOption;

    APP_COLOR_RGB		tdFieldColor;

    RGB8Color			tdTableColor;
    RGB8Color			tdSelColor;
    RGB8Color			tdCopiedSelColor;

    int				tdVisibleSelectionCopied;
    int				tdCanReplaceSelection;
    int				tdCopiedSelectionClosed;
    MemoryBuffer		tdCopiedSelection;
    MemoryBuffer		tdCopiedFont;
    MemoryBuffer		tdCopiedRuler;
    AppBitmapImage		tdCopiedImage;

    void *			tdFindProg;

#   ifdef USE_MOTIF
    XtIntervalId		tdHideIBarId;
    XtIntervalId		tdShowIBarId;
#   endif

#   ifdef USE_GTK
    guint			tdHideIBarId;
    guint			tdShowIBarId;
#   endif

    APP_WINDOW			tdObjectWindow;
    APP_WINDOW			tdObjectResizeWindows[RESIZE_COUNT];
    int				tdObjectCornerMovedX;
    int				tdObjectCornerMovedY;
    int				tdScaleChangedX;
    int				tdScaleChangedY;
    int				tdObjectResizeCorner;

    int				tdDrawTableGrid;
    } TedDocument;

/************************************************************************/
/*									*/
/*  Ted specific resources.						*/
/*									*/
/*  Also: Some global data items.					*/
/*									*/
/************************************************************************/

typedef struct TedAppResources
    {
				/****************************************/
				/*  Texts.				*/
				/****************************************/

    char *			tarFileNotRtf;
    char *			tarHeadFootNotReachable;

    char *			tarAppFileMenuText;
    char *			tarAppWinMenuText;
    char *			tarDocFileMenuText;
    char *			tarDocEditMenuText;
    char *			tarDocInsertMenuText;
    char *			tarDocWinMenuText;
    char *			tarDocFontMenuText;
    char *			tarDocFormatMenuText;
    char *			tarDocToolMenuText;
    char *			tarDocTableMenuText;
    char *			tarHelpMenuText;
    char *			tarAppHelpFileName;

    char *			tarSelectionColor;
    char *			tarCopiedSelectionColor;

				/****************************************/
				/*  Defaults.				*/
				/****************************************/
    char *			tarDefaultAnsicpgString;
    int				tarDefaultAnsicpgInt;

    char *			tarShowTableGridString;
    int				tarShowTableGridInt;

				/****************************************/
				/*  Global data.			*/
				/****************************************/

    AppInspector *		tarInspector;
    EditDocument *		tarManualDocument;

				/****************************************/
				/*  For Ted --Find and Ted --RegFind	*/
				/****************************************/

    const unsigned char *	tarFindPattern;
    int				tarFindRegex;
    } TedAppResources;

/************************************************************************/
/*									*/
/*  Used on 'Hyperlink' and 'Bookmark' dialogs.				*/
/*									*/
/************************************************************************/

typedef enum LinkKind
    {
    LINKkindHYPERLINK,
    LINKkindPAGE_NUMBER,
    LINKkindBOOKMARK_TEXT,
    LINKkindTEXT_PLUS_PAGE,

    LINKkind_COUNT
    } LinkKind;

/************************************************************************/
/*									*/
/*  Supported document formats.						*/
/*  Match an array in tedMain.c.					*/
/*									*/
/************************************************************************/

typedef enum TedDocumentKind
    {
    TEDdockindRTF= 0,
    TEDdockindTEXT_OPEN,
    TEDdockindTEXT_SAVE_FOLDED,
    TEDdockindTEXT_SAVE_WIDE,
    TEDdockindHTML_FILES,
    TEDdockindALL_FILES,

    TEDdockind_COUNT
    } TedDocumentKind;

/************************************************************************/
/*									*/
/*  Shared resources.							*/
/*									*/
/************************************************************************/

extern const int TedPrimaryTextTargetCount;
extern AppSelectionTargetType TedPrimaryTextTargets[];

extern const int TedPrimaryPictureTargetCount;
extern AppSelectionTargetType TedPrimaryPictureTargets[];

extern const int TedRulerTargetCount;
extern AppSelectionTargetType TedRulerTargets[];

/************************************************************************/
/*									*/
/*  Geometry calculations.						*/
/*									*/
/************************************************************************/

extern int tedLayoutItem(	BufferItem *			bi,
				BufferDocument *		bd,
				AppDrawingData *		add,
				ScreenFontList *		sfl,
				DocumentRectangle *		drChanged );

extern int tedFindPosition(	DocumentPosition *		dp,
				PositionGeometry *		pg,
				int *				pPart,
				const BufferDocument *		bd,
				BufferItem *			rootBi,
				const AppDrawingData *		add,
				const ScreenFontList *		sfl,
				int				x,
				int				y );

extern int tedFindParticule(	TextLine *			tl,
				TextParticule *			tp,
				int				x,
				int				y );

extern int tedFindStringOffset(	const BufferDocument *		bd,
				const BufferItem *		paraBi,
				int				part,
				const AppDrawingData *		add,
				const ScreenFontList *		sfl,
				int *				pBarX,
				int				x,
				int				y );

extern int tedHasSelection(	const TedDocument *		td );
extern int tedHasIBarSelection(	const TedDocument *		td );

extern int tedArrowDown(	DocumentPosition *		bp,
				const PositionGeometry *	pg,
				const BufferDocument *		bd,
				const AppDrawingData *		add,
				const ScreenFontList *		sfl );

extern int tedArrowUp(		DocumentPosition *		bp,
				const PositionGeometry *	pg,
				const BufferDocument *		bd,
				const AppDrawingData *		add,
				const ScreenFontList *		sfl );

extern APP_EVENT_HANDLER_H( tedObserveFocus, w, voided, event );

extern void tedSplitParagraph(		EditDocument *		ed,
					int			onNewPage );

extern void tedEditReplaceSelectionWithTab(	EditDocument *		ed );

extern int tedChangeSelectionProperties(
				EditDocument *			ed,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet,
				const PropertyMask *		ppUpdMask,
				const ParagraphProperties *	ppNew,
				const PropertyMask *		spUpdMask,
				const SectionProperties *	spNew );

extern void tedRedrawRectangle(	APP_WIDGET		w,
				TedDocument *		td,
				const DocumentRectangle *	drClip,
				AppDrawingData *	add,
				AppColors *		ac,
				int			ox,
				int			oy );

extern void tedDelimitCurrentSelection(	TedDocument *		td,
					AppDrawingData *	add );

extern void tedDocToolFind(		APP_WIDGET	findOption,
					void *		voided,
					void *		voidcbs );

extern void tedDocToolFindNext(		APP_WIDGET	findOption,
					void *		voided,
					void *		voidcbs );

extern void tedDocToolSpell(		APP_WIDGET	spellOption,
					void *		voided,
					void *		voidcbs );

extern int tedIncludePlainDocument(	EditDocument *		ed,
					BufferDocument *	bdFrom );

extern int tedIncludeRtfDocument(	EditDocument *		ed,
					BufferDocument *	bdFrom );

extern int tedGetDocumentAttributeString(	char *		scratch,
						TedDocument *	td );

extern int tedGetDocumentAttributes(
				TedDocument *			td,
				PropertyMask *			pUpdMask,
				TextAttribute *			pTaNew );

extern void tedDocCopy(		EditDocument *	ed );
extern void tedDocCut(		EditDocument *	ed );

extern void tedDocSelAll(	EditDocument *	ed );

extern void tedSetSelection(	EditDocument *			ed,
				const DocumentSelection *	dsSet,
				int				lastLine,
				int *				pScrolledX,
				int *				pScrolledY );

extern void tedDocEditPaste(	APP_WIDGET			option,
				void *				voided,
				void *				voidpbcs );

extern void tedDocReplaceSelection(	EditDocument *		ed,
					const unsigned char *	word,
					int			len );

extern int tedDeleteTableSliceSelection(	EditDocument *	ed );

extern void tedSetSelectedPosition(
				EditDocument *			ed,
				const DocumentPosition *	dp,
				int				lastLine,
				int *				pScrolledX,
				int *				pScrolledY );

extern int tedSelectItemHome(	EditDocument *			ed,
				BufferItem *			bi,
				int *				pScrolledX,
				int *				pScrolledY );

extern void tedAdaptFontIndicatorsToSelection(	EditApplication *	ea,
						EditDocument *		ed );

extern void tedAdaptToolsToSelection(	EditDocument *		ed );

extern int tedExtendSelectionToXY(
				EditDocument *			ed,
				BufferItem *			rootBi,
				const DocumentPosition *	bpAnchor,
				int				mouseX,
				int				mouseY );

extern int tedExtendSelectionToPosition(
				EditDocument *			ed,
				const DocumentPosition *	dpAnchor,
				const DocumentPosition *	dpFound );

extern void tedScrollToSelection(	EditDocument *		ed,
					int *			pScrolledX,
					int *			pScrolledY );

extern void tedDocFontBold(		APP_WIDGET	boldOption,
					void *		voided,
					void *		voidcbs	 );

extern void tedDocFontItalic(	APP_WIDGET	italicOption,
					void *		voided,
					void *		voidcbs	 );

extern void tedDocFontUnderlined(	APP_WIDGET	underlineOption,
					void *		voided,
					void *		voidcbs	 );

extern void tedDocFontSupersub(		APP_WIDGET	option,
					void *		voided,
					void *		voidcbs	 );

extern int tedLayoutDocumentTree(	TedDocument *		td,
					AppDrawingData *	add );

extern void tedScreenRectangles(	AppDrawingData *		add,
					const DocumentGeometry *	dg );

extern void tedAdaptPageToolToDocument(	EditApplication *	ea,
					EditDocument *		ed );

extern void tedAppReplace(	void *			voidea,
				const unsigned char *	word	);

extern void tedGetNamedPictures(	EditApplication *	ea );

extern int tedAppChangeSelectionAttributeString( EditDocument *	ed,
					char *		attributeString );

extern int tedDocSetTopRuler(	EditDocument *	ed );
extern int tedDocSetLeftRuler(	EditDocument *	ed );

extern void tedDocAdaptHorizontalRuler(	EditDocument *		ed,
					BufferItem *		bi );

extern void tedUndrawIBar(	const EditDocument *	ed );

extern int tedDrawIBar(		const PositionGeometry *	pg,
				int				ox,
				int				oy,
				AppDrawingData *		add );

extern void tedStartCursorBlink(	EditDocument *	ed );
extern void tedStopCursorBlink(		EditDocument *	ed );
extern void tedCleanCursorBlink(	TedDocument *	td );

extern int tedOpenItemObjects(	BufferItem *		bi,
				BufferDocument *	bd,
				AppColors *		ac,
				AppDrawingData *	add );

extern int tedDrawObject(	const BufferItem *	bi,
				const TextParticule *	tp,
				int			baseline,
				int			ox,
				int			oy,
				AppDrawingData *	add );

extern void tedCloseObject(		BufferDocument *	bd,
					BufferItem *		bi,
					TextParticule *		tp,
					void *			voidadd );

extern void tedDocInsertPicture(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern void tedDocInsertFile(		APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern void tedDocInsertLink(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs );

extern void tedDocInsertBookmark(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern void tedDocInsertFootnote(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern void tedDocInsertEndnote(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern void tedDocInsertChftnsep(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern void tedDocInsertPageNumber(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern void tedDocInsertLineBreak(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern void tedDocInsertPageBreak(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern void tedDocInsertSectBreak(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern void TEDmiDocInsertInsSectBreak(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern void tedDocFormatOnePara(	APP_WIDGET	option,
					void *		voided,
					void *		voidpbcs );

extern int tedReplaceSelectionWithObject(
					EditDocument *		ed,
					InsertedObject *	io );

extern APP_PASTE_REPLY( tedPastePrimaryRtf, w, event, voided );
extern APP_PASTE_REPLY( tedPastePrimaryString, w, event, voided );
extern APP_PASTE_REPLY( tedPasteRulerTed, w, event, voided );
extern APP_PASTE_REPLY( tedPasteFontTed, w, event, voided );
extern APP_PASTE_REPLY( tedPastePrimaryPixmap, w, event, voided );
extern APP_PASTE_REPLY( tedPastePrimaryPng, w, event, voided );

extern APP_GIVE_COPY( tedCopyPrimaryRtf, w, event, voided );
extern APP_GIVE_COPY( tedCopyPrimaryString, w, event, voided );
extern APP_GIVE_COPY( tedCopyPrimaryPng, w, event, voided );
extern APP_GIVE_COPY( tedCopyPrimaryPixmap, w, event, voided );
extern APP_GIVE_COPY( tedCopyFontTed, w, event, voided );
extern APP_GIVE_COPY( tedCopyRulerTed, w, event, voided );

extern int tedCopyAsLink(	EditDocument *			ed,
				const DocumentSelection *	ds,
				int				asRef,
				int				asPageref,
				const char *			file,
				const char *			markName,
				int				markSize );

extern void tedPrimaryLost(	APP_WIDGET			w,
				void *				voided,
				APP_EVENT *			clrEvent );

extern InsertedObject * tedObjectMakeBitmapObject(
				    	EditDocument *		ed,
					BufferItem *		bi,
					AppBitmapImage *	abi );

extern int tedGetObjectSelection(	TedDocument *   	td,
					int *			pPart,
					DocumentPosition *	dpObject,
					InsertedObject **	pIo );

extern void tedSetObjectWindows(EditDocument *			ed,
				const PositionGeometry *	pg,
				const InsertedObject *		io,
				int				ox,
				int				oy );

extern void tedMoveObjectWindows(	EditDocument *		ed );

extern int tedEditReopenObject(		EditDocument *			ed,
					int				part,
					const DocumentPosition *	dpObj,
					const PositionGeometry *	pgObj );

extern int tedReopenObject(	BufferDocument *	bd,
				BufferItem *		bi,
				TextParticule *		tp,
				AppColors *		ac,
				AppDrawingData *	add );

extern int tedSaveObjectPicture(	AppBitmapImage *	abiTo,
					InsertedObject *	io );

extern void tedExposeSelection(	const EditDocument *		ed,
				const DocumentSelection *	ds,
				int				scrolledX,
				int				scrolledY );

extern void tedPositionGeometry(	PositionGeometry *		pg,
					const DocumentPosition *	dp,
					int				lastOne,
					const BufferDocument *		bd,
					const AppDrawingData *		add,
					const ScreenFontList *		sfl );

extern void tedSelectionGeometry(
			SelectionGeometry *		sg,
			const DocumentSelection *	ds,
			int				lastLine,
			const BufferDocument *		bd,
			const AppDrawingData *		add,
			const ScreenFontList *		sfl );

extern void tedDrawRectangle(	APP_WIDGET		w,
				EditDocument *		ed,
				DocumentRectangle *	drClip,
				int			ox,
				int			oy );

extern void tedDocHorizontalScrollbarCallback(	APP_WIDGET	w,
						void *		voided,
						void *		voidscbs );

extern void tedDocVerticalScrollbarCallback(	APP_WIDGET	w,
						void *		voided,
						void *		voidscbs );

extern int tedFinishDocumentSetup(	EditDocument *		ed );
extern void tedDocumentFirstVisible(	EditDocument *		ed );

extern APP_MENU_CALLBACK_H( tedDocFormatCopyRul, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFormatPasteRul, w, voided, event );

extern APP_MENU_CALLBACK_H( tedDocFormatIncreaseIndent, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFormatDecreaseIndent, w, voided, event );

extern int tedLayoutDocument(	void *				privateData,
				int				format,
				AppDrawingData *		add,
				const DocumentGeometry *	defDg );

extern int tedMakeDocumentWidget(	EditApplication *	ea,
					EditDocument *		ed );

extern int tedInsertTable(		EditDocument *		ed,
					int			rows,
					int			columns );

extern int tedInsertColumnInTable(	EditDocument *		ed );
extern int tedAppendColumnToTable(	EditDocument *		ed );
extern int tedInsertRowInTable(		EditDocument *		ed );
extern int tedAppendRowToTable(		EditDocument *		ed );

extern APP_MENU_CALLBACK_H( tedDocTableSelectTable, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocTableSelectRow, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocTableSelectColumn, w, voided, event );

extern APP_MENU_CALLBACK_H( tedDocTableDeleteTable, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocTableDeleteRow, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocTableDeleteColumn, w, voided, event );

extern void tedDocToolInsertSymbol(	APP_WIDGET	symbolOption,
					void *		voided,
					void *		voidcbs );

extern void tedDocSetTableProperties(	EditDocument *		ed,
					const TableRectangle *	tr,
					const PropertyMask *	rpSetMask,
					const PropertyMask *	cpSetMask,
					const RowProperties *	rp );

extern void tedAppSetTableProperties(	EditApplication *	ea,
					const TableRectangle *	tr,
					const PropertyMask *	rpSetMask,
					const PropertyMask *	cpSetMask,
					const RowProperties *	rp );

extern void tedAppSetImageProperties(
				EditApplication *		ea,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pip );

extern void tedAdaptFormatToolToDocument(	EditDocument *	ed,
						int		choosePage );

extern void tedAppSetTableSelection(	EditDocument *		ed,
					const TableRectangle *	tr );

extern void tedChangeTableLayout(	EditDocument *		ed,
					BufferItem *		sectBi,
					int			row0Change,
					int			row1Change,
					int			col0Change,
					int			col1Change,
					int			row1Reformat,
					const PropertyMask *	rpSetMask,
					const PropertyMask *	cpSetMask,
					const RowProperties *	rp );

extern void tedDocTableDrawGrid(	APP_WIDGET	option,
					void *		voided,
					void *		voidcbs );

extern void tedMergeParagraphsInSelection(	EditDocument *	ed );

extern int tedDeleteRowsFromTable(	EditDocument *		ed,
					int			delRow0,
					int			delRow1 );

extern int tedDeleteColumnsFromRows(	EditDocument *	ed,
					int		delRow0,
					int		delRow1,
					int		delCol0,
					int		delCol1 );

extern int tedRunPropertyDialog( EditDocument *			ed,
				const DocumentStatistics *	ds,
				APP_WIDGET			option,
				const char *			pixmapName );

extern int tedDocSetHyperlink(	EditDocument *		ed,
				const char *		file,
				const char *		mark,
				int			asRef,
				int			asPageref );

extern int tedAppSetHyperlink(	EditApplication *	ea,
				const char *		file,
				const char *		mark,
				int			asRef,
				int			asPageref );

extern int tedSetBookmark(	EditDocument *		ed,
				const char *		mark );

extern int tedDocRemoveHyperlink(	EditDocument *		ed );
extern int tedAppRemoveHyperlink(	EditApplication *	ea );

extern int tedRemoveBookmark(	EditDocument *		ed );

extern int tedCopyBookmarkAsLink(	EditDocument *	ed,
					int		asRef,
					int		asPageref,
					const char *	link );

extern void tedRunBookmarkDialog(	EditApplication *	ea,
					EditDocument *		ed,
					APP_WIDGET		option,
					int			nameProposed,
					const char *		markName,
					int			markSize );

extern int tedGoToBookmark(	EditDocument *		ed,
				const char *		markName,
				int			markSize );

extern int tedSaveDocument(	const void *			privateData,
				int				format,
				const char *			applicationId,
				const char *			documentTitle,
				const char *			filename );

extern void * tedMakePrivateData( void );

extern void tedMakeDocumentReadonly(	EditDocument *	ed );

extern int tedDocFollowLink(	APP_WIDGET		option,
				EditDocument *		edFrom,
				const char *		fileName,
				int			fileSize,
				const char *		markName,
				int			markSize );

extern int tedAppFollowLink(	APP_WIDGET		option,
				EditApplication *	ea,
				const char *		fileName,
				int			fileSize,
				const char *		markName,
				int			markSize );

extern int tedSelectWholeParagraph(	EditApplication *	ea,
					int			direction );

extern int tedSelectWholeSection(	EditApplication *	ea,
					int			direction );

extern int tedDeleteCurrentParagraph(	EditApplication *	ea );
extern int tedDeleteCurrentSection(	EditApplication *	ea );

extern int tedInsertParagraph(	EditApplication *	ea,
				int			after );

extern int tedInsertSection(	EditApplication *	ea,
				int			after );

extern int tedGetParaLineHeight(	int *			pLineHeight,
					EditDocument *		ed );

extern void tedShowFormatTool(	APP_WIDGET		toolOption,
				EditApplication *	ea );

extern void tedFormatToolAdaptToSelection( AppInspector *	ai,
				EditDocument *			ed,
				int				choosePage,
				const DocumentSelection *	ds,
				const SelectionGeometry *	sg,
				const SelectionDescription *	sd );

extern int tedNewDocument(	EditApplication *	ea,
				EditDocument *		ed,
				const char *		filename );

extern int tedPrintDocument(	SimpleOutputStream *		sos,
				const PrintJob *		pj,
				const PrintGeometry *		pg );

extern void tedSuggestNup(	PrintGeometry *			pg,
				void *				privateData );

extern void tedFreeDocument(		void *			voidtd,
					int			format,
					AppDrawingData *	add );

extern void tedRedoDocumentLayout(	EditDocument *		ed );

extern void tedFormatShowPagePage( EditApplication *		ea );
extern void tedFormatShowFontPage( EditApplication *		ea );
extern void tedFormatShowLinkPage( EditApplication *		ea );

extern void tedSetPageLayout(	EditApplication *		ea,
				const DocumentGeometry *	dg,
				const PropertyMask *		setMask,
				int				whileDocument );

extern int tedAppSetDocumentProperties(
				EditApplication *		ea,
				const DocumentProperties *	dpNew,
				const PropertyMask *		updMask );

extern int tedChangeCurrentNote( EditApplication *		ea,
				int				autoNumber,
				const unsigned char *		fixedText,
				int				extItKind );

extern APP_EVENT_HANDLER_H( tedKeyPressed, w, voided, keyEvent );
extern APP_EVENT_HANDLER_H( tedMouseButtonPressed, w, voided, downEvent );

extern void tedHideObjectWindows(	EditDocument *	ed );

extern int tedAppChangeSectionProperties(
				EditApplication *		ea,
				const PropertyMask *		spUpdMask,
				const SectionProperties *	spNew );

extern int tedAppChangeAllSectionProperties(
				EditApplication *		ea,
				const PropertyMask *		spUpdMask,
				const SectionProperties *	spNew );

extern int tedAppChangeParagraphProperties(
				EditApplication *		ea,
				const PropertyMask *		ppUpdMask,
				const ParagraphProperties *	ppNew );

extern int tedAppSetNewList(	EditApplication *		ea );

extern int tedAppChangeCurrentList(
				EditApplication *		ea,
				const DocumentList *		dlNew );

extern int tedDocChangeParagraphProperties(
				EditDocument *			ed,
				const PropertyMask *		ppUpdMask,
				const ParagraphProperties *	ppNew );

extern void tedAppDeleteHeaderFooter(	EditApplication *	ea,
					int			which );

extern void tedAppEditHeaderFooter(	EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					int			which );

extern int tedGetSelection(	DocumentSelection *	ds,
				SelectionGeometry *	sg,
				SelectionDescription *	sd,
				TedDocument *		td );

extern void tedFillBookmarkList(	APP_WIDGET		list,
					DocumentFieldList *	dfl );

extern int tedFindRootForPosition(	ExternalItem **		pEi,
					BufferItem **		pRootBi,
					BufferItem **		pBodySectBi,
					int *			pSectNr,
					int *			pPage,
					EditDocument *		ed,
					int			docX,
					int			docY );

extern void tedDetermineDefaultSettings(	TedAppResources *	tar );

extern int tedReplaceSelectionWithBitmapImage(	EditDocument *		ed,
						AppBitmapImage *	abi );

extern void tedAppEditNote(		EditApplication *	ea );
extern void tedAppGotoNoteRef(		EditApplication *	ea );

extern int tedSetIBarSelection(		EditDocument *		ed,
					BufferItem *		bi,
					int			stroff,
					int			lastLine,
					int *			pScrolledX,
					int *			pScrolledY );

extern void tedDrawShadedRectangle(
			AppDrawingData *	add,
			APP_BITMAP_IMAGE	shadingPixmaps[DOCsp_COUNT],
			int			pattern,
			int			x0,
			int			x1,
			int			y0,
			int			y1 );

extern void tedDrawHorizontalBorderLine(AppDrawingData *	add,
					int			style,
					int			minThick,
					int			x0,
					int			x1,
					int			y );

extern int tedAppAllocateDocumentColor(	EditApplication *	ea,
					const RGB8Color *	rgb8 );

extern int tedDocAllocateDocumentColor(	EditDocument *		ed,
					const RGB8Color *	rgb8 );

extern void tedFontToolSet(	void *				voidea,
				const ExpandedTextAttribute *	etaSet,
				const PropertyMask *		taSetMask );

extern void tedListFontToolSet(	void *				voidea,
				const ExpandedTextAttribute *	etaSet,
				const PropertyMask *		taSetMask );

extern int tedOpenDocumentFile(	EditApplication *	ea,
				int *			pFormat,
				BufferDocument **	pBd,
				const char *		filename,
				APP_WIDGET		relative,
				APP_WIDGET		option );

extern int tedDocFindNext(	EditDocument *		ed );
extern int tedDocFindPrev(	EditDocument *		ed );

extern int tedFindSetPattern(	EditDocument *		ed,
				const unsigned char *	pattern,
				int			useRegex );

extern void tedGetObjectRectangle(
				DocumentRectangle *		drObject,
				APP_POINT *			xp,
				const InsertedObject *		io,
				const PositionGeometry *	pg,
				const TedDocument *		td );

extern int tedObjectDrag(	APP_WIDGET			w,
				EditDocument *			ed,
				APP_EVENT *			downEvent );

extern void tedObjectSetImageProperties(
				PropertyMask *			pipDoneMask,
				EditDocument *			ed,
				InsertedObject *		io,
				const DocumentPosition *	dpObject,
				int				partObject,
				const PositionGeometry *	pgObject,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pipFrom );

#   endif	/*  TED_APP_H	*/
