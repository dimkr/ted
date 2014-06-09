/************************************************************************/
/*									*/
/*  Read an RTF text file into a BufferDocument				*/
/*  Read fields, bookmarks, footnotes, headers, footers.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtf.h"

/************************************************************************/
/*									*/
/*  Consume a field.							*/
/*									*/
/************************************************************************/

static int docRtfReadFldrslt(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int			res;

    res= docRtfReadGroup( sis, DOClevPARA,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfDocumentWords, docRtfDocumentGroups,
				docRtfTextParticule, (RtfCommitGroup)0 );

    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

static int docRtfFldinst(	RtfReadingContext *	rrc,
				const unsigned char *	text,
				int			size	)
    {
    BufferDocument *		bd= rrc->rrcBd;
    DocumentField *		df;

    df= bd->bdFieldList.dflFields+ rrc->rrcFieldNumber;

    if  ( df->dfKind != DOCfkUNKNOWN )
	{ SDEB(docFieldKindStr(df->dfKind));	}

    if  ( docAddToFieldInst( df, text, size ) )
	{ LDEB(size); return -1;	}

    return 0;
    }

static int docRtfReadFldinst(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int		res;

    res= docRtfReadGroup( sis, DOClevPARA,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfDocumentWords, docRtfDocumentGroups,
				docRtfFldinst, (RtfCommitGroup)0 );

    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

static RtfControlWord	docRtfFieldGroups[]=
    {
	{ "fldrslt",	RTFidFLDRSLT,	DOClevPARA, docRtfReadFldrslt, },
	{ "fldinst",	RTFidFLDINST,	DOClevPARA, docRtfReadFldinst, },

	{ 0, 0, 0 }
    };

static RtfControlWord	docRtfFieldWords[]=
    {
	{ "flddirty",	RTFidFLDDIRTY,	DOClevPARA, docRtfIgnoreWord, },
	{ "fldedit",	RTFidFLDEDIT,	DOClevPARA, docRtfIgnoreWord, },
	{ "fldlock",	RTFidFLDLOCK,	DOClevPARA, docRtfIgnoreWord, },
	{ "fldpriv",	RTFidFLDPRIV,	DOClevPARA, docRtfIgnoreWord, },

	{ 0, 0, 0 }
    };

static int docRtfReadStartField(	int *			pPart,
					int *			pFieldNumber,
					RtfReadingContext *	rrc	)
    {
    RtfReadingState *	rrs= rrc->rrcState;
    BufferDocument *	bd= rrc->rrcBd;
    BufferItem *	bi= rrc->rrcBi;

    DocumentField *	df;

    int			part= bi->biParaParticuleCount;

    if  ( docRtfFlushBookmarks( -1, bi, rrc ) )
	{ LDEB(1); return -1;	}

    df= docClaimField( &(rrc->rrcFieldNumber), &(bd->bdFieldList) );
    if  ( ! df )
	{ XDEB(df); return -1;	}
    df->dfKind= DOCfkUNKNOWN;

    if  ( docInsertAdminParticule( bd, bi, bi->biParaStrlen,
					    bi->biParaParticuleCount,
					    rrc->rrcFieldNumber,
					    &(rrs->rrsTextAttribute),
					    DOCkindFIELDSTART ) )
	{ LDEB(1); return -1;	}

    rrc->rrcAfterNoteref= 0;

    *pFieldNumber= rrc->rrcFieldNumber;
    *pPart= part;

    return 0;
    }

static int docRtfTerminateField(	BufferItem *		bi,
					int			fieldNumber,
					RtfReadingContext *	rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;
    BufferDocument *	bd= rrc->rrcBd;

    if  ( rrc->rrcBi != bi )
	{ XXDEB(rrc->rrcBi,bi); }

    if  ( docInsertAdminParticule( bd, bi, bi->biParaStrlen,
					    bi->biParaParticuleCount,
					    fieldNumber,
					    &(rrs->rrsTextAttribute),
					    DOCkindFIELDEND ) )
	{ LDEB(1); return -1;	}

    rrc->rrcAfterNoteref= 0;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Finish a 'Real' field. I.E. A field that is a field in RTF terms.	*/
/*									*/
/************************************************************************/

static int docRtfFinishField(	BufferItem *		bi,
				int			fieldNumber,
				RtfReadingContext *	rrc )
    {
    DocumentField *			df;

    unsigned char *			s;
    int					n;
    unsigned char *			p;
    int					i;
    const FieldKindInformation *	fki;

    if  ( docRtfTerminateField( bi, fieldNumber, rrc ) )
	{ LDEB(fieldNumber); return -1;	}

    df= rrc->rrcBd->bdFieldList.dflFields+ fieldNumber;
    s= df->dfInstructions.mbBytes;
    n= df->dfInstructions.mbSize;

    i= 0;
    while( i < n && isspace( *s ) )
	{ i++; s++;	}
    p= s;
    while( i < n && isalpha( *p ) )
	{
	if  ( islower( *p ) )
	    { *p= toupper( *p );	}
	i++; p++;
	}

    if  ( DOC_FieldKindCount != DOCfk_COUNT )
	{ LLDEB(DOC_FieldKindCount,DOCfk_COUNT); return -1;	}

    /* LSDEB(p-s,(char *)s); */

    fki= DOC_FieldKinds;
    for ( i= 0; i < DOC_FieldKindCount; fki++, i++ )
	{
	if  ( ! fki->fkiIsFieldInRtf )
	    { continue;	}

	if  ( ! strncmp( (const char *)s, fki->fkiLabel, p- s )	&&
	      ! fki->fkiLabel[p- s]			)
	    { break;	}
	}

    if  ( i < DOC_FieldKindCount )
	{ df->dfKind= i;	}

    return 0;
    }

int docRtfReadField(	SimpleInputStream *	sis,
			const RtfControlWord *	rcw,
			int			arg,
			RtfReadingContext *	rrc )
    {
    int			res;
    BufferItem *	bi= rrc->rrcBi;

    int			startParticule;
    int			fieldNumber= -1;

    if  ( docRtfReadStartField( &startParticule, &fieldNumber, rrc ) )
	{ LDEB(bi->biParaParticuleCount); return -1;	}

    res= docRtfReadGroup( sis, DOClevPARA,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfFieldWords, docRtfFieldGroups,
				docRtfIgnoreText, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    if  ( docRtfFlushBookmarks( -1, bi, rrc ) )
	{ LDEB(1); return -1;	}

    if  ( docRtfFinishField( bi, fieldNumber, rrc ) )
	{ LDEB(bi->biParaParticuleCount); return -1;	}

    return res;
    }

static RtfControlWord	docRtfXE_Words[]=
    {
	{ "xef",	RTFidXEF,	DOClevPARA, docRtfIgnoreWord, },
	{ "bxe",	RTFidBXE,	DOClevPARA, docRtfIgnoreWord, },
	{ "ixe",	RTFidIXE,	DOClevPARA, docRtfIgnoreWord, },

	{ 0, 0, 0 }
    };

static RtfControlWord	docRtfTC_Words[]=
    {
	{ "tcf",	RTFidTCF,	DOClevPARA, docRtfIgnoreWord, },
	{ "tcl",	RTFidTCL,	DOClevPARA, docRtfIgnoreWord, },

	{ 0, 0, 0 }
    };

int docRtfLookupEntry(		SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    int			res;
    BufferItem *	bi= rrc->rrcBi;

    int			particuleKind;
    int			fieldKind;

    int			startParticule;
    int			fieldNumber;

    RtfControlWord *	firstApply= (RtfControlWord *)0;
    RtfControlWord *	contolWords= (RtfControlWord *)0;
    RtfControlWord *	groupWords= (RtfControlWord *)0;

    TextParticule *	tp;

    switch( rcw->rcwID )
	{
	case DOCfkXE:
	    particuleKind= DOCkindXE;
	    fieldKind= DOCfkXE;
	    contolWords= docRtfXE_Words;
	    break;

	case DOCfkTC:
	    particuleKind= DOCkindTC;
	    fieldKind= DOCfkTC;
	    contolWords= docRtfTC_Words;
	    break;

	case DOCfkTCN:
	    particuleKind= DOCkindTC;
	    fieldKind= DOCfkTCN;
	    contolWords= docRtfTC_Words;
	    break;

	default:
	    SDEB(rcw->rcwWord); return -1;
	}

    if  ( docRtfReadStartField( &startParticule, &fieldNumber, rrc ) )
	{ LDEB(bi->biParaParticuleCount); return -1;	}

    tp= bi->biParaParticules+ startParticule;
    tp->tpKind= particuleKind;

    res= docRtfReadGroup( sis, DOClevPARA,
				firstApply, 0, 0, rrc,
				contolWords, groupWords,
				docRtfFldinst, (RtfCommitGroup)0 );

    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

/************************************************************************/
/*									*/
/*  Process bookmarks.							*/
/*									*/
/************************************************************************/

static int docRtfBookmarkName(	RtfReadingContext *	rrc,
				const unsigned char *	name,
				int			len )
    {
    if  ( rrc->rrcBookmarkSize+ len > DOCmaxBOOKMARK )
	{
	LLDEB(rrc->rrcBookmarkSize+ len,DOCmaxBOOKMARK);
	len= DOCmaxBOOKMARK- rrc->rrcBookmarkSize;
	if  ( len < 0 )
	    { len= 0;	}
	}

    memcpy( rrc->rrcBookmarkName+ rrc->rrcBookmarkSize, name, len );
    rrc->rrcBookmarkSize += len;
    rrc->rrcBookmarkName[rrc->rrcBookmarkSize]= '\0';

    return 0;
    }

static RtfControlWord	docRtfBkmkStartWords[]=
    {
	{ "bkmkcolf",	RTFidBKMKCOLF,	DOClevANY, docRtfIgnoreWord, },
	{ "bkmkcoll",	RTFidBKMKCOLL,	DOClevANY, docRtfIgnoreWord, },
	
	{ 0, 0, 0 }
    };

int docRtfBkmkStart(	SimpleInputStream *	sis,
			const RtfControlWord *	rcw,
			int			arg,
			RtfReadingContext *	rrc )
    {
    BufferDocument *	bd= rrc->rrcBd;
    RtfBookmarkLevel *	rbl;

    int			res;

    int			startParticule;
    int			fieldNumber;
    DocumentField *	df;

    rrc->rrcBookmarkName[0]= '\0';
    rrc->rrcBookmarkSize= 0;

    if  ( docRtfReadStartField( &startParticule, &fieldNumber, rrc ) )
	{ LDEB(1); return -1;	}

    res= docRtfReadGroup( sis, DOClevPARA,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfBkmkStartWords, docRtfEmptyTable,
				docRtfBookmarkName, (RtfCommitGroup)0 );

    if  ( res )
	{ SLDEB(rcw->rcwWord,res); return res;	}

    df= bd->bdFieldList.dflFields+ fieldNumber;
    if  ( docFieldSetBookmark( df,
			    rrc->rrcBookmarkName, rrc->rrcBookmarkSize ) )
	{ LDEB(1); return -1;	}
    df->dfKind= DOCfkBOOKMARK;

    rbl= (RtfBookmarkLevel *)malloc( sizeof(RtfBookmarkLevel) );
    if  ( ! rbl )
	{ XDEB(rbl); return -1;	}
    rbl->rblFieldNumber= fieldNumber;
    rbl->rblPrevious= rrc->rrcTopBookmark;
    rrc->rrcTopBookmark= rbl;

    return res;
    }

int docRtfFlushBookmarks(	int			fieldNumber,
				BufferItem *		bi,
				RtfReadingContext *	rrc )
    {
    RtfBookmarkLevel *	rbl;

    if  ( fieldNumber >= 0 )
	{
	rbl= rrc->rrcTopBookmark;
	while( rbl )
	    {
	    if  ( rbl->rblFieldNumber == fieldNumber )
		{ break;	}

	    rbl= rbl->rblPrevious;
	    }

	if  ( ! rbl )
	    { return 0;	}
	}

    rbl= rrc->rrcTopBookmark;
    while( rbl )
	{
	if  ( rbl->rblFieldNumber == fieldNumber )
	    { break;	}

	if  ( docRtfFinishField( bi, rbl->rblFieldNumber, rrc ) )
	    { LDEB(1); return -1;	}

	rrc->rrcTopBookmark= rbl->rblPrevious;
	free( rbl );
	rbl= rrc->rrcTopBookmark;
	}

    return 0;
    }

int docRtfBkmkEnd(	SimpleInputStream *	sis,
			const RtfControlWord *	rcw,
			int			arg,
			RtfReadingContext *	rrc )
    {
    BufferDocument *	bd= rrc->rrcBd;

    int			res;
    int			fieldNumber;

    rrc->rrcBookmarkName[0]= '\0';
    rrc->rrcBookmarkSize= 0;

    res= docRtfReadGroup( sis, DOClevPARA,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfEmptyTable, docRtfEmptyTable,
				docRtfBookmarkName, (RtfCommitGroup)0 );

    if  ( res )
	{ SLDEB(rcw->rcwWord,res); return res;	}

    for ( fieldNumber= bd->bdFieldList.dflFieldCount- 1;
					    fieldNumber >= 0; fieldNumber-- )
	{
	DocumentField *		df;
	const char *		markName;
	int			markSize;

	df= bd->bdFieldList.dflFields+ fieldNumber;

	if  ( df->dfKind != DOCfkBOOKMARK )
	    { continue;	}

	if  ( docFieldGetBookmark( df, &markName, &markSize ) )
	    { LDEB(1); continue;	}

	if  ( markSize != rrc->rrcBookmarkSize )
	    { continue;	}

	if  ( memcmp( markName, rrc->rrcBookmarkName, rrc->rrcBookmarkSize ) )
	    { continue;	}

	break;
	}

    if  ( fieldNumber < 0 )
	{ /*SDEB((char *)rrc->rrcBookmarkName);*/ return 0;	}

    if  ( docRtfFlushBookmarks( fieldNumber, rrc->rrcBi, rrc ) )
	{ LDEB(1); return -1;	}

    if  ( docRtfFinishField( rrc->rrcBi, fieldNumber, rrc ) )
	{ LDEB(1); return -1;	}

    if  ( rrc->rrcTopBookmark )
	{
	RtfBookmarkLevel *	rbl= rrc->rrcTopBookmark;

	if  ( rbl->rblFieldNumber == fieldNumber )
	    {
	    rrc->rrcTopBookmark= rbl->rblPrevious;
	    free( rbl );
	    }
	}

    return res;
    }

int docRtfTextSpecialToField(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    BufferItem *		bi= rrc->rrcBi;
    BufferDocument *		bd= rrc->rrcBd;
    DocumentField *		df;

    int				startParticule;
    int				fieldNumber;

    const unsigned char *	fieldinst= (const unsigned char *)"?";
    int				fieldsize= 1;
    const unsigned char *	fieldRslt= (const unsigned char *)"?";

    int				afterNoteref= 0;

    switch( rcw->rcwID )
	{
	case DOCfkPAGE:
	    fieldinst= (const unsigned char *)" PAGE ";
	    fieldsize= 6;
	    break;
	case DOCfkDATE:
	    fieldinst= (const unsigned char *)" DATE \\* MERGEFORMAT ";
	    fieldsize= 21;
	    break;
	case DOCfkTIME:
	    fieldinst= (const unsigned char *)" TIME \\* MERGEFORMAT ";
	    fieldsize= 21;
	    break;
	case DOCfkSECTION:
	    fieldinst= (const unsigned char *)" SECTION ";
	    fieldsize= 9;
	    break;

	case DOCfkCHFTN:
	    fieldinst= (const unsigned char *)" -CHFTN ";
	    fieldsize= 8;
	    fieldRslt= (const unsigned char *)"1";
	    afterNoteref= 1;
	    break;

	default:
	    SDEB(rcw->rcwWord); return -1;
	}

    if  ( docRtfReadStartField( &startParticule, &fieldNumber, rrc ) )
	{ LDEB(bi->biParaParticuleCount); return -1;	}

    df= bd->bdFieldList.dflFields+ fieldNumber;

    if  ( docRtfTextParticule( rrc, fieldRslt, 1 ) )
	{ LDEB(2); return -1;	}

    if  ( docSetFieldInst( df, fieldinst, fieldsize ) )
	{ LDEB(6); return -1;	}

    if  ( docRtfFlushBookmarks( -1, bi, rrc ) )
	{ LDEB(1); return -1;	}

    if  ( docRtfTerminateField( bi, fieldNumber, rrc ) )
	{ LDEB(bi->biParaParticuleCount); return -1;	}

    rrc->rrcAfterNoteref= afterNoteref;
    df->dfKind= rcw->rcwID;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Consume Headers, Footers, Notes &c: Separate item trees that are.	*/
/*  embedded in the document.						*/
/*									*/
/*  1)  Save the current position.					*/
/*  2)  Make the header/note &c.					*/
/*  3)  Consume its contents.						*/
/*  4)  Make sure that no bookmarks protrude beyond the end of the	*/
/*	external item.							*/
/*  5)  If the external item happens to be empty, insert an empty	*/
/*	paragraph to avoid problems later on.				*/
/*  6)  Restore the current position.					*/
/*									*/
/************************************************************************/

int docRtfReadExternalItem(	BufferItem **		pBi,
				int *			pExtItKind,
				SimpleInputStream *	sis,
				RtfReadingContext *	rrc,
				const SelectionScope *	ss )
    {
    RtfReadingState *	rrs= rrc->rrcState;
    BufferItem *	bi;

    BufferItem *	savedBi;
    int			savedLevel;
    int			savedInTable;
    int			savedExternalItemKind;

    int			res;

    /*  1  */
    savedBi= rrc->rrcBi;
    savedLevel= rrc->rrcLevel;
    savedInTable= rrs->rrsParagraphProperties.ppInTable;
    savedExternalItemKind= rrc->rrcExternalItemKind;
    if  ( ! savedBi )
	{ XDEB(savedBi); return -1;	}

    /*  2  */
    bi= *pBi;
    if  ( ! bi )
	{
	bi= docMakeExternalItem( rrc->rrcBd,
				    ss, &(rrs->rrsSectionProperties) );
	if  ( ! bi )
	    { XDEB(bi); return -1;	}

	*pBi= bi;
	}

    /*  3  */
    rrc->rrcBi= bi;
    rrc->rrcLevel= DOClevSECT;
    rrc->rrcExternalItemKind= ss->ssInExternalItem;

    rrs->rrsParagraphProperties.ppInTable= 0;

    res= docRtfReadGroup( sis, DOClevPARA,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfDocumentWords, docRtfDocumentGroups,
				docRtfTextParticule, (RtfCommitGroup)0 );
    if  ( res )
	{ LDEB(res);	}

    /*  4  */
    if  ( docRtfFlushBookmarks( -1, bi, rrc ) )
	{ LDEB(1); res= -1; goto ready;	}

    /*  5  */
    {
    DocumentPosition	dp;

    if  ( docFirstPosition( &dp, bi ) )
	{
	const int	textAttributeNumber= 0;

	if  ( ! docInsertEmptyParagraph( rrc->rrcBd, bi,
						textAttributeNumber ) )
	    { LDEB(textAttributeNumber);	}
	}
    }

    *pExtItKind= rrc->rrcExternalItemKind;

  ready:
    /*  6  */
    rrc->rrcExternalItemKind= savedExternalItemKind;
    rrs->rrsParagraphProperties.ppInTable= savedInTable;
    rrc->rrcLevel= savedLevel;
    rrc->rrcBi= savedBi;

    return res;
    }

int docRtfReadExtIt(		SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    BufferItem *	sectBi= rrc->rrcBi;
    BufferDocument *	bd= rrc->rrcBd;
    ExternalItem *	ei;

    SelectionScope	ss;
    int			extItKind;

    int			savedSplitLevel= rrc->rrcSplitLevel;

    docInitSelectionScope( &ss );

    switch( rcw->rcwID )
	{
	case DOCinSECT_HEADER:
	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:
	case DOCinSECT_FOOTER:
	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:

	    if  ( ! sectBi || sectBi->biLevel != DOClevSECT )
		{ XDEB(sectBi); return -1;	}

	    ei= docSectionHeaderFooter( sectBi, rcw->rcwID );
	    if  ( ! ei )
		{ LXDEB(rcw->rcwID,ei); return -1;	}

	    ss.ssSectNrExternalTo= sectBi->biNumberInParent;

	    break;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    if  ( ! bd )
		{ XDEB(bd); return -1;	}

	    ei= docDocumentNoteSeparator( bd, rcw->rcwID );
	    if  ( ! ei )
		{ LXDEB(rcw->rcwID,ei); return -1;	}

	    break;

	default:
	    SDEB(rcw->rcwWord); return -1;
	}

    if  ( ei->eiItem )
	{ SXDEB(rcw->rcwWord,ei->eiItem);	}

    ss.ssInExternalItem= rcw->rcwID;
    ss.ssNoteArrayIndex= -1;

    if  ( docRtfReadExternalItem( &(ei->eiItem), &extItKind, sis, rrc, &ss ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    rrc->rrcSplitLevel= savedSplitLevel;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Consume a footnote.							*/
/*									*/
/************************************************************************/

int docRtfReadFootnote(		SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;
    BufferDocument *	bd= rrc->rrcBd;
    BufferItem *	bi= rrc->rrcBi;

    DocumentNote *	dn;
    int			noteIndex;

    BufferItem *	sectBi= bi;
    SelectionScope	ss;

    int			externalItemKind;
    int			autoNumber= 0;

    const int		objectNumber= -1;

    docInitSelectionScope( &ss );

    while( sectBi && sectBi->biLevel != DOClevSECT )
	{ sectBi= sectBi->biParent;	}

    if  ( ! sectBi )
	{ XDEB(sectBi); return -1;	}

    if  ( rrc->rrcAfterNoteref )
	{ autoNumber= 1;	}

    noteIndex= docInsertNote( &dn, bd, bi, bi->biParaStrlen, autoNumber );
    if  ( noteIndex < 0 )
	{ LDEB(noteIndex); return -1;	}

    if  ( docInsertAdminParticule( bd, bi, bi->biParaStrlen,
					    bi->biParaParticuleCount,
					    objectNumber,
					    &(rrs->rrsTextAttribute),
					    DOCkindNOTE ) )
	{ LDEB(1); return -1;	}

    rrc->rrcAfterNoteref= 0;

    ss.ssInExternalItem= DOCinFOOTNOTE;
    ss.ssSectNrExternalTo= sectBi->biNumberInParent;
    ss.ssNoteArrayIndex= noteIndex;

    if  ( docRtfReadExternalItem( &(dn->dnExternalItem.eiItem),
					&externalItemKind, sis, rrc, &ss ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    dn->dnAutoNumber= autoNumber;
    dn->dnExternalItemKind= externalItemKind;
    docSetExternalItemKind( dn->dnExternalItem.eiItem, externalItemKind );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read a group that results in a text field.				*/
/*									*/
/************************************************************************/

int docRtfReadTextField(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    int			res;
    BufferItem *	bi= rrc->rrcBi;

    int			startParticule;
    int			fieldNumber= -1;

    DocumentField *	df;

    if  ( docRtfReadStartField( &startParticule, &fieldNumber, rrc ) )
	{ LDEB(bi->biParaParticuleCount); return -1;	}

    df= rrc->rrcBd->bdFieldList.dflFields+ fieldNumber;
    df->dfKind= rcw->rcwID;

    res= docRtfReadGroup( sis, DOClevPARA,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfDocumentWords, docRtfDocumentGroups,
				docRtfTextParticule, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    if  ( docRtfFlushBookmarks( -1, bi, rrc ) )
	{ LDEB(1); return -1;	}

    if  ( docRtfFinishField( bi, fieldNumber, rrc ) )
	{ LDEB(bi->biParaParticuleCount); return -1;	}

    return res;
    }

