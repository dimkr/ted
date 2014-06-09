/************************************************************************/
/*									*/
/*  Simple io streams, Using the LZ77 algorithm of Microsoft's		*/
/*  compress.exe and lzexpand.dll.					*/
/*									*/
/*  This code was adapted from the following Book:			*/
/*  Pete Davis & Mike Wallace: "Windows Undocumented File Formats",	*/
/*  R&D Books (Imprint of Miller Freeman), Lawrence, KS, USA, 1997,	*/
/*  ISBN 0-87930-437-5.							*/
/*									*/
/*  It is very similar to or identical to (Unchecked) the algorithm	*/
/*  described by:							*/
/*									*/
/*  Storer, J.A. & T.G. Szymanski: "Data Compression via Textual	*/
/*  Substitution", Journal of the ACM 29 pp 928.. 951, New York, 1982.	*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	"sioMsLz77.h"
#   include	"sioEndian.h"

#   include	<appDebugon.h>

#   define	MSLZ77_WINSZ		4096
#   define	MSLZ77_MAGIC1		0x44445a53
#   define	MSLZ77_MAGIC2		0x3327f088
#   define	MSLZ77_MAGIC3		0x41

/************************************************************************/
/*									*/
/*  Input steam for flate decompression using zlib.			*/
/*									*/
/************************************************************************/

#   define	MSLZ77_IN_LEN	512

typedef struct MsLz77InputStream
    {
    SimpleInputStream *	misSisMsLz77;

    int			misFlagByte;
    int			misFlagMask;
    int			misFlagBit;

    unsigned char	misInputWindow[MSLZ77_WINSZ];
    int			misWindowLeft;
    int			misWindowOffset;

    int			misExhausted;
    long		misBytesRead;
    long		misDecompressedSize;
    } MsLz77InputStream;

/************************************************************************/
/*									*/
/*  Decompress an input stream using the Microsoft version of LZ77.	*/
/*									*/
/*  1)  Consume the bytes collected in a comressed stretch in the	*/
/*	input window.							*/
/*  2)  Not finished?							*/
/*  3)  Get the byte and move it to the head of the window.		*/
/*  4)  Emit and administration.					*/
/*									*/
/************************************************************************/

/*  1  */
static int sioInMsLz77GetWindowBytes(	MsLz77InputStream *	mis,
					unsigned char *		buffer,
					int			count )
    {
    int		done= 0;
    int		todo= count;

    /*  2  */
    if  ( todo > mis->misWindowLeft )
	{ todo=  mis->misWindowLeft;	}
    if  ( todo > mis->misDecompressedSize- mis->misBytesRead )
	{ todo=  mis->misDecompressedSize- mis->misBytesRead;	}

    while( todo > 0 )
	{
	unsigned char	byte;

	/*  3  */
	byte= mis->misInputWindow[mis->misWindowOffset % MSLZ77_WINSZ];
	mis->misInputWindow[mis->misBytesRead % MSLZ77_WINSZ]= byte;

	/*  4  */
	*(buffer++)= byte; done++; todo--;
	mis->misBytesRead++; mis->misWindowOffset++;
	}

    mis->misWindowLeft -= done;

    /*  2  */
    if  ( mis->misBytesRead >= mis->misDecompressedSize )
	{ mis->misExhausted= 1; }

    return done;
    }

static int sioInMsGetPacketBytes(	MsLz77InputStream *	mis,
					unsigned char *		buffer,
					int			count )
    {
    int		done= 0;
    int		got;

    while( done < count			&&
	   ! mis->misExhausted		&&
	   mis->misFlagBit < 8		)
	{
	int		byte1;
	int		byte2;

	int		todo= count- done;

	if  ( todo > mis->misDecompressedSize- mis->misBytesRead )
	    { todo=  mis->misDecompressedSize- mis->misBytesRead; }

	if  ( ! ( mis->misFlagByte & mis->misFlagMask ) )
	    {
	    int		o;

	    byte1= sioInGetCharacter( mis->misSisMsLz77 );
	    if  ( byte1 == EOF )
		{ XDEB(byte1); mis->misExhausted= 1; break;	}
	    byte2= sioInGetCharacter( mis->misSisMsLz77 );
	    if  ( byte2 == EOF )
		{ XDEB(byte2); mis->misExhausted= 1; break;	}

	    mis->misWindowLeft= ( byte2 & 0x0f )+ 3;
	    o= byte2 & 0xf0;
	    o= o >> 4;
	    o *= 256;
	    o += byte1;
	    o &= 0x0fff;
	    o += 16;
	    mis->misWindowOffset= o;

	    got= sioInMsLz77GetWindowBytes( mis, buffer, count- done );
	    if  ( got <= 0 )
		{ LDEB(got); break;	}

	    done += got; buffer += got;
	    }
	else{
	    byte1= sioInGetCharacter( mis->misSisMsLz77 );
	    if  ( byte1 == EOF )
		{ XDEB(byte1); mis->misExhausted= 1; break;	}

	    mis->misInputWindow[mis->misBytesRead % MSLZ77_WINSZ]= byte1;

	    *(buffer++)= byte1; done++; mis->misBytesRead++;
	    }

	if  ( mis->misBytesRead >= mis->misDecompressedSize )
	    { mis->misExhausted= 1; }

	mis->misFlagBit++; mis->misFlagMask= mis->misFlagMask << 1;
	}

    return done;
    }

/************************************************************************/
/*									*/
/*  The MsLz77 decompression routine:					*/
/*									*/
/************************************************************************/

static int sioInMsLz77ReadBytes( void *			voidmis,
				unsigned char *		buffer,
				int			count )
    {
    MsLz77InputStream *		mis= (MsLz77InputStream *)voidmis;

    int				done= 0;

    while( done < count && ! mis->misExhausted )
	{
	int		got;

	if  ( mis->misWindowLeft > 0 )
	    {
	    got= sioInMsLz77GetWindowBytes( mis, buffer, count- done );
	    if  ( got <= 0 )
		{ LDEB(got);	}

	    done += got; buffer += got;
	    }

	if  ( done >= count || mis->misExhausted )
	    { break;	}

	if  ( mis->misFlagBit < 8 )
	    {
	    got= sioInMsGetPacketBytes( mis, buffer, count- done );

	    if  ( got < 0 )
		{ LDEB(got); mis->misExhausted= 1; break;	}

	    buffer += got; done += got;
	    }

	if  ( done >= count || mis->misExhausted )
	    { break;	}

	mis->misFlagByte= sioInGetCharacter( mis->misSisMsLz77 );
	if  ( mis->misFlagByte == EOF )
	    { XDEB(mis->misFlagByte); mis->misExhausted= 1; break;	}

	mis->misFlagBit= 0;
	mis->misFlagMask= 0x01;
	}

    return done;
    }

/************************************************************************/
/*									*/
/*  Close a MsLz77 Input stream.					*/
/*									*/
/*  1)  First drain it.							*/
/*									*/
/************************************************************************/

static int sioInMsLz77Close(	void *	voidmis )
    {
    int				rval= 0;

    MsLz77InputStream *		mis= (MsLz77InputStream *)voidmis;

    /*  1 */
    while ( ! mis->misExhausted )
	{
	unsigned char	buffer[SIOsizBUF];

	if  ( sioInMsLz77ReadBytes( voidmis, buffer, SIOsizBUF ) < 0 )
	    { LDEB(1); rval= -1; break;	}
	}

    free( voidmis );

    return rval;
    }

SimpleInputStream * sioInMsLz77Open(	SimpleInputStream *	sisMsLz77 )
    {
    SimpleInputStream *		sis;
    MsLz77InputStream *		mis;

    long			decompressedSize;
    long			magic1;
    long			magic2;
    int				magic3;
    int				fileFix;
    int				skip;

    magic1= sioEndianGetLeInt32( sisMsLz77 );
    if  ( magic1 != MSLZ77_MAGIC1 )
	{ XXDEB(magic1,MSLZ77_MAGIC1); return (SimpleInputStream *)0;	}

    magic2= sioEndianGetLeInt32( sisMsLz77 );
    if  ( magic2 != MSLZ77_MAGIC2 )
	{ XXDEB(magic2,MSLZ77_MAGIC2); return (SimpleInputStream *)0;	}

    magic3= sioInGetCharacter( sisMsLz77 );
    if  ( magic3 != MSLZ77_MAGIC3 )
	{ XXDEB(magic3,MSLZ77_MAGIC3); return (SimpleInputStream *)0;	}

    fileFix= sioInGetCharacter( sisMsLz77 );
    if  ( fileFix == EOF )
	{ XDEB(fileFix); return (SimpleInputStream *)0;	}

    skip= sioInGetCharacter( sisMsLz77 );
    if  ( skip == EOF )
	{ XDEB(skip); return (SimpleInputStream *)0;	}
    skip= sioInGetCharacter( sisMsLz77 );
    if  ( skip == EOF )
	{ XDEB(skip); return (SimpleInputStream *)0;	}

    decompressedSize= sioEndianGetLeInt32( sisMsLz77 );

    mis= (MsLz77InputStream *)malloc( sizeof(MsLz77InputStream) );
    if  ( ! mis )
	{ XDEB(mis); return (SimpleInputStream *)0;	}

    mis->misSisMsLz77= sisMsLz77;
    mis->misFlagByte= 0x00;
    mis->misFlagMask= 0x00;
    mis->misFlagBit= 8;

    memset( mis->misInputWindow, ' ', MSLZ77_WINSZ );

    mis->misWindowLeft= 0;
    mis->misWindowOffset= 0;

    mis->misExhausted= 0;
    mis->misBytesRead= 0;
    mis->misDecompressedSize= decompressedSize;

    sis= sioInOpen( (void *)mis, sioInMsLz77ReadBytes, sioInMsLz77Close );

    if  ( ! sis )
	{ XDEB(sis); return (SimpleInputStream *)0; }

    return sis;
    }

