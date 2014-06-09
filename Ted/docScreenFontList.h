/****************************************************************/
/*								*/
/*  Keep a mapping from attribute number to screen font.	*/
/*								*/
/****************************************************************/

#   ifndef	DOC_SCREEN_FONT_LIST_H
#   define	DOC_SCREEN_FONT_LIST_H

typedef struct ScreenFontList
    {
    int *	sflAttributeToScreen;
    int		sflAttributeToScreenCount;
    } ScreenFontList;

/****************************************************************/
/*								*/
/*  Declarations.						*/
/*								*/
/****************************************************************/

extern void docInitScreenFontList(	ScreenFontList *	sfl );
extern void docCleanScreenFontList(	ScreenFontList *	sfl );

#   endif
