#   include	"appUtilConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	"utilFontEncoding.h"
#   include	<charnames.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  List of more or less actively supported font encodings.		*/
/*									*/
/************************************************************************/

static const OfficeCharsetMapping PS_DingbatsMapping=
    {
    ENCODINGpsADOBE_DINGBATS,
    (const unsigned char *)0,
    (const unsigned char *)0,
    };

FontCharset PS_Encodings[ENCODINGps_COUNT]=
{
    { psIsoLatin1GlyphNames,	256,
			    "-iso1", "Enc-iso1",
			    FONTcharsetANSI, FONTcodepageANSI,
			    "iso8859", "1",
			    docWIN1252_to_ISO1, docISO1_to_WIN1252,
			    },
    { psIsoLatin2GlyphNames,	256,
			    "-iso2", "Enc-iso2",
			    FONTcharsetEE, FONTcodepageEE,
			    "iso8859", "2",
			    docWIN1250_to_ISO2, docISO2_to_WIN1250,
			    },
    { psSymbolGlyphNames,	256,
			    "-symb", "Enc-symb",
			    FONTcharsetSYMBOL, -1,
			    "adobe", "fontspecific",
			    (const unsigned char *)0, (const unsigned char *)0, 
			    },
    { psIsoLatin5GlyphNames,	256,
			    "-cyr", "Enc-cyr",
			    FONTcharsetRUSSIAN, FONTcodepageRUSSIAN,
			    "iso8859", "5",
			    docWIN1251_to_ISO5, docISO5_to_WIN1251,
			    },
    { psDingbatGlyphNames,	256,
			    "-ding", "Enc-ding",
			    FONTcharsetDEFAULT, -1,
			    "urw", "fontspecific",
			    (const unsigned char *)0, (const unsigned char *)0, 
			    &PS_DingbatsMapping,
			    },
    { psIsoLatin7GlyphNames,	256,
			    "-iso7", "Enc-iso7",
			    FONTcharsetGREEK, FONTcodepageGREEK,
			    "iso8859", "7",
			    docWIN1253_to_ISO7, docISO7_to_WIN1253,
			    },
    { psIsoLatin9GlyphNames,	256,
			    "-iso9", "Enc-iso9",
			    FONTcharsetTURKISH, FONTcodepageTURKISH,
			    "iso8859", "9",
			    docWIN1254_to_ISO9, docISO9_to_WIN1254,
			    },
    { psIsoLatin13GlyphNames,	256,
			    "-iso13", "Enc-iso13",
			    FONTcharsetBALTIC, FONTcodepageBALTIC,
			    "iso8859", "13",
			    docWIN1257_to_ISO13, docISO13_to_WIN1257,
			    },
    { psIsoLatin15GlyphNames,	256,
			    "-iso15", "Enc-iso15",
			    FONTcharsetANSI, FONTcodepageANSI,
			    "iso8859", "15",
			    docWIN1252_to_ISO15, docISO15_to_WIN1252,
			    },
};

static const OfficeCharsetMapping PS_OfficeCharMappingsANSI[]=
{
    { ENCODINGpsISO_8859_15,	docWIN1252_to_ISO15,
				docISO15_to_WIN1252,	},
    { ENCODINGpsISO_8859_1,	docWIN1252_to_ISO1,
				docISO1_to_WIN1252,	},
};

static const OfficeCharsetMapping PS_OfficeCharMappingsEE[]=
{
    { ENCODINGpsISO_8859_2,	docWIN1250_to_ISO2,
				docISO2_to_WIN1250,	},
};

static const OfficeCharsetMapping PS_OfficeCharMappingsSYMBOL[]=
{
    { ENCODINGpsADOBE_SYMBOL,	(const unsigned char *)0,
				(const unsigned char *)0,	},
    { ENCODINGpsADOBE_DINGBATS,	(const unsigned char *)0,
				(const unsigned char *)0,	},
};

static const OfficeCharsetMapping PS_OfficeCharMappingsRUSSIAN[]=
{
    { ENCODINGpsADOBE_CYRILLIC,	docWIN1251_to_ISO5,
				docISO5_to_WIN1251,		},
};

static const OfficeCharsetMapping PS_OfficeCharMappingsDEFAULT[]=
{
    { ENCODINGpsFONTSPECIFIC,	(const unsigned char *)0,
				(const unsigned char *)0,	},
};

static const OfficeCharsetMapping PS_OfficeCharMappingsDINGBATS[]=
{
    { ENCODINGpsADOBE_DINGBATS,	(const unsigned char *)0,
				(const unsigned char *)0,	},
};

static const OfficeCharsetMapping PS_OfficeCharMappingsGREEK[]=
{
    { ENCODINGpsISO_8859_7,	docWIN1253_to_ISO7,
				docISO7_to_WIN1253,		},
};

static const OfficeCharsetMapping PS_OfficeCharMappingsTURKISH[]=
{
    { ENCODINGpsISO_8859_9,	docWIN1254_to_ISO9,
				docISO9_to_WIN1254,		},
};

static const OfficeCharsetMapping PS_OfficeCharMappingsBALTIC[]=
{
    { ENCODINGpsISO_8859_13,	docWIN1257_to_ISO13,
				docISO13_to_WIN1257,		},
};

static const OfficeCharsetMapping PS_OfficeCharMappingsMAC_ROMAN[]=
{
    { ENCODINGpsISO_8859_15,	docMAC_ROMAN_to_ISO15,
				docISO15_to_MAC_ROMAN,	},
    { ENCODINGpsISO_8859_1,	docMAC_ROMAN_to_ISO1,
				docISO1_to_MAC_ROMAN,	},
};

# define MAPPINGS(x) (sizeof(x)/sizeof(OfficeCharsetMapping)), x
# define AS_IS (-1)

static const OfficeCharset PS_OfficeCharsets[]= 
{
    { FONTcharsetANSI,	AS_IS, FONTcodepageANSI,
			(const char *)0,
			"encodingLatin1", "Latin 1",
			MAPPINGS(PS_OfficeCharMappingsANSI),
			},
    { FONTcharsetEE,	AS_IS, FONTcodepageEE,
			" CE",
			"encodingLatin2", "Latin 2",
			MAPPINGS(PS_OfficeCharMappingsEE),
			},
    { FONTcharsetSYMBOL, AS_IS, -1,
			(const char *)0,
			"encodingSymbol", "Symbol",
			MAPPINGS(PS_OfficeCharMappingsSYMBOL),
			},
    { FONTcharsetRUSSIAN, AS_IS, FONTcodepageRUSSIAN,
			" Cyr",
			"encodingCyrillic", "Cyrillic",
			MAPPINGS(PS_OfficeCharMappingsRUSSIAN),
			},
    { FONTcharsetDEFAULT, AS_IS, -1,
			(const char *)0,
			"encodingDefault", "Default",
			MAPPINGS(PS_OfficeCharMappingsDEFAULT),
			},
    { FONTcharsetGREEK, AS_IS, FONTcodepageGREEK,
			" Greek",
			"encodingLatin7", "Greek",
			MAPPINGS(PS_OfficeCharMappingsGREEK),
			},
    { FONTcharsetTURKISH, AS_IS, FONTcodepageTURKISH,
			" Tur",
			"encodingLatin9", "Turkish",
			MAPPINGS(PS_OfficeCharMappingsTURKISH),
			},
    { FONTcharsetBALTIC, AS_IS, FONTcodepageBALTIC,
			" Baltic",
			"encodingLatin13", "Baltic",
			MAPPINGS(PS_OfficeCharMappingsBALTIC),
			},

    { FONTcharsetMAC_ROMAN, FONTcharsetANSI, FONTcodepageMAC_ROMAN,
			(const char *)0,
			"encodingMacRoman", "MacRoman",
			MAPPINGS(PS_OfficeCharMappingsMAC_ROMAN),
			},

    { FONTcharsetMAC_256, FONTcharsetANSI, FONTcodepageMAC_ROMAN,
			(const char *)0,
			"encodingMacRoman256", "MacRoman256",
			MAPPINGS(PS_OfficeCharMappingsMAC_ROMAN),
			},

    /*  FONTcharsetHEBREW	*/
    /*  FONTcharsetARABIC	*/
    /*  FONTcharsetSHIFTJIS	*/
    /*  FONTcharsetHANGEUL	*/
    /*  FONTcharsetGB2313	*/
    /*  FONTcharsetCHINESEBIG5	*/
};

const OfficeCharset * utilGetOfficeCharsetByCharset(
					int *		pIdx,
					int		charset )
    {
    int				i;
    const OfficeCharset *	oc;
    int				n;

    n= sizeof(PS_OfficeCharsets)/sizeof(OfficeCharset);

    if  ( n > CHARSETidxCOUNT )
	{
	LLDEB(n,CHARSETidxCOUNT);
	*pIdx= -1; return (const OfficeCharset *)0;
	}

    oc= PS_OfficeCharsets;
    for ( i= 0; i < sizeof(PS_OfficeCharsets)/sizeof(OfficeCharset); oc++, i++ )
	{
	if  ( oc->ocOfficeCharset == charset )
	    { *pIdx= i; return oc;	}
	}

    *pIdx= -1;
    return (const OfficeCharset *)0;
    }

const OfficeCharset * utilGetOfficeCharsetByIndex(
					int		charsetIdx )
    {
    int				n;

    n= sizeof(PS_OfficeCharsets)/sizeof(OfficeCharset);

    if  ( n > CHARSETidxCOUNT )
	{ LLDEB(n,CHARSETidxCOUNT); return (const OfficeCharset *)0; }

    if  ( charsetIdx < 0 || charsetIdx >= n )
	{ LLDEB(charsetIdx,n); return (const OfficeCharset *)0; }

    return PS_OfficeCharsets+ charsetIdx;
    }

const OfficeCharset * utilGetOfficeCharsetBySuffix(
					int *		pIdx,
					const char *	fontname )
    {
    int				i;
    const OfficeCharset *	oc;
    int				n;

    int				lenN= strlen( fontname );

    n= sizeof(PS_OfficeCharsets)/sizeof(OfficeCharset);

    if  ( n > CHARSETidxCOUNT )
	{
	LLDEB(n,CHARSETidxCOUNT);
	*pIdx= -1; return (const OfficeCharset *)0;
	}

    oc= PS_OfficeCharsets;
    for ( i= 0; i < sizeof(PS_OfficeCharsets)/sizeof(OfficeCharset); oc++, i++ )
	{
	int		lenS;

	if  ( ! oc->ocOfficeFontnameSuffix )
	    { continue;	}

	lenS= strlen( oc->ocOfficeFontnameSuffix );

	if  ( lenN > lenS						&&
	      ! strcmp( fontname+ lenN- lenS,
					oc->ocOfficeFontnameSuffix )	)
	    { *pIdx= i; return oc;	}
	}

    *pIdx= -1;
    return (const OfficeCharset *)0;
    }

/************************************************************************/
/*									*/
/*  Return our style encoding from X11 registry, encoding combination.	*/
/*									*/
/*  1)  Hacks for known fonts.						*/
/*  2)  Find registry and encoding in name.				*/
/*  3)  Nothing can be done about the fact that the fontspecific X11	*/
/*	encodings return silly results.					*/
/*  4)  Table lookup.							*/
/*									*/
/************************************************************************/

int utilEncodingFromX11FontName(	const char *	x11name )
    {
    int			reg_enc= -1;
    int			enc= -1;
    int			rl;
    int			j;
    int			d;
    int			encoding;

    /*  1  */

    /*  2  */
    j= 0; d= 0;
    while( x11name[j] )
	{
	if  ( x11name[j] == '-' )
	    {
	    if  ( d == 12 )
		{ reg_enc= j+ 1;	}
	    if  ( d == 13 )
		{ enc= j+ 1;	}

	    d++;
	    }

	j++;
	}

    if  ( reg_enc < 0 || enc < 0 )
	{ SDEB(x11name); return -1;	}

    /*  3 
    if  ( ! strcmp( x11name+ enc, "fontspecific" ) )
	{ return -1;	}
    */

    rl= enc- reg_enc- 1;

    /*  4  */
    for ( encoding= 0; encoding < ENCODINGps_COUNT; encoding++ )
	{
	if  ( ! PS_Encodings[encoding].fcX11Encoding )
	    { continue;	}
	if  ( ! PS_Encodings[encoding].fcX11Registry )
	    { continue;	}

	if  ( strcmp( PS_Encodings[encoding].fcX11Encoding, x11name+ enc ) )
	    { continue;	}
	if  ( strncmp( PS_Encodings[encoding].fcX11Registry,
						    x11name+ reg_enc, rl ) )
	    { continue;	}
	if  ( PS_Encodings[encoding].fcX11Registry[rl] )
	    { continue;	}

	return encoding;
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Return our style encoding for a windows codepage.			*/
/*									*/
/************************************************************************/

int utilEncodingFromWindowsCodepage(	int	codepage )
    {
    int			i;
    const FontCharset *	fc;

    fc= PS_Encodings;
    for ( i= 0; i < ENCODINGps_COUNT; fc++, i++ )
	{
	if  ( fc->fcOfficeCodepage == codepage )
	    { return i; }
	}

    /** ??
    switch( codepage )
	{
	case FONTcodepagePC_437:	return ENCODINGpsISO_8859_1;
	case FONTcodepageMAC_X:		return ENCODINGpsISO_8859_1;
	default:
	    break;
	}
    */

    return ENCODINGpsFONTSPECIFIC;
    }

int utilWindowsCodepageFromEncoding(	int	encoding )
    {
    if  ( encoding >= 0 && encoding < ENCODINGps_COUNT )
	{ return PS_Encodings[encoding].fcOfficeCodepage; }

    LLDEB(encoding,ENCODINGps_COUNT); return -1;
    }

/************************************************************************/
/*									*/
/*  Return the (Windows) character set of a (Windows) codepage.		*/
/*									*/
/************************************************************************/

int utilWindowsCharsetFromCodepage(	int	codepage )
    {
    switch( codepage )
	{
	case FONTcodepagePC_437:	return FONTcharsetPC_437;
	case FONTcodepageTHAI:		return FONTcharsetTHAI;
	case FONTcodepageSHIFTJIS:	return FONTcharsetSHIFTJIS;
	case FONTcodepageGB2313:	return FONTcharsetGB2313;
	case FONTcodepageHANGEUL:	return FONTcharsetHANGEUL;
	case FONTcodepageCHINESEBIG5:	return FONTcharsetCHINESEBIG5;
	case FONTcodepageEE:		return FONTcharsetEE;
	case FONTcodepageRUSSIAN:	return FONTcharsetRUSSIAN;
	case FONTcodepageANSI:		return FONTcharsetANSI;
	case FONTcodepageGREEK:		return FONTcharsetGREEK;
	case FONTcodepageTURKISH:	return FONTcharsetTURKISH;
	case FONTcodepageHEBREW:	return FONTcharsetHEBREW;
	case FONTcodepageARABIC:	return FONTcharsetARABIC;
	case FONTcodepageBALTIC:	return FONTcharsetBALTIC;
	case FONTcodepageVIETNAMESE:	return FONTcharsetVIETNAMESE;
	case FONTcodepageJOHAB:		return FONTcharsetJOHAB;
	case FONTcodepageMAC_ROMAN:	return FONTcharsetMAC_ROMAN;
	}

    LDEB(codepage); return -1;
    }

/************************************************************************/
/*									*/
/*  Remember the character classification information for a particular	*/
/*  character set encoding.						*/
/*									*/
/************************************************************************/

void utilSetLatin1CharacterKinds(	unsigned char	charKinds[256],
					unsigned char	charShifts[256] )
    {
    int		i;

    for ( i= 0; i < 256; i++ )
	{
	if  ( ISO1_islower( i ) )
	    {
	    charKinds[i] |= CHARisLOWER;
	    if  ( ! ISO1_isupper( i ) )
		{ charShifts[i]= ISO1_toupper( i );	}
	    }

	if  ( ISO1_isupper( i ) )
	    {
	    charKinds[i] |= CHARisUPPER;
	    if  ( ! ISO1_islower( i ) )
		{ charShifts[i]= ISO1_tolower( i );	}
	    }

	if  ( ISO1_isdigit( i ) )
	    { charKinds[i] |= CHARisDIGIT;	}
	}

    return;
    }

void utilSetLatin15CharacterKinds(	unsigned char	charKinds[256],
					unsigned char	charShifts[256] )
    {
    int		i;

    for ( i= 0; i < 256; i++ )
	{
	if  ( ISO15_islower( i ) )
	    {
	    charKinds[i] |= CHARisLOWER;
	    if  ( ! ISO15_isupper( i ) )
		{ charShifts[i]= ISO15_toupper( i );	}
	    }

	if  ( ISO15_isupper( i ) )
	    {
	    charKinds[i] |= CHARisUPPER;
	    if  ( ! ISO15_islower( i ) )
		{ charShifts[i]= ISO15_tolower( i );	}
	    }

	if  ( ISO15_isdigit( i ) )
	    { charKinds[i] |= CHARisDIGIT;	}
	}

    return;
    }

void utilSetLatin2CharacterKinds(	unsigned char	charKinds[256],
					unsigned char	charShifts[256] )
    {
    int		i;

    for ( i= 0; i < 256; i++ )
	{
	if  ( ISO2_islower( i ) )
	    {
	    charKinds[i] |= CHARisLOWER;
	    if  ( ! ISO2_isupper( i ) )
		{ charShifts[i]= ISO2_toupper( i );	}
	    }

	if  ( ISO2_isupper( i ) )
	    {
	    charKinds[i] |= CHARisUPPER;
	    if  ( ! ISO2_islower( i ) )
		{ charShifts[i]= ISO2_tolower( i );	}
	    }

	if  ( ISO2_isdigit( i ) )
	    { charKinds[i] |= CHARisDIGIT;	}
	}

    return;
    }

void utilSetLatin5CharacterKinds(	unsigned char	charKinds[256],
					unsigned char	charShifts[256] )
    {
    int		i;

    for ( i= 0; i < 256; i++ )
	{
	if  ( ISO5_islower( i ) )
	    {
	    charKinds[i] |= CHARisLOWER;
	    if  ( ! ISO5_isupper( i ) )
		{ charShifts[i]= ISO5_toupper( i );	}
	    }

	if  ( ISO5_isupper( i ) )
	    {
	    charKinds[i] |= CHARisUPPER;
	    if  ( ! ISO5_islower( i ) )
		{ charShifts[i]= ISO5_tolower( i );	}
	    }

	if  ( ISO5_isdigit( i ) )
	    { charKinds[i] |= CHARisDIGIT;	}
	}

    return;
    }

void utilSetLatin7CharacterKinds(	unsigned char	charKinds[256],
					unsigned char	charShifts[256] )
    {
    int		i;

    for ( i= 0; i < 256; i++ )
	{
	if  ( ISO7_islower( i ) )
	    {
	    charKinds[i] |= CHARisLOWER;
	    if  ( ! ISO7_isupper( i ) )
		{ charShifts[i]= ISO7_toupper( i );	}
	    }

	if  ( ISO7_isupper( i ) )
	    {
	    charKinds[i] |= CHARisUPPER;
	    if  ( ! ISO7_islower( i ) )
		{ charShifts[i]= ISO7_tolower( i );	}
	    }

	if  ( ISO7_isdigit( i ) )
	    { charKinds[i] |= CHARisDIGIT;	}
	}

    return;
    }

void utilSetLatin9CharacterKinds(	unsigned char	charKinds[256],
					unsigned char	charShifts[256] )
    {
    int		i;

    for ( i= 0; i < 256; i++ )
	{
	if  ( ISO9_islower( i ) )
	    {
	    charKinds[i] |= CHARisLOWER;
	    if  ( ! ISO9_isupper( i ) )
		{ charShifts[i]= ISO9_toupper( i );	}
	    }

	if  ( ISO9_isupper( i ) )
	    {
	    charKinds[i] |= CHARisUPPER;
	    if  ( ! ISO9_islower( i ) )
		{ charShifts[i]= ISO9_tolower( i );	}
	    }

	if  ( ISO9_isdigit( i ) )
	    { charKinds[i] |= CHARisDIGIT;	}
	}

    return;
    }

void utilSetLatin13CharacterKinds(	unsigned char	charKinds[256],
					unsigned char	charShifts[256] )
    {
    int		i;

    for ( i= 0; i < 256; i++ )
	{
	if  ( ISO13_islower( i ) )
	    {
	    charKinds[i] |= CHARisLOWER;
	    if  ( ! ISO13_isupper( i ) )
		{ charShifts[i]= ISO13_toupper( i );	}
	    }

	if  ( ISO13_isupper( i ) )
	    {
	    charKinds[i] |= CHARisUPPER;
	    if  ( ! ISO13_islower( i ) )
		{ charShifts[i]= ISO13_tolower( i );	}
	    }

	if  ( ISO13_isdigit( i ) )
	    { charKinds[i] |= CHARisDIGIT;	}
	}

    return;
    }

void utilSetKoi8rCharacterKinds(	unsigned char	charKinds[256],
					unsigned char	charShifts[256] )
    {
    int		i;

    for ( i= 0; i < 256; i++ )
	{
	if  ( KOI8R_islower( i ) )
	    {
	    charKinds[i] |= CHARisLOWER;
	    if  ( ! KOI8R_isupper( i ) )
		{ charShifts[i]= KOI8R_toupper( i );	}
	    }

	if  ( KOI8R_isupper( i ) )
	    {
	    charKinds[i] |= CHARisUPPER;
	    if  ( ! KOI8R_islower( i ) )
		{ charShifts[i]= KOI8R_tolower( i );	}
	    }

	if  ( KOI8R_isdigit( i ) )
	    { charKinds[i] |= CHARisDIGIT;	}
	}

    return;
    }

void utilSetSymbolCharacterKinds(	unsigned char	charKinds[256],
					unsigned char	charShifts[256] )
    {
    int		i;

    for ( i= 0; i < 256; i++ )
	{
	if  ( PSSYM_islower( i ) )
	    {
	    charKinds[i] |= CHARisLOWER;
	    if  ( ! PSSYM_isupper( i ) )
		{ charShifts[i]= PSSYM_toupper( i );	}
	    }

	if  ( PSSYM_isupper( i ) )
	    {
	    charKinds[i] |= CHARisUPPER;
	    if  ( ! PSSYM_islower( i ) )
		{ charShifts[i]= PSSYM_tolower( i );	}
	    }

	if  ( PSSYM_isdigit( i ) )
	    { charKinds[i] |= CHARisDIGIT;	}
	}

    return;
    }

void utilSetEncodingCharacterKinds(	unsigned char	charKinds[256],
					unsigned char	charShifts[256],
					int		encoding )
    {
    int		i;

    memset( charKinds, 0, 256 );
    for ( i= 0; i < 256; i++ )
	{ charShifts[i]= i; }

    switch( encoding )
	{
	case ENCODINGpsFONTSPECIFIC:
	    /*  impossible */
	    break;

	case ENCODINGpsISO_8859_1:
	    utilSetLatin1CharacterKinds( charKinds, charShifts );
	    break;

	case ENCODINGpsISO_8859_2:
	    utilSetLatin2CharacterKinds( charKinds, charShifts );
	    break;

	case ENCODINGpsADOBE_SYMBOL:
	    utilSetSymbolCharacterKinds( charKinds, charShifts );
	    break;

	case ENCODINGpsADOBE_CYRILLIC:
	    utilSetKoi8rCharacterKinds( charKinds, charShifts );
	    break;

	case ENCODINGpsADOBE_DINGBATS:
	    /*  no digits, no letters */
	    break;

	case ENCODINGpsISO_8859_7:
	    utilSetLatin7CharacterKinds( charKinds, charShifts );
	    break;

	case ENCODINGpsISO_8859_9:
	    utilSetLatin9CharacterKinds( charKinds, charShifts );
	    break;

	case ENCODINGpsISO_8859_13:
	    utilSetLatin13CharacterKinds( charKinds, charShifts );
	    break;

	case ENCODINGpsISO_8859_15:
	    utilSetLatin15CharacterKinds( charKinds, charShifts );
	    break;

	default:
	    LDEB(encoding);
	    break;
	}

    return;
    }

# define CHECK_BIJ 0
# if CHECK_BIJ
static void utilCheckMapBijectivity(	const FontCharset *	fc )
    {
    int		i;
    int		o;

    if  ( ! fc->fcFromOfficeToX11	||
	  ! fc->fcFromX11ToOffice	)
	{ return;	}

    for ( i= 0; i < 256; i++ )
	{
	int		io;
	int		ioi;

	if  ( i > fc->fcGlyphCount )
	    { break;	}
	if  ( ! fc->fcGlyphNames[i] )
	    { continue;	}

	io= fc->fcFromX11ToOffice[i]; ioi= fc->fcFromOfficeToX11[io];
	if  ( ioi != i )
	    {
	    appDebug( "%s-%s: %3d -io> %3d -oi> %3d (%s ->> %s)\n",
		    fc->fcX11Registry, fc->fcX11Encoding,
		    i, io, ioi, fc->fcGlyphNames[i],
		    (ioi>=0&&ioi<fc->fcGlyphCount&&fc->fcGlyphNames[ioi])?
			fc->fcGlyphNames[ioi]:"{{?}}" );
	    }
	}

    for ( o= 0; o < 256; o++ )
	{
	int		oi;
	int		oio;

	oi= fc->fcFromOfficeToX11[o]; oio= fc->fcFromX11ToOffice[oi];

	if  ( oio != o )
	    {
	    appDebug( "%s-%s: %3d -oi> %3d -io> %3d (-> %s ->)\n",
		    fc->fcX11Registry, fc->fcX11Encoding,
		    o, oi, oio,
		    (oi>=0&&oi<fc->fcGlyphCount&&fc->fcGlyphNames[oi])?
			fc->fcGlyphNames[oi]:"{{?}}" );
	    }
	}
    }
# endif

void utilInitializeFontEncodings( void )
    {
    int		enc;

    static int	done= 0;

    if  ( done )
	{ return;	}

    for ( enc= 0; enc < ENCODINGps_COUNT; enc++ )
	{
	utilSetEncodingCharacterKinds( PS_Encodings[enc].fcCharKinds,
					PS_Encodings[enc].fcCharShifts, enc );
#	if CHECK_BIJ
	utilCheckMapBijectivity( PS_Encodings+ enc );
#	endif
	}

    done= 1;

    return;
    }

