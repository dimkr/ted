/************************************************************************/
/*									*/
/*  Shape Properties.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SHAPE_PROPERTIES_H
#   define	DOC_SHAPE_PROPERTIES_H

#   include	<geo2DInteger.h>

/************************************************************************/
/*									*/
/*  Primitives for shapes.						*/
/*									*/
/************************************************************************/

typedef enum ShapeHorizontalAttachment
    {
    SHPbxMARGIN= 0,
    SHPbxPAGE,
    SHPbxCOLUMN,
    SHPbxCHARACTER,		/*  For posrelh	*/
    SHPbxIGNORE,

    SHPbx_COUNT
    } ShapeHorizontalAttachment;

typedef enum ShapeVerticalAttachment
    {
    SHPbyMARGIN= 0,
    SHPbyPAGE,
    SHPbyPARA,
    SHPbyLINE,			/* For posrelv	*/
    SHPbyIGNORE,

    SHPby_COUNT
    } ShapeVerticalAttachment;

typedef enum ShapeWrapStyle
    {
    SHPswsTOPBOTTOM= 1,
    SHPswsAROUND,
    SHPswsNONE,
    SHPswsTIGHTLY,
    SHPswsTHROUGH
    } ShapeWrapStyle;

typedef enum ShapeWrapSide
    {
    SHPswsBOTH= 0,
    SHPswsLEFT,
    SHPswsRIGHT,
    SHPswsLARGEST
    } ShapeWrapSide;

typedef struct ShapeProperties
    {
    DocumentRectangle	spRect;
    long		spID;
    int			spZ;

    unsigned int	spHorizontalAttachment:4;	/*  (enum)	*/
    unsigned int	spVerticalAttachment:4;		/*  (enum)	*/
    unsigned int	spWrapStyle:3;			/*  (enum)	*/
    unsigned int	spWrapSide:2;			/*  (enum)	*/

    unsigned int	spShapeBelowText:1;
    unsigned int	spInHeader:1;
    unsigned int	spLockAnchor:1;
    } ShapeProperties;

/************************************************************************/
/*									*/
/*  Numbers of shape properies.					*/
/*									*/
/************************************************************************/

typedef enum ShapeProperty
    {
    SHPpropLEFT= 0,
    SHPpropTOP,
    SHPpropBOTTOM,
    SHPpropRIGHT,
    SHPpropLID,
    SHPpropZ,
    SHPpropFHDR,
    SHPpropBX,
    SHPpropBY,
    SHPpropWR,
    SHPpropWRK,
    SHPpropFBLWTXT,
    SHPpropLOCKANCHOR,

    SHPprop_COUNT
    } ShapeProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitShapeProperties(	ShapeProperties *	sp );

#   endif	/*  DOC_SHAPE_PROPERTIES_H	*/
