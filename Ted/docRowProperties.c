/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>
#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Initialise a BufferItem.						*/
/*									*/
/************************************************************************/

void docCleanInitRowProperties(	RowProperties *	rp )
    {
    docCleanRowProperties( rp );
    docInitRowProperties( rp );
    }

void docCleanRowProperties(	RowProperties *	rp )
    {
    if  ( rp->rpCells )
	{ free( rp->rpCells );	}
    }

void docInitRowProperties(	RowProperties *	rp )
    {
    rp->rpCellCount= 0;
    rp->rpCells= (CellProperties *)0;

    rp->rpHalfGapWidthTwips= 0;
    rp->rpHalfGapWidthPixels= 0;

    rp->rpHeightTwips= 0;

    rp->rpLeftIndentTwips= 0;
    rp->rpLeftIndentPixels= 0;

    rp->rpHasTableParagraphs= 0;

    rp->rpIsTableHeader= 0;
    rp->rpKeepOnOnePage= 0;
    rp->rpKeepWithNext= 0;
    rp->rpAutofit= 0;

    docInitBorderProperties( &(rp->rpTopBorder) );
    docInitBorderProperties( &(rp->rpBottomBorder) );
    docInitBorderProperties( &(rp->rpLeftBorder) );
    docInitBorderProperties( &(rp->rpRightBorder) );
    docInitBorderProperties( &(rp->rpHorizontalBorder) );
    docInitBorderProperties( &(rp->rpVerticalBorder) );

    docInitItemShading( &(rp->rpShading) );

    rp->rpAlignment= DOCiaLEFT;

    /**/
    rp->rpPreferredWidth= 0;
    rp->rpPreferredWidthUnit= TRautoNONE;

    /**/
    rp->rpLeftDefaultCellSpacing= 0;
    rp->rpRightDefaultCellSpacing= 0;
    rp->rpTopDefaultCellSpacing= 0;
    rp->rpBottomDefaultCellSpacing= 0;

    rp->rpLeftDefaultCellSpacingUnit= TRautoNONE;
    rp->rpRightDefaultCellSpacingUnit= TRautoNONE;
    rp->rpTopDefaultCellSpacingUnit= TRautoNONE;
    rp->rpBottomDefaultCellSpacingUnit= TRautoNONE;

    /**/
    rp->rpLeftCellPadding= 0;
    rp->rpRightCellPadding= 0;
    rp->rpTopCellPadding= 0;
    rp->rpBottomCellPadding= 0;

    rp->rpLeftCellPaddingUnit= TRautoNONE;
    rp->rpRightCellPaddingUnit= TRautoNONE;
    rp->rpTopCellPaddingUnit= TRautoNONE;
    rp->rpBottomCellPaddingUnit= TRautoNONE;

    /**/
    rp->rpCellWidthBefore= 0;
    rp->rpCellWidthAfter= 0;

    rp->rpCellWidthBeforeUnit= TRautoNONE;
    rp->rpCellWidthAfterUnit= TRautoNONE;

    /**/
    rp->rpRowNumber= -1;
    rp->rpRowBandNumber= -1;
    rp->rpRowStyle= -1;

    rp->rpIsLastRow= 0;
    rp->rpAuthor= -1;

    return;
    }

int docInsertRowColumn(	RowProperties *			rp,
			int				n,
			const CellProperties *		cp,
			const int *			colorMap )
    {
    CellProperties *	fresh;

    fresh= (CellProperties *)realloc( rp->rpCells,
			(rp->rpCellCount+ 1)* sizeof(CellProperties) );
    if  ( ! fresh )
	{ LXDEB(rp->rpCellCount,fresh); return -1;	}
    rp->rpCells= fresh;

    if  ( n < 0 )
	{ n= rp->rpCellCount;	}
    else{
	int		i;

	for ( i= rp->rpCellCount; i > n; i-- )
	    { docCopyCellProperties( fresh+ i, fresh+ i- 1, colorMap );	}
	}

    docInitCellProperties( fresh+ n );
    docCopyCellProperties( fresh+ n, cp, colorMap );

    rp->rpCellCount++;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Delete a number of columns from row properties.			*/
/*									*/
/************************************************************************/

int docDeleteColumnsFromRow(	RowProperties *		rp,
				int			col0,
				int			count )
    {
    int		col1= col0+ count- 1;
    int		left;
    int		wide;
    int		col;

    if  ( col0+ count > rp->rpCellCount )
	{ LLLDEB(col0,count,rp->rpCellCount); return -1;	}

    if  ( col0 == 0 )
	{ left= rp->rpLeftIndentTwips;				}
    else{ left= rp->rpCells[col0-1].cpRightBoundaryTwips;	}

    wide= rp->rpCells[col1].cpRightBoundaryTwips- left;

    rp->rpCellCount -= count;

    for ( col= col0; col < rp->rpCellCount; col++ )
	{
	docCleanCellProperties( &(rp->rpCells[col]) );
	rp->rpCells[col]= rp->rpCells[col+ count];
	rp->rpCells[col].cpRightBoundaryTwips -= wide;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Copy row properties. As this routine is not called very often,	*/
/*  just call the 'update' routine.					*/
/*									*/
/************************************************************************/

int docCopyRowProperties(	RowProperties *		rpTo,
				const RowProperties *	rpFrom,
				const int *		colorMap )
    {
    PropertyMask		rpDoneMask;
    PropertyMask		rpSetMask;

    PROPmaskCLEAR( &rpDoneMask );
    PROPmaskCLEAR( &rpSetMask );

    utilPropMaskFill( &rpSetMask, RPprop_COUNT );

    if  ( docUpdRowProperties( &rpDoneMask, rpTo,
					    &rpSetMask, rpFrom, colorMap ) )
	{ LDEB(1); return -1;	}

    rpTo->rpHasTableParagraphs= rpFrom->rpHasTableParagraphs;

    return 0;
    }

/************************************************************************/
/*									*/
/*  1)  Are the columns in two RowProperties 'the same' (Do they	*/
/*	align?)								*/
/*  2)  All internal borders equal?					*/
/*  3)  All column properties identical?				*/
/*									*/
/************************************************************************/

/*  1  */
int docApproximatelyAlignedColumns(	const RowProperties *	rp1,
					const RowProperties *	rp2 )
    {
    CellProperties *	cp1;
    CellProperties *	cp2;
    int			i;

    const int		D= 40;

    if  ( rp1->rpHasTableParagraphs != rp2->rpHasTableParagraphs )
	{ return 0;	}

    if  ( rp1->rpCellCount != rp2->rpCellCount )
	{ return 0;	}

    /* No!
    if  ( rp1->rpHalfGapWidthTwips != rp2->rpHalfGapWidthTwips )
	{ return 0;	}
    */

    if  ( rp1->rpLeftIndentTwips > rp2->rpLeftIndentTwips+ D	||
	  rp1->rpLeftIndentTwips < rp2->rpLeftIndentTwips- D	)
	{ return 0;	}

    cp1= rp1->rpCells;
    cp2= rp2->rpCells;
    for ( i= 0; i < rp1->rpCellCount; cp2++, cp1++, i++ )
	{
	if  ( cp1->cpRightBoundaryTwips > cp2->cpRightBoundaryTwips+ D	||
	      cp1->cpRightBoundaryTwips < cp2->cpRightBoundaryTwips- D	)
	    { return 0;	}
	}

    return 1;
    }

/*  2  */
int docEqualRowBorders(	const RowProperties *	rp1,
			const RowProperties *	rp2 )
    {
    PropertyMask	rpChgMask;
    PropertyMask	rpUpdMask;

    const int * const	colorMap= (const int *)0;

    PROPmaskCLEAR( &rpUpdMask );
    PROPmaskADD( &rpUpdMask, RPpropTOP_BORDER );
    PROPmaskADD( &rpUpdMask, RPpropBOTTOM_BORDER );
    PROPmaskADD( &rpUpdMask, RPpropLEFT_BORDER );
    PROPmaskADD( &rpUpdMask, RPpropRIGHT_BORDER );
    PROPmaskADD( &rpUpdMask, RPpropHORIZ_BORDER );
    PROPmaskADD( &rpUpdMask, RPpropVERT_BORDER );
    PROPmaskCLEAR( &rpChgMask );

    docRowPropertyDifference( &rpChgMask, rp1, rp2, &rpUpdMask, colorMap );

    if  ( ! utilPropMaskIsEmpty( &rpChgMask ) )
	{ return 0;	}

    return 1;
    }

/*  3  */
int docEqualRows(	const RowProperties *	rp1,
			const RowProperties *	rp2 )
    {
    PropertyMask	rpChgMask;
    PropertyMask	rpUpdMask;

    const int * const	colorMap= (const int *)0;

    utilPropMaskFill( &rpUpdMask, RPprop_COUNT );
    PROPmaskCLEAR( &rpChgMask );

    docRowPropertyDifference( &rpChgMask, rp1, rp2, &rpUpdMask, colorMap );

    if  ( ! utilPropMaskIsEmpty( &rpChgMask ) )
	{ return 0;	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Change row properties and tell what has been changed.		*/
/*									*/
/************************************************************************/

int docUpdRowProperties(	PropertyMask *			pRpDonePask,
				RowProperties *			rpTo,
				const PropertyMask *		rpUpdMask,
				const RowProperties *		rpFrom,
				const int *			colorMap )
    {
    PropertyMask		rpDoneMask;
    PropertyMask		isUpdMask;

    PROPmaskCLEAR( &rpDoneMask );

    if  ( PROPmaskISSET( rpUpdMask, RPpropCOLUMNS ) )
	{
	int			col;
	CellProperties *	cpTo;
	const CellProperties *	cpFrom;

	while( rpTo->rpCellCount > rpFrom->rpCellCount )
	    {
	    docCleanCellProperties( &(rpTo->rpCells[rpTo->rpCellCount-1]) );
	    PROPmaskADD( &rpDoneMask, RPpropCOLUMNS );
	    rpTo->rpCellCount--;
	    }

	cpTo= rpTo->rpCells;
	cpFrom= rpFrom->rpCells;
	for ( col= 0; col < rpTo->rpCellCount; cpTo++, cpFrom++, col++ )
	    {
	    PropertyMask	cpDoneMask;
	    PropertyMask	cpUpdMask;

	    utilPropMaskFill( &cpUpdMask, CLprop_COUNT );
	    PROPmaskCLEAR( &cpDoneMask );

	    if  ( docUpdCellProperties( &cpDoneMask, cpTo,
					    &cpUpdMask, cpFrom, colorMap ) )
		{ LDEB(1); return -1;	}

	    if  ( ! utilPropMaskIsEmpty( &cpDoneMask ) )
		{ PROPmaskADD( &rpDoneMask, RPpropCOLUMNS );	}
	    }

	while( rpTo->rpCellCount < rpFrom->rpCellCount )
	    {
	    if  ( docInsertRowColumn( rpTo, rpTo->rpCellCount,
							cpFrom, colorMap ) )
		{ LDEB(rpTo->rpCellCount); return -1;	}

	    PROPmaskADD( &rpDoneMask, RPpropCOLUMNS );
	    cpFrom++;
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropGAP_WIDTH ) )
	{
	if  ( rpTo->rpHalfGapWidthTwips != rpFrom->rpHalfGapWidthTwips )
	    {
	    rpTo->rpHalfGapWidthTwips= rpFrom->rpHalfGapWidthTwips;
	    PROPmaskADD( &rpDoneMask, RPpropGAP_WIDTH );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropLEFT_INDENT ) )
	{
	if  ( rpTo->rpLeftIndentTwips != rpFrom->rpLeftIndentTwips )
	    {
	    rpTo->rpLeftIndentTwips= rpFrom->rpLeftIndentTwips;
	    PROPmaskADD( &rpDoneMask, RPpropLEFT_INDENT );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropHEIGHT ) )
	{
	if  ( rpTo->rpHeightTwips != rpFrom->rpHeightTwips )
	    {
	    rpTo->rpHeightTwips= rpFrom->rpHeightTwips;
	    PROPmaskADD( &rpDoneMask, RPpropHEIGHT );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTOP_BORDER ) )
	{
	if  ( docBordersDiffer( &(rpTo->rpTopBorder),
				    &(rpFrom->rpTopBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(rpTo->rpTopBorder),
				    &(rpFrom->rpTopBorder), colorMap );
	    PROPmaskADD( &rpDoneMask, RPpropTOP_BORDER );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropBOTTOM_BORDER ) )
	{
	if  ( docBordersDiffer( &(rpTo->rpBottomBorder),
				    &(rpFrom->rpBottomBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(rpTo->rpBottomBorder),
				    &(rpFrom->rpBottomBorder), colorMap );
	    PROPmaskADD( &rpDoneMask, RPpropBOTTOM_BORDER );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropLEFT_BORDER ) )
	{
	if  ( docBordersDiffer( &(rpTo->rpLeftBorder),
				    &(rpFrom->rpLeftBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(rpTo->rpLeftBorder),
				    &(rpFrom->rpLeftBorder), colorMap );
	    PROPmaskADD( &rpDoneMask, RPpropLEFT_BORDER );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropRIGHT_BORDER ) )
	{
	if  ( docBordersDiffer( &(rpTo->rpRightBorder),
				    &(rpFrom->rpRightBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(rpTo->rpRightBorder),
				    &(rpFrom->rpRightBorder), colorMap );
	    PROPmaskADD( &rpDoneMask, RPpropRIGHT_BORDER );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropHORIZ_BORDER ) )
	{
	if  ( docBordersDiffer( &(rpTo->rpHorizontalBorder),
				    &(rpFrom->rpHorizontalBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(rpTo->rpHorizontalBorder),
				    &(rpFrom->rpHorizontalBorder), colorMap );
	    PROPmaskADD( &rpDoneMask, RPpropHORIZ_BORDER );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropVERT_BORDER ) )
	{
	if  ( docBordersDiffer( &(rpTo->rpVerticalBorder),
				    &(rpFrom->rpVerticalBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(rpTo->rpVerticalBorder),
				    &(rpFrom->rpVerticalBorder), colorMap );
	    PROPmaskADD( &rpDoneMask, RPpropVERT_BORDER );
	    }
	}

    docShadingMaskFromRowMask( &isUpdMask, rpUpdMask );
    if  ( ! utilPropMaskIsEmpty( &isUpdMask ) )
	{
	PropertyMask	isDoneMask;

	PROPmaskCLEAR( &isDoneMask );

	docUpdateItemShading( &isDoneMask, &(rpTo->rpShading),
				&isUpdMask, &(rpFrom->rpShading), colorMap );

	docShadingMaskToRowMask( &isDoneMask, &isDoneMask );
	utilPropMaskOr( &rpDoneMask, &rpDoneMask, &isDoneMask );
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropALIGNMENT ) )
	{
	if  ( rpTo->rpAlignment != rpFrom->rpAlignment )
	    {
	    rpTo->rpAlignment= rpFrom->rpAlignment;
	    PROPmaskADD( &rpDoneMask, RPpropALIGNMENT );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropIS_TABLE_HEADER ) )
	{
	if  ( rpTo->rpIsTableHeader != rpFrom->rpIsTableHeader )
	    {
	    rpTo->rpIsTableHeader= rpFrom->rpIsTableHeader;
	    PROPmaskADD( &rpDoneMask, RPpropIS_TABLE_HEADER );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropKEEP_ON_ONE_PAGE ) )
	{
	if  ( rpTo->rpKeepOnOnePage != rpFrom->rpKeepOnOnePage )
	    {
	    rpTo->rpKeepOnOnePage= rpFrom->rpKeepOnOnePage;
	    PROPmaskADD( &rpDoneMask, RPpropKEEP_ON_ONE_PAGE );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropKEEP_WITH_NEXT ) )
	{
	if  ( rpTo->rpKeepWithNext != rpFrom->rpKeepWithNext )
	    {
	    rpTo->rpKeepWithNext= rpFrom->rpKeepWithNext;
	    PROPmaskADD( &rpDoneMask, RPpropKEEP_WITH_NEXT );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropAUTOFIT ) )
	{
	if  ( rpTo->rpAutofit != rpFrom->rpAutofit )
	    {
	    rpTo->rpAutofit= rpFrom->rpAutofit;
	    PROPmaskADD( &rpDoneMask, RPpropAUTOFIT );
	    }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRW_WIDTH ) )
	{
	if  ( rpTo->rpPreferredWidth != rpFrom->rpPreferredWidth )
	    {
	    rpTo->rpPreferredWidth= rpFrom->rpPreferredWidth;
	    PROPmaskADD( &rpDoneMask, RPpropTRW_WIDTH );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRFTS_WIDTH ) )
	{
	if  ( rpTo->rpPreferredWidthUnit != rpFrom->rpPreferredWidthUnit )
	    {
	    rpTo->rpPreferredWidthUnit= rpFrom->rpPreferredWidthUnit;
	    PROPmaskADD( &rpDoneMask, RPpropTRFTS_WIDTH );
	    }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDL ) )
	{
	if  ( rpTo->rpLeftDefaultCellSpacing !=
				      rpFrom->rpLeftDefaultCellSpacing )
	    {
	    rpTo->rpLeftDefaultCellSpacing= rpFrom->rpLeftDefaultCellSpacing;
	    PROPmaskADD( &rpDoneMask, RPpropTRSPDL );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDR ) )
	{
	if  ( rpTo->rpRightDefaultCellSpacing !=
				    rpFrom->rpRightDefaultCellSpacing )
	    {
	    rpTo->rpRightDefaultCellSpacing= rpFrom->rpRightDefaultCellSpacing;
	    PROPmaskADD( &rpDoneMask, RPpropTRSPDR );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDT ) )
	{
	if  ( rpTo->rpTopDefaultCellSpacing !=
				    rpFrom->rpTopDefaultCellSpacing )
	    {
	    rpTo->rpTopDefaultCellSpacing= rpFrom->rpTopDefaultCellSpacing;
	    PROPmaskADD( &rpDoneMask, RPpropTRSPDT );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDB ) )
	{
	if  ( rpTo->rpBottomDefaultCellSpacing !=
				    rpFrom->rpBottomDefaultCellSpacing )
	    {
	    rpTo->rpBottomDefaultCellSpacing= rpFrom->rpBottomDefaultCellSpacing;
	    PROPmaskADD( &rpDoneMask, RPpropTRSPDB );
	    }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDFL ) )
	{
	if  ( rpTo->rpLeftDefaultCellSpacingUnit !=
				      rpFrom->rpLeftDefaultCellSpacingUnit )
	    {
	    rpTo->rpLeftDefaultCellSpacingUnit=
				    rpFrom->rpLeftDefaultCellSpacingUnit;
	    PROPmaskADD( &rpDoneMask, RPpropTRSPDFL );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDFR ) )
	{
	if  ( rpTo->rpRightDefaultCellSpacingUnit !=
				    rpFrom->rpRightDefaultCellSpacingUnit )
	    {
	    rpTo->rpRightDefaultCellSpacingUnit=
				    rpFrom->rpRightDefaultCellSpacingUnit;
	    PROPmaskADD( &rpDoneMask, RPpropTRSPDFR );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDFT ) )
	{
	if  ( rpTo->rpTopDefaultCellSpacingUnit !=
				    rpFrom->rpTopDefaultCellSpacingUnit )
	    {
	    rpTo->rpTopDefaultCellSpacingUnit=
				    rpFrom->rpTopDefaultCellSpacingUnit;
	    PROPmaskADD( &rpDoneMask, RPpropTRSPDFT );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDFB ) )
	{
	if  ( rpTo->rpBottomDefaultCellSpacingUnit !=
				    rpFrom->rpBottomDefaultCellSpacingUnit )
	    {
	    rpTo->rpBottomDefaultCellSpacingUnit=
				    rpFrom->rpBottomDefaultCellSpacingUnit;
	    PROPmaskADD( &rpDoneMask, RPpropTRSPDFB );
	    }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDL ) )
	{
	if  ( rpTo->rpLeftCellPadding !=
				      rpFrom->rpLeftCellPadding )
	    {
	    rpTo->rpLeftCellPadding= rpFrom->rpLeftCellPadding;
	    PROPmaskADD( &rpDoneMask, RPpropTRPADDL );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDR ) )
	{
	if  ( rpTo->rpRightCellPadding !=
				    rpFrom->rpRightCellPadding )
	    {
	    rpTo->rpRightCellPadding= rpFrom->rpRightCellPadding;
	    PROPmaskADD( &rpDoneMask, RPpropTRPADDR );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDT ) )
	{
	if  ( rpTo->rpTopCellPadding !=
				    rpFrom->rpTopCellPadding )
	    {
	    rpTo->rpTopCellPadding= rpFrom->rpTopCellPadding;
	    PROPmaskADD( &rpDoneMask, RPpropTRPADDT );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDB ) )
	{
	if  ( rpTo->rpBottomCellPadding !=
				    rpFrom->rpBottomCellPadding )
	    {
	    rpTo->rpBottomCellPadding= rpFrom->rpBottomCellPadding;
	    PROPmaskADD( &rpDoneMask, RPpropTRPADDB );
	    }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDFL ) )
	{
	if  ( rpTo->rpLeftCellPaddingUnit !=
				      rpFrom->rpLeftCellPaddingUnit )
	    {
	    rpTo->rpLeftCellPaddingUnit=
				    rpFrom->rpLeftCellPaddingUnit;
	    PROPmaskADD( &rpDoneMask, RPpropTRPADDFL );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDFR ) )
	{
	if  ( rpTo->rpRightCellPaddingUnit !=
				    rpFrom->rpRightCellPaddingUnit )
	    {
	    rpTo->rpRightCellPaddingUnit=
				    rpFrom->rpRightCellPaddingUnit;
	    PROPmaskADD( &rpDoneMask, RPpropTRPADDFR );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDFT ) )
	{
	if  ( rpTo->rpTopCellPaddingUnit !=
				    rpFrom->rpTopCellPaddingUnit )
	    {
	    rpTo->rpTopCellPaddingUnit=
				    rpFrom->rpTopCellPaddingUnit;
	    PROPmaskADD( &rpDoneMask, RPpropTRPADDFT );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDFB ) )
	{
	if  ( rpTo->rpBottomCellPaddingUnit !=
				    rpFrom->rpBottomCellPaddingUnit )
	    {
	    rpTo->rpBottomCellPaddingUnit=
				    rpFrom->rpBottomCellPaddingUnit;
	    PROPmaskADD( &rpDoneMask, RPpropTRPADDFB );
	    }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRW_WIDTHB ) )
	{
	if  ( rpTo->rpCellWidthBefore != rpFrom->rpCellWidthBefore )
	    {
	    rpTo->rpCellWidthBefore= rpFrom->rpCellWidthBefore;
	    PROPmaskADD( &rpDoneMask, RPpropTRW_WIDTHB );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRW_WIDTHA ) )
	{
	if  ( rpTo->rpCellWidthAfter != rpFrom->rpCellWidthAfter )
	    {
	    rpTo->rpCellWidthAfter= rpFrom->rpCellWidthAfter;
	    PROPmaskADD( &rpDoneMask, RPpropTRW_WIDTHA );
	    }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRFTS_WIDTHB ) )
	{
	if  ( rpTo->rpCellWidthBeforeUnit != rpFrom->rpCellWidthBeforeUnit )
	    {
	    rpTo->rpCellWidthBeforeUnit= rpFrom->rpCellWidthBeforeUnit;
	    PROPmaskADD( &rpDoneMask, RPpropTRFTS_WIDTHB );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRFTS_WIDTHA ) )
	{
	if  ( rpTo->rpCellWidthAfterUnit != rpFrom->rpCellWidthAfterUnit )
	    {
	    rpTo->rpCellWidthAfterUnit= rpFrom->rpCellWidthAfterUnit;
	    PROPmaskADD( &rpDoneMask, RPpropTRFTS_WIDTHA );
	    }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropROW_NUMBER ) )
	{
	if  ( rpTo->rpRowNumber != rpFrom->rpRowNumber )
	    {
	    rpTo->rpRowNumber= rpFrom->rpRowNumber;
	    PROPmaskADD( &rpDoneMask, RPpropROW_NUMBER );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropROW_BAND_NUMBER ) )
	{
	if  ( rpTo->rpRowBandNumber != rpFrom->rpRowBandNumber )
	    {
	    rpTo->rpRowBandNumber= rpFrom->rpRowBandNumber;
	    PROPmaskADD( &rpDoneMask, RPpropROW_BAND_NUMBER );
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropROW_STYLE ) )
	{
	if  ( rpTo->rpRowStyle != rpFrom->rpRowStyle )
	    {
	    rpTo->rpRowStyle= rpFrom->rpRowStyle;
	    PROPmaskADD( &rpDoneMask, RPpropROW_STYLE );
	    }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropIS_LAST_ROW ) )
	{
	if  ( rpTo->rpIsLastRow != rpFrom->rpIsLastRow )
	    {
	    rpTo->rpIsLastRow= rpFrom->rpIsLastRow;
	    PROPmaskADD( &rpDoneMask, RPpropIS_LAST_ROW );
	    }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRAUTH ) )
	{
	if  ( rpTo->rpAuthor != rpFrom->rpAuthor )
	    {
	    rpTo->rpAuthor= rpFrom->rpAuthor;
	    PROPmaskADD( &rpDoneMask, RPpropTRAUTH );
	    }
	}

    /**/

    *pRpDonePask= rpDoneMask; return 0;
    }

/************************************************************************/
/*									*/
/*  Compare row properties.						*/
/*									*/
/************************************************************************/

void docRowPropertyDifference(	PropertyMask *			pRpDiffPask,
				const RowProperties *		rp1,
				const RowProperties *		rp2,
				const PropertyMask *		rpUpdMask,
				const int *			colorMap )
    {
    PropertyMask		rpDiffMask;
    PropertyMask		isUpdMask;

    PROPmaskCLEAR( &rpDiffMask );

    if  ( PROPmaskISSET( rpUpdMask, RPpropCOLUMNS ) )
	{
	if  ( rp1->rpCellCount != rp2->rpCellCount )
	    { PROPmaskADD( &rpDiffMask, RPpropCOLUMNS );	}
	else{
	    int				col;
	    const CellProperties *	cp1;
	    const CellProperties *	cp2;

	    cp1= rp1->rpCells;
	    cp2= rp2->rpCells;
	    for ( col= 0; col < rp1->rpCellCount; cp1++, cp2++, col++ )
		{
		PropertyMask	cpDiffMask;
		PropertyMask	cpUpdMask;

		utilPropMaskFill( &cpUpdMask, CLprop_COUNT );
		PROPmaskCLEAR( &cpDiffMask );

		docCellPropertyDifference( &cpDiffMask, cp1,
						    cp2, &cpUpdMask, colorMap );

		if  ( ! utilPropMaskIsEmpty( &cpDiffMask ) )
		    {
		    PROPmaskADD( &rpDiffMask, RPpropCOLUMNS );
		    break;
		    }
		}
	    }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropGAP_WIDTH ) )
	{
	if  ( rp1->rpHalfGapWidthTwips != rp2->rpHalfGapWidthTwips )
	    { PROPmaskADD( &rpDiffMask, RPpropGAP_WIDTH );	}
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropLEFT_INDENT ) )
	{
	if  ( rp1->rpLeftIndentTwips != rp2->rpLeftIndentTwips )
	    { PROPmaskADD( &rpDiffMask, RPpropLEFT_INDENT );	}
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropHEIGHT ) )
	{
	if  ( rp1->rpHeightTwips != rp2->rpHeightTwips )
	    { PROPmaskADD( &rpDiffMask, RPpropHEIGHT );	}
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTOP_BORDER ) )
	{
	if  ( docBordersDiffer( &(rp1->rpTopBorder), &(rp2->rpTopBorder), colorMap ) )
	    { PROPmaskADD( &rpDiffMask, RPpropTOP_BORDER );	}
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropBOTTOM_BORDER ) )
	{
	if  ( docBordersDiffer( &(rp1->rpBottomBorder),
						&(rp2->rpBottomBorder), colorMap ) )
	    { PROPmaskADD( &rpDiffMask, RPpropBOTTOM_BORDER );	}
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropLEFT_BORDER ) )
	{
	if  ( docBordersDiffer( &(rp1->rpLeftBorder),
					&(rp2->rpLeftBorder), colorMap ) )
	    { PROPmaskADD( &rpDiffMask, RPpropLEFT_BORDER );	}
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropRIGHT_BORDER ) )
	{
	if  ( docBordersDiffer( &(rp1->rpRightBorder),
					&(rp2->rpRightBorder), colorMap ) )
	    { PROPmaskADD( &rpDiffMask, RPpropRIGHT_BORDER );	}
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropHORIZ_BORDER ) )
	{
	if  ( docBordersDiffer( &(rp1->rpHorizontalBorder),
				    &(rp2->rpHorizontalBorder), colorMap ) )
	    { PROPmaskADD( &rpDiffMask, RPpropHORIZ_BORDER );	}
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropVERT_BORDER ) )
	{
	if  ( docBordersDiffer( &(rp1->rpVerticalBorder),
				    &(rp2->rpVerticalBorder), colorMap ) )
	    { PROPmaskADD( &rpDiffMask, RPpropVERT_BORDER );	}
	}

    docShadingMaskFromRowMask( &isUpdMask, rpUpdMask );
    if  ( ! utilPropMaskIsEmpty( &isUpdMask ) )
	{
	PropertyMask	isDiffMask;

	PROPmaskCLEAR( &isDiffMask );

	docItemShadingDifference( &isDiffMask, &(rp1->rpShading),
				    &(rp2->rpShading), &isUpdMask, colorMap );

	docShadingMaskToRowMask( &isDiffMask, &isDiffMask );
	utilPropMaskOr( &rpDiffMask, &rpDiffMask, &isDiffMask );
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropALIGNMENT ) )
	{
	if  ( rp1->rpAlignment != rp2->rpAlignment )
	    { PROPmaskADD( &rpDiffMask, RPpropALIGNMENT ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropIS_TABLE_HEADER ) )
	{
	if  ( rp1->rpIsTableHeader != rp2->rpIsTableHeader )
	    { PROPmaskADD( &rpDiffMask, RPpropIS_TABLE_HEADER ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropKEEP_ON_ONE_PAGE ) )
	{
	if  ( rp1->rpKeepOnOnePage != rp2->rpKeepOnOnePage )
	    { PROPmaskADD( &rpDiffMask, RPpropKEEP_ON_ONE_PAGE ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropKEEP_WITH_NEXT ) )
	{
	if  ( rp1->rpKeepWithNext != rp2->rpKeepWithNext )
	    { PROPmaskADD( &rpDiffMask, RPpropKEEP_WITH_NEXT ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropAUTOFIT ) )
	{
	if  ( rp1->rpAutofit != rp2->rpAutofit )
	    { PROPmaskADD( &rpDiffMask, RPpropAUTOFIT ); }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRW_WIDTH ) )
	{
	if  ( rp1->rpPreferredWidth != rp2->rpPreferredWidth )
	    { PROPmaskADD( &rpDiffMask, RPpropTRW_WIDTH );	}
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRFTS_WIDTH ) )
	{
	if  ( rp1->rpPreferredWidthUnit != rp2->rpPreferredWidthUnit )
	    { PROPmaskADD( &rpDiffMask, RPpropTRFTS_WIDTH );	}
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDL ) )
	{
	if  ( rp1->rpLeftDefaultCellSpacing !=
				      rp2->rpLeftDefaultCellSpacing )
	    { PROPmaskADD( &rpDiffMask, RPpropTRSPDL ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDR ) )
	{
	if  ( rp1->rpRightDefaultCellSpacing !=
				    rp2->rpRightDefaultCellSpacing )
	    { PROPmaskADD( &rpDiffMask, RPpropTRSPDR ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDT ) )
	{
	if  ( rp1->rpTopDefaultCellSpacing !=
				    rp2->rpTopDefaultCellSpacing )
	    { PROPmaskADD( &rpDiffMask, RPpropTRSPDT ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDB ) )
	{
	if  ( rp1->rpBottomDefaultCellSpacing !=
				    rp2->rpBottomDefaultCellSpacing )
	    { PROPmaskADD( &rpDiffMask, RPpropTRSPDB ); }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDFL ) )
	{
	if  ( rp1->rpLeftDefaultCellSpacingUnit !=
				      rp2->rpLeftDefaultCellSpacingUnit )
	    { PROPmaskADD( &rpDiffMask, RPpropTRSPDFL ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDFR ) )
	{
	if  ( rp1->rpRightDefaultCellSpacingUnit !=
				    rp2->rpRightDefaultCellSpacingUnit )
	    { PROPmaskADD( &rpDiffMask, RPpropTRSPDFR ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDFT ) )
	{
	if  ( rp1->rpTopDefaultCellSpacingUnit !=
				    rp2->rpTopDefaultCellSpacingUnit )
	    { PROPmaskADD( &rpDiffMask, RPpropTRSPDFT ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRSPDFB ) )
	{
	if  ( rp1->rpBottomDefaultCellSpacingUnit !=
				    rp2->rpBottomDefaultCellSpacingUnit )
	    { PROPmaskADD( &rpDiffMask, RPpropTRSPDFB ); }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDL ) )
	{
	if  ( rp1->rpLeftCellPadding !=
				      rp2->rpLeftCellPadding )
	    { PROPmaskADD( &rpDiffMask, RPpropTRPADDL ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDR ) )
	{
	if  ( rp1->rpRightCellPadding !=
				    rp2->rpRightCellPadding )
	    { PROPmaskADD( &rpDiffMask, RPpropTRPADDR ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDT ) )
	{
	if  ( rp1->rpTopCellPadding !=
				    rp2->rpTopCellPadding )
	    { PROPmaskADD( &rpDiffMask, RPpropTRPADDT ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDB ) )
	{
	if  ( rp1->rpBottomCellPadding !=
				    rp2->rpBottomCellPadding )
	    { PROPmaskADD( &rpDiffMask, RPpropTRPADDB ); }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDFL ) )
	{
	if  ( rp1->rpLeftCellPaddingUnit !=
				      rp2->rpLeftCellPaddingUnit )
	    { PROPmaskADD( &rpDiffMask, RPpropTRPADDFL ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDFR ) )
	{
	if  ( rp1->rpRightCellPaddingUnit !=
				    rp2->rpRightCellPaddingUnit )
	    { PROPmaskADD( &rpDiffMask, RPpropTRPADDFR ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDFT ) )
	{
	if  ( rp1->rpTopCellPaddingUnit !=
				    rp2->rpTopCellPaddingUnit )
	    { PROPmaskADD( &rpDiffMask, RPpropTRPADDFT ); }
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRPADDFB ) )
	{
	if  ( rp1->rpBottomCellPaddingUnit !=
				    rp2->rpBottomCellPaddingUnit )
	    { PROPmaskADD( &rpDiffMask, RPpropTRPADDFB ); }
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRW_WIDTHB ) )
	{
	if  ( rp1->rpCellWidthBefore != rp2->rpCellWidthBefore )
	    { PROPmaskADD( &rpDiffMask, RPpropTRW_WIDTHB );	}
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRW_WIDTHA ) )
	{
	if  ( rp1->rpCellWidthAfter != rp2->rpCellWidthAfter )
	    { PROPmaskADD( &rpDiffMask, RPpropTRW_WIDTHA );	}
	}

    /**/
    if  ( PROPmaskISSET( rpUpdMask, RPpropTRFTS_WIDTHB ) )
	{
	if  ( rp1->rpCellWidthBeforeUnit != rp2->rpCellWidthBeforeUnit )
	    { PROPmaskADD( &rpDiffMask, RPpropTRFTS_WIDTHB );	}
	}

    if  ( PROPmaskISSET( rpUpdMask, RPpropTRFTS_WIDTHA ) )
	{
	if  ( rp1->rpCellWidthAfterUnit != rp2->rpCellWidthAfterUnit )
	    { PROPmaskADD( &rpDiffMask, RPpropTRFTS_WIDTHA );	}
	}

    /**/

    *pRpDiffPask= rpDiffMask; return;
    }

/************************************************************************/
/*									*/
/*  Update borders for a certain number of columns.			*/
/*									*/
/************************************************************************/

void docRowSetTopBorderInCols(	RowProperties *			rp,
				int				col0,
				int				col1,
				const PropertyMask *		bpSetMask,
				const BorderProperties *	bpSet )
    {
    CellProperties *			cp;
    int					col;

    PropertyMask			bpDoneMask;

    cp= rp->rpCells+ col0;
    for ( col= col0; col <= col1; cp++, col++ )
	{
	docUpdateBorderProperties( &bpDoneMask, &(cp->cpTopBorder),
							bpSetMask, bpSet );
	}

    return;
    }


void docRowSetBottomBorderInCols(RowProperties *		rp,
				int				col0,
				int				col1,
				const PropertyMask *		bpSetMask,
				const BorderProperties *	bpSet )
    {
    CellProperties *			cp;
    int					col;

    PropertyMask			bpDoneMask;

    cp= rp->rpCells+ col0;
    for ( col= col0; col <= col1; cp++, col++ )
	{
	docUpdateBorderProperties( &bpDoneMask, &(cp->cpBottomBorder),
							bpSetMask, bpSet );
	}

    return;
    }


void docRowSetLeftBorderInCols(	RowProperties *			rp,
				int				col0,
				int				col1,
				const PropertyMask *		bpSetMask,
				const BorderProperties *	bpSet )
    {
    CellProperties *			cp;
    int					col;

    PropertyMask			bpDoneMask;

    cp= rp->rpCells+ col0;
    for ( col= col0; col <= col1; cp++, col++ )
	{
	docUpdateBorderProperties( &bpDoneMask, &(cp->cpLeftBorder),
							bpSetMask, bpSet );
	}

    return;
    }


void docRowSetRightBorderInCols(RowProperties *			rp,
				int				col0,
				int				col1,
				const PropertyMask *		bpSetMask,
				const BorderProperties *	bpSet )
    {
    CellProperties *			cp;
    int					col;

    PropertyMask			bpDoneMask;

    cp= rp->rpCells+ col0;
    for ( col= col0; col <= col1; cp++, col++ )
	{
	docUpdateBorderProperties( &bpDoneMask, &(cp->cpRightBorder),
							bpSetMask, bpSet );
	}

    return;
    }

void docRowSetShadingInCols(	RowProperties *			rp,
				int				col0,
				int				col1,
				const PropertyMask *		isSetMask,
				const ItemShading *		isSet )
    {
    CellProperties *		cp;
    int				col;

    PropertyMask		isDoneMask;

    const int * const		colorMap= (const int *)0;

    cp= rp->rpCells+ col0;
    for ( col= col0; col <= col1; cp++, col++ )
	{
	docUpdateItemShading( &isDoneMask, &(cp->cpShading),
						isSetMask, isSet, colorMap );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate an item shading property mask to a cell property mask	*/
/*									*/
/************************************************************************/

void docShadingMaskToRowMask(	PropertyMask *		rpPropMask,
				const PropertyMask *	isPropMask )
    {
    PropertyMask	pm;

    PROPmaskCLEAR( &pm );

    if  ( PROPmaskISSET( isPropMask, ISpropFORE_COLOR ) )
	{ PROPmaskADD( &pm, RPpropSHADE_FORE_COLOR );	}

    if  ( PROPmaskISSET( isPropMask, ISpropBACK_COLOR ) )
	{ PROPmaskADD( &pm, RPpropSHADE_BACK_COLOR );	}

    if  ( PROPmaskISSET( isPropMask, ISpropLEVEL ) )
	{ PROPmaskADD( &pm, RPpropSHADE_LEVEL );	}

    if  ( PROPmaskISSET( isPropMask, ISpropPATTERN ) )
	{ PROPmaskADD( &pm, RPpropSHADE_PATTERN );	}

    *rpPropMask= pm;
    return;
    }

void docShadingMaskFromRowMask(		PropertyMask *		isPropMask,
					const PropertyMask *	rpPropMask )
    {
    PropertyMask	pm;

    PROPmaskCLEAR( &pm );

    if  ( PROPmaskISSET( rpPropMask, RPpropSHADE_FORE_COLOR ) )
	{ PROPmaskADD( &pm, ISpropFORE_COLOR );	}

    if  ( PROPmaskISSET( rpPropMask, RPpropSHADE_BACK_COLOR ) )
	{ PROPmaskADD( &pm, ISpropBACK_COLOR );	}

    if  ( PROPmaskISSET( rpPropMask, RPpropSHADE_LEVEL ) )
	{ PROPmaskADD( &pm, ISpropLEVEL );	}

    if  ( PROPmaskISSET( rpPropMask, RPpropSHADE_PATTERN ) )
	{ PROPmaskADD( &pm, ISpropPATTERN );	}

    *isPropMask= pm;
    return;
    }

