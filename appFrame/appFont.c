/************************************************************************/
/*									*/
/*  Management of X11 fonts: Translation of understandable fonts to	*/
/*  X11 structures.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	<utilPropMask.h>

#   include	<utilMatchFont.h>

#   ifdef USE_GTK
#	include	<gdk/gdkx.h>
#	define	GTK_AND_X11_INCLUDES	1
#   endif

#   include	"appDraw.h"
#   include	<bitmap.h>

#   include	<X11/Xatom.h>

/************************************************************************/
/*									*/
/*  Strings used for the analysis of X11 font names.			*/
/*									*/
/*  1)  The asterisk to insert in a font name for all properties that	*/
/*	we consider to be irrelevant.					*/
/*  2)  The templete/format to make X11 font names.			*/
/*  3)  A series of encodings with information on the X11 encoding	*/
/*	strings and on the encodings for which they can be used.	*/
/*  4)  This is the combination of character set and encoding. Given	*/
/*	a particular value, XDrawString() draws the same string with	*/
/*	the same array of characters.					*/
/*									*/
/************************************************************************/

/*  1  */
static const char *	EditFontAnyTemplate= "*";

/*  2  */
# define	EditFontFamilyTemplate \
			    "-%s-%s-%s-%s-%s-%s-%s-%s-%s-%s-%s-%s-%s-%s"

typedef struct XFontFaceSize
    {
    short int	xffsPixelSize;
    short int	xffsListIndex;
    } XFontFaceSize;

static void appInitXFontFaceSize(	XFontFaceSize *	xffs )
    {
    xffs->xffsPixelSize= 0;
    xffs->xffsListIndex= -1;
    }

# define appCleanXFontFaceSize( xffs ) /* nothing */

typedef struct XFontTypeface
    {
    char *		xftWeightStr;
    char *		xftSlantStr;
    unsigned char	xftWeight;
    unsigned char	xftIsBold;
    unsigned char	xftIsSlanted;
    unsigned char	xftIsScalable;
    unsigned char	xftIsFixedWidth;

    XFontFaceSize *	xftFaceSizes;
    int			xftFaceSizeCount;
    int			xftScalableListIndex;
    } XFontTypeface;

static void appInitXFontTypeface(	XFontTypeface *	xft )
    {
    xft->xftWeightStr= (char *)0;
    xft->xftSlantStr= (char *)0;
    xft->xftWeight= FONTweightMEDIUM;
    xft->xftIsBold= 0;
    xft->xftIsSlanted= 0;
    xft->xftIsScalable= 0;
    xft->xftIsFixedWidth= 0;

    xft->xftFaceSizes= (XFontFaceSize *)0;
    xft->xftFaceSizeCount= 0;

    xft->xftScalableListIndex= -1;

    return;
    }

static void appCleanXFontTypeface(	XFontTypeface *	xft )
    {
    int		i;

    if  ( xft->xftWeightStr )
	{ free( xft->xftWeightStr );	}
    if  ( xft->xftSlantStr )
	{ free( xft->xftSlantStr );	}

    for ( i= 0; i < xft->xftFaceSizeCount; i++ )
	{ appCleanXFontFaceSize( &(xft->xftFaceSizes[i]) );	}

    if  ( xft->xftFaceSizes )
	{ free( xft->xftFaceSizes );	}

    return;
    }

typedef struct XFontFamily
    {
    char *		xffFamily;
    char *		xffFoundry;
    int			xffSwdth;

    XFontTypeface *	xffFaces;
    int			xffFaceCount;
    } XFontFamily;

static void appInitXFontFamily(	XFontFamily *	xff )
    {
    xff->xffFamily= (char *)0;
    xff->xffFoundry= (char *)0;
    xff->xffSwdth= 0;

    xff->xffFaces= (XFontTypeface *)0;
    xff->xffFaceCount= 0;
    }

static void appCleanXFontFamily(	XFontFamily *	xff )
    {
    int		i;

    if  ( xff->xffFamily )
	{ free( xff->xffFamily );	}

    if  ( xff->xffFoundry )
	{ free( xff->xffFoundry );	}

    for ( i= 0; i < xff->xffFaceCount; i++ )
	{ appCleanXFontTypeface( &(xff->xffFaces[i]) ); }

    if  ( xff->xffFaces )
	{ free( xff->xffFaces );	}
    }

/*  3  */
typedef struct AppFontEncoding
    {
    XFontFamily *	afeXfontFamilies;
    int			afeXfontFamilyCount;
    char **		afeXFontList;	/* from XListFonts()		*/
    int			afeXFontCount;	/* from XListFonts()		*/
    } AppFontEncoding;

static AppFontEncoding	AppFontXEncodings[ENCODINGps_COUNT];

/************************************************************************/
/*									*/
/*  Make a font query string.						*/
/*									*/
/************************************************************************/

static void appFontQueryString(	char *		target,
				const char *	foundry,
				const char *	family,
				const char *	weight,
				const char *	slant,
				const char *	swdth,
				const char *	pxlsz,
				const char *	registry,
				const char *	encoding )
    {
    sprintf( target, EditFontFamilyTemplate,
		foundry,
		family,
		weight,
		slant,
		swdth,			/* swdth		*/
		EditFontAnyTemplate,	/* adstyl		*/
		pxlsz,
		EditFontAnyTemplate,	/* ptsz			*/
		EditFontAnyTemplate,	/* resx			*/
		EditFontAnyTemplate,	/* resy			*/
		EditFontAnyTemplate,	/* spacing		*/
		EditFontAnyTemplate,	/* average width	*/
		registry,
		encoding );

    return;
    }

/************************************************************************/
/*									*/
/*  Extract properties from an X font selection string.			*/
/*									*/
/************************************************************************/

static int appFontExtractString(	const char *	s,
					const char **	pPast,
					char *		target )
    {
    if  ( *s != '-' )
	{ SDEB(s); return -1;	}
    s++;
    while( *s && *s != '-' )
	{ *(target++)= *(s++); }
    *target= '\0';

    *pPast= s; return 0;
    }

static int appFontSkipString(	const char *	s,
				const char **	pPast )
    {
    if  ( *s != '-' )
	{ SDEB(s); return -1;	}
    s++;
    while( *s && *s != '-' )
	{ s++; }

    *pPast= s; return 0;
    }

static int appFontSplitXFont(	const char *	fontname,
				char *		foundry,
				char *		family,
				char *		weight,
				char *		slant,
				char *		swdth,
				char *		pxlszStr,
				char *		spacing,
				char *		avgwdth,
				char *		registry,
				char *		encoding )
    {
    const char *	s;

    s= fontname;

    if  ( appFontExtractString( s, &s, foundry ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, family ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, weight ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, slant ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, swdth ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontSkipString( s, &s ) ) /* adstyl */
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, pxlszStr ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontSkipString( s, &s ) ) /* ptsz */
	{ SDEB(fontname); return -1;	}

    if  ( appFontSkipString( s, &s ) ) /* resx */
	{ SDEB(fontname); return -1;	}

    if  ( appFontSkipString( s, &s ) ) /* resy */
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, spacing ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, avgwdth ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, registry ) )
	{ SDEB(fontname); return -1;	}

    if  ( appFontExtractString( s, &s, encoding ) )
	{ SDEB(fontname); return -1;	}

    if  ( s[0] )
	{ SSDEB(fontname,s); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make a catalog of X11 fonts.					*/
/*									*/
/************************************************************************/

static int appFontGetWeight(	unsigned char *		pWeight,
				const char *		weight )
    {
    if  ( ! strcmp( weight, "extralight" ) )
	{ *pWeight= FONTweightEXTRA_LIGHT; return 0;	}
    if  ( ! strcmp( weight, "ultralight" ) )
	{ *pWeight= FONTweightEXTRA_LIGHT; return 0;	}
    if  ( ! strcmp( weight, "thin" ) )
	{ *pWeight= FONTweightEXTRA_LIGHT; return 0;	}

    if  ( ! strcmp( weight, "light" ) )
	{ *pWeight= FONTweightLIGHT; return 0;	}

    if  ( ! strcmp( weight, "demilight" ) )
	{ *pWeight= FONTweightSEMI_LIGHT; return 0;	}
    if  ( ! strcmp( weight, "semilight" ) )
	{ *pWeight= FONTweightSEMI_LIGHT; return 0;	}
    if  ( ! strcmp( weight, "book" ) )
	{ *pWeight= FONTweightSEMI_LIGHT; return 0;	}

    if  ( ! strcmp( weight, "" ) )
	{ *pWeight= FONTweightMEDIUM; return 0;	}
    if  ( ! strcmp( weight, "roman" ) )
	{ *pWeight= FONTweightMEDIUM; return 0;	}
    if  ( ! strcmp( weight, "medium" ) )
	{ *pWeight= FONTweightMEDIUM; return 0;	}
    if  ( ! strcmp( weight, "normal" ) )
	{ *pWeight= FONTweightMEDIUM; return 0;	}
    if  ( ! strcmp( weight, "regular" ) )
	{ *pWeight= FONTweightMEDIUM; return 0;	}

    if  ( ! strcmp( weight, "semi bold" ) )
	{ *pWeight= FONTweightSEMI_BOLD; return 0;	}
    if  ( ! strcmp( weight, "demi bold" ) )
	{ *pWeight= FONTweightSEMI_BOLD; return 0;	}
    if  ( ! strcmp( weight, "semibold" ) )
	{ *pWeight= FONTweightSEMI_BOLD; return 0;	}
    if  ( ! strcmp( weight, "demibold" ) )
	{ *pWeight= FONTweightSEMI_BOLD; return 0;	}
    if  ( ! strcmp( weight, "demi" ) )
	{ *pWeight= FONTweightSEMI_BOLD; return 0;	}

    if  ( ! strcmp( weight, "bold" ) )
	{ *pWeight= FONTweightBOLD; return 0;	}
    if  ( ! strcmp( weight, "heavy" ) )
	{ *pWeight= FONTweightBOLD; return 0;	}

    if  ( ! strcmp( weight, "ultrablack" ) )
	{ *pWeight= FONTweightEXTRA_BOLD; return 0;	}
    if  ( ! strcmp( weight, "ultrabold" ) )
	{ *pWeight= FONTweightEXTRA_BOLD; return 0;	}
    if  ( ! strcmp( weight, "extrablack" ) )
	{ *pWeight= FONTweightEXTRA_BOLD; return 0;	}
    if  ( ! strcmp( weight, "extrabold" ) )
	{ *pWeight= FONTweightEXTRA_BOLD; return 0;	}
    if  ( ! strcmp( weight, "extra bold" ) )
	{ *pWeight= FONTweightEXTRA_BOLD; return 0;	}

    if  ( ! strcmp( weight, "black" ) )
	{ *pWeight= FONTweightULTRA_BOLD; return 0;	}

    SDEB(weight); return -1;
    }

/************************************************************************/
/*									*/
/*  Try to make something of the font set width field.			*/
/*									*/
/*  As the content of the field in not covered by a standard, we have	*/
/*  to live with what font designerd and font servers choose to reoport.*/
/*									*/
/************************************************************************/

static int appFontGetWidth(	int *			pWidth,
				const char *		swdth )
    {
    if  ( ! strcmp( swdth, "narrow" ) )
	{ *pWidth= FONTwidthCONDENSED; return 0;	}
    if  ( ! strcmp( swdth, "extra condensed" ) )
	{ *pWidth= FONTwidthEXTRA_CONDENSED; return 0;	}

    if  ( ! strcmp( swdth, "condensed" ) )
	{ *pWidth= FONTwidthCONDENSED; return 0;	}

    if  ( ! strcmp( swdth, "semi condensed" ) )
	{ *pWidth= FONTwidthSEMI_CONDENSED; return 0;	}
    if  ( ! strcmp( swdth, "semicondensed" ) )
	{ *pWidth= FONTwidthSEMI_CONDENSED; return 0;	}

    if  ( ! strcmp( swdth, "normal" ) )
	{ *pWidth= FONTwidthMEDIUM; return 0;	}

    if  ( ! strcmp( swdth, "semi extended" ) )
	{ *pWidth= FONTwidthSEMI_EXPANDED; return 0;	}
    if  ( ! strcmp( swdth, "semi expanded" ) )
	{ *pWidth= FONTwidthSEMI_EXPANDED; return 0;	}
    if  ( ! strcmp( swdth, "semiexpanded" ) )
	{ *pWidth= FONTwidthSEMI_EXPANDED; return 0;	}

    if  ( ! strcmp( swdth, "extended" ) )
	{ *pWidth= FONTwidthEXPANDED; return 0;	}
    if  ( ! strcmp( swdth, "expanded" ) )
	{ *pWidth= FONTwidthEXPANDED; return 0;	}

    SDEB(swdth); return -1;
    }

static int appCmpFacePixelSizes(	const void *	vxffs1,
					const void *	vxffs2 )
    {
    const XFontFaceSize *	xffs1= (const XFontFaceSize *)vxffs1;
    const XFontFaceSize *	xffs2= (const XFontFaceSize *)vxffs2;

    if  ( xffs1->xffsPixelSize > xffs2->xffsPixelSize )
	{ return  1; }

    if  ( xffs1->xffsPixelSize < xffs2->xffsPixelSize )
	{ return -1; }

    return 0;
    }

static int appFontXCatalog(	AppDrawingData *	add,
				AppFontEncoding *	afe,
				const char *		registryStr,
				const char *		encodingStr )
    {
    char			familyFormat[120];
	
    int				idx;

    XFontFamily *		xff;
    XFontTypeface *		xft;
    int				fam;
    int				face;

#   ifdef USE_MOTIF
    Display *			display= add->addDisplay;
#   endif

#   ifdef USE_GTK
    Display *			display= GDK_DISPLAY();
#   endif

    if  ( afe->afeXFontList )
	{ return 0;	}

    appFontQueryString( familyFormat,
		EditFontAnyTemplate,	/* foundry,		*/
		EditFontAnyTemplate,	/* family,		*/
		EditFontAnyTemplate,	/* weight,		*/
		EditFontAnyTemplate,	/* slant,		*/
		EditFontAnyTemplate,	/* swdth,		*/
		EditFontAnyTemplate,	/* pxlsz,		*/
		registryStr,		/* registry		*/
		encodingStr );		/* encoding		*/

    afe->afeXFontList= XListFonts( display, familyFormat,
						4000, &(afe->afeXFontCount) );

    if  ( afe->afeXFontCount == 4000 )
	{ LLDEB(afe->afeXFontCount,4000);	}

    for ( idx= 0; idx < afe->afeXFontCount; idx++ )
	{
	char			foundry[40];
	char			family[40];

	char			weightStr[40];
	char			slantStr[40];
	char			swdthStr[40];

	char			ignore[40];
	char			spacing[40];
	char			avgwdth[40];

	char			pxlszStr[40];
	int			pxlsz;
	int			swdth;

	if  ( appFontSplitXFont( afe->afeXFontList[idx],
					foundry,
					family,
					weightStr,
					slantStr,
					swdthStr,
					pxlszStr,
					spacing,
					avgwdth,
					ignore,		/*  registry  */
					ignore ) )	/*  encoding  */
	    { LSDEB(idx,afe->afeXFontList[idx]); continue;	}

	if  ( strcmp( spacing, "m" ) && strcmp( spacing, "p" ) )
	    {
	    if  ( strcmp( spacing, "c" ) )
		{ SDEB(spacing);	}

	    continue;
	    }

	if  ( strcmp( pxlszStr, "0" ) && ! strcmp( avgwdth, "0" ) )
	    { /*SSDEB(pxlszStr,avgwdth);*/ continue;	}

	if  ( appFontGetWidth( &swdth, swdthStr ) )
	    { SSSDEB(foundry,family,swdthStr); continue;	}

	xff= afe->afeXfontFamilies;
	for ( fam= 0; fam < afe->afeXfontFamilyCount; xff++, fam++ )
	    {
	    if  ( ! strcmp( xff->xffFamily, family )	&&
		  ! strcmp( xff->xffFoundry, foundry )	&&
		  xff->xffSwdth == swdth		)
		{ break;	}
	    }

	if  ( fam >= afe->afeXfontFamilyCount )
	    {
	    xff= (XFontFamily *)realloc( afe->afeXfontFamilies,
			(afe->afeXfontFamilyCount+1)* sizeof(XFontFamily) );
	    if  ( ! xff )
		{ LXDEB(afe->afeXfontFamilyCount,xff); return -1;	}
	    afe->afeXfontFamilies= xff;

	    xff += afe->afeXfontFamilyCount;

	    appInitXFontFamily( xff );

	    xff->xffFamily= strdup( family );
	    xff->xffFoundry= strdup( foundry );
	    xff->xffSwdth= swdth;

	    if  ( ! xff->xffFamily || ! xff->xffFoundry )
		{
		XXDEB(xff->xffFamily,xff->xffFoundry);
		appCleanXFontFamily( xff );
		return -1;
		}

	    afe->afeXfontFamilyCount++;
	    }

	xft= xff->xffFaces;
	for ( face= 0; face < xff->xffFaceCount; xft++, face++ )
	    {
	    if  ( ! strcmp( xft->xftWeightStr, weightStr )	&&
		  ! strcmp( xft->xftSlantStr, slantStr )	)
		{ break;	}
	    }

	if  ( face >= xff->xffFaceCount )
	    {
	    xft= (XFontTypeface *)realloc( xff->xffFaces,
				(xff->xffFaceCount+1)* sizeof(XFontTypeface) );
	    if  ( ! xft )
		{ LXDEB(xff->xffFaceCount,xft); return -1;	}
	    xff->xffFaces= xft;

	    xft += xff->xffFaceCount;

	    appInitXFontTypeface( xft );

	    if  ( appFontGetWeight( &(xft->xftWeight), weightStr ) )
		{ SSDEB(afe->afeXFontList[idx],weightStr); continue;	}

	    if  ( xft->xftWeight > FONTweightMEDIUM )
		{ xft->xftIsBold= 1;	}
	    else{ xft->xftIsBold= 0;	}

	    if  ( strcmp( slantStr, "r" )	)
		{
		if  ( ! strcmp( slantStr, "i" )	||
		      ! strcmp( slantStr, "o" )	)
		    { xft->xftIsSlanted= 1;	}
		else{ SDEB(slantStr); continue;		}
		}

	    xft->xftIsScalable= 0;
	    xft->xftIsFixedWidth= ! strcmp( spacing, "m" );

	    xft->xftWeightStr= strdup( weightStr );
	    xft->xftSlantStr= strdup( slantStr );
	    if  ( ! xft->xftWeightStr || ! xft->xftSlantStr )
		{
		XXDEB(xft->xftWeightStr,xft->xftSlantStr);
		appCleanXFontTypeface( xft );
		return -1;
		}

	    xff->xffFaceCount++;
	    }

	pxlsz= atoi( pxlszStr );
	if  ( pxlsz == 0 )
	    {
	    xft->xftIsScalable= 1;
	    xft->xftScalableListIndex= idx;
	    }
	else{
	    XFontFaceSize *	xffs= xft->xftFaceSizes;
	    int			sz;

	    for ( sz= 0; sz < xft->xftFaceSizeCount; xffs++, sz++ )
		{
		if  ( xffs->xffsPixelSize == pxlsz )
		    { break;	}
		}

	    /* duplicate? */
	    if  ( sz < xft->xftFaceSizeCount )
		{
		/*
		SLLSDEB("DUPLICATE",idx,sz,afe->afeXFontList[idx]);
		LSDEB(xffs->xffsListIndex,
				afe->afeXFontList[xffs->xffsListIndex]);
		*/
		continue;
		}

	    xffs= (XFontFaceSize *)realloc( xft->xftFaceSizes,
			( xft->xftFaceSizeCount+ 1 )* sizeof( XFontFaceSize ) );
	    if  ( ! xffs )
		{ LXDEB(xft->xftFaceSizeCount,xffs); return -1;	}
	    xft->xftFaceSizes= xffs;

	    xffs += xft->xftFaceSizeCount;

	    appInitXFontFaceSize( xffs );

	    xffs->xffsPixelSize= pxlsz;
	    xffs->xffsListIndex= idx;

	    xft->xftFaceSizeCount++;
	    }
	}

    xff= afe->afeXfontFamilies;
    for ( fam= 0; fam < afe->afeXfontFamilyCount; xff++, fam++ )
	{
	xft= xff->xffFaces;
	for ( face= 0; face < xff->xffFaceCount; xft++, face++ )
	    {
	    if  ( xft->xftFaceSizeCount > 1 )
		{
		qsort( xft->xftFaceSizes, xft->xftFaceSizeCount,
				sizeof(XFontFaceSize), appCmpFacePixelSizes );
		}
	    }
	}

    /* NO! XFreeFontNames( afe->afeXFontList ); */

    return 0;
    }

/************************************************************************/
/*									*/
/*  Get additional font properties.					*/
/*									*/
/************************************************************************/

static void appXFontProperties(	DrawScreenFont *	dsf )
    {
    XFontStruct *	xfs;
    unsigned long	prop;

#   ifdef USE_MOTIF
    xfs= dsf->apfFontStruct;
#   endif

#   ifdef USE_GTK
    xfs= GDK_FONT_XFONT( dsf->apfFontStruct );
#   endif

    if  ( XGetFontProperty( xfs, XA_UNDERLINE_THICKNESS, &prop ) )
	{ dsf->apfUnderlineThicknessPixels= prop;	}
    else{
	dsf->apfUnderlineThicknessPixels= ( xfs->ascent+ xfs->descent )/12;

	if  ( dsf->apfUnderlineThicknessPixels == 0 )
	    { dsf->apfUnderlineThicknessPixels= 1;	}
	}

    if  ( XGetFontProperty( xfs, XA_UNDERLINE_POSITION, &prop ) )
	{ dsf->apfUnderLinePositionPixels= prop;		}
    else{ dsf->apfUnderLinePositionPixels= ( xfs->descent/ 2 );	}

    if  ( XGetFontProperty( xfs, XA_X_HEIGHT, &prop ) )
	{ dsf->apfXHeightPixels= prop;					}
    else{ dsf->apfXHeightPixels= ( xfs->ascent+ xfs->descent )/2;	}

    dsf->apfAscentPixels= dsf->apfFontStruct->ascent;
    dsf->apfDescentPixels= dsf->apfFontStruct->descent;
    }

/************************************************************************/
/*									*/
/*  Find an X11 font with a postscript style font.			*/
/*									*/
/*  1)  Obtain a list of the available X11 fonts.			*/
/*  2)  Try to find an almost exact match on the family name.		*/
/*	Have a preference for fonts with a foundry.			*/
/*  3)  Do not rely too much on scalable fonts. They look ugly and do	*/
/*	not always work transparently with remote displays.		*/
/*									*/
/*  a)  Scalable fonts tend to be a little wider than the corresponding	*/
/*	PostScript fonts. Just subtract a pixel.			*/
/*									*/
/************************************************************************/

static int appFontXFontUsingQuery(	char *			target,
					const char *		queryFormat,
					int			pixelSize )
    {
    const char *	before= queryFormat;
    const char *	after;
    int			size;
    int			i;

    /*  a  */
    if  ( 0 && pixelSize > 10 )
	{ pixelSize--;	}

    if  ( *before != '-' )
	{ SDEB(queryFormat); return -1; }

    for ( i= 0; i < 6; i++ )
	{
	before= strchr( before+ 1, '-' );
	if  ( ! before )
	    { SDEB(queryFormat); return -1;	}
	}

    after= strchr( before+ 1, '-' );
    if  ( ! after )
	{ SDEB(queryFormat); return -1;	}

    size= before- queryFormat+ 1;

    memcpy( target, queryFormat, size );
    sprintf( target+ size, "%d", pixelSize );
    strcpy( target+ strlen( target ), after );

    return 0;
    }

int appFontXFont(	char *			target,
			AppDrawingData *	add,
			const AppFontFamily *	aff,
			int			encoding,
			const AppFontTypeface *	aft,
			int			pixelSize )
    {
    const XFontFamily *		xff;
    const XFontTypeface *	xft;

    int				fam;
    int				face;

    int				turn;

    AppFontEncoding *		afe;

    AfmFontInfo *		afi= aft->aftFontInfo;
    const char *		t1Entry= (const char *)0;
    const char *		scalableEntry= (const char *)0;

    if  ( ! aff || ! aft )
	{ XXDEB(aff,aft); return -1;	}

    afi= aft->aftFontInfo;
    if  ( ! afi )
	{ SXDEB(aft->aftFaceName,aft->aftFontInfo); return -1;	}

    if  ( encoding < 0 )
	{
	t1Entry= aft->aftFontSpecificQueryFormat;

	if  ( t1Entry )
	    {
	    if  ( appFontXFontUsingQuery( target, t1Entry, pixelSize ) )
		{ SDEB(aft->aftFontSpecificQueryFormat); return -1;	}

	    return 0;
	    }

	LXDEB(encoding,t1Entry); return -1;
	}

    t1Entry= aft->aftXQueryFormats[encoding];
    /*  No.. prefer screen fonts
    if  ( t1Entry )
	{
	if  ( appFontXFontUsingQuery( target, t1Entry, pixelSize ) )
	    { SDEB(aft->aftXQueryFormats[encoding]); return -1;	}

	return 0;
	}
    */

    afe= AppFontXEncodings+ encoding;

    /*  1  */
    appFontXCatalog( add, afe,
		PS_Encodings[encoding].fcX11Registry,
		PS_Encodings[encoding].fcX11Encoding );

    if  ( ! afe->afeXfontFamilies )
	{
	SXDEB(aff->affFontFamilyName,afe->afeXfontFamilies);
	LSDEB(encoding,PS_Encodings[encoding].fcX11Registry);
	LSDEB(encoding,PS_Encodings[encoding].fcX11Encoding);
	return -1;
	}

    /*  2  */
    for ( turn= 0; turn < 2; turn++ )
	{
	xff= afe->afeXfontFamilies;

	for ( fam= 0; fam < afe->afeXfontFamilyCount; xff++, fam++ )
	    {
	    char *	xf= xff->xffFamily;
	    char *	psf= aff->affFontFamilyName;

	    if  ( ! strcmp( xf, aff->affFontFamilyName ) )
		{ break;	}

	    while( ( *xf && *psf )					     &&
		   ( *xf == *psf					||
		     ( isupper( *psf ) && tolower( *psf ) == *xf )	)    )
		{ xf++; psf++;	}

	    if  ( ! * xf && ! * psf )
		{ break;	}
	    }

	if  ( fam < afe->afeXfontFamilyCount )
	    {
	    break;	}
	}
    
    if  ( fam >= afe->afeXfontFamilyCount )
	{
	if  ( t1Entry )
	    {
	    if  ( appFontXFontUsingQuery( target, t1Entry, pixelSize ) )
		{ SDEB(t1Entry); return -1;	}

	    return 0;
	    }

	SLDEB(aff->affFontFamilyName,encoding);
	fam= 0; xff= afe->afeXfontFamilies;
	}

    {
    int		faceFound= xff->xffFaceCount;
    int		difCountFound= 999;
    double	distanceFound= 999;

    xft= xff->xffFaces;
    for ( face= 0; face < xff->xffFaceCount; xft++, face++ )
	{
	int		difCount;
	double		distance;

	utilFontFaceDistance( &difCount, &distance, aft,
					xft->xftIsSlanted, xft->xftWeight );

	if  ( difCount < difCountFound		&&
	      difCount < 2			&&
	      distance < distanceFound		&&
	      distance < 0.3			)
	    {
	    faceFound= face;
	    difCountFound= difCount;
	    distanceFound= distance;
	    }
	}

    if  ( faceFound >= xff->xffFaceCount )
	{ SDEB(aft->aftFaceName); return -1;	}

    face= faceFound;
    xft= xff->xffFaces+ face;
    }

    /*  8  */
    if  ( xft->xftFaceSizeCount > 0 )
	{
	int		i;
	int		s;
	int		dv;

	for ( i= 0; i < xft->xftFaceSizeCount; i++ )
	    {
	    if  ( xft->xftFaceSizes[i].xffsPixelSize >= pixelSize )
		{ break;	}
	    }

	if  ( i >= xft->xftFaceSizeCount )
	    { i= xft->xftFaceSizeCount-1;	}

	if  ( i > 0						&&
	      xft->xftFaceSizes[i].xffsPixelSize > pixelSize	)
	    {
	    int		d0;
	    int		d1;

	    d0= ( 100* pixelSize )/ xft->xftFaceSizes[i-1].xffsPixelSize;
	    d1= ( 100* xft->xftFaceSizes[i].xffsPixelSize )/ pixelSize;

	    if  ( d1 > 105 || d0 < d1 )
		{ i--;	}
	    }

	s= xft->xftFaceSizes[i].xffsPixelSize;
	if  ( s >= pixelSize )
	    { dv= ( 100* s )/ pixelSize;	}
	else{ dv= ( 100* pixelSize )/ s;	}

	if  ( dv <= 115					||
	      ( ! t1Entry			&&
	        xft->xftScalableListIndex < 0	)	)
	    {
	    strcpy( target,
		    afe->afeXFontList[xft->xftFaceSizes[i].xffsListIndex] );

	    return 0;
	    }
	}

    if  ( t1Entry )
	{ scalableEntry= t1Entry;	}
    else{
	if  ( xft->xftScalableListIndex < 0 )
	    { LDEB(xft->xftScalableListIndex); return -1;	}

	scalableEntry= afe->afeXFontList[xft->xftScalableListIndex];
	}

    if  ( appFontXFontUsingQuery( target, scalableEntry, pixelSize ) )
	{ SLDEB(scalableEntry,pixelSize); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Get and possibly open a font.					*/
/*									*/
/*  1)  Is it an open font? If so return its number.			*/
/*  2)  Claim memory.							*/
/*  3)  Obtain a list of the available fonts.				*/
/*  4)  Try to find an exact match based on the name.			*/
/*  4a) Try the first word.						*/
/*  5)  Try to map 'fnil' to 'Helvetica'.				*/
/*  6)  Try to map 'fswiss' to 'Helvetica'.				*/
/*  7)  Try to map 'froman' to 'Times'.					*/
/*  8)  Chose the first family that has a fixed width font for 'fmodern'*/
/*  9)  Try to map 'fscript' to 'ITC Zapf Chancery'.			*/
/*									*/
/************************************************************************/

static int appGetScreenFont(	DrawScreenFont **		pApf,
				AppDrawingData *		add,
				const DocumentFont *		df,
				const TextAttribute *		ta )
    {
    DrawScreenFontList *		apfl= &(add->addScreenFontList);

    int					i;
    DrawScreenFont *			dsf;

    int					faceIndex;
    int					fullSizePixels;
    const OfficeCharsetMapping *	ocm;

    const int				twipsSize= 10* ta->taFontSizeHalfPoints;

    if  ( df->dfPsFamilyNumber < 0 )
	{ SLDEB(df->dfName,df->dfPsFamilyNumber); return -1;	}

    ocm= df->dfOfficeCharsetMapping;
    if  ( ! ocm )
	{ SXDEB(df->dfName,ocm); return -1;			}
/*
    if  ( ocm->ocmPsFontEncoding < 0 )
	{ SLDEB(df->dfName,ocm->ocmPsFontEncoding); return -1;	}
*/

    faceIndex= FACE_INDEX( ta->taFontIsSlanted, ta->taFontIsBold );
    faceIndex= df->dfPsFaceNumber[faceIndex];

    if  ( faceIndex < 0 )
	{
	SLLDEB(df->dfName,ta->taFontIsSlanted,ta->taFontIsBold);
	LDEB(FACE_INDEX( ta->taFontIsSlanted, ta->taFontIsBold ));
	LDEB(faceIndex);
	faceIndex= 0;
	}

    if  ( ta->taSuperSub == DOCfontSUPERSCRIPT		||
	  ta->taSuperSub == DOCfontSUBSCRIPT		)
	{
	fullSizePixels= TWIPStoPIXELS( add->addMagnifiedPixelsPerTwip,
						SUPERSUB_SIZE( twipsSize ) );
	}
    else{
	fullSizePixels= TWIPStoPIXELS( add->addMagnifiedPixelsPerTwip,
								twipsSize );
	}

    if  ( fullSizePixels < 2 )
	{
	/* LFLDEB(twipsSize,add->addMagnifiedPixelsPerTwip,fullSizePixels); */
	fullSizePixels= 2;
	}

    /*  1  */
    dsf= apfl->apflFonts;
    for ( i= 0; i < apfl->apflFontCount; dsf++, i++ )
	{
	if  ( df->dfPsFamilyNumber == dsf->apfPsFamilyNumber	&&
	      ocm->ocmPsFontEncoding == dsf->apfFontEncoding	&&
	      faceIndex == dsf->apfFaceIndex			&&
	      fullSizePixels == dsf->apfFullSizePixels		)
	    { *pApf= dsf; return i; }
	}

    /*  2  */
    dsf= (DrawScreenFont *)realloc( apfl->apflFonts,
			(apfl->apflFontCount+ 1)* sizeof(DrawScreenFont) );
    if  ( ! dsf )
	{ XDEB(dsf); return -1;	}
    apfl->apflFonts= dsf;
    dsf += apfl->apflFontCount;

    /*  KEY  */
    dsf->apfPsFamilyNumber= df->dfPsFamilyNumber;
    dsf->apfFontEncoding= ocm->ocmPsFontEncoding;
    dsf->apfFaceIndex= faceIndex;
    dsf->apfFullSizePixels= fullSizePixels;

    /*  DEP  */
    dsf->apfUnderLinePositionPixels= 0;
    dsf->apfUnderlineThicknessPixels= 0;
    dsf->apfAscentPixels= 0;
    dsf->apfDescentPixels= 0;
    dsf->apfXHeightPixels= 0;
    dsf->apfScapsSizePixels= 0;
    dsf->apfFontStruct= (APP_FONT *)0;
    dsf->apfScapsFontStruct= (APP_FONT *)0;

    if  ( ta->taSuperSub == DOCfontSUPERSCRIPT		||
	  ta->taSuperSub == DOCfontSUBSCRIPT		)
	{
	dsf->apfScapsSizePixels= TWIPStoPIXELS( add->addMagnifiedPixelsPerTwip,
						SUPERSUB_SIZE( twipsSize ) );
	}
    else{
	dsf->apfScapsSizePixels= TWIPStoPIXELS( add->addMagnifiedPixelsPerTwip,
						8* ta->taFontSizeHalfPoints );
	}

    if  ( dsf->apfScapsSizePixels < 2 )
	{ /* LDEB(dsf->apfScapsSizePixels); */ dsf->apfScapsSizePixels= 2; }

    *pApf= dsf;

    return apfl->apflFontCount++;
    }

/************************************************************************/
/*									*/
/*  Get the X font with a font.						*/
/*									*/
/*  1)  Determine the full font height in pixels.			*/
/*  2)  Open the font for normal use.					*/
/*  3)  Obtain some font properties.					*/
/*  4)  Try to obtain a slightly smaller font for smallcaps.		*/
/*									*/
/************************************************************************/

static int appFontOpenXFont(	AppDrawingData *		add,
				const PostScriptFontList *	psfl,
				DrawScreenFont *		dsf )
    {
    char			scratch[120];

    const AppFontFamily *	psf;
    const AppFontTypeface *	pst;

    if  ( dsf->apfPsFamilyNumber < 0				||
	  dsf->apfPsFamilyNumber >= psfl->psflFamilyCount	)
	{ LLDEB(dsf->apfPsFamilyNumber,psfl->psflFamilyCount); return -1; }

    psf= psfl->psflFamilies+ dsf->apfPsFamilyNumber;

    if  ( dsf->apfFaceIndex < 0 || dsf->apfFaceIndex >= psf->affFaceCount )
	{ LLDEB(dsf->apfFaceIndex,psf->affFaceCount); return -1;	}

    pst= psf->affFaces+ dsf->apfFaceIndex;

    /*  2  */
    if  ( appFontXFont( scratch, add, psf, dsf->apfFontEncoding, pst,
						    dsf->apfFullSizePixels ) )
	{ SLDEB(psf->affFontFamilyName,dsf->apfFontEncoding); return -1; }

#   ifdef USE_MOTIF
    dsf->apfFontStruct= XLoadQueryFont( add->addDisplay, scratch );
    if  ( ! dsf->apfFontStruct )
	{ SDEB(scratch); return -1;	}
#   endif

#   ifdef USE_GTK
    dsf->apfFontStruct= gdk_font_load( scratch );
    if  ( ! dsf->apfFontStruct )
	{ SDEB(scratch); return -1;	}
#   endif

    /*  3  */
    appXFontProperties( dsf );

    /*  4  */
    if  ( appFontXFont( scratch, add, psf, dsf->apfFontEncoding, pst,
						    dsf->apfScapsSizePixels ) )
	{ LDEB(1); return -1;	}

#   ifdef USE_MOTIF
    dsf->apfScapsFontStruct= XLoadQueryFont( add->addDisplay, scratch );
    if  ( ! dsf->apfScapsFontStruct )
	{ SDEB(scratch); return -1;	}
#   endif

#   ifdef USE_GTK
    dsf->apfScapsFontStruct= gdk_font_load( scratch );
    if  ( ! dsf->apfScapsFontStruct )
	{ SDEB(scratch); return -1;	}
#   endif

    return 0;
    }

int appOpenScreenFont(		AppDrawingData *		add,
				const DocumentFontList *	dfl,
				const TextAttribute *		ta )
    {
    const DocumentFont *	df= dfl->dflFonts+ ta->taFontNumber;
    const PostScriptFontList *	psfl= add->addPostScriptFontList;
    DrawScreenFont *		dsf;
    int				physicalFont;

    if  ( ta->taFontNumber < 0			||
	  ta->taFontNumber >= dfl->dflFontCount	)
	{ LLDEB(ta->taFontNumber,dfl->dflFontCount); return -1; }

    physicalFont= appGetScreenFont( &dsf, add, df, ta );
    if  ( physicalFont < 0 )
	{ SLDEB(df->dfName,df->dfDocFontNumber); return -1;	}

#   ifdef USE_MOTIF
    if  ( add->addDisplay && appFontOpenXFont( add, psfl, dsf ) )
	{ LDEB(1); return -1;	}
#   endif

#   ifdef USE_GTK
    if  ( add->addForScreenDrawing && appFontOpenXFont( add, psfl, dsf ) )
	{ LDEB(1); return -1;	}
#   endif

    return physicalFont;
    }

void appInitFontList(	DrawScreenFontList *	apfl )
    {
    apfl->apflFontCount= 0;
    apfl->apflFonts= (DrawScreenFont *)0;

    return;
    }

void appCleanFontList(	AppDrawingData *	add,
			DrawScreenFontList *	apfl )
    {
    int		i;

    for ( i= 0; i < apfl->apflFontCount; i++ )
	{
	if  ( apfl->apflFonts[i].apfFontStruct )
	    {
#	    ifdef USE_MOTIF
	    XFreeFont( add->addDisplay, apfl->apflFonts[i].apfFontStruct );
#	    endif

#	    ifdef USE_GTK
	    gdk_font_unref( apfl->apflFonts[i].apfFontStruct );
#	    endif
	    }

	if  ( apfl->apflFonts[i].apfScapsFontStruct )
	    {
#	    ifdef USE_MOTIF
	    XFreeFont( add->addDisplay, apfl->apflFonts[i].apfScapsFontStruct );
#	    endif

#	    ifdef USE_GTK
	    gdk_font_unref( apfl->apflFonts[i].apfScapsFontStruct );
#	    endif
	    }
	}

    if  ( apfl->apflFonts )
	{ free( apfl->apflFonts );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Does a character exist in a font?					*/
/*									*/
/*  1)  Do not trust 'xfs->all_chars_exist' by itself it is often	*/
/*	wrong. But when the other mechanism is bound to fail, we have	*/
/*	to use this unreliable struct member anyway.			*/
/*									*/
/************************************************************************/

int appCharExistsInFont(	const APP_FONT *	fs,
				int			ch )
    {
    const XFontStruct *	xfs;

#   ifdef USE_MOTIF
    xfs= fs;
#   endif

#   ifdef USE_GTK
    xfs= GDK_FONT_XFONT( (APP_FONT *)fs );
#   endif

    if  ( xfs->min_char_or_byte2 >= xfs->max_char_or_byte2	&&
	  xfs->all_chars_exist					)
	{ return 1;	}
    else{
	if  ( ! xfs->per_char )
	    {
	    if  ( ch >= xfs->min_char_or_byte2	&&
		  ch <= xfs->max_char_or_byte2	)
		{ return 1;	}

	    return 0;
	    }
	else{
	    if  ( ch >= xfs->min_char_or_byte2			&&
		  ch <= xfs->max_char_or_byte2			&&
		  xfs->per_char[ch- xfs->min_char_or_byte2].width != 0 )
		{ return 1;	}

	    return 0;
	    }
	}
    }

/************************************************************************/
/*									*/
/*  Allocate resources for drawing vertical text.			*/
/*									*/
/************************************************************************/

void appInitVerticalXFont(	VerticalXFont *	vxf )
    {
    int		i;

    vxf->vxfScratchBitmapGc= None;
    vxf->vxfPixmapGc= None;
    vxf->vxfFontStruct= (APP_FONT *)0;
    vxf->vxfScratchBitmap= None;
    vxf->vxfDirection= 0;

    for ( i= 0; i < 256; i++ )
	{ vxf->vxfCharacters[i].vcPixmap= (APP_BITMAP_IMAGE)0;	}
    }

void appCleanVerticalXFont(	VerticalXFont *		vxf,
				AppDrawingData *	add )
    {
    int		i;

#   ifdef USE_MOTIF
    if  ( vxf->vxfScratchBitmapGc )
	{ XFreeGC( add->addDisplay, vxf->vxfScratchBitmapGc );	}
    if  ( vxf->vxfPixmapGc )
	{ XFreeGC( add->addDisplay, vxf->vxfPixmapGc );	}
#   endif

#   ifdef USE_GTK
    if  ( vxf->vxfScratchBitmapGc )
	{ gdk_gc_unref( vxf->vxfScratchBitmapGc );	}
    if  ( vxf->vxfPixmapGc )
	{ gdk_gc_unref( vxf->vxfPixmapGc );		}
#   endif

    if  ( vxf->vxfFontStruct )
	{ appDrawFreeFont( add, vxf->vxfFontStruct );	}

    if  ( vxf->vxfScratchBitmap )
	{ appDrawFreePixmap( add, vxf->vxfScratchBitmap );	}

    for ( i= 0; i < 256; i++ )
	{
	if  ( vxf->vxfCharacters[i].vcPixmap )
	    {
	    appDrawFreePixmap( add, vxf->vxfCharacters[i].vcPixmap );
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Open a font and initialize the structures to use it to draw		*/
/*  vertical text.							*/
/*									*/
/************************************************************************/

int appOpenVerticalXFont(	VerticalXFont *		vxf,
				AppDrawingData *	add,
				const char *		name,
				int			direction,
				APP_COLOR_RGB *		background )
    {
    XFontStruct *	xfs;

    if  ( direction == 0 )
	{ LDEB(direction); return -1;	}

    if  ( direction > 0 )
	{ vxf->vxfDirection=  1;	}
    if  ( direction < 0 )
	{ vxf->vxfDirection= -1;	}

    vxf->vxfFontStruct= appDrawOpenFont( add, name );
    if  ( ! vxf->vxfFontStruct )
	{ SXDEB(name,vxf->vxfFontStruct); return -1;	}

#   ifdef USE_MOTIF
    xfs= vxf->vxfFontStruct;
#   endif

#   ifdef USE_GTK
    xfs= GDK_FONT_XFONT( vxf->vxfFontStruct );
#   endif

    vxf->vxfScratchBitmap= appMakeBitmap( add,
					    xfs->max_bounds.width,
					    xfs->max_bounds.ascent+
					    xfs->max_bounds.descent );

    if  ( ! vxf->vxfScratchBitmap )
	{ XDEB(vxf->vxfScratchBitmap); return -1;	}

#   ifdef USE_MOTIF
    vxf->vxfScratchBitmapGc= XCreateGC( add->addDisplay,
				vxf->vxfScratchBitmap, 0, (XGCValues *)0 );
    if  ( ! vxf->vxfScratchBitmapGc )
	{ XDEB(vxf->vxfScratchBitmapGc); return -1;	}
    vxf->vxfPixmapGc= XCreateGC( add->addDisplay, add->addDrawable,
							0, (XGCValues *)0 );
    if  ( ! vxf->vxfPixmapGc )
	{ XDEB(vxf->vxfPixmapGc); return -1;	}

    XSetFont( add->addDisplay, vxf->vxfScratchBitmapGc,
						vxf->vxfFontStruct->fid );

    XSetBackground( add->addDisplay, vxf->vxfPixmapGc,
			    background->pixel );
    XSetForeground( add->addDisplay, vxf->vxfPixmapGc,
			    BlackPixel( add->addDisplay, add->addScreen ) );
#   endif

#   ifdef USE_GTK
    vxf->vxfScratchBitmapGc= gdk_gc_new( vxf->vxfScratchBitmap );
    if  ( ! vxf->vxfScratchBitmapGc )
	{ XDEB(vxf->vxfScratchBitmapGc); return -1;	}
    vxf->vxfPixmapGc= gdk_gc_new( add->addDrawable );
    if  ( ! vxf->vxfPixmapGc )
	{ XDEB(vxf->vxfPixmapGc); return -1;	}

    gdk_gc_set_font( vxf->vxfScratchBitmapGc, vxf->vxfFontStruct );

    gdk_gc_set_background( vxf->vxfPixmapGc, background );
    /*
    gdk_gc_set_foreground( vxf->vxfPixmapGc, appDrawBlackColor( add ) );
    */
#   endif

    return 0;
    }

int appDrawVerticalString(	VerticalXFont *		vxf,
				AppDrawingData *	add,
				int			x,
				int			y,
				char *			string,
				int			length )
    {
    int			angle;

#   ifdef USE_MOTIF
    Display *			display= add->addDisplay;
    XFontStruct*		xfs= vxf->vxfFontStruct;
#   endif

#   ifdef USE_GTK
    XFontStruct*		xfs= GDK_FONT_XFONT( vxf->vxfFontStruct );
#   endif

    if  ( vxf->vxfDirection < 0 )
	{ angle=  90; x -= xfs->ascent;		}
    else{ angle= 270; x -= xfs->descent;	}

    while( length > 0 )
	{
	int		width;
	int		ch= string[0]&0xff;

	if  ( ! vxf->vxfCharacters[ch].vcPixmap )
	    {
	    XCharStruct *	xcs;
#	    ifdef USE_MOTIF
	    int			screen= DefaultScreen( display );
	    Visual *		visual= DefaultVisual( display, screen);
#	    endif
	    int			depth= 1;

	    APP_IMAGE *		xim;

	    unsigned char *	bufferIn;
	    unsigned char *	bufferOut= (unsigned char *)0;

	    BitmapDescription	bdIn;
	    BitmapDescription	bdOut;

	    bmInitDescription( &bdIn );
	    bmInitDescription( &bdOut );

	    if  ( ! xfs->per_char )
		{ xcs= &(xfs->max_bounds);	}
	    else{
		if  ( ch < xfs->min_char_or_byte2	||
		      ch > xfs->max_char_or_byte2	)
		    { length--; string++; continue;	}

		xcs= xfs->per_char+ ch- xfs->min_char_or_byte2;
		}

#	    ifdef USE_GTK
	    LDEB(1); return -1;
#	    endif

#	    ifdef USE_MOTIF
	    XSetForeground( display, vxf->vxfScratchBitmapGc, 0 );
	    XFillRectangle( display, vxf->vxfScratchBitmap,
			    vxf->vxfScratchBitmapGc,
			    0, 0, xcs->width,
			    xfs->max_bounds.ascent+ xfs->max_bounds.descent );

	    XSetForeground( display, vxf->vxfScratchBitmapGc, 1 );
	    XDrawString( display, vxf->vxfScratchBitmap,
					vxf->vxfScratchBitmapGc,
					0, xfs->max_bounds.ascent,
					string, 1 );

	    xim= XGetImage( display, vxf->vxfScratchBitmap,
			    0, 0, xcs->width,
			    xfs->max_bounds.ascent+ xfs->max_bounds.descent,
			    AllPlanes, XYPixmap );

	    if  ( ! xim )
		{ XDEB(xim); return -1;	}

	    if  ( xim->depth != 1 )
		{ LDEB(xim->depth); return -1;	}
#	    endif

	    bdIn.bdPixelsWide= xim->width;
	    bdIn.bdPixelsHigh= xim->height;
#	    ifdef USE_MOTIF
	    bdIn.bdBytesPerRow= xim->bytes_per_line;
#	    endif
#	    ifdef USE_GTK
	    bdIn.bdBytesPerRow= xim->bpl;
#	    endif
	    bdIn.bdBufferLength= bdIn.bdPixelsHigh* bdIn.bdBytesPerRow;

	    bdIn.bdBitsPerSample= 1;
	    bdIn.bdSamplesPerPixel= 1;
	    bdIn.bdBitsPerPixel= 1;

	    bdIn.bdXResolution= 1;
	    bdIn.bdYResolution= 1;
	    bdIn.bdUnit= BMunPIXEL;
	    bdIn.bdColorEncoding= BMcoBLACKWHITE;
	    bdIn.bdHasAlpha= 0;

#	    ifdef USE_MOTIF
	    if  ( xim->bitmap_bit_order == LSBFirst )
		{
		bmFlipBytes( (unsigned char *)xim->data,
				    bdIn.bdBufferLength, bdIn.bdBitsPerPixel );
		}
#	    endif

#	    ifdef USE_MOTIF
	    bufferIn= (unsigned char *)xim->data;
#	    endif
#	    ifdef USE_GTK
	    bufferIn= (unsigned char *)xim->mem;
#	    endif

	    /*
	    bmWrite( "/tmp/in.tiff",
				bufferIn, &bdIn, 0, 1.0 );
	    */

	    if  ( bmRotate90( &bdOut, &bdIn,
				&bufferOut, bufferIn, angle ) )
		{ LDEB(angle); return -1;	}

	    /* bmWrite( "/tmp/out.tiff", bufferOut, &bdOut, 0, 1.0 ); */

#	    ifdef USE_MOTIF
	    XDestroyImage( xim );
#	    endif
#	    ifdef USE_GTK
	    gdk_image_destroy( xim );
#	    endif

	    bmCleanDescription( &bdIn );

	    depth= 1;

#	    ifdef USE_MOTIF
	    vxf->vxfCharacters[ch].vcPixmap= XCreatePixmap(
				add->addDisplay, add->addDrawable,
				bdOut.bdPixelsWide, bdOut.bdPixelsHigh, depth );

	    if  ( ! vxf->vxfCharacters[ch].vcPixmap )
		{ XDEB(vxf->vxfCharacters[ch].vcPixmap); return -1;	}

	    xim= XCreateImage( add->addDisplay, visual, 1, XYBitmap,
				    0, (char *)bufferOut,
				    bdOut.bdPixelsWide, bdOut.bdPixelsHigh,
				    8, bdOut.bdBytesPerRow );
	    if  ( ! xim )
		{ XDEB(xim); return -1;	}

	    if  ( xim->bitmap_bit_order == LSBFirst )
		{
		bmFlipBytes( (unsigned char *)xim->data,
				bdOut.bdBufferLength, bdOut.bdBitsPerPixel );
		}

	    XSetBackground( add->addDisplay, vxf->vxfScratchBitmapGc, 0 );
	    XSetForeground( add->addDisplay, vxf->vxfScratchBitmapGc, 1 );

	    XPutImage( add->addDisplay, vxf->vxfCharacters[ch].vcPixmap,
				    vxf->vxfScratchBitmapGc, xim, 0, 0, 0, 0,
				    bdOut.bdPixelsWide, bdOut.bdPixelsHigh );
#	    endif

#	    ifdef USE_MOTIF
	    xim->data= (char *)0;
	    XDestroyImage( xim );
#	    endif
#	    ifdef USE_GTK
	    xim->mem= (char *)0;
	    gdk_image_destroy( xim );
#	    endif

	    bmCleanDescription( &bdOut );
	    free( bufferOut );

	    vxf->vxfCharacters[ch].vcWidth= xcs->width;
	    }

	width= vxf->vxfCharacters[ch].vcWidth;

	if  ( vxf->vxfDirection < 0 )
	    { y -= width;	}

#	ifdef USE_MOTIF
	XSetClipMask( add->addDisplay, add->addGc,
					    vxf->vxfCharacters[ch].vcPixmap );
	XSetClipOrigin( add->addDisplay, add->addGc, x, y );

	XFillRectangle( add->addDisplay, add->addDrawable, add->addGc, x, y,
		    xfs->max_bounds.ascent+ xfs->max_bounds.descent,
		    width );

	XSetClipMask( add->addDisplay, add->addGc, None );
#	endif

	if  ( vxf->vxfDirection > 0 )
	    { y += width;	}

	length--; string++;
	}

    return 0;
    }
