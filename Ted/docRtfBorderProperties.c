/************************************************************************/
/*									*/
/*  Read/Write border properties to/from RTF.				*/
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
/*  Save border definition.						*/
/*									*/
/************************************************************************/

void docRtfSaveBorder(		const char *			tag,
				int *				pCol,
				const BorderProperties *	bp,
				SimpleOutputStream *		sos )
    {
    docRtfWriteTag( tag, pCol, sos );

    switch( bp->bpStyle )
	{
	case DOCbsNONE:
	    docRtfWriteTag( "\\brdrnone", pCol, sos ); break;
	case DOCbsS:
	    docRtfWriteTag( "\\brdrs", pCol, sos ); break;
	case DOCbsTH:
	    docRtfWriteTag( "\\brdrth", pCol, sos ); break;
	case DOCbsSH:
	    docRtfWriteTag( "\\brdrsh", pCol, sos ); break;

	case DOCbsDB:
	    docRtfWriteTag( "\\brdrdb", pCol, sos ); break;
	case DOCbsHAIR:	
	    docRtfWriteTag( "\\brdrhair", pCol, sos ); break;
	case DOCbsDOT:
	    docRtfWriteTag( "\\brdrdot", pCol, sos ); break;
	case DOCbsDASH:
	    docRtfWriteTag( "\\brdrdash", pCol, sos ); break;
	case DOCbsDASHD:
	    docRtfWriteTag( "\\brdrdashd", pCol, sos ); break;
	case DOCbsDASHDD:
	    docRtfWriteTag( "\\brdrdashdd", pCol, sos ); break;
	case DOCbsTRIPLE:
	    docRtfWriteTag( "\\brdrtriple", pCol, sos ); break;

	case DOCbsTNTHSG:
	    docRtfWriteTag( "\\brdrtnthsg", pCol, sos ); break;
	case DOCbsTHTNSG:
	    docRtfWriteTag( "\\brdrthtnsg", pCol, sos ); break;
	case DOCbsTNTHTNSG:
	    docRtfWriteTag( "\\brdrtnthtnsg", pCol, sos ); break;
	case DOCbsTNTHMG:
	    docRtfWriteTag( "\\brdrtnthmg", pCol, sos ); break;
	case DOCbsTHTNMG:
	    docRtfWriteTag( "\\brdrthtnmg", pCol, sos ); break;
	case DOCbsTNTHTNMG:
	    docRtfWriteTag( "\\brdrtnthtnmg", pCol, sos ); break;
	case DOCbsTNTHLG:
	    docRtfWriteTag( "\\brdrtnthlg", pCol, sos ); break;
	case DOCbsTHTNLG:
	    docRtfWriteTag( "\\brdrthtnlg", pCol, sos ); break;
	case DOCbsTNTHTNLG:
	    docRtfWriteTag( "\\brdrtnthtnlg", pCol, sos ); break;

	case DOCbsWAVY:	
	    docRtfWriteTag( "\\brdrwavy", pCol, sos ); break;
	case DOCbsWAVYDB:
	    docRtfWriteTag( "\\brdrwavydb", pCol, sos ); break;
	case DOCbsDASHSM:
	    docRtfWriteTag( "\\brdrdashsm", pCol, sos ); break;
	case DOCbsDASHDOTSTR:
	    docRtfWriteTag( "\\brdrdashdotstr", pCol, sos ); break;
	case DOCbsEMBOSS:
	    docRtfWriteTag( "\\brdremboss", pCol, sos ); break;
	case DOCbsENGRAVE:
	    docRtfWriteTag( "\\brdrengrave", pCol, sos ); break;
	case DOCbsOUTSET:
	    docRtfWriteTag( "\\brdroutset", pCol, sos ); break;

	case DOCbsTBL:
	    docRtfWriteTag( "\\brdrtbl", pCol, sos ); break;
	case DOCbsNIL:
	    docRtfWriteTag( "\\brdrnil", pCol, sos ); break;

	default:
	    LDEB(bp->bpStyle);
	    break;
	}

    if  ( bp->bpArt != 0 )
	{ docRtfWriteArgTag( "\\brdrart", pCol, bp->bpColor, sos );	}

    if  ( bp->bpColor != 0 )
	{ docRtfWriteArgTag( "\\brdrcf", pCol, bp->bpColor, sos );	}

    if  ( bp->bpPenWideTwips != 0 )
	{ docRtfWriteArgTag( "\\brdrw", pCol, bp->bpPenWideTwips, sos ); }

    if  ( bp->bpSpacingTwips != 0 )
	{ docRtfWriteArgTag( "\\brsp", pCol, bp->bpSpacingTwips, sos );	}

    docRtfWriteNextLine( pCol, sos );

    return;
    }

int docRtfBeginBorder(		SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    docInitBorderProperties( &(rrc->rrcBorderProperties) );
    rrc->rrcBorderProperties.bpStyle= DOCbsS;
    return 0;
    }

int docRtfBrdrProperty(		SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    switch( rcw->rcwID )
	{
	case BRDRpropPEN_WIDE:
	    if  ( arg > 255 )
		{ LDEB(arg); arg= 255;	}

	    rrc->rrcBorderProperties.bpPenWideTwips= arg;
	    break;

	case BRDRpropCOLOR:
	    if  ( arg > 255 )
		{ LDEB(arg); arg= 255;	}

	    rrc->rrcBorderProperties.bpColor= arg;
	    break;

	case BRDRpropSPACING:
	    rrc->rrcBorderProperties.bpSpacingTwips= arg;
	    break;

	case BRDRpropSTYLE:
	    rrc->rrcBorderProperties.bpStyle= rcw->rcwEnumValue;
	    break;

	case BRDRpropART:
	    rrc->rrcBorderProperties.bpArt= arg;
	    break;

	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID);
	    break;
	}

    return 0;
    }

