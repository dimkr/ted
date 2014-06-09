/************************************************************************/
/*									*/
/*  Read a file with a t1 map and set the entries on PostScript fonts.	*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_T1_ENTRIES_H
#   define	UTIL_T1_ENTRIES_H

#   include	<psFont.h>

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern int utilSetT1EntriesForFonts(
				const char *			t1MapFile,
				int				mapNames,
				const PostScriptFontList *	psfl );

#   endif
