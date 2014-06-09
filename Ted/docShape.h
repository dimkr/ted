/************************************************************************/
/*									*/
/*  Declarations for shapes.						*/
/*									*/
/*  Distances are in EMU's.						*/
/*  12700 EMU/Point.							*/
/*  635 EMU/Twip.							*/
/*  914400 EMU/Inch.							*/
/*  360000 EMU/cm.							*/
/*									*/
/*  With the exception of 'shplid', the ShapeProperties that come from	*/
/*  regular rtf only apply for the top level 'shp' or shpgrp'.		*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SHAPE_H
#   define	DOC_SHAPE_H

#   include	<bmcolor.h>
#   include	<appGuiBase.h>
#   include	<geo2DInteger.h>
#   include	<utilMemoryBuffer.h>
#   include	<utilAffineTransform.h>
#   include	"docShapeProperties.h"
#   include	"docPictureProperties.h"
#   include	"docExternalItem.h"

#   define	EMUtoTWIPS(e)	((e)/635)
#   define	TWIPStoEMU(t)	(635*(t))

typedef struct ShapeVertex
    {
    int		svX;
    int		svY;
    } ShapeVertex;

typedef struct ShapePath
    {
    const char *	spName;
    short int		spXSize;
    short int		spYSize;
    short int		spClosed;
    short int		spVertexCount;
    ShapeVertex *	spVertices;
    } ShapePath;

typedef struct ShapeArrow
    {
    unsigned char	saArrowHead;
#				define	DSarrowNONE			0
#				define	DSarrowARROW			1
#				define	DSarrowSTEALTH_ARROW		2
#				define	DSarrowDIAMOND			3
#				define	DSarrowOVAL			4
#				define	DSarrowOPEN_ARROW		5
#				define	DSarrowCHEVRON_ARROW		7
#				define	DSarrow2CHEVRON_ARROW		8
    unsigned char	saArrowWidth;
#				define	DSarrowNARROW			0
#				define	DSarrowMEDIUM			1
#				define	DSarrowWIDE			2
    unsigned char	saArrowLength;
#				define	DSarrowSHORT			0
/*				define	DSarrowMEDIUM			1 */
#				define	DSarrowLONG			2
    } ShapeArrow;

typedef struct DrawingShape
    {
			/************************************************/
			/*  Properties as found in regular rtf tags.	*/
			/*  Are only set for the top level shape in a	*/
			/*  hierarchy.					*/
			/************************************************/
    ShapeProperties	dsShapeProperties;

			/************************************************/
			/*  Position.					*/
			/************************************************/
    unsigned int	dsIsChildShape:8;

    unsigned int	dsHorizontalAlignment:8;
    unsigned int	dsHorizontalRelativeTo:8;
    unsigned int	dsVerticalAlignment:8;
    unsigned int	dsVerticalRelativeTo:8;

    unsigned int	ds_fUseShapeAnchor:1;
    unsigned int	ds_fLayoutInCell:1;
    unsigned int	ds_fAllowOverlap:1;
    unsigned int	ds_fChangePage:1;

			/************************************************/
			/*  Object Type					*/
			/************************************************/
    int			dsShapeType;
    int			dsRotation;

    int			dsWrapDistLeftEmu;
    int			dsWrapDistTopEmu;
    int			dsWrapDistRightEmu;
    int			dsWrapDistBottomEmu;

    long		dsMasterShape;
    long		dsNextShape;

    int			dsXLimo;
    int			dsYLimo;

    unsigned int	ds_fIsBullet:1;
    unsigned int	ds_fFlipH:1;
    unsigned int	ds_fFlipV:1;
    unsigned int	ds_fBehindDocument:1;
    unsigned int	ds_fIsButton:1;
    unsigned int	ds_fHidden:1;
    unsigned int	ds_fReallyHidden:1;
    unsigned int	ds_fArrowheadsOK:1;
    unsigned int	ds_fBackground:1;
    unsigned int	ds_fDeleteAttachedObject:1;
    unsigned int	ds_fEditedWrap:1;
    unsigned int	ds_fHitTestFill:1;
    unsigned int	ds_fHitTestLine:1;
    unsigned int	ds_fInitiator:1;
    unsigned int	ds_fNoFillHitTest:1;
    unsigned int	ds_fNoHitTestPicture:1;
    unsigned int	ds_fNoLineDrawDash:1;
    unsigned int	ds_fOleIcon:1;
    unsigned int	ds_fOnDblClickNotify:1;
    unsigned int	ds_fOneD:1;
    unsigned int	ds_fPreferRelativeSize:1;
    unsigned int	ds_fPrint:1;

    unsigned int	ds_fPseudoInline:1;

				/****************************************/
				/*  Text box.				*/
				/****************************************/
    int				ds_dxTextLeft;
    int				ds_dyTextTop;
    int				ds_dxTextRight;
    int				ds_dyTextBottom;

    int				ds_scaleText;
    int				ds_lTxid;

    int				ds_WrapText:8;
    int				ds_anchorText:8;
    int				ds_txflTextFlow:8;
    int				ds_cdirFont:8;

    unsigned int		ds_fAutoTextMargin:1;
    unsigned int		ds_fRotateText:1;
    unsigned int		ds_fSelectText:1;
    unsigned int		ds_fFitShapeToText:1;
    unsigned int		ds_fFitTextToShape:1;

			/************************************************/
			/*  Lock.					*/
			/************************************************/
    unsigned int	ds_fLockRotation:1;
    unsigned int	ds_fLockAspectRatio:1;
    unsigned int	ds_fLockAgainstSelect:1;
    unsigned int	ds_fLockCropping:1;
    unsigned int	ds_fLockVerticies:1;
    unsigned int	ds_fLockText:1;
    unsigned int	ds_fLockAdjustHandles:1;
    unsigned int	ds_fLockAgainstGrouping:1;
    unsigned int	ds_fLockShapeType:1;

    unsigned int	ds_fLockPosition:1;

			/************************************************/
			/*  Grouped Shapes				*/
			/*						*/
			/*  Relative position of children.		*/
			/*						*/
			/*  Position of groups. Actually the positions	*/
			/*  are given by shpleft,shpright,		*/
			/*  shptop,shpbottom in a root shape.		*/
			/************************************************/
    DocumentRectangle	dsRelRect;

    int			dsRelRotation;
    long		dsRegroupID;

    DocumentRectangle	dsGroupRect;

    unsigned int	ds_fRelChangePage:1;
    unsigned int	ds_fRelFlipH:1;
    unsigned int	ds_fRelFlipV:1;

			    /********************************************/
			    /*  Geometry				*/
			    /********************************************/
    DocumentRectangle	dsGeoRect;
    long		dsAdjustValue;
    long		dsAdjust2Value;
    long		dsAdjust3Value;
    long		dsAdjust4Value;
    long		dsAdjust5Value;
    long		dsAdjust6Value;
    long		dsAdjust7Value;
    long		dsAdjust8Value;
    long		dsAdjust9Value;
    long		dsAdjust10Value;

    ShapeVertex *	dsVertices;
    int			dsVertexCount;
    ShapeVertex *	dsWrapPolygonVertices;
    int			dsWrapPolygonVertexCount;
    int *		dsSegmentInfos;
    int			dsSegmentInfoCount;

    /*
    DSHPprop_pFragments,
    DSHPprop_pGuides,
    DSHPprop_pInscribe,
    DSHPprop_pAdjustHandles,
    */

			/************************************************/
			/*  Fill.					*/
			/************************************************/
    unsigned char	dsFillType;
#				define	DSfillSOLID			0
#				define	DSfillPATTERN			1
#				define	DSfillTEXTURE			2
#				define	DSfillCENTERED_PICTURE		3
#				define	DSfillSHADE_START_TO_END	4
#				define	DSfillSHADE_BOUNDS_TO_END	5
#				define	DSfillSHADE_OUTLINE_TO_END	6
#				define	DSfillSHADE_ANGLE		7
    unsigned char	dsFillblipflags;
    unsigned char	dsFillFocus;
    unsigned char	dsFillDztype;
    unsigned char	dsFillShadeType;

    int			dsFillOpacity;
    int			dsFillBackOpacity;

    int			dsFillWidthEmu;
    int			dsFillHeightEmu;
    int			dsFillAngle;

    int			dsFillToLeft;
    int			dsFillToTop;
    int			dsFillToRight;
    int			dsFillToBottom;

    int			dsFillOriginX;
    int			dsFillOriginY;
    int			dsFillShapeOriginX;
    int			dsFillShapeOriginY;

    int			dsFillRectLeftEmu;
    int			dsFillRectTopEmu;
    int			dsFillRectRightEmu;
    int			dsFillRectBottomEmu;

    long		dsFillShadePreset;

    RGB8Color		dsFillColor;
    RGB8Color		dsFillBackColor;
    RGB8Color		dsFillCrModColor;

    unsigned int	ds_fFilled:1;
    unsigned int	ds_fillShape:1;
    unsigned int	ds_fillUseRect:1;
    unsigned int	ds_fFillOK:1;
    unsigned int	ds_fFillShadeShapeOK:1;

			    /********************************************/
			    /*  Number/Flag: Line.			*/
			    /********************************************/
    unsigned int	dsLineType:8;
#				define	DSlineSOLID			0
#				define	DSlinePATTERN			1
#				define	DSlineTEXTURE			2
#				define	DSlinePICTURE			3

    unsigned int	dsLineFillBlipFlags:8;

    int			dsLineFillWidthEmu;
    int			dsLineFillHeightEmu;
    int			dsLineWidthEmu;

    unsigned int	dsLineStyle:8;
    unsigned int	dsLineDashing:8;
#				define	DSdashSOLID			0
#				define	DSdashDASHED			1
#				define	DSdashDOT			2
#				define	DSdashDASHDOT			3
#				define	DSdashDASHDOTDOT		4
#				define	DSdashDOT_X			6
#				define	DSdashDASHED_X			7
#				define	DSdashDASHED_L			8
#				define	DSdashDASHDOT_X			9
#				define	DSdashDASHDOT_L			10
#				define	DSdashDASHDOTDOT_L		11
#				define	DSdashHOLLOW			12/*do*/
    ShapeArrow		dsLineStartArrow;
    ShapeArrow		dsLineEndArrow;
    unsigned int	dsLineEndCapStyle:8;
    unsigned int	dsLineFillDztype:8;
    unsigned int	dsLineJoinStyle:8;

    int			dsLineMiterLimit;

    RGB8Color		dsLineColor;
    RGB8Color		dsLineBackColor;
    RGB8Color		dsLineCrModColor;

    unsigned int	ds_fLine:1;
    unsigned int	ds_fLineOK:1;

    unsigned int	ds_fLineUseShapeAnchor:1;
    unsigned int	ds_fColumnLineOK:1;
    unsigned int	ds_fColumnLine:1;
    unsigned int	ds_fLeftLine:1;
    unsigned int	ds_fTopLine:1;
    unsigned int	ds_fRightLine:1;
    unsigned int	ds_fBottomLine:1;

    unsigned int	ds_fColumnHitTestLine:1;
    unsigned int	ds_fLeftHitTestLine:1;
    unsigned int	ds_fTopHitTestLine:1;
    unsigned int	ds_fRightHitTestLine:1;
    unsigned int	ds_fBottomHitTestLine:1;

    unsigned int	ds_lineFillShape:1;
    unsigned int	ds_lineColumnFillShape:1;
    unsigned int	ds_lineLeftFillShape:1;
    unsigned int	ds_lineTopFillShape:1;
    unsigned int	ds_lineRightFillShape:1;
    unsigned int	ds_lineBottomFillShape:1;

    unsigned int	ds_fInsetPen:1;
    unsigned int	ds_fLeftInsetPen:1;
    unsigned int	ds_fTopInsetPen:1;
    unsigned int	ds_fRightInsetPen:1;
    unsigned int	ds_fBottomInsetPen:1;

			/************************************************/
			/*  Shadow.					*/
			/************************************************/
    unsigned int	dsShadowType:8;
    int			dsShadowOpacity;
    int			dsShadowOffsetXEmu;
    int			dsShadowOffsetYEmu;
    int			dsShadowOffset2XEmu;
    int			dsShadowOffset2YEmu;
    int			dsShadowScaleXToX;
    int			dsShadowScaleYToX;
    int			dsShadowScaleXToY;
    int			dsShadowScaleYToY;
    int			dsShadowPerspectiveX;
    int			dsShadowPerspectiveY;
    int			dsShadowWeight;
    int			dsShadowOriginX;
    int			dsShadowOriginY;

    unsigned int	ds_fShadow:1;
    unsigned int	ds_fshadowObscured:1;
    unsigned int	ds_fshadowOK:1;

    RGB8Color		dsShadowColor;
    RGB8Color		dsShadowHighlightColor;
    RGB8Color		dsShadowCrModColor;
    RGB8Color		dsShadowc3DExtrusionColor;
    RGB8Color		dsShadowc3DCrModColor;
			/************************************************/
			/*  Connectors.					*/
			/************************************************/
    unsigned int	dsConnectionSite:8;
    unsigned int	dsConnectionStyle:8;

				/****************************************/

    int				ds_shapePath;

				/****************************************/

    ExternalItem		dsExternalItem;
    struct DrawingShape **	dsChildren;
    int				dsChildCount;

    PictureProperties		dsPictureProperties;
    MemoryBuffer		dsPictureData;
    APP_BITMAP_IMAGE		dsPixmap;
    void *			dsPrivate;

    } DrawingShape;

typedef enum DrawingShapeProperty
    {
			    /****************************/
			    /*  Strings			*/
			    /****************************/
    DSHPprop_WzName,
    DSHPprop_gtextUNICODE,
    DSHPprop_gtextFont,
    DSHPprop_pibName,
			    /****************************/
			    /*  Picture			*/
			    /****************************/
    DSHPprop_pib,

			    /********************************************/
			    /*   Number/Flag: Position.			*/
			    /********************************************/
    DSHPprop_posv,
    DSHPprop_posh,
    DSHPprop_posrelv,
    DSHPprop_posrelh,

    DSHPprop_fUseShapeAnchor,
    DSHPprop_fLayoutInCell,
    DSHPprop_fAllowOverlap,
    DSHPprop_fChangePage,

			    /********************************************/
			    /*  Number/Flag: Object Type		*/
			    /********************************************/
    DSHPprop_shapeType,
    DSHPprop_rotation,

    DSHPprop_dxWrapDistLeft,
    DSHPprop_dyWrapDistTop,
    DSHPprop_dxWrapDistRight,
    DSHPprop_dyWrapDistBottom,

    DSHPprop_hspMaster,
    DSHPprop_hspNext,

    DSHPprop_xLimo,
    DSHPprop_yLimo,

    DSHPprop_fIsBullet,
    DSHPprop_fFlipV,
    DSHPprop_fFlipH,
    DSHPprop_fBehindDocument,
    DSHPprop_fIsButton,
    DSHPprop_fHidden,
    DSHPprop_fReallyHidden,
    DSHPprop_fArrowheadsOK,
    DSHPprop_fBackground,
    DSHPprop_fDeleteAttachedObject,
    DSHPprop_fEditedWrap,
    DSHPprop_fHitTestFill,
    DSHPprop_fHitTestLine,
    DSHPprop_fInitiator,
    DSHPprop_fNoFillHitTest,
    DSHPprop_fNoHitTestPicture,
    DSHPprop_fNoLineDrawDash,
    DSHPprop_fOleIcon,
    DSHPprop_fOnDblClickNotify,
    DSHPprop_fOneD,
    DSHPprop_fPreferRelativeSize,
    DSHPprop_fPrint,

    /* ? */
    DSHPprop_fPseudoInline,

				/****************************************/
				/*  Text box.				*/
				/****************************************/
    DSHPprop_dxTextLeft,
    DSHPprop_dyTextTop,
    DSHPprop_dxTextRight,
    DSHPprop_dyTextBottom,

    DSHPprop_scaleText,
    DSHPprop_lTxid,

    DSHPprop_WrapText,
    DSHPprop_anchorText,
    DSHPprop_txflTextFlow,
    DSHPprop_cdirFont,

    DSHPprop_fAutoTextMargin,
    DSHPprop_fRotateText,
    DSHPprop_fSelectText,
    DSHPprop_fFitShapeToText,
    DSHPprop_fFitTextToShape,

			    /********************************************/
			    /*  Number/Flag: Lock.			*/
			    /********************************************/
    DSHPprop_fLockRotation,
    DSHPprop_fLockAspectRatio,
    DSHPprop_fLockAgainstSelect,
    DSHPprop_fLockCropping,
    DSHPprop_fLockVerticies,
    DSHPprop_fLockText,
    DSHPprop_fLockAdjustHandles,
    DSHPprop_fLockAgainstGrouping,
    DSHPprop_fLockShapeType,

    /* ? */
    DSHPprop_fLockPosition,

			    /********************************************/
			    /*  Geometry				*/
			    /********************************************/
    DSHPprop_geoLeft,
    DSHPprop_geoTop,
    DSHPprop_geoRight,
    DSHPprop_geoBottom,
    DSHPprop_adjustValue,
    DSHPprop_adjust2Value,
    DSHPprop_adjust3Value,
    DSHPprop_adjust4Value,
    DSHPprop_adjust5Value,
    DSHPprop_adjust6Value,
    DSHPprop_adjust7Value,
    DSHPprop_adjust8Value,
    DSHPprop_adjust9Value,
    DSHPprop_adjust10Value,

    DSHPprop_pVerticies,
    DSHPprop_pWrapPolygonVertices,
    DSHPprop_pSegmentInfo,
    DSHPprop_pFragments,
    DSHPprop_pGuides,
    DSHPprop_pInscribe,
    DSHPprop_pAdjustHandles,
			    /********************************************/
			    /*  Number/Flag: Grouped			*/
			    /********************************************/
    DSHPprop_relLeft,
    DSHPprop_relTop,
    DSHPprop_relRight,
    DSHPprop_relBottom,

    DSHPprop_relRotation,
    DSHPprop_lidRegroup,

    DSHPprop_groupLeft,
    DSHPprop_groupTop,
    DSHPprop_groupRight,
    DSHPprop_groupBottom,

    DSHPprop_fRelFlipH,
    DSHPprop_fRelFlipV,
    DSHPprop_fRelChangePage,

			    /********************************************/
			    /*  Number/Flag: Fill.			*/
			    /********************************************/
    DSHPprop_fillType,
    DSHPprop_fillOpacity,
    DSHPprop_fillBackOpacity,

    DSHPprop_fillblipflags,

    DSHPprop_fillWidth,
    DSHPprop_fillHeight,
    DSHPprop_fillAngle,
    DSHPprop_fillFocus,

    DSHPprop_fillToLeft,
    DSHPprop_fillToTop,
    DSHPprop_fillToRight,
    DSHPprop_fillToBottom,

    DSHPprop_fillOriginX,
    DSHPprop_fillOriginY,
    DSHPprop_fillShapeOriginX,
    DSHPprop_fillShapeOriginY,

    DSHPprop_fillDztype,

    DSHPprop_fillRectLeft,
    DSHPprop_fillRectTop,
    DSHPprop_fillRectRight,
    DSHPprop_fillRectBottom,

    DSHPprop_fillShadePreset,
    DSHPprop_fillShadeType,

    DSHPprop_fFilled,
    DSHPprop_fillShape,
    DSHPprop_fillUseRect,
    DSHPprop_fFillOK,
    DSHPprop_fFillShadeShapeOK,

    DSHPprop_fillColor,
    DSHPprop_fillBackColor,
    DSHPprop_fillCrMod,

    DSHPprop_fillBlip,
    DSHPprop_fillBlipName,
    DSHPprop_fillShadeColors,

			    /********************************************/
			    /*  Number/Flag: Line.			*/
			    /********************************************/
    DSHPprop_lineType,

    DSHPprop_lineFillBlipFlags,

    DSHPprop_lineFillWidth,
    DSHPprop_lineFillHeight,
    DSHPprop_lineWidth,

    DSHPprop_lineStyle,
    DSHPprop_lineDashing,
    DSHPprop_lineStartArrowhead,
    DSHPprop_lineEndArrowhead,
    DSHPprop_lineStartArrowWidth,
    DSHPprop_lineStartArrowLength,
    DSHPprop_lineEndArrowWidth,
    DSHPprop_lineEndArrowLength,
    DSHPprop_lineEndCapStyle,
    DSHPprop_lineFillDztype,
    DSHPprop_lineJoinStyle,

    DSHPprop_lineMiterLimit,

    DSHPprop_fLine,
    DSHPprop_fLineOK,

    DSHPprop_fLineUseShapeAnchor,
    DSHPprop_fColumnLineOK,
    DSHPprop_fColumnLine,
    DSHPprop_fLeftLine,
    DSHPprop_fTopLine,
    DSHPprop_fRightLine,
    DSHPprop_fBottomLine,

    DSHPprop_fColumnHitTestLine,
    DSHPprop_fLeftHitTestLine,
    DSHPprop_fTopHitTestLine,
    DSHPprop_fRightHitTestLine,
    DSHPprop_fBottomHitTestLine,

    DSHPprop_lineFillShape,
    DSHPprop_lineColumnFillShape,
    DSHPprop_lineLeftFillShape,
    DSHPprop_lineTopFillShape,
    DSHPprop_lineRightFillShape,
    DSHPprop_lineBottomFillShape,

    DSHPprop_fInsetPen,
    DSHPprop_fLeftInsetPen,
    DSHPprop_fTopInsetPen,
    DSHPprop_fRightInsetPen,
    DSHPprop_fBottomInsetPen,

    DSHPprop_lineFillBlipName,
    DSHPprop_lineDashStyle,

    DSHPprop_lineColor,
    DSHPprop_lineBackColor,
    DSHPprop_lineCrMod,

    /*  ? */
    DSHPprop_lineOpacity,
    DSHPprop_lineblipflags,

			    /********************************************/
			    /*  Number/Flag: Shadow.			*/
			    /********************************************/
    DSHPprop_shadowType,
    DSHPprop_shadowOpacity,
    DSHPprop_shadowOffsetX,
    DSHPprop_shadowOffsetY,
    DSHPprop_shadowSecondOffsetX,
    DSHPprop_shadowSecondOffsetY,
    DSHPprop_shadowScaleXToX,
    DSHPprop_shadowScaleYToX,
    DSHPprop_shadowScaleXToY,
    DSHPprop_shadowScaleYToY,
    DSHPprop_shadowPerspectiveX,
    DSHPprop_shadowPerspectiveY,
    DSHPprop_shadowWeight,
    DSHPprop_shadowOriginX,
    DSHPprop_shadowOriginY,

    DSHPprop_fShadow,
    DSHPprop_fshadowObscured,
    DSHPprop_fShadowOK,

    DSHPprop_shadowColor,
    DSHPprop_shadowHighlight,
    DSHPprop_shadowCrMod,

			    /********************************************/
			    /*  Number/Flag: Connectors.		*/
			    /********************************************/
    DSHPprop_cxk,
    DSHPprop_cxstyle,

			    /****************************/
			    /*  Number/Flag		*/
			    /****************************/
    DSHPprop_gtextAlign,
    DSHPprop_fGtext,
    DSHPprop_gtextFVertical,
    DSHPprop_gtextFKern,
    DSHPprop_gtextFTight,
    DSHPprop_gtextFStretch,
    DSHPprop_gtextFShrinkFit,
    DSHPprop_gtextFBestFit,
    DSHPprop_gtextFNormalize,
    DSHPprop_gtextFDxMeasure,
    DSHPprop_gtextFBold,
    DSHPprop_gtextFItalic,
    DSHPprop_gtextFUnderline,
    DSHPprop_gtextFShadow,
    DSHPprop_gtextFSmallcaps,
    DSHPprop_gtextFStrikethrough,
    DSHPprop_pibFlags,
    DSHPprop_pictureGray,
    DSHPprop_pictureBiLevel,
    DSHPprop_c3DEdgeThickness,
    DSHPprop_c3DExtrudeForward,
    DSHPprop_c3DExtrudeBackward,
    DSHPprop_f3D,
    DSHPprop_fc3DMetallic,
    DSHPprop_fc3DUseExtrusionColor,
    DSHPprop_fc3DLightFace,
    DSHPprop_c3DRotationAxisX,
    DSHPprop_c3DRotationAxisY,
    DSHPprop_c3DRotationAxisZ,
    DSHPprop_fC3DRotationCenterAut,
    DSHPprop_c3DRotationCenterZ,
    DSHPprop_c3DXViewpoint,
    DSHPprop_c3DYViewpoint,
    DSHPprop_c3DZViewpoint,
    DSHPprop_c3DKeyX,
    DSHPprop_c3DKeyY,
    DSHPprop_c3DKeyZ,
    DSHPprop_c3DFillX,
    DSHPprop_c3DFillY,
    DSHPprop_c3DFillZ,
    DSHPprop_fc3DParallel,
    DSHPprop_fc3DKeyHarsh,
    DSHPprop_fc3DFillHarsh,
    DSHPprop_fc3DConstrainRotation,
    DSHPprop_fc3DRotationCenterAuto,
    DSHPprop_spcot,
    DSHPprop_dxyCalloutGap,
    DSHPprop_spcoa,
    DSHPprop_spcod,
    DSHPprop_dxyCalloutDropSpecified,
    DSHPprop_dxyCalloutLengthSpecified,
    DSHPprop_fCallout,
    DSHPprop_fCalloutAccentBar,
    DSHPprop_fCalloutTextBorder,
    DSHPprop_fCalloutDropAuto,
    DSHPprop_fCalloutLengthSpecified,
    DSHPprop_pictureId,
    DSHPprop_pictureActive,
    DSHPprop_pictureContrast,
    DSHPprop_pictureBrightness,
    DSHPprop_pictureDblCrMod,
    DSHPprop_pictureFillCrMod,
    DSHPprop_pictureLineCrMod,
    DSHPprop_fCalloutMinusX,
    DSHPprop_fCalloutMinusY,
    DSHPprop_shapePath,
    DSHPprop_cropFromLeft,
    DSHPprop_cropFromRight,
    DSHPprop_cropFromTop,
    DSHPprop_cropFromBottom,
    DSHPprop_bWMode,
    DSHPprop_f3DOK,
    DSHPprop_fPreferRelativeResize,
    DSHPprop_fRecolorFillAsPicture,
    DSHPprop_dgmt,
    DSHPprop_ccol,
    DSHPprop_dzColMargin,
    DSHPprop_fScriptAnchor,
    DSHPprop_fFakeMaster,
			    /****************************/
    DSHPprop_c3DAmbientIntensity,
    DSHPprop_c3DDiffuseAmt,
    DSHPprop_c3DExtrudePlane,
    DSHPprop_c3DFillIntensity,
    DSHPprop_c3DKeyIntensity,
    DSHPprop_c3DOriginX,
    DSHPprop_c3DOriginY,
    DSHPprop_c3DRenderMode,
    DSHPprop_c3DRotationAngle,
    DSHPprop_c3DRotationCenterX,
    DSHPprop_c3DRotationCenterY,
    DSHPprop_c3DShininess,
    DSHPprop_c3DSkewAmount,
    DSHPprop_c3DSkewAngle,
    DSHPprop_c3DSpecularAmt,
    DSHPprop_c3DTolerance,
    DSHPprop_c3DXRotationAngle,
    DSHPprop_c3DYRotationAngle,
    DSHPprop_c3DExtrusionColorExt,
    DSHPprop_c3DExtrusionColorExtCMY,
    DSHPprop_c3DExtrusionColorExtK,
    DSHPprop_c3DExtrusionColorExtMod,
    DSHPprop_fillColorExt,
    DSHPprop_fillColorExtCMY,
    DSHPprop_fillColorExtK,
    DSHPprop_fillColorExtMod,
    DSHPprop_lineBackColorExt,
    DSHPprop_lineBackColorExtCMY,
    DSHPprop_lineBackColorExtK,
    DSHPprop_lineBackColorExtMod,
    DSHPprop_lineBottomBackColor,
    DSHPprop_lineBottomBackColorExt,
    DSHPprop_lineBottomBackColorExtCMY,
    DSHPprop_lineBottomBackColorExtK,
    DSHPprop_lineBottomBackColorExtMod,
    DSHPprop_lineBottomColor,
    DSHPprop_lineBottomColorExt,
    DSHPprop_lineBottomColorExtCMY,
    DSHPprop_lineBottomColorExtK,
    DSHPprop_lineBottomColorExtMod,
    DSHPprop_lineBottomCrMod,
    DSHPprop_lineBottomDashing,
    DSHPprop_lineBottomEndArrowLength,
    DSHPprop_lineBottomEndArrowWidth,
    DSHPprop_lineBottomEndArrowhead,
    DSHPprop_lineBottomEndCapStyle,
    DSHPprop_lineBottomFillBlipFlags,
    DSHPprop_lineBottomFillDztype,
    DSHPprop_lineBottomFillHeight,
    DSHPprop_lineBottomFillWidth,
    DSHPprop_lineBottomMiterLimit,
    DSHPprop_lineBottomOpacity,
    DSHPprop_lineBottomStartArrowLength,
    DSHPprop_lineBottomStartArrowWidth,
    DSHPprop_lineBottomStartArrowhead,
    DSHPprop_lineBottomStyle,
    DSHPprop_lineBottomType,
    DSHPprop_lineBottomWidth,
    DSHPprop_lineColorExt,
    DSHPprop_lineColorExtCMY,
    DSHPprop_lineColorExtK,
    DSHPprop_lineColorExtMod,
    DSHPprop_lineColumnBackColor,
    DSHPprop_lineColumnBackColorExt,
    DSHPprop_lineColumnBackColorExtCMY,
    DSHPprop_lineColumnBackColorExtK,
    DSHPprop_lineColumnBackColorExtMod,
    DSHPprop_lineColumnColor,
    DSHPprop_lineColumnColorExt,
    DSHPprop_lineColumnColorExtCMY,
    DSHPprop_lineColumnColorExtK,
    DSHPprop_lineColumnColorExtMod,
    DSHPprop_lineColumnCrMod,
    DSHPprop_lineColumnDashing,
    DSHPprop_lineColumnEndArrowLength,
    DSHPprop_lineColumnEndArrowWidth,
    DSHPprop_lineColumnEndArrowhead,
    DSHPprop_lineColumnEndCapStyle,
    DSHPprop_lineColumnFillBlipFlags,
    DSHPprop_lineColumnFillDztype,
    DSHPprop_lineColumnFillHeight,
    DSHPprop_lineColumnFillWidth,
    DSHPprop_lineColumnMiterLimit,
    DSHPprop_lineColumnOpacity,
    DSHPprop_lineColumnStartArrowLength,
    DSHPprop_lineColumnStartArrowWidth,
    DSHPprop_lineColumnStartArrowhead,
    DSHPprop_lineColumnStyle,
    DSHPprop_lineColumnType,
    DSHPprop_lineColumnWidth,
    DSHPprop_lineLeftBackColor,
    DSHPprop_lineLeftBackColorExt,
    DSHPprop_lineLeftBackColorExtCMY,
    DSHPprop_lineLeftBackColorExtK,
    DSHPprop_lineLeftBackColorExtMod,
    DSHPprop_lineLeftColor,
    DSHPprop_lineLeftColorExt,
    DSHPprop_lineLeftColorExtCMY,
    DSHPprop_lineLeftColorExtK,
    DSHPprop_lineLeftColorExtMod,
    DSHPprop_lineLeftCrMod,
    DSHPprop_lineLeftDashing,
    DSHPprop_lineLeftEndArrowLength,
    DSHPprop_lineLeftEndArrowWidth,
    DSHPprop_lineLeftEndArrowhead,
    DSHPprop_lineLeftEndCapStyle,
    DSHPprop_lineLeftFillBlipFlags,
    DSHPprop_lineLeftFillDztype,
    DSHPprop_lineLeftFillHeight,
    DSHPprop_lineLeftFillWidth,
    DSHPprop_lineLeftMiterLimit,
    DSHPprop_lineLeftOpacity,
    DSHPprop_lineLeftStartArrowLength,
    DSHPprop_lineLeftStartArrowWidth,
    DSHPprop_lineLeftStartArrowhead,
    DSHPprop_lineLeftStyle,
    DSHPprop_lineLeftType,
    DSHPprop_lineLeftWidth,
    DSHPprop_lineRightBackColor,
    DSHPprop_lineRightBackColorExt,
    DSHPprop_lineRightBackColorExtCMY,
    DSHPprop_lineRightBackColorExtK,
    DSHPprop_lineRightBackColorExtMod,
    DSHPprop_lineRightColor,
    DSHPprop_lineRightColorExt,
    DSHPprop_lineRightColorExtCMY,
    DSHPprop_lineRightColorExtK,
    DSHPprop_lineRightColorExtMod,
    DSHPprop_lineRightCrMod,
    DSHPprop_lineRightDashing,
    DSHPprop_lineRightEndArrowLength,
    DSHPprop_lineRightEndArrowWidth,
    DSHPprop_lineRightEndArrowhead,
    DSHPprop_lineRightEndCapStyle,
    DSHPprop_lineRightFillBlipFlags,
    DSHPprop_lineRightFillDztype,
    DSHPprop_lineRightFillHeight,
    DSHPprop_lineRightFillWidth,
    DSHPprop_lineRightMiterLimit,
    DSHPprop_lineRightOpacity,
    DSHPprop_lineRightStartArrowLength,
    DSHPprop_lineRightStartArrowWidth,
    DSHPprop_lineRightStartArrowhead,
    DSHPprop_lineRightStyle,
    DSHPprop_lineRightType,
    DSHPprop_lineRightWidth,
    DSHPprop_lineTopBackColor,
    DSHPprop_lineTopBackColorExt,
    DSHPprop_lineTopBackColorExtCMY,
    DSHPprop_lineTopBackColorExtK,
    DSHPprop_lineTopBackColorExtMod,
    DSHPprop_lineTopColor,
    DSHPprop_lineTopColorExt,
    DSHPprop_lineTopColorExtCMY,
    DSHPprop_lineTopColorExtK,
    DSHPprop_lineTopColorExtMod,
    DSHPprop_lineTopCrMod,
    DSHPprop_lineTopDashing,
    DSHPprop_lineTopEndArrowLength,
    DSHPprop_lineTopEndArrowWidth,
    DSHPprop_lineTopEndArrowhead,
    DSHPprop_lineTopEndCapStyle,
    DSHPprop_lineTopFillBlipFlags,
    DSHPprop_lineTopFillDztype,
    DSHPprop_lineTopFillHeight,
    DSHPprop_lineTopFillWidth,
    DSHPprop_lineTopMiterLimit,
    DSHPprop_lineTopOpacity,
    DSHPprop_lineTopStartArrowLength,
    DSHPprop_lineTopStartArrowWidth,
    DSHPprop_lineTopStartArrowhead,
    DSHPprop_lineTopStyle,
    DSHPprop_lineTopType,
    DSHPprop_lineTopWidth,
    DSHPprop_shadowColorExt,
    DSHPprop_shadowColorExtCMY,
    DSHPprop_shadowColorExtK,
    DSHPprop_shadowColorExtMod,
    DSHPprop_shadowHighlightExt,
    DSHPprop_shadowHighlightExtCMY,
    DSHPprop_shadowHighlightExtK,
    DSHPprop_shadowHighlightExtMod,
			    /****************************/
			    /*  Color			*/
			    /****************************/
    DSHPprop_pictureTransparent,
    DSHPprop_c3DExtrusionColor,
    DSHPprop_c3DCrMod,

    DSHPprop_COUNT
    } DrawingShapeProperty;

/************************************************************************/
/*									*/
/*  Kinds of shape.							*/
/*									*/
/************************************************************************/

typedef enum ShapeType
    {
    SHPtyFREE= -1,

    SHPtyFREEFORM_OR_NON_AUTOSHAPE= 0,
    SHPtyRECTANGLE= 1,
    SHPtyROUND_RECTANGLE= 2,
    SHPtyELLIPSE= 3,
    SHPtyDIAMOND= 4,
    SHPtyISOSCELES_TRIANGLE= 5,
    SHPtyRIGHT_TRIANGLE= 6,
    SHPtyPARALLELOGRAM= 7,
    SHPtyTRAPEZOID= 8,
    SHPtyHEXAGON= 9,
    SHPtyOCTAGON= 10,
    SHPtyPLUS_SIGN= 11,
    SHPtySTAR= 12,
    SHPtyARROW= 13,
    SHPtyTHICK_ARROW= 14,
    SHPtyHOME_PLATE= 15,
    SHPtyCUBE= 16,
    SHPtyBALLOON= 17,
    SHPtySEAL= 18,
    SHPtyARC= 19,
    SHPtyLINE= 20,
    SHPtyPLAQUE= 21,
    SHPtyCAN= 22,
    SHPtyDONUT= 23,
    SHPtyTEXT_SIMPLE= 24,
    SHPtyTEXT_OCTAGON= 25,
    SHPtyTEXT_HEXAGON= 26,
    SHPtyTEXT_CURVE= 27,
    SHPtyTEXT_WAVE= 28,
    SHPtyTEXT_RING= 29,
    SHPtyTEXT_ON_CURVE= 30,
    SHPtyTEXT_ON_RING= 31,
    SHPtyCALLOUT_1= 41,
    SHPtyCALLOUT_2= 42,
    SHPtyCALLOUT_3= 43,
    SHPtyACCENT_CALLOUT_1= 44,
    SHPtyACCENT_CALLOUT_2= 45,
    SHPtyACCENT_CALLOUT_3= 46,
    SHPtyBORDER_CALLOUT_1= 47,
    SHPtyBORDER_CALLOUT_2= 48,
    SHPtyBORDER_CALLOUT_3= 49,
    SHPtyACCENT_BORDER_CALLOUT_1= 50,
    SHPtyACCENT_BORDER_CALLOUT_2= 51,
    SHPtyACCENT_BORDER_CALLOUT_3= 52,
    SHPtyRIBBON= 53,
    SHPtyRIBBON2= 54,
    SHPtyCHEVRON= 55,
    SHPtyPENTAGON= 56,
    SHPtyNO_SMOKING= 57,
    SHPtySEAL8= 58,
    SHPtySEAL16= 59,
    SHPtySEAL32= 60,
    SHPtyWEDGE_RECT_CALLOUT= 61,
    SHPtyWEDGE_RRECT_CALLOUT= 62,
    SHPtyWEDGE_ELLIPSE_CALLOUT= 63,
    SHPtyWAVE= 64,
    SHPtyFOLDED_CORNER= 65,
    SHPtyLEFT_ARROW= 66,
    SHPtyDOWN_ARROW= 67,
    SHPtyUP_ARROW= 68,
    SHPtyLEFT_RIGHT_ARROW= 69,
    SHPtyUP_DOWN_ARROW= 70,
    SHPtyIRREGULARSEAL1= 71,
    SHPtyIRREGULARSEAL2= 72,
    SHPtyLIGHTNING_BOLT= 73,
    SHPtyHEART= 74,
    SHPtyPICTURE_FRAME= 75,
    SHPtyQUAD_ARROW= 76,
    SHPtyLEFT_ARROW_CALLOUT= 77,
    SHPtyRIGHT_ARROW_CALLOUT= 78,
    SHPtyUP_ARROW_CALLOUT= 79,
    SHPtyDOWN_ARROW_CALLOUT= 80,
    SHPtyLEFT_RIGHT_ARROW_CALLOUT= 81,
    SHPtyUP_DOWN_ARROW_CALLOUT= 82,
    SHPtyQUAD_ARROW_CALLOUT= 83,
    SHPtyBEVEL= 84,
    SHPtyLEFT_BRACKET= 85,
    SHPtyRIGHT_BRACKET= 86,
    SHPtyLEFT_BRACE= 87,
    SHPtyRIGHT_BRACE= 88,
    SHPtyLEFT_UP_ARROW= 89,
    SHPtyBENT_UP_ARROW= 90,
    SHPtyBENT_ARROW= 91,
    SHPtySEAL24= 92,
    SHPtySTRIPED_RIGHT_ARROW= 93,
    SHPtyNOTCHED_RIGHT_ARROW= 94,
    SHPtyBLOCK_ARC= 95,
    SHPtySMILEY_FACE= 96,
    SHPtyVERTICAL_SCROLL= 97,
    SHPtyHORIZONTAL_SCROLL= 98,
    SHPtyCIRCULAR_ARROW= 99,
    SHPtyNOTCHED_CIRCULAR_ARROW= 100,
    SHPtyUTURN_ARROW= 101,
    SHPtyCURVED_RIGHT_ARROW= 102,
    SHPtyCURVED_LEFT_ARROW= 103,
    SHPtyCURVED_UP_ARROW= 104,
    SHPtyCURVED_DOWN_ARROW= 105,
    SHPtyCLOUD_CALLOUT= 106,
    SHPtyELLIPSE_RIBBON= 107,
    SHPtyELLIPSE_RIBBON_2= 108,
    SHPtyFLOW_CHART_PROCESS= 109,
    SHPtyFLOW_CHART_DECISION= 110,
    SHPtyFLOW_CHART_INPUT_OUTPUT= 111,
    SHPtyFLOW_CHART_PREDEFINED_PROCESS= 112,
    SHPtyFLOW_CHART_INTERNAL_STORAGE= 113,
    SHPtyFLOW_CHART_DOCUMENT= 114,
    SHPtyFLOW_CHART_MULTIDOCUMENT= 115,
    SHPtyFLOW_CHART_TERMINATOR= 116,
    SHPtyFLOW_CHART_PREPARATION= 117,
    SHPtyFLOW_CHART_MANUAL_INPUT= 118,
    SHPtyFLOW_CHART_MANUAL_OPERATION= 119,
    SHPtyFLOW_CHART_CONNECTOR= 120,
    SHPtyFLOW_CHART_PUNCHED_CARD= 121,
    SHPtyFLOW_CHART_PUNCHED_TAPE= 122,
    SHPtyFLOW_CHART_SUMMING_JUNCTION= 123,
    SHPtyFLOW_CHART_OR= 124,
    SHPtyFLOW_CHART_COLLATE= 125,
    SHPtyFLOW_CHART_SORT= 126,
    SHPtyFLOW_CHART_EXTRACT= 127,
    SHPtyFLOW_CHART_MERGE= 128,
    SHPtyFLOW_CHART_OFFLINE_STORAGE= 129,
    SHPtyFLOW_CHART_ONLINE_STORAGE= 130,
    SHPtyFLOW_CHART_MAGNETIC_TAPE= 131,
    SHPtyFLOW_CHART_MAGNETIC_DISK= 132,
    SHPtyFLOW_CHART_MAGNETIC_DRUM= 133,
    SHPtyFLOW_CHART_DISPLAY= 134,
    SHPtyFLOW_CHART_DELAY= 135,
    SHPtyTEXT_PLAIN_TEXT= 136,
    SHPtyTEXT_STOP= 137,
    SHPtyTEXT_TRIANGLE= 138,
    SHPtyTEXT_TRIANGLE_INVERTED= 139,
    SHPtyTEXT_CHEVRON= 140,
    SHPtyTEXT_CHEVRON_INVERTED= 141,
    SHPtyTEXT_RING_INSIDE= 142,
    SHPtyTEXT_RING_OUTSIDE= 143,
    SHPtyTEXT_ARCH_UP_CURVE= 144,
    SHPtyTEXT_ARCH_DOWN_CURVE= 145,
    SHPtyTEXT_CIRCLE_CURVE= 146,
    SHPtyTEXT_BUTTON_CURVE= 147,
    SHPtyTEXT_ARCH_UP_POUR= 148,
    SHPtyTEXT_ARCH_DOWN_POUR= 149,
    SHPtyTEXT_CIRCLE_POUR= 150,
    SHPtyTEXT_BUTTON_POUR= 151,
    SHPtyTEXT_CURVE_UP= 152,
    SHPtyTEXT_CURVE_DOWN= 153,
    SHPtyTEXT_CASCADE_UP= 154,
    SHPtyTEXT_CASCADE_DOWN= 155,
    SHPtyTEXT_WAVE1= 156,
    SHPtyTEXT_WAVE2= 157,
    SHPtyTEXT_WAVE3= 158,
    SHPtyTEXT_WAVE4= 159,
    SHPtyTEXT_INFLATE= 160,
    SHPtyTEXT_DEFLATE= 161,
    SHPtyTEXT_INFLATE_BOTTOM= 162,
    SHPtyTEXT_DEFLATE_BOTTOM= 163,
    SHPtyTEXT_INFLATE_TOP= 164,
    SHPtyTEXT_DEFLATE_TOP= 165,
    SHPtyTEXT_DEFLATE_INFLATE= 166,
    SHPtyTEXT_DEFLATE_INFLATE_DEFLATE= 167,
    SHPtyTEXT_FADE_RIGHT= 168,
    SHPtyTEXT_FADE_LEFT= 169,
    SHPtyTEXT_FADE_UP= 170,
    SHPtyTEXT_FADE_DOWN= 171,
    SHPtyTEXT_SLANT_UP= 172,
    SHPtyTEXT_SLANT_DOWN= 173,
    SHPtyTEXT_CAN_UP= 174,
    SHPtyTEXT_CAN_DOWN= 175,
    SHPtyFLOW_CHART_ALTERNATE_PROCESS= 176,
    SHPtyFLOW_CHART_OFF_PAGE_CONNECTOR= 177,
    SHPtyCALLOUT_90= 178,
    SHPtyACCENT_CALLOUT_90= 179,
    SHPtyBORDER_CALLOUT_90= 180,
    SHPtyACCENT_BORDER_CALLOUT_90= 181,
    SHPtyLEFT_RIGHT_UP_ARROW= 182,
    SHPtySUN= 183,
    SHPtyMOON= 184,
    SHPtyBRACKET_PAIR= 185,
    SHPtyBRACE_PAIR= 186,
    SHPtySEAL4= 187,
    SHPtyDOUBLE_WAVE= 188,
    SHPtyHOST_CONTROL= 201,
    SHPtyTEXT_BOX= 202,

    SHPtyUNKNOWN,
    SHPtyGROUP,
    SHPtyPICPROP,

    SHPty_COUNT
    } ShapeType;

/************************************************************************/
/*									*/
/*  Descriptions of shapes.						*/
/*									*/
/************************************************************************/

extern const ShapePath	SP_33;
extern const ShapePath	SP_34;
extern const ShapePath	SP_LINE;
extern const ShapePath	SP_RECTANGLE;
extern const ShapePath	SP_DIAMOND;
extern const ShapePath	SP_ISOSCELES_TRIANGLE;
extern const ShapePath	SP_RIGHT_TRIANGLE;
extern const ShapePath	SP_PARALLELOGRAM;
extern const ShapePath	SP_TRAPEZOID;
extern const ShapePath	SP_HEXAGON;
extern const ShapePath	SP_OCTAGON;
extern const ShapePath	SP_PLUS_SIGN;
extern const ShapePath	SP_ARROW;
extern const ShapePath	SP_NOTCHED_RIGHT_ARROW;
extern const ShapePath	SP_HOME_PLATE;
extern const ShapePath	SP_CHEVRON;
extern const ShapePath	SP_LEFT_ARROW;
extern const ShapePath	SP_RIGHT_ARROW_CALLOUT;
extern const ShapePath	SP_LEFT_ARROW_CALLOUT;
extern const ShapePath	SP_UP_ARROW_CALLOUT;
extern const ShapePath	SP_LEFT_RIGHT_ARROW_CALLOUT;
extern const ShapePath	SP_UP_DOWN_ARROW_CALLOUT;
extern const ShapePath	SP_DOWN_ARROW_CALLOUT;
extern const ShapePath	SP_QUAD_ARROW_CALLOUT;
extern const ShapePath	SP_LEFT_RIGHT_ARROW;
extern const ShapePath	SP_UP_ARROW;
extern const ShapePath	SP_DOWN_ARROW;
extern const ShapePath	SP_UP_DOWN_ARROW;
extern const ShapePath	SP_QUAD_ARROW;
extern const ShapePath	SP_LEFT_RIGHT_UP_ARROW;
extern const ShapePath	SP_LEFT_UP_ARROW;
extern const ShapePath	SP_BENT_UP_ARROW;
extern const ShapePath	SP_PENTAGON;
extern const ShapePath	SP_FLOW_CHART_PUNCHED_CARD;
extern const ShapePath	SP_FLOW_CHART_MANUAL_INPUT;
extern const ShapePath	SP_FLOW_CHART_OFF_PAGE_CONNECTOR;
extern const ShapePath	SP_WEDGE_RECT_CALLOUT;
extern const ShapePath	SP_STAR;
extern const ShapePath	SP_SEAL4;
extern const ShapePath	SP_FLOW_CHART_MERGE;

# define DSflipHORIZONTAL( ds ) \
    ( ( ds->dsIsChildShape && ds->ds_fRelFlipH ) || ds->ds_fFlipH )

# define DSflipVERTICAL( ds ) \
    ( ( ds->dsIsChildShape && ds->ds_fRelFlipV ) || ds->ds_fFlipV )

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitDrawingShape(	DrawingShape *		ds );
extern void docInitDrawingShapeAllocated(
					DrawingShape *		ds );

extern void docCleanDrawingShape(	struct BufferDocument *	bd,
					DrawingShape *		ds );

extern void docFreeDrawingShape(	struct BufferDocument *	bd,
					DrawingShape *		ds );

DrawingShape * docInsertShapeInParent(	DrawingShape *		parent,
					int			n,
					int			kind );

extern const char * docShapeTypeString(	int	shapeType );

extern void docShapeInternalTransform(	AffineTransform2D *	at,
					const DrawingShape *	ds );

extern void docShapeArrowSizesTwips(	int *			pLength,
					int *			pLength2,
					int *			pWidth,
					const ShapeArrow *	sa );

extern void docInitShapeArrow(		ShapeArrow *		sa );

#   endif	/*  DOC_SHAPE_H	*/
