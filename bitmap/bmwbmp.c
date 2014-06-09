#   include	"bitmapConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<string.h>

#   include	"bmintern.h"
#   include	<sioStdio.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Read a WBMP file.							*/
/*									*/
/************************************************************************/

static int bmWbmpGetNumber(	unsigned int *		pNum,
				SimpleInputStream *	sis )
    {
    unsigned int	num= 0;
    unsigned int	kontinue= 0x80;

    while( kontinue )
	{
	int	c= sioInGetCharacter( sis );

	if  ( c == EOF )
	    { XDEB(c); return -1;	}

	num= 128* num+ ( c & 0x7f );
	kontinue= c & 0x80;
	}

    *pNum= num; return 0;
    }

static int bmWbmpReadWbmp(	BitmapDescription *	bd,
				unsigned char **	pBuffer,
				SimpleInputStream *	sis )
    {
    unsigned char *	buffer= (unsigned char *)0;
    int			done;

    unsigned int	typeField;
    int			fixHeaderField;

    if  ( bmWbmpGetNumber( &typeField, sis ) )
	{ LDEB(1); return -1;	}
    if  ( typeField != 0 )
	{ LDEB(typeField); return -1;	}

    fixHeaderField= sioInGetCharacter( sis );
    if  ( fixHeaderField & 0x80 )
	{ XDEB(fixHeaderField); return -1; }

    if  ( bmWbmpGetNumber( &(bd->bdPixelsWide), sis ) )
	{ LDEB(1); return -1;	}
    if  ( bmWbmpGetNumber( &(bd->bdPixelsHigh), sis ) )
	{ LDEB(1); return -1;	}

    bd->bdXResolution= bd->bdYResolution= 1;
    bd->bdUnit= BMunPIXEL;

    bd->bdColorEncoding= BMcoWHITEBLACK;
    bd->bdSamplesPerPixel= 1;
    bd->bdBitsPerSample= 1;
    bd->bdBitsPerPixel= 1;

    bmCalculateSizes( bd );

    buffer= (unsigned char *)malloc( bd->bdBufferLength );
    if  ( ! buffer )
	{ XDEB(buffer); return -1;	}

    done= sioInReadBytes( sis, buffer, bd->bdBufferLength );
    if  ( done != bd->bdBufferLength )
	{ LLDEB(done,bd->bdBufferLength); free( buffer ); return -1; }

    *pBuffer= buffer;
    return 0;
    }

int bmReadWbmpFile(	const char *		filename,
			unsigned char **	pBuffer,
			BitmapDescription *	bd,
			int *			pPrivateFormat,
			double *		pCompressionFactor )
    {
    SimpleInputStream *		sis;

    sis= sioInStdioOpen( filename );
    if  ( ! sis )
	{ SXDEB(filename,sis); return -1;	}

    if  ( bmWbmpReadWbmp( bd, pBuffer, sis ) )
	{ SDEB(filename); sioInClose( sis ); return -1; }

    *pPrivateFormat= 0;

    sioInClose( sis );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write a PGM file.							*/
/*									*/
/************************************************************************/

int bmCanWriteWbmpFile(	const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    {
    if  ( bd->bdColorEncoding != BMcoBLACKWHITE		&&
	  bd->bdColorEncoding != BMcoWHITEBLACK		)
	{ return -1;	}

    if  ( bd->bdBitsPerPixel > 1 )
	{ return -1;	}

    if  ( privateFormat != 0 )
	{ LDEB(privateFormat); return -1;	}

    return 0;
    }

static void bmWbmpPutNumber(	int			num,
				SimpleOutputStream *	sos )
    {
    unsigned int	kontinue= 0x00;
    unsigned char	bytes[5];
    int			n= 0;

    for (;;)
	{
	if  ( n >= sizeof(bytes) )
	    { LLDEB(n,sizeof(bytes)); break;	}

	bytes[n]= num & 0x7f;
	num /= 128;

	bytes[n++] |= kontinue;

	if  ( num == 0 )
	    { break;	}

	kontinue= 0x80;
	}

    n--;
    while( n >= 0 )
	{
	sioOutPutCharacter( bytes[n], sos );
	n--;
	}

    return;
    }

int bmWriteWbmpFile(	const char *			filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    {
    int				done;
    SimpleOutputStream *	sos;

    sos= sioOutStdioOpen( filename );
    if  ( ! sos )
	{ SXDEB(filename,sos); return -1;	}

    if  ( privateFormat != 0 )
	{ LDEB(privateFormat); return -1;	}

    bmWbmpPutNumber( privateFormat, sos );

    sioOutPutCharacter( 0, sos );

    bmWbmpPutNumber( bd->bdPixelsWide, sos );
    bmWbmpPutNumber( bd->bdPixelsHigh, sos );

    done= sioOutWriteBytes( sos, buffer, bd->bdBufferLength );
    if  ( done != bd->bdBufferLength )
	{ LLDEB(done,bd->bdBufferLength); sioOutClose( sos ); return -1; }

    sioOutClose( sos );
    return 0;
    }

