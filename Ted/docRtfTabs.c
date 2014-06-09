/************************************************************************/
/*									*/
/*  Read/Write tab stops from/to rtf.					*/
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
/*  Save a tab stop.							*/
/*									*/
/************************************************************************/

void docRtfSaveTabStopList( 	SimpleOutputStream *		sos,
				int *				pCol,
				const TabStopList *		tsl )
    {
    int			i;
    const TabStop *	ts;

    ts= tsl->tslTabStops;
    for ( i= 0; i < tsl->tslTabStopCount; ts++, i++ )
	{
	if  ( *pCol >= 65 )
	    { docRtfWriteNextLine( pCol, sos );	}

	switch( ts->tsAlignment )
	    {
	    case DOCtaLEFT:
		break;
	    case DOCtaRIGHT:
		docRtfWriteTag( "\\tqr", pCol, sos );
		break;
	    case DOCtaCENTER:
		docRtfWriteTag( "\\tqc", pCol, sos );
		break;
	    case DOCtaDECIMAL:
		docRtfWriteTag( "\\tqdec", pCol, sos );
		break;
	    default:
		LDEB(ts->tsAlignment); break;
	    }

	switch( ts->tsLeader )
	    {
	    case DOCtlNONE:
		break;
	    case DOCtlDOTS:
		docRtfWriteTag( "\\tldot", pCol, sos );
		break;
	    case DOCtlUNDERLINE:
		docRtfWriteTag( "\\tlul", pCol, sos );
		break;
	    case DOCtlHYPH:
		docRtfWriteTag( "\\tlhyph", pCol, sos );
		break;
	    case DOCtlTHICK:
		docRtfWriteTag( "\\tlth", pCol, sos );
		break;
	    case DOCtlEQUAL:
		docRtfWriteTag( "\\tleq", pCol, sos );
		break;
	    default:
		LDEB(ts->tsLeader); break;
	    }

	if  ( ts->tsFromStyleOrList )
	    { docRtfWriteTag( "\\jclisttab", pCol, sos );	}

	docRtfWriteArgTag( "\\tx", pCol, ts->tsTwips, sos );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Handle a tab stop property when reading RTF.			*/
/*									*/
/************************************************************************/

int docRtfRememberTabStopProperty(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    TabStop *	ts= &(rrc->rrcTabStop);

    switch( rcw->rcwID )
	{
	case TABpropX:
	    ts->tsTwips= arg; /* but never used */
	    break;

	case TABpropALIGN:
	    ts->tsAlignment= rcw->rcwEnumValue;
	    break;

	case TABpropLEADER:
	    ts->tsLeader= rcw->rcwEnumValue;
	    break;

	case TABpropFROM_STYLE:
	    ts->tsFromStyleOrList= arg != 0;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    return 0;
	}

    return 0;
    }
