/************************************************************************/
/*									*/
/*  Values and sub structures used in various document property		*/
/*  structs.								*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PROP_VAL_H
#   define	DOC_PROP_VAL_H

typedef enum ItemAlignment
    {
    DOCiaLEFT= 0,
    DOCiaRIGHT,
    DOCiaCENTERED,
    DOCiaJUSTIFIED,

    DOCia_COUNT
    } ItemAlignment;

# define DOCia_BITS	3

typedef enum TextVerticalAlignment
    {
    DOCtvaTOP= 0,
    DOCtvaCENTERED,
    DOCtvaBOTTOM,

    DOCtva_COUNT
    } TextVerticalAlignment;

typedef enum VerticalTextDirection
    {
    DOCvtdBOTTOM_UP,
    DOCvtdTOP_DOWN,

    DOCvtd_COUNT
    } VerticalTextDirection;

#   endif	/*  DOC_PROP_VAL_H  */
