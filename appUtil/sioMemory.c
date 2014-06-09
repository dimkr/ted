/************************************************************************/
/*									*/
/*  Simple io streams using the C stdio.				*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	<sioMemory.h>
#   include	<appDebugon.h>

typedef struct SioMemoryPrivate
    {
    MemoryBuffer *	smpBuffer;
    int			smpPosition;
    } SioMemoryPrivate;

static int sioMemoryClose(	void *	voidsmp )
    { free( voidsmp ); return 0;	}

static int sioInMemoryReadBytes(	void *		voidsmp,
					unsigned char *	buffer,
					int		count )
    {
    SioMemoryPrivate *	smp= (SioMemoryPrivate *)voidsmp;
    MemoryBuffer *	mb= smp->smpBuffer;

    if  ( smp->smpPosition >= mb->mbSize )
	{ return -1;	}

    if  ( smp->smpPosition+ count > mb->mbSize )
	{ count= mb->mbSize- smp->smpPosition;	}

    memcpy( buffer, mb->mbBytes+ smp->smpPosition, count );
    smp->smpPosition += count;

    return count;
    }

static int sioMemorySeek(	void *	voidsmp,
				long	pos )
    {
    SioMemoryPrivate *	smp= (SioMemoryPrivate *)voidsmp;

    smp->smpPosition= pos;

    return 0;
    }

SimpleInputStream * sioInMemoryOpen(	const MemoryBuffer *	mb )
    {
    SimpleInputStream *	sis;
    SioMemoryPrivate *	smp;

    smp= (SioMemoryPrivate *)malloc( sizeof(SioMemoryPrivate) );
    if  ( ! smp )
	{ XDEB(smp); return (SimpleInputStream *)0;	}

    smp->smpBuffer= (MemoryBuffer *)mb;
    smp->smpPosition= 0;

    sis= sioInOpen( (void *)smp, sioInMemoryReadBytes, sioMemoryClose );

    if  ( ! sis )
	{ XDEB(sis); free( smp ); return (SimpleInputStream *)0; }

    return sis;
    }

static int sioOutMemoryWriteBytes(	void *			voidsmp,
					const unsigned char *	buffer,
					int			count )
    {
    SioMemoryPrivate *	smp= (SioMemoryPrivate *)voidsmp;
    MemoryBuffer *	mb= smp->smpBuffer;

    if  ( smp->smpPosition+ count > mb->mbSize )
	{
	unsigned char *	fresh;

	fresh= (unsigned char *)realloc( mb->mbBytes, smp->smpPosition+ count );
	if  ( ! fresh )
	    { LXDEB(smp->smpPosition+ count,fresh); return -1;	}

	mb->mbBytes= fresh;
	}

    memcpy( mb->mbBytes+ smp->smpPosition, buffer, count );

    smp->smpPosition += count;
    if  ( mb->mbSize < smp->smpPosition )
	{ mb->mbSize=  smp->smpPosition;	}

    return count;
    }

SimpleOutputStream * sioOutMemoryOpen(	MemoryBuffer *	mb )
    {
    SimpleOutputStream *	sos;
    SioMemoryPrivate *		smp;

    smp= (SioMemoryPrivate *)malloc( sizeof(SioMemoryPrivate) );
    if  ( ! smp )
	{ XDEB(smp); return (SimpleOutputStream *)0;	}

    smp->smpBuffer= mb;
    smp->smpPosition= 0;

    sos= sioOutOpen( (void *)smp, sioOutMemoryWriteBytes,
					    sioMemorySeek, sioMemoryClose );

    if  ( ! sos )
	{ XDEB(sos); free( smp ); return (SimpleOutputStream *)0; }

    mb->mbSize= 0;

    return sos;
    }
