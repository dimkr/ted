#   include	"appUtilConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	"utilDocFont.h"
#   include	"psFont.h"
#   include	"appSystem.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Set an encoding array for a PostScript font.			*/
/*									*/
/************************************************************************/

static int psSetEncoding(	unsigned char *		pNonStdNames,
				short int *		codeToGlyph,
				const AfmFontInfo *	afi,
				const FontCharset *	fc,
				int			complain )
    {
    int				i;
    int				rval= 0;
    int				missing= 0;
    int				nonStdNameCount= 0;

    const char * const *	glyphNames= fc->fcGlyphNames;
    int				nameCount= fc->fcGlyphCount;

    if  ( nameCount > 256 )
	{ LLDEB(nameCount,256); return -1;	}

    for ( i= 0; i < nameCount; i++ )
	{
	int		j;
	AfmCharMetric *	acm;

	if  ( ! glyphNames[i] )
	    { codeToGlyph[i]= -1; continue;	}

	acm= afi->afiMetrics;
	for ( j= 0; j < afi->afiMetricCount; acm++, j++ )
	    {
	    if  ( acm->acmN && ! strcmp( acm->acmN, glyphNames[i] ) )
		{ break;	}
	    }

	if  ( j >= afi->afiMetricCount )
	    {
	    nonStdNameCount++;
	    j= psFindAlternate( afi, glyphNames[i] );
	    }

	if  ( j < 0 )
	    {
	    missing++;
	    codeToGlyph[i]= -1;
	    rval= 1;
	    }
	else{ codeToGlyph[i]= j;	}
	}

    if  ( complain && missing > 0 )
	{
	appDebug( "FONT %s encoding %s-%s %3d glyphs miss.\n",
				afi->afiFontName,
				fc->fcX11Registry, fc->fcX11Encoding,
				missing );

	if  ( missing <= 20 )
	    {
	    for ( i= 0; i < nameCount; i++ )
		{
		if  ( ! glyphNames[i] )
		    { continue;	}

		if  ( codeToGlyph[i] < 0 )
		    {
		    appDebug( "    %s-%s @%4d=0x%02x NO %s\n",
				    fc->fcX11Registry, fc->fcX11Encoding,
				    i, i ,glyphNames[i] );
		    }
		}
	    }
	}

    while( i < 256 )
	{ codeToGlyph[i++]= -1;	}

    *pNonStdNames= ( nonStdNameCount > 0 );
    return rval;
    }

/************************************************************************/
/*									*/
/*  Try to determine what character sets are supported by a particular	*/
/*  PostScript font.							*/
/*									*/
/*  The positions in the array should match the ENCODINGpsSOMETHING	*/
/*  defines in <utilFontEncoding.h>					*/
/*									*/
/************************************************************************/

static int psTryEncoding(	SupportedCharset *	sc,
				const AfmFontInfo *	afi,
				const FontCharset *	fc,
				int			complain )
    {
    if  ( ! psSetEncoding( &(sc->scNonStandardGlyphNames),
			    sc->scCodeToGlyphMapping, afi, fc, complain ) )
	{ sc->scSupported= 1; return 0; }

    sc->scSupported= 0;
    return -1;
    }

int psGetFontEncodings(	AfmFontInfo *	afi,
			int		complain )
    {
    int				i;
    const FontCharset *		fc;
    SupportedCharset *		sc;
    int				hasSupportedCharset= 0;

    fc= PS_Encodings;
    sc= afi->afiSupportedCharsets;
    for ( i= 0; i < ENCODINGps_COUNT; sc++, fc++, i++ )
	{
	if  ( ! psTryEncoding( sc, afi, fc, 0 ) )
	    { hasSupportedCharset= 1;	}
	}

    if  ( hasSupportedCharset )
	{ return 0;	}

    if  ( complain )
	{
	fc= PS_Encodings;
	sc= afi->afiSupportedCharsets;
	for ( i= 0; i < ENCODINGps_COUNT; sc++, fc++, i++ )
	    { psTryEncoding( sc, afi, fc, 1 ); }
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Calculate the width of a string.					*/
/*									*/
/*  1)  Assume that space is encoded as space.				*/
/*  2)  Contrary to expectations, kerning is an aplication subject. The	*/
/*	printer by itself does not do kerning.				*/
/*									*/
/************************************************************************/

int psCalculateStringExtents(	AfmBBox *		abb,
				const unsigned char *	s,
				int			len,
				int			twipsSize,
				int			withKerning,
				int			encoding,
				const AfmFontInfo *	afi )
    {
    long		right= 0L;
    long		unitsWide= 0L;

    long		top= 0;
    long		bottom= 0;
    long		prevRight= 0;

    const short int *	codeToGlyph= afi->afiDefaultCodeToGlyph;

    if  ( encoding >= 0 )
	{
	if  ( encoding >= ENCODINGps_COUNT	)
	    {
	    SLLDEB(afi->afiFullName,encoding,ENCODINGps_COUNT);
	    return -1;
	    }

	if  ( ! afi->afiSupportedCharsets[encoding].scSupported )
	    {
	    const int supported=
			afi->afiSupportedCharsets[encoding].scSupported;

	    SLLDEB(afi->afiFullName,encoding,supported);

	    for ( encoding= 0; encoding < ENCODINGps_COUNT; encoding++ )
		{
		if  ( afi->afiSupportedCharsets[encoding].scSupported )
		    { break;	}
		}

	    if  ( encoding >= ENCODINGps_COUNT )
		{ LDEB(ENCODINGps_COUNT); return -1;	}
	    }

	codeToGlyph= afi->afiSupportedCharsets[encoding].scCodeToGlyphMapping;
	}

    while( len > 0 )
	{
	int		c= *(s++);
	AfmCharMetric *	acm;

	c= codeToGlyph[c];
	if  ( c < 0 )
	    {
	    c= ' ';
	    c= codeToGlyph[c];

	    if  ( c < 0 )
		{ LDEB(c); return -1;	}
	    }

	acm= afi->afiMetrics+ c;
	if  ( top < acm->acmBBox.abbTop )
	    { top= acm->acmBBox.abbTop;	}
	if  ( bottom > acm->acmBBox.abbBottom )
	    { bottom= acm->acmBBox.abbBottom;	}

	prevRight= unitsWide;
	unitsWide += acm->acmWX;

	/*  2  */
	if  ( withKerning && len > 1 )
	    {
	    int			cc= *s;

	    cc= codeToGlyph[cc];

	    if  ( c >= 0 )
		{
		int			j;
		AfmKerningPair *	akp= acm->acmKernPairs;

		for ( j= 0; j < acm->acmKernPairCount; akp++, j++ )
		    {
		    if  ( akp->akpPosition == cc )
			{ unitsWide += akp->akpXVec; break; }
		    }
		}
	    }

	if  ( c != ' ' )
	    { right= prevRight+ acm->acmBBox.abbRight; }

	len--;
	}

    abb->abbLeft= 0;
    abb->abbTop= ( twipsSize* top+ 500 )/1000;
    abb->abbBottom= ( twipsSize* bottom+ 500 )/1000;
    abb->abbRight= ( twipsSize* right+ 500 )/1000;

    return ( twipsSize* unitsWide+ 500 )/1000;
    }

/************************************************************************/
/*									*/
/*  Initialise a font family struct.					*/
/*									*/
/************************************************************************/

void docInitFontFamily(	AppFontFamily *	aff )
    {
    aff->affFontFamilyName_Orig= (char *)0;
    aff->affFontFamilyName= (char *)0;
    aff->affFaces= (AppFontTypeface *)0;
    aff->affFaceCount= 0;
    aff->affHasFixedWidth= 0;
    aff->affHasProportionalWidth= 0;
    aff->affDefaultEncoding= -1;

    return;
    }

void docInitFontTypeface(	AppFontTypeface *	aft )
    {
    int		enc;

    aft->aftFaceName= (char *)0;
    aft->aftIsBold= 0;
    aft->aftIsSlanted= 0;
    aft->aftFontInfo= (AfmFontInfo *)0;

    aft->aftFontSpecificQueryFormat= (char *)0;
    for ( enc= 0; enc < ENCODINGps_COUNT; enc++ )
	{ aft->aftXQueryFormats[enc]= (char *)0;	}

    aft->aftWidth= FONTwidthMEDIUM;
    aft->aftWeight= FONTweightMEDIUM;
    aft->aftDefaultEncoding= -1;

    return;
    }

