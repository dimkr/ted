/************************************************************************/
/*									*/
/*  Buffer administration routines relating to the text particules in a	*/
/*  text paragraph.							*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docEdit.h"

/*
# define DEB_PARTICULES
*/

/************************************************************************/
/*									*/
/*  Split a text particule.						*/
/*									*/
/*  E.G. To change the attributes of part of its text or to insert	*/
/*  something in the middle.						*/
/*									*/
/************************************************************************/

int docSplitTextParticule(	TextParticule **		pTpPart,
				TextParticule **		pTpNext,
				BufferItem *			paraBi,
				int				part,
				int				stroff )
    {
    TextParticule	tpScratch;
    TextParticule *	tpPart;
    TextParticule *	tpNext;

    if  ( part < 0 || part >= paraBi->biParaParticuleCount )
	{ LLDEB(part,paraBi->biParaParticuleCount); return -1;	}

    tpPart= paraBi->biParaParticules+ part;
    tpScratch= *tpPart;

    if  ( tpPart->tpKind != DOCkindTEXT			||
	  stroff <= tpPart->tpStroff			||
	  stroff >= tpPart->tpStroff+ tpPart->tpStrlen	)
	{
	SDEB(docKindStr(tpPart->tpKind));
	LLLDEB(stroff,tpPart->tpStroff,tpPart->tpStrlen);
	return -1;
	}

    tpPart= docInsertTextParticule( paraBi, part,
					    tpScratch.tpStroff,
					    stroff- tpScratch.tpStroff,
					    tpScratch.tpKind,
					    tpScratch.tpTextAttributeNumber );
    if  ( ! tpPart )
	{ LXDEB(part,tpPart); return -1;	}

    tpNext= tpPart+ 1;

    tpNext->tpStroff= stroff;
    tpNext->tpStrlen= ( tpScratch.tpStroff+ tpScratch.tpStrlen )- stroff;

    *pTpPart= tpPart;
    *pTpNext= tpNext;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Add a Particule to a paragraph.					*/
/*									*/
/************************************************************************/

static TextParticule *	docInsertParticules(	BufferItem *	paraBi,
						int		part,
						int		count )
    {
    int			i;
    TextParticule *	tp;

    tp= (TextParticule *)realloc( paraBi->biParaParticules,
	    ( paraBi->biParaParticuleCount+ count ) * sizeof( TextParticule ) );
    if  ( ! tp )
	{ LLDEB(paraBi->biParaParticuleCount,tp); return tp; }
    paraBi->biParaParticules= tp;

    if  ( part < 0 )
	{ part= paraBi->biParaParticuleCount;	}
    else{
	int	tail= paraBi->biParaParticuleCount- part;

	tp= paraBi->biParaParticules+ paraBi->biParaParticuleCount+ count- 1;

	for ( i= 0; i < tail; tp--, i++ )
	    { tp[0]= tp[-count];	}
	}

    tp= paraBi->biParaParticules+ part;
    for ( i= 0; i < count; tp++, i++ )
	{
	tp->tpTextAttributeNumber= -1;

	tp->tpStroff= 0;
	tp->tpStrlen= 0;
	tp->tpKind= DOCkindTEXT;
	tp->tpObjectNumber= -1;

	tp->tpX0= 0;
	tp->tpPixelsWide= 0;

	paraBi->biParaParticuleCount++;
	}

    return paraBi->biParaParticules+ part;
    }

/************************************************************************/
/*									*/
/*  Insert a text particule.						*/
/*									*/
/************************************************************************/

TextParticule *	docInsertTextParticule(	BufferItem *	bi,
					int		part,
					int		off,
					int		len,
					int		kind,
					int		textAttributeNumber )
    {
    const int		count= 1;
    TextParticule *	tp;

    tp= docInsertParticules( bi, part, count );
    if  ( ! tp )
	{ XDEB(tp); return tp;	}

    tp->tpStroff= off;
    tp->tpStrlen= len;
    tp->tpKind= kind;
    tp->tpTextAttributeNumber= textAttributeNumber;

    return tp;
    }

/************************************************************************/
/*									*/
/*  Delete a series of particules.					*/
/*									*/
/************************************************************************/

void docDeleteParticules(	BufferItem *	bi,
				int		first,
				int		count )
    {
    if  ( first > bi->biParaParticuleCount )
	{
	LLDEB(first,bi->biParaParticuleCount);
	first= bi->biParaParticuleCount;
	}

    if  ( first+ count > bi->biParaParticuleCount )
	{
	LLDEB(first+count,bi->biParaParticuleCount);
	count= bi->biParaParticuleCount- first;
	}

    if  ( count <= 0 )
	{ LDEB(count); return;	}

    bi->biParaParticuleCount -= count;

    while( first < bi->biParaParticuleCount )
	{
	bi->biParaParticules[first]= bi->biParaParticules[first+ count];
	first++;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Copy particules plus contents from one paragraph to another.	*/
/*									*/
/*  This might be a copy to a different document, so font numbers and	*/
/*  attribute numbers need to be recalculated.				*/
/*									*/
/*  1)  Find out where to insert.					*/
/*	(At the end of the paragraph or before particule[partTo].	*/
/*  2)  Find position and length of the source string.			*/
/*  3)  Find out whether to replace the empty particule that we use to	*/
/*	have at least one particule in a paragraph.			*/
/*  4)  Insert the desired number of particules in the correct		*/
/*	position. Note that that is one less in a paragraph that was	*/
/*	originally empty.						*/
/*  5)  Insert the new text into the paragraph string.			*/
/*  6)  Shift the particules in the tail of the target paragraph to	*/
/*	make place for the new text string bytes. The possible		*/
/*	overwritten 'empty' particule should not be shifted.		*/
/*  7)  Patch the particules in the hole created above to correct	*/
/*	values derived from the source particules.			*/
/*									*/
/************************************************************************/

int docCopyParticules(	DocumentCopyJob *		dcj,
			EditOperation *			eo,
			BufferItem *			biTo,
			const BufferItem *		biFrom,
			int				partTo,
			int				partFrom,
			int				countFrom,
			int *				pParticulesInserted,
			int *				pCharactersCopied )
    {
    TextParticule *		tpTo;
    const TextParticule *	tpFrom;

    int				stroffTo;
    int				stroffShift;

    int				i;

    int				replaceEmpty= 0;

    int				stroffFrom;
    int				strlenFrom;

    int				notesCopied= 0;

    int				paraNrTo= docNumberOfParagraph( biTo );

    /*  1  */
    if  ( partTo > biTo->biParaParticuleCount )
	{
	LLDEB(partTo,biTo->biParaParticuleCount);
	partTo= biTo->biParaParticuleCount;
	}

    if  ( partTo == biTo->biParaParticuleCount )
	{ stroffTo= biTo->biParaStrlen;				}
    else{ stroffTo= biTo->biParaParticules[partTo].tpStroff;	}

    /*  2  */
    tpFrom= biFrom->biParaParticules+ partFrom;
    stroffFrom= tpFrom->tpStroff;
    strlenFrom= tpFrom[countFrom- 1].tpStroff+ tpFrom[countFrom- 1].tpStrlen-
							    tpFrom->tpStroff;

    /*  3  */
    if  ( biTo->biParaStrlen == 0 && biTo->biParaParticuleCount == 1 )
	{
	if  ( partTo < 0 || partTo > 1 )
	    { LDEB(partTo);	}
	if  ( partTo != 0 )
	    { partTo= 0;	}

	replaceEmpty= 1;
	}

    /*  4  */
    tpTo= docInsertParticules( biTo, partTo, countFrom- replaceEmpty );
    if  ( ! tpTo )
	{ XDEB(tpTo); return -1;	}

    /*  5  */
    if  ( docParaStringReplace( &stroffShift, biTo, stroffTo, stroffTo,
			    biFrom->biParaString+ stroffFrom, strlenFrom ) )
	{
	LDEB(strlenFrom);
	docDeleteParticules( biTo, partTo, countFrom- replaceEmpty );
	return -1;
	}

    /*  6  */
    if  ( docEditShiftParticuleOffsets( eo, biTo, paraNrTo,
						partTo+ countFrom,
						biTo->biParaParticuleCount,
						stroffFrom, stroffShift ) )
	{ LDEB(stroffShift);	}

    /*  7  */
    tpTo= biTo->biParaParticules+ partTo;
    for ( i= 0; i < countFrom; tpTo++, tpFrom++, i++ )
	{
	int		textAttributeNumberTo;

	textAttributeNumberTo= docMapTextAttributeNumber( dcj,
					    tpFrom->tpTextAttributeNumber );
	if  ( textAttributeNumberTo < 0 )
	    { LDEB(textAttributeNumberTo); return -1;	}

	tpTo->tpStroff= stroffTo;
	tpTo->tpStrlen= tpFrom->tpStrlen;
	tpTo->tpKind= tpFrom->tpKind;
	tpTo->tpTextAttributeNumber= textAttributeNumberTo;

	/*  4  */
	if  ( tpFrom->tpKind == DOCkindFIELDSTART	&&
	      dcj->dcjRefFileName			)
	    {
	    if  ( docCopyFieldRelative( dcj, eo,
					    biTo, biFrom, tpTo, tpFrom ) )
		{ LLDEB(partTo,i); return -1;	}
	    }
	else{
	    if  ( docCopyParticuleData( dcj, eo,
					    biTo, biFrom, tpTo, tpFrom ) )
		{ LLDEB(partTo,i); return -1;	}
	    }

	stroffTo += tpTo->tpStrlen;

	if  ( tpTo->tpKind == DOCkindNOTE )
	    { notesCopied++;	}
	}

    dcj->dcjNotesCopied += notesCopied;
    *pParticulesInserted += countFrom- replaceEmpty;
    *pCharactersCopied += stroffShift;

    return 0;
    }

int docCopyParticuleAndData(		TextParticule **	pTpTo,
					DocumentCopyJob *	dcj,
					EditOperation *		eo,
					BufferItem *		paraBiTo,
					int			partTo,
					int			stroffTo,
					int			strlenTo,
					const BufferItem *	paraBiFrom,
					const TextParticule *	tpFrom )
    {
    TextParticule	tpSaved;
    int			textAttributeNumberTo;

    TextParticule *	tpTo;

    tpSaved= *tpFrom;

    textAttributeNumberTo= docMapTextAttributeNumber( dcj,
					tpSaved.tpTextAttributeNumber );

    if  ( textAttributeNumberTo < 0 )
	{ LDEB(textAttributeNumberTo); return -1;	}

    if  ( partTo < 0 )
	{ partTo= paraBiTo->biParaParticuleCount;	}

    tpTo= docInsertTextParticule( paraBiTo, partTo,
					    stroffTo, strlenTo,
					    tpSaved.tpKind,
					    textAttributeNumberTo );
    if  ( ! tpTo )
	{ LXDEB(partTo,tpTo); return -1;	}

    if  ( docCopyParticuleData( dcj, eo,
				    paraBiTo, paraBiFrom, tpTo, &tpSaved ) )
	{
	docDeleteParticules( paraBiTo, partTo, 1 );
	LDEB(partTo); return -1;
	}

    *pTpTo= tpTo;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Save paragraph contents for readers.				*/
/*									*/
/************************************************************************/

int docSaveParticules(	BufferDocument *		bd,
			BufferItem *			bi,
			const TextAttribute *		ta,
			const unsigned char *		map,
			const unsigned char *		text,
			int				len )
    {
    int			textAttributeNumber;

    textAttributeNumber= docTextAttributeNumber( bd, ta );
    if  ( textAttributeNumber < 0 )
	{ LDEB(textAttributeNumber); return -1;	}

    if  ( docInflateTextString( bi, len ) )
	{ LLDEB(bi->biParaStrlen,len); return -1; }

    while( len > 0 )
	{
	const unsigned char *	particuleText= text;
	int			particuleLength= 0;
	unsigned char *		to;
	int			i;

	int	off= bi->biParaStrlen;

	while( len > 0 && text[0] != ' ' )
	    { text++; particuleLength++; len--; }
	while( len > 0 && text[0] == ' ' )
	    { text++; particuleLength++; len--; }

	/*
	appDebug( "%s(%3d): \"%.*s\"\n",
			__FILE__, __LINE__, particuleLength, particuleText );
	*/

	to= bi->biParaString+ off;
	if  ( map )
	    {
	    for ( i= 0; i < particuleLength; i++ )
		{ *(to++)= map[*(particuleText++)];	}
	    }
	else{
	    for ( i= 0; i < particuleLength; i++ )
		{ *(to++)= *(particuleText++);	}
	    }
	*to= '\0';

	bi->biParaStrlen += particuleLength;

	if  ( ! docInsertTextParticule( bi, -1, off, bi->biParaStrlen- off,
					DOCkindTEXT, textAttributeNumber ) )
	    { LDEB(bi->biParaParticuleCount); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a length 1 particule that is used to represent tabs, breaks,	*/
/*  or objects like images in the paragraph and in the string of the	*/
/*  paragraph.								*/
/*									*/
/************************************************************************/

int docSaveSpecialParticule(	BufferDocument *		bd,
				BufferItem *			paraBi,
				int				atBegin,
				const TextAttribute *		ta,
				int				kind )
    {
    int			textAttributeNumber;
    int			n= -1;

    int			stroffShift= 0;
    int			stroff= paraBi->biParaStrlen;

    if  ( paraBi->biLevel != DOClevPARA )
	{ LLDEB(paraBi->biLevel,DOClevPARA); return -1;	}

    if  ( atBegin )
	{ n= 0; stroff= 0;	}

    textAttributeNumber= docTextAttributeNumber( bd, ta );
    if  ( textAttributeNumber < 0 )
	{ LDEB(textAttributeNumber); return -1;	}

    if  ( docParaStringReplace( &stroffShift, paraBi, stroff, stroff,
					    (const unsigned char *)" ", 1 ) )
	{ LDEB(paraBi->biParaStrlen); return -1;	}

    if  ( ! docInsertTextParticule( paraBi, n, stroff, 1,
						kind, textAttributeNumber ) )
	{ LDEB(paraBi->biParaParticuleCount); return -1;	}


    if  ( atBegin )
	{
	docShiftParticuleOffsets( bd, paraBi,
			    n+ 1, paraBi->biParaParticuleCount, stroffShift );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a zero length particule that is used for internal		*/
/*  administration such as delimiting text fields.			*/
/*									*/
/*  Also used to insert the one and only particule in an empty		*/
/*  paragraph.								*/
/*									*/
/*  1)  Paranoia: can be called directly from file readers.		*/
/*									*/
/************************************************************************/

int docInsertAdminParticule(		BufferDocument *	bd,
					BufferItem *		paraBi,
					int			off,
					int			n,
					int			objectNumber,
					const TextAttribute *	ta,
					int			kind )
    {
    const int		len= 0;

    TextParticule *	tp;
    int			textAttrNr;

    /*  1  */
    if  ( ! bd || ! paraBi )
	{ XXDEB(bd,paraBi); return -1;	}

    textAttrNr= docTextAttributeNumber( bd, ta );
    if  ( textAttrNr < 0 )
	{ LDEB(textAttrNr); return -1;	}

    tp= docInsertTextParticule( paraBi, n, off, len, kind, textAttrNr );
    if  ( ! tp )
	{ XDEB(tp); return -1;	}

    tp->tpObjectNumber= objectNumber;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make sure the a paragraph does not end in an explicit break.	*/
/*									*/
/************************************************************************/

int docCheckNoBreakAsLast(	EditOperation *		eo,
				BufferItem *		paraBi )
    {
    const int			part= paraBi->biParaParticuleCount;
    const int			stroff= paraBi->biParaStrlen;
    const int			len= 0;

    const TextParticule *	tp= paraBi->biParaParticules+ part- 1;

    if  ( tp->tpKind != DOCkindLINEBREAK	&&
	  tp->tpKind != DOCkindPAGEBREAK	&&
	  tp->tpKind != DOCkindCOLUMNBREAK	)
	{ return 0;	}

    tp= docInsertTextParticule( paraBi, part, stroff, len,
				    DOCkindTEXT, tp->tpTextAttributeNumber );
    if  ( ! tp )
	{ XDEB(tp); return -1;	}

    docEditIncludeItemInReformatRange( eo, paraBi );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Redivide a piece of a paragraph in particules.			*/
/*									*/
/************************************************************************/

int docRedivideStringInParticules(	BufferItem *	bi,
					int		strOff,
					int		strLen,
					int		part,
					int		partsFree,
					int		textAttributeNumber )
    {
    int			bytesDone= 0;
    int			partsDone= 0;

    TextParticule *	tp= bi->biParaParticules+ part;

    while( bytesDone < strLen )
	{
	int	len= 0;

#	ifdef DEB_PARTICULES
	char *	label= "???";
#	endif

	while( bytesDone+ len < strLen			&&
	       bi->biParaString[strOff+ len] != ' '	)
	    { len++;	}
	while( bytesDone+ len < strLen			&&
	       bi->biParaString[strOff+ len] == ' '	)
	    { len++;	}

	if  ( partsDone < partsFree )
	    {
	    tp->tpStroff= strOff;
	    tp->tpStrlen= len;
	    tp->tpKind= DOCkindTEXT;
	    tp->tpTextAttributeNumber= textAttributeNumber;

	    tp->tpX0= -1;
	    tp->tpPixelsWide= 0;

#	    ifdef DEB_PARTICULES
	    label= "NW.";
#	    endif
	    }
	else{
	    tp= docInsertTextParticule( bi, part,
			    strOff, len, DOCkindTEXT, textAttributeNumber );
	    if  ( ! tp )
		{ XDEB(tp); return -1;	}
#	    ifdef DEB_PARTICULES
	    label= "NW+";
#	    endif
	    }

#	ifdef DEB_PARTICULES
	appDebug( "%s %3d: [%4d..%4d] %s \"%.*s\" len= %d\n", label, part,
		    tp->tpStroff,
		    tp->tpStroff+ tp->tpStrlen,
		    docKindStr( tp->tpKind ),
		    (int)tp->tpStrlen,
		    bi->biParaString+ tp->tpStroff,
		    tp->tpStrlen );
#	endif

	strOff += len; bytesDone += len;
	partsDone++; part++, tp++;
	}

    return partsDone;
    }

/************************************************************************/
/*									*/
/*  Shift the particules in a paragraph after an insertion or a		*/
/*  deletion.								*/
/*									*/
/************************************************************************/

int docShiftParticuleOffsets(	BufferDocument *	bd,
				BufferItem *		paraBi,
				int			partFrom,
				int			partUpto,
				int			stroffShift )
    {
    int			part;
    TextParticule *	tp;

    if  ( stroffShift < 0 )
	{
	tp= paraBi->biParaParticules+ partFrom;
	for ( part= partFrom; part < partUpto; tp++, part++ )
	    {
	    if  ( tp->tpKind == DOCkindNOTE )
		{
		DocumentNote *	dn;

		if  ( docGetNote( &dn, bd, paraBi, tp->tpStroff ) < 0 )
		    { LDEB(tp->tpStroff);		}
		else{ dn->dnStroff += stroffShift;	}
		}

	    tp->tpStroff += stroffShift;
	    }
	}

    if  ( stroffShift > 0 )
	{
	tp= paraBi->biParaParticules+ partUpto- 1;
	for ( part= partUpto- 1; part >= partFrom; tp--, part-- )
	    {
	    if  ( tp->tpKind == DOCkindNOTE )
		{
		DocumentNote *	dn;

		if  ( docGetNote( &dn, bd, paraBi, tp->tpStroff ) < 0 )
		    { LDEB(tp->tpStroff);		}
		else{ dn->dnStroff += stroffShift;	}
		}

	    tp->tpStroff += stroffShift;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Determine a text attribute number for a particule.			*/
/*									*/
/************************************************************************/

int docTextAttributeNumber(	BufferDocument *	bd,
				const TextAttribute *	ta )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);
    const DocumentFontList *	dfl= &(dp->dpFontList);

    int				textAttributeNumber;

    TextAttribute		taCopy= *ta;

    if  ( taCopy.taFontNumber < 0			||
	  taCopy.taFontNumber >= dfl->dflFontCount	)
	{
	if  ( taCopy.taFontNumber >= dfl->dflFontCount )
	    { LLDEB(taCopy.taFontNumber,dfl->dflFontCount);	}

	taCopy.taFontNumber= dp->dpDefaultFont;

	if  ( taCopy.taFontNumber < 0 )
	    { taCopy.taFontNumber= 0;	}

	if  ( taCopy.taFontNumber >= dfl->dflFontCount )
	    { LLDEB(taCopy.taFontNumber,dfl->dflFontCount);	}
	}

    textAttributeNumber= utilTextAttributeNumber(
				    &(bd->bdTextAttributeList), &taCopy );
    if  ( textAttributeNumber < 0 )
	{ LDEB(textAttributeNumber); return -1;	}

    return textAttributeNumber;
    }

