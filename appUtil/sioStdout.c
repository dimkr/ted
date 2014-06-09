/************************************************************************/
/*									*/
/*  Simple output stream to stdout.					*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<unistd.h>

#   include	<sioStdout.h>
#   include	<appDebugon.h>

static int sioStdoutClose(	void *	void1 )
    { return 0;	}

static int sioStdoutSeek(		void *			void0,
					long			pos )
    { LDEB(pos); return -1;	}

static int sioOutStdoutWriteBytes(	void *			void0,
					const unsigned char *	buffer,
					int			count )
    { return write( 1, buffer, count );	}

SimpleOutputStream * sioOutStdoutOpen( void )
    {
    SimpleOutputStream *	sos;

    sos= sioOutOpen( (void *)0, sioOutStdoutWriteBytes,
					    sioStdoutSeek, sioStdoutClose );

    if  ( ! sos )
	{ XDEB(sos); return (SimpleOutputStream *)0; }

    return sos;
    }
