/************************************************************************/
/*									*/
/*  Layout utility functions relating to DrawingShapes			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	"docLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Remember that the layout calculations for a shape must be redone.	*/
/*									*/
/************************************************************************/

void docDrawingShapeInvalidateTextLayout(	DrawingShape *	ds )
    {
    int		child;

    ds->dsExternalItem.eiPageFormattedFor= -1;

    for ( child= 0; child < ds->dsChildCount; child++ )
	{ docDrawingShapeInvalidateTextLayout( ds->dsChildren[child] ); }

    return;
    }

/************************************************************************/
/*									*/
/*  Calculate the page rectangle for a shape.				*/
/*									*/
/*  1)  Note that += for lpPageYTwips gives problems: It is an unsigned	*/
/*	short for memory efficiency reasons.				*/
/*									*/
/************************************************************************/

void docShapePageY(		LayoutPosition *	lpShapeTop,
				LayoutPosition *	lpBelowShape,
				const DrawingShape *	ds,
				const BufferItem *	paraBi,
				const LayoutPosition *	lpLineTop,
				const ParagraphFrame *	pf )
    {
    const ShapeProperties *	sp= &(ds->dsShapeProperties);
    int				by= sp->spVerticalAttachment;
    int				high= sp->spRect.drY1- sp->spRect.drY0;

    if  ( by == SHPbyIGNORE )
	{ by= ds->dsVerticalRelativeTo; }

    *lpShapeTop= *lpLineTop;

    switch( by )
	{
	case SHPbyMARGIN:
	    lpShapeTop->lpPageYTwips= pf->pfBlockY0Twips;
	    break;

	case SHPbyPAGE:
	    lpShapeTop->lpPageYTwips= 0;
	    break;

	case SHPbyPARA:
	    *lpShapeTop= paraBi->biTopPosition;
	    break;

	case SHPbyLINE:
	    *lpShapeTop= *lpLineTop;
	    break;

	case SHPbyIGNORE:
	default:
	    LDEB(by);
	    *lpShapeTop= paraBi->biTopPosition;
	    break;
	}

    /*  1  */
    if  ( sp->spRect.drY0 < -(int)lpShapeTop->lpPageYTwips )
	{ lpShapeTop->lpPageYTwips= 0;	}
    else{
	lpShapeTop->lpPageYTwips= (int)lpShapeTop->lpPageYTwips+ 
						    sp->spRect.drY0; /*  1  */
	}

    *lpBelowShape= *lpShapeTop;
    lpBelowShape->lpPageYTwips += high;

    return;
    }

void docShapePageRectangle(	LayoutPosition *	lpShapeTop,
				LayoutPosition *	lpBelowShape,
				int *			pX0,
				int *			pX1,
				const DrawingShape *	ds,
				const BufferItem *	paraBi,
				const LayoutPosition *	lpLineTop,
				const ParagraphFrame *	pf,
				int			x0Twips )
    {
    const ShapeProperties *	sp= &(ds->dsShapeProperties);
    int				bx= sp->spHorizontalAttachment;
    int				x0;
    int				wide= sp->spRect.drX1- sp->spRect.drX0;

    docShapePageY( lpShapeTop, lpBelowShape, ds, paraBi, lpLineTop, pf );

    if  ( bx == SHPbxIGNORE )
	{ bx= ds->dsHorizontalRelativeTo; }

    switch( bx )
	{
	case SHPbxMARGIN:
	    x0= pf->pfBlockX0Twips;
	    x0 += sp->spRect.drX0;
	    break;

	case SHPbxPAGE:
	    x0= pf->pfBlockX0Twips;
	    x0 += sp->spRect.drX0;
	    break;

	case SHPbxCOLUMN:
	    x0= pf->pfBlockX0Twips;
	    x0 += sp->spRect.drX0;
	    break;

	case SHPbxCHARACTER:
	    x0= x0Twips;
	    x0 += sp->spRect.drX0;
	    break;

	case SHPbxIGNORE:
	default:
	    LDEB(bx);
	    x0= pf->pfBlockX0Twips;
	    x0 += sp->spRect.drX0;
	    break;
	}

    *pX0= x0;
    *pX1= x0+ wide;

    return;
    }

/************************************************************************/
/*									*/
/*  Recalculate the layout of a text box (shptxt) if necessary.		*/
/*									*/
/************************************************************************/

int docShapeCheckTextLayout(
			DrawingShape *			ds,
			const DocumentRectangle *	twipsRect,
			DocumentRectangle *		drChanged,
			int				pageNumber,
			BufferDocument *		bd,
			AppDrawingData *		add,
			ScreenFontList *		sfl,
			INIT_LAYOUT_EXTERNAL		initLayoutExternal,
			DOC_CLOSE_OBJECT		closeObject )
    {
    DocumentGeometry	dgBox;

    int			wide;
    int			high;

    high= twipsRect->drY1- twipsRect->drY0;

    utilInitDocumentGeometry( &dgBox );

    if  ( twipsRect->drX0 <= twipsRect->drX1 )
	{
	wide= twipsRect->drX1- twipsRect->drX0;
	dgBox.dgLeftMarginTwips= twipsRect->drX0;
	}
    else{
	wide= twipsRect->drX0- twipsRect->drX1;
	dgBox.dgLeftMarginTwips= twipsRect->drX1;
	}

    if  ( twipsRect->drY0 <= twipsRect->drY1 )
	{
	high= twipsRect->drY1- twipsRect->drY0;
	dgBox.dgTopMarginTwips= twipsRect->drY0;
	}
    else{
	high= twipsRect->drY0- twipsRect->drY1;
	dgBox.dgTopMarginTwips= twipsRect->drY1;
	}

    dgBox.dgRightMarginTwips=  dgBox.dgPageWideTwips- 
					dgBox.dgLeftMarginTwips- wide;

    dgBox.dgLeftMarginTwips += EMUtoTWIPS( ds->ds_dxTextLeft );
    dgBox.dgTopMarginTwips += EMUtoTWIPS( ds->ds_dyTextTop );
    dgBox.dgRightMarginTwips += EMUtoTWIPS( ds->ds_dxTextRight );
    /*
    Irrelevant:
    dgBox.dgBottomMarginTwips= 
    */

    ds->dsExternalItem.eiItem->biSectDocumentGeometry= dgBox;

    if  ( pageNumber != ds->dsExternalItem.eiPageFormattedFor	&&
	  docLayoutExternalItem( &(ds->dsExternalItem),
		drChanged, pageNumber, dgBox.dgTopMarginTwips,
		bd, (BufferItem *)0, add, sfl,
		initLayoutExternal, closeObject ) )
	{ LDEB(1); return -1;	}

    return 0;
    }
