/************************************************************************/
/*									*/
/*  Simple io streams: Duplicate the stream to another one: Usually	*/
/*  for debugging purposes.						*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>

#   include	"sioDebug.h"
#   include	<appDebugon.h>


/************************************************************************/
/*									*/
/*  Duplicate input to logging.						*/
/*									*/
/************************************************************************/

static int sioDebugClose( void *	voids )
    {
    if  ( voids )
	{ free( voids );	}

    return 0;
    }

typedef struct DebugedInputStream
    {
    SimpleInputStream *		disSisIn;
    int				disExhausted;
    } DebugedInputStream;

static int sioInDebugReadBytes(	void *		voiddis,
				unsigned char *	buffer,
				int		count )
    {
    DebugedInputStream *	dis= (DebugedInputStream *)voiddis;
    int				done= 0;

    if  ( dis->disExhausted )
	{ LDEB(dis->disExhausted); return -1;	}

    while( done < count )
	{
	int		got;

	got= sioInReadBytes( dis->disSisIn, buffer, count- done );
	if  ( got <= 0 )
	    { dis->disExhausted= 1; break;	}

	appDebug( "%*s", got, buffer );

	done += got;
	buffer += got;
	}

    return done;
    }

SimpleInputStream * sioInDebugOpen(	SimpleInputStream *	sisIn )
    {
    SimpleInputStream *		sis;
    DebugedInputStream *		dis;

    dis= malloc( sizeof(DebugedInputStream) );
    if  ( ! dis )
	{ XDEB(dis); return (SimpleInputStream *)0;	}

    dis->disSisIn= sisIn;
    dis->disExhausted= 0;

    sis= sioInOpen( (void *)dis, sioInDebugReadBytes, sioDebugClose );

    if  ( ! sis )
	{ XDEB(sis); free( dis ); return (SimpleInputStream *)0; }

    return sis;
    }

/************************************************************************/
/*									*/
/************************************************************************/

typedef struct DebuggingOutputStream
    {
    SimpleOutputStream *	dosSosOut;
    } DebuggingOutputStream;

static int sioOutDebugWriteBytes(	void *			voiddos,
					const unsigned char *	buffer,
					int			count )
    {
    DebuggingOutputStream *	dos= (DebuggingOutputStream *)voiddos;
    int				i;

    for ( i= 0; i < count; i++ )
	{ appDebug( "%c", buffer[i] ); }

    if  ( dos->dosSosOut )
	{ return sioOutWriteBytes( dos->dosSosOut, buffer, count );	}
    else{ return count;							}
    }

SimpleOutputStream * sioOutDebugOpen(	SimpleOutputStream *	sosOut )
    {
    SimpleOutputStream *	sos;
    DebuggingOutputStream *	dos;

    dos= malloc( sizeof(DebugedInputStream) );
    if  ( ! dos )
	{ XDEB(dos); return (SimpleOutputStream *)0;	}

    dos->dosSosOut= sosOut;

    sos= sioOutOpen( (void *)dos, sioOutDebugWriteBytes,
						(SIOoutSEEK)0, sioDebugClose );

    if  ( ! sos )
	{ XDEB(sos); free( dos ); return (SimpleOutputStream *)0; }

    return sos;
    }

/************************************************************************/
/*									*/
/*  Return a simple output stream to the debug log.			*/
/*									*/
/************************************************************************/

SimpleOutputStream * sioOutAppDebugOpen()
    {
    SimpleOutputStream *	sos;
    DebuggingOutputStream *	dos;

    dos= malloc( sizeof(DebugedInputStream) );
    if  ( ! dos )
	{ XDEB(dos); return (SimpleOutputStream *)0;	}

    dos->dosSosOut= (SimpleOutputStream *)0;

    sos= sioOutOpen( (void *)dos, sioOutDebugWriteBytes,
						(SIOoutSEEK)0, sioDebugClose );

    if  ( ! sos )
	{ XDEB(sos); free( dos ); return (SimpleOutputStream *)0; }

    return sos;
    }

