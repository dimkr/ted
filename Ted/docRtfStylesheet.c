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
/*  Consume the 'stylesheet' of a document				*/
/*									*/
/************************************************************************/

static int docRtfStyleName(	RtfReadingContext *	rrc,
				const unsigned char *	name,
				int			len )
    {
    RtfReadingState *		rrs= rrc->rrcState;
    BufferDocument *		bd= rrc->rrcBd;
    DocumentProperties *	dp= &(bd->bdProperties);

    DocumentStyle *		ds;

    PropertyMask		ppChgMask;
    PropertyMask		ppUpdMask;

    const int * const		colorMap= (const int *)0;
    const int * const		listStyleMap= (const int *)0;

    ds= &(rrc->rrcDocumentStyle);
    switch( ds->dsLevel )
	{
	case DOClevTEXT:
	    break;

	case 0: case -1:
	    LDEB(rrc->rrcDocumentStyle.dsLevel);
	    ds->dsStyleNumber= 0;
	    ds->dsLevel= DOClevPARA;
	    break;

	case DOClevPARA:
	    ds->dsStyleNumber= rrs->rrsParagraphProperties.ppStyle;
	    break;

	case DOClevSECT:
	    ds->dsStyleNumber= rrs->rrsSectionProperties.spStyle;
	    break;

	default:
	    LDEB(rrc->rrcDocumentStyle.dsLevel); return -1;
	}

    if  ( len > 0 && name[len-1] == ';' )
	{ len--;	}

    ds= docInsertStyle( &(rrc->rrcBd->bdStyleSheet),
					rrc->rrcDocumentStyle.dsStyleNumber );
    if  ( ! ds )
	{ XDEB(ds); return -1;	}

    if  ( docCopyStyle( ds, &(rrc->rrcDocumentStyle) ) )
	{ LDEB(1); return -1;	}

    if  ( len > 0 )
	{
	unsigned char *	s;

	s= realloc( ds->dsName, len+ 1 );
	if  ( ! s )
	    { XDEB(s); return -1;	}
	memcpy( s, name, len );
	s[len]= '\0';

	ds->dsName= s;
	}

    docCopySectionProperties( &(ds->dsSectionProperties),
					    &(rrs->rrsSectionProperties) );

    PROPmaskCLEAR( &ppChgMask );

    PROPmaskCLEAR( &ppUpdMask );
    utilPropMaskFill( &ppUpdMask, PPprop_COUNT );

    docUpdParaProperties( &ppChgMask, &(ds->dsParagraphProperties),
				&ppUpdMask, &(rrs->rrsParagraphProperties),
				colorMap, listStyleMap );

    ds->dsTextAttribute= rrs->rrsTextAttribute;

    docCleanDocumentStyle( &(rrc->rrcDocumentStyle) );
    docInitDocumentStyle( &(rrc->rrcDocumentStyle) );

    docCleanParagraphProperties( &(rrs->rrsParagraphProperties) );
    docInitParagraphProperties( &(rrs->rrsParagraphProperties) );
    utilInitTextAttribute( &(rrs->rrsTextAttribute) );
    rrs->rrsTextAttribute.taFontSizeHalfPoints= 24;
    rrc->rrcInDeletedText= 0;

    if  ( dp->dpDefaultFont >= 0 )
	{ rrs->rrsTextAttribute.taFontNumber= dp->dpDefaultFont;	}

    return 0;
    }

static int docRtfStyleGroup(		SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;

    switch( rcw->rcwID )
	{
	case RTFidCS:
	    rrc->rrcDocumentStyle.dsStyleNumber= arg;
	    rrc->rrcDocumentStyle.dsLevel= DOClevTEXT;
	    break;

	case RTFidDS:
	    rrs->rrsSectionProperties.spStyle= arg;
	    rrc->rrcDocumentStyle.dsLevel= DOClevSECT;
	    break;

	case RTFidS:
	    rrs->rrsParagraphProperties.ppStyle= arg;
	    rrc->rrcDocumentStyle.dsLevel= DOClevPARA;
	    break;

	default:
	    LDEB(rcw->rcwID);
	}

    if  ( docRtfReadGroup( sis, rcw->rcwLevel,
				    (RtfControlWord *)0, 0, 0, rrc,
				    docRtfEmptyTable, docRtfEmptyTable,
				    docRtfStyleName, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    rrc->rrcDocumentStyle.dsLevel= DOClevANY;

    return 0;
    }

static RtfControlWord	docRtfStylesheetGroups[]=
    {
	{ "cs",		RTFidCS,	DOClevDOC, docRtfStyleGroup, },
	{ "ds",		RTFidDS,	DOClevDOC, docRtfStyleGroup, },
	{ "s",		RTFidS,		DOClevDOC, docRtfStyleGroup, },

	{ 0, 0, 0 }
    };

/************************************************************************/
/*									*/
/*  Consume the stylesheet in a document.				*/
/*									*/
/*  Because <styledef> is optional in the stylesheet. (For the first	*/
/*  style at least.) The normal mechanism of the parser does not work	*/
/*  for the stylesheet. Do things by hand.				*/
/*									*/
/************************************************************************/

int docRtfStylesheet(		SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    RtfReadingState *		rrs= rrc->rrcState;
    int				res;

    char			controlWord[TEDszRTFCONTROL+1];
    int				gotArg;
    int				c;

    const int			textLevel= DOClevANY;
    
    const int			listOnly= 1;

    docCleanDocumentStyle( &(rrc->rrcDocumentStyle) );
    docInitDocumentStyle( &(rrc->rrcDocumentStyle) );

#   if 0
    docCleanParagraphProperties( &(rrc->rrcParagraphProperties) );
    docInitParagraphProperties( &(rrc->rrcParagraphProperties) );
    utilInitTextAttribute( &(rrc->rrcCurrentTextAttribute) );
    rrc->rrcCurrentTextAttribute.taFontSizeHalfPoints= 24;
    rrc->rrcInDeletedText= 0;
#   endif

    res= docRtfFindControl( sis, rrc, &c, controlWord, &gotArg, &arg );
    if  ( res < 0 )
	{ LDEB(res); return -1;	}

    rrc->rrcDocumentStyle.dsLevel= DOClevPARA;
    rrs->rrsParagraphProperties.ppStyle= 0;

    for (;;)
	{
	rrs= rrc->rrcState;

	switch( res )
	    {
	    case RTFfiCLOSE:
		break;

	    case RTFfiCTRLGROUP:
		rcw= docRtfFindWord( controlWord,
					    docRtfStylesheetGroups, listOnly );
		if  ( ! rcw )
		    {
		    if  ( docRtfReadWordGroup( sis, textLevel, rrc,
					gotArg, arg, controlWord,
					docRtfEmptyTable, docRtfEmptyTable,
					docRtfStyleName ) )
			{ SDEB(controlWord); return -1;	}
		    }
		else{
		  groupFound:
		    if  ( rcw->rcwLevel != DOClevANY			&&
			  docRtfAdjustLevel( rrc,
					    rcw->rcwLevel, textLevel )	)
			{ SDEB(controlWord); return -1; }

		    res= docRtfApplyControlWord( sis, rcw, gotArg, arg, rrc );

		    if  ( res < 0 )
			{ LDEB(res); SDEB(controlWord); return -1;	}
		    }

		res= docRtfFindControl( sis, rrc, &c,
						controlWord, &gotArg, &arg );
		if  ( res < 0 )
		    { LDEB(res); return -1;	}
		continue;

	    case RTFfiSTARGROUP:
		rcw= docRtfFindWord( controlWord,
					    docRtfStylesheetGroups, listOnly );
		if  ( rcw )
		    { goto groupFound; }

		rrc->rrcInIgnoredGroup++;

		if  ( docRtfReadUnknownGroup( sis, rrc ) )
		    { LDEB(1); rrc->rrcInIgnoredGroup--; return -1;	}

		rrc->rrcInIgnoredGroup--;

		res= docRtfFindControl( sis, rrc, &c,
					    controlWord, &gotArg, &arg );
		if  ( res < 0 )
		    { LDEB(res); return -1;	}
		continue;

	    default:
		LDEB(res); return -1;
	    }

	break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write a Style Sheet.						*/
/*									*/
/************************************************************************/

void docRtfWriteStyleSheet(	SimpleOutputStream *		sos,
				const unsigned char *		outputMapping,
				int *				pCol,
				const DocumentStyleSheet *	dss )
    {
    int				i;
    const DocumentStyle *	ds;

    docRtfWriteDestinationBegin( "\\stylesheet", pCol, sos );
    docRtfWriteNextLine( pCol, sos );

    ds= dss->dssStyles;
    for ( i= 0; i < dss->dssStyleCount; ds++, i++ )
	{
	if  ( ds->dsStyleNumber < 0 )
	    { continue;	}

	switch( ds->dsLevel )
	    {
	    case DOClevTEXT:
		docRtfWriteArgDestinationBegin( "\\*\\cs", pCol,
						    ds->dsStyleNumber, sos );
		docRtfSaveTextAttribute( sos, pCol,
				&(ds->dsTextMask), &(ds->dsTextAttribute) );
		break;

	    case DOClevPARA:
		if  ( ds->dsStyleNumber > 0 )
		    {
		    docRtfWriteArgDestinationBegin( "\\s", pCol,
						    ds->dsStyleNumber, sos );
		    }
		else{
		    docRtfWriteDestinationBegin( "", pCol, sos );
		    }

		docRtfSaveParagraphProperties( sos, pCol,
			    &(ds->dsParaMask), &(ds->dsParagraphProperties) );
		docRtfSaveTextAttribute( sos, pCol,
			    &(ds->dsTextMask), &(ds->dsTextAttribute) );
		break;

	    case DOClevSECT:
		docRtfWriteArgDestinationBegin( "\\ds", pCol,
						    ds->dsStyleNumber, sos );

		docRtfSaveSectionProperties( sos, outputMapping, pCol,
			    &(ds->dsSectMask), &(ds->dsSectionProperties) );
		docRtfSaveParagraphProperties( sos, pCol,
			    &(ds->dsParaMask), &(ds->dsParagraphProperties) );
		docRtfSaveTextAttribute( sos, pCol,
			    &(ds->dsTextMask), &(ds->dsTextAttribute) );
		break;

	    default:
		LDEB(ds->dsLevel); continue;
	    }

	if  ( ds->dsAdditive )
	    { docRtfWriteTag( "\\additive", pCol, sos ); }
	if  ( ds->dsBasedOn >= 0 )
	    { docRtfWriteArgTag( "\\sbasedon", pCol, ds->dsBasedOn, sos ); }
	if  ( ds->dsNext >= 0 )
	    { docRtfWriteArgTag( "\\snext", pCol, ds->dsNext, sos ); }
	if  ( ds->dsAutoupd )
	    { docRtfWriteTag( "\\sautoupd", pCol, sos ); }
	if  ( ds->dsHidden )
	    { docRtfWriteTag( "\\shidden", pCol, sos ); }
	if  ( ds->dsLocked )
	    { docRtfWriteTag( "\\slocked", pCol, sos ); }
	if  ( ds->dsSemiHidden )
	    { docRtfWriteTag( "\\ssemihidden", pCol, sos ); }
	if  ( ds->dsPersonal )
	    { docRtfWriteTag( "\\spersonal", pCol, sos ); }
	if  ( ds->dsLink >= 0 )
	    { docRtfWriteTag( "\\slink", pCol, sos ); }

	sioOutPutCharacter( ' ', sos ); *pCol += 1;
	docRtfEscapeString( ds->dsName, outputMapping,
				pCol, strlen( (char *)ds->dsName ), sos );
	sioOutPutCharacter( ';', sos ); *pCol += 1;

	docRtfWriteDestinationEnd( pCol, sos );
	docRtfWriteNextLine( pCol, sos );
	}

    docRtfWriteDestinationEnd( pCol, sos );
    docRtfWriteNextLine( pCol, sos );

    return;
    }

/************************************************************************/
/*									*/
/*  Handle a style property when reading RTF.				*/
/*									*/
/************************************************************************/

int docRtfRememberStyleProperty(	SimpleInputStream *	sis,
					const RtfControlWord *	rcw,
					int			arg,
					RtfReadingContext *	rrc )
    {
    DocumentStyle *	ds= &(rrc->rrcDocumentStyle);

    switch( rcw->rcwID )
	{
	/* \s \cs \ds tags but \s and \ds handeled with the para/sect style */
	case DSpropSTYLE_NUMBER:
	    ds->dsStyleNumber= arg;
	    ds->dsLevel= rcw->rcwEnumValue;
	    break;

	case DSpropADDITIVE:
	    ds->dsAdditive= arg != 0;
	    break;
	case DSpropBASED_ON:
	    ds->dsBasedOn= arg;
	    break;
	case DSpropNEXT:
	    ds->dsNext= arg;
	    break;
	case DSpropLINK:
	    ds->dsLink= arg;
	    break;
	case DSpropAUTO_UPD:
	    ds->dsAutoupd= arg != 0;
	    break;
	case DSpropHIDDEN:
	    ds->dsHidden= arg != 0;
	    break;
	case DSpropLOCKED:
	    ds->dsLocked= arg != 0;
	    break;
	case DSpropSEMI_HIDDEN:
	    ds->dsSemiHidden= arg != 0;
	    break;
	case DSpropPERSONAL:
	    ds->dsPersonal= arg != 0;
	    break;

	default:
	    SDEB(rcw->rcwWord);
	    return -1;
	}

    return 0;
    }
