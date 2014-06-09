#   ifndef	DOC_DRAW_H
#   define	DOC_DRAW_H

#   include	"docBuf.h"
#   include	"docLayout.h"
#   include	<appDraw.h>

/************************************************************************/
/*									*/
/*  The different resources needed for drawing.				*/
/*									*/
/************************************************************************/

struct DrawingContext;

typedef void (*PARA_SIDES_PIXELS)( struct ParagraphFrame *	pf,
				const AppDrawingData *		add,
				const BufferItem *		bi );

typedef int (*DRAW_TEXT_LINE)(	const BufferItem *		bi,
				int				line,
				const struct ParagraphFrame *	pf,
				void *				through,
				struct DrawingContext *		dc,
				int				pShift,
				int				xShift,
				int				yShift );

typedef int (*DRAW_PARA_TOP)(	const BorderProperties *	bpTop,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				const struct ParagraphFrame *	pf,
				const LayoutPosition *		lpTop,
				void *				through,
				struct DrawingContext *		dc );

typedef int (*DRAW_PARA_BOTTOM)( const BorderProperties *	bpBottom,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				const struct ParagraphFrame *	pf,
				const LayoutPosition *		lpBelow,
				void *				through,
				struct DrawingContext *		dc );

typedef int (*DRAW_PARA_LEFT)(	const BorderProperties *	bpLeft,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpBottom,
				const struct ParagraphFrame *	pf,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow,
				void *				through,
				struct DrawingContext *		dc );

typedef int (*DRAW_PARA_RIGHT)( const BorderProperties *	bpRight,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpBottom,
				const struct ParagraphFrame *	pf,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow,
				void *				through,
				struct DrawingContext *		dc );

typedef int (*DRAW_PARA_SHADE)(	const ParagraphProperties *	pp,
				const BorderProperties *	bpTop,
				void *				through,
				struct DrawingContext *		dc,
				const struct ParagraphFrame *	pf,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow );

typedef int (*DRAW_CELL_TOP)(	const BorderProperties *	bp,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				int				asGrid,
				int				x0Twips,
				int				x1Twips,
				void *				through,
				struct DrawingContext *		dc,
				const LayoutPosition *		lpTop );

typedef int (*DRAW_CELL_BOTTOM)( const BorderProperties *	bp,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				int				asGrid,
				int				x0Twips,
				int				x1Twips,
				void *				through,
				struct DrawingContext *		dc,
				const LayoutPosition *		lpBottom );

typedef int (*DRAW_CELL_LEFT)(	const BorderProperties *	bp,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpBottom,
				int				asGrid,
				void *				through,
				struct DrawingContext *		dc,
				int				x0Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow );

typedef int (*DRAW_CELL_RIGHT)(	const BorderProperties *	bp,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpBottom,
				int				asGrid,
				void *				through,
				struct DrawingContext *		dc,
				int				x1Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow );

typedef int (*DRAW_CELL_SHADE)(	const CellProperties *		cp,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				void *				through,
				struct DrawingContext *		dc,
				int				x0Twips,
				int				x1Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow );

typedef int (*FINISH_PAGE)(	void *				through,
				struct DrawingContext *		dc,
				BufferItem *			bodySectBi,
				int				page,
				int				asLast );

typedef int (*START_PAGE)(	void *				through,
				const DocumentGeometry *	dg,
				struct DrawingContext *		dc,
				int				page );

typedef int (*SET_COLOR_RGB)(	struct DrawingContext *		dc,
				void *				through,
				int				r,
				int				g,
				int				b );

typedef int (*SET_FONT)(	struct DrawingContext *		dc,
				void *				through,
				int				textAttr,
				const TextAttribute *		ta );

typedef int (*DRAW_SHAPE)(	const DocumentRectangle *	twipsRect,
				int				page,
				DrawingShape *			ds,
				struct DrawingContext *		dc,
				void *				through );

typedef struct DrawingContext
    {
    int				dcCurrentTextAttributeSet;
    TextAttribute		dcCurrentTextAttribute;
    int				dcCurrentColorSet;
    RGB8Color			dcCurrentColor;

    const PostScriptFontList *	dcPostScriptFontList;
    AppDrawingData *		dcDrawingData;
    ScreenFontList *		dcScreenFontList;	/*  Do not use	*/
							/*  for printing */
    BufferDocument *		dcDocument;
    const DocumentRectangle *	dcClipRect;
    const DocumentSelection *	dcDocumentSelection;
    const SelectionGeometry *	dcSelectionGeometry;
    int				dcFirstPage;
    int				dcLastPage;
    int				dcDrawExternalItems;
    int				dcPostponeHeadersFooters;
    int				dcDocHasPageHeaders;
    int				dcDocHasPageFooters;

    int				dcDrawTableGrid;

    SET_COLOR_RGB		dcSetColorRgb;
    SET_FONT			dcSetFont;
    DRAW_SHAPE			dcDrawShape;

    DRAW_TEXT_LINE		dcDrawTextLine;

    DRAW_PARA_TOP		dcDrawParaTop;
    DRAW_PARA_BOTTOM		dcDrawParaBottom;
    DRAW_PARA_LEFT		dcDrawParaLeft;
    DRAW_PARA_RIGHT		dcDrawParaRight;
    DRAW_PARA_SHADE		dcDrawParaShade;
    DRAW_CELL_TOP 		dcDrawCellTop;
    DRAW_CELL_BOTTOM 		dcDrawCellBottom;
    DRAW_CELL_LEFT		dcDrawCellLeft;
    DRAW_CELL_RIGHT		dcDrawCellRight;
    DRAW_CELL_SHADE		dcDrawCellShade;
    FINISH_PAGE			dcFinishPage;
    START_PAGE			dcStartPage;
    PARA_SIDES_PIXELS		dcParaFramePixels;
    INIT_LAYOUT_EXTERNAL	dcInitLayoutExternal;
    DOC_CLOSE_OBJECT		dcCloseObject;
    } DrawingContext;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitDrawingContext(	DrawingContext *	dc );

extern int docDrawItem(	BufferItem *			bi,
			void *				through,
			DrawingContext *		dc );

extern int docDrawPageHeader(	BufferItem *			sectBi,
				void *				through,
				DrawingContext *		dc,
				int				page );

extern int docDrawPageFooter(	BufferItem *			sectBi,
				void *				through,
				DrawingContext *		dc,
				int				page );

extern int docDrawShapesForPage(	BufferItem *		shapeRootBi,
					void *			through,
					DrawingContext *	dc,
					int			page );

extern int docDrawFootnotesForColumn(
				int				page,
				void *				through,
				DrawingContext *		dc );

extern int docDrawEndnotesForSection(	int			sect,
					void *			through,
					DrawingContext *	dc );

extern int docDrawEndnotesForDocument(	void *			through,
					DrawingContext *	dc );

extern int docDrawToPageOfItem(		BufferItem *		prevBodyBi,
					BufferItem *		thisBodyBi,
					BufferItem *		thisBi,
					void *			through,
					LayoutPosition *	lpHere,
					DrawingContext *	dc,
					int			pShift );

extern void docDrawSetColorRgb(		DrawingContext *	dc,
					void *			through,
					int			r,
					int			g,
					int			b );

extern void docDrawSetColorNumber(	DrawingContext *	dc,
					void *			through,
					int			colorNumber );

extern void docDrawDelimitString(	int *			pLen,
					int *			pDrawn,
					TextAttribute *		ta,
					const BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			count,
					int			separate );

extern void docDrawSetFont(	DrawingContext *	dc,
				void *			through,
				int			textAttr,
				const TextAttribute *	ta );

extern int docShapeGetFill(	int *			pFill,
				const DrawingShape *	ds,
				DrawingContext *	dc,
				void *			through );

extern int docShapeGetLine(	int *			pFill,
				const DrawingShape *	ds,
				DrawingContext *	dc,
				void *			through );

extern int docDrawShape(	DrawingContext *		dc,
				void *				through,
				const InsertedObject *		io );

extern void docShapeGetPixelRect(
				DocumentRectangle *		pixelRect,
				const DrawingContext *		dc,
				const DocumentRectangle *	twipsRect,
				int				page );

#   endif
