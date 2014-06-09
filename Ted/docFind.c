/************************************************************************/
/*									*/
/*  Find utility routines.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docFind.h"

/************************************************************************/
/*									*/
/*  Look for something in a document.					*/
/*									*/
/*  The 'Find Next' action scans the document in the following order:	*/
/*									*/
/*  1)  The body of the document.					*/
/*  2)  The external items of the sections of the document in the order	*/
/*	of the sections. [No attempt is made to start with a particular	*/
/*	section.]							*/
/*  3)  Per section, the headers and footers are tried in the order of	*/
/*	the 'DOC_FindHeaderFooters[]' array.				*/
/*  4)  Then the foot/endnotes are tried in the order in which they	*/
/*	appear in the internal administration of the document.		*/
/*									*/
/*  A search wraps (And will wrap indefinitely) when the tree in which	*/
/*  it started is left for the second time.				*/
/*									*/
/************************************************************************/

typedef struct DocumentFindJob
    {
    DocumentSelection	dfjStartSelection;
    DocumentPosition	dfjCurrentPosition;

    int			dfjExternalItemPage;

    int			dfjWrapCount;
    } DocumentFindJob;

static int DOC_FindHeaderFooters[]=
    {
    DOCinFIRST_HEADER,		DOCinFIRST_FOOTER,
    DOCinSECT_HEADER,		DOCinSECT_FOOTER,
    DOCinLEFT_HEADER,		DOCinLEFT_FOOTER,
    DOCinRIGHT_HEADER,		DOCinRIGHT_FOOTER,
    };

static int DOC_FindHeaderFooterCount= 
			sizeof(DOC_FindHeaderFooters)/sizeof(int);

/**/

static int DOC_FindDocNoteItems[]=
    {
    DOCinFTNSEP,	DOCinFTNSEPC,	DOCinFTNCN,
    DOCinAFTNSEP,	DOCinAFTNSEPC,	DOCinAFTNCN,
    };

static int DOC_FindDocNoteItemCount= 
			sizeof(DOC_FindDocNoteItems)/sizeof(int);

/************************************************************************/

static int docFindGetHeaderFooterIndex(	int	inExtIt )
    {
    int		i;

    for ( i= 0; i < DOC_FindHeaderFooterCount; i++ )
	{
	if  ( DOC_FindHeaderFooters[i] == inExtIt )
	    { return i;	}
	}

    LDEB(inExtIt); return -1;
    }

static int docFindGetDocNoteItemIndex(	int	inExtIt )
    {
    int		i;

    for ( i= 0; i < DOC_FindDocNoteItemCount; i++ )
	{
	if  ( DOC_FindDocNoteItems[i] == inExtIt )
	    { return i;	}
	}

    LDEB(inExtIt); return -1;
    }

/************************************************************************/

static ExternalItem * docFindGetHeaderFooter(
				BufferItem *			bodySectBi,
				const DocumentProperties *	dp,
				int				hf )
    {
    const SectionProperties *	sp= &(bodySectBi->biSectProperties);

    switch( DOC_FindHeaderFooters[hf] )
	{
	case DOCinFIRST_HEADER:
	    if  ( ! sp->spHasTitlePage )
		{ return (ExternalItem *)0;	}
	    return &(bodySectBi->biSectFirstPageHeader);

	case DOCinFIRST_FOOTER:
	    if  ( ! sp->spHasTitlePage )
		{ return (ExternalItem *)0;	}
	    return &(bodySectBi->biSectFirstPageFooter);

	case DOCinSECT_HEADER:
	    if  ( dp->dpHasFacingPages )
		{ return (ExternalItem *)0;	}
	    return &(bodySectBi->biSectHeader);

	case DOCinSECT_FOOTER:
	    if  ( dp->dpHasFacingPages )
		{ return (ExternalItem *)0;	}
	    return &(bodySectBi->biSectFooter);

	case DOCinLEFT_HEADER:
	    if  ( ! dp->dpHasFacingPages )
		{ return (ExternalItem *)0;	}
	    return &(bodySectBi->biSectLeftPageHeader);

	case DOCinLEFT_FOOTER:
	    if  ( ! dp->dpHasFacingPages )
		{ return (ExternalItem *)0;	}
	    return &(bodySectBi->biSectLeftPageFooter);

	case DOCinRIGHT_HEADER:
	    if  ( ! dp->dpHasFacingPages )
		{ return (ExternalItem *)0;	}
	    return &(bodySectBi->biSectRightPageHeader);

	case DOCinRIGHT_FOOTER:
	    if  ( ! dp->dpHasFacingPages )
		{ return (ExternalItem *)0;	}
	    return &(bodySectBi->biSectRightPageFooter);

	default:
	    LDEB(DOC_FindHeaderFooters[hf]);
	    return (ExternalItem *)0;
	}
    }

/************************************************************************/
/*									*/
/*  Initialize a find job.						*/
/*									*/
/*  NOTE that a document without a body is not always handeled		*/
/*	correctly. solve this in the caller by making an empty body. It	*/
/*	is not worth the hassle to find a work around here.		*/
/*									*/
/************************************************************************/

static int docFindInitFindJob(	DocumentFindJob *		dfj,
				int				reverse,
				const DocumentPosition *	dpFrom,
				BufferDocument *		bd )
    {
    dfj->dfjExternalItemPage= -1;

    if  ( dpFrom->dpBi )
	{
	dfj->dfjCurrentPosition= *dpFrom;

	if  ( dpFrom->dpBi->biInExternalItem != DOCinBODY )
	    {
	    ExternalItem *	ei;
	    BufferItem *	bodySectBi;

	    if  ( docGetExternalItem( &ei, &bodySectBi, bd, dpFrom->dpBi ) )
		{ LDEB(1); return -1;	}

	    dfj->dfjExternalItemPage= ei->eiPageSelectedUpon;
	    }
	}
    else{
	if  ( reverse )
	    {
	    if  ( docLastPosition( &(dfj->dfjCurrentPosition),
							&(bd->bdItem) ) )
		{ LDEB(1); return 1;	}
	    }
	else{
	    if  ( docFirstPosition( &(dfj->dfjCurrentPosition),
							&(bd->bdItem) ) )
		{ LDEB(1); return 1;	}
	    }
	}

    docInitDocumentSelection( &(dfj->dfjStartSelection) );
    docSetIBarSelection( &(dfj->dfjStartSelection),
					    &(dfj->dfjCurrentPosition) );

    dfj->dfjWrapCount= 0;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Get the next header or footer to look in for a find action.		*/
/*									*/
/************************************************************************/

static int docFindNextHeaderFooter(	DocumentPosition *	dpNew,
					int *			pExtItPage,
					BufferDocument *	bd,
					int			sectFrom,
					int			hfFrom )
    {
    BufferItem *		docBi= &(bd->bdItem);
    const DocumentProperties *	dp= &(bd->bdProperties);

    int				sect;
    BufferItem *		bodySectBi;
    int				hf;
    ExternalItem *		ei;

    for ( sect= sectFrom; sect < docBi->biChildCount; sect++ )
	{
	bodySectBi= docBi->biChildren[sect];

	for ( hf= hfFrom; hf < DOC_FindHeaderFooterCount; hf++ )
	    {
	    int			page;
	    int			exists;

	    ei= docFindGetHeaderFooter( bodySectBi, dp, hf );
	    if  ( ! ei || ! ei->eiItem )
		{ continue;	}

	    page= docSectionHeaderFooterFirstPage( &exists, bodySectBi,
					    DOC_FindHeaderFooters[hf], dp );

	    if  ( page < 0 || ! exists )
		{ continue;	}

	    if  ( docFirstPosition( dpNew, ei->eiItem ) )
		{
		SDEB(docExternalKindStr(DOC_FindHeaderFooters[hf]));
		continue;
		}

	    *pExtItPage= page;
	    return 0;
	    }

	hfFrom= 0;
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Get the next note to look in for a find action.			*/
/*									*/
/************************************************************************/

static int docFindNextNote(		DocumentPosition *	dp,
					BufferDocument *	bd,
					int			noteFrom )
    {
    DocumentNote *	dn;
    int			note;

    dn= bd->bdNotes+ noteFrom;
    for ( note= noteFrom; note < bd->bdNoteCount; dn++, note++ )
	{
	if  ( dn->dnParaNr < 0 )
	    { continue;		}
	if  ( ! dn->dnExternalItem.eiItem )
	    { continue;		}

	if  ( docFirstPosition( dp, dn->dnExternalItem.eiItem ) )
	    { LDEB(1); return -1;	}

	return 0;
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Get the next note item to look in for a find action.		*/
/*									*/
/************************************************************************/

static int docFindNextSeparator(	DocumentPosition *	dp,
					int *			pExtItPage,
					BufferDocument *	bd,
					int			sepFrom )
    {
    int			sep;

    for ( sep= sepFrom; sep < DOC_FindDocNoteItemCount; sep++ )
	{
	ExternalItem *		ei;
	DocumentNote *		dn;
	int			page= -1;

	ei= docDocumentNoteSeparator( bd, DOC_FindDocNoteItems[sep] );
	if  ( ! ei || ! ei->eiItem )
	    { continue;	}

	switch( DOC_FindDocNoteItems[sep] )
	    {
	    case DOCinFTNSEP:
		if  ( docGetFirstNoteOfDocument( &dn, bd, DOCinFOOTNOTE ) < 0 )
		    { continue;	}
		if  ( dn->dnReferringPage < 0 )
		    { continue;	}
		page= dn->dnReferringPage;
		break;

	    case DOCinFTNSEPC:
	    case DOCinFTNCN:
		/*LDEB(DOC_FindDocNoteItems[sep]);*/ continue;

	    case DOCinAFTNSEP:
		if  ( docGetFirstNoteOfDocument( &dn, bd, DOCinENDNOTE ) < 0 )
		    { continue;	}
		page= ei->eiItem->biTopPosition.lpPage;
		break;

	    case DOCinAFTNSEPC:
	    case DOCinAFTNCN:
		/*LDEB(DOC_FindDocNoteItems[sep]);*/ continue;

	    default:
		LDEB(DOC_FindDocNoteItems[sep]); return -1;
	    }

	if  ( docFirstPosition( dp, ei->eiItem ) )
	    { LDEB(1); return -1;	}

	*pExtItPage= page;
	return 0;
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Get the next tree to look in in a find action.			*/
/*									*/
/*  It is the responsibility of the caller to avoid loops. This routine	*/
/*  just returns the next tree indefinitely. (It wraps from the end to	*/
/*  the beginning.)							*/
/*									*/
/************************************************************************/

static int docFindNextNextTree(		DocumentPosition *	dp,
					int *			pPage,
					int			samePhase,
					BufferDocument *	bd )
    {
    int				phaseTo;
    int				phaseFrom;

    int				res;
    int				hfFrom;
    int				noteFrom;
    int				sectFrom;
    int				sepFrom;

    const BufferItem *		selSectBi;
    const SelectionScope *	ss;

    selSectBi= dp->dpBi;
    while( selSectBi && selSectBi->biLevel != DOClevSECT )
	{ selSectBi= selSectBi->biParent;	}
    if  ( ! selSectBi )
	{ XDEB(selSectBi); return -1;	}
    ss= &(selSectBi->biSectSelectionScope);

    phaseTo= 0;
    sectFrom= 0; hfFrom= 0; noteFrom= 0; sepFrom= 0;

    switch( ss->ssInExternalItem )
	{
	case DOCinBODY:

	    phaseFrom= 0;
	    phaseTo= 1;
	    break;

	case DOCinFIRST_HEADER:	case DOCinFIRST_FOOTER:
	case DOCinSECT_HEADER:	case DOCinSECT_FOOTER:
	case DOCinLEFT_HEADER:	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_HEADER:	case DOCinRIGHT_FOOTER:

	    phaseFrom= 1;
	    phaseTo= 1;
	    sectFrom= ss->ssSectNrExternalTo;
	    hfFrom= docFindGetHeaderFooterIndex( selSectBi->biInExternalItem );
	    if  ( hfFrom < 0 )
		{ LLDEB(selSectBi->biInExternalItem,hfFrom); return -1;	}
	    hfFrom++;

	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	    phaseFrom= 2;
	    phaseTo= 2;
	    noteFrom= ss->ssNoteArrayIndex+ 1;

	    break;

	case DOCinFTNSEP:	case DOCinAFTNSEP:
	case DOCinFTNSEPC:	case DOCinAFTNSEPC:
	case DOCinFTNCN:	case DOCinAFTNCN:

	    phaseFrom= 3;
	    phaseTo= 3;
	    sepFrom= docFindGetDocNoteItemIndex( selSectBi->biInExternalItem );
	    if  ( sepFrom < 0 )
		{ LLDEB(selSectBi->biInExternalItem,sepFrom); return -1; }
	    sepFrom++;

	    break;

	default:
	    LDEB(ss->ssInExternalItem);
	    LDEB(selSectBi->biInExternalItem);
	    return -1;
	}

    if  ( phaseTo <= 0 || phaseTo > 3 )
	{ LDEB(phaseTo); return -1;	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( phaseTo <= 1 )
	{
	res= docFindNextHeaderFooter( dp, pPage, bd, sectFrom, hfFrom );
	if  ( res < 0 )
	    { LDEB(res); return res;	}
	if  ( res == 0 )
	    { return res;	}

	phaseTo++;
	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( phaseTo <= 2 )
	{
	res= docFindNextNote( dp, bd, noteFrom );
	if  ( res < 0 )
	    { LDEB(res); return res;	}
	if  ( res == 0 )
	    { return res;	}

	phaseTo++;
	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( phaseTo <= 3 )
	{
	res= docFindNextSeparator( dp, pPage, bd, sepFrom );
	if  ( res < 0 )
	    { LDEB(res); return res;	}
	if  ( res == 0 )
	    { return res;	}

	phaseTo++;
	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( docFirstPosition( dp, &(bd->bdItem) ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Prepare for the next 'Find Prev' attempt.				*/
/*									*/
/************************************************************************/

static int docFindPrevHeaderFooter(	DocumentPosition *	dpNew,
					int *			pExtItPage,
					BufferDocument *	bd,
					int			sectFrom,
					int			hfFrom )
    {
    BufferItem *		docBi= &(bd->bdItem);
    const DocumentProperties *	dp= &(bd->bdProperties);

    int				sect;
    BufferItem *		bodySectBi;
    int				hf;
    ExternalItem *		ei;

    for ( sect= sectFrom; sect >= 0; sect-- )
	{
	bodySectBi= docBi->biChildren[sect];

	for ( hf= hfFrom; hf >= 0; hf-- )
	    {
	    int			page;
	    int			exists;

	    ei= docFindGetHeaderFooter( bodySectBi, dp, hf );
	    if  ( ! ei || ! ei->eiItem )
		{ continue;	}

	    page= docSectionHeaderFooterFirstPage( &exists, bodySectBi,
					    DOC_FindHeaderFooters[hf], dp );

	    if  ( page < 0 || ! exists )
		{ continue;	}

	    if  ( docLastPosition( dpNew, ei->eiItem ) )
		{ LDEB(1); return -1;	}

	    *pExtItPage= page;
	    return 0;
	    }

	hfFrom= DOC_FindHeaderFooterCount- 1;
	}

    return 1;
    }

static int docFindPrevNote(		DocumentPosition *	dp,
					BufferDocument *	bd,
					int			noteFrom )
    {
    DocumentNote *	dn;
    int			note;

    dn= bd->bdNotes+ noteFrom;
    for ( note= noteFrom; note >= 0; dn--, note-- )
	{
	if  ( dn->dnParaNr < 0 )
	    { continue;		}
	if  ( ! dn->dnExternalItem.eiItem )
	    { continue;		}

	if  ( docLastPosition( dp, dn->dnExternalItem.eiItem ) )
	    { LDEB(1); return -1;	}

	return 0;
	}

    return 1;
    }

static int docFindPrevSeparator(	DocumentPosition *	dp,
					int *			pExtItPage,
					BufferDocument *	bd,
					int			sepFrom )
    {
    int			sep;

    for ( sep= sepFrom; sep >= 0; sep-- )
	{
	ExternalItem *		ei;
	DocumentNote *		dn;
	int			page= -1;

	ei= docDocumentNoteSeparator( bd, DOC_FindDocNoteItems[sep] );
	if  ( ! ei || ! ei->eiItem )
	    { continue;	}

	switch( DOC_FindDocNoteItems[sep] )
	    {
	    case DOCinFTNSEP:
		if  ( docGetFirstNoteOfDocument( &dn, bd, DOCinFOOTNOTE ) < 0 )
		    { continue;	}
		if  ( dn->dnReferringPage < 0 )
		    { continue;	}
		page= dn->dnReferringPage;
		break;

	    case DOCinFTNSEPC:
	    case DOCinFTNCN:
		LDEB(DOC_FindDocNoteItems[sep]); continue;

	    case DOCinAFTNSEP:
	    case DOCinAFTNSEPC:
	    case DOCinAFTNCN:
		LDEB(DOC_FindDocNoteItems[sep]); continue;

	    default:
		LDEB(DOC_FindDocNoteItems[sep]); return -1;
	    }

	if  ( docLastPosition( dp, ei->eiItem ) )
	    { LDEB(1); return -1;	}

	return 0;
	}

    return 1;
    }

static int docFindPrevPrevTree(		DocumentPosition *	dp,
					int *			pPage,
					int			samePhase,
					BufferDocument *	bd )
    {
    int				phaseTo;
    int				phaseFrom;

    int				res;
    int				hfFrom;
    int				noteFrom;
    int				sectFrom;
    int				sepFrom;

    const BufferItem *		selSectBi;
    const SelectionScope *	ss;

    selSectBi= dp->dpBi;
    while( selSectBi && selSectBi->biLevel != DOClevSECT )
	{ selSectBi= selSectBi->biParent;	}
    if  ( ! selSectBi )
	{ XDEB(selSectBi); return -1;	}
    ss= &(selSectBi->biSectSelectionScope);

    noteFrom= bd->bdNoteCount- 1;
    sectFrom= bd->bdItem.biChildCount- 1;
    hfFrom= DOC_FindHeaderFooterCount- 1;
    sepFrom= DOC_FindDocNoteItemCount- 1;

    switch( ss->ssInExternalItem )
	{
	case DOCinBODY:

	    phaseFrom= 0;
	    phaseTo= 3;
	    break;

	case DOCinFIRST_HEADER:	case DOCinFIRST_FOOTER:
	case DOCinSECT_HEADER:	case DOCinSECT_FOOTER:
	case DOCinLEFT_HEADER:	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_HEADER:	case DOCinRIGHT_FOOTER:

	    phaseFrom= 1;
	    phaseTo= 1;
	    sectFrom= ss->ssSectNrExternalTo;
	    hfFrom= docFindGetHeaderFooterIndex( selSectBi->biInExternalItem );
	    if  ( hfFrom < 0 )
		{ LLDEB(selSectBi->biInExternalItem,hfFrom); return -1;	}

	    hfFrom--;

	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	    phaseFrom= 2;
	    phaseTo= 2;
	    noteFrom= ss->ssNoteArrayIndex- 1;

	    break;

	case DOCinFTNSEP:	case DOCinAFTNSEP:
	case DOCinFTNSEPC:	case DOCinAFTNSEPC:
	case DOCinFTNCN:	case DOCinAFTNCN:

	    phaseFrom= 3;
	    phaseTo= 3;
	    sepFrom= docFindGetDocNoteItemIndex( selSectBi->biInExternalItem );
	    if  ( sepFrom < 0 )
		{ LLDEB(selSectBi->biInExternalItem,sepFrom); return -1; }
	    sepFrom--;

	    break;

	default:
	    LDEB(ss->ssInExternalItem);
	    LDEB(selSectBi->biInExternalItem);
	    return -1;
	}

    if  ( phaseTo <= 0 || phaseTo > 3 )
	{ LDEB(phaseTo); return -1;	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( phaseTo >= 3 )
	{
	res= docFindPrevSeparator( dp, pPage, bd, sepFrom );
	if  ( res < 0 )
	    { LDEB(res); return res;	}
	if  ( res == 0 )
	    { return res;	}

	phaseTo--;
	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( phaseTo >= 2 )
	{
	res= docFindPrevNote( dp, bd, noteFrom );
	if  ( res < 0 )
	    { LDEB(res); return res;	}
	if  ( res == 0 )
	    { return res;	}

	phaseTo--;
	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( phaseTo >= 1 )
	{
	res= docFindPrevHeaderFooter( dp, pPage, bd, sectFrom, hfFrom );
	if  ( res < 0 )
	    { LDEB(res); return res;	}
	if  ( res == 0 )
	    { return res;	}

	phaseTo--;
	}

    if  ( samePhase && ( phaseTo != phaseFrom ) )
	{ return 1;	}

    if  ( docLastPosition( dp, &(bd->bdItem) ) )
	{ LDEB(1); return 1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find something forward/backward in one branch of a tree.		*/
/*									*/
/*  This can be done by walking over the leaf nodes of the tree.	*/
/*									*/
/************************************************************************/

int docFindFindNextInTree(	DocumentSelection *		ds,
				const BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				PARA_FIND_STRING		findNext,
				void *				through )
    {
    BufferItem *		bi= dpFrom->dpBi;

    int				ret;

    DocumentPosition		dpHere;

    if  ( bi->biLevel != DOClevPARA )
	{ LLDEB(bi->biLevel,DOClevPARA); return -1;	}

    dpHere= *dpFrom;

    for (;;)
	{
	ret= (*findNext)( ds, bi, bd, &dpHere, through );

	if  ( ret == 0 )
	    { return ret;		}
	if  ( ret < 0 )
	    { LDEB(ret); return ret;	}

	bi= docNextParagraph( bi );
	if  ( ! bi )
	    { return 1;	}
	if  ( docFirstPosition( &dpHere, bi ) )
	    { LDEB(1); return 1;	}
	}
    }

static int docFindFindPrevInTree(
				DocumentSelection *		ds,
				const BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				PARA_FIND_STRING		findPrev,
				void *				through )
    {
    BufferItem *		bi= dpFrom->dpBi;

    int				ret;

    DocumentPosition		dpHere;

    if  ( bi->biLevel != DOClevPARA )
	{ LLDEB(bi->biLevel,DOClevPARA); return -1;	}

    dpHere= *dpFrom;

    for (;;)
	{
	ret= (*findPrev)( ds, bi, bd, &dpHere, through );

	if  ( ret == 0 )
	    { return ret;		}
	if  ( ret < 0 )
	    { LDEB(ret); return ret;	}

	bi= docPrevParagraph( bi );
	if  ( ! bi )
	    { return 1;	}
	if  ( docLastPosition( &dpHere, bi ) )
	    { LDEB(1); return 1;	}
	}
    }

/************************************************************************/
/*									*/
/*  Look for the next occurrence of something in a whole document	*/
/*  including the section headers and footers.				*/
/*									*/
/************************************************************************/

int docFindFindNextInDocument(	DocumentSelection *		ds,
				const DocumentPosition *	dpFrom,
				BufferDocument *		bd,
				PARA_FIND_STRING		findNext,
				void *				through )
    {
    int				ret= -1;
    DocumentFindJob		dfj;
    DocumentSelection		dsNew;

    const int			reverse= 0;
    const int			samePhase= 0;

    docInitDocumentSelection( &dsNew );
    docFindInitFindJob( &dfj, reverse, dpFrom, bd );

    for (;;)
	{
	ret= docFindFindNextInTree( &dsNew, bd,
			    &(dfj.dfjCurrentPosition), findNext, through );

	if  ( ret < 0 )
	    { LDEB(ret); return ret;	}

	if  ( ret == 0 )
	    {
	    dsNew.dsDirection= 1;
	    dsNew.dsAnchor= dsNew.dsBegin;

	    docSetSelectionScope( &dsNew, dsNew.dsBegin.dpBi );

	    if  ( dsNew.dsBegin.dpBi->biInExternalItem != DOCinBODY )
		{
		ExternalItem *	ei;
		BufferItem *	bodySectBi;

		if  ( docGetExternalItem( &ei, &bodySectBi, bd,
							dsNew.dsBegin.dpBi ) )
		    { LDEB(1); return -1;	}

		ei->eiPageSelectedUpon= dfj.dfjExternalItemPage;
		}

	    *ds= dsNew;

	    return 0;
	    }

	if  ( docSelectionSameRoot( &(dfj.dfjStartSelection),
					    dfj.dfjCurrentPosition.dpBi ) )
	    {
	    if  ( dfj.dfjWrapCount > 0 )
		{ break;	}

	    dfj.dfjWrapCount++;
	    }

	ret= docFindNextNextTree( &(dfj.dfjCurrentPosition),
				&(dfj.dfjExternalItemPage), samePhase, bd );
	if  ( ret < 0 )
	    { LDEB(ret); return -1;	}
	if  ( ret > 0 )
	    { return ret;	}
	}

    return 1;
    }

int docFindFindPrevInDocument(	DocumentSelection *		ds,
				const DocumentPosition *	dpFrom,
				BufferDocument *		bd,
				PARA_FIND_STRING		findPrev,
				void *				through )
    {
    int				ret= -1;
    DocumentFindJob		dfj;
    DocumentSelection		dsNew;

    const int			reverse= 1;
    const int			samePhase= 0;

    docInitDocumentSelection( &dsNew );
    docFindInitFindJob( &dfj, reverse, dpFrom, bd );

    for (;;)
	{
	ret= docFindFindPrevInTree( &dsNew, bd,
			    &(dfj.dfjCurrentPosition), findPrev, through );

	if  ( ret < 0 )
	    { LDEB(ret); return ret;	}

	if  ( ret == 0 )
	    {
	    dsNew.dsDirection= -1;
	    dsNew.dsAnchor= dsNew.dsEnd;

	    docSetSelectionScope( &dsNew, dsNew.dsBegin.dpBi );

	    if  ( dsNew.dsBegin.dpBi->biInExternalItem != DOCinBODY )
		{
		ExternalItem *	ei;
		BufferItem *	bodySectBi;

		if  ( docGetExternalItem( &ei, &bodySectBi, bd,
							dsNew.dsBegin.dpBi ) )
		    { LDEB(1); return -1;	}

		ei->eiPageSelectedUpon= dfj.dfjExternalItemPage;
		}

	    *ds= dsNew;

	    return 0;
	    }

	if  ( docSelectionSameRoot( &(dfj.dfjStartSelection),
					    dfj.dfjCurrentPosition.dpBi ) )
	    {
	    if  ( dfj.dfjWrapCount > 0 )
		{ break;	}

	    dfj.dfjWrapCount++;
	    }

	ret= docFindPrevPrevTree( &(dfj.dfjCurrentPosition),
				&(dfj.dfjExternalItemPage), samePhase, bd );
	if  ( ret < 0 )
	    { LDEB(ret); return -1;	}
	if  ( ret > 0 )
	    { return ret;	}
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Navigation betweem external items with the PageUp/PageDown keys.	*/
/*									*/
/************************************************************************/

int docNextSimilarRoot(		DocumentPosition *	dp,
				int *			pPage,
				BufferDocument *	bd )
    {
    int		page= dp->dpBi->biTopPosition.lpPage;
    const int	samePhase= 1;

    int ret= docFindNextNextTree( dp, &page, samePhase, bd );

    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}

    *pPage= page;
    return ret;
    }

int docPrevSimilarRoot(		DocumentPosition *	dp,
				int *			pPage,
				BufferDocument *	bd )
    {
    int		page= dp->dpBi->biTopPosition.lpPage;
    const int	samePhase= 1;

    int ret= docFindPrevPrevTree( dp, &page, samePhase, bd );

    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}

    *pPage= page;
    return ret;
    }
