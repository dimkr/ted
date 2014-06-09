/************************************************************************/
/*									*/
/*  Layout of a document.						*/
/*									*/
/*  Functionality to format notes and to subtract the space occupied by	*/
/*  notes and footnotes from what is left over prom the page (or the	*/
/*  column).								*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	"docLayout.h"

#   include	<appDebugon.h>

#   define	SHOW_NOTE_LAYOUT	0

/************************************************************************/
/*									*/
/*  Commit the space allocated for footnotes and the footnote separator	*/
/*									*/
/************************************************************************/

static void docLayoutCommitNotesReservation(
				NotesReservation *		nrTotal,
				const NotesReservation *	nr )
    {
    if  ( nrTotal->nrFirstFootnote < 0		&&
	  nr->nrFirstFootnote >= 0		)
	{ nrTotal->nrFirstFootnote= nr->nrFirstFootnote;	}

    if  ( nrTotal->nrFtnsepHeight == 0		&&
	  nr->nrFtnsepHeight > 0		)
	{ nrTotal->nrFtnsepHeight= nr->nrFtnsepHeight;	}

    nrTotal->nrFootnoteCount += nr->nrFootnoteCount;
    nrTotal->nrFootnoteHeight += nr->nrFootnoteHeight;

    return;
    }

/************************************************************************/
/*									*/
/*  Collect the sum of the height of footnotes between two positions	*/
/*  in the same paragraph. This routine and the subsequent one are	*/
/*  used to subtract the space claimed for footnotes on the current	*/
/*  page at the beginning of a formatting action.			*/
/*									*/
/************************************************************************/

static int docLayoutCollectOneFootnote( NotesReservation *	nr,
					int *			pHigh,
					BufferDocument *	bd,
					const DocumentNote *	dn,
					int			noteIndex )
    {
    const ExternalItem *	ei;
    int				high;

    if  ( nr->nrFootnoteCount == 0 )
	{
	nr->nrFirstFootnote= noteIndex;

	ei= docDocumentNoteSeparator( bd, DOCinFTNSEP );
	if  ( ! ei )
	    { LXDEB(DOCinFTNSEP,ei); return -1;	}
	if  ( ! ei->eiItem )
	    { LXDEB(DOCinFTNSEP,ei->eiItem);	}

	high= ei->eiY1UsedTwips- ei->eiY0UsedTwips;
	nr->nrFtnsepHeight= high;
	}

    ei= &(dn->dnExternalItem);
    high= ei->eiY1UsedTwips- ei->eiY0UsedTwips;

    nr->nrFootnoteCount++;
    nr->nrFootnoteHeight += high;

    *pHigh= high;

    return 0;
    }

int docLayoutCollectParaFootnoteHeight(	NotesReservation *	nrTotal,
					int			referringPage,
					int			referringColumn,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			partFrom,
					int			partUpto )
    {
    int			part;
    TextParticule *	tp;
    
    NotesReservation	nrLocal;

    docInitNotesReservation( &nrLocal );

    if  ( partUpto > paraBi->biParaParticuleCount )
	{ LLDEB(partUpto,paraBi->biParaParticuleCount); return -1;	}

    tp= paraBi->biParaParticules+ partFrom;
    for ( part= partFrom; part < partUpto; tp++, part++ )
	{
	DocumentNote *	dn;
	int		noteIndex;
	int		high;

	if  ( tp->tpKind != DOCkindNOTE )
	    { continue;	}

	noteIndex= docGetNote( &dn, bd, paraBi, tp->tpStroff );
	if  ( noteIndex < 0 )
	    {
	    int	paraNr= docNumberOfParagraph( paraBi );
	    LLLDEB(paraNr,tp->tpStroff,noteIndex); return -1;
	    }

	dn->dnReferringPage= referringPage;
	dn->dnReferringColumn= referringColumn;

	if  ( dn->dnExternalItemKind != DOCinFOOTNOTE )
	    { continue;	}

	if  ( docLayoutCollectOneFootnote( &nrLocal, &high,
						    bd, dn, noteIndex ) )
	    { LDEB(noteIndex); return -1;	}

#	if SHOW_NOTE_LAYOUT
	appDebug( "PAGE %3d FOOTNOTE      --%5d note %d (Reserve)\n",
					    referringPage,
					    high, noteIndex );
#	endif
	}

    docLayoutCommitNotesReservation( nrTotal, &nrLocal );

#   if SHOW_NOTE_LAYOUT
    if  ( nrTotal->nrFootnoteCount > 0 )
	{
	appDebug( "PAGE %3d FOOTNOTES     --%5d %d notes\n",
					    referringPage,
					    nrTotal->nrFootnoteHeight,
					    nrTotal->nrFootnoteCount );
	}
#   endif

    return 0;
    }

static int docLayoutCollectFootnoteHeight(
			NotesReservation *		nr,
			int				referringPage,
			int				referringColumn,
			BufferDocument *		bd,
			int				partFrom,
			int				partUpto,
			const DocumentPosition *	dpFrom,
			const DocumentPosition *	dpUpto )
    {
    int			cmp;

    int			partHere= partFrom;
    DocumentPosition	dpHere= *dpFrom;

    cmp= docComparePositions( &dpHere, dpUpto );
    if  ( cmp > 0 )
	{ LDEB(cmp);	}
    while( cmp < 0 )
	{
	if  ( dpHere.dpBi == dpUpto->dpBi )
	    {
	    if  ( docLayoutCollectParaFootnoteHeight( nr,
				    referringPage, referringColumn,
				    bd, dpHere.dpBi, partHere, partUpto ) )
		{ LDEB(1);	}

	    break;
	    }
	else{
	    if  ( docLayoutCollectParaFootnoteHeight( nr,
				referringPage, referringColumn,
				bd, dpHere.dpBi, partHere,
				dpHere.dpBi->biParaParticuleCount ) )
		{ LDEB(1);	}

	    dpHere.dpBi= docNextParagraph( dpHere.dpBi );
	    if  ( ! dpHere.dpBi )
		{ XDEB(dpHere.dpBi); return -1;	}

	    if  ( docFirstPosition( &dpHere, dpHere.dpBi ) )
		{ LDEB(1); return -1;	}
	    partHere= 0;

	    cmp= docComparePositions( &dpHere, dpUpto );
	    if  ( cmp > 0 )
		{ LDEB(cmp);	}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Collect the footnotes in the same column above the beginning of a	*/
/*  layout action.							*/
/*									*/
/************************************************************************/

int docCollectFootnotesForColumn(
				BlockFrame *			bf,
				const DocumentPosition *	dpHere,
				int				partHere,
				const LayoutJob *		refLj )
    {
    BufferDocument *		bd= refLj->ljBd;

    DocumentPosition		dpPageTop;
    NotesReservation		nr;

    int				partPageTop;
    int				linePageTop;

    docInitNotesReservation( &nr );

    if  ( docGetTopOfColumn( &dpPageTop, &linePageTop, &partPageTop, bd,
					refLj->ljPosition.lpPage,
					refLj->ljPosition.lpColumn ) )
	{ LDEB(refLj->ljPosition.lpPage); return -1; }

    if  ( docLayoutCollectFootnoteHeight( &nr,
					    refLj->ljPosition.lpPage,
					    refLj->ljPosition.lpColumn,
					    bd, partPageTop, partHere,
					    &dpPageTop, dpHere ) )
	{ LDEB(1); return -1;	}

    bf->bfNotesReservation= nr;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Place the lines of a footnote on the page that refers to it.	*/
/*									*/
/*  2)  Get the separator between body and notes and calculate its	*/
/*	height.								*/
/*  3)  Start a new layout job directly below the section text.		*/
/*  4)  When the notes are to be placed at the bottom of the page, find	*/
/*	another start position.						*/
/*  5)  Skip the space reserved for the separator line between the	*/
/*	body text and the notes.					*/
/*  6)  Place the notes one below the other from the calculated top	*/
/*	position. As the notes are supposed to be small, their layout	*/
/*	is recalculated at little extra cost.				*/
/*  7)  Force the footnote separator to be reformatted later on.	*/
/*									*/
/************************************************************************/

int docLayoutFootnotesForColumn(	LayoutPosition *	lpBelowNotes,
					const BlockFrame *	refBf,
					int			belowText,
					const LayoutPosition *	lpBelowText,
					const LayoutJob *	refLj )
    {
    const NotesReservation *	refNr= &(refBf->bfNotesReservation);
    BufferDocument *		bd= refLj->ljBd;
    const DocumentProperties *	dp= &(bd->bdProperties);
    const NotesProperties *	npFootnotes= &(dp->dpFootnoteProperties);

    LayoutJob			notesLj;

    int				i;
    ExternalItem *		ei;
    ExternalItem *		eiNoteSep;
    int				noteIndex;
    int				notesDone;

    int				sepHigh= 0;

    if  ( refBf->bfFootnotesPlaced )
	{ LDEB(refBf->bfFootnotesPlaced); return 0;	}

    /*  2  */
    eiNoteSep= docDocumentNoteSeparator( bd, DOCinFTNSEP );
    if  ( ! eiNoteSep )
	{ LXDEB(DOCinFTNSEP,eiNoteSep); return -1;	}
    if  ( ! eiNoteSep->eiItem )
	{ LXDEB(DOCinFTNSEP,eiNoteSep->eiItem);	}

    sepHigh= eiNoteSep->eiY1UsedTwips- eiNoteSep->eiY0UsedTwips;

    /*  3  */
    notesLj= *refLj;
    notesLj.ljPosition.lpPage= lpBelowText->lpPage;
    notesLj.ljPosition.lpPageYTwips= lpBelowText->lpPageYTwips;
    notesLj.ljPosition.lpAtTopOfColumn= 1; /* not really */

    /*  4  */
    if  ( ! belowText && npFootnotes->npPosition == FTN_POS_PAGE_BOTTOM )
	{
	int		high= sepHigh;

	noteIndex= refNr->nrFirstFootnote;
	notesDone= 0;
	for ( i= noteIndex;
	      i < bd->bdNoteCount && notesDone < refNr->nrFootnoteCount;
	      i++ )
	    {
	    DocumentNote *		dn= bd->bdNotes+ i;

	    if  ( dn->dnParaNr < 0 )
		{ continue;	}
	    if  ( dn->dnExternalItemKind != DOCinFOOTNOTE )
		{ continue;	}

	    ei= &(dn->dnExternalItem);
	    if  ( ! ei->eiItem )
		{ XDEB(ei->eiItem); continue;	}

	    high += ei->eiY1UsedTwips- ei->eiY0UsedTwips;
	    notesDone++;
	    }

	notesLj.ljPosition.lpPageYTwips= refBf->bfY1Twips- high;
	}

    /*  5  */
    notesLj.ljPosition.lpPageYTwips += sepHigh;

    /*  6  */
    noteIndex= refNr->nrFirstFootnote;
    notesDone= 0;
    for ( i= noteIndex;
	  i < bd->bdNoteCount && notesDone < refNr->nrFootnoteCount;
	  i++ )
	{
	DocumentNote *		dn= bd->bdNotes+ i;
	int			y0Twips= notesLj.ljPosition.lpPageYTwips;

	if  ( dn->dnParaNr < 0 )
	    { continue;	}
	if  ( dn->dnExternalItemKind != DOCinFOOTNOTE )
	    { continue;	}


	/*  7  */
	eiNoteSep->eiPageFormattedFor= -1;

	ei= &(dn->dnExternalItem);
	if  ( ! ei->eiItem )
	    { XDEB(ei->eiItem); continue;	}

	if  ( ei->eiPageFormattedFor != lpBelowText->lpPage	||
	      ei->eiY0UsedTwips != y0Twips			)
	    {
	    if  ( docLayoutItemAndParents( ei->eiItem, &notesLj ) )
		{ LDEB(1); return -1;	}

	    ei->eiPageFormattedFor= lpBelowText->lpPage;
	    ei->eiY0UsedTwips= y0Twips;
	    ei->eiY1UsedTwips= notesLj.ljPosition.lpPageYTwips;
	    }
	else{
	    notesLj.ljPosition.lpPageYTwips= ei->eiY1UsedTwips;
	    }

	notesDone++;

#	if SHOW_NOTE_LAYOUT
	appDebug( "PAGE %3d NOTE          --%5d note %d (Layout)\n",
			    lpBelowText->lpPage,
			    ei->eiY1UsedTwips- ei->eiY0UsedTwips, noteIndex );
#       endif
	}

    *lpBelowNotes= notesLj.ljPosition;
    lpBelowNotes->lpAtTopOfColumn= lpBelowText->lpAtTopOfColumn;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Place the notes belonging to a section immediately below its text.	*/
/*									*/
/*  1)  Are there any?							*/
/*  2)  Get separator.							*/
/*  2a) If there are footnotes on the same page, the footnotes come	*/
/*	first.								*/
/*  3)  Layout of separator.						*/
/*  4)  Layout of the endnotes of this section.				*/
/*  7)  Force the footnote separator to be reformatted later on.	*/
/*									*/
/************************************************************************/

int docLayoutEndnotesForSection(	int			sect,
					BlockFrame *		bf,
					LayoutJob *		lj )
    {
    BufferDocument *		bd= lj->ljBd;

    DocumentNote *		dn;
    int				noteIndex;

    int				i;

    ExternalItem *		ei;
    ExternalItem *		eiNoteSep;

    int				attempt;

    /*  1  */
    if  ( sect >= 0 )
	{ noteIndex= docGetFirstNoteOfSection( &dn, bd, sect, DOCinENDNOTE ); }
    else{ noteIndex= docGetFirstNoteOfDocument( &dn, bd, DOCinENDNOTE ); }

    if  ( noteIndex < 0 )
	{ return 0;	}

    /*  2  */
    eiNoteSep= docDocumentNoteSeparator( bd, DOCinAFTNSEP );
    if  ( ! eiNoteSep )
	{ LXDEB(DOCinAFTNSEP,eiNoteSep); return -1;	}
    if  ( ! eiNoteSep->eiItem )
	{ LXDEB(DOCinAFTNSEP,eiNoteSep->eiItem);	}

    /*  2a  */
    if  ( BF_HAS_FOOTNOTES( bf ) )
	{
	const int	belowText= 1;

	if  ( docLayoutFootnotesForColumn( &(lj->ljPosition), bf,
					belowText, &(lj->ljPosition), lj ) )
	    { LDEB(1); return -1;	}

	bf->bfFootnotesPlaced= 1;
	}

    for ( attempt= 0; attempt < 2; attempt++ )
	{
	int		retry= 0;

	if  ( eiNoteSep->eiItem )
	    {
	    int		y0Twips= lj->ljPosition.lpPageYTwips;

	    /*  3  */
	    if  ( docLayoutItemImplementation( eiNoteSep->eiItem, bf, lj ) )
		{ LDEB(1); return -1;	}

	    eiNoteSep->eiPageFormattedFor=
				    eiNoteSep->eiItem->biTopPosition.lpPage;
	    eiNoteSep->eiY0UsedTwips= y0Twips;
	    eiNoteSep->eiY1UsedTwips= lj->ljPosition.lpPageYTwips;

	    /*  7  */
	    eiNoteSep->eiPageFormattedFor= -1;
	    }

	/*  4  */
	for ( i= noteIndex; i < bd->bdNoteCount; dn++, i++ )
	    {
	    int		y0Twips= lj->ljPosition.lpPageYTwips;

	    if  ( dn->dnParaNr < 0 )
		{ continue;	}
	    if  ( sect >= 0 && dn->dnSectNr > sect )
		{ break;	}

	    if  ( dn->dnExternalItemKind != DOCinENDNOTE )
		{ continue;	}

	    ei= &(dn->dnExternalItem);
	    if  ( ! ei->eiItem )
		{ XDEB(ei->eiItem); continue;	}

	    if  ( docLayoutItemImplementation( ei->eiItem, bf, lj ) )
		{ LDEB(1); return -1;	}

	    ei->eiPageFormattedFor= ei->eiItem->biTopPosition.lpPage;
	    ei->eiY0UsedTwips= y0Twips;
	    ei->eiY1UsedTwips= lj->ljPosition.lpPageYTwips;

	    if  ( attempt == 0						&&
		  i == noteIndex					&&
		  eiNoteSep->eiItem					&&
		  eiNoteSep->eiItem->biTopPosition.lpPage <
				    ei->eiItem->biTopPosition.lpPage	)
		{
		lj->ljPosition= ei->eiItem->biTopPosition;
		retry= 1;
		break;
		}
	    }

	if  ( ! retry )
	    { break;	}
	}

    return 0;
    }

int docLayoutEndnotesForDocument(	BlockFrame *		bf,
					LayoutJob *		lj )
    {
    return docLayoutEndnotesForSection( -1, bf, lj );
    }

/************************************************************************/
/*									*/
/*  Determine the rectangle that the note separator would occupy on a	*/
/*  certain page.							*/
/*									*/
/************************************************************************/

int docNoteSeparatorRectangle(	DocumentRectangle *	drExtern,
				ExternalItem **		pEiNoteSep,
				int *			pY0Twips,
				BufferDocument *	bd,
				const DocumentNote *	dnFirstNote,
				int			extItKind,
				const AppDrawingData *	add )
    {
    const ExternalItem *	eiFirstNote= &(dnFirstNote->dnExternalItem);
    ExternalItem *		eiNoteSep;

    int				page;
    int				pageY0;

    int				y0Twips;
    int				y1Twips;

    int				high;

    eiNoteSep= docDocumentNoteSeparator( bd, extItKind );
    if  ( ! eiNoteSep )
	{ LXDEB(extItKind,eiNoteSep); return -1;	}
    if  ( ! eiNoteSep->eiItem )
	{ LXDEB(extItKind,eiNoteSep->eiItem); return 1;	}

    page= eiFirstNote->eiItem->biTopPosition.lpPage;
    y1Twips= eiFirstNote->eiItem->biTopPosition.lpPageYTwips;
    high= eiNoteSep->eiY1UsedTwips- eiNoteSep->eiY0UsedTwips;
    y0Twips= y1Twips- high;

    drExtern->drX0= add->addBackRect.drX0;
    drExtern->drX1= add->addBackRect.drX1;

    pageY0= page* add->addPageStepPixels;
    drExtern->drY0= pageY0+ Y_PIXELS( add, y0Twips );
    drExtern->drY1= pageY0+ Y_PIXELS( add, y1Twips );

    *pEiNoteSep= eiNoteSep;
    *pY0Twips= y0Twips;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Commit the space allocated for footnotes and the footnote separator	*/
/*  in a frame.								*/
/*									*/
/************************************************************************/

void docLayoutReserveNoteHeight(	ParagraphFrame *		pf,
					BlockFrame *			bf,
					const NotesReservation *	nr )
    {
    int			footnoteHeight;
    NotesReservation *	nrBf= &(bf->bfNotesReservation);

    docLayoutCommitNotesReservation( nrBf, nr );

    footnoteHeight= nrBf->nrFtnsepHeight+ nrBf->nrFootnoteHeight;

    if  ( pf->pfPageY1Twips > bf->bfY1Twips- footnoteHeight )
	{
	pf->pfPageY1Twips=  bf->bfY1Twips- footnoteHeight;

	if  ( pf->pfFrameY1Twips > pf->pfPageY1Twips )
	    { pf->pfFrameY1Twips=  pf->pfPageY1Twips;	}
	}

#   if SHOW_NOTE_LAYOUT
    if  ( nrBf->nrFootnoteCount > 0 )
	{
	appDebug( "         NOTES         ,,%5d %d notes\n",
						    footnoteHeight,
						    nrBf->nrFootnoteCount );
	}
#   endif

    return;
    }

