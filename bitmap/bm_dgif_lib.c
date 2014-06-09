/******************************************************************************
*   "Gif-Lib" - Yet another gif library.				      *
*									      *
* Written by:  Gershon Elber			IBM PC Ver 1.1,	Aug. 1990     *
*******************************************************************************
* The kernel of the GIF Decoding process can be found here.		      *
*******************************************************************************
* History:								      *
* 16 Jun 89 - Version 1.0 by Gershon Elber.				      *
*  3 Sep 90 - Version 1.1 by Gershon Elber (Support for Gif89, Unique names). *
******************************************************************************/

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	"bm_gif_lib.h"

#   include	<sioBlocked.h>
#   include	<sioLzw.h>
#   include	<sioEndian.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Read a palette from the input file.					*/
/*									*/
/************************************************************************/

void bmGifInitGifColorMap(	GifColorMap *	gcm )
    {
    int		i;
    RGB8Color *	rgb8;

    gcm->gcmColorCount= 0;
    gcm->gcmBitsPerPixel= 0;

    rgb8= gcm->gcmColors;
    for ( i = 0; i < 256; rgb8++, i++ )
	{
	rgb8->rgb8Red= 255;
	rgb8->rgb8Green= 255;
	rgb8->rgb8Blue= 255;
	rgb8->rgb8Alpha= 255;
	}

    return;
    }

static int bmGifReadColorMap(	GifFileType *		gft,
				GifColorMap *		gcm,
				int			bitsPerPixel )
    {
    RGB8Color *		rgb8;
    int			i;

    bmGifInitGifColorMap( gcm );

    gcm->gcmBitsPerPixel= bitsPerPixel;
    gcm->gcmColorCount= 1 << bitsPerPixel;

    rgb8= gcm->gcmColors;
    for ( i = 0; i < gcm->gcmColorCount; rgb8++, i++ )
	{
	rgb8->rgb8Red= sioInGetCharacter( gft->gftSis );
	rgb8->rgb8Green= sioInGetCharacter( gft->gftSis );
	rgb8->rgb8Blue= sioInGetCharacter( gft->gftSis );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read the screen descriptor from the input file.			*/
/*  This is the first information that is read from the file by the	*/
/*  open routine.							*/
/*									*/
/*  1)  Opened for reading?						*/
/*  2)  Read screen size.						*/
/*  3)  Read the description of the global color map.			*/
/*  4)  And interpret it.						*/
/*  5)  Read global palette (if any).					*/
/*									*/
/************************************************************************/

static int bmGifReadScreenDescriptor(	GifFileType *	gft )
{
    GifScreenDescriptor *	gsd= &(gft->gftScreenDescriptor);

    int				bitsPerPixel;

    /*  1  */
    if  ( ! gft->gftSis )
	{
	XDEB(gft->gftSis);
	_GifError= D_GIF_ERR_NOT_READABLE;
	return GIF_ERROR;
	}

    bmGifInitGifColorMap( &(gsd->gsdScreenColorMap) );

    /*  2  */
    gsd->gsdScreenWide= sioEndianGetLeInt16( gft->gftSis );
    gsd->gsdScreenHigh= sioEndianGetLeInt16( gft->gftSis );

    /*  3  */
    gsd->gsdPackedFields= sioInGetCharacter( gft->gftSis );
    gsd->gsdScreenBackgroundColor= sioInGetCharacter( gft->gftSis );
    gsd->gsdScreenAspectRatio= sioInGetCharacter( gft->gftSis );

    /*  4  */
    gsd->gsdScreenBitsPerPixel = (((gsd->gsdPackedFields & 0x70) + 1) >> 4) + 1;
    bitsPerPixel= ( gsd->gsdPackedFields & 0x07 ) + 1;

    /*  5  */
    if  ( gsd->gsdPackedFields & 0x80 )
	{
	if  ( bmGifReadColorMap( gft,
			    &(gsd->gsdScreenColorMap), bitsPerPixel ) )
	    { LDEB(bitsPerPixel); return GIF_ERROR;	}
	}

    return GIF_OK;
}

/************************************************************************/
/*									*/
/*  Setup the LZ decompression for this image				*/
/*									*/
/*  3)  Open the blocked input stream. (Push it on the unstructured	*/
/*	one.)								*/
/*  4)  Open the LZW stream (Push it on the blocked one.)		*/
/*									*/
/************************************************************************/

static int DGifSetupDecompress(	GifFileType *	gft )
{
    int				codeSize;

    codeSize= sioInGetCharacter( gft->gftSis );
    if  ( codeSize == EOF )
	{ LDEB(codeSize); return GIF_ERROR;	}

    /*  3  */
    if  ( gft->gftSisBlocked )
	{ XDEB(gft->gftSisBlocked);	}

    gft->gftSisBlocked= sioInBlockedOpen( gft->gftSis );
    if  ( ! gft->gftSisBlocked )
	{ XDEB(gft->gftSisBlocked); return GIF_ERROR;	}

    /*  4  */
    if  ( gft->gftSisLzw )
	{ XDEB(gft->gftSisLzw);	}

    gft->gftSisLzw= sioInLzwGifOpen( gft->gftSisBlocked, codeSize );
    if  ( ! gft->gftSisLzw )
	{ XDEB(gft->gftSisLzw); return GIF_ERROR;	}

    return GIF_OK;
}

static void bmGifCleanupDecompress(	GifFileType *	gft )
    {
    if  ( gft->gftSisLzw )
	{
	if  ( sioInClose( gft->gftSisLzw ) )
	    { XDEB(gft->gftSisLzw);	}

	gft->gftSisLzw= (SimpleInputStream *)0;
	}

    if  ( gft->gftSisBlocked )
	{
	if  ( sioInClose( gft->gftSisBlocked ) )
	    { XDEB(gft->gftSisBlocked);	}

	gft->gftSisBlocked= (SimpleInputStream *)0;
	}

    return;
    }

/******************************************************************************
*   Update a new gif file, given its file handle.                             *
*   Returns GifFileType pointer dynamically allocated which serves as the gif *
*   info record. _GifError is cleared if succesfull.                          *
******************************************************************************/

GifFileType *DGifOpenFileHandle(	SimpleInputStream *	sis )
{
    GifFileType *		gft;

    gft= (GifFileType *)malloc( sizeof(GifFileType) );
    if  ( ! gft )
	{ XDEB(gft); return (GifFileType *)0;	}

    memset( gft, '\0', sizeof(GifFileType));

    strncpy( gft->gftVersionString, GIF87_STAMP, 6 )[6]= '\0';

    gft->gftSis= sis;
    gft->gftSisBlocked= (SimpleInputStream *)0;
    gft->gftSisLzw= (SimpleInputStream *)0;

    /* Lets see if this is a GIF file: */
    if  ( sioInReadBytes( gft->gftSis,
			    (unsigned char *)gft->gftVersionString, 6 ) != 6 )
	{
	LDEB(6);
        _GifError= D_GIF_ERR_READ_FAILED;
        free( gft );
        return (GifFileType *)0;
	}

    if  ( strcmp( gft->gftVersionString, GIF87_STAMP )	&&
	  strcmp( gft->gftVersionString, GIF89_STAMP )	)
	{
	SDEB(gft->gftVersionString);
        _GifError= D_GIF_ERR_NOT_GIF_FILE;
        free( gft );
        return (GifFileType *)0;
	}

    if  ( bmGifReadScreenDescriptor( gft ) == GIF_ERROR )
	{
	LDEB(1);
        free( gft );
        return (GifFileType *)0;
	}

    _GifError = 0;

    return gft;
}

/******************************************************************************
*   This routine should be called before any attemp to read an image.         *
******************************************************************************/

int bmGifGetRecordType(GifFileType *gft, GifRecordType *Type)
{
    unsigned char		c;

    /*  1  */
    if  ( ! gft->gftSis )
	{
	XDEB(gft->gftSis);
	_GifError= D_GIF_ERR_NOT_READABLE;
	return GIF_ERROR;
	}

    c= sioInGetCharacter( gft->gftSis );

    switch( c )
	{
	case ',':
	    *Type= IMAGE_DESC_RECORD_TYPE;
	    break;

	case '!':
	    *Type= EXTENSION_RECORD_TYPE;
	    break;

	case ';':
	    *Type= TERMINATE_RECORD_TYPE;
	    break;

	default:
	    CDEB(c);
	    *Type= UNDEFINED_RECORD_TYPE;
	    _GifError = D_GIF_ERR_WRONG_RECORD;
	    return GIF_ERROR;
	}

    return GIF_OK;
}

/************************************************************************/
/*									*/
/*  Read an image descriptor from the input file.			*/
/*  NOTE that it is assumed the Image desc. header (',') has been read.	*/
/*									*/
/*  1)  Opened for reading?						*/
/*  2)  Read image geometry.						*/
/*  3)  Get image flag bits.						*/
/*  4)  Does the image have its own palette.. If so read it.		*/
/*									*/
/************************************************************************/

int DGifGetImageDesc(	GifFileType *	gft )
{
    int				bitsPerPixel;
    GifByteType			Buf[3];
    GifImageDesc *		gid= &(gft->gftCurrentImageDescriptor);

    /*  1  */
    if  ( ! gft->gftSis )
	{
	XDEB(gft->gftSis);
	_GifError= D_GIF_ERR_NOT_READABLE;
	return GIF_ERROR;
	}

    /*  2  */
    gid->Left= sioEndianGetLeInt16( gft->gftSis );
    gid->Top= sioEndianGetLeInt16( gft->gftSis );
    gid->Width= sioEndianGetLeInt16( gft->gftSis );
    gid->Height= sioEndianGetLeInt16( gft->gftSis );

    /*  3  */
    Buf[0]= sioInGetCharacter( gft->gftSis );

    bitsPerPixel= (Buf[0] & 0x07) + 1;
    gid->Interlace= (Buf[0] & 0x40);

    bmGifInitGifColorMap( &(gid->gidImageColorMap) );

    /*  4  */
    if  ( Buf[0] & 0x80 )
	{
	if  ( bmGifReadColorMap( gft, &(gid->gidImageColorMap),
							    bitsPerPixel ) )
	    { LDEB(bitsPerPixel); return GIF_ERROR;	}
	}

    gft->gftPixelCount= (long)gid->Width *(long)gid->Height;

    DGifSetupDecompress( gft );  /* Reset decompress algorithm parameters. */

    return GIF_OK;
}

/************************************************************************/
/*									*/
/*  Get one full scanned line (Line) of length LineLen from GIF file.	*/
/*									*/
/*  3)  The image is finished.. Close (and drain) to blocked input	*/
/*	stream.								*/
/*									*/
/************************************************************************/

int bmGifGetPixels(	GifFileType *	gft,
			GifPixelType *	Line,
			int		LineLen )
{
    if  ( ! gft->gftSis )
	{
	XDEB(gft->gftSis);
	_GifError= D_GIF_ERR_NOT_READABLE;
	return GIF_ERROR;
	}

    if  ( LineLen > gft->gftPixelCount )
	{
	LLDEB(LineLen,gft->gftPixelCount);
	_GifError = D_GIF_ERR_DATA_TOO_BIG;
	return GIF_ERROR;
	}

    gft->gftPixelCount -= LineLen;

    if  ( sioInReadBytes( gft->gftSisLzw, Line, LineLen ) == LineLen )
	{
	if  ( gft->gftPixelCount == 0 )
	    { bmGifCleanupDecompress( gft );	}

	return GIF_OK;
	}
    else{ LDEB(LineLen); return GIF_ERROR;	}
}

/************************************************************************/
/*									*/
/*  Get an extension block (see GIF manual) from gif file. This		*/
/*  routine only returns the first data block, and			*/
/*  DGifGetExtensionNext shouldbe called after this one until NULL	*/
/*  extension is returned.						*/
/*  The Extension should NOT be freed by the user (not dynamically	*/
/*  allocated).								*/
/*  Note it is assumed the Extension desc. header ('!') has been read.	*/
/*									*/
/************************************************************************/

int DGifGetExtension(	GifFileType *		gft,
			int *			ExtCode,
			GifByteType		Extension[256] )
{
    GifByteType			Buf;
    int				got;

    if  ( ! gft->gftSis )
	{
	XDEB(gft->gftSis);
	_GifError= D_GIF_ERR_NOT_READABLE;
	return GIF_ERROR;
	}

    Buf= sioInGetCharacter( gft->gftSis );

    *ExtCode= Buf;

    if  ( DGifGetExtensionNext( gft, &got, Extension) != GIF_OK )
	{ LDEB(1); return GIF_ERROR;	}
    if  ( ! got )
	{ LDEB(got); return GIF_ERROR;	}

    return GIF_OK;
}

/************************************************************************/
/*									*/
/*  Get a following extension block (see GIF manual) from gif file.	*/
/*  This routine should be called until NULL Extension is returned.	*/
/*  The Extension should NOT be freed by the user (not dynamically	*/
/*  allocated).								*/
/*									*/
/************************************************************************/

int DGifGetExtensionNext(	GifFileType *	gft,
				int *		pGot,
				GifByteType	Extension[256] )
{
    int				byte;

    byte= sioInGetCharacter( gft->gftSis );
    if  ( byte == EOF )
	{
	XDEB(byte);
	_GifError = D_GIF_ERR_READ_FAILED;
	return GIF_ERROR;
	}

    if  ( byte == 0 )
	{ *pGot= 0; return GIF_OK; }

    if  ( sioInReadBytes( gft->gftSis, Extension+ 1, byte ) != byte )
	{
	LDEB(byte);
	_GifError = D_GIF_ERR_READ_FAILED; return GIF_ERROR;
	}
    *pGot= 1;
    Extension[0]= byte;
    return GIF_OK;
}

/************************************************************************/
/*									*/
/*  Close a gif file that was opened for reading: Do the necessary	*/
/*  cleanup.								*/
/*									*/
/************************************************************************/

int DGifCloseFile(GifFileType *gft)
{
    if (gft == NULL) return GIF_ERROR;

    if  ( ! gft->gftSis )
	{
	XDEB(gft->gftSis);
	_GifError= D_GIF_ERR_NOT_READABLE;
	return GIF_ERROR;
	}

    bmGifCleanupDecompress( gft );

    free(gft);

    return GIF_OK;
}

