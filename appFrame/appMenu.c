/************************************************************************/
/*  Shortcuts for the most commun menu building actions.		*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	"appFrame.h"
#   include	<appDebugon.h>

APP_WIDGET appMakeMenu(	APP_WIDGET *		pButton,
			AppToplevel *		at,
			EditApplication *	ea,
			APP_WIDGET		menuBar,
			const char *		itemText,
			int			isHelp,
			AppMenuItem *		ami,
			int			itemCount,
			void *			target )

    {
    AppConfigurableResource *	acr;
    AppConfigurableResource *	acri;
    APP_WIDGET			menu;
    APP_WIDGET			button;

    int				ignored= 0;

    int				i;

    acr= (AppConfigurableResource *)malloc(
			    3* itemCount* sizeof(AppConfigurableResource) );
    if  ( ! acr )
	{ XDEB(acr); return (APP_WIDGET)0;	}

    acri= acr;
    for ( i= 0; i < itemCount; i++ )
	{
	if  ( ami[i].amiItemType == ITEMtySEPARATOR )
	    { continue;	}

	APP_SET_RESOURCE( acri, ami[i].amiTextResName,
				    i* sizeof(AppMenuItem)+
					    offsetof(AppMenuItem,amiItemText),
				    ami[i].amiItemText );

	acri++;

	APP_SET_RESOURCE( acri, ami[i].amiKeyResName,
				    i* sizeof(AppMenuItem)+
					    offsetof(AppMenuItem,amiKey),
				    ami[i].amiKey );

	acri++;
			    
	APP_SET_RESOURCE( acri, ami[i].amiKeyTextResName,
				    i* sizeof(AppMenuItem)+
					    offsetof(AppMenuItem,amiKeyText),
				    ami[i].amiKeyText );

	acri++;
	}

    appGuiGetResourceValues( &ignored, ea, ami, acr, acri- acr );

    free( acr );

    menu= appMakeMenuInParent( &button, at, menuBar, itemText, isHelp );

    for ( i= 0; i < itemCount; i++, ami++ )
	{
	switch( ami->amiItemType )
	    {
	    case ITEMtyOPTION:
		ami->amiOptionWidget=
				appSetMenuItem( menu, at, ami, target );
		break;

	    case ITEMtyTOGGLE_OFF:
	    case ITEMtyTOGGLE_ON:
		ami->amiOptionWidget=
				appSetToggleMenuItem( menu, at, ami, target );
		break;

	    case ITEMtySEPARATOR:
		ami->amiOptionWidget=
				appSetMenuSeparator( menu, at, ami, target );
		break;
	    default:
		LDEB(ami->amiItemType);
		break;
	    }
	}

    *pButton= button; return menu;
    }
