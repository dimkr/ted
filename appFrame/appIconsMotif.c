/************************************************************************/
/*									*/
/*  Retrieve images.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	"appFrame.h"
#   include	"appIcons.h"

#   include	<appDebugon.h>

#   ifdef	USE_MOTIF

#   include	<X11/xpm.h>

static XpmAttributes	APP_XpmAttributes;

/************************************************************************/
/*									*/
/*  Get a pixmap. Keep trying with more and more tolerance for the	*/
/*  exactness of the colors.						*/
/*									*/
/************************************************************************/

int appGetImagePixmap(	EditApplication *	ea,
			const char *		name,
			APP_BITMAP_IMAGE *	pPixmap,
			APP_BITMAP_MASK *	pMask )
    {
    int			i;
    int			res;
    NamedPicture *	np;

    Display *		display= XtDisplay( ea->eaToplevel.atTopWidget );

    Pixmap		pixmap= (Pixmap)0;
    Pixmap		mask= (Pixmap)0;

    if  ( ! APP_XpmAttributes.valuemask )
	{
	APP_XpmAttributes.valuemask= XpmRGBCloseness;

	APP_XpmAttributes.red_closeness= 8;
	APP_XpmAttributes.green_closeness= 8;
	APP_XpmAttributes.blue_closeness= 16;
	}

    np= ea->eaNamedPictures;
    for ( i= 0; i < ea->eaNamedPictureCount; np++, i++ )
	{
	if  ( ! strcmp( name, np->npPictureName ) )
	    { break;	}
	}

    if  ( i >= ea->eaNamedPictureCount )
	{ SLDEB(name,ea->eaNamedPictureCount); return -1;	}

    if  ( np->npPixmap )
	{ *pPixmap= np->npPixmap; *pMask= mask; return 0;	}

    res= XpmCreatePixmapFromData( display,
		RootWindow( display, DefaultScreen( display ) ),
		np->npBuffer, &pixmap, &mask,
		&APP_XpmAttributes );

    while( res == XpmColorFailed && APP_XpmAttributes.red_closeness < 10000 )
	{
	APP_XpmAttributes.red_closeness *= 2;
	APP_XpmAttributes.green_closeness *= 2;
	APP_XpmAttributes.blue_closeness *= 2;

	res= XpmCreatePixmapFromData( display,
		RootWindow( display, DefaultScreen( display ) ),
		np->npBuffer, &pixmap, &mask,
		&APP_XpmAttributes );
	}

    if  ( res )
	{ SLDEB(name,res); return -1;	}

    np->npPixmap= pixmap;
    np->npMask= mask;

    *pPixmap= pixmap; *pMask= mask; return 0;
    }

void appDiscardImagePixmaps(	EditApplication *	ea )
    {
    int			i;
    NamedPicture *	np;

    Display *		display= XtDisplay( ea->eaToplevel.atTopWidget );

    np= ea->eaNamedPictures;
    for ( i= 0; i < ea->eaNamedPictureCount; np++, i++ )
	{
	if  ( np->npPixmap )
	    { XFreePixmap( display, np->npPixmap ); np->npPixmap= None; }
	if  ( np->npMask )
	    { XFreePixmap( display, np->npMask ); np->npMask= None; }
	}

    return;
    }

#   endif
