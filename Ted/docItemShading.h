/************************************************************************/
/*									*/
/*  Item Shading.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_ITEM_SHADING_H
#   define	DOC_ITEM_SHADING_H

#   include	"utilPropMask.h"
#   include	<bmcolor.h>

typedef enum ShadingPattern
    {
    DOCspSOLID= 0,

    DOCspBGHORIZ,
    DOCspBGVERT,
    DOCspBGFDIAG,
    DOCspBGBDIAG,
    DOCspBGCROSS,
    DOCspBGDCROSS,
    DOCspBGDKHORIZ,
    DOCspBGDKVERT,
    DOCspBGDKFDIAG,
    DOCspBGDKBDIAG,
    DOCspBGDKCROSS,
    DOCspBGDKDCROSS,

    DOCsp_COUNT
    } ShadingPattern;

#   define DOCsp_BITS	4

/************************************************************************/
/*									*/
/*  1)  The shading of a BufferItem such as a table cell or a		*/
/*	paragraph.							*/
/*  2)  Numbers for the properties.					*/
/*  3)  Expanded form of the shading for the use in tools. (Remove the	*/
/*	indirection via the palette).					*/
/*									*/
/************************************************************************/

/*  1  */
typedef struct ItemShading
    {
    short		isBackColor;
    short		isForeColor;

    unsigned int	isLevel:14;
    unsigned int	isPattern:DOCsp_BITS;
    } ItemShading;

# define DOCisSHADING(is) ((is)->isBackColor!=0||(is)->isForeColor!=0)

/*  2  */
typedef enum ShadingProperty
    {
    ISprop_NONE= -1,

    ISpropBACK_COLOR= 0,
    ISpropFORE_COLOR,
    ISpropLEVEL,
    ISpropPATTERN,

    ISprop_COUNT
    } ShadingProperty;

/*  3  */
typedef struct ExpandedItemShading
    {
    int			eisBackColorExplicit;
    int			eisForeColorExplicit;
    RGB8Color		eisBackColor;
    RGB8Color		eisForeColor;

    unsigned int	eisLevel;
    unsigned int	eisPattern;
    } ExpandedItemShading;

#   define	docCopyItemShading( is1, is2 ) *(is1)= *(is2)
#   define	docCleanItemShading( is ) /*void*/

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitItemShading(		ItemShading *		is );
extern void docInitExpandedItemShading(	ExpandedItemShading *	eis );

extern void docUpdateItemShading(
				PropertyMask *			pIsDoneMask,
				ItemShading *			isTo,
				const PropertyMask *		isSetMask,
				const ItemShading *		isFrom,
				const int *			colorMap );

extern void docUpdateExpandedItemShading(
				PropertyMask *			pIsDoneMask,
				ExpandedItemShading *		eisTo,
				const PropertyMask *		isSetMask,
				const ExpandedItemShading *	eisFrom );

extern void docExpandItemShading(	ExpandedItemShading *	eis,
					const ItemShading *	is,
					const RGB8Color *	colors,
					int			colorCount );

extern int docIndirectItemShading(
				PropertyMask *			pDoneMask,
				ItemShading *			is,
				const PropertyMask *		setMask,
				const ExpandedItemShading *	eis,
				RGB8Color **			pColors,
				int *				pColorCount );

extern void docItemShadingDifference(
				PropertyMask *			pIsUpdMask,
				const ItemShading *		isTo,
				const ItemShading *		isFrom,
				const PropertyMask *		isSetMask,
				const int *			colorMap );

#   endif	/*  DOC_ITEM_SHADING_H  */
