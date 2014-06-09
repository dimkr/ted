/************************************************************************/
/*									*/
/*  Retrieve images.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<string.h>

#   include	"appFrame.h"
#   include	"appIcons.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Get a pixmap.							*/
/*									*/
/************************************************************************/

#   ifdef USE_GTK

int appGetImagePixmap(	EditApplication *	ea,
			const char *		name,
			APP_BITMAP_IMAGE *	pPixmap,
			APP_BITMAP_MASK *	pMask )
    {
    int			i;
    NamedPicture *	np;

    APP_BITMAP_IMAGE	pixmap= (APP_BITMAP_IMAGE)0;
    APP_BITMAP_MASK	mask= (APP_BITMAP_MASK)0;

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

    pixmap= gdk_pixmap_create_from_xpm_d( ea->eaToplevel.atTopWidget->window,
					    &mask,
					    NULL,
					    (gchar **)np->npBuffer );

    if  ( ! pixmap )
	{ SXDEB(name,pixmap); return -1;	}

    np->npPixmap= pixmap;
    np->npMask= mask;

    *pPixmap= pixmap; *pMask= mask; return 0;
    }

void appDiscardImagePixmaps(	EditApplication *	ea )
    {
    int			i;
    NamedPicture *	np;

    np= ea->eaNamedPictures;
    for ( i= 0; i < ea->eaNamedPictureCount; np++, i++ )
	{
	if  ( np->npPixmap )
	    { gdk_pixmap_unref( np->npPixmap ); np->npPixmap= NULL; }
	if  ( np->npMask )
	    { gdk_pixmap_unref( np->npMask ); np->npMask= NULL; }
	}

    return;
    }

#   endif
