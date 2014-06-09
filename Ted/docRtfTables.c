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
/*                                                                      */
/*  Consume the 'revision table' of a document                          */
/*    added by davina 6-2-00                                            */
/*                                                                      */
/************************************************************************/

static int docRtfSaveRevisionTable(	RtfReadingContext *	rrc,
					const unsigned char *	text,
					int			len )
    {
    unsigned char **		newAuthorArray;
    unsigned char *		newAuthor;
    DocumentProperties *	dp= &(rrc->rrcBd->bdProperties);

    newAuthorArray = (unsigned char**)realloc( dp->dpAuthors, 
			   (dp->dpAuthorCount+1) * sizeof(unsigned char *));
    if  ( ! newAuthorArray )
	{ XDEB(newAuthorArray); return -1;	}

    dp->dpAuthors= newAuthorArray;
    newAuthor= (unsigned char*)strdup( (char *)text );

    if  ( ! newAuthor )
	{ XDEB(newAuthor); return -1; }
    dp->dpAuthors[dp->dpAuthorCount++]= newAuthor;

    return 0;
    }

int docRtfRevisionTable(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    if  ( docRtfReadGroup(sis, rcw->rcwLevel, (RtfControlWord*)0, 0, 0, rrc,
                          docRtfDocumentWords, docRtfDocumentGroups,
                          docRtfSaveRevisionTable, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord, arg); return -1; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read the info group.						*/
/*									*/
/************************************************************************/

static int docRtfInfoText(	RtfReadingContext *	rrc,
				unsigned char **	pText,
				const unsigned char *	text,
				int			len )
    {
    char *	saved;
    char *	to;

    int		oldLen= 0;

    if  ( *pText )
	{ oldLen= strlen( (char *)*pText );	}

    saved= (char *)malloc( oldLen+ len+ 1 );
    if  ( ! saved )
	{ XDEB(saved); return -1;	}

    to= saved;

    if  ( *pText )
	{ strcpy( to, (const char *)*pText ); to += oldLen;	}

    while( len > 0 )
	{ *(to++)= *(text++); len--;	}
    *to= '\0';

    if  ( *pText )
	{ free( *pText );	}

    *pText= (unsigned char *)saved;

    return 0;
    }

static int docRtfGeneratorText(	RtfReadingContext *	rrc,
				const unsigned char *	text,
				int			len )
    {
    return docRtfInfoText( rrc, &(rrc->rrcBd->bdProperties.dpGenerator),
								text, len );
    }

static int docRtfTitleText(	RtfReadingContext *	rrc,
				const unsigned char *	text,
				int			len )
    {
    return docRtfInfoText( rrc, &(rrc->rrcBd->bdProperties.dpTitle),
								text, len );
    }

static int docRtfAuthorText(	RtfReadingContext *	rrc,
				const unsigned char *	text,
				int			len )
    {
    return docRtfInfoText( rrc, &(rrc->rrcBd->bdProperties.dpAuthor),
								text, len );
    }

static int docRtfCompanyText(	RtfReadingContext *	rrc,
				const unsigned char *	text,
				int			len )
    {
    return docRtfInfoText( rrc, &(rrc->rrcBd->bdProperties.dpCompany),
								text, len );
    }

static int docRtfSubjectText(	RtfReadingContext *	rrc,
				const unsigned char *	text,
				int			len )
    {
    return docRtfInfoText( rrc, &(rrc->rrcBd->bdProperties.dpSubject),
								text, len );
    }

static int docRtfKeywordsText(	RtfReadingContext *	rrc,
				const unsigned char *	text,
				int			len )
    {
    return docRtfInfoText( rrc, &(rrc->rrcBd->bdProperties.dpKeywords),
								text, len );
    }

static int docRtfDoccommText(	RtfReadingContext *	rrc,
				const unsigned char *	text,
				int			len )
    {
    return docRtfInfoText( rrc, &(rrc->rrcBd->bdProperties.dpDoccomm),
								text, len );
    }

static int docRtfHlinkbaseText(	RtfReadingContext *	rrc,
				const unsigned char *	text,
				int			len	)
    {
    return docRtfInfoText( rrc, &(rrc->rrcBd->bdProperties.dpHlinkbase),
								text, len );
    }

int docRtfDocInfoGroup(		SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    RtfAddTextParticule		addTextParticule= (RtfAddTextParticule)0;

    switch( rcw->rcwID )
	{
	case DPpropTITLE:
	    addTextParticule= docRtfTitleText;
	    break;

	case DPpropAUTHOR:
	    addTextParticule= docRtfAuthorText;
	    break;

	case DPpropCOMPANY:
	    addTextParticule= docRtfCompanyText;
	    break;

	case DPpropSUBJECT:
	    addTextParticule= docRtfSubjectText;
	    break;

	case DPpropKEYWORDS:
	    addTextParticule= docRtfKeywordsText;
	    break;

	case DPpropDOCCOMM:
	    addTextParticule= docRtfDoccommText;
	    break;

	case DPpropHLINKBASE:
	    addTextParticule= docRtfHlinkbaseText;
	    break;

	case DPpropGENERATOR:
	    addTextParticule= docRtfGeneratorText;
	    break;

	default:
	    SDEB(rcw->rcwWord); return -1;
	}

    if  ( docRtfReadGroup( sis, rcw->rcwLevel,
				    (RtfControlWord *)0, 0, 0, rrc,
				    docRtfEmptyTable, docRtfEmptyTable,
				    addTextParticule, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

static int docRtfDocTimeGroup(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    appInvalidateTime( &(rrc->rrcTm) );

    if  ( docRtfReadGroup( sis, rcw->rcwLevel,
				    (RtfControlWord *)0, 0, 0, rrc,
				    docRtfEmptyTable, docRtfEmptyTable,
				    docRtfRefuseText, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    switch( rcw->rcwID )
	{
	case DPpropCREATIM:
	    rrc->rrcBd->bdProperties.dpCreatim= rrc->rrcTm;
	    break;

	case DPpropREVTIM:
	    rrc->rrcBd->bdProperties.dpRevtim= rrc->rrcTm;
	    break;

	case DPpropPRINTIM:
	    rrc->rrcBd->bdProperties.dpPrintim= rrc->rrcTm;
	    break;

	default:
	    SDEB(rcw->rcwWord); return -1;
	}

    return 0;
    }

static RtfControlWord	docRtfInfoGroups[]=
    {
	{ "title",	DPpropTITLE,	DOClevDOC, docRtfDocInfoGroup,	},
	{ "author",	DPpropAUTHOR,	DOClevDOC, docRtfDocInfoGroup,	},
	{ "company",	DPpropCOMPANY,	DOClevDOC, docRtfDocInfoGroup,	},
	{ "subject",	DPpropSUBJECT,	DOClevDOC, docRtfDocInfoGroup,	},
	{ "keywords",	DPpropKEYWORDS,	DOClevDOC, docRtfDocInfoGroup,	},
	{ "comment",	DPpropDOCCOMM,	DOClevDOC, docRtfDocInfoGroup,	},
	{ "doccomm",	DPpropDOCCOMM,	DOClevDOC, docRtfDocInfoGroup,	},
	{ "hlinkbase",	DPpropHLINKBASE,DOClevDOC, docRtfDocInfoGroup,	},

	{ "creatim",	DPpropCREATIM,	DOClevDOC, docRtfDocTimeGroup, },
	{ "revtim",	DPpropREVTIM,	DOClevDOC, docRtfDocTimeGroup, },
	{ "printim",	DPpropPRINTIM,	DOClevDOC, docRtfDocTimeGroup, },

	{ "operator",	RTFidOPERATOR,	DOClevDOC, docRtfSkipGroup, },
	{ "version",	RTFidVERSION,	DOClevDOC, docRtfSkipGroup, },
	{ "edmins",	RTFidEDMINS,	DOClevDOC, docRtfSkipGroup, },
	{ "nofpages",	RTFidNOFPAGES,	DOClevDOC, docRtfSkipGroup, },
	{ "nofwords",	RTFidNOFWORDS,	DOClevDOC, docRtfSkipGroup, },
	{ "nofchars",	RTFidNOFCHARS,	DOClevDOC, docRtfSkipGroup, },
	{ "nofcharsws",	RTFidNOFCHARSWS,DOClevDOC, docRtfSkipGroup, },
	{ "vern",	RTFidVERN,	DOClevDOC, docRtfSkipGroup, },

	{ 0, 0, 0 }
    };

int docRtfReadInfo(		SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc	)
    {
    int		res;

    res= docRtfReadGroup( sis, DOClevPARA,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfEmptyTable, docRtfInfoGroups,
				docRtfIgnoreText, (RtfCommitGroup)0 );
    if  ( res )
	{ SLDEB(rcw->rcwWord,res);	}

    return res;
    }

