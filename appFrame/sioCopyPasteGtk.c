/************************************************************************/
/*									*/
/*  Simple io streams using X11 windows properties.			*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>

#   include	<sioXprop.h>
#   include	<sioMemory.h>

#   include	<appDebugon.h>

#   ifdef	USE_GTK

static GdkAtom	XA_XV_CLIPBOARD= (GdkAtom)0;
static GdkAtom	XA_STRING= (GdkAtom)0;

/************************************************************************/
/*									*/
/*  Input..								*/
/*									*/
/************************************************************************/

typedef struct PasteInputStream
    {
    MemoryBuffer		pisMemoryBuffer;
    SimpleInputStream *		pisPasteStream;
    } PasteInputStream;

static int sioInPasteReadBytes(		void *			voidxsc,
					unsigned char *		buffer,
					int			count )
    {
    PasteInputStream *	pis= (PasteInputStream *)voidxsc;

    return sioInReadBytes( pis->pisPasteStream, buffer, count );
    }

static int sioInPasteClose(		void *			voidxsc )
    {
    PasteInputStream *	pis= (PasteInputStream *)voidxsc;

    if  ( ! pis->pisPasteStream			||
	  sioInClose( pis->pisPasteStream )	)
	{ XDEB(pis->pisPasteStream); return -1;	}

    free( pis );

    return 0;
    }

SimpleInputStream * sioInOpenPaste(	APP_WIDGET		w,
					GtkSelectionData *	gsd )
    {
    SimpleInputStream *		sis;
    PasteInputStream *		pis;

    pis= (PasteInputStream *)malloc( sizeof( PasteInputStream ) );
    if  ( ! pis )
	{ XDEB(pis); return (SimpleInputStream *)0;	}

    utilInitMemoryBuffer( &(pis->pisMemoryBuffer) );
    pis->pisMemoryBuffer.mbBytes= gsd->data;
    pis->pisMemoryBuffer.mbSize= gsd->length;

    pis->pisPasteStream= sioInMemoryOpen( &(pis->pisMemoryBuffer) );
    if  ( ! pis->pisPasteStream )
	{
	XDEB(pis->pisPasteStream);
	free( pis ); return (SimpleInputStream *)0;
	}

    sis= sioInOpen( (void *)pis, sioInPasteReadBytes, sioInPasteClose );
    if  ( ! sis )
	{ XDEB(sis); free( pis ); return (SimpleInputStream *)0; }

    return sis;
    }

SimpleInputStream * sioInOpenXvPaste(	APP_WIDGET	w )
    {
    LDEB(1); return (SimpleInputStream *)0;
    }

/************************************************************************/
/*									*/
/*  Output: Save Bytes for later delivery.				*/
/*									*/
/************************************************************************/

typedef struct CopyStream
    {
    GtkSelectionData *		csSelectionData;
    GdkAtom			csType;
    MemoryBuffer		csMemoryBuffer;
    SimpleOutputStream *	csCopyStream;
    } CopyStream;

static int sioOutCopyWriteBytes(	void *			voidxsc,
					const unsigned char *	buffer,
					int			count )
    {
    CopyStream *	cs= (CopyStream *)voidxsc;

    return sioOutWriteBytes( cs->csCopyStream, buffer, count );
    }

static int sioOutCopyClose(		void *			voidxsc )
    {
    CopyStream *	cs= (CopyStream *)voidxsc;

    if  ( ! cs->csCopyStream			||
	  sioOutClose( cs->csCopyStream )	)
	{ XDEB(cs->csCopyStream); return -1;	}

    gtk_selection_data_set( cs->csSelectionData, cs->csType, 8,
				    cs->csMemoryBuffer.mbBytes,
				    cs->csMemoryBuffer.mbSize );

    utilCleanMemoryBuffer( &(cs->csMemoryBuffer) );

    free( cs );

    return 0;
    }

SimpleOutputStream * sioOutOpenCopy(	APP_WIDGET		w,
					GtkSelectionData *	gsd )
    {
    SimpleOutputStream *	sos;
    CopyStream *		cs;

    cs= (CopyStream *)malloc( sizeof( CopyStream ) );
    if  ( ! cs )
	{ XDEB(cs); return (SimpleOutputStream *)0;	}

    cs->csType= gsd->target;
    cs->csSelectionData= gsd;
    utilInitMemoryBuffer( &(cs->csMemoryBuffer) );

    cs->csCopyStream= sioOutMemoryOpen( &(cs->csMemoryBuffer) );
    if  ( ! cs->csCopyStream )
	{
	XDEB(cs->csCopyStream);
	free( cs ); return (SimpleOutputStream *)0;
	}

    sos= sioOutOpen( (void *)cs, sioOutCopyWriteBytes, (SIOoutSEEK)0,
							    sioOutCopyClose );
    if  ( ! sos )
	{ XDEB(sos); free( cs ); return (SimpleOutputStream *)0; }

    return sos;
    }

/************************************************************************/
/*									*/
/*  Output: Copy to the 'xv' clipboard on the root window.		*/
/*									*/
/************************************************************************/

static int sioOutXvCopyClose(		void *			voidxsc )
    {
    CopyStream *	cs= (CopyStream *)voidxsc;

    if  ( ! cs->csCopyStream			||
	  sioOutClose( cs->csCopyStream )	)
	{ XDEB(cs->csCopyStream); return -1;	}

    gdk_property_change( (GdkWindow *)0, XA_XV_CLIPBOARD, XA_STRING, 8,
				    GDK_PROP_MODE_REPLACE,
				    cs->csMemoryBuffer.mbBytes,
				    cs->csMemoryBuffer.mbSize );

    utilCleanMemoryBuffer( &(cs->csMemoryBuffer) );

    free( cs );

    return 0;
    }

SimpleOutputStream * sioOutOpenXvCopy(	APP_WIDGET		w )
    {
    SimpleOutputStream *	sos;
    CopyStream *		cs;

    if  ( ! XA_XV_CLIPBOARD )
	{
	XA_XV_CLIPBOARD= gdk_atom_intern( "XV_CLIPBOARD", FALSE );
	if  ( ! XA_XV_CLIPBOARD  )
	    { XDEB(XA_XV_CLIPBOARD); return (SimpleOutputStream *)0;	}
	}
    if  ( ! XA_STRING )
	{
	XA_STRING= gdk_atom_intern( "STRING", FALSE );
	if  ( ! XA_STRING  )
	    { XDEB(XA_STRING); return (SimpleOutputStream *)0;	}
	}

    cs= (CopyStream *)malloc( sizeof( CopyStream ) );
    if  ( ! cs )
	{ XDEB(cs); return (SimpleOutputStream *)0;	}

    cs->csType= XA_XV_CLIPBOARD;
    cs->csSelectionData= (GtkSelectionData *)0;
    utilInitMemoryBuffer( &(cs->csMemoryBuffer) );

    cs->csCopyStream= sioOutMemoryOpen( &(cs->csMemoryBuffer) );
    if  ( ! cs->csCopyStream )
	{
	XDEB(cs->csCopyStream);
	free( cs ); return (SimpleOutputStream *)0;
	}

    sos= sioOutOpen( (void *)cs, sioOutCopyWriteBytes,
					    (SIOoutSEEK)0, sioOutXvCopyClose );
    if  ( ! sos )
	{ XDEB(sos); free( cs ); return (SimpleOutputStream *)0; }

    return sos;
    }

#   endif
