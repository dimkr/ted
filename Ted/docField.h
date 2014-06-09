/************************************************************************/
/*									*/
/*  Fields: parts of the document that are calculated in stead of typed	*/
/*  by the user.							*/
/*									*/
/************************************************************************/

#   ifndef DOC_FIELD_H
#   define DOC_FIELD_H

#   include	<utilMemoryBuffer.h>
#   include	"docObject.h"
#   include	"docEditPosition.h"

struct BufferItem;
struct BufferDocument;
struct DocumentField;

/************************************************************************/
/*									*/
/*  Kind of field. Is an index in a descriptive array of field kinds.	*/
/*									*/
/*  Fields in the 'Ted' sense are not necessarily fields in the 'Word'	*/
/*  sense. Everything inside the text, that can possibly result in	*/
/*  a piece of document, but is possibly under the control of the	*/
/*  application is a field in the sense of 'Ted'.			*/
/*									*/
/*  FieldKindInformation:						*/
/*  a)  The (case insensitive) string that identifies the field in the	*/
/*	field instructions. NOTE that the notion of a field is abused	*/
/*	by 'Ted' for a few things that are not fields in MS-Word.	*/
/*  b)  Some things that are a destination in RTF are stored as a field	*/
/*	in Ted. This is the control word of the destination.		*/
/*  c)  The level at which this kind of field occurs.			*/
/*  d)  Tells whether this kind of field is a field in the RTF file.	*/
/*  e)  Tells whether this kind of field is a destination in the RTF	*/
/*	file.								*/
/*  f1) Recalculate a TEXT level field.					*/
/*	The standard workhorse docRecalculateParaStringTextParticules()	*/
/*	can be used for all substitutions that simply return a string.	*/
/*	In its turn it uses fki->fkiCalculateTextString() to obtain	*/
/*	the string. Other particule avluators might decide to do	*/
/*	something similar.						*/
/*  f2) Recalculate the string for a TEXT level field.			*/
/*  g)  When to recalculate fields in the text.				*/
/*  h)  Is the field result editable such as with a HYPERLINK or	*/
/*	readonly such as with a PAGEREF?				*/
/*									*/
/************************************************************************/

/*  f1  */
typedef int (*CALCULATE_TEXT_PARTICULES)(
				int *				pCalculated,
				struct BufferDocument *		bd,
				int *				pPartShift,
				int *				pStroffShift,
				struct BufferItem *		paraBi,
				int				part,
				int				partCount,
				struct DocumentField *		df,
				void *				voidadd,
				DOC_CLOSE_OBJECT		closeObject );

/*  f2  */
typedef int (*CALCULATE_TEXT_STRING)(
				int *				pCalculated,
				int *				pNewSize,
				unsigned char *			target,
				int				targetSize,
				struct BufferDocument *		bd,
				const struct BufferItem *	paraBi,
				int				part,
				int				partCount,
				const struct DocumentField *	df );

/*  g  */
#   define	FIELDdoNOTHING		0
#   define	FIELDdoNEVER		0
#   define	FIELDdoDOC_FORMATTED	(1<<0)
#   define	FIELDdoPAGE_NUMBER	(1<<1)
#   define	FIELDdoDOC_INFO		(1<<2)
#   define	FIELDdoDOC_COMPLETE	(1<<3)
#   define	FIELDdoCHFTN		(1<<4)
#   define	FIELDdoLISTTEXT		(1<<5)

typedef struct FieldKindInformation
    {
    const char *		fkiLabel;			/*  a	*/
    const char *		fkiDestinationLabel;		/*  b	*/
    int				fkiLevel;			/*  c	*/
    int				fkiIsFieldInRtf;		/*  d	*/
    int				fkiIsDestinationInRtf;		/*  e	*/
    CALCULATE_TEXT_PARTICULES	fkiCalculateTextParticules;	/*  f1	*/
    CALCULATE_TEXT_STRING	fkiCalculateTextString;		/*  f2	*/
    unsigned int		fkiCalculateWhen;		/*  g	*/
    unsigned int		fkiResultEditable;		/*  h	*/
    } FieldKindInformation;

typedef enum FieldKind
    {
    DOCfkFREE= -1,
    DOCfkUNKNOWN= 0,
    DOCfkHYPERLINK,
    DOCfkXE,
    DOCfkTC,
    DOCfkTCN,
    DOCfkBOOKMARK,
    DOCfkSECTION,
    DOCfkSECTIONPAGES,
    DOCfkPAGE,
    DOCfkPAGEREF,
    DOCfkNUMPAGES,
    DOCfkREF,

    DOCfkCREATEDATE,
    DOCfkSAVEDATE,
    DOCfkPRINTDATE,
    DOCfkDATE,
    DOCfkTIME,

    DOCfkAUTHOR,
    DOCfkCOMMENTS,
    DOCfkDOCCOMM,
    DOCfkKEYWORDS,
    DOCfkSUBJECT,
    DOCfkTITLE,
    DOCfkFILENAME,

    DOCfkSYMBOL,
    DOCfkCHFTN,

    DOCfkLISTTEXT,

    DOCfkINCLUDEPICTURE,

    DOCfk_COUNT
    } FieldKind;

typedef struct DocumentField
    {
    int			dfPage;
    FieldKind		dfKind;
    MemoryBuffer	dfData;
    MemoryBuffer	dfInstructions;
    } DocumentField;

typedef struct FieldInstructionsComponent
    {
    int		ficOffset;
    int		ficSize;
    } FieldInstructionsComponent;

#   define	DOCmaxBOOKMARK	40

typedef struct DocumentFieldList
    {
    DocumentField *	dflFields;
    int			dflFieldCount;
    } DocumentFieldList;

/************************************************************************/
/*									*/
/*  Field Kind Information.						*/
/*									*/
/************************************************************************/

extern const FieldKindInformation	DOC_FieldKinds[];
extern const int			DOC_FieldKindCount;

/************************************************************************/
/*									*/
/*  Pass through information for recalculating fields.			*/
/*									*/
/************************************************************************/

typedef struct RecalculateFields
    {
    struct BufferDocument *	rfBd;
    void *			rfVoidadd;
    DOC_CLOSE_OBJECT		rfCloseObject;
    unsigned int		rfFieldsUpdated;
    unsigned int		rfUpdateFlags;
    EditPosition		rfSelBegin;
    EditPosition		rfSelEnd;
    } RecalculateFields;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern void docAdaptBookmarkName(	int *		pLength,
					char *		mark );


extern int docGetBookmarkField(	DocumentField **		pDf,
				const char *			markName,
				int				markSize,
				const DocumentFieldList *	dfl );

extern void docInitRecalculateFields(	RecalculateFields *	rf );

extern int docFieldGetSymbol(		const DocumentField *	df,
					const char **		pFontName,
					int *			pFontSize,
					const char **		pSymbName,
					int *			pSymbSize,
					const char **		pSizeString,
					int *			pSizeSize );

extern int docFieldGetHyperlink(	const DocumentField *	df,
					const char **		pFileName,
					int *			pFileSize,
					const char **		pMarkName,
					int *			pMarkSize );

extern int docFieldGetBookmark(		const DocumentField *	df,
					const char **		pMarkName,
					int *			pMarkSize );

extern int docFieldGetPageref(		const DocumentField *	df,
					const char **		pMarkName,
					int *			pMarkSize );

extern int docFieldGetIncludePicture(	const DocumentField *	df,
					const char **		pFileName,
					int *			pFileSize );

extern int docFieldGetRef(		const DocumentField *	df,
					const char **		pMarkName,
					int *			pMarkSize );

extern int docFieldSetBookmark(		DocumentField *		df,
					const unsigned char *	mark,
					int			markSize );

extern int docFieldSetRef(		DocumentField *		df,
					const unsigned char *	mark,
					int			markSize );

extern int docFieldSetPageref(		DocumentField *		df,
					const unsigned char *	mark,
					int			markSize );

extern void docInitField(		DocumentField *	df );

extern void docCleanField(		DocumentField *		df );

extern void docCleanFieldList(		DocumentFieldList *	dfl );
extern void docInitFieldList(		DocumentFieldList *	dfl );

DocumentField *	docClaimFieldCopy(	DocumentFieldList *	dfl,
					int *			pNr,
					const DocumentField *	dfFrom );

extern int docFieldSetHyperlink(	DocumentField *		df,
					const unsigned char *	fileName,
					int			fileSize,
					const unsigned char *	markName,
					int			markSize );

#   endif /*  DOC_FIELD_H  */
