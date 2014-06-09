/************************************************************************/
/*									*/
/*  Attributes of text.							*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_TEXT_ATTRIBUTE_H
#   define	UTIL_TEXT_ATTRIBUTE_H

#   include	<utilPropMask.h>

typedef struct TextAttribute
    {
    short int		taFontNumber;
    short int		taFontSizeHalfPoints;
    short int		taTextColorNumber;
    short int		taTextStyleNumber;

    unsigned int	taFontIsBold:1;
    unsigned int	taFontIsSlanted:1;
    unsigned int	taTextIsUnderlined:1;

    unsigned int	taSmallCaps:1;
    unsigned int	taHasStrikethrough:1;
    unsigned int	taCapitals:1;

    unsigned int	taSuperSub:2;
#   define		DOCfontREGULAR		0
#   define		DOCfontSUPERSCRIPT	1
#   define		DOCfontSUBSCRIPT	2
    } TextAttribute;

typedef enum TextProperty
    {
    TAprop_NONE= -1,

    TApropDOC_FONT_NUMBER= 0,
    TApropFONTSIZE,
    TApropFONTBOLD,
    TApropFONTSLANTED,
    TApropTEXTUNDERLINED,
    TApropSUPERSUB,
    TApropSMALLCAPS,
    TApropCAPITALS,
    TApropSTRIKETHROUGH,
    TApropTEXT_COLOR,
    TApropTEXT_STYLE,

    TAprop_COUNT
    } TextProperty;

#   define	docEqualFontAttributes(ta1,ta2) ( \
		(ta1)->taFontIsBold == (ta2)->taFontIsBold && \
		(ta1)->taFontIsSlanted == (ta2)->taFontIsSlanted )

#   define	docEqualFont(ta1,ta2) ( \
		(ta1)->taFontNumber == (ta2)->taFontNumber && \
		(ta1)->taFontSizeHalfPoints == (ta2)->taFontSizeHalfPoints && \
		(ta1)->taSuperSub == (ta2)->taSuperSub && \
		(ta1)->taSmallCaps == (ta2)->taSmallCaps && \
		docEqualFontAttributes( (ta1), (ta2) ) )

#   define	docEqualTextAttributes(ta1,ta2) ( \
		docEqualFont((ta1),(ta2)) && \
		(ta1)->taTextIsUnderlined == (ta2)->taTextIsUnderlined && \
		(ta1)->taHasStrikethrough == (ta2)->taHasStrikethrough && \
		(ta1)->taCapitals == (ta2)->taCapitals && \
		(ta1)->taTextColorNumber == (ta2)->taTextColorNumber )

#   define	SUPERSUB_SIZE( sz )	( 6*(sz)/10 )

/************************************************************************/
/*									*/
/*  Default attributes.							*/
/*									*/
/************************************************************************/

extern TextAttribute   DocDefaultAttributes;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void utilInitTextAttribute(	TextAttribute *		ta );

extern void utilAttributeDifference(	PropertyMask *		pChangeMask,
					const TextAttribute *	taTo,
					const TextAttribute *	taFrom,
					const PropertyMask *	updMask );

extern void utilUpdateTextAttribute(	PropertyMask *		pChangeMask,
					TextAttribute *		taTo,
					const TextAttribute *	taFrom,
					const PropertyMask *	updMask );

#   endif	/*  UTIL_TEXT_ATTRIBUTE_H	*/
