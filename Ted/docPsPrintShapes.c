/************************************************************************/
/*									*/
/*  Infrastructure for printing 'shapes'.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<math.h>

#   include	<sioGeneral.h>

#   include	<appMetafile.h>

#   include	"docDraw.h"
#   include	"docPsPrint.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Include procedures to draw the paths that make up the simple shapes	*/
/*  in the PostScript printout.						*/
/*									*/
/************************************************************************/

static void docPsPrintShapePathProc(	SimpleOutputStream *	sos,
					const ShapePath *	sp )
    {
    int		i;

    sioOutPrintf( sos, "\n" );
    sioOutPrintf( sos, "/%s\n", sp->spName );
    sioOutPrintf( sos, "  {\n" );
    sioOutPrintf( sos, "  %g %g bp ",
			    sp->spVertices[0].svX- 0.5* sp->spXSize,
			    sp->spVertices[0].svY- 0.5* sp->spYSize );

    for ( i= 1; i < sp->spVertexCount; i++ )
	{
	sioOutPrintf( sos, "%d %d rl",
			sp->spVertices[i].svX- sp->spVertices[i- 1].svX,
			sp->spVertices[i].svY- sp->spVertices[i- 1].svY );

	if  ( i % 8 == 7 || i == sp->spVertexCount- 1 )
	    { sioOutPrintf( sos, "\n  " );		}
	else{ sioOutPutCharacter( ' ', sos );		}
	}

    if  ( sp->spClosed )
	{ sioOutPrintf( sos, "  closepath\n" );	}

    sioOutPrintf( sos, "  } bind def\n" );

    return;
    }

void docPsPrintShapeDefineProcsetPs(	SimpleOutputStream *	sos )
    {
    sioOutPrintf( sos, "%%%%BeginProcSet TedShapeToPs 1.0 1\n" );

    docPsPrintShapePathProc( sos, &SP_33 );
    docPsPrintShapePathProc( sos, &SP_34 );
    docPsPrintShapePathProc( sos, &SP_LINE );
    docPsPrintShapePathProc( sos, &SP_RECTANGLE );
    docPsPrintShapePathProc( sos, &SP_DIAMOND );
    docPsPrintShapePathProc( sos, &SP_ISOSCELES_TRIANGLE );
    docPsPrintShapePathProc( sos, &SP_RIGHT_TRIANGLE );
    docPsPrintShapePathProc( sos, &SP_PARALLELOGRAM );
    docPsPrintShapePathProc( sos, &SP_TRAPEZOID );
    docPsPrintShapePathProc( sos, &SP_HEXAGON );
    docPsPrintShapePathProc( sos, &SP_OCTAGON );
    docPsPrintShapePathProc( sos, &SP_PLUS_SIGN );
    docPsPrintShapePathProc( sos, &SP_ARROW );
    docPsPrintShapePathProc( sos, &SP_NOTCHED_RIGHT_ARROW );
    docPsPrintShapePathProc( sos, &SP_HOME_PLATE );
    docPsPrintShapePathProc( sos, &SP_CHEVRON );
    docPsPrintShapePathProc( sos, &SP_LEFT_ARROW );
    docPsPrintShapePathProc( sos, &SP_RIGHT_ARROW_CALLOUT );
    docPsPrintShapePathProc( sos, &SP_LEFT_ARROW_CALLOUT );
    docPsPrintShapePathProc( sos, &SP_UP_ARROW_CALLOUT );
    docPsPrintShapePathProc( sos, &SP_LEFT_RIGHT_ARROW_CALLOUT );
    docPsPrintShapePathProc( sos, &SP_UP_DOWN_ARROW_CALLOUT );
    docPsPrintShapePathProc( sos, &SP_DOWN_ARROW_CALLOUT );
    docPsPrintShapePathProc( sos, &SP_QUAD_ARROW_CALLOUT );
    docPsPrintShapePathProc( sos, &SP_LEFT_RIGHT_ARROW );
    docPsPrintShapePathProc( sos, &SP_UP_ARROW );
    docPsPrintShapePathProc( sos, &SP_DOWN_ARROW );
    docPsPrintShapePathProc( sos, &SP_UP_DOWN_ARROW );
    docPsPrintShapePathProc( sos, &SP_QUAD_ARROW );
    docPsPrintShapePathProc( sos, &SP_LEFT_RIGHT_UP_ARROW );
    docPsPrintShapePathProc( sos, &SP_LEFT_UP_ARROW );
    docPsPrintShapePathProc( sos, &SP_BENT_UP_ARROW );
    docPsPrintShapePathProc( sos, &SP_PENTAGON );
    docPsPrintShapePathProc( sos, &SP_FLOW_CHART_PUNCHED_CARD );
    docPsPrintShapePathProc( sos, &SP_FLOW_CHART_MANUAL_INPUT );
    docPsPrintShapePathProc( sos, &SP_FLOW_CHART_OFF_PAGE_CONNECTOR );
    docPsPrintShapePathProc( sos, &SP_WEDGE_RECT_CALLOUT );
    docPsPrintShapePathProc( sos, &SP_STAR );
    docPsPrintShapePathProc( sos, &SP_SEAL4 );
    docPsPrintShapePathProc( sos, &SP_FLOW_CHART_MERGE );

    sioOutPrintf( sos, "\n%%%%EndProcSet\n\n" );

    return;
    }

static int docPsPrintArrowHead(	PrintingState *			ps,
				const AffineTransform2D *	atF,
				double				xTo,
				double				yTo,
				double				xFrom,
				double				yFrom,
				const ShapeArrow *		sa )
    {
    AffineTransform2D		at;
    AffineTransform2D		att;

    int				length;
    int				length2;
    int				width;

    SimpleOutputStream *	sos= ps->psSos;

    double			xToT;
    double			yToT;
    double			xFromT;
    double			yFromT;

    docShapeArrowSizesTwips( &length, &length2, &width, sa );

    xToT= AT2_X( xTo, yTo, atF );
    yToT= AT2_Y( xTo, yTo, atF );
    xFromT= AT2_X( xFrom, yFrom, atF );
    yFromT= AT2_Y( xFrom, yFrom, atF );

    utilTranslationAffineTransform2D( &at, xToT, yToT );

    utilRotationAffineTransform2DAtan( &att, yToT- yFromT, xToT- xFromT );
    utilAffineTransform2DProduct( &at, &at, &att );

    sioOutPrintf( ps->psSos, "gsave " );
    utilPsTransformMatrix( ps->psSos, &at );
    sioOutPrintf( ps->psSos, " concat %% arrow\n" );

    switch( sa->saArrowHead )
	{
	case DSarrowNONE:
	    sioOutPrintf( ps->psSos, "grestore\n" );
	    return 0;

	case DSarrowARROW:
	    sioOutPrintf( sos, "0 0 bp " );
	    sioOutPrintf( sos, "%d %d rl ", -length,  -width );
	    sioOutPrintf( sos, "%d %d rl ", 0,  2* width );
	    sioOutPrintf( sos, "closepath fill\n" );
	    break;

	case DSarrowSTEALTH_ARROW:
	    sioOutPrintf( sos, "0 0 bp " );
	    sioOutPrintf( sos, "%d %d rl ", -length2, width );
	    sioOutPrintf( sos, "%d %d rl ", length2- length, -width );
	    sioOutPrintf( sos, "%d %d rl ", length- length2, -width );
	    sioOutPrintf( sos, "closepath fill\n" );
	    break;

	case DSarrowDIAMOND:
	    sioOutPrintf( sos, "0 0 bp " );
	    sioOutPrintf( sos, "%d %d rl ", -length/ 2,   width );
	    sioOutPrintf( sos, "%d %d rl ", -length/ 2,  -width );
	    sioOutPrintf( sos, "%d %d rl ",  length/ 2,  -width );
	    sioOutPrintf( sos, "%d %d rl ",  length/ 2,   width );
	    sioOutPrintf( sos, "closepath fill\n" );
	    break;

	case DSarrowOPEN_ARROW:
	    sioOutPrintf( sos, "[] 0 setdash\n" );

	    sioOutPrintf( sos, "%d %d bp ", -length,   width );
	    sioOutPrintf( sos, "%d %d rl ", length,   -width );
	    sioOutPrintf( sos, "%d %d rl ", -length,  -width );
	    sioOutPrintf( sos, "stroke\n" );
	    break;

	case DSarrowOVAL:
	    sioOutPrintf( sos, "newpath %d %d %d 0 360 arc closepath fill\n",
				    -length/ 2, 0, -length/ 2 );
	    break;

	case DSarrowCHEVRON_ARROW:
	case DSarrow2CHEVRON_ARROW:
	default:
	    LDEB(sa->saArrowHead);
	}

    sioOutPrintf( ps->psSos, "grestore\n" );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Line needed?							*/
/*									*/
/************************************************************************/

static void docPsPrintGetLine(	int *				pLine,
				const DrawingShape *		ds,
				DrawingContext *		dc,
				PrintingState *			ps,
				const AffineTransform2D *	at )
    {
    docShapeGetLine( pLine, ds, dc, ps );

    if  ( *pLine )
	{
	SimpleOutputStream *	sos= ps->psSos;
	int			widthTwips= EMUtoTWIPS( ds->dsLineWidthEmu );
	double			widthScaled= widthTwips;

	if  ( at )
	    {
	    double	x;
	    double	y;
	    double	fac;

	    x= AT2_X( 1.0, 1.0, at )- at->at2Tx;
	    y= AT2_Y( 1.0, 1.0, at )- at->at2Ty;

	    fac= sqrt( x*x+ y*y );

	    if  ( fac == 0 )
		{ FDEB(fac); widthScaled= 0.001;	}
	    else{ widthScaled= widthTwips/ fac;		}
	    }

	sioOutPrintf( ps->psSos, "%g setlinewidth\n", widthScaled );

	switch( ds->dsLineDashing )
	    {
	    case DSdashSOLID:
		sioOutPrintf( sos, "[] 0 setdash\n" );
		break;

	    case DSdashDASHED:
	    case DSdashDASHED_X:
	    case DSdashDASHED_L:
		sioOutPrintf( sos, "[%g %g] 0 setdash\n",
						6* widthScaled,
						4* widthScaled );
		break;

	    case DSdashDOT:
	    case DSdashDOT_X:
		sioOutPrintf( sos, "[%g %g] 0 setdash\n",
						1* widthScaled,
						2* widthScaled );
		break;

	    case DSdashDASHDOT:
	    case DSdashDASHDOT_X:
	    case DSdashDASHDOT_L:
		sioOutPrintf( sos, "[%g %g %g %g] 0 setdash\n",
						6* widthScaled,
						2* widthScaled,
						1* widthScaled,
						2* widthScaled );
		break;

	    case DSdashDASHDOTDOT:
	    case DSdashDASHDOTDOT_L:
		sioOutPrintf( sos, "[%g %g %g %g %g %g] 0 setdash\n",
						6* widthScaled,
						2* widthScaled,
						1* widthScaled,
						2* widthScaled,
						1* widthScaled,
						2* widthScaled );
		break;

	    default:
		LDEB(ds->dsLineDashing);
		sioOutPrintf( sos, "[] 0 setdash\n" );
	    }
	}

    return;
    }

static void docShapeEndShapeTransform(	PrintingState *		ps,
					DrawingContext *	dc )
    {
    SimpleOutputStream *	sos= ps->psSos;

    sioOutPrintf( sos, "grestore %% <}\n" );

    dc->dcCurrentColorSet= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the form given by the path for an individual shape.		*/
/*									*/
/************************************************************************/

static int docPsPrintFinishPath( const DrawingShape *		ds,
				const AffineTransform2D *	at,
				double				xs,
				double				ys,
				const ShapeVertex *		sv,
				int				nv,
				int				closed,
				DrawingContext *		dc,
				PrintingState *			ps )
    {
    SimpleOutputStream *	sos= ps->psSos;

    int			line= 0;

    if  ( closed )
	{
	int		fill= 0;

	docShapeGetFill( &fill, ds, dc, (void *)ps );
	if  ( fill )
	    { sioOutPrintf( sos, "gsave fill grestore\n" ); }
	}

    docPsPrintGetLine( &line, ds, dc, ps, at );
    if  ( line )
	{ sioOutPrintf( sos, "stroke\n" );	}

    docShapeEndShapeTransform( ps, dc );

    if  ( ! closed )
	{
	if  ( ds->dsLineStartArrow.saArrowHead != DSarrowNONE )
	    {
	    docPsPrintGetLine( &line, ds, dc, ps, (AffineTransform2D *)0 );

	    docPsPrintArrowHead( ps, at, sv[0].svX- 0.5* xs,
					sv[0].svY- 0.5* ys,
					sv[1].svX- 0.5* xs,
					sv[1].svY- 0.5* ys,
					&(ds->dsLineStartArrow) );
	    }
	if  ( ds->dsLineEndArrow.saArrowHead != DSarrowNONE )
	    {
	    docPsPrintGetLine( &line, ds, dc, ps, (AffineTransform2D *)0 );

	    docPsPrintArrowHead( ps, at, sv[nv-1].svX- 0.5* xs,
					sv[nv-1].svY- 0.5* ys,
					sv[nv-2].svX- 0.5* xs,
					sv[nv-2].svY- 0.5* ys,
					&(ds->dsLineEndArrow) );
	    }
	}

    return 0;
    }

static void docShapeStartShapeTransform(
				AffineTransform2D *		at,
				const DrawingShape *		ds,
				const DocumentRectangle *	dr,
				PrintingState *			ps,
				int				xSize,
				int				ySize )
    {
    double			xs= 1.0;
    double			ys= 1.0;

    AffineTransform2D		att;

    if  ( dr->drX1 != dr->drX0 )
	{ xs= ( dr->drX1- dr->drX0 )/ ( 1.0* xSize );	}
    if  ( dr->drY1 != dr->drY0 )
	{ ys= ( dr->drY1- dr->drY0 )/ ( 1.0* ySize );	}

    utilScaleAffineTransform2D( at, xs, ys );

    utilTranslationAffineTransform2D( &att,
				( dr->drX1+ dr->drX0 )/ 2.0,
				( dr->drY1+ dr->drY0 )/ 2.0 );
    utilAffineTransform2DProduct( at, &att, at );

    docShapeInternalTransform( &att, ds );
    utilAffineTransform2DProduct( at, at, &att );

    sioOutPrintf( ps->psSos, "gsave " );
    utilPsTransformMatrix( ps->psSos, at );
    sioOutPrintf( ps->psSos, " concat %%{>\n" );

    return;
    }

static int docPsPrintShapePath(	const DrawingShape *		ds,
				const ShapePath *		sp,
				const DocumentRectangle *	dr,
				DrawingContext *		dc,
				PrintingState *			ps )
    {
    SimpleOutputStream *	sos= ps->psSos;

    int				rval= 0;
    AffineTransform2D		at;

    docShapeStartShapeTransform( &at, ds, dr, ps, sp->spXSize, sp->spYSize );

    sioOutPrintf( sos, "%s\n", sp->spName );

    docPsPrintFinishPath( ds, &at, sp->spXSize, sp->spYSize,
					    sp->spVertices, sp->spVertexCount,
					    sp->spClosed, dc, ps );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Draw an individual shape.						*/
/*									*/
/************************************************************************/

int docPsPrintDrawDrawingShape(	const DocumentRectangle *	twipsRect,
				int				page,
				DrawingShape *			ds,
				DrawingContext *		dc,
				void *				vps )
    {
    PrintingState *		ps= (PrintingState *)vps;
    SimpleOutputStream *	sos= ps->psSos;
    const PostScriptFontList *	psfl= dc->dcPostScriptFontList;

    int				rval= 0;

    int				fill= 0;
    int				line= 0;

    AffineTransform2D		at;

    switch( ds->dsShapeType )
	{
	case SHPtyGROUP:
	    /* Done by docDrawDrawingShape(). */
	    LSDEB(ds->dsShapeType,docShapeTypeString(ds->dsShapeType));
	    break;

	case 33:
	    if  ( docPsPrintShapePath( ds, &SP_33, twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case 34:
	    if  ( docPsPrintShapePath( ds, &SP_34, twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPICTURE_FRAME:
	    {
	    const ShapePath *	sp= &SP_RECTANGLE;

	    docShapeStartShapeTransform( &at, ds, twipsRect, ps,
						    sp->spXSize, sp->spYSize );

	    docShapeGetFill( &fill, ds, dc, ps );
	    if  ( fill )
		{ sioOutPrintf( sos, "%s fill\n", sp->spName ); }

	    if  ( ds->dsPictureProperties.pipType != DOCokUNKNOWN )
		{ docPsPrintShapeImage( ps, dc, ds, psfl );	}

	    docPsPrintGetLine( &line, ds, dc, ps, &at );
	    if  ( line )
		{ sioOutPrintf( sos, "%s stroke\n", sp->spName ); }

	    docShapeEndShapeTransform( ps, dc );
	    }
	    break;

	case SHPtyCALLOUT_1:
	case SHPtyCALLOUT_90:
	    {
	    const ShapePath *	sp= &SP_RECTANGLE;

	    docShapeStartShapeTransform( &at, ds, twipsRect, ps,
						    sp->spXSize, sp->spYSize );

	    docShapeGetFill( &fill, ds, dc, ps );
	    if  ( fill )
		{ sioOutPrintf( sos, "%s fill\n", sp->spName ); }

	    docPsPrintGetLine( &line, ds, dc, ps, &at );
	    if  ( line )
		{
		const int	SC= 21600; /* adjust to geoLeft etc? */

		sioOutPrintf( sos, "newpath %g %g moveto %g %g lineto stroke\n",
				ds->dsAdjustValue/  ( 1.0* SC )- 0.5,
				ds->dsAdjust2Value/ ( 1.0* SC )- 0.5,
				ds->dsAdjust3Value/ ( 1.0* SC )- 0.5,
				ds->dsAdjust4Value/ ( 1.0* SC )- 0.5 );
		}

	    docShapeEndShapeTransform( ps, dc );
	    }
	    break;

	case SHPtyRECTANGLE:
	case SHPtyFLOW_CHART_PROCESS:
	case SHPtyACCENT_BORDER_CALLOUT_90:
	case SHPtyTEXT_BOX:
	    if  ( docPsPrintShapePath( ds, &SP_RECTANGLE, twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyROUND_RECTANGLE:
	case SHPtyFLOW_CHART_ALTERNATE_PROCESS:
	    {
	    const int	x0= -40;
	    const int	y0= -40;
	    const int	x1= +40;
	    const int	y1= +40;
	    const int	w= 10;
	    const int	h= 10;

	    docShapeStartShapeTransform( &at,
				    ds, twipsRect, ps, x1- x0, y1- y0 );

	    appMetafileRoundRectPathPs( ps, x0, y0, x1, y1, w, h, (char *)0 );

	    docShapeGetFill( &fill, ds, dc, (void *)ps );
	    if  ( fill )
		{ sioOutPrintf( sos, "gsave fill grestore\n" ); }

	    docPsPrintGetLine( &line, ds, dc, ps, &at );
	    if  ( line )
		{ sioOutPrintf( sos, "stroke\n" );	}

	    docShapeEndShapeTransform( ps, dc );
	    }
	    break;

	case SHPtyELLIPSE:
	case SHPtyFLOW_CHART_CONNECTOR:
	    docShapeStartShapeTransform( &at, ds, twipsRect, ps, 2, 2 );

	    sioOutPrintf( sos, "newpath 0 0 1 0 360 arc\n" );

	    docShapeGetFill( &fill, ds, dc, ps );
	    if  ( fill )
		{ sioOutPrintf( sos, "gsave fill grestore\n" ); }
	    docPsPrintGetLine( &line, ds, dc, ps, &at );
	    if  ( line )
		{ sioOutPrintf( sos, "stroke\n" );	}

	    docShapeEndShapeTransform( ps, dc );
	    break;

	case SHPtyARC:
	    docShapeStartShapeTransform( &at, ds, twipsRect, ps, 2, 2 );

	    sioOutPrintf( sos, "newpath -1 0 1 -90 0 arc\n" );

	    docShapeGetFill( &fill, ds, dc, ps );
	    if  ( fill )
		{ sioOutPrintf( sos, "gsave fill grestore\n" ); }
	    docPsPrintGetLine( &line, ds, dc, ps, &at );
	    if  ( line )
		{ sioOutPrintf( sos, "stroke\n" );	}

	    docShapeEndShapeTransform( ps, dc );
	    break;

	case SHPtyDIAMOND:
	case SHPtyFLOW_CHART_DECISION:
	    if  ( docPsPrintShapePath( ds, &SP_DIAMOND, twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyISOSCELES_TRIANGLE:
	case SHPtyFLOW_CHART_EXTRACT:
	    if  ( docPsPrintShapePath( ds, &SP_ISOSCELES_TRIANGLE,
							twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyFLOW_CHART_MERGE:
	    if  ( docPsPrintShapePath( ds, &SP_FLOW_CHART_MERGE,
							twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyRIGHT_TRIANGLE:
	    if  ( docPsPrintShapePath( ds, &SP_RIGHT_TRIANGLE,
							twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPARALLELOGRAM:
	case SHPtyFLOW_CHART_INPUT_OUTPUT:
	    if  ( docPsPrintShapePath( ds, &SP_PARALLELOGRAM,
							twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyTRAPEZOID:
	case SHPtyFLOW_CHART_MANUAL_OPERATION:
	    if  ( docPsPrintShapePath( ds, &SP_TRAPEZOID, twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyHEXAGON:
	case SHPtyFLOW_CHART_PREPARATION:
	    if  ( docPsPrintShapePath( ds, &SP_HEXAGON, twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyOCTAGON:
	    if  ( docPsPrintShapePath( ds, &SP_OCTAGON, twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPLUS_SIGN:
	    if  ( docPsPrintShapePath( ds, &SP_PLUS_SIGN, twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyARROW:
	    if  ( docPsPrintShapePath( ds, &SP_ARROW, twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyNOTCHED_RIGHT_ARROW:
	    if  ( docPsPrintShapePath( ds, &SP_NOTCHED_RIGHT_ARROW,
							twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyHOME_PLATE:
	    if  ( docPsPrintShapePath( ds, &SP_HOME_PLATE, twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyCHEVRON:
	    if  ( docPsPrintShapePath( ds, &SP_CHEVRON, twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_ARROW:
	    if  ( docPsPrintShapePath( ds, &SP_LEFT_ARROW, twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyRIGHT_ARROW_CALLOUT:
	    if  ( docPsPrintShapePath( ds, &SP_RIGHT_ARROW_CALLOUT,
							twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyLEFT_ARROW_CALLOUT:
	    if  ( docPsPrintShapePath( ds, &SP_LEFT_ARROW_CALLOUT,
							twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyUP_ARROW_CALLOUT:
	    if  ( docPsPrintShapePath( ds, &SP_UP_ARROW_CALLOUT,
							twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyDOWN_ARROW_CALLOUT:
	    if  ( docPsPrintShapePath( ds, &SP_DOWN_ARROW_CALLOUT,
							twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_RIGHT_ARROW_CALLOUT:
	    if  ( docPsPrintShapePath( ds, &SP_LEFT_RIGHT_ARROW_CALLOUT,
							twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyUP_DOWN_ARROW_CALLOUT:
	    if  ( docPsPrintShapePath( ds, &SP_UP_DOWN_ARROW_CALLOUT,
							twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyQUAD_ARROW_CALLOUT:
	    if  ( docPsPrintShapePath( ds, &SP_QUAD_ARROW_CALLOUT,
							twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_RIGHT_ARROW:
	    if  ( docPsPrintShapePath( ds, &SP_LEFT_RIGHT_ARROW,
							twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyUP_ARROW:
	    if  ( docPsPrintShapePath( ds, &SP_UP_ARROW, twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyDOWN_ARROW:
	    if  ( docPsPrintShapePath( ds, &SP_DOWN_ARROW, twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyUP_DOWN_ARROW:
	    if  ( docPsPrintShapePath( ds, &SP_UP_DOWN_ARROW,
							twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyQUAD_ARROW:
	    if  ( docPsPrintShapePath( ds, &SP_QUAD_ARROW, twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_RIGHT_UP_ARROW:
	    if  ( docPsPrintShapePath( ds, &SP_LEFT_RIGHT_UP_ARROW,
							twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_UP_ARROW:
	    if  ( docPsPrintShapePath( ds, &SP_LEFT_UP_ARROW,
							twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyBENT_UP_ARROW:
	    if  ( docPsPrintShapePath( ds, &SP_BENT_UP_ARROW,
							twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPENTAGON:
	    if  ( docPsPrintShapePath( ds, &SP_PENTAGON, twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtySTAR:
	    if  ( docPsPrintShapePath( ds, &SP_STAR, twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtySEAL4:
	    if  ( docPsPrintShapePath( ds, &SP_SEAL4, twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case 32:
	case SHPtyLINE:
	    if  ( docPsPrintShapePath( ds, &SP_LINE, twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyFLOW_CHART_MANUAL_INPUT:
	    if  ( docPsPrintShapePath( ds, &SP_FLOW_CHART_MANUAL_INPUT,
							twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyFLOW_CHART_OFF_PAGE_CONNECTOR:
	    if  ( docPsPrintShapePath( ds, &SP_FLOW_CHART_OFF_PAGE_CONNECTOR,
							twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyFLOW_CHART_PUNCHED_CARD:
	    if  ( docPsPrintShapePath( ds, &SP_FLOW_CHART_PUNCHED_CARD,
							twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyWEDGE_RECT_CALLOUT:
	    if  ( docPsPrintShapePath( ds, &SP_WEDGE_RECT_CALLOUT,
							twipsRect, dc, ps ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyCAN:
	case SHPtyFLOW_CHART_MAGNETIC_DISK:
	    {
	    docShapeStartShapeTransform( &at, ds, twipsRect, ps, 2, 2 );

	    docShapeGetFill( &fill, ds, dc, (void *)ps );
	    if  ( fill )
		{
		sioOutPrintf( sos, "gsave 0 -0.6667 translate 1 0.3333 scale" );
		sioOutPrintf( sos, " newpath 0 0 1 0 360 arc" );
		sioOutPrintf( sos, " closepath fill grestore\n" );

		sioOutPrintf( sos, "-1 -0.6667 2 1.3334 rectfill\n" );

		sioOutPrintf( sos, "gsave 0  0.6667 translate 1 0.3333 scale" );
		sioOutPrintf( sos, " newpath 0 0 1 0 180 arc" );
		sioOutPrintf( sos, " closepath fill grestore\n" );
		}
	    docPsPrintGetLine( &line, ds, dc, ps, &at );
	    if  ( line )
		{
		sioOutPrintf( sos, "gsave 0 -0.6667 translate 1 0.3333 scale" );
		sioOutPrintf( sos, " newpath 0 0 1 0 360 arc" );
		sioOutPrintf( sos, " stroke grestore\n" );

		sioOutPrintf( sos, "newpath -1 -0.6667 moveto" );
		sioOutPrintf( sos, " 0 1.3334 rlineto stroke\n" );
		sioOutPrintf( sos, "newpath  1 -0.6667 moveto" );
		sioOutPrintf( sos, " 0 1.3334 rlineto stroke\n" );

		sioOutPrintf( sos, "gsave 0  0.6667 translate 1 0.3333 scale" );
		sioOutPrintf( sos, " newpath 0 0 1 0 180 arc" );
		sioOutPrintf( sos, " stroke grestore\n" );
		}

	    docShapeEndShapeTransform( ps, dc );
	    }
	    break;

	case SHPtyRIGHT_BRACE:
	    {
	    docShapeStartShapeTransform( &at, ds, twipsRect, ps, 2, 2 );

	    docPsPrintGetLine( &line, ds, dc, ps, &at );

	    sioOutPrintf( sos, "gsave 0 -0.75 translate 1 0.25 scale" );
	    sioOutPrintf( sos, " newpath -1 0 1 270 360 arc" );
	    sioOutPrintf( sos, " stroke grestore\n" );

	    sioOutPrintf( sos, "newpath 0 -0.75 moveto 0 0.5 rlineto stroke\n" );

	    sioOutPrintf( sos, "gsave 0 -0.25 translate 1 0.25 scale" );
	    sioOutPrintf( sos, " newpath  1 0 1  90 180 arc" );
	    sioOutPrintf( sos, " stroke grestore\n" );

	    sioOutPrintf( sos, "gsave 0  0.25 translate 1 0.25 scale" );
	    sioOutPrintf( sos, " newpath  1 0 1 180 270 arc" );
	    sioOutPrintf( sos, " stroke grestore\n" );

	    sioOutPrintf( sos, "newpath 0  0.25 moveto 0 0.5 rlineto stroke\n" );
	    sioOutPrintf( sos, "gsave 0  0.75 translate 1 0.25 scale" );
	    sioOutPrintf( sos, " newpath -1 0 1   0  90 arc" );
	    sioOutPrintf( sos, " stroke grestore\n" );

	    docShapeEndShapeTransform( ps, dc );
	    }
	    break;


	case SHPtyFREEFORM_OR_NON_AUTOSHAPE:
	    if  ( ds->dsVertexCount > 1 )
		{
		int		i;

		docShapeStartShapeTransform( &at, ds, twipsRect, ps,
					twipsRect->drX1- twipsRect->drX0,
					twipsRect->drY1- twipsRect->drY0 );

		sioOutPrintf( sos, "%d %d bp ",
			ds->dsVertices[0].svX-
				( twipsRect->drX1- twipsRect->drX0 )/ 2,
			ds->dsVertices[0].svY-
				( twipsRect->drY1- twipsRect->drY0 )/ 2 );

		for ( i= 1; i < ds->dsVertexCount; i++ )
		    {
		    sioOutPrintf( sos, "%d %d rl",
			    ds->dsVertices[i].svX- ds->dsVertices[i- 1].svX,
			    ds->dsVertices[i].svY- ds->dsVertices[i- 1].svY );

		    if  ( i % 8 == 7 || i == ds->dsVertexCount- 1 )
			{ sioOutPutCharacter( '\n', sos );	}
		    else{ sioOutPutCharacter( ' ', sos );	}
		    }

		docShapeGetFill( &fill, ds, dc, (void *)ps );
		if  ( fill )
		    { sioOutPrintf( sos, "gsave closepath fill grestore\n" ); }

		docPsPrintGetLine( &line, ds, dc, ps, &at );
		if  ( line )
		    { sioOutPrintf( sos, "stroke\n" );	}

		docShapeEndShapeTransform( ps, dc );
		}
	    break;

	default:
	    LSDEB(ds->dsShapeType,docShapeTypeString(ds->dsShapeType));
	    break;
	}

    /* Done by docDrawDrawingShape()....
    if  ( ds->dsExternalItem.eiItem )
	{}
    */

    ps->psLastPageMarked= ps->psPagesPrinted;
    return rval;
    }

