/************************************************************************/
/*									*/
/*  Font administration for a document.					*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	"utilPropMask.h"
#   include	"utilDocFont.h"
#   include	"utilFontEncoding.h"

/************************************************************************/
/*									*/
/*  Initialise a Font List.						*/
/*  Clean a Font List.							*/
/*  Copy a Font List.							*/
/*									*/
/************************************************************************/

static void docInitFontFamily(	DocumentFontFamily *	dff )
    {
    int		i;

    dff->dffFamilyName= (char *)0;

    for ( i= 0; i < CHARSETidxCOUNT; i++ )
	{ dff->dffFontForCharsetIndex[i]= -1;	}
    }

static void docCleanFontFamily(	DocumentFontFamily *	dff )
    {
    if  ( dff->dffFamilyName )
	{ free( dff->dffFamilyName );	}
    }

void docInitFontList(	DocumentFontList *	dfl )
    {
    utilInitializeFontEncodings();

    dfl->dflFontCount= 0;
    dfl->dflFonts= (DocumentFont *)0;
    dfl->dflFamilyCount= 0;
    dfl->dflFamilies= (DocumentFontFamily *)0;
    }

void docCleanFontList(	DocumentFontList *	dfl )
    {
    int		i;

    for ( i = 0; i < dfl->dflFontCount; i++ )
	{ docCleanDocumentFont( dfl->dflFonts+ i );	}

    if  ( dfl->dflFonts )
	{ free( dfl->dflFonts );	}

    for ( i = 0; i < dfl->dflFamilyCount; i++ )
	{ docCleanFontFamily( dfl->dflFamilies+ i );	}

    if  ( dfl->dflFamilies )
	{ free( dfl->dflFamilies );	}

    return;
    }

int docRememberFontNameAndFamily(	DocumentFontList *	dfl,
					DocumentFont *		df,
					int			charset )
    {
    int				rval= 0;
    char *			allocated= (char *)0;
    const char *		fontFamilyName= df->dfName;
    DocumentFontFamily *	dff;

    int				familyIdx;

    int				charsetIdx= -1;
    const OfficeCharset *	oc= (const OfficeCharset *)0;

    if  ( charset < 0 )
	{ charset= df->dfCharset;	}

    oc= utilGetOfficeCharsetByCharset( &charsetIdx, charset );

    if  ( oc )
	{
	const char *		suffix= oc->ocOfficeFontnameSuffix;

	if  ( suffix )
	    {
	    int			lenN= strlen( fontFamilyName );
	    int			lenS= strlen( suffix );

	    if  ( lenN > lenS						&&
		  ! strcmp( fontFamilyName+ lenN- lenS, suffix )	)
		{
		allocated= strdup( fontFamilyName );
		if  ( ! allocated )
		    { LXDEB(lenN,allocated); rval= -1; goto ready;	}

		allocated[lenN- lenS]= '\0';
		fontFamilyName= allocated;
		}
	    }
	}

    dff= dfl->dflFamilies;
    for ( familyIdx= 0; familyIdx < dfl->dflFamilyCount; dff++, familyIdx++ )
	{
	if  ( ! strcmp( dff->dffFamilyName, fontFamilyName ) )
	    { break; }
	}

    if  ( familyIdx >= dfl->dflFamilyCount )
	{
	dff= realloc( dfl->dflFamilies,
		    ( dfl->dflFamilyCount+ 1 )* sizeof(DocumentFontFamily) );
	if  ( ! dff )
	    { LXDEB(dfl->dflFamilyCount,dff); rval= -1; goto ready;	}

	dfl->dflFamilies= dff;

	dff += dfl->dflFamilyCount;

	docInitFontFamily( dff );

	if  ( ! allocated )
	    {
	    allocated= strdup( fontFamilyName );
	    if  ( ! allocated )
		{ XDEB(allocated); rval= -1; goto ready;	}
	    }

	dff->dffFamilyName= allocated; allocated= (char *)0; /* steal */

	dfl->dflFamilyCount++;
	}

    df->dfDocFamilyIndex= familyIdx;

    if  ( charset != FONTcharsetDEFAULT )
	{
	df->dfCharsetIndex= charsetIdx;
	if  ( charsetIdx >= 0 )
	    { dff->dffFontForCharsetIndex[charsetIdx]= df->dfDocFontNumber; }
	}

  ready:

    if  ( allocated )
	{ free( allocated );	}

    return rval;
    }

int docCopyFontList(	DocumentFontList *		to,
			const DocumentFontList *	from )
    {
    int				rval= 0;

    int				i;
    DocumentFontList		copy;

    DocumentFont *		dfTo;
    const DocumentFont *	dfFrom;

    DocumentFontFamily *	dffTo;
    const DocumentFontFamily *	dffFrom;

    docInitFontList( &copy );

    copy.dflFonts= malloc( from->dflFontCount* sizeof(DocumentFont) );
    if  ( ! copy.dflFonts )
	{ XDEB(copy.dflFonts); rval= -1; goto ready;	}

    for ( i = 0; i < from->dflFontCount; i++ )
	{ docInitDocumentFont( copy.dflFonts+ i );	}
    copy.dflFontCount= from->dflFontCount;

    copy.dflFamilies= malloc( from->dflFamilyCount*
						sizeof(DocumentFontFamily) );
    if  ( ! copy.dflFamilies )
	{ XDEB(copy.dflFamilies); rval= -1; goto ready;	}

    for ( i = 0; i < from->dflFamilyCount; i++ )
	{ docInitFontFamily( copy.dflFamilies+ i );	}
    copy.dflFamilyCount= from->dflFamilyCount;

    dfTo= copy.dflFonts;
    dfFrom= from->dflFonts;
    for ( i = 0; i < from->dflFontCount; dfTo++, dfFrom++, i++ )
	{
	if  ( docCopyDocumentFont( dfTo, dfFrom ) )
	    { LDEB(i); rval= -1; goto ready;	}
	}

    dffTo= copy.dflFamilies;
    dffFrom= from->dflFamilies;
    for ( i = 0; i < from->dflFamilyCount; dffTo++, dffFrom++, i++ )
	{
	if  ( docCopyDocumentFontFamily( dffTo, dffFrom ) )
	    { LDEB(i); rval= -1; goto ready;	}
	}

    docCleanFontList( to );

    *to= copy; docInitFontList( &copy ); /* steal contents */

  ready:
    docCleanFontList( &copy );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Initialise a font.							*/
/*  Clean a font.							*/
/*  Copy a font.							*/
/*									*/
/************************************************************************/

void docInitDocumentFont(	DocumentFont *	df )
    {
    int		i;

    df->dfFamilyStyle= (char *)0;
    df->dfName= (char *)0;
    df->dfAltName= (char *)0;
    df->dfDocFontNumber= -1;
    df->dfPsFamilyNumber= -1;
    df->dfDocFamilyIndex= -1;
    df->dfCharset= FONTcharsetDEFAULT;
    df->dfCharsetIndex= -1;
    df->dfCodepage= -1;
    df->dfPitch= FONTpitchDEFAULT;
    df->dfUsed= 0;

    df->dfPanose[0]= '\0';

    for ( i= 0; i < FONTface_COUNT; i++ )
	{ df->dfPsFaceNumber[i]= -1;	}

    df->dfOfficeCharsetMapping= (OfficeCharsetMapping *)0;
    }

void docCleanDocumentFont(	DocumentFont *	df	)
    {
    if  ( df->dfFamilyStyle )
	{ free( df->dfFamilyStyle );	}

    if  ( df->dfName )
	{ free( df->dfName );	}

    if  ( df->dfAltName )
	{ free( df->dfAltName );	}
    }

static int docFontSetFamilyStyleString(	DocumentFont *	df,
					const char *	style )
    {
    char *	 saved= (char *)0;

    if  ( style )
	{
	saved= strdup( style );
	if  ( ! saved )
	    { XDEB(saved); return -1;	}
	}

    if  ( df->dfFamilyStyle )
	{ free( df->dfFamilyStyle );	}

    df->dfFamilyStyle= saved;

    return 0;
    }

static int docFontFamilySetFamilyName(	DocumentFontFamily *	dff,
					const char *		name )
    {
    char *	 saved= (char *)0;

    if  ( name )
	{
	saved= strdup( name );
	if  ( ! saved )
	    { XDEB(saved); return -1;	}
	}

    if  ( dff->dffFamilyName )
	{ free( dff->dffFamilyName );	}

    dff->dffFamilyName= saved;

    return 0;
    }

int docFontSetFamilyName(		DocumentFont *	df,
					const char *	name )
    {
    char *	 saved= (char *)0;

    if  ( name )
	{
	saved= strdup( name );
	if  ( ! saved )
	    { XDEB(saved); return -1;	}
	}

    if  ( df->dfName )
	{ free( df->dfName );	}

    df->dfName= saved;

    return 0;
    }

int docFontSetAltName(		DocumentFont *	df,
				const char *	name )
    {
    char *	 saved= (char *)0;

    if  ( name )
	{
	saved= strdup( name );
	if  ( ! saved )
	    { XDEB(saved); return -1;	}
	}

    if  ( df->dfAltName )
	{ free( df->dfAltName );	}

    df->dfAltName= saved;

    return 0;
    }

const char * docFontFamilyStyleString(	int styleInt )
    {
    switch( styleInt )
	{
	case DFstyleFNIL:	return "fnil";		break;
	case DFstyleFROMAN:	return "froman";	break;
	case DFstyleFSWISS:	return "fswiss";	break;
	case DFstyleFMODERN:	return "fmodern";	break;
	case DFstyleFSCRIPT:	return "fscript";	break;
	case DFstyleFDECOR:	return "fdecor";	break;
	case DFstyleFTECH:	return "ftech";		break;

	default:
	    LDEB(styleInt); return (const char *)0;
	}
    }

int docFontSetFamilyStyle(	DocumentFont *	df,
				int		styleInt )
    {
    const char *	styleStr= (const char *)0;

    styleStr= docFontFamilyStyleString( styleInt );
    if  ( ! styleStr )
	{ LXDEB(styleInt,styleStr); return -1;	}

    if  ( docFontSetFamilyStyleString( df, styleStr ) )
	{ LSDEB(styleInt,styleStr); return -1;	}

    return 0;
    }

int docCopyDocumentFont(	DocumentFont *		to,
				const DocumentFont *	from )
    {
    int			rval= 0;
    DocumentFont	copy;

    copy= *from;

    copy.dfFamilyStyle= (char *)0;
    copy.dfName= (char *)0;
    copy.dfAltName= (char *)0;

    if  ( docFontSetFamilyStyleString( &copy, from->dfFamilyStyle ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docFontSetFamilyName( &copy, from->dfName ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docFontSetAltName( &copy, from->dfAltName ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docCleanDocumentFont( to );

    *to= copy; docInitDocumentFont( &copy ); /* steal */

  ready:

    docCleanDocumentFont( &copy );

    return 0;
    }

int docCopyDocumentFontFamily(	DocumentFontFamily *		to,
				const DocumentFontFamily *	from )
    {
    int			rval= 0;
    DocumentFontFamily	copy;

    copy= *from;

    copy.dffFamilyName= (char *)0;

    if  ( docFontFamilySetFamilyName( &copy, from->dffFamilyName ) )
	{ LDEB(1); rval= -1; goto ready;	}

    docCleanFontFamily( to );

    *to= copy; docInitFontFamily( &copy ); /* steal */

  ready:

    docCleanFontFamily( &copy );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a font.							*/
/*									*/
/************************************************************************/

DocumentFont * docInsertFont(	DocumentFontList *	dfl,
				int			n,
				const DocumentFont *	dfSet )
    {
    int			i;
    DocumentFont *	df;

    DocumentFont	dfNew;

    utilInitializeFontEncodings();

    docInitDocumentFont( &dfNew );

    if  ( n > dfl->dflFontCount )
	{
	df= (DocumentFont *)realloc( dfl->dflFonts,
		    ( n + 1 ) * sizeof( DocumentFont ) );
	}
    else{
	df= (DocumentFont *)realloc( dfl->dflFonts,
		    ( dfl->dflFontCount + 1 ) * sizeof( DocumentFont ) );
	}
    if  ( ! df )
	{ LLDEB(dfl->dflFontCount,df); goto failed;	}
    dfl->dflFonts= df;

    if  ( docCopyDocumentFont( &dfNew, dfSet ) )
	{ LDEB(1); goto failed;	}

    if  ( n == -1 )
	{
	df= dfl->dflFonts;
	for ( n= 0; n < dfl->dflFontCount; df++, n++ )
	    {
	    if  ( df->dfDocFontNumber < 0 )
		{ break;	}
	    }
	}
    else{
	if  ( n < dfl->dflFontCount && dfl->dflFonts[n].dfDocFontNumber >= 0 )
	    { LLDEB(n,dfl->dflFonts[n].dfDocFontNumber);	}

	/* No!!
	df= dfl->dflFonts;
	for ( i= dfl->dflFontCount; i > n; i-- )
	    { df[i]= df[i-1];	}
	*/
	}

    df= dfl->dflFonts+ dfl->dflFontCount;
    for ( i= dfl->dflFontCount; i <= n; df++, i++ )
	{ docInitDocumentFont( df );	}

    dfNew.dfDocFontNumber= n;

    if  ( docRememberFontNameAndFamily( dfl, &dfNew, -1 ) )
	{ SDEB(dfNew.dfName); goto failed;	}

    df= dfl->dflFonts+ n;
    docCleanDocumentFont( df );

    *df= dfNew;

    if  ( n >= dfl->dflFontCount )
	{ dfl->dflFontCount= n+ 1;	}

    return df;

  failed:

    docCleanDocumentFont( &dfNew );

    return (DocumentFont *)0;
    }

/************************************************************************/
/*									*/
/*  Find or insert a font in a document font list.			*/
/*									*/
/*  In office documents, the same font in PostScript terms can occur	*/
/*  several times in the font list with a different character set and	*/
/*  with a suffix to the family name to differentiate between the	*/
/*  occurences.								*/
/*									*/
/*  NOTE that the name of a font is the key field in this		*/
/*	administration. Character set and family style are dependent	*/
/*	fields.								*/
/*									*/
/*  1)  Find an office character set with the character set given. In	*/
/*	many cases, this does not really make sense: The font is given	*/
/*	with the 'default' character set.				*/
/*  2)  If an office character set was given, derive the family name	*/
/*	from the suffix of the character set. [I.E: remove it from the	*/
/*	name.]								*/
/*  3)  Otherwise, try to find an office character set on the basis	*/
/*	if the suffix to the name.					*/
/*									*/
/************************************************************************/

int docGetFontByNameAndCharset(	DocumentFontList *	dflTo,
				const char *		fontFamilyName,
				int			charset )
    {
    int				i;
    DocumentFont *		dfTo;

    char *			allocated= (char *)0;

    int				charsetWasDefault;
    int				charsetIdx= -1;
    const OfficeCharset *	oc= (const OfficeCharset *)0;

    charsetWasDefault= ( charset == FONTcharsetDEFAULT );

    utilInitializeFontEncodings();

    /*  1  */
    if  ( charset != FONTcharsetDEFAULT )
	{ oc= utilGetOfficeCharsetByCharset( &charsetIdx, charset );	}

    /*  2  */
    if  ( oc )
	{
	const char *		suffix= oc->ocOfficeFontnameSuffix;

	if  ( suffix )
	    {
	    int			lenN= strlen( fontFamilyName );
	    int			lenS= strlen( suffix );

	    if  ( lenN <= lenS					||
		  strcmp( fontFamilyName+ lenN- lenS, suffix )	)
		{
		allocated= malloc( lenN+ lenS+ 1 );
		if  ( ! allocated )
		    { LLXDEB(lenN,lenS,allocated); return -1;	}

		strcpy( allocated,       fontFamilyName );
		strcpy( allocated+ lenN, suffix );

		fontFamilyName= allocated;
		}
	    }
	}
    else{
	/*  3  */
	oc= utilGetOfficeCharsetBySuffix( &charsetIdx, fontFamilyName ); 
	}

    /*  4  */
    dfTo= dflTo->dflFonts;
    for ( i= 0; i < dflTo->dflFontCount; dfTo++, i++ )
	{
	if  ( dfTo->dfDocFontNumber < 0	||
	      ! dfTo->dfName		)
	    { continue;	}


	if  ( ! strcmp( dfTo->dfName, fontFamilyName )	&&
	      dfTo->dfCharset == charset		)
	    { break;	}
	}

    if  ( i >= dflTo->dflFontCount && charsetWasDefault )
	{
	dfTo= dflTo->dflFonts;
	for ( i= 0; i < dflTo->dflFontCount; dfTo++, i++ )
	    {
	    if  ( dfTo->dfDocFontNumber < 0	||
		  ! dfTo->dfName		)
		{ continue;	}

	    if  ( ! strcmp( dfTo->dfName, fontFamilyName ) )
		{ break;	}
	    }
	}

    if  ( i >= dflTo->dflFontCount )
	{
	int			fontFamilyStyle;
	DocumentFont		dfNew;

	fontFamilyStyle= utilFontFamilyStyle( fontFamilyName );

	docInitDocumentFont( &dfNew );
	dfNew.dfCharset= charset;
	dfNew.dfCharsetIndex= charsetIdx;

	if  ( docFontSetFamilyStyle( &dfNew, fontFamilyStyle ) )
	    { LDEB(1); return -1;	}
	if  ( docFontSetFamilyName( &dfNew, fontFamilyName ) )
	    { LDEB(1); return -1;	}

	dfTo= docInsertFont( dflTo, -1, &dfNew );
	if  ( ! dfTo )
	    { LDEB(dflTo->dflFontCount); return -1;	}

	docCleanDocumentFont( &dfNew );
	}

    if  ( allocated )
	{ free( allocated );	}

    return dfTo->dfDocFontNumber;
    }

int docMergeFontIntoFontlist(	DocumentFontList *	dflTo,
				const DocumentFont *	dfFrom )
    {
    int			i;
    DocumentFont *	dfTo;

    dfTo= dflTo->dflFonts;
    for ( i= 0; i < dflTo->dflFontCount; dfTo++, i++ )
	{
	if  ( dfTo->dfDocFontNumber < 0 )
	    { continue;	} 
	if  ( ! dfTo->dfFamilyStyle )
	    { XDEB(dfTo->dfFamilyStyle); continue;	}
	if  ( ! dfTo->dfName )
	    { XDEB(dfTo->dfName); continue;	}

	if  ( ! strcmp( dfTo->dfName, dfFrom->dfName )	&&
	      dfTo->dfCharset == dfFrom->dfCharset	)
	    { break;	}
	}

    if  ( i < dflTo->dflFontCount )
	{ return i;	}

    dfTo= docInsertFont( dflTo, -1, dfFrom );
    if  ( ! dfTo )
	{ XDEB(dfTo); return -1;	}

    return dfTo->dfDocFontNumber;
    }

