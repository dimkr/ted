/************************************************************************/
/*									*/
/*  Simple io streams, recursive use for packing in 255 byte blocks	*/
/*  with a length indicator.						*/
/*  [Like in the GIF spec.]						*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	"sioBlocked.h"
#   include	<appDebugon.h>


typedef struct BlockedInputStream
    {
    SimpleInputStream *		bisBlocked;
    unsigned char		bisBytes[256];
    int				bisOffset;
    int				bisExhausted;
    } BlockedInputStream;

static int sioInBlockedReadBytes(	void *		voidbis,
					unsigned char *	buffer,
					int		count )
    {
    BlockedInputStream *	bis= (BlockedInputStream *)voidbis;
    int				done= 0;

    int				todo;

    if  ( bis->bisExhausted )
	{ return -1;	}

    if  ( bis->bisOffset < bis->bisBytes[0]+ 1 )
	{
	todo= bis->bisBytes[0]+ 1- bis->bisOffset;

	if  ( todo > count )
	    { todo= count;	}

	memcpy( buffer, bis->bisBytes+ bis->bisOffset, todo );

	bis->bisOffset += todo;
	buffer += todo;
	done += todo;
	}

    while( done < count )
	{
	if  ( bis->bisOffset != bis->bisBytes[0]+ 1 )
	    { LLDEB(bis->bisOffset,bis->bisBytes[0]);	}

	bis->bisOffset= 1;

	todo= sioInGetCharacter( bis->bisBlocked );
	if  ( todo == EOF )
	    { LDEB(todo); bis->bisExhausted= 1; return -1;	}

	bis->bisBytes[0]= todo;

	if  ( bis->bisBytes[0] == 0 )
	    { bis->bisExhausted= 1; break;	}
	
	if  ( todo > count- done )
	    {
	    if  ( sioInReadBytes( bis->bisBlocked, bis->bisBytes+ 1,
							    todo ) != todo )
		{ LDEB(todo); bis->bisExhausted= 1; return -1; }

	    todo= count- done;

	    memcpy( buffer, bis->bisBytes+ 1, todo );
	    }
	else{
	    if  ( sioInReadBytes( bis->bisBlocked, buffer, todo ) != todo )
		{ LDEB(todo); bis->bisExhausted= 1; return -1; }
	    }

	bis->bisOffset= 1+ todo;
	buffer += todo;
	done += todo;
	}

    return done;
    }

/************************************************************************/
/*									*/
/*  Close a Blocked Input stream.					*/
/*									*/
/*  1)  First drain it.							*/
/*									*/
/************************************************************************/

static int sioInBlockedClose(	void *	voidbis )
    {
    int				rval= 0;
    BlockedInputStream *	bis= (BlockedInputStream *)voidbis;

    /*  1 */
    while ( ! bis->bisExhausted )
	{
	unsigned char	bytes[SIOsizBUF];

	if  ( sioInBlockedReadBytes( voidbis, bytes, SIOsizBUF ) < 0 )
	    { LDEB(1); rval= -1; break;	}
	}

    free( voidbis );

    return rval;
    }

SimpleInputStream * sioInBlockedOpen(	SimpleInputStream *	sisBlocked )
    {
    SimpleInputStream *		sis;
    BlockedInputStream *	bis;

    bis= (BlockedInputStream *)malloc( sizeof(BlockedInputStream) );
    if  ( ! bis )
	{ XDEB(bis); return (SimpleInputStream *)0;	}

    bis->bisBlocked= sisBlocked;
    bis->bisBytes[0]= 0;
    bis->bisOffset= 1;
    bis->bisExhausted= 0;

    sis= sioInOpen( (void *)bis, sioInBlockedReadBytes, sioInBlockedClose );

    if  ( ! sis )
	{ XDEB(sis); free( bis ); return (SimpleInputStream *)0; }

    return sis;
    }

/************************************************************************/
/*									*/
/*  Output.								*/
/*									*/
/************************************************************************/

typedef struct BlockedOutputStream
    {
    SimpleOutputStream *	bosSosBlocked;
    unsigned char		bosBytes[256];
    } BlockedOutputStream;

/************************************************************************/
/*									*/
/*  Close a blocked stream.						*/
/*									*/
/*  1)  If necessary flush the cache.					*/
/*  2)  Should be impossible.						*/
/*  3)  Superfluous, but it feels nice to have predictable values in	*/
/*	the buffer.							*/
/*  4)  Emit the remaining digits and the padding '=' characters.	*/
/*									*/
/************************************************************************/

static int sioOutBlockedClose(	void *	voidbos )
    {
    int				rval= 0;
    BlockedOutputStream *	bos= (BlockedOutputStream *)voidbos;

    /*  1  */
    if  ( bos->bosBytes[0] > 0 )
	{
	if  ( sioOutWriteBytes( bos->bosSosBlocked, bos->bosBytes,
				bos->bosBytes[0]+ 1 ) != bos->bosBytes[0]+ 1 )
	    { LDEB(bos->bosBytes[0]); rval= -1;	}
	}

    sioOutPutCharacter( '\0', bos->bosSosBlocked );

    free( bos );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Write bytes in 255 byte blocks with a length indicator.		*/
/*									*/
/************************************************************************/

static int sioOutBlockedWriteBytes(	void *			voidbos,
					const unsigned char *	buffer,
					int			count )
    {
    BlockedOutputStream *	bos= (BlockedOutputStream *)voidbos;
    int				done= 0;

    while( done < count )
	{
	int		todo;

	todo= count- done;
	if  ( todo > 255- bos->bosBytes[0] )
	    { todo= 255- bos->bosBytes[0]; }

	memcpy( bos->bosBytes+ bos->bosBytes[0]+ 1, buffer, todo );
	bos->bosBytes[0] += todo;

	if  ( bos->bosBytes[0] == 255 )
	    {
	    if  ( sioOutWriteBytes( bos->bosSosBlocked, bos->bosBytes,
				bos->bosBytes[0]+ 1 ) != bos->bosBytes[0]+ 1 )
		{ LDEB(bos->bosBytes[0]); return -1;	}

	    bos->bosBytes[0]= 0;
	    }

	buffer += todo;
	done += todo;
	}

    return count;
    }

SimpleOutputStream * sioOutBlockedOpen(	SimpleOutputStream *	sosBlocked )
    {
    SimpleOutputStream *	sos;
    BlockedOutputStream *	bos;

    bos= (BlockedOutputStream *)malloc( sizeof(BlockedOutputStream) );
    if  ( ! bos )
	{ XDEB(bos); return (SimpleOutputStream *)0;	}

    bos->bosSosBlocked= sosBlocked;
    bos->bosBytes[0]= 0;

    sos= sioOutOpen( (void *)bos, sioOutBlockedWriteBytes,
					(SIOoutSEEK)0, sioOutBlockedClose );

    if  ( ! sos )
	{ XDEB(sos); free( bos ); return (SimpleOutputStream *)0; }

    return sos;
    }
