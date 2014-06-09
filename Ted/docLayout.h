/************************************************************************/
/*									*/
/*  Document Layout and Printing related functionality.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LAYOUT_H
#   define	DOC_LAYOUT_H

#   include	<psFont.h>
#   include	"docBuf.h"
#   include	"docEditRange.h"
#   include	"docScreenFontList.h"
#   include	"utilPs.h"

#   include	<appDraw.h>

/************************************************************************/
/*									*/
/*  Pixel position on a page						*/
/*									*/
/************************************************************************/

# define LP_YPIXELS( add, lp ) \
	    ( (add)->addPageStepPixels* (lp)->lpPage+ \
	      TWIPStoPIXELS( (add)->addMagnifiedPixelsPerTwip, \
						    (lp)->lpPageYTwips ) )

/**/

# define BI_TOP_PIXELS( add, bi ) \
			LP_YPIXELS( add, &((bi)->biTopPosition) )

# define BI_BELOW_PIXELS( add, bi ) \
			LP_YPIXELS( add, &((bi)->biBelowPosition) )

# define BI_TOP_PIXELS_SH( add, bi, pS, yS ) \
	( (add)->addPageStepPixels* ( (bi)->biTopPosition.lpPage+ (pS) )+ \
	  TWIPStoPIXELS( (add)->addMagnifiedPixelsPerTwip, \
	    (bi)->biTopPosition.lpPageYTwips+ (yS) ) )

/**/

# define PG_TOP_PIXELS( add, pg ) \
			LP_YPIXELS( add, &((pg)->pgTopPosition) )

/**/

# define TL_TOP_PIXELS( add, tl ) LP_YPIXELS( add, &((tl)->tlTopPosition) )

# define TL_BASE_PIXELS( add, tl ) \
	( (add)->addPageStepPixels* (tl)->tlTopPosition.lpPage+ \
	  TWIPStoPIXELS( (add)->addMagnifiedPixelsPerTwip, \
	    (tl)->tlTopPosition.lpPageYTwips+ (tl)->tlLineAscentTwips ) )

# define TL_BELOW_PIXELS( add, tl ) \
	( (add)->addPageStepPixels* (tl)->tlTopPosition.lpPage+ \
	  TWIPStoPIXELS( (add)->addMagnifiedPixelsPerTwip, \
	    (tl)->tlTopPosition.lpPageYTwips+ (tl)->tlLineSpacingTwips ) )

# define TL_TOP_PIXELS_SH( add, tl, pS, yS ) \
	( (add)->addPageStepPixels* ( (tl)->tlTopPosition.lpPage+ (pS) )+ \
	  TWIPStoPIXELS( (add)->addMagnifiedPixelsPerTwip, \
	    (tl)->tlTopPosition.lpPageYTwips+ (yS) ) )

# define TL_ASC_PIXELS_SH( add, tl, pS, yS ) \
	( (add)->addPageStepPixels* ( (tl)->tlTopPosition.lpPage+ (pS) )+ \
	  TWIPStoPIXELS( (add)->addMagnifiedPixelsPerTwip, \
	    (tl)->tlTopPosition.lpPageYTwips+ \
	    (tl)->tlLineHeightTwips- \
	    (tl)->tlLineAscentTwips+ (yS) ) )

# define TL_BASE_PIXELS_SH( add, tl, pS, yS ) \
	( (add)->addPageStepPixels* ( (tl)->tlTopPosition.lpPage+ (pS) )+ \
	  TWIPStoPIXELS( (add)->addMagnifiedPixelsPerTwip, \
	    (tl)->tlTopPosition.lpPageYTwips+ (yS)+ (tl)->tlLineAscentTwips ) )

# define TL_BELOW_PIXELS_SH( add, tl, pS, yS ) \
	( (add)->addPageStepPixels* ( (tl)->tlTopPosition.lpPage+ (pS) )+ \
	  TWIPStoPIXELS( (add)->addMagnifiedPixelsPerTwip, \
	    (tl)->tlTopPosition.lpPageYTwips+ (yS)+ (tl)->tlLineSpacingTwips ) )

/************************************************************************/
/*									*/
/*  The frame in which a series of successive lines in a paragraph is	*/
/*  is formatted. The frame lies on one page, and when the text has to	*/
/*  flow around an illustration, the formatter is called with different	*/
/*  frames for the text above the illustration, at the side of the	*/
/*  illustration and below the illustration.				*/
/*									*/
/*  Coordinates are absolute coordinates on the page or on the window.	*/
/*  [On the window they are shifted by the scrollbars.]			*/
/*									*/
/************************************************************************/

typedef struct ParagraphFrame
    {
    int		pfX0GeometryTwips;
    int		pfX0TextLinesTwips;
    int		pfX0FirstLineTwips;

    int		pfX1GeometryTwips;
    int		pfX1TextLinesTwips;

    int		pfPageY1Twips;
    int		pfFrameY1Twips;

    int		pfX0TextLinesPixels;
    int		pfX0FirstLinePixels;
    int		pfX1TextLinesPixels;

    int		pfX0Pixels;
    int		pfX1Pixels;

    int		pfPageHigh;
    int		pfStripHigh;
    int		pfHasBottom;

    int		pfBlockX0Twips;
    int		pfBlockX1Twips;
    int		pfBlockY0Twips;
    int		pfBlockY1Twips;

    int		pfRedrawX0Twips;
    int		pfRedrawX1Twips;
    int		pfRedrawX0Pixels;
    int		pfRedrawX1Pixels;
    } ParagraphFrame;

/************************************************************************/
/*									*/
/*  Frame in which the regular text on a page is formatted. This is	*/
/*  either the page or a (newspaper style) column on the page. The	*/
/*  page headers and footers get their own block frame. The same	*/
/*  for the footnotes and endnotes. The space for footnotes is however	*/
/*  subtracted from the block frame for the page.			*/
/*									*/
/*  NOTE that table columns are not implemented with a block frame.	*/
/*									*/
/************************************************************************/

typedef struct NotesReservation
    {
    int		nrFirstFootnote;
    int		nrFootnoteCount;

    int		nrFtnsepHeight;
    int		nrFootnoteHeight;
    } NotesReservation;

typedef struct BlockFrame
    {
    int			bfX0Twips;
    int			bfX1Twips;
    int			bfY0Twips;
    int			bfY1Twips;

    int			bfYBelowShapes;

    int			bfColumnWidthTwips;

    int			bfFootnotesPlaced;
    NotesReservation	bfNotesReservation;
    } BlockFrame;

#   define	BF_HAS_FOOTNOTES( bf ) \
		    ((bf)->bfNotesReservation.nrFirstFootnote >= 0)

/************************************************************************/
/*									*/
/*  Additional information for the layout of a line of text. Some of	*/
/*  the sfields are used for scratch in the process.			*/
/*									*/
/************************************************************************/

typedef struct ParticuleData
    {
    int			pdX0;
    int			pdWidth;
    int			pdDecWidth;
    int			pdVisibleWidth;
    int			pdTabKind;
    int			pdTabNumber;
    int			pdTabPosition;
    const AfmFontInfo *	pdAfi;

    int			pdVisiblePixels;
    int			pdWhiteUnits;
    int			pdCorrectBy;
    } ParticuleData;

/************************************************************************/
/*									*/
/*  Pass through information for a layout job.				*/
/*									*/
/************************************************************************/

typedef int (*LAYOUT_SCREEN_LINE)( TextLine *			tl,
				const BufferItem *		bi,
				const BufferDocument *		bd,
				int				part,
				int				accepted,
				AppDrawingData *		add,
				const ScreenFontList *		sfl,
				const ParagraphFrame *		ffPixels,
				ParticuleData *			pd );

typedef int (*START_PARAGRAPH)(	const BufferDocument *		bd,
				BufferItem *			bi,
				AppDrawingData *		add );

typedef void (*SCREEN_FRAME)(	ParagraphFrame *		pf,
				const AppDrawingData *		add,
				const BufferItem *		bi );

typedef int (*START_SCREEN_PARAGRAPH)(
				BufferItem *			bi,
				const ParagraphFrame *		pf,
				AppDrawingData *		add,
				ScreenFontList *		sfl,
				BufferDocument *		bd );

typedef int (*START_ROW)(	BufferItem *			rowBi,
				AppDrawingData *		add,
				BufferDocument *		bd );

typedef struct ScreenLayout
    {
    START_ROW			slStartRow;
    START_SCREEN_PARAGRAPH	slStartParagraph;
    SCREEN_FRAME		slScreenFrame;
    LAYOUT_SCREEN_LINE		slLayoutLine;
    } ScreenLayout;

/************************************************************************/
/*									*/
/*  A Layout Job.							*/
/*									*/
/*  A)	The rectangle that is to be redrawn on the screen. Coordinates	*/
/*	are in pixels, but relative to the document as a whole. ( Not	*/
/*	dependent on the position of the scrollbars. ) In non		*/
/*	interactive layout jobs this is a NULL pointer.			*/
/*  B)	Drawing data for font information, scaling from wips to pixels	*/
/*	etc.								*/
/*  C)	The document as a whole.					*/
/*  D)	The Node in the document tree that initiated the layout job.	*/
/*									*/
/*  E)	The current position on paper of the layout job.		*/
/*									*/
/*  F)	Indirection for screen calculations when laying out the		*/
/*	document.							*/
/*  G)	Indirection for screen calculations when just placing the lines	*/
/*	in the document again.						*/
/*									*/
/************************************************************************/

typedef struct LayoutJob
    {
    DocumentRectangle *		ljChangedRectanglePixels;	/*  A	*/
    AppDrawingData *		ljAdd;				/*  B	*/
    BufferDocument *		ljBd;				/*  C	*/
    BufferItem *		ljChangedItem;			/*  D	*/
    ScreenFontList *		ljScreenFontList;

    LayoutPosition		ljPosition;			/*  E	*/

    ScreenLayout		ljLayoutScreen;			/*  F	*/
    ScreenLayout		ljPlaceScreen;			/*  G	*/
    } LayoutJob;

/************************************************************************/
/*									*/
/*  For formatting a series of paragraphs. ( Inside a Cell or just	*/
/*  below eachother )							*/
/*									*/
/************************************************************************/

typedef struct ParagraphStripPosition
    {
    int			pspPara;
    int			pspPart;
    int			pspLine;
    } ParagraphStripPosition;

typedef struct ParagraphLayoutPosition
    {
    ParagraphStripPosition	plpProgress;
    LayoutPosition		plpPos;
    ParagraphFrame		plpFormattingFrame;
    } ParagraphLayoutPosition;

typedef struct ParagraphLayoutJob
    {
    int				pljParaUpto;

    ParagraphLayoutPosition	pljPos;
    ParagraphLayoutPosition	pljPos0;
    } ParagraphLayoutJob;

struct ParagraphLayoutContext;

typedef int (*LAYOUT_LINES)(
			ParagraphLayoutPosition *		plp,
			BlockFrame *				bf,
			const struct ParagraphLayoutContext *	plc,
			BufferItem *				bi );

typedef void (*ADJUST_BOTTOM)(		int *			pChanged,
					BufferItem *		bi,
					const LayoutPosition *	lp,
					AppDrawingData *	add,
					DocumentRectangle *	drChanged );

typedef struct ParagraphLayoutContext
    {
    DocumentRectangle *		plcChangedRectanglePixels;
    AppDrawingData *		plcAdd;
    BufferDocument *		plcBd;
    ScreenFontList *		plcScreenFontList;

    ScreenLayout		plcScreenLayout;
    START_PARAGRAPH		plcStartParagraph;
    ADJUST_BOTTOM		plcAdjustBottom;
    } ParagraphLayoutContext;

typedef int (*INIT_LAYOUT_EXTERNAL)(
				LayoutJob *			lj,
				ExternalItem *			ei,
				int				page );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docPsInitLayoutJob(		LayoutJob *	lj );
extern void docPsCleanLayoutJob(	LayoutJob *	lj );

extern int docLayoutLineBox(	const BufferDocument *		bd,
				TextLine *			tl,
				const BufferItem *		bi,
				int				part,
				const PostScriptFontList *	apfl,
				ParticuleData *			pd,
				const ParagraphFrame *		pf );

extern int docPsClaimParticuleData(	const BufferItem *	bi,
					ParticuleData **	pData );

extern int docPsParagraphLineExtents(	const BufferDocument *		bd,
					const PostScriptFontList *	psfl,
					BufferItem *			bi );

extern int docPsPrintGetDocumentFonts(
				BufferDocument *		bd,
				PostScriptTypeList *		pstl,
				const PostScriptFontList *	x_psfl );

extern int docPsPrintDocument(
			SimpleOutputStream *		sos,
			const char *			title,
			const char *			applicationName,
			const char *			applicationReference,
			const char *			fontDirectory,
			AppDrawingData *		add,
			BufferDocument *		bd,
			const PrintGeometry *		pg,
			DOC_CLOSE_OBJECT		closeObject );

extern int docLayoutItemAndParents(	BufferItem *		bi,
					LayoutJob *		lj );

extern int docAdjustParaLayout(		BufferItem *		bi,
					int			line,
					int			stroffShift,
					int			upto,
					LayoutJob *		lj );

extern void docLayoutSectColumnTop(
				BufferItem *			bodySectBi,
				BufferDocument *		bd,
				LayoutPosition *		lpTop,
				BlockFrame *			bf );

extern int docPsListImageFonts( PostScriptTypeList *		pstl,
				const PictureProperties *	pip,
				const MemoryBuffer *		mb,
				const PostScriptFontList *	psfl,
				const char *			prefix );

extern int docLayoutExternalItem( ExternalItem *	ei,
				DocumentRectangle *	drChanged,
				int			pageNumber,
				int			y0Twips,
				BufferDocument *	bd,
				const BufferItem *	sectBi,
				AppDrawingData *	add,
				ScreenFontList *	sfl,
				INIT_LAYOUT_EXTERNAL	initLayoutExternal,
				DOC_CLOSE_OBJECT	closeObject );

extern int docLayoutParagraphsInStrip(
				ParagraphLayoutJob *		plj,
				BlockFrame *			bf,
				const ParagraphLayoutContext *	plc,
				BufferItem *			cellBi,
				int				paraUpto,
				int				bottomTwips,
				int				stripHigh );

extern int docLayoutParagraphs(	const ParagraphLayoutContext *	plc,
				BufferItem *			cellBi,
				BlockFrame *			bf,
				LayoutJob *			lj,
				ParagraphLayoutJob *		plj );

extern void docInitParagraphLayoutJob( ParagraphLayoutJob *	plj );

extern void docLayoutToNextColumn(	BufferItem *			bi,
					BufferDocument *		bd,
					LayoutPosition *		lpTop,
					BlockFrame *			bf );

extern void docPsBeginParagraphLayoutProgress(	ParagraphLayoutJob *	plj,
						int			para,
						int			line,
						int			part,
						int			pUpto,
						const LayoutPosition *	lp );

extern int docLayoutRowItem(	BufferItem *			rowBi,
				BlockFrame *			bf,
				LayoutJob *			lj,
				const ParagraphLayoutContext *	plc );

extern void docLayoutPushBottomDown(	LayoutPosition *	lpRowBottom,
					const LayoutPosition *	lpColBottom );

extern void docLayoutPushBottomDownShifted(
					LayoutPosition *	lpRowBottom,
					const LayoutPosition *	lpColBottom,
					int			pShift,
					int			yShift );

extern void docParagraphFrameTwips(
				ParagraphFrame *		pf,
				const BlockFrame *		bf,
				int				bottom,
				int				stripHigh,
				const BufferItem *		paraBi );

extern void docPsAdvanceParagraphLayout(
				int *				pAdvanced,
				ParagraphStripPosition *	psp0,
				const ParagraphStripPosition *	psp00,
				const ParagraphStripPosition *	psp1,
				int				page,
				const BufferItem *		cellBi );

extern void docParagraphStripFrame(
				const ScreenLayout *		sl,
				BufferItem *			cellBi,
				AppDrawingData *		add,
				int				bottomTwips,
				int				stripHigh,
				const BlockFrame *		bf,
				ParagraphLayoutJob *		plj );

extern void docBlockFrameTwips( BlockFrame *			bf,
				BufferItem *			bi,
				const BufferDocument *		bd,
				int				page,
				int				column );

extern void docLayoutInitBlockFrame(	BlockFrame *		bf );
extern void docInitNotesReservation(	NotesReservation *      nr );

extern void docLayoutReserveNoteHeight(
				ParagraphFrame *		pf,
				BlockFrame *			bf,
				const NotesReservation *	nrLine );

extern int docLayoutFootnotesForColumn(	LayoutPosition *	lpBelowNotes,
					const BlockFrame *	refBf,
					int			belowText,
					const LayoutPosition *	lpBelowText,
					const LayoutJob *	refLj );

extern int docLayoutCollectParaFootnoteHeight(
					NotesReservation *	nr,
					int			referringPage,
					int			referringColumn,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			partFrom,
					int			partUpto );

extern int docCollectFootnotesForColumn(
				BlockFrame *			bf,
				const DocumentPosition *	dpHere,
				int				partHere,
				const LayoutJob *		refLj );

extern int docNoteSeparatorRectangle(	DocumentRectangle *	drExtern,
					ExternalItem **		pEiNoteSep,
					int *			pY0Twips,
					BufferDocument *	bd,
					const DocumentNote *	dnFirstNote,
					int			extItKind,
					const AppDrawingData *	add );

extern int docGetExternalItemBox(
				DocumentRectangle *		dr,
				const BufferItem *		bodySectBi,
				const ExternalItem *		ei,
				int				justUsed,
				int				page,
				BufferDocument *		bd,
				AppDrawingData *		add );

extern int docExternalItemPrelayout(	ExternalItem *			ei,
					const DocumentGeometry *	dgRef,
					LayoutJob *			lj );

extern int docLayoutItemImplementation(	BufferItem *		bi,
					BlockFrame *		bf,
					LayoutJob *		lj );

extern int docSectHeaderFooterPrelayout(	BufferItem *	sectBi,
						LayoutJob *	lj );

extern int docLayoutEndnotesForSection(	int			sect,
					BlockFrame *		bf,
					LayoutJob *		lj );

extern int docLayoutEndnotesForDocument(	BlockFrame *		bf,
						LayoutJob *		lj );

extern void docLayoutAddBorderToInset(	int *				pInset,
					const BorderProperties *	bp );

extern void docLayoutStretchInsetForBorder(
					int *				pInset,
					const BorderProperties *	bp );

extern int docLayoutLines(	ParagraphLayoutPosition *	plp,
				BlockFrame *			bf,
				const ParagraphLayoutContext *	plc,
				BufferItem *			paraBi );

extern void docLayoutScaleObjectToFitParagraphFrame(
				InsertedObject *		io,
				const ParagraphFrame *		pf );

extern int docLayoutSectItem(		BufferItem *		sectBi,
					BlockFrame *		bf,
					LayoutJob *		lj );

extern void docLayoutPlaceSectTop(	BufferItem *		sectBi,
					BlockFrame *		bf,
					LayoutJob *		lj );

extern int docMakeCapsString(	unsigned char **	pUpperString,
				int **			pSegments,
				int *			pSegmentCount,
				const BufferDocument *	bd,
				const TextAttribute *	ta,
				const unsigned char *	printString,
				int			len );

extern void docResetExternalItemLayout(	BufferDocument *	bd );

extern void docDrawingShapeInvalidateTextLayout(	DrawingShape *	ds );

extern void docShapePageY(	LayoutPosition *	lpShapeTop,
				LayoutPosition *	lpBelowShape,
				const DrawingShape *	ds,
				const BufferItem *	paraBi,
				const LayoutPosition *	lpLineTop,
				const ParagraphFrame *	pf );

extern void docShapePageRectangle(
				LayoutPosition *	lpShapeTop,
				LayoutPosition *	lpBelowShape,
				int *			pX0,
				int *			pX1,
				const DrawingShape *	ds,
				const BufferItem *	paraBi,
				const LayoutPosition *	lpLineTop,
				const ParagraphFrame *	pf,
				int			x0Twips );

extern int docShapeCheckTextLayout(
			DrawingShape *			ds,
			const DocumentRectangle *	twipsRect,
			DocumentRectangle *		drChanged,
			int				pageNumber,
			BufferDocument *		bd,
			AppDrawingData *		add,
			ScreenFontList *		sfl,
			INIT_LAYOUT_EXTERNAL		initLayoutExternal,
			DOC_CLOSE_OBJECT		closeObject );

extern int docItemLayoutStartPosition(	LayoutPosition *	lp,
					const BufferItem *	bi );

extern int docLayoutInvalidateRange(	DocumentSelection *	dsLayout,
					BufferItem *		selRootBi,
					EditRange *		er );

#   endif	/*  DOC_LAYOUT_H  */
