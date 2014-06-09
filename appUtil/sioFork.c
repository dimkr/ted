/************************************************************************/
/*									*/
/*  Simple io streams: Duplicate the stream to another one: Usually	*/
/*  for debugging putposes.						*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>

#   include	"sioFork.h"
#   include	<appDebugon.h>


/************************************************************************/
/*									*/
/*  Exchange of hexed binary little endian data.			*/
/*									*/
/************************************************************************/

static int sioForkClose( void *	voids )
    {
    if  ( voids )
	{ free( voids );	}

    return 0;
    }

typedef struct ForkedInputStream
    {
    SimpleInputStream *		fisSisIn;
    SimpleOutputStream *	fisSosFork;
    int				fisExhausted;
    } ForkedInputStream;

static int sioInForkReadBytes(	void *		voidfis,
				unsigned char *	buffer,
				int		count )
    {
    ForkedInputStream *	fis= (ForkedInputStream *)voidfis;
    int			done= 0;

    if  ( fis->fisExhausted )
	{ LDEB(fis->fisExhausted); return -1;	}

    while( done < count )
	{
	int		got;

	got= sioInReadBytes( fis->fisSisIn, buffer, count- done );
	if  ( got <= 0 )
	    { fis->fisExhausted= 1; break;	}

	sioOutWriteBytes( fis->fisSosFork, buffer, got );

	done += got;
	buffer += got;
	}

    return done;
    }


SimpleInputStream * sioInForkOpen(	SimpleInputStream *	sisIn,
					SimpleOutputStream *	sosFork )
    {
    SimpleInputStream *		sis;
    ForkedInputStream *		fis;

    fis= malloc( sizeof(ForkedInputStream) );
    if  ( ! fis )
	{ XDEB(fis); return (SimpleInputStream *)0;	}

    fis->fisSisIn= sisIn;
    fis->fisSosFork= sosFork;
    fis->fisExhausted= 0;

    sis= sioInOpen( (void *)fis, sioInForkReadBytes, sioForkClose );

    if  ( ! sis )
	{ XDEB(sis); free( fis ); return (SimpleInputStream *)0; }

    return sis;
    }

/************************************************************************/
/*									*/
/************************************************************************/

typedef struct ForkedOutputStream
    {
    SimpleOutputStream *	fosSosOut;
    SimpleOutputStream *	fosSosFork;
    } ForkedOutputStream;

static int sioOutForkWriteBytes(	void *			voidfos,
					const unsigned char *	buffer,
					int			count )
    {
    ForkedOutputStream *	fos= (ForkedOutputStream *)voidfos;

    sioOutWriteBytes( fos->fosSosFork, buffer, count );

    return sioOutWriteBytes( fos->fosSosOut, buffer, count );
    }

SimpleOutputStream * sioOutForkOpen(	SimpleOutputStream *	sosOut,
					SimpleOutputStream *	sosFork )
    {
    SimpleOutputStream *	sos;
    ForkedOutputStream *	fos;

    fos= malloc( sizeof(ForkedInputStream) );
    if  ( ! fos )
	{ XDEB(fos); return (SimpleOutputStream *)0;	}

    fos->fosSosOut= sosOut;
    fos->fosSosFork= sosFork;

    sos= sioOutOpen( (void *)fos, sioOutForkWriteBytes,
						(SIOoutSEEK)0, sioForkClose );

    if  ( ! sos )
	{ XDEB(sos); free( fos ); return (SimpleOutputStream *)0; }

    return sos;
    }
