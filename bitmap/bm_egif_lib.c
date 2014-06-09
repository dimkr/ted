/************************************************************************/
/*									*/
/*  Gif writing basic functionality.					*/
/*									*/
/*  ORIGIN: "Gif-Lib" - Yet another gif library.			*/
/*  Written by:  Gershon Elber, Ver 1.1, Aug. 1990			*/
/*  Version 3.0 by Eric S. Raymond (Full GIF89 support)			*/
/*									*/
/*  Modified to what it is now by Mark de Does				*/
/*									*/
/************************************************************************/

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	"bm_gif_lib.h"

#   include	<appDebugon.h>
#   include	<sioBlocked.h>
#   include	<sioLzw.h>
#   include	<sioEndian.h>

/************************************************************************/
/*									*/
/*  Write a color map to file.						*/
/*									*/
/************************************************************************/

static int bmGifWriteColorMap(	GifFileType *		gft,
				const GifColorMap *	gcm )
    {
    int			i;
    const RGB8Color *	rgb8;
    int			count= 1 << gcm->gcmBitsPerPixel;

    if  ( gcm->gcmColorCount > count )
	{ LLDEB(gcm->gcmColorCount,count);	}

    rgb8= gcm->gcmColors;
    for ( i= 0; i < gcm->gcmColorCount && i < count; rgb8++, i++ )
	{
	sioOutPutCharacter( rgb8->rgb8Red, gft->gftSos );
	sioOutPutCharacter( rgb8->rgb8Green, gft->gftSos );
	sioOutPutCharacter( rgb8->rgb8Blue, gft->gftSos );
	}

    for ( i= gcm->gcmColorCount; i < count; rgb8++, i++ )
	{
	sioOutPutCharacter( 255, gft->gftSos );
	sioOutPutCharacter( 255, gft->gftSos );
	sioOutPutCharacter( 255, gft->gftSos );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Initialize GIF writing to an OutputStream.				*/
/*									*/
/*  _GifError is cleared if succesfull.					*/
/*									*/
/************************************************************************/

GifFileType * EGifOpenFileHandle(	SimpleOutputStream *	sos )
{
    GifFileType *		gft;

    gft= (GifFileType *)malloc(sizeof(GifFileType));
    if  ( ! gft )
	{ XDEB(gft); return (GifFileType *)0;	}

    memset( gft, '\0', sizeof(GifFileType) );

    strncpy( gft->gftVersionString, GIF87_STAMP, 6 )[6]= '\0';

    gft->gftSos= sos;
    gft->gftSosBlocked= (SimpleOutputStream *)0;
    gft->gftSosLzw= (SimpleOutputStream *)0;
    
    _GifError = 0;

    return gft;
}

/******************************************************************************
*   Routine to set current GIF version. All files open for write will be      *
* using this version until next call to this routine. Version consists of     *
* 3 characters as "87a" or "89a". No test is made to validate the version.    *
******************************************************************************/

void bmGifSetVersion(	GifFileType *	gft,
			const char *	version )
{
    if  ( strcmp( version, GIF87_STAMP+ 3 )	&&
	  strcmp( version, GIF89_STAMP+ 3 )	)
	{ SDEB(version); return;	}

    strncpy( gft->gftVersionString+ 3, version, 3 );
}

/************************************************************************/
/*									*/
/*  Write the Screen descriptor to the output.				*/
/*									*/
/*  This routine should be called before any other EGif calls, I.E	*/
/*  immediately after the GIF file openning.				*/
/*									*/
/*  1)  First write the version prefix into the file.			*/
/*  2)  Put the screen descriptor into the file				*/
/*  3)  If we have Global color map - dump it also			*/
/*									*/
/************************************************************************/

int EGifPutScreenDesc(	GifFileType *		gft,
			int			Width,
			int			Height,
			int			bitsPerPixel,
			int			BackGround,
			const GifColorMap *	gcm )
{
    GifScreenDescriptor *	gsd= &(gft->gftScreenDescriptor);

    if  ( ! gft->gftSos )
	{
	XDEB(gft->gftSos);
	_GifError = E_GIF_ERR_NOT_WRITEABLE;
	return GIF_ERROR;
	}

    /*  1  */
    sioOutPutString( gft->gftVersionString, gft->gftSos );

    gsd->gsdScreenWide= Width;
    gsd->gsdScreenHigh= Height;
    gsd->gsdScreenBitsPerPixel= bitsPerPixel;
    gsd->gsdScreenBackgroundColor= BackGround;
    gsd->gsdScreenAspectRatio= 0;

    bmGifInitGifColorMap( &(gsd->gsdScreenColorMap) );

    if  ( gcm )
	{ gsd->gsdScreenColorMap= *gcm;	}

    /*  2  */
    sioEndianPutLeInt16( gsd->gsdScreenWide, gft->gftSos );
    sioEndianPutLeInt16( gsd->gsdScreenHigh, gft->gftSos );

    gsd->gsdPackedFields= 0;
    if  ( gcm && gcm->gcmColorCount > 0 )
	{
	gsd->gsdPackedFields |= 0x80;
	gsd->gsdPackedFields |= gcm->gcmBitsPerPixel -1;
	}
    gsd->gsdPackedFields |= ( bitsPerPixel -1 ) << 4;

    sioOutPutCharacter( gsd->gsdPackedFields, gft->gftSos );
    sioOutPutCharacter( gsd->gsdScreenBackgroundColor, gft->gftSos );
    sioOutPutCharacter( gsd->gsdScreenAspectRatio, gft->gftSos );

    /*  3  */
    if  ( gcm && gcm->gcmColorCount > 0 )
	{
	if  ( bmGifWriteColorMap( gft, &(gsd->gsdScreenColorMap) ) )
	    { XDEB(gcm); return GIF_ERROR;	}
	}

    return GIF_OK;
}

/************************************************************************/
/*									*/
/*  Setup the LZ compression for an image:				*/
/*									*/
/*  The result is that pixels emitted to the image are LZW compressed,	*/
/*  the compressed data is divided in packets and the packets are	*/
/*  emitted over the output stream for the image.			*/
/*									*/
/************************************************************************/

static int bmGifSetupCompress(	GifFileType *		gft,
				int			codeSize )
    {
    /*  4  */
    if  ( gft->gftSosBlocked )
	{ XDEB(gft->gftSosBlocked);	}

    gft->gftSosBlocked= sioOutBlockedOpen( gft->gftSos );
    if  ( ! gft->gftSosBlocked )
	{ XDEB(gft->gftSosBlocked); return -1;	}

    /*  5  */
    if  ( gft->gftSosLzw )
	{ XDEB(gft->gftSosLzw);	}

    gft->gftSosLzw= sioOutLzwGifOpen( gft->gftSosBlocked, codeSize );
    if  ( ! gft->gftSosLzw )
	{ XDEB(gft->gftSosLzw); return -1;	}

    return 0;
    }

static int bmGifCleanupCompress(	GifFileType *	gft )
    {
    if  ( gft->gftSosLzw )
	{
	if  ( sioOutClose( gft->gftSosLzw ) )
	    { LDEB(1); return -1;	}
	gft->gftSosLzw= (SimpleOutputStream *)0;
	}

    if  ( gft->gftSosBlocked )
	{
	if  ( sioOutClose( gft->gftSosBlocked ) )
	    { LDEB(1); return -1;	}
	gft->gftSosBlocked= (SimpleOutputStream *)0;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Emit an image descriptor to file.					*/
/*									*/
/*  This routine should be called after a screen descriptor is emmitted	*/
/*  and before any of the image data is emitted. If the image has	*/
/*  transparent parts, the graphics extension saying so should have	*/
/*  been emitted before.						*/
/*									*/
/*  In general, any extensions and comments relating to an image should	*/
/*  be emitted BEFORE the image.					*/
/*									*/
/*  1)  File writable?							*/
/*  2)  Remember parameters.						*/
/*  3)  Write descriptor to file.					*/
/*  4)  Write the color map to file (If one is given)			*/
/*  5)  Remember how many pixels are to be written.			*/
/*  6)  Find out what color map is relevant and derive LZW initial code	*/
/*	size from the relevant color map.				*/
/*  7)  Emit the code size and initialize the compressor and packet	*/
/*	streams.							*/
/*									*/
/************************************************************************/

int EGifPutImageDesc(	GifFileType *		gft,
			int			Left,
			int			Top,
			int			Width,
			int			Height,
			int			Interlace,
			const GifColorMap *	gcm )
{
    unsigned char		flags;
    GifScreenDescriptor *	gsd= &(gft->gftScreenDescriptor);
    GifImageDesc *		gid= &(gft->gftCurrentImageDescriptor);

    int				codeSize;

    /*  1  */
    if  ( ! gft->gftSos )
	{
	XDEB(gft->gftSos);
	_GifError = E_GIF_ERR_NOT_WRITEABLE;
	return GIF_ERROR;
	}

    /*  2  */
    gid->Left = Left;
    gid->Top = Top;
    gid->Width = Width;
    gid->Height = Height;
    gid->Interlace = Interlace;

    bmGifInitGifColorMap( &(gid->gidImageColorMap) );

    if  ( gcm )
	{ gid->gidImageColorMap= *gcm;	}

    /*  3  */
    sioOutPutCharacter( ',', gft->gftSos );
    sioEndianPutLeInt16( gid->Left, gft->gftSos );
    sioEndianPutLeInt16( gid->Top, gft->gftSos );
    sioEndianPutLeInt16( gid->Width, gft->gftSos );
    sioEndianPutLeInt16( gid->Height, gft->gftSos );

    flags= 0;
    if  ( gcm && gcm->gcmColorCount > 0 )
	{
	flags |= 0x80;
	flags |= gcm->gcmBitsPerPixel - 1;
	}
    if  ( Interlace )
	{
	flags |= 0x40;
	}
    sioOutPutCharacter( flags, gft->gftSos );

    /*  4  */
    if  ( gcm && gcm->gcmColorCount > 0 )
	{
	if  ( bmGifWriteColorMap( gft, gcm ) )
	    { XDEB(gcm); return GIF_ERROR;	}
	}

    /*  5  */
    gft->gftPixelCount= (long) Width * (long) Height;

    /*  6  */
    if  ( gcm && gcm->gcmColorCount > 0 )
	{ codeSize= gcm->gcmBitsPerPixel;	}
    else{
	if  ( gsd->gsdScreenColorMap.gcmColorCount > 0 )
	    { codeSize= gsd->gsdScreenColorMap.gcmBitsPerPixel;	}
	else{
	    if  ( gcm )
		{ LDEB(gcm->gcmColorCount);	}
	    else{ XDEB(gcm);			}
	    LDEB( gsd->gsdScreenColorMap.gcmBitsPerPixel );

	    _GifError = E_GIF_ERR_NO_COLOR_MAP;
	    return GIF_ERROR;
	    }
	}

    /*  7  */
    codeSize= ( codeSize < 2 ? 2 : codeSize );
    sioOutPutCharacter( codeSize, gft->gftSos );

    if  ( bmGifSetupCompress( gft, codeSize ) )
	{ LDEB(1); return GIF_ERROR;	}

    return GIF_OK;
}

/************************************************************************/
/*									*/
/*  Emit a series of pixel values to file.. typically a scanline.	*/
/*									*/
/*  1)  File not writable.. Refuse.					*/
/*  2)  Refuse to exceed the number of pixels in the image.		*/
/*  3)  Subtract the number of pixels from that left in the file.	*/
/*  4)  Actually emit the pixels.					*/
/*  5)  When the image is finished, clean compressor.			*/
/*									*/
/************************************************************************/

int bmGifPutPixels(		GifFileType *		gft,
				const GifPixelType *	Line,
				int			LineLen )
{
    /*  1  */
    if  ( ! gft->gftSos )
	{
	XDEB(gft->gftSos);
	_GifError = E_GIF_ERR_NOT_WRITEABLE;
	return GIF_ERROR;
	}

    /*  2  */
    if  ( gft->gftPixelCount < (unsigned)LineLen )
	{
	LLDEB(gft->gftPixelCount,LineLen);
	_GifError = E_GIF_ERR_DATA_TOO_BIG;
	return GIF_ERROR;
	}

    /*  3  */
    gft->gftPixelCount -= LineLen;

    /*  4  */
    if  ( sioOutWriteBytes( gft->gftSosLzw, Line, LineLen ) != LineLen )
	{
	LDEB(LineLen);
	_GifError = E_GIF_ERR_DISK_IS_FULL; return GIF_ERROR;
	}

    /*  5  */
    if  ( gft->gftPixelCount == 0		&&
	  bmGifCleanupCompress( gft )		)
	{ LDEB(gft->gftPixelCount); return GIF_ERROR;	}

    return GIF_OK;
}

/************************************************************************/
/*									*/
/*  Put a comment into GIF file using the GIF89 comment extension	*/
/*  block.								*/
/*									*/
/************************************************************************/

int EGifPutComment(	GifFileType *	gft,
			const char *	Comment )
{
    return EGifPutExtension(gft, COMMENT_EXT_FUNC_CODE, strlen(Comment),
								Comment);
}

/******************************************************************************
*   Put a first extension block (see GIF manual) into gif file.  Here more    *
* extensions can be dumped using EGifPutExtensionMid until		      *
* EGifPutExtensionLast is invoked.					      *
******************************************************************************/

int EGifPutExtensionFirst(	GifFileType *	gft,
				int		ExtCode,
				int		ExtLen,
				const void *	Extension)
{
    if  ( ! gft->gftSos )
	{
	XDEB(gft->gftSos);
	_GifError = E_GIF_ERR_NOT_WRITEABLE;
	return GIF_ERROR;
	}

    if  ( ExtCode == 0 )
	{
	sioOutPutCharacter( ExtLen, gft->gftSos );
	}
    else{
	sioOutPutCharacter( '!', gft->gftSos );
	sioOutPutCharacter( ExtCode, gft->gftSos );
	sioOutPutCharacter( ExtLen, gft->gftSos );
	}

    if  ( sioOutWriteBytes( gft->gftSos, Extension, ExtLen ) != ExtLen )
	{ LDEB(ExtLen); return GIF_ERROR;	}

    return GIF_OK;
}

/******************************************************************************
*   Put a middle extension block (see GIF manual) into gif file.	      *
******************************************************************************/

int EGifPutExtensionNext(	GifFileType *	gft,
				int		ExtCode,
				int		ExtLen,
				const void *	Extension)
{
    if  ( ! gft->gftSos )
	{
	XDEB(gft->gftSos);
	_GifError = E_GIF_ERR_NOT_WRITEABLE;
	return GIF_ERROR;
	}

    sioOutPutCharacter( ExtLen, gft->gftSos );

    if  ( sioOutWriteBytes( gft->gftSos, Extension, ExtLen ) != ExtLen )
	{ LDEB(ExtLen); return GIF_ERROR;	}

    return GIF_OK;
}

/******************************************************************************
*   Put a last extension block (see GIF manual) into gif file.		      *
******************************************************************************/

int EGifPutExtensionLast(	GifFileType *	gft,
				int		ExtCode,
				int		ExtLen,
				const void *	Extension )
{
    if  ( ! gft->gftSos )
	{
	XDEB(gft->gftSos);
	_GifError = E_GIF_ERR_NOT_WRITEABLE;
	return GIF_ERROR;
	}

    sioOutPutCharacter( ExtLen, gft->gftSos );

    if  ( sioOutWriteBytes( gft->gftSos, Extension, ExtLen ) != ExtLen )
	{ LDEB(ExtLen); return GIF_ERROR;	}

    sioOutPutCharacter( '\0', gft->gftSos );

    return GIF_OK;
}

/******************************************************************************
*   Put an extension block (see GIF manual) into gif file.		      *
******************************************************************************/

int EGifPutExtension(	GifFileType *	gft,
			int		ExtCode,
			int		ExtLen,
			const void *	Extension )
{
    if  ( ! gft->gftSos )
	{
	XDEB(gft->gftSos);
	_GifError = E_GIF_ERR_NOT_WRITEABLE;
	return GIF_ERROR;
	}

    if  ( ExtCode == 0 )
	{
	sioOutPutCharacter( ExtLen, gft->gftSos );
	}
    else{
	sioOutPutCharacter( '!', gft->gftSos );
	sioOutPutCharacter( ExtCode, gft->gftSos );
	sioOutPutCharacter( ExtLen, gft->gftSos );
	}

    if  ( sioOutWriteBytes( gft->gftSos, Extension, ExtLen ) != ExtLen )
	{ LDEB(ExtLen); return GIF_ERROR;	}

    sioOutPutCharacter( '\0', gft->gftSos );

    return GIF_OK;
}

/************************************************************************/
/*									*/
/*  Close a gif file for writing.					*/
/*									*/
/************************************************************************/

int EGifCloseFile(	GifFileType *	gft )
{
    int		rval= GIF_OK;

    if  ( ! gft->gftSos )
	{
	XDEB(gft->gftSos);
	_GifError = E_GIF_ERR_NOT_WRITEABLE;
	rval= GIF_ERROR;
	}

    if  ( gft->gftSos )
	{ sioOutPutCharacter( ';', gft->gftSos );	}

    bmGifCleanupCompress( gft );

    free( gft );

    return rval;
}
