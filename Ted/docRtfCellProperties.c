/************************************************************************/
/*									*/
/*  Read/Write paragraph cell to/from RTF.				*/
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
/*  Save Cell Properties.						*/
/*									*/
/************************************************************************/

void docRtfSaveCellProperties(		const CellProperties *	cp,
					int *			pCol,
					int			shiftLeft,
					SimpleOutputStream *	sos )
    {
    switch( cp->cpTextVerticalAlignment )
	{
	case DOCtvaTOP:
	    /*
	    docRtfWriteTag( "\\clvertalt", pCol, sos );
	    */
	    break;
	    
	case DOCtvaCENTERED:
	    docRtfWriteTag( "\\clvertalc", pCol, sos );
	    break;
	    
	case DOCtvaBOTTOM:
	    docRtfWriteTag( "\\clvertalb", pCol, sos );
	    break;
	    
	default:
	    LDEB(cp->cpTextVerticalAlignment);
	    break;
	}

    if  ( cp->cpLeftInMergedRow )
	{ docRtfWriteTag( "\\clmgf", pCol, sos );	}
    if  ( cp->cpMergedWithLeft )
	{ docRtfWriteTag( "\\clmrg", pCol, sos );	}
    if  ( cp->cpTopInMergedColumn )
	{ docRtfWriteTag( "\\clvmgf", pCol, sos );	}
    if  ( cp->cpMergedWithAbove )
	{ docRtfWriteTag( "\\clvmrg", pCol, sos );	}

    docRtfWriteNextLine( pCol, sos );

    if  ( cp->cpTopBorder.bpStyle != DOCbsNONE )
	{ docRtfSaveBorder( "\\clbrdrt", pCol, &(cp->cpTopBorder), sos ); }
    if  ( cp->cpLeftBorder.bpStyle != DOCbsNONE )
	{ docRtfSaveBorder( "\\clbrdrl", pCol, &(cp->cpLeftBorder), sos ); }
    if  ( cp->cpRightBorder.bpStyle != DOCbsNONE )
	{ docRtfSaveBorder( "\\clbrdrr", pCol, &(cp->cpRightBorder), sos ); }
    if  ( cp->cpBottomBorder.bpStyle != DOCbsNONE )
	{ docRtfSaveBorder( "\\clbrdrb", pCol, &(cp->cpBottomBorder), sos ); }

    /*  NOTE: Word 2003 swaps left and top  */
    docRtfSaveAutoSpace(  pCol, sos,
			    "\\clpadft", cp->cpLeftPaddingUnit,
			    "\\clpadt", cp->cpLeftPadding );  /* !!!  */
    docRtfSaveAutoSpace(  pCol, sos,
			    "\\clpadfl", cp->cpTopPaddingUnit,
			    "\\clpadl", cp->cpTopPadding );  /* !!!  */
    docRtfSaveAutoSpace(  pCol, sos,
			    "\\clpadfr", cp->cpRightPaddingUnit,
			    "\\clpadr", cp->cpRightPadding );
    docRtfSaveAutoSpace(  pCol, sos,
			    "\\clpadfb", cp->cpBottomPaddingUnit,
			    "\\clpadb", cp->cpBottomPadding );

    switch( cp->cpShading.isPattern )
	{
	case DOCspSOLID:
	    break;

	case DOCspBGBDIAG:
	    docRtfWriteTag( "\\clbgbdiag", pCol, sos );
	    break;
	case DOCspBGCROSS:
	    docRtfWriteTag( "\\clbgcross", pCol, sos );
	    break;
	case DOCspBGDCROSS:
	    docRtfWriteTag( "\\clbgdcross", pCol, sos );
	    break;
	case DOCspBGDKBDIAG:
	    docRtfWriteTag( "\\clbgdkbdiag", pCol, sos );
	    break;
	case DOCspBGDKCROSS:
	    docRtfWriteTag( "\\clbgdkcross", pCol, sos );
	    break;
	case DOCspBGDKDCROSS:
	    docRtfWriteTag( "\\clbgdkdcross", pCol, sos );
	    break;
	case DOCspBGDKFDIAG:
	    docRtfWriteTag( "\\clbgdkfdiag", pCol, sos );
	    break;
	case DOCspBGDKHORIZ:
	    docRtfWriteTag( "\\clbgdkhor", pCol, sos );
	    break;
	case DOCspBGDKVERT:
	    docRtfWriteTag( "\\clbgdkvert", pCol, sos );
	    break;
	case DOCspBGFDIAG:
	    docRtfWriteTag( "\\clbgfdiag", pCol, sos );
	    break;
	case DOCspBGHORIZ:
	    docRtfWriteTag( "\\clbghoriz", pCol, sos );
	    break;
	case DOCspBGVERT:
	    docRtfWriteTag( "\\clbgvert", pCol, sos );
	    break;

	default:
	    LDEB(cp->cpShading.isPattern);
	    break;
	}

    if  ( cp->cpShading.isForeColor > 0 )
	{
	docRtfWriteArgTag( "\\clcfpat", pCol, cp->cpShading.isForeColor, sos );
	}
    if  ( cp->cpShading.isBackColor > 0 )
	{
	docRtfWriteArgTag( "\\clcbpat", pCol, cp->cpShading.isBackColor, sos );
	}
    if  ( cp->cpShading.isLevel > 0 )
	{
	docRtfWriteArgTag( "\\clshdng", pCol, cp->cpShading.isLevel, sos );
	}

    /**/
    docRtfSaveAutoSpace( pCol, sos,
			"\\clftsWidth", cp->cpPreferredWidthUnit,
			"\\clwWidth", cp->cpPreferredWidth );

    /**/
    switch( cp->cpTextFlow )
	{
	case CLflowTXLRTB:
	    /*
	    docRtfWriteTag( "\\cltxlrtb", pCol, sos );
	    */
	    break;
	case CLflowTXTBRL:
	    docRtfWriteTag( "\\cltxtbrl", pCol, sos );
	    break;
	case CLflowTXBTLR:
	    docRtfWriteTag( "\\cltxbtlr", pCol, sos );
	    break;
	case CLflowTXLRTBV:
	    docRtfWriteTag( "\\cltxlrtbv", pCol, sos );
	    break;
	case CLflowTXTBRLV:
	    docRtfWriteTag( "\\cltxtbrlv", pCol, sos );
	    break;
	    
	default:
	    LDEB(cp->cpTextFlow);
	    break;
	}

    /*  LAST! */
    docRtfWriteArgTag( "\\cellx", pCol,
				cp->cpRightBoundaryTwips- shiftLeft, sos );

    docRtfWriteNextLine( pCol, sos );
    }

/************************************************************************/
/*									*/
/*  Handle a cell property when reading RTF.				*/
/*									*/
/************************************************************************/

int docRtfRememberCellProperty(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    CellProperties *	cp= &(rrc->rrcCellProperties);

    switch( rcw->rcwID )
	{
	case CLprop_NONE:
	    docCleanCellProperties( cp );
	    docInitCellProperties(  cp );
	    return 0;

	/**/
	case CLpropTOP_BORDER:
	    cp->cpTopBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;
	case CLpropBOTTOM_BORDER:
	    cp->cpBottomBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;
	case CLpropLEFT_BORDER:
	    cp->cpLeftBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;
	case CLpropRIGHT_BORDER:
	    cp->cpRightBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;

	/**/
	case CLpropSHADE_FORE_COLOR:
	    cp->cpShading.isForeColor= arg;
	    break;
	case CLpropSHADE_BACK_COLOR:
	    cp->cpShading.isBackColor= arg;
	    break;
	case CLpropSHADE_LEVEL:
	    cp->cpShading.isLevel= arg;
	    break;
	case CLpropSHADE_PATTERN:
	    cp->cpShading.isPattern= rcw->rcwEnumValue;
	    break;

	/**/
	case CLpropCLW_WIDTH:
	    cp->cpPreferredWidth= arg;
	    break;
	case CLpropCLFTS_WIDTH:
	    cp->cpPreferredWidthUnit= arg;
	    break;

	/**/
	case CLpropTOP_PADDING:
	    cp->cpTopPadding= arg;
	    break;
	case CLpropBOTTOM_PADDING:
	    cp->cpBottomPadding= arg;
	    break;
	case CLpropLEFT_PADDING:
	    cp->cpLeftPadding= arg;
	    break;
	case CLpropRIGHT_PADDING:
	    cp->cpRightPadding= arg;
	    break;

	/**/
	case CLpropTOP_PADDING_UNIT:
	    cp->cpTopPaddingUnit= arg;
	    break;
	case CLpropBOTTOM_PADDING_UNIT:
	    cp->cpBottomPaddingUnit= arg;
	    break;
	case CLpropLEFT_PADDING_UNIT:
	    cp->cpLeftPaddingUnit= arg;
	    break;
	case CLpropRIGHT_PADDING_UNIT:
	    cp->cpRightPaddingUnit= arg;
	    break;

	/**/
	case CLpropLEFT_IN_MERGED:
	    cp->cpLeftInMergedRow= ( arg != 0 );
	    break;
	case CLpropMERGED_WITH_LEFT:
	    cp->cpMergedWithLeft= ( arg != 0 );
	    break;
	case CLpropTOP_IN_MERGED:
	    cp->cpTopInMergedColumn= ( arg != 0 );
	    break;
	case CLpropMERGED_WITH_ABOVE:
	    cp->cpMergedWithAbove= ( arg != 0 );
	    break;
	case CLpropNO_WRAP:
	    cp->cpNoWrap= ( arg != 0 );
	    break;

	case CLpropNO_SHADING:
	    cp->cpNoShading= ( arg != 0 );
	    break;

	case CLpropTEXT_VERT_ALIGN:
	    cp->cpTextVerticalAlignment= rcw->rcwEnumValue;
	    break;

	case CLpropTEXT_FLOW:
	    cp->cpTextFlow= rcw->rcwEnumValue;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    return -1;
	}

    return 0;
    }
