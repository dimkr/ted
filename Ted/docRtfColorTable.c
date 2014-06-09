/************************************************************************/
/*									*/
/*  Read the various document tables of an RTF text file into a		*/
/*  BufferDocument.							*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>
#   include	"docRtf.h"

/************************************************************************/
/*									*/
/*  Read a color table.							*/
/*									*/
/************************************************************************/

static int docRtfSaveColor(	RtfReadingContext *	rrc,
				const unsigned char *	text,
				int			len )
    {
    DocumentProperties *	dp= &(rrc->rrcBd->bdProperties);
    RGB8Color *			fresh;

    fresh= (RGB8Color *)realloc( dp->dpColors,
			    ( dp->dpColorCount+ 1) * sizeof( RGB8Color ) );
    if  ( ! fresh )
	{ LXDEB(dp->dpColorCount,fresh); return -1;	}

    dp->dpColors= fresh;
    fresh[dp->dpColorCount++]= rrc->rrcColor;

    if  ( ! rrc->rrcGotComponent )
	{
	if  ( dp->dpDefaultColor < 0 )
	    { dp->dpDefaultColor= dp->dpColorCount;	}
	}

    rrc->rrcGotComponent= 0;
    return 0;
    }

static int docRtfColorComp(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    switch( rcw->rcwID )
	{
	case	RTFidRED:
	    rrc->rrcGotComponent= 1;
	    rrc->rrcColor.rgb8Red= arg;
	    break;
	case	RTFidGREEN:
	    rrc->rrcGotComponent= 1;
	    rrc->rrcColor.rgb8Green= arg;
	    break;
	case	RTFidBLUE:
	    rrc->rrcGotComponent= 1;
	    rrc->rrcColor.rgb8Blue= arg;
	    break;
	default:
	    /* SLDEB(rcw->rcwWord,arg); */
	    break;
	}

    return 0;
    }

static RtfControlWord	docRtfColorTableWords[]=
    {
	{ "red",	RTFidRED,	DOClevANY, docRtfColorComp, },
	{ "green",	RTFidGREEN,	DOClevANY, docRtfColorComp, },
	{ "blue",	RTFidBLUE,	DOClevANY, docRtfColorComp, },
	{ 0, 0, 0 }
    };

int docRtfColorTable(		SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    rrc->rrcGotComponent= 0;

    rrc->rrcColor.rgb8Red= 255;
    rrc->rrcColor.rgb8Green= 255;
    rrc->rrcColor.rgb8Blue= 255;
    rrc->rrcColor.rgb8Alpha= 255;

    if  ( docRtfReadGroup( sis, rcw->rcwLevel,
				    (RtfControlWord *)0, 0, 0, rrc,
				    docRtfColorTableWords, docRtfEmptyTable,
				    docRtfSaveColor, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

