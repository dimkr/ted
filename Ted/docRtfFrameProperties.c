/************************************************************************/
/*									*/
/*  Read/Write text frame properties to/from RTF.			*/
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
/*  Text Frame Properties.						*/
/*									*/
/************************************************************************/

void docRtfSaveParaFrameProperties( SimpleOutputStream *	sos,
				int *				pCol,
				const PropertyMask *		updMask,
				const FramePosition *		fp )
    {
    if  ( PROPmaskISSET( updMask, TFPpropABSW ) )
	{ docRtfWriteArgTag( "\\absw", pCol, fp->fpFrameWidthTwips, sos ); }

    if  ( PROPmaskISSET( updMask, TFPpropABSH ) )
	{ docRtfWriteArgTag( "\\absh", pCol, fp->fpFrameHeightTwips, sos ); }

    if  ( PROPmaskISSET( updMask, TFPpropHORIZONTAL_REF ) )
	{
	switch( fp->fpHorizontalFrameReference )
	    {
	    case HFRphCOL:
		docRtfWriteTag( "\\phcol", pCol, sos );
		break;

	    case HFRphPG:
		docRtfWriteTag( "\\phpg", pCol, sos );
		break;

	    case HFRphMRG:
		docRtfWriteTag( "\\phmrg", pCol, sos );
		break;

	    default:
		LDEB(fp->fpHorizontalFrameReference);
	    }
	}

    if  ( PROPmaskISSET( updMask, TFPpropHORIZONTAL_POS ) )
	{
	if  ( fp->fpFrameXTwips >= 0 )
	    { docRtfWriteArgTag( "\\posx", pCol, fp->fpFrameXTwips, sos ); }
	else{
	    docRtfWriteArgTag( "\\posxneg", pCol, -fp->fpFrameXTwips, sos );
	    }
	}

    if  ( PROPmaskISSET( updMask, TFPpropHORIZONTAL_ALIGN ) )
	{
	switch( fp->fpHorizontalFrameAlignment )
	    {
	    case HFAposXL:
		docRtfWriteTag( "\\posxl", pCol, sos );
		break;

	    case HFAposXR:
		docRtfWriteTag( "\\posxr", pCol, sos );
		break;

	    case HFAposXC:
		docRtfWriteTag( "\\posxc", pCol, sos );
		break;

	    case HFAposXI:
		docRtfWriteTag( "\\posxi", pCol, sos );
		break;

	    case HFAposXO:
		docRtfWriteTag( "\\posxo", pCol, sos );
		break;

	    default:
		LDEB(fp->fpHorizontalFrameAlignment);
	    }
	}

    if  ( PROPmaskISSET( updMask, TFPpropVERTICAL_REF ) )
	{
	switch( fp->fpVerticalFrameReference )
	    {
	    case VFRpvMRG:
		docRtfWriteTag( "\\pvmrg", pCol, sos );
		break;

	    case VFRpvPG:
		docRtfWriteTag( "\\pvpg", pCol, sos );
		break;

	    case VFRpvPARA:
		docRtfWriteTag( "\\pvpara", pCol, sos );
		break;

	    default:
		LDEB(fp->fpVerticalFrameReference);
	    }
	}

    if  ( PROPmaskISSET( updMask, TFPpropVERTICAL_POS ) )
	{
	if  ( fp->fpFrameXTwips >= 0 )
	    { docRtfWriteArgTag( "\\posy", pCol, fp->fpFrameYTwips, sos ); }
	else{
	    docRtfWriteArgTag( "\\posyneg", pCol, -fp->fpFrameYTwips, sos );
	    }
	}

    if  ( PROPmaskISSET( updMask, TFPpropVERTICAL_ALIGN ) )
	{
	switch( fp->fpVerticalFrameAlignment )
	    {
	    case VFAposYIL:
		docRtfWriteTag( "\\posyil", pCol, sos );
		break;
	    case VFAposYIN:
		docRtfWriteTag( "\\posyin", pCol, sos );
		break;
	    case VFAposYOUT:
		docRtfWriteTag( "\\posyout", pCol, sos );
		break;
	    case VFAposYT:
		docRtfWriteTag( "\\posyt", pCol, sos );
		break;
	    case VFAposYC:
		docRtfWriteTag( "\\posyc", pCol, sos );
		break;
	    case VFAposYB:
		docRtfWriteTag( "\\posyb", pCol, sos );
		break;

	    default:
		LDEB(fp->fpVerticalFrameAlignment);
	    }
	}

    if  ( PROPmaskISSET( updMask, TFPpropLOCKED_TO_PARA ) )
	{
	if  ( fp->fpFrameLockedToParagraph )
	    { docRtfWriteTag( "\\abslock", pCol, sos );		 }
	else{
	    docRtfWriteArgTag( "\\abslock", pCol,
					fp->fpFrameLockedToParagraph, sos );
	    }
	}

    if  ( PROPmaskISSET( updMask, TFPpropWRAP_STYLE ) )
	{
	switch( fp->fpFrameWrapStyle )
	    {
	    case FWSwrapWRAP:
		LDEB(fp->fpFrameWrapStyle);
		break;

	    case FWSwrapNOWRAP:
		docRtfWriteTag( "\\nowrap", pCol, sos );
		break;

	    case FWSwrapOVERLAY:
		docRtfWriteTag( "\\overlay", pCol, sos );
		break;

	    default:
		LDEB(fp->fpFrameWrapStyle);
	    }
	}

    if  ( PROPmaskISSET( updMask, TFPpropDFRMTXT ) )
	{
	int	val= fp->fpDistanceFromTextTwipsLeft;

	docRtfWriteArgTag( "\\dxfrtext", pCol, val, sos );
	}

    if  ( PROPmaskISSET( updMask, TFPpropDFRMTXTX ) )
	{
	int	val= fp->fpDistanceFromTextTwipsLeft;

	docRtfWriteArgTag( "\\dfrmtxtx", pCol, val, sos );
	}

    if  ( PROPmaskISSET( updMask, TFPpropDFRMTXTY ) )
	{
	int	val= fp->fpDistanceFromTextTwipsTop;

	docRtfWriteArgTag( "\\dfrmtxty", pCol, val, sos );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Handle a frame property when reading RTF.				*/
/*									*/
/************************************************************************/

static int docRtfRememberFrameProperty(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					FramePosition *		fp )
    {
    switch( rcw->rcwID )
	{
	case TFPpropABSW:
	    fp->fpFrameWidthTwips= arg;
	    break;

	case TFPpropABSH:
	    fp->fpFrameHeightTwips= arg;
	    break;

	case TFPpropHORIZONTAL_REF:
	    fp->fpHorizontalFrameReference= rcw->rcwEnumValue;
	    break;

	case TFPpropHORIZONTAL_POS:
	    fp->fpFrameXTwips= arg;
	    break;

	case TFPpropHORIZONTAL_ALIGN:
	    fp->fpHorizontalFrameAlignment= rcw->rcwEnumValue;
	    break;

	case TFPpropVERTICAL_REF:
	    fp->fpVerticalFrameReference= rcw->rcwEnumValue;
	    break;

	case TFPpropVERTICAL_POS:
	    fp->fpFrameYTwips= arg;
	    break;

	case TFPpropVERTICAL_ALIGN:
	    fp->fpVerticalFrameAlignment= rcw->rcwEnumValue;
	    break;

	case TFPpropLOCKED_TO_PARA:
	    fp->fpFrameLockedToParagraph= arg != 0;
	    break;

	case TFPpropWRAP_STYLE:
	    fp->fpFrameWrapStyle= rcw->rcwEnumValue;
	    break;

	case TFPpropNO_OVERLAP:
	    fp->fpNoOverlap= arg != 0;
	    break;

	case TFPpropDFRMTXT:
	    fp->fpDistanceFromTextTwipsLeft= arg;
	    fp->fpDistanceFromTextTwipsRight= arg;
	    fp->fpDistanceFromTextTwipsTop= arg;
	    fp->fpDistanceFromTextTwipsBottom= arg;
	    fp->fpDistanceFromTextSet |=
			(DISTsetLEFT|DISTsetRIGHT|DISTsetTOP|DISTsetBOTTOM);
	    break;

	case TFPpropDFRMTXTX:
	    fp->fpDistanceFromTextTwipsLeft= arg;
	    fp->fpDistanceFromTextTwipsRight= arg;
	    fp->fpDistanceFromTextSet |= (DISTsetLEFT|DISTsetRIGHT);
	    break;

	case TFPpropDFRMTXTY:
	    fp->fpDistanceFromTextTwipsTop= arg;
	    fp->fpDistanceFromTextTwipsBottom= arg;
	    fp->fpDistanceFromTextSet |= (DISTsetTOP|DISTsetBOTTOM);
	    break;

	case TFPpropDFRMTXTL:
	    fp->fpDistanceFromTextTwipsLeft= arg;
	    fp->fpDistanceFromTextSet |= DISTsetLEFT;
	    break;

	case TFPpropDFRMTXTR:
	    fp->fpDistanceFromTextTwipsRight= arg;
	    fp->fpDistanceFromTextSet |= DISTsetRIGHT;
	    break;

	case TFPpropDFRMTXTT:
	    fp->fpDistanceFromTextTwipsTop= arg;
	    fp->fpDistanceFromTextSet |= DISTsetTOP;
	    break;

	case TFPpropDFRMTXTB:
	    fp->fpDistanceFromTextTwipsBottom= arg;
	    fp->fpDistanceFromTextSet |= DISTsetBOTTOM;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    return 0;
	}

    /*
    PROPmaskADD( &(rrc->rrcStyle.dsParaMask), rcw->rcwID );
    */

    return 0;
    }

/************************************************************************/
/*									*/
/*  Handle a paragraph frame property when reading RTF.			*/
/*									*/
/************************************************************************/

int docRtfRememberParaFrameProperty(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    FramePosition *	fp= &(rrc->rrcParaFramePosition);

    if  ( docRtfRememberFrameProperty( sis, rcw, arg, fp ) )
	{ LSLDEB(rcw->rcwID,rcw->rcwWord,arg); return -1;	}


    /*
    PROPmaskADD( &(rrc->rrcStyle.dsParaMask), rcw->rcwID );
    */

    return 0;
    }

int docRtfRememberRowFrameProperty(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    FramePosition *	fp= &(rrc->rrcRowFramePosition);

    if  ( docRtfRememberFrameProperty( sis, rcw, arg, fp ) )
	{ LSLDEB(rcw->rcwID,rcw->rcwWord,arg); return -1;	}


    /*
    PROPmaskADD( &(rrc->rrcStyle.dsParaMask), rcw->rcwID );
    */

    return 0;
    }
