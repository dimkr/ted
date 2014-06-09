/************************************************************************/
/*									*/
/*  Simple io streams, recursive use reads PostScript 'pfb' encoded	*/
/*  fonts as if they were 'pfa' encoded.				*/
/*									*/
/************************************************************************/

#   include	<stdlib.h>

#   include	"appUtilConfig.h"

#   include	"sioPfb.h"
#   include	<sioEndian.h>
#   include	<appDebugon.h>


/************************************************************************/
/*									*/
/*  Exchange of hexed binary little endian data.			*/
/*									*/
/************************************************************************/

typedef struct BinaryFontStream
    {
    SimpleInputStream *	bfsSis;
    int			bfsKindOfBlock;
    unsigned long	bfsBytesLeftInBlock;
    int			bfsHexColumn;
    unsigned char	bfsAhead;
    unsigned char	bfsHasAhead;
    } BinaryFontStream;

#   define MARKER	128
#   define UNKNOWN	0
#   define ASCII	1
#   define BINARY	2
#   define DONE		3

static int sioPfbClose( void *	voidbfs )
    {
    free( voidbfs );
    return 0;
    }

/*
static char SIO_PFB_HEXDIGITS[]= "0123456789abcdef";
*/
static char SIO_PFB_HEXDIGITS[]= "0123456789ABCDEF";

static int sioInPfbReadBytes(	void *		voidbfs,
				unsigned char *	buffer,
				int		count )
    {
    BinaryFontStream *	bfs= (BinaryFontStream *)voidbfs;
    SimpleInputStream *	sisPfb= bfs->bfsSis;
    int			done= 0;
    int			todo;

    int			c;

    if  ( bfs->bfsKindOfBlock == DONE )
	{ return 0;	}

    if  ( bfs->bfsHasAhead )
	{
	*(buffer++)= bfs->bfsAhead; done++;
	bfs->bfsHasAhead= 0;
	}

    while( done < count	)
	{
	if  ( bfs->bfsBytesLeftInBlock == 0 )
	    {
	    c= sioInGetCharacter( sisPfb );
	    if  ( c != MARKER )
		{ LDEB(c); return -1;	}

	    bfs->bfsKindOfBlock= sioInGetCharacter( sisPfb );
	    switch( bfs->bfsKindOfBlock )
		{
		case ASCII:
		    if  ( bfs->bfsHexColumn > 0 )
			{ *(buffer++)= '\n'; done++;	}

		    bfs->bfsHexColumn= 0;
		    bfs->bfsBytesLeftInBlock= sioEndianGetLeUint32( sisPfb );
		    break;

		case BINARY:
		    bfs->bfsHexColumn= 0;
		    bfs->bfsBytesLeftInBlock= sioEndianGetLeUint32( sisPfb );
		    break;

		case DONE:
		    return done;

		default:
		    LDEB(bfs->bfsKindOfBlock); return -1;
		}
	    }

	if  ( bfs->bfsKindOfBlock == ASCII )
	    {
	    int		i;

	    if  ( bfs->bfsBytesLeftInBlock < count- done )
		{ todo= bfs->bfsBytesLeftInBlock;	}
	    else{ todo= count- done;			}

	    todo= sioInReadBytes( sisPfb, buffer, todo );
	    if  ( todo < 1 )
		{ LDEB(todo); return todo;	}

	    for ( i= 0; i < todo; buffer++, i++ )
		{
		if  ( *buffer == '\r' )
		    { *buffer=   '\n';	}
		}

	    done += todo;
	    bfs->bfsBytesLeftInBlock -= todo;

	    continue;
	    }

	if  ( bfs->bfsKindOfBlock == BINARY )
	    {
	    while( bfs->bfsBytesLeftInBlock > 0	&&
		   done < count			)
		{
		if  ( bfs->bfsHexColumn >= 64 )
		    { done++; *(buffer++)= '\n'; bfs->bfsHexColumn= 0;	}

		if  ( done+ 1 >= count )
		    { break;	}

		c= sioInGetCharacter( sisPfb );
		if  ( c == EOF )
		    { return done;	}

		bfs->bfsBytesLeftInBlock--;
		*(buffer++)= SIO_PFB_HEXDIGITS[c/16];
		*(buffer++)= SIO_PFB_HEXDIGITS[c%16];
		done += 2; bfs->bfsHexColumn += 2;
		}

	    if  ( bfs->bfsBytesLeftInBlock > 0	&&
		  done < count			)
		{
		c= sioInGetCharacter( sisPfb );
		if  ( c == EOF )
		    { return done;	}

		bfs->bfsBytesLeftInBlock--;
		*(buffer++)= SIO_PFB_HEXDIGITS[c/16];
		bfs->bfsAhead= SIO_PFB_HEXDIGITS[c%16];
		bfs->bfsHasAhead= 1;
		done += 1; bfs->bfsHexColumn += 2; /* += 2! */
		}

	    continue;
	    }

	LDEB(bfs->bfsKindOfBlock); return -1;
	}

    return done;
    }


SimpleInputStream * sioInPfbOpen(	SimpleInputStream *	sisPfb )
    {
    BinaryFontStream *	bfs;
    SimpleInputStream *	sis;

    bfs= malloc( sizeof(BinaryFontStream) );
    if  ( ! bfs )
	{ XDEB(bfs); return (SimpleInputStream *)0;	}

    bfs->bfsSis= sisPfb;
    bfs->bfsKindOfBlock= UNKNOWN;
    bfs->bfsBytesLeftInBlock= 0;
    bfs->bfsHexColumn= 0;
    bfs->bfsHasAhead= 0;

    sis= sioInOpen( (void *)bfs, sioInPfbReadBytes, sioPfbClose );

    if  ( ! sis )
	{ XDEB(sis); free( bfs ); return (SimpleInputStream *)0; }

    return sis;
    }

