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
#   include	<psFont.h>
#   include	<utilMatchFont.h>
#   include	"docRtf.h"

/************************************************************************/
/*									*/
/*  Consume a font table.						*/
/*									*/
/************************************************************************/

static int docRtfCommitFaltName(	RtfReadingContext *	rrc )
    {
    const int	removeSemicolon= 0;
    int		size= 0;

    if  ( docRtfRememberText( &(rrc->rrcCurrentFont.dfAltName), &size,
						    rrc, removeSemicolon ) )
	{ LDEB(1); return -1; }

    return 0;
    }

static int docRtfFontFaltGroup( SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    if  ( docRtfReadGroup( sis, rcw->rcwLevel,
				    (RtfControlWord *)0, 0, 0, rrc,
				    docRtfEmptyTable, docRtfEmptyTable,
				    docRtfSaveText, docRtfCommitFaltName ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

static int docRtfFontPanoseText(	RtfReadingContext *	rrc,
					const unsigned char *	panose,
					int			len )
    {
    if  ( len != FONTlenPANOSE )
	{ LLDEB(len,FONTlenPANOSE); return 0;	}

    memcpy( rrc->rrcCurrentFont.dfPanose, panose, len );

    rrc->rrcCurrentFont.dfPanose[FONTlenPANOSE]= '\0';

    return 0;
    }

static int docRtfFontPanoseGroup( SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    if  ( docRtfReadGroup( sis, rcw->rcwLevel,
				    (RtfControlWord *)0, 0, 0, rrc,
				    docRtfEmptyTable, docRtfEmptyTable,
				    docRtfFontPanoseText, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

static int docRtfAddCurrentFontToList(	RtfReadingContext *	rrc )
    {
    const int	removeSemicolon= 1;
    int		size= 0;

    if  ( docRtfRememberText( &(rrc->rrcCurrentFont.dfName), &size,
						    rrc, removeSemicolon ) )
	{ LDEB(1); return -1; }

    if  ( rrc->rrcCurrentFont.dfName )
	{
	DocumentFont *		df;
	DocumentProperties *	dp= &(rrc->rrcBd->bdProperties);
	DocumentFontList *	dfl= &(dp->dpFontList);

	if  ( ! strcmp( rrc->rrcCurrentFont.dfName, "ZapfDingbats" ) )
	    { rrc->rrcCurrentFont.dfName= strdup( "ITC Zapf Dingbats" ); }

	rrc->rrcCurrentFont.dfUsed= 1;

	df= docInsertFont( dfl, rrc->rrcCurrentFont.dfDocFontNumber,
						    &(rrc->rrcCurrentFont) );
	if  ( ! df )
	    { SDEB(rrc->rrcCurrentFont.dfName); return -1; }
	}

    docCleanDocumentFont( &(rrc->rrcCurrentFont) );
    docInitDocumentFont( &(rrc->rrcCurrentFont) );

    return 0;
    }

static int docRtfFontName(	RtfReadingContext *	rrc,
				const unsigned char *	name,
				int			len )
    {
    if  ( docRtfSaveText( rrc, name, len ) )
	{ LDEB(len); return -1;	}

    if  ( docRtfAddCurrentFontToList( rrc ) )
	{ LDEB(len); return -1;	}

    return 0;
    }

static int docRtfFontNumber(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    docCleanDocumentFont( &(rrc->rrcCurrentFont) );
    docInitDocumentFont( &(rrc->rrcCurrentFont) );

    /*
    DocumentProperties *	dp= &(rrc->rrcBd->bdProperties);

    if  ( dp->dpAnsiCodepage >= 0 )
	{
	int charset;

	charset= utilWindowsCharsetFromCodepage( dp->dpAnsiCodepage );

	if  ( charset >= 0 )
	    { rrc->rrcCurrentFont.dfCharset= charset;	}
	}
    */

    rrc->rrcCurrentFont.dfDocFontNumber= arg;

    return 0;
    }

int docRtfFontProperty(		SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    switch( rcw->rcwID )
	{
	case DFpropFAMILY_STYLE:

	    rrc->rrcCurrentFont.dfFamilyStyle= strdup( rcw->rcwWord );

	    if  ( ! rrc->rrcCurrentFont.dfFamilyStyle )
		{ XDEB(rrc->rrcCurrentFont.dfFamilyStyle); return -1;	}

	    break;

	case DFpropCHARSET:
	    rrc->rrcCurrentFont.dfCharset= arg;
	    break;

	case DFpropCODEPAGE:
	    rrc->rrcCurrentFont.dfCodepage= arg;
	    break;

	case DFpropPITCH:
	    rrc->rrcCurrentFont.dfPitch= arg;
	    break;

	default:
	    SLDEB(rcw->rcwWord,arg);
	}

    return 0;
    }

static RtfControlWord	docRtfFontGroupGroups[]=
    {
	{ "panose",	DFpropPANOSE,	DOClevANY, docRtfFontPanoseGroup, },
	{ "falt",	DFpropALT_NAME,	DOClevANY, docRtfFontFaltGroup,	  },

	{ 0, 0, 0 }
    };

static int docRtfFontGroup(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    docCleanDocumentFont( &(rrc->rrcCurrentFont) );
    docInitDocumentFont( &(rrc->rrcCurrentFont) );

    /*
    DocumentProperties *	dp= &(rrc->rrcBd->bdProperties);

    if  ( dp->dpAnsiCodepage >= 0 )
	{
	int charset;

	charset= utilWindowsCharsetFromCodepage( dp->dpAnsiCodepage );

	if  ( charset >= 0 )
	    { rrc->rrcCurrentFont.dfCharset= charset;	}
	}
    */

    rrc->rrcCurrentFont.dfDocFontNumber= arg;

    if  ( docRtfReadGroup( sis, rcw->rcwLevel,
				(RtfControlWord *)0, 0, 0, rrc,
				(RtfControlWord *)0, docRtfFontGroupGroups,
				docRtfSaveText, docRtfAddCurrentFontToList ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

static RtfControlWord	docRtfFontTableWords[]=
    {
	{ "f",		RTFidF,		DOClevANY, docRtfFontNumber, },

	{ 0, 0, 0 }
    };

static RtfControlWord	docRtfFontTableGroups[]=
    {
	{ "f",		RTFidF,		DOClevANY,  docRtfFontGroup, },

	{ 0, 0, 0 }
    };

int docRtfFontTable(	SimpleInputStream *	sis,
			const RtfControlWord *	rcw,
			int			arg,
			RtfReadingContext *	rrc	)
    {
    DocumentProperties *	dp= &(rrc->rrcBd->bdProperties);
    DocumentFontList *		dfl= &(dp->dpFontList);
    const PostScriptFontList *	psfl= rrc->rrcPostScriptFontList;

    if  ( docRtfReadGroup( sis, rcw->rcwLevel,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfFontTableWords, docRtfFontTableGroups,
				docRtfFontName, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    if  ( ! psfl )
	{ XDEB(psfl); return -1;	}

    if  ( utilFindPsFontsForDocFonts( dfl, psfl ) )
	{ LDEB(1);	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write a font table.							*/
/*									*/
/************************************************************************/

void docRtfWriteFontTable(	SimpleOutputStream *		sos,
				int *				pCol,
				const DocumentFontList *	dfl )
    {
    int				i;
    const DocumentFont *	df= dfl->dflFonts;

    docRtfWriteDestinationBegin( "\\fonttbl", pCol, sos );
    docRtfWriteNextLine( pCol, sos );

    for ( i= 0; i < dfl->dflFontCount; df++, i++ )
	{
	int		afterTag= 0;

	if  ( ! df->dfUsed )
	    { continue;	}
	if  ( ! df->dfFamilyStyle && ! df->dfName )
	    { continue;	}

	docRtfWriteArgDestinationBegin( "\\f", pCol,
					    df->dfDocFontNumber, sos );

	sioOutPutCharacter( '\\', sos ); *pCol += 1;
	if  ( df->dfFamilyStyle )
	    {
	    sioOutPutString( df->dfFamilyStyle, sos );
	    *pCol += strlen( df->dfFamilyStyle );
	    }
	else{
	    sioOutPutString( "fnil", sos );
	    *pCol += 4;
	    }
	afterTag= 1;

	if  ( df->dfCodepage >= 0 )
	    {
	    docRtfWriteArgTag( "\\cpg", pCol, df->dfCodepage, sos );
	    afterTag= 1;
	    }

	if  ( df->dfCharset != FONTcharsetDEFAULT )
	    {
	    docRtfWriteArgTag( "\\fcharset", pCol, df->dfCharset, sos );
	    afterTag= 1;
	    }

	if  ( df->dfPitch != FONTpitchDEFAULT )
	    {
	    docRtfWriteArgTag( "\\fprq", pCol, df->dfPitch, sos );
	    afterTag= 1;
	    }

	if  ( df->dfPanose[0] )
	    {
	    const int	addSemicolon= 0;

	    docRtfWriteStringDestination( "\\*\\panose", pCol,
			(unsigned char *)df->dfPanose, strlen( df->dfPanose ),
			addSemicolon, sos );

	    afterTag= 0;
	    }

	if  ( df->dfName )
	    {
	    const char *	name= df->dfName;

	    if  ( ! strcmp( name, "ITC Zapf Dingbats" ) )
		{ name= "ZapfDingbats";	}

	    if  ( afterTag )
		{ sioOutPutCharacter( ' ', sos ); *pCol += 1;	}

	    sioOutPutString( name, sos );
	    *pCol += strlen( name );
	    }

	if  ( df->dfAltName )
	    {
	    const int	addSemicolon= 0;

	    docRtfWriteStringDestination( "\\*\\falt", pCol,
			(unsigned char *)df->dfAltName,
			strlen( df->dfAltName ),
			addSemicolon, sos );

	    afterTag= 0;
	    }

	sioOutPutCharacter( ';', sos ); *pCol += 1;

	docRtfWriteDestinationEnd( pCol, sos );
	if  ( i+ 1 < dfl->dflFontCount )
	    { docRtfWriteNextLine( pCol, sos );	}
	}

    docRtfWriteDestinationEnd( pCol, sos );
    docRtfWriteNextLine( pCol, sos );

    return;
    }

