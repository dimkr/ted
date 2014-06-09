/************************************************************************/
/*									*/
/*  Read/Write MS-Word 95 style drawing objects from/to rtf.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<bitmap.h>

#   include	<appDebugon.h>

#   include	"docRtf.h"

static int docRtfDrawingObjectAllocatePoints(	DrawingShape *	ds,
						int		n )
    {
    ShapeVertex *	fresh;

    fresh= (ShapeVertex *)realloc( ds->dsVertices, n* sizeof(ShapeVertex) );
    if  ( ! fresh )
	{ LXDEB(n,fresh); return -1;	}

    ds->dsVertices= fresh;
    fresh += ds->dsVertexCount;

    while( ds->dsVertexCount < n )
	{
	fresh->svX= fresh->svY= 0;
	fresh++; ds->dsVertexCount++;
	}

    return 0;
    }

int docRtfDrawingObjectProperty(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    DrawingShape *	ds= ds= rrc->rrcDrawingShape;
    ShapeProperties *	sp;

    if  ( rrc->rrcInIgnoredGroup > 0 )
	{ return 0;	}

    if  ( ! ds )
	{ SLLXDEB(rcw->rcwWord,arg,rrc->rrcCurrentLine,ds); return 0;	}

    sp= &(ds->dsShapeProperties);

    switch( rcw->rcwID )
	{
	case DOpropANCHOR_LOCKED:
	    sp->spLockAnchor= arg != 0;
	    break;

	case DOpropX_ATTACH:
	    ds->dsHorizontalRelativeTo= rcw->rcwEnumValue;
	    sp->spHorizontalAttachment= rcw->rcwEnumValue;
	    break;

	case DOpropY_ATTACH:
	    ds->dsVerticalRelativeTo= rcw->rcwEnumValue;
	    sp->spVerticalAttachment= rcw->rcwEnumValue;
	    break;

	case DOpropKIND:
	    ds->dsShapeType= rcw->rcwEnumValue;

	    if  ( ds->dsShapeType == SHPtyLINE )
		{
		rrc->rrcNextObjectVertex= ds->dsVertexCount;
		if  ( docRtfDrawingObjectAllocatePoints( ds, 2 ) )
		    { LDEB(arg); return -1;	}
		}
	    break;

	case DOpropLINE_STYLE:
	    if  ( rcw->rcwEnumValue == DSdashHOLLOW )
		{
		ds->dsLineDashing= DSdashSOLID;
		ds->ds_fLine= 0;
		}
	    else{
		ds->dsLineDashing= rcw->rcwEnumValue;
		ds->ds_fLine= 1;
		}
	    break;

	case DOpropFILL_PATTERN:
	    switch( arg )
		{
		static int op[]= { 100,5,10,20,30,40,50,60,70,75,80,90 };

		case 0:
		    ds->ds_fFilled= 0;
		    ds->dsFillType= DSfillSOLID;
		    break;
		case  1: case  2: case  3: case  4: case  5: case  6: case  7:
		case  8: case  9: case 10: case 11: case 12: case 13:
		    ds->ds_fFilled= 1;
		    ds->dsFillType= DSfillSOLID;
		    ds->dsFillOpacity= ( op[arg-1]* 65536 )/ 100;
		    break;

		default:
		    LDEB(arg); break;
		}
	    break;

	case DOpropARC_FLIP_X:
	    ds->ds_fFlipH= arg != 0;
	    ds->ds_fRelFlipH= arg != 0;
	    break;
	case DOpropARC_FLIP_Y:
	    ds->ds_fFlipV= arg != 0;
	    ds->ds_fRelFlipV= arg != 0;
	    break;

	case DOpropX:
	    sp->spRect.drX0= arg;
	    ds->dsGeoRect.drX0= arg;
	    break;
	case DOpropY:
	    sp->spRect.drY0= arg;
	    ds->dsGeoRect.drY0= arg;
	    break;
	case DOpropZ:
	    sp->spZ= arg;
	    break;
	case DOpropWIDE:
	    sp->spRect.drX1= sp->spRect.drX0+ arg;
	    ds->dsGeoRect.drX1= ds->dsGeoRect.drX0+ arg;
	    break;
	case DOpropHIGH:
	    sp->spRect.drY1= sp->spRect.drY0+ arg;
	    ds->dsGeoRect.drY1= ds->dsGeoRect.drY0+ arg;
	    break;

	case DOpropTEXT_BOX_MARGIN:
	    ds->ds_dxTextLeft= TWIPStoEMU( arg );
	    ds->ds_dyTextTop= TWIPStoEMU( arg );
	    ds->ds_dxTextRight= TWIPStoEMU( arg );
	    ds->ds_dyTextBottom= TWIPStoEMU( arg );
	    break;

	case DOpropLINE_WIDTH:
	    ds->dsLineWidthEmu= TWIPStoEMU( arg );
	    break;

	case DOpropPOINT_COUNT:
	    rrc->rrcNextObjectVertex= ds->dsVertexCount;
	    if  ( docRtfDrawingObjectAllocatePoints( ds, arg ) )
		{ LDEB(arg); return -1;	}
	    break;

	case DOpropSTART_ARROW_HEAD:
	    ds->dsLineStartArrow.saArrowHead= rcw->rcwEnumValue;
	    break;
	case DOpropEND_ARROW_HEAD:
	    ds->dsLineEndArrow.saArrowHead= rcw->rcwEnumValue;
	    break;
	case DOpropSTART_ARROW_WIDTH:
	    ds->dsLineStartArrow.saArrowWidth= rcw->rcwEnumValue;
	    break;
	case DOpropEND_ARROW_WIDTH:
	    ds->dsLineEndArrow.saArrowWidth= rcw->rcwEnumValue;
	    break;
	case DOpropSTART_ARROW_LENGTH:
	    ds->dsLineStartArrow.saArrowLength= rcw->rcwEnumValue;
	    break;
	case DOpropEND_ARROW_LENGTH:
	    ds->dsLineEndArrow.saArrowLength= rcw->rcwEnumValue;
	    break;

	case DOpropLINE_RED:
	    ds->dsLineColor.rgb8Red= arg;
	    break;
	case DOpropLINE_GREEN:
	    ds->dsLineColor.rgb8Green= arg;
	    break;
	case DOpropLINE_BLUE:
	    ds->dsLineColor.rgb8Blue= arg;
	    break;
	case DOpropLINE_GRAY:
	    ds->dsLineColor.rgb8Red= arg;
	    ds->dsLineColor.rgb8Green= arg;
	    ds->dsLineColor.rgb8Blue= arg;
	    break;

	case DOpropFILL_FORE_RED:
	    ds->dsFillColor.rgb8Red= arg;
	    break;
	case DOpropFILL_FORE_GREEN:
	    ds->dsFillColor.rgb8Green= arg;
	    break;
	case DOpropFILL_FORE_BLUE:
	    ds->dsFillColor.rgb8Blue= arg;
	    break;
	case DOpropFILL_FORE_GRAY:
	    ds->dsFillColor.rgb8Red= arg;
	    ds->dsFillColor.rgb8Green= arg;
	    ds->dsFillColor.rgb8Blue= arg;
	    break;

	case DOpropFILL_BACK_RED:
	    ds->dsFillBackColor.rgb8Red= arg;
	    break;
	case DOpropFILL_BACK_GREEN:
	    ds->dsFillBackColor.rgb8Green= arg;
	    break;
	case DOpropFILL_BACK_BLUE:
	    ds->dsFillBackColor.rgb8Blue= arg;
	    break;
	case DOpropFILL_BACK_GRAY:
	    ds->dsFillBackColor.rgb8Red= arg;
	    ds->dsFillBackColor.rgb8Green= arg;
	    ds->dsFillBackColor.rgb8Blue= arg;
	    break;

	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID);
	    /* SLDEB(rcw->rcwWord,rcw->rcwID); */
	    break;
	}

    return 0;
    }

int docRtfDrawingObjectCoordinate(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    DrawingShape *	ds= ds= rrc->rrcDrawingShape;
    ShapeProperties *	sp;

    if  ( rrc->rrcInIgnoredGroup > 0 )
	{ return 0;	}

    if  ( ! ds )
	{ SLLXDEB(rcw->rcwWord,arg,rrc->rrcCurrentLine,ds); return 0;	}

    sp= &(ds->dsShapeProperties);

    if  ( rrc->rrcNextObjectVertex >= ds->dsVertexCount	||
	  rrc->rrcNextObjectVertex < 0			)
	{
	LLLDEB(ds->dsShapeType,rrc->rrcNextObjectVertex,ds->dsVertexCount);
	return 0;
	}

    switch( rcw->rcwID )
	{
	case DOpropX:
	    ds->dsVertices[rrc->rrcNextObjectVertex  ].svX= arg;
	    break;
	case DOpropY:
	    ds->dsVertices[rrc->rrcNextObjectVertex++].svY= arg;
	    break;

	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID);
	    break;
	}

    return 0;
    }

