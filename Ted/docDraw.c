#   include	"tedConfig.h"

#   include	<stdlib.h>

#   include	"docDraw.h"
#   include	"docLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  For dividing table rows over pages.					*/
/*									*/
/************************************************************************/

typedef struct CellDrawingProgress
    {
    int			cdpColumnAdvanced;
    int			cdpAdvanceColumnThisPage;
    int			cdpPara;
    int			cdpLinesDone;

    int			cdpX0Twips;
    int			cdpX1Twips;

    ParagraphFrame	cdpParagraphFrame;

    int			cdpIsMergedWithBelow;
    LayoutPosition	cdpBelowCellPosition;
    } CellDrawingProgress;

static void docInitCellDrawingProgress(	CellDrawingProgress *	cdp )
    {
    cdp->cdpColumnAdvanced= 0;
    cdp->cdpAdvanceColumnThisPage= 0;

    cdp->cdpPara= 0;
    cdp->cdpLinesDone= 0;

    cdp->cdpX0Twips= 0;
    cdp->cdpX1Twips= 0;

    cdp->cdpIsMergedWithBelow= 0;

    docInitLayoutPosition( &(cdp->cdpBelowCellPosition) );

    return;
    }

/************************************************************************/
/*									*/
/*  Generic Document drawing machinery.					*/
/*									*/
/************************************************************************/

static int docDrawOneTextLine(	void *				through,
				const BufferItem *		paraBi,
				int				part,
				const LayoutPosition *		lpShadeTop,
				const LayoutPosition *		lpShadeBelow,
				const ParagraphFrame *		pf,
				int				line,
				DrawingContext *		dc,
				int				pShift,
				int				xShift,
				int				yShift )
    {
    const TextLine *		tl= paraBi->biParaLines+ line;

    int				accepted;

    const BorderProperties * const	bpLeft= &(paraBi->biParaLeftBorder);
    const BorderProperties * const	bpRight= &(paraBi->biParaRightBorder);

    if  ( part == 0				&&
	  paraBi->biParaBorderAboveParagraph	&&
	  dc->dcDrawParaTop			)
	{
	LayoutPosition		lpTop;

	lpTop= *lpShadeTop;

	lpTop.lpPage += pShift;
	lpTop.lpPageYTwips += yShift;

	if  ( (*dc->dcDrawParaTop)( paraBi->biParaBorderAboveParagraph,
				bpLeft, bpRight, pf, &lpTop, through, dc ) )
	    { LDEB(1); return -1;	}
	}

    if  ( dc->dcDrawTextLine )
	{
	accepted= (*dc->dcDrawTextLine)( paraBi, line, pf, through,
						dc, pShift, xShift, yShift );
	}
    else{ accepted= tl->tlParticuleCount;	}

    if  ( accepted < 1 )
	{ LDEB(accepted); return -1;	}

    if  ( part+ accepted == paraBi->biParaParticuleCount )
	{
	if  ( paraBi->biParaBorderBelowParagraph	&&
	      dc->dcDrawParaBottom		)
	    {
	    LayoutPosition	lpBelow;

	    lpBelow= *lpShadeBelow;

	    lpBelow.lpPage += pShift;
	    lpBelow.lpPageYTwips += yShift;

	    if  ( (*dc->dcDrawParaBottom)( paraBi->biParaBorderBelowParagraph,
						bpLeft, bpRight,
						pf, &lpBelow, through, dc ) )
		{ LDEB(1); return -1;	}
	    }
	}

    return accepted;
    }

/************************************************************************/
/*									*/
/*  Draw a series of text lines in a paragraph.				*/
/*									*/
/*  1)  For all lines that apply...					*/
/*  2)  If the line is on a subsequent page.. stop.			*/
/*  3)  Is line is on a page before the pages to draw?			*/
/*  4)  Is the line in the rectangle to redraw?				*/
/*  5)  Printing a page range.. and not in range?			*/
/*  6)  Drawing a selection? E.G. To highlight it?			*/
/*									*/
/************************************************************************/

static int docDrawTextLines(	void *				through,
				const BufferItem *		paraBi,
				const LayoutPosition *		lpShadeTop,
				const LayoutPosition *		lpShadeBelow,
				const ParagraphFrame *		pf,
				int *				pLinesDone,
				DrawingContext *		dc,
				int				pShift,
				int				xShift,
				int				yShift )
    {
    int				done= 0;
    int				pastSelectionEnd= 0;

    int				line= *pLinesDone;
    const TextLine *		tl= paraBi->biParaLines+ line;

    if  ( lpShadeTop->lpPage != lpShadeBelow->lpPage )
	{ LLDEB(lpShadeTop->lpPage,lpShadeBelow->lpPage);	}

    /*  1  */
    while( line < paraBi->biParaLineCount )
	{
	int	accepted;
	int	beforeSelectionBegin= 0;

	/*  2  */
	if  ( tl->tlTopPosition.lpPage+ pShift > lpShadeTop->lpPage )
	    { break;	}

	/*  3  */
	if  ( tl->tlTopPosition.lpPage+ pShift < lpShadeTop->lpPage )
	    { beforeSelectionBegin= 1;	}

	/*  4  */
	if  ( dc->dcClipRect )
	    {
	    AppDrawingData *		add= dc->dcDrawingData;

	    int y0= TL_TOP_PIXELS_SH( add, tl, pShift, yShift );
	    int	y1= TL_BELOW_PIXELS_SH( add, tl, pShift, yShift );

	    if  ( y0 > dc->dcClipRect->drY1 )
		{ pastSelectionEnd= 1;	}
	    if  ( y1 < dc->dcClipRect->drY0 )
		{ beforeSelectionBegin= 1;	}
	    }

	/*  5  */
	if  ( dc->dcFirstPage >= 0			&&
	      lpShadeTop->lpPage < dc->dcFirstPage	)
	    { beforeSelectionBegin= 1;	}

	/*  6  */
	if  ( dc->dcDocumentSelection					&&
	      ! beforeSelectionBegin					&&
	      ! pastSelectionEnd					&&
	      ( paraBi == dc->dcDocumentSelection->dsBegin.dpBi	||
	        paraBi == dc->dcDocumentSelection->dsEnd.dpBi	)	)
	    {
	    const DocumentSelection *	ds= dc->dcDocumentSelection;

	    DocumentSelection		dsLine;

	    int				partLineBegin;
	    int				partLineEnd;

	    docLineSelection( &dsLine, &partLineBegin, &partLineEnd,
								paraBi, line );

	    if  ( paraBi == ds->dsEnd.dpBi				&&
		  docComparePositions( &(dsLine.dsBegin),
						&(ds->dsEnd) ) > 0	)
		{ pastSelectionEnd= 1;	}

	    if  ( paraBi == ds->dsBegin.dpBi				&&
		  docComparePositions( &(dsLine.dsEnd),
					    &(ds->dsBegin) ) < 0	)
		{ beforeSelectionBegin= 1;	}
	    }

	if  ( beforeSelectionBegin || pastSelectionEnd )
	    { accepted= tl->tlParticuleCount;		}
	else{
	    accepted= docDrawOneTextLine( through, paraBi, tl->tlFirstParticule,
				lpShadeTop, lpShadeBelow, pf,
				line, dc, pShift, xShift, yShift );
	    }

	if  ( accepted < 1 )
	    { LDEB(accepted); return -1; }

	done += accepted; line++; tl++;
	}

    *pLinesDone= line;

    return 0;
    }

static int docDrawParaOrnaments( void *				through,
				const BufferItem *		paraBi,
				const LayoutPosition *		lpShadeTop,
				const LayoutPosition *		lpShadeBelow,
				const ParagraphFrame *		pf,
				DrawingContext *		dc )
    {
    if  ( dc->dcDrawParaShade					&&
	  (*dc->dcDrawParaShade)( &(paraBi->biParaProperties),
				    paraBi->biParaBorderAboveParagraph,
				    through, dc,
				    pf, lpShadeTop, lpShadeBelow ) )
	{ LDEB(1); return -1;	}

    if  ( dc->dcDrawParaLeft					&&
	  (*dc->dcDrawParaLeft)( &(paraBi->biParaLeftBorder),
				    paraBi->biParaBorderAboveParagraph,
				    paraBi->biParaBorderBelowParagraph,
				    pf, lpShadeTop, lpShadeBelow,
				    through, dc ) )
	{ LDEB(1); return -1;	}

    if  ( dc->dcDrawParaRight					&&
	  (*dc->dcDrawParaRight)( &(paraBi->biParaRightBorder),
				    paraBi->biParaBorderAboveParagraph,
				    paraBi->biParaBorderBelowParagraph,
				    pf, lpShadeTop, lpShadeBelow,
				    through, dc ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw the shapes for this page					*/
/*									*/
/************************************************************************/

int docDrawShapesForPage(		BufferItem *		shapeRootBi,
					void *			through,
					DrawingContext *	dc,
					int			page )
    {
    const int			column= 0;

    DocumentPosition		dpPageTop;
    int				partPageTop;
    int				linePageTop;

    BufferItem *		paraBi;
    const TextLine *		tl;
    const TextParticule *	tp;
    int				part;
    int				line;

    if  ( ! dc->dcDrawShape )
	{ XDEB(dc->dcDrawShape); return 0; }

    if  ( docGetFirstInColumnForItem( &dpPageTop, &linePageTop, &partPageTop,
						shapeRootBi, page, column ) )
	{ return 0; }

    paraBi= dpPageTop.dpBi;
    line= linePageTop;
    part= partPageTop;
    tl= paraBi->biParaLines+ line;
    tp= paraBi->biParaParticules+ part;

    for(;;)
	{
	if  ( paraBi->biTopPosition.lpPage > page )
	    { break;	}

	while( line < paraBi->biParaLineCount )
	    {
	    if  ( tl->tlTopPosition.lpPage > page )
		{ break; }

	    while( part < tl->tlFirstParticule+ tl->tlParticuleCount )
		{
		if  ( tp->tpKind == DOCkindOBJECT )
		    {
		    const InsertedObject *	io;

		    io= paraBi->biParaObjects+ tp->tpObjectNumber;

		    if  ( io->ioKind == DOCokDRAWING_SHAPE )
			{
			if  ( docDrawShape( dc, through, io ) )
			    { LDEB(1);	}
			}
		    }

		part++; tp++;
		}

	    line++; tl++;
	    }

	if  ( line < paraBi->biParaLineCount )
	    { break;	}

	paraBi= docNextParagraph( paraBi );
	if  ( ! paraBi )
	    { break;	}

	line= 0; tl= paraBi->biParaLines;
	part= 0; tp= paraBi->biParaParticules;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Skip to the next page.						*/
/*									*/
/*  A)  Draw the shapes for this page.					*/
/*  1)  Just to make the compiler happy, if nothing is to be printed	*/
/*	anymore, the current position is not going to be used.		*/
/*									*/
/************************************************************************/

static int docDrawToNextColumn(		BufferItem *		thisBodyBi,
					BufferItem *		nextBodyBi,
					void *			through,
					LayoutPosition *	lpHere,
					BlockFrame *		bf,
					DrawingContext *	dc )
    {
    BufferDocument *		bd= dc->dcDocument;

    const int			asLast= 0;

    int				startNewPage= 0;

    while( thisBodyBi && thisBodyBi->biLevel != DOClevSECT )
	{ thisBodyBi= thisBodyBi->biParent; }
    while( nextBodyBi && nextBodyBi->biLevel != DOClevSECT )
	{ nextBodyBi= nextBodyBi->biParent; }

    if  ( dc->dcFirstPage < 0				||
	  lpHere->lpPage >= dc->dcFirstPage		)
	{
	startNewPage= 1;

	if  ( thisBodyBi						&&
	      dc->dcDrawExternalItems					)
	    {
	    if  ( docDrawFootnotesForColumn( lpHere->lpPage, through, dc ) )
		{ LDEB(lpHere->lpPage); return -1;	}

	    if  ( ! dc->dcPostponeHeadersFooters			&&
		  docDrawPageFooter( thisBodyBi, through, dc,
						    lpHere->lpPage )	)
		{ LDEB(lpHere->lpPage); return -1;	}
	    }

	/*  A  */
	if  ( docDrawShapesForPage( &(bd->bdItem),
					    through, dc, lpHere->lpPage ) )
	    { LDEB(1);	}

	if  ( dc->dcFinishPage						&&
	      (*dc->dcFinishPage)( through, dc, thisBodyBi,
					    lpHere->lpPage, asLast )	)
	    { LDEB(1); return -1;	}
	}

    if  ( dc->dcLastPage < 0				||
	  lpHere->lpPage+ 1 <= dc->dcLastPage		)
	{
	lpHere->lpPage++;

	if  ( nextBodyBi )
	    {
	    const SectionProperties *	sp= &(nextBodyBi->biSectProperties);
	    const DocumentGeometry *	dg= &(sp->spDocumentGeometry);

	    docLayoutSectColumnTop( nextBodyBi, bd, lpHere, bf );

	    if  ( startNewPage )
		{
		if  ( dc->dcStartPage					&&
		      (*dc->dcStartPage)( through, dg, dc,
						    lpHere->lpPage )	)
		    { LDEB(1); return -1;	}

		if  ( dc->dcDrawExternalItems				&&
		      ! dc->dcPostponeHeadersFooters			&&
		      docDrawPageHeader( nextBodyBi, through, dc,
						    lpHere->lpPage )	)
		    { LDEB(lpHere->lpPage); return -1;	}
		}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Skip to the page where a BufferItem begins.				*/
/*									*/
/************************************************************************/

int docDrawToPageOfItem(		BufferItem *		prevBodyBi,
					BufferItem *		thisBodyBi,
					BufferItem *		thisBi,
					void *			through,
					LayoutPosition *	lpHere,
					DrawingContext *	dc,
					int			pShift )
    {
    BlockFrame	bf;

    while( thisBi->biTopPosition.lpPage+ pShift > lpHere->lpPage	&&
	   thisBi->biTopPosition.lpPage+ pShift > dc->dcFirstPage	)
	{
	if  ( docDrawToNextColumn( prevBodyBi, thisBodyBi, through,
						    lpHere, &bf, dc )	)
	    { SDEB(docLevelStr(thisBi->biLevel)); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Count the number of lines that fit on the current page.		*/
/*									*/
/************************************************************************/

static int docDrawBelowPara(	int *				pLinesDone,
				const BufferItem *		paraBi,
				int				countAfter,
				int				linesDone,
				const LayoutPosition *		lpHere,
				LayoutPosition *		lpBelow,
				int				pShift,
				int				xShift,
				int				yShift )
    {
    const TextLine *	tl;
    LayoutPosition	lp;


    tl= paraBi->biParaLines+ linesDone;
    while( linesDone < paraBi->biParaLineCount		&&
	   tl->tlTopPosition.lpPage <= lpHere->lpPage	)
	{
	if  ( tl->tlTopPosition.lpPage < lpHere->lpPage )
	    { linesDone++; tl++; continue;	}

	lp.lpPage= tl->tlTopPosition.lpPage;
	lp.lpPageYTwips= tl->tlTopPosition.lpPageYTwips+
					    tl->tlLineSpacingTwips;
	lp.lpAtTopOfColumn= 0;

	docLayoutPushBottomDownShifted( lpBelow, &lp, pShift, yShift );

	linesDone++; tl++;
	}

    if  ( linesDone == paraBi->biParaLineCount )
	{
	lp= paraBi->biBelowPosition;

	if  ( ! countAfter )
	    { lp.lpPageYTwips -= paraBi->biParaSpaceAfterTwips;	}

	docLayoutPushBottomDownShifted( lpBelow, &lp, pShift, yShift );
	}

    *pLinesDone= linesDone;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Print a node in the BufferItem hierarchy.				*/
/*									*/
/*  1)  The last page is finished by the caller.			*/
/*									*/
/************************************************************************/

static int docDrawParaItem(	BufferItem *			paraBi,
				void *				through,
				DrawingContext *		dc,
				int				pShift,
				int				xShift,
				int				yShift )
    {
    ParagraphFrame		pf;
    int				line= 0;

    int				bottomTwips= -1;
    int				stripHigh= -1;

    BufferDocument *		bd= dc->dcDocument;

    BlockFrame			bf;
    LayoutPosition		lpTop;
    LayoutPosition		lpShadeTop;

    docBlockFrameTwips( &bf, paraBi, bd, paraBi->biTopPosition.lpPage,
					    paraBi->biTopPosition.lpColumn );

    docParagraphFrameTwips( &pf, &bf, bottomTwips, stripHigh, paraBi );

    if  ( dc->dcParaFramePixels )
	{ (*dc->dcParaFramePixels) ( &pf, dc->dcDrawingData, paraBi ); }

    lpTop= paraBi->biTopPosition;

    lpTop.lpPage += pShift;
    lpTop.lpPageYTwips += yShift;

    lpShadeTop= lpTop;

    if  ( ! lpShadeTop.lpAtTopOfColumn )
	{ lpShadeTop.lpPageYTwips += paraBi->biParaSpaceBeforeTwips; }

    while( line < paraBi->biParaLineCount )
	{
	int			paraLinesDone= line;
	LayoutPosition		lpShadeBelow;
	const int		countAfter= 0;

	lpShadeBelow= lpShadeTop;

	if  ( docDrawBelowPara( &paraLinesDone, paraBi, countAfter, line,
			&lpTop, &lpShadeBelow, pShift, xShift, yShift ) )
	    { LDEB(1); return -1;	}

	if  ( paraLinesDone > line )
	    {
	    if  ( docDrawParaOrnaments( through, paraBi,
				    &lpShadeTop, &lpShadeBelow, &pf, dc ) )
		{ LDEB(1); return -1;	}
	    }

	if  ( docDrawTextLines( through, paraBi,
					&lpShadeTop, &lpShadeBelow, &pf, &line,
					dc, pShift, xShift, yShift ) )
	    { LDEB(lpTop.lpPage); return -1;	}

	/*  1  */
	if  ( dc->dcLastPage >= 0		&&
	      lpTop.lpPage >= dc->dcLastPage	)
	    { break;	}

	if  ( line < paraBi->biParaLineCount )
	    {
	    if  ( docDrawToNextColumn( paraBi, paraBi,
					    through, &lpTop, &bf, dc )	)
		{ SDEB(docLevelStr(paraBi->biLevel));return -1;	}

	    lpShadeTop= lpTop;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw part of the grid originating from a table row.			*/
/*									*/
/*  2)  Get the table row below the current row.			*/
/*  3)  Draw the cell shades (If any)					*/
/*  4)  Draw the cells.							*/
/*  5)  Skip cells that are included in the colspan or rowspan of	*/
/*	another cell. The left/top cell of a span has to draw the	*/
/*	borders and the background.					*/
/*  6)  If a cell has a right boder, and the next one a left border,	*/
/*	MS-Word only draws the right border of the leftmost cell.	*/
/*	( The same applies in the vertical direction. )			*/
/*  7)  Base the decisions on whether to draw a table grid on the same	*/
/*	criteria.							*/
/*  8)  Draw cell top border.						*/
/*  9)  Draw cell left border.						*/
/* 10)  Draw cell right border.						*/
/* 11)  Draw cell bottom border.					*/
/*									*/
/************************************************************************/

static int docDrawCellOrnaments(
			const BufferItem *			rowBi,
			const CellDrawingProgress * const	cdpRow,
			int					atTopOfPage,
			int					atBottomOfPage,
			void *					through,
			DrawingContext *			dc,
			int					xShift,
			int					yShift,
			const LayoutPosition *			lpTop,
			const LayoutPosition *			lpBelow )
    {
    const CellDrawingProgress *	cdp;
    const CellProperties *	cp;

    int				col;

    /*  4  */
    cdp= cdpRow;
    cp= rowBi->biRowCells;
    for ( col= 0; col < rowBi->biChildCount; cdp++, cp++, col++ )
	{
	int				topAsGrid= 0;
	int				leftAsGrid= 0;
	int				rightAsGrid= 0;
	int				bottomAsGrid= 0;

	int				drawCellLeft= 1;
	int				drawCellRight= 1;

	const BorderProperties *	bpLeft;
	const BorderProperties *	bpRight;
	const BorderProperties *	bpTop;
	const BorderProperties *	bpBottom;

	int				useAbove= 0;
	int				useBelow= 0;
	int				leftHasBorder= 0;
	int				rightHasBorder= 0;

	LayoutPosition			lpBelowCell;

	docGetCellTopBorder( &bpTop, &useAbove,
					    rowBi, col, atTopOfPage );
	docGetCellBottomBorder( &bpBottom, &useBelow,
					    rowBi, col, atBottomOfPage );

	/*  5  */
	if  ( cp->cpMergedWithLeft || cp->cpMergedWithAbove )
	    { continue;	}

	lpBelowCell= *lpBelow;

	if  ( cp->cpTopInMergedColumn		&&
	      ! atBottomOfPage			)
	    { lpBelowCell= cdp->cdpBelowCellPosition;	}

	if  ( col < rowBi->biChildCount- 1	&&
	      cp[1].cpMergedWithLeft		)
	    { drawCellRight= 0;	}

	/*  6  */
	if  ( col > 0 && DOCisBORDER( &(cp[-1].cpRightBorder) ) )
	    { leftHasBorder= 1;	}
	if  ( col < rowBi->biChildCount- 1		&&
	      DOCisBORDER( &(cp[1].cpLeftBorder) )	)
	    { rightHasBorder= 1;	}

	bpLeft= &(cp->cpLeftBorder);
	bpRight= &(cp->cpRightBorder);

	if  ( leftHasBorder )
	    { bpLeft= &(cp[-1].cpRightBorder);		}
	if  ( ! DOCisBORDER( &(cp->cpRightBorder) ) && rightHasBorder )
	    { bpRight= &(cp[1].cpLeftBorder);	}

	/*  7  */
	if  ( dc->dcDrawTableGrid )
	    {
	    if  ( ! DOCisBORDER( &(cp->cpLeftBorder) )	&&
		  ! leftHasBorder			)
		{ leftAsGrid= 1;	}

	    if  ( ! DOCisBORDER( &(cp->cpRightBorder) )	&&
		  ! rightHasBorder			)
		{ rightAsGrid= 1;	}

	    if  ( ! DOCisBORDER( bpTop )		&&
		  ! useAbove				)
		{ topAsGrid= 1;	}

	    if  ( ! DOCisBORDER( bpBottom )		&&
		  ! useBelow				)
		{ bottomAsGrid= 1;	}
	    }

	/*  3  */
	if  ( dc->dcDrawCellShade )
	    {
	    if  ( (*dc->dcDrawCellShade)( cp, bpTop, bpLeft, bpRight,
					through, dc,
					cdp->cdpX0Twips, cdp->cdpX1Twips,
					lpTop, &lpBelowCell ) )
		{ LDEB(1); return -1;	}
	    }

	/*  8  */
	if  ( dc->dcDrawCellTop				&&
	      ! useAbove				&&
	      ( DOCisBORDER( bpTop ) || topAsGrid )	)
	    {
	    if  ( (*dc->dcDrawCellTop)(	bpTop, bpLeft, bpRight,
					topAsGrid,
					cdp->cdpX0Twips,
					cdp->cdpX1Twips,
					through, dc, lpTop ) )
		{ LDEB(1); return -1;	}
	    }

	/*  9  */
	if  ( drawCellLeft						&&
	      dc->dcDrawCellLeft					&&
	      ! leftHasBorder						&&
	      ( DOCisBORDER( &(cp->cpLeftBorder) ) || leftAsGrid )	)
	    {
	    if  ( (*dc->dcDrawCellLeft)( &(cp->cpLeftBorder),
						bpTop, bpBottom,
						leftAsGrid, through, dc,
						cdp->cdpX0Twips,
						lpTop, &lpBelowCell ) )
		{ LDEB(1); return -1;	}
	    }

	/*  10  */
	if  ( drawCellRight						&&
	      dc->dcDrawCellRight					&&
	      ( DOCisBORDER( &(cp->cpRightBorder) ) || rightAsGrid )	)
	    {
	    if  ( (*dc->dcDrawCellRight)( &(cp->cpRightBorder),
						bpTop, bpBottom,
						rightAsGrid, through, dc,
						cdp->cdpX1Twips,
						lpTop, &lpBelowCell ) )
		{ LDEB(1); return -1;	}
	    }

	/*  11  */
	if  ( dc->dcDrawCellBottom					&&
	      ( DOCisBORDER( &(cp->cpBottomBorder) ) || bottomAsGrid )	)
	    {
	    if  ( (*dc->dcDrawCellBottom)( bpBottom, bpLeft, bpRight,
				bottomAsGrid,
				cdp->cdpX0Twips,
				cdp->cdpX1Twips,
				through, dc, &lpBelowCell ) )
		{ LDEB(1); return -1;	}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find the position upto where this cell has advanced on the page.	*/
/*									*/
/*  1)  For all paragraphs in the cell..				*/
/*  2)  Find the lowest position on the current page.			*/
/*  3)  Was anything added to this page?				*/
/*  4)  If this paragraph does not fit as a whole. Subsequent		*/
/*	paragraphs will not fit either.					*/
/*  5)  Not all paragraphs completely placed on this page.. Some lines	*/
/*	will be printed on the next one.				*/
/*									*/
/************************************************************************/

static int docDrawBelowCell(
			int *					pAdvanced,
			int *					pColToNextPage,
			const BufferItem *			cellBi,
			const LayoutPosition *			lpHere,
			const CellDrawingProgress * const	cdp,
			LayoutPosition *			lpBelow,
			int					pShift,
			int					xShift,
			int					yShift )
    {
    int		para= cdp->cdpPara;
    int		linesDone= cdp->cdpLinesDone;

    int		advanced= 0;

    /*  1  */
    while( para < cellBi->biChildCount )
	{
	BufferItem *		paraBi= cellBi->biChildren[para];
	int			paraLinesDone= linesDone;
	const int		countAfter= 1;

	/*  2  */
	if  ( docDrawBelowPara( &paraLinesDone, paraBi, countAfter, linesDone,
				    lpHere, lpBelow, pShift, xShift, yShift ) )
	    { LDEB(1); return -1;	}

	/*  3  */
	if  ( paraLinesDone != linesDone )
	    { advanced= 1; linesDone= paraLinesDone;	}

	/*  4  */
	if  ( linesDone < paraBi->biParaLineCount )
	    { break;	}

	linesDone= 0; para++;
	}

    /*  5  */
    if  ( para < cellBi->biChildCount )
	{ *pColToNextPage= 1;	}
    if  ( advanced )
	{ *pAdvanced= 1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Print as much of a table-cell/column in a row as fits on the	*/
/*  current page.							*/
/*									*/
/************************************************************************/

static int docDrawAdvanceCell(	void *				through,
				const BufferItem *		cellBi,
				const LayoutPosition *		lpHere,
				CellDrawingProgress * const	cdp,
				BlockFrame *			bf,
				DrawingContext *		dc,
				int				pShift,
				int				xShift,
				int				yShift )
				
    {
    int				bottomTwips= -1;
    int				stripHigh= -1;

    LayoutPosition		lpTop;
    LayoutPosition		lpShadeTop;

    lpTop= *lpHere;
    lpShadeTop= lpTop;

    while( cdp->cdpPara < cellBi->biChildCount )
	{
	BufferItem *		paraBi= cellBi->biChildren[cdp->cdpPara];
	int			linesDone= cdp->cdpLinesDone;

	int			paraLinesDone= cdp->cdpLinesDone;
	LayoutPosition		lpShadeBelow;
	const int		countAfter= 0;

	if  ( paraBi->biTopPosition.lpPage > lpHere->lpPage )
	    { break;	}

	if  ( cdp->cdpLinesDone == 0 )
	    {
	    docParagraphFrameTwips( &(cdp->cdpParagraphFrame), bf,
					    bottomTwips, stripHigh, paraBi );

	    if  ( dc->dcParaFramePixels )
		{
		(*dc->dcParaFramePixels) ( &(cdp->cdpParagraphFrame),
						dc->dcDrawingData, paraBi );
		}

	    lpTop= paraBi->biTopPosition;

	    lpTop.lpPage += pShift;
	    lpTop.lpPageYTwips += yShift;

	    lpShadeTop= lpTop;
	    lpShadeTop.lpPageYTwips += paraBi->biParaSpaceBeforeTwips;
	    }

	/**/
	paraLinesDone= cdp->cdpLinesDone;

	lpShadeBelow= lpShadeTop;

	if  ( docDrawBelowPara( &paraLinesDone, paraBi, countAfter,
				cdp->cdpLinesDone, &lpTop, &lpShadeBelow,
				pShift, xShift, yShift ) )
	    { LDEB(1); return -1;	}

	if  ( paraLinesDone > cdp->cdpLinesDone )
	    {
	    if  ( docDrawParaOrnaments( through, paraBi,
				    &lpShadeTop, &lpShadeBelow,
				    &(cdp->cdpParagraphFrame), dc ) )
		{ LDEB(1); return -1;	}
	    }
	/**/

	if  ( docDrawTextLines( through, paraBi, &lpShadeTop, &lpShadeBelow,
				    &(cdp->cdpParagraphFrame), &linesDone,
				    dc, pShift, xShift, yShift ) )
	    { LDEB(lpHere->lpPage); return -1;	}

	cdp->cdpLinesDone= linesDone;

	if  ( cdp->cdpLinesDone < paraBi->biParaLineCount )
	    { break;	}

	cdp->cdpLinesDone= 0;
	cdp->cdpPara++;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Print as much of a table row as fits on the current page.		*/
/*									*/
/************************************************************************/

static int docDrawRowPageStrip(	BufferItem *			rowBi,
				const LayoutPosition *		lpHere,
				int *				pToNextPage,
				CellDrawingProgress * const	cdpRow,
				void *				through,
				DrawingContext *		dc,
				int				pShift,
				int				xShift,
				int				yShift,
				const LayoutPosition *		lpTop )
    {
    BufferDocument *		bd= dc->dcDocument;
    int				col;

    int				toNextPage= 0;

    int				rowAdvanced;
    int				atRowTop= 1;

    LayoutPosition		lpBottom;

    CellDrawingProgress *	cdp;
    const CellProperties *	cp;

    BlockFrame			bf;

    docBlockFrameTwips( &bf, rowBi, bd, lpHere->lpPage, lpHere->lpColumn );

    lpBottom= *lpTop;

    rowAdvanced= 0;
    cdp= cdpRow;
    cp= rowBi->biRowCells;
    for ( col= 0; col < rowBi->biChildCount; cp++, cdp++, col++ )
	{
	BufferItem *	cellBi= rowBi->biChildren[col];
	int		colToNextPage= 0;

	LayoutPosition	lpBelowCell;

	cdp->cdpAdvanceColumnThisPage= 0;

	if  ( cdp->cdpColumnAdvanced )
	    { atRowTop= 0;	}

	lpBelowCell= lpBottom;

	if  ( docDrawBelowCell( &(cdp->cdpAdvanceColumnThisPage),
						&colToNextPage, cellBi,
						lpHere, cdp, &lpBelowCell,
						pShift, xShift, yShift ) )
	    { LDEB(1); return -1;	}

	if  ( ! cp->cpTopInMergedColumn )
	    { docLayoutPushBottomDown( &lpBottom, &lpBelowCell );	}

	if  ( cdp->cdpAdvanceColumnThisPage )
	    { rowAdvanced= 1; cdp->cdpColumnAdvanced= 1;	}

	if  ( colToNextPage )
	    { toNextPage= 1;	}
	}

    if  ( ! toNextPage )
	{
	docLayoutPushBottomDownShifted( &lpBottom,
				&(rowBi->biBelowPosition), pShift, yShift );
	}

    if  ( rowAdvanced )
	{
	const int	atTopOfColumn= lpTop->lpAtTopOfColumn;
	const int	atBottomOfPage= toNextPage;

	if  ( docDrawCellOrnaments( rowBi, cdpRow,
			    atTopOfColumn, atBottomOfPage,
			    through, dc, xShift, yShift, lpTop, &lpBottom ) )
	    { LDEB(1); return -1;	}
	}

    rowAdvanced= 0;
    cdp= cdpRow;
    cp= rowBi->biRowCells;
    for ( col= 0; col < rowBi->biChildCount; cp++, cdp++, col++ )
	{
	BufferItem *	cellBi= rowBi->biChildren[col];
	int		cellAdvanced= 0;
	int		colToNextPage= 0;

	LayoutPosition	lpBelowCell;

	if  ( dc->dcDocumentSelection )
	    {
	    const DocumentSelection *	ds= dc->dcDocumentSelection;

	    if  ( docCompareItemPositions( cellBi, ds->dsBegin.dpBi ) < 0 )
		{ continue;	}

	    if  ( docCompareItemPositions( cellBi, ds->dsEnd.dpBi ) > 0	)
		{ continue;	}

	    if  ( ( ds->dsCol0 >= 0 && col < ds->dsCol0	)	||
	          ( ds->dsCol1 >= 0 && col > ds->dsCol1	)	)
		{ continue;	}
	    }

	if  ( cdp->cdpColumnAdvanced )
	    { atRowTop= 0;	}

	lpBelowCell= lpBottom;

	if  ( docDrawBelowCell( &cellAdvanced, &colToNextPage, cellBi,
						lpHere, cdp, &lpBelowCell,
						pShift, xShift, yShift ) )
	    { LDEB(1); return -1;	}

	if  ( ! cp->cpTopInMergedColumn )
	    { docLayoutPushBottomDown( &lpBottom, &lpBelowCell );	}

	if  ( docDrawAdvanceCell( through, cellBi, lpHere, cdp, &bf,
						dc, pShift, xShift, yShift ) )
	    { LDEB(1); return -1;	}

	if  ( cellAdvanced )
	    { rowAdvanced= 1; cdp->cdpColumnAdvanced= 1;	}

	if  ( cdp->cdpPara < cellBi->biChildCount )
	    { toNextPage= 1;	}
	}

    *pToNextPage= toNextPage; return 0;
    }

/************************************************************************/
/*									*/
/*  Initialisation for row drawing.					*/
/*									*/
/************************************************************************/

static void docStartDrawRowItem(	const BufferItem *	rowBi,
					CellDrawingProgress *	cdp )
    {
    const BufferItem *		sectBi= rowBi->biParent;
    const DocumentGeometry *	dg= &(sectBi->biSectDocumentGeometry);

    const RowProperties *	rp= &(rowBi->biRowProperties);
    const CellProperties *	cp= rp->rpCells;

    int				x0Twips;
    int				x1Twips;

    int				col;

    int				nextRow= rowBi->biNumberInParent+ 1;
    const BufferItem *		nextRowBi= (const BufferItem *)0;

    if  ( nextRow < rowBi->biRowTablePast )
	{ nextRowBi= sectBi->biChildren[nextRow]; }

    x0Twips= dg->dgLeftMarginTwips+ rowBi->biRowLeftIndentTwips;

    for ( col= 0; col < rowBi->biChildCount; cp++, cdp++, col++ )
	{
	x1Twips= dg->dgLeftMarginTwips+ cp->cpRightBoundaryTwips;

	docInitCellDrawingProgress( cdp );

	cdp->cdpX0Twips= x0Twips;
	cdp->cdpX1Twips= x1Twips;

	cdp->cdpBelowCellPosition= rowBi->biBelowPosition;

	if  ( cp->cpTopInMergedColumn		&&
	      nextRow < rowBi->biRowTablePast	)
	    {
	    const CellProperties *	nextCp;
	    int				row;

	    nextCp= &(nextRowBi->biRowCells[col]);

	    if  ( nextCp->cpMergedWithAbove )
		{ cdp->cdpIsMergedWithBelow= 1; }

	    for ( row= nextRow; row < rowBi->biRowTablePast; row++ )
		{
		const BufferItem *	bi;

		bi= rowBi->biParent->biChildren[row];
		nextCp= &(bi->biRowCells[col]);

		if  ( !  nextCp->cpMergedWithAbove )
		    { break;	}

		cdp->cdpBelowCellPosition= bi->biBelowPosition;
		}
	    }

	if  ( cp->cpLeftInMergedRow )
	    {
	    int		col1;

	    for ( col1= col+ 1; col1 < rowBi->biChildCount; col1++ )
		{
		if  ( ! rowBi->biRowCells[col1].cpMergedWithLeft )
		    { break;	}
		}

	    cdp->cdpX1Twips=  dg->dgLeftMarginTwips+
				rowBi->biRowCells[col1-1].cpRightBoundaryTwips;
	    }

	x0Twips= x1Twips;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a table header.						*/
/*									*/
/************************************************************************/

static int docDrawRowItem(	BufferItem *			rowBi,
				void *				through,
				DrawingContext *		dc,
				int				pShift,
				int				xShift,
				int				yShift );

static int docDrawTableHeader(	const BufferItem *		rowBi,
				void *				through,
				DrawingContext *		dc,
				const LayoutPosition *		lpTop )
    {
    int			pShift;
    int			xShift= 0;
    int			yShift;

    BufferItem *	headerBi;

    if  ( rowBi->biRowTableHeaderRow < 0 )
	{ LDEB(rowBi->biRowTableHeaderRow); return -1;	}

    headerBi= rowBi->biParent->biChildren[rowBi->biRowTableHeaderRow];

    pShift= lpTop->lpPage- headerBi->biTopPosition.lpPage;
    yShift= lpTop->lpPageYTwips- headerBi->biTopPosition.lpPageYTwips;

    if  ( docDrawRowItem( headerBi, through, dc, pShift, xShift, yShift ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a table row by successively drawing the portions that fit on	*/
/*  the current page and moving to the next page.			*/
/*									*/
/*  NOTE that any drawing routine can be recursive: Even though Ted	*/
/*	does not support nested tables, the fact that headers and	*/
/*	footers are drawn at a page transition, and that headers and	*/
/*	footers can contain tables can lead to recursion of the row	*/
/*	drawing routine.						*/
/*									*/
/*  2)  Do not draw table headers if the subsequent row appears on the	*/
/*	next page.							*/
/*									*/
/************************************************************************/

static int docDrawRowItem(	BufferItem *			rowBi,
				void *				through,
				DrawingContext *		dc,
				int				pShift,
				int				xShift,
				int				yShift )
    {
    const RowProperties *	rp= &(rowBi->biRowProperties);

    int				toNextPage;

    CellDrawingProgress *	cdp= (CellDrawingProgress *)0;

    LayoutPosition		lpTop;
    LayoutPosition		lpHere;

    int				rval= 0;

    int				nextRow= rowBi->biNumberInParent+ 1;
    const BufferItem *		nextRowBi= (const BufferItem *)0;

    if  ( nextRow < rowBi->biRowTablePast )
	{ nextRowBi= rowBi->biParent->biChildren[nextRow]; }

    if  ( rp->rpCellCount < rowBi->biChildCount )
	{
	LLDEB(rp->rpCellCount,rowBi->biChildCount);
	docListItem( 0, rowBi );
	rval= -1; goto ready;
	}

    /*  2  */
    if  ( nextRowBi			&&
	  rowBi->biRowIsTableHeader	&&
	  pShift == 0			)
	{
	if  ( nextRowBi->biTopPosition.lpPage > rowBi->biBelowPosition.lpPage )
	    { goto ready;	}
	}

    cdp= (CellDrawingProgress *)malloc(
				rp->rpCellCount* sizeof(CellDrawingProgress) );
    if  ( ! cdp )
	{ LXDEB(rp->rpCellCount,cdp); rval= -1; goto ready;	}

    docStartDrawRowItem( rowBi, cdp );

    toNextPage= 0;
    lpTop= rowBi->biTopPosition;
    lpHere= rowBi->biTopPosition;

    lpTop.lpPage += pShift;
    lpTop.lpPageYTwips += yShift;

    lpHere.lpPage += pShift;
    lpHere.lpPageYTwips += yShift;

    if  ( rowBi->biRowPrecededByHeader )
	{
	LayoutPosition	lpHeader;

	lpHeader= rowBi->biRowAboveHeaderPosition;

	if  ( docDrawTableHeader( rowBi, through, dc, &lpHeader ) )
	    { LDEB(rowBi->biRowPrecededByHeader); rval= -1; goto ready;	}
	}

    if  ( docDrawRowPageStrip( rowBi, &lpHere, &toNextPage, cdp,
				through, dc, pShift, xShift, yShift, &lpTop ) )
	{ LDEB(1); rval= -1; goto ready;	}

    while( toNextPage )
	{
	BlockFrame		bf;

	/*  1  */
	if  ( dc->dcLastPage >= 0		&&
	      lpHere.lpPage >= dc->dcLastPage	)
	    { break;	}

	if  ( docDrawToNextColumn( rowBi, rowBi, through, &lpTop, &bf, dc ) )
	    { SDEB(docLevelStr(rowBi->biLevel)); rval= -1; goto ready;	}

	lpHere= lpTop;

	if  ( ! rowBi->biRowIsTableHeader	&&
	      rowBi->biRowTableHeaderRow >= 0	)
	    {
	    if  ( docDrawTableHeader( rowBi, through, dc, &lpHere ) )
		{ LDEB(rowBi->biRowIsTableHeader); rval= -1; goto ready; }
	    }

	if  ( docDrawRowPageStrip( rowBi, &lpHere, &toNextPage, cdp,
			    through, dc, pShift, xShift, yShift, &lpTop ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    if  ( cdp )
	{ free( cdp );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Draw the children of a group item.					*/
/*									*/
/************************************************************************/

static int docDrawGroupItem(	const BufferItem *		bi,
				void *				through,
				DrawingContext *		dc,
				int				pShift,
				int				xShift,
				int				yShift )
    {
    AppDrawingData *		add= dc->dcDrawingData;

    int				i;
    LayoutPosition		lpHere;

    BufferItem *		prevBi= (BufferItem *)0;

    if  ( bi->biChildCount > 0 )
	{ prevBi= bi->biChildren[0];	}

    lpHere= bi->biTopPosition;
    for ( i= 0; i < bi->biChildCount; i++ )
	{
	BufferItem *	childBi= bi->biChildren[i];
	int		y0= BI_TOP_PIXELS_SH( add, childBi, pShift, yShift );

	if  ( dc->dcClipRect		&&
	      y0 > dc->dcClipRect->drY1	)
	    {
	    if  ( childBi->biLevel == DOClevROW		&&
		  childBi->biRowPrecededByHeader	)
		{
		int		hY0;
		int		hY1;

		LayoutPosition	lpHeader= childBi->biRowAboveHeaderPosition;

		hY0= LP_YPIXELS( add, &lpHeader );
		hY1= LP_YPIXELS( add, &(childBi->biTopPosition) );

		if  ( hY0 <= dc->dcClipRect->drY1	&&
		      hY1 >= dc->dcClipRect->drY0	)
		    {
		    if  ( docDrawTableHeader( childBi,
						through, dc, &lpHeader ) )
			{ LDEB(childBi->biRowPrecededByHeader); return -1; }
		    }
		}

	    break;
	    }

	if  ( dc->dcDocumentSelection					&&
	      docCompareItemPositions( childBi,
			    dc->dcDocumentSelection->dsEnd.dpBi ) > 0	)
	    { break;	}

	if  ( dc->dcLastPage >= 0					&&
	      childBi->biTopPosition.lpPage+ pShift > dc->dcLastPage	)
	    { break;	}

	if  ( dc->dcFirstPage < 0					||
	      childBi->biBelowPosition.lpPage+ pShift >=
						    dc->dcFirstPage	)
	    {
	    if  ( docDrawToPageOfItem( prevBi, childBi, childBi,
					    through, &lpHere, dc, pShift ) )
		{ SDEB(docLevelStr(childBi->biLevel)); return -1;	}

	    if  ( docDrawItem( childBi, through, dc ) )
		{ LDEB(i); return -1;	}

	    lpHere= childBi->biBelowPosition;
	    }

	prevBi= childBi;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw the document root item.					*/
/*									*/
/************************************************************************/

static int docDrawDocItem(	const BufferItem *		docBi,
				void *				through,
				DrawingContext *		dc )
    {
    BufferDocument *		bd= dc->dcDocument;
    const DocumentProperties *	dp= &(bd->bdProperties);

    const int			pShift= 0;
    const int			xShift= 0;
    const int			yShift= 0;

    if  ( docDrawGroupItem( docBi, through, dc, pShift, xShift, yShift ) )
	{ LDEB(1); return -1;	}

    if  ( docBi->biInExternalItem == DOCinBODY				&&
	  dp->dpEndnoteProperties.npPosition == FTN_POS_DOC_END	)
	{
	if  ( docDrawEndnotesForDocument( through, dc ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a section and its dependants.					*/
/*									*/
/************************************************************************/

static int docDrawSectItem(	const BufferItem *		sectBi,
				void *				through,
				DrawingContext *		dc )
    {
    BufferDocument *		bd= dc->dcDocument;
    const DocumentProperties *	dp= &(bd->bdProperties);

    const int			pShift= 0;
    const int			xShift= 0;
    const int			yShift= 0;

    if  ( docDrawGroupItem( sectBi, through, dc, pShift, xShift, yShift ) )
	{ LDEB(1); return -1;	}

    if  ( sectBi->biInExternalItem == DOCinBODY				&&
	  dp->dpEndnoteProperties.npPosition == FTN_POS_SECT_END	)
	{
	if  ( docDrawEndnotesForSection( sectBi->biNumberInParent,
							    through, dc ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw an arbitrary item.						*/
/*									*/
/*  1)  Exception for table rows with 'rowspan' cells.			*/
/*  2)  Intersects clipping rectangle?					*/
/*  3)  Inside selection to draw?					*/
/*  4)  Inside page range to draw.					*/
/*									*/
/************************************************************************/

int docDrawItem(	BufferItem *			bi,
			void *				through,
			DrawingContext *		dc )
    {
    AppDrawingData *		add= dc->dcDrawingData;
    const DocumentSelection *	ds= dc->dcDocumentSelection;

    LayoutPosition		lpBelow;

    const int			pShift= 0;

    /*  1  */
    lpBelow= bi->biBelowPosition;
    if  ( bi->biLevel == DOClevROW		&&
	  bi->biRowHasTableParagraphs		)
	{ lpBelow= bi->biRowBelowAllPosition;	}

    /*  2  */
    if  ( dc->dcClipRect )
	{
	if  ( BI_TOP_PIXELS( add, bi ) > dc->dcClipRect->drY1 )
	    { return 0;	}

	if  ( LP_YPIXELS( add, &lpBelow ) < dc->dcClipRect->drY0 )
	    { return 0; }
	}

    /*  3  */
    if  ( ds )
	{
	if  ( docCompareItemPositions( bi, ds->dsBegin.dpBi ) < 0 )
	    { return 0;	}

	if  ( docCompareItemPositions( bi, ds->dsEnd.dpBi ) > 0 )
	    { return 0; }
	}

    /*  4  */
    if  ( dc->dcFirstPage >= 0					&&
	  lpBelow.lpPage+ pShift < dc->dcFirstPage		)
	{ return 0;	}

    if  ( dc->dcLastPage >= 0					&&
	  bi->biTopPosition.lpPage+ pShift > dc->dcLastPage	)
	{ return 0;	}

    switch( bi->biLevel )
	{
	case DOClevDOC:
	    if  ( docDrawDocItem( bi, through, dc ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevSECT:
	    if  ( docDrawSectItem( bi, through, dc ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevCELL:
	rowAsGroup:
	    {
	    const int	xShift= 0;
	    const int	yShift= 0;

	    if  ( docDrawGroupItem( bi, through, dc, pShift, xShift, yShift ) )
		{ LDEB(1); return -1;	}
	    }
	    break;

	case DOClevROW:
	    if  ( ! bi->biRowHasTableParagraphs )
		{ goto rowAsGroup;	}

	    {
	    const int	xShift= 0;
	    const int	yShift= 0;

	    if  ( docDrawRowItem( bi, through, dc, pShift, xShift, yShift ) )
		{ LDEB(1); return -1;	}
	    }

	    break;

	case DOClevPARA:
	    {
	    const int	xShift= 0;
	    const int	yShift= 0;

	    if  ( docDrawParaItem( bi, through, dc, pShift, xShift, yShift ) )
		{ LDEB(1); return -1;	}
	    }

	    break;

	default:
	    LDEB(bi->biLevel); return -1;
	}

    return 0;
    }
