/************************************************************************/
/*									*/
/*  The stylesheet of a document.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_STYLE_SHEET_H
#   define	DOC_STYLE_SHEET_H

#   include	<utilPropMask.h>
#   include	<utilDocFont.h>

#   include	"docSectProperties.h"
#   include	"docParaProperties.h"

/************************************************************************/
/*									*/
/*  Styles in a document.						*/
/*									*/
/*  dsBusy is used to prevent recursion when styles are expanded.	*/
/*									*/
/************************************************************************/

typedef struct DocumentStyle
    {
    int			dsStyleNumber;
    int			dsBasedOn;
    int			dsAdditive;
    int			dsNext;
    int			dsLink;
    int			dsLevel;
    int			dsAutoupd;
    int			dsHidden;
    int			dsLocked;
    int			dsSemiHidden;
    int			dsPersonal;
    int			dsBusy;

    SectionProperties	dsSectionProperties;
    ParagraphProperties	dsParagraphProperties;
    TextAttribute	dsTextAttribute;

    unsigned char *	dsName;
    PropertyMask	dsSectMask;
    PropertyMask	dsParaMask;
    PropertyMask	dsTextMask;
    } DocumentStyle;

typedef enum DocumentStyleProperty
    {
    DSpropSTYLE_NUMBER= 0,
    DSpropBASED_ON,
    DSpropADDITIVE,
    DSpropNEXT,
    DSpropLINK,
    DSpropLEVEL,
    DSpropAUTO_UPD,
    DSpropHIDDEN,
    DSpropLOCKED,
    DSpropSEMI_HIDDEN,
    DSpropPERSONAL,
    DSpropNAME,

    DSprop_COUNT
    } DocumentStyleProperty;

typedef struct DocumentStyleSheet
    {
    DocumentStyle *	dssStyles;
    int			dssStyleCount;
    } DocumentStyleSheet;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitDocumentStyle(	DocumentStyle *	ds );
extern void docCleanDocumentStyle(	DocumentStyle *	ds );

extern int docCopyStyle(	DocumentStyle *		to,
				const DocumentStyle *	from );

extern DocumentStyle * docInsertStyle(	DocumentStyleSheet *	dss,
					int			n );

extern void docInitStyleSheet(	DocumentStyleSheet *	dss );
extern void docCleanStyleSheet(	DocumentStyleSheet *	dss );

#   endif	/*  DOC_STYLE_SHEET_H  */
