/************************************************************************/
/*									*/
/*  Simple io streams translating from/to the PostsScript ASCII base 85	*/
/*  encoding.								*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	"sioBase85.h"
#   include	"utilBase85.h"
#   include	<appDebugon.h>


/************************************************************************/
/*									*/
/*  Input.								*/
/*									*/
/************************************************************************/

typedef struct Base85InputStream
    {
    SimpleInputStream *		bisSisBase85;
    unsigned char		bisBytes[4];
    int				bisCount;
    int				bisFoundEnd;
    const int *			bisBase85Indices;
    } Base85InputStream;

static int sioInBase85ReadBytes(	void *		voidbis,
					unsigned char *	buffer,
					int		count )
    {
    Base85InputStream *		bis= (Base85InputStream *)voidbis;
    int				done= 0;
    int				i;

    while( bis->bisCount+ done < count )
	{
	int			d[5];
	unsigned long		v;
	int			n;

	d[4]= d[3]= d[2]= d[1]= d[0]= 0;

	if  ( bis->bisCount > 0 )
	    {
	    memcpy( buffer, bis->bisBytes, bis->bisCount );
	    buffer += bis->bisCount; done += bis->bisCount;
	    bis->bisCount= 0;
	    }

	for ( n= 0; n < 5; n++ )
	    {
	    /*  1,2  */
	    d[n]= sioInGetCharacter( bis->bisSisBase85 );

	    while( d[n] == ' '			||
		   d[n] == '\t'			||
		   d[n] == '\r'			||
		   d[n] == '\n'			||
		   d[n] == '\f'			)
		{ d[n]= sioInGetCharacter( bis->bisSisBase85 );	}

	    if  ( d[n] == EOF )
		{ LDEB(d[n]); bis->bisFoundEnd= 1; return -1;	}

	    if  ( d[n] == '~' )
		{
		int	c= sioInGetCharacter( bis->bisSisBase85 );

		if  ( c != '>' )
		    { CCDEB(d[n],c); bis->bisFoundEnd= 1; return -1; }

		bis->bisFoundEnd= 1;
		break;
		}

	    if  ( n == 0 && d[n] == 'z' )
		{ memcpy( d, "!!!!!", 5 ); n= 5; break;	}
	    }

	if  ( n == 0 )
	    { break; }
	if  ( n == 1 )
	    { LDEB(n); break; }

	for ( i= 0; i < n; i++ )
	    {
	    d[i]= bis->bisBase85Indices[d[i]];

	    if  ( d[i] >= 85 )
		{ LDEB(d[i]); bis->bisFoundEnd= 1; return -1;	}
	    }

	v= 0;
	for ( i= 0; i < 5; i++ )
	    { v= 85* v+ d[i];	}

	for ( i= 0; i < 4; i++ )
	    { bis->bisBytes[3-i]= v % 256; v /= 256;	}

	bis->bisCount += n- 1;

	if  ( n < 5 )
	    { break;	}
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
/*  Close a Base 85 input stream.					*/
/*									*/
/*  1)  Drain the underlaying stream upto '~>'				*/
/*									*/
/************************************************************************/

static int sioInBase85Close(	void *	voidbis )
    {
    Base85InputStream *		bis= (Base85InputStream *)voidbis;
    int				c;

    /*  1  */
    if  ( ! bis->bisFoundEnd )
	{
	c= sioInGetCharacter( bis->bisSisBase85 );
	while( c != EOF			&&
	       c != '~'			)
	    { c= sioInGetCharacter( bis->bisSisBase85 );	}

	if  ( c == '~' )
	    {
	    int	c2= sioInGetCharacter( bis->bisSisBase85 );

	    if  ( c2 != '>' )
		{ CCDEB(c,c2);	}
	    }
	}

    free( bis ); return 0;
    }

SimpleInputStream * sioInBase85Open(	SimpleInputStream *	sisBase85 )
    {
    SimpleInputStream *	sis;
    Base85InputStream *	bis;

    bis= (Base85InputStream *)malloc( sizeof(Base85InputStream) );
    if  ( ! bis )
	{ XDEB(bis); return (SimpleInputStream *)0;	}

    bis->bisSisBase85= sisBase85;
    bis->bisCount= 0;
    bis->bisFoundEnd= 0;
    bis->bisBase85Indices= utilBase85GetIndexArray();


    sis= sioInOpen( (void *)bis, sioInBase85ReadBytes, sioInBase85Close );

    if  ( ! sis )
	{ XDEB(sis); free( bis ); return (SimpleInputStream *)0; }

    return sis;
    }

/************************************************************************/
/*									*/
/*  Output.								*/
/*									*/
/************************************************************************/

typedef struct Base85OutputStream
    {
    SimpleOutputStream *	bosSosBase85;
    unsigned char		bosBytes[4];
    int				bosCount;
    int				bosColumn;
    } Base85OutputStream;

/************************************************************************/
/*									*/
/*  Close a base85 stream.						*/
/*									*/
/*  1)  If necessary flush the cache.					*/
/*  2)  Should be impossible.						*/
/*  3)  Superfluous, but it feels nice to have predictable values in	*/
/*	the buffer.							*/
/*  4)  Emit the remaining digits and the padding '=' characters.	*/
/*									*/
/************************************************************************/

static int sioOutBase85Close(	void *	voidbos )
    {
    Base85OutputStream *	bos= (Base85OutputStream *)voidbos;

    /*  1  */
    if  ( bos->bosCount > 0 )
	{
	int			d[5];
	int			i;
	const int		last= 1;

	/*  2  */
	if  ( bos->bosCount > 3 )
	    { LDEB(bos->bosCount); return -1;	}

	/*  3  */
	for ( i= bos->bosCount; i < 4; i++ )
	    { bos->bosBytes[i]= '\0';	}

	utilBase85Make( d, bos->bosBytes, last );

	/*  4  */
	for ( i= 0; i < bos->bosCount+ 1; i++ )
	    {
	    if  ( bos->bosColumn >= 72 )
		{
		sioOutPutCharacter( '\n', bos->bosSosBase85 );
		bos->bosColumn= 0;
		}

	    sioOutPutCharacter( d[i], bos->bosSosBase85 );
	    }
	}

    sioOutPutCharacter( '~', bos->bosSosBase85 );
    sioOutPutCharacter( '>', bos->bosSosBase85 );
    sioOutPutCharacter( '\n', bos->bosSosBase85 );

    free( bos ); return 0;
    }

/************************************************************************/
/*									*/
/*  Write bytes in base 85 format.					*/
/*									*/
/*  1)  Fill cache untill full.						*/
/*  2)  If there is not enough output to fill the cache, we are ready.	*/
/*  3)  Otherwise flush the cache.					*/
/*  4)  Emit all complete byte triples.					*/
/*  5)  Chache the incomplete triple.					*/
/*									*/
/************************************************************************/

static int sioOutBase85WriteBytes(	void *			voidbos,
					const unsigned char *	buffer,
					int			count )
    {
    Base85OutputStream *	bos= (Base85OutputStream *)voidbos;
    int				done= 0;

    int				d[5];
    int				i;
    int				made;

    const int			last= 0;

    /*  1  */
    while( bos->bosCount < 4	&&
	   done < count		)
	{ bos->bosBytes[bos->bosCount++]= *(buffer++); done++;	}

    /*  2  */
    if  ( bos->bosCount < 4 )
	{ return done;	}

    made= utilBase85Make( d, bos->bosBytes, last );
    for ( i= 0; i < made; i++ )
	{
	/*  3  */
	if  ( bos->bosColumn >= 72 )
	    {
	    sioOutPutCharacter( '\n', bos->bosSosBase85 );
	    bos->bosColumn= 0;
	    }

	sioOutPutCharacter( d[i], bos->bosSosBase85 ); bos->bosColumn++;
	}

    bos->bosCount= 0;

    /*  4  */
    while( done+ 3 < count )
	{
	made= utilBase85Make( d, buffer, last );

	for ( i= 0; i < made; i++ )
	    {
	    /*  3  */
	    if  ( bos->bosColumn >= 72 )
		{
		sioOutPutCharacter( '\n', bos->bosSosBase85 );
		bos->bosColumn= 0;
		}

	    sioOutPutCharacter( d[i], bos->bosSosBase85 ); bos->bosColumn++;
	    }

	buffer += 4; done += 4;
	}

    /*  5  */
    while( done < count )
	{ bos->bosBytes[bos->bosCount++]= *(buffer++); done++;	}

    return count;
    }

SimpleOutputStream * sioOutBase85Open(	SimpleOutputStream *	sosBase85 )
    {
    SimpleOutputStream *	sos;
    Base85OutputStream *	bos;

    bos= (Base85OutputStream *)malloc( sizeof(Base85OutputStream) );
    if  ( ! bos )
	{ XDEB(bos); return (SimpleOutputStream *)0;	}

    bos->bosSosBase85= sosBase85;
    bos->bosCount= 0;
    bos->bosColumn= 0;

    sos= sioOutOpen( (void *)bos, sioOutBase85WriteBytes,
					    (SIOoutSEEK)0, sioOutBase85Close );

    if  ( ! sos )
	{ XDEB(sos); free( bos ); return (SimpleOutputStream *)0; }

    return sos;
    }
