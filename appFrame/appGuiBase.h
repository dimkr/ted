/************************************************************************/
/*									*/
/*  Translation of GUI Drawing primitives to the actual implementing	*/
/*  GUI system. As we do not have the pretentence to introduce our own	*/
/*  GUI layer, as much as possible is done through macros. (#defines)	*/
/*									*/
/************************************************************************/

#   ifndef	APP_GUI_BASE_H
#   define	APP_GUI_BASE_H

#   ifdef __VMS
#	if defined(USE_MOTIF) || defined(USE_GTK)
#	    include	<vms_x_fix.h>
#	    undef XtDisplay
#	    undef XtScreen
#	    undef XtWindow
#	    undef XtIsRealized
#	    undef XtParent
#	endif
#   endif

	    /* yes BOTH! for dependencies in makefile */
#   include "appGuiBaseMotif.h"
#   include "appGuiBaseGtk.h"


#   endif	/* APP_GUI_BASE_H */
