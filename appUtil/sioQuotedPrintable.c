/************************************************************************/
/*									*/
/*  Simple io streams translating from/to the MIME (RFC 2045)		*/
/*  'Quoted Printable' content transfer encoding.			*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	"sioQuotedPrintable.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  For Hexadecimal decoding.						*/
/*									*/
/************************************************************************/

static const unsigned char	SioHexDigits[]= "0123456789ABCDEF";
static unsigned char		SioHexIndices[256];

/************************************************************************/
/*									*/
/*  Input.								*/
/*									*/
/*  1)  Contrary to the rfc 2045 standard, decoding stops at a single	*/
/*	'-' character at the beginning of a line. It is up to the	*/
/*	caller to read a series of chunks of input that are corrupted	*/
/*	with '-' signs to restart decoding after every '-'.		*/
/*									*/
/************************************************************************/

typedef struct QuotedInputStream
    {
    SimpleInputStream *		qisSisQuoted;
    int				qisExhausted;
    int				qisColumn;
    } QuotedInputStream;

static int sioInQuotedReadBytes(	void *		voidqis,
					unsigned char *	buffer,
					int		count )
    {
    QuotedInputStream *		qis= (QuotedInputStream *)voidqis;
    int				done= 0;

    if  ( qis->qisExhausted )
	{ return -1;	}

    while( done < count )
	{
	int			c;

	c= sioInGetCharacter( qis->qisSisQuoted );
	if  ( c == EOF )
	    { qis->qisExhausted= 1; break;	}

	if  ( c == '-' && qis->qisColumn == 0 )
	    {
	    sioInUngetLastRead( qis->qisSisQuoted );
	    qis->qisExhausted= 1; break;
	    }

	if  ( c == '\r' )
	    {
	    c= sioInGetCharacter( qis->qisSisQuoted );
	    if  ( c != '\n' )
		{ *(buffer++)= '\r'; done++;	}

	    sioInUngetLastRead( qis->qisSisQuoted );

	    continue;
	    }
	if  ( c == '\n' )
	    { *(buffer++)= c; done++; qis->qisColumn= 0; continue; }

	if  ( c == '=' )
	    {
	    int		x1;
	    int		x2;

	    x1= sioInGetCharacter( qis->qisSisQuoted );

	    switch( x1 )
		{
		case '\n':
		    continue;

		case '\r':
		    x2= sioInGetCharacter( qis->qisSisQuoted );
		    if  ( x2 != '\n' )
			{
			CDEB(x2);
			if  ( x2 != EOF )
			    { sioInUngetLastRead( qis->qisSisQuoted ); }
			}
		    continue;

		case EOF:
		    XDEB(x1);
		    qis->qisExhausted= 1;
		    return -1;

		default:
		    XDEB(x1);
		    sioInUngetLastRead( qis->qisSisQuoted );
		    *(buffer++)= c; done++;
		    break;
		}
	    }

	*(buffer++)= c; done++; qis->qisColumn++; continue;
	}

    return done;
    }

/************************************************************************/
/*									*/
/*  Close a 'Quoted Printable' input stream.				*/
/*									*/
/*  1)  Drain the underlaying stream upto a '-' at the beginning of.	*/
/*	a line. This is a violation of rfc 2045. See the comment above.	*/
/*									*/
/************************************************************************/

static int sioInQuotedClose(	void *	voidqis )
    {
    int				rval= 0;
    QuotedInputStream *		qis= (QuotedInputStream *)voidqis;

    while( ! qis->qisExhausted )
	{
	int		res;
	unsigned char	scratch[SIOsizBUF];

	res= sioInQuotedReadBytes( voidqis, scratch, SIOsizBUF );
	if  ( res < 0 )
	    { LDEB(res); rval= -1; break;	}
	}

    free( qis );

    return rval;
    }

SimpleInputStream * sioInQuotedPrintableOpen(
					SimpleInputStream *	sisQuoted )
    {
    SimpleInputStream *	sis;
    QuotedInputStream *	qis;

    if  ( SioHexIndices[0] == 0 )
	{
	int	i;

	for ( i= 0; i < sizeof(SioHexIndices); i++ )
	    { SioHexIndices[i]= 0xff;	}

	i= 0;
	while( SioHexDigits[i] )
	    { SioHexIndices[SioHexDigits[i]]= i; i++;	}
	}

    qis= (QuotedInputStream *)malloc( sizeof(QuotedInputStream) );
    if  ( ! qis )
	{ XDEB(qis); return (SimpleInputStream *)0;	}

    qis->qisSisQuoted= sisQuoted;
    qis->qisExhausted= 0;
    qis->qisColumn= 0;

    sis= sioInOpen( (void *)qis, sioInQuotedReadBytes, sioInQuotedClose );

    if  ( ! sis )
	{ XDEB(sis); free( qis ); return (SimpleInputStream *)0; }

    return sis;
    }

/************************************************************************/
/*									*/
/*  Output.								*/
/*									*/
/************************************************************************/

typedef struct QuotedOutputStream
    {
    SimpleOutputStream *	qosSosQuoted;
    int				qosColumn;
    int				qosAfterWhiteSpace;
    } QuotedOutputStream;

/************************************************************************/
/*									*/
/*  Close a 'Quoted Printable' output stream.				*/
/*									*/
/*  1)  If necessary finish the current line of output.			*/
/*									*/
/************************************************************************/

static int sioOutQuotedClose(	void *	voidqos )
    {
    QuotedOutputStream *	qos= (QuotedOutputStream *)voidqos;

    /*  1  */
    if  ( qos->qosColumn > 0 )
	{
	sioOutPutCharacter( '=',  qos->qosSosQuoted );
	sioOutPutCharacter( '\r', qos->qosSosQuoted );
	sioOutPutCharacter( '\n', qos->qosSosQuoted );
	qos->qosColumn= 0;
	}

    free( qos );
    return 0;
    }

/************************************************************************/
/*									*/
/*  Write bytes in Quoted Printable format.				*/
/*									*/
/************************************************************************/

static void sioOutQuotedBreakLine(	QuotedOutputStream *	qos )
    {
    sioOutPutCharacter( '=',  qos->qosSosQuoted );
    sioOutPutCharacter( '\r', qos->qosSosQuoted );
    sioOutPutCharacter( '\n', qos->qosSosQuoted );

    qos->qosColumn= 0;
    qos->qosAfterWhiteSpace= 0;

    return;
    }

static void sioOutQoutedEmitHex(	QuotedOutputStream *	qos,
					int			c )
    {
    sioOutPutCharacter( '=', qos->qosSosQuoted );
    sioOutPutCharacter( SioHexDigits[( c >> 4 ) & 0x0f], qos->qosSosQuoted );
    sioOutPutCharacter( SioHexDigits[( c >> 0 ) & 0x0f], qos->qosSosQuoted );

    qos->qosColumn += 3;
    qos->qosAfterWhiteSpace= 0;

    return;
    }

static int sioOutQuotedWriteBytes(	void *			voidqos,
					const unsigned char *	buffer,
					int			count )
    {
    QuotedOutputStream *	qos= (QuotedOutputStream *)voidqos;
    int				done= 0;

    while( done < count )
	{
	if  ( *buffer == '\r' && qos->qosAfterWhiteSpace )
	    {
	    if  ( qos->qosColumn > 72 )
		{ sioOutQuotedBreakLine( qos );	}

	    sioOutQoutedEmitHex( qos, *buffer );

	    buffer++; done++; continue;
	    }

	if  ( *buffer == '\n' && qos->qosAfterWhiteSpace )
	    {
	    if  ( qos->qosColumn > 72 )
		{ sioOutQuotedBreakLine( qos );	}

	    sioOutQoutedEmitHex( qos, *buffer );
	    sioOutQuotedBreakLine( qos );

	    buffer++; done++; continue;
	    }

	if  ( *buffer == '='				||
	      *buffer < 32				||	/* ' '	*/
	      *buffer > 126				||	/* '~'	*/
	      ( *buffer == '-' && qos->qosColumn == 0 )	)
	    {
	    if  ( qos->qosColumn > 72 )
		{ sioOutQuotedBreakLine( qos );	}

	    sioOutQoutedEmitHex( qos, *buffer );

	    buffer++; done++; continue;
	    }

	if  ( qos->qosColumn > 75 )
	    { sioOutQuotedBreakLine( qos );	}

	if  ( *buffer == ' ' || *buffer == '\t' )
	    { qos->qosAfterWhiteSpace= 1;	}
	else{ qos->qosAfterWhiteSpace= 0;	}

	sioOutPutCharacter( *buffer, qos->qosSosQuoted );

	qos->qosColumn += 1;
	buffer++; done++; continue;
	}

    return count;
    }

SimpleOutputStream * sioOutQuotedPrintableOpen(
				    SimpleOutputStream *	sosQuoted )
    {
    SimpleOutputStream *	sos;
    QuotedOutputStream *	qos;

    qos= (QuotedOutputStream *)malloc( sizeof(QuotedOutputStream) );
    if  ( ! qos )
	{ XDEB(qos); return (SimpleOutputStream *)0;	}

    qos->qosSosQuoted= sosQuoted;
    qos->qosColumn= 0;
    qos->qosAfterWhiteSpace= 0;

    sos= sioOutOpen( (void *)qos, sioOutQuotedWriteBytes,
					    (SIOoutSEEK)0, sioOutQuotedClose );

    if  ( ! sos )
	{ XDEB(sos); free( qos ); return (SimpleOutputStream *)0; }

    return sos;
    }
