/************************************************************************/
/*									*/
/*  Exchange between row properties and Rtf.				*/
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
/*  Save row properties to RTF.						*/
/*									*/
/************************************************************************/

void docRtfSaveRowProperties(		const RowProperties *	rp,
					int *			pCol,
					int			col0,
					int			col1,
					SimpleOutputStream *	sos )
    {
    const CellProperties *	cp= rp->rpCells;

    int				col;
    int				shiftLeft= 0;

    docRtfWriteTag( "\\trowd", pCol, sos );

    if  ( rp->rpRowNumber >= 0 )
	{ docRtfWriteArgTag( "\\irow", pCol, rp->rpRowNumber, sos ); }
    if  ( rp->rpRowBandNumber >= 0 )
	{ docRtfWriteArgTag( "\\irowband", pCol, rp->rpRowBandNumber, sos ); }
    if  ( rp->rpRowStyle >= 0 )
	{ docRtfWriteArgTag( "\\ts", pCol, rp->rpRowStyle, sos ); }

    if  ( rp->rpHalfGapWidthTwips != 0 )
	{ docRtfWriteArgTag( "\\trgaph", pCol, rp->rpHalfGapWidthTwips, sos ); }

    if  ( rp->rpLeftIndentTwips != 0 )
	{ docRtfWriteArgTag( "\\trleft", pCol, rp->rpLeftIndentTwips, sos ); }

    if  ( rp->rpHeightTwips != 0 )
	{ docRtfWriteArgTag( "\\trrh", pCol, rp->rpHeightTwips, sos ); }

    if  ( rp->rpIsTableHeader )
	{ docRtfWriteTag( "\\trhdr", pCol, sos ); }
    if  ( rp->rpKeepOnOnePage )
	{ docRtfWriteTag( "\\trkeep", pCol, sos ); }
    if  ( rp->rpKeepWithNext )
	{ docRtfWriteTag( "\\trkeepfollow", pCol, sos ); }
    if  ( rp->rpAutofit )
	{ docRtfWriteArgTag( "\\trautofit", pCol, rp->rpAutofit, sos ); }

    docRtfWriteNextLine( pCol, sos );

    if  ( rp->rpTopBorder.bpStyle != DOCbsNONE )
	{ docRtfSaveBorder( "\\trbrdrt", pCol, &(rp->rpTopBorder), sos ); }
    if  ( rp->rpLeftBorder.bpStyle != DOCbsNONE )
	{ docRtfSaveBorder( "\\trbrdrl", pCol, &(rp->rpLeftBorder), sos ); }
    if  ( rp->rpRightBorder.bpStyle != DOCbsNONE )
	{ docRtfSaveBorder( "\\trbrdrr", pCol, &(rp->rpRightBorder), sos ); }
    if  ( rp->rpBottomBorder.bpStyle != DOCbsNONE )
	{ docRtfSaveBorder( "\\trbrdrb", pCol, &(rp->rpBottomBorder), sos ); }

    if  ( rp->rpVerticalBorder.bpStyle != DOCbsNONE )
	{
	docRtfSaveBorder( "\\trbrdrv", pCol, &(rp->rpVerticalBorder), sos );
	}
    if  ( rp->rpHorizontalBorder.bpStyle != DOCbsNONE )
	{
	docRtfSaveBorder( "\\trbrdrh", pCol, &(rp->rpHorizontalBorder), sos );
	}

    /**/
    switch( rp->rpShading.isPattern )
	{
	case DOCspSOLID:
	    break;

	case DOCspBGBDIAG:
	    docRtfWriteTag( "\\trbgbdiag", pCol, sos );
	    break;
	case DOCspBGCROSS:
	    docRtfWriteTag( "\\trbgcross", pCol, sos );
	    break;
	case DOCspBGDCROSS:
	    docRtfWriteTag( "\\trbgdcross", pCol, sos );
	    break;
	case DOCspBGDKBDIAG:
	    docRtfWriteTag( "\\trbgdkbdiag", pCol, sos );
	    break;
	case DOCspBGDKCROSS:
	    docRtfWriteTag( "\\trbgdkcross", pCol, sos );
	    break;
	case DOCspBGDKDCROSS:
	    docRtfWriteTag( "\\trbgdkdcross", pCol, sos );
	    break;
	case DOCspBGDKFDIAG:
	    docRtfWriteTag( "\\trbgdkfdiag", pCol, sos );
	    break;
	case DOCspBGDKHORIZ:
	    docRtfWriteTag( "\\trbgdkhor", pCol, sos );
	    break;
	case DOCspBGDKVERT:
	    docRtfWriteTag( "\\trbgdkvert", pCol, sos );
	    break;
	case DOCspBGFDIAG:
	    docRtfWriteTag( "\\trbgfdiag", pCol, sos );
	    break;
	case DOCspBGHORIZ:
	    docRtfWriteTag( "\\trbghoriz", pCol, sos );
	    break;
	case DOCspBGVERT:
	    docRtfWriteTag( "\\trbgvert", pCol, sos );
	    break;

	default:
	    LDEB(rp->rpShading.isPattern);
	    break;
	}

    if  ( rp->rpShading.isForeColor > 0 )
	{
	docRtfWriteArgTag( "\\trcfpat", pCol, rp->rpShading.isForeColor, sos );
	}
    if  ( rp->rpShading.isBackColor > 0 )
	{
	docRtfWriteArgTag( "\\trcbpat", pCol, rp->rpShading.isBackColor, sos );
	}
    if  ( rp->rpShading.isLevel > 0 )
	{
	docRtfWriteArgTag( "\\trshdng", pCol, rp->rpShading.isLevel, sos );
	}

    /**/
    docRtfSaveAutoSpace( pCol, sos,
			"\\trftsWidth", rp->rpPreferredWidthUnit,
			"\\trwWidth", rp->rpPreferredWidth );

    /**/
    docRtfSaveAutoSpace( pCol, sos,
			"\\trspdfl", rp->rpLeftDefaultCellSpacingUnit,
			"\\trspdl", rp->rpLeftDefaultCellSpacing );

    docRtfSaveAutoSpace( pCol, sos,
			"\\trspdfr", rp->rpRightDefaultCellSpacingUnit,
			"\\trspdr", rp->rpRightDefaultCellSpacing );

    docRtfSaveAutoSpace( pCol, sos,
			"\\trspdft", rp->rpTopDefaultCellSpacingUnit,
			"\\trspdt", rp->rpTopDefaultCellSpacing );

    docRtfSaveAutoSpace( pCol, sos,
			"\\trspdfb", rp->rpBottomDefaultCellSpacingUnit,
			"\\trspdb", rp->rpBottomDefaultCellSpacing );

    /**/
    docRtfSaveAutoSpace( pCol, sos,
			"\\trpaddfl", rp->rpLeftCellPaddingUnit,
			"\\trpaddl", rp->rpLeftCellPadding );

    docRtfSaveAutoSpace( pCol, sos,
			"\\trpaddfr", rp->rpRightCellPaddingUnit,
			"\\trpaddr", rp->rpRightCellPadding );

    docRtfSaveAutoSpace( pCol, sos,
			"\\trpaddft", rp->rpTopCellPaddingUnit,
			"\\trpaddt", rp->rpTopCellPadding );

    docRtfSaveAutoSpace( pCol, sos,
			"\\trpaddfb", rp->rpBottomCellPaddingUnit,
			"\\trpaddb", rp->rpBottomCellPadding );

    /**/
    docRtfSaveAutoSpace( pCol, sos,
			"\\trftsWidthB", rp->rpCellWidthBeforeUnit,
			"\\trwWidthB", rp->rpCellWidthBefore );

    docRtfSaveAutoSpace( pCol, sos,
			"\\trftsWidthA", rp->rpCellWidthAfterUnit,
			"\\trwWidthA", rp->rpCellWidthAfter );

    /**/
    if  ( col0 > 0 )
	{ shiftLeft= cp[col0-1].cpRightBoundaryTwips; }

    for ( col= 0; col < rp->rpCellCount; cp++, col++ )
	{
	if  ( ( col0 < 0 || col >= col0 )	&&
	      ( col1 < 0 || col <= col1 )	)
	    { docRtfSaveCellProperties( cp, pCol, shiftLeft, sos );	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Handle a row property when reading RTF.				*/
/*									*/
/************************************************************************/

int docRtfRememberRowProperty(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    RowProperties *	rp= &(rrc->rrcRowProperties);

    switch( rcw->rcwID )
	{
	case RPprop_NONE:
	    docCleanRowProperties( rp );
	    docInitRowProperties(  rp );

	    docCleanCellProperties( &(rrc->rrcCellProperties) );
	    docInitCellProperties(  &(rrc->rrcCellProperties) );
	    return 0;

	case RPpropGAP_WIDTH:
	    rp->rpHalfGapWidthTwips= arg;
	    break;

	case RPpropLEFT_INDENT:
	    rp->rpLeftIndentTwips= arg;
	    break;

	case RPpropHEIGHT:
	    rp->rpHeightTwips= arg;
	    break;

	case RPpropTOP_BORDER:
	    rp->rpTopBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;
	case RPpropBOTTOM_BORDER:
	    rp->rpBottomBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;
	case RPpropLEFT_BORDER:
	    rp->rpLeftBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;
	case RPpropRIGHT_BORDER:
	    rp->rpRightBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;
	case RPpropHORIZ_BORDER:
	    rp->rpHorizontalBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;
	case RPpropVERT_BORDER:
	    rp->rpVerticalBorder= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;

	/**/
	case RPpropSHADE_FORE_COLOR:
	    rp->rpShading.isForeColor= arg;
	    break;
	case RPpropSHADE_BACK_COLOR:
	    rp->rpShading.isBackColor= arg;
	    break;
	case RPpropSHADE_LEVEL:
	    rp->rpShading.isLevel= arg;
	    break;
	case RPpropSHADE_PATTERN:
	    rp->rpShading.isPattern= rcw->rcwEnumValue;
	    break;

	/**/
	case RPpropALIGNMENT:
	    rp->rpAlignment= rcw->rcwEnumValue;
	    break;

	case RPpropIS_TABLE_HEADER:
	    rp->rpIsTableHeader= ( arg != 0 );
	    break;
	case RPpropKEEP_ON_ONE_PAGE:
	    rp->rpKeepOnOnePage= ( arg != 0 );
	    break;
	case RPpropKEEP_WITH_NEXT:
	    rp->rpKeepWithNext= ( arg != 0 );
	    break;
	case RPpropAUTOFIT:
	    rp->rpAutofit= ( arg != 0 );
	    break;

	/**/
	case RPpropTRW_WIDTH:
	    rp->rpPreferredWidth= arg;
	    break;
	case RPpropTRFTS_WIDTH:
	    rp->rpPreferredWidthUnit= arg;
	    break;

	/**/
	case RPpropTRSPDL:
	    rp->rpLeftDefaultCellSpacing= arg;
	    break;
	case RPpropTRSPDR:
	    rp->rpRightDefaultCellSpacing= arg;
	    break;
	case RPpropTRSPDT:
	    rp->rpTopDefaultCellSpacing= arg;
	    break;
	case RPpropTRSPDB:
	    rp->rpBottomDefaultCellSpacing= arg;
	    break;

	case RPpropTRSPDFL:
	    rp->rpLeftDefaultCellSpacingUnit= arg;
	    break;
	case RPpropTRSPDFR:
	    rp->rpRightDefaultCellSpacingUnit= arg;
	    break;
	case RPpropTRSPDFT:
	    rp->rpTopDefaultCellSpacingUnit= arg;
	    break;
	case RPpropTRSPDFB:
	    rp->rpBottomDefaultCellSpacingUnit= arg;
	    break;

	/**/
	case RPpropTRPADDL:
	    rp->rpLeftCellPadding= arg;
	    break;
	case RPpropTRPADDR:
	    rp->rpRightCellPadding= arg;
	    break;
	case RPpropTRPADDT:
	    rp->rpTopCellPadding= arg;
	    break;
	case RPpropTRPADDB:
	    rp->rpBottomCellPadding= arg;
	    break;

	case RPpropTRPADDFL:
	    rp->rpLeftCellPaddingUnit= arg;
	    break;
	case RPpropTRPADDFR:
	    rp->rpRightCellPaddingUnit= arg;
	    break;
	case RPpropTRPADDFT:
	    rp->rpTopCellPaddingUnit= arg;
	    break;
	case RPpropTRPADDFB:
	    rp->rpBottomCellPaddingUnit= arg;
	    break;

	/**/
	case RPpropTRW_WIDTHB:
	    rp->rpCellWidthBefore= arg;
	    break;
	case RPpropTRW_WIDTHA:
	    rp->rpCellWidthAfter= arg;
	    break;

	case RPpropTRFTS_WIDTHB:
	    rp->rpCellWidthBeforeUnit= arg;
	    break;
	case RPpropTRFTS_WIDTHA:
	    rp->rpCellWidthAfterUnit= arg;
	    break;

	case RPpropAUTOFORMAT_BORDERS:
	    rp->rpAutoformatBorders= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_SHADING:
	    rp->rpAutoformatShading= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_FONT:
	    rp->rpAutoformatFont= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_COLOR:
	    rp->rpAutoformatColor= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_APPLY_BEST_FIT:
	    rp->rpAutoformatApplyBestFit= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_FIRST_ROW:
	    rp->rpAutoformatFirstRow= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_LAST_ROW:
	    rp->rpAutoformatLastRow= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_FIRST_COLUMN:
	    rp->rpAutoformatFirstColumn= ( arg != 0 );
	    break;
	case RPpropAUTOFORMAT_LAST_COLUMN:
	    rp->rpAutoformatLastColumn= ( arg != 0 );
	    break;

	case RPpropROW_NUMBER:
	    rp->rpRowNumber= arg;
	    break;
	case RPpropROW_BAND_NUMBER:
	    rp->rpRowBandNumber= arg;
	    break;
	case RPpropROW_STYLE:
	    rp->rpRowStyle= arg;
	    break;

	case RPpropIS_LAST_ROW:
	    rp->rpIsLastRow= ( arg != 0 );
	    break;

	case RPpropTRAUTH:
	    rp->rpAuthor= arg;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    return -1;
	}

    return 0;
    }

