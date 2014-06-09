/************************************************************************/
/*									*/
/*  Ted, generic shape drawing functionality.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<math.h>

#   include	"docDraw.h"

#   include	<appDebugon.h>

int docShapeGetFill(	int *			pFill,
			const DrawingShape *	ds,
			DrawingContext *	dc,
			void *			through )
    {
    RGB8Color	co;
    int		fill= 1;

    if  ( ! ds->ds_fFilled )
	{ *pFill= 0; return 0; }
    if  ( ds->dsFillOpacity == 0 )
	{ *pFill= 0; return 0; }

    switch( ds->dsFillType )
	{
	case DSfillSOLID:
	    co= ds->dsFillColor;
	    fill= 1;
	    break;

	case DSfillSHADE_START_TO_END:
	case DSfillSHADE_BOUNDS_TO_END:
	case DSfillSHADE_OUTLINE_TO_END:
	case DSfillSHADE_ANGLE:
	    /*LDEB(ds->dsFillType);*/
	    co= ds->dsFillColor;
	    fill= 1;
	    break;

	default:
	    LDEB(ds->dsFillType);
	    return -1;
	}

    if  ( fill )
	{
	docDrawSetColorRgb( dc, through,
				    co.rgb8Red, co.rgb8Green, co.rgb8Blue );
	}

    *pFill= fill;
    return 0;
    }

int docShapeGetLine(	int *			pLine,
			const DrawingShape *	ds,
			DrawingContext *	dc,
			void *			through )
    {
    RGB8Color	co;
    int		line= 1;

    if  ( ! ds->ds_fLine )
	{ *pLine= 0; return 0; }

    switch( ds->dsLineType )
	{
	case DSlineSOLID:
	    co= ds->dsLineColor;
	    line= 1;
	    break;

	default:
	    LDEB(ds->dsLineType);
	    return -1;
	}

    if  ( line )
	{
	docDrawSetColorRgb( dc, through,
				    co.rgb8Red, co.rgb8Green, co.rgb8Blue );
	}

    *pLine= line;
    return 0;
    }


/************************************************************************/
/*									*/
/*  Get the pixel rectangle for a shape.				*/
/*									*/
/************************************************************************/

void docShapeGetPixelRect(	DocumentRectangle *		pixelRect,
				const DrawingContext *		dc,
				const DocumentRectangle *	twipsRect,
				int				page )
    {
    const AppDrawingData *	add= dc->dcDrawingData;

    LayoutPosition		lp0;
    LayoutPosition		lp1;

    lp0.lpPage= lp1.lpPage= page;
    lp0.lpPageYTwips= twipsRect->drY0;
    lp1.lpPageYTwips= twipsRect->drY1;

    pixelRect->drX0= X_PIXELS( add, twipsRect->drX0 );
    pixelRect->drX1= X_PIXELS( add, twipsRect->drX1 );
    pixelRect->drY0= LP_YPIXELS( add, &lp0 );
    pixelRect->drY1= LP_YPIXELS( add, &lp1 );

    return;
    }

/************************************************************************/
/*									*/
/*  Traverse a tree of shapes and draw the leaves.			*/
/*									*/
/*  1)  Clip leaf shapes: It is unknown whether chidren can protrude	*/
/*	from their parents. As a matter of fact, it is even unclear	*/
/*	whether a free form can leave its rectangle.			*/
/*									*/
/************************************************************************/

static int docDrawDrawingShape(	const DocumentRectangle *	twipsRect,
				int				page,
				DrawingShape *			ds,
				DrawingContext *		dc,
				void *				through )
    {
    int			rval= 0;
    int			child;

    DocumentRectangle	drHere= *twipsRect;

    if  ( DSflipHORIZONTAL( ds ) )
	{ int sw= drHere.drX0; drHere.drX0= drHere.drX1; drHere.drX1= sw; }

    if  ( DSflipVERTICAL( ds ) )
	{ int sw= drHere.drY0; drHere.drY0= drHere.drY1; drHere.drY1= sw; }

    if  ( ds->dsShapeType == SHPtyGROUP )
	{
	double			xfac;
	double			yfac;

	xfac= ( 1.0* ( drHere.drX1- drHere.drX0 ) )/
			    ( ds->dsGroupRect.drX1- ds->dsGroupRect.drX0 );
	yfac= ( 1.0* ( drHere.drY1- drHere.drY0 ) )/
			    ( ds->dsGroupRect.drY1- ds->dsGroupRect.drY0 );

	for ( child= 0; child < ds->dsChildCount; child++ )
	    {
	    DrawingShape *	dsChild= ds->dsChildren[child];
	    DocumentRectangle	drChild;

	    drChild.drX0= drHere.drX0+ xfac* 
			( dsChild->dsRelRect.drX0- ds->dsGroupRect.drX0 );
	    drChild.drY0= drHere.drY0+ yfac*
			( dsChild->dsRelRect.drY0- ds->dsGroupRect.drY0 );
	    drChild.drX1= drHere.drX0+ xfac* 
			( dsChild->dsRelRect.drX1- ds->dsGroupRect.drX0 );
	    drChild.drY1= drHere.drY0+ yfac*
			( dsChild->dsRelRect.drY1- ds->dsGroupRect.drY0 );

	    if  ( docDrawDrawingShape( &drChild, page, dsChild, dc, through ) )
		{ LDEB(child); rval= -1;	}
	    }
	}
    else{
	if  ( dc->dcClipRect )
	    {
	    DocumentRectangle	pixelRect;

	    docShapeGetPixelRect( &pixelRect, dc, twipsRect, page );

	    if  ( ! geoIntersectRectangle( (DocumentRectangle *)0,
						dc->dcClipRect, &pixelRect ) )
		{ return 0;	}
	    }

	if  ( ! dc->dcDrawShape						||
	      (*dc->dcDrawShape)( twipsRect, page, ds, dc, through )	)
	    { LDEB(1); rval= -1;	}

	dc->dcCurrentTextAttributeSet= 0;
	dc->dcCurrentColorSet= 0;

	if  ( ds->dsExternalItem.eiItem )
	    {
	    if  ( page != ds->dsExternalItem.eiPageFormattedFor	&&
		  docShapeCheckTextLayout( ds, &drHere,
			(DocumentRectangle *)0, page, dc->dcDocument,
			dc->dcDrawingData, dc->dcScreenFontList,
			dc->dcInitLayoutExternal, dc->dcCloseObject ) )
		{ LDEB(1); return -1;	}

	    if  ( docDrawItem( ds->dsExternalItem.eiItem, through, dc ) )
		{ LDEB(1);	}
	    }
	}

    return rval;
    }

int docDrawShape(	DrawingContext *		dc,
			void *				through,
			const InsertedObject *		io )
    {
    DrawingShape *		ds= io->ioDrawingShape;
    const ShapeProperties *	sp= &(ds->dsShapeProperties);

    int				page= io->ioY0Position.lpPage;

    DocumentRectangle		twipsRect;

    if  ( ! ds )
	{ XDEB(ds); return 0;	}

    twipsRect.drX0= io->ioX0Twips;
    twipsRect.drX1= twipsRect.drX0+ sp->spRect.drX1- sp->spRect.drX0;
    twipsRect.drY0= io->ioY0Position.lpPageYTwips;
    twipsRect.drY1= twipsRect.drY0+ sp->spRect.drY1- sp->spRect.drY0;

    if  ( dc->dcClipRect )
	{
	DocumentRectangle	pixRect;

	docShapeGetPixelRect( &pixRect, dc, &twipsRect, page );

	if  ( ! geoIntersectRectangle( (DocumentRectangle *)0,
					    &pixRect, dc->dcClipRect ) )
	    { return 0;	}
	}

    docDrawDrawingShape( &twipsRect, page, ds, dc, through );

    return 0;
    }

