/************************************************************************/
/*									*/
/*  sioPipe.[ch]:	sio equivalents to popen/pclose.		*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<sioPipe.h>

#   include	<stdlib.h>
#   include	<stdarg.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	<signal.h>

#   include	<errno.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Open an output stream to a command using popen().			*/
/*									*/
/************************************************************************/

static int sioOutPipeWriteBytes(	void *			voidf,
					const unsigned char *	buffer,
					int			count )
    {
    int		rval;
    void	(*prevSigPipeHandler)( int );

    prevSigPipeHandler= signal( SIGPIPE, SIG_IGN );

    rval= fwrite( buffer, 1, count, (FILE *)voidf );

    signal( SIGPIPE, prevSigPipeHandler );

    return rval;
    }

static int sioPipeClose(	void *	voidf )
    {
    int		rval;
    void	(*prevSigPipeHandler)( int );

    prevSigPipeHandler= signal( SIGPIPE, SIG_IGN );

    rval= pclose( (FILE *)voidf );

    signal( SIGPIPE, prevSigPipeHandler );

    return rval;
    }

SimpleOutputStream * sioOutPipeOpen(	const char *	command )
    {
    SimpleOutputStream *	sos= (SimpleOutputStream *)0;
    FILE *			f;

    void			(*prevSigPipeHandler)( int );

    prevSigPipeHandler= signal( SIGPIPE, SIG_IGN );

    f= popen( command, "w" );
    if  ( ! f )
	{ SXDEB(command,f); goto ready;	}

    sos= sioOutOpen( (void *)f, sioOutPipeWriteBytes,
						(SIOoutSEEK)0, sioPipeClose );

    if  ( ! sos )
	{ SXDEB(command,sos); pclose( f ); goto ready;	}

  ready:

    signal( SIGPIPE, prevSigPipeHandler );

    return sos;
    }

