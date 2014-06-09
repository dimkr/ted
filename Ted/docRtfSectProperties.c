/************************************************************************/
/*									*/
/*  Exchange of section properties with RTF.				*/
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
/*  Write Section Properties.						*/
/*									*/
/************************************************************************/

void docRtfSaveSectionProperties( SimpleOutputStream *		sos,
				const unsigned char *		outputMapping,
				int *				pCol,
				const PropertyMask *		updMask,
				const SectionProperties *	sp )
    {
    int				i;
    const ParagraphNumber *	pn;

    const DocumentGeometry *	dg= &(sp->spDocumentGeometry);

    /**************/

    if  ( PROPmaskISSET( updMask, DGpropPAGE_WIDTH ) )
	{
	docRtfWriteArgTag( "\\pgwsxn",	pCol, dg->dgPageWideTwips, sos );
	}
    if  ( PROPmaskISSET( updMask, DGpropPAGE_HEIGHT ) )
	{
	docRtfWriteArgTag( "\\pghsxn",	pCol, dg->dgPageHighTwips, sos );
	}
    if  ( PROPmaskISSET( updMask, DGpropLEFT_MARGIN ) )
	{
	docRtfWriteArgTag( "\\marglsxn",pCol, dg->dgLeftMarginTwips, sos );
	}
    if  ( PROPmaskISSET( updMask, DGpropTOP_MARGIN ) )
	{
	docRtfWriteArgTag( "\\margtsxn",pCol, dg->dgTopMarginTwips, sos );
	}
    if  ( PROPmaskISSET( updMask, DGpropRIGHT_MARGIN ) )
	{
	docRtfWriteArgTag( "\\margrsxn",pCol, dg->dgRightMarginTwips, sos );
	}
    if  ( PROPmaskISSET( updMask, DGpropBOTTOM_MARGIN ) )
	{
	docRtfWriteArgTag( "\\margbsxn",pCol, dg->dgBottomMarginTwips, sos );
	}
    if  ( PROPmaskISSET( updMask, DGpropGUTTER ) )
	{
	docRtfWriteArgTag( "\\guttersxn",pCol, dg->dgGutterTwips, sos );
	}
    if  ( PROPmaskISSET( updMask, DGpropHEADER_POSITION ) )
	{
	docRtfWriteArgTag( "\\headery", pCol, dg->dgHeaderPositionTwips, sos );
	}
    if  ( PROPmaskISSET( updMask, DGpropFOOTER_POSITION ) )
	{
	docRtfWriteArgTag( "\\footery", pCol, dg->dgFooterPositionTwips, sos );
	}

    /**************/

    if  ( PROPmaskISSET( updMask, SPpropSTYLE ) )
	{
	docRtfWriteArgTag( "\\ds",pCol, sp->spStyle, sos );
	}

    if  ( PROPmaskISSET( updMask, SPpropTITLEPG ) )
	{
	if  ( sp->spHasTitlePage )
	    { docRtfWriteTag( "\\titlepg", pCol, sos );			 }
	else{ docRtfWriteArgTag( "\\titlepg", pCol, sp->spHasTitlePage, sos ); }
	}

    if  ( PROPmaskISSET( updMask, SPpropBREAK_KIND ) )
	{
	switch( sp->spBreakKind )
	    {
	    case DOCsbkNONE:
		docRtfWriteTag( "\\sbknone", pCol, sos );	break;
	    case DOCsbkCOL:
		docRtfWriteTag( "\\sbkcol", pCol, sos );	break;
	    case DOCsbkPAGE:
		docRtfWriteTag( "\\sbkpage", pCol, sos );	break;
	    case DOCsbkEVEN:
		docRtfWriteTag( "\\sbkeven", pCol, sos );	break;
	    case DOCsbkODD:
		docRtfWriteTag( "\\sbkodd", pCol, sos );	break;
	    default:
		LDEB(sp->spBreakKind);
	    }
	}

    if  ( PROPmaskISSET( updMask, SPpropNUMBER_STYLE ) )
	{
	switch( sp->spPageNumberStyle )
	    {
	    case DOCpgnDEC:
		docRtfWriteTag( "\\pgndec", pCol, sos );	break;
	    case DOCpgnUCRM:
		docRtfWriteTag( "\\pgnucrm", pCol, sos );	break;
	    case DOCpgnLCRM:
		docRtfWriteTag( "\\pgnlcrm", pCol, sos );	break;
	    case DOCpgnUCLTR:
		docRtfWriteTag( "\\pgnucltr", pCol, sos );	break;
	    case DOCpgnLCLTR:
		docRtfWriteTag( "\\pgnlcltr", pCol, sos );	break;
	    default:
		LDEB(sp->spPageNumberStyle);
	    }
	}

    if  ( PROPmaskISSET( updMask, SPpropPAGE_RESTART ) )
	{
	if  ( sp->spRestartPageNumbers )
	    { docRtfWriteTag( "\\pgnrestart", pCol, sos );	}
	else{ docRtfWriteTag( "\\pgncont", pCol, sos );		}
	}

    if  ( PROPmaskISSET( updMask, SPpropSTART_PAGE ) )
	{
	docRtfWriteArgTag( "\\pgnstarts",
				pCol, sp->spStartPageNumber+ 1, sos );
	}

    if  ( PROPmaskISSET( updMask, SPpropCOLUMN_COUNT ) )
	{ docRtfWriteArgTag( "\\cols", pCol, sp->spColumnCount, sos );	}
    if  ( PROPmaskISSET( updMask, SPpropCOLUMN_SPACING ) )
	{ docRtfWriteArgTag( "\\colsx", pCol, sp->spColumnSpacingTwips, sos ); }
    if  ( PROPmaskISSET( updMask, SPpropCOLUMN_WIDTH ) )
	{ docRtfWriteArgTag( "\\colw", pCol, sp->spColumnWidthTwips, sos ); }

    docRtfWriteNextLine( pCol, sos );

    /**************/

    if  ( PROPmaskISSET( updMask, SPpropFOOTNOTE_POSITION ) )
	{
	switch( sp->spFootnoteProperties.npPosition )
	    {
	    /* No!
	    case FTN_POS_SECT_END:
		docRtfWriteTag( "\\endnotes", pCol, sos );
		break;
	    case FTN_POS_DOC_END:
		docRtfWriteTag( "\\enddoc", pCol, sos );
		break;
	    */
	    case FTN_POS_BELOW_TEXT:
		docRtfWriteTag( "\\sftntj", pCol, sos );
		break;
	    case FTN_POS_PAGE_BOTTOM:
		docRtfWriteTag( "\\sftnbj", pCol, sos );
		break;
	    default:
		LDEB(sp->spFootnoteProperties.npPosition); break;
	    }
	}

    if  ( PROPmaskISSET( updMask, SPpropENDNOTE_POSITION ) )
	{
	switch( sp->spEndnoteProperties.npPosition )
	    {
	    /* No!
	    case FTN_POS_SECT_END:
		docRtfWriteTag( "\\aendnotes", pCol, sos );
		break;
	    case FTN_POS_DOC_END:
		docRtfWriteTag( "\\aenddoc", pCol, sos );
		break;
	    */
	    case FTN_POS_BELOW_TEXT:
		docRtfWriteTag( "\\saftntj", pCol, sos );
		break;
	    case FTN_POS_PAGE_BOTTOM:
		docRtfWriteTag( "\\saftnbj", pCol, sos );
		break;
	    default:
		LDEB(sp->spEndnoteProperties.npPosition); break;
	    }
	}

    if  ( PROPmaskISSET( updMask, SPpropFOOTNOTE_STARTNR ) )
	{
	docRtfWriteArgTag( "\\sftnstart", pCol,
			    sp->spFootnoteProperties.npStartNumber, sos );
	}
    if  ( PROPmaskISSET( updMask, SPpropENDNOTE_STARTNR ) )
	{
	docRtfWriteArgTag( "\\saftnstart", pCol,
			    sp->spEndnoteProperties.npStartNumber, sos );
	}

    if  ( PROPmaskISSET( updMask, SPpropFOOTNOTE_RESTART ) )
	{
	switch( sp->spFootnoteProperties.npRestart )
	    {
	    case FTN_RST_CONTINUOUS:
		docRtfWriteTag( "\\sftnrstcont", pCol, sos );
		break;
	    case FTN_RST_PER_SECTION:
		docRtfWriteTag( "\\sftnrestart", pCol, sos );
		break;
	    case FTN_RST_PER_PAGE:
		docRtfWriteTag( "\\sftnrstpg", pCol, sos );
		break;
	    default:
		LDEB(sp->spFootnoteProperties.npRestart); break;
	    }
	}

    if  ( PROPmaskISSET( updMask, SPpropENDNOTE_RESTART ) )
	{
	switch( sp->spEndnoteProperties.npRestart )
	    {
	    case FTN_RST_CONTINUOUS:
		docRtfWriteTag( "\\saftnrstcont", pCol, sos );
		break;
	    case FTN_RST_PER_SECTION:
		docRtfWriteTag( "\\saftnrestart", pCol, sos );
		break;
	    /* No!
	    case FTN_RST_PER_PAGE:
		docRtfWriteTag( "\\saftnrstpg", pCol, sos );
		break;
	    */
	    default:
		LDEB(sp->spEndnoteProperties.npRestart); break;
	    }
	}

    if  ( PROPmaskISSET( updMask, SPpropFOOTNOTE_STYLE ) )
	{
	switch( sp->spFootnoteProperties.npNumberStyle )
	    {
	    case FTNstyleNAR:
		docRtfWriteTag( "\\sftnnar", pCol, sos );
		break;
	    case FTNstyleNALC:
		docRtfWriteTag( "\\sftnnalc", pCol, sos );
		break;
	    case FTNstyleNAUC:
		docRtfWriteTag( "\\sftnnauc", pCol, sos );
		break;
	    case FTNstyleNRLC:
		docRtfWriteTag( "\\sftnnrlc", pCol, sos );
		break;
	    case FTNstyleNRUC:
		docRtfWriteTag( "\\sftnnruc", pCol, sos );
		break;
	    case FTNstyleNCHI:
		docRtfWriteTag( "\\sftnnchi", pCol, sos );
		break;
	    default:
		LDEB(sp->spFootnoteProperties.npNumberStyle); break;
	    }
	}

    if  ( PROPmaskISSET( updMask, SPpropENDNOTE_STYLE ) )
	{
	switch( sp->spEndnoteProperties.npNumberStyle )
	    {
	    case FTNstyleNAR:
		docRtfWriteTag( "\\saftnnar", pCol, sos );
		break;
	    case FTNstyleNALC:
		docRtfWriteTag( "\\saftnnalc", pCol, sos );
		break;
	    case FTNstyleNAUC:
		docRtfWriteTag( "\\saftnnauc", pCol, sos );
		break;
	    case FTNstyleNRLC:
		docRtfWriteTag( "\\saftnnrlc", pCol, sos );
		break;
	    case FTNstyleNRUC:
		docRtfWriteTag( "\\saftnnruc", pCol, sos );
		break;
	    case FTNstyleNCHI:
		docRtfWriteTag( "\\saftnnchi", pCol, sos );
		break;
	    default:
		LDEB(sp->spEndnoteProperties.npNumberStyle); break;
	    }
	}

    /**************/

    pn= sp->spParagraphNumbers;
    for ( i= 0; i < sp->spParagraphNumberCount; pn++, i++ )
	{
	if  ( pn->pnLevel < 0 )
	    { continue;	}

	docRtfWriteDestinationBegin( "\\*", pCol, sos );
	docRtfWriteArgTag( "\\pnseclvl", pCol, pn->pnLevel+ 1, sos );

	switch( pn->pnNumberStyle )
	    {
	    case DOCpnCARD:
		docRtfWriteTag( "\\pncard", pCol, sos );
		break;
	    case DOCpnDEC:
		docRtfWriteTag( "\\pndec", pCol, sos );
		break;
	    case DOCpnUCLTR:
		docRtfWriteTag( "\\pnucltr", pCol, sos );
		break;
	    case DOCpnUCRM:
		docRtfWriteTag( "\\pnucrm", pCol, sos );
		break;
	    case DOCpnLCLTR:
		docRtfWriteTag( "\\pnlcltr", pCol, sos );
		break;
	    case DOCpnLCRM:
		docRtfWriteTag( "\\pnlcrm", pCol, sos );
		break;
	    case DOCpnORD:
		docRtfWriteTag( "\\pnord", pCol, sos );
		break;
	    case DOCpnORDT:
		docRtfWriteTag( "\\pnordt", pCol, sos );
		break;
	    default:
		LDEB(pn->pnNumberStyle); break;
	    }

	docRtfWriteArgTag( "\\pnstart", pCol, pn->pnStartAt+ 1, sos );

	if  ( pn->pnUseHangingIndent )
	    { sioOutPutString( "\\pnhang", sos );	}

	if  ( pn->pnIndent > 0 )
	    { docRtfWriteArgTag( "\\pnindent", pCol, pn->pnIndent, sos ); }

	if  ( pn->pnSpace > 0 )
	    { docRtfWriteArgTag( "\\pnsp", pCol, pn->pnSpace, sos ); }

	if  ( pn->pnTextBefore[0] )
	    {
	    docRtfWriteDestinationBegin( "\\pntxtb ", pCol, sos );

	    docRtfEscapeString( pn->pnTextBefore, outputMapping,
			    pCol, strlen( (char *)pn->pnTextBefore ), sos );

	    docRtfWriteDestinationEnd( pCol, sos );
	    }

	if  ( pn->pnTextAfter[0] )
	    {
	    docRtfWriteDestinationBegin( "\\pntxta ", pCol, sos );

	    docRtfEscapeString( pn->pnTextAfter, outputMapping,
			    pCol, strlen( (char *)pn->pnTextAfter ), sos );

	    docRtfWriteDestinationEnd( pCol, sos );
	    }

	docRtfWriteDestinationEnd( pCol, sos );
	docRtfWriteNextLine( pCol, sos );
	}

    return;
    }


/************************************************************************/
/*									*/
/*  Handle a section property when reading RTF.				*/
/*									*/
/************************************************************************/

int docRtfRememberSectionProperty(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    RtfReadingState *		rrs= rrc->rrcState;
    SectionProperties *		sp= &(rrs->rrsSectionProperties);
    DocumentGeometry *		dg= &(sp->spDocumentGeometry);

    switch( rcw->rcwID )
	{
	case SPprop_NONE:
	    {
	    const DocumentProperties *	dp;

	    if  ( ! rrc->rrcBd )
		{ XDEB(rrc->rrcBd); return -1;	}

	    dp= &(rrc->rrcBd->bdProperties);

	    docCleanSectionProperties( sp );
	    docInitSectionProperties( sp );

	    sp->spDocumentGeometry= dp->dpGeometry;
	    sp->spFootnoteProperties= dp->dpFootnoteProperties;
	    sp->spEndnoteProperties= dp->dpEndnoteProperties;
	    }
	    return 0;

	case DGpropPAGE_WIDTH:
	    dg->dgPageWideTwips= arg;
	    break;

	case DGpropPAGE_HEIGHT:
	    dg->dgPageHighTwips= arg;
	    break;

	case DGpropLEFT_MARGIN:
	    dg->dgLeftMarginTwips= arg;
	    break;

	case DGpropRIGHT_MARGIN:
	    dg->dgRightMarginTwips= arg;
	    break;

	case DGpropTOP_MARGIN:
	    dg->dgTopMarginTwips= arg;
	    break;

	case DGpropBOTTOM_MARGIN:
	    dg->dgBottomMarginTwips= arg;
	    break;

	case DGpropHEADER_POSITION:
	    if  ( arg != 0 )
		{ dg->dgHeaderPositionTwips= arg; }
	    break;

	case DGpropFOOTER_POSITION:
	    if  ( arg != 0 )
		{ dg->dgFooterPositionTwips= arg; }
	    break;

	case DGpropGUTTER:
	    dg->dgGutterTwips= arg;
	    break;

	case SPpropSTYLE:
	    rrc->rrcDocumentStyle.dsLevel= DOClevSECT;
	    sp->spStyle= arg;
	    break;

	case SPpropTITLEPG:
	    sp->spHasTitlePage= ( arg != 0 );
	    break;

	case SPpropBREAK_KIND:
	    sp->spBreakKind= rcw->rcwEnumValue;
	    break;

	case SPpropNUMBER_STYLE:
	    sp->spPageNumberStyle= rcw->rcwEnumValue;
	    break;

	case SPpropPAGE_RESTART:
	    sp->spRestartPageNumbers= rcw->rcwEnumValue;
	    break;

	case SPpropSTART_PAGE:
	    sp->spStartPageNumber= arg;
	    break;

	case SPpropCOLUMN_COUNT:
	    sp->spColumnCount= arg;
	    break;

	case SPpropCOLUMN_SPACING:
	    sp->spColumnSpacingTwips= arg;
	    break;

	case SPpropCOLUMN_WIDTH:
	    sp->spColumnWidthTwips= arg;
	    break;

	/***/
	case SPpropFOOTNOTE_STARTNR:
	    sp->spFootnoteProperties.npStartNumber= arg;
	    break;
	case SPpropENDNOTE_STARTNR:
	    sp->spEndnoteProperties.npStartNumber= arg;
	    break;

	case SPpropFOOTNOTE_POSITION:
	    sp->spFootnoteProperties.npPosition= rcw->rcwEnumValue;
	    break;
	/* No!
	case SPpropENDNOTE_POSITION:
	    sp->spEndnoteProperties.npPosition= rcw->rcwEnumValue;
	    break;
	*/

	case SPpropFOOTNOTE_RESTART:
	    sp->spFootnoteProperties.npRestart= rcw->rcwEnumValue;
	    break;

	case SPpropENDNOTE_RESTART:
	    sp->spEndnoteProperties.npRestart= rcw->rcwEnumValue;
	    break;

	case SPpropFOOTNOTE_STYLE:
	    sp->spFootnoteProperties.npNumberStyle= rcw->rcwEnumValue;
	    break;

	case SPpropENDNOTE_STYLE:
	    sp->spEndnoteProperties.npNumberStyle= rcw->rcwEnumValue;
	    break;

	/***/

	default:
	    SDEB(rcw->rcwWord);
	    return 0;
	}

    PROPmaskADD( &(rrc->rrcDocumentStyle.dsSectMask), rcw->rcwID );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save the headers and footers of a Section.				*/
/*									*/
/************************************************************************/

int docRtfSaveSectHeadersFooters(	SimpleOutputStream *	sos,
					int *			pCol,
					const BufferItem *	bi,
					const BufferDocument *	bd,
					RtfWritingContext *	rwc )
    {
    if  ( docRtfSaveExternalItem( sos, pCol, bi->biSectHeader.eiItem,
						bd, "\\header", rwc )	)
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveExternalItem( sos, pCol, bi->biSectFirstPageHeader.eiItem,
						bd, "\\headerf", rwc )	)
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveExternalItem( sos, pCol, bi->biSectLeftPageHeader.eiItem,
						bd, "\\headerl", rwc )	)
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveExternalItem( sos, pCol, bi->biSectRightPageHeader.eiItem,
						bd, "\\headerr", rwc )	)
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveExternalItem( sos, pCol, bi->biSectFooter.eiItem,
						bd, "\\footer", rwc )	)
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveExternalItem( sos, pCol, bi->biSectFirstPageFooter.eiItem,
						bd, "\\footerf", rwc )	)
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveExternalItem( sos, pCol, bi->biSectLeftPageFooter.eiItem,
						bd, "\\footerl", rwc )	)
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveExternalItem( sos, pCol, bi->biSectRightPageFooter.eiItem,
						bd, "\\footerr", rwc )	)
	{ LDEB(1); return -1;	}

    return 0;
    }

