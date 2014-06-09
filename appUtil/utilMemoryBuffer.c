/************************************************************************/
/*									*/
/*  Manage an array of data bytes and its size.				*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	<utilMemoryBuffer.h>
#   include	<appDebugon.h>

void utilInitMemoryBuffer(	MemoryBuffer *	mb )
    {
    mb->mbSize= 0;
    mb->mbBytes= (unsigned char *)0;
    return;
    }

void utilCleanMemoryBuffer(	MemoryBuffer *	mb )
    {
    if  ( mb->mbBytes )
	{ free( mb->mbBytes );	}

    mb->mbSize= 0;
    mb->mbBytes= (unsigned char *)0;

    return;
    }

void utilEmptyMemoryBuffer(	MemoryBuffer *	mb )
    {
    mb->mbSize= 0;
    if  ( mb->mbBytes )
	{ mb->mbBytes[0]= '\0';	}

    return;
    }

int utilSizeMemoryBuffer(	MemoryBuffer *	mb,
				int		size )
    {
    unsigned char *	fresh;

    fresh= (unsigned char *)realloc( mb->mbBytes, size+ 1 );
    if  ( ! fresh )
	{ LXDEB(size,fresh); return -1;	}

    if  ( ! mb->mbBytes )
	{ fresh[0]= '\0';	}

    mb->mbBytes= fresh;
    mb->mbSize= size;
    fresh[mb->mbSize]= '\0';

    return 0;
    }

int utilSetMemoryBuffer(	MemoryBuffer *		mb,
				const unsigned char *	bytes,
				int			size )
    {
    unsigned char *	fresh;

    fresh= (unsigned char *)realloc( mb->mbBytes, size+ 1 );
    if  ( ! fresh )
	{ LXDEB(size,fresh); return -1;	}

    memcpy( fresh, bytes, size );

    mb->mbBytes= fresh;
    mb->mbSize= size;
    fresh[mb->mbSize]= '\0';

    return 0;
    }

int utilAddToMemoryBuffer(	MemoryBuffer *		mb,
				const unsigned char *	bytes,
				int			size )
    {
    unsigned char *	fresh;

    fresh= (unsigned char *)realloc( mb->mbBytes, mb->mbSize+ size+ 1 );
    if  ( ! fresh )
	{ LXDEB(size,fresh); return -1;	}

    memcpy( fresh+ mb->mbSize, bytes, size );

    mb->mbBytes= fresh;
    mb->mbSize= mb->mbSize+ size;
    fresh[mb->mbSize]= '\0';

    return 0;
    }

int utilMemoryAppendBuffer(		MemoryBuffer *		to,
					const MemoryBuffer *	from )
    {
    return utilAddToMemoryBuffer( to, from->mbBytes, from->mbSize );
    }

int utilCopyMemoryBuffer(	MemoryBuffer *		mbTo,
				const MemoryBuffer *	mbFrom )
    {
    if  ( utilSetMemoryBuffer( mbTo, mbFrom->mbBytes, mbFrom->mbSize ) )
	{ LDEB(mbFrom->mbSize); return -1;	}

    return 0;
    }

int utilMemoryCompareBuffers(	const MemoryBuffer *	mb1,
				const MemoryBuffer *	mb2 )
    {
    int		len;
    int		cmp;

    len= mb1->mbSize;
    if  ( len > mb2->mbSize )
	{ len=  mb2->mbSize;	}

    cmp= memcmp( mb1->mbBytes, mb2->mbBytes, len );
    if  ( cmp > 0 )
	{ cmp=  1;	}
    if  ( cmp < 0 )
	{ cmp= -1;	}

    return cmp;
    }
