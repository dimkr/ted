/************************************************************************/
/*									*/
/*  Layout of a table row in the document.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	"docLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Some column layout administration.					*/
/*									*/
/************************************************************************/

typedef struct ColumnLayoutJob
    {
    ParagraphLayoutJob	cljPlj;
    int			cljIsMergedWithBelow;
    int			cljMergedCellTopRow;
    LayoutPosition	cljMergedCellBelowPosion;
    } ColumnLayoutJob;

static void docInitColumnLayoutJob(	ColumnLayoutJob *	clj )
    {
    docInitParagraphLayoutJob( &(clj->cljPlj) );

    clj->cljIsMergedWithBelow= 0;
    clj->cljMergedCellTopRow= -1;

    docInitLayoutPosition( &(clj->cljMergedCellBelowPosion) );

    return;
    }

/************************************************************************/
/*									*/
/*  Calculate layout insets for a (table) row.				*/
/*									*/
/*  This routine is also used to calculate the inset that is used to	*/
/*  accomodate space for the bottom border of the previous row. This	*/
/*  inset is not only used in real rows, but also in the immediate	*/
/*  successor of a real row.						*/
/*									*/
/*  1)  Reserve space for the top border of all cells.			*/
/*									*/
/************************************************************************/

static void docLayoutCalculateRowTopInset(
					BufferItem *		rowBi,
					int			atTopOfPage )
    {
    int				col;

    rowBi->biTopInsetTwips= 0;

    if  ( ! rowBi->biRowHasTableParagraphs )
	{ LDEB(rowBi->biRowHasTableParagraphs); return;	}

    /*  1  */
    for ( col= 0; col < rowBi->biChildCount; col++ )
	{
	int				useAbove= 0;
	const BorderProperties *	bpTop;

	docGetCellTopBorder( &bpTop, &useAbove, rowBi, col, atTopOfPage );

	docLayoutStretchInsetForBorder( &(rowBi->biTopInsetTwips), bpTop );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Layout as much of a table row as fits on the current page.		*/
/*									*/
/*  1)  For a fixed height row, determine the amount of space left for	*/
/*	it.								*/
/*  2)  For all columns in the table..					*/
/*  3)  Cells in a row span or a colspan play no role...		*/
/*  4)  This column was already exhausted on a previous page..		*/
/*  5)  Format paragraphs inside this cell.				*/
/*  6)  Remember how far we have descended on the page.			*/
/*  7)  Anything left over that must be moved to the next page?		*/
/*									*/
/************************************************************************/

static int docLayoutRowPageStrip( BufferItem *			rowBi,
				int *				pToNextPage,
				int *				pRowHeight,
				const ParagraphLayoutContext *	plc,
				ColumnLayoutJob *		clj,
				BlockFrame *			bf,
				LayoutJob *			lj )
    {
    int				col;

    int				continueOnNextPage= 0;

    LayoutPosition		lpBottom;
    int				bottomTwips= -1;
    int				stripHigh= -1;
    const CellProperties *	cp;

    const ScreenLayout *	sl= &(plc->plcScreenLayout);

    lpBottom= lj->ljPosition;

    continueOnNextPage= 0;

    /*  1  */
    if  ( rowBi->biRowHeightTwips < 0 )
	{
	bottomTwips= lj->ljPosition.lpPageYTwips-
				    rowBi->biRowHeightTwips- *pRowHeight;

	stripHigh= -rowBi->biRowHeightTwips;
	}

    /*  2  */
    cp= rowBi->biRowCells;
    for ( col= 0; col < rowBi->biChildCount; cp++, clj++, col++ )
	{
	BufferItem *			cellBi= rowBi->biChildren[col];
	ParagraphLayoutJob *		plj= &(clj->cljPlj);
	ParagraphLayoutPosition *	plp= &(plj->pljPos);

	/*  3  */
	if  ( cp->cpMergedWithLeft || cp->cpMergedWithAbove )
	    { continue;	}

	/*  4  */
	if  ( plp->plpProgress.pspPara >= cellBi->biChildCount )
	    { continue;	}

	/*  5  */
	plp->plpPos= lj->ljPosition;

	docParagraphStripFrame( sl, cellBi, plc->plcAdd,
					bottomTwips, stripHigh, bf, plj );

	if  ( docLayoutParagraphsInStrip( plj, bf, plc, cellBi,
			    plj->pljParaUpto, bottomTwips, stripHigh ) )
	    { LDEB(1); return -1;	}

	/*  6  */
	docLayoutPushBottomDown(
			&(rowBi->biRowBelowAllPosition), &(plp->plpPos) );
	if  ( ! clj->cljIsMergedWithBelow )
	    { docLayoutPushBottomDown( &lpBottom, &(plp->plpPos) );	}

	/*  7  */
	if  ( cellBi->biInExternalItem == DOCinBODY		&&
	      plp->plpProgress.pspPara < plj->pljParaUpto  	&&
	      ( bottomTwips < 0				||
	        plp->plpPos.lpPageYTwips < bottomTwips	)	)
	    { continueOnNextPage= 1;	}
	}

    *pRowHeight += lpBottom.lpPageYTwips- lj->ljPosition.lpPageYTwips;

    lj->ljPosition= lpBottom;

    *pToNextPage= continueOnNextPage;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Calculate the layout of a row in the document.			*/
/*									*/
/*  1)  Advance by top inset.						*/
/*  2)  Begin formatting.						*/
/*  3)  Columns with a colspan do not push the bottom of THIS row down.	*/
/*	Remember this.							*/
/*  4)  But in a cell with a rowspan, the top cell of the cell might	*/
/*	push the bottom down. Look for it.				*/
/*									*/
/************************************************************************/

static void docLayoutStartRow(	BufferItem *			rowBi,
				LayoutPosition *		lp,
				ColumnLayoutJob *		clj )
    {
    CellProperties *		cp;
    int				col;

    int				nextRow= rowBi->biNumberInParent+ 1;
    const BufferItem *		nextRowBi= (const BufferItem *)0;

    if  ( nextRow < rowBi->biRowTablePast )
	{ nextRowBi= rowBi->biParent->biChildren[nextRow]; }

    /*  1  */
    if  ( rowBi->biTopInsetTwips > 0 )
	{
	lp->lpPageYTwips += rowBi->biTopInsetTwips;
	lp->lpAtTopOfColumn= 0;
	}

    rowBi->biRowBelowAllPosition= *lp;

    /*  2  */
    cp= rowBi->biRowCells;
    for ( col= 0; col < rowBi->biChildCount; cp++, clj++, col++ )
	{
	BufferItem *	cellBi= rowBi->biChildren[col];
	const int	para= 0;
	const int	line= 0;
	const int	part= 0;

	docInitColumnLayoutJob( clj );

	clj->cljPlj.pljParaUpto= cellBi->biChildCount;

	docPsBeginParagraphLayoutProgress( &(clj->cljPlj), para, line, part,
						cellBi->biChildCount, lp );

	cellBi->biTopPosition= *lp;

	/*  3,4  */
	if  ( cp->cpTopInMergedColumn || cp->cpMergedWithAbove )
	    {
	    if  ( nextRow < rowBi->biRowTablePast )
		{
		const CellProperties *	nextCp;

		nextCp= &(nextRowBi->biRowCells[col]);

		/*  3  */
		if  ( cp->cpTopInMergedColumn && nextCp->cpMergedWithAbove )
		    { clj->cljIsMergedWithBelow= 1; }

		/*  4  */
		if  ( cp->cpMergedWithAbove && ! nextCp->cpMergedWithAbove )
		    {
		    int		topRow;

		    for ( topRow= rowBi->biNumberInParent- 1;
			  topRow >= rowBi->biRowTableFirst;
			  topRow-- )
			{
			const BufferItem *	topRowBi;
			const CellProperties *	topCp;

			topRowBi= rowBi->biParent->biChildren[topRow];
			topCp= &(topRowBi->biRowCells[col]);

			if  ( topCp->cpTopInMergedColumn )
			    {
			    const BufferItem *	topCellBi;

			    topCellBi= topRowBi->biChildren[col];

			    clj->cljMergedCellTopRow= topRow;
			    clj->cljMergedCellBelowPosion=
						topCellBi->biBelowPosition;
			    break;
			    }
			}

		    if  ( clj->cljMergedCellTopRow < 0 )
			{ LDEB(clj->cljMergedCellTopRow);	}
		    }
		}
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Reserve space for a table header.					*/
/*									*/
/************************************************************************/

static void docLayoutRowSkipHeaderHeight( LayoutPosition *	lp,
					BufferItem *		rowBi,
					int			atTopOfRow )
    {
    const BufferItem *	headerBi;
    int			headerHeight;

    if  ( rowBi->biRowTableHeaderRow < 0 )
	{ LDEB(rowBi->biRowTableHeaderRow); return;	}
    if  ( rowBi->biRowIsTableHeader )
	{ LDEB(rowBi->biRowIsTableHeader); return;	}

    headerBi= rowBi->biParent->biChildren[rowBi->biRowTableHeaderRow];
    if  ( ! headerBi->biRowIsTableHeader )
	{ LDEB(headerBi->biRowIsTableHeader);	}

    if  ( headerBi->biBelowPosition.lpPage	!=
	  headerBi->biTopPosition.lpPage	)
	{
	LLDEB(headerBi->biBelowPosition.lpPage,headerBi->biTopPosition.lpPage);
	return;
	}

    if  ( atTopOfRow )
	{
	rowBi->biRowAboveHeaderPosition= *lp;
	rowBi->biRowPrecededByHeader= 1;
	}

    headerHeight= headerBi->biBelowPosition.lpPageYTwips- 
					headerBi->biTopPosition.lpPageYTwips;

    lp->lpAtTopOfColumn= 0;
    lp->lpPageYTwips += headerHeight;

    return;
    }

/************************************************************************/
/*									*/
/*  Continue a table row on the next page.				*/
/*									*/
/*  1)  Finish the footnotes that we found on the current page.		*/
/*  2)  Move to the next page. (Column in multi column sections)	*/
/*  3)  Skipping to a subsequent page.. Allocate space for the table	*/
/*	header.								*/
/*  4)  Reserve space for the top border of the cells.			*/
/*  5)  Initialize the layout of the different table columns.		*/
/*									*/
/************************************************************************/

static int docRowToNextPage(	BufferItem *			rowBi,
				LayoutJob *			lj,
				int				bottomTwips,
				int				stripHigh,
				int				atTopOfRow,
				BlockFrame *			bf,
				ColumnLayoutJob *		rowClj,
				const ParagraphLayoutContext *	plc )
    {
    CellProperties *		cp;
    int				col;

    ColumnLayoutJob *		clj;

    LayoutPosition		lpBefore;

    const int			atTopOfPage= 1;
    int				inset= 0;
    const int			belowText= 0;
    LayoutPosition		lpBelowNotes;

    lpBefore= lj->ljPosition;

    /*  1  */
    if  ( BF_HAS_FOOTNOTES( bf )					&&
	  docLayoutFootnotesForColumn( &lpBelowNotes, bf,
				    belowText, &(lj->ljPosition), lj )	)
	{ LDEB(1); return -1;	}

    /*  2  */
    docLayoutToNextColumn( rowBi, lj->ljBd, &(lj->ljPosition), bf );

    /*  3  */
    if  ( ! rowBi->biRowIsTableHeader		&&
	  rowBi->biRowTableHeaderRow >= 0	)
	{
	docLayoutRowSkipHeaderHeight( &(lj->ljPosition), rowBi, atTopOfRow );
	}

    /*  4  */
    for ( col= 0; col < rowBi->biChildCount; col++ )
	{
	int				useAbove= 0;
	const BorderProperties *	bpTop;

	docGetCellTopBorder( &bpTop, &useAbove, rowBi, col, atTopOfPage );

	docLayoutStretchInsetForBorder( &inset, bpTop );
	}

    lj->ljPosition.lpPageYTwips += inset;

    /*  5  */
    cp= rowBi->biRowCells;
    clj= rowClj;
    for ( col= 0; col < rowBi->biChildCount; cp++, clj++, col++ )
	{
	ParagraphLayoutJob *	plj= &(clj->cljPlj);
	BufferItem *		cellBi= rowBi->biChildren[col];
	ParagraphStripPosition	psp0;
	int			advanced;

	if  ( plj->pljPos.plpProgress.pspPara >= cellBi->biChildCount )
	    { continue;	}

	psp0= plj->pljPos0.plpProgress;

	docPsAdvanceParagraphLayout( &advanced,
					    &psp0,
					    &(plj->pljPos0.plpProgress),
					    &(plj->pljPos.plpProgress),
					    lpBefore.lpPage, cellBi );

	if  ( advanced )
	    {
	    plj->pljPos0.plpProgress= psp0;
	    plj->pljPos.plpProgress= psp0;
	    }
	else{ plj->pljPos0.plpProgress= plj->pljPos.plpProgress;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Finish row layout.							*/
/*									*/
/*  1)  The bottom is below all cells.					*/
/*  2)  If there is a bottom inset, step over it.			*/
/*  3)  Stretch height to the minimum height (If any)			*/
/*  3)  Stretch height to the exact given height (If any)		*/
/*									*/
/************************************************************************/

static void docLayoutFinishRow(	BufferItem *		rowBi,
				LayoutJob *		lj,
				ColumnLayoutJob *	clj,
				int			rowHeightTwips )
    {
    int				col;
    CellProperties *		cp;

    /*  1  */
    cp= rowBi->biRowCells;
    for ( col= 0; col < rowBi->biChildCount; cp++, clj++, col++ )
	{
	BufferItem *			cellBi= rowBi->biChildren[col];
	ParagraphLayoutJob *		plj= &(clj->cljPlj);
	ParagraphLayoutPosition *	plp= &(plj->pljPos);

	if  ( cp->cpMergedWithAbove && clj->cljMergedCellTopRow >= 0 )
	    {
	    docLayoutPushBottomDown( &(rowBi->biRowBelowAllPosition),
					&(clj->cljMergedCellBelowPosion) );
	    docLayoutPushBottomDown( &(lj->ljPosition),
					&(clj->cljMergedCellBelowPosion) );
	    continue;
	    }

	if  ( cp->cpMergedWithLeft || cp->cpMergedWithAbove )
	    { continue;	}

	plp->plpPos.lpPageYTwips += cellBi->biBottomInsetTwips;
	plp->plpPos.lpAtTopOfColumn= 0;

	cellBi->biBelowPosition= plp->plpPos;

	docLayoutPushBottomDown(
			&(rowBi->biRowBelowAllPosition), &(plp->plpPos) );

	if  ( ! clj->cljIsMergedWithBelow )
	    { docLayoutPushBottomDown( &(lj->ljPosition), &(plp->plpPos) ); }
	}

    /*  2  */
    if  ( rowBi->biBottomInsetTwips > 0 )
	{
	lj->ljPosition.lpPageYTwips += rowBi->biBottomInsetTwips;
	lj->ljPosition.lpAtTopOfColumn= 0;
	}

    /*  3  */
    if  ( rowHeightTwips < rowBi->biRowHeightTwips )
	{
	lj->ljPosition.lpPageYTwips +=
				rowBi->biRowHeightTwips- rowHeightTwips;
	lj->ljPosition.lpAtTopOfColumn= 0;
	}

    /*  4  */
    if  ( rowHeightTwips < -rowBi->biRowHeightTwips )
	{
	lj->ljPosition.lpPageYTwips +=
				-rowBi->biRowHeightTwips- rowHeightTwips;
	lj->ljPosition.lpAtTopOfColumn= 0;
	}

    docLayoutPushBottomDown( &(rowBi->biRowBelowAllPosition),
							&(lj->ljPosition) );

    return;
    }

/************************************************************************/
/*									*/
/*  Calculate the layout of a table row.				*/
/*									*/
/*  NOTE  Table headers that appear by themselves as the last table row	*/
/*	on the page should have been pushed to the next page. Some	*/
/*	thought reveals however that the result on the formatting of	*/
/*	the rest of the document is not altered by moving the header	*/
/*	row to the next page. For that reason, and to keep the 		*/
/*	formatter simple, we simplay ignore the situation. To		*/
/*	compensate, the drawing code simply skips the row in its home	*/
/*	position and the visible result is the same.			*/
/*									*/
/*  1)  Sanity check against crashes.					*/
/*	This fix is a thourough memory leak. It is hardly ever		*/
/*	activated and it prevents crashes with incorrect rtf files.	*/
/*  2)  Allocate memory to monitor the progress of the layout of the	*/
/*	cells in the row.						*/
/*  3)  Initialize and place as much row content on the current page	*/
/*	as possible.							*/
/*  4)  Some of the content of the row did not fit on the current page.	*/
/*	Find out whether the row must be restarted on the next page.	*/
/*	(This does not make sense if the top of the row already is at	*/
/*	the top of the page.)						*/
/*  5)  When there are cells that remain empty on this page.. restart	*/
/*	on the next page.						*/
/*  6)  Go to the next page.						*/
/*  7)  If there is a reason, reinitialize the row layout job, to move	*/
/*	the whole row to the next page.					*/
/*  8)  Continue or restart (7) on the next page.			*/
/*  9)  As long as necessary, move to the next page and place content	*/
/*	there. (The exceptions above only apply for the first page, as	*/
/*	it is clearly impossible to keep the contents of a table higher	*/
/*	than one page on one page)					*/
/*									*/
/************************************************************************/

int docLayoutRowItem(		BufferItem *			rowBi,
				BlockFrame *			bf,
				LayoutJob *			lj,
				const ParagraphLayoutContext *	plc )
    {
    int				rval= 0;
    int				rowHeightTwips;
    int				toNextPage;

    ColumnLayoutJob *		rowClj;

    int				bottomTwips= -1;

    const ScreenLayout *	sl= &(plc->plcScreenLayout);
    int				keepRowOnOnePage;

    keepRowOnOnePage= rowBi->biRowKeepOnOnePage || rowBi->biRowIsTableHeader;

    /*  1  */
    if  ( rowBi->biRowCellCount < rowBi->biChildCount )
	{
	LLDEB(rowBi->biRowCellCount,rowBi->biChildCount);
	docListItem( 0, rowBi );
	rowBi->biChildCount= rowBi->biRowCellCount;
	}

    /*  2  */
    rowClj= (ColumnLayoutJob *)malloc( 
			rowBi->biRowCellCount* sizeof(ColumnLayoutJob) );
    if  ( ! rowClj )
	{ LXDEB(rowBi->biRowCellCount,rowClj); rval= -1; goto ready;	}

    if  ( sl && sl->slStartRow					&&
	  (*sl->slStartRow)( rowBi, lj->ljAdd, lj->ljBd )	)
	{ LDEB(1); rval= -1; goto ready;	}

    /*  3  */
    toNextPage= 0;
    rowHeightTwips= 0;
    if  ( rowBi->biRowHeightTwips < 0 )
	{ bottomTwips= lj->ljPosition.lpPageYTwips- rowBi->biRowHeightTwips; }

    docLayoutCalculateRowTopInset( rowBi, lj->ljPosition.lpAtTopOfColumn );

    rowBi->biRowAboveHeaderPosition= rowBi->biTopPosition;
    rowBi->biRowPrecededByHeader= 0;

    docLayoutStartRow( rowBi, &(lj->ljPosition), rowClj );

    if  ( docLayoutRowPageStrip( rowBi, &toNextPage,
				    &rowHeightTwips, plc, rowClj, bf, lj ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  4  */
    if  ( toNextPage && ! rowBi->biTopPosition.lpAtTopOfColumn )
	{
	int				someAtTop= 0;
	int				restartRow;
	int				col;
	const ColumnLayoutJob *		clj;

	/*  5  */
	clj= rowClj;
	for ( col= 0; col < rowBi->biChildCount; clj++, col++ )
	    {
	    const ParagraphLayoutJob *		plj= &(clj->cljPlj);
	    const ParagraphLayoutPosition *	plp= &(plj->pljPos);
	    const ParagraphStripPosition *	psp= &(plp->plpProgress);

	    if  ( psp->pspPara == 0 && psp->pspPart == 0 )
		{
		const BufferItem *	cellBi= rowBi->biChildren[col];

		if  ( cellBi->biChildCount > 0 )
		    {
		    const BufferItem *	paraBi;

		    paraBi= cellBi->biChildren[psp->pspPara];

		    if  ( psp->pspPart < paraBi->biParaParticuleCount )
			{ someAtTop= 1; break; }
		    }
		}
	    }

	/*  7  */
	restartRow= rowBi->biRowKeepOnOnePage || someAtTop;

	/*  6  */
	if  ( docRowToNextPage( rowBi, lj, bottomTwips, rowHeightTwips,
							restartRow,
							bf, rowClj, plc ) )
	    { LDEB(toNextPage); rval= -1; goto ready;	}

	/*  7  */
	if  ( restartRow )
	    {
	    rowBi->biTopPosition= lj->ljPosition;

	    if  ( rowBi->biRowIsTableHeader		||
		  rowBi->biRowTableHeaderRow <= 0	)
		{ rowBi->biRowAboveHeaderPosition= rowBi->biTopPosition; }

	    docLayoutCalculateRowTopInset( rowBi,
					    lj->ljPosition.lpAtTopOfColumn );

	    docLayoutStartRow( rowBi, &(lj->ljPosition), rowClj );

	    rowHeightTwips= 0;
	    if  ( rowBi->biRowHeightTwips < 0 )
		{
		bottomTwips=
			lj->ljPosition.lpPageYTwips- rowBi->biRowHeightTwips;
		}
	    }

	/*  8  */
	if  ( docLayoutRowPageStrip( rowBi, &toNextPage,
				&rowHeightTwips, plc, rowClj, bf, lj ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    /*  9  */
    while( toNextPage )
	{
	const int	atTopOfRow= 0;

	if  ( docRowToNextPage( rowBi, lj, bottomTwips, rowHeightTwips,
						atTopOfRow, bf, rowClj, plc ) )
	    { LDEB(toNextPage); rval= -1; goto ready;	}

	if  ( docLayoutRowPageStrip( rowBi, &toNextPage,
				&rowHeightTwips, plc, rowClj, bf, lj ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    docLayoutFinishRow( rowBi, lj, rowClj, rowHeightTwips );

  ready:

    if  ( rowClj )
	{ free( rowClj );	}

    return rval;
    }

