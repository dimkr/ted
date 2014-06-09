#   include	"appUtilConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	<locale.h>

#   include	"sioStdio.h"
#   include	"utilDocFont.h"
#   include	"psFont.h"
#   include	"utilMatchFont.h"
#   include	"appSystem.h"
#   include	"utilT1Entries.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  HACK: map some (URW) names to standard font names (ADOBE base 35).	*/
/*									*/
/************************************************************************/

typedef struct NameMap
    {
    const char *	nmFrom;
    const char *	nmTo;
    } NameMap;

static const NameMap PS_FamilyMap[]=
    {
	{ "Century Schoolbook L",	"New Century Schoolbook"	},
	{ "Nimbus Mono L",		"Courier"			},
	{ "Nimbus Roman No9 L",		"Times"				},
	{ "Nimbus Sans L",		"Helvetica"			},
	{ "Standard Symbols L",		"Symbol"			},
	{ "URW Antiqua T",		"New Century Schoolbook"	},
	{ "URW Bookman L",		"ITC Bookman"			},
	{ "URW Chancery L",		"ITC Zapf Chancery"		},
	{ "URW Gothic L",		"ITC Avant Garde Gothic"	},
	{ "URW Palladio L",		"Palatino"			},
	{ "Dingbats",			"ITC Zapf Dingbats"		},
    };

static const NameMap PS_FullMap[]=
    {
    { "URW Gothic L Book",  "ITC Avant Garde Gothic Book"	},
    { "URW Gothic L Book Oblique",
			    "ITC Avant Garde Gothic Book Oblique"	},
    { "URW Gothic L Demi",  "ITC Avant Garde Gothic Demi"	},
    { "URW Gothic L Demi Oblique",
			    "ITC Avant Garde Gothic Demi Oblique"	},
    { "URW Bookman L Light",
			    "ITC Bookman Light"	},
    { "URW Bookman L Light Italic",
			    "ITC Bookman Light Italic"	},
    { "URW Bookman L Demi Bold",
			    "ITC Bookman Demi"	},
    { "URW Bookman L Demi Bold Italic",
			    "ITC Bookman Demi Italic"	},
    { "Nimbus Mono L Regular",
			    "Courier"	},
    { "Nimbus Mono L Regular Oblique",
			    "Courier Oblique"	},
    { "Nimbus Mono L Bold", "Courier Bold"	},
    { "Nimbus Mono L Bold Oblique",
			    "Courier Bold Oblique"	},
    { "Nimbus Sans L Regular",
			    "Helvetica"	},
    { "Nimbus Sans L Regular Italic",
			    "Helvetica Oblique"	},
    { "Nimbus Sans L Bold", "Helvetica Bold"	},
    { "Nimbus Sans L Bold Italic",
			    "Helvetica Bold Oblique"	},
    { "Nimbus Sans L Regular Condensed",
			    "Helvetica Narrow"	},
    { "Nimbus Sans L Regular Condensed Italic" ,
			    "Helvetica Narrow Oblique"	},
    { "Nimbus Sans L Bold Condensed",
			    "Helvetica Narrow Bold"	},
    { "Nimbus Sans L Bold Condensed Italic",
			    "Helvetica Narrow Bold Oblique"	},
    { "Century Schoolbook L Roman",
			    "New Century Schoolbook Roman"	},
    { "Century Schoolbook L Italic",
			    "New Century Schoolbook Italic"	},
    { "Century Schoolbook L Bold",
			    "New Century Schoolbook Bold"	},
    { "Century Schoolbook L Bold Italic",
			    "New Century Schoolbook Bold Italic"	},
    { "URW Palladio L Roman",
			    "Palatino Roman"	},
    { "URW Palladio L Italic",
			    "Palatino Italic"	},
    { "URW Palladio L Bold",
			    "Palatino Bold"	},
    { "URW Palladio L Bold Italic",
			    "Palatino Bold Italic"	},
    { "Standard Symbols L", "Symbol"	},
    { "Nimbus Roman No9 L Regular",
			    "Times Roman"	},
    { "Nimbus Roman No9 L Regular Italic",
			    "Times Italic"	},
    { "Nimbus Roman No9 L Medium",
			    "Times Bold"	},
    { "Nimbus Roman No9 L Medium Italic",
			    "Times Bold Italic"	},
    { "URW Chancery L Medium Italic",
			    "ITC Zapf Chancery Medium Italic"	},
    { "Dingbats",	    "ITC Zapf Dingbats"	},
    };

static void psFillFaceFromInfo(		AppFontTypeface *	aft,
					const AfmFontInfo *	afi )
    {
    int		i;

    int		widthStart;
    int		widthLength;

    int		weightStart;
    int		weightLength;

    int		w;

    if  ( afi->afiWeight )
	{
	w= aft->aftWeight;

	if  ( ! utilFontWeightFromString( &w,
			    &weightStart, &weightLength, afi->afiWeight ) )
	    { SSDEB(afi->afiFontName,afi->afiWeight);	}

	aft->aftWeight= w;
	}

    if  ( aft->aftWeight > FONTweightMEDIUM )
	{ aft->aftIsBold= 1;	}
    else{ aft->aftIsBold= 0;	}

    if  ( afi->afiItalicAngle < -1.0 )
	{ aft->aftIsSlanted= 1; }
    else{ aft->aftIsSlanted= 0;	}

    w= aft->aftWidth;
    utilFontWidthFromString( &w, &widthStart, &widthLength, afi->afiFullName );
    aft->aftWidth= w;

    for ( i= 0; i < ENCODINGps_COUNT; i++ )
	{
	if  ( afi->afiSupportedCharsets[i].scSupported  )
	    { aft->aftDefaultEncoding= i; break;	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Compare routine for sorting the faces of a font.			*/
/*									*/
/*  Serves two purposes:						*/
/*									*/
/*  1)  Present them in a predictable order in the font chooser.	*/
/*  2)  Makes it easier to present width variants as different		*/
/*	families.							*/
/*									*/
/************************************************************************/

static int psFontCompareFaces(	const AppFontTypeface *		aft1,
				const AppFontTypeface *		aft2,
				const AfmFontInfo *		afi1,
				const AfmFontInfo *		afi2 )
    {
    int				cmp;

    cmp= strcmp( afi1->afiFamilyName, afi2->afiFamilyName );
    if  ( cmp != 0 )
	{ return cmp;	}

    return utilFontCompareFaces( (void *)aft1, (void *)aft2 );
    }

static int psFontCompareTypeInfos(	const void *	voidafi1,
					const void *	voidafi2 )
    {
    const AfmFontInfo *	afi1= (AfmFontInfo *)voidafi1;
    const AfmFontInfo *	afi2= (AfmFontInfo *)voidafi2;

    AppFontTypeface	aft1;
    AppFontTypeface	aft2;

    docInitFontTypeface( &aft1 );
    docInitFontTypeface( &aft2 );

    psFillFaceFromInfo( &aft1, afi1 );
    psFillFaceFromInfo( &aft2, afi2 );

    return psFontCompareFaces( &aft1, &aft2, afi1, afi2 );
    }

/************************************************************************/
/*									*/
/*  Make a catalog of postscript fonts for the font chooser.		*/
/*									*/
/*  The code assumes that afmDirectory is some kind of a resource, that	*/
/*  does not vary over different calls to the routine.			*/
/*									*/
/*  A:  Consume one file.						*/
/*  B:  Ignore errors in cerian hopeless files.				*/
/*									*/
/************************************************************************/

#   define	FILEL	400

static AfmFontInfo *	UtilFontPsInfos= (AfmFontInfo *)0;
static int		UtilFontPsInfoCount= 0;


/*  A  */
static int psGotAfmFile(	const char *	filename,
				void *		through )
    {
    SimpleInputStream *		sisAfm;
    int				res;

    int				i;
    int				mappedFamily= -1;
    int				mappedFull= -1;

    AfmFontInfo *		afi;

    int				len= strlen( filename );

    if  ( len == 0 )
	{ LDEB(len); return -1;	}

    afi= (AfmFontInfo *)realloc( UtilFontPsInfos,
			    (UtilFontPsInfoCount+ 1)* sizeof(AfmFontInfo) );
    if  ( ! afi )
	{ XDEB(afi); return -1;	}
    UtilFontPsInfos= afi;

    afi += UtilFontPsInfoCount;
    psInitAfmFontInfo( afi );

    sisAfm= sioInStdioOpen( filename );
    if  ( ! sisAfm )
	{
	SXDEB(filename,sisAfm);
	psCleanAfmFontInfo( afi ); return 0;
	}

    res= psAfmReadAfm( sisAfm, afi );

    sioInClose( sisAfm );

    if  ( res < 0 )
	{ SLDEB(filename,res); psCleanAfmFontInfo( afi ); return 0; }

    if  ( psGetFontEncodings( afi, 0 ) )
	{
	int		l= strlen( filename );
	int		h;
	int		isHopeless= 0;

	const char * const	hopeless[]=
	    {
	    "/ghostscript/fonts/fcyr.afm",
	    "/ghostscript/fonts/fcyri.afm",
	    };

	for ( h= 0; h < sizeof(hopeless)/sizeof(char *); h++ )
	    {
	    int		ll= strlen( hopeless[h] );

	    if  ( ll < l && ! strcmp( filename+ l- ll,  hopeless[h] ) )
		{ isHopeless= 1; break;	}
	    }

	if  ( isHopeless )
	    { psCleanAfmFontInfo( afi ); return 0;	}

	if  ( ! isHopeless )
	    {
	    appDebug( "FONT %s SUPPORTS NO KNOWN ENCODINGS:\n",
							afi->afiFullName );
	    psGetFontEncodings( afi, 1 );
	    }

	afi->afiUseFontspecificEncoding= 1;
	}

    if  ( ! afi->afiFamilyName )
	{
	SXDEB(filename,afi->afiFamilyName);
	psCleanAfmFontInfo( afi ); return 0;
	}

    for ( i= 0; i < sizeof(PS_FamilyMap)/sizeof(NameMap); i++ )
	{
	if  ( ! strcmp( afi->afiFamilyName, PS_FamilyMap[i].nmFrom ) )
	    { mappedFamily= i; break; }
	}

    for ( i= 0; i < sizeof(PS_FullMap)/sizeof(NameMap); i++ )
	{
	if  ( ! strcmp( afi->afiFullName, PS_FullMap[i].nmFrom ) )
	    { mappedFull= i; break; }
	}

    if  ( mappedFamily >= 0 && mappedFull >= 0 )
	{
	free( afi->afiFamilyName );
	afi->afiFamilyName= strdup( PS_FamilyMap[mappedFamily].nmTo );

	free( afi->afiFullName );
	afi->afiFullName= strdup( PS_FullMap[mappedFull].nmTo );
	}

    UtilFontPsInfoCount++; return 0;
    }

/************************************************************************/
/*									*/
/*  Set the faces of a font family.					*/
/*									*/
/*  1)  Some hacked fonts have a hyphen here.				*/
/*  2)  The face name is the tail of the full name.			*/
/*									*/
/************************************************************************/

static int appPsSetFontFaces(	AppFontFamily *		aff,
				int			count,
				AfmFontInfo *		afi )
    {
    AppFontTypeface *	aft;
    int			face;

    int			width= -1;
    int			widthStart= -1;
    int			widthLength= 0;
    const char *	widthStartString= (const char *)0;

    if  ( count == 0 )
	{ LDEB(count); return 0;	}

    aft= realloc( aff->affFaces, count* sizeof( AppFontTypeface ) );
    if  ( ! aft )
	{ LXDEB(count,aft); return -1;	}
    aff->affFaces= aft;

    aff->affFontFamilyName_Orig= afi->afiFamilyName;
    aff->affFontFamilyName= afi->afiFamilyName;

    docInitFontTypeface( aft );
    psFillFaceFromInfo( aft, afi );

    aff->affWidth= aft->aftWidth;

    utilFontWidthFromString( &width, &widthStart, &widthLength,
							afi->afiFullName );
    if  ( widthStart >= 0 )
	{
	if  ( ! afi->afiWidth )
	    {
	    afi->afiWidth= malloc( widthLength+ 1 );
	    if  ( afi->afiWidth )
		{
		strncpy( afi->afiWidth,
			afi->afiFullName+ widthStart,
			widthLength )[widthLength]= '\0';
		}
	    }

	widthStartString= afi->afiFullName+ widthStart;
	}

    for ( face= 0; face < count; aft++, afi++, face++ )
	{
	int		fl;

	docInitFontTypeface( aft );
	psFillFaceFromInfo( aft, afi );

	aft->aftFontInfo= afi;
	aft->aftFaceName= afi->afiFullName;

	if  ( afi->afiIsFixedPitch )
	    {
	    if  ( aff->affHasProportionalWidth )
		{ SLDEB(afi->afiFullName,afi->afiIsFixedPitch);	}
	    aff->affHasFixedWidth= 1;
	    }
	else{
	    if  ( aff->affHasFixedWidth )
		{ SLDEB(afi->afiFullName,afi->afiIsFixedPitch);	}
	    aff->affHasProportionalWidth= 1;
	    }

	if  ( afi->afiUseFontspecificEncoding )
	    {
	    aff->affUseFontspecificEncoding= afi->afiUseFontspecificEncoding;
	    }

	fl= strlen( afi->afiFamilyName );

	/*  1  */
	if  ( ! strncmp( afi->afiFamilyName, afi->afiFullName, fl )	&&
	      afi->afiFullName[fl] == '-'				)
	    { afi->afiFullName[fl]=   ' ';	}

	/*  2  */
	if  ( ! strncmp( afi->afiFamilyName, afi->afiFullName, fl )	&&
	      ( afi->afiFullName[fl] == ' '		||
		afi->afiFullName[fl] == '\0'	)	)
	    {
	    aft->aftFaceName= afi->afiFullName+ fl;
	    while( aft->aftFaceName[0] == ' ' )
		{ aft->aftFaceName++;	}
	    if  ( ! aft->aftFaceName[0] )
		{ aft->aftFaceName= afi->afiWeight;	}
	    }
	else{
	    SSDEB(afi->afiFamilyName,afi->afiFullName);
	    if  ( afi->afiWeight )
		{ aft->aftFaceName= afi->afiWeight;	}
	    else{ aft->aftFaceName= afi->afiFullName;	}
	    }
	}

    aff->affFaceCount= count;

    if  ( aff->affFaceCount > 0 )
	{
	int			enc;

	qsort( aff->affFaces, aff->affFaceCount,
			    sizeof(AppFontTypeface), utilFontCompareFaces );

	aft= aff->affFaces;
	for ( face= 0; face < aff->affFaceCount; aft++, face++ )
	    {
	    AfmFontInfo *	afiFace= aft->aftFontInfo;

	    for ( enc= 0; enc < ENCODINGps_COUNT; enc++ )
		{
		if  ( face == 0 )
		    {
		    aff->affSupportedCharsets[enc]= 
					    afiFace->afiSupportedCharsets[enc];
		    }
		else{
		    if  ( ! afiFace->afiSupportedCharsets[enc].scSupported )
			{ aff->affSupportedCharsets[enc].scSupported= 0; }
		    }
		}
	    }

	aff->affDefaultEncoding= aff->affFaces[0].aftDefaultEncoding;
	}

    if  ( widthStart >= 0 )
	{
	int	len= strlen( aff->affFontFamilyName );
	char *	text= malloc( len+ 1+ widthLength+ 1 );

	if  ( text )
	    {
	    strcpy( text, aff->affFontFamilyName );
	    text[len]= ' ';
	    strncpy( text+ len+ 1, widthStartString, widthLength );
	    text[len+ 1+ widthLength]= '\0';

	    aff->affFontFamilyName= text;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make a catalog of all the fonts for which an AFM file is found.	*/
/*									*/
/*  1)  Collect the information from all AFM files.			*/
/*	NOTE: Afm files are in the "C" locale.				*/
/*  2)  Count families.							*/
/*	NOTE: Width variants of the same family are treated like	*/
/*	different families here.					*/
/*  3)  Allocate space for the families.				*/
/*  4)  Divide the faces over the families.				*/
/*									*/
/************************************************************************/

static AppFontFamily *	EditFontPsFamilies= (AppFontFamily *)0;
static int		EditFontPsFamilyCount= 0;

int psFontCatalog(	PostScriptFontList *	psfl,
			const char *		afmDirectory )
    {
    int				j;
    int				from;

    int				fam;
    AppFontFamily *		aff;

    char			scratch[FILEL+ 1];

#   ifdef __VMS
    const char * const		afmExtension= "AFM";
#   else
    const char * const		afmExtension= "afm";
#   endif

    if  ( EditFontPsFamilyCount > 0 )
	{
	psfl->psflFamilies= EditFontPsFamilies;
	psfl->psflFamilyCount= EditFontPsFamilyCount;
	return 0;
	}

    /*  1  */
    setlocale( LC_NUMERIC, "C" );

    j= appForAllFiles( afmDirectory, afmExtension, (void *)0, psGotAfmFile );

    setlocale( LC_NUMERIC, "" );

    if  ( j )
	{ SDEB(afmDirectory); return -1;	}

    if  ( UtilFontPsInfoCount < 1 )
	{
	LDEB(UtilFontPsInfoCount);

	psfl->psflFamilies= EditFontPsFamilies;
	psfl->psflFamilyCount= EditFontPsFamilyCount;

	return 0;
	}

    qsort( UtilFontPsInfos, UtilFontPsInfoCount,
			    sizeof(AfmFontInfo), psFontCompareTypeInfos );

    /*  2  */
    fam= 1;
    for ( j= 1; j < UtilFontPsInfoCount; j++ )
	{
	AppFontTypeface	aft1;
	AppFontTypeface	aft2;

	docInitFontTypeface( &aft1 );
	docInitFontTypeface( &aft2 );

	psFillFaceFromInfo( &aft1, &(UtilFontPsInfos[j+ 0]) );
	psFillFaceFromInfo( &aft2, &(UtilFontPsInfos[j- 1]) );

	if  ( strcmp( UtilFontPsInfos[j].afiFamilyName,
				UtilFontPsInfos[j- 1].afiFamilyName )	||
	      aft1.aftWidth != aft2.aftWidth				)
	    { fam++;	}
	}

    /*  3  */
    aff= realloc( EditFontPsFamilies, fam* sizeof(AppFontFamily) );
    if  ( ! aff )
	{ LXDEB(fam,aff); return -1;	}
    EditFontPsFamilies= aff;

    aff= EditFontPsFamilies;
    for ( j= 0; j < fam; aff++, j++ )
	{ docInitFontFamily( aff );	}

    /*  2  */
    from= 0; fam= 0;
    for ( j= 1; j < UtilFontPsInfoCount; j++ )
	{
	AppFontTypeface	aft1;
	AppFontTypeface	aft2;

	docInitFontTypeface( &aft1 );
	docInitFontTypeface( &aft2 );

	psFillFaceFromInfo( &aft1, &(UtilFontPsInfos[j+ 0]) );
	psFillFaceFromInfo( &aft2, &(UtilFontPsInfos[j- 1]) );

	if  ( ! strcmp( UtilFontPsInfos[j].afiFamilyName,
				UtilFontPsInfos[j- 1].afiFamilyName )	&&
	      aft1.aftWidth == aft2.aftWidth				)
	    { continue;	}

	if  ( appPsSetFontFaces( EditFontPsFamilies+ fam,
					    j- from, UtilFontPsInfos+ from ) )
	    { LDEB(j- from); return -1;	}

	from= j; fam++;
	}

    if  ( appPsSetFontFaces( EditFontPsFamilies+ fam,
					    j- from, UtilFontPsInfos+ from ) )
	{ LDEB(j- from); return -1;	}

    EditFontPsFamilyCount= fam+ 1;

    psfl->psflFamilies= EditFontPsFamilies;
    psfl->psflFamilyCount= EditFontPsFamilyCount;

    sprintf( scratch, "%s/xfonts.dir", afmDirectory );
    if  ( ! appTestFileExists( scratch ) )
	{
	const int	mapNames= 0;

	if  ( utilSetT1EntriesForFonts( scratch, mapNames, psfl ) )
	    { SDEB(scratch); return -1;	}
	}

    return 0;
    }

