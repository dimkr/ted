/************************************************************************/
/*									*/
/*  Positioned Objects and Frames					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_FRAME_PROPERTIES_H
#   define	DOC_FRAME_PROPERTIES_H

typedef enum HorizontalFrameReference
    {
    HFRphCOL= 0,
    HFRphPG,
    HFRphMRG,

    HFRph_COUNT
    } HorizontalFrameReference;

typedef enum VerticalFrameReference
    {
    VFRpvMRG= 0,
    VFRpvPG,
    VFRpvPARA,

    VFRpv_COUNT
    } VerticalFrameReference;

typedef enum HorizontalFrameAlignment
    {
    HFAposXL= 0,
    HFAposXR,
    HFAposXC,
    HFAposXI,
    HFAposXO,

    HFApos_COUNT
    } HorizontalFrameAlignment;

typedef enum VerticalFrameAlignment
    {
    VFAposYIL= 0,
    VFAposYIN,
    VFAposYOUT,
    VFAposYT,
    VFAposYC,
    VFAposYB,

    VFApos_COUNT
    } VerticalFrameAlignment;

typedef enum FrameWrapStyle
    {
    FWSwrapWRAP= 0,
    FWSwrapNOWRAP,
    FWSwrapOVERLAY,

    FWSwrap_COUNT
    } FrameWrapStyle;

typedef struct FramePosition
    {
    int			fpFrameWidthTwips;
    int			fpFrameHeightTwips;

    int			fpFrameXTwips;
    int			fpFrameYTwips;

    int			fpDistanceFromTextTwipsLeft;
    int			fpDistanceFromTextTwipsRight;
    int			fpDistanceFromTextTwipsTop;
    int			fpDistanceFromTextTwipsBottom;
    unsigned int	fpDistanceFromTextSet:4;
#			define DISTsetLEFT	(1<<0)
#			define DISTsetRIGHT	(1<<1)
#			define DISTsetTOP	(1<<2)
#			define DISTsetBOTTOM	(1<<3)

    unsigned int	fpHorizontalFrameReference:2;
    unsigned int	fpHorizontalFrameAlignment:3;

    unsigned int	fpVerticalFrameReference:2;
    unsigned int	fpVerticalFrameAlignment:3;
    unsigned int	fpFrameLockedToParagraph:1;

    unsigned int	fpFrameWrapStyle:2;
    unsigned int	fpNoOverlap:1;
    } FramePosition;

/************************************************************************/
/*									*/
/*  Property numbers and masks.						*/
/*									*/
/************************************************************************/

typedef enum TextFrameProperty
    {
    TFPprop_NONE= -1,

    TFPpropABSW= 0,
    TFPpropABSH,

    TFPpropNO_OVERLAP,

    TFPpropHORIZONTAL_REF,
    TFPpropHORIZONTAL_POS,
    TFPpropHORIZONTAL_ALIGN,

    TFPpropVERTICAL_REF,
    TFPpropVERTICAL_POS,
    TFPpropVERTICAL_ALIGN,
    TFPpropLOCKED_TO_PARA,

    TFPpropWRAP_STYLE,
    TFPpropDFRMTXT,
    TFPpropDFRMTXTX,
    TFPpropDFRMTXTY,
    TFPpropDFRMTXTL,
    TFPpropDFRMTXTR,
    TFPpropDFRMTXTT,
    TFPpropDFRMTXTB,

    TFPprop_COUNT
    } TextFrameProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitTextFrameProperties(	FramePosition *	fp );

#   endif	/*  DOC_FRAME_PROPERTIES_H	*/
