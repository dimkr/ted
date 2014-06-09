/************************************************************************/
/*									*/
/*  Table editing.							*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"docEdit.h"
#   include	"docLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Insert a series of rows in a table.					*/
/*									*/
/************************************************************************/

int docInsertTableRows(		BufferItem *		sectBi,
				BufferItem **		pNewBi,
				const BufferItem *	refRowBi,
				const RowProperties *	rp,
				int			textAttributeNumber,
				int			n,
				int			paraNr,
				int			rows,
				EditOperation *		eo )
    {
    DocumentPosition	dp;

    int			row;
    int			firstPos= n;

    for ( row= 0; row < rows; n++, row++ )
	{
	BufferItem *		rowBi= (BufferItem *)0;

	const int		stroffFrom= 0;
	const int		sectShift= 0;
	const int		stroffShift= 0;
	const int		paraShift= rp->rpCellCount;

	rowBi= docInsertRowItem( eo->eoBd, sectBi, n, rp, textAttributeNumber );
	if  ( ! rowBi )
	    { XDEB(rowBi); return -1;	}

	docEditShiftReferences( eo, &(eo->eoSelectionScope),
				    paraNr, stroffFrom,
				    sectShift, paraShift, stroffShift );

	paraNr += rp->rpCellCount;

	if  ( refRowBi						&&
	      docCopyRowColumnAttributes( rowBi, refRowBi )	)
	    { LDEB(1); return -1;	}

	docEditIncludeItemInReformatRange( eo, rowBi );
	}

    docSectDelimitTables( sectBi );

    if  ( docFirstPosition( &dp, sectBi->biChildren[firstPos] ) )
	{ LDEB(firstPos);	}

    *pNewBi= dp.dpBi; return 0;
    }

/************************************************************************/
/*									*/
/*  Insert cells/columns in tables.					*/
/*									*/
/************************************************************************/

static BufferItem * docInsertCellInRow(
				BufferDocument *	bd,
				BufferItem *		rowBi,
				int			col,
				const CellProperties *	cp,
				int			textAttributeNumber )
    {
    const int * const	colorMap= (const int *)0;
    RowProperties *	rp;

    BufferItem *	cellBi;
    BufferItem *	paraBi;

    rp= &(rowBi->biRowProperties);

    if  ( docInsertRowColumn( rp, col, cp, colorMap ) )
	{ LDEB(col); return (BufferItem *)0;	}

    cellBi= docInsertItem( bd, rowBi, col, DOClevCELL );
    if  ( ! cellBi )
	{ LXDEB(col,cellBi); return (BufferItem *)0;	}

    paraBi= docInsertEmptyParagraph( bd, cellBi, textAttributeNumber );
    if  ( ! paraBi )
	{ XDEB(paraBi); return (BufferItem *)0;	}

    paraBi->biParaInTable= 1;

    return paraBi;
    }

/************************************************************************/
/*									*/
/*  Insert cells/columns in tables.					*/
/*									*/
/************************************************************************/

int docSplitColumnInRows(	BufferItem **		pNewParaBi,
				EditOperation *		eo,
				BufferItem *		sectBi,
				int			row0,
				int			row,
				int			row1,
				int			col,
				int			after )
    {
    BufferDocument *		bd= eo->eoBd;

    BufferItem *		newParaBi= (BufferItem *)0;

    BufferItem *		rowBi;

    if  ( after )
	{ after= 1;	}

    while( row0 <= row1 )
	{
	RowProperties *		rp;

	CellProperties *	oldCp;
	CellProperties		newCp;

	int			left;
	int			middle;

	BufferItem *		cellBi;
	BufferItem *		paraBi;

	int			textAttributeNumber;

	rowBi= sectBi->biChildren[row0];
	rp= &(rowBi->biRowProperties);

	oldCp= rp->rpCells+ col;

	if  ( col == 0 )
	    { left= rp->rpLeftIndentTwips;			}
	else{ left= rp->rpCells[col-1].cpRightBoundaryTwips;	}

	middle= ( rp->rpCells[col].cpRightBoundaryTwips+ left )/ 2;

	newCp= *oldCp;
	if  ( after )
	    { oldCp->cpRightBoundaryTwips= middle;	}
	else{ newCp. cpRightBoundaryTwips= middle;	}

	cellBi= rowBi->biChildren[col];
	paraBi= cellBi->biChildren[0];
	textAttributeNumber= paraBi->biParaParticules[0].tpTextAttributeNumber;

	paraBi= docInsertCellInRow( bd, rowBi, col+ after, &newCp,
							textAttributeNumber );
	if  ( row0 == row )
	    { newParaBi= paraBi;	}

	{
	const int	stroffFrom= 0;
	const int	stroffShift= 0;
	const int	sectShift= 0;

	const int	paraNr= docNumberOfParagraph( paraBi );
	const int	paraShift= 1;

	docEditShiftReferences( eo, &(eo->eoSelectionScope),
					paraNr, stroffFrom,
					sectShift, paraShift, stroffShift );
	}

	docEditIncludeItemInReformatRange( eo, rowBi );

	row0++;
	}

    docSectDelimitTables( sectBi );

    *pNewParaBi= newParaBi;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Update the properties of a table in response to a user action.	*/
/*									*/
/************************************************************************/

void docChangeTableLayout(	EditOperation *		eo,
				BufferItem *		sectBi,
				int			row0,
				int			row1,
				int			col0,
				int			col1,
				const PropertyMask *	rpSetMask,
				const PropertyMask *	cpSetMask,
				const RowProperties *	rpSet )
    {
    BufferItem *		rowBi= sectBi->biChildren[row0];

    PropertyMask		rpDoneMask;
    PropertyMask		cpDoneMask;

    const int * const		colorMap= (const int *)0;

    PROPmaskCLEAR( &rpDoneMask );
    PROPmaskCLEAR( &cpDoneMask );

    if  ( col1 > rpSet->rpCellCount- 1 )
	{ col1=  rpSet->rpCellCount- 1;	}

    while( row0 <= row1 )
	{
	int			col;

	rowBi= sectBi->biChildren[row0];

	if  ( docUpdRowProperties( &rpDoneMask, &(rowBi->biRowProperties),
						rpSetMask, rpSet, colorMap ) )
	    { LDEB(row0);	}

	if  ( ! utilPropMaskIsEmpty( cpSetMask ) )
	    {
	    const CellProperties *	cpSet;
	    CellProperties *		cpTo;

	    cpSet= rpSet->rpCells+ col0;
	    cpTo= rowBi->biRowProperties.rpCells+ col0;
	    for ( col= col0; col <= col1; cpTo++, cpSet++, col++ )
		{
		if  ( col >= rowBi->biRowCellCount )
		    { break;	}

		if  ( docUpdCellProperties( &cpDoneMask, cpTo,
						cpSetMask, cpSet, colorMap ) )
		    { LLDEB(row0,col);	}
		}
	    }

	docEditIncludeItemInReformatRange( eo, rowBi );

	row0++;
	}

    docSectDelimitTables( sectBi );

    return;
    }

/************************************************************************/
/*									*/
/*  Delete columns from a table.					*/
/*									*/
/************************************************************************/

int docDeleteColumnsFromRows(	EditOperation *	eo,
				BufferItem *	sectBi,
				int		delRow0,
				int		delRow1,
				int		delCol0,
				int		delCol1 )
    {
    int				count= delCol1- delCol0+ 1;
    int				row;

    for ( row= delRow0; row <= delRow1; row++ )
	{
	BufferItem *	rowBi= sectBi->biChildren[row];

	int		sectionsDeleted= 0;
	int		firstParaDeleted= -1;
	int		paragraphsDeleted= 0;

	if  ( docDeleteColumnsFromRow( &(rowBi->biRowProperties),
							    delCol0, count ) )
	    { LLDEB(delCol0,count); continue;	}

	docEditDeleteItems( eo, &sectionsDeleted,
					&firstParaDeleted, &paragraphsDeleted,
					rowBi, delCol0, count );

	docEditIncludeItemInReformatRange( eo, rowBi );
	}

    docSectDelimitTables( sectBi );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the cell spans (rowspan/colspan) of a table cell.		*/
/*									*/
/*  0)  Paranoia checks.						*/
/*  1)  The current position is not in a cells span?			*/
/*  2)  Check that no other merged cells start in the fresh rectangle.	*/
/*  3)  Shrink wider colspans.						*/
/*  4)  Shrink higher rowspans.						*/
/*  5)  Extend narrower colspans.					*/
/*  6)  Extend higher rowspans.						*/
/*  9)  Tell caller about the last row that we changed.			*/
/*									*/
/************************************************************************/

int docChangeCellSpans(		int *			pRowChanged1,
				BufferItem *		sectBi,
				int			row0,
				int			col0,
				int			rowspan,
				int			colspan )
    {
    int			rowChanged1= row0- 1;

    BufferItem *	rowBi;

    int			row;
    int			col;
    int			rowp;
    int			colp;

    CellProperties *	cp;

    rowBi= sectBi->biChildren[row0];
    rowp= rowBi->biRowTablePast;
    colp= rowBi->biRowCellCount;

    /*  0  */
    if  ( row0+ rowspan > rowBi->biRowTablePast )
	{ LLLDEB(row0,rowspan,rowBi->biRowTablePast); return -1;	}
    if  ( col0+ colspan > colp )
	{ LLLDEB(col0,colspan,colp); return -1;	}

    /*  1  */
    cp= rowBi->biRowCells+ col0;
    if  ( cp->cpMergedWithLeft || cp->cpMergedWithAbove )
	{ LLDEB(cp->cpMergedWithLeft,cp->cpMergedWithAbove); return -1;	}

    /*  2  */
    for ( row= row0+ 1; row < row0+ rowspan; row++ )
	{
	rowBi= sectBi->biChildren[row];
	cp= rowBi->biRowCells+ col0;

	for ( col= col0; col < col0+ colspan; cp++, col++ )
	    {
	    if  ( cp->cpTopInMergedColumn )
		{ LDEB(cp->cpTopInMergedColumn); return -1;	}
	    }
	}

    /*  2  */
    for ( row= row0; row < row0+ rowspan; row++ )
	{
	rowBi= sectBi->biChildren[row];
	cp= rowBi->biRowCells+ col0+ 1;

	for ( col= col0+ 1; col < col0+ colspan; cp++, col++ )
	    {
	    if  ( cp->cpLeftInMergedRow )
		{ LDEB(cp->cpLeftInMergedRow); return -1;	}
	    }
	}

    /*  3  */
    for ( row= row0; row < rowp; row++ )
	{
	int			rowChanged= 0;

	rowBi= sectBi->biChildren[row];
	cp= rowBi->biRowCells+ col0;
	if  ( ! cp->cpLeftInMergedRow )
	    { continue;	}

	if  ( colspan == 1 )
	    {
	    cp->cpLeftInMergedRow= 0;
	    rowChanged= 1;
	    }

	cp= rowBi->biRowCells+ col0+ 1;
	for ( col= col0+ 1; col < colp; cp++, col++ )
	    {
	    if  ( ! cp->cpMergedWithLeft )
		{ break;	}

	    if  ( col- col0 >= colspan && cp->cpMergedWithLeft )
		{ cp->cpMergedWithLeft= 0; rowChanged= 1;	}
	    }

	if  ( rowChanged && rowChanged1 < row )
	    { rowChanged1= row;	}
	}

    /*  4  */
    for ( col= col0; col < colp; col++ )
	{
	rowBi= sectBi->biChildren[row0];
	cp= rowBi->biRowCells+ col;
	if  ( ! cp->cpTopInMergedColumn )
	    { continue;	}

	if  ( rowspan == 1 )
	    {
	    cp->cpTopInMergedColumn= 0;
	    if  ( rowChanged1 < row0 )
		{ rowChanged1=  row0;	}
	    }

	for ( row= row0+ 1; row < rowp; row++ )
	    {
	    int		rowChanged= 0;

	    rowBi= sectBi->biChildren[row];
	    cp= rowBi->biRowCells+ col;

	    if  ( ! cp->cpMergedWithAbove )
		{ break;	}

	    if  ( col- col0 >= colspan && cp->cpMergedWithAbove )
		{ cp->cpMergedWithAbove= 0; rowChanged= 1;	}

	    if  ( rowChanged && rowChanged1 < row )
		{ rowChanged1= row;	}
	    }
	}

    /*  5  */
    for ( row= row0; row < row0+ rowspan; row++ )
	{
	int			rowChanged= 0;

	rowBi= sectBi->biChildren[row];
	cp= rowBi->biRowCells+ col0;

	if  ( colspan > 1 && ! cp->cpLeftInMergedRow )
	    {
	    cp->cpLeftInMergedRow= 1;
	    rowChanged= 1;
	    }

	cp= rowBi->biRowCells+ col0+ 1;
	for ( col= col0+ 1; col < col0+ colspan; cp++, col++ )
	    {
	    if  ( ! cp->cpMergedWithLeft )
		{ cp->cpMergedWithLeft= 1; rowChanged= 1; }
	    }

	if  ( rowChanged && rowChanged1 < row )
	    { rowChanged1= row;	}
	}

    /*  6  */
    for ( col= col0; col < col0+ colspan; col++ )
	{
	rowBi= sectBi->biChildren[row0];
	cp= rowBi->biRowCells+ col;

	if  ( rowspan > 1 && ! cp->cpTopInMergedColumn )
	    {
	    cp->cpTopInMergedColumn= 1;
	    if  ( rowChanged1 < row0 )
		{ rowChanged1=  row0;	}
	    }

	for ( row= row0+ 1; row < row0+ rowspan; row++ )
	    {
	    rowBi= sectBi->biChildren[row];
	    cp= rowBi->biRowCells+ col;

	    if  ( ! cp->cpMergedWithAbove )
		{
		cp->cpMergedWithAbove= 1;

		if  ( rowChanged1 < row )
		    { rowChanged1=  row;	}
		}
	    }
	}

    /*  9  */
    if  ( *pRowChanged1 < rowChanged1 )
	{ *pRowChanged1=  rowChanged1;	}

    return 0;
    }

