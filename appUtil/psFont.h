/************************************************************************/
/*									*/
/*  Data structures that describe postscript fonts.			*/
/*									*/
/************************************************************************/

#   ifndef	PS_FONT_H
#   define	PS_FONT_H

#   include	<utilFontEncoding.h>
#   include	<sioGeneral.h>

/************************************************************************/
/*									*/
/*  Charsets/Encodings supported by a font.				*/
/*									*/
/************************************************************************/

typedef struct SupportedCharset
    {
    unsigned char	scSupported;
    unsigned char	scNonStandardGlyphNames;
    short int		scCodeToGlyphMapping[256];
    } SupportedCharset;

/************************************************************************/
/*									*/
/*  Information extracted from an AFM file.				*/
/*									*/
/************************************************************************/

typedef struct AfmKerningPair
    {
    int		akpPosition;
    int		akpXVec;
    } AfmKerningPair;

typedef struct AfmBBox
    {
    int		abbLeft;
    int		abbBottom;
    int		abbRight;
    int		abbTop;
    } AfmBBox;

typedef struct AfmCharMetric
    {
    int			acmC;
    int			acmWX;
    AfmBBox		acmBBox;
    char *		acmN;
    AfmKerningPair *	acmKernPairs;
    int			acmKernPairCount;
    } AfmCharMetric;

typedef struct AfmFontInfo
    {
    char *		afiFontName;
    char *		afiFullName;
    char *		afiNotice;
    char *		afiVersion;
    char *		afiFamilyName;
    char *		afiWeight;
    char *		afiWidth;
    double		afiItalicAngle;
    double		afiTanItalicAngle;
    unsigned char	afiIsFixedPitch;
    AfmBBox		afiFontBBox;
    double		afiUnderlinePosition;
    double		afiUnderlineThickness;
    int			afiCapHeight;
    int			afiXHeight;
    int			afiAscender;
    int			afiDescender;

    char *		afiEncodingScheme;
    char *		afiCharacterSet;
    char *		afiVendor;

    char *		afiFontFileName;

    int			afiMetricCount;
    AfmCharMetric *	afiMetrics;

    int			afiUseFontspecificEncoding;
    SupportedCharset	afiSupportedCharsets[ENCODINGps_COUNT];
    short int		afiDefaultCodeToGlyph[256];
    } AfmFontInfo;

/************************************************************************/
/*									*/
/*  Font information.							*/
/*									*/
/*  Used to map a users idea of fonts to implementation fonts.		*/
/*  Used by the font chooser to select fonts.				*/
/*									*/
/*  1)  Set width defines. The values math those in paragraph 3.2.37	*/
/*	of the X11R5 logical font description convention.		*/
/*	[ The differences in value are more interesting than the values	*/
/*	themselves. ]							*/
/*  1)  Set weight defines. The values math those in paragraph 3.2.38	*/
/*	of the X11R5 logical font description convention.		*/
/*	[ The differences in value are more interesting than the values	*/
/*	themselves. ]							*/
/*									*/
/************************************************************************/

/*  1  */
#   define	FONTwidth_UNDEFINED		0

#   define	FONTwidth_MIN			FONTwidthULTRA_CONDENSED
#   define	FONTwidthULTRA_CONDENSED	10
#   define	FONTwidthEXTRA_CONDENSED	20
#   define	FONTwidthCONDENSED		30
#   define	FONTwidthSEMI_CONDENSED		40
#   define	FONTwidthMEDIUM			50
#   define	FONTwidthSEMI_EXPANDED		60
#   define	FONTwidthEXPANDED		70
#   define	FONTwidthEXTRA_EXPANDED		80
#   define	FONTwidthULTRA_EXPANDED		90
#   define	FONTwidth_MAX			FONTwidthULTRA_EXPANDED

/*  2  */
#   define	FONTweight_UNDEFINED		0

#   define	FONTweight_MIN			FONTweightULTRA_LIGHT
#   define	FONTweightULTRA_LIGHT		10  /* = Thin */
#   define	FONTweightEXTRA_LIGHT		20
#   define	FONTweightLIGHT			30
#   define	FONTweightSEMI_LIGHT		40
#   define	FONTweightMEDIUM		50
#   define	FONTweightSEMI_BOLD		60
#   define	FONTweightBOLD			70
#   define	FONTweightEXTRA_BOLD		80
#   define	FONTweightULTRA_BOLD		90  /* = Heavy = Black */
#   define	FONTweight_MAX			FONTweightULTRA_BOLD

typedef struct AppFontTypeface
    {
    char *			aftFaceName;
    unsigned int		aftIsBold:1;
    unsigned int		aftIsSlanted:1;
    AfmFontInfo *		aftFontInfo;
    unsigned char		aftWidth;
    unsigned char		aftWeight;
    short int			aftDefaultEncoding;
    char *			aftFontSpecificQueryFormat;
    char *			aftXQueryFormats[ENCODINGps_COUNT];
    } AppFontTypeface;

/************************************************************************/
/*									*/
/*  A family of PostScript fonts as harvested from AFM files.		*/
/*									*/
/*  1)  The name of the family.						*/
/*  2)  The URW++ fonts that are similar to the Adobe base 35 fonts are	*/
/*	mapped to the Adobe fonts. If this is done, affFontFamilyName	*/
/*	set to the Adobe base 35 name, affFontFamilyName_Orig is the	*/
/*	name of the font that comes from the afm file.			*/
/*  3)  A list of faces (Bold, Italic &c) that belong to this family.	*/
/*  4)  Possibly conflicting statements about the fixed/variable width	*/
/*	of the fonts in the family: If the AFM files are strange, both	*/
/*	can be set.							*/
/*									*/
/************************************************************************/

typedef struct AppFontFamily
    {
    char *			affFontFamilyName;		/*  1	*/
    char *			affFontFamilyName_Orig;		/*  2	*/
    AppFontTypeface *		affFaces;			/*  3	*/
    short int			affFaceCount;
    unsigned char		affHasFixedWidth;		/*  4	*/
    unsigned char		affHasProportionalWidth;	/*  4	*/
    unsigned char		affUseFontspecificEncoding;
    short int			affDefaultEncoding;
    unsigned char		affWidth;
    SupportedCharset		affSupportedCharsets[ENCODINGps_COUNT];
    } AppFontFamily;

typedef struct PostScriptFontList
    {
    AppFontFamily *	psflFamilies;
    int			psflFamilyCount;
    } PostScriptFontList;

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern void docInitFontFamily(		AppFontFamily *		aff );
extern void docInitFontTypeface(	AppFontTypeface *	aft );
extern void psInitPostScriptFontList(	PostScriptFontList *	psfl );

extern int psCalculateStringExtents(	AfmBBox *		abb,
					const unsigned char *	s,
					int			len,
					int			twipsSize,
					int			withKerning,
					int			encoding,
					const AfmFontInfo *	afi );

extern int psFontCatalog(	PostScriptFontList *	psfl,
				const char *		afmDirectory );

extern int psGetFontEncodings(		AfmFontInfo *		afi,
					int			complain );

extern void utilFontFaceDistance(	int *			pDifCount,
					double *		pDistance,
					const AppFontTypeface *	aft1,
					int			isSlanted,
					int			weight );

extern void psInitAfmBBox(		AfmBBox *	abb );
extern void psInitAfmCharMetric(	AfmCharMetric *	acm );
extern void psCleanAfmCharMetric(	AfmCharMetric *	acm );
extern void psInitAfmFontInfo(		AfmFontInfo *	afi );
extern void psCleanAfmFontInfo(		AfmFontInfo *	afi );

extern void psWriteAfmFile(	SimpleOutputStream *	sos,
				const AfmFontInfo *	afi );

extern void psWriteFontInfoDict(	SimpleOutputStream *	sos,
					const AfmFontInfo *	afi );

extern int psTtfToPf42(		SimpleOutputStream *	sosPf42,
				const char *		fontFileName,
				SimpleInputStream *	sisTtf );

extern int psTtfToPt1(		SimpleOutputStream *	sosPt1,
				const char *		fontFileName,
				SimpleInputStream *	sisTtf );

extern int psTtfToAfm(		SimpleOutputStream *	sosAfm,
				const char *		fontFileName,
				SimpleInputStream *	sisTtf );

extern int psAfmToXFontsDir(	SimpleOutputStream *	sosFontDir,
				const char *		afmFileName,
				SimpleInputStream *	sisAfm );

extern int psAfmToGSFontmap(	SimpleOutputStream *	sosFontDir,
				const char *		afmFileName,
				SimpleInputStream *	sisAfm );

extern int psFindAlternate(	const AfmFontInfo *	afi,
				const char *		glyphName );

extern int psAfmReadAfm(	SimpleInputStream *	sisAfm,
				AfmFontInfo *		afi );

extern void psInitSupportedCharset(	SupportedCharset *	sc );
#   endif
