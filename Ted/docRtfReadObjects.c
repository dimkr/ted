/************************************************************************/
/*									*/
/*  Read an RTF text file into a BufferDocument				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<bitmap.h>

#   include	<appDebugon.h>

#   include	"docRtf.h"


int docRtfObjectProperty(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    InsertedObject *	io= rrc->rrcInsertedObject;

    if  ( rrc->rrcInIgnoredGroup > 0 )
	{ return 0;	}

    if  ( ! io )
	{
	LSXDEB(rrc->rrcCurrentLine,rcw->rcwWord,rrc->rrcInsertedObject);
	return 0;
	}

    switch( rcw->rcwID )
	{
	case IOpropRESULT_KIND:
	    io->ioRtfResultKind= rcw->rcwEnumValue;
	    break;

	case IOpropEMBED_KIND:
	    io->ioRtfEmbedKind= rcw->rcwEnumValue;
	    break;

	case IOpropOBJTWIPS_WIDE:
	    io->ioTwipsWide= arg;
	    break;
	case IOpropOBJTWIPS_HIGH:
	    io->ioTwipsHigh= arg;
	    break;

	case IOpropOBJSCALE_X:
	    io->ioScaleXSet= arg;
	    io->ioScaleXUsed= io->ioScaleXSet;
	    break;
	case IOpropOBJSCALE_Y:
	    io->ioScaleYSet= arg;
	    io->ioScaleYUsed= io->ioScaleYSet;
	    break;

	case IOpropOBJCROP_TOP:
	    io->ioTopCropTwips= arg;
	    break;
	case IOpropOBJCROP_BOTTOM:
	    io->ioBottomCropTwips= arg;
	    break;
	case IOpropOBJCROP_LEFT:
	    io->ioLeftCropTwips= arg;
	    break;
	case IOpropOBJCROP_RIGHT:
	    io->ioRightCropTwips= arg;
	    break;

	case IOprop_UNSUPPORTED:
	    break;

	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID);
	    break;
	}

    return 0;
    }

int docRtfPictureProperty(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    PictureProperties *	pip= &(rrc->rrcPictureProperties);

    if  ( rrc->rrcInIgnoredGroup > 0 )
	{ return 0;	}

    switch( rcw->rcwID )
	{
	case PIPpropTYPE:
	    pip->pipType= rcw->rcwEnumValue;

	    if  ( pip->pipType == DOCokPICTWMETAFILE )
		{ pip->pipMapMode= arg;	}

	    break;

	case PIPpropPICX_WIN_EXT:
	    pip->pip_xWinExt= arg;
	    break;
	case PIPpropPICY_WIN_EXT:
	    pip->pip_yWinExt= arg;
	    break;

	case PIPpropPICTWIPS_WIDE:
	    pip->pipTwipsWide= arg;
	    break;
	case PIPpropPICTWIPS_HIGH:
	    pip->pipTwipsHigh= arg;
	    break;

	case PIPpropPICSCALE_X:
	    pip->pipScaleXSet= arg;
	    pip->pipScaleXUsed= pip->pipScaleXSet;
	    break;
	case PIPpropPICSCALE_Y:
	    pip->pipScaleYSet= arg;
	    pip->pipScaleYUsed= pip->pipScaleYSet;
	    break;

	case PIPpropPICCROP_TOP:
	    pip->pipTopCropTwips= arg;
	    break;
	case PIPpropPICCROP_BOTTOM:
	    pip->pipBottomCropTwips= arg;
	    break;
	case PIPpropPICCROP_LEFT:
	    pip->pipLeftCropTwips= arg;
	    break;
	case PIPpropPICCROP_RIGHT:
	    pip->pipRightCropTwips= arg;
	    break;

	case PIPpropBLIPTAG:
	    pip->pipBliptag= arg;
	    break;

	case PIPpropPICBPP:
	    pip->pipMetafileBitmapBpp= arg;
	    break;
	case PIPpropBLIPUPI:
	    pip->pipBmUnitsPerInch= arg;
	    break;
	case PIPpropWBMBITSPIXEL:
	    pip->pipBmBitsPerPixel= arg;
	    break;
	case PIPpropWBMPLANES:
	    pip->pipBmPlanes= arg;
	    break;
	case PIPpropWBMWIDTHBYTES:
	    pip->pipBmBytesPerRow= arg;
	    break;

	case PIPpropDEFSHP:
	    pip->pipPictureIsWordArt= arg != 0;
	    break;
	case PIPpropPICBMP:
	    pip->pipMetafileIsBitmap= arg != 0;
	    break;
	case PIPpropPICSCALED:
	    pip->pipPictIsScaled= arg != 0;
	    break;

	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID);
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Consume picture property tags.					*/
/*									*/
/*  In particular:							*/
/*	shplid:	The identification of the shape of the picture.		*/
/*	sp:	Various shape properties for the picture.		*/
/*  We make a drawing shape to receive the properties.			*/
/*									*/
/************************************************************************/

static RtfControlWord	docRtfPicpropGroups[]=
    {
	{ "sp",		RTFidSP,	DOClevPARA, docRtfShapeProperty, },
    };

static int docRtfReadPicprop(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    int			rval;
    DrawingShape *	parent= rrc->rrcDrawingShape;
    DrawingShape *	ds;

    ds= malloc( sizeof(DrawingShape) );
    if  ( ! ds )
	{ XDEB(ds); return -1;	}

    docInitDrawingShape( ds );
    ds->dsShapeType= rcw->rcwID;

    rrc->rrcDrawingShape= ds;

    rval= docRtfConsumeGroup( sis, DOClevPARA, rrc,
			(const RtfControlWord *)0, docRtfPicpropGroups,
			docRtfIgnoreText );
    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    rrc->rrcDrawingShape= parent;
    return rval;
    }

static RtfControlWord	docRtfPictGroups[]=
    {
	{ "picprop",	SHPtyPICPROP,	DOClevPARA, docRtfReadPicprop, },
    };

static int docRtfSaveObjectData(	RtfReadingContext *	rrc,
					const unsigned char *	text,
					int			len )
    {
    if  ( rrc->rrcInIgnoredGroup > 0 )
	{ return 0;	}

    if  ( docObjectSetData( rrc->rrcInsertedObject, text, len ) )
	{ LDEB(len); return -1;	}

    return 0;
    }

static int docRtfSaveResultData(	RtfReadingContext *	rrc,
					const unsigned char *	text,
					int			len	)
    {
    if  ( docSetResultData( rrc->rrcInsertedObject, text, len ) )
	{ LDEB(len); return -1;	}

    return 0;
    }

int docRtfReadPict(	SimpleInputStream *	sis,
			const RtfControlWord *	rcw,
			int			arg,
			RtfReadingContext *	rrc )
    {
    RtfReadingState *		externRrs= rrc->rrcState;
    RtfReadingState		internRrs;

    int				rval;
    TextParticule *		tp;

    BufferItem *		bi= rrc->rrcBi;
    int				off= bi->biParaStrlen;
    InsertedObject *		ioSave= rrc->rrcInsertedObject;

    docInitPictureProperties( &(rrc->rrcPictureProperties) );

    /*
    LSLDEB(rrc->rrcCurrentLine,rcw->rcwWord,rrc->rrcInIgnoredGroup);
    */

    if  ( ! rrc->rrcInIgnoredGroup )
	{
	if  ( docInflateTextString( bi, 1 ) )
	    { LLDEB(bi->biParaStrlen,1); return -1; }

	bi->biParaString[off]= ' ';
	bi->biParaStrlen++;
	bi->biParaString[bi->biParaStrlen]= '\0';

	tp= docInsertObject( rrc->rrcBd, bi, &(rrc->rrcInsertedObject),
				    -1, off, &(externRrs->rrsTextAttribute) );
	if  ( ! tp )
	    { LDEB(bi->biParaParticuleCount); return -1;	}
	}

    docRtfPushReadingState( rrc, &internRrs );

    rval= docRtfConsumeGroup( sis, DOClevPARA, rrc,
				(const RtfControlWord *)0, docRtfPictGroups,
				docRtfSaveObjectData );

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    if  ( rrc->rrcInsertedObject )
	{
	InsertedObject *	io= rrc->rrcInsertedObject;
	PictureProperties *	pip= &(rrc->rrcPictureProperties);

	io->ioKind= pip->pipType;
	io->ioPictureProperties= rrc->rrcPictureProperties;

	io->ioTwipsWide= pip->pipTwipsWide;
	io->ioTwipsHigh= pip->pipTwipsHigh;
	io->ioScaleXSet= pip->pipScaleXSet;
	io->ioScaleYSet= pip->pipScaleYSet;
	io->ioScaleXUsed= pip->pipScaleXUsed;
	io->ioScaleYUsed= pip->pipScaleYUsed;
	}

    rrc->rrcInsertedObject= ioSave;
    docRtfPopReadingState( rrc );

    return rval;
    }

static RtfControlWord	docRtfXshppictGroups[]=
    {
	{ "pict",	RTFidPICT,	DOClevPARA, docRtfReadPict, },
    };

int docRtfReadNonshppict(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    int				rval;

    rrc->rrcInIgnoredGroup++;

    rval= docRtfConsumeGroup( sis, DOClevPARA, rrc,
			    (const RtfControlWord *)0, docRtfXshppictGroups,
			    docRtfIgnoreText );

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    rrc->rrcInIgnoredGroup--;

    return rval;
    }

int docRtfReadShppict(		SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    int				rval;

    rval= docRtfConsumeGroup( sis, DOClevPARA, rrc,
			    (const RtfControlWord *)0, docRtfXshppictGroups,
			    docRtfIgnoreText );
    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }

static int docRtfReadResultPict(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    int				rval;

    rval= docRtfConsumeGroup( sis, DOClevPARA, rrc,
			(const RtfControlWord *)0, docRtfPictGroups,
			docRtfSaveResultData );

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    if  ( rrc->rrcInsertedObject )
	{
	InsertedObject *	io= rrc->rrcInsertedObject;
	PictureProperties *	pip= &(rrc->rrcPictureProperties);

	io->ioResultKind= pip->pipType;
	io->ioPictureProperties= rrc->rrcPictureProperties;

	if  ( io->ioTwipsWide < 2 )
	    { io->ioTwipsWide= pip->pipTwipsWide;	}
	if  ( io->ioTwipsHigh < 2 )
	    { io->ioTwipsHigh= pip->pipTwipsHigh;	}
	if  ( io->ioScaleXSet == 100 )
	    {
	    io->ioScaleXSet= pip->pipScaleXSet;
	    io->ioScaleXUsed= pip->pipScaleXUsed;
	    }
	if  ( io->ioScaleYSet == 100 )
	    {
	    io->ioScaleYSet= pip->pipScaleYSet;
	    io->ioScaleYUsed= pip->pipScaleYUsed;
	    }
	}

    return rval;
    }

static RtfControlWord	docRtfNonshpResultpictGroups[]=
    {
	{ "pict",	RTFidPICT,	DOClevPARA, docRtfReadResultPict, },
    };

static int docRtfReadNonshResultpict(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    int				rval;

    rval= docRtfConsumeGroup( sis, DOClevPARA, rrc,
				docRtfEmptyTable, docRtfNonshpResultpictGroups,
				docRtfIgnoreText );

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }

static int docRtfSaveObjectClass(	RtfReadingContext *	rrc,
					const unsigned char *	text,
					int			len	)
    {
    if  ( docSetObjectClass( rrc->rrcInsertedObject, text, len ) )
	{ LDEB(len); return -1;	}

    return 0;
    }

static int docRtfSaveObjectName(	RtfReadingContext *	rrc,
					const unsigned char *	text,
					int			len	)
    {
    if  ( docSetObjectName( rrc->rrcInsertedObject, text, len ) )
	{ LDEB(len); return -1;	}

    return 0;
    }

static int docRtfObjectClass(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int				rval;

    rval= docRtfConsumeGroup( sis, DOClevPARA, rrc,
				docRtfEmptyTable, docRtfEmptyTable,
				docRtfSaveObjectClass );

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }

static int docRtfObjectName(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int				rval;

    rval= docRtfConsumeGroup( sis, DOClevPARA, rrc,
				docRtfEmptyTable, docRtfEmptyTable,
				docRtfSaveObjectName );
    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }

static int docRtfObjectData(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int				rval;

    rval= docRtfConsumeGroup( sis, DOClevPARA, rrc,
				docRtfEmptyTable, docRtfEmptyTable,
				docRtfSaveObjectData );
    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }

static RtfControlWord	docRtfResultGroups[]=
    {
	{ "pict",	RTFidPICT,	DOClevPARA, docRtfReadResultPict, },
	{ "nonshppict",	RTFidNONSHPPICT,DOClevPARA, docRtfReadNonshResultpict, },
	{ 0, 0, 0 }
    };

static int docRtfObjectResult(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int				rval;

    rval= docRtfConsumeGroup( sis, DOClevPARA, rrc,
				(const RtfControlWord *)0, docRtfResultGroups,
				docRtfIgnoreText );
    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }

static RtfControlWord	docRtfObjectGroups[]=
    {
	{ "objname",	RTFidOBJNAME,	DOClevPARA, docRtfObjectName, },
	{ "objclass",	RTFidOBJCLASS,	DOClevPARA, docRtfObjectClass, },
	{ "objdata",	RTFidOBJDATA,	DOClevPARA, docRtfObjectData, },
	{ "result",	RTFidOBJDATA,	DOClevPARA, docRtfObjectResult, },
	{ 0, 0, 0 }
    };

int docRtfReadObject(	SimpleInputStream *	sis,
			const RtfControlWord *	rcw,
			int			arg,
			RtfReadingContext *	rrc )
    {
    RtfReadingState *		rrs= rrc->rrcState;
    int				rval;

    BufferItem *		bi= rrc->rrcBi;
    int				off= bi->biParaStrlen;
    InsertedObject *		ioSave= rrc->rrcInsertedObject;

    docInitPictureProperties( &(rrc->rrcPictureProperties) );

    if  ( docInflateTextString( bi, 1 ) )
	{ LLDEB(bi->biParaStrlen,1); return -1; }

    bi->biParaString[off]= ' ';
    bi->biParaStrlen++;
    bi->biParaString[bi->biParaStrlen]= '\0';

    if  ( ! docInsertObject( rrc->rrcBd, bi, &(rrc->rrcInsertedObject), -1,
					    off, &(rrs->rrsTextAttribute) ) )
	{ LDEB(bi->biParaParticuleCount); return -1;	}

    rrc->rrcInsertedObject->ioKind= DOCokOLEOBJECT;

    rval= docRtfConsumeGroup( sis, DOClevPARA, rrc,
				(const RtfControlWord *)0, docRtfObjectGroups,
				docRtfSaveObjectData );

    rrc->rrcInsertedObject= ioSave;

    if  ( rval )
	{ SLDEB(rcw->rcwWord,rval);	}

    return rval;
    }
