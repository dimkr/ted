/************************************************************************/
/*									*/
/*  Simple io streams, recursive use for hexadecimal data.		*/
/*									*/
/*  Do not forget to manually add the '>' at the end if you use these	*/
/*  streams in conjuntion with '/ASCIIHexDecode filter' in PostScript.	*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>

#   include	"sioHex.h"
#   include	<appDebugon.h>


/************************************************************************/
/*									*/
/*  For Hexadecimal decoding.						*/
/*									*/
/************************************************************************/

static const unsigned char	SioHexDigitsU[]= "0123456789ABCDEF";
static const unsigned char	SioHexDigitsL[]= "0123456789abcdef";
static unsigned char		SioHexIndices[256];

/************************************************************************/
/*									*/
/*  Exchange of hexed binary data.					*/
/*									*/
/************************************************************************/

static int sioInHexClose( void *	voids )
    { return 0;	}

static int sioInHexReadBytes(	void *		voidsis,
				unsigned char *	buffer,
				int		count )
    {
    SimpleInputStream *	sis= (SimpleInputStream *)voidsis;
    int			done= 0;

    while( done < count )
	{
	int		c;

	c= sioInGetCharacter( sis );

	while( c == ' ' || c == '\r' || c == '\n' )
	    { c= sioInGetCharacter( sis );	}

	if  ( c == EOF )
	    { return done;	}
	else{
	    if  ( SioHexIndices[c] == 0xff )
		{ sioInUngetLastRead( sis ); return done;	}
	    else{ buffer[0]= SioHexIndices[c];			}
	    }

	c= sioInGetCharacter( sis );

	buffer[0] *= 16;

	while( c == ' ' || c == '\r' || c == '\n' )
	    { c= sioInGetCharacter( sis );	}

	if  ( c == EOF )
	    { LDEB(c); return -1;			}

	if  ( SioHexIndices[c] == 0xff )
	    { CDEB(c); return -1;			}
	else{ buffer[0] += SioHexIndices[c];		}

	buffer++; done++;
	}

    return done;
    }


SimpleInputStream * sioInHexOpen(	SimpleInputStream *	sisHex )
    {
    SimpleInputStream *	sis;

    if  ( SioHexIndices[0] == 0 )
	{
	int	i;

	for ( i= 0; i < sizeof(SioHexIndices); i++ )
	    { SioHexIndices[i]= 0xff;	}

	i= 0;
	while( SioHexDigitsU[i] )
	    { SioHexIndices[SioHexDigitsU[i]]= i; i++;	}
	i= 0;
	while( SioHexDigitsL[i] )
	    { SioHexIndices[SioHexDigitsL[i]]= i; i++;	}
	}

    sis= sioInOpen( (void *)sisHex, sioInHexReadBytes, sioInHexClose );

    if  ( ! sis )
	{ XDEB(sis); return (SimpleInputStream *)0; }

    return sis;
    }

/************************************************************************/
/*									*/
/*  Output.								*/
/*									*/
/************************************************************************/

typedef struct HexOutputStream
    {
    SimpleOutputStream *	hosSosHex;
    int				hosWide;
    int				hosLastNl;
    int				hosColumn;
    } HexOutputStream;

static int sioOutHexClose( void *	voidhos )
    {
    HexOutputStream *		hos= (HexOutputStream *)voidhos;
    SimpleOutputStream *	sos= hos->hosSosHex;

    if  ( hos->hosWide > 0 && hos->hosLastNl && hos->hosColumn > 0 )
	{
	sioOutPutCharacter( '\n', sos );
	hos->hosColumn= 0;
	}

    free( hos );

    return 0;
    }

static int sioOutHexWriteBytes(	void *			voidhos,
				const unsigned char *	buffer,
				int			count )
    {
    HexOutputStream *		hos= (HexOutputStream *)voidhos;
    SimpleOutputStream *	sos= hos->hosSosHex;
    int				done= 0;

    while( done < count )
	{
	if  ( hos->hosWide > 0 && hos->hosColumn >= hos->hosWide )
	    {
	    sioOutPutCharacter( '\n', sos );
	    hos->hosColumn= 0;
	    }

	sioOutPutCharacter( SioHexDigitsL[ ( (*buffer) >> 4 ) & 0x0f ], sos );
	sioOutPutCharacter( SioHexDigitsL[ ( (*buffer) >> 0 ) & 0x0f ], sos );

	if  ( hos->hosWide > 0 )
	    { hos->hosColumn += 2;	}

	buffer++; done++;
	}

    return count;
    }

static int sioHexSeek(		void *			voidhos,
				long			pos )
    {
    HexOutputStream *		hos= (HexOutputStream *)voidhos;

    if  ( hos->hosWide > 0 )
	{ LDEB(hos->hosWide); return -1;	}

    if  ( sioOutSeek( hos->hosSosHex, 2* pos ) )
	{ LDEB(0); return -1;	}

    return 0;
    }

SimpleOutputStream * sioOutHexOpenFolded(
				    SimpleOutputStream *	sosHex,
				    int				wide,
				    int				lastNl )
    {
    HexOutputStream *		hos= malloc( sizeof(HexOutputStream) );
    SimpleOutputStream *	sos;

    if  ( ! hos )
	{ XDEB(hos); return (SimpleOutputStream *)0;	}

    hos->hosSosHex= sosHex;
    hos->hosWide= wide;
    hos->hosLastNl= lastNl;
    hos->hosColumn= 0;

    sos= sioOutOpen( (void *)hos, sioOutHexWriteBytes,
						sioHexSeek, sioOutHexClose );

    if  ( ! sos )
	{ XDEB(sos); free( hos ); return (SimpleOutputStream *)0; }

    return sos;
    }

SimpleOutputStream * sioOutHexOpen(	SimpleOutputStream *	sosHex )
    {
    return sioOutHexOpenFolded( sosHex, 0, 0 );
    }

