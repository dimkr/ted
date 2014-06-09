/************************************************************************/
/*									*/
/*  Simple input stream from stdin.					*/
/*									*/
/*  NOTE that this is for non-interactive programs. No attempt is made	*/
/*	to synchronize with stdout.					*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<unistd.h>

#   include	<sioStdin.h>
#   include	<appDebugon.h>

static int sioStdinClose(	void *	void1 )
    { return 0;	}

static int sioInStdinReadBytes(	void *			void0,
				unsigned char *		buffer,
				int			count )
    { return read( 0, buffer, count );	}

SimpleInputStream * sioInStdinOpen( void )
    {
    SimpleInputStream *	sis;

    sis= sioInOpen( (void *)0, sioInStdinReadBytes, sioStdinClose );

    if  ( ! sis )
	{ XDEB(sis); return (SimpleInputStream *)0; }

    return sis;
    }
