/************************************************************************/
/*									*/
/*  Simplification of stdio.						*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	"sioGeneral.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Input...								*/
/*									*/
/************************************************************************/

int sioInFillBuffer(	SimpleInputStream *	sis )
    {
    int		got;

    if  ( sis->sisN > 0 )
	{ sis->sisN--; return *(sis->sisP++);	}

    got= (*sis->sisReadBytes)( sis->sisPrivate, sis->sisBuffer, SIOsizBUF );

    if  ( got <= 0 )
	{ return EOF;	}

    sis->sisReadUpto += got;
    sis->sisN= got;
    sis->sisP= sis->sisBuffer;

    sis->sisN--; return *(sis->sisP++);
    }

int sioInUngetLastRead(	SimpleInputStream *	sis )
    {
    if  ( sis->sisN >= SIOsizBUF )
	{ LLDEB(sis->sisN,SIOsizBUF); return -1;	}

    sis->sisN++; sis->sisP--;

    return 0;
    }

int sioInClose(	SimpleInputStream *		sis )
    {
    int		ret;

    ret= (*sis->sisClose)( sis->sisPrivate );

    free( sis );

    return ret;
    }

SimpleInputStream * sioInOpen(	void *			specific,
				SIOinREADBYTES		readBytes,
				SIOinCLOSE		closeIt )
    {
    SimpleInputStream *	sis;

    sis= (SimpleInputStream *)malloc( sizeof(SimpleInputStream) );
    if  ( ! sis )
	{ XDEB(sis); return sis;	}

    sis->sisReadUpto= 0;
    sis->sisN= 0;
    sis->sisP= sis->sisBuffer;
    sis->sisPrivate= specific;

    sis->sisReadBytes= readBytes;
    sis->sisClose= closeIt;

    return sis;
    }

char * sioInGetString(	char *			s,
			int			size,
			SimpleInputStream *	sis )
    {
    int		c;
    char *	to= s;

    if  ( size == 0 )
	{ LDEB(size); return (char *)0;		}
    if  ( size == 1 )
	{ LDEB(size); *s= '\0'; return s;	}

    c= sioInGetCharacter( sis );

    if  ( c == EOF )
	{ return (char *)0;	}

    while( size > 1 )
	{
	*(to++)= c;

	if  ( c == '\n' )
	    { break;	}

	c= sioInGetCharacter( sis );
	if  ( c == EOF )
	    { break;	}
	}

    *to= '\0';

    return s;
    }

int sioInReadBytes(	SimpleInputStream *	sis,
			unsigned char *		buf,
			int			count )
    {
    int		done= 0;

    while( done < count )
	{
	int	todo;
	int	c;

	c= sioInGetCharacter( sis );

	if  ( c == EOF )
	    { break;	}

	*(buf++)= c;
	done++;

	todo= count- done;
	if  ( todo > sis->sisN )
	    { todo=  sis->sisN;	}

	memcpy( buf, sis->sisP, todo );

	buf += todo; sis->sisP += todo; sis->sisN -= todo;
	done += todo;
	}

    return done;
    }

/************************************************************************/
/*									*/
/*  Output...								*/
/*									*/
/************************************************************************/
static int sioOutWriteBuffer(	SimpleOutputStream *	sos,
				const unsigned char *	b,
				int			n )
    {
    while( n > 0 )
	{
	int		done;

	done= (*sos->sosWriteBytes)( sos->sosPrivate, b, n );
	if  ( done <= 0 )
	    { LLDEB(n,done); return -1;	}

	b += done; n -= done;
	}

    return 0;
    }

int sioOutFlushBuffer(	SimpleOutputStream *	sos )
    {
    if  ( sioOutWriteBuffer( sos, sos->sosBuffer, sos->sosN ) )
	{ LDEB(sos->sosN); return -1;	}

    sos->sosN= 0;
    sos->sosP= sos->sosBuffer;

    return 0;
    }

SimpleOutputStream * sioOutOpen(	void *			specific,
					SIOoutWRITEBYTES	writeBytes,
					SIOoutSEEK		seekTo,
					SIOoutCLOSE		closeIt )
    {
    SimpleOutputStream *	sos;

    sos= (SimpleOutputStream *)malloc( sizeof(SimpleOutputStream) );
    if  ( ! sos )
	{ XDEB(sos); return sos;	}

    sos->sosP= sos->sosBuffer;
    sos->sosN= 0;
    sos->sosPrivate= specific;

    sos->sosWriteBytes= writeBytes;
    sos->sosSeek= seekTo;
    sos->sosClose= closeIt;

    return sos;
    }

int sioOutSeek(		SimpleOutputStream *		sos,
			long				pos )
    {
    if  ( sioOutFlushBuffer( sos ) )
	{ LDEB(pos); return -1;	}

    if  ( (*sos->sosSeek)( sos->sosPrivate, pos ) )
	{ XLDEB(sos->sosPrivate,pos); return -1;	}

    return 0;
    }

int sioOutClose(	SimpleOutputStream *		sos )
    {
    int		rval= 0;

    if  ( sioOutFlushBuffer( sos ) )
	{ LDEB(sos->sosN); rval= -1;	}

    if  ( (*sos->sosClose)( sos->sosPrivate ) )
	{ XDEB(sos->sosPrivate); rval= -1;	}

    free( sos );

    return rval;
    }

int sioOutPutString(	const char *		s,
			SimpleOutputStream *	sos )
    {
    while( *s )
	{
	if  ( sioOutPutCharacter( *(s++), sos ) )
	    { return -1;	}
	}
    return 0;
    }

int sioOutWriteBytes(	SimpleOutputStream *	sos,
			const unsigned char *	buf,
			int			count )
    {
    int		done;

    done= count;
    if  ( done > SIOsizBUF- sos->sosN )
	{ done= SIOsizBUF- sos->sosN;	}

    memcpy( sos->sosP, buf, done );

    count -= done; buf += done;  sos->sosN += done; sos->sosP += done;

    if  ( count > 0 )
	{
	if  ( sioOutFlushBuffer( sos ) )
	    { LDEB(sos->sosN); return -1;	}

	while( count >= SIOsizBUF )
	    {
	    if  ( sioOutWriteBuffer( sos, buf, SIOsizBUF ) )
		{ LDEB(sos->sosN); return -1;	}

	    count -= SIOsizBUF; buf += SIOsizBUF; done += SIOsizBUF;
	    }

	if  ( count > 0 )
	    {
	    memcpy( sos->sosBuffer, buf, count );
	    done += count; sos->sosN= count; sos->sosP= sos->sosBuffer+ count;
	    }
	}

    return done;
    }

