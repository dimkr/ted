/************************************************************************/
/*									*/
/*  Section Properties.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SECT_PROP_H
#   define	DOC_SECT_PROP_H

#   include	<utilDocumentGeometry.h>
#   include	"docNotesProperties.h"
#   include	"docParaNumber.h"

typedef enum SectionBreak
    {
    DOCsbkNONE= 0,
    DOCsbkCOL,
    DOCsbkPAGE,
    DOCsbkEVEN,
    DOCsbkODD,

    DOCsbk_COUNT
    } SectionBreak;

typedef enum PageNumberStyle
    {
    DOCpgnDEC= 0,
    DOCpgnUCRM,
    DOCpgnLCRM,
    DOCpgnUCLTR,
    DOCpgnLCLTR,

    DOCpgn_COUNT
    } PageNumberStyle;

typedef struct SectionProperties
    {
    DocumentGeometry	spDocumentGeometry;

    int			spStyle;

    int			spColumnCount;
    int			spColumnSpacingTwips;
    int			spColumnWidthTwips;

    unsigned int	spHasTitlePage:1;
    unsigned int	spBreakKind:3;
    unsigned int	spPageNumberStyle:3;
    unsigned int	spRestartPageNumbers:1;

    int			spStartPageNumber;

    NotesProperties	spFootnoteProperties;
    NotesProperties	spEndnoteProperties;

    ParagraphNumber *	spParagraphNumbers;
    int			spParagraphNumberCount;
    } SectionProperties;

typedef enum SectionProperty
    {
    SPprop_NONE= -1,

    /* document geometry */

    SPpropSTYLE= DGprop_COUNT,
    SPpropTITLEPG,
    SPpropBREAK_KIND,
    SPpropNUMBER_STYLE,
    SPpropPAGE_RESTART,
    SPpropSTART_PAGE,
    SPpropCOLUMN_COUNT,
    SPpropCOLUMN_SPACING,
    SPpropCOLUMN_WIDTH,

    SPpropFOOTNOTE_STARTNR,
    SPpropFOOTNOTE_POSITION,
    SPpropFOOTNOTE_RESTART,
    SPpropFOOTNOTE_STYLE,

    SPpropENDNOTE_STARTNR,
    SPpropENDNOTE_POSITION,
    SPpropENDNOTE_RESTART,
    SPpropENDNOTE_STYLE,

    SPprop_COUNT
    } SectionProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitSectionProperties(	SectionProperties *	sp );
extern void docCleanSectionProperties(	SectionProperties *	sp );

extern int docCopySectionProperties(	SectionProperties *		to,
					const SectionProperties *	from );

extern void docSectPropertyDifference(
				PropertyMask *			pChgMask,
				const SectionProperties *	sp1,
				const SectionProperties *	sp2,
				const PropertyMask *		updMask );

extern int docUpdSectProperties( PropertyMask *			pSpChgMask,
				SectionProperties *		to,
				const PropertyMask *		spUpdMask,
				const SectionProperties *	from );

#   endif	/*  DOC_SECT_PROP_H  */
