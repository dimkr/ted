#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"appFrame.h"

#   ifdef USE_MOTIF

#   include	<X11/Xatom.h>
#   include	<X11/keysym.h>

#   include	<charnames.h>
#   include	<appDebugon.h>

		/* old typo */
#   ifndef	XK_Greek_IOTAdieresis
#	define	XK_Greek_IOTAdieresis	XK_Greek_IOTAdiaeresis
#   endif

static int appGuiKeysymCludge(		unsigned char *		buf,
					APP_KEY_VALUE		key )
    {
#   ifdef XK_EuroSign
    if  ( key == XK_EuroSign )
	{ buf[0]= ISO15_Euro; buf[1]= '\0'; return 1; }
#   endif

    /*  Latin 2  */
    if  ( ( key & 0xffffff00 ) == 0x100 )
	{ buf[0]= key & 0xff; buf[1]= '\0'; return 1; }

    /*  Greek  */
    if  ( ( key & 0xffffff00 ) == 0x700 )
	{
	if  ( key >= 0xc1 )
	    { buf[0]= key & 0xff; buf[1]= '\0'; return 1; }

	switch( key )
	    {
	    case XK_Greek_ALPHAaccent:
		 buf[0]= ISO7_Alphatonos; buf[1]= '\0'; return 1;
	    case XK_Greek_EPSILONaccent:
		 buf[0]= ISO7_Epsilontonos; buf[1]= '\0'; return 1;
	    case XK_Greek_ETAaccent:
		 buf[0]= ISO7_Etatonos; buf[1]= '\0'; return 1;
	    case XK_Greek_IOTAaccent:
		 buf[0]= ISO7_Iotatonos; buf[1]= '\0'; return 1;
	    case XK_Greek_IOTAdieresis:
		 buf[0]= ISO7_Iotadieresis; buf[1]= '\0'; return 1;
	    case XK_Greek_OMICRONaccent:
		 buf[0]= ISO7_Omicrontonos; buf[1]= '\0'; return 1;
	    case XK_Greek_UPSILONaccent:
		 buf[0]= ISO7_Upsilontonos; buf[1]= '\0'; return 1;
	    case XK_Greek_UPSILONdieresis:
		 buf[0]= ISO7_Upsilondieresis; buf[1]= '\0'; return 1;
	    case XK_Greek_OMEGAaccent:
		 buf[0]= ISO7_Omegatonos; buf[1]= '\0'; return 1;

	    case XK_Greek_accentdieresis:
		 buf[0]= ISO7_dieresistonos; buf[1]= '\0'; return 1;

	    case XK_Greek_alphaaccent:
		 buf[0]= ISO7_alphatonos; buf[1]= '\0'; return 1;
	    case XK_Greek_epsilonaccent:
		 buf[0]= ISO7_epsilontonos; buf[1]= '\0'; return 1;
	    case XK_Greek_etaaccent:
		 buf[0]= ISO7_etatonos; buf[1]= '\0'; return 1;
	    case XK_Greek_iotaaccent:
		 buf[0]= ISO7_iotatonos; buf[1]= '\0'; return 1;
	    case XK_Greek_iotadieresis:
		 buf[0]= ISO7_iotadieresis; buf[1]= '\0'; return 1;
	    case XK_Greek_iotaaccentdieresis:
		 buf[0]= ISO7_iotadieresistonos; buf[1]= '\0'; return 1;
	    case XK_Greek_omicronaccent:
		 buf[0]= ISO7_omicrontonos; buf[1]= '\0'; return 1;
	    case XK_Greek_upsilonaccent:
		 buf[0]= ISO7_upsilontonos; buf[1]= '\0'; return 1;
	    case XK_Greek_upsilondieresis:
		 buf[0]= ISO7_upsilondieresis; buf[1]= '\0'; return 1;
	    case XK_Greek_upsilonaccentdieresis:
		 buf[0]= ISO7_upsilondieresistonos; buf[1]= '\0'; return 1;
	    case XK_Greek_omegaaccent:
		 buf[0]= ISO7_omegatonos; buf[1]= '\0'; return 1;
	    default:
		break;
	    }
	}

    return 0;
    }

void appGuiGetStringFromKeyboardEvent(	APP_INPUT_CONTEXT	ic,
					APP_WIDGET		w,
					APP_EVENT *		event,
					int *			pGotString,
					int *			pGotKey,
					unsigned int *		pState,
					unsigned char *		buf,
					int			capacity,
					APP_KEY_VALUE *		pKey )
    {
    int			gotString;
    XKeyPressedEvent *	keyEvent= &(event->xkey);

    if  ( event->type != KeyPress )
	{ LLDEB(event->type,KeyPress); *pGotString= 0; return;	}

    if  ( ic )
	{
	Status	status;

	gotString= XmbLookupString( ic, keyEvent, (char *)buf, capacity- 1,
							    pKey, &status );
	switch( status )
	    {
	    case XBufferOverflow:
		LDEB(status);
		*pGotString= 0;
		*pGotKey= 0;
		return;

	    case XLookupNone:
		*pGotString= 0;
		*pGotKey= 0;
		return;

	    case XLookupBoth:
		buf[gotString]= '\0';
		*pGotString= gotString;
		*pGotKey= 1;
		*pState= keyEvent->state;
		return;

	    case XLookupChars:
		buf[gotString]= '\0';
		*pGotString= gotString;
		*pGotKey= 0;
		*pState= keyEvent->state;
		return;

	    case XLookupKeySym:
		*pGotString= 0;
		*pGotKey= 1;
		*pState= keyEvent->state;

		if  ( appGuiKeysymCludge( buf, *pKey ) )
		    { *pGotString= 1; }

		return;
	    }
	}
    else{
	gotString= XLookupString( keyEvent, (char *)buf, capacity- 1,
					    pKey, (XComposeStatus *)0 );

	if  ( gotString > 0 )
	    { buf[gotString]= '\0';	}
	*pGotString= gotString;
	*pGotKey= 1;
	*pState= keyEvent->state;

	if  ( ! gotString && appGuiKeysymCludge( buf, *pKey ) )
	    { *pGotString= 1; }

	return;
	}

    }

#   endif
