/************************************************************************/
/*									*/
/*  Simple io streams using X11 windows properties.			*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>

#   include	<sioXprop.h>

#   include	<appDebugon.h>

#   ifdef USE_MOTIF

#   include	<X11/Xlib.h>
#   include	<X11/Xatom.h>

static Atom	XA_XV_CLIPBOARD= None;

/************************************************************************/
/*									*/
/*  Input.. Paste.							*/
/*									*/
/************************************************************************/

typedef struct XpropPasteStream
    {
    Display *		xpsDisplay;
    APP_WINDOW		xpsWindow;
    APP_ATOM		xpsProperty;
    long		xpsOffset;
    long		xpsExhausted;

    int			xpsDeleteOnClose;
    } XpropPasteStream;

static int sioXpropPasteClose(	void *	voidxps )
    {
    XpropPasteStream *	xps= (XpropPasteStream *)voidxps;

    if  ( xps->xpsDeleteOnClose )
	{
	XDeleteProperty( xps->xpsDisplay, xps->xpsWindow, xps->xpsProperty );
	}

    free( voidxps );

    return 0;
    }

static int sioInXpropReadBytes(	void *		voidxps,
				unsigned char *	buffer,
				int		count )
    {
    XpropPasteStream *	xps= (XpropPasteStream *)voidxps;

    Atom		typeFound;
    int			formatFound;

    unsigned long	itemsReturned;
    unsigned long	itemsLeft;
    unsigned char *	dataReturned;

    int			ret;

    if  ( xps->xpsExhausted )
	{ return -1;	}

    ret= XGetWindowProperty( xps->xpsDisplay, xps->xpsWindow, xps->xpsProperty,
		    xps->xpsOffset/4, count/4, False, AnyPropertyType,
		    &typeFound, &formatFound,
		    &itemsReturned, &itemsLeft, &dataReturned );

    if  ( ret != Success )
	{ LLDEB(ret,Success); return -1;	}
    if  ( itemsReturned == 0 )
	{ LDEB(itemsReturned); return -1;	}
    if  ( formatFound != 8 )
	{ LLDEB(typeFound,formatFound); return -1;	}

    memcpy( buffer, dataReturned, itemsReturned );
    XFree( dataReturned );

    xps->xpsOffset += itemsReturned;
    if  ( itemsLeft == 0 )
	{ xps->xpsExhausted= 1;	}

    return itemsReturned;
    }

SimpleInputStream * sioInOpenPaste(	APP_WIDGET	w,
					APP_EVENT *	event )
    {
    XSelectionEvent *		selEvent= &(event->xselection);

    SimpleInputStream *		sis;
    XpropPasteStream *		xps;

    xps= (XpropPasteStream *)malloc( sizeof( XpropPasteStream ) );
    if  ( ! xps )
	{ XDEB(xps); return (SimpleInputStream *)0;	}

    xps->xpsDisplay= XtDisplay( w );
    xps->xpsWindow= XtWindow( w );
    xps->xpsProperty= selEvent->property;
    xps->xpsOffset= 0L;
    xps->xpsExhausted= 0;
    xps->xpsDeleteOnClose= 1;

    sis= sioInOpen( (void *)xps, sioInXpropReadBytes, sioXpropPasteClose );

    if  ( ! sis )
	{ XDEB(sis); free( xps ); return (SimpleInputStream *)0; }

    return sis;
    }

SimpleInputStream * sioInOpenXvPaste(	APP_WIDGET	w )
    {
    Display *			display= XtDisplay( w );
    int				screen= DefaultScreen( display );

    SimpleInputStream *		sis;
    XpropPasteStream *		xps;

    if  ( XA_XV_CLIPBOARD == None )
	{
	XA_XV_CLIPBOARD= XInternAtom( display, "XV_CLIPBOARD", False );
	if  ( XA_XV_CLIPBOARD == None )
	    { XDEB(XA_XV_CLIPBOARD); return (SimpleInputStream *)0;	}
	}

    xps= (XpropPasteStream *)malloc( sizeof( XpropPasteStream ) );
    if  ( ! xps )
	{ XDEB(xps); return (SimpleInputStream *)0;	}

    xps->xpsDisplay= display;
    xps->xpsWindow= XRootWindow( display, screen );
    xps->xpsProperty= XA_XV_CLIPBOARD;
    xps->xpsOffset= 0L;
    xps->xpsExhausted= 0;
    xps->xpsDeleteOnClose= 0;

    sis= sioInOpen( (void *)xps, sioInXpropReadBytes, sioXpropPasteClose );

    if  ( ! sis )
	{ XDEB(sis); free( xps ); return (SimpleInputStream *)0; }

    return sis;
    }

/************************************************************************/
/*									*/
/*  Output.. Copy.							*/
/*									*/
/************************************************************************/

typedef struct XpropCopyStream
    {
    Display *		xcsDisplay;
    APP_WINDOW		xcsRequestor;
    Atom		xcsProperty;
    Atom		xcsTarget;
    int			xcsPropMode;
    } XpropCopyStream;

static int sioXpropCopyClose(	void *	voidxcs )
    {
    free( voidxcs );

    return 0;
    }

static int sioOutCopyWriteBytes(	void *			voidxcs,
					const unsigned char *	buffer,
					int			count )
    {
    XpropCopyStream *	xcs= (XpropCopyStream *)voidxcs;

    XChangeProperty( xcs->xcsDisplay, xcs->xcsRequestor,
				xcs->xcsProperty, xcs->xcsTarget,
				8, xcs->xcsPropMode, buffer, count );

    xcs->xcsPropMode= PropModeAppend;

    return count;
    }

SimpleOutputStream * sioOutOpenCopy(	APP_WIDGET		w,
					APP_EVENT *		event )
    {
    XSelectionEvent *		selEvent= &(event->xselection);
    SimpleOutputStream *	sos;
    XpropCopyStream *		xcs;

    xcs= (XpropCopyStream *)malloc( sizeof( XpropCopyStream ) );
    if  ( ! xcs )
	{ XDEB(xcs); return (SimpleOutputStream *)0;	}

    xcs->xcsDisplay= XtDisplay( w );
    xcs->xcsRequestor= selEvent->requestor;
    xcs->xcsProperty= selEvent->property;
    xcs->xcsTarget= selEvent->target;
    xcs->xcsPropMode= PropModeReplace;

    sos= sioOutOpen( (void *)xcs, sioOutCopyWriteBytes,
					    (SIOoutSEEK)0, sioXpropCopyClose );

    if  ( ! sos )
	{ XDEB(sos); free( xcs ); return (SimpleOutputStream *)0; }

    return sos;
    }

SimpleOutputStream * sioOutOpenXvCopy(	APP_WIDGET		w )
    {
    Display *			display= XtDisplay( w );
    int				screen= DefaultScreen( display );

    SimpleOutputStream *	sos;
    XpropCopyStream *		xcs;

    if  ( ! XA_XV_CLIPBOARD )
	{
	XA_XV_CLIPBOARD= XInternAtom( display, "XV_CLIPBOARD", False );
	if  ( ! XA_XV_CLIPBOARD  )
	    { XDEB(XA_XV_CLIPBOARD); return (SimpleOutputStream *)0;	}
	}

    xcs= (XpropCopyStream *)malloc( sizeof( XpropCopyStream ) );
    if  ( ! xcs )
	{ XDEB(xcs); return (SimpleOutputStream *)0;	}

    xcs->xcsDisplay= XtDisplay( w );
    xcs->xcsRequestor= XRootWindow( display, screen );
    xcs->xcsProperty= XA_XV_CLIPBOARD;
    xcs->xcsTarget= XA_STRING;
    xcs->xcsPropMode= PropModeReplace;

    sos= sioOutOpen( (void *)xcs, sioOutCopyWriteBytes,
					    (SIOoutSEEK)0, sioXpropCopyClose );

    if  ( ! sos )
	{ XDEB(sos); free( xcs ); return (SimpleOutputStream *)0; }

    return sos;
    }

#   endif
