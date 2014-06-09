/************************************************************************/
/*									*/
/*  Open the objects embedded in a document.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<sioMemory.h>
#   include	<sioStdio.h>
#   include	<sioHex.h>
#   include	<sioEndian.h>

#   include	<appImage.h>
#   include	<appWinMeta.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Make the bitmap for an image.					*/
/*  Make a name for an image.						*/
/*									*/
/************************************************************************/

int docGetBitmap(	bmReadBitmap		readBitmap,
			void **			pPrivate,
			const MemoryBuffer *	mb )
    {
    int			res;
    AppBitmapImage *	abi= (AppBitmapImage *)malloc( sizeof(AppBitmapImage) );

    SimpleInputStream *	sisMem;
    SimpleInputStream *	sisBitmap;

    if  ( ! abi )
	{ XDEB(abi); return -1;	}
    appInitBitmapImage( abi );

    sisMem= sioInMemoryOpen( mb );
    if  ( ! sisMem )
	{ XDEB(sisMem); return -1;	}

    sisBitmap= sioInHexOpen( sisMem );
    if  ( ! sisBitmap )
	{ XDEB(sisMem); return -1;	}

    res= (*readBitmap)( &(abi->abiBitmap), &(abi->abiBuffer), sisBitmap );

    sioInClose( sisBitmap );
    sioInClose( sisMem );

    if  ( res < 0 )
	{ LDEB(res); return -1;	}
    if  ( res > 0 )
	{ return 0;		}

    *pPrivate= (void *)abi;

    return 0;
    }

int docGetBitmapForObject(	InsertedObject *	io )
    {
    switch( io->ioKind )
	{
	case DOCokPICTWMETAFILE:
	    if  ( ! io->ioPrivate					&&
		  docGetBitmap( appMetaPlayWmfImg,
				&(io->ioPrivate), &(io->ioObjectData) )	)
		{ LDEB(1); return 0;	}

	    if  ( io->ioPrivate )
		{ io->ioPictureProperties.pipMetafileIsBitmap= 1;	}

	    break;

	case DOCokPICTPNGBLIP:
	    if  ( ! io->ioPrivate					&&
		  docGetBitmap( bmPngReadPng,
				&(io->ioPrivate), &(io->ioObjectData) )	)
		{ LDEB(1); return 0;	}
	    break;

	case DOCokPICTJPEGBLIP:
	    if  ( ! io->ioPrivate					&&
		  docGetBitmap( bmJpegReadJfif,
				&(io->ioPrivate), &(io->ioObjectData) )	)
		{ LDEB(1); return 0;	}

	    break;

	case DOCokOLEOBJECT:
	    if  ( io->ioResultKind == DOCokPICTWMETAFILE )
		{
		if  ( ! io->ioPrivate					&&
		      docGetBitmap( appMetaPlayWmfImg,
				&(io->ioPrivate), &(io->ioResultData) )	)
		    { LDEB(1); return 0;	}

		if  ( io->ioPrivate )
		    { io->ioPictureProperties.pipMetafileIsBitmap= 1;	}

		return 0;
		}
	    return 0;

	case DOCokMACPICT:
	case DOCokPICTEMFBLIP:
	default:
	    LDEB(io->ioKind); return 0;
	}

    return 0;
    }


/************************************************************************/
/*									*/
/*  Read a metafile object from file.					*/
/*									*/
/************************************************************************/

int docReadMetafileObject(	InsertedObject *	io,
				const char *		filename )
    {
    int				rval= 0;

    MemoryBuffer		mb;

    SimpleInputStream *		sisIn= (SimpleInputStream *)0;
    SimpleOutputStream *	sosMem= (SimpleOutputStream *)0;
    SimpleOutputStream *	sosMeta= (SimpleOutputStream *)0;

    PictureProperties *		pip= &(io->ioPictureProperties);

    utilInitMemoryBuffer( &mb );

    sisIn= sioInStdioOpen( filename );
    if  ( ! sisIn )
	{ SXDEB(filename,sisIn); rval= -1; goto ready;	}

    sosMem= sioOutMemoryOpen( &mb );
    if  ( ! sosMem )
	{ XDEB(sosMem); rval= -1; goto ready;	}

    sosMeta= sioOutHexOpen( sosMem );
    if  ( ! sosMeta )
	{ XDEB(sosMeta); rval= -1; goto ready;	}

    {
    unsigned long		key;
    unsigned int		handle;
    int				left;
    int				top;
    int				right;
    int				bottom;
    unsigned int		inch;

    unsigned long		reserved;
    unsigned int		checksum;

    key= sioEndianGetLeUint32( sisIn );

    if  ( key == 0x9ac6cdd7 )
	{
	handle= sioEndianGetLeUint16( sisIn );
	left= sioEndianGetLeInt16( sisIn );
	top= sioEndianGetLeInt16( sisIn );
	right= sioEndianGetLeInt16( sisIn );
	bottom= sioEndianGetLeInt16( sisIn );
	inch= sioEndianGetLeUint16( sisIn );

	reserved= sioEndianGetLeUint32( sisIn );
	checksum= sioEndianGetLeUint16( sisIn );

	if  ( right > left )
	    { pip->pipTwipsWide= ( 20* 72* ( right- left ) )/ inch;	}
	else{ pip->pipTwipsWide= ( 20* 72* ( left- right ) )/ inch;	}

	if  ( bottom > top )
	    { pip->pipTwipsHigh= ( 20* 72* ( bottom- top ) )/ inch;	}
	else{ pip->pipTwipsHigh= ( 20* 72* ( top- bottom ) )/ inch;	}

	io->ioTwipsWide= pip->pipTwipsWide;
	io->ioTwipsHigh= pip->pipTwipsHigh;

	pip->pip_xWinExt= right- left;
	pip->pip_yWinExt= top- bottom;
	}
    else{
	if  ( key == 0x90001 )
	    {
	    pip->pipTwipsWide= 20* 72* 4;		/*  10 cm	*/
	    pip->pipTwipsHigh= 20* 72* 4;		/*  10 cm	*/
	    pip->pip_xWinExt= 72* 4;
	    pip->pip_yWinExt= 72* 4;
	    sioEndianPutLeUint32( key, sosMeta );

	    io->ioTwipsWide= pip->pipTwipsWide;
	    io->ioTwipsHigh= pip->pipTwipsHigh;
	    }
	else{ XDEB(key); rval= -1; goto ready; }
	}
    }

    for (;;)
	{
	unsigned char		buf[4096];
	int			done;

	done= sioInReadBytes( sisIn, buf, 4096 );
	if  ( done < 1 )
	    { break;	}

	if  ( sioOutWriteBytes( sosMeta, buf, done ) != done )
	    { LDEB(done); rval= -1; goto ready;	}
	}

     sioOutClose( sosMeta ); sosMeta= (SimpleOutputStream *)0;
     sioOutClose( sosMem ); sosMem= (SimpleOutputStream *)0;

    io->ioObjectData= mb; utilInitMemoryBuffer( &mb ); /* steal */
    io->ioKind= DOCokPICTWMETAFILE;
    pip->pipType= DOCokPICTWMETAFILE;
    pip->pipMapMode= 8;

  ready:

    utilCleanMemoryBuffer( &mb );

    if  ( sosMeta )
	{ sioOutClose( sosMeta );	}
    if  ( sosMem )
	{ sioOutClose( sosMem );	}
    if  ( sisIn )
	{ sioInClose( sisIn );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Read a bitmap/pixmap image file.					*/
/*									*/
/************************************************************************/

int docReadBitmapObject(	InsertedObject *	io,
				const char *		filename )
    {
    int				rval= 0;
    int				res;
    AppBitmapImage		abi;
    const char *		typeId= "?";
    int				copyAsFile= 0;
    int				includedAsBitmap= 0;

    double			compressionFactor;

    PictureProperties *		pip= &(io->ioPictureProperties);

    SimpleOutputStream *	sosMem= (SimpleOutputStream *)0;
    SimpleOutputStream *	sosHex= (SimpleOutputStream *)0;
    SimpleInputStream *		sisFile= (SimpleInputStream *)0;

    appInitBitmapImage( &abi );

    res= bmRead( filename, &(abi.abiBuffer), &(abi.abiBitmap),
					&(abi.abiFormat), &compressionFactor );
    if  ( res )
	{ SLDEB(filename,res); rval= -1; goto ready;	}

    if  ( abi.abiFormat >= 0 )
	{ typeId= bmFileFormats[abi.abiFormat].bffFileType->bftTypeId; }

    io->ioKind= DOCokPICTPNGBLIP;
    io->ioInline= 1;

    bmImageSizeTwips( &(io->ioTwipsWide), &(io->ioTwipsHigh),
							&(abi.abiBitmap) );

    pip->pipTwipsWide= io->ioTwipsWide;
    pip->pipTwipsHigh= io->ioTwipsHigh;

    io->ioResultData.mbSize= 0;
    sosMem= sioOutMemoryOpen( &(io->ioObjectData) );
    if  ( ! sosMem )
	{ XDEB(sosMem); rval= -1; goto ready;	}

    sosHex= sioOutHexOpen( sosMem );
    if  ( ! sosHex )
	{ XDEB(sosHex); sioOutClose( sosMem ); rval= -1; goto ready;	}

    if  ( abi.abiFormat >= 0				&&
	  ( ! strcmp( typeId, "jpgFile" )	||
	    ! strcmp( typeId, "jpegFile" )	)	)
	{
	io->ioKind= DOCokPICTJPEGBLIP;
	pip->pipType= DOCokPICTJPEGBLIP;
	copyAsFile= 1;
	}

    if  ( abi.abiFormat >= 0			&&
	  ! strcmp( typeId, "pngFile" )		)
	{
	io->ioKind= DOCokPICTPNGBLIP;
	pip->pipType= DOCokPICTPNGBLIP;
	copyAsFile= 1;
	}

    if  ( copyAsFile )
	{
	unsigned char		buf[4096];
	int			got;

	sisFile= sioInStdioOpen( filename );
	if  ( ! sisFile )
	    { SXDEB(filename,sisFile); rval= -1; goto ready; }

	for (;;)
	    {
	    got= sioInReadBytes( sisFile, buf, sizeof(buf) );
	    if  ( got < 0 )
		{ LDEB(got); rval= -1; goto ready; }
	    if  ( got <= 0 )
		{ break; }

	    if  ( sioOutWriteBytes( sosHex, buf, got ) < 0 )
		{ LDEB(got); rval= -1; goto ready;	}
	    }

	includedAsBitmap= 1;
	}

    if  ( ! includedAsBitmap					&&
	  ! bmCanWritePngFile(  &(abi.abiBitmap), 1, 0 )	)
	{
	if  ( bmPngWritePng( &(abi.abiBitmap), abi.abiBuffer, sosHex ) )
	    { LDEB(1); rval= -1; goto ready;	}

	io->ioKind= DOCokPICTPNGBLIP;
	pip->pipType= DOCokPICTPNGBLIP;
	includedAsBitmap= 1;
	}

    if  ( ! includedAsBitmap )
	{
	if  ( bmWmfWriteWmf( &(abi.abiBitmap), abi.abiBuffer, sosHex ) )
	    { LDEB(1); rval= -1; goto ready;	}

	pip->pipMetafileIsBitmap= 1;
	pip->pipMetafileBitmapBpp= abi.abiBitmap.bdBitsPerPixel;

	io->ioKind= DOCokPICTWMETAFILE;
	pip->pipType= DOCokPICTWMETAFILE;
	pip->pipMapMode= 8;
	}

  ready:

    appCleanBitmapImage( &abi );

    if  ( sisFile )
	{ sioInClose( sisFile );	}
    if  ( sosHex )
	{ sioOutClose( sosHex );	}
    if  ( sosMem )
	{ sioOutClose( sosMem );	}

    return rval;
    }
