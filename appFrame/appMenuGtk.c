/************************************************************************/
/*									*/
/*  Menu building GTK specific code.					*/
/*									*/
/*  Thanks to Andrea Frome who contributed the original code.		*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	"appFrame.h"
#   include	<appDebugon.h>

#   ifdef USE_GTK


#   include	<gdk/gdkkeysyms.h>

static int appMenuParseKey(	const char *		key,
				unsigned int *		pKeyMask,
				unsigned int *		pKeyCode )
    {
    const char *	s= key;
    unsigned int	keyMask= 0;
    unsigned int	keyCode= 0;
    int			got= 0;

    for (;;)
	{
	while( isspace( *s ) )
	    { s++;	}
	if  ( ! *s )
	    { break;	}

	if  ( ! strncmp( s, "Ctrl", 4 ) )
	    { keyMask |= GDK_CONTROL_MASK; s += 4; continue;	}
	if  ( ! strncmp( s, "Shift", 5 ) )
	    { keyMask |= GDK_SHIFT_MASK; s += 5; continue;	}
	if  ( ! strncmp( s, "Lock", 4 ) )
	    { keyMask |= GDK_LOCK_MASK; s += 4; continue;	}
	if  ( ! strncmp( s, "Mod1", 4 ) )
	    { keyMask |= GDK_MOD1_MASK; s += 4; continue;	}
	if  ( ! strncmp( s, "Mod2", 4 ) )
	    { keyMask |= GDK_MOD2_MASK; s += 4; continue;	}
	if  ( ! strncmp( s, "Mod3", 4 ) )
	    { keyMask |= GDK_MOD3_MASK; s += 4; continue;	}
	if  ( ! strncmp( s, "Mod4", 4 ) )
	    { keyMask |= GDK_MOD4_MASK; s += 4; continue;	}
	if  ( ! strncmp( s, "Mod5", 4 ) )
	    { keyMask |= GDK_MOD5_MASK; s += 4; continue;	}

	break;
	}

    if  ( strncmp( s, "<Key>", 5 ) )
	{ SDEB(key); return -1;	}
    s += 5;
    while( isspace( *s ) )
	{ s++;	}

    if  ( ! got && strlen( s ) == 1 )
	{ keyCode= s[0]; got= 1;	}

    if  ( ! got && ! strcmp( s, "F1" ) )
	{ keyCode= GDK_F1; got= 1;	}
    if  ( ! got && ! strcmp( s, "F2" ) )
	{ keyCode= GDK_F2; got= 1;	}
    if  ( ! got && ! strcmp( s, "F3" ) )
	{ keyCode= GDK_F3; got= 1;	}
    if  ( ! got && ! strcmp( s, "F4" ) )
	{ keyCode= GDK_F4; got= 1;	}
    if  ( ! got && ! strcmp( s, "F5" ) )
	{ keyCode= GDK_F5; got= 1;	}
    if  ( ! got && ! strcmp( s, "F6" ) )
	{ keyCode= GDK_F6; got= 1;	}
    if  ( ! got && ! strcmp( s, "F7" ) )
	{ keyCode= GDK_F7; got= 1;	}
    if  ( ! got && ! strcmp( s, "F8" ) )
	{ keyCode= GDK_F8; got= 1;	}
    if  ( ! got && ! strcmp( s, "F9" ) )
	{ keyCode= GDK_F9; got= 1;	}
    if  ( ! got && ! strcmp( s, "F10" ) )
	{ keyCode= GDK_F10; got= 1;	}
    if  ( ! got && ! strcmp( s, "F11" ) )
	{ keyCode= GDK_F11; got= 1;	}
    if  ( ! got && ! strcmp( s, "F12" ) )
	{ keyCode= GDK_F12; got= 1;	}

    if  ( ! got )
	{ SDEB(key); return -1;	}

    *pKeyMask= keyMask; *pKeyCode= keyCode; return 0;
    }

APP_WIDGET appSetMenuItem(	APP_WIDGET		menu,
				AppToplevel *		at,
				AppMenuItem *		ami,
				void *			target )
    {
    GtkWidget *	rval=	gtk_menu_item_new_with_label( ami->amiItemText );

    if  ( ami->amiKey )
	{
	unsigned int	keyMask= 0;
	unsigned int	keyCode;

	if  ( appMenuParseKey( ami->amiKey, &keyMask, &keyCode ) )
	    { SDEB(ami->amiKey);	}
	else{
	    gtk_widget_add_accelerator( rval, "activate", at->atAccelGroup,
					keyCode, keyMask, GTK_ACCEL_VISIBLE );
	    }
	}

    if  ( ami->amiCallback )
	{
	gtk_signal_connect( GTK_OBJECT( rval ),
			"activate", (GtkSignalFunc)ami->amiCallback, target );
	}

    gtk_menu_append( GTK_MENU( menu ), rval );
    gtk_widget_show( rval );

    return rval;
    }

APP_WIDGET appSetMenuSeparator(	APP_WIDGET		menu,
				AppToplevel *		at,
				AppMenuItem *		ami,
				void *			target )
    {
    APP_WIDGET		rval;

    rval= gtk_menu_item_new();

    gtk_menu_append( GTK_MENU( menu ), rval );
    gtk_widget_show( rval );

    return rval;
    }	    

APP_WIDGET appSetToggleMenuItem(	APP_WIDGET		menu,
					AppToplevel *		at,
					AppMenuItem *		ami,
					void *			target )
    {
    GtkWidget *	rval=	gtk_check_menu_item_new_with_label( ami->amiItemText );

    if  ( ami->amiKey )
	{
	unsigned int	keyMask= 0;
	unsigned int	keyCode;

	if  ( appMenuParseKey( ami->amiKey, &keyMask, &keyCode ) )
	    { SDEB(ami->amiKey);	}
	else{
	    gtk_widget_add_accelerator( rval, "activate", at->atAccelGroup,
					keyCode, keyMask, GTK_ACCEL_VISIBLE );
	    }
	}

    gtk_check_menu_item_set_state( GTK_CHECK_MENU_ITEM( rval ),
					ami->amiItemType == ITEMtyTOGGLE_ON );
    gtk_check_menu_item_set_show_toggle( GTK_CHECK_MENU_ITEM( rval ),
					TRUE );

    gtk_menu_append( GTK_MENU( menu ), rval );
    gtk_widget_show( rval );

    if  ( ami->amiCallback )
	{
	gtk_signal_connect( GTK_OBJECT( rval ),
			"toggled", (GtkSignalFunc)ami->amiCallback, target );
	}

    if  ( target )
	{ gtk_object_set_user_data( GTK_OBJECT( rval ), target );	}

    return rval;
    }

APP_WIDGET appMakeMenuInParent(	APP_WIDGET *		pButton,
				AppToplevel *		at,
				APP_WIDGET		menuBar,
				const char *		itemText,
				int			isHelp )
    {
    GtkWidget *			menu;
    GtkWidget *			button;

    menu= gtk_menu_new();

    button= gtk_menu_item_new_with_label( itemText );
    gtk_menu_bar_append( GTK_MENU_BAR( menuBar ), button );

    gtk_menu_item_set_submenu( GTK_MENU_ITEM( button ), menu );

    if  ( isHelp )
	{
#	if GTK_MAJOR_VERSION < 2
	gtk_menu_item_set_placement( GTK_MENU_ITEM( button ),
						    GTK_DIRECTION_RIGHT );
#	else
	gtk_menu_item_set_right_justified( GTK_MENU_ITEM( button ), TRUE );
#	endif
	}

    gtk_widget_show( button );

    *pButton= button;

    return menu;
    }

void appGuiSetToggleItemState(	APP_WIDGET		toggle,
				int			set )
    {
    gtk_check_menu_item_set_state( GTK_CHECK_MENU_ITEM( toggle ), set != 0 );
    }

void appGuiSetToggleItemLabel(	APP_WIDGET		toggle,
				const char *		label )
    {
    appGuiGtkSetChildLabel( toggle, label );

    return;
    }

int appGuiGetMenuToggleStateFromCallback( APP_WIDGET		toggle,
					void *			voidcbs )
    {
    GtkCheckMenuItem *	item= GTK_CHECK_MENU_ITEM( toggle );

    return item->active;
    }

/************************************************************************/
/*									*/
/*  1)  Retrieve the text from menu options.				*/
/*  2)  Free the result obtained in this way.				*/
/*									*/
/************************************************************************/

/*  1  */
char *	appGetTextFromMenuOption(	APP_WIDGET		w )
    {
    static char	slabel[]= "<< ? >>";
    char *	label= slabel;

    if  ( ! appGuiGtkGetChildLabel( &label, w ) )
	{ SDEB(label);	}

    return label;
    }

/*  2  */
void appFreeTextFromMenuOption(	char *		s )
    { return;	}

#   endif

