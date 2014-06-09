/************************************************************************/
/*									*/
/*  Manage notes.							*/
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
/*  Manage Footnotes/Endnotes.						*/
/*									*/
/************************************************************************/

void docInitNote(		DocumentNote *		dn )
    {
    docInitExternalItem( &(dn->dnExternalItem) );

    dn->dnNoteNumber= 0;

    dn->dnReferringPage= -1;
    dn->dnReferringColumn= -1;
    dn->dnSectNr= -1;
    dn->dnParaNr= -1;
    dn->dnStroff= -1;

    dn->dnExternalItemKind= DOCinFOOTNOTE;
    dn->dnAutoNumber= 1;

    return;
    }

void docCleanNote(		BufferDocument *	bd,
				DocumentNote *		dn )
    {
    docCleanExternalItem( bd, &(dn->dnExternalItem) );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a Footnote/Endnote in a BufferItem.				*/
/*									*/
/*  1)  Look for a note or the place to insert a new one.		*/
/*	NOTE that positions with a negative paragraph number are always	*/
/*	skipped.							*/
/*	NOTE that when a position for a new note is found, its index	*/
/*	is that of its successor, that usually will be shifted away.	*/
/*  2)  When the previous position is an empty hole, use it in stead	*/
/*	of shifting the successors away.				*/
/*  3)  Make a guess about the note number. If the guess is wrong, it	*/
/*	fixed later. Fixing can involve a reformat, so making not too	*/
/*	bad a guess here does help.					*/
/*									*/
/************************************************************************/

/*  1  */
static int docGetNoteIndex(		int *			pFound,
					const BufferDocument *	bd,
					int			paraNr,
					int			stroff )
    {
    DocumentNote *	dn;
    int			note;

    dn= bd->bdNotes;
    for ( note= 0; note < bd->bdNoteCount; dn++, note++ )
	{
	if  ( dn->dnParaNr < paraNr )
	    { continue;	}
	if  ( dn->dnParaNr > paraNr )
	    { break;	}

	if  ( dn->dnStroff < stroff )
	    { continue;	}
	if  ( dn->dnStroff > stroff )
	    { break;	}

	*pFound= 1; return note;
	}

    *pFound= 0; return note;
    }

static void docSetNote(		DocumentNote *	fresh,
				int		autoNumber,
				int		sectNr,
				int		paraNr,
				int		stroff )
    {
    docInitNote( fresh );

    /*  3  */
    if  ( autoNumber )
	{ fresh->dnNoteNumber= autoNumber;	}
    else{ fresh->dnNoteNumber= 0;		}

    fresh->dnSectNr= sectNr;
    fresh->dnParaNr= paraNr;
    fresh->dnStroff= stroff;
    fresh->dnAutoNumber= autoNumber;

    return;
    }

int docInsertNote(			DocumentNote **		pDn,
					BufferDocument *	bd,
					BufferItem *		paraBi,
					int			stroff,
					int			autoNumber )
    {
    int			sectNr;
    int			paraNr;
    int			noteIndex;
    int			found;

    BufferItem *	sectBi;
    DocumentNote *	fresh;

    int			i;

    if  ( paraBi->biLevel != DOClevPARA		||
	  paraBi->biInExternalItem != DOCinBODY	)
	{
	SLDEB(docLevelStr(paraBi->biLevel),paraBi->biInExternalItem);
	return -1;
	}

    paraNr= docNumberOfParagraph( paraBi );
    if  ( paraNr < 0 )
	{ LDEB(paraNr); return -1;	}

    sectBi= paraBi;
    while( sectBi && sectBi->biLevel != DOClevSECT )
	{ sectBi= sectBi->biParent;	}
    sectNr= sectBi->biNumberInParent;

    if  ( ! sectBi )
	{ XDEB(sectBi); return -1;	}

    fresh= realloc( bd->bdNotes, ( bd->bdNoteCount+ 1 )* sizeof(DocumentNote) );
    if  ( ! fresh )
	{ LXDEB(bd->bdNoteCount,fresh); return -1;	}
    bd->bdNotes= fresh;

    noteIndex= docGetNoteIndex( &found, bd, paraNr, stroff );
    if  ( found )
	{ LDEB(found);	}

    /*  2  */
    if  ( noteIndex > 0 && fresh[noteIndex-1].dnParaNr < 0 )
	{
	while( noteIndex > 0 && fresh[noteIndex-1].dnParaNr < 0 )
	    { noteIndex--;	}

	docSetNote( fresh+ noteIndex, autoNumber, sectNr, paraNr, stroff );
	}
    else{
	for ( i= bd->bdNoteCount; i > noteIndex; i-- )
	    {
	    fresh[i]= fresh[i-1];

	    if  ( fresh[i].dnExternalItem.eiItem )
		{
		fresh[i].dnExternalItem.eiItem->
				    biSectSelectionScope.ssNoteArrayIndex= i;
		}
	    }

	docSetNote( fresh+ noteIndex, autoNumber, sectNr, paraNr, stroff );

	bd->bdNoteCount++;
	}

    *pDn= fresh+ noteIndex; return noteIndex;
    }

/************************************************************************/
/*									*/
/*  Retrieve the note corresponding to the current position.		*/
/*									*/
/*  1)  It is either the note the current posion refers to,		*/
/*  2)  Or the note that contains the current position.			*/
/*									*/
/************************************************************************/

int docGetNote(		DocumentNote **		pDn,
			const BufferDocument *	bd,
			const BufferItem *	paraBi,
			int			stroff )
    {
    int			paraNr;
    int			noteIndex;
    int			found;

    if  ( paraBi->biLevel != DOClevPARA )
	{
	SLDEB(docLevelStr(paraBi->biLevel),paraBi->biInExternalItem);
	return -1;
	}

    /*  1  */
    if  ( paraBi->biInExternalItem == DOCinBODY	)
	{
	paraNr= docNumberOfParagraph( paraBi );
	if  ( paraNr < 0 )
	    { LDEB(paraNr); return -1;	}

	noteIndex= docGetNoteIndex( &found, bd, paraNr, stroff );
	if  ( ! found )
	    {
	    /* LLLDEB(paraNr,stroff,found); docListNotes( bd ); */
	    return -1;
	    }

	*pDn= bd->bdNotes+ noteIndex;
	return noteIndex;
	}

    /*  2  */
    if  ( paraBi->biInExternalItem == DOCinFOOTNOTE	||
	  paraBi->biInExternalItem == DOCinENDNOTE	)
	{
	const BufferItem *	noteSectBi;

	noteSectBi= paraBi;
	while( noteSectBi && noteSectBi->biLevel != DOClevSECT )
	    { noteSectBi= noteSectBi->biParent;	}

	if  ( ! noteSectBi )
	    { XDEB(noteSectBi); return -1;	}

	noteIndex= noteSectBi->biSectSelectionScope.ssNoteArrayIndex;

	*pDn= bd->bdNotes+ noteIndex;
	return noteIndex;
	}

    /*
    SLDEB(docLevelStr(paraBi->biLevel),paraBi->biInExternalItem);
    */
    return -1;
    }

/************************************************************************/
/*									*/
/*  Count the number f notes in the document.				*/
/*									*/
/************************************************************************/

int docCountNotes(		const BufferDocument *	bd )
    {
    int			count= 0;

    int			i;
    DocumentNote *	dn;

    dn= bd->bdNotes;
    for ( i= 0; i < bd->bdNoteCount; dn++, i++ )
	{
	if  ( dn->dnParaNr < 0 )
	    { continue;	}

	count++;
	}

    return count;
    }

/************************************************************************/
/*									*/
/*  Return the first note on a page.					*/
/*									*/
/************************************************************************/

int docGetFirstNoteFromPage(	DocumentNote **		pDn,
				const BufferDocument *	bd,
				int			page,
				int			extItKind )
    {
    int			i;
    DocumentNote *	dn;

    dn= bd->bdNotes;
    for ( i= 0; i < bd->bdNoteCount; dn++, i++ )
	{
	if  ( dn->dnParaNr < 0 )
	    { continue;	}
	if  ( dn->dnExternalItemKind != extItKind )
	    { continue;	}

	if  ( dn->dnReferringPage == page )
	    { *pDn= dn; return i;	}
	}

    return -1;
    }

int docGetFirstNoteOnPage(	DocumentNote **		pDn,
				const BufferDocument *	bd,
				int			page,
				int			extItKind )
    {
    int			i;
    DocumentNote *	dn;

    dn= bd->bdNotes;
    for ( i= 0; i < bd->bdNoteCount; dn++, i++ )
	{
	ExternalItem *	ei= &(dn->dnExternalItem);

	if  ( dn->dnParaNr < 0 )
	    { continue;	}
	if  ( dn->dnExternalItemKind != extItKind )
	    { continue;	}

	if  ( ei->eiPageFormattedFor == page )
	    { *pDn= dn; return i;	}
	}

    return -1;
    }

int docGetFirstNoteOfSection(	DocumentNote **		pDn,
				const BufferDocument *	bd,
				int			sect,
				int			extItKind )
    {
    int			i;
    DocumentNote *	dn;

    dn= bd->bdNotes;
    for ( i= 0; i < bd->bdNoteCount; dn++, i++ )
	{
	if  ( dn->dnParaNr < 0 )
	    { continue;	}
	if  ( dn->dnExternalItemKind != extItKind )
	    { continue;	}

	if  ( dn->dnSectNr == sect )
	    { *pDn= dn; return i;	}
	}

    return -1;
    }

int docGetFirstNoteOfDocument(	DocumentNote **		pDn,
				const BufferDocument *	bd,
				int			extItKind )
    {
    int			i;
    DocumentNote *	dn;

    dn= bd->bdNotes;
    for ( i= 0; i < bd->bdNoteCount; dn++, i++ )
	{
	if  ( dn->dnParaNr < 0 )
	    { continue;	}
	if  ( dn->dnExternalItemKind != extItKind )
	    { continue;	}

	*pDn= dn; return i;
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Renumber the notes of a document.					*/
/*									*/
/************************************************************************/

void docRenumberNotes(		int *			pChanged,
				BufferDocument *	bd )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);
    const NotesProperties *	np;

    int				footN= 0;
    int				endN= 0;

    int				page= -1;
    int				sect= -1;

    int				i;
    DocumentNote *		dn;

    dn= bd->bdNotes;
    for ( i= 0; i < bd->bdNoteCount; dn++, i++ )
	{
	if  ( dn->dnParaNr < 0 )
	    { continue;	}

	if  ( ! dn->dnAutoNumber )
	    {
	    if  ( dn->dnNoteNumber != 0 )
		{ *pChanged= 1;	}

	    dn->dnNoteNumber= 0;
	    continue;
	    }

	/****/
	np= &(dp->dpFootnoteProperties);

	if  ( np->npRestart == FTN_RST_PER_SECTION	&&
	      dn->dnSectNr != sect			)
	    { footN= 0;	}

	if  ( np->npRestart == FTN_RST_PER_PAGE	&&
	      dn->dnReferringPage != page		)
	    { footN= 0;	}

	/****/
	np= &(dp->dpEndnoteProperties);

	if  ( np->npRestart == FTN_RST_PER_SECTION	&&
	      dn->dnSectNr != sect			)
	    { endN= 0;	}

	if  ( np->npRestart == FTN_RST_PER_PAGE	&&
	      dn->dnReferringPage != page		)
	    { endN= 0;	}

	/****/

	switch( dn->dnExternalItemKind )
	    {
	    case DOCinFOOTNOTE:
		if  ( dn->dnNoteNumber != footN )
		    { dn->dnNoteNumber=   footN; *pChanged= 1;	}
		footN++;
		break;

	    case DOCinENDNOTE:
		if  ( dn->dnNoteNumber != endN )
		    { dn->dnNoteNumber=   endN; *pChanged= 1;	}
		endN++;
		break;

	    default:
		LLDEB(i,dn->dnExternalItemKind);
		break;
	    }

	sect= dn->dnSectNr;
	page= dn->dnReferringPage;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt references from notes to the text because the number of	*/
/*  paragraphs changed in the document.					*/
/*									*/
/*  1)  Paranoia check. Skipping the paragraphs before the forst one	*/
/*	also skips the deleted paragraphs with dn->dnParaNr == -1.	*/
/*									*/
/************************************************************************/

void docShiftNoteReferences(		BufferDocument *	bd,
					int			paraFrom,
					int			stroffFrom,
					int			sectShift,
					int			paraShift,
					int			stroffShift )
    {
    DocumentNote *	dn;
    int			i;

    /*
    appDebug( "docShiftNoteReferences( ..,"
		" para:%d+%d, stroff:%d+%d )\n",
	    paraFrom, paraShift, stroffFrom, stroffShift );
    */

    /*  1  */
    if  ( paraFrom < 1 )
	{ LDEB(paraFrom); return;	}

    dn= bd->bdNotes;
    for ( i= 0; i < bd->bdNoteCount; dn++, i++ )
	{
	/*  1  */
	if  ( dn->dnParaNr < paraFrom )
	    { continue;	}

	if  ( dn->dnParaNr > paraFrom )
	    {
	    if  ( sectShift == 0 && paraShift == 0 )
		{ break;	}

	    dn->dnSectNr += sectShift;
	    dn->dnParaNr += paraShift;

	    if  ( dn->dnExternalItem.eiItem )
		{
		dn->dnExternalItem.eiItem->
			biSectSelectionScope.ssSectNrExternalTo= dn->dnSectNr;
		}

	    continue;
	    }

	if  ( dn->dnStroff < stroffFrom )
	    { continue;	}
	if  ( dn->dnStroff > stroffFrom )
	    {
	    dn->dnSectNr += sectShift;
	    dn->dnParaNr += paraShift;
	    dn->dnStroff += stroffShift;

	    if  ( dn->dnExternalItem.eiItem )
		{
		dn->dnExternalItem.eiItem->
			biSectSelectionScope.ssSectNrExternalTo= dn->dnSectNr;
		}

	    continue;
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make a note at the given position.					*/
/*									*/
/*  1)  Claim memory for a note.					*/
/*  2)  Insert a particule that refers to the note.			*/
/*  4)  Insert an empty paragraph in the note to begin with.		*/
/*  5)  Make a chftn field to represent the note in the external	*/
/*	paragraph.							*/
/*  6)  Insert a space after the note number field. This also is to	*/
/*	work around a bug in the editing around fields.			*/
/*									*/
/*  !)  The chftn field that precedes the note was inserted by the	*/
/*	caller.								*/
/*									*/
/************************************************************************/

int docMakeNote(	DocumentNote **			pDn,
			BufferDocument *		bd,
			BufferItem *			paraBi,
			int				part,
			int				stroff,
			int				extItKind )
    {
    TextParticule *	tp;
    TextAttribute	ta;
    int			textAttributeNumberPlain;
    int			textAttributeNumberSuper;

    DocumentNote *	dn;

    int			noteIndex;

    int			stroffShift= 0;
    const int		autoNumber= 1;

    BufferItem *	noteParaBi;

    int			fieldNr= -1;
    int			partBegin= -1;
    int			partEnd= -1;
    int			stroffBegin= -1;
    int			stroffEnd= -1;

    if  ( paraBi->biInExternalItem != DOCinBODY )
	{ LDEB(paraBi->biInExternalItem); return -1;	}

    /*  1  */
    noteIndex= docInsertNote( &dn, bd, paraBi, stroff, autoNumber );
    if  ( noteIndex < 0 )
	{ LDEB(noteIndex); return -1;	}

    dn->dnExternalItemKind= extItKind;

    if  ( part > 0 )
	{ tp= paraBi->biParaParticules+ part- 1;	}
    else{ tp= paraBi->biParaParticules+ part;		}

    utilGetTextAttributeByNumber( &ta,
		&(bd->bdTextAttributeList), tp->tpTextAttributeNumber );
    ta.taSuperSub= DOCfontREGULAR;
    textAttributeNumberPlain=
		utilTextAttributeNumber( &(bd->bdTextAttributeList), &ta );

    /*  2  */
    tp= docInsertTextParticule( paraBi, part+ 1,
			stroff, 0, DOCkindNOTE, textAttributeNumberPlain );
    if  ( ! tp )
	{ LDEB(paraBi->biParaParticuleCount); return -1; }

    /*  4  */
    noteParaBi= docMakeExternalparagraph( bd, &(dn->dnExternalItem),
					paraBi, textAttributeNumberPlain,
					noteIndex, extItKind );
    if  ( ! noteParaBi )
	{ XDEB(noteParaBi); return -1;	}

    {
    TextAttribute	taSet;

    taSet= ta;

    taSet.taSuperSub= DOCfontSUPERSCRIPT;

    textAttributeNumberSuper=
		utilTextAttributeNumber( &(bd->bdTextAttributeList), &taSet );
    }

    /*  5  */
    if  ( docInsertParaHeadField( &fieldNr, &partBegin, &partEnd,
				    &stroffBegin, &stroffEnd,
				    noteParaBi, bd,
				    DOCfkCHFTN, textAttributeNumberSuper ) )
	{ LDEB(1); return -1;	}

    /*  6  */
    if  ( docParaStringReplace( &stroffShift, noteParaBi,
					    stroffEnd, stroffEnd,
					    (const unsigned char *)" ", 1 ) )
	{ LDEB(noteParaBi->biParaStrlen); return -1; }

    tp= docInsertTextParticule( noteParaBi, noteParaBi->biParaParticuleCount,
			    noteParaBi->biParaStrlen- 1, 1,
			    DOCkindTEXT, textAttributeNumberPlain );
    if  ( ! tp )
	{ LDEB(noteParaBi->biParaParticuleCount); return -1; }

    *pDn= dn; return 0;
    }

/************************************************************************/
/*									*/
/*  Delete the note that corresponds to a particule.			*/
/*									*/
/************************************************************************/

int docDeleteNoteOfParticule(		BufferDocument *	bd,
					const BufferItem *	paraBi,
					const TextParticule *	tp )
    {
    DocumentNote *	dn;

    if  ( docGetNote( &dn, bd, paraBi, tp->tpStroff ) < 0 )
	{ LDEB(tp->tpStroff); return -1;	}

    docCleanNote( bd, dn );
    docInitNote( dn );

    while( bd->bdNoteCount > 0					&&
	   bd->bdNotes[ bd->bdNoteCount- 1].dnParaNr < 0	)
	{ bd->bdNoteCount--;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Get the note corresponding to a selection.				*/
/*									*/
/*  1)  Get the note. Outside the note itself, only notes that are	*/
/*	exactly the end position of the selection are found. In the	*/
/*	note itself, the note is returned.				*/
/*  2)  In body?							*/
/*  3)  If the selection spans more than one paragraph, it does not	*/
/*	qualify as the selection of exactly the bote refrence.		*/
/*  4)  For a range selection, see whether the selection exactly covers	*/
/*	the note number.						*/
/*  5)  Look for the beginning of a note field at the beginning of the	*/
/*	selection.							*/
/*  6)  Does the same note field end at the end of the selection?	*/
/*  A)  Look for a note number field that ends at the IBar.		*/
/*  B)  Look for the beginning of the field.				*/
/*  a)  Look for a note number field at the beginning of the note item.	*/
/*  b)  Look for the end of the field.					*/
/*									*/
/************************************************************************/

void docNoteGetTextBefore(	unsigned char *		fixedText,
				int *			pFixedLen,
				int *			pTextAttrNr,
				int			fixedTextSize,
				const BufferItem *	paraBi,
				int			stroff )
    {
    int				part;
    const TextParticule *	tp;
    const int			lastOne= 1;

    if  ( docFindParticule( &part, paraBi, stroff, lastOne ) )
	{ LDEB(stroff); fixedText[0]= '\0'; return;	}

    if  ( part >= paraBi->biParaParticuleCount )
	{
	LLDEB(part,paraBi->biParaParticuleCount);
	part= paraBi->biParaParticuleCount- 1;
	}

    tp= paraBi->biParaParticules+ part;
    while( part >= 0 )
	{
	if  ( tp->tpStroff+ tp->tpStrlen < stroff )
	    { break;	}

	if  ( tp->tpKind == DOCkindTEXT		&&
	      tp->tpStroff < stroff		)
	    {
	    int		from= tp->tpStroff;
	    int		len= tp->tpStrlen;

	    if  ( len > fixedTextSize )
		{ len= fixedTextSize; from += len- fixedTextSize; }

	    *pFixedLen= len;
	    *pTextAttrNr= tp->tpTextAttributeNumber;

	    strncpy( (char *)fixedText,
			(char *)paraBi->biParaString+ from, len )[len]= '\0';
	    return;
	    }

	part--; tp--;
	}

    fixedText[0]= '\0';
    return;
    }

void docNoteGetTextAtHead(	unsigned char *		fixedText,
				int *			pFixedLen,
				int			fixedTextSize,
				const BufferItem *	paraBi )
    {
    const int			partB= 0;
    const int			stroff= 0;
    int				part= partB;
    const TextParticule *	tp= paraBi->biParaParticules+ part;

    while( part >= 0 )
	{
	if  ( tp->tpStroff > stroff )
	    { break;	}

	if  ( tp->tpKind == DOCkindTEXT			&&
	      tp->tpStroff+ tp->tpStrlen > stroff	)
	    {
	    int		len= tp->tpStrlen;

	    if  ( len > fixedTextSize )
		{ len= fixedTextSize;	}

	    *pFixedLen= len;
	    strncpy( (char *)fixedText,
		(char *)paraBi->biParaString+ tp->tpStroff, len )[len]= '\0';
	    return;
	    }

	part++; tp++;
	}

    *pFixedLen= 0;
    fixedText[0]= '\0';
    return;
    }

int docDelimitNoteReference(	int *			pFieldNr,
				int *			pPartBegin,
				int *			pPartEnd,
				int *			pStroffBegin,
				int *			pStroffEnd,
				const BufferItem *	paraBi,
				int			stroff,
				const BufferDocument *	bd )
    {
    const TextParticule *	tp;

    int				fieldNr= -1;
    int				partBegin= -1;
    int				partEnd= -1;
    int				stroffBegin= -1;
    int				stroffEnd= -1;

    if  ( docFindParticule( &partEnd, paraBi, stroff, 1 ) )
	{ LDEB(stroff); return -1;	}

    /*  A  */
    tp= paraBi->biParaParticules+ partEnd;
    while( partEnd >= 0			&&
	   tp->tpStroff == stroff	)
	{
	if  ( tp->tpKind == DOCkindFIELDEND )
	    {
	    const DocumentField *	df;

	    df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;
	    if  ( df->dfKind == DOCfkCHFTN )
		{
		fieldNr= tp->tpObjectNumber;
		stroffEnd= tp->tpStroff;
		break;
		}
	    }

	partEnd--; tp--;
	}

    /*  B  */
    if  ( fieldNr >= 0 )
	{
	partBegin= partEnd;
	partBegin--; tp--;

	while( partBegin >= 0 )
	    {
	    if  ( tp->tpKind == DOCkindFIELDSTART	&&
		  tp->tpObjectNumber == fieldNr		)
		{
		stroffBegin= tp->tpStroff;
		break;
		}

	    partBegin--; tp--;
	    }

	if  ( partBegin < 0 )
	    { fieldNr= -1;	}
	}

    if  ( fieldNr < 0 )
	{ return 1;	}

    *pFieldNr= fieldNr;
    *pPartBegin= partBegin;
    *pPartEnd= partEnd;
    *pStroffBegin= stroffBegin;
    *pStroffEnd= stroffEnd;

    return 0;
    }

int docGetSelectedNote(		DocumentNote **			pDn,
				int *				pFieldNr,
				unsigned char *			fixedText,
				int				fixedTextSize,
				BufferDocument *		bd,
				const DocumentSelection *	ds )
    {
    DocumentNote *		dn;
    int				noteIndex;

    const BufferItem *		paraBi;

    int				fieldNr= -1;
    int				partBegin= -1;
    int				partEnd= -1;
    int				stroffBegin= -1;
    int				stroffEnd= -1;

    /*  1  */
    noteIndex= docGetNote( &dn, bd, ds->dsEnd.dpBi, ds->dsEnd.dpStroff );
    if  ( noteIndex < 0 )
	{ return noteIndex;	}

    /*  2  */
    if  ( ds->dsSelectionScope.ssInExternalItem == DOCinFOOTNOTE	||
	  ds->dsSelectionScope.ssInExternalItem == DOCinENDNOTE		)
	{
	DocumentPosition	dpBegin;

	if  ( docFirstPosition( &dpBegin, dn->dnExternalItem.eiItem ) )
	    { LDEB(1); return -1;	}

	paraBi= dpBegin.dpBi;

	if  ( docDelimitParaHeadField( &fieldNr, &partBegin, &partEnd,
				    &stroffBegin, &stroffEnd, paraBi, bd ) )
	    {
	    if  ( fixedTextSize > 0 )
		{
		int	fixedLen= 0;

		docNoteGetTextAtHead( fixedText, &fixedLen, fixedTextSize,
								    paraBi );
		}
	    }
	else{
	    if  ( fixedTextSize > 0 )
		{ fixedText[0]= '\0'; }
	    }
	}
    else{
	paraBi= ds->dsBegin.dpBi;

	/*  3  */
	if  ( ! docIsParaSelection( ds ) )
	    { return -1;	}

	if  ( ! docDelimitNoteReference( &fieldNr, &partBegin, &partEnd,
		&stroffBegin, &stroffEnd, paraBi, ds->dsEnd.dpStroff, bd ) )
	    {
	    if  ( ! docIsIBarSelection( ds )		&&
		  ds->dsBegin.dpStroff != stroffBegin	)
		{ return -1;	}
	    }

	if  ( fixedTextSize > 0 )
	    {
	    if  ( fieldNr >= 0 )
		{ fixedText[0]= '\0'; }
	    else{
		int	fixedLen= 0;
		int	textAttrNr;

		docNoteGetTextBefore( fixedText, &fixedLen, &textAttrNr,
				fixedTextSize, paraBi, ds->dsEnd.dpStroff );
		}
	    }
	}

    *pDn= dn;
    *pFieldNr= fieldNr;
    return noteIndex;
    }

/************************************************************************/
/*									*/
/*  Verify that a certain kind of notes separator exists. Make it if	*/
/*  not.								*/
/*									*/
/*  1)  Correctly handeled by docTextAttributeNumber().			*/
/*									*/
/************************************************************************/

int docCheckNoteSeparatorItem(		BufferDocument *	bd,
					int			extItKind )
    {
    ExternalItem *	ei;
    int			particuleKind;

    switch( extItKind )
	{
	case DOCinFTNSEP:
	case DOCinAFTNSEP:
	    particuleKind= DOCkindCHFTNSEP;
	    break;

	case DOCinFTNSEPC:
	case DOCinAFTNSEPC:
	    particuleKind= DOCkindCHFTNSEPC;
	    break;

	default:
	    LDEB(extItKind);
	    particuleKind= DOCkindCHFTNSEP;
	    break;
	}

    ei= docDocumentNoteSeparator( bd, extItKind );
    if  ( ! ei )
	{ LXDEB(extItKind,ei); return -1;	}

    if  ( ! ei->eiItem )
	{
	const int		noteIndex= -1;
	int			attributeNumber;

	BufferItem *	bi;
	TextAttribute	ta;

	utilInitTextAttribute( &ta );
	ta.taFontNumber= -1; /*  1  */
	ta.taFontSizeHalfPoints= 24;

	attributeNumber= docTextAttributeNumber( bd, &ta );
	if  ( attributeNumber < 0 )
	    { LDEB(attributeNumber); return -1;	}

	bi= docMakeExternalparagraph( bd, ei, &(bd->bdItem), attributeNumber,
						    noteIndex, extItKind );
	if  ( ! bi )
	    { XDEB(bi);	}
	else{
	    if  ( docInflateTextString( bi, 1 ) )
		{ LDEB(bi->biParaStrlen);	}
	    else{
		TextParticule *	tp= bi->biParaParticules;

		tp->tpKind= particuleKind;
		tp->tpStrlen= 1;
		bi->biParaStrlen++;
		}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Return the position in the document body where a note is		*/
/*  referenced.								*/
/*									*/
/************************************************************************/

int docGetNotePosition(		DocumentPosition *		dp,
				BufferDocument *		bd,
				const DocumentNote *		dn )
    {
    BufferItem *	paraBi;

    paraBi= docGetParagraphByNumber( &(bd->bdItem), dn->dnParaNr );
    if  ( ! paraBi )
	{ LXDEB(dn->dnParaNr,paraBi); return -1;	}

    if  ( docSetDocumentPosition( dp, paraBi, dn->dnStroff ) )
	{ LLDEB(paraBi->biParaStrlen,dn->dnStroff); return -1;	}

    return 0;
    }

