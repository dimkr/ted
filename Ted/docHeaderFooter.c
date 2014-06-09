/************************************************************************/
/*									*/
/*  Get headers and footers for a certain page/position.		*/
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
/*  Determine what header/footer applies for a certain page.		*/
/*									*/
/************************************************************************/

static int docCheckItemIsEmpty(	int *			pIsEmpty,
				const ExternalItem *	ei )
    {
    int		isEmpty= 1;

    if  ( ei && ei->eiItem )
	{
	DocumentPosition		dpBegin;
	DocumentPosition		dpEnd;

	if  ( docFirstPosition( &dpBegin, ei->eiItem ) )
	    { LDEB(1); docInitDocumentPosition( &dpBegin );	}
	if  ( docLastPosition( &dpEnd, ei->eiItem ) )
	    { LDEB(1); docInitDocumentPosition( &dpEnd );	}

	if  ( ! docSamePosition( &dpBegin, &dpEnd ) )
	    { isEmpty= 0;	}
	else{
	    const ParagraphProperties *	pp= &(dpBegin.dpBi->biParaProperties);

	    if  ( pp->ppInTable )
		{ isEmpty= 0;	}

	    if  ( DOCisBORDER( &(pp->ppTopBorder) ) )
		{ isEmpty= 0;	}
	    if  ( DOCisBORDER( &(pp->ppBottomBorder) ) )
		{ isEmpty= 0;	}
	    if  ( DOCisBORDER( &(pp->ppLeftBorder) ) )
		{ isEmpty= 0;	}
	    if  ( DOCisBORDER( &(pp->ppRightBorder) ) )
		{ isEmpty= 0;	}
	    if  ( DOCisBORDER( &(pp->ppBetweenBorder) ) )
		{ isEmpty= 0;	}
	    if  ( DOCisBORDER( &(pp->ppBar) ) )
		{ isEmpty= 0;	}

	    if  ( DOCisSHADING( &(pp->ppShading) ) )
		{ isEmpty= 0;	}
	    }
	}

    *pIsEmpty= isEmpty;
    return 0;
    }

int docWhatPageHeader(	ExternalItem **			pEi,
			int *				pIsEmpty,
			BufferItem *			bodySectBi,
			int				page,
			const DocumentProperties *	dp )
    {
    const SectionProperties *	sp= &(bodySectBi->biSectProperties);

    if  ( page == bodySectBi->biTopPosition.lpPage && sp->spHasTitlePage )
	{
	*pEi= &(bodySectBi->biSectFirstPageHeader);
	docCheckItemIsEmpty( pIsEmpty, *pEi );
	return DOCinFIRST_HEADER;
	}

    if  ( ! dp->dpHasFacingPages )
	{
	*pEi= &(bodySectBi->biSectHeader);
	docCheckItemIsEmpty( pIsEmpty, *pEi );
	return DOCinSECT_HEADER;
	}

    if  ( page % 2 )
	{
	*pEi= &(bodySectBi->biSectLeftPageHeader);
	docCheckItemIsEmpty( pIsEmpty, *pEi );
	return DOCinLEFT_HEADER;
	}
    else{
	*pEi= &(bodySectBi->biSectRightPageHeader);
	docCheckItemIsEmpty( pIsEmpty, *pEi );
	return DOCinRIGHT_HEADER;
	}
    }

int docWhatPageFooter(	ExternalItem **			pEi,
			int *				pIsEmpty,
			BufferItem *			bodySectBi,
			int				page,
			const DocumentProperties *	dp )
    {
    const SectionProperties *	sp= &(bodySectBi->biSectProperties);

    if  ( page == bodySectBi->biTopPosition.lpPage && sp->spHasTitlePage )
	{
	*pEi= &(bodySectBi->biSectFirstPageFooter);
	docCheckItemIsEmpty( pIsEmpty, *pEi );
	return DOCinFIRST_FOOTER;
	}

    if  ( ! dp->dpHasFacingPages )
	{
	*pEi= &(bodySectBi->biSectFooter);
	docCheckItemIsEmpty( pIsEmpty, *pEi );
	return DOCinSECT_FOOTER;
	}

    if  ( page % 2 )
	{
	*pEi= &(bodySectBi->biSectLeftPageFooter);
	docCheckItemIsEmpty( pIsEmpty, *pEi );
	return DOCinLEFT_FOOTER;
	}
    else{
	*pEi= &(bodySectBi->biSectRightPageFooter);
	docCheckItemIsEmpty( pIsEmpty, *pEi );
	return DOCinRIGHT_FOOTER;
	}
    }

/************************************************************************/
/*									*/
/*  Return a particular header or footer by scope.			*/
/*									*/
/************************************************************************/

ExternalItem *	docSectionHeaderFooter(	BufferItem *		bodySectBi,
					int			which )
    {
    switch( which )
	{
	case DOCinBODY:
	    LDEB(which); return (ExternalItem *)0;

	case DOCinSECT_HEADER:
	    return &(bodySectBi->biSectHeader);

	case DOCinFIRST_HEADER:
	    return &(bodySectBi->biSectFirstPageHeader);

	case DOCinLEFT_HEADER:
	    return &(bodySectBi->biSectLeftPageHeader);

	case DOCinRIGHT_HEADER:
	    return &(bodySectBi->biSectRightPageHeader);

	case DOCinSECT_FOOTER:
	    return &(bodySectBi->biSectFooter);

	case DOCinFIRST_FOOTER:
	    return &(bodySectBi->biSectFirstPageFooter);

	case DOCinLEFT_FOOTER:
	    return &(bodySectBi->biSectLeftPageFooter);

	case DOCinRIGHT_FOOTER:
	    return &(bodySectBi->biSectRightPageFooter);

	default:
	    LDEB(which); return (ExternalItem *)0;
	}
    }

int docSectionHeaderFooterFirstPage(
				int *				pExists,
				const BufferItem *		bodySectBi,
				int				which,
				const DocumentProperties *	dp )
    {
    const SectionProperties *	sp= &(bodySectBi->biSectProperties);
    int				topPage= bodySectBi->biTopPosition.lpPage;
    int				belowPage= bodySectBi->biBelowPosition.lpPage;
    int				page;

    const BufferItem *		prevBi= (const BufferItem *)0;

    if  ( bodySectBi->biNumberInParent > 0 )
	{
	prevBi= bodySectBi->biParent->biChildren[
					    bodySectBi->biNumberInParent- 1];

	if  ( prevBi->biBelowPosition.lpPage >= belowPage )
	    { *pExists= 0; return -1;	}
	}

    switch( which )
	{
	case DOCinBODY:
	    LDEB(which); *pExists= 0; return -1;

	case DOCinSECT_HEADER:
	case DOCinSECT_FOOTER:
	    if  ( dp->dpHasFacingPages )
		{ *pExists= 0; return -1;	}
	    if  ( sp->spHasTitlePage )
		{ page= topPage+ 1;	}
	    else{ page= topPage;	}
	    break;

	case DOCinFIRST_HEADER:
	case DOCinFIRST_FOOTER:
	    if  ( ! sp->spHasTitlePage )
		{ *pExists= 0; return -1;	}

	    if  ( prevBi && prevBi->biBelowPosition.lpPage >= topPage )
		{ *pExists= 0; return topPage;
		}
	    page= topPage;
	    break;

	case DOCinLEFT_HEADER:
	case DOCinLEFT_FOOTER:
	    if  ( ! dp->dpHasFacingPages )
		{ *pExists= 0; return -1;	}
	    if  ( topPage % 2 )
		{
		if  ( sp->spHasTitlePage )
		    { page= topPage+ 2;	}
		else{ page= topPage;	}
		}
	    else{ page= topPage+ 1;	}
	    break;

	case DOCinRIGHT_HEADER:
	case DOCinRIGHT_FOOTER:
	    if  ( ! dp->dpHasFacingPages )
		{ *pExists= 0; return -1;	}
	    if  ( ! ( topPage % 2 ) )
		{
		if  ( sp->spHasTitlePage )
		    { page= topPage+ 2;	}
		else{ page= topPage;	}
		}
	    else{ page= topPage+ 1;	}
	    break;

	default:
	    LDEB(which); *pExists= 0; return -1;
	}

    *pExists= page <= bodySectBi->biBelowPosition.lpPage;

    return page;
    }

/************************************************************************/
/*									*/
/*  Return a particular separator of notice by scope.			*/
/*									*/
/************************************************************************/

ExternalItem *	docDocumentNoteSeparator(
					BufferDocument *	bd,
					int			which )
    {
    switch( which )
	{
	case DOCinFTNSEP:
	    return &(bd->bdEiFtnsep);

	case DOCinFTNSEPC:
	    return &(bd->bdEiFtnsepc);

	case DOCinFTNCN:
	    return &(bd->bdEiFtncn);

	case DOCinAFTNSEP:
	    return &(bd->bdEiAftnsep);

	case DOCinAFTNSEPC:
	    return &(bd->bdEiAftnsepc);

	case DOCinAFTNCN:
	    return &(bd->bdEiAftncn);

	default:
	    LDEB(which); return (ExternalItem *)0;
	}
    }

/************************************************************************/
/*									*/
/*  Get the external item that an item is contained in.			*/
/*									*/
/************************************************************************/

int docGetRootOfSelectionScope(	ExternalItem **		pEi,
				BufferItem **		pSelRootBi,
				BufferItem **		pBodySectBi,
				BufferDocument *	bd,
				const SelectionScope *	ss )
    {
    BufferItem *	docBi= &(bd->bdItem);
    BufferItem *	bodySectBi;
    BufferItem *	selRootBi;
    ExternalItem *	ei;

    int			noteIndex;
    DocumentNote *	dn;

    switch( ss->ssInExternalItem )
	{
	case DOCinBODY:
	    selRootBi= docBi;
	    bodySectBi= (BufferItem *)0;
	    ei= (ExternalItem *)0;
	    break;

	case DOCinSECT_HEADER:
	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:

	case DOCinSECT_FOOTER:
	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:
	    bodySectBi= docBi->biChildren[ss->ssSectNrExternalTo];
	    if  ( ! bodySectBi || bodySectBi->biLevel != DOClevSECT )
		{ XDEB(bodySectBi); return -1;	}

	    ei= docSectionHeaderFooter( bodySectBi, ss->ssInExternalItem );
	    if  ( ! ei )
		{ LXDEB(ss->ssInExternalItem,ei); return -1;	}
	    selRootBi= ei->eiItem;

	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:
	    noteIndex= ss->ssNoteArrayIndex;
	    if  ( noteIndex < 0 || noteIndex >= bd->bdNoteCount )
		{
		LLDEB(noteIndex,bd->bdNoteCount);
		return -1;
		}
	    dn= bd->bdNotes+ noteIndex;
	    ei= &(dn->dnExternalItem);
	    selRootBi= ei->eiItem;

	    if  ( dn->dnSectNr < 0 || dn->dnSectNr >= docBi->biChildCount )
		{ LLDEB(dn->dnSectNr,docBi->biChildCount); return -1; }

	    bodySectBi= docBi->biChildren[dn->dnSectNr];
	    if  ( ! bodySectBi || bodySectBi->biLevel != DOClevSECT )
		{ XDEB(bodySectBi); return -1;	}

	    break;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:
	    ei= docDocumentNoteSeparator( bd, ss->ssInExternalItem );
	    if  ( ! ei )
		{ LXDEB(ss->ssInExternalItem,ei); return -1;	}
	    selRootBi= ei->eiItem;
	    bodySectBi= (BufferItem *)0;
	    break;

	default:
	    LDEB(ss->ssInExternalItem);
	    return -1;
	}

    *pEi= ei;
    *pSelRootBi= selRootBi;
    *pBodySectBi= bodySectBi;

    return 0;
    }

int docGetExternalItem(		ExternalItem **		pEi,
				BufferItem **		pBodySectBi,
				BufferDocument *	bd,
				BufferItem *		bi )
    {
    BufferItem *	selRootBi;

    BufferItem *	selSectBi;
    BufferItem *	bodySectBi;

    ExternalItem *	ei;

    SelectionScope *	ss;

    while( bi && bi->biLevel != DOClevSECT )
	{ bi= bi->biParent;	}

    if  ( ! bi )
	{ XDEB(bi); return -1;	}

    selSectBi= bi;
    ss= &(selSectBi->biSectSelectionScope);

    if  ( docGetRootOfSelectionScope( &ei, &selRootBi, &bodySectBi, bd, ss ) )
	{ LDEB(1); return -1;	}

    if  ( ss->ssInExternalItem == DOCinBODY )
	{ bodySectBi= selSectBi;	}

    *pEi= ei;
    *pBodySectBi= bodySectBi;

    return 0;
    }

/************************************************************************/
/*									*/
/*  What BufferItem is the common root of the selection.		*/
/*									*/
/************************************************************************/

BufferItem * docGetSelectionRoot(
			ExternalItem **			pEi,
			BufferItem **			pBodySectBi,
			BufferDocument *		bd,
			const DocumentSelection *	ds )
    {
    BufferItem *	bi1= ds->dsBegin.dpBi;
    BufferItem *	bi2= ds->dsEnd.dpBi;

    ExternalItem *	ei= (ExternalItem *)0;
    BufferItem *	selRootBi= (BufferItem *)0;
    BufferItem *	bodySectBi= (BufferItem *)0;

    if  ( ! bi1 || ! bi2 )
	{ XXDEB(bi1,bi2); return (BufferItem *)0;	}

    while( bi1 && bi1->biLevel > bi2->biLevel )
	{ bi1= bi1->biParent;	}

    if  ( ! bi1 )
	{
	XDEB(bi1);
	LLDEB(ds->dsBegin.dpBi->biLevel,ds->dsEnd.dpBi->biLevel);
	/*
	return (BufferItem *)0;
	*/
	}

    while( bi2 && bi2->biLevel > bi1->biLevel )
	{ bi2= bi2->biParent;	}

    while( bi1					&&
	   bi2					&&
	   bi1 != bi2				)
	{ bi1= bi1->biParent; bi2= bi2->biParent; }

    if  ( ! bi1 || ! bi2 )
	{ XXDEB(bi1,bi2); return (BufferItem *)0;	}

    selRootBi= bi1;

    if  ( selRootBi->biInExternalItem != DOCinBODY )
	{
	if  ( docGetExternalItem( &ei, &bodySectBi, bd, bi1 ) )
	    { LDEB(1); return (BufferItem *)0;	}
	}

    if  ( pEi )
	{ *pEi= ei;			}
    if  ( pBodySectBi )
	{ *pBodySectBi= bodySectBi;	}

    return selRootBi;
    }

/************************************************************************/
/*									*/
/*  Return the Header/Footer corrsponding to 'which'. If the selection	*/
/*  itself is in an external item, go to the section in the body to	*/
/*  get the header or footer.						*/
/*									*/
/************************************************************************/

int docGetHeaderFooter(		ExternalItem **			pEi,
				BufferItem **			pBodySectBi,
				const DocumentSelection *	ds,
				BufferDocument *		bd,
				int				which )
    {
    ExternalItem *	ei;
    BufferItem *	bodySectBi;

    if  ( docGetExternalItem( &ei, &bodySectBi, bd, ds->dsBegin.dpBi ) )
	{ LDEB(1); return -1;	}

    ei= docSectionHeaderFooter( bodySectBi, which );
    if  ( ! ei )
	{ XDEB(ei); return -1;	}

    *pEi= ei; *pBodySectBi= bodySectBi; return 0;
    }

/************************************************************************/
/*									*/
/*  Determine whether that document has headers and footers at all.	*/
/*									*/
/************************************************************************/

int docInquireHeadersFooters(	int *			pDocHasHeaders,
				int *			pDocHasFooters,
				const BufferItem *	docBi )
    {
    int			i;
    int			hasPageHeader= 0;
    int			hasPageFooter= 0;

    for ( i= 0; i < docBi->biChildCount; i++ )
	{
	int			j;
	BufferItem *		sectBi= docBi->biChildren[i];

	for ( j= 0; j < PAGES__COUNT; j++ )
	    {
	    ExternalItem *	ei;

	    ei= docSectionHeaderFooter( sectBi, DOC_HeaderScopes[j] );
	    if  ( ei && ei->eiItem )
		{ hasPageHeader= 1;	}

	    ei= docSectionHeaderFooter( sectBi, DOC_FooterScopes[j] );
	    if  ( ei && ei->eiItem )
		{ hasPageFooter= 1;	}
	    }
	}

    *pDocHasHeaders= hasPageHeader;
    *pDocHasFooters= hasPageFooter;
    return 0;
    }
