/************************************************************************/
/*									*/
/*  Layout of a document.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	<appImage.h>
#   include	"docLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialisation for paragraph layout.				*/
/*									*/
/*  1)  Optimalisation: If the previous particule has the same text	*/
/*	attributes, it MUST have the same physical font.		*/
/*  2)  If no physical font has been determined for this particule,	*/
/*	open its physical font.						*/
/*  2a) Translate attribute number to the actual attribute values.	*/
/*  2b) Get the physical font for those attributes.			*/
/*									*/
/************************************************************************/

static int docLayoutParaInit(	const BufferDocument *		bd,
				BufferItem *			paraBi,
				AppDrawingData *		add )
    {
    if  ( docPsParagraphLineExtents( bd, add->addPostScriptFontList, paraBi ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static int docPlaceParaInit(	const BufferDocument *		bd,
				BufferItem *			paraBi,
				AppDrawingData *		add )
    {
    return 0;
    }

static int docLayoutParaItem(	BufferItem *			paraBi,
				BlockFrame *			bf,
				LayoutJob *			lj,
				const ParagraphLayoutContext *	plc )
    {
    ParagraphLayoutJob		plj;
    const int			line= 0;
    const int			part= 0;

    docPsBeginParagraphLayoutProgress( &plj,
			paraBi->biNumberInParent, line, part,
			paraBi->biNumberInParent+ 1,
			&(lj->ljPosition) );

    if  ( docLayoutParagraphs( plc, paraBi->biParent, bf, lj, &plj ) )
	{ LDEB(1); return -1;	}

    lj->ljPosition= plj.pljPos.plpPos;

    if  ( paraBi->biParaLineCount < 1 )
	{ LDEB(paraBi->biParaLineCount); docListItem(0,paraBi); return -1; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Adjust the bottom of an item to changes inside.			*/
/*									*/
/************************************************************************/

static void docPsPlaceAdjustBottom(	int *			pChanged,
					BufferItem *		bi,
					const LayoutPosition *	lp,
					AppDrawingData *	add,
					DocumentRectangle *	drChanged )
    {
    int			changed= 0;

    int			oldY1Pixels;
    int			newY1Pixels;

    oldY1Pixels= BI_BELOW_PIXELS( add, bi );

    if  ( ! DOC_SAME_POSITION( &(bi->biBelowPosition), lp ) )
	{ bi->biBelowPosition= *lp; changed= 1; }

    newY1Pixels= LP_YPIXELS( add, lp );

    if  ( oldY1Pixels < newY1Pixels )
	{
	if  ( drChanged					&&
	      drChanged->drY1 < newY1Pixels -1	)
	    { drChanged->drY1=  newY1Pixels -1;	}
	}

    if  ( oldY1Pixels > newY1Pixels )
	{
	if  ( drChanged							&&
	      drChanged->drY1 < oldY1Pixels- 1	)
	    { drChanged->drY1=  oldY1Pixels- 1;	}
	}

    if  ( changed )
	{ *pChanged= changed;	}

    return;
    }

static void docPsLayoutAdjustBottom(	int *			pChanged,
					BufferItem *		bi,
					const LayoutPosition *	lp,
					AppDrawingData *	add,
					DocumentRectangle *	drChanged )
    {
    docPsPlaceAdjustBottom( pChanged, bi, lp, add, drChanged );

    *pChanged= 1;
    }

/************************************************************************/
/*									*/
/*  Place successive items, after the predecessor.			*/
/*									*/
/*  NOTE: In a future incremental version of the formatter, the loop	*/
/*	over the children can be stopped if the bottom position of a	*/
/*	child is not changed, and there is no reason to continue	*/
/*	anyway.								*/
/*									*/
/*	One of the reasons to continue anyway is that a previous table	*/
/*	header has changed height.					*/
/*									*/
/************************************************************************/

static int docLayoutPlaceChildren(
				BufferItem *			parentBi,
				int				from,
				BlockFrame *			bf,
				LayoutJob *			lj,
				const ParagraphLayoutContext *	plc )
    {
    int				i;
    int				changed;
    DocumentRectangle *		drChanged= lj->ljChangedRectanglePixels;

    BufferDocument *		bd= lj->ljBd;
    const DocumentProperties *	dp= &(bd->bdProperties);
    const NotesProperties *	npEndnotes= &(dp->dpEndnoteProperties);

    for ( i= from; i < parentBi->biChildCount; i++ )
	{
	BufferItem *	child= parentBi->biChildren[i];

	child->biTopPosition= lj->ljPosition;

	switch( child->biLevel )
	    {
	    case DOClevSECT:
		docLayoutPlaceSectTop( child, bf, lj );

		if  ( docLayoutPlaceChildren( child, 0, bf, lj, plc ) )
		    { LDEB(1); return -1;	}

		if  ( child->biInExternalItem == DOCinBODY		&&
		      npEndnotes->npPosition == FTN_POS_SECT_END	)
		    {
		    if  ( docLayoutEndnotesForSection(
					    child->biNumberInParent, bf, lj ) )
			{ LDEB(child->biNumberInParent); return -1;	}
		    }

		break;

	    case DOClevDOC:
	    rowAsGroup:
		if  ( docLayoutPlaceChildren( child, 0, bf, lj, plc ) )
		    { LDEB(1); return -1;	}
		break;

	    case DOClevCELL:
		{
		ParagraphLayoutJob	plj;
		const int		para= 0;
		const int		line= 0;
		const int		part= 0;

		docInitParagraphLayoutJob( &plj );

		docPsBeginParagraphLayoutProgress( &plj,
				    para, line, part, child->biChildCount,
				    &(lj->ljPosition) );

		if  ( docLayoutParagraphs( plc, child, bf, lj, &plj ) )
		    { LDEB(1); return -1;	}

		lj->ljPosition= plj.pljPos.plpPos;
		}
		break;

	    case DOClevROW:
		if  ( ! child->biRowHasTableParagraphs )
		    { goto rowAsGroup;	}

		if  ( docLayoutRowItem( child, bf, lj, plc ) )
		    { LDEB(1); return -1;	}

		break;

	    case DOClevPARA:
		if  ( child->biParaLineCount == 0 )
		    { break;	}

		if  ( docLayoutParaItem( child, bf, lj, plc ) )
		    { LDEB(1); return -1;	}

		break;

	    default:
		LDEB(parentBi->biLevel); return -1;
	    }

	docPsPlaceAdjustBottom( &changed, child,
				    &(lj->ljPosition), lj->ljAdd, drChanged );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Redo the layout of a section because one of its external items	*/
/*  changed size.							*/
/*									*/
/************************************************************************/

int docItemLayoutStartPosition(	LayoutPosition *	lp,
				const BufferItem *	bi )
    {
    if  ( bi->biNumberInParent == 0 )
	{
	if  ( bi->biParent )
	    { *lp= bi->biParent->biTopPosition;	}
	else{
	    if  ( bi->biInExternalItem == DOCinBODY )
		{ docInitLayoutPosition( lp );		}
	    else{ *lp= bi->biTopPosition;		}
	    }
	}
    else{
	const BufferItem *	prevBi;

	prevBi= bi->biParent->biChildren[bi->biNumberInParent- 1];

	*lp= prevBi->biBelowPosition;
	}

    return 0;
    }

static int docRedoBodyLayout(		BufferItem *		bodyBi,
					const LayoutJob *	ljRef )
    {
    LayoutJob			bodyLj;

    if  ( bodyBi->biInExternalItem != DOCinBODY )
	{ LDEB(bodyBi->biInExternalItem);	}

    bodyLj= *ljRef;
    bodyLj.ljAdd= ljRef->ljAdd;
    bodyLj.ljBd= ljRef->ljBd;
    bodyLj.ljScreenFontList= ljRef->ljScreenFontList;
    bodyLj.ljChangedItem= bodyBi;

    docItemLayoutStartPosition( &(bodyLj.ljPosition), bodyBi );

    if  ( docLayoutItemAndParents( bodyBi, &bodyLj ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static int docRedoBodyItemLayout(	BufferItem *		bi,
					LayoutJob *		lj )

    {
    BufferDocument *		bd= lj->ljBd;

    ExternalItem *		ei;
    BufferItem *		bodyBi;

    if  ( lj->ljChangedItem->biInExternalItem != bi->biInExternalItem )
	{
	LDEB(lj->ljChangedItem->biInExternalItem);
	LDEB(bi->biInExternalItem);
	return 0;
	}

    if  ( docGetExternalItem( &ei, &bodyBi, bd, bi ) )
	{ LDEB(bi->biInExternalItem); return -1; }
    if  ( ! bodyBi )
	{ bodyBi= &(bd->bdItem);	}

    if  ( lj->ljPosition.lpPageYTwips == ei->eiY1UsedTwips )
	{ return 0;	}

    if  ( docRedoBodyLayout( bodyBi, lj ) )
	{ LDEB(bi->biInExternalItem); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Adjust the document rectangle to the changed layout of the document	*/
/*									*/
/************************************************************************/

static void docLayoutAdjustDocumentRectangle( LayoutJob *		ljRef )
    {
    AppDrawingData *		add= ljRef->ljAdd;
    DocumentRectangle *		drChanged= ljRef->ljChangedRectanglePixels;

    int				extendRectangle;
    int				y1;

    y1= ljRef->ljPosition.lpPage* add->addPageStepPixels+
			add->addPaperRect.drY1- add->addPaperRect.drY0;

    extendRectangle= add->addBackRect.drY1 != y1;

    if  ( drChanged					&&
	  extendRectangle				&&
	  drChanged->drY1 < add->addBackRect.drY1 )
	{ drChanged->drY1=  add->addBackRect.drY1; }

    add->addBackRect.drY1= y1;
    add->addDocRect.drY1= y1- add->addBottomMarginPixels;

    if  ( drChanged					&&
	  extendRectangle				&&
	  drChanged->drY1 < add->addBackRect.drY1 )
	{ drChanged->drY1=  add->addBackRect.drY1; }

    return;
    }

/************************************************************************/
/*									*/
/*  Fixup routine that is called when the first child item does not	*/
/*  have the same top position as its parent.				*/
/*									*/
/*  This can happen when during a reformat of part of the document	*/
/*  the child moves to a different page. E.G: When a property change	*/
/*  of a paragraph makes it higher or lower.				*/
/*									*/
/************************************************************************/

static int docPsFixupParentGeometry(	BufferItem *		bi,
					BufferItem *		biParent )
    {
    LayoutPosition	lpBi= bi->biTopPosition;
    LayoutPosition	lpPa= biParent->biTopPosition;

    lpBi.lpPageYTwips -= biParent->biTopInsetTwips;

    if  ( ! DOC_SAME_POSITION( &lpPa, &lpBi ) )
	{
#	if 0
	if  ( bi->biLevel == DOClevPARA)
	    { SDEB((char *)bi->biParaString);	}

	SSDEB(docLevelStr(biParent->biLevel),docLevelStr(bi->biLevel));
	LLDEB(lpPa.lpPage,lpBi.lpPage);
	LLDEB(lpPa.lpPageYTwips,lpBi.lpPageYTwips);
#	endif

	biParent->biTopPosition= lpBi;

	return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Adjust the geometry of a parent item to changes in a child.		*/
/*									*/
/*  This actually is a full layout action of everything below the	*/
/*  recently reformatted part. There are two differences:		*/
/*  a)  Reformatting stops when a buffer item lands in the same		*/
/*	position where it was before the change that forced us to redo	*/
/*	part of the layout.						*/
/*  b)  We try not to recalculate the layout of the text inside the	*/
/*	individual text lines.						*/
/*									*/
/************************************************************************/

static int docAdjustParentGeometry(	BufferItem *		bi,
					BlockFrame *		bf,
					LayoutJob *		lj )
    {
    BufferDocument *		bd= lj->ljBd;
    const DocumentProperties *	dp= &(bd->bdProperties);
    const NotesProperties *	npEndnotes= &(dp->dpEndnoteProperties);

    DocumentRectangle *		drChanged= lj->ljChangedRectanglePixels;
    BufferItem *		parentBi;
    int				from;

    ParagraphLayoutContext	plc;

    plc.plcChangedRectanglePixels= lj->ljChangedRectanglePixels;
    plc.plcAdd= lj->ljAdd;
    plc.plcBd= lj->ljBd;
    plc.plcScreenFontList= lj->ljScreenFontList;

    plc.plcScreenLayout= lj->ljPlaceScreen;
    plc.plcStartParagraph= docPlaceParaInit;
    plc.plcAdjustBottom= docPsPlaceAdjustBottom;

    parentBi= bi->biParent;

    if  ( bi->biLevel == DOClevPARA )
	{
	ParagraphLayoutJob	plj;
	int			advanced= 0;
	const int		line= 0;
	const int		part= 0;

	docInitParagraphLayoutJob( &plj );

	docPsBeginParagraphLayoutProgress( &plj,
				    bi->biNumberInParent+ 1, line, part,
				    parentBi->biChildCount,
				    &(lj->ljPosition) );

	docPsAdvanceParagraphLayout( &advanced,
				    &(plj.pljPos0.plpProgress),
				    &(plj.pljPos0.plpProgress),
				    &(plj.pljPos.plpProgress),
				    lj->ljPosition.lpPage, parentBi );

	if  ( bi->biParaStartsOnNewPage && ! bi->biParaInTable )
	    {
	    BufferItem *	bbi= bi;

	    while( bbi->biNumberInParent == 0 )
		{
		if  ( ! bbi->biParent )
		    { break;	}

		bbi->biParent->biTopPosition= bi->biTopPosition;
		bbi= bbi->biParent;
		}
	    }

	if  ( plj.pljPos0.plpProgress.pspPara >= bi->biNumberInParent+ 1 )
	    {
	    if  ( plj.pljPos0.plpProgress.pspPara > bi->biNumberInParent+ 1 )
		{ LLDEB(plj.pljPos0.plpProgress.pspPara,bi->biNumberInParent); }

	    from= bi->biNumberInParent+ 1;
	    }
	else{
	    from= plj.pljPos0.plpProgress.pspPara;

	    if  ( from == 0 )
		{
		lj->ljPosition= parentBi->biChildren[from]->biTopPosition;
		}
	    else{
		lj->ljPosition=
			    parentBi->biChildren[from- 1]->biBelowPosition;
		}
	    }
	}
    else{ from= bi->biNumberInParent+ 1;	}

    while( parentBi )
	{
	int		changed= 0;

	switch( parentBi->biLevel )
	    {
	    int		keepRowOnPage;

	    case DOClevDOC:
		if  ( bi->biNumberInParent == 0			&&
		      docPsFixupParentGeometry( bi, parentBi )	)
		    { from= 0; lj->ljPosition= bi->biTopPosition; }

		if  ( from <= parentBi->biChildCount- 1 )
		    {
		    docLayoutPlaceChildren( parentBi, from, bf, lj, &plc );
		    }
		else{ lj->ljPosition= bi->biBelowPosition;		}

		if  ( parentBi->biInExternalItem == DOCinBODY	&&
		      npEndnotes->npPosition == FTN_POS_DOC_END	)
		    {
		    if  ( docLayoutEndnotesForDocument( bf, lj ) )
			{ LDEB(1); return -1;	}
		    }

		break;

	    case DOClevSECT:
		if  ( bi->biNumberInParent == 0			&&
		      docPsFixupParentGeometry( bi, parentBi )	)
		    { from= 0; lj->ljPosition= bi->biTopPosition; }

		if  ( from <= parentBi->biChildCount- 1 )
		    {
		    docLayoutPlaceChildren( parentBi, from, bf, lj, &plc );
		    }
		else{ lj->ljPosition= bi->biBelowPosition;		}

		if  ( parentBi->biInExternalItem == DOCinBODY		&&
		      npEndnotes->npPosition == FTN_POS_SECT_END	)
		    {
		    if  ( docLayoutEndnotesForSection(
				    parentBi->biNumberInParent, bf, lj ) )
			{ LDEB(1); return -1;	}
		    }

		break;

	    parentRowAsGroup:
		if  ( bi->biNumberInParent == 0			&&
		      docPsFixupParentGeometry( bi, parentBi )	)
		    { from= 0; lj->ljPosition= bi->biTopPosition; }

		if  ( from <= parentBi->biChildCount- 1 )
		    {
		    docLayoutPlaceChildren( parentBi, from, bf, lj, &plc );
		    }
		else{ lj->ljPosition= bi->biBelowPosition;		}

		break;

	    case DOClevCELL:
		if  ( bi->biNumberInParent == 0			&&
		      docPsFixupParentGeometry( bi, parentBi )	)
		    { from= 0; lj->ljPosition= bi->biTopPosition; }

		if  ( from <= parentBi->biChildCount- 1 )
		    {
		    ParagraphLayoutJob	plj;
		    const int		line= 0;
		    const int		part= 0;

		    docInitParagraphLayoutJob( &plj );

		    docPsBeginParagraphLayoutProgress( &plj,
						    from, line, part,
						    parentBi->biChildCount,
						    &(lj->ljPosition) );

		    if  ( docLayoutParagraphs( &plc, parentBi, bf, lj, &plj ) )
			{ LDEB(1); return -1;	}

		    lj->ljPosition= plj.pljPos.plpPos;
		    }
		else{ lj->ljPosition= bi->biBelowPosition;		}

		break;

	    case DOClevROW:
		if  ( ! parentBi->biRowHasTableParagraphs )
		    { goto parentRowAsGroup;	}

		keepRowOnPage= parentBi->biRowKeepOnOnePage ||
					    parentBi->biRowIsTableHeader;

		if  ( parentBi->biNumberInParent > 0	&&
		      parentBi->biParent		&&
		      keepRowOnPage			)
		    {
		    BufferItem *	pp= parentBi->biParent;
		    BufferItem *	ch;

		    ch= pp->biChildren[parentBi->biNumberInParent- 1];

		    parentBi->biTopPosition= ch->biBelowPosition;
		    lj->ljPosition= ch->biBelowPosition;
		    }

		lj->ljPosition= parentBi->biTopPosition;

		if  ( docLayoutRowItem( parentBi, bf, lj, &plc ) )
		    { LDEB(1); return -1;	}

		break;

	    default:
		LDEB(parentBi->biLevel); return -1;
	    }

	docPsPlaceAdjustBottom( &changed, parentBi,
				    &(lj->ljPosition), lj->ljAdd, drChanged );

	if  ( ! changed )
	    { break;	}

	bi= parentBi; parentBi= bi->biParent;
	from= bi->biNumberInParent+ 1;
	}

    if  ( bi->biInExternalItem == DOCinBODY	&&
	  BF_HAS_FOOTNOTES( bf )		)
	{
	const int		belowText= 0;
	LayoutPosition		lpBelowNotes;

	if  ( docLayoutFootnotesForColumn( &lpBelowNotes, bf,
					belowText, &(lj->ljPosition), lj ) )
	    { LDEB(1); return -1;	}
	}

    if  ( ! parentBi )
	{
	switch( bi->biInExternalItem )
	    {
	    case DOCinBODY:
		docLayoutAdjustDocumentRectangle( lj );
		break;

	    case DOCinSECT_HEADER:
	    case DOCinFIRST_HEADER:
	    case DOCinLEFT_HEADER:
	    case DOCinRIGHT_HEADER:
	    case DOCinSECT_FOOTER:
	    case DOCinFIRST_FOOTER:
	    case DOCinLEFT_FOOTER:
	    case DOCinRIGHT_FOOTER:

		if  ( bi->biLevel != DOClevSECT )
		    { LLDEB(bi->biLevel,DOClevSECT); return -1;	}
		if  ( lj->ljChangedItem->biInExternalItem == DOCinBODY )
		    { break; }

		if  ( docRedoBodyItemLayout( bi, lj ) )
		    { LDEB(1); return -1;	}

		break;

	    case DOCinFOOTNOTE:
	    case DOCinENDNOTE:

		if  ( bi->biLevel != DOClevSECT )
		    { LLDEB(bi->biLevel,DOClevSECT); return -1;	}
		if  ( lj->ljChangedItem->biInExternalItem == DOCinBODY )
		    { break; }

		if  ( docRedoBodyItemLayout( bi, lj ) )
		    { LDEB(1); return -1;	}

		break;

	    case DOCinFTNSEP:
	    case DOCinFTNSEPC:
	    case DOCinFTNCN:
	    case DOCinAFTNSEP:
	    case DOCinAFTNSEPC:
	    case DOCinAFTNCN:

		if  ( bi->biLevel != DOClevSECT )
		    { LLDEB(bi->biLevel,DOClevSECT); return -1;	}
		if  ( lj->ljChangedItem->biInExternalItem == DOCinBODY )
		    { break; }

		if  ( docRedoBodyItemLayout( bi, lj ) )
		    { LDEB(1); return -1;	}

		break;

	    case DOCinSHPTXT:
		/*nothing*/
		break;

	    default:
		LDEB(bi->biInExternalItem);
		break;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Do the layout of the document root item.				*/
/*									*/
/************************************************************************/

static int docLayoutDocItem(		BufferItem *		docBi,
					BlockFrame *		bf,
					LayoutJob *		lj )
    {
    BufferDocument *		bd= lj->ljBd;
    const DocumentProperties *	dp= &(bd->bdProperties);
    const NotesProperties *	npEndnotes= &(dp->dpEndnoteProperties);
    const DocumentGeometry *	dgDoc= &(dp->dpGeometry);

    int				i;

    if  ( docExternalItemPrelayout( &(bd->bdEiFtnsep), dgDoc, lj ) )
	{ LDEB(1); return -1;	}
    if  ( docExternalItemPrelayout( &(bd->bdEiFtnsepc), dgDoc, lj ) )
	{ LDEB(1); return -1;	}
    if  ( docExternalItemPrelayout( &(bd->bdEiFtncn), dgDoc, lj ) )
	{ LDEB(1); return -1;	}

    if  ( docExternalItemPrelayout( &(bd->bdEiAftnsep), dgDoc, lj ) )
	{ LDEB(1); return -1;	}
    if  ( docExternalItemPrelayout( &(bd->bdEiAftnsepc), dgDoc, lj ) )
	{ LDEB(1); return -1;	}
    if  ( docExternalItemPrelayout( &(bd->bdEiAftncn), dgDoc, lj ) )
	{ LDEB(1); return -1;	}

    for ( i= 0; i < docBi->biChildCount; i++ )
	{
	if  ( docLayoutItemImplementation( docBi->biChildren[i], bf, lj ) )
	    { LDEB(1); return -1;	}
	}

    if  ( docBi->biInExternalItem == DOCinBODY		&&
	  npEndnotes->npPosition == FTN_POS_DOC_END	)
	{
	if  ( docLayoutEndnotesForDocument( bf, lj ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Do the layout of a document item.					*/
/*									*/
/*  This is the main entry poin of the formatter.			*/
/*									*/
/************************************************************************/

int docLayoutItemImplementation(	BufferItem *		bi,
					BlockFrame *		bf,
					LayoutJob *		lj )
    {
    DocumentRectangle *		drChanged= lj->ljChangedRectanglePixels;
    int				i;
    int				y1;

    ParagraphLayoutContext	plc;

    plc.plcChangedRectanglePixels= lj->ljChangedRectanglePixels;
    plc.plcAdd= lj->ljAdd;
    plc.plcBd= lj->ljBd;
    plc.plcScreenFontList= lj->ljScreenFontList;

    plc.plcScreenLayout= lj->ljLayoutScreen;
    plc.plcStartParagraph= docLayoutParaInit;
    plc.plcAdjustBottom= docPsLayoutAdjustBottom;

    y1= BI_BELOW_PIXELS( lj->ljAdd, bi )- 1;

    i= BI_TOP_PIXELS( lj->ljAdd, bi );

    if  ( drChanged				&&
	  drChanged->drY0 > i	)
	{ drChanged->drY0=  i;	}

    bi->biTopPosition= lj->ljPosition;

    if  ( drChanged )
	{
	i= BI_TOP_PIXELS( lj->ljAdd, bi );

	if  ( drChanged->drY0 > i )
	    { drChanged->drY0=  i;			}
	if  ( drChanged->drY1 < y1 )
	    { drChanged->drY1=  y1;	}
	}

    switch( bi->biLevel )
	{
	case DOClevDOC:

	    if  ( docLayoutDocItem( bi, bf, lj ) )
		{ LDEB(1); return -1;	}
	    break;

	rowAsGroup:
	sectAsGroup:
	    for ( i= 0; i < bi->biChildCount; i++ )
		{
		if  ( docLayoutItemImplementation( bi->biChildren[i], bf, lj ) )
		    { LDEB(1); return -1;	}
		}
	    if  ( bi->biChildCount > 0 )
		{ bi->biTopPosition= bi->biChildren[0]->biTopPosition;	}
	    break;

	case DOClevCELL:
	    {
	    ParagraphLayoutJob		plj;
	    const int			para= 0;
	    const int			line= 0;
	    const int			part= 0;

	    docInitParagraphLayoutJob( &plj );

	    docPsBeginParagraphLayoutProgress( &plj,
					para, line, part, bi->biChildCount,
					&(lj->ljPosition) );

	    if  ( docLayoutParagraphs( &plc, bi, bf, lj, &plj ) )
		{ LDEB(1); return -1;	}

	    lj->ljPosition= plj.pljPos.plpPos;
	    }
	    break;

	case DOClevSECT:
	    if  ( ! bi->biParent )
		{ goto sectAsGroup;	}

	    if  ( docLayoutSectItem( bi, bf, lj ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevROW:
	    if  ( ! bi->biRowHasTableParagraphs )
		{ goto rowAsGroup;	}

	    if  ( docLayoutRowItem( bi, bf, lj, &plc ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevPARA:
	    if  ( docLayoutParaItem( bi, bf, lj, &plc ) )
		{ LDEB(1); return -1;	}

	    break;

	default:
	    LDEB(bi->biLevel); return -1;
	}

    bi->biBelowPosition= lj->ljPosition;

    y1= BI_BELOW_PIXELS( lj->ljAdd, bi )- 1;

    if  ( drChanged		&&
	  drChanged->drY1 < y1	)
	{ drChanged->drY1=  y1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Recalculate the layout for a block in the document hierarchy.	*/
/*									*/
/*  1)  If we do not have to recalculate the layout of the document as	*/
/*	a whole..							*/
/*  2)  Calculate the frame in which the text is to be laid out.	*/
/*  3)  If the preceding paragraph ends on the same page where this	*/
/*	nodes begins, reserve space for the footnotes upto the		*/
/*	beginning of this block and subtract the height from the buttom	*/
/*	of the frame.							*/
/*  4)  Perform the actual layout operation.				*/
/*  5)  Adjust the positions of the parent nodes and any children of	*/
/*	the parent below this node.					*/
/*									*/
/************************************************************************/

static int docLayoutGetInitialFrame(	BlockFrame *		bf,
					const LayoutJob *	lj,
					BufferItem *		bi )
    {
    BufferDocument *	bd= lj->ljBd;
    const BufferItem *	prevParaBi= (const BufferItem *)0;

    /*  2  */
    docBlockFrameTwips( bf, bi, bd,
			lj->ljPosition.lpPage, lj->ljPosition.lpColumn );

    /*  3  */
    if  ( bi->biInExternalItem == DOCinBODY )
	{ prevParaBi= docPrevParagraph( bi );	}

    if  ( prevParaBi						&&
	  prevParaBi->biBelowPosition.lpPage >=
					lj->ljPosition.lpPage	)
	{
	DocumentPosition		dpHere;
	int				partHere;

	if  ( docFirstPosition( &dpHere, bi ) )
	    { LDEB(1); return -1;	}
	partHere= 0;

	if  ( docCollectFootnotesForColumn( bf, &dpHere, partHere, lj ) )
	    { LDEB(lj->ljPosition.lpPage); return -1;	}
	}

    return 0;
    }

int docLayoutItemAndParents(	BufferItem *		bi,
				LayoutJob *		lj )
    {
    BlockFrame			bf;

    docLayoutInitBlockFrame( &bf );

    /*  1  */
    if  ( bi->biLevel != DOClevDOC			&&
	  docLayoutGetInitialFrame( &bf, lj, bi )	)
	{ LDEB(1); return -1;	}

    /*  4  */
    if  ( docLayoutItemImplementation( bi, &bf, lj ) )
	{ LDEB(1); return -1;	}

    /*  5  */
    if  ( docAdjustParentGeometry( bi, &bf, lj ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Redo the layout of a paragraph after an editing action.		*/
/*									*/
/*  To avoid screen flashing, a minimalistic approach is used: As	*/
/*  little of the layout is redone, and an administration is kept of	*/
/*  what has been changed, in order to redraw a minimal screen		*/
/*  rectangle.								*/
/*									*/
/*  a)  Shift the offsets of the lines in the rest of the paragraph.	*/
/*	This means that we optimistically assume that the change in the	*/
/*	text of the paragraph only affected the current line, and that	*/
/*	the lines below are unchanged. This assumption is verified by	*/
/*	docLayoutLines() so out naive optimism does no harm.		*/
/*  b)  Invalidate the column with of the lines that are to be		*/
/*	reformatted anyway to force a reformat by docLayoutLines().	*/
/*	[stroffUpto is in terms of the new line and particule offsets.]	*/
/*	Below, fromLine can change value. This forced invalidation	*/
/*	refers to the original value however: The beginning of the	*/
/*	edited stretch of text.						*/
/*  c)  Remember the end of the reformatting range.			*/
/*  d)  With widow/orphan control enabled, and at the top of a page,	*/
/*	start a little earlier to make sure that all special cases are	*/
/*	covered by the regular formatter algorithm.			*/
/*  2)  Calculate the frame in which the text is to be laid out.	*/
/*  3)  Reserve space for the footnotes upto the beginning of this line	*/
/*	and subtract the height from the buttom of the frame.		*/
/*  6)  Set the start position for the layout operation to the current	*/
/*	line. Try only to redo the layout of the current paragraph.	*/
/*									*/
/************************************************************************/

int docAdjustParaLayout(	BufferItem *		paraBi,
				int			fromLine,
				int			stroffShift,
				int			stroffUpto,
				LayoutJob *		lj )
    {
    AppDrawingData *		add= lj->ljAdd;
    ScreenFontList *		sfl= lj->ljScreenFontList;
    BufferDocument *		bd= lj->ljBd;

    BlockFrame			bf;
    int				fromPart;
    int				line;
    int				advanced= 0;

    const int			bottomTwips= -1;
    const int			stripHigh= -1;

    ParagraphLayoutJob		plj;
    ParagraphLayoutContext	plc;

    LayoutPosition		oldLpBelow;
    int				paraUpto;
    const BufferItem *		prevParaBi= (const BufferItem *)0;
    BufferItem *		cellBi= paraBi->biParent;

    int				fromPara;
    BufferItem *		biParaFrom;

    if  ( paraBi->biLevel != DOClevPARA )
	{ LLDEB(paraBi->biLevel,DOClevPARA); return -1; }
    if  ( fromLine < 0 || fromLine >= paraBi->biParaLineCount )
	{ LLDEB(fromLine, paraBi->biParaLineCount); return -1;	}

    /*  a,b  */
    {
    TextLine *	tlShift;

    tlShift= paraBi->biParaLines+ fromLine;
    tlShift->tlFrameWidthTwips= 0;
    tlShift->tlFrameX0Twips= 0;
    tlShift++;
    for ( line= fromLine+ 1; line < paraBi->biParaLineCount; tlShift++, line++ )
	{
	tlShift->tlStroff += stroffShift;

	if  ( tlShift->tlStroff < stroffUpto )
	    {
	    tlShift->tlFrameWidthTwips= 0;
	    tlShift->tlFrameX0Twips= 0;
	    }
	}
    }

    /*  c  */
    oldLpBelow= paraBi->biBelowPosition;
    paraUpto= paraBi->biNumberInParent+ 1;

    plc.plcChangedRectanglePixels= lj->ljChangedRectanglePixels;
    plc.plcAdd= lj->ljAdd;
    plc.plcBd= lj->ljBd;
    plc.plcScreenFontList= lj->ljScreenFontList;

    plc.plcScreenLayout= lj->ljLayoutScreen;
    plc.plcStartParagraph= docLayoutParaInit;
    plc.plcAdjustBottom= docPsLayoutAdjustBottom;

    docLayoutInitBlockFrame( &bf );

    if  ( paraBi->biNumberInParent > 0 )
	{ prevParaBi= cellBi->biChildren[paraBi->biNumberInParent- 1]; }

    /*  d  */
    biParaFrom= paraBi;
    fromPara= biParaFrom->biNumberInParent;
    if  ( paraBi->biParaWidowControl	&&
	  fromLine < 3			&&
	  paraBi->biNumberInParent > 0	)
	{
	fromLine= 0;
	fromPart= 0;

	while( prevParaBi->biNumberInParent > 0 )
	    {
	    prevParaBi= cellBi->biChildren[prevParaBi->biNumberInParent- 1];

	    fromPara--;
	    biParaFrom= cellBi->biChildren[fromPara];

	    if  ( ! prevParaBi->biParaKeepWithNext )
		{ break;	}
	    }

	lj->ljPosition= prevParaBi->biBelowPosition;
	}
    else{
	const TextLine *	tl;

	tl= paraBi->biParaLines+ fromLine;
	fromPart= tl->tlFirstParticule;
	if  ( fromPart == 0 )
	    {
	    if  ( paraBi->biNumberInParent == 0 )
		{ lj->ljPosition= paraBi->biTopPosition;	}
	    else{ lj->ljPosition= prevParaBi->biBelowPosition;	}
	    }
	else{ lj->ljPosition= tl->tlTopPosition;		}
	}

    /*  2  */
    docBlockFrameTwips( &bf, biParaFrom, bd,
			lj->ljPosition.lpPage, lj->ljPosition.lpColumn );

    /*  3  */
    if  ( biParaFrom->biInExternalItem == DOCinBODY )
	{
	TextLine *			tlHere;
	DocumentPosition		dpHere;

	tlHere= biParaFrom->biParaLines+ fromLine;
	dpHere.dpBi= biParaFrom;
	dpHere.dpStroff= tlHere->tlStroff;

	if  ( docCollectFootnotesForColumn( &bf, &dpHere, fromPart, lj ) )
	    { LDEB(lj->ljPosition.lpPage); return -1;	}
	}

    docInitParagraphLayoutJob( &plj );

    docParagraphFrameTwips( &(plj.pljPos.plpFormattingFrame), &bf,
					bottomTwips, stripHigh, biParaFrom );

    if  ( docPsParagraphLineExtents( bd, add->addPostScriptFontList,
								biParaFrom ) )
	{ LDEB(1); return -1;	}

    if  ( lj->ljLayoutScreen.slScreenFrame )
	{
	(*lj->ljLayoutScreen.slScreenFrame)( &(plj.pljPos.plpFormattingFrame),
							    add, biParaFrom );
	}

    if  ( lj->ljLayoutScreen.slStartParagraph				&&
	  (*lj->ljLayoutScreen.slStartParagraph)( biParaFrom,
		    &(plj.pljPos.plpFormattingFrame), add, sfl, bd )	)
	{ LDEB(1); return -1;	}

    /*  6  */
    {
    docPsBeginParagraphLayoutProgress( &plj,
			biParaFrom->biNumberInParent, fromLine, fromPart,
			paraUpto, &(lj->ljPosition) );
    }

    docPsAdvanceParagraphLayout( &advanced,
				    &(plj.pljPos0.plpProgress),
				    &(plj.pljPos0.plpProgress),
				    &(plj.pljPos.plpProgress),
				    lj->ljPosition.lpPage, cellBi );

    if  ( docLayoutParagraphs( &plc, cellBi, &bf, lj, &plj ) )
	{ LDEB(1); return -1;	}

    lj->ljPosition= plj.pljPos.plpPos;

    if  ( paraBi->biParaLineCount < 1 )
	{ LDEB(paraBi->biParaLineCount); docListItem(0,paraBi); return -1; }

    if  ( ! DOC_SAME_POSITION( &(paraBi->biBelowPosition),
						    &(lj->ljPosition) ) )
	{
	LLDEB(paraBi->biBelowPosition.lpPage,lj->ljPosition.lpPage);
	LDEB(paraBi->biBelowPosition.lpPageYTwips);
	LDEB(lj->ljPosition.lpPageYTwips);

	paraBi->biBelowPosition= lj->ljPosition;
	}

    if  ( ! DOC_SAME_POSITION( &(paraBi->biBelowPosition), &oldLpBelow ) )
	{
	/*  5  */
	if  ( docAdjustParentGeometry( paraBi, &bf, lj ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }
