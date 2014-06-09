/************************************************************************/
/*									*/
/*  Simple io streams using X11 window properties.			*/
/*									*/
/************************************************************************/

#   include	<appGuiBase.h>

#   include	<sioGeneral.h>

extern SimpleInputStream * sioInOpenPaste(	APP_WIDGET		w,
						APP_SELECTION_EVENT *	event );

extern SimpleInputStream * sioInOpenXvPaste(	APP_WIDGET	w );

extern SimpleOutputStream * sioOutOpenCopy(	APP_WIDGET		w,
						APP_SELECTION_EVENT *	event );

extern SimpleOutputStream * sioOutOpenXvCopy(	APP_WIDGET	w );

