/************************************************************************/
/*									*/
/*  Ted: data structures and routines for on screen interactive		*/
/*  editing.								*/
/*									*/
/************************************************************************/

#   ifndef		TED_EDIT_H
#   define		TED_EDIT_H

#   include	"tedApp.h"
#   include	"docEdit.h"

/************************************************************************/
/*									*/
/*  Routine Declarations:						*/
/*									*/
/************************************************************************/

extern int tedEditRefreshLayout(	EditOperation *		eo,
					EditDocument *		ed );

extern int tedAdjustParagraphLayout(
				EditOperation *			eo,
				AppDrawingData *		add,
				BufferItem *			bi,
				int				line,
				int				stroffShift,
				int				upto );

extern int tedStartEditOperation(	EditOperation *		eo,
					DocumentSelection *	ds,
					SelectionGeometry *	sg,
					SelectionDescription *	sd,
					EditDocument *		ed,
					int			fullWidth );

extern int tedEditFinishIBarSelection(	EditDocument *		ed,
					EditOperation *		eo,
					BufferItem *		bi,
					int			stroff );

extern int tedEditFinishSelection(
				EditDocument *			ed,
				EditOperation *			eo,
				const DocumentSelection *	dsNew );

extern int tedEditFinishOldSelection(
				EditDocument *			ed,
				EditOperation *			eo );

extern int tedEditFinishEndOfInsert(
				EditDocument *			ed,
				EditOperation *			eo,
				int				lastLine );

extern void tedAdjustRedrawBegin(	EditDocument *		ed,
					EditOperation *		eo,
					int *			pLine );

extern int tedEditReplaceSelection(	EditOperation *		eo,
					DocumentSelection *	dsRep,
					EditDocument *		ed,
					const unsigned char *	addedText,
					int			addedLength );

extern int tedSplitParaContents(EditOperation *			eo,
				BufferItem **			pNewBi,
				const DocumentPosition *	dp,
				EditDocument *			ed,
				int				splitLevel,
				int				onNewPage );

extern int tedEditIncludeItemInRedraw(	EditOperation *		eo,
					EditDocument *		ed,
					const BufferItem *	bi );

extern int tedEditIncludeRowsInRedraw(	EditOperation *		eo,
					EditDocument *		ed,
					const BufferItem *	sectBi,
					int			row0,
					int			row1 );

extern int tedChangeParticuleAttributes(
				PropertyMask *			pTaAllMask,
				AppDrawingData *		add,
				ScreenFontList *		sfl,
				BufferDocument *		bd,
				BufferItem *			bi,
				int				partFrom,
				int				partUpto,
				const TextAttribute *		taSet,
				const PropertyMask *		taSetMask );

extern int tedOpenParaScreenFonts( BufferDocument *		bd,
				AppDrawingData *		add,
				ScreenFontList *		sfl,
				BufferItem *			paraBi,
				int				partFrom,
				int				partUpto );

extern int tedSectionParagraph(
			EditOperation *			eo,
			BufferItem **			pParaBi,
			BufferItem *			sectBi,
			int				sectShift,
			const ParagraphProperties *	pp,
			int				textAttributeNumber );

#   endif	/*	TED_EDIT_H	*/
