/************************************************************************/
/*									*/
/*  Read the list table of a document.					*/
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
/*  Remember the 'listlevel' related strings.				*/
/*									*/
/************************************************************************/

static int docRtfCommitListLevelText(	RtfReadingContext *	rrc )
    {
    const int	removeSemicolon= 1;
    int		size= 0;

    if  ( docRtfRememberUText( &(rrc->rrcDocumentListLevel.dllText),
					    &size, rrc, removeSemicolon ) )
	{ LDEB(1); return -1; }

    rrc->rrcDocumentListLevel.dllTextSize= size;

    return 0;
    }

static int docRtfCommitListLevelNumbers(	RtfReadingContext *	rrc )
    {
    const int	removeSemicolon= 1;
    int		size= 0;

    if  ( docRtfRememberUText( &(rrc->rrcDocumentListLevel.dllNumbers),
					    &size, rrc, removeSemicolon ) )
	{ LDEB(1); return -1; }

    rrc->rrcDocumentListLevel.dllNumberSize= size;

    return 0;
    }

static int docRtfListLevelGroup(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    switch( rcw->rcwID )
	{
	case LLpropTEXT:

	    if  ( docRtfReadGroup( sis, rcw->rcwLevel,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfEmptyTable, docRtfEmptyTable,
				docRtfSaveText, docRtfCommitListLevelText ) )
		{ SLDEB(rcw->rcwWord,arg); return -1;	}

	    break;

	case LLpropNUMBERS:

	    if  ( docRtfReadGroup( sis, rcw->rcwLevel,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfEmptyTable, docRtfEmptyTable,
				docRtfSaveText, docRtfCommitListLevelNumbers ) )
		{ SLDEB(rcw->rcwWord,arg); return -1;	}

	    break;

	default:
	    SLDEB(rcw->rcwWord,arg); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remember the name of a list.					*/
/*									*/
/************************************************************************/

static int docRtfCommitListName(	RtfReadingContext *	rrc )
    {
    const int	removeSemicolon= 1;
    int		size= 0;

    if  ( docRtfRememberText( &(rrc->rrcDocumentList.dlListName), &size,
						    rrc, removeSemicolon ) )
	{ LDEB(1); return -1; }

    return 0;
    }

static int docRtfCommitListStyleName(	RtfReadingContext *	rrc )
    {
    const int	removeSemicolon= 1;
    int		size= 0;

    if  ( docRtfRememberText( &(rrc->rrcDocumentList.dlListStyleName), &size,
						    rrc, removeSemicolon ) )
	{ LDEB(1); return -1; }

    return 0;
    }

static int docRtfListStringGroup(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    switch( rcw->rcwID )
	{
	case DLpropNAME:

	    if  ( docRtfReadGroup( sis, rcw->rcwLevel,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfEmptyTable, docRtfEmptyTable,
				docRtfSaveText, docRtfCommitListName ) )
		{ SLDEB(rcw->rcwWord,arg); return -1;	}

	    break;

	case DLpropSTYLENAME:

	    if  ( docRtfReadGroup( sis, rcw->rcwLevel,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfEmptyTable, docRtfEmptyTable,
				docRtfSaveText, docRtfCommitListStyleName ) )
		{ SLDEB(rcw->rcwWord,arg); return -1;	}

	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfRememberListLevelProperty(
				SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    DocumentListLevel *		dll= &(rrc->rrcDocumentListLevel);

    switch( rcw->rcwID )
	{
	case LLpropSTARTAT:
	    dll->dllStartAt= arg;
	    break;

	case LLpropSTYLE:
	    dll->dllNumberStyle= arg;
	    break;

	case LLpropJUSTIFY:
	    dll->dllJustification= arg;
	    break;

	case LLpropFOLLOW:
	    dll->dllFollow= arg;
	    break;

	case LLpropLEGAL:
	    dll->dllPrevToDigits= arg != 0;
	    break;

	case LLpropNORESTART:
	    dll->dllNoRestart= arg != 0;
	    break;

	case LLpropPICTURE:
	    dll->dllPictureNumber= arg;
	    break;

	case LLpropFBIAS:
	    dll->dllFontBias= arg != 0;
	    break;

	/* LLpropTEXT */

	case LLpropTEMPLATEID:
	    dll->dllTemplateID= arg;
	    break;

	/* LLpropNUMBERS */

	case LLpropOLD:
	    dll->dllFromOld= arg != 0;
	    break;

	case LLpropPREV:
	    dll->dllUsePrevText= arg != 0;
	    break;

	case LLpropPREVSPACE:
	    dll->dllUsePrevSpace= arg != 0;
	    break;

	case LLpropINDENT:
	    dll->dllIndent= arg;
	    break;

	case LLpropSPACE:
	    dll->dllSpace= arg;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }


/************************************************************************/
/*									*/
/*  Remember list properties.						*/
/*									*/
/*  1)  The same tag is used in the listoverrides, so set the value in	*/
/*	both structures. (Only one is used)				*/
/*									*/
/************************************************************************/

int docRtfRememberListProperty(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    DocumentList *	dl= &(rrc->rrcDocumentList);

    switch( rcw->rcwID )
	{
	case DLpropLISTID:
	    /*  1  */
	    dl->dlListID= arg;
	    rrc->rrcListOverride.loListID= arg;
	    break;

	case DLpropTEMPLATEID:
	    dl->dlListTemplateID= arg;
	    break;

	case DLpropHYBRID:
	    dl->dlListHybrid= rcw->rcwEnumValue;
	    break;

	case DLpropRESTARTHDN:
	    dl->dlRestartPerSect= arg != 0;
	    break;

	case DLpropSTYLEID:
	    dl->dlListStyleID= arg;
	    break;

	/* DLpropNAME */
	/* DLpropLEVELS */

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

RtfControlWord	docRtfListLevelGroups[]=
    {
	{ "leveltext",	LLpropTEXT,	DOClevANY, docRtfListLevelGroup, },
	{ "levelnumbers",
			LLpropNUMBERS,	DOClevANY, docRtfListLevelGroup, },

	{ 0, 0, 0 }
    };

int docRtfCommitListLevelStyle(	RtfReadingContext *	rrc )
    {
    BufferDocument *		bd= rrc->rrcBd;
    DocumentProperties *	dp= &(bd->bdProperties);
    RtfReadingState *		rrs= rrc->rrcState;

    if  ( documentListLevelSetStyle( &(rrc->rrcDocumentListLevel),
					&(rrc->rrcDocumentStyle.dsParaMask),
					&(rrs->rrsParagraphProperties),
					&(rrc->rrcDocumentStyle.dsTextMask),
					&(rrs->rrsTextAttribute) ) )
	{ LDEB(1); return -1;	}

    docCleanParagraphProperties( &(rrs->rrsParagraphProperties) );
    docInitParagraphProperties( &(rrs->rrsParagraphProperties) );
    utilInitTextAttribute( &(rrs->rrsTextAttribute) );
    rrs->rrsTextAttribute.taFontSizeHalfPoints= 24;
    rrc->rrcInDeletedText= 0;

    if  ( dp->dpDefaultFont >= 0 )
	{ rrs->rrsTextAttribute.taFontNumber= dp->dpDefaultFont;	}

    PROPmaskCLEAR( &(rrc->rrcDocumentStyle.dsParaMask) );
    PROPmaskCLEAR( &(rrc->rrcDocumentStyle.dsTextMask) );

    return 0;
    }

static int docRtfListLevel(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    DocumentProperties *	dp= &(rrc->rrcBd->bdProperties);
    RtfReadingState *		rrs= rrc->rrcState;

    docCleanDocumentListLevel( &(rrc->rrcDocumentListLevel) );
    docInitDocumentListLevel( &(rrc->rrcDocumentListLevel) );

    docCleanParagraphProperties( &(rrs->rrsParagraphProperties) );
    docInitParagraphProperties( &(rrs->rrsParagraphProperties) );
    utilInitTextAttribute( &(rrs->rrsTextAttribute) );
    rrs->rrsTextAttribute.taFontSizeHalfPoints= 24;
    rrc->rrcInDeletedText= 0;

    if  ( dp->dpDefaultFont >= 0 )
	{ rrs->rrsTextAttribute.taFontNumber= dp->dpDefaultFont;	}

    PROPmaskCLEAR( &(rrc->rrcDocumentStyle.dsParaMask) );
    PROPmaskCLEAR( &(rrc->rrcDocumentStyle.dsTextMask) );

    if  ( docRtfReadGroup( sis, rcw->rcwLevel,
			    (RtfControlWord *)0, 0, 0, rrc,
			    docRtfEmptyTable, docRtfListLevelGroups,
			    docRtfRefuseText, docRtfCommitListLevelStyle ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    if  ( docDocumentListAddLevel( &(rrc->rrcDocumentList),
			&(rrc->rrcDocumentListLevel), (int *)0, (int *)0 ) )
	{ LDEB(1); return -1;	}

    docCleanDocumentListLevel( &(rrc->rrcDocumentListLevel) );
    docInitDocumentListLevel( &(rrc->rrcDocumentListLevel) );

    return 0;
    }

static RtfControlWord	docRtfListGroups[]=
    {
	{ "listname",	DLpropNAME,	DOClevANY, docRtfListStringGroup, },
	{ "liststylename",
			DLpropSTYLENAME,
					DOClevANY, docRtfListStringGroup, },
	{ "listlevel",	DLpropLEVELS,	DOClevANY, docRtfListLevel, },

	{ 0, 0, 0 }
    };

static int docRtfList(		SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    DocumentProperties *	dp= &(rrc->rrcBd->bdProperties);

    docCleanDocumentList( &(rrc->rrcDocumentList) );
    docInitDocumentList( &(rrc->rrcDocumentList) );

    if  ( docRtfReadGroup( sis, rcw->rcwLevel,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfEmptyTable, docRtfListGroups,
				docRtfRefuseText, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    if  ( docDocumentListTableAddList( &(dp->dpListTable),
				&(rrc->rrcDocumentList), (int *)0, (int *)0 ) )
	{ LDEB(1); return -1;	}

    docCleanDocumentList( &(rrc->rrcDocumentList) );
    docInitDocumentList( &(rrc->rrcDocumentList) );

    return 0;
    }

static RtfControlWord	docRtfListtableGroups[]=
    {
	{ "list",	DPpropLISTTABLE,	DOClevANY, docRtfList, },

	{ 0, 0, 0 }
    };

int docRtfListTable(		SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    if  ( docRtfReadGroup( sis, rcw->rcwLevel,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfEmptyTable, docRtfListtableGroups,
				docRtfIgnoreText, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write a list level to rtf.						*/
/*									*/
/************************************************************************/

void docRtfWriteListLevel(	SimpleOutputStream *		sos,
				const unsigned char *		outputMapping,
				int *				pCol,
				const DocumentListLevel *	dll )
    {
    docRtfWriteDestinationBegin( "\\listlevel", pCol, sos );

    docRtfWriteArgTag( "\\levelnfc", pCol, dll->dllNumberStyle, sos );
    docRtfWriteArgTag( "\\levelnfcn", pCol, dll->dllNumberStyle, sos );
    docRtfWriteArgTag( "\\leveljc", pCol, dll->dllJustification, sos );
    docRtfWriteArgTag( "\\leveljcn", pCol, dll->dllJustification, sos );

    docRtfWriteArgTag( "\\levelfollow", pCol, dll->dllFollow, sos );
    docRtfWriteArgTag( "\\levelstartat", pCol, dll->dllStartAt, sos );

    if  ( dll->dllPrevToDigits )
	{ docRtfWriteTag( "\\levellegal", pCol, sos );	}
    if  ( dll->dllNoRestart )
	{ docRtfWriteTag( "\\levelnorestart", pCol, sos );	}

    if  ( dll->dllFromOld )
	{ docRtfWriteTag( "\\levelold", pCol, sos );	}
    if  ( dll->dllUsePrevText )
	{ docRtfWriteTag( "\\levelprev", pCol, sos );	}
    if  ( dll->dllUsePrevSpace )
	{ docRtfWriteTag( "\\levelprevspace", pCol, sos );	}

    if  ( dll->dllIndent != 0 )
	{ docRtfWriteArgTag( "\\levelindent", pCol, dll->dllIndent, sos ); }
    if  ( dll->dllSpace != 0 )
	{ docRtfWriteArgTag( "\\levelspace", pCol, dll->dllSpace, sos ); }

    if  ( dll->dllText )
	{
	const int	addSemicolon= 1;
	char		tag[50];

	strcpy( tag, "\\leveltext" );

	if  ( dll->dllTemplateID < -5 || dll->dllTemplateID > -1 )
	    {
	    sprintf( tag, "\\leveltext\\leveltemplateid%ld",
						dll->dllTemplateID );
	    }

	docRtfWriteStringDestination( tag, pCol,
						dll->dllText,
						dll->dllTextSize,
						addSemicolon, sos );
	}

    if  ( dll->dllNumbers )
	{
	const int	addSemicolon= 1;

	docRtfWriteStringDestination( "\\levelnumbers", pCol,
						dll->dllNumbers,
						dll->dllNumberSize,
						addSemicolon, sos );
	}

    if  ( dll->dllPictureNumber >= 0 )
	{
	docRtfWriteArgTag( "\\levelpicture",
				    pCol, dll->dllPictureNumber, sos );
	}

    docRtfSaveTextAttribute( sos, pCol,
				    &(dll->dllTextAttributeMask),
				    &(dll->dllTextAttribute) );

    if  ( ! utilPropMaskIsEmpty( &(dll->dllParaPropertyMask) ) )
	{
	ParagraphProperties	pp;

	docInitParagraphProperties( &pp );

	pp.ppTabStopList= dll->dllTabStopList; /* HACK */
	pp.ppLeftIndentTwips= dll->dllLeftIndentTwips;
	pp.ppFirstIndentTwips= dll->dllFirstIndentTwips;

	docRtfSaveParagraphProperties( sos, pCol,
					&(dll->dllParaPropertyMask), &pp );
	}

    docRtfWriteDestinationEnd( pCol, sos );

    return;
    }

/************************************************************************/
/*									*/
/*  Write a listtable to rtf.						*/
/*									*/
/************************************************************************/

void docRtfWriteListTable(	SimpleOutputStream *		sos,
				const unsigned char *		outputMapping,
				int *				pCol,
				const DocumentListTable *	dlt )
    {
    int				i;
    const DocumentList *	dl= dlt->dltLists;

    docRtfWriteDestinationBegin( "\\*\\listtable", pCol, sos );
    docRtfWriteNextLine( pCol, sos );

    for ( i= 0; i < dlt->dltListCount; dl++, i++ )
	{
	const DocumentListLevel *	dll;
	int				lev;

	docRtfWriteDestinationBegin( "\\list", pCol, sos );

	if  ( dl->dlListTemplateID != -1 )
	    {
	    docRtfWriteArgTag( "\\listtemplateid",
					    pCol, dl->dlListTemplateID, sos );
	    }

	if  ( dl->dlListHybrid )
	    { docRtfWriteTag( "\\listhybrid", pCol, sos );	}
	else{ docRtfWriteTag( "\\listsimple", pCol, sos );	}

	if  ( dl->dlLevelCount > 0 )
	    { docRtfWriteNextLine( pCol, sos );	}

	dll= dl->dlLevels;
	for ( lev= 0; lev < dl->dlLevelCount; dll++, lev++ )
	    {
	    docRtfWriteListLevel( sos, outputMapping, pCol, dll );
	    docRtfWriteNextLine( pCol, sos );
	    }

	if  ( dl->dlListID != -1 )
	    { docRtfWriteArgTag( "\\listid", pCol, dl->dlListID, sos );	}

	{
	const int	addSemicolon= 1;
	const char *	listname= "";

	if  ( dl->dlListName )
	    { listname= dl->dlListName;	}

	docRtfWriteStringDestination( "\\listname", pCol,
			(unsigned char *)listname, strlen( listname ),
			addSemicolon, sos );
	}

	if  ( dl->dlListStyleID != -1 )
	    {
	    docRtfWriteArgTag( "\\liststyleid", pCol, dl->dlListStyleID, sos );
	    }

	docRtfWriteDestinationEnd( pCol, sos );
	if  ( i+ 1 < dlt->dltListCount )
	    { docRtfWriteNextLine( pCol, sos );	}
	}

    docRtfWriteDestinationEnd( pCol, sos );
    docRtfWriteNextLine( pCol, sos );

    return;
    }

/************************************************************************/
/*									*/
/*  Word 95 type paragraph numbers: Just remember the properties.	*/
/*									*/
/************************************************************************/

int docRtfPnProperty(		SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    ParagraphNumber *		pn= &(rrc->rrcParagraphNumber);

    switch( rcw->rcwID )
	{
	case LLpropSTARTAT:
	    pn->pnStartAt= arg;
	    break;

	case LLpropSTYLE:
	    pn->pnNumberStyle= rcw->rcwEnumValue;
	    break;

	case LLpropJUSTIFY:
	    pn->pnJustification= rcw->rcwEnumValue;
	    break;

	case LLpropINDENT:
	    pn->pnIndent= arg;
	    break;

	case LLpropPREV:
	    pn->pnUsePrevText= arg != 0;
	    break;

	case LLpropSPACE:
	    pn->pnSpace= arg;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }


