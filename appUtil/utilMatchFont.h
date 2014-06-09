/************************************************************************/
/*									*/
/*  Map between PostScript type and Document type font administrations.	*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_MATCH_FONT_H
#   define	UTIL_MATCH_FONT_H

#   include	<utilDocFont.h>
#   include	<psFont.h>

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern int utilAddPsFontsToDocList( DocumentFontList *		dfl,
				const PostScriptFontList *	psfl );

extern int utilFindPsFontsForDocFonts( DocumentFontList *	dfl,
				const PostScriptFontList *	psfl );

extern int utilFindPsFontForDocFont(
				DocumentFont *			df,
				DocumentFontList *		dfl,
				const PostScriptFontList *	psfl );

extern AfmFontInfo * utilPsGetAfi(	int *				pEnc,
					const DocumentFontList *	dfl,
					const PostScriptFontList *	psfl,
					const TextAttribute *		ta );

extern int utilFontWeightFromString(	int *		pWieght,
					int *		pStart,
					int *		pLength,
					const char *	name );

extern int utilFontWidthFromString(	int *		pWidth,
					int *		pStart,
					int *		pLength,
					const char *	name );

extern int utilFontlistSetPreferredEncodings(	DocumentFontList *	dfl );

#   endif
