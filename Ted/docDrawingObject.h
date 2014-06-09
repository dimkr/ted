/************************************************************************/
/*									*/
/*  Drawing objects as used by Ms-Word 95				*/
/*									*/
/************************************************************************/

#   ifndef DOC_DRAWING_OBJECT_H
#   define DOC_DRAWING_OBJECT_H

#   include	<appFrameConfig.h>

#   include	<bmcolor.h>
#   include	<appGuiBase.h>
#   include	"docShape.h"

typedef enum DrawingObjectFillPattern
    {
    DOfillCLEAR= 0,
    DOfillSOLID,

    DOfillSHADE_5P,
    DOfillSHADE_10P,
    DOfillSHADE_20P,
    DOfillSHADE_25P,
    DOfillSHADE_30P,
    DOfillSHADE_40P,
    DOfillSHADE_50P,
    DOfillSHADE_60P,
    DOfillSHADE_70P,
    DOfillSHADE_75P,
    DOfillSHADE_80P,
    DOfillSHADE_90P,

    DOfillDKHORIZ,
    DOfillDKVERT,
    DOfillDKFDIAG,
    DOfillDKBDIAG,
    DOfillDKCROSS,
    DOfillDKDCROSS,

    DOfillHORIZ,
    DOfillVERT,
    DOfillFDIAG,
    DOfillBDIAG,
    DOfillCROSS,
    DOfillDCROSS,

    DOfill_COUNT
    } DrawingObjectFillPattern;

typedef enum DrawingObjectProperty
    {
    DOpropANCHOR_LOCKED= 0,
    DOpropX_ATTACH,
    DOpropY_ATTACH,
    DOpropKIND,
    DOpropLINE_STYLE,
    DOpropFILL_PATTERN,

    DOpropARC_FLIP_X,
    DOpropARC_FLIP_Y,

    DOpropZ,

    DOpropX,
    DOpropY,
    DOpropWIDE,
    DOpropHIGH,
    DOpropTEXT_BOX_MARGIN,

    DOpropLINE_WIDTH,
    DOpropPOINT_COUNT,

    DOpropSTART_ARROW_HEAD,
    DOpropEND_ARROW_HEAD,
    DOpropSTART_ARROW_WIDTH,
    DOpropSTART_ARROW_LENGTH,
    DOpropEND_ARROW_WIDTH,
    DOpropEND_ARROW_LENGTH,

    DOpropLINE_RED,
    DOpropLINE_GREEN,
    DOpropLINE_BLUE,
    DOpropLINE_GRAY,

    DOpropFILL_FORE_RED,
    DOpropFILL_FORE_GREEN,
    DOpropFILL_FORE_BLUE,
    DOpropFILL_FORE_GRAY,

    DOpropFILL_BACK_RED,
    DOpropFILL_BACK_GREEN,
    DOpropFILL_BACK_BLUE,
    DOpropFILL_BACK_GRAY,

    DOprop_COUNT
    } DrawingObjectProperty;

#   endif /*  DOC_DRAWING_OBJECT_H  */
