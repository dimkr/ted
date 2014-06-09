/************************************************************************/
/*									*/
/*  Read/Write paragraph properties to/from RTF.			*/
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
/*  Save Paragraph Properties.						*/
/*									*/
/************************************************************************/

void docRtfSaveParagraphProperties( SimpleOutputStream *	sos,
				int *				pCol,
				const PropertyMask *		updMask,
				const ParagraphProperties *	pp )
    {
    if  ( PROPmaskISSET( updMask, PPpropSTYLE ) )
	{ docRtfWriteArgTag( "\\s", pCol, pp->ppStyle, sos ); }

    if  ( PROPmaskISSET( updMask, PPpropLISTOVERRIDE ) )
	{ docRtfWriteArgTag( "\\ls", pCol, pp->ppListOverride, sos ); }
    if  ( PROPmaskISSET( updMask, PPpropLISTLEVEL ) )
	{ docRtfWriteArgTag( "\\ilvl", pCol, pp->ppListLevel, sos ); }

    if  ( PROPmaskISSET( updMask, PPpropHYPHPAR ) )
	{
	if  ( pp->ppHyphenateParagraph )
	    { docRtfWriteTag( "\\hyphpar", pCol, sos );		 }
	else{
	    docRtfWriteArgTag( "\\hyphpar", pCol,
					pp->ppHyphenateParagraph, sos );
	    }
	}

    if  ( PROPmaskISSET( updMask, PPpropNEWPAGE ) )
	{
	if  ( pp->ppStartsOnNewPage )
	    { docRtfWriteTag( "\\pagebb", pCol, sos );			 }
	else{
	    docRtfWriteArgTag( "\\pagebb", pCol,
					    pp->ppStartsOnNewPage, sos );
	    }
	}

    if  ( PROPmaskISSET( updMask, PPpropWIDCTLPAR ) )
	{
	if  ( pp->ppWidowControl )
	    { docRtfWriteTag( "\\widctlpar", pCol, sos );	}
	else{ docRtfWriteTag( "\\nowidctlpar", pCol, sos );	}
	}

    if  ( PROPmaskISSET( updMask, PPpropKEEP ) )
	{
	if  ( pp->ppKeepOnPage )
	    { docRtfWriteTag( "\\keep", pCol, sos );		 }
	else{
	    docRtfWriteArgTag( "\\keep", pCol,
					pp->ppKeepOnPage, sos );
	    }
	}

    if  ( PROPmaskISSET( updMask, PPpropKEEPN ) )
	{
	if  ( pp->ppKeepWithNext )
	    { docRtfWriteTag( "\\keepn", pCol, sos );		 }
	else{
	    docRtfWriteArgTag( "\\keepn", pCol,
					pp->ppKeepWithNext, sos );
	    }
	}

    if  ( PROPmaskISSET( updMask, PPpropOUTLINELEVEL ) )
	{
	docRtfWriteArgTag( "\\outlinelevel", pCol, pp->ppOutlineLevel, sos );
	}

    if  ( PROPmaskISSET( updMask, PPpropFIRST_INDENT ) )
	{ docRtfWriteArgTag( "\\fi", pCol, pp->ppFirstIndentTwips, sos ); }

    if  ( PROPmaskISSET( updMask, PPpropLEFT_INDENT ) )
	{ docRtfWriteArgTag( "\\li", pCol, pp->ppLeftIndentTwips, sos ); }

    if  ( PROPmaskISSET( updMask, PPpropRIGHT_INDENT ) )
	{ docRtfWriteArgTag( "\\ri", pCol, pp->ppRightIndentTwips, sos ); }

    if  ( PROPmaskISSET( updMask, PPpropSPACE_BEFORE ) )
	{ docRtfWriteArgTag( "\\sb", pCol, pp->ppSpaceBeforeTwips, sos ); }

    if  ( PROPmaskISSET( updMask, PPpropSPACE_AFTER ) )
	{ docRtfWriteArgTag( "\\sa", pCol, pp->ppSpaceAfterTwips, sos ); }

    if  ( PROPmaskISSET( updMask, PPpropLINE_SPACING_DIST ) ||
	  PROPmaskISSET( updMask, PPpropLINE_SPACING_MULT ) )
	{
	docRtfWriteArgTag( "\\sl",
				pCol, pp->ppLineSpacingTwips, sos );
	docRtfWriteArgTag( "\\slmult",
				pCol, pp->ppLineSpacingIsMultiple, sos );
	}

    if  ( PROPmaskISSET( updMask, PPpropALIGNMENT ) )
	{
	switch( pp->ppAlignment )
	    {
	    case DOCiaLEFT:	docRtfWriteTag( "\\ql", pCol, sos ); break;
	    case DOCiaRIGHT:	docRtfWriteTag( "\\qr", pCol, sos ); break;
	    case DOCiaCENTERED:	docRtfWriteTag( "\\qc", pCol, sos ); break;
	    case DOCiaJUSTIFIED:docRtfWriteTag( "\\qj", pCol, sos ); break;
	    default:
		LDEB(pp->ppAlignment); break;
	    }
	}

    if  ( PROPmaskISSET( updMask, PPpropTAB_STOPS ) )
	{ docRtfSaveTabStopList( sos, pCol, &(pp->ppTabStopList) ); }

    if  ( PROPmaskISSET( updMask, PPpropTOP_BORDER ) )
	{ docRtfSaveBorder( "\\brdrt", pCol, &(pp->ppTopBorder), sos ); }

    if  ( PROPmaskISSET( updMask, PPpropBOTTOM_BORDER ) )
	{ docRtfSaveBorder( "\\brdrb", pCol, &(pp->ppBottomBorder), sos ); }

    if  ( PROPmaskISSET( updMask, PPpropLEFT_BORDER ) )
	{ docRtfSaveBorder( "\\brdrl", pCol, &(pp->ppLeftBorder), sos ); }

    if  ( PROPmaskISSET( updMask, PPpropRIGHT_BORDER ) )
	{ docRtfSaveBorder( "\\brdrr", pCol, &(pp->ppRightBorder), sos ); }

    if  ( PROPmaskISSET( updMask, PPpropBETWEEN_BORDER ) )
	{ docRtfSaveBorder( "\\brdrbtw", pCol, &(pp->ppBetweenBorder), sos ); }

    if  ( PROPmaskISSET( updMask, PPpropBAR_BORDER ) )
	{ docRtfSaveBorder( "\\brdrbar", pCol, &(pp->ppBar), sos ); }

    if  ( PROPmaskISSET( updMask, PPpropSHADE_PATTERN ) )
	{
	switch( pp->ppShading.isPattern )
	    {
	    case DOCspSOLID:
		break;

	    case DOCspBGBDIAG:
		docRtfWriteTag( "\\bgbdiag", pCol, sos );
		break;
	    case DOCspBGCROSS:
		docRtfWriteTag( "\\bgcross", pCol, sos );
		break;
	    case DOCspBGDCROSS:
		docRtfWriteTag( "\\bgdcross", pCol, sos );
		break;
	    case DOCspBGDKBDIAG:
		docRtfWriteTag( "\\bgdkbdiag", pCol, sos );
		break;
	    case DOCspBGDKCROSS:
		docRtfWriteTag( "\\bgdkcross", pCol, sos );
		break;
	    case DOCspBGDKDCROSS:
		docRtfWriteTag( "\\bgdkdcross", pCol, sos );
		break;
	    case DOCspBGDKFDIAG:
		docRtfWriteTag( "\\bgdkfdiag", pCol, sos );
		break;
	    case DOCspBGDKHORIZ:
		docRtfWriteTag( "\\bgdkhoriz", pCol, sos );
		break;
	    case DOCspBGDKVERT:
		docRtfWriteTag( "\\bgdkvert", pCol, sos );
		break;
	    case DOCspBGFDIAG:
		docRtfWriteTag( "\\bgfdiag", pCol, sos );
		break;
	    case DOCspBGHORIZ:
		docRtfWriteTag( "\\bghoriz", pCol, sos );
		break;
	    case DOCspBGVERT:
		docRtfWriteTag( "\\bgvert", pCol, sos );
		break;

	    default:
		LDEB(pp->ppShading.isPattern); break;
	    }
	}

    if  ( PROPmaskISSET( updMask, PPpropSHADE_LEVEL ) )
	{
	docRtfWriteArgTag( "\\shading", pCol, pp->ppShading.isLevel, sos );
	}
    if  ( PROPmaskISSET( updMask, PPpropSHADE_FORE_COLOR ) )
	{
	docRtfWriteArgTag( "\\cfpat", pCol, pp->ppShading.isForeColor, sos );
	}
    if  ( PROPmaskISSET( updMask, PPpropSHADE_BACK_COLOR ) )
	{
	docRtfWriteArgTag( "\\cbpat", pCol, pp->ppShading.isBackColor, sos );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Handle a paragraph property when reading RTF.			*/
/*									*/
/************************************************************************/

int docRtfRememberParagraphProperty(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    RtfReadingState *		rrs= rrc->rrcState;
    ParagraphProperties *	pp= &(rrs->rrsParagraphProperties);

    switch( rcw->rcwID )
	{
	case PPprop_NONE:
	    {
	    int		sav= pp->ppStyle;

	    docCleanParagraphProperties( pp );
	    docInitParagraphProperties( pp );

	    if  ( rrc->rrcDocumentStyle.dsLevel == DOClevPARA )
		{ pp->ppStyle= sav; }
	    }
	    return 0;

	case PPpropSTYLE:
	    rrc->rrcDocumentStyle.dsLevel= DOClevPARA;
	    pp->ppStyle= arg;
	    break;

	case PPpropLISTOVERRIDE:
	    rrc->rrcListOverride.loIndex= arg;
	    pp->ppListOverride= arg;
	    break;

	case PPpropIN_TABLE:
	    pp->ppInTable= arg != 0;
	    if  ( rrc->rrcBd )
		{ rrc->rrcBd->bdProperties.dpContainsTables= 1;	}
	    else{ XDEB(rrc->rrcBd);				}
	    break;

	case PPpropLEFT_INDENT:
	    pp->ppLeftIndentTwips= arg;
	    break;

	case PPpropFIRST_INDENT:
	    pp->ppFirstIndentTwips= arg;
	    break;

	case PPpropRIGHT_INDENT:
	    pp->ppRightIndentTwips= arg;
	    break;

	case PPpropALIGNMENT:
	    pp->ppAlignment= rcw->rcwEnumValue;
	    break;

	case PPpropTAB_STOPS:
	    SDEB(rcw->rcwWord);
	    return 0;

	case PPpropNEWPAGE:
	    pp->ppStartsOnNewPage= arg != 0;
	    break;

	case PPpropWIDCTLPAR:
	    pp->ppWidowControl= rcw->rcwEnumValue;
	    break;

	case PPpropKEEP:
	    pp->ppKeepOnPage= arg != 0;
	    break;

	case PPpropKEEPN:
	    pp->ppKeepWithNext= arg != 0;
	    break;

	case PPpropSPACE_BEFORE:
	    pp->ppSpaceBeforeTwips= arg;
	    break;

	case PPpropSPACE_AFTER:
	    pp->ppSpaceAfterTwips= arg;
	    break;

	case PPpropLINE_SPACING_DIST:
	    pp->ppLineSpacingTwips= arg;
	    break;

	case PPpropLINE_SPACING_MULT:
	    pp->ppLineSpacingIsMultiple= ( arg != 0 );
	    break;

	case PPpropTOP_BORDER:
	    pp->ppTopBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;

	case PPpropBOTTOM_BORDER:
	    pp->ppBottomBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;

	case PPpropLEFT_BORDER:
	    pp->ppLeftBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;

	case PPpropRIGHT_BORDER:
	    pp->ppRightBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;

	case PPpropBOX_BORDER:
	    pp->ppTopBorder= rrc->rrcBorderProperties;
	    pp->ppBottomBorder= rrc->rrcBorderProperties;
	    pp->ppLeftBorder= rrc->rrcBorderProperties;
	    pp->ppRightBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;

	case PPpropBETWEEN_BORDER:
	    pp->ppBetweenBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;

	case PPpropBAR_BORDER:
	    pp->ppBar= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;

	case PPpropOUTLINELEVEL:
	    if  ( arg >= 0 && arg < 10 )
		{ pp->ppOutlineLevel= arg;	}
	    else{ pp->ppOutlineLevel= 9;	}
	    break;

	case PPpropLISTLEVEL:
	    if  ( arg > 0 && arg < 10 )
		{ pp->ppListLevel= arg;	}
	    else{ pp->ppListLevel= 0;	}
	    break;

	case PPpropSHADE_LEVEL:
	    pp->ppShading.isLevel= arg;
	    break;

	case PPpropSHADE_PATTERN:
	    pp->ppShading.isPattern= rcw->rcwEnumValue;
	    break;

	case PPpropSHADE_FORE_COLOR:
	    pp->ppShading.isForeColor= arg;
	    break;

	case PPpropSHADE_BACK_COLOR:
	    pp->ppShading.isBackColor= arg;
	    break;

	case PPpropHYPHPAR:
	    pp->ppHyphenateParagraph= arg != 0;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    return 0;
	}

    PROPmaskADD( &(rrc->rrcDocumentStyle.dsParaMask), rcw->rcwID );

    return 0;
    }
