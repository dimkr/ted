/************************************************************************/
/*									*/
/*  Read/Write document properties to/from RTF.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	<psFont.h>

#   include	"docRtf.h"

/************************************************************************/
/*									*/
/*  Remember Document Properties.					*/
/*									*/
/************************************************************************/

void docRtfReadSetAnsicpg(	RtfReadingContext *	rrc,
				int			arg )
    {
    int				i;
    DocumentProperties *	dp= &(rrc->rrcBd->bdProperties);
    int				encoding;

    for ( i= 0; i < 256; i++ )
	{ rrc->rrcDefaultInputMapping[i]= i;	}

    encoding= utilEncodingFromWindowsCodepage( arg );

    if  ( encoding >= 0 )
	{
	const FontCharset *	fc= PS_Encodings+ encoding;

	dp->dpAnsiCodepage= arg;

	if  ( fc->fcFromOfficeToX11 )
	    {
	    memcpy( rrc->rrcDefaultInputMapping, fc->fcFromOfficeToX11, 256 );
	    }
	}

    if  ( arg == 10000 )
	{
	memcpy( rrc->rrcDefaultInputMapping, docMAC_ROMAN_to_ISO1, 256 );
	dp->dpAnsiCodepage= arg;
	}

    return;
    }

static void docRtfReadSetDocumentCharset(
				RtfReadingContext *	rrc,
				int			arg )
    {
    int				i;
    DocumentProperties *	dp= &(rrc->rrcBd->bdProperties);

    switch( arg )
	{
	case DOCcharsetANSI:
	    for ( i= 0; i < 256; i++ )
		{ rrc->rrcDefaultInputMapping[i]= i;	}
	    dp->dpDocumentCharset= arg;
	    break;

	case DOCcharsetPC:
	    memcpy( rrc->rrcDefaultInputMapping, docDOS437_to_ISO1, 256 );
	    dp->dpDocumentCharset= arg;
	    break;

	case DOCcharsetPCA:
	    memcpy( rrc->rrcDefaultInputMapping, docDOS850_to_ISO1, 256 );
	    dp->dpDocumentCharset= arg;
	    break;

	case DOCcharsetMAC:
	    memcpy( rrc->rrcDefaultInputMapping, docMAC_ROMAN_to_ISO1, 256 );
	    dp->dpDocumentCharset= arg;
	    break;

	default:
	    LDEB(arg);
	    break;
	}
    }

int docRtfRememberDocProperty(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    DocumentProperties *	dp= &(rrc->rrcBd->bdProperties);
    RtfReadingState *		rrs= rrc->rrcState;
    SectionProperties *		sp= &(rrs->rrsSectionProperties);

    switch( rcw->rcwID )
	{
	case DGpropLEFT_MARGIN:
	    dp->dpGeometry.dgLeftMarginTwips= arg;
	    sp->spDocumentGeometry.dgLeftMarginTwips= arg;
	    rrc->rrcGotDocGeometry= 1;
	    break;
	case DGpropTOP_MARGIN:
	    dp->dpGeometry.dgTopMarginTwips= arg;
	    sp->spDocumentGeometry.dgTopMarginTwips= arg;
	    rrc->rrcGotDocGeometry= 1;
	    break;
	case DGpropRIGHT_MARGIN:
	    dp->dpGeometry.dgRightMarginTwips= arg;
	    sp->spDocumentGeometry.dgRightMarginTwips= arg;
	    rrc->rrcGotDocGeometry= 1;
	    break;
	case DGpropBOTTOM_MARGIN:
	    dp->dpGeometry.dgBottomMarginTwips= arg;
	    sp->spDocumentGeometry.dgBottomMarginTwips= arg;
	    break;
	case DGpropPAGE_WIDTH:
	    dp->dpGeometry.dgPageWideTwips= arg;
	    sp->spDocumentGeometry.dgPageWideTwips= arg;
	    rrc->rrcGotDocGeometry= 1;
	    break;
	case DGpropPAGE_HEIGHT:
	    dp->dpGeometry.dgPageHighTwips= arg;
	    sp->spDocumentGeometry.dgPageHighTwips= arg;
	    rrc->rrcGotDocGeometry= 1;
	    break;
	case DGpropGUTTER:
	    dp->dpGeometry.dgGutterTwips= arg;
	    sp->spDocumentGeometry.dgGutterTwips= arg;
	    rrc->rrcGotDocGeometry= 1;
	    break;

	case DPpropFACING_PAGES:
	    dp->dpHasFacingPages= arg != 0;
	    break;
	case DPpropWIDOWCTRL:
	    dp->dpWidowControl= arg != 0;
	    break;
	case DPpropTWO_ON_ONE:
	    dp->dpTwoOnOne= arg != 0;
	    break;
	case DPpropDOCTEMP:
	    dp->dpIsDocumentTemplate= arg != 0;
	    break;

	case DPpropSTART_PAGE:
	    dp->dpStartPageNumber= arg;
	    break;
	case DPpropDEFTAB:
	    dp->dpTabIntervalTwips= arg;
	    break;

	case DPpropDEFF:
	    dp->dpDefaultFont= arg;
	    rrs->rrsTextAttribute.taFontNumber= arg;
	    break;
	case DPpropSTSHFDBCH:
	    dp->dpDefaultFontDbch= arg;
	    break;
	case DPpropSTSHFLOCH:
	    dp->dpDefaultFontLoch= arg;
	    break;
	case DPpropSTSHFHICH:
	    dp->dpDefaultFontHich= arg;
	    break;
	case DPpropSTSHFBI:
	    dp->dpDefaultFontBi= arg;
	    break;

	case DPpropNOTETYPES:
	    if  ( arg < 0 || arg > 2 )
		{ LDEB(arg); break;	}
	    dp->dpFootEndNoteType= arg;
	    break;

	case DPpropFOOTNOTE_STARTNR:
	    dp->dpFootnoteProperties.npStartNumber= arg;
	    break;
	case DPpropENDNOTE_STARTNR:
	    dp->dpEndnoteProperties.npStartNumber= arg;
	    break;

	case DPpropFOOTNOTE_POSITION:
	    dp->dpFootnoteProperties.npPosition= rcw->rcwEnumValue;
	    break;

	case DPpropENDNOTE_POSITION:
	    dp->dpEndnoteProperties.npPosition= rcw->rcwEnumValue;
	    break;

	case DPpropFOOTNOTE_RESTART:
	    dp->dpFootnoteProperties.npRestart= rcw->rcwEnumValue;
	    break;

	case DPpropENDNOTE_RESTART:
	    dp->dpEndnoteProperties.npRestart= rcw->rcwEnumValue;
	    break;

	case DPpropFOOTNOTE_STYLE:
	    dp->dpFootnoteProperties.npNumberStyle= rcw->rcwEnumValue;
	    break;

	case DPpropENDNOTE_STYLE:
	    dp->dpEndnoteProperties.npNumberStyle= rcw->rcwEnumValue;
	    break;

	case DPpropDOC_CHARSET:
	    docRtfReadSetDocumentCharset( rrc, rcw->rcwEnumValue );
	    break;

	case DPpropANSICPG:
	    docRtfReadSetAnsicpg( rrc, arg );
	    break;

	case DPpropTOP_BORDER:
	    dp->dpTopBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;

	case DPpropBOTTOM_BORDER:
	    dp->dpBottomBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;

	case DPpropLEFT_BORDER:
	    dp->dpLeftBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;

	case DPpropRIGHT_BORDER:
	    dp->dpRightBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;

	case DPpropHEAD_BORDER:
	    dp->dpHeadBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;

	case DPpropFOOT_BORDER:
	    dp->dpFootBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;

	default:
	    break;
	}
    
    return 0;
    }

/************************************************************************/
/*									*/
/*  Save Document Properties to RTF.					*/
/*									*/
/************************************************************************/

static void docRtfSaveInfo(	const char *		tag,
				const unsigned char *	info,
				RtfWritingContext *	rwc,
				int *			pCol,
				SimpleOutputStream *	sos )
    {
    if  ( ! info )
	{ return;	}

    docRtfWriteDestinationBegin( tag, pCol, sos );

    docRtfEscapeString( info, rwc->rwcDefaultOutputMapping,
					pCol, strlen( (char *)info ), sos );

    docRtfWriteDestinationEnd( pCol, sos );
    docRtfWriteNextLine( pCol, sos );

    return;
    }

static void docRtfSaveDate(	const char *		tag,
				int *			pCol,
				const struct tm *	tm,
				SimpleOutputStream *	sos )
    {
    if  ( tm->tm_mday == 0 )
	{ return;	}

    docRtfWriteDestinationBegin( tag, pCol, sos );

    docRtfWriteArgTag( "\\yr",	pCol, tm->tm_year+ 1900, sos );
    docRtfWriteArgTag( "\\mo",	pCol, tm->tm_mon+ 1, sos );
    docRtfWriteArgTag( "\\dy",	pCol, tm->tm_mday, sos );
    docRtfWriteArgTag( "\\hr",	pCol, tm->tm_hour, sos );
    docRtfWriteArgTag( "\\min",	pCol, tm->tm_min, sos );
    docRtfWriteArgTag( "\\sec",	pCol, tm->tm_sec, sos );

    docRtfWriteDestinationEnd( pCol, sos );
    docRtfWriteNextLine( pCol, sos );

    return;
    }

int docRtfSaveDocumentProperties(	SimpleOutputStream *	sos,
					int *			pCol,
					RtfWritingContext *	rwc,
					const BufferDocument *	bd )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);
    const DocumentGeometry *	dg= &(dp->dpGeometry);

    int				codePage= bd->bdProperties.dpAnsiCodepage;
    int				defaultCharset= FONTcharsetDEFAULT;

    if  ( codePage >= 0 )
	{
	int		encoding;
	defaultCharset= utilWindowsCharsetFromCodepage( codePage );

	if  ( defaultCharset < 0 )
	    {
	    LLDEB(codePage,defaultCharset);
	    defaultCharset= FONTcharsetDEFAULT;
	    }

	encoding= utilEncodingFromWindowsCodepage( codePage );
	if  ( encoding >= 0 )
	    {
	    const FontCharset *	fc= PS_Encodings+ encoding;

	    if  ( fc->fcFromX11ToOffice )
		{
		memcpy( rwc->rwcDefaultOutputMapping,
					    fc->fcFromX11ToOffice, 256 );
		}
	    }
	}

    if  ( dp->dpDefaultFont >= 0			&&
	  dp->dpDefaultFont < dp->dpFontList.dflFontCount	)
	{ docRtfWriteArgTag( "\\deff", pCol, dp->dpDefaultFont, sos );	}

    if  ( dp->dpDefaultFontDbch >= 0			&&
	  dp->dpDefaultFontDbch < dp->dpFontList.dflFontCount	)
	{
	docRtfWriteArgTag( "\\stshfdbch", pCol, dp->dpDefaultFontDbch, sos );
	}

    if  ( dp->dpDefaultFontLoch >= 0			&&
	  dp->dpDefaultFontLoch < dp->dpFontList.dflFontCount	)
	{
	docRtfWriteArgTag( "\\stshfloch", pCol, dp->dpDefaultFontLoch, sos );
	}

    if  ( dp->dpDefaultFontHich >= 0			&&
	  dp->dpDefaultFontHich < dp->dpFontList.dflFontCount	)
	{
	docRtfWriteArgTag( "\\stshfhich", pCol, dp->dpDefaultFontHich, sos );
	}

    if  ( dp->dpDefaultFontBi >= 0			&&
	  dp->dpDefaultFontBi < dp->dpFontList.dflFontCount	)
	{ docRtfWriteArgTag( "\\stshfbi", pCol, dp->dpDefaultFontBi, sos ); }

    if  ( dp->dpFontList.dflFontCount > 0 )
	{
	docRtfWriteNextLine( pCol, sos );
	docRtfWriteFontTable( sos, pCol, &(dp->dpFontList) );
	}

    if  ( dp->dpColorCount > 0 )
	{
	docRtfWriteNextLine( pCol, sos );
	docRtfWriteColorTable( sos, pCol, dp );
	}

    if  ( bd->bdStyleSheet.dssStyleCount > 0 )
	{
	docRtfWriteNextLine( pCol, sos );
	docRtfWriteStyleSheet( sos, rwc->rwcDefaultOutputMapping,
					    pCol, &(bd->bdStyleSheet) );
	}

    if  ( dp->dpListTable.dltListCount > 0 )
	{
	docRtfWriteNextLine( pCol, sos );
	docRtfWriteListTable( sos, rwc->rwcDefaultOutputMapping,
					    pCol, &(dp->dpListTable) );
	}

    if  ( dp->dpListOverrideTable.lotOverrideCount > 0 )
	{
	docRtfWriteNextLine( pCol, sos );
	docRtfWriteListOverrideTable( sos, rwc->rwcDefaultOutputMapping,
					    pCol, &(dp->dpListOverrideTable) );
	}

    if  ( dp->dpAuthorCount > 0 )
	{
	docRtfWriteNextLine( pCol, sos );
	docRtfWriteRevisorTable( sos, rwc->rwcDefaultOutputMapping, pCol, dp );
	}

    docRtfSaveInfo( "\\*\\generator ",	dp->dpGenerator, rwc, pCol, sos );

    if  ( docHasDocumentInfo( dp ) )
	{
	docRtfWriteNextLine( pCol, sos );
	docRtfWriteDestinationBegin( "\\info", pCol, sos );
	docRtfWriteNextLine( pCol, sos );

	docRtfSaveInfo( "\\title ",	dp->dpTitle, rwc, pCol, sos );
	docRtfSaveInfo( "\\author ",	dp->dpAuthor, rwc, pCol, sos );
	docRtfSaveInfo( "\\company ",	dp->dpCompany, rwc, pCol, sos );
	docRtfSaveInfo( "\\subject ",	dp->dpSubject, rwc, pCol, sos );
	docRtfSaveInfo( "\\keywords ",	dp->dpKeywords, rwc, pCol, sos );
	docRtfSaveInfo( "\\doccomm ",	dp->dpDoccomm, rwc, pCol, sos );
	docRtfSaveInfo( "\\hlinkbase ",	dp->dpHlinkbase, rwc, pCol, sos );

	docRtfSaveDate( "\\creatim ",	pCol, &(dp->dpCreatim), sos );
	docRtfSaveDate( "\\revtim ",	pCol, &(dp->dpRevtim), sos );
	docRtfSaveDate( "\\printim ",	pCol, &(dp->dpPrintim), sos );

	docRtfWriteDestinationEnd( pCol, sos );
	}

    docRtfWriteNextLine( pCol, sos );

    docRtfWriteArgTag( "\\paperw", pCol, dg->dgPageWideTwips, sos );
    docRtfWriteArgTag( "\\paperh", pCol, dg->dgPageHighTwips, sos );

    docRtfWriteArgTag( "\\margl", pCol, dg->dgLeftMarginTwips, sos );
    docRtfWriteArgTag( "\\margr", pCol, dg->dgRightMarginTwips, sos );
    docRtfWriteArgTag( "\\margt", pCol, dg->dgTopMarginTwips, sos );
    docRtfWriteArgTag( "\\margb", pCol, dg->dgBottomMarginTwips, sos );

    if  ( dg->dgGutterTwips != 0 )
	{ docRtfWriteArgTag( "\\gutter", pCol, dg->dgGutterTwips, sos ); }

    if  ( dp->dpHasFacingPages )
	{ docRtfWriteTag( "\\facingp", pCol, sos );	}

    if  ( dp->dpWidowControl )
	{ docRtfWriteTag( "\\widowctrl", pCol, sos );	}

    if  ( dp->dpTwoOnOne )
	{ docRtfWriteTag( "\\twoonone", pCol, sos );	}
    if  ( dp->dpIsDocumentTemplate )
	{ docRtfWriteTag( "\\doctemp", pCol, sos );	}

    if  ( dp->dpTabIntervalTwips > 0 && dp->dpTabIntervalTwips != 720 )
	{ docRtfWriteArgTag( "\\deftab", pCol, dp->dpTabIntervalTwips, sos ); }

    docRtfWriteNextLine( pCol, sos );

    switch( dp->dpFootnoteProperties.npPosition )
	{
	case FTN_POS_SECT_END:
	    docRtfWriteTag( "\\endnotes", pCol, sos );
	    break;
	case FTN_POS_DOC_END:
	    docRtfWriteTag( "\\enddoc", pCol, sos );
	    break;
	case FTN_POS_BELOW_TEXT:
	    docRtfWriteTag( "\\ftntj", pCol, sos );
	    break;
	case FTN_POS_PAGE_BOTTOM:
	    docRtfWriteTag( "\\ftnbj", pCol, sos );
	    break;
	default:
	    LDEB(dp->dpFootnoteProperties.npPosition); break;
	}

    switch( dp->dpEndnoteProperties.npPosition )
	{
	case FTN_POS_SECT_END:
	    docRtfWriteTag( "\\aendnotes", pCol, sos );
	    break;
	case FTN_POS_DOC_END:
	    docRtfWriteTag( "\\aenddoc", pCol, sos );
	    break;
	case FTN_POS_BELOW_TEXT:
	    docRtfWriteTag( "\\aftntj", pCol, sos );
	    break;
	case FTN_POS_PAGE_BOTTOM:
	    docRtfWriteTag( "\\aftnbj", pCol, sos );
	    break;
	default:
	    LDEB(dp->dpEndnoteProperties.npPosition); break;
	}

    if  ( dp->dpFootnoteProperties.npStartNumber != 1 )
	{
	docRtfWriteArgTag( "\\ftnstart", pCol,
			    dp->dpFootnoteProperties.npStartNumber, sos );
	}
    if  ( dp->dpEndnoteProperties.npStartNumber != 1 )
	{
	docRtfWriteArgTag( "\\aftnstart", pCol,
			    dp->dpEndnoteProperties.npStartNumber, sos );
	}

    switch( dp->dpFootnoteProperties.npRestart )
	{
	case FTN_RST_CONTINUOUS:
	    docRtfWriteTag( "\\ftnrstcont", pCol, sos );
	    break;
	case FTN_RST_PER_SECTION:
	    docRtfWriteTag( "\\ftnrestart", pCol, sos );
	    break;
	case FTN_RST_PER_PAGE:
	    docRtfWriteTag( "\\ftnrstpg", pCol, sos );
	    break;
	default:
	    LDEB(dp->dpFootnoteProperties.npRestart); break;
	}

    switch( dp->dpEndnoteProperties.npRestart )
	{
	case FTN_RST_CONTINUOUS:
	    docRtfWriteTag( "\\aftnrstcont", pCol, sos );
	    break;
	case FTN_RST_PER_SECTION:
	    docRtfWriteTag( "\\aftnrestart", pCol, sos );
	    break;
	/* No!
	case FTN_RST_PER_PAGE:
	    docRtfWriteTag( "\\aftnrstpg", pCol, sos );
	    break;
	*/
	default:
	    LDEB(dp->dpEndnoteProperties.npRestart); break;
	}


    switch( dp->dpFootnoteProperties.npNumberStyle )
	{
	case FTNstyleNAR:
	    docRtfWriteTag( "\\ftnnar", pCol, sos );
	    break;
	case FTNstyleNALC:
	    docRtfWriteTag( "\\ftnnalc", pCol, sos );
	    break;
	case FTNstyleNAUC:
	    docRtfWriteTag( "\\ftnnauc", pCol, sos );
	    break;
	case FTNstyleNRLC:
	    docRtfWriteTag( "\\ftnnrlc", pCol, sos );
	    break;
	case FTNstyleNRUC:
	    docRtfWriteTag( "\\ftnnruc", pCol, sos );
	    break;
	case FTNstyleNCHI:
	    docRtfWriteTag( "\\ftnnchi", pCol, sos );
	    break;
	default:
	    LDEB(dp->dpFootnoteProperties.npNumberStyle); break;
	}

    switch( dp->dpEndnoteProperties.npNumberStyle )
	{
	case FTNstyleNAR:
	    docRtfWriteTag( "\\aftnnar", pCol, sos );
	    break;
	case FTNstyleNALC:
	    docRtfWriteTag( "\\aftnnalc", pCol, sos );
	    break;
	case FTNstyleNAUC:
	    docRtfWriteTag( "\\aftnnauc", pCol, sos );
	    break;
	case FTNstyleNRLC:
	    docRtfWriteTag( "\\aftnnrlc", pCol, sos );
	    break;
	case FTNstyleNRUC:
	    docRtfWriteTag( "\\aftnnruc", pCol, sos );
	    break;
	case FTNstyleNCHI:
	    docRtfWriteTag( "\\aftnnchi", pCol, sos );
	    break;
	default:
	    LDEB(dp->dpEndnoteProperties.npNumberStyle); break;
	}

    if  ( dp->dpTopBorder.bpStyle != DOCbsNONE )
	{ docRtfSaveBorder( "\\pgbrdrt", pCol, &(dp->dpTopBorder), sos ); }
    if  ( dp->dpBottomBorder.bpStyle != DOCbsNONE )
	{ docRtfSaveBorder( "\\pgbrdrb", pCol, &(dp->dpBottomBorder), sos ); }
    if  ( dp->dpLeftBorder.bpStyle != DOCbsNONE )
	{ docRtfSaveBorder( "\\pgbrdrl", pCol, &(dp->dpLeftBorder), sos ); }
    if  ( dp->dpRightBorder.bpStyle != DOCbsNONE )
	{ docRtfSaveBorder( "\\pgbrdrr", pCol, &(dp->dpRightBorder), sos ); }
    if  ( dp->dpHeadBorder.bpStyle != DOCbsNONE )
	{ docRtfSaveBorder( "\\pgbrdrhead", pCol, &(dp->dpHeadBorder), sos ); }
    if  ( dp->dpFootBorder.bpStyle != DOCbsNONE )
	{ docRtfSaveBorder( "\\pgbrdrfoot", pCol, &(dp->dpFootBorder), sos ); }

    docRtfWriteNextLine( pCol, sos );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save the various separators relating to notes.			*/
/*									*/
/************************************************************************/

int docRtfSaveDocNotesSeparators(	SimpleOutputStream *	sos,
					int *			pCol,
					const BufferDocument *	bd,
					RtfWritingContext *	rwc )
    {
    if  ( docRtfSaveExternalItem( sos, pCol, bd->bdEiFtnsep.eiItem,
						bd, "\\ftnsep", rwc )	)
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveExternalItem( sos, pCol, bd->bdEiFtnsepc.eiItem,
						bd, "\\ftnsepc", rwc )	)
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveExternalItem( sos, pCol, bd->bdEiFtncn.eiItem,
						bd, "\\ftncn", rwc )	)
	{ LDEB(1); return -1;	}

    /******/

    if  ( docRtfSaveExternalItem( sos, pCol, bd->bdEiAftnsep.eiItem,
						bd, "\\aftnsep", rwc )	)
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveExternalItem( sos, pCol, bd->bdEiAftnsepc.eiItem,
						bd, "\\aftnsepc", rwc )	)
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveExternalItem( sos, pCol, bd->bdEiAftncn.eiItem,
						bd, "\\aftncn", rwc )	)
	{ LDEB(1); return -1;	}

    return 0;
    }

