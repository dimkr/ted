/************************************************************************/
/*									*/
/*  Document Properties.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_DOCUMENT_PROPERTIES_H
#   define	DOC_DOCUMENT_PROPERTIES_H

#   include	<time.h>

#   include	<utilPropMask.h>

#   include	<utilDocumentGeometry.h>
#   include	<appUnit.h>
#   include	<utilDocFont.h>
#   include	<bmcolor.h>

#   include	"docNotesProperties.h"
#   include	"docListTable.h"
#   include	"docListOverrideTable.h"

typedef enum DocumentProperty
    {
    DPpropNONE= -1,

    DPpropSTART_PAGE= DGprop_COUNT,

    DPpropFACING_PAGES,
    DPpropWIDOWCTRL,
    DPpropTWO_ON_ONE,
    DPpropDOCTEMP,

    DPpropNOTETYPES,	/*  \fetN	*/

    DPpropFOOTNOTE_STARTNR,
    DPpropFOOTNOTE_POSITION,
    DPpropFOOTNOTE_RESTART,
    DPpropFOOTNOTE_STYLE,

    DPpropENDNOTE_STARTNR,
    DPpropENDNOTE_POSITION,
    DPpropENDNOTE_RESTART,
    DPpropENDNOTE_STYLE,

    DPpropDEFTAB,
    DPpropDOC_CHARSET,
    DPpropANSICPG,

    DPpropDEFF,
    DPpropSTSHFDBCH,
    DPpropSTSHFLOCH,
    DPpropSTSHFHICH,
    DPpropSTSHFBI,

    DPpropGENERATOR,
    DPpropTITLE,
    DPpropSUBJECT,
    DPpropKEYWORDS,
    DPpropDOCCOMM,
    DPpropAUTHOR,
    DPpropCOMPANY,
    DPpropHLINKBASE,

    DPpropCREATIM,
    DPpropREVTIM,
    DPpropPRINTIM,

    DPpropTOP_BORDER,
    DPpropBOTTOM_BORDER,
    DPpropLEFT_BORDER,
    DPpropRIGHT_BORDER,
    DPpropHEAD_BORDER,
    DPpropFOOT_BORDER,

    DPpropLISTTABLE,
    DPpropLISTOVERRIDETABLE,

				/****************************************/
    DPpropIGNORED,		/*  Various document properties that	*/
				/*  are ignored, but accepted to avoid	*/
				/*  warnings.				*/
				/****************************************/

    DPprop_COUNT
    } DocumentProperty;

typedef struct DocumentProperties
    {
    int			dpContainsTables;
    int			dpTabIntervalTwips;
    int			dpDefaultColor;
    int			dpDocumentCharset; /* ansi, mac, pc, pca */
    int			dpAnsiCodepage;

    int			dpDefaultFont;
    int			dpDefaultFontDbch;
    int			dpDefaultFontLoch;
    int			dpDefaultFontHich;
    int			dpDefaultFontBi;

    int			dpStartPageNumber;

    unsigned int	dpHasFacingPages:1;
    unsigned int	dpWidowControl:1;
    unsigned int	dpTwoOnOne:1;
    unsigned int	dpIsDocumentTemplate:1;

    unsigned int	dpFootEndNoteType:2;
    NotesProperties	dpFootnoteProperties;
    NotesProperties	dpEndnoteProperties;

    DocumentGeometry	dpGeometry;
    DocumentFontList	dpFontList;

    DocumentListTable	dpListTable;
    ListOverrideTable	dpListOverrideTable;

    RGB8Color *		dpColors;
    int			dpColorCount;

    unsigned char **	dpAuthors;
    int			dpAuthorCount;

    struct tm		dpCreatim;
    struct tm		dpRevtim;
    struct tm		dpPrintim;

    unsigned char *	dpGenerator;
    unsigned char *	dpTitle;
    unsigned char *	dpSubject;
    unsigned char *	dpKeywords;
    unsigned char *	dpDoccomm;
    unsigned char *	dpAuthor;
    unsigned char *	dpCompany;
    unsigned char *	dpHlinkbase;

    BorderProperties	dpTopBorder;
    BorderProperties	dpBottomBorder;
    BorderProperties	dpLeftBorder;
    BorderProperties	dpRightBorder;
    BorderProperties	dpHeadBorder;
    BorderProperties	dpFootBorder;

    unsigned char *	dpFilename;
    } DocumentProperties;

#   define	docHasDocumentInfo( dp )		\
		( (dp)->dpTitle			||	\
		  (dp)->dpSubject		||	\
		  (dp)->dpKeywords		||	\
		  (dp)->dpDoccomm		||	\
		  (dp)->dpHlinkbase		||	\
		  (dp)->dpAuthor		||	\
		  (dp)->dpCompany		||	\
		  (dp)->dpCreatim.tm_mday != 0	||	\
		  (dp)->dpRevtim.tm_mday != 0	||	\
		  (dp)->dpPrintim.tm_mday != 0	)

#   define	DOCcolorBITS		8
#   define	DOCcolorMAXCOUNT	(1<<DOCcolorBITS)

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docInitDocumentProperties(	DocumentProperties *		dp );

extern void docCleanDocumentProperties(	DocumentProperties *		dp );

extern int docCopyDocumentProperties(	DocumentProperties *		to,
					const DocumentProperties *	from );

extern int docUpdDocumentProperties(
				PropertyMask *			pChgPask,
				DocumentProperties *		dp,
				const PropertyMask *		updMask,
				const DocumentProperties *	dpNew );

extern int docPropertiesSetFilename(	DocumentProperties *	dp,
					const char *		filename );

extern int docAllocateDocumentColor(	DocumentProperties *	dp,
					const RGB8Color *	rgb8 );

extern int docMakeOverrideForEveryList(	DocumentProperties *	dp );

#   endif	/*  DOC_DOCUMENT_PROPERTIES_H  */
