/************************************************************************/
/*									*/
/*  Ted: screen specific shape drawing.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<math.h>

#   include	"tedApp.h"
#   include	"tedLayout.h"
#   include	"tedDraw.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Draw a Word 97+ drawing shape.					*/
/*									*/
/************************************************************************/

static int tedShapeDrawArrowHead(	AppDrawingData *	add,
					int			xTo,
					int			yTo,
					int			xFrom,
					int			yFrom,
					int			widthPixels,
					const ShapeArrow *	sa )
    {
    AffineTransform2D	at;
    AffineTransform2D	att;
    APP_POINT		points[5];

    int			length;
    int			length2;
    int			width;

    docShapeArrowSizesTwips( &length, &length2, &width, sa );

    length= X_PIXELS( add, length );
    length2= X_PIXELS( add, length2 );
    width= Y_PIXELS( add, width );

    utilRotationAffineTransform2DAtan( &at, yTo- yFrom, xTo- xFrom );
    utilTranslationAffineTransform2D( &att, xTo, yTo );
    utilAffineTransform2DProduct( &at, &att, &at );

    points[0].x= xTo;
    points[0].y= yTo;

    switch( sa->saArrowHead )
	{
	case DSarrowNONE:
	    return 0;
	case DSarrowARROW:
	    points[1].x= AT2_X( -length,  width, &at );
	    points[1].y= AT2_Y( -length,  width, &at );
	    points[2].x= AT2_X( -length, -width, &at );
	    points[2].y= AT2_Y( -length, -width, &at );
	    points[3]= points[0];
	    appDrawFillPolygon( add, points, 3 );
	    break;

	case DSarrowSTEALTH_ARROW:
	    points[1].x= AT2_X( -length2,  width, &at );
	    points[1].y= AT2_Y( -length2,  width, &at );
	    points[2].x= AT2_X( -length,  0, &at );
	    points[2].y= AT2_Y( -length,  0, &at );
	    points[3].x= AT2_X( -length2, -width, &at );
	    points[3].y= AT2_Y( -length2, -width, &at );
	    points[4]= points[0];
	    appDrawFillPolygon( add, points, 4 );
	    break;

	case DSarrowDIAMOND:
	    points[1].x= AT2_X( -length/ 2,  width, &at );
	    points[1].y= AT2_Y( -length/ 2,  width, &at );
	    points[2].x= AT2_X( -length,  0, &at );
	    points[2].y= AT2_Y( -length,  0, &at );
	    points[3].x= AT2_X( -length/ 2, -width, &at );
	    points[3].y= AT2_Y( -length/ 2, -width, &at );
	    points[4]= points[0];
	    appDrawFillPolygon( add, points, 4 );
	    break;

	case DSarrowOPEN_ARROW:
	    points[0].x= AT2_X( -length,  width, &at );
	    points[0].y= AT2_Y( -length,  width, &at );
	    points[1].x= AT2_X( 0, 0, &at );
	    points[1].y= AT2_Y( 0, 0, &at );
	    points[2].x= AT2_X( -length, -width, &at );
	    points[2].y= AT2_Y( -length, -width, &at );

	    appDrawSetLineAttributes( add,
		    widthPixels, LINEstyleSOLID, LINEcapBUTT, LINEjoinMITER,
		    (unsigned char *)0, 0 );

	    appDrawDrawLines( add, points, 3 );
	    break;

	case DSarrowOVAL:
	    points[0].x= AT2_X( -length/ 2,  width, &at );
	    points[0].y= AT2_Y( -length/ 2,  width, &at );
	    points[1].x= AT2_X(  length/ 2, -width, &at );
	    points[1].y= AT2_Y(  length/ 2, -width, &at );
	    if  ( points[0].x > points[1].x )
		{
		int x= points[0].x;
		       points[0].x= points[1].x;
		                    points[1].x= x;
		}
	    if  ( points[0].y > points[1].y )
		{
		int y= points[0].y;
		       points[0].y= points[1].y;
		                    points[1].y= y;
		}
	    appDrawFillArc( add, points[0].x, points[0].y,
					    points[1].x- points[0].x+ 1,
					    points[1].y- points[0].y+ 1,
					    64* 0, 64* 360 );
	    break;

	case DSarrowCHEVRON_ARROW:
	case DSarrow2CHEVRON_ARROW:
	default:
	    LDEB(sa->saArrowHead); return 0;
	}

    return 0;
    }

static APP_POINT * tedDrawGetVertices(
				const DocumentRectangle *	twipsRect,
				const DrawingShape *		ds,
				AppDrawingData *		add,
				ScreenDrawingData *		sdd )
    {
    int			x0Ref= twipsRect->drX0;
    int			y0Ref= twipsRect->drY0;

    int			i;
    const ShapeVertex *	sv;

    APP_POINT *		xp;
    APP_POINT *		xpret;

    xp= xpret= malloc( ( ds->dsVertexCount+ 1)* sizeof(APP_POINT) );
    if  ( ! xp )
	{ LXDEB(ds->dsVertexCount,xp); return (APP_POINT *)0;	}

    sv= ds->dsVertices;
    for ( i= 0; i < ds->dsVertexCount; xp++, sv++, i++ )
	{
	xp->x= X_PIXELS( add, x0Ref+ sv->svX )- sdd->sddOx;
	xp->y= Y_PIXELS( add, y0Ref+ sv->svY )- sdd->sddOy;
	}

    if  ( ds->dsVertexCount > 0 )
	{ xpret[ds->dsVertexCount]= xpret[0]; }

    return xpret;
    }

static void tedShapeGetLine(	int *				pLine,
				int *				pWidthPixels,
				const DrawingShape *		ds,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd )
    {
    docShapeGetLine( pLine, ds, dc, sdd );

    if  ( *pLine )
	{
	AppDrawingData *	add= dc->dcDrawingData;
	int			widthTwips= EMUtoTWIPS( ds->dsLineWidthEmu );
	int			widthPixels= X_PIXELS( add, widthTwips );

	static unsigned char	dash[]= { 3, 2 };
	static unsigned char	dot[]= { 1, 2 };
	static unsigned char	dashdot[]= { 2, 2, 1, 2 };
	static unsigned char	dashdotdot[]= { 2, 2, 1, 2, 1, 2 };

	const unsigned char *	dashList= (const unsigned char *)0;
	int			dashCount= 0;
	int			lineStyle= LINEstyleSOLID;
	int			capStyle= LINEcapBUTT;
	int			joinStyle= LINEjoinMITER;

	if  ( widthPixels < 1 )
	    { widthPixels=  1;	}

	switch( ds->dsLineDashing )
	    {
	    case DSdashSOLID:
		dashList= (const unsigned char *)0;
		dashCount= 0;
		lineStyle= LINEstyleSOLID;
		capStyle= LINEcapBUTT;
		joinStyle= LINEjoinMITER;
		break;

	    case DSdashDASHED:
	    case DSdashDASHED_X:
	    case DSdashDASHED_L:
		dashList= dash;
		dashCount= sizeof( dash );
		lineStyle= LINEstyleON_OFF_DASH;
		capStyle= LINEcapBUTT;
		joinStyle= LINEjoinMITER;
		break;

	    case DSdashDOT:
	    case DSdashDOT_X:
		dashList= dot;
		dashCount= sizeof( dot );
		lineStyle= LINEstyleON_OFF_DASH;
		capStyle= LINEcapBUTT;
		joinStyle= LINEjoinMITER;
		break;

	    case DSdashDASHDOT:
	    case DSdashDASHDOT_X:
	    case DSdashDASHDOT_L:
		dashList= dashdot;
		dashCount= sizeof( dashdot );
		lineStyle= LINEstyleON_OFF_DASH;
		capStyle= LINEcapBUTT;
		joinStyle= LINEjoinMITER;
		break;

	    case DSdashDASHDOTDOT:
	    case DSdashDASHDOTDOT_L:
		dashList= dashdotdot;
		dashCount= sizeof( dashdotdot );
		lineStyle= LINEstyleON_OFF_DASH;
		capStyle= LINEcapBUTT;
		joinStyle= LINEjoinMITER;
		break;

	    default:
		LDEB(ds->dsLineDashing);
		dashList= (const unsigned char *)0;
		dashCount= 0;
	    }

	appDrawSetLineAttributes( add,
		    widthPixels, lineStyle, capStyle, joinStyle,
		    dashList, dashCount );

	*pWidthPixels= widthPixels;
	}
    }

static int tedShapeDrawPoints(	const DrawingShape *		ds,
				const APP_POINT *		xp,
				int				np,
				int				closed,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd )
    {
    AppDrawingData *	add= dc->dcDrawingData;

    int			line= 0;
    int			widthPixels;

    if  ( closed )
	{
	int		fill= 0;

	docShapeGetFill( &fill, ds, dc, sdd );
	if  ( fill )
	    { appDrawFillPolygon( add, xp, np ); }
	}

    tedShapeGetLine( &line, &widthPixels, ds, dc, sdd );
    if  ( line )
	{ appDrawDrawLines( add, xp, np+ ( closed != 0 ) ); }

    if  ( line && ! closed && np >= 2 )
	{
	if  ( ds->dsLineStartArrow.saArrowHead != DSarrowNONE )
	    {
	    tedShapeDrawArrowHead( add, xp[0].x, xp[0].y,
					xp[1].x, xp[1].y,
					widthPixels,
					&(ds->dsLineStartArrow) );
	    }
	if  ( ds->dsLineEndArrow.saArrowHead != DSarrowNONE )
	    {
	    tedShapeDrawArrowHead( add, xp[np-1].x, xp[np-1].y,
					xp[np-2].x, xp[np-2].y,
					widthPixels,
					&(ds->dsLineEndArrow) );
	    }
	}

    return 0;
    }

static void tedDrawSetShapePath(APP_POINT *			xp,
				const DrawingShape *		ds,
				const ShapePath *		sp,
				const DocumentRectangle *	dr )
    {
    int			np= sp->spVertexCount;
    const ShapeVertex *	sv= sp->spVertices;
    int			i;

    if  ( ds->dsRotation == 0 )
	{
	int			xs= sp->spXSize;
	int			ys= sp->spYSize;

	int			x0= dr->drX0;
	int			x1= dr->drX1;
	int			y0= dr->drY0;
	int			y1= dr->drY1;

	if  ( DSflipHORIZONTAL( ds ) )
	    { int swap= x0; x0= x1; x1= swap;	}
	if  ( DSflipVERTICAL( ds ) )
	    { int swap= y0; y0= y1; y1= swap;	}

	for ( i= 0; i < np; sv++, i++ )
	    {
	    xp[i].x= ( ( xs- sv->svX )* x0+ sv->svX* x1 )/ xs;
	    xp[i].y= ( ( ys- sv->svY )* y0+ sv->svY* y1 )/ ys;
	    }
	}
    else{
	AffineTransform2D	at;
	double			xs= sp->spXSize;
	double			ys= sp->spYSize;
	double			xm= ( dr->drX1+ dr->drX0 )/ 2.0;
	double			ym= ( dr->drY1+ dr->drY0 )/ 2.0;

	docShapeInternalTransform( &at, ds );

	for ( i= 0; i < np; sv++, i++ )
	    {
	    double	x;
	    double	y;

	    x= ( ( xs- sv->svX )* dr->drX0+ sv->svX* dr->drX1 )/ xs -xm;
	    y= ( ( ys- sv->svY )* dr->drY0+ sv->svY* dr->drY1 )/ ys- ym;

	    xp[i].x= AT2_X( x, y, &at )+ xm;
	    xp[i].y= AT2_Y( x, y, &at )+ ym;
	    }
	}

    xp[np]= xp[0];

    return;
    }

static int tedDrawShapePath(	const DrawingShape *		ds,
				const ShapePath *		sp,
				const DocumentRectangle *	dr,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd )
    {
    int			rval= 0;
    int			np= sp->spVertexCount;
    APP_POINT *		xp;

    xp= malloc( ( np+ 1 )* sizeof(APP_POINT) );
    if  ( ! xp )
	{ LXDEB(np,xp); return -1;	}

    tedDrawSetShapePath( xp, ds, sp, dr );

    if  ( tedShapeDrawPoints( ds, xp, np, sp->spClosed, dc, sdd ) )
	{ LDEB(np); rval= -1;	}

    free( xp );

    return rval;
    }

int tedDrawDrawingShape(	const DocumentRectangle *	twipsRect,
				int				page,
				DrawingShape *			ds,
				DrawingContext *		dc,
				void *				vsdd )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;

    int				rval= 0;

    DocumentRectangle		pixRect;
    int				fill= 0;
    int				line= 0;
    int				widthPixels= 0;

    int				wide;
    int				high;

    AppDrawingData *		add= dc->dcDrawingData;

    APP_POINT *			xp= (APP_POINT *)0;

    docShapeGetPixelRect( &pixRect, dc, twipsRect, page );

    if  ( dc->dcClipRect						&&
	  ! geoIntersectRectangle( (DocumentRectangle *)0,
					    &pixRect, dc->dcClipRect ) )
	{ return 0;	}

    pixRect.drX0 -= sdd->sddOx;
    pixRect.drX1 -= sdd->sddOx;
    pixRect.drY0 -= sdd->sddOy;
    pixRect.drY1 -= sdd->sddOy;

    switch( ds->dsShapeType )
	{
	case SHPtyGROUP:
	    /* Done by docDrawDrawingShape(). */
	    LSDEB(ds->dsShapeType,docShapeTypeString(ds->dsShapeType));
	    break;

	case 33:
	    if  ( tedDrawShapePath( ds, &SP_33, &pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case 34:
	    if  ( tedDrawShapePath( ds, &SP_34, &pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPICTURE_FRAME:
	    {
	    const ShapePath *	sp= &SP_RECTANGLE;
	    int			np= sp->spVertexCount;
	    APP_POINT *		xp;

	    xp= malloc( ( np+ 1 )* sizeof(APP_POINT) );
	    if  ( ! xp )
		{ LXDEB(np,xp); return -1;	}

	    tedDrawSetShapePath( xp, ds, sp, &pixRect );

	    docShapeGetFill( &fill, ds, dc, sdd );
	    if  ( fill )
		{ appDrawFillPolygon( add, xp, np ); }

	    if  ( ds->dsPixmap )
		{
		appDrawDrawPixmap( add, ds->dsPixmap,
			0, 0, /* src: x,y */
			pixRect.drX1- pixRect.drX0, /* wide */
			pixRect.drY1- pixRect.drY0, /* high */
			pixRect.drX0, pixRect.drY0 ); /* dest: x,y */
		}

	    tedShapeGetLine( &line, &widthPixels, ds, dc, sdd );
	    if  ( line )
		{ appDrawDrawLines( add, xp, np+ 1 ); }

	    free( xp );
	    }
	    break;

	case SHPtyCALLOUT_1:
	case SHPtyCALLOUT_90:
	    {
	    const ShapePath *	sp= &SP_RECTANGLE;
	    int			np= sp->spVertexCount;
	    APP_POINT *		xp;

	    if  ( np != 4 )
		{ LDEB(np); return -1;	}

	    xp= malloc( ( np+ 1+ 2+ 1 )* sizeof(APP_POINT) );
	    if  ( ! xp )
		{ LXDEB(np,xp); return -1;	}

	    tedDrawSetShapePath( xp, ds, sp, &pixRect );

	    docShapeGetFill( &fill, ds, dc, sdd );
	    if  ( fill )
		{ appDrawFillPolygon( add, xp, np ); }

	    tedShapeGetLine( &line, &widthPixels, ds, dc, sdd );
	    if  ( line )
		{
		int		pp0= 5;
		int		pp1= pp0;
		long		a0, a2;

		const int	SC= 21600; /* adjust to geoLeft etc? */

		a2= ds->dsAdjustValue; a0= SC- a2;
		xp[pp1].x= ( a0* xp[0].x+ a2* xp[2].x )/ SC;
		a2= ds->dsAdjust2Value; a0= SC- a2;
		xp[pp1].y= ( a0* xp[0].y+ a2* xp[2].y )/ SC;
		pp1++;

		a2= ds->dsAdjust3Value; a0= SC- a2;
		xp[pp1].x= ( a0* xp[0].x+ a2* xp[2].x )/ SC;
		a2= ds->dsAdjust4Value; a0= SC- a2;
		xp[pp1].y= ( a0* xp[0].y+ a2* xp[2].y )/ SC;
		pp1++;

		appDrawDrawLines( add, xp+ pp0, pp1- pp0 );
		}

	    free( xp );
	    }
	    break;

	case SHPtyRECTANGLE:
	case SHPtyFLOW_CHART_PROCESS:
	case SHPtyACCENT_BORDER_CALLOUT_90:
	case SHPtyTEXT_BOX:
	    if  ( tedDrawShapePath( ds, &SP_RECTANGLE, &pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyROUND_RECTANGLE:
	case SHPtyFLOW_CHART_ALTERNATE_PROCESS:
	    {
	    const int	w= ( pixRect.drX1- pixRect.drX0 )/ 4;
	    const int	h= ( pixRect.drY1- pixRect.drY0 )/ 4;

	    docShapeGetFill( &fill, ds, dc, sdd );
	    if  ( fill )
		{
		appDrawFillRoundRectX11( add, pixRect.drX0, pixRect.drY0,
					    pixRect.drX1, pixRect.drY1, w, h );
		}
	    tedShapeGetLine( &line, &widthPixels, ds, dc, sdd );
	    if  ( line )
		{
		appDrawDrawRoundRectX11( add, pixRect.drX0, pixRect.drY0,
					    pixRect.drX1, pixRect.drY1, w, h );
		}
	    }
	    break;

	case SHPtyELLIPSE:
	case SHPtyFLOW_CHART_CONNECTOR:
	    wide= pixRect.drX1- pixRect.drX0;
	    high= pixRect.drY1- pixRect.drY0;
	    docShapeGetFill( &fill, ds, dc, sdd );
	    if  ( fill )
		{
		appDrawFillArc( add, pixRect.drX0, pixRect.drY0,
					    wide+ 1, high+ 1,
					    64* 0, 64* 360 );
		}
	    tedShapeGetLine( &line, &widthPixels, ds, dc, sdd );
	    if  ( line )
		{
		appDrawDrawArc( add, pixRect.drX0, pixRect.drY0,
					    wide+ 1, high+ 1,
					    64* 0, 64* 360 );
		}
	    break;

	case SHPtyARC:
	    wide= pixRect.drX1- pixRect.drX0;
	    high= pixRect.drY1- pixRect.drY0;
	    docShapeGetFill( &fill, ds, dc, sdd );
	    if  ( fill )
		{
		appDrawFillArc( add, pixRect.drX0- wide, pixRect.drY0,
					    2* wide+ 1, 2* high+ 1,
					    64* 0, 64* 90 );
		}
	    tedShapeGetLine( &line, &widthPixels, ds, dc, sdd );
	    if  ( line )
		{
		appDrawDrawArc( add, pixRect.drX0- wide, pixRect.drY0,
					    2* wide+ 1, 2* high+ 1,
					    64* 0, 64* 90 );
		}
	    break;

	case SHPtyDIAMOND:
	case SHPtyFLOW_CHART_DECISION:
	    if  ( tedDrawShapePath( ds, &SP_DIAMOND, &pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyISOSCELES_TRIANGLE:
	case SHPtyFLOW_CHART_EXTRACT:
	    if  ( tedDrawShapePath( ds, &SP_ISOSCELES_TRIANGLE,
							&pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyFLOW_CHART_MERGE:
	    if  ( tedDrawShapePath( ds, &SP_FLOW_CHART_MERGE,
							&pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyRIGHT_TRIANGLE:
	    if  ( tedDrawShapePath( ds, &SP_RIGHT_TRIANGLE,
							&pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPARALLELOGRAM:
	case SHPtyFLOW_CHART_INPUT_OUTPUT:
	    if  ( tedDrawShapePath( ds, &SP_PARALLELOGRAM,
							&pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyTRAPEZOID:
	case SHPtyFLOW_CHART_MANUAL_OPERATION:
	    if  ( tedDrawShapePath( ds, &SP_TRAPEZOID, &pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyHEXAGON:
	case SHPtyFLOW_CHART_PREPARATION:
	    if  ( tedDrawShapePath( ds, &SP_HEXAGON, &pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyOCTAGON:
	    if  ( tedDrawShapePath( ds, &SP_OCTAGON, &pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPLUS_SIGN:
	    if  ( tedDrawShapePath( ds, &SP_PLUS_SIGN, &pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyARROW:
	    if  ( tedDrawShapePath( ds, &SP_ARROW, &pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyNOTCHED_RIGHT_ARROW:
	    if  ( tedDrawShapePath( ds, &SP_NOTCHED_RIGHT_ARROW,
							&pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyHOME_PLATE:
	    if  ( tedDrawShapePath( ds, &SP_HOME_PLATE, &pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyCHEVRON:
	    if  ( tedDrawShapePath( ds, &SP_CHEVRON, &pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_ARROW:
	    if  ( tedDrawShapePath( ds, &SP_LEFT_ARROW, &pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyRIGHT_ARROW_CALLOUT:
	    if  ( tedDrawShapePath( ds, &SP_RIGHT_ARROW_CALLOUT,
							&pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyLEFT_ARROW_CALLOUT:
	    if  ( tedDrawShapePath( ds, &SP_LEFT_ARROW_CALLOUT,
							&pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyUP_ARROW_CALLOUT:
	    if  ( tedDrawShapePath( ds, &SP_UP_ARROW_CALLOUT,
							&pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyDOWN_ARROW_CALLOUT:
	    if  ( tedDrawShapePath( ds, &SP_DOWN_ARROW_CALLOUT,
							&pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_RIGHT_ARROW_CALLOUT:
	    if  ( tedDrawShapePath( ds, &SP_LEFT_RIGHT_ARROW_CALLOUT,
							&pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyUP_DOWN_ARROW_CALLOUT:
	    if  ( tedDrawShapePath( ds, &SP_UP_DOWN_ARROW_CALLOUT,
							&pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyQUAD_ARROW_CALLOUT:
	    if  ( tedDrawShapePath( ds, &SP_QUAD_ARROW_CALLOUT,
							&pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_RIGHT_ARROW:
	    if  ( tedDrawShapePath( ds, &SP_LEFT_RIGHT_ARROW,
							&pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyUP_ARROW:
	    if  ( tedDrawShapePath( ds, &SP_UP_ARROW, &pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyDOWN_ARROW:
	    if  ( tedDrawShapePath( ds, &SP_DOWN_ARROW, &pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyUP_DOWN_ARROW:
	    if  ( tedDrawShapePath( ds, &SP_UP_DOWN_ARROW,
							&pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyQUAD_ARROW:
	    if  ( tedDrawShapePath( ds, &SP_QUAD_ARROW, &pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_RIGHT_UP_ARROW:
	    if  ( tedDrawShapePath( ds, &SP_LEFT_RIGHT_UP_ARROW,
							&pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyLEFT_UP_ARROW:
	    if  ( tedDrawShapePath( ds, &SP_LEFT_UP_ARROW,
							&pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyBENT_UP_ARROW:
	    if  ( tedDrawShapePath( ds, &SP_BENT_UP_ARROW,
							&pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyPENTAGON:
	    if  ( tedDrawShapePath( ds, &SP_PENTAGON, &pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtySTAR:
	    if  ( tedDrawShapePath( ds, &SP_STAR, &pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtySEAL4:
	    if  ( tedDrawShapePath( ds, &SP_SEAL4, &pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case 32:
	case SHPtyLINE:
	    if  ( tedDrawShapePath( ds, &SP_LINE, &pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyFLOW_CHART_MANUAL_INPUT:
	    if  ( tedDrawShapePath( ds, &SP_FLOW_CHART_MANUAL_INPUT,
							&pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;
	case SHPtyFLOW_CHART_OFF_PAGE_CONNECTOR:
	    if  ( tedDrawShapePath( ds, &SP_FLOW_CHART_OFF_PAGE_CONNECTOR,
							&pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyFLOW_CHART_PUNCHED_CARD:
	    if  ( tedDrawShapePath( ds, &SP_FLOW_CHART_PUNCHED_CARD,
							&pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyWEDGE_RECT_CALLOUT:
	    if  ( tedDrawShapePath( ds, &SP_WEDGE_RECT_CALLOUT,
							&pixRect, dc, sdd ) )
		{ LDEB(1); rval= -1;	}
	    break;

	case SHPtyFLOW_CHART_ONLINE_STORAGE:
	    {
	    const ShapePath *	sp= &SP_RECTANGLE;
	    int			np= sp->spVertexCount;
	    APP_POINT *		xp;

	    int			wide;
	    int			high;
	    int			rx0;
	    int			rx1;

	    xp= malloc( ( np+ 1 )* sizeof(APP_POINT) );
	    if  ( ! xp )
		{ LXDEB(np,xp); return -1;	}

	    tedDrawSetShapePath( xp, ds, sp, &pixRect );

	    wide= pixRect.drX1- pixRect.drX0;
	    high= pixRect.drY1- pixRect.drY0;
	    rx0= ( 5* pixRect.drX0+ 1* pixRect.drX1 )/ 6;
	    rx1= ( 1* pixRect.drX0+ 5* pixRect.drX1 )/ 6;

	    docShapeGetFill( &fill, ds, dc, sdd );
	    if  ( fill )
		{
		appDrawFillArc( add, pixRect.drX0, pixRect.drY0,
				    ( wide+ 2 )/ 3, high+ 1,
				    64* 90, 64* 180 );

		appDrawFillRectangle( add, rx0, pixRect.drY0,
				    rx1- rx0+ 1, high );

		/*
		appDrawFillArc( add, pixRect.drX0,
				    ( 1* pixRect.drY0+ 2* pixRect.drY1 )/ 3,
				    wide+ 1, ( high+ 2 )/ 3,
				    64* 180, 64* 180 );
		*/
		}
	    tedShapeGetLine( &line, &widthPixels, ds, dc, sdd );
	    if  ( line )
		{
		appDrawDrawArc( add, pixRect.drX0, pixRect.drY0,
				    ( wide+ 2 )/ 3, high+ 1,
				    64* 90, 64* 180 );

		appDrawDrawLine( add, rx0,  pixRect.drY0, rx1,  pixRect.drY0 );
		appDrawDrawLine( add, rx0,  pixRect.drY1, rx1,  pixRect.drY1 );

		appDrawDrawArc( add, ( 1* pixRect.drX0+ 2* pixRect.drX1 )/ 3,
				    pixRect.drY0,
				    ( wide+ 2 )/ 3, high+ 1,
				    64* 90, 64* 180 );
		}

	    free( xp );
	    }
	    break;


	case SHPtyCAN:
	case SHPtyFLOW_CHART_MAGNETIC_DISK:
	    {
	    const ShapePath *	sp= &SP_RECTANGLE;
	    int			np= sp->spVertexCount;
	    APP_POINT *		xp;

	    int			wide;
	    int			high;
	    int			ry0;
	    int			ry1;

	    xp= malloc( ( np+ 1 )* sizeof(APP_POINT) );
	    if  ( ! xp )
		{ LXDEB(np,xp); return -1;	}

	    tedDrawSetShapePath( xp, ds, sp, &pixRect );

	    wide= pixRect.drX1- pixRect.drX0;
	    high= pixRect.drY1- pixRect.drY0;
	    ry0= ( 5* pixRect.drY0+ 1* pixRect.drY1 )/ 6;
	    ry1= ( 1* pixRect.drY0+ 5* pixRect.drY1 )/ 6;

	    docShapeGetFill( &fill, ds, dc, sdd );
	    if  ( fill )
		{
		appDrawFillArc( add, pixRect.drX0, pixRect.drY0,
				    wide+ 1, ( high+ 2 )/ 3,
				    64* 0, 64* 360 );

		appDrawFillRectangle( add, pixRect.drX0, ry0,
				    wide, ry1- ry0+ 1 );

		appDrawFillArc( add, pixRect.drX0,
				    ( 1* pixRect.drY0+ 2* pixRect.drY1 )/ 3,
				    wide+ 1, ( high+ 2 )/ 3,
				    64* 180, 64* 180 );
		}
	    tedShapeGetLine( &line, &widthPixels, ds, dc, sdd );
	    if  ( line )
		{
		appDrawDrawArc( add, pixRect.drX0, pixRect.drY0,
				    wide+ 1, ( high+ 2 )/ 3,
				    64* 0, 64* 360 );

		appDrawDrawLine( add, pixRect.drX0, ry0,
				    pixRect.drX0, ry1 );
		appDrawDrawLine( add, pixRect.drX1, ry0,
				    pixRect.drX1, ry1 );

		appDrawDrawArc( add, pixRect.drX0,
				    ( 1* pixRect.drY0+ 2* pixRect.drY1 )/ 3,
				    wide+ 1, ( high+ 2 )/ 3,
				    64* 180, 64* 180 );
		}

	    free( xp );
	    }
	    break;

	case SHPtyRIGHT_BRACE:
	    {
	    const ShapePath *	sp= &SP_RECTANGLE;
	    int			np= sp->spVertexCount;
	    APP_POINT *		xp;

	    int			wide;
	    int			high;

	    xp= malloc( ( np+ 1 )* sizeof(APP_POINT) );
	    if  ( ! xp )
		{ LXDEB(np,xp); return -1;	}

	    tedDrawSetShapePath( xp, ds, sp, &pixRect );

	    wide= xp[2].x- xp[0].x;
	    high= xp[2].y- xp[0].y;

	    tedShapeGetLine( &line, &widthPixels, ds, dc, sdd );

	    appDrawDrawArc( add, xp[0].x- wide/ 2,
				    ( 8* xp[0].y+ 0* xp[2].y )/ 8,
				    wide+ 1, ( high+ 3 )/ 4,
				    64* 0, 64* 90 );

	    appDrawDrawLine( add,
		    xp[0].x+ wide/ 2, ( 7* xp[0].y+ 1* xp[2].y )/ 8,
		    xp[0].x+ wide/ 2, ( 5* xp[0].y+ 3* xp[2].y )/ 8 );

	    appDrawDrawArc( add, xp[2].x- wide/ 2,
				    ( 6* xp[0].y+ 2* xp[2].y )/ 8,
				    wide+ 1, ( high+ 3 )/ 4,
				    64* 180, 64* 90 );

	    appDrawDrawArc( add, xp[2].x- wide/ 2,
				    ( 4* xp[0].y+ 4* xp[2].y )/ 8,
				    wide+ 1, ( high+ 3 )/ 4,
				    64* 90, 64* 90 );

	    appDrawDrawLine( add,
		    xp[0].x+ wide/ 2, ( 3* xp[0].y+ 5* xp[2].y )/ 8,
		    xp[0].x+ wide/ 2, ( 1* xp[0].y+ 7* xp[2].y )/ 8 );

	    appDrawDrawArc( add, xp[0].x- wide/ 2,
				    ( 2* xp[0].y+ 6* xp[2].y )/ 8,
				    wide+ 1, ( high+ 3 )/ 4,
				    64* 270, 64* 90 );

	    free( xp );
	    }
	    break;

	case SHPtyFREEFORM_OR_NON_AUTOSHAPE:
	    if  ( ds->dsVertexCount > 1 )
		{
		const int	closed= 1;

		xp= tedDrawGetVertices( twipsRect, ds, add, sdd );
		if  ( ! xp )
		    { XDEB(xp); rval= -1; goto ready;	}

		tedShapeDrawPoints( ds, xp, ds->dsVertexCount- 1,
							closed, dc, sdd );
		}
	    break;

	case SHPtyBENT_ARROW:
	default:
	    LSDEB(ds->dsShapeType,docShapeTypeString(ds->dsShapeType));
	    break;
	}

    /* Done by docDrawDrawingShape()....
    if  ( ds->dsExternalItem.eiItem )
	{}
    */

  ready:
    if  ( xp )
	{ free( xp ); }

    return rval;
    }

