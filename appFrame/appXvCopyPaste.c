/************************************************************************/
/*  Scan, main module.							*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	<sioXprop.h>
#   include	<appImage.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  get the image from the clipboard that 'xv' simulates on the root	*/
/*  window.								*/
/*									*/
/************************************************************************/

int appImgGetXvClipboard(	BitmapDescription *	bd,
				unsigned char **	pBuffer,
				APP_WIDGET		w )
    {
    SimpleInputStream *	sis;

    int			done;

    sis= sioInOpenXvPaste( w );
    if  ( ! sis )
	{ XDEB(sis); return -1;	}

    done= bmXvReadPaste( bd, pBuffer, sis );

    sioInClose( sis );

    if  ( done < 0 )
	{ LDEB(done); return -1;	}

    return 0;
    }

int appImgPutXvClipboard(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				APP_WIDGET			w )
    {
    SimpleOutputStream *	sos;

    int				done;

    sos= sioOutOpenXvCopy( w );
    if  ( ! sos )
	{ XDEB(sos); return -1;	}

    done= bmXvWritePaste( bd, buffer, sos );

    sioOutClose( sos );

    if  ( done < 0 )
	{ LDEB(done); return -1;	}

    return 0;
    }
