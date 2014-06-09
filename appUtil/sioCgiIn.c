/************************************************************************/
/*									*/
/*  Simple io streams consuming PUT or POST input of a CGI request.	*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<unistd.h>

#   include	<sioCgiIn.h>
#   include	<appDebugon.h>

typedef struct CgiInputStream
    {
    int		cisContentLength;
    int		cisConsumed;
    int		cisExhausted;
    } CgiInputStream;

static int sioInCgiClose(	void *	voidcis )
    { free( voidcis ); return 0;	}

static int sioInCgiReadBytes(	void *		voidcis,
				unsigned char *	buffer,
				int		count )
    {
    CgiInputStream *	cis= (CgiInputStream *)voidcis;
    int			done= 0;

    if  ( cis->cisExhausted )
	{ return -1;	}

    while( done < count && cis->cisConsumed < cis->cisContentLength )
	{
	int		todo= count- done;
	int		got;

	if  ( todo > cis->cisContentLength- cis->cisConsumed )
	    { todo=  cis->cisContentLength- cis->cisConsumed;	}

	got= read( 0, buffer, todo );
	if  ( got < 1 )
	    { cis->cisExhausted= 1; break;	}

	done += got; cis->cisConsumed += got; buffer += got;
	}

    return done;
    }

extern SimpleInputStream * sioInCgiOpen(	CGIRequest *	cgir )
    {
    SimpleInputStream *	sis;
    CgiInputStream *	cis;

    int			res;
    int			null;
    long		contentLength= 0;

    if  ( ! cgir->cgirRequestMethod )
	{ XDEB(cgir->cgirRequestMethod); return (SimpleInputStream *)0;	}

    if  ( strcmp( cgir->cgirRequestMethod, "POST" )	&&
	  strcmp( cgir->cgirRequestMethod, "PUT" )	)
	{ SDEB(cgir->cgirRequestMethod); return (SimpleInputStream *)0;	}

    if  ( cgir->cgirStdinUsed )
	{ LDEB(cgir->cgirStdinUsed); return (SimpleInputStream *)0;	}

    res= appTagvalGetLongValue( &contentLength, &null,
			    cgir->cgirEnvironmentValues, "CONTENT_LENGTH" );
    if  ( res || null )
	{ LLDEB(res,null); return (SimpleInputStream *)0;	}

    cis= malloc( sizeof(CgiInputStream) );
    if  ( ! cis )
	{ XDEB(cis); return (SimpleInputStream *)0;	}

    cis->cisContentLength= contentLength;
    cis->cisConsumed= 0;
    cis->cisExhausted= 0;

    sis= sioInOpen( (void *)cis, sioInCgiReadBytes, sioInCgiClose );

    if  ( ! sis )
	{ XDEB(sis); free( cis ); return (SimpleInputStream *)0; }

    cgir->cgirStdinUsed= 1;
    return sis;
    }

