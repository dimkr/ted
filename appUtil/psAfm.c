#   include	"appUtilConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	"psFont.h"
#   include	"appSystem.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Bookkeeping and management of AFM information.			*/
/*									*/
/************************************************************************/

void psInitAfmBBox(	AfmBBox *	abb )
    {
    abb->abbLeft= 0;
    abb->abbBottom= 0;
    abb->abbRight= 0;
    abb->abbTop= 0;
    }

void psInitAfmCharMetric(	AfmCharMetric *	acm )
    {
    acm->acmC= -1;
    acm->acmWX= 0;;
    psInitAfmBBox ( &(acm->acmBBox) );
    acm->acmN= (char *)0;
    acm->acmKernPairs= (AfmKerningPair *)0;
    acm->acmKernPairCount= 0;
    }

void psCleanAfmCharMetric(	AfmCharMetric *	acm )
    {
    if  ( acm->acmN )
	{ free( acm->acmN );	}
    if  ( acm->acmKernPairs )
	{ free( acm->acmKernPairs );	}
    }

/************************************************************************/
/*									*/
/*  Initialise an AfmFontInfo						*/
/*									*/
/************************************************************************/

void psInitSupportedCharset(	SupportedCharset *	sc )
    {
    int		i;

    sc->scSupported= 0;
    sc->scNonStandardGlyphNames= 0;

    for ( i= 0; i < 256; i++ )
	{ sc->scCodeToGlyphMapping[i]= 0;	}
    }

void psInitAfmFontInfo(	AfmFontInfo *	afi )
    {
    int		i;

    afi->afiFontName= (char *)0;
    afi->afiFullName= (char *)0;
    afi->afiNotice= (char *)0;
    afi->afiVersion= (char *)0;
    afi->afiFamilyName= (char *)0;
    afi->afiWeight= (char *)0;
    afi->afiWidth= (char *)0;

    afi->afiItalicAngle= 0.0;
    afi->afiTanItalicAngle= 0.0;

    afi->afiIsFixedPitch= 0;
    psInitAfmBBox( &(afi->afiFontBBox) );
    afi->afiUnderlinePosition= 0;
    afi->afiUnderlineThickness= 0;
    afi->afiEncodingScheme= (char *)0;
    afi->afiCapHeight= 0;
    afi->afiXHeight= 0;
    afi->afiAscender= 0;
    afi->afiDescender= 0;
    afi->afiCharacterSet= (char *)0;
    afi->afiVendor= (char *)0;

    afi->afiFontFileName= (char *)0;

    afi->afiMetricCount= 0;
    afi->afiMetrics= (AfmCharMetric *)0;

    for ( i= 0; i < ENCODINGps_COUNT; i++ )
	{ psInitSupportedCharset( &(afi->afiSupportedCharsets[i]) );	}
    for ( i= 0; i < 256; i++ )
	{ afi->afiDefaultCodeToGlyph[i]= -1; }

    afi->afiUseFontspecificEncoding= 0;

    return;
    }

void psCleanAfmFontInfo(	AfmFontInfo *	afi )
    {
    int		i;

    if  ( afi->afiFontName )
	{ free( afi->afiFontName );	}
    if  ( afi->afiFullName )
	{ free( afi->afiFullName );	}
    if  ( afi->afiNotice )
	{ free( afi->afiNotice );	}
    if  ( afi->afiVersion )
	{ free( afi->afiVersion );	}
    if  ( afi->afiFamilyName )
	{ free( afi->afiFamilyName );	}
    if  ( afi->afiWeight )
	{ free( afi->afiWeight );	}
    if  ( afi->afiWidth )
	{ free( afi->afiWidth );	}
    if  ( afi->afiEncodingScheme )
	{ free( afi->afiEncodingScheme );}
    if  ( afi->afiCharacterSet )
	{ free( afi->afiCharacterSet );}
    if  ( afi->afiVendor )
	{ free( afi->afiVendor );}
    if  ( afi->afiFontFileName )
	{ free( afi->afiFontFileName );}

    for ( i= 0; i < afi->afiMetricCount; i++ )
	{ psCleanAfmCharMetric( &(afi->afiMetrics[i]) ); }

    if  ( afi->afiMetrics )
	{ free( afi->afiMetrics );}

    return;
    }

static void psAfmWriteXFontsDir(
			SimpleOutputStream *	sos,
			const char *		prefix,
			const AfmFontInfo *	afi )
    {
    int				i;

    const char *		vendor= "unknown";
    const char *		weight= "medium";
    const char *		slant= "r";
    const char *		width= "normal";
    const char *		fixed= "p";

    char *			weightX= (char *)0;
    char *			widthX= (char *)0;
    char *			s;

    const FontCharset *		fc;

    const char *		filename= "FontFileName";
    int				xWritten= 0;

    if  ( afi->afiFontFileName )
	{ filename= appRelativeName( afi->afiFontFileName );	}
    if  ( afi->afiVendor )
	{ vendor= afi->afiVendor;	}


    if  ( afi->afiItalicAngle < -1.0 )
	{ slant= "i";	}
    if  ( afi->afiIsFixedPitch )
	{ fixed= "m";	}

    if  ( afi->afiWeight && afi->afiWeight[0] )
	{ weight= afi->afiWeight; }
    if  ( afi->afiWidth && afi->afiWidth[0] )
	{ width= afi->afiWidth; }

    if  ( ! strcmp( weight, "Regular" ) )
	{ weight= "medium";	}
    if  ( ! strcmp( weight, "regular" ) )
	{ weight= "medium";	}

    weightX= strdup( weight );
    if  ( weightX )
	{
	s= weightX;
	while( *s )
	    {
	    if  ( isupper( *s ) )
		{ *s= tolower( *s );	}
	    s++;
	    }
	}

    widthX= strdup( width );
    if  ( widthX )
	{
	s= widthX;
	while( *s )
	    {
	    if  ( isupper( *s ) )
		{ *s= tolower( *s );	}
	    s++;
	    }
	}

    fc= PS_Encodings;
    for ( i= 0; i < ENCODINGps_COUNT; fc++, i++ )
	{
	if  ( ! afi->afiSupportedCharsets[i].scSupported )
	    { continue;	}
	if  ( ! fc->fcX11Registry || ! fc->fcX11Encoding )
	    { continue;	}

	sioOutPrintf( sos,
	    "%s%s -%s-%s-%s-%s-%s--0-0-0-0-%s-0-%s-%s\n",
					prefix,
					filename,
					vendor,
					afi->afiFamilyName,
					weightX?weightX:weight,
					slant,
					widthX?widthX:width,
					fixed,
					fc->fcX11Registry,
					fc->fcX11Encoding );

	xWritten++;
	}

    if  ( ! xWritten )
	{
	sioOutPrintf( sos,
	    "%s%s -%s-%s-%s-%s-%s--0-0-0-0-%s-0-%s-%s\n",
					prefix,
					filename,
					vendor,
					afi->afiFamilyName,
					weightX?weightX:weight,
					slant,
					widthX?widthX:width,
					fixed,
					vendor, "fontspecific" );
	}

    if  ( weightX )
	{ free( weightX ); }
    if  ( widthX )
	{ free( widthX ); }

    return;
    }

static void psAfmWriteGSFontmap(
			SimpleOutputStream *	sos,
			const char *		prefix,
			const AfmFontInfo *	afi )
    {
    const char *		filename= "FontFileName";

    if  ( afi->afiFontFileName )
	{ filename= appRelativeName( afi->afiFontFileName );	}

    sioOutPrintf( sos, "%s/%s (%s) ;\n",
			    prefix, afi->afiFontName, filename );

    }

void psWriteAfmFile(	SimpleOutputStream *	sos,
			const AfmFontInfo *	afi )
    {
    int				i;
    const AfmCharMetric *	acm= afi->afiMetrics;
    int				nchars;
    int				map[256];
    int				kernPairCount;

    sioOutPrintf( sos, "StartFontMetrics 4.1\n" );

    if  ( afi->afiFontName )
	{ sioOutPrintf( sos, "FontName %s\n", afi->afiFontName );	}

    if  ( afi->afiFullName )
	{ sioOutPrintf( sos, "FullName %s\n", afi->afiFullName );	}

    if  ( afi->afiFamilyName )
	{ sioOutPrintf( sos, "FamilyName %s\n", afi->afiFamilyName );	}

    if  ( afi->afiWeight )
	{ sioOutPrintf( sos, "Weight %s\n", afi->afiWeight );	}

    if  ( afi->afiWidth )
	{ sioOutPrintf( sos, "Comment Width %s\n", afi->afiWidth );	}

    if  ( afi->afiItalicAngle )
	{ sioOutPrintf( sos, "ItalicAngle %g\n", afi->afiItalicAngle );	}

    sioOutPrintf( sos, "IsFixedPitch %s\n",
				    afi->afiIsFixedPitch?"true":"false" );

    sioOutPrintf( sos, "FontBBox %d %d %d %d\n",
			    afi->afiFontBBox.abbLeft,
			    afi->afiFontBBox.abbBottom,
			    afi->afiFontBBox.abbRight,
			    afi->afiFontBBox.abbTop );

    sioOutPrintf( sos, "UnderlinePosition %g\n", afi->afiUnderlinePosition );
    sioOutPrintf( sos, "UnderlineThickness %g\n", afi->afiUnderlineThickness );

    if  ( afi->afiVersion )
	{ sioOutPrintf( sos, "Version %s\n", afi->afiVersion );	}

    if  ( afi->afiNotice )
	{ sioOutPrintf( sos, "Notice %s\n", afi->afiNotice );	}

    if  ( afi->afiEncodingScheme )
	{
	sioOutPrintf( sos, "EncodingScheme %s\n", afi->afiEncodingScheme );
	}

    if  ( afi->afiCapHeight != 0 )
	{ sioOutPrintf( sos, "CapHeight %d\n", afi->afiCapHeight );	}

    if  ( afi->afiXHeight != 0 )
	{ sioOutPrintf( sos, "XHeight %d\n", afi->afiXHeight );	}

    if  ( afi->afiAscender != 0 )
	{ sioOutPrintf( sos, "Ascender %d\n", afi->afiAscender );	}

    if  ( afi->afiDescender != 0 )
	{ sioOutPrintf( sos, "Descender %d\n", afi->afiDescender );	}

    for ( i= 0; i < 256; i++ )
	{ map[i]= -1;	}

    nchars= 0;
    acm= afi->afiMetrics;
    for ( i= 0; i < afi->afiMetricCount; acm++, i++ )
	{
	if  ( ! acm->acmN )
	    { /*LXDEB(i,acm->acmN);*/ continue;	}

	if  ( acm->acmC >= 0 && acm->acmC < 256 )
	    { map[acm->acmC]= i;	}

	nchars++;
	}

    sioOutPrintf( sos, "StartCharMetrics %d\n", nchars );

    for ( i= 0; i < 256; i++ )
	{
	if  ( map[i] >= 0 )
	    {
	    acm= afi->afiMetrics+ map[i];

	    sioOutPrintf( sos, "C %d ; WX %d ; N %s ; B %d %d %d %d ;\n",
			    acm->acmC,
			    acm->acmWX,
			    acm->acmN,
			    acm->acmBBox.abbLeft,
			    acm->acmBBox.abbBottom,
			    acm->acmBBox.abbRight,
			    acm->acmBBox.abbTop );
	    }
	}

    kernPairCount= 0;
    acm= afi->afiMetrics;
    for ( i= 0; i < afi->afiMetricCount; acm++, i++ )
	{
	if  ( ! acm->acmN )
	    { /*LXDEB(i,acm->acmN);*/ continue;	}

	kernPairCount += acm->acmKernPairCount;

	if  ( acm->acmC >= 0 && acm->acmC < 256 )
	    { continue; }

	sioOutPrintf( sos, "C %d ; WX %d ; N %s ; B %d %d %d %d ;\n",
			acm->acmC,
			acm->acmWX,
			acm->acmN,
			acm->acmBBox.abbLeft,
			acm->acmBBox.abbBottom,
			acm->acmBBox.abbRight,
			acm->acmBBox.abbTop );
	}

    if  ( kernPairCount > 0 )
	{
	sioOutPrintf( sos, "StartKernData\n" );
	sioOutPrintf( sos, "StartKernPairs %d\n", kernPairCount );

	acm= afi->afiMetrics;
	for ( i= 0; i < afi->afiMetricCount; acm++, i++ )
	    {
	    int		k;

	    if  ( ! acm->acmN )
		{ continue;	}

	    for ( k= 0; k < acm->acmKernPairCount; k++ )
		{
		const AfmCharMetric *	acmK;

		if  ( acm->acmKernPairs[k].akpPosition < 0	||
		      acm->acmKernPairs[k].akpPosition >=
					    afi->afiMetricCount	)
		    {
		    LLDEB(acm->acmKernPairs[k].akpPosition,afi->afiMetricCount);
		    continue;
		    }

		acmK= afi->afiMetrics+ acm->acmKernPairs[k].akpPosition;
		if  ( ! acmK->acmN )
		    { XDEB(acmK->acmN); continue;	}

		sioOutPrintf( sos, "KPX %s %s %d\n",
					    acm->acmN, acmK->acmN,
					    acm->acmKernPairs[k].akpXVec );
		}
	    }

	sioOutPrintf( sos, "EndKernPairs\n" );
	sioOutPrintf( sos, "EndKernData\n" );
	}

    sioOutPrintf( sos, "EndCharMetrics\n" );

    if  ( afi->afiVendor )
	{ sioOutPrintf( sos, "Comment Vendor %s\n", afi->afiVendor );	}

    psAfmWriteXFontsDir( sos, "Comment fonts.dir ", afi );
    psAfmWriteGSFontmap( sos, "Comment Fontmap ", afi );

    sioOutPrintf( sos, "EndFontMetrics\n" );

    return;
    }

void psWriteFontInfoDict(	SimpleOutputStream *	sos,
				const AfmFontInfo *	afi )
    {
    sioOutPrintf( sos, "/FontInfo 10 dict dup begin\n" );

    if  ( afi->afiFamilyName )
	{
	sioOutPrintf( sos, "/FamilyName (%s) readonly def\n",
						    afi->afiFamilyName );
	}

    if  ( afi->afiFullName )
	{
	sioOutPrintf( sos, "/FullName (%s) readonly def\n",
						    afi->afiFullName );
	}

    if  ( afi->afiNotice )
	{
	sioOutPrintf( sos, "/Notice (%s) readonly def\n",
						    afi->afiNotice );
	}

    if  ( afi->afiWeight )
	{
	sioOutPrintf( sos, "/Weight (%s) readonly def\n",
						    afi->afiWeight );
	}

    if  ( afi->afiVersion )
	{
	sioOutPrintf( sos, "/Version (%s) readonly def\n",
						    afi->afiVersion );
	}

    sioOutPrintf( sos, "/ItalicAngle %g def\n", afi->afiItalicAngle );

    sioOutPrintf( sos, "/isFixedPitch %s def\n",
				    afi->afiIsFixedPitch?"true":"false" );

    sioOutPrintf( sos, "/UnderlinePosition %g def\n",
					afi->afiUnderlinePosition );
    sioOutPrintf( sos, "/UnderlineThickness %g def\n",
					afi->afiUnderlineThickness );

    sioOutPrintf( sos, "end readonly def\n" );

    return;
    }

int psAfmToXFontsDir(	SimpleOutputStream *	sosFontDir,
			const char *		afmFileName,
			SimpleInputStream *	sisAfm )
    {
    int			rval= 0;
    AfmFontInfo		afi;
    const char *	prefix= "";
    const int		complain= 1;

    psInitAfmFontInfo( &afi );

    if  ( psAfmReadAfm( sisAfm, &afi ) )
	{ SDEB(afmFileName); rval= -1; goto ready;	}

    if  ( psGetFontEncodings( &afi, complain ) )
	{ SDEB(afmFileName); rval= -1; goto ready;	}

    psAfmWriteXFontsDir( sosFontDir, prefix, &afi );

  ready:

    psCleanAfmFontInfo( &afi );

    return rval;
    }

int psAfmToGSFontmap(	SimpleOutputStream *	sosFontDir,
			const char *		afmFileName,
			SimpleInputStream *	sisAfm )
    {
    int			rval= 0;
    AfmFontInfo		afi;
    const char *	prefix= "";

    psInitAfmFontInfo( &afi );

    if  ( psAfmReadAfm( sisAfm, &afi ) )
	{ SDEB(afmFileName); rval= -1; goto ready;	}

    psAfmWriteGSFontmap( sosFontDir, prefix, &afi );

  ready:

    psCleanAfmFontInfo( &afi );

    return rval;
    }
