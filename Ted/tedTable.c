/************************************************************************/
/*									*/
/*  Ted: Table manipulation.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedEdit.h"
#   include	"docLayout.h"

#   include	<appDebugon.h>


/************************************************************************/
/*									*/
/*  Table related menu option callbacks.				*/
/*									*/
/************************************************************************/

static void tedDocTableSelectTableRectangle(	EditDocument *		ed,
						const TableRectangle *	tr )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		dsNew;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    BufferItem *		selSectBi;

    int				scrolledX= 0;
    int				scrolledY= 0;

    const int			lastLine= 0;

    if  ( tedGetSelection( &dsNew, &sg, &sd, td  ) )
	{ LDEB(1); return;	}

    if  ( docTableRectangleSelection( &dsNew, &selSectBi, bd, tr ) )
	{ LDEB(1); return;	}

    tedSetSelection( ed, &dsNew, lastLine, &scrolledX, &scrolledY );

    tedAdaptToolsToSelection( ed );

    return;
    }

APP_MENU_CALLBACK_H( tedDocTableSelectTable, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    TableRectangle		tr;

    if  ( tedGetSelection( &ds, &sg, &sd, td  ) )
	{ LDEB(1); return;	}

    if  ( docGetTableRectangle( &tr, &ds ) )
	{ LDEB(1); return;	}

    docExpandTableRectangleToWholeTable( &tr );

    tedDocTableSelectTableRectangle( ed, &tr );

    return;
    }

APP_MENU_CALLBACK_H( tedDocTableSelectRow, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    TableRectangle	tr;

    if  ( tedGetSelection( &ds, &sg, &sd, td  ) )
	{ LDEB(1); return;	}

    if  ( docGetTableRectangle( &tr, &ds ) )
	{ LDEB(1); return;	}

    docExpandTableRectangleToWholeRows( &tr );

    tedDocTableSelectTableRectangle( ed, &tr );

    return;
    }

APP_MENU_CALLBACK_H( tedDocTableSelectColumn, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    TableRectangle		tr;

    if  ( tedGetSelection( &ds, &sg, &sd, td  ) )
	{ LDEB(1); return;	}

    if  ( docGetTableRectangle( &tr, &ds ) )
	{ LDEB(1); return;	}

    docExpandTableRectangleToWholeColumns( &tr );

    tedDocTableSelectTableRectangle( ed, &tr );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a table in the document.					*/
/*									*/
/*  0)  Also expose the table borders.					*/
/*  1)  If a range of data was selected, first discard it.		*/
/*  2)  If the selection was at the end of a paragraph, try to move to	*/
/*	the beginning of the next one.					*/
/*  3)  If the IBar is in the middle of a paragraph, split it.		*/
/*	tedSplitParaContents() does the layout of both halves.		*/
/*  4)  If the paragraph is not the first one in the 'cell' that	*/
/*	contains it, split the cell.					*/
/*  5)  If the 'cell' is not the first one in the 'row' that contains	*/
/*	it, split the row.						*/
/*									*/
/************************************************************************/

int tedInsertTable(		EditDocument *		ed,
				int			rows,
				int			columns )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);

    BufferItem *		newBi;
    BufferItem *		sectBi;
    BufferItem *		refRowBi;
    int				col;
    int				paraNr;

    int				textAttributeNumber;

    int				wide;

    RowProperties		rp;

    EditOperation		eo;
    DocumentSelection		dsOld;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    BufferItem *		bi;

    DocumentSelection		dsRep;

    tedStartEditOperation( &eo, &dsOld, &sg, &sd, ed, 1 );

    /*  0  */
    eo.eoChangedRect.drY0--;

    /*  1  */
    if  ( tedEditReplaceSelection( &eo, &dsRep, ed,
					    (const unsigned char *)0, 0 ) )
	{ LDEB(1); return -1;	}


    /*  2  */
    bi= dsRep.dsBegin.dpBi;
    if  ( dsRep.dsBegin.dpStroff > 0			&&
	  dsRep.dsBegin.dpStroff == bi->biParaStrlen	)
	{
	DocumentPosition	dpNext;

	dpNext= dsRep.dsBegin;
	docNextPosition( &dpNext );

	if  ( docPositionsInsideCell( &dpNext, &(dsRep.dsBegin) ) )
	    {
	    dsRep.dsBegin= dpNext;
	    bi= dsRep.dsBegin.dpBi;
	    }
	}

    /*  3  */
    if  ( dsRep.dsBegin.dpStroff != 0 )
	{
	int		onNewPage= 0;

	if  ( tedSplitParaContents( &eo, &newBi, &(dsRep.dsBegin),
						ed, DOClevROW, onNewPage ) )
	    { LDEB(1); return -1;	}

	bi= newBi;
	}
    else{
	BufferItem *		aftBi;

	/* 4,5 */
	if  ( bi->biNumberInParent > 0			||
	      bi->biParent->biNumberInParent > 0	)
	    {
	    if  ( docSplitGroupItem( bd, &newBi, &aftBi, bi->biParent,
					bi->biNumberInParent, DOClevROW ) )
		{ LDEB(bi->biNumberInParent); return -1;	}

	    if  ( aftBi && aftBi->biParent )
		{
		docEditIncludeItemInReformatRange( &eo, aftBi->biParent );
		}
	    else{ XDEB(aftBi);	}
	    }
	}

    textAttributeNumber= bi->biParaParticules[0].tpTextAttributeNumber;

    wide=   dp->dpGeometry.dgPageWideTwips;
    wide -= dp->dpGeometry.dgLeftMarginTwips;
    wide -= dp->dpGeometry.dgRightMarginTwips;

    docInitRowProperties( &rp );

    {
    TextAttribute	ta;

    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
							textAttributeNumber );

    rp.rpHalfGapWidthTwips= 5* ta.taFontSizeHalfPoints;
    rp.rpLeftIndentTwips=  -5* ta.taFontSizeHalfPoints;
    }

    for ( col= 0; col < columns; col++ )
	{
	CellProperties		cp;
	const int * const	colorMap= (const int *)0;

	docInitCellProperties( &cp );
	cp.cpRightBoundaryTwips= ( ( col+ 1 )* wide )/ columns;

	if  ( docInsertRowColumn( &rp, col, &cp, colorMap ) )
	    { LDEB(col); return -1;	}

	rp.rpHasTableParagraphs= 1;

	docCleanCellProperties( &cp );
	}

    newBi= (BufferItem *)0;
    refRowBi= (BufferItem *)0;
    sectBi= bi->biParent->biParent->biParent;
    paraNr= docNumberOfParagraph( bi );

    if  ( docInsertTableRows( sectBi, &newBi, refRowBi,
			    &rp, textAttributeNumber,
			    bi->biParent->biParent->biNumberInParent, paraNr,
			    rows, &eo ) )
	{ LDEB(rows); return -1;	}

    docCleanRowProperties( &rp );

    docSectDelimitTables( sectBi );

    tedEditFinishIBarSelection( ed, &eo, newBi, 0 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a number of rows in a table.					*/
/*									*/
/*  1)  Check that the section is not empty. Ted avoids empty items at	*/
/*	any price. Every Item is supposed to contain at least one child	*/
/*	or at least one particule.					*/
/*  2)  Sanity check.							*/
/*  4)  Section related administration.					*/
/*  5)  Set an I-Bar selection of the first position in the first	*/
/*	(empty) cell of the first fresh row.				*/
/*									*/
/************************************************************************/

static int tedInsertRowsInTable(
			BufferItem *			sectBi,
			EditOperation *			eo,
			int				paraNr,
			int				textAttributeNumber,
			BufferItem *			refRowBi,
			EditDocument *			ed,
			int				row,
			int				rows )
    {
    BufferItem *		newBi;
    const RowProperties *	rp;

    /*  1  */
    if  ( sectBi->biChildCount == 0 )
	{ LDEB(sectBi->biChildCount); return -1;	}

    /*  2  */
    if  ( refRowBi->biLevel != DOClevROW	||
	  ! refRowBi->biRowHasTableParagraphs	)
	{
	SDEB(docLevelStr(refRowBi->biLevel));
	LDEB(refRowBi->biRowHasTableParagraphs);
	return -1;
	}

    newBi= (BufferItem *)0;

    rp= &(refRowBi->biRowProperties);
    if  ( docInsertTableRows( sectBi, &newBi, refRowBi, rp,
						textAttributeNumber,
						row, paraNr, rows, eo ) )
	{ LDEB(rows); return -1;	}

    /*  4  */
    docSectDelimitTables( sectBi );

    /*  5  */
    tedEditFinishIBarSelection( ed, eo, newBi, 0 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a row before the current row in a table.			*/
/*									*/
/*  1)  Start edit operation.						*/
/*  2)  Find the position of the beginning of the selection in the	*/
/*	table.								*/
/*  3)  Get the row that serves as a template.				*/
/*  4)  Paragraph number of the paragraph that comes directly below	*/
/*	the fresh rows.							*/
/*  5)  Finish..							*/
/*									*/
/************************************************************************/

int tedInsertRowInTable(	EditDocument *		ed )
    {
    BufferItem *		sectBi;
    BufferItem *		refRowBi;

    int				col;
    int				row;
    int				row0;
    int				row1;

    const int			rows= 1;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpRef;
    int				part;
    const int			lastOne= 1;
    int				paraNr;
    int				textAttributeNumber;

    /*  1  */
    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    /*  2  */
    if  ( docDelimitTable( ds.dsBegin.dpBi, &sectBi,
						&col, &row0, &row, &row1 ) )
	{ LDEB(1); return -1;	}

    /*  3  */
    refRowBi= sectBi->biChildren[row];
    if  ( docFirstPosition( &dpRef, refRowBi ) )
	{ LDEB(row); return -1;	}
    if  ( docFindParticuleOfPosition( &part, &dpRef, lastOne ) )
	{ LDEB(dpRef.dpStroff); return -1;	}

    /*  4  */
    paraNr= docNumberOfParagraph( dpRef.dpBi );
    textAttributeNumber= dpRef.dpBi->biParaParticules[part].
						    tpTextAttributeNumber;

    /*  5  */
    if  ( tedInsertRowsInTable( sectBi, &eo,
		    paraNr, textAttributeNumber, refRowBi, ed, row, rows ) )
	{ LDEB(row); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a row after the current row in a table.			*/
/*									*/
/*  1)  Start edit operation.						*/
/*  2)  Find the position of the end of the selection in the table	*/
/*  3)  Get the row that serves as a template.				*/
/*  4)  Paragraph number of the paragraph that comes directly below	*/
/*	the fresh rows.							*/
/*  5)  Finish..							*/
/*									*/
/************************************************************************/

int tedAppendRowToTable(	EditDocument *		ed )
    {
    BufferItem *		sectBi;
    BufferItem *		refRowBi;

    int				col;
    int				row;
    int				row0;
    int				row1;

    const int			rows= 1;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpRef;
    int				part;
    const int			lastOne= 0;
    int				paraNr;
    int				textAttributeNumber;

    /*  1  */
    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    /*  2  */
    if  ( docDelimitTable( ds.dsEnd.dpBi, &sectBi,
						&col, &row0, &row, &row1 ) )
	{ LDEB(1); return -1;	}

    /*  3  */
    refRowBi= sectBi->biChildren[row];
    if  ( docLastPosition( &dpRef, refRowBi ) )
	{ LDEB(row); return -1;	}
    if  ( docFindParticuleOfPosition( &part, &dpRef, lastOne ) )
	{ LDEB(dpRef.dpStroff); return -1;	}

    /*  4  */
    paraNr= docNumberOfParagraph( dpRef.dpBi )+ 1;
    textAttributeNumber= dpRef.dpBi->biParaParticules[part].
							tpTextAttributeNumber;

    /*  5  */
    if  ( tedInsertRowsInTable( sectBi, &eo,
		    paraNr, textAttributeNumber, refRowBi, ed, row+ 1, rows ) )
	{ LDEB(row); return -1;	}
    
    return 0;
    }

/************************************************************************/
/*									*/
/*  Delete rows from a table.						*/
/*									*/
/*  4)  Sanity check.							*/
/*									*/
/************************************************************************/

static int tedDeleteRowsFromTableOperation(
					EditDocument *		ed,
					EditOperation *		eo,
					DocumentSelection *	ds,
					int			delRow0,
					int			delRow1 )
    {
    int				rval= 0;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    BufferItem *		selSectBi;

    int				col;
    int				row;
    int				row0;
    int				row1;

    DocumentPosition		dpNew;

    int				paraNr;
    int				sectionsDeleted= 0;
    int				firstParaDeleted= -1;
    int				paragraphsDeleted= 0;

    PropertyMask		ppDoneMask;
    PropertyMask		ppSetMask;
    const int * const		colorMap= (const int *)0;
    const int * const		listStyleMap= (const int *)0;

    ParagraphProperties		pp;

    docInitParagraphProperties( &pp );

    if  ( docDelimitTable( ds->dsBegin.dpBi, &selSectBi,
						&col, &row0, &row, &row1 ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( delRow0 > delRow1 )
	{ LLDEB(delRow0,delRow1); rval= -1; goto ready;	}
    if  ( delRow0 < row0 || delRow0 > row1 )
	{ LLLDEB(row0,delRow0,row1); rval= -1; goto ready;	}
    if  ( delRow1 < row0 || delRow1 > row1 )
	{ LLLDEB(row0,delRow1,row1); rval= -1; goto ready;	}

    docInitDocumentPosition( &dpNew );
    if  ( docLastPosition( &dpNew, selSectBi->biChildren[delRow1] ) )
	{ LLDEB(delRow0,delRow1); rval= -1; goto ready;	}

    paraNr= docNumberOfParagraph( dpNew.dpBi )+ 1;

    PROPmaskCLEAR( &ppDoneMask );
    PROPmaskCLEAR( &ppSetMask );
    utilPropMaskFill( &ppSetMask, PPprop_COUNT );
    PROPmaskUNSET( &ppSetMask, PPpropIN_TABLE );

    if  ( docUpdParaProperties( &ppDoneMask, &pp,
				&ppSetMask, &(dpNew.dpBi->biParaProperties),
				colorMap, listStyleMap ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docInitDocumentPosition( &dpNew );
    if  ( docLastPosition( &dpNew, selSectBi->biChildren[delRow1] )	||
	  docNextPosition( &dpNew )					)
	{
	docInitDocumentPosition( &dpNew );
	if  ( docFirstPosition( &dpNew,
				selSectBi->biChildren[delRow0] )	||
	      docPrevPosition( &dpNew )					)
	    {
	    docInitDocumentPosition( &dpNew );
	    }
	}

    tedEditIncludeRowsInRedraw( eo, ed, selSectBi, delRow0, delRow1 );

    docEditDeleteItems( eo, &sectionsDeleted,
				&firstParaDeleted, &paragraphsDeleted,
				selSectBi, delRow0, delRow1- delRow0+ 1 );

    docEditIncludeItemInReformatRange( eo, selSectBi );

    if  ( ! dpNew.dpBi )
	{
	const int	sectShift= 0;

	if  ( tedSectionParagraph( eo, &(dpNew.dpBi),
				    selSectBi, sectShift,
				    &pp, td->tdCurrentTextAttributeNumber ) )
	    { LDEB(1); rval= -1; goto ready;	}

	docEditIncludeItemInReformatRange( eo, selSectBi );

	if  ( docFirstPosition( &dpNew, dpNew.dpBi ) )
	    { LDEB(1);	}
	}

    docSectDelimitTables( selSectBi );

    tedEditFinishIBarSelection( ed, eo, dpNew.dpBi, dpNew.dpStroff );

  ready:

    docCleanParagraphProperties( &pp );

    return rval;
    }

int tedDeleteRowsFromTable(	EditDocument *		ed,
				int			delRow0,
				int			delRow1 )
    {
    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    if  ( tedDeleteRowsFromTableOperation( ed, &eo, &ds, delRow0, delRow1 ) )
	{ LDEB(1); return -1;	}

    appDocumentChanged( ed, 1 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert cells/columns in tables.					*/
/*									*/
/************************************************************************/

static int tedSplitColumnInRows(	EditDocument *	ed,
					EditOperation *	eo,
					BufferItem *	sectBi,
					int		row0,
					int		row,
					int		row1,
					int		col,
					int		after )
    {
    BufferItem *		newParaBi= (BufferItem *)0;

    tedEditIncludeRowsInRedraw( eo, ed, sectBi, row0, row1 );

    if  ( docSplitColumnInRows( &newParaBi, eo,
				    sectBi, row0, row, row1, col, after ) )
	{ LDEB(1); return -1;	}

    docSectDelimitTables( sectBi );

    tedEditFinishIBarSelection( ed, eo, newParaBi, 0 );

    return 0;
    }

int tedInsertColumnInTable(	EditDocument *		ed )
    {
    BufferItem *		sectBi;

    int				col;
    int				row;
    int				row0;
    int				row1;

    int				after= 0;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    if  ( docDelimitTable( ds.dsBegin.dpBi, &sectBi,
					    &col, &row0, &row, &row1 ) )
	{ LDEB(1); return -1;	}

    return tedSplitColumnInRows( ed, &eo, sectBi, row0, row, row1, col, after );
    }

int tedAppendColumnToTable(	EditDocument *		ed )
    {
    BufferItem *		sectBi;

    int				col;
    int				row;
    int				row0;
    int				row1;

    int				after= 1;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    if  ( docDelimitTable( ds.dsBegin.dpBi, &sectBi,
					    &col, &row0, &row, &row1 ) )
	{ LDEB(1); return -1;	}

    return tedSplitColumnInRows( ed, &eo, sectBi, row0, row, row1, col, after );
    }

static int tedDeleteColumnsFromRowsOperation(
					EditDocument *		ed,
					EditOperation *		eo,
					DocumentSelection *	ds,
					int			delRow0,
					int			delRow1,
					int			delCol0,
					int			delCol1 )
    {
    BufferItem *		sectBi;
    BufferItem *		rowBi;

    int				col;
    int				row;
    int				row0;
    int				row1;

    DocumentPosition		dpNew;

    if  ( docDelimitTable( ds->dsBegin.dpBi, &sectBi,
					    &col, &row0, &row, &row1 ) )
	{ LDEB(1); return -1;	}

    tedEditIncludeRowsInRedraw( eo, ed, sectBi, delRow0, delRow1 );

    docInitDocumentPosition( &dpNew );
    rowBi= sectBi->biChildren[delRow1];
    if  ( delCol1 >= rowBi->biChildCount- 1			||
	  docLastPosition( &dpNew, rowBi->biChildren[delCol1] )	||
	  docNextPosition( &dpNew )				)
	{
	docInitDocumentPosition( &dpNew );
	rowBi= sectBi->biChildren[delRow0];
	if  ( delCol0 <= 0						||
	      docFirstPosition( &dpNew, rowBi->biChildren[delCol0] )	||
	      docPrevPosition( &dpNew )					)
	    { docInitDocumentPosition( &dpNew ); }
	}

    if  ( docDeleteColumnsFromRows( eo, sectBi,
				    delRow0, delRow1, delCol0, delCol1 ) )
	{ LDEB(1); return -1;	}

    tedEditFinishIBarSelection( ed, eo, dpNew.dpBi, dpNew.dpStroff );

    return 0;
    }

int tedDeleteColumnsFromRows(	EditDocument *	ed,
				int		delRow0,
				int		delRow1,
				int		delCol0,
				int		delCol1 )
    {
    int				rval;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    rval= tedDeleteColumnsFromRowsOperation( ed, &eo, &ds,
					delRow0, delRow1, delCol0, delCol1 );

    appDocumentChanged( ed, 1 );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Implementation of the 'Change' functionality of the table related	*/
/*  pages of the format tool.						*/
/*									*/
/*  1)  Retrieve the current selection of the document.			*/
/*  2)  Obtain a selection for the table rectangle.			*/
/*  3)  Were the colspan/rowspan of a cell to be changed?		*/
/*  4)  Remove spans from mask.						*/
/*  5)  Update other properties.					*/
/*									*/
/************************************************************************/

void tedDocSetTableProperties(	EditDocument *		ed,
				const TableRectangle *	trChange,
				const PropertyMask *	rpSetMask,
				const PropertyMask *	cpSetMask,
				const RowProperties *	rpFrom )
    {
    TedDocument *		td;
    BufferDocument *		bd;

    BufferItem *		selSectBi;
    int				row1Reformat;

    DocumentSelection		dsNew;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    PropertyMask		cellSpanMask;
    PropertyMask		cpSetMaskStripped;

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;

    /*  1  */
    docInitDocumentSelection( &dsNew );
    if  ( tedGetSelection( &dsNew, &sg, &sd, td ) )
	{ LDEB(1); return;	}

    /*  2  */
    if  ( docTableRectangleSelection( &dsNew, &selSectBi, bd, trChange ) )
	{ LDEB(1); return;	}

    row1Reformat= trChange->trRow1;

    /*  3  */
    PROPmaskCLEAR( &cellSpanMask );
    PROPmaskADD( &cellSpanMask, CLprop_ROWSPAN );
    PROPmaskADD( &cellSpanMask, CLprop_COLSPAN );

    utilPropMaskAnd( &cellSpanMask, &cellSpanMask, cpSetMask );

    if  ( ! utilPropMaskIsEmpty( &cellSpanMask ) )
	{
	int		row1SpanChanged= trChange->trRow0- 1;

	if  ( docChangeCellSpans( &row1SpanChanged, selSectBi,
					    trChange->trRow0, trChange->trCol0,
					    trChange->trCellRowspan,
					    trChange->trCellColspan ) )
	    { LDEB(1); return;	}

	if  ( row1Reformat < row1SpanChanged )
	    { row1Reformat=  row1SpanChanged;	}
	}

    /*  4  */
    cpSetMaskStripped= *cpSetMask;
    PROPmaskUNSET( &cpSetMaskStripped, CLprop_ROWSPAN );
    PROPmaskUNSET( &cpSetMaskStripped, CLprop_COLSPAN );

    /*  5  */
    tedChangeTableLayout( ed, selSectBi,
				    trChange->trRow0, trChange->trRow1,
				    trChange->trCol0, trChange->trCol1,
				    row1Reformat,
				    rpSetMask, &cpSetMaskStripped, rpFrom );

    tedAdaptToolsToSelection( ed );

    if  ( ! utilPropMaskIsEmpty( rpSetMask )	||
	  ! utilPropMaskIsEmpty( cpSetMask )	)
	{ appDocumentChanged( ed, 1 );	}

    return;
    }

void tedAppSetTableProperties(	EditApplication *	ea,
				const TableRectangle *	trChange,
				const PropertyMask *	rpSetMask,
				const PropertyMask *	cpSetMask,
				const RowProperties *	rpFrom )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocSetTableProperties( ed, trChange, rpSetMask, cpSetMask, rpFrom );

    return;
    }

/************************************************************************/
/*									*/
/*  Select a rectangle in a table.					*/
/*									*/
/************************************************************************/

void tedAppSetTableSelection(	EditDocument *		ed,
				const TableRectangle *	trSet )
    {
    TedDocument *		td;

    TableRectangle		tr;

    int				tableRectangle= 0;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    td= (TedDocument *)ed->edPrivateData;

    tedGetSelection( &ds, &sg, &sd, td );

    if  ( ! docGetTableRectangle( &tr, &ds ) )
	{ tableRectangle= 1;	}

    if  ( ! tableRectangle )
	{ LDEB(tableRectangle); return;	}

    docExpandTableRectangleToWholeTable( &tr );

    if  ( ! docIntersectTableRectangle( &tr, &tr, trSet ) )
	{ return;	}

    tedDocTableSelectTableRectangle( ed, &tr );

    return;
    }

/************************************************************************/
/*									*/
/*  Change table layout from various table related tools.		*/
/*									*/
/*  1)  Reformat at least all rows that are touched.			*/
/*  2)  Start edit operation.						*/
/*  3)  Include all rows touched by the operation in the reformat.	*/
/*  4)  If the 'IsTableHeader' property is changed, the whole table	*/
/*	must be reformatted.						*/
/*  5)  Apply changes.							*/
/*  6)  Finish edit operation. I.E: Redo layout where needed and expose	*/
/*	the affected area to be redrawn.				*/
/*									*/
/************************************************************************/

void tedChangeTableLayout(	EditDocument *		ed,
				BufferItem *		sectBi,
				int			row0Change,
				int			row1Change,
				int			col0Change,
				int			col1Change,
				int			row1Reformat,
				const PropertyMask *	rpSetMask,
				const PropertyMask *	cpSetMask,
				const RowProperties *	rpSet )
    {
    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    /*  1  */
    if  ( row1Reformat < row1Change )
	{ row1Reformat=  row1Change;	}

    /*  2  */
    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    /*  3  */
    docEditIncludeRowsInReformatRange( &eo,
				    sectBi, row0Change, row1Reformat );
    tedEditIncludeRowsInRedraw( &eo, ed,
				    sectBi, row0Change, row1Reformat );

    /*  4  */
    if  ( PROPmaskISSET( rpSetMask, RPpropIS_TABLE_HEADER ) )
	{
	const BufferItem *	rowBi= sectBi->biChildren[row0Change];

	if  ( row0Change != row1Change )
	    { LLDEB(row0Change,row1Change);	}

	docEditIncludeRowsInReformatRange( &eo,
				sectBi, row0Change, rowBi->biRowTablePast- 1 );
	tedEditIncludeRowsInRedraw( &eo, ed,
				sectBi, row0Change, rowBi->biRowTablePast- 1 );
	}

    /*  5  */
    docChangeTableLayout( &eo, sectBi,
			    row0Change, row1Change, col0Change, col1Change,
			    rpSetMask, cpSetMask, rpSet );

    /*  6  */
    if  ( tedEditFinishOldSelection( ed, &eo ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Callback for the 'DrawTableGrid' toggle.				*/
/*									*/
/************************************************************************/

void tedDocTableDrawGrid(	APP_WIDGET	option,
				void *		voided,
				void *		voidcbs )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    int				set;

    set= appGuiGetMenuToggleStateFromCallback( option, voidcbs );

    if  ( set )
	{ td->tdDrawTableGrid=  1;	}
    else{ td->tdDrawTableGrid= -1;	}

    appDocExposeRectangle( ed, (const DocumentRectangle *)0, 0,0 );

    return;
    }

/************************************************************************/
/*									*/
/*  Delete a slice from a table. Either horizontal or vertical or both:	*/
/*  the table as a whole.						*/
/*									*/
/*  1)  See whether the ccurrent selection is a table slice.		*/
/*  2)  If it is a row slice (Whole rows) delete the rows. This covers	*/
/*	deleteing the whole table as well. [ Handling it in the column	*/
/*	branch would leave empty rows.]					*/
/*  3)  If it is a column slice (Whole columns) delete the columns.	*/
/*  4)  Impossible!							*/
/*									*/
/************************************************************************/

int tedDeleteTableSliceSelection(	EditDocument *		ed )
    {
    int				isRowSlice= 0;
    int				isColSlice= 0;
    TableRectangle		tr;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    int				ret;

    if  ( ed->edIsReadonly )
	{ return 1;	}

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    /*  1  */
    if  ( docGetTableSliceSelection( &isRowSlice, &isColSlice, &tr, &ds ) )
	{ return 1;	}

    /*  2  */
    if  ( isRowSlice )
	{
	ret= tedDeleteRowsFromTableOperation( ed, &eo, &ds,
							tr.trRow0, tr.trRow1 );
	if  ( ret )
	    { LDEB(ret); return -1;	}

	appDocumentChanged( ed, 1 );
	return 0;
	}

    /*  3  */
    if  ( isColSlice )
	{
	ret= tedDeleteColumnsFromRowsOperation( ed, &eo, &ds,
				tr.trRow0, tr.trRow1, tr.trCol0, tr.trCol1 );
	if  ( ret )
	    { LDEB(ret); return -1;	}

	appDocumentChanged( ed, 1 );
	return 0;
	}

    /*  4  */
    LLDEB(isRowSlice,isColSlice); return -1;
    }

APP_MENU_CALLBACK_H( tedDocTableDeleteTable, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    TableRectangle		tr;

    if  ( tedGetSelection( &ds, &sg, &sd, td  ) )
	{ LDEB(1); return;	}

    if  ( docGetTableRectangle( &tr, &ds ) )
	{ LDEB(1); return;	}

    if  ( tedDeleteRowsFromTable( ed, tr.trRow00, tr.trRow11 ) )
	{ LLDEB(tr.trRow00,tr.trRow11); return;	}

    return;
    }

APP_MENU_CALLBACK_H( tedDocTableDeleteRow, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    TableRectangle		tr;

    if  ( tedGetSelection( &ds, &sg, &sd, td  ) )
	{ LDEB(1); return;	}

    if  ( docGetTableRectangle( &tr, &ds ) )
	{ LDEB(1); return;	}

    if  ( tedDeleteRowsFromTable( ed, tr.trRow0, tr.trRow1 ) )
	{ LLDEB(tr.trRow00,tr.trRow11); return;	}

    return;
    }

APP_MENU_CALLBACK_H( tedDocTableDeleteColumn, option, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    TableRectangle		tr;

    if  ( tedGetSelection( &ds, &sg, &sd, td  ) )
	{ LDEB(1); return;	}

    if  ( docGetTableRectangle( &tr, &ds ) )
	{ LDEB(1); return;	}

    if  ( tedDeleteColumnsFromRows( ed,
		tr.trRow00, tr.trRow11, tr.trCol0, tr.trCol1 ) )
	{ LLDEB(tr.trRow00,tr.trRow11); return;	}

    return;
    }

