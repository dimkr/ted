/************************************************************************/
/*									*/
/*  Declare field evaluation routines.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_EVAL_FIELD_H
#   define	DOC_EVAL_FIELD_H

#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docFormatChftnField(	unsigned char *			target,
				int				targetSize,
				const DocumentProperties *	dp,
				int				noteNumber,
				int				extItKind );

extern int docCalculateChftnFieldString(int *			pCalculated,
					int *			pNewSize,
					unsigned char *		target,
					int			targetSize,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			partCount,
					const DocumentField *	dfRef );

extern int docCalculateSymbolFieldString(int *			pCalculated,
					int *			pNewSize,
					unsigned char *		target,
					int			targetSize,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			partCount,
					const DocumentField *	dfRef );

extern int docCalculatePagerefFieldString(
					int *			pCalculated,
					int *			pNewSize,
					unsigned char *		target,
					int			targetSize,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			partCount,
					const DocumentField *	dfRef );

extern int docCalculateSectionFieldString(
					int *			pCalculated,
					int *			pNewSize,
					unsigned char *		target,
					int			targetSize,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			partCount,
					const DocumentField *	df );

extern int docCalculateSectionPagesFieldString(
					int *			pCalculated,
					int *			pNewSize,
					unsigned char *		target,
					int			targetSize,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			partCount,
					const DocumentField *	df );

extern int docCalculatePageFieldString(	int *			pCalculated,
					int *			pNewSize,
					unsigned char *		target,
					int			targetSize,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			partCount,
					const DocumentField *	df );

extern int docCalculateNumpagesFieldString(
					int *			pCalculated,
					int *			pNewSize,
					unsigned char *		target,
					int			targetSize,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			partCount,
					const DocumentField *	df );

extern int docCalculateDocDateFieldString(
					int *			pCalculated,
					int *			pNewSize,
					unsigned char *		target,
					int			targetSize,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			partCount,
					const DocumentField *	df );

extern int docCalculateDocStringFieldString(
					int *			pCalculated,
					int *			pNewSize,
					unsigned char *		target,
					int			targetSize,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			partCount,
					const DocumentField *	df );

extern int docCalculateFilenameFieldString(
					int *			pCalculated,
					int *			pNewSize,
					unsigned char *		target,
					int			targetSize,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			partCount,
					const DocumentField *	df );

extern int docCalculateRefFieldString(	int *			pCalculated,
					int *			pNewSize,
					unsigned char *		target,
					int			targetSize,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			partCount,
					const DocumentField *	dfRef );

extern int docRecalculateHyperlinkField(
				int *				pCalculated,
				BufferDocument *		bd,
				int *				pPartShift,
				int *				pStroffShift,
				BufferItem *			bi,
				int				part,
				int				partCount,
				DocumentField *			df,
				void *				voidadd,
				DOC_CLOSE_OBJECT		closeObject );

extern int docRecalculateIncludePictureField(
				int *				pCalculated,
				BufferDocument *		bd,
				int *				pPartShift,
				int *				pStroffShift,
				BufferItem *			bi,
				int				part,
				int				partCount,
				DocumentField *			df,
				void *				voidadd,
				DOC_CLOSE_OBJECT		closeObject );

extern int docRecalculateParaStringTextParticules(
				int *				pCalculated,
				BufferDocument *		bd,
				int *				pPartShift,
				int *				pStroffShift,
				BufferItem *			bi,
				int				part,
				int				partCount,
				DocumentField *			df,
				void *				voidadd,
				DOC_CLOSE_OBJECT		closeObject );

extern int docRecalculateParaChftnTextParticules(
				int *				pCalculated,
				BufferDocument *		bd,
				int *				pPartShift,
				int *				pStroffShift,
				BufferItem *			bi,
				int				part,
				int				partCount,
				DocumentField *			df,
				void *				voidadd,
				DOC_CLOSE_OBJECT		closeObject );

extern int docRecalculateParaListtextTextParticules(
				int *				pCalculated,
				BufferDocument *		bd,
				int *				pPartShift,
				int *				pStroffShift,
				BufferItem *			bi,
				int				part,
				int				partCount,
				DocumentField *			df,
				void *				voidadd,
				DOC_CLOSE_OBJECT		closeObject );

extern int docRecalculateParaSymbolTextParticules(
				int *				pCalculated,
				BufferDocument *		bd,
				int *				pPartShift,
				int *				pStroffShift,
				BufferItem *			bi,
				int				part,
				int				partCount,
				DocumentField *			df,
				void *				voidadd,
				DOC_CLOSE_OBJECT		closeObject );

extern int docFieldReplaceContents(	int *			pStroff,
					int *			pStroffShift,
					int *			pTextAttrNr,
					BufferDocument *	bd,
					BufferItem *		paraBi,
					int			part,
					int			partCount,
					int			stroffShift,
					const unsigned char *	addedString,
					int			addedStrlen,
					void *			voidadd,
					DOC_CLOSE_OBJECT	closeObject );

#   endif	/*  DOC_EVAL_FIELD_H  */
