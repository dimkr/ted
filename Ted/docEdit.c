/************************************************************************/
/*									*/
/*  Buffer manipulation routines.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>

#   include	"docBuf.h"
#   include	"docEdit.h"

#   define	DEB_PARTICULES		0
#   define	SHOW_SELECTION_RANGE	0

/************************************************************************/
/*									*/
/*  Administration of fields that are deleted in the editing process.	*/
/*									*/
/************************************************************************/

static int docEditClaimFieldParticules(	TextParticule **	pTp,
					int			count )
    {
    TextParticule *	fresh;

    fresh= (TextParticule *)realloc( *pTp, count* sizeof( TextParticule ) );
    if  ( ! fresh )
	{ LXDEB(count,fresh); return -1;	}

    *pTp= fresh; return 0;
    }

/************************************************************************/
/*									*/
/*  Cleanup the set of fields that have been set apart during a		*/
/*  text substitution in a paragrapph.					*/
/*									*/
/*  1)  Fields that begin and end in the text that is replaced are	*/
/*	dropped.							*/
/*  3)  Skip particules that have been deleted.				*/
/*  4)  Ends of fields are considered later on.				*/
/*  5)  Index entries and table of contents entries from the old text	*/
/*	are dropped.							*/
/*									*/
/*  a)  Superfluous as the array is reorganised.			*/
/*									*/
/************************************************************************/

static int docCleanupFields(	int *			pBulletDeleted,
				BufferDocument *	bd,
				BufferItem *		paraBi,
				TextParticule *		fieldParticules,
				int			fieldCount )
    {
    int		from;
    int		to;
    int		bulletDeleted= 0;

    to= 0;
    for ( from= 0; from < fieldCount; from++ )
	{
	DocumentField *		dfFrom;
	int			i;

	switch( fieldParticules[from].tpKind )
	    {
	    /*  1  */
	    case DOCkindFIELDSTART:
		dfFrom= bd->bdFieldList.dflFields+
					fieldParticules[from].tpObjectNumber;
		for ( i= from+ 1; i < fieldCount; i++ )
		    {
		    if  ( fieldParticules[i].tpKind == DOCkindFIELDEND )
			{ break;	}
		    }

		if  ( i < fieldCount )
		    {
		    if  ( dfFrom->dfKind == DOCfkLISTTEXT )
			{ bulletDeleted= 1;	}

		    docCleanField( dfFrom );
		    docInitField( dfFrom );
		    fieldParticules[from].tpKind= DOCkindUNKNOWN; /* a */
		    fieldParticules[i].tpKind= DOCkindUNKNOWN;
		    }
		else{ fieldParticules[to++]= fieldParticules[from]; }
		break;

	    /*  3  */
	    case DOCkindUNKNOWN:
		continue;

	    /*  4  */
	    case DOCkindFIELDEND:
		fieldParticules[to++]= fieldParticules[from];
		break;

	    /*  5  */
	    case DOCkindXE:
	    case DOCkindTC:
		dfFrom= bd->bdFieldList.dflFields+
					fieldParticules[from].tpObjectNumber;
		docCleanField( dfFrom );
		docInitField( dfFrom );
		fieldParticules[from].tpKind= DOCkindUNKNOWN; /* a */
		continue;

	    case DOCkindLINEBREAK:
	    case DOCkindPAGEBREAK:
	    case DOCkindCOLUMNBREAK:
	    default:
		LDEB(fieldParticules[from].tpKind);
		return -1;
	    }
	}

    if  ( bulletDeleted && paraBi->biParaListOverride > 0 )
	{
	if  ( docRemoveParagraphFromList( paraBi, bd ) )
	    { LDEB(bulletDeleted);	}
	}

    *pBulletDeleted= bulletDeleted;
    return to;
    }

/************************************************************************/
/*									*/
/*  Reinsert a field particule that was contained in text that was	*/
/*  removed from the paragraph. Ends are only reinserted when the	*/
/*  corresponding beginning is also there.				*/
/*									*/
/************************************************************************/

static int docReinsertFieldParticule(	BufferDocument *	bd,
					BufferItem *		oldBi,
					int			stroff,
					int			part,
					TextParticule *		tpField,
					TextParticule *		tpTo )
    {
    TextParticule *	tpFrom;
    int			i;

    int			endObjectNumber= -1;

    switch( tpField->tpKind )
	{
	case DOCkindFIELDSTART:
	case DOCkindXE:
	case DOCkindTC:
	    break;

	case DOCkindFIELDEND:
	    tpFrom= oldBi->biParaParticules+ part;
	    endObjectNumber= tpField->tpObjectNumber;
	    tpFrom--;
	    for ( i= part- 1; i >= 0; tpFrom--, i-- )
		{
		if  ( tpFrom->tpKind == DOCkindFIELDSTART	&&
		      tpFrom->tpObjectNumber == endObjectNumber	)
		    { break;	}
		}

	    if  ( i < 0 )
		{ return 0;	}

	    break;

	case DOCkindUNKNOWN:
	case DOCkindLINEBREAK:
	case DOCkindPAGEBREAK:
	case DOCkindCOLUMNBREAK:
	default:
	    LDEB(tpField->tpKind);
	    return -1;
	}

    if  ( tpTo )
	{
	*tpTo= *tpField;
	tpTo->tpStroff= stroff;
	}
    else{
	tpTo= docInsertTextParticule( oldBi, part, stroff,
					tpField->tpStrlen, tpField->tpKind,
					tpField->tpTextAttributeNumber );

	tpTo->tpObjectNumber= tpField->tpObjectNumber;
	}

#   if DEB_PARTICULES
    {
    const int	indent= 0;
    docListParticule( indent, "NFL", part, oldBi, tpTo );
    }
#   endif

    return 1;
    }

/************************************************************************/
/*									*/
/*  Replace text in a paragraph.					*/
/*									*/
/*  1)  Claim space to set apart special particules for field,		*/
/*	bookmarks, index entries &c.					*/
/*  2)  Make sure that enough space is allocated for the paragraph	*/
/*	contents.							*/
/*  3)  If the substitution actually is a deletion, compare attributes	*/
/*	to the current position.					*/
/*									*/
/*  4)  Find the last particule that does not end before the beginning	*/
/*	of the substitution.						*/
/*	NOTE that this automatically includes all particules that start	*/
/*	or end bookmarks.						*/
/*  5)  If the substitution starts at the beginning of a particule, and	*/
/*	the previous one has the same attributes as the inserted text,	*/
/*	start the redivision of the text in particules one particule	*/
/*	back.								*/
/*									*/
/*  6)  Find the first particule that does not begin before the		*/
/*	beginning of the substitution.					*/
/*	NOTE that this automatically includes all particules that start	*/
/*	or end bookmarks.						*/
/*  7)  If the substitution ends at the end of a particule, and	the	*/
/*	next one has the same attributes as the inserted text, include	*/
/*	the next one in the redivision of the text in particules.	*/
/*									*/
/*  8)  Decide whether the start particule needs to be split.		*/
/*  9)  Decide whether the end particule needs to be split.		*/
/*  9b) If the substitution is in the middle of a particule, it is not	*/
/*	possible to just adjust the begin of the particule, as it is	*/
/*	needed to do so for the first half. So make a new one.		*/
/*  10) Adapt the range of particules that are to be replaced to the	*/
/*	outcome of the decisions.					*/
/*									*/
/*  11) Delete particules corresponding to what is deleted, and to what	*/
/*	is to be redivided in particules.				*/
/*									*/
/*  12) Change the string contents of the paragraph.			*/
/*  13) Get rid of fields and bookmarks that start and end in de	*/
/*	deleted range.							*/
/*  14) Does the replacemeny start in a field?				*/
/*  15) Reinsert field and bookmark starts defore the new contents.	*/
/*									*/
/*  16) Sanity check.							*/
/*  17) Divide the replacement and its surroundings in particules.	*/
/*									*/
/*  18) Insert the remaining field, bookmark and index entry related	*/
/*	particules after the replacement.				*/
/*  19) Delete the particumes that are left over.			*/
/*  20) If the final result is an empty paragraph, insert a dummy	*/
/*	particule.							*/
/*									*/
/*  21) Adjust the offsets for the particules in the rest of the	*/
/*	paragraph.							*/
/*									*/
/*  22) Realise that the Text Line administration is left in an		*/
/*	inconsistent state.						*/
/*  23) Remember the current particule number.				*/
/*									*/
/************************************************************************/

static void docFindSubstitutionBegin(	int *			pPartBegin,
					TextParticule **	pTpBegin,
					const BufferItem *	bi,
					int			addedAttrNr,
					int			stroffBegin,
					int			partBegin )
    {
    TextParticule *	tpBegin= bi->biParaParticules+ partBegin;

    if  ( partBegin > 0						&&
	   tpBegin[-1].tpKind == DOCkindFIELDSTART			&&
	   tpBegin->tpStroff == stroffBegin				)
	{
	while( partBegin > 0						&&
	       tpBegin[-1].tpKind == DOCkindFIELDSTART			&&
	       tpBegin->tpStroff == stroffBegin				)
	    { tpBegin--; partBegin--; }

	*pPartBegin= partBegin; *pTpBegin= tpBegin; return;
	}

    /*  4  */
    while( partBegin > 0						&&
	   tpBegin[-1].tpStroff+ tpBegin[-1].tpStrlen > stroffBegin	)
	{ tpBegin--; partBegin--; }

    while( partBegin > 0						&&
	   tpBegin[-1].tpKind == DOCkindTEXT				&&
	   tpBegin[-1].tpStroff+ tpBegin[-1].tpStrlen >= stroffBegin	)
	{ tpBegin--; partBegin--; }

    /*  5  */
    if  ( partBegin > 0							&&
	  tpBegin[-1].tpKind == DOCkindTEXT				&&
	  tpBegin->tpStroff == stroffBegin				&&
	  addedAttrNr == tpBegin[-1].tpTextAttributeNumber		)
	{ tpBegin--; partBegin--; }

    *pPartBegin= partBegin; *pTpBegin= tpBegin; return;
    }

static void docFindSubstitutionEnd(	int *			pPartEnd,
					TextParticule **	pTpEnd,
					const BufferItem *	bi,
					int			addedAttrNr,
					int			stroffEnd,
					int			partBegin )
    {
    int				partEnd= partBegin;
    TextParticule *		tpEnd= bi->biParaParticules+ partEnd;

    /*  6  */
    while( partEnd < bi->biParaParticuleCount- 1			&&
	   tpEnd[1].tpStroff < stroffEnd				)
	{ tpEnd++; partEnd++;	}

    if  ( partEnd < bi->biParaParticuleCount- 1				&&
	   tpEnd[1].tpKind == DOCkindFIELDEND				&&
	   tpEnd->tpStroff+ tpEnd->tpStrlen == stroffEnd		)
	{
	while( partEnd < bi->biParaParticuleCount- 1			&&
	       tpEnd[1].tpKind == DOCkindFIELDEND			&&
	       tpEnd->tpStroff+ tpEnd->tpStrlen == stroffEnd		)
	    { tpEnd++; partEnd++;	}

	while( partEnd < bi->biParaParticuleCount- 1			&&
	       tpEnd[1].tpKind == DOCkindNOTE				&&
	       tpEnd->tpStroff+ tpEnd->tpStrlen == stroffEnd		)
	    { tpEnd++; partEnd++;	}

	*pPartEnd= partEnd; *pTpEnd= tpEnd; return;
	}

    while( partEnd < bi->biParaParticuleCount- 1			&&
	   tpEnd[1].tpKind == DOCkindTEXT				&&
	   tpEnd[1].tpStroff <= stroffEnd				)
	{ tpEnd++; partEnd++;	}

    /*  7  */
    if  ( partEnd < bi->biParaParticuleCount- 1				&&
	  tpEnd[1].tpKind == DOCkindTEXT				&&
	  tpEnd->tpStroff+ tpEnd->tpStrlen == stroffEnd			&&
	  addedAttrNr == tpEnd[1].tpTextAttributeNumber			)
	{ tpEnd++; partEnd++;	}

    *pPartEnd= partEnd; *pTpEnd= tpEnd; return;
    }

int docParaReplaceText(		EditOperation *		eo,
				BufferItem *		paraBi,
				int			paraNr,
				unsigned int		stroffBegin,
				int *			pPartShift,
				int *			pStroffShift,
				unsigned int		stroffEnd,
				const unsigned char *	addedString,
				unsigned int		addedStrlen,
				int			addedAttributeNumber )
    {
    int				stroffShift;
    int				partShift;
    int				oldCount;
    int				newCount;

    int				part;
    TextParticule *		tp;

    int				partBegin;
    int				partEnd;
    TextParticule *		tpBegin;
    TextParticule *		tpEnd;
    int				excludeBegin= 0;
    int				excludeEnd= 0;
    int				newBegin;
    int				newEnd;
    int				stroffEndEnd;

    int				i;
    TextParticule *		tpField;
    int				res;
    int				partsDone;

    static TextParticule *	fieldParticules;
    int				fieldCount= 0;

    int				savedAttributeNumber;
    const int			lastOne= 1;

    /*  1  */
    if  ( docEditClaimFieldParticules( &fieldParticules,
						paraBi->biParaParticuleCount ) )
	{ LDEB(paraBi->biParaParticuleCount); return -1;	}

    /*  2  */
    stroffShift= 0;
    partShift= 0;

    if  ( docFindParticule( &partBegin, paraBi, stroffBegin, lastOne ) )
	{ LDEB(stroffBegin); return -1;	}

    /*  3  */
    if  ( addedStrlen == 0 )
	{
	tpBegin= paraBi->biParaParticules+ partBegin;
	addedAttributeNumber= tpBegin->tpTextAttributeNumber;
	}

    /*  4,5  */
    docFindSubstitutionBegin( &partBegin, &tpBegin, paraBi,
				addedAttributeNumber, stroffBegin, partBegin );

    /*  6,7  */
    docFindSubstitutionEnd( &partEnd, &tpEnd, paraBi,
				addedAttributeNumber, stroffEnd, partBegin );
    stroffEndEnd= tpEnd->tpStroff+ tpEnd->tpStrlen;

    /*  8  */
    if  ( tpBegin->tpKind == DOCkindTEXT				&&
	  addedAttributeNumber == tpBegin->tpTextAttributeNumber	)
	{ newBegin= tpBegin->tpStroff;			}
    else{
	newBegin= stroffBegin;

	if  ( newBegin > tpBegin->tpStroff )
	    { excludeBegin= 1; }

	if  ( newBegin == paraBi->biParaStrlen )
	    {
	    if  ( tpBegin->tpKind == DOCkindNOTE	||
		  tpBegin->tpKind == DOCkindFIELDEND	)
		{ excludeBegin= 1;	}
	    }
	}

    /*  9  */
    if  ( tpEnd->tpKind == DOCkindTEXT					&&
	  addedAttributeNumber == tpEnd->tpTextAttributeNumber		)
	{ newEnd= tpEnd->tpStroff+ tpEnd->tpStrlen;	}
    else{
	newEnd= stroffEnd;

	if  ( newEnd < tpEnd->tpStroff+ tpEnd->tpStrlen )
	    { excludeEnd= 1; }
	}

    /*  8  */
    if  ( excludeBegin )
	{
	tpBegin->tpStrlen= newBegin- tpBegin->tpStroff;

#	if DEB_PARTICULES
	appDebug( "<<= %3d: [%4d..%4d] %s \"%.*s\"\n", partBegin,
				tpBegin->tpStroff,
				tpBegin->tpStroff+ tpBegin->tpStrlen,
				docKindStr( tpBegin->tpKind ),
				(int)tpBegin->tpStrlen,
				paraBi->biParaString+ tpBegin->tpStroff );
#	endif
	}

    /*  9  */
    if  ( excludeEnd )
	{
	/*  9b  */
	if  ( partEnd == partBegin	&&
	      excludeBegin		)
	    {
	    tp= docInsertTextParticule( paraBi, partEnd+ 1,
				newEnd, stroffEndEnd- newEnd,
				DOCkindTEXT, tpEnd->tpTextAttributeNumber );
	    if  ( ! tp )
		{ XDEB(tp); return -1;	}

	    partShift++;
	    tpEnd= tp;
	    }
	else{
	    tpEnd->tpStroff= newEnd;
	    tpEnd->tpStrlen= stroffEndEnd- newEnd;
	    }

#	if DEB_PARTICULES
	appDebug( "=>> %3d: [%4d..%4d] %s \"%.*s\"\n", partEnd,
				tpEnd->tpStroff,
				tpEnd->tpStroff+ tpEnd->tpStrlen,
				docKindStr( tpEnd->tpKind ),
				(int)tpEnd->tpStrlen,
				paraBi->biParaString+ tpEnd->tpStroff );
#	endif
	}

    /*  10  */
    oldCount= 0;
    if  ( excludeBegin )
	{ partBegin++;	}
    if  ( excludeEnd )
	{ partEnd--;	}

    tpBegin= paraBi->biParaParticules+ partBegin;
    tpEnd= paraBi->biParaParticules+ partEnd;

    savedAttributeNumber= tpBegin->tpTextAttributeNumber;

    if  ( paraBi->biParaParticuleCount > partEnd+ 1			&&
	  paraBi->biParaParticules[partEnd+ 0].tpKind == DOCkindTEXT	&&
	  paraBi->biParaParticules[partEnd+ 0].tpStroff >= stroffBegin	&&
	  paraBi->biParaParticules[partEnd+ 1].tpKind == DOCkindNOTE	&&
	  ( paraBi->biParaParticules[partEnd+ 0].tpStroff >
						    stroffBegin	||
	    addedStrlen == 0					)	)
	{ partEnd++;	}

    /*  11  */
    tp= tpBegin;
    for ( part= partBegin; part <= partEnd; tp++, part++ )
	{
#	if DEB_PARTICULES
	const int	indent= 0;
	docListParticule( indent, "OLD", part, paraBi, tp );
#	endif

	if  ( eo->eoCloseObject )
	    { (*eo->eoCloseObject)( eo->eoBd, paraBi, tp, eo->eoVoidadd ); }

	docCleanParticuleObject( eo->eoBd, paraBi, tp );

	if  ( tp->tpKind == DOCkindFIELDSTART	||
	      tp->tpKind == DOCkindFIELDEND	||
	      tp->tpKind == DOCkindXE		||
	      tp->tpKind == DOCkindTC		)
	    { fieldParticules[fieldCount++]= *tp;	}

	if  ( tp->tpKind == DOCkindNOTE )
	    {
	    if  ( docDeleteNoteOfParticule( eo->eoBd, paraBi, tp ) )
		{ LDEB(tp->tpStroff);	}

	    eo->eoNotesDeleted++;
	    }

	tp->tpObjectNumber= -1;
	partShift--; oldCount++;
	}

    /*  12  */
    if  ( docParaStringReplace( &stroffShift, paraBi, stroffBegin, stroffEnd,
						addedString, addedStrlen ) )
	{ LDEB(addedStrlen); return -1;	}

    /*  13  */
    if  ( fieldCount > 0 )
	{
	int	bulletDeleted= 0;

	fieldCount= docCleanupFields( &bulletDeleted, eo->eoBd,
					paraBi, fieldParticules, fieldCount );

	if  ( bulletDeleted )
	    { eo->eoFieldUpdate |= FIELDdoLISTTEXT;	}
	}
    if  ( fieldCount < 0 )
	{ LDEB(fieldCount); return -1;	}

    /*  14  */
    newEnd += stroffShift;
    newCount= 0;

    /*  15  */
    tp= tpBegin;
    part= partBegin;
    tpField= fieldParticules;
    for ( i= 0; i < fieldCount; tpField++, i++ )
	{
	if  ( tpField->tpKind != DOCkindFIELDSTART )
	    { continue;	}

	if  ( newCount < oldCount )
	    {
	    res= docReinsertFieldParticule( eo->eoBd, paraBi, newBegin, part,
							tpField, tp );
	    }
	else{
	    res= docReinsertFieldParticule( eo->eoBd, paraBi, newBegin, part,
					    tpField, (TextParticule *)0 );
	    }

	if  ( res < 0 )
	    { LDEB(res); return -1;	}

	if  ( res > 0 )
	    { partShift++; newCount++; part++, tp++;	}

	tpField->tpKind= DOCkindUNKNOWN;
	}

    /*  16  */
    if  ( newBegin > newEnd )
	{ LLDEB(newBegin,newEnd); }

    /*  17  */
    partsDone= docRedivideStringInParticules( paraBi,
			    newBegin, newEnd- newBegin,
			    part, oldCount- newCount, addedAttributeNumber );
    if  ( partsDone < 0 )
	{ LDEB(partsDone); return -1;	}
    newCount += partsDone;
    partShift += partsDone;
    part += partsDone;

    /*  18  */
    tpField= fieldParticules;
    for ( i= 0; i < fieldCount; tpField++, i++ )
	{
	if  ( tpField->tpKind == DOCkindUNKNOWN	)
	    { continue;	}

	if  ( newCount < oldCount )
	    {
	    res= docReinsertFieldParticule( eo->eoBd, paraBi, newEnd, part,
				    tpField, paraBi->biParaParticules+ part );
	    }
	else{
	    res= docReinsertFieldParticule( eo->eoBd, paraBi, newEnd, part,
					tpField, (TextParticule *)0 );
	    }

	if  ( res < 0 )
	    { LDEB(res); return -1;	}

	if  ( res > 0 )
	    { partShift++; newCount++; part++;	}
	}

    /*  19  */
    if  ( newCount < oldCount )
	{
	docDeleteParticules( paraBi, part, oldCount- newCount );

	if  ( paraBi->biParaParticuleCount == 0 )
	    {
	    if  ( part != 0 || paraBi->biParaStrlen != 0 )
		{ LLDEB(part,paraBi->biParaStrlen);	}

	    tp= docInsertTextParticule( paraBi, part, paraBi->biParaStrlen,
					0, DOCkindTEXT, savedAttributeNumber );
	    if  ( ! tp )
		{ XDEB(tp); return -1;	}

	    /* No.. Gives problems in the incremental formatter!
	    docInvalidateParagraphLayout( paraBi );
	    */
	    part= 1; partShift++;
	    }
	}

    /*  21  */
    if  ( docEditShiftParticuleOffsets( eo, paraBi, paraNr,
					part, paraBi->biParaParticuleCount,
					stroffBegin, stroffShift ) )
	{ LDEB(stroffShift);	}

#   if DEB_PARTICULES
    {
    int		p;

    appDebug( "\n" );

    p= 0; tp= paraBi->biParaParticules;
    while( p < paraBi->biParaParticuleCount )
	{
	const int	indent= 0;
	docListParticule( indent, "===", p, paraBi, tp );
	p++; tp++;
	}
    }
#   endif

    /*  22  */
    *pPartShift += partShift;
    *pStroffShift += stroffShift;

    eo->eoInsParaBi= paraBi;
    eo->eoInsStroff= stroffBegin+ addedStrlen;

    /* 23 */
    if  ( docFindParticule( &(eo->eoInsParticule), eo->eoInsParaBi,
						eo->eoInsStroff, lastOne ) )
	{ LDEB(eo->eoInsStroff); return -1;	}

    if  ( eo->eoInsStroff ==
		eo->eoInsParaBi->biParaParticules[eo->eoInsParticule].tpStroff )
	{ eo->eoInsAtPartHead= 1;	}
    else{ eo->eoInsAtPartHead= 0;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Append the tail of a paragraph to another paragraph in the same	*/
/*  document.								*/
/*									*/
/*  1)  Claim memory for the appended contents.				*/
/*  2)  Claim memory for the appended particules.			*/
/*  3a) Do away with the empty particule that might have been inserted	*/
/*	after an explicit line/page/column break.			*/
/*  3)  Append the contents of the first particule as text. This will	*/
/*	merge it with the last one of the target if necessary.		*/
/*  4)  Copy contents.							*/
/*  5)  Copy particules.						*/
/*									*/
/************************************************************************/

static int docParaAppend(	EditOperation *		eo,
				DocumentCopyJob *	dcj,
				unsigned int *		pFieldUpd,
				BufferItem *		biTo,
				BufferItem *		biFrom,
				int			paraNrTo,
				unsigned int		stroffFrom )
    {
    int			rval= 0;

    TextParticule *	tpFrom;
    TextParticule *	newTp;

    int			stroffShift= 0;
    int			partShift= 0;
    int			particuleCount;

    int			stroffTo;

    const int		lastOne= 0;
    int			part;

    if  ( docFindParticule( &part, biFrom, stroffFrom, lastOne ) )
	{ LLDEB(stroffFrom,biFrom->biParaStrlen); return -1;	}

    tpFrom= biFrom->biParaParticules+ part;

    while( part < biFrom->biParaParticuleCount- 1		&&
	  tpFrom->tpKind != DOCkindFIELDSTART			&&
	  stroffFrom == tpFrom->tpStroff+ tpFrom->tpStrlen	)
	{ tpFrom++; part++;	}

    /*  2  */
    particuleCount= biTo->biParaParticuleCount+
				    biFrom->biParaParticuleCount- part+ 1;
    newTp= (TextParticule *)realloc( biTo->biParaParticules,
				    particuleCount* sizeof(TextParticule) );
    if  ( ! newTp )
	{ LXDEB(particuleCount,newTp); rval= -1; goto ready;	}
    biTo->biParaParticules= newTp;

    if  ( biTo->biParaParticuleCount > 0 )
	{
	newTp=	biTo->biParaParticules+ biTo->biParaParticuleCount- 1;

	/*  3a  */
	if  ( biTo->biParaParticuleCount > 1	&&
	      newTp->tpKind == DOCkindTEXT	&&
	      newTp->tpStrlen == 0		)
	    { biTo->biParaParticuleCount--; newTp--;	}

	/*  3  */
	if  ( tpFrom->tpKind == DOCkindTEXT				&&
	      tpFrom->tpTextAttributeNumber ==
					  newTp->tpTextAttributeNumber	)
	    {
	    int		xPartShift= 0;

	    if  ( docParaReplaceText( eo, biTo, paraNrTo, biTo->biParaStrlen,
				&partShift, &xPartShift,
				biTo->biParaStrlen,
				biFrom->biParaString+ stroffFrom,
				tpFrom->tpStroff+ tpFrom->tpStrlen- stroffFrom,
				tpFrom->tpTextAttributeNumber ) )
		{ LDEB(tpFrom->tpStrlen); rval= -1; goto ready;	}

	    stroffFrom= tpFrom->tpStroff+ tpFrom->tpStrlen;
	    part++; tpFrom++;
	    }
	}

    stroffTo= biTo->biParaStrlen;

    /*  1,4,5  */
    if  ( docParaStringReplace( &stroffShift, biTo, stroffTo, stroffTo,
					biFrom->biParaString+ stroffFrom,
					biFrom->biParaStrlen- stroffFrom ) )
	{ LDEB(biFrom->biParaStrlen); rval= -1; goto ready;	}

    while( part < biFrom->biParaParticuleCount )
	{
	int		strlenTo;
	const int	partTo= -1;

	strlenTo= tpFrom->tpStroff+ tpFrom->tpStrlen- stroffFrom;

	if  ( docCopyParticuleAndData( &newTp, dcj, eo, biTo, partTo,
				    stroffTo, strlenTo, biFrom, tpFrom ) )
	    { LDEB(1); rval= -1; goto ready;	}

	/*
	appDebug( "NEW %3d: [%4d..%4d] %s \"%.*s\"\n",
					    biTo->biParaParticuleCount- 1,
		    newTp->tpStroff,
		    newTp->tpStroff+ newTp->tpStrlen,
		    docKindStr( tpFrom->tpKind ),
		    (int)newTp->tpStrlen,
		    biTo->biParaString+ newTp->tpStroff );
	*/

	stroffTo += strlenTo; stroffFrom += strlenTo;
	tpFrom++; part++; partShift++;
	}

    eo->eoInsParaBi= biTo;
    eo->eoInsStroff= biTo->biParaStrlen;
    eo->eoInsParticule= biTo->biParaParticuleCount- 1;
    eo->eoInsAtPartHead= 0;

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Replace a selection with a piece of text.				*/
/*									*/
/*  1)  If the selection spans more than one paragraph...		*/
/*  2)  Replace the tail of the first paragraph with the new text.	*/
/*  2b) Make sure thet the bullet of the last paragraph is not included	*/
/*	in the pail that is to be appended to the first paragraph.	*/
/*  3)  Merge the two paragraphs. I.E: append what remains of the last	*/
/*	paragraph to the first paragraph.				*/
/*  4)  Erase all paragraphs after the beginning of the selection	*/
/*	upto and including the end of the selection.			*/
/*  5)  Otherwise just replace the text inside the selected paragraph.	*/
/*									*/
/************************************************************************/

int docReplaceSelection(EditOperation *			eo,
			const DocumentSelection *	ds,
			int *				pPartShift,
			int *				pStroffShift,
			const unsigned char *		addedString,
			int				addedStrlen,
			int				addedAttributeNumber )
    {
    int			rval= 0;
    DocumentCopyJob	dcj;

    int			paraNrBegin= docNumberOfParagraph( ds->dsBegin.dpBi );

    docInitDocumentCopyJob( &dcj );

    /*  1  */
    if  ( ds->dsBegin.dpBi != ds->dsEnd.dpBi )
	{
	int		replaceEnd= ds->dsEnd.dpStroff;
	int		untable= 0;
	EditPosition	epSave;

	if  ( ds->dsBegin.dpBi->biParaInTable	&&
	      ! ds->dsEnd.dpBi->biParaInTable	)
	    { untable= 1;	}

	/*  2  */
	if  ( docParaReplaceText( eo, ds->dsBegin.dpBi, paraNrBegin,
			ds->dsBegin.dpStroff,
			pPartShift, pStroffShift,
			ds->dsBegin.dpBi->biParaStrlen,
			addedString, addedStrlen, addedAttributeNumber ) )
	    { LDEB(addedStrlen); rval= -1; goto ready;	}

	epSave= eo->eoSelectedRange.erEnd;
	epSave.epParaNr= paraNrBegin;
	epSave.epStroff= ds->dsBegin.dpBi->biParaStrlen;

	/*  2b  */
	if  ( ds->dsEnd.dpBi->biParaListOverride > 0 )
	    {
	    int		fieldNr= -1;
	    int		partBegin= -1;
	    int		partEnd= -1;
	    int		stroffBegin= -1;
	    int		stroffEnd= -1;

	    if  ( docDelimitParaHeadField( &fieldNr, &partBegin, &partEnd,
						&stroffBegin, &stroffEnd,
						ds->dsEnd.dpBi, eo->eoBd ) )
		{ LDEB(1);	}
	    else{
		if  ( replaceEnd < stroffEnd )
		    { replaceEnd=  stroffEnd;	}
		}
	    }

	/*  3  */
	if  ( replaceEnd < ds->dsEnd.dpBi->biParaStrlen )
	    {
	    unsigned int	fieldUpd= FIELDdoNOTHING;

	    if  ( docSet1DocumentCopyJob( &dcj, eo->eoBd ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    if  ( docParaAppend( eo, &dcj, &fieldUpd,
			    ds->dsBegin.dpBi, ds->dsEnd.dpBi, paraNrBegin,
			    replaceEnd ) )
		{ LDEB(addedStrlen); rval= -1; goto ready;	}
	    }

	eo->eoSelectedRange.erEnd= epSave;

	/*  4  */
	if  ( docRemoveSelectionTail( eo, ds ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( untable )
	    {
	    BufferItem *	rowBi= ds->dsBegin.dpBi;

	    while( rowBi && rowBi->biLevel != DOClevROW )
		{ rowBi= rowBi->biParent;	}

	    if  ( ! rowBi )
		{ XDEB(rowBi);	}
	    else{
		if  ( rowBi->biRowHasTableParagraphs	&&
		      rowBi->biChildCount == 1		)
		    {
		    docCleanRowProperties( &(rowBi->biRowProperties) );
		    docInitRowProperties( &(rowBi->biRowProperties) );

		    rowBi->biRowHasTableParagraphs= 0;
		    ds->dsBegin.dpBi->biParaInTable= 0;
		    }
		}
	    }
	}
    else{
	/*  5  */
	if  ( docParaReplaceText( eo, ds->dsBegin.dpBi, paraNrBegin,
			    ds->dsBegin.dpStroff,
			    pPartShift, pStroffShift,
			    ds->dsEnd.dpStroff,
			    addedString, addedStrlen, addedAttributeNumber ) )
	    { LDEB(addedStrlen); rval= -1; goto ready;	}
	}

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Split a paragraph.							*/
/*									*/
/*  a)  Find the first particule that ends on or after the given	*/
/*	position. If several particules apply, choose the one with the	*/
/*	lowest level of field nesting.					*/
/*  1)  Insert a paragraph AFTER the current one.			*/
/*  1a) Copy indents and tab stops.					*/
/*  2)  Allocate space for the string of the successor.			*/
/*  3)  If the position is in the middle of a particule, split it.	*/
/*  4)  Move the rest of the particules to the new paragraph.		*/
/*  5)  Keep field ends at the split position in the old paragraph.	*/
/*	So do not insert field ends in the new paragraph. They are	*/
/*	reinserted at the end of the old paragraph.			*/
/*									*/
/************************************************************************/

int docSplitParaItemAtStroff(	EditOperation *			eo,
				BufferItem **			pNewBi,
				const DocumentPosition *	dp,
				int				splitLevel )
    {
    int				part= 0;
    TextParticule *		tp;
    TextParticule *		newTp;
    int				inField;

    /*  a,b  */
    inField= docPositionInField( &part, dp, eo->eoBd );

    tp= dp->dpBi->biParaParticules+ part;

    if  ( part < dp->dpBi->biParaParticuleCount-1	&&
	  dp->dpStroff == tp->tpStroff+ tp->tpStrlen	&&
	  tp[1].tpKind == DOCkindFIELDEND		)
	{ tp++; part++;	}

    while( part < dp->dpBi->biParaParticuleCount	&&
	   tp->tpStrlen == 0				&&
	   ( tp->tpKind == DOCkindFIELDEND	||
	     tp->tpKind == DOCkindNOTE		)	)
	{ tp++; part++;	}

    /*  3  */
    if  ( part < dp->dpBi->biParaParticuleCount	&&
	  dp->dpStroff > tp->tpStroff		)
	{
	if  ( dp->dpStroff == tp->tpStroff+ tp->tpStrlen )
	    { part++; tp++;	}
	else{
	    if  ( docSplitTextParticule( &tp, &newTp,
					    dp->dpBi, part, dp->dpStroff ) )
		{ LDEB(part); return -1;	}

	    part++; tp= newTp;
	    }
	}

    return docSplitParaItemAtPart( eo, pNewBi, dp->dpBi, part, splitLevel );
    }

static void docEditShiftSplitReferences(EditOperation *		eo,
					const BufferItem *	oldBi,
					int			splitLevel )
    {
    const int	paraNr= docNumberOfParagraph( oldBi );
    const int	stroffFrom= oldBi->biParaStrlen;
    const int	paraShift= 1;
    const int	stroffShift= -oldBi->biParaStrlen;
    const int	sectShift= splitLevel <= DOClevSECT;

    docEditShiftReferences( eo, &(eo->eoSelectionScope),
				    paraNr, stroffFrom,
				    sectShift, paraShift, stroffShift );

    return;
    }

static int docMoveTailToNew(	EditOperation *		eo,
				DocumentCopyJob *	dcj,
				BufferItem *		newBi,
				BufferItem *		oldBi,
				TextParticule *		fieldParticules,
				int			part,
				int			stroff )
    {
    BufferDocument *	bd= eo->eoBd;
    TextParticule *	newTp;

    int			stroffShift= 0;
    int			fieldLevel= 0;
    int			newStrlen= 0;
    int			partShift= 0;
    TextParticule *	tp;

    unsigned int	fieldUpd= FIELDdoNOTHING;
    int			truncatedParticuleCount= part;

    int			fieldCount= 0;

    /*  2  */
    if  ( docParaStringReplace( &stroffShift, newBi,
					newBi->biParaStrlen,
					newBi->biParaStrlen,
					oldBi->biParaString+ stroff,
					oldBi->biParaStrlen- stroff ) )
	{
	LLDEB(oldBi->biParaStrlen,stroff);
	return -1;
	}

    fieldLevel= 0;
    {
    int				p;
    const TextParticule *	t;

    t= oldBi->biParaParticules;
    for ( p= 0; p < part; t++, p++ )
	{
	if  ( t->tpKind == DOCkindFIELDSTART )
	    { fieldLevel++;	}
	if  ( t->tpKind == DOCkindFIELDEND )
	    { fieldLevel--;	}
	}

    if  ( fieldLevel < 0 )
	{ LDEB(fieldLevel); fieldLevel= 0;	}
    }

    /*  4  */
    newStrlen= 0;
    tp= oldBi->biParaParticules+ part;
    while( part < oldBi->biParaParticuleCount )
	{
	switch( tp->tpKind )
	    {
	    default:
		LDEB(tp->tpKind);
		newStrlen += tp->tpStrlen; part++; tp++;
		continue;

	    case DOCkindFIELDEND:
		/*  5  */
		fieldParticules[fieldCount++]= *tp;
		if  ( fieldLevel > 0 )
		    {
		    fieldLevel--;
		    newStrlen += tp->tpStrlen; part++; tp++;
		    continue;
		    }
		break;

	    case DOCkindFIELDSTART:
	    case DOCkindXE:
	    case DOCkindTC:
		break;

	    case DOCkindOBJECT:
	    case DOCkindTEXT:
	    case DOCkindTAB:
	    case DOCkindLINEBREAK:
	    case DOCkindPAGEBREAK:
	    case DOCkindCOLUMNBREAK:
		break;

	    case DOCkindNOTE:
		eo->eoNotesAdded++;
		break;
	    }

	if  ( docCopyParticuleAndData( &newTp, dcj, eo,
						newBi, partShift++,
						newStrlen, tp->tpStrlen,
						oldBi, tp ) )
	    { LDEB(part); return -1;	}

	newStrlen += tp->tpStrlen; part++; tp++;
	}

    if  ( fieldCount > 0 )
	{
	int		i;
	TextParticule *	tpField;
	int		bulletDeleted= 0;

	fieldCount= docCleanupFields( &bulletDeleted,
				    bd, oldBi, fieldParticules, fieldCount );

	if  ( bulletDeleted )
	    { fieldUpd |= FIELDdoLISTTEXT;	}

	if  ( fieldCount < 0 )
	    { LDEB(fieldCount); return -1;	}

	tpField= fieldParticules;
	tp= oldBi->biParaParticules+ truncatedParticuleCount;
	for ( i= 0; i < fieldCount; tpField++, i++ )
	    {
	    int		res;

	    /*  5  */
	    res= docReinsertFieldParticule( bd, oldBi, stroff,
				    truncatedParticuleCount, tpField, tp );

	    if  ( res < 0 )
		{ LDEB(res); return -1;	}

	    if  ( res > 0 )
		{ truncatedParticuleCount++; tp++;	}
	    }
	}

    tp= oldBi->biParaParticules+ truncatedParticuleCount;
    for ( part= truncatedParticuleCount;
	  part < oldBi->biParaParticuleCount;
	  tp++, part++ )
	{
	if  ( tp->tpKind == DOCkindNOTE )
	    {
	    eo->eoNotesDeleted++;
	    if  ( docDeleteNoteOfParticule( bd, oldBi, tp ) )
		{ LLDEB(docNumberOfParagraph(oldBi),tp->tpStroff);	}
	    }
	}

    oldBi->biParaParticuleCount= truncatedParticuleCount;
    oldBi->biParaStrlen= stroff;

    if  ( newBi->biParaParticuleCount == 0 )
	{
	tp= oldBi->biParaParticules+ oldBi->biParaParticuleCount- 1;

	newTp= docInsertTextParticule( newBi, -1, 0, 0,
				    DOCkindTEXT, tp->tpTextAttributeNumber );
	if  ( ! newTp )
	    { LDEB(newBi->biParaParticuleCount); return -1; }
	}

    if  ( oldBi->biParaParticuleCount == 0 )
	{
	tp= oldBi->biParaParticules;

	newTp= docInsertTextParticule( oldBi, -1, 0, 0,
				    DOCkindTEXT, tp->tpTextAttributeNumber );
	if  ( ! newTp )
	    { LDEB(oldBi->biParaParticuleCount); return -1; }
	}
    else{ docCheckNoBreakAsLast( eo, oldBi );	}


    return 0;
    }

int docSplitParaItemAtPart(	EditOperation *		eo,
				BufferItem **		pNewBi,
				BufferItem *		oldBi,
				int			part,
				int			splitLevel )
    {
    BufferDocument *		bd= eo->eoBd;
    int				rval= 0;

    BufferItem *		newBi;
    int				stroff;

    static TextParticule *	fieldParticules;

    PropertyMask		ppChgMask;
    PropertyMask		ppUpdMask;

    DocumentCopyJob		dcj;

    docInitDocumentCopyJob( &dcj );

    if  ( docSet1DocumentCopyJob( &dcj, bd ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docEditClaimFieldParticules( &fieldParticules,
						oldBi->biParaParticuleCount ) )
	{ LDEB(oldBi->biParaParticuleCount); rval= -1; goto ready; }

    if  ( part >= oldBi->biParaParticuleCount )
	{ stroff= oldBi->biParaStrlen;				}
    else{ stroff= oldBi->biParaParticules[part].tpStroff;	}

#   if SHOW_SELECTION_RANGE
    appDebug( "SPLIT     %3d(%3d)\n",
			    docNumberOfParagraph( oldBi ), stroff );
#   endif

    /*  1  */
    newBi= docInsertItem( bd, oldBi->biParent,
				oldBi->biNumberInParent+ 1, oldBi->biLevel );
    if  ( ! newBi )
	{ XDEB(newBi); rval= -1; goto ready;	}

    if  ( splitLevel < DOClevPARA )
	{
	BufferItem *	insBi;
	BufferItem *	aftBi;

	if  ( docSplitGroupItem( bd, &insBi, &aftBi, oldBi->biParent,
				 oldBi->biNumberInParent+ 1, splitLevel ) )
	    { LDEB(1); return -1;	}

	docEditShiftSplitReferences( eo, oldBi, splitLevel );

	if  ( aftBi && aftBi->biParent )
	    { docEditIncludeItemInReformatRange( eo, aftBi->biParent );	}
	else{ XDEB(aftBi);	}
	}
    else{
	docEditShiftSplitReferences( eo, oldBi, splitLevel );

	/*  2  */
	docEditIncludeItemInReformatRange( eo, oldBi );

	/*  4  */
	docEditIncludeItemInReformatRange( eo, newBi );
	}

    /*  2  */
    if  ( docMoveTailToNew( eo, &dcj, newBi, oldBi,
					fieldParticules, part, stroff ) )
	{
	LLDEB(oldBi->biParaStrlen,stroff);
	docDeleteItems( bd, oldBi->biParent, oldBi->biNumberInParent+ 1, 1 );
	rval= -1; goto ready;
	}

    PROPmaskCLEAR( &ppChgMask );
    PROPmaskCLEAR( &ppUpdMask );
    utilPropMaskFill( &ppUpdMask, PPprop_COUNT );

    /*  1a  */
    if  ( docEditUpdParaProperties( eo, &ppChgMask, newBi,
				&ppUpdMask, &(oldBi->biParaProperties),
				dcj.dcjColorMap, dcj.dcjListStyleMap ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( oldBi->biParaListOverride > 0 )
	{ dcj.dcjBulletsCopied++;	}
    if  ( newBi->biParaListOverride > 0 )
	{
	eo->eoFieldUpdate |= FIELDdoLISTTEXT;
	eo->eoBulletsChanged++;
	}

    docInvalidateParagraphLayout( newBi );
    docInvalidateParagraphLayout( oldBi );

    *pNewBi= newBi;

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Insert a tab or another kind of special particule for something	*/
/*  that takes up some space in the document.				*/
/*									*/
/*  1)  Insert a space in the text string.				*/
/*  2)  Replace the empty particule in an empty paragraph.		*/
/*  3)  Can be inserted before the current particule.			*/
/*  4)  Can be inserted after the current particule.			*/
/*  5)  First split the particule,					*/
/*  6)  Then insert between the two halves.				*/
/*  7)  Shift particile offsets in the rest of the paragraph.		*/
/*									*/
/************************************************************************/

TextParticule * docParaSpecialParticule(EditOperation *		eo,
					BufferItem *		paraBi,
					int			kind,
					int			part,
					int			stroff,
					int *			pPartShift,
					int *			pStroffShift )
    {
    TextParticule *	tp;
    TextParticule *	tpRet;
    int			stroffShift= 0;

    int			paraNr= docNumberOfParagraph( paraBi );

    /*  1  */
    if  ( docParaStringReplace( &stroffShift, paraBi, stroff, stroff,
						(unsigned char *)" ", 1 ) )
	{ LDEB(stroff); return (TextParticule *)0;	}

    if  ( part == paraBi->biParaParticuleCount		&&
	  stroff == paraBi->biParaStrlen- 1			)
	{
	tp= paraBi->biParaParticules+ part- 1;

	tpRet= docInsertTextParticule( paraBi, part, stroff, 1,
					    kind, tp->tpTextAttributeNumber );
	if  ( ! tpRet )
	    { LXDEB(part,tpRet); return (TextParticule *)0;	}

	tpRet->tpObjectNumber= -1; /* set by caller if needed */

	*pPartShift= 1; *pStroffShift= stroffShift;
	return tpRet;
	}

    if  ( part < 0 || part >= paraBi->biParaParticuleCount )
	{ LLDEB(part,paraBi->biParaParticuleCount); return (TextParticule *)0; }

    tp= paraBi->biParaParticules+ part;

    /*  2  */
    if  ( paraBi->biParaParticuleCount == 1			&&
	  paraBi->biParaStrlen == 1				&&
	  paraBi->biParaParticules->tpKind == DOCkindTEXT	)
	{
	paraBi->biParaParticules->tpKind= kind;
	paraBi->biParaParticules->tpStrlen= 1;

	*pPartShift= 0; *pStroffShift= stroffShift;
	return paraBi->biParaParticules;
	}

    /*  3  */
    if  ( stroff == tp->tpStroff )
	{
	tpRet= docInsertTextParticule( paraBi, part, stroff, 1,
					    kind, tp->tpTextAttributeNumber );
	if  ( ! tpRet )
	    { LXDEB(part,tpRet); return (TextParticule *)0;	}

	tpRet->tpObjectNumber= -1; /* set by caller if needed */

	*pPartShift= 1;
	part++;
	}
    else{
	/*  4  */
	if  ( stroff == tp->tpStroff+ tp->tpStrlen )
	    {
	    tpRet= docInsertTextParticule( paraBi, part+ 1, stroff, 1,
					    kind, tp->tpTextAttributeNumber );
	    if  ( ! tpRet )
		{ LXDEB(part,tpRet); return (TextParticule *)0;	}

	    tpRet->tpObjectNumber= -1; /* set by caller if needed */

	    *pPartShift= 2;
	    part += 2;
	    }
	else{
	    TextParticule *	tpNew;

	    /*  5  */
	    if  ( docSplitTextParticule( &tp, &tpNew, paraBi, part, stroff ) )
		{ LLDEB(part,stroff); return (TextParticule *)0;	}

	    /*  6  */
	    tpRet= docInsertTextParticule( paraBi, part+ 1, stroff, 1,
					    kind, tp->tpTextAttributeNumber );
	    if  ( ! tpRet )
		{ LXDEB(part+1,tpRet); return (TextParticule *)0;	}

	    tpRet->tpObjectNumber= -1; /* set by caller if needed */

	    *pPartShift= 2;
	    part += 2;
	    }
	}

    /*  7  */
    if  ( docEditShiftParticuleOffsets( eo, paraBi, paraNr,
			    part, paraBi->biParaParticuleCount, stroff, 1 ) )
	{ LDEB(1);	}

    *pStroffShift= stroffShift;

    return tpRet;
    }

/************************************************************************/
/*									*/
/*  Make a paragraph empty by deleting its contents.			*/
/*									*/
/************************************************************************/

int docEditMakeParagraphEmpty(		BufferItem *		paraBi,
					EditOperation *		eo )
    {
    TextParticule *	tp= paraBi->biParaParticules;
    int			textAttributeNumber= tp->tpTextAttributeNumber;

    int			notesDeleted= 0;
    int			bulletsDeleted= 0;
    int			paragraphCount= 0;

    docCleanItemObjects( &notesDeleted, &bulletsDeleted, &paragraphCount,
			eo->eoBd, paraBi, eo->eoVoidadd, eo->eoCloseObject );

    eo->eoNotesDeleted += notesDeleted;
    eo->eoBulletsChanged += bulletsDeleted;

    paraBi->biParaStrlen= 0;
    docDeleteParticules( paraBi, 0, paraBi->biParaParticuleCount );
    docDeleteLines( paraBi, 0, paraBi->biParaLineCount );

    if  ( ! docInsertTextParticule( paraBi, 0, 0, 0, DOCkindTEXT,
							textAttributeNumber ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Shift references in various administrations.			*/
/*									*/
/************************************************************************/

static void docEditShiftEditRange(	EditRange  *		er,
					int			paraFrom,
					int			stroffFrom,
					int			paraShift,
					int			stroffShift )
    {
    docAdjustEditPositionB( &(er->erStart),
				paraFrom, stroffFrom, paraShift, stroffShift );
    docAdjustEditPositionE( &(er->erEnd),
				paraFrom, stroffFrom, paraShift, stroffShift );

    return;
    }

/************************************************************************/
/*									*/
/*  Shift references after an insertion.				*/
/*									*/
/*  1)  If a paragraph is split, only shift the list number references	*/
/*	from the next paragraph.					*/
/*									*/
/************************************************************************/

void docEditShiftReferences(		EditOperation *		eo,
					const SelectionScope *	ssRoot,
					int			paraFrom,
					int			stroffFrom,
					int			sectShift,
					int			paraShift,
					int			stroffShift )
    {
    BufferDocument *		bd= eo->eoBd;

#   if SHOW_SELECTION_RANGE
    appDebug( "REFS FROM %3d(%3d) BY %2d(%2d) <%s>\n",
			    paraFrom, stroffFrom,
			    paraShift, stroffShift,
			    docExternalKindStr( ssRoot->ssInExternalItem) );
#   endif

    if  ( docSelectionSameScope( &(eo->eoSelectionScope), ssRoot ) )
	{
#	if SHOW_SELECTION_RANGE
	appDebug( "> SEL     %3d(%3d) .. %3d(%3d)\n",
			    eo->eoSelectedRange.erStart.epParaNr,
			    eo->eoSelectedRange.erStart.epStroff,
			    eo->eoSelectedRange.erEnd.epParaNr,
			    eo->eoSelectedRange.erEnd.epStroff );
	appDebug( "> FORMAT  %3d(%3d) .. %3d(%3d)\n",
			    eo->eoReformatRange.erStart.epParaNr,
			    eo->eoReformatRange.erStart.epStroff,
			    eo->eoReformatRange.erEnd.epParaNr,
			    eo->eoReformatRange.erEnd.epStroff );
#	endif

	if  ( eo->eoParaAdjustParagraphNumber >= 0	&&
	      eo->eoParaAdjustParagraphNumber >= paraFrom	)
	    { eo->eoParaAdjustParagraphNumber += paraShift;	}

	docEditShiftEditRange( &(eo->eoReformatRange),
			    paraFrom, stroffFrom, paraShift, stroffShift );
	docEditShiftEditRange( &(eo->eoSelectedRange),
			    paraFrom, stroffFrom, paraShift, stroffShift );

#	if SHOW_SELECTION_RANGE
	appDebug( "< SEL     %3d(%3d) .. %3d(%3d)\n",
			    eo->eoSelectedRange.erStart.epParaNr,
			    eo->eoSelectedRange.erStart.epStroff,
			    eo->eoSelectedRange.erEnd.epParaNr,
			    eo->eoSelectedRange.erEnd.epStroff );
	appDebug( "< FORMAT  %3d(%3d) .. %3d(%3d)\n",
			    eo->eoReformatRange.erStart.epParaNr,
			    eo->eoReformatRange.erStart.epStroff,
			    eo->eoReformatRange.erEnd.epParaNr,
			    eo->eoReformatRange.erEnd.epStroff );
#	endif
	}

    if  ( ssRoot->ssInExternalItem == DOCinBODY )
	{
	docShiftNoteReferences( bd, paraFrom, stroffFrom,
					sectShift, paraShift, stroffShift );

	if  ( paraShift != 0 )
	    {
	    int				i;
	    ListNumberTreeNode *	node;

	    int				listParaFrom= paraFrom;

	    /*  1  */
	    if  ( paraShift > 0 && stroffFrom > 0 )
		{ listParaFrom++;	}

	    node= bd->bdListNumberTrees;
	    for ( i= 0; i < bd->bdListNumberTreeCount; node++, i++ )
		{ docShiftListTreeReferences( node, listParaFrom, paraShift ); }
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Shift offsets in particules, selection and notes to reflect a	*/
/*  change in the paragraph string.					*/
/*									*/
/************************************************************************/

int docEditShiftParticuleOffsets(	EditOperation *		eo,
					BufferItem *		paraBi,
					int			paraNr,
					int			partFrom,
					int			partUpto,
					int			stroffFrom,
					int			stroffShift )
    {
    BufferDocument *		bd= eo->eoBd;
    int				rval;

    docAdjustEditPositionOffsetB( &(eo->eoSelectedRange.erStart),
					    paraNr, stroffFrom, stroffShift );
    docAdjustEditPositionOffsetE( &(eo->eoSelectedRange.erEnd),
					    paraNr, stroffFrom, stroffShift );

    rval= docShiftParticuleOffsets( bd, paraBi,
					    partFrom, partUpto, stroffShift );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Update paragraph properties and do extra administration.		*/
/*									*/
/************************************************************************/

int docEditUpdParaProperties(	EditOperation *			eo,
				PropertyMask *			pPpDoneMask,
				BufferItem *			paraBi,
				const PropertyMask *		ppSetMask,
				const ParagraphProperties *	ppNew,
				const int *			colorMap,
				const int *			listStyleMap )
    {
    BufferDocument *		bd= eo->eoBd;

    int				wasInList= 0;
    int				isInList= 0;
    int				listChange= 0;

    ListNumberTreeNode *	root= (ListNumberTreeNode *)0;
    int				override= -1;
    int				ilvl= 0;

    PropertyMask		ppDoneMask;
    PropertyMask		ppSetMaskThisPara;

    int				bulletFieldNr= -1;
    int				bulletPartBegin= -1;
    int				bulletPartEnd= -1;
    int				bulletStroffBegin= -1;
    int				bulletStroffEnd= -1;

    int				paraNr= -1;

    ppSetMaskThisPara= *ppSetMask;

    PROPmaskCLEAR( &ppDoneMask );

    /*  A  */
    if  ( paraBi->biParaListOverride > 0 )
	{
	wasInList= 1;
	override= paraBi->biParaListOverride;
	ilvl= paraBi->biParaListLevel;
	root= &(bd->bdListNumberTrees[override]);

	if  ( docDelimitParaHeadField( &bulletFieldNr,
			&bulletPartBegin, &bulletPartEnd,
			&bulletStroffBegin, &bulletStroffEnd, paraBi, bd ) )
	    { /* LDEB(paraBi->biParaListOverride); */	}

	PROPmaskUNSET( &ppSetMaskThisPara, PPpropLEFT_INDENT );
	PROPmaskUNSET( &ppSetMaskThisPara, PPpropFIRST_INDENT );
	}

    if  ( ! utilPropMaskIsEmpty( &ppSetMaskThisPara ) )
	{
	if  ( docUpdParaProperties( &ppDoneMask, &(paraBi->biParaProperties),
						&ppSetMaskThisPara, ppNew,
						colorMap, listStyleMap ) )
	    { LDEB(1); return -1;	}
	}

    if  ( paraBi->biParaListOverride > 0 )
	{ isInList= 1;	}

    if  ( wasInList || isInList )
	{ paraNr= docNumberOfParagraph( paraBi );	}

    if  ( wasInList && ! isInList )
	{
	int		partShift= 0;
	int		stroffShift= 0;

	int		addedStrlen= 0;
	int		textAttributeNumberFrom= -1;

	if  ( docListNumberTreeDeleteParagraph( root, paraNr ) )
	    { /*LLDEB(override,paraNr);*/	}

	if  ( docParaReplaceText( eo, paraBi, paraNr, bulletStroffBegin,
				    &partShift, &stroffShift,
				    bulletStroffEnd, (const unsigned char *)0,
				    addedStrlen, textAttributeNumberFrom ) )
	    { LDEB(1); return -1;	}

	eo->eoFieldUpdate |= FIELDdoLISTTEXT;
	eo->eoBulletsChanged++;
	}

    if  ( ! wasInList && isInList )
	{
	int		fieldNr= -1;
	int		partBegin= -1;
	int		partEnd= -1;
	int		stroffBegin= -1;
	int		stroffEnd= -1;

	if  ( docInsertListtextField( &fieldNr, &partBegin, &partEnd,
				    &stroffBegin, &stroffEnd, paraBi, bd ) )
	    { LDEB(1); return -1;	}

	/*  NO! done by docInsertListtextField()
	if  ( docEditShiftParticuleOffsets( eo, paraBi, paraNr,
				    partEnd+ 1, paraBi->biParaParticuleCount,
				    stroffBegin, stroffEnd- stroffBegin ) )
	    { LDEB(stroffEnd-stroffBegin);	}
	*/

	listChange= 1;
	eo->eoFieldUpdate |= FIELDdoLISTTEXT;
	eo->eoBulletsChanged++;
	}

    if  ( wasInList						&&
	  isInList						&&
	  ( paraBi->biParaListOverride != override	||
	    paraBi->biParaListLevel != ilvl		)	)
	{
	if  ( docListNumberTreeDeleteParagraph( root, paraNr ) )
	    { LLDEB(override,paraNr);	}

	override= paraBi->biParaListOverride;
	ilvl= paraBi->biParaListLevel;
	root= &(bd->bdListNumberTrees[override]);

	if  ( docListNumberTreeInsertParagraph( root, ilvl, paraNr ) )
	    { LLLDEB(override,ilvl,paraNr);	}

	listChange= 1;
	eo->eoFieldUpdate |= FIELDdoLISTTEXT;
	eo->eoBulletsChanged++;
	}

    if  ( listChange )
	{
	int		indentChanged= 0;

	if  ( docAdaptParagraphToListLevel( &indentChanged, paraBi, bd ) )
	    { LDEB(1);		}
	}

    *pPpDoneMask= ppDoneMask;
    return 0;
    }

