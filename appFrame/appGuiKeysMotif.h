/************************************************************************/
/*									*/
/*  Defines for the key names: Map to plain X11 names.			*/
/*									*/
/************************************************************************/

# ifdef USE_MOTIF

#   include	<X11/keysym.h>

#  ifdef XK_ISO_Left_Tab
#   define KEY_ISO_Left_Tab	XK_ISO_Left_Tab
#  endif
#   define KEY_i		XK_i
#   define KEY_Tab		XK_Tab
#   define KEY_j		XK_j
#   define KEY_m		XK_m
#   define KEY_KP_Enter		XK_KP_Enter
#   define KEY_Return		XK_Return
#   define KEY_KP_Delete	XK_KP_Delete

#   ifdef __VMS
	/* VMS normally has only a "Delete" configured which is on the same
	 * location on the keyboard and serves the same purposes as the
	 * BackSpace on Unix. To simulate a BackSpace on VMS normally
	 * <CTRL>H is used.
	 */
#	define KEY_Delete	XK_BackSpace
#	define KEY_BackSpace	XK_Delete
#   else
#	define KEY_Delete	XK_Delete
#	define KEY_BackSpace	XK_BackSpace
#   endif

#   define KEY_KP_Home		XK_KP_Home
#   define KEY_Home		XK_Home
#   define KEY_KP_End		XK_KP_End
#   define KEY_End		XK_End
#   define KEY_KP_Left		XK_KP_Left
#   define KEY_Left		XK_Left
#   define KEY_KP_Right		XK_KP_Right
#   define KEY_Right		XK_Right
#   define KEY_KP_Up		XK_KP_Up
#   define KEY_Up		XK_Up
#   define KEY_KP_Down		XK_KP_Down
#   define KEY_Down		XK_Down
#   define KEY_KP_Prior		XK_KP_Prior
#   define KEY_Prior		XK_Prior
#   define KEY_KP_Next		XK_KP_Next
#   define KEY_Next		XK_Next
#   define KEY_c		XK_c
#   define KEY_x		XK_x
#   define KEY_v		XK_v
#   define KEY_y		XK_y
#   define KEY_Shift_L		XK_Shift_L
#   define KEY_Shift_R		XK_Shift_R
#   define KEY_Alt_L		XK_Alt_L
#   define KEY_Alt_R		XK_Alt_R
#   define KEY_Control_L	XK_Control_L
#   define KEY_Control_R	XK_Control_R
#   define KEY_Caps_Lock	XK_Caps_Lock
#   define KEY_Insert		XK_Insert
#   define KEY_KP_Insert	XK_KP_Insert
#   define KEY_Num_Lock		XK_Num_Lock
# endif

