/************************************************************************/
/*									*/
/*  Manipulate X11 font related strings.				*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_X11_FONT_NAMES_H
#   define	UTIL_X11_FONT_NAMES_H

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern int utilX11SplitFontsDirLine(	char *		scratch,
					int *		pX11Start,
					int *		pX11Length,
					int		line );

#   endif
