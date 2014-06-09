/************************************************************************/
/*									*/
/*  Text Editor Buffer structure.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_BUF_H
#   define	DOC_BUF_H

#   include	<time.h>

#   include	<bitmap.h>
#   include	<utilDocFont.h>
#   include	<psFont.h>
#   include	"docShape.h"
#   include	<geo2DInteger.h>
#   include	<sioGeneral.h>
#   include	<utilMemoryBuffer.h>
#   include	<utilTextAttributeAdmin.h>

#   include	"docParaProperties.h"
#   include	"docRowProperties.h"
#   include	"docSectProperties.h"
#   include	"docStyleSheet.h"
#   include	"docDocumentProperties.h"
#   include	"docShapeProperties.h"
#   include	"docField.h"
#   include	"docObject.h"
#   include	"docSelect.h"
#   include	"docExternalItem.h"
#   include	"docListTable.h"
#   include	"docListNumberTree.h"

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
/*  An homogenous piece of text. Without hyphenation, it will not be	*/
/*  divided over lines.							*/
/*  It consists of some characters and then some spaces.		*/
/*									*/
/************************************************************************/

typedef enum DocParticuleKind
    {
    DOCkindUNKNOWN= 0,

    DOCkindTEXT,
    DOCkindTAB,
    DOCkindOBJECT,
    DOCkindFIELDSTART,
    DOCkindFIELDEND,
    DOCkindNOTE,
    DOCkindTC,
    DOCkindXE,
    DOCkindLINEBREAK,
    DOCkindPAGEBREAK,
    DOCkindCOLUMNBREAK,
    DOCkindCHFTNSEP,
    DOCkindCHFTNSEPC,

    DOCkind_COUNT
    } DocParticuleKind;

#   define DOCkindBETWEEN_TEXT(k) \
	( \
	    (k) == DOCkindFIELDSTART	|| \
	    (k) == DOCkindFIELDEND	|| \
	    (k) == DOCkindNOTE		   \
	)

#   define	DOCkind_BITS	6

typedef struct TextParticule
    {
    unsigned int	tpStroff:16;
    unsigned int	tpStrlen:14;
    unsigned int	tpX0:12;
    unsigned int	tpPixelsWide:12;
    unsigned int	tpKind:DOCkind_BITS;
    unsigned int	tpInField:1;
    short int		tpObjectNumber;
    int			tpTextAttributeNumber;
    } TextParticule;

/************************************************************************/
/*									*/
/*  Used to lay out the text on the page.				*/
/*  A text line consists of a series of particules.			*/
/*									*/
/*  1)  What part of the string in the paragraph.			*/
/*  2)  What particules.						*/
/*  3)  Where is it geo/typographically?				*/
/*  4)  Postscript geometry information.				*/
/*	lineSpacing includes the leading that is irrelevant for the	*/
/*	last line of a page.						*/
/*  5)  Screen geometry.						*/
/*									*/
/*  6)  The width of the column for which the line was last formatted.	*/
/*	When the line is shifted to a column with the same width, it	*/
/*	is sufficient to change the coordinates of the line and its	*/
/*	contents during a reformat of the document. When the column	*/
/*	width is diffrerent, the line must be reformatted. (and most	*/
/*	probably subsequent lines as well).				*/
/*  6-) Before (6) is really used, we have to do away with the pixel	*/
/*	coordinates.							*/
/*									*/
/************************************************************************/

typedef struct TextLine
    {
								/*  1,2	*/
    int			tlStroff;
    int			tlFirstParticule;
    short int		tlStrlen;
    short int		tlParticuleCount;
    short int		tlWordCount;
								/*  3  */
    LayoutPosition	tlTopPosition;
								/*  4  */
    short int		tlLineAscentTwips;
    short int		tlLineHeightTwips;
    short int		tlLineSpacingTwips;
								/*  5  */
    unsigned char	tlHasPageBreak;
    unsigned char	tlHasInlineContent;

    short int		tlX0Pixels;
    short int		tlX1Pixels;

    short int		tlFrameWidthTwips;			/*  6  */
    short int		tlFrameX0Twips;				/*  6- */
    } TextLine;

/************************************************************************/
/*									*/
/*  A Paragraph.							*/
/*									*/
/************************************************************************/

typedef struct BufferPara
    {
    unsigned char *		btString;
    unsigned int		btStrlen;

    int				btParticuleCount;
    TextParticule *		btParticules;

    int				btObjectCount;
    InsertedObject *		btObjects;

    int				btLineCount;
    TextLine *			btLines;

    int				btShapeCount;
    DrawingShape *		btShapes;

    ParagraphProperties		btProperties;

    int				btAscentTwips;
    int				btDescentTwips;
    int				btLeadingTwips;

    const BorderProperties *	btBorderAboveParagraph;
    const BorderProperties *	btBorderBelowParagraph;
    } BufferPara;

/************************************************************************/
/*									*/
/*  Type dependent part of a BufferItem of 'Section' level.		*/
/*									*/
/*  Section level BufferItems are the sections of the document and the	*/
/*  'External Items' that are not part of the regular tree of items	*/
/*  that forms the document. Examples of 'External Items' are the page	*/
/*  headers and footers and the footnotes and endnotes in the document.	*/
/*									*/
/*  The root of external items are recognised by the fact that		*/
/*  bi->biInExternalItem != DOCinBODY. In practice there are no parents	*/
/*  to the section level items in external items. This is accidental	*/
/*  and should not be used as a recognition criterion.			*/
/*									*/
/*  1)  Formatting properties of the section.				*/
/*  2)  Page Headers.							*/
/*  3)  Page Footers.							*/
/*  5)  Number of the section in the document tree that an external	*/
/*	item belongs to.						*/
/*									*/
/************************************************************************/

typedef struct BufferSect
    {
				/*  1  */
    SectionProperties		bsProperties;

				/*  2  */
    ExternalItem		bsHeader;
    ExternalItem		bsFirstPageHeader;
    ExternalItem		bsLeftPageHeader;
    ExternalItem		bsRightPageHeader;

				/*  3  */
    ExternalItem		bsFooter;
    ExternalItem		bsFirstPageFooter;
    ExternalItem		bsLeftPageFooter;
    ExternalItem		bsRightPageFooter;

				/*  5  */
    SelectionScope		bsSelectionScope;

				/****************************************/
				/*					*/
				/*  Not used in the body of a document:	*/
				/*					*/
				/*  A)  Normally not set. Is set by the	*/
				/*	formatter to pass information	*/
				/*	to calculate page numbers.	*/
				/*  B)  Normally not set. In headers	*/
				/*	and footers it is the number of	*/
				/*	the page it is formatted for.	*/
				/*	[ Either during the formatting	*/
				/*	process or after formatting has	*/
				/*	been finished.]			*/
				/*					*/
				/****************************************/
    const struct BufferItem *	bsUseForSectBi;			/*  A	*/
    int				bsUseForPage;			/*  B	*/

				/*  A  */
#   define			biSectHeaderFooterUseForSectBi \
					    BIU.biuSect.bsUseForSectBi
				/*  B	*/
#   define			biSectHeaderFooterUseForPage \
					    BIU.biuSect.bsUseForPage
    } BufferSect;

/************************************************************************/
/*									*/
/*  Row specific information.						*/
/*									*/
/*  1)  The rtf file format does not really support tables. Keep an	*/
/*	administration of the set of rows that looks like a table.	*/
/*	First and past refer to similar rows. The header can come	*/
/*	before 'first'.							*/
/*  2)  Properties of the row.						*/
/*  3)  Cells with a rowspan can protrude below the actual row: To	*/
/*	optimize position lookups and to delimit areas to redraw	*/
/*	correctly, the position below all 'rowspan' columns is		*/
/*	remembered.							*/
/*  4)  If a row is a table header, it might be repeated on every page	*/
/*	where the table is continued. When the row is drawn on a	*/
/*	different page. A separate administration must be kept to	*/
/*	handle the case where a row starts on a new page and the header	*/
/*	appears above it. Additionally we need to know on what page the	*/
/*	header apprears in the normal text flow.			*/
/*  6)  The top position of the table header row that appears above an	*/
/*	ordinary row. (This must be kept with ordinary rows at the top	*/
/*	of a page.)							*/
/*  7)  A flag indicating that this row is preceded by a table header	*/
/*	that does not appear at its home position.			*/
/*									*/
/************************************************************************/

typedef struct BufferRow
    {
				/*  1  */
    int				brTableHeaderRow;
    int				brTableFirst;
    int				brTablePast;

				/*  2  */
    RowProperties		brRowProperties;

				/*  3  */
    LayoutPosition		brBelowAllPosition;

				/*  4,6  */
    LayoutPosition		brAboveHeaderPosition;
				/*  4,7  */
    int				brPrecededByHeader;
    } BufferRow;

/************************************************************************/
/*									*/
/*  Levels of nesting.							*/
/*									*/
/************************************************************************/

typedef enum ItemLevel
    {
    DOClevANY,
    DOClevOUT,
			/****************************************/
			/*  Ignore; Garbage values.		*/
			/****************************************/
    DOClevDOC,
			/****************************************/
			/*					*/
			/*  Document.				*/
			/*					*/
			/*  * biGroupChildren			*/
			/*  * biGroupChildCount			*/
			/****************************************/
    DOClevSECT,
			/****************************************/
			/*					*/
			/*  Section.				*/
			/*					*/
			/*  * biGroupChildren			*/
			/*  * biGroupChildCount			*/
			/*					*/
			/****************************************/

#	define	biSectHeader		BIU.biuSect.bsHeader
#	define	biSectFirstPageHeader	BIU.biuSect.bsFirstPageHeader
#	define	biSectLeftPageHeader	BIU.biuSect.bsLeftPageHeader
#	define	biSectRightPageHeader	BIU.biuSect.bsRightPageHeader

#	define	biSectFooter		BIU.biuSect.bsFooter
#	define	biSectFirstPageFooter	BIU.biuSect.bsFirstPageFooter
#	define	biSectLeftPageFooter	BIU.biuSect.bsLeftPageFooter
#	define	biSectRightPageFooter	BIU.biuSect.bsRightPageFooter

#	define	biSectSelectionScope	BIU.biuSect.bsSelectionScope

#	define	biSectProperties	BIU.biuSect.bsProperties

#	define	biSectDocumentGeometry	biSectProperties.spDocumentGeometry
#	define	biSectPageWideTwips	biSectProperties.spPageWideTwips
#	define	biSectPageHighTwips	biSectProperties.spPageHighTwips
#	define	biSectLeftMarginTwips	biSectProperties.spLeftMarginTwips
#	define	biSectTopMarginTwips	biSectProperties.spTopMarginTwips
#	define	biSectRightMarginTwips	biSectProperties.spRightMarginTwips
#	define	biSectBottomMarginTwips	biSectProperties.spBottomMarginTwips
#	define	biSectHeaderYTwips	biSectProperties.spHeaderYTwips
#	define	biSectFooterYTwips	biSectProperties.spFooterYTwips

#	define	biSectHasTitlePage	biSectProperties.spHasTitlePage
#	define	biSectBreakKind		biSectProperties.spBreakKind
#	define	biSectPageNumberStyle	biSectProperties.spPageNumberStyle
#	define	biSectRestartPageNumbers \
					biSectProperties.spRestartPageNumbers
#	define	biSectStartPageNumber	biSectProperties.spStartPageNumber

#	define	biSectParagraphNumbers	biSectProperties.spParagraphNumbers
#	define	biSectParagraphNumberCount \
					biSectProperties.spParagraphNumberCount

			/****************************************/

    DOClevROW,
			/****************************************/
			/*  Row.				*/
			/*  * biGroupChildren			*/
			/*  * biGroupChildCount			*/
			/****************************************/

#	define	biRowTableHeaderRow	BIU.biuRow.brTableHeaderRow
#	define	biRowTableFirst		BIU.biuRow.brTableFirst
#	define	biRowTablePast		BIU.biuRow.brTablePast
#	define	biRowPrecededByHeader	BIU.biuRow.brPrecededByHeader
#	define	biRowBelowAllPosition	BIU.biuRow.brBelowAllPosition
#	define	biRowAboveHeaderPosition \
					BIU.biuRow.brAboveHeaderPosition

#	define	biRowProperties		BIU.biuRow.brRowProperties

#	define	biRowHasTableParagraphs	biRowProperties.rpHasTableParagraphs
#	define	biRowHalfGapWidthTwips	biRowProperties.rpHalfGapWidthTwips
#	define	biRowHalfGapWidthPixels biRowProperties.rpHalfGapWidthPixels
#	define	biRowLeftIndentTwips	biRowProperties.rpLeftIndentTwips
#	define	biRowLeftIndentPixels	biRowProperties.rpLeftIndentPixels
#	define	biRowHeightTwips	biRowProperties.rpHeightTwips
#	define	biRowIsTableHeader	biRowProperties.rpIsTableHeader
#	define	biRowKeepOnOnePage	biRowProperties.rpKeepOnOnePage
#	define	biRowKeepWithNext	biRowProperties.rpKeepWithNext
#	define	biRowAutofit		biRowProperties.rpAutofit
#	define	biRowCells		biRowProperties.rpCells
#	define	biRowCellCount		biRowProperties.rpCellCount
#	define	biRowAlignment		biRowProperties.rpAlignment

#	define	biRowTopBorder		biRowProperties.rpTopBorder
#	define	biRowBottomBorder	biRowProperties.rpBottomBorder
#	define	biRowLeftBorder		biRowProperties.rpLeftBorder
#	define	biRowRightBorder	biRowProperties.rpRightBorder
#	define	biRowVerticalBorder	biRowProperties.rpVerticalBorder
#	define	biRowHorizontalBorder	biRowProperties.rpHorizontalBorder

    DOClevCELL,
			/****************************************/
			/*  Cell.				*/
			/*  * biGroupChildren			*/
			/*  * biGroupChildCount			*/
			/****************************************/
	/* No.. Information is stored with the row.
	define	biCellProperties	BIU.biuGroup.BGU.bguCellProperties
	*/

    DOClevPARA,
			/****************************************/
			/*  Paragraph.				*/
			/****************************************/
#	define	biParaString		BIU.biuPara.btString
#	define	biParaStrlen		BIU.biuPara.btStrlen
#	define	biParaParticuleCount	BIU.biuPara.btParticuleCount
#	define	biParaParticules	BIU.biuPara.btParticules
#	define	biParaObjectCount	BIU.biuPara.btObjectCount
#	define	biParaObjects		BIU.biuPara.btObjects
#	define	biParaLineCount		BIU.biuPara.btLineCount
#	define	biParaLines		BIU.biuPara.btLines

#	define	biParaAscentTwips	BIU.biuPara.btAscentTwips
#	define	biParaDescentTwips	BIU.biuPara.btDescentTwips
#	define	biParaLeadingTwips	BIU.biuPara.btLeadingTwips

#	define	biParaBorderAboveParagraph \
					BIU.biuPara.btBorderAboveParagraph
#	define	biParaBorderBelowParagraph \
					BIU.biuPara.btBorderBelowParagraph

#	define	biParaProperties	BIU.biuPara.btProperties

#	define	biParaInTable		biParaProperties.ppInTable
#	define	biParaLineSpacingIsMultiple \
					biParaProperties.ppLineSpacingIsMultiple
#	define	biParaStartsOnNewPage	biParaProperties.ppStartsOnNewPage
#	define	biParaFirstIndentTwips	biParaProperties.ppFirstIndentTwips
#	define	biParaLeftIndentTwips	biParaProperties.ppLeftIndentTwips
#	define	biParaRightIndentTwips	biParaProperties.ppRightIndentTwips
#	define	biParaSpaceBeforeTwips	biParaProperties.ppSpaceBeforeTwips
#	define	biParaSpaceAfterTwips	biParaProperties.ppSpaceAfterTwips
#	define	biParaLineSpacingTwips	biParaProperties.ppLineSpacingTwips
#	define	biParaAlignment		biParaProperties.ppAlignment
#	define	biParaTabStopList	biParaProperties.ppTabStopList
#	define	biParaKeepOnPage	biParaProperties.ppKeepOnPage
#	define	biParaKeepWithNext	biParaProperties.ppKeepWithNext
#	define	biParaShading		biParaProperties.ppShading
#	define	biParaOutlineLevel	biParaProperties.ppOutlineLevel
#	define	biParaListLevel		biParaProperties.ppListLevel
#	define	biParaTopBorder		biParaProperties.ppTopBorder
#	define	biParaBottomBorder	biParaProperties.ppBottomBorder
#	define	biParaLeftBorder	biParaProperties.ppLeftBorder
#	define	biParaRightBorder	biParaProperties.ppRightBorder
#	define	biParaBetweenBorder	biParaProperties.ppBetweenBorder
#	define	biParaBar		biParaProperties.ppBar
#	define	biParaStyle		biParaProperties.ppStyle
#	define	biParaListOverride	biParaProperties.ppListOverride
#	define	biParaWidowControl	biParaProperties.ppWidowControl

#	define	biParaTabStopCount	biParaTabStopList.tslTabStopCount
#	define	biParaTabStops		biParaTabStopList.tslTabStops
    DOClevTEXT
			/****************************************/
			/*  Handeled inside TEXT.		*/
			/****************************************/
    } ItemLevel;

/************************************************************************/
/*									*/
/*  A BufferItem.							*/
/*									*/
/*  A BufferItem is a node in what we fashionably call the document	*/
/*  object model.							*/
/*									*/
/*  1)  Fields for managing the hierarchy.				*/
/*  2)  Geometry information that is set by the formatter.		*/
/*	The insets are an amount of space that the item claims for	*/
/*	itself. The content of the item (children, or lines) appears	*/
/*	between the insets. Typically, the insets are used for spacing	*/
/*	or borders.							*/
/*  3)  Information that is specific to the kind of item.		*/
/*									*/
/************************************************************************/

typedef struct BufferItem
    {
				/*  1  */
    short int			biLevel;
    unsigned char		biInExternalItem;
    struct BufferItem *		biParent;
    struct BufferItem **	biChildren;
    short int			biChildCount;
    short int			biNumberInParent;
    int				biLeftParagraphs;

				/*  2  */
    LayoutPosition		biTopPosition;
    LayoutPosition		biBelowPosition;
    int				biTopInsetTwips;
    int				biBottomInsetTwips;

				/*  3  */
    union			BufferItemUnion
	{
	BufferPara		biuPara;
	BufferSect		biuSect;
	BufferRow		biuRow;
	} BIU;

    } BufferItem;

/************************************************************************/
/*									*/
/*  The document as a whole.						*/
/*									*/
/************************************************************************/

typedef struct BufferDocument
    {
    TextAttributeList		bdTextAttributeList;

    BufferItem			bdItem;
    DocumentProperties		bdProperties;

    DocumentStyleSheet		bdStyleSheet;
    DocumentFieldList		bdFieldList;

    DocumentNote *		bdNotes;
    int				bdNoteCount;

    ListNumberTreeNode *	bdListNumberTrees;
    int				bdListNumberTreeCount;

    ExternalItem		bdEiFtnsep;
    ExternalItem		bdEiFtnsepc;
    ExternalItem		bdEiFtncn;

    ExternalItem		bdEiAftnsep;
    ExternalItem		bdEiAftnsepc;
    ExternalItem		bdEiAftncn;
    } BufferDocument;

/************************************************************************/
/*									*/
/*  Statistics about a document. Used in the 'Document Properties'	*/
/*  dialog.								*/
/*									*/
/************************************************************************/

typedef struct DocumentStatistics
    {
    int		dsPageCount;
    int		dsParagraphCount;
    int		dsLineCount;
    int		dsWordCount;
    int		dsCharacterCount;
    } DocumentStatistics;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern BufferDocument *	docPlainReadFile( SimpleInputStream *	sis,
				    int *			pMxL,
				    int				docCharset,
				    int				ansiCodepage,
				    const DocumentGeometry *	dg );

extern BufferDocument *	docRtfReadFile(
				SimpleInputStream *		sis,
				const PostScriptFontList *	psfl,
				int				defAnsigpg );

extern BufferDocument * docNewFile(
				TextAttribute *			taDefault,
				const char *			defaultFont,
				int				docCharset,
				int				ansiCodepage,
				const PostScriptFontList *	psfl,
				const DocumentGeometry * 	dg );

extern int docRtfSaveDocument(	SimpleOutputStream *		sos,
				BufferDocument *		bd,
				const DocumentSelection *	bs,
				int				saveBookmarks );

extern int docRtfSaveSelectionAsLink(
				SimpleOutputStream *		sos,
				const BufferDocument *		bd,
				const DocumentSelection *	ds,
				int				asRef,
				int				asPageref,
				const unsigned char *		fileName,
				int				fileSize,
				const unsigned char *		markName,
				int				markSize );

extern int docHtmlSaveDocument(		SimpleOutputStream *	sos,
					BufferDocument *	bd,
					int			asMimeAggr,
					const char *		mimeBoundary,
					const char *		filename );

extern int docPlainSaveDocument(SimpleOutputStream *		sos,
				BufferDocument *		bd,
				const DocumentSelection *	ds,
				int				fold,
				int				closed );

extern int docInflateTextString(	BufferItem *    paraBi,
					int		by );

extern void docInitTextLine(		TextLine *	tl );

extern TextLine * docInsertTextLine(	BufferItem *	bi,
					int		line );

extern void docCleanItem(	BufferDocument *	bd,
				BufferItem *		bi );

extern void docFreeItem(	BufferDocument *	bd,
				BufferItem *		bi );

extern void docFreeDocument( BufferDocument *	bd );

extern void docInitDocument(	BufferDocument *	bd	);

extern BufferItem * docInsertItem(	const BufferDocument *	bd,
					BufferItem *		parent,
					int			n,
					ItemLevel		level );

extern void docDeleteItems(		BufferDocument *	bd,
					BufferItem *		bi,
					int			first,
					int			count );

extern TextParticule *	docInsertTextParticule(	BufferItem *	bi,
						int		n,
						int		off,
						int		len,
						int		kind,
						int		textAttrNr );

extern TextParticule * docCopyParticule(	BufferItem *		bi,
						int			n,
						int			off,
						int			len,
						int			kind,
						const TextParticule *	from );

extern int docSplitTextParticule(
				TextParticule **		pTpPart,
				TextParticule **		pTpNext,
				BufferItem *			paraBi,
				int				part,
				int				stroff );

extern void docDeleteParticules(	BufferItem *	bi,
					int		first,
					int		count );

extern void docDeleteLines(		BufferItem *	bi,
					int		first,
					int		count );

extern void docInitSelectionScope(	SelectionScope *	ss );
extern void docInitTableRectangle(	TableRectangle *	tr );

extern int docSplitGroupItem(	const BufferDocument *	bd,
				BufferItem **		pNewBi,
				BufferItem **		pOldBi,
				BufferItem *		oldBi,
				int			n,
				int			level );

extern int docMergeGroupItems(		BufferItem *	to,
					BufferItem *	from );

extern BufferItem * docNextParagraph(	BufferItem *	bi );
extern BufferItem * docPrevParagraph(	BufferItem *	bi );

extern BufferItem * docNextSection(	BufferItem *	bi );
extern BufferItem * docPrevSection(	BufferItem *	bi );

extern int docNextPosition(	DocumentPosition *	dp );

extern int docPrevPosition(	DocumentPosition *	dp );

extern int docNextWord(		DocumentPosition *	dp );
extern int docPrevWord(		DocumentPosition *	dp );

extern int docParaBegin(	DocumentPosition *	dp );
extern int docParaEnd(		DocumentPosition *	dp );

extern int docLineDown(		TextLine **		pTl,
				DocumentPosition *	dp,
				int			line );

extern int docLineUp(		TextLine **		pTl,
				DocumentPosition *	dp,
				int			line );

extern int docFirstPosition(	DocumentPosition *	dp,
				BufferItem *		bi );

extern int docLastPosition(	DocumentPosition *	dp,
				BufferItem *		bi );

extern int docFirstDocumentPosition(	DocumentPosition *	dp,
					BufferDocument *	bd );

extern int docLastDocumentPosition(	DocumentPosition *	dp,
					BufferDocument *	bd );

extern void docSetParaSelection( DocumentSelection *	ds,
				BufferItem *		bi,
				int			direction,
				int			start,
				int			length );

extern int docComparePositions(	const DocumentPosition *	dp1,
				const DocumentPosition *	dp2 );

extern int docCompareItemPositions(	const BufferItem *	bi1,
					const BufferItem *	bi2 );

extern int docSetDocumentPosition(	DocumentPosition *	dp,
					BufferItem *		bi,
					int			stroff );

extern int docFindParticule(		int *			pPart,
					const BufferItem *	bi,
					int			stroff,
					int			lastOne );

extern int docFindParticuleOfPosition(
				int *				pPart,
				const DocumentPosition *	dp,
				int				lastOne );

extern int docFindLineOfPosition(
				int *				pLine,
				const DocumentPosition *	dp,
				int				lastOne );

extern void docDeleteItem(		BufferDocument *	bd,
					BufferItem *		bi );

extern void docListItem(	int			indent,
				const BufferItem *	bi );
extern void docListRootItem(	int			indent,
				const BufferItem *	bi );

extern int docCheckItem(	const BufferItem *	bi );
extern int docCheckRootItem(	const BufferItem *	bi );

extern void docListParticule(	int			indent,
				const char *		label,
				int			n,
				const BufferItem *	bi,
				const TextParticule *	tp );

extern void docListTextLine(	int			indent,
				const char *		label,
				int			n,
				const BufferItem *	bi,
				const TextLine *	tl );

extern void docInitItem(	BufferItem *		bi,
				BufferItem *		parent,
				const BufferDocument *	bd,
				int			numberInParent,
				int			level,
				int			inExternalItem );

extern const char * docKindStr(		int			kind );
extern const char * docLevelStr(	int			level );
extern const char * docExternalKindStr(	int			level );
extern const char * docFieldKindStr(	int			kind );
extern const char * docObjectKindStr(	int			kind );
extern const char * docAttributeStr(	const TextAttribute *	ta );

extern void docListNotes(	const BufferDocument *	bd );

extern TextParticule *	docInsertObject(	BufferDocument *	bd,
						BufferItem *		paraBi,
						InsertedObject **	pIo,
						int			n,
						int			off,
						const TextAttribute *	ta );

extern void docCleanParticuleObject(	BufferDocument *	bd,
					BufferItem *		bi,
					TextParticule *		tp );

extern InsertedObject *	docClaimObject(	int *			pNr,
					BufferItem *		bi );

extern InsertedObject * docClaimObjectCopy(
					BufferDocument *	bd,
					BufferItem *		bi,
					int *			pNr,
					const InsertedObject *	ioFrom );

extern DocumentField * docClaimField(	int *			pNr,
					DocumentFieldList *	dfl );

extern int docSetFieldInst(	DocumentField *		df,
				const unsigned char *	bytes,
				int			size );

extern int docAddToFieldInst(	DocumentField *		df,
				const unsigned char *	bytes,
				int			size );

extern int docDelimitTable(	BufferItem *		paraBi,
				BufferItem **		pSectBi,
				int *			pCol,
				int *			pRow0,
				int *			pRow,
				int *			pRow1 );

extern void docTableDetermineCellspans(	int *			pRowspan,
					int *			pColspan,
					const BufferItem *	rowBi,
					int			col );

extern int docGetBookmarkForPosition(
				const BufferDocument *		bd,
				const DocumentPosition *	dp,
				DocumentSelection *		dsBookmark,
				int *				pPart,
				int *				pEndPart,
				const char **			pMarkName,
				int *				pMarkSize );

extern int docGetHyperlinkForPosition(
				const BufferDocument *		bd,
				const DocumentPosition *	dp,
				DocumentSelection *		dsHyperlink,
				int *				pStartPart,
				int *				pEndPart,
				const char **			pFileName,
				int *				pFileSize,
				const char **			pMarkName,
				int *				pMarkSize );

extern int docPositionInField(		int *				pPart,
					const DocumentPosition *	dp,
					const BufferDocument *		bd );

extern int docSaveParticules(	BufferDocument *	bd,
				BufferItem *		paraBi,
				const TextAttribute *	ta,
				const unsigned char *	map,
				const unsigned char *	text,
				int			len );

extern int docSaveSpecialParticule(	BufferDocument *	bd,
					BufferItem *		paraBi,
					int			atBegin,
					const TextAttribute *	ta,
					int			kind );

extern void docLogRectangle(	const char *			label,
				const DocumentRectangle *	dr );

extern void docLogRectangles(	const char *			label1,
				const DocumentRectangle *	dr1,
				const char *			label2,
				const DocumentRectangle *	dr2 );

extern void docLineSelection(	DocumentSelection *	dsLine,
				int *			pPartLineBegin,
				int *			pPartLineEnd,
				const BufferItem *	bi,
				int			line );

extern void docWordSelection(
			DocumentSelection *		dsWord,
			int *				pIsObject,
			const DocumentPosition *	dpAround,
			int				partAround );

extern void docInitDocumentStatistics(		DocumentStatistics *	ds );

extern void docCollectDocumentStatistics(	DocumentStatistics *	ds,
						const BufferDocument *	bd );

extern int docWhatPageHeader(	ExternalItem **			pEi,
				int *				pIsEmpty,
				BufferItem *			sectBi,
				int				page,
				const DocumentProperties *	dp );

extern int docWhatPageFooter(	ExternalItem **			pEi,
				int *				pIsEmpty,
				BufferItem *			sectBi,
				int				page,
				const DocumentProperties *	dp );

extern BufferItem * docGetSelectionRoot(
				ExternalItem **			pEi,
				BufferItem **			pBodySectBi,
				BufferDocument *		bd,
				const DocumentSelection *	ds );

extern void docInitParagraphNumber(	ParagraphNumber *	pn );

extern int docRecalculateTextLevelFields(	RecalculateFields *	rf,
						BufferItem *		bi );

extern int docRecalculateTextLevelFieldsInExternalItem(
					RecalculateFields *	rf,
					ExternalItem *		ei,
					const BufferItem *	sectBi,
					int			page );

extern int docRedivideStringInParticules(
					BufferItem *	bi,
					int		strOff,
					int		strLen,
					int		part,
					int		partsFree,
					int		textAttributeNumber );

extern int docSplitFieldInstructions(	const MemoryBuffer *	mb,
				FieldInstructionsComponent *	fic,
				int				count );

extern int docGetHeaderFooter(	ExternalItem **			pEi,
				BufferItem **			pBodySectBi,
				const DocumentSelection *	ds,
				BufferDocument *		bd,
				int				which );

extern int docInquireHeadersFooters(
				int *				pDocHasHeaders,
				int *				pDocHasFooters,
				const BufferItem *		docBi );

extern BufferItem * docMakeExternalItem(
				BufferDocument *		bd,
				const SelectionScope *		ss,
				const SectionProperties *	sp );

extern BufferItem * docMakeExternalparagraph(
				BufferDocument *		bd,
				ExternalItem *			ei,
				const BufferItem *		bodyBi,
				int				txtAttrNr,
				int				noteIndex,
				int				inExtIt );

extern BufferItem * docInsertEmptyParagraph(
				BufferDocument *	bd,
				BufferItem *		bi,
				int			textAttributeNumber );

extern int docIntersectTableRectangle(	TableRectangle *		tr,
					const TableRectangle *		tr1,
					const TableRectangle *		tr2 );

extern void docExpandTableRectangleToWholeTable( TableRectangle *	tr );
extern void docExpandTableRectangleToWholeRows(	TableRectangle *	tr );
extern void docExpandTableRectangleToWholeColumns( TableRectangle *	tr );

extern int docShiftTableRectangleByRows( TableRectangle *	tr,
					int			rows );
extern int docShiftTableRectangleByColumns( TableRectangle *	tr,
					int			columns );

extern int docTableRectangleSelection(	DocumentSelection *	ds,
					BufferItem **		pSelSectBi,
					BufferDocument *	bd,
					const TableRectangle *	tr );

extern int docMergeDocumentLists(
				int **				pFontMap,
				int **				pColorMap,
				int **				pListStyleMap,
				BufferDocument *		bdTo,
				BufferDocument *		bdFrom,
				const PostScriptFontList *	psfl );

extern int docFindBookmarkInDocument(	DocumentSelection *	ds,
					BufferDocument *	bd,
					const char *		markName,
					int			markSize );

extern int docSurroundTextSelectionByField(
				DocumentField **		pDf,
				BufferDocument *		bd,
				int *				pStartPart,
				int *				pEndPart,
				const DocumentSelection *	ds,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet );

extern void docConstrainSelectionToOneParagraph( int *		pBeginMoved,
					int *			pEndMoved,
					DocumentSelection *	ds );

extern void docUnionParaSelections(	DocumentSelection *		ds,
					const DocumentSelection *	ds1,
					const DocumentSelection *	ds2 );

extern int * docAllocateFieldMap(	const BufferDocument *	bdFrom );

extern int docParaStringReplace(	int *			pStroffShift,
					BufferItem *		bi,
					int			stroffBegin,
					int			stroffEnd,
					const unsigned char *	addedString,
					int			addedStrlen );

extern int docInsertNote(		DocumentNote **		pDn,
					BufferDocument *	bd,
					BufferItem *		paraBi,
					int			stroff,
					int			autoNumber );

extern int docGetNote(			DocumentNote **		pDn,
					const BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			stroff );

extern int docMakeNote(	DocumentNote **			pDn,
			BufferDocument *		bd,
			BufferItem *			paraBi,
			int				part,
			int				stroff,
			int				extItKind );

int docGetSelectedNote(		DocumentNote **			pDn,
				int *				pFieldNr,
				unsigned char *			fixedText,
				int				fixedTextSize,
				BufferDocument *		bd,
				const DocumentSelection *	ds );

extern void docNoteGetTextAtHead(	unsigned char *		fixedText,
					int *			pFixedLen,
					int			fixedTextSize,
					const BufferItem *	paraBi );

extern void docNoteGetTextBefore(	unsigned char *		fixedText,
					int *			pFixedLen,
					int *			pTextAttrNr,
					int			fixedTextSize,
					const BufferItem *	paraBi,
					int			stroff );

extern int docDelimitNoteReference(
				int *			pFieldNr,
				int *			pPartBegin,
				int *			pPartEnd,
				int *			pStroffBegin,
				int *			pStroffEnd,
				const BufferItem *	paraBi,
				int			stroff,
				const BufferDocument *	bd );

extern void docRenumberNotes(		int *			pChanged,
					BufferDocument *	bd );

extern ExternalItem *	docSectionHeaderFooter(	BufferItem *	sectBi,
						int		which );

ExternalItem *	docDocumentNoteSeparator(
					BufferDocument *	bd,
					int			which );

extern int docGetExternalItem(		ExternalItem **		pEi,
					BufferItem **		pBodySectBi,
					BufferDocument *	bd,
					BufferItem *		bi );

extern int docGetRootOfSelectionScope(	ExternalItem **		pEi,
					BufferItem **		pSelRootBi,
					BufferItem **		pBodySectBi,
					BufferDocument *	bd,
					const SelectionScope *	ss );

extern int docGetTopOfColumn(	DocumentPosition *		dp,
				int *				pLineTop,
				int *				pPartTop,
				BufferDocument *		bd,
				int				page,
				int				column );

extern int docGetFirstInColumnForItem(
				DocumentPosition *		dp,
				int *				pLineTop,
				int *				pPartTop,
				BufferItem *			bi,
				int				page,
				int				column );

extern int docGetBottomOfColumn(DocumentPosition *		dp,
				int *				pPartBot,
				BufferDocument *		bd,
				int				page,
				int				column );

extern int docGetLastInColumnForItem(
				DocumentPosition *		dp,
				int *				pLineBot,
				int *				pPartBot,
				BufferItem *			bi,
				int				page,
				int				column );

extern BufferItem * docInsertRowItem(
				BufferDocument *	bd,
				BufferItem *		sectBi,
				int			n,
				const RowProperties *	rp,
				int			textAttributeNumber );

extern int docCopyRowColumnAttributes(	BufferItem *		rowBi,
					const BufferItem *	refRowBi );

extern void docSetSelectionScope(	DocumentSelection *	ds,
					const BufferItem *	bi );

extern int docSelectionSameRoot(
				const DocumentSelection *	dsFrom,
				const BufferItem *		biTo );

extern int docSelectionSameScope(
				const SelectionScope *		ssFrom,
				const SelectionScope *		ssTo );

extern int docSectionHeaderFooterFirstPage(
				int *				pExists,
				const BufferItem *		bodySectBi,
				int				which,
				const DocumentProperties *	dp );

extern int docSelectWholeSection(	DocumentSelection *	bs,
					BufferDocument *	bd,
					int			direction );

extern int docSelectAll(		DocumentSelection *	ds,
					BufferDocument *	bd );

extern int docNumberOfParagraph(	const BufferItem *	paraBi );

extern BufferItem * docGetParagraphByNumber(	BufferItem *	bi,
						int		n );

extern int docShiftParticuleOffsets(	BufferDocument *	bd,
					BufferItem *		paraBi,
					int			partFrom,
					int			partUpto,
					int			stroffShift );

extern void docShiftNoteReferences(	BufferDocument *	bd,
					int			paraFrom,
					int			stroffFrom,
					int			sectShift,
					int			paraShift,
					int			stroffShift );

extern void docInitNote(		DocumentNote *		dn );

extern void docCleanNote(		BufferDocument *	bd,
					DocumentNote *		dn );

extern int docDeleteNoteOfParticule(	BufferDocument *	bd,
					const BufferItem *	paraBi,
					const TextParticule *	tp );

extern int docCountNotes(		const BufferDocument *	bd );

extern int docGetFirstNoteFromPage(	DocumentNote **		pDn,
					const BufferDocument *	bd,
					int			page,
					int			extItKind );

extern int docGetFirstNoteOnPage(	DocumentNote **		pDn,
					const BufferDocument *	bd,
					int			page,
					int			extItKind );

extern int docGetFirstNoteOfSection(	DocumentNote **		pDn,
					const BufferDocument *	bd,
					int			sect,
					int			extItKind );

extern int docGetFirstNoteOfDocument(	DocumentNote **		pDn,
					const BufferDocument *	bd,
					int			extItKind );

extern void docSetExternalItemKind(	BufferItem *		bi,
					int			extItKind );

extern int docCheckNoteSeparatorItem(	BufferDocument *	bd,
					int			extItKind );

extern int docCountParticulesInField(	const BufferItem *	bi,
					int			part,
					int			partUpto );

extern int docGetSolidRgbShadeOfItem(	int *			pIsSolid,
					int *			pRed,
					int *			pGreen,
					int *			pBlue,
					const BufferDocument *	bd,
					const ItemShading *	is );

extern void docSectDelimitTables(	BufferItem *		sectBi );

extern void docGetCellTopBorder(
			const BorderProperties **	pBpTop,
			int *				pUseAbove,
			const BufferItem *		rowBi,
			int				col,
			int				atTopOfPage );

extern void docGetCellBottomBorder(
			const BorderProperties **	pBpButtom,
			int *				pUseBelow,
			const BufferItem *		rowBi,
			int				col,
			int				atBottomOfPage );

extern int docTextAttributeNumber(	BufferDocument *	bd,
					const TextAttribute *	ta );

extern int docInsertAdminParticule(	BufferDocument *	bd,
					BufferItem *		paraBi,
					int			off,
					int			n,
					int			objectNumber,
					const TextAttribute *	ta,
					int			kind );

extern int docBalanceFieldSelection(	int *			pBeginMoved,
					int *			pEndMoved,
					const BufferDocument *	bd,
					DocumentPosition *	dpBegin,
					DocumentPosition *	dpEnd );

extern int docGetListOfParagraph(	ListOverride **		pLo,
					ListNumberTreeNode **	pRoot,
					DocumentList **		pDl,
					int			ls,
					const BufferDocument *	bd );

extern int docGetListOfOverride(	ListOverride *			lo,
					const DocumentListTable *	dlt );

extern int docListGetFormatPath(int *				startPath,
				int *				formatPath,
				const DocumentListLevel **	pDll,
				int				ilvl,
				const DocumentList *		dl,
				const ListOverride *		lo );

extern int docInsertListtextField(
				int *			pFieldNr,
				int *			pPartBegin,
				int *			pPartEnd,
				int *			pStroffBegin,
				int *			pStroffEnd,
				BufferItem *		paraBi,
				BufferDocument *	bd );

extern int docRemoveParagraphFromList(	BufferItem *		paraBi,
					BufferDocument *	bd );

extern int docParaHeadFieldKind(	const BufferItem *	paraBi,
					const BufferDocument *	bd );

extern int docDelimitParaHeadField(
				int *			pFieldNr,
				int *			pPartBegin,
				int *			pPartEnd,
				int *			pStroffBegin,
				int *			pStroffEnd,
				const BufferItem *	paraBi,
				const BufferDocument *	bd );

extern int docFindListOfSelection(
				int *				pLs,
				int *				pLevel,
				int *				pMultiList,
				int *				pMultiLevel,
				int *				pParaNr,
				const DocumentSelection *	ds );

extern int docApplyListRuler(	const ListNumberTreeNode *	root,
				const DocumentList *		dl,
				const ListOverride *		lo,
				BufferDocument *		bd );

extern void docGetSelectionAttributes(
				BufferDocument *		bd,
				const DocumentSelection *	ds,
				PropertyMask *			pUpdMask,
				TextAttribute *			pTaNew );

extern int docInsertParaHeadField(
				int *			pFieldNr,
				int *			pPartBegin,
				int *			pPartEnd,
				int *			pStroffBegin,
				int *			pStroffEnd,
				BufferItem *		paraBi,
				BufferDocument *	bd,
				int			fieldKind,
				int			textAttrNr );

extern int docGetNotePosition(	DocumentPosition *		dp,
				BufferDocument *		bd,
				const DocumentNote *		dn );

extern int docNextSimilarRoot(	DocumentPosition *		dp,
				int *				pPage,
				BufferDocument *		bd );

extern int docPrevSimilarRoot(	DocumentPosition *		dp,
				int *				pPage,
				BufferDocument *		bd );

extern int docNewList(		int *				pLs,
				BufferDocument *		bd,
				const PropertyMask *		taSetMask,
				const TextAttribute *		taSet );

extern int docGetBitmap(	bmReadBitmap			readBitmap,
				void **				pPrivate,
				const MemoryBuffer *		mb );

extern int docAdaptParagraphToListLevel(
				int *				pChanged,
				BufferItem *			paraBi,
				const BufferDocument *		bd );

extern void docInvalidateParagraphLayout(	BufferItem *	paraBi );

#   endif
