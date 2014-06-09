#   include	"bitmapConfig.h"

#   include	<stdlib.h>
#   include	"bmintern.h"
#   include	<appDebugon.h>
#   include	<sioStdio.h>
#   include	<sioEndian.h>
#   include	<utilEndian.h>

/************************************************************************/
/*									*/
/*  Write a bitmap to a wmf stream.					*/
/*									*/
/************************************************************************/

#   define	MM_ANISOTROPIC			8

#   define	WINMETA_SaveDC			0x001e
#   define	WINMETA_SetMapMode		0x0103
#   define	WINMETA_SetWindowExt		0x020c
#   define	WINMETA_SetWindowOrg		0x020b
#   define	WINMETA_StretchBlt		0x0b41
#   define	WINMETA_RestoreDC		0x0127

static int bmWmfWriteSpecialHeader(
				const BitmapDescription *	bd,
				SimpleOutputStream *		sos )
    {
    unsigned int	checksum= 0;
    unsigned int	val;

    int			twipsWide;
    int			twipsHigh;

    const unsigned long	key= 0x9ac6cdd7;

    bmImageSizeTwips( &twipsWide, &twipsHigh, bd );

    sioEndianPutLeInt32( key, sos );
    checksum ^= ( key & 0x0000ffff );
    checksum ^= ( key & 0xffff0000 ) >> 16;

    /*  handle  */
    val= 0;
    sioEndianPutLeUint16( val, sos );
    checksum ^= val;

    /*  left,top  */
    val= 0;
    sioEndianPutLeUint16( val, sos );
    checksum ^= val;
    sioEndianPutLeUint16( val, sos );
    checksum ^= val;

    /*  right,bottom  */
    sioEndianPutLeUint16( twipsWide, sos );
    checksum ^= twipsWide;
    sioEndianPutLeUint16( twipsHigh, sos );
    checksum ^= twipsHigh;

    /*  inch  */
    val= 1440;
    sioEndianPutLeUint16( val, sos );
    checksum ^= val;

    /*  reserved  */
    val= 0;
    sioEndianPutLeUint32( val, sos );
    checksum ^= ( val & 0x0000ffff );
    checksum ^= ( val & 0xffff0000 ) >> 16;

    /*  checksum  */
    sioEndianPutLeUint16( checksum, sos );

    return 22;
    }

static int bmWmfWriteWmfIntern(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos,
				int				specialHeader )
    {
    int			done;
    int			bytesWritten= 0;
    long		bltArgCountPos= -1L;
    long		bltArgCount;

    int			headSize= 0;
    long		fileSize= 0L;
    long		headerOffset= 0L;
    long		recordSize;
    long		maxRecordSize= 0L;

    if  ( specialHeader )
	{
	done= bmWmfWriteSpecialHeader( bd, sos );

	if  ( done < 0 )
	    { LDEB(done); return -1;	}

	headSize= done;
	bytesWritten += done;
	}

    sioEndianPutLeInt16( 1, sos );		/*  fileType		*/
    sioEndianPutLeInt16( 9, sos );		/*  headerSize		*/
    sioEndianPutLeInt16( 768, sos );		/*  windowsVersion	*/
    sioEndianPutLeInt32( fileSize, sos );	/*  fileSize		*/
    sioEndianPutLeInt16( 0, sos );		/*  objectCount		*/
    sioEndianPutLeInt32( maxRecordSize, sos );	/*  maxRecordSize	*/
    sioEndianPutLeInt16( 0, sos );		/*  parameterCount	*/

    bytesWritten += 2+ 2+ 2+ 4+ 2+ 4+ 2;

    recordSize= 2+ 1;
    sioEndianPutLeInt32( recordSize, sos );
    sioEndianPutLeInt16( WINMETA_SaveDC, sos );

    bytesWritten += 2* recordSize;
    if  ( maxRecordSize < recordSize )
	{ maxRecordSize=  recordSize;	}

    recordSize= 2+ 1+ 1;
    sioEndianPutLeInt32( recordSize, sos );
    sioEndianPutLeInt16( WINMETA_SetMapMode, sos );
    sioEndianPutLeInt16( MM_ANISOTROPIC, sos );

    bytesWritten += 2* recordSize;
    if  ( maxRecordSize < recordSize )
	{ maxRecordSize=  recordSize;	}

    recordSize= 2+ 1+ 1+ 1;
    sioEndianPutLeInt32( recordSize, sos );
    sioEndianPutLeInt16( WINMETA_SetWindowOrg, sos );
    sioEndianPutLeInt16( 0, sos );
    sioEndianPutLeInt16( 0, sos );

    bytesWritten += 2* recordSize;
    if  ( maxRecordSize < recordSize )
	{ maxRecordSize=  recordSize;	}

    recordSize= 2+ 1+ 1+ 1;
    sioEndianPutLeInt32( recordSize, sos );
    sioEndianPutLeInt16( WINMETA_SetWindowExt, sos );
    sioEndianPutLeInt16( bd->bdPixelsHigh, sos );
    sioEndianPutLeInt16( bd->bdPixelsWide, sos );

    bytesWritten += 2* recordSize;
    if  ( maxRecordSize < recordSize )
	{ maxRecordSize=  recordSize;	}

    bltArgCountPos= bytesWritten;
    recordSize= 2+ 1+ 2+ ( 1+ 1+ 1+ 1 )+ ( 1+ 1+ 1+ 1 );
    sioEndianPutLeInt32( recordSize, sos );
    sioEndianPutLeInt16( WINMETA_StretchBlt, sos );
    sioEndianPutLeInt32( 0xcc0020, sos );
    sioEndianPutLeInt16( bd->bdPixelsHigh, sos );
    sioEndianPutLeInt16( bd->bdPixelsWide, sos );
    sioEndianPutLeInt16( 0, sos );
    sioEndianPutLeInt16( 0, sos );
    sioEndianPutLeInt16( bd->bdPixelsHigh, sos );
    sioEndianPutLeInt16( bd->bdPixelsWide, sos );
    sioEndianPutLeInt16( 0, sos );
    sioEndianPutLeInt16( 0, sos );

    done= bmBmpSaveDib( bd, buffer, bytesWritten+ 2* recordSize, (void *)sos );
    if  ( done < 0 || done % 2 )
	{ LDEB(done); return -1;	}

    recordSize += done/ 2;
    bltArgCount= recordSize;

    bytesWritten += 2* recordSize;
    if  ( maxRecordSize < recordSize )
	{ maxRecordSize=  recordSize;	}

    recordSize= 2+ 1+ 1;
    sioEndianPutLeInt32( recordSize, sos );
    sioEndianPutLeInt16( WINMETA_RestoreDC, sos );
    sioEndianPutLeInt16( -1, sos );

    bytesWritten += 2* recordSize;
    if  ( maxRecordSize < recordSize )
	{ maxRecordSize=  recordSize;	}

    recordSize= 2+ 1;
    sioEndianPutLeInt32( recordSize, sos );
    sioEndianPutLeInt16( 0, sos );

    bytesWritten += 2* recordSize;
    if  ( maxRecordSize < recordSize )
	{ maxRecordSize=  recordSize;	}

    fileSize= bytesWritten- headSize;

    if  ( sioOutSeek( sos, headerOffset ) )
	{ LDEB(headerOffset); return -1; }

    if  ( specialHeader )
	{
	done= bmWmfWriteSpecialHeader( bd, sos );

	if  ( done < 0 )
	    { LDEB(done); return -1;	}
	}

    sioEndianPutLeInt16( 1, sos );		/*  fileType		*/
    sioEndianPutLeInt16( 9, sos );		/*  headerSize		*/
    sioEndianPutLeInt16( 768, sos );		/*  windowsVersion	*/
    sioEndianPutLeInt32( fileSize, sos );	/*  fileSize		*/
    sioEndianPutLeInt16( 0, sos );		/*  objectCount		*/
    sioEndianPutLeInt32( maxRecordSize, sos );	/*  maxRecordSize	*/
    sioEndianPutLeInt16( 0, sos );		/*  parameterCount	*/

    if  ( sioOutSeek( sos, bltArgCountPos ) )
	{ LDEB(headerOffset); return -1; }

    sioEndianPutLeInt32( bltArgCount, sos );

    if  ( sioOutSeek( sos, headSize+ fileSize ) )
	{ LDEB(headerOffset); return -1; }

    return 0;
    }

int bmWmfWriteWmf(		const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos )
    { return bmWmfWriteWmfIntern( bd, buffer, sos, 0 ); }

/************************************************************************/
/*									*/
/*  Can write a bitmap to a WMF file?					*/
/*									*/
/************************************************************************/

int bmCanWriteWmfFile( const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    { return bmCanWriteBmpFile( bd, privateFormat, compressionFactor ); }

int bmWriteWmfFile(	const char *			filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    {
    SimpleOutputStream *	sos;

    sos= sioOutStdioOpen( filename );
    if  ( ! sos )
	{ SDEB(filename); return -1;	}

    if  ( bmWmfWriteWmfIntern( bd, buffer, sos, 1 ) )
	{ SDEB(filename); sioOutClose( sos ); return -1; }

    sioOutClose( sos );

    return 0;
    }

