/************************************************************************/
/*									*/
/*  Paragraph properties.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PARA_PROP_H
#   define	DOC_PARA_PROP_H

#   include	<utilDocFont.h>
#   include	"utilPropMask.h"
#   include	"docPropVal.h"
#   include	"docTabStop.h"
#   include	"docBorderProperties.h"
#   include	"docItemShading.h"
#   include	"docParaNumber.h"
#   include	"docFrameProperties.h"

/************************************************************************/
/*									*/
/*  Paragraph properties.						*/
/*									*/
/*  Unless explicitly stated, all properties except numbers in pixels	*/
/*  are properties of the paragraph.					*/
/*									*/
/*  1)  Tab stops.							*/
/*  2)  Indentations.							*/
/*  3)  White space above/after the paragraph.				*/
/*  4)  Style, List Override ID.					*/
/*  5)  Borders.							*/
/*  6)  Shading.							*/
/*									*/
/************************************************************************/

typedef struct ParagraphProperties
    {
    /*  1  */
    TabStopList		ppTabStopList;

    /*  2  */
    short int		ppFirstIndentTwips;
    short int		ppLeftIndentTwips;
    short int		ppRightIndentTwips;

    /*  3  */
    short int		ppSpaceBeforeTwips;
    short int		ppSpaceAfterTwips;
    short int		ppLineSpacingTwips;

    /*  4  */
    short int		ppStyle;
    short int		ppListOverride;

    /*  5  */
    BorderProperties	ppTopBorder;
    BorderProperties	ppBottomBorder;
    BorderProperties	ppLeftBorder;
    BorderProperties	ppRightBorder;
    BorderProperties	ppBetweenBorder;
    BorderProperties	ppBar;

    /*  6  */
    ItemShading		ppShading;

    unsigned int	ppOutlineLevel:4;
    unsigned int	ppListLevel:4; 			/*  ilvl	*/

    unsigned int	ppAlignment:3;			/*  (enum)	*/

    unsigned int	ppStartsOnNewPage:1;
    unsigned int	ppInTable:1;
    unsigned int	ppLineSpacingIsMultiple:1;

    unsigned int	ppKeepOnPage:1;
    unsigned int	ppKeepWithNext:1;
    unsigned int	ppWidowControl:1;
    unsigned int	ppHyphenateParagraph:1;
    } ParagraphProperties;

/************************************************************************/
/*									*/
/*  Numbers of paragraph properies.					*/
/*									*/
/************************************************************************/

typedef enum ParagraphProperty
    {
    PPprop_NONE= -1,

    PPpropSTYLE= 0,
    PPpropLISTOVERRIDE,
    PPpropIN_TABLE,
    PPpropLEFT_INDENT,
    PPpropFIRST_INDENT,
    PPpropRIGHT_INDENT,
    PPpropALIGNMENT,
    PPpropTAB_STOPS,
    PPpropNEWPAGE,
    PPpropWIDCTLPAR,
    PPpropKEEP,
    PPpropKEEPN,
    PPpropSPACE_BEFORE,
    PPpropSPACE_AFTER,
    PPpropLINE_SPACING_DIST,
    PPpropLINE_SPACING_MULT,

    PPpropTOP_BORDER,
    PPpropBOTTOM_BORDER,
    PPpropLEFT_BORDER,
    PPpropRIGHT_BORDER,
    PPpropBETWEEN_BORDER,
    PPpropBAR_BORDER,

    PPpropSHADE_FORE_COLOR,
    PPpropSHADE_BACK_COLOR,
    PPpropSHADE_LEVEL,
    PPpropSHADE_PATTERN,

    PPpropOUTLINELEVEL,
    PPpropLISTLEVEL,
    PPpropHYPHPAR,

    PPpropCOLUMNS,			/*  Not a real property.	*/
					/*  But used for managing the	*/
					/*  ruler of a document window.	*/
    PPpropBOX_BORDER,			/*  Not a real property.	*/
					/*  But used to support the	*/
					/*  obsolete \\box tag.		*/

    PPprop_COUNT
    } ParagraphProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docParaAddTab(	ParagraphProperties *	pp,
				const TabStop *		tsNew );

extern void docParaDeleteTab(	ParagraphProperties *	pp,
				int			n );

extern int docUpdParaProperties( PropertyMask *			pPpChgPask,
				ParagraphProperties *		pp,
				const PropertyMask *		ppUpdMask,
				const ParagraphProperties *	ppNew,
				const int *			colorMap,
				const int *			listStyleMap );

extern void docParaPropertyDifference(
				PropertyMask *			pChgMask,
				const ParagraphProperties *	pp1,
				const ParagraphProperties *	pp2,
				const PropertyMask *		updMask );

extern void docCleanParagraphProperties(	ParagraphProperties *	pp );

extern void docInitParagraphProperties(		ParagraphProperties *	pp );

extern int docCopyParagraphProperties(
				ParagraphProperties *		ppTo,
				const ParagraphProperties *	ppFrom );

extern void docShadingMaskToParagraphMask(
					PropertyMask *		ppPropMask,
					const PropertyMask *	isPropMask );

extern void docShadingMaskFromParagraphMask(
					PropertyMask *		ppPropMask,
					const PropertyMask *	isPropMask );

#   endif	/*  DOC_PARA_PROP_H	*/
