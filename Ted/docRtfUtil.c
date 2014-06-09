/************************************************************************/
/*									*/
/*  Read a plain text file into a BufferDocument			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<charnames.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>

#   include	"docRtf.h"

/************************************************************************/
/*									*/
/*  An empty tag table.							*/
/*									*/
/************************************************************************/

RtfControlWord	docRtfEmptyTable[]=
    {
	{ 0, 0, 0, }
    };

/************************************************************************/
/*									*/
/*  Reset the attributes in an RtfReadingContext to the defaults.	*/
/*									*/
/************************************************************************/

void docRtfInitReadingContext(	RtfReadingContext *	rrc	)
    {
    int		i;

    rrc->rrcLevel= DOClevOUT;
    rrc->rrcSplitLevel= DOClevOUT;
    rrc->rrcState= (RtfReadingState *)0;
    rrc->rrcInIgnoredGroup= 0;

				/****************************************/
				/*  The depth in the document		*/
				/*  == Type of BufferItem.		*/
				/****************************************/
    rrc->rrcComplainUnknown= 1;
				/****************************************/
				/*  Complain about unknown control.	*/
				/****************************************/
    rrc->rrcCharacterAhead= EOF;
    rrc->rrcAfterNoteref= 0;

    rrc->rrcCurrentLine= 1;
    rrc->rrcBd= (BufferDocument *)0;
    rrc->rrcBi= (BufferItem *)0;
				/****************************************/
				/*  The current position in the input.	*/
				/****************************************/
    rrc->rrcInDeletedText= 0;

    rrc->rrcTextShadingPattern= DOCspSOLID;
    rrc->rrcTextShadingLevel= 0;
    rrc->rrcTextForeground= -1;
    rrc->rrcTextBackground= -1;
				/****************************************/
				/* Attributes of the current position.	*/
				/****************************************/
    docInitTextFrameProperties( &(rrc->rrcParaFramePosition) );
    docInitTextFrameProperties( &(rrc->rrcRowFramePosition) );
    docInitCellProperties( &(rrc->rrcCellProperties) );
    docInitRowProperties( &(rrc->rrcRowProperties) );

    rrc->rrcExternalItemKind= DOCinBODY;

    docInitDocumentList( &(rrc->rrcDocumentList) );
    docInitListOverride( &(rrc->rrcListOverride) );
    docInitDocumentListLevel( &(rrc->rrcDocumentListLevel) );
    docInitListOverrideLevel( &(rrc->rrcListOverrideLevel) );

    docInitBorderProperties( &(rrc->rrcBorderProperties) );
    rrc->rrcDrawingShape= (DrawingShape *)0;
    rrc->rrcNextObjectVertex= 0;
    docInitTabStop( &(rrc->rrcTabStop) );

    docInitParagraphNumber( &(rrc->rrcParagraphNumber) );
    rrc->rrcWhichPnText= -1;
    rrc->rrcParagraphNumbers= (ParagraphNumber *)0;
    rrc->rrcParagraphNumberCount= 0;

    rrc->rrcColor.rgb8Red= rrc->rrcColor.rgb8Green= rrc->rrcColor.rgb8Blue= 0;
    rrc->rrcGotComponent= 0;

    appInvalidateTime( &(rrc->rrcTm) );
    rrc->rrcInfoText= (unsigned char *)0;
				/****************************************/
				/*  Document properties.		*/
				/****************************************/
    docInitDocumentFont( &(rrc->rrcCurrentFont) );
    docInitDocumentStyle( &(rrc->rrcDocumentStyle) );
				/****************************************/
				/*  For reading the font table.		*/
				/****************************************/
    rrc->rrcInsertedObject= (InsertedObject *)0;
    rrc->rrcFieldNumber= -1;
    rrc->rrcBookmarkName[0]= '\0';
    rrc->rrcBookmarkSize= 0;
    rrc->rrcTopBookmark= (RtfBookmarkLevel *)0;
				/****************************************/
				/*  For reading 'objects' and pictures.	*/
				/*  For reading 'fields'.		*/
				/****************************************/
    rrc->rrcJustAfterPntext= 0;
    rrc->rrcGotDocGeometry= 0;
				/****************************************/
				/*  For coping with the way word saves	*/
				/*  {\pntext ... }			*/
				/****************************************/

    for ( i= 0; i < 256; i++ )
	{ rrc->rrcDefaultInputMapping[i]= i;	}
    rrc->rrcTextInputMapping= rrc->rrcDefaultInputMapping;

    rrc->rrcPostScriptFontList= (const PostScriptFontList *)0;
    }

void docRtfCleanReadingContext(	RtfReadingContext *	rrc )
    {
    while( rrc->rrcState )
	{
	XDEB(rrc->rrcState);
	docRtfPopReadingState( rrc );
	}

    /*docCleanCellProperties( &(rrc->rrcRowProperties) );*/
    docCleanRowProperties( &(rrc->rrcRowProperties) );
    /*docCleanShapeProperties( &(rrc->rrcShapeProperties) );*/

    if  ( rrc->rrcDrawingShape )
	{ docFreeDrawingShape( rrc->rrcBd, rrc->rrcDrawingShape );	}

    docCleanDocumentFont( &(rrc->rrcCurrentFont) );

    docCleanDocumentList( &(rrc->rrcDocumentList) );
    docCleanListOverride( &(rrc->rrcListOverride) );
    docCleanDocumentListLevel( &(rrc->rrcDocumentListLevel) );
    docCleanListOverrideLevel( &(rrc->rrcListOverrideLevel) );

    docCleanDocumentStyle( &(rrc->rrcDocumentStyle) );

    if  ( rrc->rrcParagraphNumbers )
	{ free( rrc->rrcParagraphNumbers );	}

#   if 0
    rbl= rrc->rrcTopBookmark;
    while( rbl )
	{
	RtfBookmarkLevel *	prev= rbl->rblPrevious;

	free( rbl );

	rbl= prev;
	}
#   endif
    }

int docRtfCopyReadingContext(	RtfReadingContext *		to,
				const RtfReadingContext *	from )
    {
    RowProperties		rpCopy;
    DocumentStyle		dsCopy;

    PropertyMask		ppChgMask;
    PropertyMask		ppUpdMask;

    const int * const		fontMap= (const int *)0;
    const int * const		colorMap= (const int *)0;

    PROPmaskCLEAR( &ppChgMask );

    PROPmaskCLEAR( &ppUpdMask );
    utilPropMaskFill( &ppUpdMask, PPprop_COUNT );

    docInitRowProperties( &rpCopy );
    if  ( docCopyRowProperties( &rpCopy,
			    &(from->rrcRowProperties), (const int *)0 ) )
	{ LDEB(1); return -1;	}

    docInitDocumentStyle( &dsCopy );
    if  ( docCopyStyle( &dsCopy, &(from->rrcDocumentStyle) ) )
	{ LDEB(1); return -1;	}

    docCleanDocumentList( &(to->rrcDocumentList) );
    docCleanListOverride( &(to->rrcListOverride) );
    docCleanDocumentListLevel( &(to->rrcDocumentListLevel) );
    docCleanListOverrideLevel( &(to->rrcListOverrideLevel) );

    *to= *from;

    to->rrcParagraphNumbers= (ParagraphNumber *)0;
    to->rrcParagraphNumberCount= 0;

    to->rrcRowProperties= rpCopy;
    to->rrcDocumentStyle= dsCopy;

    to->rrcTopBookmark= from->rrcTopBookmark;

    docInitDocumentList( &(to->rrcDocumentList) );
    docInitListOverride( &(to->rrcListOverride) );
    docInitDocumentListLevel( &(to->rrcDocumentListLevel) );
    docInitListOverrideLevel( &(to->rrcListOverrideLevel) );

    docCopyDocumentList( &(to->rrcDocumentList),
			    &(from->rrcDocumentList), fontMap, colorMap );
    docCopyListOverride( &(to->rrcListOverride),
			    &(from->rrcListOverride), fontMap, colorMap );
    docCopyDocumentListLevel( &(to->rrcDocumentListLevel),
			    &(from->rrcDocumentListLevel), fontMap, colorMap );
    docCopyListOverrideLevel( &(to->rrcListOverrideLevel),
			    &(from->rrcListOverrideLevel), fontMap, colorMap );

    return 0;
    }


void docRtfCopyReadingContextBack(	RtfReadingContext *	to,
					RtfReadingContext *	from )
    {
    const int * const		fontMap= (const int *)0;
    const int * const		colorMap= (const int *)0;

    to->rrcBi= from->rrcBi;
    to->rrcLevel= from->rrcLevel;
    to->rrcSplitLevel= from->rrcSplitLevel;
    to->rrcCurrentLine= from->rrcCurrentLine;
    to->rrcFieldNumber= from->rrcFieldNumber;
    to->rrcJustAfterPntext= from->rrcJustAfterPntext;

    memcpy( to->rrcBookmarkName, from->rrcBookmarkName, DOCmaxBOOKMARK+ 1 );
    to->rrcBookmarkSize= from->rrcBookmarkSize;
    to->rrcTopBookmark= from->rrcTopBookmark;
    from->rrcTopBookmark= (RtfBookmarkLevel *)0;

    to->rrcCurrentFont= from->rrcCurrentFont;
    docInitDocumentFont( &(from->rrcCurrentFont) );

    to->rrcParagraphNumber= from->rrcParagraphNumber;

    to->rrcExternalItemKind= from->rrcExternalItemKind;

    to->rrcTm= from->rrcTm;

    docCopyDocumentList( &(to->rrcDocumentList),
			    &(from->rrcDocumentList), fontMap, colorMap );
    docCopyListOverride( &(to->rrcListOverride),
			    &(from->rrcListOverride), fontMap, colorMap );
    docCopyDocumentListLevel( &(to->rrcDocumentListLevel),
			    &(from->rrcDocumentListLevel), fontMap, colorMap );
    docCopyListOverrideLevel( &(to->rrcListOverrideLevel),
			    &(from->rrcListOverrideLevel), fontMap, colorMap );
    }

void docRtfPushReadingState(	RtfReadingContext *	rrc,
				RtfReadingState *	rrs )
    {
    BufferDocument *		bd= rrc->rrcBd;
    const DocumentProperties *	dp= (const DocumentProperties *)0;

    if  ( bd )
	{ dp= &(bd->bdProperties); }

    utilInitTextAttribute( &(rrs->rrsTextAttribute) );
    rrs->rrsTextAttribute.taFontSizeHalfPoints= 24;

    if  ( dp && dp->dpDefaultFont >= 0 )
	{ rrs->rrsTextAttribute.taFontNumber= dp->dpDefaultFont;	}

    docInitParagraphProperties( &(rrs->rrsParagraphProperties) );
    docInitSectionProperties( &(rrs->rrsSectionProperties) );

    utilInitMemoryBuffer( &(rrs->rrsSavedText) );

    rrs->rrsBytesPerUnicode= 1;
    rrs->rrsUnicodeBytesToSkip= 0;

    if  ( rrc->rrcState )
	{
	const RtfReadingState *	above= rrc->rrcState;

	rrs->rrsBytesPerUnicode= above->rrsBytesPerUnicode;
	rrs->rrsUnicodeBytesToSkip= 0;

	/*
	utilCopyTextAttribute( &(rrs->rrsTextAttribute),
					&(above->rrsTextAttribute) );
	*/
	rrs->rrsTextAttribute= above->rrsTextAttribute;

	docCopyParagraphProperties( &(rrs->rrsParagraphProperties),
				    &(above->rrsParagraphProperties) );
	docCopySectionProperties( &(rrs->rrsSectionProperties),
				    &(above->rrsSectionProperties) );
	}

    rrs->rrsPrev= rrc->rrcState;
    rrc->rrcState= rrs;

    return;
    }

void docRtfPopReadingState(	RtfReadingContext *	rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;

    if  ( ! rrs )
	{ XDEB(rrs); return;	}

    /*
    utilCleanTextAttribute( &(rrs->rrsTextAttribute) );
    */
    docCleanParagraphProperties( &(rrs->rrsParagraphProperties) );
    docCleanSectionProperties( &(rrs->rrsSectionProperties) );

    utilCleanMemoryBuffer( &(rrs->rrsSavedText) );

    /**/
    rrc->rrcState= rrs->rrsPrev;

    return;
    }

/************************************************************************/
/*									*/
/*  Read a control word from the input stream.				*/
/*									*/
/************************************************************************/

static int docRtfReadControlWord(	SimpleInputStream *	sis,
					RtfReadingContext *	rrc,
					int *			pC,
					char *			controlWord,
					int *			pGotArg,
					int *			pArg )
    {
    RtfReadingState *	rrs= rrc->rrcState;

    int			c;
    int			len= 0;
    int			sign= 1;

    c= sioInGetCharacter( sis );
    while( c == '\n' || c == '\r' )
	{
	if  ( c == '\n' )
	    { rrc->rrcCurrentLine++;	}
	c= sioInGetCharacter( sis );
	}

    if  ( c != '\\' )
	{ CDEB(c); return -1;	}

    c= sioInGetCharacter( sis );
    if  ( ! isalpha( c ) )
	{
	switch( c )
	    {
	    case '\n':
		rrc->rrcCurrentLine++;
		/*FALLTHROUGH*/
	    case '\r':
		strcpy( controlWord, "par" );
		*pGotArg= 0;
		return 0;
	    case '\t':
		strcpy( controlWord, "tab" );
		*pGotArg= 0;
		return 0;
	    case '_':
		*pC= '-'; return 1;
	    default:
		*pC= c; return 1;
	    }
	}
    controlWord[len++]= c;

    for (;;)
	{
	c= sioInGetCharacter( sis );
	if  ( ! isalpha( c ) )
	    { controlWord[len]= '\0'; break;	}
	if  ( len >= TEDszRTFCONTROL )
	    { LLDEB(len,TEDszRTFCONTROL); return -1;	}
	controlWord[len++]= c;
	}

    /*HACK*/
    if  ( ! strcmp( controlWord, "endash" )	||
          ! strcmp( controlWord, "emdash" )	)
	{
	*pC= ISO1_hyphen;
	if  ( c != ' ' )
	    { sioInUngetLastRead( sis );	}
	return 2;
	}

    if  ( ! strcmp( controlWord, "rquote" )	)
	{
	*pC= ISO1_quotesingle;
	if  ( c != ' ' )
	    { sioInUngetLastRead( sis );	}
	return 2;
	}

    if  ( ! strcmp( controlWord, "lquote" )	)
	{
	*pC= ISO1_quoteleft;
	if  ( c != ' ' )
	    { sioInUngetLastRead( sis );	}
	return 2;
	}

    if  ( ! strcmp( controlWord, "ldblquote" )	)
	{
	*pC= ISO1_quotedbl;
	if  ( c != ' ' )
	    { sioInUngetLastRead( sis );	}
	return 2;
	}

    if  ( ! strcmp( controlWord, "rdblquote" )	)
	{
	*pC= ISO1_quotedbl;
	return 2;
	}

    if  ( ! strcmp( controlWord, "bullet" )	)
	{
	*pC= ISO1_periodcentered;
	if  ( c != ' ' )
	    { sioInUngetLastRead( sis );	}
	return 2;
	}

    if  ( c == '-' )
	{ sign= -1; c= sioInGetCharacter( sis );	}

    *pGotArg= ( isdigit( c ) != 0 );
    if  ( *pGotArg )
	{
	int arg= c- '0';

	c= sioInGetCharacter( sis );

	while( isdigit( c ) )
	    { arg= 10* arg+ c- '0'; c= sioInGetCharacter( sis ); }

	*pArg= sign* arg;
	}

    /* HACK */
    if  ( *pGotArg && ! strcmp( controlWord, "u" ) )
	{
	if  ( *pArg < 0 )
	    { *pArg= (int)0x1000+ *pArg;	}

	if  ( *pArg >= 0 && *pArg <= 255 )
	    {
	    *pC= *pArg;

	    rrs->rrsUnicodeBytesToSkip= rrs->rrsBytesPerUnicode;
	    }
	else{ *pC= ISO1_currency;	}

	if  ( c != ' ' )
	    { sioInUngetLastRead( sis );	}

	return 2;
	}

    if  ( c != ' ' )
	{ sioInUngetLastRead( sis );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find a Control word.						*/
/*									*/
/************************************************************************/

const RtfControlWord * docRtfFindWord(	const char *		controlWord,
					const RtfControlWord *	contolWords,
					int			listOnly )
    {
    if  ( contolWords )
	{
	while( contolWords->rcwWord )
	    {
	    if  ( ! strcmp( contolWords->rcwWord, controlWord ) )
		{ return contolWords;	}

	    contolWords++;
	    }
	}

    if  ( ! listOnly )
	{
	contolWords= docRtfFindPropertyWord( controlWord );

	return contolWords;
	}

    return (RtfControlWord *)0;
    }

int docRtfApplyControlWord(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			gotArg,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    if  ( ! gotArg )
	{ arg= -1;	}

    if  ( (*rcw->rcwApply) ( sis, rcw, arg, rrc ) )
	{ LSLDEB(rrc->rrcCurrentLine,rcw->rcwWord,arg); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find out what to do.						*/
/*									*/
/*  Return	RTFfiCTRLGROUP:	Found a control group.			*/
/*  Return	RTFfiWORD:	Found a control word.			*/
/*  Return	RTFfiCHAR:	Found a character.			*/
/*  Return	RTFfiCLOSE:	Found an unescaped brace.		*/
/*  Return	-1:		Error.					*/
/*									*/
/************************************************************************/

int docRtfFindControl(		SimpleInputStream *	sis,
				RtfReadingContext *	rrc,
				int *			pC,
				char *			controlWord,
				int *			pGotArg,
				int *			pArg		)
    {
    RtfReadingState *	rrs= rrc->rrcState;

    int			c;
    int			res;

    if  ( rrc->rrcCharacterAhead != EOF )
	{
	*pC= rrc->rrcCharacterAhead;
	rrc->rrcCharacterAhead= EOF;
	return RTFfiCHAR;
	}

    for (;;)
	{
	c= sioInGetCharacter( sis );

	switch( c )
	    {
	    case EOF:
		XDEB(c); *pC= '}'; return RTFfiCLOSE;
	    case '\\':
		sioInUngetLastRead( sis );
		res= docRtfReadControlWord( sis, rrc, &c,
						controlWord, pGotArg, pArg );

		if  ( res < 0 )
		    { LDEB(res); return -1;	}
		if  ( res > 0 )
		    {
		    switch( c )
			{
			case '\\': case '{': case '}':
			    *pC= c; return RTFfiCHAR;

			case '~':
			    c= ISO1_nobreakspace;
			    *pC= c; return RTFfiCHAR;

			case '_':
			    c= '-';
			    *pC= c; return RTFfiCHAR;

			case '\'':
			    if  ( res == 1 )
				{
				char		b[3];
				unsigned int	uc;

				b[0]= sioInGetCharacter( sis );
				b[1]= sioInGetCharacter( sis );
				b[2]= '\0';

				sscanf( b, "%x", &uc ); c= uc;
				}
			    goto defaultCase;

			default:
			    if  ( res == 2 )
				{ *pC= c; return RTFfiCHAR;	}
			    else{ goto defaultCase;		}
			}
		    }
		return RTFfiWORD;

	    case '{':
		c= sioInGetCharacter( sis );
		while( c == '\n' || c == '\r' )
		    {
		    if  ( c == '\n' )
			{ rrc->rrcCurrentLine++;	}
		    c= sioInGetCharacter( sis );
		    }

		if  ( c == '\\' )
		    {
		    sioInUngetLastRead( sis );
		    res= docRtfReadControlWord( sis, rrc, &c,
						controlWord, pGotArg, pArg );
		    if  ( res < 0 )
			{ LDEB(res); return -1;	}
		    if  ( res > 0 )
			{
			if  ( c == '*' )
			    {
			    res= docRtfReadControlWord( sis, rrc, &c,
						controlWord, pGotArg, pArg );
			    if  ( res )
				{ LDEB(res); return -1;	}

			    return RTFfiSTARGROUP;
			    }

			if  ( c=='\'' )
			    {
			    char		b[3];
			    unsigned int	uc;

			    b[0]= sioInGetCharacter( sis );
			    b[1]= sioInGetCharacter( sis );
			    b[2]= '\0';

			    sscanf( b, "%x", &uc ); c= uc;
			    }

			if  ( rrs->rrsUnicodeBytesToSkip > 0 )
			    { rrs->rrsUnicodeBytesToSkip= 0;	}

			rrc->rrcCharacterAhead= c;

			return RTFfiTEXTGROUP;
			}

		    return RTFfiCTRLGROUP;
		    }

		sioInUngetLastRead( sis );
		return RTFfiTEXTGROUP;

	    case '\n':
		rrc->rrcCurrentLine++;
		/*FALLTHROUGH*/
	    case '\r':
		continue;

	    case '}':
		*pC= c; return RTFfiCLOSE;

	    case '\t':
		*pC= c; return RTFfiTAB;

	    default: defaultCase:

		if  ( rrs && rrs->rrsUnicodeBytesToSkip > 0 )
		    { rrs->rrsUnicodeBytesToSkip--; continue;	}

		*pC= c; return RTFfiCHAR;
	    }
	}
    }

/************************************************************************/
/*									*/
/*  Adjust the level in the hierarchy for docRtfReadGroup().		*/
/*									*/
/*  1)  Empty paragraphs get one empty particule to have a height.	*/
/*  3)  Jade puts its tabs inside the text and Word can handle this,	*/
/*	so use this HACK to also support this.				*/
/*	The same applies for the way MS-Word inserts bookmarks before	*/
/*	list texts.							*/
/*  5)  Jade again.							*/
/*									*/
/************************************************************************/

static int docRtfFinishParagraph(	RtfReadingContext *	rrc,
					BufferItem *		paraBi )
    {
    int			addEmptyParticule= 0;

    if  ( paraBi->biParaParticuleCount == 0 )
	{ addEmptyParticule= 1;	}
    else{
	const TextParticule *	tp;

	tp= paraBi->biParaParticules+ paraBi->biParaParticuleCount- 1;

	if  ( tp->tpKind == DOCkindLINEBREAK	||
	      tp->tpKind == DOCkindPAGEBREAK	||
	      tp->tpKind == DOCkindCOLUMNBREAK	)
	    { addEmptyParticule= 1;	}
	}

    if  ( addEmptyParticule )
	{
	RtfReadingState *	rrs= rrc->rrcState;

	const int		part= paraBi->biParaParticuleCount;
	const int		stroff= paraBi->biParaStrlen;
	const int		objectNumber= -1;

	if  ( docInflateTextString( paraBi, 0 ) )
	    { LDEB(paraBi->biParaStrlen); return -1;	}
	paraBi->biParaString[paraBi->biParaStrlen]= '\0';

	if  ( docInsertAdminParticule( rrc->rrcBd, paraBi,
			    stroff, part, objectNumber,
			    &(rrs->rrsTextAttribute), DOCkindTEXT ) )
	    { LDEB(1); return -1;	}

	rrc->rrcAfterNoteref= 0;
	}

    return 0;
    }

int docRtfAdjustLevel(	RtfReadingContext *	rrc,
			int			toLevel,
			int			textLevel )
    {
    RtfReadingState *		rrs= rrc->rrcState;
    BufferItem *		bi= rrc->rrcBi;
    BufferDocument *		bd= rrc->rrcBd;
    const DocumentProperties *	dp= &(bd->bdProperties);

    const int * const		colorMap= (const int *)0;
    const int * const		listStyleMap= (const int *)0;

    if  ( ! bi )
	{ XDEB(bi); return -1;	}

    /*  1  */
    if  ( toLevel < rrc->rrcLevel		&&
	  bi->biLevel == DOClevPARA		&&
	  docRtfFinishParagraph( rrc, bi )	)
	{ LDEB(1); return -1;	}

    while( toLevel < rrc->rrcLevel )
	{
	if  ( ! bi )
	    { XDEB(bi); return -1;		}
	if  ( ! bi->biParent )
	    { XDEB(bi->biParent); return -1;	}

	if  ( bi->biLevel == DOClevPARA )
	    {
	    PropertyMask		ppChgMask;
	    PropertyMask		ppUpdMask;
	    int				paraNr;
	    int				textAttributeNumber;

	    ListNumberTreeNode *	root;

	    textAttributeNumber= bi->biParaParticules->tpTextAttributeNumber;

	    paraNr= docNumberOfParagraph( bi );

	    if  ( docRtfFlushBookmarks( -1, bi, rrc ) )
		{ LDEB(1); return -1;	}

	    PROPmaskCLEAR( &ppChgMask );

	    PROPmaskCLEAR( &ppUpdMask );
	    utilPropMaskFill( &ppUpdMask, PPprop_COUNT );
	    PROPmaskUNSET( &ppUpdMask, PPpropIN_TABLE );

	    /*  3  */
	    if  ( docUpdParaProperties( &ppChgMask, &(bi->biParaProperties),
				&ppUpdMask, &(rrs->rrsParagraphProperties),
				colorMap, listStyleMap ) )
		{ LDEB(1); return -1;	}

	    if  ( bi->biParaListOverride > 0 )
		{
		int				fieldNr= -1;
		int				partBegin= -1;
		int				partEnd= -1;
		int				stroffBegin= -1;
		int				stroffEnd= -1;

		const ListOverrideTable *	lot;

		lot= &(dp->dpListOverrideTable);

		if  ( bi->biParaListOverride >= lot->lotOverrideCount )
		    {
		    LLDEB(bi->biParaListOverride,lot->lotOverrideCount);
		    return -1;
		    }

		if  ( docDelimitParaHeadField( &fieldNr, &partBegin, &partEnd,
					&stroffBegin, &stroffEnd, bi, bd ) )
		    {
		    if  ( docInsertParaHeadField( &fieldNr,
				&partBegin, &partEnd, &stroffBegin, &stroffEnd,
				bi, bd, DOCfkLISTTEXT, textAttributeNumber ) )
			{
			LDEB(bi->biParaListOverride); return -1;
			}
		    }

		root= &(bd->bdListNumberTrees[bi->biParaListOverride]);

		if  ( docListNumberTreeInsertParagraph( root,
					    bi->biParaListLevel, paraNr ) )
		    { LDEB(paraNr);	}
		}
	    }

	if  ( bi->biLevel == DOClevSECT )
	    {
	    int		changed= 0;

	    docRenumberNotes( &changed, rrc->rrcBd );
	    }

	bi= bi->biParent; rrc->rrcLevel--;
	}

    rrc->rrcSplitLevel= rrc->rrcLevel;

    while( rrc->rrcLevel < toLevel && rrc->rrcLevel < textLevel )
	{
	BufferItem *	fresh;

	if  ( rrc->rrcLevel == DOClevDOC )
	    { ; }

	fresh= docInsertItem( rrc->rrcBd, bi, -1, rrc->rrcLevel+ 1 );
	if  ( ! fresh )
	    {
	    SLDEB(docLevelStr(rrc->rrcLevel),fresh);
	    SSDEB(docLevelStr(toLevel),docLevelStr(textLevel));
	    return -1;
	    }

	rrc->rrcAfterNoteref= 0;

	bi= fresh;
	
	rrc->rrcLevel++;

	if  ( rrc->rrcLevel == DOClevPARA )
	    {
	    PropertyMask	ppChgMask;
	    PropertyMask	ppUpdMask;

	    BufferItem *	cellBi= bi->biParent;
	    BufferItem *	rowBi= cellBi->biParent;

	    PROPmaskCLEAR( &ppChgMask );

	    PROPmaskCLEAR( &ppUpdMask );
	    utilPropMaskFill( &ppUpdMask, PPprop_COUNT );

	    if  ( docUpdParaProperties( &ppChgMask, &(bi->biParaProperties),
				&ppUpdMask, &(rrs->rrsParagraphProperties),
				colorMap, listStyleMap ) )
		{ LDEB(1); return -1;	}

	    /*  5  */
	    if  ( bi->biParaLeftIndentTwips < 0	 )
		{ bi->biParaLeftIndentTwips= 0;	}
	    if  ( bi->biParaFirstIndentTwips+ bi->biParaLeftIndentTwips < 0 )
		{ bi->biParaFirstIndentTwips= -bi->biParaLeftIndentTwips; }

	    if  ( rowBi && bi->biParaInTable )
		{
		if  ( cellBi->biChildCount == 1	&&
		      rowBi->biChildCount == 1	)
		    {
		    if  ( ! rowBi->biRowHasTableParagraphs )
			{
			if  ( docCopyRowProperties( &(rowBi->biRowProperties),
				 &(rrc->rrcRowProperties), (const int *)0 ) )
			    { LDEB(1); return -1;	}

			rowBi->biRowHasTableParagraphs= 1;
			}
		    }
		else{
		    if  ( ! rowBi->biRowHasTableParagraphs )
			{
			BufferItem *	insBi;
			BufferItem *	aftBi;

			if  ( cellBi->biChildCount > 1 )
			    {
			    if  ( docSplitGroupItem(
					rrc->rrcBd, &insBi, &aftBi, cellBi,
					bi->biNumberInParent, DOClevCELL ) )
				{ LDEB(1); return -1;	}
			    }

			if  ( rowBi->biChildCount > 1 )
			    {
			    if  ( docSplitGroupItem(
					rrc->rrcBd, &insBi, &aftBi, rowBi,
					cellBi->biNumberInParent, DOClevROW ) )
				{ LDEB(1); return -1;	}
			    }

			if  ( docCopyRowProperties( &(rowBi->biRowProperties),
				 &(rrc->rrcRowProperties), (const int *)0 ) )
			    { LDEB(1); return -1;	}

			rowBi->biRowHasTableParagraphs= 1;
			}
		    }
		}
	    else{
		if  ( rowBi && rowBi->biRowHasTableParagraphs )
		    { LDEB(rowBi->biRowHasTableParagraphs); }
		}
	    }

	if  ( rrc->rrcLevel == DOClevROW && bi->biRowHasTableParagraphs )
	    {
	    if  ( docCopyRowProperties( &(bi->biRowProperties),
				 &(rrc->rrcRowProperties), (const int *)0 ) )
		{ LDEB(1); return -1;	}
	    }

	if  ( rrc->rrcLevel == DOClevSECT )
	    {
	    if  ( docCopySectionProperties( &(bi->biSectProperties),
					     &(rrs->rrsSectionProperties) ) )
		{ LDEB(1); return -1;	}

	    if  ( rrc->rrcParagraphNumberCount > 0 )
		{
		if  ( bi->biSectParagraphNumbers )
		    {
		    XDEB(bi->biSectParagraphNumbers);
		    free( bi->biSectParagraphNumbers );
		    }

		bi->biSectParagraphNumbers= rrc->rrcParagraphNumbers;
		bi->biSectParagraphNumberCount= rrc->rrcParagraphNumberCount;

		rrc->rrcParagraphNumbers= (ParagraphNumber *)0;
		rrc->rrcParagraphNumberCount= 0;
		}
	    }

	/*
	if  ( rrc->rrcLevel == DOClevCELL )
	    { }
	*/
	}

    rrc->rrcSplitLevel= rrc->rrcLevel;
    rrc->rrcBi= bi;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Consume free text upto a control word or a group.			*/
/*									*/
/************************************************************************/

static int docRtfReadText(	SimpleInputStream *	sis,
				int			c,
				int *			pC,

				char *			controlWord,
				int *			pGotArg,
				int *			pArg,

				RtfReadingContext *	rrc,
				RtfAddTextParticule	addParticule )
    {
    int				res;

    static unsigned char *	collectedText;
    static int			collectedSize;

    unsigned char *		fresh;
    int				size;
    int				len;

    len= 0;
    if  ( len >= collectedSize )
	{
	size= 100;

	fresh= (unsigned char *)realloc( collectedText, size+ 2 );
	if  ( ! fresh )
	    { LXDEB(size,fresh); return -1;	}

	collectedText= fresh;
	collectedSize= size;
	}

    collectedText[len++]= c;
    collectedText[len  ]= '\0';

    for (;;)
	{
	int		map= 1;

	c= sioInGetCharacter( sis );

	switch( c )
	    {
	    case EOF:
		LDEB(c); return -1;

	    case '}':
		if  ( len > 0 )
		    {
		    if  ( (*addParticule)( rrc, collectedText, len ) )
			{ LDEB(len); return -1;	}
		    }
		return RTFfiCLOSE;

	    case '{':
		sioInUngetLastRead( sis );
		res= docRtfFindControl( sis, rrc, &c,
						controlWord, pGotArg, pArg );
		if  ( res < 0 )
		    { LDEB(res); return res;	}
		if  ( (*addParticule)( rrc, collectedText, len ) )
		    { LDEB(len); return -1;	}
		*pC= c; return res;

	    case '\n':
		rrc->rrcCurrentLine++;
		/*FALLTHROUGH*/

	    case '\r' :
		continue;

	    case '\\':
		sioInUngetLastRead( sis );
		res= docRtfFindControl( sis, rrc, &c,
						controlWord, pGotArg, pArg );
		if  ( res < 0 )
		    { LDEB(res); return res;	}

		if  ( res != RTFfiCHAR )
		    {
		    if  ( (*addParticule)( rrc, collectedText, len ) )
			{ LDEB(len); return -1;	}

		    return res;
		    }

		map= 0;
		/*FALLTHROUGH*/

	    default:
		if  ( len >= collectedSize )
		    {
		    size= ( 3* collectedSize+ 2 )/ 2;

		    if  ( size < len )
			{ size= len+ 2;	}

		    fresh= (unsigned char *)realloc( collectedText, size+ 2 );
		    if  ( ! fresh )
			{ LXDEB(size,fresh); return -1;	}

		    collectedText= fresh;
		    collectedSize= size;
		    }

		collectedText[len++]= c;
		collectedText[len  ]= '\0';
		break;

	    case '\t':
		if  ( (*addParticule)( rrc, collectedText, len ) )
		    { LDEB(len); return -1;	}
		strcpy( controlWord, "tab" ); *pGotArg= 0; *pArg= -1;
		res= RTFfiWORD;
		return res;
	    }
	}
    }

/************************************************************************/
/*									*/
/*  Skip an unknown or superfluous group.				*/
/*									*/
/************************************************************************/

int docRtfSkipGroup(	SimpleInputStream *	sis,
			const RtfControlWord *	rcw,
			int			textLevel,
			RtfReadingContext *	rrc )
    {
    int		complainUnknown= rrc->rrcComplainUnknown;

    rrc->rrcComplainUnknown= 0;

    if  ( docRtfReadGroup( sis, rcw->rcwLevel,
				    (RtfControlWord *)0, 0, 0, rrc,
				    docRtfEmptyTable, docRtfEmptyTable,
				    docRtfIgnoreText, (RtfCommitGroup)0 ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    rrc->rrcComplainUnknown= complainUnknown;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read an RTF control group. typically the document.			*/
/*									*/
/************************************************************************/

int docRtfReadGroup(	SimpleInputStream *	sis,
			int			textLevel,
			const RtfControlWord *	applyFirst,
			int			gotArg,
			int			arg,
			RtfReadingContext *	rrc,
			const RtfControlWord *	contolWords,
			const RtfControlWord *	groupWords,
			RtfAddTextParticule	addParticule,
			RtfCommitGroup		commitGroup )
    {
    int				rval;
    RtfReadingState		internRrs;

    docRtfPushReadingState( rrc, &internRrs );

    rrc->rrcCharacterAhead= EOF;

    if  ( applyFirst )
	{
	rval= docRtfApplyControlWord( sis, applyFirst, gotArg, arg, rrc );
	if  ( rval )
	    { SDEB(applyFirst->rcwWord);	}
	}

    rval= docRtfConsumeGroup( sis, textLevel, rrc,
				    contolWords, groupWords, addParticule );

    if  ( rval )
	{ LDEB(rval);	}

    if  ( commitGroup && (*commitGroup)( rrc ) )
	{ LDEB(1); rval= -1;	}

    docRtfPopReadingState( rrc );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Read an unknown group.						*/
/*									*/
/************************************************************************/

int docRtfReadUnknownGroup(	SimpleInputStream *	sis,
				RtfReadingContext *	rrc )
    {
    int		complainUnknown= rrc->rrcComplainUnknown;

    rrc->rrcComplainUnknown= 0;

    if  ( docRtfReadGroup( sis, rrc->rrcLevel,
					(RtfControlWord *)0, 0, 0, rrc,
					docRtfEmptyTable, docRtfEmptyTable,
					docRtfIgnoreText, (RtfCommitGroup)0 ) )
	{ LDEB(1); return -1;	}

    rrc->rrcComplainUnknown= complainUnknown;

    return 0;
    }

int docRtfReadWordGroup(	SimpleInputStream *	sis,
				int			textLevel,
				RtfReadingContext *	rrc,
				int			gotArg,
				int			arg,
				const char *		controlWord,
				const RtfControlWord *	contolWords,
				const RtfControlWord *	groupWords,
				RtfAddTextParticule	addParticule )
    {
    const RtfControlWord *	rcw;
    const int			listOnly= 0;

    rcw= docRtfFindWord( controlWord, contolWords, listOnly );
    if  ( rcw )
	{
	if  ( rcw->rcwLevel != DOClevANY		&&
	      docRtfAdjustLevel( rrc,
			    rcw->rcwLevel, textLevel )	)
	    { SDEB(controlWord); return -1; }

	if  ( docRtfReadGroup( sis, textLevel,
				rcw, gotArg, arg, rrc,
				contolWords, groupWords,
				addParticule, (RtfCommitGroup)0 ) )
	    { SDEB(rcw->rcwWord); return -1;	}
	}
    else{
	if  ( rrc->rrcComplainUnknown )
	    { LSDEB(rrc->rrcCurrentLine,controlWord);	}

	if  ( docRtfReadUnknownGroup( sis, rrc ) )
	    { LSDEB(rrc->rrcCurrentLine,controlWord); return -1;	}
	}

    return 0;
    }

int docRtfConsumeGroup(	SimpleInputStream *	sis,
			int			textLevel,
			RtfReadingContext *	rrc,
			const RtfControlWord *	contolWords,
			const RtfControlWord *	groupWords,
			RtfAddTextParticule	addParticule )
    {
    int				res;
    const RtfControlWord *	rcw;

    char			controlWord[TEDszRTFCONTROL+1];
    int				gotArg;
    int				arg= -1;
    int				c;

    int				resAhead= -2;
    int				gotArgAhead;
    int				argAhead;

    res= docRtfFindControl( sis, rrc, &c, controlWord, &gotArg, &arg );
    if  ( res < 0 )
	{ LDEB(res); return -1;	}

    for (;;)
	{
	const RtfControlWord *	rcwApplyFirst;
	int			argApplyFirst;
	int			gotArgApplyFirst;

	switch( res )
	    {
	    case RTFfiCLOSE:
		return 0;

	    case RTFfiCHAR:
		if  ( textLevel != DOClevANY				&&
		      docRtfAdjustLevel( rrc, textLevel, textLevel )	)
		    { LLDEB(textLevel,rrc->rrcLevel); return -1; }

		res= docRtfReadText( sis, c, &c, controlWord,
				    &gotArg, &arg, rrc, addParticule );
		if  ( res < 0 )
		    { SLDEB(controlWord,res); return -1;	}
		break;

	    case RTFfiTAB:
		strcpy( controlWord, "tab" );
		/*FALLTHROUGH*/

	    case RTFfiWORD:
		rcw= docRtfFindWord( controlWord, contolWords, 0 );
		if  ( ! rcw )
		    {
		    if  ( rrc->rrcComplainUnknown )
			{ LSDEB(rrc->rrcCurrentLine,controlWord);	}
		    }
		else{
		    if  ( rcw->rcwLevel != DOClevANY			&&
			  docRtfAdjustLevel( rrc,
					    rcw->rcwLevel, textLevel )	)
			{ SDEB(controlWord); return -1; }

		    if  ( rcw->rcwPrepare )
			{
			if  ( (*rcw->rcwPrepare) ( sis, rcw, arg, rrc ) )
			    { SLDEB(rcw->rcwWord,arg); return -1;	}
			}

		    if  ( rcw->rcwDetailWords )
			{
			for (;;)
			    {
			    const RtfControlWord *	rcwAhead;

			    resAhead= docRtfFindControl( sis, rrc, &c,
					controlWord, &gotArgAhead, &argAhead );

			    if  ( resAhead != RTFfiWORD )
				{ break;	}

			    rcwAhead= docRtfFindWord( controlWord,
						    rcw->rcwDetailWords, 1 );

			    if  ( ! rcwAhead )
				{ break;	}

			    resAhead= docRtfApplyControlWord( sis,
				    rcwAhead, gotArgAhead, argAhead, rrc  );

			    if  ( resAhead < 0 )
				{ LSDEB(resAhead,controlWord); return -1; }
			    }
			}

		    res= docRtfApplyControlWord( sis, rcw,
							gotArg, arg, rrc );

		    if  ( res < 0 )
			{ LSDEB(res,controlWord); return -1;	}
		    }

		if  ( resAhead == -2 )
		    {
		    res= docRtfFindControl( sis, rrc, &c,
						controlWord, &gotArg, &arg );
		    }
		else{
		    res= resAhead; arg= argAhead; gotArg= gotArgAhead;
		    resAhead= -2;
		    }

		if  ( res < 0 )
		    { LDEB(res); return -1;	}
		break;
	    case RTFfiCTRLGROUP:
		rcw= docRtfFindWord( controlWord, groupWords, 1 );
		if  ( ! rcw )
		    {
		    if  ( docRtfReadWordGroup( sis, textLevel, rrc,
				    gotArg, arg, controlWord,
				    contolWords, groupWords, addParticule ) )
			{ SDEB(controlWord); return -1;	}
		    }
		else{
		  groupFound:
		    if  ( rcw->rcwLevel != DOClevANY			&&
			  docRtfAdjustLevel( rrc,
					    rcw->rcwLevel, textLevel )	)
			{ SDEB(controlWord); return -1; }

		    res= docRtfApplyControlWord( sis, rcw, gotArg, arg, rrc );

		    if  ( res < 0 )
			{ LDEB(res); SDEB(controlWord); return -1;	}
		    }

		res= docRtfFindControl( sis, rrc, &c,
						controlWord, &gotArg, &arg );
		if  ( res < 0 )
		    { LDEB(res); return -1;	}
		break;
	    case RTFfiSTARGROUP:
		rcw= docRtfFindWord( controlWord, groupWords, 1 );
		if  ( rcw )
		    { goto groupFound; }

		rcw= docRtfFindWord( controlWord, groupWords, 0 );
		if  ( rcw )
		    {
		    if  ( ! gotArg )
			{ arg= -1;	}

		    rcwApplyFirst= rcw;
		    argApplyFirst= arg;
		    gotArgApplyFirst= gotArg;

		    goto textGroup;
		    }

		rrc->rrcInIgnoredGroup++;

		if  ( docRtfReadUnknownGroup( sis, rrc ) )
		    { LDEB(1); rrc->rrcInIgnoredGroup--; return -1;	}

		rrc->rrcInIgnoredGroup--;

		res= docRtfFindControl( sis, rrc, &c,
					    controlWord, &gotArg, &arg );
		if  ( res < 0 )
		    { LDEB(res); return -1;	}
		break;

	    case RTFfiTEXTGROUP:
		rcwApplyFirst= (RtfControlWord *)0;
		argApplyFirst= -1;
		gotArgApplyFirst= 0;

	      textGroup:
		if  ( docRtfReadGroup( sis, textLevel,
			    rcwApplyFirst, gotArgApplyFirst, argApplyFirst,
			    rrc,
			    contolWords, groupWords,
			    addParticule, (RtfCommitGroup)0 ) )
		    { LDEB(1); return -1;	}

		res= docRtfFindControl( sis, rrc, &c,
					    controlWord, &gotArg, &arg );
		if  ( res < 0 )
		    { LDEB(res); return -1;	}
		break;

	    default:
		LDEB(res); return -1;
	    }
	}
    }

/************************************************************************/
/*									*/
/*  Just ignore a control word.						*/
/*									*/
/************************************************************************/

int docRtfIgnoreWord(	SimpleInputStream *	sis,
			const RtfControlWord *	rcw,
			int			arg,
			RtfReadingContext *	rrc )
    { return 0;	}

/************************************************************************/
/*									*/
/*  Handle text..							*/
/*									*/
/*  1)  Ignore it.							*/
/*  2)  Refuse it.							*/
/*  3)  Save it for later use.						*/
/*									*/
/************************************************************************/

/*  1  */
int docRtfIgnoreText(	RtfReadingContext *	rrc,
			const unsigned char *	text,
			int			len )
    { return 0; }

/*  2  */
int docRtfRefuseText(	RtfReadingContext *	rrc,
			const unsigned char *	text,
			int			len )
    { LDEB(1); return -1; }

/*  3  */
int docRtfSaveText(	RtfReadingContext *	rrc,
			const unsigned char *	text,
			int			len )
    {
    RtfReadingState *	rrs= rrc->rrcState;

    if  ( utilAddToMemoryBuffer( &(rrs->rrsSavedText), text, len ) )
	{ LDEB(len); return -1;	}

    return 0;
    }

int docRtfRememberText(	char **			pTarget,
			int *			pSize,
			RtfReadingContext *	rrc,
			int			removeSemicolon )
    {
    return docRtfRememberUText( (unsigned char **)pTarget, pSize,
						rrc, removeSemicolon );
    }

int docRtfRememberUText(	unsigned char **	pTarget,
				int *			pSize,
				RtfReadingContext *	rrc,
				int			removeSemicolon )
    {
    RtfReadingState *	rrs= rrc->rrcState;

    unsigned char *	fresh;
    int			size;

    if  ( rrs->rrsSavedText.mbSize == 0 )
	{
	*pSize= 0;
	return 0;
	}

    size= rrs->rrsSavedText.mbSize;
    fresh= realloc( *pTarget, size+ 1 );
    if  ( ! fresh )
	{ LXDEB(size,fresh); return -1;	}

    memcpy( fresh, rrs->rrsSavedText.mbBytes, size );
    fresh[size]= '\0';

    if  ( removeSemicolon		&&
	  size > 0			&&
	  fresh[size- 1] == ';'		)
	{
	size--;
	fresh[size]= '\0';
	}

    utilEmptyMemoryBuffer( &(rrs->rrsSavedText) );

    *pTarget= fresh;
    *pSize= size;
    return 0;
    }
