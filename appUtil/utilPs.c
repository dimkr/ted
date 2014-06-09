/************************************************************************/
/*									*/
/*  Print Postscript.							*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<time.h>

#   include	<utilPs.h>
#   include	<utilTree.h>
#   include	<geo2DInteger.h>
#   include	<utilFontmap.h>
#   include	<utilPostscriptFace.h>
#   include	<appSystem.h>
#   include	<sioStdio.h>
#   include	<sioPfb.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Print a bounding box.						*/
/*									*/
/*  %%BoundingBox <llx> <lly> <urx> <ury>				*/
/*									*/
/************************************************************************/

static void utilPsBoundingBoxComment(
			    const PrintingState *	ps,
			    const char *		comment,
			    const char *		orientationComment )
    {
    SimpleOutputStream *	sos= ps->psSos;

    sioOutPrintf( sos, "%%%%%s: %d %d %d %d\n",
			    comment,
			    ps->psSheetBoundingBox.drX0,
			    ps->psSheetBoundingBox.drY1,
			    ps->psSheetBoundingBox.drX1,
			    ps->psSheetBoundingBox.drY0 );

    sioOutPrintf( sos, "%%%%%s: %s\n",
				    orientationComment, ps->psOrientation );
    }

/************************************************************************/
/*									*/
/*  List the font names that appear in a document.			*/
/*									*/
/************************************************************************/

static void utilPsGetEmbed(		PostScriptFace *	psf,
					const char *		fontDirectory )
    {
    const char *	fontFile;

    fontFile= utilFontmapGetEntry( psf->psfAfi->afiFontName );
    if  ( fontFile )
	{
	int		relativeIsFile= 0;
	char		absolute[1000+1];
	int		l;

	l= appAbsoluteName( absolute, sizeof(absolute)-1, fontFile,
					    relativeIsFile, fontDirectory );
	if  ( l >= 0 )
	    {
	    if  ( ! appTestFileExists( absolute ) )
		{
		const char *	extension;

		if  ( psf->psfFontFileName )
		    { free( psf->psfFontFileName );	}

		psf->psfFontFileName= strdup( absolute );
		if  ( ! psf->psfFontFileName )
		    { XDEB(psf->psfFontFileName); return;	}

		extension= appFileExtensionOfName( psf->psfFontFileName );

		psf->psfEmbed= PSembedNO;

		if  ( extension &&
		      ( ! strcmp( extension, "pfb" ) 	||
			! strcmp( extension, "PFB" ) 	)	)
		    { psf->psfEmbed= PSembedBTOA;	}

		if  ( extension &&
		      ( ! strcmp( extension, "pfa" ) 	||
			! strcmp( extension, "PFA" ) 	)	)
		    { psf->psfEmbed= PSembedCOPY;	}

		if  ( extension &&
		      ( ! strcmp( extension, "ttf" ) 	||
			! strcmp( extension, "TTF" ) 	)	)
		    { psf->psfEmbed= PSembedTTFTO42;	}

		psf->psfFontFileNameLength= l;
		}
	    else{ psf->psfEmbed= PSembedNO;	}
	    }
	}
    else{ psf->psfEmbed= PSembedNO;	}

    return;
    }

static void utilPsListFontNames(	SimpleOutputStream *		sos,
					const PostScriptTypeList *	pstl )
    {
    PostScriptFace *	psf;
    const char *	fontName;
    int			done;

    if  ( ! pstl->pstlFaceTree )
	{ return;	}

    done= 0;
    psf= (PostScriptFace *)utilTreeGetFirst( pstl->pstlFaceTree,
								&fontName );
    while( psf )
	{
	if  ( done > 0 )
	    {
	    sioOutPrintf( sos, "%%%%+ %s\n", psf->psfAfi->afiFontName );
	    }
	else{
	    sioOutPrintf( sos, "%%%%DocumentFonts: %s\n",
					    psf->psfAfi->afiFontName );
	    }

	done++;
	psf= (PostScriptFace *)utilTreeGetNext( pstl->pstlFaceTree,
							    &fontName );
	}

    if  ( pstl->pstlFontDirectory )
	{
	done= 0;
	psf= (PostScriptFace *)utilTreeGetFirst( pstl->pstlFaceTree,
								&fontName );
	while( psf )
	    {
	    if  ( psf->psfEmbed == PSembedUNKNOWN )
		{ utilPsGetEmbed( psf, pstl->pstlFontDirectory );	}

	    if  ( psf->psfEmbed > 0 )
		{
		if  ( done > 0 )
		    {
		    sioOutPrintf( sos, "%%%%+ %s\n", psf->psfAfi->afiFontName );
		    }
		else{
		    sioOutPrintf( sos, "%%%%DocumentSuppliedFonts: %s\n",
						    psf->psfAfi->afiFontName );
		    }

		done++;
		}

	    psf= (PostScriptFace *)utilTreeGetNext( pstl->pstlFaceTree,
								&fontName );
	    }
	}

    done= 0;
    psf= (PostScriptFace *)utilTreeGetFirst( pstl->pstlFaceTree,
							    &fontName );
    while( psf )
	{
	if  ( psf->psfEmbed <= 0 )
	    {
	    if  ( done > 0 )
		{
		sioOutPrintf( sos, "%%%%+ %s\n", psf->psfAfi->afiFontName );
		}
	    else{
		sioOutPrintf( sos, "%%%%DocumentNeededFonts: %s\n",
						psf->psfAfi->afiFontName );
		}

	    done++;
	    }

	psf= (PostScriptFace *)utilTreeGetNext( pstl->pstlFaceTree,
							    &fontName );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Write a DSC comment. Fold the value, not to exceed the maximum line	*/
/*  width.								*/
/*									*/
/************************************************************************/

static void utilPsFoldComment(	SimpleOutputStream *	sos,
				const char *		name,
				const char *		value )
    {
    int         done;
    int		len= strlen( value );

    int		tl= strlen( name );

    while( len > 0 )
	{
	int     l;

	l= len;

	if  ( len+ tl > 72 )
	    {
	    int         ll;

	    l= ll= 0;
	    while( ll+ tl < 72 && ll < len )
		{
		if  ( isspace( value[ll] ) )
		    { l= ll+ 1; }

		ll++;
		}

	    if  ( l == 0 )
		{ l= ll;        }
	    else{
		while( isspace( value[l] ) && l < len )
		    { l++;      }
		}
	    }

	sioOutPrintf( sos, "%%%%%s ", name );
	name= "+"; tl= 1;

	for ( done= 0; done < l; done++ )
	    { sioOutPutCharacter( value[done], sos );	}

	len -= l; value += l;
	sioOutPrintf( sos, "\n" );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Start a DSC convention document.					*/
/*									*/
/************************************************************************/

void utilPsStartDSCDocument(
			const PrintingState *		ps,
			const PostScriptTypeList *	pstl,
			const char *			title,
			const char *			creatorName,
			const char *			creatorReference )
    {
    SimpleOutputStream *	sos= ps->psSos;

    sioOutPrintf( sos, "%%!PS-Adobe-2.0\n" );
    if  ( title )
	{ utilPsFoldComment( sos, "Title:", title );	}

    utilPsBoundingBoxComment( ps, "BoundingBox", "Orientation" );

    sioOutPrintf( sos, "%%%%Creator: %s: %s\n",
				creatorName, creatorReference );
    sioOutPrintf( sos, "%%%%Pages: (atend)\n" );
    sioOutPrintf( sos, "%%%%PageOrder: Ascend\n" );

    if  ( pstl )
	{ utilPsListFontNames( sos, pstl );	}

    sioOutPrintf( sos, "%%%%EndComments\n" );

    return;
    }

/************************************************************************/
/*									*/
/*  Initialisation for 'nup' printing.					*/
/*									*/
/************************************************************************/

int utilPsSetNupSchema(	PrintingState *			ps,
			const DocumentGeometry *	dgPage,
			const PrintGeometry *		pg,
			int				hasPageHeader,
			int				hasPageFooter )
    {
    const double		fac= 0.05;
    int				rotatePages;

    AffineTransform2D		at1Page;

    if  ( utilNupGetBaseTranform( &at1Page, &rotatePages, pg, dgPage, fac ) )
	{ LDEB(1); return -1;	}

    ps->psRotateSheetGrid= rotatePages;
    ps->psPrintGeometry= *pg;

    if  ( utilNupSetSchema( &(ps->psNupSchema),
			ps->psRotateSheetGrid, &at1Page, pg, fac, dgPage ) )
	{ LLDEB(pg->pgGridRows,pg->pgGridCols); return -1;	}

    if  ( ps->psRotateSheetGrid )
	{ ps->psOrientation= "Landscape";	}
    else{ ps->psOrientation= "Portrait";	}

    utilNupSheetBoundingBox( &(ps->psSheetBoundingBox), &(ps->psNupSchema),
					dgPage, hasPageHeader, hasPageFooter );

    return 0;
    }

void utilPsRefreshNupSchema(
			PrintingState *			ps,
			const DocumentGeometry *	dgPage )
    {
    const double		fac= 0.05;
    const PrintGeometry *	pg= &(ps->psPrintGeometry);
    AffineTransform2D		at1Page;

    int				rotatePages; /* ignored */

    if  ( utilNupGetBaseTranform( &at1Page, &rotatePages, pg, dgPage, fac ) )
	{ LDEB(1); return;	}

    if  ( utilNupSetSchema( &(ps->psNupSchema),
			ps->psRotateSheetGrid, &at1Page, pg, fac, dgPage ) )
	{ LLDEB(pg->pgGridRows,pg->pgGridCols); return;	}

    /* ??
    utilNupSheetBoundingBox( &(ps->psSheetBoundingBox), &(ps->psNupSchema),
					dgPage, hasPageHeader, hasPageFooter );
    */

    return;
    }

/************************************************************************/
/*									*/
/*  Issue a character string.						*/
/*									*/
/************************************************************************/

void utilPsPrintString(	PrintingState *		ps,
			const unsigned char *	s,
			int			len )
    {
    SimpleOutputStream *	sos= ps->psSos;
    int				i;

    for ( i= 0; i < len; s++, i++ )
	{
	if  ( *s == '(' || *s == ')' || *s == '\\' )
	    {
	    sioOutPutCharacter( '\\', sos );
	    sioOutPutCharacter( *s, sos );
	    continue;
	    }

	if  ( *s == '\r' )
	    {
	    sioOutPutCharacter( '\\', sos );
	    sioOutPutCharacter( 'r', sos );
	    continue;
	    }

	if  ( *s == '\n' )
	    {
	    sioOutPutCharacter( '\\', sos );
	    sioOutPutCharacter( 'n', sos );
	    continue;
	    }

	sioOutPutCharacter( *s, sos );
	}

    return;
    }

void utilPsPrintStringValue(	PrintingState *		ps,
				const unsigned char *	s,
				int			len )
    {
    SimpleOutputStream *	sos= ps->psSos;

    sioOutPutCharacter( '(', sos );
    utilPsPrintString( ps, s, len );
    sioOutPutCharacter( ')', sos );

    return;
    }

void utilPsMovePrintStringValue(	PrintingState *		ps,
					const unsigned char *	s,
					int			len,
					int			x,
					int			y )
    {
    SimpleOutputStream *	sos= ps->psSos;

    sioOutPutCharacter( '(', sos );
    utilPsPrintString( ps, s, len );
    sioOutPrintf( sos, ") %d %d mvs\n", x, y );

    ps->psLastPageMarked= ps->psPagesPrinted;

    return;
    }

/************************************************************************/
/*									*/
/*  Set the font, this depends on the assumption that fonts have been	*/
/*  defined for the different fonts in the document.			*/
/*									*/
/*  NOTE that 'smallcaps' is never set for characters that are not to	*/
/*  be scaled.								*/
/*									*/
/************************************************************************/

void utilPsSetFont(	PrintingState *			ps,
			const char *			prefix,
			const TextAttribute *		ta )
    {
    SimpleOutputStream *	sos= ps->psSos;
    int				fontSizeTwips= 10* ta->taFontSizeHalfPoints;

    if  ( ta->taSuperSub == DOCfontSUPERSCRIPT	||
	  ta->taSuperSub == DOCfontSUBSCRIPT	)
	{ fontSizeTwips= SUPERSUB_SIZE( fontSizeTwips ); }

    if  ( ta->taSmallCaps )
	{ fontSizeTwips= ( 8* fontSizeTwips )/ 10;	}

    if  ( fontSizeTwips == 0 )
	{ LDEB(fontSizeTwips); fontSizeTwips= 1;	}

    sioOutPrintf( sos, "[ %d 0 0 %d 0 0 ] %s%d",
		fontSizeTwips, -fontSizeTwips, prefix, ta->taFontNumber );

    if  ( ta->taFontIsBold )
	{ sioOutPutCharacter( 'b', sos );	}
    if  ( ta->taFontIsSlanted )
	{ sioOutPutCharacter( 'i', sos );	}
    sioOutPutCharacter( '\n', sos );

    ps->psLastPageMarked= ps->psPagesPrinted;

    return;
    }

/************************************************************************/
/*									*/
/*  Initialise a printing session.					*/
/*									*/
/************************************************************************/

void appPsInitPrintingState(	PrintingState *	ps )
    {
    ps->psSos= (SimpleOutputStream *)0;

    ps->psLastPageMarked= -1;
    ps->psLastSheetMarked= -1;
    ps->psPagesPrinted= 0;
    ps->psSheetsPrinted= 0;
    ps->psSheetsStarted= 0;

    utilInitPrintGeometry( &(ps->psPrintGeometry) );

    utilInitNupSchema( &(ps->psNupSchema) );
    ps->psRotateSheetGrid= 0;
    utilIdentityAffineTransform2D( &(ps->psCurrentTransform) );

    ps->psOrientation= (char *)0;

    docInitRectangle( &(ps->psSheetBoundingBox) );

    ps->psInsideLink= 0;

    ps->psLinkParticulesDone= 0;
    ps->psLinkRectLeft= -1;
    ps->psLinkFile= (const char *)0;
    ps->psLinkFileSize= 0;
    ps->psLinkMark= (const char *)0;
    ps->psLinkMarkSize= 0;

    ps->psUsePostScriptFilters= 1;
    ps->psUsePostScriptIndexedImages= 1;

    utilCleanPrintGeometry( &(ps->psPrintGeometry) );

    return;
    }

void appPsCleanPrintingState(	PrintingState *	ps )
    {
    utilCleanNupSchema( &(ps->psNupSchema) );

    return;
    }

/************************************************************************/
/*									*/
/*  Set the color for subsequent drawing operations.			*/
/*									*/
/************************************************************************/

void utilPsSetRgbColor(	PrintingState *		ps,
			double			r,
			double			g,
			double			b )
    {
    if  ( r == g && g == b )
	{ sioOutPrintf( ps->psSos, "%g setgray\n", r );			}
    else{ sioOutPrintf( ps->psSos, "%g %g %g setrgbcolor\n", r, g, b );	}

    ps->psLastPageMarked= ps->psPagesPrinted;

    return;
    }

/************************************************************************/
/*									*/
/*  Go to the next page.						*/
/*									*/
/************************************************************************/

static void utilPsPageOperator(		const char *		operator,
					const PrintingState *	ps,
					int			documentPage )
    {
    sioOutPrintf( ps->psSos, "%s %% Page doc:%d -> print:%d Sheet %d\n",
				    operator,
				    documentPage+ 1,
				    ps->psPagesPrinted+ 1,
				    ps->psSheetsPrinted+ 1 );
    }

void utilPsStartPage(	PrintingState *			ps,
			int				documentPage )
    {
    int				nup= ps->psNupSchema.nsNup;
    int				firstOnSheet= 0;

    if  ( nup == 1			||
	  ps->psPagesPrinted % nup == 0	)
	{ firstOnSheet= 1;	}

    if  ( ps->psSheetsStarted > ps->psSheetsPrinted )
	{ firstOnSheet= 0;	}

    if  ( firstOnSheet )
	{
	if  ( nup == 1 )
	    {
	    sioOutPrintf( ps->psSos, "%%%%Page: %d %d\n",
				    documentPage+ 1, ps->psSheetsPrinted+ 1 );
	    }
	else{
	    sioOutPrintf( ps->psSos, "%%%%Page: (%d ..) %d\n",
				    documentPage+ 1, ps->psSheetsPrinted+ 1 );
	    }

	utilPsBoundingBoxComment( ps, "PageBoundingBox", "PageOrientation" );

	sioOutPrintf( ps->psSos, "%%%%BeginPageSetup\n" );
	ps->psSheetsStarted= ps->psSheetsPrinted+ 1;
	}

    utilNupGetPageTranform( &(ps->psCurrentTransform),
				    &(ps->psNupSchema), ps->psPagesPrinted );

    utilPsPageOperator( "gsave", ps, documentPage );

#   if 0
    if  ( firstOnSheet )
	{
	const DocumentRectangle *	dr= &(ps->psSheetBoundingBox);

	sioOutPrintf( ps->psSos, "newpath" );
	sioOutPrintf( ps->psSos, " %d %d moveto", dr->drX0, dr->drY0 );
	sioOutPrintf( ps->psSos, " %d %d lineto", dr->drX1, dr->drY0 );
	sioOutPrintf( ps->psSos, " %d %d lineto", dr->drX1, dr->drY1 );
	sioOutPrintf( ps->psSos, " %d %d lineto", dr->drX0, dr->drY1 );
	sioOutPrintf( ps->psSos, " %d %d lineto", dr->drX0, dr->drY0 );

	sioOutPrintf( ps->psSos, " stroke\n" );
	}
#   endif

    utilPsTransformMatrix( ps->psSos, &(ps->psCurrentTransform) );
    sioOutPrintf( ps->psSos, " concat\n" );

    if  ( firstOnSheet )
	{ sioOutPrintf( ps->psSos, "%%%%EndPageSetup\n" );	}

    return;
    }

void utilPsTransformMatrix(	SimpleOutputStream *		sos,
				const AffineTransform2D *	at )
    {
    sioOutPrintf( sos, "[ %g %g %g %g %g %g ]",
				    at->at2Axx, at->at2Axy,
				    at->at2Ayx, at->at2Ayy,
				    at->at2Tx, at->at2Ty );
    return;
    }

/************************************************************************/
/*									*/
/*  Finish the page.							*/
/*									*/
/*  1)  Print something invisible on absolutely empty pages: Some	*/
/*	versions of acroread (5 for windows) do not like the pdf that	*/
/*	ghostscript generates for an absolutely empty page.		*/
/*									*/
/************************************************************************/

/*  1  */
static void utilPsDrawSometingInvisible(	PrintingState *	ps )
    {
    sioOutPrintf( ps->psSos,
	    "1 setgray 0 0 moveto 1 0 rlineto stroke"
	    "  %% Avoid an empty page\n" );
    }

void utilPsFinishPage(	PrintingState *		ps,
			int			documentPage,
			int			asLast )
    {
    int			nup= ps->psNupSchema.nsNup;
    int			pageIsEmpty;
    int			sheetIsEmpty;

    pageIsEmpty= ps->psLastPageMarked < ps->psPagesPrinted;
    sheetIsEmpty= ps->psLastSheetMarked < ps->psSheetsPrinted;

    if  ( ! pageIsEmpty )
	{
	ps->psLastSheetMarked= ps->psSheetsPrinted;
	sheetIsEmpty= 0;
	}

    if  ( asLast				||
	  nup == 1				||
	  ( ps->psPagesPrinted+ 1 ) % nup == 0	)
	{
	/*  1  */
	if  ( sheetIsEmpty )
	    { utilPsDrawSometingInvisible( ps );	}

	utilPsPageOperator( "showpage grestore", ps, documentPage );

	sioOutPrintf( ps->psSos, "%%%%PageTrailer\n" );

	ps->psPagesPrinted++;
	ps->psSheetsPrinted++;
	}
    else{
	utilPsPageOperator( "grestore", ps, documentPage );

	ps->psPagesPrinted++;
	}

    if  ( asLast )
	{
	sioOutPrintf( ps->psSos, "%%%%Trailer\n" );
	sioOutPrintf( ps->psSos, "%%%%Pages: %d\n", ps->psSheetsPrinted );
	sioOutPrintf( ps->psSos, "%%%%EOF\n" );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Abort a page as there is a reason not to print it. Usually, this is	*/
/*  because it is empty.						*/
/*									*/
/************************************************************************/

void utilPsAbortPage(	PrintingState *		ps,
			int			documentPage,
			int			asLast )
    {
    if  ( asLast					&&
	  ps->psLastSheetMarked == ps->psSheetsPrinted	)
	{ utilPsPageOperator( "showpage grestore", ps, documentPage );	}
    else{ utilPsPageOperator( "grestore", ps, documentPage );		}

    if  ( asLast )
	{
	sioOutPrintf( ps->psSos, "%%%%Trailer\n" );
	sioOutPrintf( ps->psSos, "%%%%Pages: %d\n", ps->psSheetsPrinted );
	sioOutPrintf( ps->psSos, "%%%%EOF\n" );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Define a font encoding.						*/
/*									*/
/*  1)  Store an array with the desired name in the cirrent dictionary.	*/
/*  2)  Store '.notdef' in all positions of the array.			*/
/*  3)  Store glyph names in the array.					*/
/*									*/
/************************************************************************/

static void utilPsDefineFontEncoding(
				SimpleOutputStream *	sos,
				const char *		name,
				const char * const *	glyphNames )
    {
    int		i;

    /*  1  */
    sioOutPrintf( sos, "/%s 256 array def\n", name );

    /*  2  */
    sioOutPrintf( sos, "0 1 255 { %s exch /.notdef put } for %s\n",
							    name, name );

    /*  3  */
    for ( i= 0; i < 256; i++ )
	{
	if  ( glyphNames[i] )
	    { sioOutPrintf( sos, "  dup %3d /%s put\n", i, glyphNames[i] ); }
	}

    sioOutPrintf( sos, "readonly\n\n" );

    return;
    }

/************************************************************************/
/*									*/
/*  Make function names for setting the fonts in a list.		*/
/*									*/
/*  1)  Define all standard encodings.					*/
/*  2)  Define all encoded fonts.					*/
/*									*/
/************************************************************************/

static void utilPsDefineEncodedFont(
				SimpleOutputStream *	sos,
				const AfmFontInfo *	afi,
				const char *		encodingSuffix,
				const char *		encodingArrayName,
				const char *		exceptionSuffix )
    {
    sioOutPrintf( sos, "/%s findfont dup length dict begin\n",
							afi->afiFontName );
    sioOutPrintf( sos, "  {\n" );
    sioOutPrintf( sos, "    1 index /FID ne\n" );
    sioOutPrintf( sos, "      { def } { pop pop } ifelse\n" );
    sioOutPrintf( sos, "  } forall\n");
    sioOutPrintf( sos, "  /Encoding %s%s def currentdict\n",
					encodingArrayName, exceptionSuffix );
    sioOutPrintf( sos, "end " );

    sioOutPrintf( sos, "/%s%s%s exch definefont pop\n\n",
				afi->afiFontName,
				encodingSuffix?encodingSuffix:"",
				exceptionSuffix );

    return;
    }

int appPsIncludeFonts(	PrintingState *			ps,
			const PostScriptTypeList *	pstl )
    {
    const char *		fontName;
    PostScriptFace *		psf;

    SimpleOutputStream *	sos= ps->psSos;

    psf= (PostScriptFace *)utilTreeGetFirst( pstl->pstlFaceTree,
								&fontName );
    while( psf )
	{
	if  ( psf->psfEmbed == PSembedUNKNOWN )
	    { utilPsGetEmbed( psf, pstl->pstlFontDirectory );	}

	if  ( psf->psfEmbed > 0 )
	    {
	    SimpleInputStream *	sisFile= (SimpleInputStream *)0;
	    SimpleInputStream *	sisPfb= (SimpleInputStream *)0;
	    SimpleInputStream *	sis= (SimpleInputStream *)0;

	    sisFile= sioInStdioOpen( psf->psfFontFileName );
	    if  ( ! sisFile )
		{ SXDEB(psf->psfFontFileName,sisFile); return -1;	}
	    sis= sisFile;

	    sioOutPrintf( sos, "\n" );
	    sioOutPrintf( sos, "%%%%%%%%\n" );
	    sioOutPrintf( sos, "%%%%%%%%    %s\n", psf->psfAfi->afiFullName );
	    sioOutPrintf( sos, "%%%%%%%% :: %s\n", psf->psfFontFileName );
	    sioOutPrintf( sos, "%%%%%%%%\n" );

	    sioOutPrintf( sos, "%%%%BeginFont %s\n",
					    psf->psfAfi->afiFontName );

	    switch( psf->psfEmbed )
		{
		int		c;
		int		atBol;

		case PSembedBTOA:
		    sisPfb= sioInPfbOpen( sisFile );
		    if  ( ! sisPfb )
			{
			SXDEB(psf->psfFontFileName,sisPfb);
			sioInClose( sisFile );
			return -1;
			}

		    sis= sisPfb;

		    /*FALLTHROUGH*/
		case PSembedCOPY:
		    atBol= 1;
		    while( ( c= sioInGetCharacter( sis ) ) != EOF )
			{
			sioOutPutCharacter( c, sos );
			atBol= c == '\n';
			}

		    if  ( ! atBol )
			{ sioOutPutCharacter( '\n', sos );	}

		    break;

		case PSembedTTFTO42:
		    if  ( psTtfToPf42( sos,  psf->psfFontFileName, sisFile ) )
			{
			SDEB(psf->psfFontFileName);
			sioInClose( sisFile );
			return -1;
			}

		    break;

		default:
		    LDEB(psf->psfEmbed);
		    sioInClose( sisFile );
		    return -1;
		}

	    sioOutPrintf( sos, "%%%%EndFont %s\n",
					    psf->psfAfi->afiFontName );

	    if  ( sisPfb )
		{ sioInClose( sisPfb );	}
	    if  ( sisFile )
		{ sioInClose( sisFile );	}
	    }

	psf= (PostScriptFace *)utilTreeGetNext( pstl->pstlFaceTree,
								&fontName );
	}

    return 0;
    }

void appPsFontNames(	PrintingState *			ps,
			const PostScriptTypeList *	pstl,
			int				allFonts )
    {
    SimpleOutputStream *	sos= ps->psSos;

    const char *		fontName;
    PostScriptFace *		psf;

    int				enc;
    const FontCharset *		fc;

    int				faceNumber= 1;

    /*  1  */
    fc= PS_Encodings;
    for ( enc= 0; enc < ENCODINGps_COUNT; fc++, enc++ )
	{
	if  ( pstl->pstlEncodingUsed[enc] )
	    {
	    sioOutPrintf( sos, "\n" );
	    utilPsDefineFontEncoding( sos,
				fc->fcPsEncodingArrayName, fc->fcGlyphNames );
	    }
	}

    /*  2  */
    psf= (PostScriptFace *)utilTreeGetFirst( pstl->pstlFaceTree, &fontName );
    while( psf )
	{
	const AfmFontInfo *	afi= psf->psfAfi;
	char			exceptionSuffix[22];

	sioOutPrintf( sos, "\n" );
	sioOutPrintf( sos, "%%%%%%%%\n" );
	sioOutPrintf( sos, "%%%%%%%%    %s\n", afi->afiFullName );
	sioOutPrintf( sos, "%%%%%%%% -> %s\n", afi->afiFontName );
	sioOutPrintf( sos, "%%%%%%%%\n\n" );

	if  ( psf->psfAppearsInText || allFonts )
	    {
	    const FaceReference *	fr;
	    int				ref;
	    const SupportedCharset *	sc;

	    fc= PS_Encodings;
	    sc= afi->afiSupportedCharsets;
	    for ( enc= 0; enc < ENCODINGps_COUNT; sc++, fc++, enc++ )
		{
		if  ( ! psf->psfEncodingUsed[enc] )
		    { continue;	}

		exceptionSuffix[0]= '\0';

		if  ( sc->scNonStandardGlyphNames )
		    {
		    int				i;
		    const char * const *	glyphNames= fc->fcGlyphNames;

		    sprintf( exceptionSuffix, "-x%d", faceNumber );

		    sioOutPrintf( sos, "/%s%s 256 array def\n",
						fc->fcPsEncodingArrayName,
						exceptionSuffix );
		    sioOutPrintf( sos, "%s %s%s copy\n",
						fc->fcPsEncodingArrayName,
						fc->fcPsEncodingArrayName,
						exceptionSuffix );

		    for ( i= 0; i < fc->fcGlyphCount; i++ )
			{
			int			glyphno;
			const AfmCharMetric *	acm;

			if  ( i >= 256 )
			    { LDEB(i); break;	}
			if  ( ! glyphNames[i] )
			    { continue;	}

			glyphno= sc->scCodeToGlyphMapping[i];
			if  ( glyphno < 0 )
			    { continue;	}
			if  ( glyphno >= afi->afiMetricCount )
			    { LLDEB(glyphno,afi->afiMetricCount); continue; }

			acm= afi->afiMetrics+ glyphno;
			if  ( strcmp( acm->acmN, glyphNames[i] ) )
			    {
			    sioOutPrintf( sos, "  dup %d /%s put\n",
							i,  acm->acmN );
			    }
			}

		    sioOutPrintf( sos, "readonly\n" );
		    }

		utilPsDefineEncodedFont( sos, afi,
			    fc->fcPsEncodingSuffix,
			    fc->fcPsEncodingArrayName,
			    exceptionSuffix );
		}

	    fr= psf->psfReferences;
	    for ( ref= 0; ref < psf->psfReferenceCount; fr++, ref++ )
		{
		const char *	encodingSuffix= "";

		if  ( ! fr->frAppearsInText && ! allFonts )
		    { continue;	}

		exceptionSuffix[0]= '\0';

		if  ( fr->frEncoding >= 0		&&
		      fr->frEncoding < ENCODINGps_COUNT	)
		    {
		    fc= PS_Encodings+ fr->frEncoding;
		    sc= afi->afiSupportedCharsets+ fr->frEncoding;

		    encodingSuffix= fc->fcPsEncodingSuffix;
		    if  ( sc->scNonStandardGlyphNames )
			{ sprintf( exceptionSuffix, "-x%d", faceNumber ); }
		    }

		sioOutPrintf( sos, "/%s\t{ /%s%s%s exch selectfont } def\n",
				fr->frFaceId,
				afi->afiFontName,
				encodingSuffix,
				exceptionSuffix );
		}
	    }

	psf= (PostScriptFace *)utilTreeGetNext( pstl->pstlFaceTree,
								&fontName );
	faceNumber++;
	}

    sioOutPrintf( sos, "\n" );

    return;
    }

/************************************************************************/
/*									*/
/*  Write the header for an EPS file.					*/
/*									*/
/************************************************************************/

void appPsWriteEpsHeader(	SimpleOutputStream *	sos,
				const char *		creator,
				const char *		title,
				int			pointsWide,
				int			pointsHigh )
    {
    time_t			now;

    now= time( (time_t *)0 );

    sioOutPrintf( sos, "%%!PS-Adobe-3.0 EPSF-3.0\n" );

    if  ( creator )
	{ utilPsFoldComment( sos, "Creator:", creator );	}

    if  ( title )
	{ utilPsFoldComment( sos, "Title:", title );	}

    sioOutPrintf( sos, "%%%%CreationDate: %s", ctime(&now) );
    sioOutPrintf( sos, "%%%%BoundingBox: 0 0 %d %d\n", pointsWide, pointsHigh );
    sioOutPrintf( sos, "%%%%EndComments\n");

    return;
    }

/************************************************************************/
/*									*/
/*  Define a procedure from an array of lines of PostScript code.	*/
/*  Empty lines and lines completely consisting of a comment are	*/
/*  skipped.								*/
/*									*/
/************************************************************************/

void utilPsDefineProcedure(	SimpleOutputStream *	sos,
				const char **		lines,
				int			count )
    {
    int		i;

    for ( i= 0; i < count; lines++, i++ )
	{
	const char *	s= *lines;

	while( isspace( *s ) )
	    { s++;	}

	if  ( ! *s || *s == '%' )
	    { continue;	}

	sioOutPrintf( sos, "%s\n", *lines );
	}

    sioOutPrintf( sos, "\n" );
    }

/************************************************************************/
/*									*/
/*  Write the header for an EPS file.					*/
/*									*/
/************************************************************************/

static const char * appPsPdfmarkEmulation[]=
    {
    "%%pdfmark emulation",
    "/pdfmark where",
    "    { pop }",
    "    { userdict /pdfmark /cleartomark load put }",
    "ifelse",
    };

void appPsSetPdfmarkEmulation(	SimpleOutputStream *	sos )
    {
    utilPsDefineProcedure( sos, appPsPdfmarkEmulation,
		    sizeof(appPsPdfmarkEmulation)/sizeof(const char *) );

    return;
    }

static const char * appPsRectfillEmulation[]=
    {
    "%%rectfill emulation for one rectangle only",
    "/rectfill where",
    "    { pop }",
    "    { /rectfill",
    "        { 4 2 roll moveto 2 copy",
    "            0 exch rlineto 0 rlineto ",
    "            neg 0 exch rlineto pop closepath fill",
    "        } bind def",
    "    }",
    "ifelse",
    };

void appPsSetRectfillEmulation(	SimpleOutputStream *	sos )
    {
    utilPsDefineProcedure( sos, appPsRectfillEmulation,
		    sizeof(appPsRectfillEmulation)/sizeof(const char *) );

    return;
    }

static const char * appPsSelectfontEmulation[]=
    {
    "%%selectfont emulation",
    "/selectfont where",
    "    { pop }",
    "    { /selectfont",
    "        { exch findfont exch dup type /arraytype eq",
    "          { makefont } { scalefont } ifelse",
    "          setfont",
    "        } bind def",
    "    }",
    "ifelse",
    };

void appPsSetSelectfontEmulation(	SimpleOutputStream *	sos )
    {
    utilPsDefineProcedure( sos, appPsSelectfontEmulation,
		    sizeof(appPsSelectfontEmulation)/sizeof(const char *) );

    return;
    }

/************************************************************************/
/*									*/
/*  Define procedures to Begin/End the inclusion of an EPS graphic.	*/
/*									*/
/*  *)  Copied directly from the EPS spec.				*/
/*  1)  I spent some time wondering why the 'begin' operator was not	*/
/*	matched by an 'end' operator. The matching end is in the loop	*/
/*	as 'op_count' is remembered before the copy of 'userdict' is	*/
/*	pushed.								*/
/*									*/
/************************************************************************/

static const char * appBeginEPSF[]=
    {
    "/BeginEPSF",
    "    {",
    "    /b4_Inc_state save def          % Save state for cleanup",
    "    /dict_count countdictstack def  % Count objects on dict stack",
    "    /op_count count 1 sub def       % Count objects on operand stack",
	 /*  1  */
    "    userdict begin                  % Push userdict on dict stack",
    "    /showpage { } def               % Redefine showpage, { } = null proc",
    "    0 setgray 0 setlinecap          % Prepare graphics state",
    "    1 setlinewidth 0 setlinejoin",
    "    10 setmiterlimit [ ] 0 setdash newpath",
    "    /languagelevel where            % If level not equal to 1 then",
    "    { pop languagelevel             % set strokeadjust and",
    "        1 ne                        % overprint to their defaults.",
    "            { false setstrokeadjust false setoverprint",
    "            } if",
    "        } if",
    "    } bind def",
    };

static const char * appEndEPSF[]=
    {
    "/EndEPSF",
    "    {",
    "    count op_count sub {pop} repeat % Clean up stacks",
	 /*  1  */
    "    countdictstack dict_count sub {end} repeat",
    "    b4_Inc_state restore",
    "    } bind def",
    };

void appPsDefineEpsProcs(	SimpleOutputStream *		sos )
    {
    utilPsDefineProcedure( sos, appBeginEPSF,
				sizeof(appBeginEPSF)/sizeof(const char *) );

    utilPsDefineProcedure( sos, appEndEPSF,
				sizeof(appEndEPSF)/sizeof(const char *) );

    return;
    }

void appPsBeginEpsObject(	PrintingState *		ps,
				int			x0Twips,
				int			y0Twips,
				double			xScale,
				double			yScale,
				int			llxTwips,
				int			llyTwips,
				int			urxTwips,
				int			uryTwips,
				const unsigned char *	file )
    {
    if  ( ! file )
	{ file= (const unsigned char *)"??";	}

    sioOutPrintf( ps->psSos, "BeginEPSF\n" );

    sioOutPrintf( ps->psSos, "[ %g %d %d %g %d %d ] concat\n",
			20* xScale, 0, 0, -20* yScale, x0Twips, y0Twips );

    sioOutPrintf( ps->psSos, "newpath %d %d moveto ",
					    llxTwips/20, llyTwips/20 );
    sioOutPrintf( ps->psSos, "%d %d lineto ",
					    urxTwips/20, llyTwips/20 );
    sioOutPrintf( ps->psSos, "%d %d lineto ",
					    urxTwips/20, uryTwips/20 );
    sioOutPrintf( ps->psSos, "%d %d lineto ",
					    llxTwips/20, uryTwips/20 );
    sioOutPrintf( ps->psSos, "closepath clip\n" );

    sioOutPrintf( ps->psSos, "%%%%BeginDocument: (" );
    utilPsPrintString( ps, file, strlen( (char *)file ) );
    sioOutPrintf( ps->psSos, ")\n" );

    return;
    }


void appPsEndEpsObject(	PrintingState *		ps )
    {
    sioOutPrintf( ps->psSos, "%%%%EndDocument\n" );
    sioOutPrintf( ps->psSos, "EndEPSF\n" );

    return;
    }

/************************************************************************/
/*									*/
/*  Fill a rectangle							*/
/*									*/
/************************************************************************/

void utilPsFillRectangle(	PrintingState *		ps,
				int			x,
				int			y,
				int			wide,
				int			high )
    {
    sioOutPrintf( ps->psSos, "%d %d %d %d rectfill\n", x, y, wide, high );

    ps->psLastPageMarked= ps->psPagesPrinted;

    return;
    }

/************************************************************************/
/*									*/
/*  Insert the destination of a pdfmark jump in the Printout.		*/
/*									*/
/************************************************************************/

int utilPsDestPdfmark(		PrintingState *		ps,
				int			lineTop,
				const char *		refName,
				int			refSize )
    {
    AffineTransform2D *	at= &(ps->psCurrentTransform);
    int			top;
    int			x= 0;
    int			y= lineTop;

    const char *	s;
    int			i;

    top= AT2_Y( x, y, at );

    sioOutPrintf( ps->psSos, "[ /Dest /" );

    s= refName;
    for ( i= 0; i < refSize; s++, i++ )
	{
	if  ( *s == '('	|| *s == ')'		||
	      *s == '<'	|| *s == '>'		||
	      *s == '['	|| *s == ']'		||
	      *s == '{'	|| *s == '}'		||
	      *s == '/'	|| *s == '%'		||
	      isspace( *s ) || ! isascii( *s )	)
	    {
	    sioOutPutCharacter( '_', ps->psSos );
	    continue;
	    }

	sioOutPutCharacter( *s, ps->psSos );
	}

    sioOutPrintf( ps->psSos, "\n" );

    sioOutPrintf( ps->psSos, "  /View [ /XYZ null %d null ]\n", top );

    sioOutPrintf( ps->psSos, "/DEST pdfmark\n" );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write links to be picked up by the distiller.			*/
/*									*/
/************************************************************************/

static void utilPsUriLinkDestination(	PrintingState *	ps )
    {
    utilPsPrintString( ps,
	(const unsigned char *)ps->psLinkFile, ps->psLinkFileSize );

    if  ( ps->psLinkMarkSize > 0 )
	{
	sioOutPutCharacter( '#', ps->psSos );
	utilPsPrintString( ps,
			    (const unsigned char *)ps->psLinkMark,
			    ps->psLinkMarkSize );
	}
    }

static void utilPsWebLinkDestination(	PrintingState *	ps )
    {
    sioOutPrintf( ps->psSos, "  /Action << /Subtype /URI /URI (" );

    utilPsUriLinkDestination( ps );

    sioOutPrintf( ps->psSos, ") >>\n" );

    return;
    }

static void utilPsFileLinkDestMark(	PrintingState *		ps )
    {
    const unsigned char *	file;
    int				size;

    file= (const unsigned char *)ps->psLinkFile;
    size= ps->psLinkFileSize;

    if  ( size > 5 && ! strncmp( (const char *)file, "file:", 5 ) )
	{ file += 5; size -= 5; }
    else{
	while( size > 0 && isalpha( *file ) )
	    { file++; size--;	}

	if  ( size > 0 && *file == ':' )
	    { utilPsWebLinkDestination( ps ); return; }

	file= (const unsigned char *)ps->psLinkFile;
	size= ps->psLinkFileSize;
	}

    sioOutPrintf( ps->psSos, "  /Action /Launch /File (" );

    utilPsPrintString( ps, file, size );

    sioOutPrintf( ps->psSos, ")\n" );

    if  ( ps->psLinkMarkSize )
	{
	sioOutPrintf( ps->psSos, "  /URI (" );
	utilPsUriLinkDestination( ps );
	sioOutPrintf( ps->psSos, ")\n" );
	}

    return;
    }

void utilPsFlushLink(		PrintingState *		ps,
				int			x0,
				int			wide,
				int			lineTop,
				int			lineHeight )
    {
    if  ( ps->psLinkParticulesDone > 0 )
	{
	sioOutPrintf( ps->psSos, "[ /Rect [ %d %d %d %d ]\n",
				    ps->psLinkRectLeft, lineTop+ lineHeight, 
				    x0+ wide,
				    lineTop );

	sioOutPrintf( ps->psSos, "  /Border [ 0 0 0 ]\n" );

	if  ( ps->psLinkFileSize == 0 )
	    {
	    if  ( ! ps->psLinkMark )
		{ XDEB(ps->psLinkMark);	}
	    else{
		int		i;
		const char *	s;

		sioOutPrintf( ps->psSos, "  /Dest /" );

		s= ps->psLinkMark;
		for ( i= 0; i < ps->psLinkMarkSize; s++, i++ )
		    {
		    if  ( *s == '('	|| *s == ')'		||
			  *s == '<'	|| *s == '>'		||
			  *s == '['	|| *s == ']'		||
			  *s == '{'	|| *s == '}'		||
			  *s == '/'	|| *s == '%'		||
			  isspace( *s ) || ! isascii( *s )	)
			{
			sioOutPutCharacter( '_', ps->psSos );
			continue;
			}

		    sioOutPutCharacter( *s, ps->psSos );
		    }

		sioOutPrintf( ps->psSos, "\n" );
		}
	    }
	else{
	    utilPsFileLinkDestMark( ps );
	    }


	sioOutPrintf( ps->psSos, "  /Subtype /Link\n" );
	sioOutPrintf( ps->psSos, "/ANN pdfmark\n" );

	ps->psLinkParticulesDone= 0;
	ps->psLinkRectLeft= x0;
	}

    return;
    }

