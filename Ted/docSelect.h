#   ifndef		DOC_SELECT_H
#   define		DOC_SELECT_H

#   include		"docLayoutPosition.h"

/************************************************************************/
/*									*/
/*  Forward declarations of structs.					*/
/*									*/
/************************************************************************/

struct BufferDocument;
struct BufferItem;
struct DocumentField;

/************************************************************************/
/*									*/
/*  A position in a document.						*/
/*									*/
/*  1)  Indication for the kind of root item for the selection.		*/
/*  2)  Geometry is derived from the position, NOT the other way round.	*/
/*									*/
/*  A)  Indication for the kind of root item for the selection.		*/
/*  B)  When the selection is not in the body of the document, the	*/
/*	number of the section that the external item belongs to.	*/
/*  D)  If the external item is a footnote/endnote: The index in the	*/
/*	array of notes in the section.					*/
/*									*/
/*  E)  The beginning, end and anchor of the selection. The order is in	*/
/*	the reading direction. NOT in the order that the selection was	*/
/*	made. USE EXTREME CARE WHEN YOU REFER TO THE OLD POSITION IN	*/
/*	EDITING CODE.							*/
/*									*/
/************************************************************************/

typedef struct DocumentPosition
    {
    struct BufferItem *	dpBi;
    unsigned int	dpStroff;
    } DocumentPosition;

typedef struct SelectionScope
    {
    int			ssInExternalItem;			/*  A  */
    int			ssSectNrExternalTo;			/*  B  */
    int			ssNoteArrayIndex;			/*  D  */
    } SelectionScope;

typedef struct DocumentSelection
    {
    SelectionScope	dsSelectionScope;

    DocumentPosition	dsBegin;				/*  E  */
    DocumentPosition	dsEnd;					/*  E  */
    DocumentPosition	dsAnchor;				/*  E  */

    int			dsCol0;
    int			dsCol1;
    int			dsDirection;
    } DocumentSelection;

/*  2  */
typedef struct PositionGeometry
    {
    int			pgLine;
    int			pgAtLineHead;
    int			pgAtLineEnd;

    int			pgXPixels;
    LayoutPosition	pgTopPosition;
    int			pgY1Pixels;
    int			pgBaselinePixels;
    } PositionGeometry;

typedef struct SelectionGeometry
    {
    DocumentRectangle	sgRectangle;

    PositionGeometry	sgBegin;				/*  E  */
    PositionGeometry	sgEnd;					/*  E  */
    PositionGeometry	sgAnchor;				/*  E  */
    } SelectionGeometry;

typedef struct TableRectangle
    {
    int		trCol0;
    int		trCol1;
    int		trCol11;

    int		trRow00;
    int		trRow0;
    int		trRow1;
    int		trRow11;

    int		trCellColspan;
    int		trCellRowspan;
    } TableRectangle;

#   define	docSamePosition(b,e) \
		((b)->dpBi == (e)->dpBi && (b)->dpStroff == (e)->dpStroff)

#   define	docPositionsInsideParagraph(b,e) \
		((b)->dpBi == (e)->dpBi)
#   define	docPositionsInsideCell(b,e) \
		((b)->dpBi->biParent == (e)->dpBi->biParent)
#   define	docPositionsInsideRow(b,e) \
		((b)->dpBi->biParent->biParent == (e)->dpBi->biParent->biParent)

#   define	docSelectionInsideParagraph(ds) \
		docPositionsInsideParagraph(&(ds)->dsBegin,&(ds)->dsEnd)
#   define	docSelectionInsideCell(ds) \
		docPositionsInsideCell(&(ds)->dsBegin,&(ds)->dsEnd)
#   define	docSelectionInsideRow(ds) \
		docPositionsInsideRow(&(ds)->dsBegin,&(ds)->dsEnd)

/************************************************************************/
/*									*/
/*  Describe a selection and its relevance for application tools.	*/
/*									*/
/************************************************************************/

typedef struct SelectionDescription
    {
    unsigned int	sdDocumentId;
    int			sdIsSet;
    int			sdDocumentReadonly;
    int			sdIsIBarSelection;
    int			sdIsSingleParagraph;
    int			sdIsSingleCell;
    int			sdBeginInTable;
    int			sdBeginInTableHeader;
    int			sdIsColSlice;
    int			sdIsRowSlice;
    int			sdIsTableSlice;
    int			sdIsTableRectangle;
    int			sdIsObjectSelection;

    int			sdCanReplace;
    int			sdInExternalItem;
    int			sdInDocumentBody;
    int			sdInHeaderFooter;

    int			sdBeginInField;
    int			sdBeginInHyperlink;
    int			sdBeginInBookmark;

    int			sdIsListBullet;

    int			sdHasLists;
    int			sdFirstListParaNr;
    int			sdListOverride;
    int			sdListLevel;
    int			sdMultiList;
    int			sdMultiLevel;
    } SelectionDescription;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docInitDocumentPosition(	DocumentPosition *	dp );
extern void docInitDocumentSelection(	DocumentSelection *	ds );
extern void docInitPositionGeometry(	PositionGeometry *	pg );
extern void docInitSelectionGeometry(	SelectionGeometry *	sg );

extern void docSetIBarSelection(	DocumentSelection *		ds,
					const DocumentPosition *	dp );

extern int docIsIBarSelection( const DocumentSelection *		ds );
extern int docIsParaSelection( const DocumentSelection *		ds );

extern int docGetObjectSelection(
				const DocumentSelection *	ds,
				int *				pPart,
				DocumentPosition *		dpObject,
				InsertedObject **		pIo );

extern void docSetRangeSelection(
				DocumentSelection *		ds,
				const DocumentPosition *	dpFrom,
				const DocumentPosition *	dpTo,
				int				direction,
				int				col0,
				int				col1 );

extern int docBeginOfLine(	DocumentPosition *		dp,
				int				wasAtHead );
extern int docEndOfLine(	DocumentPosition *		dp,
				int				wasAtHead );

extern int docGetTableRectangle(	TableRectangle *		tr,
					const DocumentSelection *	ds );

extern int docGetTableSliceSelection(
				int *				pIsRowSlice,
				int *				pIsColSlice,
				TableRectangle *		tr,
				const DocumentSelection *	ds );

extern void docDescribeSelection(
			    SelectionDescription *		sd,
			    const DocumentSelection *		ds,
			    const struct BufferDocument *	bd,
			    unsigned int			documentId,
			    int					documentRo );

extern void docInitSelectionDescription(	SelectionDescription *	sd );

#   endif	/*	DOC_SELECT_H	*/
