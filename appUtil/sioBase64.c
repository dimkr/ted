/************************************************************************/
/*									*/
/*  Simple io streams translating from/to the MIME (RFC 2045)		*/
/*  'Base 64' content transfer encoding.				*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	"sioBase64.h"
#   include	"utilBase64.h"
#   include	<appDebugon.h>


/************************************************************************/
/*									*/
/*  Input.								*/
/*									*/
/*  1)  Contrary to the rfc 2045 standard, decoding stops at a single	*/
/*	'-' character. It is up to the caller to read a series of	*/
/*	chunks of input that are corrupted with '-' signs to restart	*/
/*	decoding after every '-'.					*/
/*  2)  So read bytes, in portions of four upto either a '-' or a '='	*/
/*									*/
/************************************************************************/

typedef struct Base64InputStream
    {
    SimpleInputStream *		bisSisBase64;
    unsigned char		bisBytes[3];
    int				bisCount;
    int				bisColumn;
    const int *			bisBase64Indices;
    } Base64InputStream;

static int sioInBase64ReadBytes(	void *		voidbis,
					unsigned char *	buffer,
					int		count )
    {
    Base64InputStream *		bis= (Base64InputStream *)voidbis;
    int				done= 0;
    int				i;

    while( bis->bisCount+ done < count )
	{
	int			d[4];
	int			n;

	d[3]= d[2]= d[1]= d[0]= 0;

	if  ( bis->bisCount > 0 )
	    {
	    memcpy( buffer, bis->bisBytes, bis->bisCount );
	    buffer += bis->bisCount; done += bis->bisCount;
	    bis->bisCount= 0;
	    }

	for ( n= 0; n < 4; n++ )
	    {
	    /*  1,2  */
	    d[n]= sioInGetCharacter( bis->bisSisBase64 );
	    while( d[n] != EOF			&&
		   d[n] != '='			&&
		   d[n] != '-'			)
		{ d[n]= sioInGetCharacter( bis->bisSisBase64 );	}

	    if  ( d[n] == EOF || d[n] == '=' || d[n] == '-' )
		{ break;	}
	    }

	if  ( n < 4 && d[n] == '-' )
	    { sioInUngetLastRead( bis->bisSisBase64 );	}

	if  ( n == 0 )
	    { break; }
	if  ( n == 1 )
	    { LDEB(n); break; }

	for ( i= 0; i < n; i++ )
	    {
	    d[i]= bis->bisBase64Indices[d[i]];

	    if  ( d[i] >= 64 )
		{ LDEB(d[i]); return -1;	}
	    }

	bis->bisBytes[bis->bisCount++]=	utilBase64Byte0( d );

	if  ( n == 2 )
	    { break; }

	bis->bisBytes[bis->bisCount++]=	utilBase64Byte1( d );

	if  ( n == 3 )
	    { break; }

	bis->bisBytes[bis->bisCount++]=	utilBase64Byte2( d );
	}

    if  ( done < count && bis->bisCount > 0 )
	{
	int	todo= count- done;

	if  ( todo > bis->bisCount )
	    { todo=  bis->bisCount;	}

	memcpy( buffer, bis->bisBytes, todo );

	buffer += todo; done += todo;

	memmove( bis->bisBytes, bis->bisBytes+ todo, bis->bisCount- todo );
	bis->bisCount -= todo;
	}

    return done;
    }

/************************************************************************/
/*									*/
/*  Close a Base 64 input stream.					*/
/*									*/
/*  1)  Drain the underlaying stream upto a '-'.			*/
/*	This is a violation of rfc 2045. See the comment above.		*/
/*									*/
/************************************************************************/

static int sioInBase64Close(	void *	voidbis )
    {
    Base64InputStream *		bis= (Base64InputStream *)voidbis;
    int				c;

    /*  1  */
    c= sioInGetCharacter( bis->bisSisBase64 );
    while( c != EOF			&&
	   c != '-'			)
	{ c= sioInGetCharacter( bis->bisSisBase64 );	}

    if  ( c == '-' )
	{ sioInUngetLastRead( bis->bisSisBase64 );	}

    free( bis ); return 0;
    }

SimpleInputStream * sioInBase64Open(	SimpleInputStream *	sisBase64 )
    {
    SimpleInputStream *	sis;
    Base64InputStream *	bis;

    bis= (Base64InputStream *)malloc( sizeof(Base64InputStream) );
    if  ( ! bis )
	{ XDEB(bis); return (SimpleInputStream *)0;	}

    bis->bisSisBase64= sisBase64;
    bis->bisCount= 0;
    bis->bisColumn= 0;
    bis->bisBase64Indices= utilBase64GetIndexArray();


    sis= sioInOpen( (void *)bis, sioInBase64ReadBytes, sioInBase64Close );

    if  ( ! sis )
	{ XDEB(sis); free( bis ); return (SimpleInputStream *)0; }

    return sis;
    }

/************************************************************************/
/*									*/
/*  Output.								*/
/*									*/
/************************************************************************/

typedef struct Base64OutputStream
    {
    SimpleOutputStream *	bosSosBase64;
    unsigned char		bosBytes[3];
    int				bosCount;
    int				bosColumn;
    } Base64OutputStream;

/************************************************************************/
/*									*/
/*  Close a base64 stream.						*/
/*									*/
/*  1)  If necessary flush the cache.					*/
/*  2)  Should be impossible.						*/
/*  3)  Superfluous, but it feels nice to have predictable values in	*/
/*	the buffer.							*/
/*  4)  Emit the remaining digits and the padding '=' characters.	*/
/*									*/
/************************************************************************/

static int sioOutBase64Close(	void *	voidbos )
    {
    Base64OutputStream *	bos= (Base64OutputStream *)voidbos;

    /*  1  */
    if  ( bos->bosCount > 0 )
	{
	int				d[4];
	int				i;

	/*  2  */
	if  ( bos->bosCount > 2 )
	    { LDEB(bos->bosCount); return -1;	}

	/*  3  */
	for ( i= bos->bosCount; i < 3; i++ )
	    { bos->bosBytes[i]= '\0';	}

	if  ( bos->bosColumn >= 72 )
	    {
	    sioOutPutCharacter( '\r', bos->bosSosBase64 );
	    sioOutPutCharacter( '\n', bos->bosSosBase64 );
	    bos->bosColumn= 0;
	    }

	util_Base64Make( d, bos->bosBytes );

	/*  4  */
	for ( i= 0; i < bos->bosCount+ 1; i++ )
	    {
	    sioOutPutCharacter( UTIL_Base64Digits[d[i]], bos->bosSosBase64 );
	    }

	for ( i= bos->bosCount; i < 3; i++ )
	    { sioOutPutCharacter( '=', bos->bosSosBase64 ); }
	}

    sioOutPutCharacter( '\r', bos->bosSosBase64 );
    sioOutPutCharacter( '\n', bos->bosSosBase64 );

    free( bos ); return 0;
    }

/************************************************************************/
/*									*/
/*  Write bytes in base 64 format.					*/
/*									*/
/*  1)  Fill cache untill full.						*/
/*  2)  If there is not enough output to fill the cache, we are ready.	*/
/*  3)  Otherwise flush the cache.					*/
/*  4)  Emit all complete byte triples.					*/
/*  5)  Chache the incomplete triple.					*/
/*									*/
/************************************************************************/

static int sioOutBase64WriteBytes(	void *			voidbos,
					const unsigned char *	buffer,
					int			count )
    {
    Base64OutputStream *	bos= (Base64OutputStream *)voidbos;
    int				done= 0;

    int				d[4];
    int				i;

    /*  1  */
    while( bos->bosCount < 3	&&
	   done < count		)
	{ bos->bosBytes[bos->bosCount++]= *(buffer++); done++;	}

    /*  2  */
    if  ( bos->bosCount < 3 )
	{ return done;	}

    /*  3  */
    if  ( bos->bosColumn >= 72 )
	{
	sioOutPutCharacter( '\r', bos->bosSosBase64 );
	sioOutPutCharacter( '\n', bos->bosSosBase64 );
	bos->bosColumn= 0;
	}

    util_Base64Make( d, bos->bosBytes );
    for ( i= 0; i < 4; i++ )
	{ sioOutPutCharacter( UTIL_Base64Digits[d[i]], bos->bosSosBase64 ); }
    bos->bosColumn += 4;

    bos->bosCount= 0;

    /*  4  */
    while( done+ 2 < count )
	{
	if  ( bos->bosColumn >= 72 )
	    {
	    sioOutPutCharacter( '\r', bos->bosSosBase64 );
	    sioOutPutCharacter( '\n', bos->bosSosBase64 );
	    bos->bosColumn= 0;
	    }

	util_Base64Make( d, buffer );
	for ( i= 0; i < 4; i++ )
	    {
	    sioOutPutCharacter( UTIL_Base64Digits[d[i]], bos->bosSosBase64 );
	    }
	bos->bosColumn += 4;

	buffer += 3; done += 3;
	}

    /*  5  */
    while( done < count )
	{ bos->bosBytes[bos->bosCount++]= *(buffer++); done++;	}

    return count;
    }

SimpleOutputStream * sioOutBase64Open(	SimpleOutputStream *	sosBase64 )
    {
    SimpleOutputStream *	sos;
    Base64OutputStream *	bos;

    bos= (Base64OutputStream *)malloc( sizeof(Base64OutputStream) );
    if  ( ! bos )
	{ XDEB(bos); return (SimpleOutputStream *)0;	}

    bos->bosSosBase64= sosBase64;
    bos->bosCount= 0;
    bos->bosColumn= 0;

    sos= sioOutOpen( (void *)bos, sioOutBase64WriteBytes,
					    (SIOoutSEEK)0, sioOutBase64Close );

    if  ( ! sos )
	{ XDEB(sos); free( bos ); return (SimpleOutputStream *)0; }

    return sos;
    }
