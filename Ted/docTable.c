/************************************************************************/
/*									*/
/*  Delimit tables and table rectangles.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Find the range of rows for a 'table'				*/
/*									*/
/************************************************************************/

int docDelimitTable(	BufferItem *		paraBi,
			BufferItem **		pSectBi,
			int *			pCol,
			int *			pRow0,
			int *			pRow,
			int *			pRow1 )
    {
    BufferItem *	rowBi;
    BufferItem *	sectBi;
    int			col;

    if  ( paraBi->biLevel != DOClevPARA		||
	  ! paraBi->biParaInTable		)
	{ /* LLDEB(paraBi->biLevel,paraBi->biParaInTable); */ return -1; }

    rowBi= paraBi->biParent->biParent;
    sectBi= rowBi->biParent;
    col= paraBi->biParent->biNumberInParent;

    *pSectBi= sectBi; *pCol= col;
    *pRow0= rowBi->biRowTableFirst;
    *pRow= rowBi->biNumberInParent;
    *pRow1= rowBi->biRowTablePast- 1;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Determine the spans of a table cell.				*/
/*									*/
/************************************************************************/

void docTableDetermineCellspans(	int *			pRowspan,
					int *			pColspan,
					const BufferItem *	rowBi0,
					int			col0 )
    {
    const BufferItem *		sectBi0= rowBi0->biParent;

    int				colspan= 1;
    int				rowspan= 1;

    int				row0;
    int				rowp;
    const BufferItem *		rowBi;
    const CellProperties *	cp;
    const CellProperties *	cpx;

    row0= rowBi0->biNumberInParent;
    rowp= rowBi0->biRowTablePast;
    cp= &(rowBi0->biRowCells[col0]);

    if  ( cp->cpMergedWithLeft )
	{ colspan= 0;	}
    else{
	if  ( cp->cpLeftInMergedRow )
	    {
	    int		col;

	    cpx= cp+ 1;
	    for ( col= col0+ 1; col < rowBi0->biRowCellCount; cpx++, col++ )
		{
		if  ( ! cpx->cpMergedWithLeft )
		    { break;	}

		colspan++;
		}
	    }
	}

    if  ( cp->cpMergedWithAbove )
	{ rowspan= 0;	}
    else{
	if  ( cp->cpTopInMergedColumn )
	    {
	    int		row;

	    for ( row= row0+ 1; row < rowp; row++ )
		{
		rowBi= sectBi0->biChildren[row];
		cpx= &rowBi->biRowCells[col0];

		if  ( ! cpx->cpMergedWithAbove )
		    { break;	}

		rowspan++;
		}
	    }
	}

    *pRowspan= rowspan;
    *pColspan= colspan;
    return;
    }

/************************************************************************/
/*									*/
/*  Determine the rectangular area as it is selected in a table.	*/
/*									*/
/*  This funtion fails if the whole selection is not inside ONE table.	*/
/*									*/
/*  1)  Find the table environment of the beginning of the selection.	*/
/*  2)  Find the table environment of the end of the selection.		*/
/*  3)  Different section -> Different table.				*/
/*  4)  Different begin or different end -> Different table.		*/
/*  5)  Paranoia checks.						*/
/*  6)  Delimit rectangle.						*/
/*  7)  If the selection is inside one cell determine its		*/
/*	colspan/rowspan.						*/
/*									*/
/************************************************************************/

int docGetTableRectangle(	TableRectangle *		tr,
				const DocumentSelection *	ds )
    {
    BufferItem *	sectBi0;
    BufferItem *	sectBi1;

    TableRectangle	trBegin;
    TableRectangle	trEnd;

    /*  1  */
    if  ( docDelimitTable( ds->dsBegin.dpBi, &sectBi0,
						&(trBegin.trCol0),
						&(trBegin.trRow00),
						&(trBegin.trRow0),
						&(trBegin.trRow11) ) )
	{ /* LDEB(1); */ return -1;	}

    /*  2  */
    if  ( docDelimitTable( ds->dsEnd.dpBi, &sectBi1,
						&(trEnd.trCol0),
						&(trEnd.trRow00),
						&(trEnd.trRow0),
						&(trEnd.trRow11) ) )
	{ /* LDEB(1); */ return -1;	}

    /*  3  */
    if  ( sectBi0 != sectBi1 )
	{ /* XXDEB(sectBi0,sectBi1); */ return -1;	}

    /*  4  */
    if  ( trBegin.trRow00 != trEnd.trRow00	||
	  trBegin.trRow11 != trEnd.trRow11	)
	{ return -1;	}

    /*  5  */
    if  ( trBegin.trCol0 > trEnd.trCol0 )
	{ return -1;	}
    if  ( trBegin.trRow0 > trEnd.trRow0 )
	{ return -1;	}

    /*  6  */
    tr->trCol0= trBegin.trCol0;
    tr->trCol1= trEnd.trCol0;
    tr->trCol11= sectBi0->biChildren[trBegin.trRow0]->biChildCount- 1;

    tr->trRow00= trBegin.trRow00;
    tr->trRow0= trBegin.trRow0;
    tr->trRow1= trEnd.trRow0;
    tr->trRow11= trBegin.trRow11;

    tr->trCellColspan= 0;
    tr->trCellRowspan= 0;

    /*  7  */
    if  ( tr->trRow0 == tr->trRow1	&&
	  tr->trCol0 == tr->trCol1	)
	{
	docTableDetermineCellspans(
			    &(tr->trCellRowspan), &(tr->trCellColspan),
			    sectBi0->biChildren[trBegin.trRow0], tr->trCol0 );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Get a slice from a table. Either horizontal or vertical or both:	*/
/*  the table as a whole.						*/
/*									*/
/*  1)  Not in a table.. Not a slice either.				*/
/*  2)  Let us decide that a single cell cannot be a slice		*/
/*  3)  Whole column?							*/
/*  4)  Whole row?							*/
/*  5)  Not a row, not a column -> not a slice.				*/
/*									*/
/************************************************************************/

int docGetTableSliceSelection(	int *				pIsRowSlice,
				int *				pIsColSlice,
				TableRectangle *		tr,
				const DocumentSelection *	ds )
    {
    int			isRowSlice= 0;
    int			isColSlice= 0;
    TableRectangle	trTry;

    /*  1  */
    if  ( docGetTableRectangle( &trTry, ds ) )
	{ return 1;	}

    /*  2  */
    if  ( trTry.trRow0 == trTry.trRow1	&&
	  trTry.trCol0 == trTry.trCol1	)
	{ return 1;	}

    /*  3  */
    if  ( trTry.trRow0 == trTry.trRow00 && trTry.trRow1 == trTry.trRow11 )
	{ isColSlice= 1;	}
    /*  4  */
    if  ( trTry.trCol0 == 0 && trTry.trCol1 == trTry.trCol11 )
	{ isRowSlice= 1;	}

    /*  5  */
    if  ( ! isRowSlice && ! isColSlice )
	{ return 1;	}

    *tr= trTry;
    *pIsRowSlice= isRowSlice;
    *pIsColSlice= isColSlice;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Delimit the tables in a section.					*/
/*									*/
/************************************************************************/

void docSectDelimitTables(	BufferItem *		sectBi )
    {
    int		n;
    int		headerRow= -1;

    if  ( sectBi->biLevel != DOClevSECT )
	{ LDEB(sectBi->biLevel); return;	}

    n= 0;
    while( n < sectBi->biChildCount )
	{
	BufferItem *	rowBiFirst;
	BufferItem *	rowBi;

	int		first;
	int		past;

	rowBiFirst= sectBi->biChildren[n];
	if  ( ! rowBiFirst->biRowHasTableParagraphs )
	    {
	    rowBiFirst->biRowTableHeaderRow= -1;
	    rowBiFirst->biRowTableFirst= -1;
	    rowBiFirst->biRowTablePast= -1;

	    headerRow= -1;
	    n++; continue;
	    }

	first= n;
	past= n+ 1;

	while( past < sectBi->biChildCount )
	    {
	    rowBi= sectBi->biChildren[past];

	    if  ( ! docApproximatelyAlignedColumns(
					&(rowBiFirst->biRowProperties),
					&(rowBi->biRowProperties) )	)
		{ break;	}

	    /*
	    No !
	    if  ( ! docEqualRowBorders( &(rowBiFirst->biRowProperties),
					&(rowBi->biRowProperties) )	)
		{ break;	}
	    */

	    past++; 
	    }

	while( n < past )
	    {
	    rowBi= sectBi->biChildren[n];

	    if  ( rowBi->biRowIsTableHeader )
		{ headerRow= n;	}

	    rowBi->biRowTableHeaderRow= headerRow;
	    rowBi->biRowTableFirst= first;
	    rowBi->biRowTablePast= past;

	    n++;
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Decide what top border to use for a table cell (If any)		*/
/*									*/
/*  1)  Get the table row above the current row.			*/
/*  2)  If a cell has a bottom boder, and the next one a top border,	*/
/*	MS-Word only draws the bottom border of the topmost cell.	*/
/*  3)  By default use the cells top border.				*/
/*  4)  Use the table top border at the top of a page.			*/
/*									*/
/************************************************************************/

void docGetCellTopBorder(	const BorderProperties **	pBpTop,
				int *				pUseAbove,
				const BufferItem *		rowBi,
				int				col,
				int				atTopOfPage )
    {
    const BufferItem *		sectBi= rowBi->biParent;
    const CellProperties *	cp= &(rowBi->biRowCells[col]);

    const BufferItem *		rowBiAbove= (const BufferItem *)0;

    const CellProperties *	aboveCpR= (const CellProperties *)0;
    const CellProperties *	aboveCpC= (const CellProperties *)0;
    int				useAbove= 0;

    const BorderProperties *	bpTop;

    /*  1  */
    if  ( rowBi->biNumberInParent > rowBi->biRowTableFirst )
	{
	if  ( rowBi->biNumberInParent <= 0 )
	    { LDEB(rowBi->biNumberInParent);	}
	else{ rowBiAbove= sectBi->biChildren[rowBi->biNumberInParent- 1]; }
	}

    if  ( rowBiAbove )
	{ aboveCpR= rowBiAbove->biRowCells;	}

    if  ( rowBiAbove && col < rowBiAbove->biRowCellCount )
	{ aboveCpC= aboveCpR+ col;				}

    /*  2  */
    if  ( aboveCpC && DOCisBORDER( &(aboveCpC->cpBottomBorder) ) )
	{ useAbove= 1;	}

    /*  3  */
    bpTop= &(cp->cpTopBorder);

    /*  2  */
    if  ( useAbove )
	{ bpTop= &(aboveCpC->cpBottomBorder);	}

    /*  4  */
    if  ( rowBi->biNumberInParent > rowBi->biRowTableFirst	&&
          atTopOfPage						)
	{
	bpTop= &(rowBi->biRowTopBorder);
	useAbove= 0;
	}

    *pBpTop= bpTop;
    *pUseAbove= useAbove;

    return;
    }

/************************************************************************/
/*									*/
/*  Decide what bottom border to use for a table cell (If any)		*/
/*									*/
/*  0)  Paranoia checks: Bugs that I actually once programmed.		*/
/*  1)  Get the table row below the current row.			*/
/*  2)  By default use the cells bottom border.				*/
/*  3)  If a cell has a bottom boder, and the next one a top border,	*/
/*	MS-Word only draws the bottom border of the topmost cell.	*/
/*  4)  Use the table bottom border at the bottom of a page.		*/
/*									*/
/************************************************************************/

void docGetCellBottomBorder(	const BorderProperties **	pBpBottom,
				int *				pUseBelow,
				const BufferItem *		rowBi,
				int				col,
				int				atBottomOfPage )
    {
    const BufferItem *		sectBi= rowBi->biParent;
    const CellProperties *	cp= &(rowBi->biRowCells[col]);

    const BufferItem *		rowBiBelow= (const BufferItem *)0;

    const CellProperties *	belowCpR= (const CellProperties *)0;
    const CellProperties *	belowCpC= (const CellProperties *)0;
    int				useBelow= 0;

    const BorderProperties *	bpBottom;

    /*  0  */
    if  ( col >= rowBi->biRowCellCount )
	{ LLDEB(col,rowBi->biRowCellCount);	}
    if  ( rowBi->biRowTablePast < 0				||
	  rowBi->biRowTablePast > rowBi->biParent->biChildCount	)
	{ LLDEB(rowBi->biRowTablePast,rowBi->biParent->biChildCount); }

    /*  1  */
    if  ( rowBi->biNumberInParent < rowBi->biRowTablePast- 1 )
	{ rowBiBelow= sectBi->biChildren[rowBi->biNumberInParent+ 1];	}

    if  ( rowBiBelow )
	{ belowCpR= rowBiBelow->biRowCells;	}

    if  ( rowBiBelow && col < rowBiBelow->biRowCellCount )
	{ belowCpC= belowCpR+ col;				}

    /*  2  */
    bpBottom= &(cp->cpBottomBorder);

    /*  3  */
    if  ( ! DOCisBORDER( &(cp->cpBottomBorder) )	&&
	  belowCpC					&&
	  DOCisBORDER( &(belowCpC->cpBottomBorder) )	)
	{ useBelow= 1;	}

    /*  4  */
    if  ( rowBi->biNumberInParent < rowBi->biRowTablePast- 1	&&
	  atBottomOfPage					)
	{
	bpBottom= &(rowBi->biRowBottomBorder);
	useBelow= 0;
	}

    *pBpBottom= bpBottom;
    *pUseBelow= useBelow;

    return;
    }

