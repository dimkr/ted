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

#   include	<appDebugon.h>

#   include	<psFont.h>
#   include	"docRtf.h"

/************************************************************************/
/*									*/
/*  Forward declarations.						*/
/*									*/
/************************************************************************/

static int docRtfReadPntext(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc );
static int docRtfSkipPn(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc );
static int docRtfReadUpr(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc );
static int docRtfReadPnseclvl(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc );

/************************************************************************/
/*									*/
/*  Special actions for section properties.				*/
/*									*/
/************************************************************************/

int docRtfTextSpecialChar(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    switch( rcw->rcwID )
	{
	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID);
	    break;
	}

    return 0;
    }

int docRtfTextSpecialParticule(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;

    if  ( rrc->rrcInIgnoredGroup > 0 )
	{ return 0;	}

    switch( rcw->rcwID )
	{
	case DOCkindTAB:
	    if  ( docSaveSpecialParticule( rrc->rrcBd, rrc->rrcBi, 0,
				&(rrs->rrsTextAttribute), DOCkindTAB ) )
		{ LDEB(1); return -1;	}

	    rrc->rrcAfterNoteref= 0;

	    break;

	case DOCkindLINEBREAK:
	    if  ( docSaveSpecialParticule( rrc->rrcBd, rrc->rrcBi, 0,
				&(rrs->rrsTextAttribute), DOCkindLINEBREAK ) )
		{ LDEB(1); return -1;	}

	    rrc->rrcAfterNoteref= 0;

	    break;

	case DOCkindPAGEBREAK:
	case DOCkindCOLUMNBREAK:
	    {
	    int	atBegin= 0;

	    if  ( rrc->rrcJustAfterPntext )
		{ atBegin= 1; }

	    if  ( docSaveSpecialParticule( rrc->rrcBd, rrc->rrcBi, atBegin,
				&(rrs->rrsTextAttribute), rcw->rcwID ) )
		{ LDEB(1); return -1;	}

	    rrc->rrcAfterNoteref= 0;

	    break;
	    }

	case DOCkindCHFTNSEP:
	    if  ( docSaveSpecialParticule( rrc->rrcBd, rrc->rrcBi, 0,
				&(rrs->rrsTextAttribute), DOCkindCHFTNSEP ) )
		{ LDEB(1); return -1;	}

	    rrc->rrcAfterNoteref= 0;
	    break;

	case DOCkindCHFTNSEPC:
	    if  ( docSaveSpecialParticule( rrc->rrcBd, rrc->rrcBi, 0,
				&(rrs->rrsTextAttribute), DOCkindCHFTNSEPC ) )
		{ LDEB(1); return -1;	}

	    rrc->rrcAfterNoteref= 0;
	    break;

	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID);
	    break;
	}

    return 0;
    }

static int docRtfHierarchy(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    switch( rcw->rcwID )
	{
	case RTFidSECT:
	    if  ( rrc->rrcSplitLevel < DOClevSECT )
		{
		if  ( docRtfAdjustLevel( rrc, DOClevPARA, DOClevTEXT ) )
		    { SDEB(rcw->rcwWord); return -1; }
		}

	    if  ( docRtfAdjustLevel( rrc, DOClevSECT- 1, DOClevTEXT ) )
		{ SDEB(rcw->rcwWord); return -1; }
	    rrc->rrcSplitLevel= DOClevSECT- 1;
	    break;

	case RTFidROW:
	    if  ( rrc->rrcSplitLevel < DOClevROW )
		{
		if  ( docRtfAdjustLevel( rrc, DOClevPARA, DOClevTEXT ) )
		    { SDEB(rcw->rcwWord); return -1; }
		}

	    if  ( docRtfAdjustLevel( rrc, DOClevROW- 1, DOClevTEXT ) )
		{ SDEB(rcw->rcwWord); return -1; }
	    rrc->rrcSplitLevel= DOClevROW- 1;
	    break;

	case RTFidCELL:
	    if  ( rrc->rrcSplitLevel <= DOClevCELL )
		{
		if  ( docRtfAdjustLevel( rrc, DOClevPARA, DOClevTEXT ) )
		    { SDEB(rcw->rcwWord); return -1; }
		}

	    if  ( docRtfAdjustLevel( rrc, DOClevCELL- 1, DOClevTEXT ) )
		{ SDEB(rcw->rcwWord); return -1; }
	    rrc->rrcSplitLevel= DOClevCELL- 1;

	    break;

	case RTFidPAR:
	    if  ( docRtfAdjustLevel( rrc, DOClevPARA, DOClevTEXT ) )
		{ SDEB(rcw->rcwWord); return -1; }

	    if  ( docRtfAdjustLevel( rrc, DOClevPARA- 1, DOClevTEXT ) )
		{ SDEB(rcw->rcwWord); return -1; }
	    break;

	case RTFidNONESTTABLES:
	    /* should be a destination */
	    break;

	case RTFidNESTCELL:
	    break;

	default:
	    SLDEB(rcw->rcwWord,rcw->rcwID); break;
	}

    return 0;
    }

RtfControlWord	docRtfDocumentWords[]=
    {
	{ "fc",		RTFidFC,	DOClevANY,  docRtfRememberProperty, },
				/****************************************/
				/*  Hierarchy/Structure tags.		*/
				/****************************************/
	{ "par",	RTFidPAR,	DOClevANY,  docRtfHierarchy, },
	{ "cell",	RTFidCELL,	DOClevANY,  docRtfHierarchy, },
	{ "row",	RTFidROW,	DOClevANY,  docRtfHierarchy, },
	{ "sect",	RTFidSECT,	DOClevANY,  docRtfHierarchy, },

	{ "nestcell",	RTFidNESTCELL,	DOClevANY,  docRtfHierarchy, },

	{ "nonesttables",
			RTFidNONESTTABLES,
					DOClevANY,  docRtfHierarchy, },

	{ 0, 0, 0 }
    };

RtfControlWord	docRtfDocumentGroups[]=
    {
	{ "upr",	RTFidUPR,	DOClevANY,  docRtfReadUpr, },

	{ "fonttbl",	RTFidFONTTBL,	DOClevDOC,  docRtfFontTable,  },
	{ "colortbl",	RTFidCOLORTBL,	DOClevDOC,  docRtfColorTable, },
	{ "stylesheet",	RTFidSTYLESHEET,DOClevDOC,  docRtfStylesheet,  },
	{ "listtable",	DPpropLISTTABLE, DOClevANY, docRtfListTable,  },
	{ "listoverridetable",
			DPpropLISTOVERRIDETABLE,
					DOClevANY,  docRtfListOverrideTable, },
	{ "generator",	DPpropGENERATOR, DOClevDOC, docRtfDocInfoGroup,	},
	{ "info",	RTFidINFO,	DOClevDOC,  docRtfReadInfo,  },
	{ "revtbl",	RTFidREVTBL,	DOClevDOC,  docRtfRevisionTable,  },
	{ "pn",		RTFidPN,	DOClevANY,  docRtfSkipPn,   },
	{ "pntext",	DOCfkLISTTEXT,	DOClevPARA, docRtfReadPntext, },
	{ "pict",	RTFidPICT,	DOClevPARA, docRtfReadPict, },
	{ "do",		RTFidDO,	DOClevPARA, docRtfReadDrawingObject, },
	{ "nonshppict",	RTFidNONSHPPICT,DOClevPARA, docRtfReadNonshppict, },
	{ "shppict",	RTFidSHPPICT,	DOClevPARA, docRtfReadShppict, },
	{ "object",	RTFidOBJECT,	DOClevPARA, docRtfReadObject, },
	{ "field",	RTFidFIELD,	DOClevPARA, docRtfReadField, },
	{ "shp",	SHPtyUNKNOWN,	DOClevTEXT, docRtfReadShape, },
	{ "shpgrp",	SHPtyGROUP,	DOClevTEXT, docRtfReadShape, },

	{ "xe",		DOCfkXE,	DOClevPARA, docRtfLookupEntry, },
	{ "tc",		DOCfkTC,	DOClevPARA, docRtfLookupEntry, },
	{ "tcn",	DOCfkTCN,	DOClevPARA, docRtfLookupEntry, },

				/****************************************/
				/*  Headers and Footers.		*/
				/****************************************/
	{ "footer",	DOCinSECT_FOOTER, DOClevSECT, docRtfReadExtIt, },
	{ "footerf",	DOCinFIRST_FOOTER, DOClevSECT, docRtfReadExtIt, },
	{ "footerl",	DOCinLEFT_FOOTER, DOClevSECT, docRtfReadExtIt, },
	{ "footerr",	DOCinRIGHT_FOOTER, DOClevSECT, docRtfReadExtIt, },

	{ "header",	DOCinSECT_HEADER, DOClevSECT, docRtfReadExtIt, },
	{ "headerf",	DOCinFIRST_HEADER, DOClevSECT, docRtfReadExtIt, },
	{ "headerl",	DOCinLEFT_HEADER, DOClevSECT, docRtfReadExtIt, },
	{ "headerr",	DOCinRIGHT_HEADER, DOClevSECT, docRtfReadExtIt, },

	{ "ftnsep",	DOCinFTNSEP,	DOClevDOC, docRtfReadExtIt, },
	{ "ftnsepc",	DOCinFTNSEPC,	DOClevDOC, docRtfReadExtIt, },
	{ "ftncn",	DOCinFTNCN,	DOClevDOC, docRtfReadExtIt, },

	{ "aftnsep",	DOCinAFTNSEP,	DOClevDOC, docRtfReadExtIt, },
	{ "aftnsepc",	DOCinAFTNSEPC,	DOClevDOC, docRtfReadExtIt, },
	{ "aftncn",	DOCinAFTNCN,	DOClevDOC, docRtfReadExtIt, },

	{ "footnote",	DOCinFOOTNOTE,	DOClevPARA, docRtfReadFootnote, },

				/****************************************/
				/*  Multilevel Lists.			*/
				/****************************************/
	{ "pnseclvl",	RTFidPNSECLVL,	DOClevANY, docRtfReadPnseclvl,	},
				/****************************************/
				/*  Lists.				*/
				/****************************************/
	{ "listtext",	DOCfkLISTTEXT,	DOClevPARA, docRtfReadTextField, },
				/****************************************/
				/*  Annotation.				*/
				/****************************************/
	{ "chatn",	RTFidCHATN,	DOClevPARA, docRtfSkipGroup, },
				/****************************************/
				/*  Bookmarks				*/
				/****************************************/
	{ "bkmkstart",	RTFidBKMKSTART,	DOClevPARA, docRtfBkmkStart,	},
	{ "bkmkend",	RTFidBKMKEND,	DOClevPARA, docRtfBkmkEnd,	},
				/****************************************/
				/*  To deal with faulty documents.	*/
				/****************************************/
	{ "sp",		RTFidSP,	DOClevPARA, docRtfShapeProperty, },

	{ 0, 0, 0 }
    };

/************************************************************************/
/*									*/
/*  Read a document as a whole.						*/
/*									*/
/************************************************************************/

int docRtfTextParticule(	RtfReadingContext *	rrc,
				const unsigned char *	text,
				int			len )
    {
    RtfReadingState *		rrs= rrc->rrcState;

    const BufferDocument *	bd= rrc->rrcBd;
    const DocumentProperties *	dp= &(bd->bdProperties);
    const DocumentFontList *	dfl= &(dp->dpFontList);

    const unsigned char *	inputMapping= rrc->rrcDefaultInputMapping;

    if  ( rrc->rrcInDeletedText )
	{ return 0;	}

    if  ( rrc->rrcJustAfterPntext )
	{
	PropertyMask		ppChgMask;
	PropertyMask		ppUpdMask;

	const int * const	colorMap= (const int *)0;
	const int * const	listStyleMap= (const int *)0;

	PROPmaskCLEAR( &ppChgMask );

	PROPmaskCLEAR( &ppUpdMask );
	utilPropMaskFill( &ppUpdMask, PPprop_COUNT );

	if  ( docUpdParaProperties( &ppChgMask, &(rrc->rrcBi->biParaProperties),
				&ppUpdMask, &(rrs->rrsParagraphProperties),
				colorMap, listStyleMap ) )
	    { LDEB(1); return -1;	}

	rrc->rrcJustAfterPntext= 0;
	}

    if  ( rrs->rrsTextAttribute.taFontNumber >= 0			&&
	  rrs->rrsTextAttribute.taFontNumber < dfl->dflFontCount	)
	{
	const DocumentFont *		df;
	const OfficeCharsetMapping *	ocm;

	df= dfl->dflFonts+ rrs->rrsTextAttribute.taFontNumber;
	ocm= df->dfOfficeCharsetMapping;

	inputMapping= rrc->rrcDefaultInputMapping;

	if  ( ! ocm )
	    { inputMapping= (const unsigned char *)0; }
	else{
	    if  ( ocm->ocmFromOfficeToX11 )
		{ inputMapping= ocm->ocmFromOfficeToX11;	}
	    }
	}

    if  ( docSaveParticules( rrc->rrcBd, rrc->rrcBi, &(rrs->rrsTextAttribute),
						    inputMapping, text, len ) )
	{ LDEB(1); return -1;	}

    rrc->rrcAfterNoteref= 0;
    
    return 0;
    }

static int docRtfReadDoc(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    int		res;

    res= docRtfReadGroup( sis, DOClevPARA,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfDocumentWords, docRtfDocumentGroups,
				docRtfTextParticule, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

static int docRtfReadUpr(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    int		res;

    res= docRtfReadGroup( sis, DOClevPARA,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfDocumentWords, docRtfDocumentGroups,
				docRtfTextParticule, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

/************************************************************************/
/*									*/
/*  Consume the description of a Paragraph Numbering level for a	*/
/*  section.								*/
/*									*/
/************************************************************************/

static int docRtfPndescText(	RtfReadingContext *	rrc,
				const unsigned char *	text,
				int			len )
    {
    unsigned char *	target;
    int			olen;

    switch( rrc->rrcWhichPnText )
	{
	case RTFidPNTXTA:
	    target= rrc->rrcParagraphNumber.pnTextAfter;
	    break;

	case RTFidPNTXTB:
	    target= rrc->rrcParagraphNumber.pnTextBefore;
	    break;

	default:
	    LDEB(rrc->rrcWhichPnText); return -1;
	}

    olen= strlen( (char *)target );
    if  ( olen+ len >= 32 )
	{ SLDEB((char *)target,len); return -1;	}

    memcpy( target+ olen, text, len );
    target[olen+ len]= '\0';

    return 0;
    }

static int docRtfPndescGroup(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int			res;

    rrc->rrcWhichPnText= rcw->rcwID;

    res= docRtfReadGroup( sis, DOClevANY,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfDocumentWords, docRtfDocumentGroups,
				docRtfPndescText, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    rrc->rrcWhichPnText= -1;

    return res;
    }


RtfControlWord	docRtfPndescGroups[]=
    {
	{ "pntxta",	RTFidPNTXTA,	DOClevANY, docRtfPndescGroup, },
	{ "pntxtb",	RTFidPNTXTB,	DOClevANY, docRtfPndescGroup, },

	{ 0, 0, 0 }
    };

static int docRtfReadPnseclvl(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int			res;
    int			level= arg- 1;

    if  ( level < 0 )
	{ LDEB(level);	}

    docInitParagraphNumber( &(rrc->rrcParagraphNumber) );

    res= docRtfReadGroup( sis, DOClevANY,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfDocumentWords, docRtfPndescGroups,
				docRtfRefuseText, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res); return res;	}

    rrc->rrcParagraphNumber.pnLevel= level;

    if  ( level >= rrc->rrcParagraphNumberCount )
	{
	ParagraphNumber *	fresh;

	fresh= realloc( rrc->rrcParagraphNumbers,
				( level+ 1 )* sizeof( ParagraphNumber ) );
	if  ( ! fresh )
	    { LXDEB(level,fresh); return -1;	}

	rrc->rrcParagraphNumbers= fresh;

	while( rrc->rrcParagraphNumberCount <= level )
	    {
	    docInitParagraphNumber( rrc->rrcParagraphNumbers+ 
					    rrc->rrcParagraphNumberCount++ );
	    }
	}

    rrc->rrcParagraphNumbers[level]= rrc->rrcParagraphNumber;

    return res;
    }

static RtfControlWord	docRtfPnWords[]=
    {
				/****************************************/
				/*  Bullets and Numbering.		*/
				/*  (No destinations)			*/
				/****************************************/
	{ "pnacross",	RTFidPNACROSS,	DOClevANY, docRtfIgnoreWord, },
	{ "pnlvl",	RTFidPNLVL,	DOClevANY, docRtfIgnoreWord, },
	{ "pnlvlblt",	RTFidPNLVLBLT,	DOClevANY, docRtfIgnoreWord, },
	{ "pnlvlbody",	RTFidPNLVLBODY,	DOClevANY, docRtfIgnoreWord, },
	{ "pnlvlcont",	RTFidPNLVLCONT,	DOClevANY, docRtfIgnoreWord, },
	{ "pnnumonce",	RTFidPNNUMONCE,	DOClevANY, docRtfIgnoreWord, },
	{ "pnrestart",	RTFidPNRESTART,	DOClevANY, docRtfIgnoreWord, },

	{ "pnrnot",	RTFidPNRNOT,	DOClevANY, docRtfIgnoreWord, },

	{ 0, 0, 0 }
    };

static int docRtfSkipPn(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int		res;

    docInitParagraphNumber( &(rrc->rrcParagraphNumber) );

    res= docRtfReadGroup( sis, DOClevPARA,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfPnWords, docRtfPndescGroups,
				docRtfRefuseText, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

/************************************************************************/
/*									*/
/*  Read the result of a bullet.					*/
/*									*/
/*  For one reason or another, Word resets the paragraph properties	*/
/*  inside the result. So save and restore them.			*/
/*									*/
/************************************************************************/

static int docRtfReadPntext(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    int				res;

#   if  0
    ParagraphProperties		pp;

    PropertyMask		ppChgMask;
    PropertyMask		ppUpdMask;

    PROPmaskCLEAR( &ppChgMask );

    PROPmaskCLEAR( &ppUpdMask );
    utilPropMaskFill( &ppUpdMask, PPprop_COUNT );

    docInitParagraphProperties( &pp );
    docUpdParaProperties( &ppChgMask, &pp, &ppUpdMask,
			    &(rrc->rrcBi->biParaProperties), (const int *)0 );

    res= docRtfReadGroup( sis, DOClevPARA,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfDocumentWords, docRtfEmptyTable,
				docRtfTextParticule, (RtfCommitGroup)0 );

    PROPmaskCLEAR( &ppChgMask );

    PROPmaskCLEAR( &ppUpdMask );
    utilPropMaskFill( &ppUpdMask, PPprop_COUNT );

    docUpdParaProperties( &ppChgMask, &(rrc->rrcBi->biParaProperties),
					    &ppUpdMask, &pp, (const int *)0 );
    docCleanParagraphProperties( &pp );
#   else
#   endif

    res= docRtfReadTextField( sis, rcw, arg, rrc );

    rrc->rrcJustAfterPntext= 1;

    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

/************************************************************************/
/*									*/
/*  Read a whole document.						*/
/*									*/
/*  As at least MS-Word 2000 crashes on lists without an override,	*/
/*  and on the other hand staroffice/openoffice creates them under	*/
/*  certain circumstances: Make an empty override for lists without	*/
/*  overrides.								*/
/*									*/
/************************************************************************/

static RtfControlWord	docRtfOutsideGroups[]=
    {
	{ "rtf",	RTFidRTF,	DOClevDOC, docRtfReadDoc,	},
	{ 0, 0, 0 }
    };

BufferDocument * docRtfReadFile(SimpleInputStream *		sis,
				const PostScriptFontList *	psfl,
				int				defAnsigpg )
    {
    BufferDocument *		bd;
    RtfReadingContext		rrc;

    int				res;
    const RtfControlWord *	rcw;

    char			controlWord[TEDszRTFCONTROL+1];
    int				gotArg;
    int				arg= -1;
    int				c;

    const int			listOnly= 1;

    int				noteIndex;
    DocumentNote *		dn;

    bd= (BufferDocument *)malloc( sizeof(BufferDocument) );
    if  ( ! bd )
	{ XDEB(bd); return bd;	}

    docInitDocument( bd );

    docRtfInitReadingContext( &rrc );
    rrc.rrcPostScriptFontList= psfl;

    rrc.rrcBd= bd;
    rrc.rrcBi= &(bd->bdItem);
    rrc.rrcLevel= DOClevDOC;
    rrc.rrcSplitLevel= DOClevDOC;

    if  ( defAnsigpg >= 0 )
	{ docRtfReadSetAnsicpg( &rrc, defAnsigpg );	}

    res= docRtfFindControl( sis, &rrc, &c, controlWord, &gotArg, &arg );

    if  ( res != RTFfiCTRLGROUP )
	{ docFreeDocument( bd ); return (BufferDocument *)0; }

    rcw= docRtfFindWord( controlWord, docRtfOutsideGroups, listOnly );
    if  ( ! rcw )
	{ LDEB(1); docFreeDocument( bd ); return (BufferDocument *)0; }

    res= docRtfApplyControlWord( sis, rcw, gotArg, arg, &rrc );
    if  ( res )
	{ LDEB(1); docFreeDocument( bd ); return (BufferDocument *)0; }

    if  ( ! rrc.rrcGotDocGeometry	&&
	  bd->bdItem.biChildCount > 0	)
	{
	bd->bdProperties.dpGeometry=
			    bd->bdItem.biChildren[0]->biSectDocumentGeometry;
	}

    docRtfCleanReadingContext( &rrc );

    noteIndex= docGetFirstNoteOfDocument( &dn, bd, DOCinFOOTNOTE );
    if  ( noteIndex >= 0 )
	{
	if  ( docCheckNoteSeparatorItem( bd, DOCinFTNSEP ) )
	    {
	    LDEB(DOCinFTNSEP);
	    docFreeDocument( bd ); return (BufferDocument *)0;
	    }
	}

    noteIndex= docGetFirstNoteOfDocument( &dn, bd, DOCinENDNOTE );
    if  ( noteIndex >= 0 )
	{
	if  ( docCheckNoteSeparatorItem( bd, DOCinAFTNSEP ) )
	    {
	    LDEB(DOCinAFTNSEP);
	    docFreeDocument( bd ); return (BufferDocument *)0;
	    }
	}

    if  ( docMakeOverrideForEveryList( &(bd->bdProperties) ) )
	{
	LDEB(1);
	docFreeDocument( bd ); return (BufferDocument *)0;
	}

    return bd;
    }

/************************************************************************/
/*									*/
/*  Remember properties to be used subsequently.			*/
/*									*/
/*  The level must be DOClevANY.					*/
/*									*/
/*  References are to be replaced with feature specific routine		*/
/*  refrerences.							*/
/*									*/
/************************************************************************/

int docRtfRememberProperty(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;

    switch( rcw->rcwID )
	{
	case RTFidDEFLANG:
	case RTFidDEFLANGFE:
	case RTFidUC:
	    break;

	case RTFidDELETED:
	    rrc->rrcInDeletedText= 1;
	    break;

	case RTFidFTNALT:
	    if  ( rrc->rrcExternalItemKind != DOCinFOOTNOTE )
		{ LLDEB(rrc->rrcExternalItemKind,DOCinFOOTNOTE);	}
	    else{ rrc->rrcExternalItemKind= DOCinENDNOTE;		}

	    break;

	case RTFidTX:
	    rrc->rrcTabStop.tsTwips= arg;
	    if  ( docParaAddTab( &(rrs->rrsParagraphProperties),
						    &(rrc->rrcTabStop) ) < 0 )
		{ LDEB(arg); return -1;	}
	    docInitTabStop( &(rrc->rrcTabStop) );

	    PROPmaskADD( &(rrc->rrcDocumentStyle.dsParaMask),
							PPpropTAB_STOPS );
	    break;

	case RTFidADJUSTRIGHT:
	case RTFidADJUSTLEFT:
	    break;

	case RTFidFC:
	    /*  Does not appear in the RTF spec.		*/
	    /*  It can however be found in NextStep files.	*/
	    break;
	case RTFidUP:
	    break;
				/****************************************/
				/*  Set/Unset Text Attributes.		*/
				/****************************************/
	case RTFidULNONE:
	    rrs->rrsTextAttribute.taTextIsUnderlined= 0;
	    break;
				/****************************************/
				/*  Set/Unset Text Attributes.		*/
				/*  In Paragraph Numbering.		*/
				/****************************************/
	case RTFidPNULNONE:
	    rrc->rrcParagraphNumber.pnTextAttribute.taTextIsUnderlined= 0;
	    break;

				/****************************************/
	case RTFidCHCFPAT:
	    rrc->rrcTextForeground= arg;
	    break;
	case RTFidCHCBPAT:
	    rrc->rrcTextBackground= arg;
	    break;

	case RTFidCHSHDNG:
	    rrc->rrcTextShadingLevel= arg;
	    break;

	case RTFidCHBRDR:
	    rrc->rrcTextBorderProperties= rrc->rrcBorderProperties;
	    docInitBorderProperties( &(rrc->rrcBorderProperties) );
	    break;

	case RTFidCELLX:
	    rrc->rrcCellProperties.cpRightBoundaryTwips= arg;

	    if  ( docInsertRowColumn( &(rrc->rrcRowProperties), -1,
				    &rrc->rrcCellProperties, (const int *)0 ) )
		{ LDEB(rrc->rrcRowProperties.rpCellCount); return -1; }

	    docInitCellProperties( &(rrc->rrcCellProperties) );

	    break;

				/****************************************/
				/*  Paragraph Numbering.		*/
				/****************************************/
	case RTFidPNHANG:
	    rrc->rrcParagraphNumber.pnUseHangingIndent= 1;
	    break;
				/****************************************/
				/*  Encoding.				*/
				/****************************************/
	case RTFidCGRID:
	case RTFidLANG:
	    break;
				/****************************************/
				/*  Fields in dates and times.		*/
				/****************************************/
	case RTFidHR:
	    rrc->rrcTm.tm_hour= arg;
	    break;
	case RTFidMIN:
	    rrc->rrcTm.tm_min= arg;
	    break;
	case RTFidSEC:
	    rrc->rrcTm.tm_sec= arg;
	    break;
	case RTFidDY:
	    rrc->rrcTm.tm_mday= arg;
	    break;
	case RTFidMO:
	    rrc->rrcTm.tm_mon= arg- 1;
	    break;
	case RTFidYR:
	    rrc->rrcTm.tm_year= arg- 1900;
	    break;
				/****************************************/
				/*  Unknown				*/
				/****************************************/
	default:
	    /*SLDEB(rcw->rcwWord,rcw->rcwID);*/
	    break;
	}

    return 0;
    }
