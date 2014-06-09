/************************************************************************/
/*									*/
/*  Shapes.. Administration.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<math.h>

#   include	<utilAffineTransform.h>

#   include	"docShape.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Return sizes for arrows in twips.					*/
/*									*/
/************************************************************************/

void docShapeArrowSizesTwips(	int *			pLength,
				int *			pLength2,
				int *			pWidth,
				const ShapeArrow *	sa )
    {
    switch( sa->saArrowLength )
	{
	case DSarrowSHORT:
	    *pLength= 6;
	    *pLength2= 9;
	    break;
	case DSarrowMEDIUM:
	    *pLength= 8;
	    *pLength2= 12;
	    break;
	case DSarrowLONG:
	    *pLength= 10;
	    *pLength2= 15;
	    break;
	default:
	    LDEB(sa->saArrowLength);
	    *pLength= 9;
	    *pLength2= 13;
	}

    switch( sa->saArrowWidth )
	{
	case DSarrowNARROW:
	    *pWidth= 4;
	    break;
	case DSarrowMEDIUM:
	    *pWidth= 6;
	    break;
	case DSarrowWIDE:
	    *pWidth= 8;
	    break;
	default:
	    LDEB(sa->saArrowWidth);
	    *pWidth= 6;
	}

    *pLength *= 15;
    *pLength2 *= 15;
    *pWidth *= 15;

    return;
    }

/************************************************************************/
/*									*/
/*  Return the transform for a shape.					*/
/*									*/
/************************************************************************/

void docShapeInternalTransform(	AffineTransform2D *	at,
				const DrawingShape *	ds )
    {
    if  ( ds->dsRotation != 0 )
	{
	utilRotationAffineTransform2D( at, ( M_PI* ds->dsRotation )/
							( 180.0 * 65536.0 ) );
	}
    else{
	utilIdentityAffineTransform2D( at );
	}

    if  ( DSflipHORIZONTAL( ds ) )
	{
	AffineTransform2D	att;

	utilScaleAffineTransform2D( &att, -1.0, 1.0 );
	utilAffineTransform2DProduct( at, at, &att );
	}

    if  ( DSflipVERTICAL( ds ) )
	{
	AffineTransform2D	att;

	utilScaleAffineTransform2D( &att, 1.0, -1.0 );
	utilAffineTransform2DProduct( at, at, &att );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Initialize/Clean a shape.						*/
/*									*/
/************************************************************************/

void docInitDrawingShapeAllocated(	DrawingShape *	ds )
    {
    utilInitMemoryBuffer( &(ds->dsPictureData) );

    ds->dsChildCount= 0;
    ds->dsChildren= (DrawingShape **)0;

    ds->dsVertexCount= 0;
    ds->dsVertices= (ShapeVertex *)0;

    ds->dsWrapPolygonVertexCount= 0;
    ds->dsWrapPolygonVertices= (ShapeVertex *)0;

    ds->dsSegmentInfoCount= 0;
    ds->dsSegmentInfos= (int *)0;

    docInitExternalItem( &(ds->dsExternalItem) );

    ds->dsPixmap= (APP_BITMAP_IMAGE)0;
    ds->dsPrivate= (void *)0;

    return;
    }

void docCleanDrawingShape(	struct BufferDocument *	bd,
				DrawingShape *		ds )
    {
    int		i;

    utilCleanMemoryBuffer( &(ds->dsPictureData) );

    for ( i= 0; i < ds->dsChildCount; i++ )
	{ docCleanDrawingShape( bd, ds->dsChildren[i] ); }

    if  ( ds->dsChildren )
	{ free( ds->dsChildren ); }

    if  ( ds->dsVertices )
	{ free( ds->dsVertices ); }
    if  ( ds->dsWrapPolygonVertices )
	{ free( ds->dsWrapPolygonVertices ); }
    if  ( ds->dsSegmentInfos )
	{ free( ds->dsSegmentInfos ); }

    docCleanExternalItem( bd, &(ds->dsExternalItem) );

    return;
    }

void docFreeDrawingShape(	struct BufferDocument *	bd,
				DrawingShape *		ds )
    {
    docCleanDrawingShape( bd, ds );
    free( ds );
    }

void docInitShapeArrow(	ShapeArrow *	sa )
    {
    sa->saArrowHead= 0;
    sa->saArrowWidth= 1;
    sa->saArrowLength= 1;
    }

void docInitDrawingShape(	DrawingShape *	ds )
    {
    docInitShapeProperties( &(ds->dsShapeProperties) );

			/************************************************/
			/*  Position.					*/
			/************************************************/
    ds->dsIsChildShape= 0;

    ds->dsHorizontalAlignment= 0;
    ds->dsHorizontalRelativeTo= SHPbxCOLUMN;
    ds->dsVerticalAlignment= 0;
    ds->dsVerticalRelativeTo= SHPbyPARA;

    ds->ds_fUseShapeAnchor= 1;
    ds->ds_fLayoutInCell= 0;
    ds->ds_fAllowOverlap= 1;
    ds->ds_fChangePage= 0;

			/************************************************/
			/*  Object Type.				*/
			/************************************************/
    ds->dsShapeType= SHPtyFREE;
    ds->dsRotation= 0;

    ds->dsWrapDistLeftEmu= 114305;
    ds->dsWrapDistTopEmu= 0;
    ds->dsWrapDistRightEmu= 114305;
    ds->dsWrapDistBottomEmu= 0;

    ds->dsMasterShape= 0;
    ds->dsNextShape= 0;

    ds->dsXLimo= 0;
    ds->dsYLimo= 0;

    ds->ds_fIsBullet= 0;
    ds->ds_fFlipH= 0;
    ds->ds_fFlipV= 0;
    ds->ds_fBehindDocument= 0;
    ds->ds_fIsButton= 0;
    ds->ds_fHidden= 0;
    ds->ds_fReallyHidden= 0;
    ds->ds_fArrowheadsOK= 0;
    ds->ds_fBackground= 0;
    ds->ds_fDeleteAttachedObject= 0;
    ds->ds_fEditedWrap= 0;
    ds->ds_fHitTestFill= 1;
    ds->ds_fHitTestLine= 1;
    ds->ds_fInitiator= 0;
    ds->ds_fNoFillHitTest= 0;
    ds->ds_fNoHitTestPicture= 0;
    ds->ds_fNoLineDrawDash= 0;
    ds->ds_fOleIcon= 0;
    ds->ds_fOnDblClickNotify= 0;
    ds->ds_fOneD= 0;
    ds->ds_fPreferRelativeSize= 0;
    ds->ds_fPrint= 1;

    ds->ds_fPseudoInline= 0;

				/****************************************/
				/*  Text box.				*/
				/****************************************/
    ds->ds_dxTextLeft= 91440;
    ds->ds_dyTextTop= 45720;
    ds->ds_dxTextRight= 91440;
    ds->ds_dyTextBottom= 45720;

    ds->ds_scaleText= 0;
    ds->ds_lTxid= 0;

    ds->ds_WrapText= 0;
    ds->ds_anchorText= 0;
    ds->ds_txflTextFlow= 0;
    ds->ds_cdirFont= 0;

    ds->ds_fAutoTextMargin= 0;
    ds->ds_fRotateText= 0;
    ds->ds_fSelectText= 1;
    ds->ds_fFitShapeToText= 0;
    ds->ds_fFitTextToShape= 0;

			/************************************************/
			/*  Lock.					*/
			/************************************************/
    ds->ds_fLockRotation= 0;
    ds->ds_fLockAspectRatio= 0;
    ds->ds_fLockAgainstSelect= 0;
    ds->ds_fLockCropping= 0;
    ds->ds_fLockVerticies= 0;
    ds->ds_fLockText= 0;
    ds->ds_fLockAdjustHandles= 0;
    ds->ds_fLockAgainstGrouping= 0;
    ds->ds_fLockShapeType= 0;

    ds->ds_fLockPosition= 0;

			/************************************************/
			/*  Fill.					*/
			/************************************************/
    ds->dsFillType= 0;
    ds->dsFillblipflags= 0;
    ds->dsFillFocus= 0;
    ds->dsFillDztype= 0;
    ds->dsFillShadeType= 0;

    ds->dsFillOpacity= 65536;
    ds->dsFillBackOpacity= 65536;

    ds->dsFillWidthEmu= 0;
    ds->dsFillHeightEmu= 0;
    ds->dsFillAngle= 0;

    ds->dsFillToLeft= 0;
    ds->dsFillToTop= 0;
    ds->dsFillToRight= 0;
    ds->dsFillToBottom= 0;

    ds->dsFillOriginX= 0;
    ds->dsFillOriginY= 0;
    ds->dsFillShapeOriginX= 0;
    ds->dsFillShapeOriginY= 0;

    ds->dsFillRectLeftEmu= 0;
    ds->dsFillRectTopEmu= 0;
    ds->dsFillRectRightEmu= 0;
    ds->dsFillRectBottomEmu= 0;

    ds->dsFillShadePreset= 0;

    bmInitRGB8Color( &(ds->dsFillColor) );
    bmInitRGB8Color( &(ds->dsFillBackColor) );
    bmInitRGB8Color( &(ds->dsFillCrModColor) );

    ds->dsFillColor.rgb8Red= 255;
    ds->dsFillColor.rgb8Green= 255;
    ds->dsFillColor.rgb8Blue= 255;

    ds->dsFillBackColor.rgb8Red= 255;
    ds->dsFillBackColor.rgb8Green= 255;
    ds->dsFillBackColor.rgb8Blue= 255;

    ds->ds_fFilled= 1;
    ds->ds_fillShape= 1;
    ds->ds_fillUseRect= 0;
    ds->ds_fFillOK= 1;
    ds->ds_fFillShadeShapeOK= 0;

			    /********************************************/
			    /*  Number/Flag: Line.			*/
			    /********************************************/
    ds->dsLineType= 0;

    ds->dsLineFillBlipFlags= 0;

    ds->dsLineFillWidthEmu= 0;
    ds->dsLineFillHeightEmu= 0;
    ds->dsLineWidthEmu= 9525;

    ds->dsLineStyle= 0;
    ds->dsLineDashing= 0;
    docInitShapeArrow( &(ds->dsLineStartArrow) );
    docInitShapeArrow( &(ds->dsLineEndArrow) );
    ds->dsLineEndCapStyle= 2;
    ds->dsLineFillDztype= 0;
    ds->dsLineJoinStyle= 2;

    ds->dsLineMiterLimit= 524288;

    bmInitRGB8Color( &(ds->dsLineColor) );
    bmInitRGB8Color( &(ds->dsLineBackColor) );
    bmInitRGB8Color( &(ds->dsLineCrModColor) );

    ds->ds_fLine= 1;
    ds->ds_fLineOK= 1;

    ds->ds_fLineUseShapeAnchor= 0;
    ds->ds_fColumnLineOK= 0;
    ds->ds_fColumnLine= 0;
    ds->ds_fLeftLine= 0;
    ds->ds_fTopLine= 0;
    ds->ds_fRightLine= 0;
    ds->ds_fBottomLine= 0;

    ds->ds_fColumnHitTestLine= 0;
    ds->ds_fLeftHitTestLine= 0;
    ds->ds_fTopHitTestLine= 0;
    ds->ds_fRightHitTestLine= 0;
    ds->ds_fBottomHitTestLine= 0;

    ds->ds_lineFillShape= 0;
    ds->ds_lineColumnFillShape= 0;
    ds->ds_lineLeftFillShape= 0;
    ds->ds_lineTopFillShape= 0;
    ds->ds_lineRightFillShape= 0;
    ds->ds_lineBottomFillShape= 0;

    ds->ds_fInsetPen= 0;
    ds->ds_fLeftInsetPen= 0;
    ds->ds_fTopInsetPen= 0;
    ds->ds_fRightInsetPen= 0;
    ds->ds_fBottomInsetPen= 0;

				/****************************************/
				/*  Text box.				*/
				/****************************************/
    ds->dsGeoRect.drX0= 0;
    ds->dsGeoRect.drY0= 0;
    ds->dsGeoRect.drX1= 21600;
    ds->dsGeoRect.drY1= 21600;
    ds->dsAdjustValue= 0;
    ds->dsAdjust2Value= 0;
    ds->dsAdjust3Value= 0;
    ds->dsAdjust4Value= 0;
    ds->dsAdjust5Value= 0;
    ds->dsAdjust6Value= 0;
    ds->dsAdjust7Value= 0;
    ds->dsAdjust8Value= 0;
    ds->dsAdjust9Value= 0;
    ds->dsAdjust10Value= 0;

				/****************************************/
				/*  Grouped Shapes			*/
				/****************************************/
    ds->dsRelRect.drX0= 0;
    ds->dsRelRect.drY0= 0;
    ds->dsRelRect.drX1= 1;
    ds->dsRelRect.drY1= 1;

    ds->dsRelRotation= 0;
    ds->dsRegroupID= 0;

    ds->dsGroupRect.drX0= 0;
    ds->dsGroupRect.drY0= 0;
    ds->dsGroupRect.drX1= 20000;
    ds->dsGroupRect.drY1= 20000;

    ds->ds_fRelChangePage= 0;
    ds->ds_fRelFlipH= 0;
    ds->ds_fRelFlipV= 0;

			/************************************************/
			/*  Shadow.					*/
			/************************************************/
    ds->dsShadowType= 0;

    ds->dsShadowOpacity= 65536;
    ds->dsShadowOffsetXEmu= 0;
    ds->dsShadowOffsetYEmu= 0;
    ds->dsShadowOffset2XEmu= 25400;
    ds->dsShadowOffset2YEmu= 25400;
    ds->dsShadowScaleXToX= 65536;
    ds->dsShadowScaleYToX= 0;
    ds->dsShadowScaleXToY= 0;
    ds->dsShadowScaleYToY= 65536;
    ds->dsShadowPerspectiveX= 0;
    ds->dsShadowPerspectiveY= 0;
    ds->dsShadowWeight= 32768;
    ds->dsShadowOriginX= 0;
    ds->dsShadowOriginY= 0;

    ds->ds_fShadow= 0;
    ds->ds_fshadowObscured= 0;
    ds->ds_fshadowOK= 1;

    bmInitRGB8Color( &(ds->dsShadowColor) );
    bmInitRGB8Color( &(ds->dsShadowHighlightColor) );
    bmInitRGB8Color( &(ds->dsShadowCrModColor) );
    bmInitRGB8Color( &(ds->dsShadowc3DExtrusionColor) );
    bmInitRGB8Color( &(ds->dsShadowc3DCrModColor) );

    ds->dsShadowColor.rgb8Red= 128;
    ds->dsShadowColor.rgb8Green= 128;
    ds->dsShadowColor.rgb8Blue= 128;

    ds->dsShadowHighlightColor.rgb8Red= 203;
    ds->dsShadowHighlightColor.rgb8Green= 203;
    ds->dsShadowHighlightColor.rgb8Blue= 203;

			/************************************************/
			/*  Connectors.					*/
			/************************************************/
    ds->dsConnectionSite= 1;
    ds->dsConnectionStyle= 3;

    docInitPictureProperties( &(ds->dsPictureProperties) );

			/************************************************/
    docInitDrawingShapeAllocated( ds );

    return;
    }

/************************************************************************/
/*									*/
/*  Add a new child to a parent.					*/
/*									*/
/************************************************************************/

DrawingShape * docInsertShapeInParent(	DrawingShape *		parent,
					int			n,
					int			kind )
    {
    int			i;

    int			newSize;

    DrawingShape **	freshChildren;
    DrawingShape *	ds;

    if  ( ! parent )
	{
	ds= (DrawingShape *)malloc( sizeof(DrawingShape) );
	if  ( ! ds )
	    { XDEB(ds); return ds;	}

	docInitDrawingShape( ds );
	ds->dsShapeType= kind;

	return ds;
	}

    if  ( n == -1 )
	{ n= parent->dsChildCount;	}

    newSize= parent->dsChildCount+ 1;
    newSize *= sizeof(DrawingShape *);

    freshChildren= (DrawingShape **)realloc( parent->dsChildren, newSize );
    if  ( ! freshChildren )
	{ LXDEB(newSize,freshChildren); return (DrawingShape *)0; }
    parent->dsChildren= freshChildren;

    ds= (DrawingShape *)malloc( sizeof(DrawingShape) );
    if  ( ! ds )
	{ XDEB(ds); return ds;	}

    if  ( n == -1 )
	{ n= parent->dsChildCount; }

    docInitDrawingShape( ds );
    ds->dsShapeType= kind;

    for ( i= parent->dsChildCount; i > n; i-- )
	{ freshChildren[i]= freshChildren[i-1];	}

    freshChildren[n]= ds;
    parent->dsChildCount++;

    ds->dsIsChildShape= 1;

    return ds;
    }

void docInitShapeProperties(	ShapeProperties *	sp )
    {
    sp->spRect.drX0= 0;
    sp->spRect.drY0= 0;
    sp->spRect.drY1= 0;
    sp->spRect.drX1= 0;

    sp->spHorizontalAttachment= SHPbxCOLUMN;
    sp->spVerticalAttachment= SHPbyPARA;
    sp->spWrapStyle= SHPswsTOPBOTTOM;
    sp->spWrapSide= SHPswsBOTH;
    sp->spLockAnchor= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Translate shape type to string.					*/
/*									*/
/************************************************************************/

const char * docShapeTypeString(	int	shapeType )
    {
    static char		scratch[12];

    switch( shapeType )
	{
	case SHPtyFREEFORM_OR_NON_AUTOSHAPE: return "FREEFORM_OR_NON_AUTOSHAPE";
	case SHPtyRECTANGLE: return "RECTANGLE";
	case SHPtyROUND_RECTANGLE: return "ROUND_RECTANGLE";
	case SHPtyELLIPSE: return "ELLIPSE";
	case SHPtyDIAMOND: return "DIAMOND";
	case SHPtyISOSCELES_TRIANGLE: return "ISOSCELES_TRIANGLE";
	case SHPtyRIGHT_TRIANGLE: return "RIGHT_TRIANGLE";
	case SHPtyPARALLELOGRAM: return "PARALLELOGRAM";
	case SHPtyTRAPEZOID: return "TRAPEZOID";
	case SHPtyHEXAGON: return "HEXAGON";
	case SHPtyOCTAGON: return "OCTAGON";
	case SHPtyPLUS_SIGN: return "PLUS_SIGN";
	case SHPtySTAR: return "STAR";
	case SHPtyARROW: return "ARROW";
	case SHPtyTHICK_ARROW: return "THICK_ARROW";
	case SHPtyHOME_PLATE: return "HOME_PLATE";
	case SHPtyCUBE: return "CUBE";
	case SHPtyBALLOON: return "BALLOON";
	case SHPtySEAL: return "SEAL";
	case SHPtyARC: return "ARC";
	case SHPtyLINE: return "LINE";
	case SHPtyPLAQUE: return "PLAQUE";
	case SHPtyCAN: return "CAN";
	case SHPtyDONUT: return "DONUT";
	case SHPtyTEXT_SIMPLE: return "TEXT_SIMPLE";
	case SHPtyTEXT_OCTAGON: return "TEXT_OCTAGON";
	case SHPtyTEXT_HEXAGON: return "TEXT_HEXAGON";
	case SHPtyTEXT_CURVE: return "TEXT_CURVE";
	case SHPtyTEXT_WAVE: return "TEXT_WAVE";
	case SHPtyTEXT_RING: return "TEXT_RING";
	case SHPtyTEXT_ON_CURVE: return "TEXT_ON_CURVE";
	case SHPtyTEXT_ON_RING: return "TEXT_ON_RING";
	case SHPtyCALLOUT_1: return "CALLOUT_1";
	case SHPtyCALLOUT_2: return "CALLOUT_2";
	case SHPtyCALLOUT_3: return "CALLOUT_3";
	case SHPtyACCENT_CALLOUT_1: return "ACCENT_CALLOUT_1";
	case SHPtyACCENT_CALLOUT_2: return "ACCENT_CALLOUT_2";
	case SHPtyACCENT_CALLOUT_3: return "ACCENT_CALLOUT_3";
	case SHPtyBORDER_CALLOUT_1: return "BORDER_CALLOUT_1";
	case SHPtyBORDER_CALLOUT_2: return "BORDER_CALLOUT_2";
	case SHPtyBORDER_CALLOUT_3: return "BORDER_CALLOUT_3";
	case SHPtyACCENT_BORDER_CALLOUT_1: return "ACCENT_BORDER_CALLOUT_1";
	case SHPtyACCENT_BORDER_CALLOUT_2: return "ACCENT_BORDER_CALLOUT_2";
	case SHPtyACCENT_BORDER_CALLOUT_3: return "ACCENT_BORDER_CALLOUT_3";
	case SHPtyRIBBON: return "RIBBON";
	case SHPtyRIBBON2: return "RIBBON2";
	case SHPtyCHEVRON: return "CHEVRON";
	case SHPtyPENTAGON: return "PENTAGON";
	case SHPtyNO_SMOKING: return "NO_SMOKING";
	case SHPtySEAL8: return "SEAL8";
	case SHPtySEAL16: return "SEAL16";
	case SHPtySEAL32: return "SEAL32";
	case SHPtyWEDGE_RECT_CALLOUT: return "WEDGE_RECT_CALLOUT";
	case SHPtyWEDGE_RRECT_CALLOUT: return "WEDGE_RRECT_CALLOUT";
	case SHPtyWEDGE_ELLIPSE_CALLOUT: return "WEDGE_ELLIPSE_CALLOUT";
	case SHPtyWAVE: return "WAVE";
	case SHPtyFOLDED_CORNER: return "FOLDED_CORNER";
	case SHPtyLEFT_ARROW: return "LEFT_ARROW";
	case SHPtyDOWN_ARROW: return "DOWN_ARROW";
	case SHPtyUP_ARROW: return "UP_ARROW";
	case SHPtyLEFT_RIGHT_ARROW: return "LEFT_RIGHT_ARROW";
	case SHPtyUP_DOWN_ARROW: return "UP_DOWN_ARROW";
	case SHPtyIRREGULARSEAL1: return "IRREGULARSEAL1";
	case SHPtyIRREGULARSEAL2: return "IRREGULARSEAL2";
	case SHPtyLIGHTNING_BOLT: return "LIGHTNING_BOLT";
	case SHPtyHEART: return "HEART";
	case SHPtyPICTURE_FRAME: return "PICTURE_FRAME";
	case SHPtyQUAD_ARROW: return "QUAD_ARROW";
	case SHPtyLEFT_ARROW_CALLOUT: return "LEFT_ARROW_CALLOUT";
	case SHPtyRIGHT_ARROW_CALLOUT: return "RIGHT_ARROW_CALLOUT";
	case SHPtyUP_ARROW_CALLOUT: return "UP_ARROW_CALLOUT";
	case SHPtyDOWN_ARROW_CALLOUT: return "DOWN_ARROW_CALLOUT";
	case SHPtyLEFT_RIGHT_ARROW_CALLOUT: return "LEFT_RIGHT_ARROW_CALLOUT";
	case SHPtyUP_DOWN_ARROW_CALLOUT: return "UP_DOWN_ARROW_CALLOUT";
	case SHPtyQUAD_ARROW_CALLOUT: return "QUAD_ARROW_CALLOUT";
	case SHPtyBEVEL: return "BEVEL";
	case SHPtyLEFT_BRACKET: return "LEFT_BRACKET";
	case SHPtyRIGHT_BRACKET: return "RIGHT_BRACKET";
	case SHPtyLEFT_BRACE: return "LEFT_BRACE";
	case SHPtyRIGHT_BRACE: return "RIGHT_BRACE";
	case SHPtyLEFT_UP_ARROW: return "LEFT_UP_ARROW";
	case SHPtyBENT_UP_ARROW: return "BENT_UP_ARROW";
	case SHPtyBENT_ARROW: return "BENT_ARROW";
	case SHPtySEAL24: return "SEAL24";
	case SHPtySTRIPED_RIGHT_ARROW: return "STRIPED_RIGHT_ARROW";
	case SHPtyNOTCHED_RIGHT_ARROW: return "NOTCHED_RIGHT_ARROW";
	case SHPtyBLOCK_ARC: return "BLOCK_ARC";
	case SHPtySMILEY_FACE: return "SMILEY_FACE";
	case SHPtyVERTICAL_SCROLL: return "VERTICAL_SCROLL";
	case SHPtyHORIZONTAL_SCROLL: return "HORIZONTAL_SCROLL";
	case SHPtyCIRCULAR_ARROW: return "CIRCULAR_ARROW";
	case SHPtyNOTCHED_CIRCULAR_ARROW: return "NOTCHED_CIRCULAR_ARROW";
	case SHPtyUTURN_ARROW: return "UTURN_ARROW";
	case SHPtyCURVED_RIGHT_ARROW: return "CURVED_RIGHT_ARROW";
	case SHPtyCURVED_LEFT_ARROW: return "CURVED_LEFT_ARROW";
	case SHPtyCURVED_UP_ARROW: return "CURVED_UP_ARROW";
	case SHPtyCURVED_DOWN_ARROW: return "CURVED_DOWN_ARROW";
	case SHPtyCLOUD_CALLOUT: return "CLOUD_CALLOUT";
	case SHPtyELLIPSE_RIBBON: return "ELLIPSE_RIBBON";
	case SHPtyELLIPSE_RIBBON_2: return "ELLIPSE_RIBBON_2";
	case SHPtyFLOW_CHART_PROCESS: return "FLOW_CHART_PROCESS";
	case SHPtyFLOW_CHART_DECISION: return "FLOW_CHART_DECISION";
	case SHPtyFLOW_CHART_INPUT_OUTPUT: return "FLOW_CHART_INPUT_OUTPUT";
	case SHPtyFLOW_CHART_PREDEFINED_PROCESS:
				    return "FLOW_CHART_PREDEFINED_PROCESS";
	case SHPtyFLOW_CHART_INTERNAL_STORAGE:
				    return "FLOW_CHART_INTERNAL_STORAGE";
	case SHPtyFLOW_CHART_DOCUMENT: return "FLOW_CHART_DOCUMENT";
	case SHPtyFLOW_CHART_MULTIDOCUMENT: return "FLOW_CHART_MULTIDOCUMENT";
	case SHPtyFLOW_CHART_TERMINATOR: return "FLOW_CHART_TERMINATOR";
	case SHPtyFLOW_CHART_PREPARATION: return "FLOW_CHART_PREPARATION";
	case SHPtyFLOW_CHART_MANUAL_INPUT: return "FLOW_CHART_MANUAL_INPUT";
	case SHPtyFLOW_CHART_MANUAL_OPERATION:
				    return "FLOW_CHART_MANUAL_OPERATION";
	case SHPtyFLOW_CHART_CONNECTOR: return "FLOW_CHART_CONNECTOR";
	case SHPtyFLOW_CHART_PUNCHED_CARD: return "FLOW_CHART_PUNCHED_CARD";
	case SHPtyFLOW_CHART_PUNCHED_TAPE: return "FLOW_CHART_PUNCHED_TAPE";
	case SHPtyFLOW_CHART_SUMMING_JUNCTION:
				    return "FLOW_CHART_SUMMING_JUNCTION";
	case SHPtyFLOW_CHART_OR: return "FLOW_CHART_OR";
	case SHPtyFLOW_CHART_COLLATE: return "FLOW_CHART_COLLATE";
	case SHPtyFLOW_CHART_SORT: return "FLOW_CHART_SORT";
	case SHPtyFLOW_CHART_EXTRACT: return "FLOW_CHART_EXTRACT";
	case SHPtyFLOW_CHART_MERGE: return "FLOW_CHART_MERGE";
	case SHPtyFLOW_CHART_OFFLINE_STORAGE:
					return "FLOW_CHART_OFFLINE_STORAGE";
	case SHPtyFLOW_CHART_ONLINE_STORAGE: return "FLOW_CHART_ONLINE_STORAGE";
	case SHPtyFLOW_CHART_MAGNETIC_TAPE: return "FLOW_CHART_MAGNETIC_TAPE";
	case SHPtyFLOW_CHART_MAGNETIC_DISK: return "FLOW_CHART_MAGNETIC_DISK";
	case SHPtyFLOW_CHART_MAGNETIC_DRUM: return "FLOW_CHART_MAGNETIC_DRUM";
	case SHPtyFLOW_CHART_DISPLAY: return "FLOW_CHART_DISPLAY";
	case SHPtyFLOW_CHART_DELAY: return "FLOW_CHART_DELAY";
	case SHPtyTEXT_PLAIN_TEXT: return "TEXT_PLAIN_TEXT";
	case SHPtyTEXT_STOP: return "TEXT_STOP";
	case SHPtyTEXT_TRIANGLE: return "TEXT_TRIANGLE";
	case SHPtyTEXT_TRIANGLE_INVERTED: return "TEXT_TRIANGLE_INVERTED";
	case SHPtyTEXT_CHEVRON: return "TEXT_CHEVRON";
	case SHPtyTEXT_CHEVRON_INVERTED: return "TEXT_CHEVRON_INVERTED";
	case SHPtyTEXT_RING_INSIDE: return "TEXT_RING_INSIDE";
	case SHPtyTEXT_RING_OUTSIDE: return "TEXT_RING_OUTSIDE";
	case SHPtyTEXT_ARCH_UP_CURVE: return "TEXT_ARCH_UP_CURVE";
	case SHPtyTEXT_ARCH_DOWN_CURVE: return "TEXT_ARCH_DOWN_CURVE";
	case SHPtyTEXT_CIRCLE_CURVE: return "TEXT_CIRCLE_CURVE";
	case SHPtyTEXT_BUTTON_CURVE: return "TEXT_BUTTON_CURVE";
	case SHPtyTEXT_ARCH_UP_POUR: return "TEXT_ARCH_UP_POUR";
	case SHPtyTEXT_ARCH_DOWN_POUR: return "TEXT_ARCH_DOWN_POUR";
	case SHPtyTEXT_CIRCLE_POUR: return "TEXT_CIRCLE_POUR";
	case SHPtyTEXT_BUTTON_POUR: return "TEXT_BUTTON_POUR";
	case SHPtyTEXT_CURVE_UP: return "TEXT_CURVE_UP";
	case SHPtyTEXT_CURVE_DOWN: return "TEXT_CURVE_DOWN";
	case SHPtyTEXT_CASCADE_UP: return "TEXT_CASCADE_UP";
	case SHPtyTEXT_CASCADE_DOWN: return "TEXT_CASCADE_DOWN";
	case SHPtyTEXT_WAVE1: return "TEXT_WAVE1";
	case SHPtyTEXT_WAVE2: return "TEXT_WAVE2";
	case SHPtyTEXT_WAVE3: return "TEXT_WAVE3";
	case SHPtyTEXT_WAVE4: return "TEXT_WAVE4";
	case SHPtyTEXT_INFLATE: return "TEXT_INFLATE";
	case SHPtyTEXT_DEFLATE: return "TEXT_DEFLATE";
	case SHPtyTEXT_INFLATE_BOTTOM: return "TEXT_INFLATE_BOTTOM";
	case SHPtyTEXT_DEFLATE_BOTTOM: return "TEXT_DEFLATE_BOTTOM";
	case SHPtyTEXT_INFLATE_TOP: return "TEXT_INFLATE_TOP";
	case SHPtyTEXT_DEFLATE_TOP: return "TEXT_DEFLATE_TOP";
	case SHPtyTEXT_DEFLATE_INFLATE: return "TEXT_DEFLATE_INFLATE";
	case SHPtyTEXT_DEFLATE_INFLATE_DEFLATE:
					return "TEXT_DEFLATE_INFLATE_DEFLATE";
	case SHPtyTEXT_FADE_RIGHT: return "TEXT_FADE_RIGHT";
	case SHPtyTEXT_FADE_LEFT: return "TEXT_FADE_LEFT";
	case SHPtyTEXT_FADE_UP: return "TEXT_FADE_UP";
	case SHPtyTEXT_FADE_DOWN: return "TEXT_FADE_DOWN";
	case SHPtyTEXT_SLANT_UP: return "TEXT_SLANT_UP";
	case SHPtyTEXT_SLANT_DOWN: return "TEXT_SLANT_DOWN";
	case SHPtyTEXT_CAN_UP: return "TEXT_CAN_UP";
	case SHPtyTEXT_CAN_DOWN: return "TEXT_CAN_DOWN";
	case SHPtyFLOW_CHART_ALTERNATE_PROCESS:
					return "FLOW_CHART_ALTERNATE_PROCESS";
	case SHPtyFLOW_CHART_OFF_PAGE_CONNECTOR:
					return "FLOW_CHART_OFF_PAGE_CONNECTOR";
	case SHPtyCALLOUT_90: return "CALLOUT_90";
	case SHPtyACCENT_CALLOUT_90: return "ACCENT_CALLOUT_90";
	case SHPtyBORDER_CALLOUT_90: return "BORDER_CALLOUT_90";
	case SHPtyACCENT_BORDER_CALLOUT_90: return "ACCENT_BORDER_CALLOUT_90";
	case SHPtyLEFT_RIGHT_UP_ARROW: return "LEFT_RIGHT_UP_ARROW";
	case SHPtySUN: return "SUN";
	case SHPtyMOON: return "MOON";
	case SHPtyBRACKET_PAIR: return "BRACKET_PAIR";
	case SHPtyBRACE_PAIR: return "BRACE_PAIR";
	case SHPtySEAL4: return "SEAL4";
	case SHPtyDOUBLE_WAVE: return "DOUBLE_WAVE";
	case SHPtyHOST_CONTROL: return "HOST_CONTROL";
	case SHPtyTEXT_BOX: return "TEXT_BOX";

	case SHPtyUNKNOWN: return "UNKNOWN";
	case SHPtyGROUP: return "GROUP";
	case SHPtyPICPROP: return "PICPROP";

	default:
	    sprintf( scratch, "SHPty%d", shapeType );
	    return scratch;
	}
    }
