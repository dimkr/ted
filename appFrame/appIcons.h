/************************************************************************/
/*									*/
/*  A simple mechanism to find XImages and Pixmaps by name.		*/
/*									*/
/************************************************************************/

#   ifndef	APP_ICONS_H
#   define	APP_ICONS_H

#   include	<appGuiBase.h>

typedef struct NamedPicture
    {
    char *		npPictureName;
    char **		npBuffer;
    APP_BITMAP_IMAGE	npPixmap;
    APP_BITMAP_MASK	npMask;
    } NamedPicture;

#   endif
