/************************************************************************/
/*									*/
/*  Simple output stream to a particular file descriptor.		*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<unistd.h>
#   include	<stdlib.h>

#   include	<sioFd.h>
#   include	<appDebugon.h>

static int sioFdClose(	void *	voidpfd )
    {
    if  ( voidpfd )
	{ free( voidpfd );	}

    return 0;
    }

static int sioOutFdWriteBytes(		void *			voidpfd,
					const unsigned char *	buffer,
					int			count )
    {
    int *	pfd= (int *)voidpfd;

    return write( *pfd, buffer, count );
    }

SimpleOutputStream * sioOutFdOpen( int		fd )
    {
    int *			pfd;
    SimpleOutputStream *	sos;

    pfd= malloc( sizeof( int ) );
    if  ( ! pfd )
	{ XDEB(pfd); return (SimpleOutputStream *)0;	}

    *pfd= fd;

    sos= sioOutOpen( (void *)pfd, sioOutFdWriteBytes,
					    (SIOoutSEEK)0, sioFdClose );

    if  ( ! sos )
	{ XDEB(sos); free( pfd ); return (SimpleOutputStream *)0; }

    return sos;
    }

