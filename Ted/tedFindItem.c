/************************************************************************/
/*									*/
/*  Geometry calculations when looking for a position in the document.	*/
/*									*/
/*  Find the object in the document for an x,y position.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<limits.h>
#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	"tedApp.h"
#   include	"tedLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Find the cell in a table row that contains the given x,y position	*/
/*									*/
/*  1)  For all columns..						*/
/*  2)  Skip cells that just make up a rowspan/colspan.			*/
/*  3)  For cells with a colspan.. determine the right hand side.	*/
/*  4)  Skip cell to the left of the mouse click. (But give the last	*/
/*	one a chance anyway.						*/
/*  5)  For the top cell of a rowspan, find the bottom of the row where	*/
/*	it ends.							*/
/*  6)  Check the bottom of the cell. Normally this is done by the	*/
/*	caller, but because of rowspan calls, her criterion could have	*/
/*	been too lenient.						*/
/*  7)  Found!								*/
/*									*/
/************************************************************************/

static BufferItem * tedFindRowCell(	BufferItem *		rowBi,
					const AppDrawingData *	add,
					int			x,
					int			y )
    {
    const CellProperties *	cp;
    int				i;

    int				rowBottom= BI_BELOW_PIXELS( add, rowBi );

    /*  1  */
    cp= rowBi->biRowProperties.rpCells;
    for ( i= 0; i < rowBi->biChildCount; cp++, i++ )
	{
	BufferItem *	cellBi;
	int		j;

	int		cellRight= cp->cpRightBoundaryPixels;
	int		cellBottom= rowBottom;

	/*  2  */
	if  ( cp->cpMergedWithLeft || cp->cpMergedWithAbove )
	    { continue; }

	/*  3  */
	if  ( cp->cpLeftInMergedRow )
	    {
	    const CellProperties *	cpp= cp;

	    for ( j= i; j < rowBi->biChildCount- 1; cpp++, j++ )
		{
		if  ( ! cpp[1].cpMergedWithLeft )
		    { break;	}

		cellRight= cpp[1].cpRightBoundaryPixels;
		}
	    }

	/*  4  */
	if  ( x > add->addDocRect.drX0+ cellRight		&&
	      i < rowBi->biChildCount- 1			)
	    { continue;	}

	cellBi= rowBi->biChildren[i];

	/*  5  */
	if  ( cp->cpTopInMergedColumn )
	    {
	    int			nextRow= rowBi->biNumberInParent+ 1;
	    BufferItem *	nextRowBi= (BufferItem *)0;

	    while( nextRow < rowBi->biRowTablePast )
		{
		const CellProperties *	nextCp;

		nextRowBi= rowBi->biParent->biChildren[nextRow];
		nextCp= &(nextRowBi->biRowCells[i]);

		if  ( ! nextCp->cpMergedWithAbove )
		    { break;	}

		cellBottom= BI_BELOW_PIXELS( add, nextRowBi );
		nextRow++;
		}
	    }

	/*  6  */
	if  ( y > cellBottom )
	    { continue;	}

	/*  7  */
	return cellBi;
	}

    /*
    LLDEB(x,y);
    LDEB(rowBi->biChildCount);
    */
    return (BufferItem *)0;
    }

/************************************************************************/
/*									*/
/*  Find a child in a parent that stacks its children in the normal	*/
/*  page order.								*/
/*									*/
/*  1)  For all children..						*/
/*  2)  A subsequent child?						*/
/*  3)  Determine the position below the child. Rows with 'rowspan'	*/
/*	children might protrude below below the actual bottom of the	*/
/*	child.								*/
/*  4)  If there is a 'next' on the current page and the y is below the	*/
/*	bottom of the current child, skip the current child.		*/
/*  5)  For rows, try to find a cell of the row. (Skips a level!)	*/
/*  6)  All reasons to reject the child have been covered above so	*/
/*	return this child.						*/
/*									*/
/************************************************************************/

static BufferItem * tedFindGroupChild(	BufferItem *		bi,
					const AppDrawingData *	add,
					int			x,
					int			y,
					int			page )
    {
    int			i;
    BufferItem *	child;

    /*  1  */
    for ( i= 0; i < bi->biChildCount; i++ )
	{
	BufferItem *	next= (BufferItem *)0;
	LayoutPosition	lpBelow;

	child= bi->biChildren[i];

	/*  2  */
	if  ( i+ 1 < bi->biChildCount )
	    { next= bi->biChildren[i+ 1];	}

	/*  3  */
	lpBelow= child->biBelowPosition;

	if  ( child->biLevel == DOClevROW	&&
	      child->biRowHasTableParagraphs	)
	    { lpBelow= child->biRowBelowAllPosition; }

	/*  4  */
	if  ( y > LP_YPIXELS( add, &lpBelow )		&&
	      next					&&
	      next->biTopPosition.lpPage <= page	)
	    { continue;	}

	/*  5  */
	if  ( child->biLevel == DOClevROW	&&
	      child->biRowHasTableParagraphs	)
	    {
	    BufferItem *	cellBi;

	    cellBi= tedFindRowCell( child, add, x, y );
	    if  ( cellBi )
		{ return cellBi; }

	    continue;
	    }

	return child;
	}

    if  ( bi->biChildCount > 0 )
	{
	child= bi->biChildren[bi->biChildCount- 1];

	return child;
	}

    XDEB(bi->biChildCount);
    return (BufferItem *)0;
    }

/************************************************************************/
/*									*/
/*  Find the object in the document for an x,y position.		*/
/*									*/
/************************************************************************/

static BufferItem * tedFindItem(	BufferItem *		bi,
					const AppDrawingData *	add,
					int			x,
					int			y )
    {
    BufferItem *		child;
    BufferItem *		found;
    int				page= y/ add->addPageStepPixels;

    switch( bi->biLevel )
	{
	case DOClevDOC:
	case DOClevSECT:
	case DOClevCELL:
	rowAsGroup:

	    child= tedFindGroupChild( bi, add, x, y, page );
	    if  ( child )
		{
		found= tedFindItem( child, add, x, y );
		return found;
		}

	    LDEB(bi->biChildCount);
	    break;

	case DOClevROW:
	    if  ( ! bi->biRowHasTableParagraphs )
		{ goto rowAsGroup;	}

	    child= tedFindRowCell( bi, add, x, y );
	    if  ( child )
		{
		found= tedFindItem( child, add, x, y );
		return found;
		}

	    LLDEB(x,y);
	    return (BufferItem *)0;

	case DOClevPARA:
	    return bi;

	default:
	    LDEB(bi->biLevel); return (BufferItem *)0;
	}

    LSDEB(y,docLevelStr(bi->biLevel)); return (BufferItem *)0;
    }

static int tedFindLine(	const BufferItem *	bi,
			const AppDrawingData *	add,
			int			x,
			int			y	)
    {
    int			page= y/ add->addPageStepPixels;

    int			i;
    TextLine *		tl;
    int			y1;

    if  ( y < BI_TOP_PIXELS( add, bi ) )
	{ return 0;	}

    if  ( bi->biLevel != DOClevPARA )
	{ LLDEB(bi->biLevel,DOClevPARA); return -1;	}

    tl= bi->biParaLines;
    for ( i= 0; i < bi->biParaLineCount; tl++, i++ )
	{
	y1= TL_BELOW_PIXELS( add, tl );

	if  ( y < y1						||
	      ( i+ 1 < bi->biParaLineCount		&&
	        tl[1].tlTopPosition.lpPage > page	)	)
	    { return i;	}
	}

    if  ( bi->biParaLineCount > 0 )
	{ return bi->biParaLineCount- 1;	}

    LLDEB(y,bi->biParaLineCount); return -1;
    }

int tedFindParticule(	TextLine *	tl,
			TextParticule *	tp,
			int		x,
			int		y )
    {
    int				i;

    tp += tl->tlFirstParticule;

    if  ( x < tp->tpX0 )
	{ return tl->tlFirstParticule;	}

    for ( i= 0; i < tl->tlParticuleCount; tp++, i++ )
	{
	if  ( tp->tpX0 <= x &&  x < tp->tpX0+ tp->tpPixelsWide )
	    { return tl->tlFirstParticule+ i;	}
	}

    return tl->tlFirstParticule+ tl->tlParticuleCount- 1;
    }

/************************************************************************/
/*									*/
/*  Return the X position for an I bar.					*/
/*									*/
/************************************************************************/

static int tedCalculateX(	const BufferDocument *	bd,
				const BufferItem *	bi,
				const TextParticule *	tp,
				const AppDrawingData *	add,
				const ScreenFontList *	sfl,
				int			stroff )
    {
    const DrawScreenFontList *	apfl= &(add->addScreenFontList);
    const unsigned char *	s;
    int				len;
    int				x;

    TextAttribute		ta;
    int				textAttr;
    int				screenFont;
    DrawScreenFont *		dsf;

    switch( tp->tpKind )
	{
	case DOCkindOBJECT:
	case DOCkindTAB:
	case DOCkindCHFTNSEP:
	case DOCkindCHFTNSEPC:
	    len= stroff- tp->tpStroff;

	    if  ( len == 0 )
		{ x= tp->tpX0;	}
	    else{
		if  ( len == 1 )
		    { x= tp->tpX0+ tp->tpPixelsWide;	}
		else{
		    LLDEB(stroff,tp->tpStroff);
		    docListParticule( 0, "???", tp-bi->biParaParticules,
								    bi, tp );
		    docListItem( 0, bi );
		    x= tp->tpX0;
		    }
		}
	    return x;

	case DOCkindTEXT:
	    textAttr= tp->tpTextAttributeNumber;

	    if  ( textAttr >= sfl->sflAttributeToScreenCount	||
		  sfl->sflAttributeToScreen[textAttr] < 0	)
		{ LDEB(textAttr); return tp->tpX0;	}

	    screenFont= sfl->sflAttributeToScreen[textAttr];
	    dsf= apfl->apflFonts+ screenFont;

	    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
								textAttr );

	    s= bi->biParaString+ tp->tpStroff;
	    len= stroff- tp->tpStroff;

	    if  ( len == tp->tpStrlen )
		{ x= tp->tpX0+ tp->tpPixelsWide;			}
	    else{ x= tp->tpX0+ tedTextWidth( dsf, &ta, bd, add, s, len ); }
	    return x;

	case DOCkindNOTE:
	case DOCkindFIELDSTART:
	case DOCkindFIELDEND:
	case DOCkindXE:
	case DOCkindTC:
	case DOCkindLINEBREAK:
	case DOCkindPAGEBREAK:
	case DOCkindCOLUMNBREAK:
	    x= tp->tpX0;
	    return x;

	default:
	    LDEB(tp->tpKind);
	    x= tp->tpX0;
	    return x;
	}
    }

/************************************************************************/
/*									*/
/*  Return the offset in the paragraph string of a mouse event		*/
/*									*/
/*  1)  For Empty particules report the position of the beginning of	*/
/*	the particule.							*/
/*	For clicks in the left margin, report the position of the	*/
/*	particule.							*/
/*  2)  Note the <=, also consider the position after the last		*/
/*	character.							*/
/*									*/
/************************************************************************/

int tedFindStringOffset(	const BufferDocument *	bd,
				const BufferItem *	paraBi,
				int			part,
				const AppDrawingData *	add,
				const ScreenFontList *	sfl,
				int *			pBarX,
				int			x,
				int			y )
    {
    const TextParticule *	tp= paraBi->biParaParticules+ part;
    const DrawScreenFontList *	apfl= &(add->addScreenFontList);
    int				len;
    int				stroff= tp->tpStroff+ 1;
    const unsigned char *	s= paraBi->biParaString+ tp->tpStroff;
    int				before= tp->tpX0;
    int				past= tp->tpX0;

    int				textAttr;
    int				screenFont;
    DrawScreenFont *		dsf;

    TextAttribute		ta;

    /*  1  */
    if  ( x < before || tp->tpStrlen == 0 )
	{ *pBarX= tp->tpX0; return tp->tpStroff; }

    textAttr= tp->tpTextAttributeNumber;

    if  ( textAttr >= sfl->sflAttributeToScreenCount	||
	  sfl->sflAttributeToScreen[textAttr] < 0	)
	{ LDEB(textAttr); *pBarX= tp->tpX0; return tp->tpStroff;	}

    screenFont= sfl->sflAttributeToScreen[textAttr];
    dsf= apfl->apflFonts+ screenFont;

    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

    /*  2  */
    for ( len= 1; len <= tp->tpStrlen; stroff++, len++ )
	{
	int	between;

	if  ( len == tp->tpStrlen )
	    { past= tp->tpX0+ tp->tpPixelsWide;				}
	else{ past= tp->tpX0+ tedTextWidth( dsf, &ta, bd, add, s, len ); }

	between= ( before+ past )/ 2;

	if  ( before <= x && x < between )
	    { *pBarX= before; return stroff- 1;	}
	if  ( between <= x && x < past )
	    { *pBarX= past; return stroff;		}

	before= past;
	}

    *pBarX= tp->tpX0+ tp->tpPixelsWide;
    return tp->tpStroff+ tp->tpStrlen;
    }

/************************************************************************/
/*									*/
/*  Translate a Window coordinate to a position in a text buffer.	*/
/*									*/
/************************************************************************/

int tedFindPosition(	DocumentPosition *		dp,
			PositionGeometry *		pg,
			int *				pPart,
			const BufferDocument *		bd,
			BufferItem *			rootBi,
			const AppDrawingData *		add,
			const ScreenFontList *		sfl,
			int				x,
			int				y )
    {
    const BufferItem *	bi;

    int			part;
    int			stroff;
    int			barX;
    int			line;

    TextLine *		tl;

    bi= tedFindItem( rootBi, add, x, y );
    if  ( ! bi )
	{
	if  ( y < BI_TOP_PIXELS( add, rootBi ) )
	    {
	    const int	lastOne= 0;

	    if  ( docFirstPosition( dp, rootBi ) )
		{ LDEB(y); return -1; }

	    tedPositionGeometry( pg, dp, lastOne, bd, add, sfl );
	    return 0;
	    }

	if  ( y >= BI_BELOW_PIXELS( add, rootBi ) )
	    {
	    const int	lastOne= 0;

	    if  ( docLastPosition( dp, rootBi ) )
		{ LDEB(y); return -1; }

	    tedPositionGeometry( pg, dp, lastOne, bd, add, sfl );
	    return 0;
	    }

	LDEB(y);
	return -1;
	}

    if  ( bi->biLevel != DOClevPARA )
	{ LLDEB(bi->biLevel,DOClevPARA); return -1;	}

    if  ( bi->biParaLineCount > 0				&&
	  y < TL_TOP_PIXELS( add, &(bi->biParaLines[0]) )	)
	{
	line= 0;
	part= 0;
	stroff= 0;
	barX= bi->biParaParticules[0].tpX0;
	}
    else{
	line= tedFindLine( bi, add, x, y );
	if  ( line < 0 )
	    {
	    if  ( bi->biParaLineCount < 1 )
		{ LDEB(bi->biParaLineCount); return -1;	}

	    if  ( y > BI_BELOW_PIXELS( add, bi )	&&
		  bi->biParaLineCount > 0		&&
		  bi->biParaParticuleCount > 0		)
		{
		line= bi->biParaLineCount- 1;
		part= bi->biParaParticuleCount- 1;
		stroff= bi->biParaStrlen;
		barX= bi->biParaParticules[part].tpX0+
				    bi->biParaParticules[part].tpPixelsWide;
		}
	    else{
		line= 0; part= 0; stroff= 0;
		if  ( bi->biParaParticuleCount == 0 )
		    { LLDEB(line,bi->biParaParticuleCount); return -1;	}
		barX= bi->biParaParticules[part].tpX0;
		}
	    }
	else{
	    part= tedFindParticule( bi->biParaLines+ line,
						bi->biParaParticules, x, y );
	    if  ( part < 0 )
		{ LLDEB(x,part); return -1;			}

	    stroff= tedFindStringOffset( bd, bi, part, add, sfl, &barX, x, y );
	    if  ( stroff < 0 )
		{ LDEB(stroff); return -1;	}
	    }
	}

#   if 0
    {
    TextParticule *	tp= bi->biParaParticules+ part;

    appDebug( "(%3d,%3d): %.*s|%.*s\n", x, y,
			stroff- tp->tpStroff, bi->biParaString+ tp->tpStroff,
			tp->tpStroff+ tp->tpStrlen- stroff,
			bi->biParaString+ stroff );
    }
#   endif

    tl= bi->biParaLines+ line;

    dp->dpBi= (BufferItem *)bi;	/*  Discards const */
    dp->dpStroff= stroff;

    *pPart= part;

    pg->pgLine= line;
    pg->pgAtLineHead= 0;
    pg->pgAtLineEnd= 0;
    if  ( stroff == tl->tlStroff )
	{ pg->pgAtLineHead= 1;	}
    if  ( stroff == tl->tlStroff+ tl->tlStrlen )
	{ pg->pgAtLineEnd= 1;	}

    pg->pgXPixels= barX;
    pg->pgTopPosition= tl->tlTopPosition;
    pg->pgY1Pixels= TL_BELOW_PIXELS( add, tl );
    pg->pgBaselinePixels= TL_BASE_PIXELS( add, tl );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Calculate the smallest rectangle that contains the selection.	*/
/*									*/
/*  1)  Same paragraph?							*/
/*  1b) For a single position, widen to contain the whole I-Bar.	*/
/*  2)  Same table row?							*/
/*  3)  Expose the whole cell.						*/
/*									*/
/************************************************************************/

#   define	IW		5

static void tedSelectionRectangle(
				SelectionGeometry *		sg,
				const AppDrawingData *		add,
				const BufferDocument *		bd,
				const DocumentSelection *	ds )
    {
    DocumentRectangle *		drSel= &(sg->sgRectangle);
    DocumentRectangle		dr;

    ParagraphFrame		pf;
    const int			bottom= -1;
    const int			stripHigh= -1;

    BlockFrame			bf;

    drSel->drY0= PG_TOP_PIXELS( add, &(sg->sgBegin) );
    drSel->drY1= sg->sgEnd.pgY1Pixels;

    /*  1  */
    if  ( ds->dsEnd.dpBi == ds->dsBegin.dpBi )
	{
	if  ( sg->sgEnd.pgLine == sg->sgBegin.pgLine )
	    {
	    /*  1b  */
	    if  ( ds->dsEnd.dpStroff == ds->dsBegin.dpStroff )
		{
		drSel->drX0= sg->sgBegin.pgXPixels- IW;
		drSel->drX1= sg->sgEnd.pgXPixels+ IW;
		}
	    else{
		drSel->drX0= sg->sgBegin.pgXPixels;
		drSel->drX1= sg->sgEnd.pgXPixels;
		}

	    return;
	    }

	docBlockFrameTwips( &bf, ds->dsBegin.dpBi, bd,
				    ds->dsBegin.dpBi->biTopPosition.lpPage,
				    ds->dsBegin.dpBi->biTopPosition.lpColumn );

	docParagraphFrameTwips( &pf, &bf, bottom, stripHigh, ds->dsBegin.dpBi );
	tedParagraphFramePixels( &pf, add, ds->dsBegin.dpBi );

	drSel->drX0= pf.pfX0Pixels;
	drSel->drX1= pf.pfX1Pixels;

	return;
	}

    /*  2  */
    if  ( ! ds->dsBegin.dpBi->biParaInTable		||
	  ! ds->dsEnd.dpBi->biParaInTable		||
	  ! docSelectionInsideRow( ds )			)
	{
	drSel->drX0= add->addBackRect.drX0;
	drSel->drX1= add->addBackRect.drX1;
	}

    /*  3  */
    if  ( ds->dsBegin.dpBi->biParaInTable )
	{
	docBlockFrameTwips( &bf, ds->dsBegin.dpBi, bd,
				    ds->dsBegin.dpBi->biTopPosition.lpPage,
				    ds->dsBegin.dpBi->biTopPosition.lpColumn );

	docParagraphFrameTwips( &pf, &bf, bottom, stripHigh, ds->dsBegin.dpBi );
	tedParagraphFramePixels( &pf, add, ds->dsBegin.dpBi );

	dr.drX0= pf.pfX0Pixels;
	dr.drX1= pf.pfX1Pixels;

	if  ( docSelectionInsideCell( ds ) )
	    {
	    dr.drY0= BI_TOP_PIXELS( add, ds->dsBegin.dpBi );
	    dr.drY1= BI_BELOW_PIXELS( add, ds->dsBegin.dpBi );
	    }
	else{
	    BufferItem *	rowBi= ds->dsBegin.dpBi->biParent->biParent;

	    dr.drY0= BI_TOP_PIXELS( add, rowBi );
	    dr.drY1= BI_BELOW_PIXELS( add, rowBi );
	    }

	geoUnionRectangle( drSel, drSel, &dr );
	}

    /*  3  */
    if  ( ds->dsEnd.dpBi->biParaInTable )
	{
	docBlockFrameTwips( &bf, ds->dsEnd.dpBi, bd,
				    ds->dsEnd.dpBi->biTopPosition.lpPage,
				    ds->dsEnd.dpBi->biTopPosition.lpColumn );

	docParagraphFrameTwips( &pf, &bf, bottom, stripHigh, ds->dsEnd.dpBi );
	tedParagraphFramePixels( &pf, add, ds->dsEnd.dpBi );

	dr.drX0= pf.pfX0Pixels;
	dr.drX1= pf.pfX1Pixels;

	if  ( docSelectionInsideCell( ds ) )
	    {
	    dr.drY0= BI_TOP_PIXELS( add, ds->dsEnd.dpBi );
	    dr.drY1= BI_BELOW_PIXELS( add, ds->dsEnd.dpBi );
	    }
	else{
	    BufferItem *	rowBi= ds->dsEnd.dpBi->biParent->biParent;

	    dr.drY0= BI_TOP_PIXELS( add, rowBi );
	    dr.drY1= BI_BELOW_PIXELS( add, rowBi );
	    }

	geoUnionRectangle( drSel, drSel, &dr );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the poition on paper/screen of a certain position in the	*/
/*  document.								*/
/*									*/
/************************************************************************/

void tedPositionGeometry(	PositionGeometry *		pg,
				const DocumentPosition *	dp,
				int				lastOne,
				const BufferDocument *		bd,
				const AppDrawingData *		add,
				const ScreenFontList *		sfl )
    {
    int			part;
    int			line;

    TextLine *		tl;
    TextParticule *	tp;

    if  ( docFindParticuleOfPosition( &part, dp, lastOne ) )
	{ LDEB(dp->dpStroff); return;	}

    if  ( docFindLineOfPosition( &line, dp, lastOne ) )
	{ LDEB(dp->dpStroff); return;	}

    tp= dp->dpBi->biParaParticules+ part;
    tl= dp->dpBi->biParaLines+ line;

    if  ( dp->dpStroff == tl->tlStroff+ tl->tlStrlen	&&
	  part < dp->dpBi->biParaParticuleCount		&&
	  line < dp->dpBi->biParaLineCount- 1		)
	{
	if  ( tp->tpKind == DOCkindLINEBREAK	||
	      tp->tpKind == DOCkindPAGEBREAK	||
	      tp->tpKind == DOCkindCOLUMNBREAK	)
	    { line++; tl++; part++; tp++;	}
	}

    pg->pgLine= line;
    pg->pgAtLineHead= 0;
    pg->pgAtLineEnd= 0;
    if  ( dp->dpStroff == tl->tlStroff )
	{ pg->pgAtLineHead= 1;	}
    if  ( dp->dpStroff == tl->tlStroff+ tl->tlStrlen )
	{ pg->pgAtLineEnd= 1;	}

    pg->pgXPixels= tedCalculateX( bd, dp->dpBi, tp, add, sfl, dp->dpStroff );
    pg->pgTopPosition= tl->tlTopPosition;
    pg->pgY1Pixels= TL_BELOW_PIXELS( add, tl );
    pg->pgBaselinePixels= TL_BASE_PIXELS( add, tl );

#   if 0
    Gives silly high I Bar.
    if  ( sg->sgLine == bi->biParaLineCount- 1 )
	{ sg->sgY1 += bi->biParaSpaceAfterPixels; }
#   endif

    return;
    }

void tedSelectionGeometry(	SelectionGeometry *		sg,
				const DocumentSelection *	ds,
				int				lastLine,
				const BufferDocument *		bd,
				const AppDrawingData *		add,
				const ScreenFontList *		sfl )
    {
    if  ( ds->dsDirection != 0 )
	{ lastLine= 0;	}

    tedPositionGeometry( &(sg->sgBegin), &(ds->dsBegin),
					    ds->dsDirection > 0 || lastLine,
					    bd, add, sfl );
    tedPositionGeometry( &(sg->sgAnchor), &(ds->dsAnchor),
					    ds->dsDirection > 0 || lastLine,
					    bd, add, sfl );
    tedPositionGeometry( &(sg->sgEnd), &(ds->dsEnd), lastLine,
					    bd, add, sfl );

    tedSelectionRectangle( sg, add, bd, ds );

    return;
    }

/************************************************************************/
/*									*/
/*  Find the root item corresponding to a mouse click.			*/
/*									*/
/*  1)  Calculate the page number from the coordinates.			*/
/*  2)  Find the first section corresponding to this page.		*/
/*	For finding headers and footers this is sufficient. For finding	*/
/*	the actual body section, it is not: Below a more precise	*/
/*	attempt is made.						*/
/*  3)  If no section can be found, the click might have been in a note	*/
/*	protruding beyond the the last page with regular text.		*/
/*  4)  Is the click in the header corresponding to this section?	*/
/*  5)  Is the click in the footer corresponding to this section?	*/
/*  6)  Is the click in a note on this page?				*/
/*  7)  Is the click in the footnote separator on this page.		*/
/*  7)  Is the click in the endnote separator on this page.		*/
/*									*/
/*  9)  Not in an exceptional place.. assume it is in the body.		*/
/*									*/
/************************************************************************/

/*  7,8  */
static int tedFindTryNoteSeparator(	ExternalItem **		pEi,
					BufferItem **		pRootBi,
					BufferDocument *	bd,
					AppDrawingData *	add,
					int			page,
					int			docX,
					int			docY,
					int			noteItKind,
					int			sepItKind )
    {
    int			noteIndex;
    DocumentNote *	dn;

    DocumentRectangle	drExtern;

    ExternalItem *	eiFirstNote;
    ExternalItem *	eiNoteSep;
    int			y0Twips;

    int			ret;

    noteIndex= docGetFirstNoteOnPage( &dn, bd, page, noteItKind );
    if  ( noteIndex < 0 )
	{ return 1;	}

    eiFirstNote= &(dn->dnExternalItem);

    ret= docNoteSeparatorRectangle( &drExtern, &eiNoteSep, &y0Twips,
						    bd, dn, sepItKind, add );

    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}
    if  ( ret > 0 )
	{ return 1;	}

    if  ( docY < drExtern.drY0 || docY > drExtern.drY1 )
	{ return 1;	}

    *pEi= eiNoteSep;
    *pRootBi= eiNoteSep->eiItem;

    return 0;
    }

int tedFindRootForPosition(		ExternalItem **		pEi,
					BufferItem **		pRootBi,
					BufferItem **		pBodySectBi,
					int *			pSectNr,
					int *			pPage,
					EditDocument *		ed,
					int			docX,
					int			docY )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    const DocumentProperties *	dp= &(bd->bdProperties);

    AppDrawingData *		add= &(ed->edDrawingData);

    int				page;
    int				sectNr;

    int				pageY;

    BufferItem *		bodySectBi= (BufferItem *)0;

    int				inHeaderFooter;
    ExternalItem *		ei;
    DocumentNote *		dn;
    int				ret;

    int				i;

    const int			justUsed= 0;
    DocumentRectangle		drExtern;

    /*  1  */
    page= docY/ add->addPageStepPixels;
    pageY= docY- ( page* add->addPageStepPixels );

    /*  2  */
    for ( sectNr= 0; sectNr < bd->bdItem.biChildCount; sectNr++ )
	{
	bodySectBi= bd->bdItem.biChildren[sectNr];

	if  ( bodySectBi->biTopPosition.lpPage <= page		&&
	      bodySectBi->biBelowPosition.lpPage >= page	)
	    { break;	}
	}

    /*  3
    if  ( sectNr >= bd->bdItem.biChildCount )
	{ LLDEB(page,sectNr); return -1;	}
    */

    if  ( sectNr < bd->bdItem.biChildCount )
	{
	const DocumentGeometry *	dgSect;
	int				isEmpty;

	dgSect= &(bodySectBi->biSectDocumentGeometry);

	/*  4  */
	ei= (ExternalItem *)0;
	inHeaderFooter= docWhatPageHeader( &ei, &isEmpty,
						    bodySectBi, page, dp );
	if  ( ei && ei->eiItem )
	    {
	    if  ( docGetExternalItemBox( &drExtern, bodySectBi, ei, justUsed,
							    page, bd, add ) )
		{ LDEB(1);	}
	    else{
		if  ( docY >= drExtern.drY0 && docY <= drExtern.drY1 )
		    {
		    *pEi= ei;
		    *pRootBi= ei->eiItem;
		    *pBodySectBi= bodySectBi;
		    *pSectNr= sectNr;
		    *pPage= page;

		    return 0;
		    }
		}
	    }

	/*  5  */
	ei= (ExternalItem *)0;
	inHeaderFooter= docWhatPageFooter( &ei, &isEmpty,
						    bodySectBi, page, dp );
	if  ( ei && ei->eiItem )
	    {
	    if  ( docGetExternalItemBox( &drExtern, bodySectBi, ei, justUsed,
							    page, bd, add ) )
		{ LDEB(1);	}
	    else{
		if  ( docY >= drExtern.drY0 && docY <= drExtern.drY1 )
		    {
		    *pEi= ei;
		    *pRootBi= ei->eiItem;
		    *pBodySectBi= bodySectBi;
		    *pSectNr= sectNr;
		    *pPage= page;

		    return 0;
		    }
		}
	    }
	}

    /*  6  */
    dn= bd->bdNotes;
    for ( i= 0; i < bd->bdNoteCount; dn++, i++ )
	{
	BufferItem *		noteBodySectBi;

	ei= &(dn->dnExternalItem);

	if  ( dn->dnParaNr < 0 )
	    { continue;	}
	if  ( ! ei->eiItem )
	    { XDEB(ei->eiItem); continue;	}

	noteBodySectBi= bd->bdItem.biChildren[dn->dnSectNr];

	if  ( ei->eiItem->biBelowPosition.lpPage < page )
	    { continue;	}
	if  ( ei->eiItem->biTopPosition.lpPage > page )
	    { continue;	}

	if  ( docGetExternalItemBox( &drExtern, noteBodySectBi, ei, justUsed,
							page, bd, add ) )
	    { LDEB(1);	}
	else{
	    if  ( docY >= drExtern.drY0 && docY <= drExtern.drY1 )
		{
		*pEi= ei;
		*pRootBi= ei->eiItem;
		*pBodySectBi= noteBodySectBi;
		*pSectNr= dn->dnSectNr;
		*pPage= page;

		return 0;
		}
	    }
	}

    /*  3  */
    if  ( sectNr >= bd->bdItem.biChildCount )
	{ LLDEB(page,sectNr); return -1;	}

    /*  7  */
    ret= tedFindTryNoteSeparator( pEi, pRootBi, bd, add,
				page, docX, docY, DOCinFOOTNOTE, DOCinFTNSEP );
    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}
    if  ( ret == 0 )
	{
	*pBodySectBi= bodySectBi;
	*pSectNr= sectNr;
	*pPage= page;

	return 0;
	}

    /*  8  */
    ret= tedFindTryNoteSeparator( pEi, pRootBi, bd, add,
				page, docX, docY, DOCinENDNOTE, DOCinAFTNSEP );
    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}
    if  ( ret == 0 )
	{
	*pBodySectBi= bodySectBi;
	*pSectNr= sectNr;
	*pPage= page;

	return 0;
	}

    /*  9  */
    bodySectBi= tedFindGroupChild( &(bd->bdItem), add, docX, docY, page );

    *pEi= (ExternalItem *)0;
    *pRootBi= bodySectBi;
    *pBodySectBi= bodySectBi;
    *pSectNr= sectNr;
    *pPage= page;

    return 0;
    }
