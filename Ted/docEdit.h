/************************************************************************/
/*									*/
/*  Document Editing.							*/
/*									*/
/************************************************************************/

#   ifndef		DOC_EDIT_H
#   define		DOC_EDIT_H

#   include		"docEditPosition.h"
#   include		"docEditRange.h"
#   include		"docScreenFontList.h"
#   include		"docBuf.h"

/************************************************************************/
/*									*/
/*  Describes an edit operation. Part is only used for interactive edit	*/
/*  operations.								*/
/*									*/
/*  A)  Indentation level of the bottom item that contains all items	*/
/*	in the selection.						*/
/*									*/
/*  1)  Current insertion position.					*/
/*									*/
/************************************************************************/

typedef struct EditOperation
    {
    SelectionScope	eoSelectionScope;

    DocumentRectangle	eoChangedRect;
    int			eoChangedRectSet;
    int			eoOldBackY1;

    int			eoIBarSelectionOld;
    int			eoMultiParagraphSelectionOld;

    /**/
    int			eoParaAdjustParagraphNumber;
    int			eoParaAdjustFromLine;
    int			eoParaAdjustStroffShift;
    int			eoParaAdjustStroffUpto;

    /**/
    EditRange		eoReformatRange;
    EditRange		eoSelectedRange;

    /**/
    TextAttribute	eoSavedTextAttribute;
    int			eoSavedTextAttributeNumber;

    /**/
    int			eoNotesDeleted;
    int			eoNotesAdded;
    int			eoSectionsDeleted;
    int			eoSectionsAdded;
    int			eoBulletsChanged;
    int			eoParagraphsInserted;

    unsigned int	eoFieldUpdate;

    /**/
    BufferDocument *	eoBd;
    ScreenFontList *	eoScreenFontList;
    void *		eoVoidadd;
    DOC_CLOSE_OBJECT	eoCloseObject;

    /*  1  */
    BufferItem *	eoInsParaBi;
    int			eoInsParticule;
    int			eoInsAtPartHead;
    int			eoInsStroff;
    } EditOperation;

/************************************************************************/
/*									*/
/*  For copying information from one location to another. Possibly	*/
/*  between different documents.					*/
/*									*/
/************************************************************************/

typedef struct DocumentCopyJob
    {
    BufferDocument *	dcjBdTo;
    BufferDocument *	dcjBdFrom;
    int *		dcjFieldMap;
    int *		dcjFontMap;
    int *		dcjColorMap;
    int *		dcjListStyleMap;
    char *		dcjRefFileName;
    int			dcjRefFileSize;

    int			dcjCurrentTextAttributeNumberFrom;
    int			dcjCurrentTextAttributeNumberTo;

    int			dcjNotesCopied;
    int			dcjBulletsCopied;
    } DocumentCopyJob;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int docParaReplaceText(	EditOperation *		eo,
				BufferItem *		paraBi,
				int			paraNr,
				unsigned int		stroffBegin,
				int *			pPartShift,
				int *			pStroffShift,
				unsigned int		stroffEnd,
				const unsigned char *	addedText,
				unsigned int		addedLength,
				int			addedAttributeNumber );

extern int docSplitParaItemAtStroff(
				EditOperation *			eo,
				BufferItem **			pNewBi,
				const DocumentPosition *	dp,
				int				splitLevel );

extern int docSplitParaItemAtPart(
				EditOperation *		eo,
				BufferItem **		pNewBi,
				BufferItem *		oldBi,
				int			part,
				int			splitLevel );

extern int docRemoveSelectionTail(
				EditOperation *			eo,
				const DocumentSelection *	ds );

extern void docEditDeleteItems(	EditOperation *		eo,
				int *			pSectionsDeleted,
				int *			pFirstParaDeleted,
				int *			pParagraphsDeleted,
				BufferItem *		bi,
				int			first,
				int			count );

extern void docCleanItemObjects(	int *			pNoteCount,
					int *			pBulletsDeleted,
					int *			pParagraphCount,
					BufferDocument *	bd,
					BufferItem *		bi,
					void *			voidadd,
					DOC_CLOSE_OBJECT	closeObject );

extern TextParticule * docParaSpecialParticule(
					EditOperation *		eo,
					BufferItem *		bi,
					int			kind,
					int			part,
					int			stroff,
					int *			pPartShift,
					int *			pStroffShift );

extern int docReplaceSelection(
			EditOperation *			eo,
			const DocumentSelection *	bs,
			int *				pPartShift,
			int *				pStroffShift,
			const unsigned char *		addedText,
			int				addedLength,
			int				addedAttributeNumber );

extern void docInitEditOperation(	EditOperation *	eo );
extern void docInitEditRange(		EditRange *	er );

extern void docIncludePositionInReformat(
					EditOperation *		eo,
					const BufferItem *	paraBi,
					int			stroff );

extern void docEditIncludeItemInReformatRange(	EditOperation *		eo,
						BufferItem *		bi );

extern void docEditIncludeRowsInReformatRange(
					EditOperation *		eo,
					BufferItem *		sectBi,
					int			row0,
					int			row1 );

extern void docSetParagraphAdjust(	EditOperation *		eo,
					BufferItem *		paraBi,
					int			fromLine,
					int			stroffShift,
					int			stroffUpto );

extern void docExtendParagraphAdjust(	EditOperation *		eo,
					BufferItem *		paraBi,
					int			stroffShift );

extern int docEditMakeParagraphEmpty(	BufferItem *		paraBi,
					EditOperation *		eo );

extern int docChangeParticuleAttributes(PropertyMask *		pTaAllMask,
					BufferDocument *	bd,
					BufferItem *		bi,
					int			part,
					int			partUpto,
					const TextAttribute *	taSet,
					const PropertyMask *	taSetMask );

extern int docMapTextAttributeNumber(
				DocumentCopyJob *	dcj,
				int			attributeNumberFrom );

extern BufferItem * docCopyParaItem(	DocumentCopyJob *	dcj,
					EditOperation *		eo,
					const SelectionScope *	ssRoot,
					BufferItem *		biCellTo,
					int			n,
					BufferItem *		biParaFrom,
					int			inTable );

extern BufferItem * docCopyRowItem(	DocumentCopyJob *	dcj,
					EditOperation *		eo,
					const SelectionScope *	ssRoot,
					BufferItem *		biSectTo,
					int			n,
					BufferItem *		biRowFrom,
					int			inTable );

extern BufferItem * docCopySectItem(	DocumentCopyJob *	dcj,
					EditOperation *		eo,
					const SelectionScope *	ssRoot,
					BufferItem *		biParentTo,
					int			n,
					BufferItem *		biSectFrom );

extern int docCopyParticules(	DocumentCopyJob *	dcj,
				EditOperation *		eo,
				BufferItem *		biTo,
				const BufferItem *	biFrom,
				int			partTo,
				int			partFrom,
				int			countFrom,
				int *			pParticulesInserted,
				int *			pCharactersCopied );

extern int docCopyParticuleData(	DocumentCopyJob *	dcj,
					EditOperation *		eo,
					BufferItem *		biTo,
					const BufferItem *	biFrom,
					TextParticule *		tpTo,
					const TextParticule *	tpFrom );

extern void docInitDocumentCopyJob(	DocumentCopyJob *	dcj );
extern void docCleanDocumentCopyJob(	DocumentCopyJob *	dcj );

extern int docSet1DocumentCopyJob(	DocumentCopyJob *	dcj,
					BufferDocument *	bd );

extern int docSet2DocumentCopyJob(
				DocumentCopyJob *		dcj,
				BufferDocument *		bdTo,
				BufferDocument *		bdFrom,
				const PostScriptFontList *	psfl,
				const char *			refFileName );

extern int docCopyFieldRelative(	DocumentCopyJob *	dcj,
					EditOperation *		eo,
					BufferItem *		biTo,
					const BufferItem *	biFrom,
					TextParticule *		tpTo,
					const TextParticule *	tpFrom );

extern int docCopyParticuleAndData(	TextParticule **	pTpTo,
					DocumentCopyJob *	dcj,
					EditOperation *		eo,
					BufferItem *		paraBiTo,
					int			partTo,
					int			stroffTo,
					int			strlenTo,
					const BufferItem *	paraBiFrom,
					const TextParticule *	tpFrom );

extern int docInsertTableRows(	BufferItem *		sectBi,
				BufferItem **		pNewBi,
				const BufferItem *	refRowBi,
				const RowProperties *	rp,
				int			textAttributeNumber,
				int			n,
				int			paraNr,
				int			rows,
				EditOperation *		eo );

extern int docSplitColumnInRows(	BufferItem **	pNewParaBi,
					EditOperation *	eo,
					BufferItem *	sectBi,
					int		row0,
					int		row,
					int		row1,
					int		col,
					int		after );

extern void docChangeTableLayout(	EditOperation *		eo,
					BufferItem *		sectBi,
					int			row0,
					int			row1,
					int			col0,
					int			col1,
					const PropertyMask *	rpSetMask,
					const PropertyMask *	cpSetMask,
					const RowProperties *	rpSet );

extern int docDeleteColumnsFromRows(	EditOperation *		eo,
					BufferItem *		sectBi,
					int			delRow0,
					int			delRow1,
					int			delCol0,
					int			delCol1 );

extern int docChangeCellSpans(	int *			pRowChanged1,
				BufferItem *		sectBi,
				int			row0,
				int			col0,
				int			rowspan,
				int			colspan );

extern void docEditShiftReferences(	EditOperation *		eo,
					const SelectionScope *	ssRoot,
					int			paraFrom,
					int			stroffFrom,
					int			sectShift,
					int			paraShift,
					int			stroffShift );

extern void docIncludeRectangleInChange( EditOperation *		eo,
					const DocumentRectangle *	dr );

extern int docEditUpdParaProperties(
				EditOperation *			eo,
				PropertyMask *			pPpChgPask,
				BufferItem *			paraBi,
				const PropertyMask *		ppUpdMask,
				const ParagraphProperties *	ppNew,
				const int *			colorMap,
				const int *			listStyleMap );

extern int docEditShiftParticuleOffsets( EditOperation *	eo,
					BufferItem *		paraBi,
					int			paraNr,
					int			partFrom,
					int			partUpto,
					int			stroffFrom,
					int			stroffShift );

extern int docCopyNote(			DocumentCopyJob *	dcj,
					EditOperation *		eo,
					BufferItem *		biTo,
					TextParticule *		tpTo,
					const BufferItem *	biFrom,
					const TextParticule *	tpFrom );

extern DrawingShape * docCopyDrawingShape(
					DocumentCopyJob *	dcj,
					EditOperation *		eo,
					const DrawingShape *	from );

extern int docCheckNoBreakAsLast(	EditOperation *		eo,
					BufferItem *		paraBi );

#   endif	/*	DOC_EDIT_H	*/
