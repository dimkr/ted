/************************************************************************/
/*									*/
/*  Layout of a document.						*/
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

#   include	<charnames.h>
#   include	<utilMatchFont.h>

#   include	"docLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Default leading in terms of the size of the dominant font in a	*/
/*  paragraph.								*/
/*									*/
/************************************************************************/

#   define LINEDISTFAC	7

/************************************************************************/
/*									*/
/*  Initialize a layout job.						*/
/*									*/
/************************************************************************/

void docPsInitLayoutJob(	LayoutJob *	lj )
    {
    lj->ljChangedRectanglePixels= (DocumentRectangle *)0;
    lj->ljAdd= (AppDrawingData *)0;
    lj->ljBd= (BufferDocument *)0;
    lj->ljScreenFontList= (ScreenFontList *)0;
    lj->ljChangedItem= (BufferItem *)0;

    docInitLayoutPosition( &(lj->ljPosition) );

    lj->ljLayoutScreen.slStartRow= (START_ROW)0;
    lj->ljLayoutScreen.slStartParagraph= (START_SCREEN_PARAGRAPH)0;
    lj->ljLayoutScreen.slScreenFrame= (SCREEN_FRAME)0;
    lj->ljLayoutScreen.slLayoutLine= (LAYOUT_SCREEN_LINE)0;

    lj->ljPlaceScreen.slStartRow= (START_ROW)0;
    lj->ljPlaceScreen.slStartParagraph= (START_SCREEN_PARAGRAPH)0;
    lj->ljPlaceScreen.slScreenFrame= (SCREEN_FRAME)0;
    lj->ljPlaceScreen.slLayoutLine= (LAYOUT_SCREEN_LINE)0;

    return;
    }

void docPsCleanLayoutJob(	LayoutJob *	lj )
    {
    return;
    }

void docLayoutInitBlockFrame(	BlockFrame *	bf )
    {
    bf->bfX0Twips= 0;
    bf->bfX1Twips= 0;
    bf->bfY0Twips= 0;
    bf->bfY1Twips= 0;

    bf->bfYBelowShapes= 0;

    bf->bfColumnWidthTwips= 0;

    bf->bfFootnotesPlaced= 0;
    docInitNotesReservation( &(bf->bfNotesReservation) );
    }

void docInitNotesReservation(	NotesReservation *	nr )
    {
    nr->nrFirstFootnote= -1;
    nr->nrFootnoteCount= 0;

    nr->nrFtnsepHeight= 0;
    nr->nrFootnoteHeight= 0;
    }

/************************************************************************/
/*									*/
/*  Continue to lay out the text on a subsequent page.			*/
/*									*/
/************************************************************************/

void docLayoutSectColumnTop(	BufferItem *		bodySectBi,
				BufferDocument *	bd,
				LayoutPosition *	lpTop,
				BlockFrame *		bf )
    {
    docBlockFrameTwips( bf, bodySectBi, bd, lpTop->lpPage, lpTop->lpColumn );

    lpTop->lpPageYTwips= bf->bfY0Twips;
    lpTop->lpAtTopOfColumn= 1;

    return;
    }

void docLayoutToNextColumn(	BufferItem *		bi,
				BufferDocument *	bd,
				LayoutPosition *	lpTop,
				BlockFrame *		bf )
    {
    while ( bi				&&
	    bi->biLevel != DOClevSECT	)
	{ bi= bi->biParent;	}

    if  ( ! bi )
	{ XDEB(bi); return;	}

    lpTop->lpPage++;

    docLayoutSectColumnTop( bi, bd, lpTop, bf );

    return;
    }

/************************************************************************/
/*									*/
/*  Add the space occupied by a border to an inset.			*/
/*									*/
/************************************************************************/

void docLayoutAddBorderToInset(	int *				pInset,
				const BorderProperties *	bp )
    {
    if  ( ! DOCisBORDER( bp ) )
	{ return;	}

    (*pInset) += bp->bpSpacingTwips;
    (*pInset) += bp->bpPenWideTwips;

    return;
    }

void docLayoutStretchInsetForBorder(
				int *				pInset,
				const BorderProperties *	bp )
    {
    int		inset= 0;

    inset += bp->bpSpacingTwips;
    inset += bp->bpPenWideTwips;

    if  ( (*pInset) < inset )
	{ (*pInset)=  inset;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Calculate the height of a series of lines in a paragraph.		*/
/*									*/
/************************************************************************/

static void docLayoutCalculateParaTopInset(	BufferItem *	paraBi )
    {
    int				topInset= 0;
    const BorderProperties *	bp= (const BorderProperties *)0;
    const BufferItem *		cellBi= paraBi->biParent;
    const BufferItem *		prevBi= (const BufferItem *)0;

    int				cellMargin= 0;

    if  ( paraBi->biNumberInParent > 0 )
	{ prevBi= cellBi->biChildren[paraBi->biNumberInParent- 1]; }

    topInset= paraBi->biParaSpaceBeforeTwips;

    if  ( paraBi->biParaInTable			&&
	  paraBi->biNumberInParent == 0		)
	{
	const BufferItem *	rowBi= cellBi->biParent;

	const RowProperties *	rp= &(rowBi->biRowProperties);
	const CellProperties *	cp= rp->rpCells+ cellBi->biNumberInParent;

	switch( rp->rpTopCellPaddingUnit )
	    {
	    case TRautoNONE:
		break;

	    case TRautoTWIPS:
		cellMargin= rp->rpTopCellPadding;
		break;

	    default:
		LDEB(rp->rpTopCellPaddingUnit);
		break;
	    }

	switch( cp->cpTopPaddingUnit )
	    {
	    case TRautoNONE:
		break;

	    case TRautoTWIPS:
		cellMargin= cp->cpTopPadding;
		break;

	    default:
		LDEB(cp->cpTopPaddingUnit);
		break;
	    }
	}

    if  ( DOCisBORDER( &(paraBi->biParaTopBorder) ) )
	{
	if  ( ! prevBi						||
	      ! DOCisBORDER( &(prevBi->biParaBottomBorder) )	)
	    {
	    docLayoutAddBorderToInset( &topInset, &(paraBi->biParaTopBorder) );
	    bp= &(paraBi->biParaTopBorder);
	    }
	}

    paraBi->biTopInsetTwips= cellMargin+ topInset;
    paraBi->biParaBorderAboveParagraph= bp;

    return;
    }

static void docParaSpaceForBottomBorder(int *				pBelow,
					const BorderProperties *	bp,
					const BufferItem *		paraBi )
    {
    int		below= *pBelow;

    below += paraBi->biParaLeadingTwips;

    docLayoutAddBorderToInset( &below, bp );

    *pBelow= below;
    }

static void docLayoutCalculateParaBottomInset(	BufferItem *	paraBi )
    {
    int				bottomInset= 0;
    const BorderProperties *	bp= (const BorderProperties *)0;
    const BufferItem *		cellBi= paraBi->biParent;
    const BufferItem *		nextBi= (const BufferItem *)0;

    int				cellMargin= 0;

    if  ( paraBi->biNumberInParent < paraBi->biParent->biChildCount- 1 )
	{ nextBi= cellBi->biChildren[paraBi->biNumberInParent+ 1]; }

    if  ( paraBi->biParaInTable					&&
	  paraBi->biNumberInParent == cellBi->biChildCount- 1	)
	{
	const BufferItem *	rowBi= cellBi->biParent;

	const RowProperties *	rp= &(rowBi->biRowProperties);
	const CellProperties *	cp= rp->rpCells+ cellBi->biNumberInParent;

	switch( rp->rpBottomCellPaddingUnit )
	    {
	    case TRautoNONE:
		break;

	    case TRautoTWIPS:
		cellMargin= rp->rpBottomCellPadding;
		break;

	    default:
		LDEB(rp->rpBottomCellPaddingUnit);
		break;
	    }

	switch( cp->cpBottomPaddingUnit )
	    {
	    case TRautoNONE:
		break;

	    case TRautoTWIPS:
		cellMargin= cp->cpBottomPadding;
		break;

	    default:
		LDEB(cp->cpTopPaddingUnit);
		break;
	    }
	}

    if  ( DOCisBORDER( &(paraBi->biParaBottomBorder) ) )
	{
	if  ( ! nextBi						||
	      ! DOCisBORDER( &(nextBi->biParaTopBorder) )	)
	    {
	    docParaSpaceForBottomBorder( &bottomInset,
				    &(paraBi->biParaBottomBorder), paraBi );
	    bp= &(paraBi->biParaBottomBorder);
	    }
	}

    bottomInset += paraBi->biParaSpaceAfterTwips;

    paraBi->biBottomInsetTwips= bottomInset+ cellMargin;
    paraBi->biParaBorderBelowParagraph= bp;

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the 'majority' font of a paragraph, and get the font	*/
/*  extents for that font.						*/
/*									*/
/*  1)  Note that subscript/superscript is NOT taken into account.	*/
/*									*/
/************************************************************************/

int docPsParagraphLineExtents(	const BufferDocument *		bd,
				const PostScriptFontList *	psfl,
				BufferItem *			bi )
    {
    const TextParticule *	tp= bi->biParaParticules;

    int				sizeTwips;
    int				paraAscent= 0;
    int				paraDescent= 0;
    int				i;

    static int *		counts;
    int *			fresh;

    int				found;
    int				foundCount;

    const TextAttributeList *	tal= &(bd->bdTextAttributeList);

    fresh= (int *)realloc( counts, tal->talCount* sizeof(int) );
    if  ( ! fresh )
	{ LXDEB(tal->talCount,fresh); return -1;	}
    counts= fresh;

    for ( i= 0; i < tal->talCount; i++ )
	{ counts[i]= 0;	}

    for ( i= 0; i < bi->biParaParticuleCount; tp++, i++ )
	{
	if  ( tp->tpKind != DOCkindTEXT		&&
	      tp->tpKind != DOCkindTAB		&&
	      tp->tpKind != DOCkindOBJECT	)
	    { continue;	}

	if  ( tp->tpTextAttributeNumber < 0			||
	      tp->tpTextAttributeNumber >= tal->talCount	)
	    {
	    LLDEB(tp->tpTextAttributeNumber,tal->talCount);
	    continue;
	    }

	counts[tp->tpTextAttributeNumber] += tp->tpStrlen+ 1;
	}

    found= -1;
    foundCount= 0;
    for ( i= 0; i < tal->talCount; i++ )
	{
	if  ( counts[i] > foundCount )
	    { found= i; foundCount= counts[i];	}
	}

    if  ( found >= 0 )
	{
	const DocumentProperties *	dp= &(bd->bdProperties);
	const DocumentFontList *	dfl= &(dp->dpFontList);

	TextAttribute			ta;
	AfmFontInfo *			afi;
	int				encoding;

	utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList), found );

	afi= utilPsGetAfi( &encoding, dfl, psfl, &ta );
	if  ( ! afi )
	    { XDEB(afi); return -1;	}

	sizeTwips= 10* ta.taFontSizeHalfPoints;

	paraAscent= ( sizeTwips* afi->afiFontBBox.abbTop+ 500 ) / 1000;
	paraDescent= paraAscent- sizeTwips;
	}
    else{ /* LDEB(found); */ sizeTwips= 200;	}

    bi->biParaAscentTwips= paraAscent;
    bi->biParaDescentTwips= paraDescent;

    bi->biParaLeadingTwips= sizeTwips/ LINEDISTFAC;

    docLayoutCalculateParaTopInset( bi );
    docLayoutCalculateParaBottomInset( bi );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert the correct page number in a page header/footer.		*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Calculate the layout of a page header.				*/
/*									*/
/************************************************************************/

int docLayoutExternalItem(	ExternalItem *		ei,
				DocumentRectangle *	drChanged,
				int			page,
				int			y0Twips,
				BufferDocument *	bd,
				const BufferItem *	sectBi,
				AppDrawingData *	add,
				ScreenFontList *	sfl,
				INIT_LAYOUT_EXTERNAL	initLayoutExternal,
				DOC_CLOSE_OBJECT	closeObject )
    {
    int		rval= 0;

    if  ( page != ei->eiPageFormattedFor )
	{
	RecalculateFields		rf;
	LayoutJob			lj;

	docInitRecalculateFields( &rf );
	docPsInitLayoutJob( &lj );

	rf.rfBd= bd;
	rf.rfVoidadd= (void *)add;
	rf.rfCloseObject= closeObject;
	rf.rfUpdateFlags=
		FIELDdoDOC_FORMATTED|FIELDdoDOC_COMPLETE|FIELDdoPAGE_NUMBER;
	rf.rfFieldsUpdated= 0;


	if  ( docRecalculateTextLevelFieldsInExternalItem( &rf, ei,
							    sectBi, page ) )
	    { LDEB(page); return -1;	}

	lj.ljPosition.lpPage= page;
	lj.ljPosition.lpPageYTwips= y0Twips;
	lj.ljPosition.lpAtTopOfColumn= 1; /* not really */

	lj.ljChangedRectanglePixels= drChanged;
	lj.ljAdd= add;
	lj.ljBd= bd;
	lj.ljScreenFontList= sfl;
	lj.ljChangedItem= &(bd->bdItem);

	if  ( initLayoutExternal 			&&
	      (*initLayoutExternal)( &lj, ei, page )	)
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( docLayoutItemAndParents( ei->eiItem, &lj ) )
	    { LDEB(1); rval= -1; goto ready;	}

	ei->eiPageFormattedFor= page;
	ei->eiY0UsedTwips= y0Twips;
	ei->eiY1UsedTwips= lj.ljPosition.lpPageYTwips;

      ready:
	docPsCleanLayoutJob( &lj );
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Derive the frame for a paragraph from the page rectangle and the	*/
/*  paragraph properties.						*/
/*									*/
/*  For paragraphs inside a table cell, geometry is derived from the	*/
/*  table column.							*/
/*									*/
/*  1)  The block starts below what has been reserved for the page	*/
/*	header.								*/
/*  2)  The block starts above what has been reserved for the page	*/
/*	footer.								*/
/*									*/
/************************************************************************/

void docBlockFrameTwips(	BlockFrame *		bf,
				BufferItem *		bi,
				const BufferDocument *	bd,
				int			page,
				int			column )
    {
    ExternalItem *		ei;
    int				inHeaderFooter;

    const DocumentProperties *	dp= &(bd->bdProperties);
    const SectionProperties *	sp;
    const DocumentGeometry *	dg;

    BufferItem *		sectBi;

    while( bi				&&
	   bi->biLevel != DOClevSECT	)
	{ bi= bi->biParent;	}

    if  ( ! bi )
	{ XDEB(bi); return;	}

    sp= &(bi->biSectProperties);
    dg= &(sp->spDocumentGeometry);

    bf->bfX0Twips= dg->dgLeftMarginTwips;
    bf->bfX1Twips= dg->dgPageWideTwips- dg->dgRightMarginTwips;
    bf->bfColumnWidthTwips= bf->bfX1Twips- bf->bfX0Twips;

    docInitNotesReservation( &(bf->bfNotesReservation) );

    switch( bi->biInExternalItem )
	{
	int		isEmpty;

	case DOCinBODY:

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    inHeaderFooter= docWhatPageHeader( &ei, &isEmpty, bi, page, dp );

	    /*  1 Reserved!!  */
	    if  ( ei && ei->eiItem )
		{ bf->bfY0Twips= ei->eiY1ReservedTwips;		}
	    else{ bf->bfY0Twips= dg->dgTopMarginTwips;		}

	    bf->bfYBelowShapes= bf->bfY0Twips;

	    inHeaderFooter= docWhatPageFooter( &ei, &isEmpty, bi, page, dp );

	    /*  2 Reserved!!  */
	    if  ( ei && ei->eiItem )
		{
		bf->bfY1Twips= ei->eiY0ReservedTwips;
		}
	    else{
		bf->bfY1Twips=
			    dg->dgPageHighTwips- dg->dgBottomMarginTwips;
		}

	    /*
	    appDebug( "PAGE %3d BLOCK Y: %5d..%5d %s\n",
			    page, bf->bfY0Twips, bf->bfY1Twips,
			    docExternalKindStr( bi->biInExternalItem ) );
	    */

	    break;

	case DOCinSECT_HEADER:
	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:

	case DOCinSECT_FOOTER:
	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:

	    if  ( column != 0 )
		{ LDEB(column);	}

	    sectBi= bd->bdItem.biChildren[bi->biNumberInParent];
	    ei= docSectionHeaderFooter( sectBi, bi->biInExternalItem );
	    bf->bfY0Twips= ei->eiY0ReservedTwips;
	    bf->bfY1Twips= ei->eiY1ReservedTwips;
	    bf->bfYBelowShapes= bf->bfY0Twips;
	    break;

	case DOCinSHPTXT:
	    bf->bfY0Twips= dg->dgTopMarginTwips;
	    bf->bfY1Twips= dg->dgPageHighTwips- dg->dgBottomMarginTwips;
	    bf->bfYBelowShapes= bf->bfY0Twips;
	    break;

	default:
	    LDEB(bi->biInExternalItem);
	}

    return;
    }

void docParagraphFrameTwips(	ParagraphFrame *	pf,
				const BlockFrame *	bf,
				int			bottom,
				int			stripHigh,
				const BufferItem *	paraBi )
    {
    const NotesReservation *	nrBf= &(bf->bfNotesReservation);
    int				footnoteHeight;

    footnoteHeight= nrBf->nrFtnsepHeight+ nrBf->nrFootnoteHeight;

    pf->pfBlockX0Twips= bf->bfX0Twips;
    pf->pfBlockX1Twips= bf->bfX1Twips;
    pf->pfBlockY0Twips= bf->bfY0Twips;
    pf->pfBlockY1Twips= bf->bfY1Twips;

    pf->pfRedrawX0Twips= bf->bfX0Twips;
    pf->pfRedrawX1Twips= bf->bfX1Twips;

    pf->pfX0GeometryTwips= bf->bfX0Twips;
    pf->pfX1GeometryTwips= bf->bfX1Twips;

    pf->pfPageHigh= bf->bfY1Twips- bf->bfY0Twips;

    pf->pfStripHigh= stripHigh;
    pf->pfPageY1Twips= bf->bfY1Twips- footnoteHeight;
    pf->pfFrameY1Twips= pf->pfPageY1Twips;

    if  ( bottom > 0			&&
	  bottom <= pf->pfFrameY1Twips	)
	{ pf->pfFrameY1Twips= bottom;	}

    if  ( paraBi->biParaInTable )
	{
	int			col0;
	int			col1;
	const BufferItem *	cellBi= paraBi->biParent;
	const BufferItem *	rowBi= cellBi->biParent;
	const RowProperties *	rp= &(rowBi->biRowProperties);
	const CellProperties *	cp0;

	int			leftMargin= rowBi->biRowHalfGapWidthTwips;
	int			rightMargin= rowBi->biRowHalfGapWidthTwips;

	col0= col1= cellBi->biNumberInParent;
	cp0= rp->rpCells+ col0;

	switch( rp->rpLeftCellPaddingUnit )
	    {
	    case TRautoNONE:
		break;
	    case TRautoTWIPS:
		leftMargin= rp->rpLeftCellPadding;
		break;
	    default:
		LDEB(rp->rpLeftCellPaddingUnit);
		break;
	    }

	switch( cp0->cpLeftPaddingUnit )
	    {
	    case TRautoNONE:
		break;
	    case TRautoTWIPS:
		leftMargin= cp0->cpLeftPadding;
		break;
	    default:
		LDEB(cp0->cpLeftPaddingUnit);
		break;
	    }

	switch( rp->rpRightCellPaddingUnit )
	    {
	    case TRautoNONE:
		break;
	    case TRautoTWIPS:
		rightMargin= rp->rpRightCellPadding;
		break;
	    default:
		LDEB(rp->rpRightCellPaddingUnit);
		break;
	    }

	switch( cp0->cpRightPaddingUnit )
	    {
	    case TRautoNONE:
		break;
	    case TRautoTWIPS:
		rightMargin= cp0->cpRightPadding;
		break;
	    default:
		LDEB(cp0->cpRightPaddingUnit);
		break;
	    }

	if  ( col1 >= rowBi->biRowCellCount )
	    { LLDEB(col1,rowBi->biRowCellCount);	}
	else{
	    const CellProperties *	cp1= rp->rpCells+ col1;

	    if  ( cp1->cpLeftInMergedRow )
		{
		while( col1 < rowBi->biRowCellCount- 1	&&
		       cp1[1].cpMergedWithLeft		)
		    { col1++; cp1++;	}
		}

	    pf->pfX1GeometryTwips= bf->bfX0Twips+ cp1->cpRightBoundaryTwips;

	    if  ( col0 == 0 )
		{ pf->pfX0GeometryTwips += rowBi->biRowLeftIndentTwips;	}
	    else{
		const CellProperties *	cpp;

		cpp= rp->rpCells+ col0- 1;
		pf->pfX0GeometryTwips += cpp->cpRightBoundaryTwips;
		}
	    }

	pf->pfX0GeometryTwips += leftMargin;
	pf->pfX1GeometryTwips -= rightMargin;

	if  ( rowBi->biRowCellCount < 1 )
	    { LDEB(rowBi->biRowCellCount);	}
	else{
	    pf->pfRedrawX0Twips= -rowBi->biRowLeftIndentTwips;
	    pf->pfRedrawX1Twips=
		    rp->rpCells[rowBi->biRowCellCount-1].cpRightBoundaryTwips;
	    }
	}

    pf->pfX0TextLinesTwips= pf->pfX0GeometryTwips+
					paraBi->biParaLeftIndentTwips;
    pf->pfX1TextLinesTwips= pf->pfX1GeometryTwips-
					paraBi->biParaRightIndentTwips;

    pf->pfX0FirstLineTwips= pf->pfX0TextLinesTwips+
					paraBi->biParaFirstIndentTwips;

    if  ( paraBi->biInExternalItem == DOCinBODY		||
	  paraBi->biInExternalItem == DOCinENDNOTE	||
	  paraBi->biInExternalItem == DOCinAFTNSEP	)
	{ pf->pfHasBottom= 1;	}
    else{ pf->pfHasBottom= 0;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Scale an inserted object to fit where we want to place it.		*/
/*									*/
/*  1)  If the object fits on the page at its current scale, nothing	*/
/*	needs to be done.						*/
/*  2)  Otherwise scale the object to fit on the page. X and Y scale	*/
/*	are the same, irrespective of the original scaling.		*/
/*									*/
/************************************************************************/

void docLayoutScaleObjectToFitParagraphFrame(	InsertedObject *	io,
						const ParagraphFrame *	pf )
    {
    int		textWideTwips= pf->pfX1TextLinesTwips- pf->pfX0TextLinesTwips;

    int		objectWideTwips= ( io->ioScaleXSet* io->ioTwipsWide )/ 100.0;
    int		objectHighTwips= ( io->ioScaleYSet* io->ioTwipsHigh )/ 100.0;

    double	scaleX;
    double	scaleY;

    PictureProperties *	pip= &(io->ioPictureProperties);

    /*  1  */
    if  ( io->ioScaleXUsed == io->ioScaleXSet	&&
	  io->ioScaleYUsed == io->ioScaleYSet	&&
	  objectWideTwips <= textWideTwips	&&
	  objectHighTwips <= pf->pfPageHigh	)
	{ return;	}

    /*  2  */
    scaleX= (double)textWideTwips/ (double)io->ioTwipsWide;
    scaleY= (double)pf->pfPageHigh/ (double)io->ioTwipsHigh;

    if  ( scaleY > scaleX )
	{ scaleY=  scaleX;	}

    io->ioScaleXUsed= io->ioScaleXSet;
    io->ioScaleYUsed= io->ioScaleYSet;

    pip->pipScaleXUsed= io->ioScaleXUsed;
    pip->pipScaleYUsed= io->ioScaleYUsed;

    if  ( scaleY < 0.99 )
	{
	int	scaleMax= (int)( 99* scaleY );

	if  ( io->ioScaleXUsed > scaleMax )
	    { io->ioScaleXUsed= scaleMax;	}
	if  ( io->ioScaleYUsed > scaleMax )
	    { io->ioScaleYUsed= scaleMax;	}

	if  ( io->ioScaleXUsed < 1 )
	    { io->ioScaleXUsed=  1;	}
	if  ( io->ioScaleYUsed < 1 )
	    { io->ioScaleYUsed=  1;	}

	pip->pipScaleXUsed= io->ioScaleXUsed;
	pip->pipScaleYUsed= io->ioScaleYUsed;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Push a position to one at least below another one. An example of	*/
/*  its use is pushing down the bottom of a row to the bottom of its	*/
/*  tallest cell.							*/
/*									*/
/************************************************************************/

void docLayoutPushBottomDown(		LayoutPosition *	lpParentBottom,
					const LayoutPosition *	lpChildBottom )
    {
    if  ( lpParentBottom->lpPage < lpChildBottom->lpPage )
	{
	lpParentBottom->lpPage= lpChildBottom->lpPage;
	lpParentBottom->lpPageYTwips= lpChildBottom->lpPageYTwips;
	lpParentBottom->lpAtTopOfColumn= lpChildBottom->lpAtTopOfColumn;
	}
    else{
	if  ( lpParentBottom->lpPage == lpChildBottom->lpPage )
	    {
	    if  ( lpParentBottom->lpPageYTwips < lpChildBottom->lpPageYTwips )
		{
		lpParentBottom->lpPageYTwips= lpChildBottom->lpPageYTwips;
		lpParentBottom->lpAtTopOfColumn= lpChildBottom->lpAtTopOfColumn;
		}
	    }
	}

    return;
    }

void docLayoutPushBottomDownShifted(	LayoutPosition *	lpParentBottom,
					const LayoutPosition *	lpChildBottom,
					int			pShift,
					int			yShift )
    {
    LayoutPosition	lpShifted;

    lpShifted= *lpChildBottom;
    lpShifted.lpPage += pShift;
    lpShifted.lpPageYTwips += yShift;

    if  ( yShift != 0 )
	{ lpShifted.lpAtTopOfColumn= 0;	}

    docLayoutPushBottomDown( lpParentBottom, &lpShifted );
    }

/************************************************************************/
/*									*/
/*  Translate a stretch of text to upper case for capitals or smallcaps	*/
/*  drawing.								*/
/*									*/
/*  3)  Worst case alternating, starting with lower case. 1+ len	*/
/*	segments. But when it is just one lower case letter we need 3!	*/
/*									*/
/************************************************************************/

int docMakeCapsString(		unsigned char **	pUpperString,
				int **			pSegments,
				int *			pSegmentCount,
				const BufferDocument *	bd,
				const TextAttribute *	ta,
				const unsigned char *	printString,
				int			len )
    {
    int				rval= 0;

    const DocumentProperties *	dp= &(bd->bdProperties);
    const DocumentFontList *	dfl= &(dp->dpFontList);
    const DocumentFont *	df= dfl->dflFonts+ ta->taFontNumber;

    int				segmentCount= 0;
    unsigned char *		upperString= (unsigned char *)0;
    int *			segments= (int *)0;

    int				encoding= -1;

    if  ( df->dfOfficeCharsetMapping )
	{ encoding= df->dfOfficeCharsetMapping->ocmPsFontEncoding; }

    if  ( encoding >= 0		&&
	  encoding < ENCODINGps_COUNT	)
	{
	const unsigned char *	charKinds;
	const unsigned char *	charShifts;
	int			n;
	const unsigned char *	from;
	unsigned char *		to;

	charKinds= PS_Encodings[encoding].fcCharKinds;
	charShifts= PS_Encodings[encoding].fcCharShifts;

	upperString= malloc( len+ 1 );
	if  ( ! upperString )
	    { XDEB(upperString); rval= -1; goto ready;	}

	if  ( ta->taSmallCaps && ! ta->taCapitals )
	    {
	    /*  3  */
	    segments= malloc( (len+ 2)* sizeof(int) );
	    if  ( ! segments )
		{ XDEB(segments); rval= -1; goto ready;	}
	    }

	to= upperString;
	from= printString;
	n= 0;
	while( n < len )
	    {
	    int		n0= 0;
	    int		n1= 0;

	    while( n < len && ! ( charKinds[*from] & CHARisLOWER ) )
		{
		*to= *from;
		n0++; from++; to++; n++;
		}

	    while( n < len &&   ( charKinds[*from] & CHARisLOWER ) )
		{
		*to= charShifts[*from];
		n1++; from++; to++; n++;
		}

	    if  ( segments )
		{
		segments[2*segmentCount+ 0]= n0;
		segments[2*segmentCount+ 1]= n1;
		segmentCount++;
		}
	    }

	*to= '\0';
	}

    if  ( segmentCount > len+ 2 )
	{ LLDEB(len,segmentCount);	}

    *pUpperString= upperString; upperString= (unsigned char *)0;
    *pSegments= segments; segments= (int *)0;
    *pSegmentCount= segmentCount;

  ready:

    if  ( upperString )
	{ free( upperString );	}
    if  ( segments )
	{ free( segments );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Invalidate the layout of all paragraphs in the modified range.	*/
/*									*/
/************************************************************************/

void docInvalidateParagraphLayout(	BufferItem *	paraBi )
    {
    paraBi->biParaLineCount= 0;
    }

static void docInvalidateChangedLayout(	BufferItem *	biParaStart,
					BufferItem *	biParaEnd )
    {
    BufferItem *	bi= biParaStart;

    while( bi )
	{
	bi->biParaLineCount= 0;

	if  ( bi == biParaEnd )
	    { break;	}

	bi= docNextParagraph( bi );
	if  ( ! bi )
	    { XDEB(bi); }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Redo layout of the relevant part of the document after editing.	*/
/*									*/
/************************************************************************/

int docLayoutInvalidateRange(	DocumentSelection *	dsLayout,
				BufferItem *		selRootBi,
				EditRange *		er )
    {
    DocumentPosition	dp;
    BufferItem *	startBi;
    BufferItem *	endBi;

    if  ( er->erStart.epParaNr == 0 )
	{
	LDEB(er->erStart.epParaNr);
	er->erStart.epParaNr= 1;
	}

    startBi= docGetParagraphByNumber( selRootBi,
				    er->erStart.epParaNr );
    endBi= docGetParagraphByNumber( selRootBi,
				    er->erEnd.epParaNr );

    if  ( ! startBi )
	{
	/*LXDEB(er->erStart.epParaNr,startBi);*/
	if  ( docFirstPosition( &dp, selRootBi ) )
	    { LDEB(er->erStart.epParaNr); return -1;	}

	startBi= dp.dpBi;
	}
    if  ( ! endBi )
	{
	/* LXDEB(er->erEnd.epParaNr,endBi); */
	if  ( docLastPosition( &dp, selRootBi ) )
	    { LDEB(er->erEnd.epParaNr); return -1;		}

	endBi= dp.dpBi;
	}

    docInvalidateChangedLayout( startBi, endBi );

    docInitDocumentSelection( dsLayout );
    dsLayout->dsBegin.dpBi= startBi;
    dsLayout->dsBegin.dpStroff= 0;

    dsLayout->dsEnd.dpBi= endBi;
    dsLayout->dsEnd.dpStroff= endBi->biParaStrlen;

    return 0;
    }

