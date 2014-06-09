/****************************************************************/
/*								*/
/*  Keep a mapping from attribute number to screen font.	*/
/*								*/
/****************************************************************/

#   include	<stdlib.h>
#   include	<string.h>

#   include	"docScreenFontList.h"

#   include	<appDebugon.h>

void docInitScreenFontList(	ScreenFontList *	sfl )
    {
    sfl->sflAttributeToScreen= (int *)0;
    sfl->sflAttributeToScreenCount= 0;
    return;
    }

void docCleanScreenFontList(	ScreenFontList *	sfl )
    {
    if  ( sfl->sflAttributeToScreen )
	{ free( sfl->sflAttributeToScreen );	}

    return;
    }
