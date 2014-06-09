#   include	"bitmapConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<sioStdio.h>

#   include	"bmintern.h"
#   include	<appDebugon.h>

#   include	"bm_gif_lib.h"

int _GifError = 0;

/************************************************************************/
/*									*/
/*  Read a GIF file.							*/
/*									*/
/************************************************************************/

int bmGifReadGif(		BitmapDescription *	bd,
				unsigned char **	pBuffer,
				SimpleInputStream *	sis )
    {
    GifFileType *	gft;
    GifImageDesc *	gid;
    int			transparent= -1;
    int			gotImage= 0;

    unsigned char *	buffer= (unsigned char *)0;

    gft= DGifOpenFileHandle( sis );
    if  ( ! gft )
	{ XDEB(gft); return -1;	}

    gid= &(gft->gftCurrentImageDescriptor);

    for (;;)
	{
	GifRecordType		grt;
	unsigned int		row;
	int			bpr;
	int			wide;

	int			cc;

	GifColorMap *		gcm;
	const RGB8Color *	rgb8From;
	RGB8Color *		rgb8To;

	if  ( bmGifGetRecordType( gft, &grt ) != GIF_OK )
	    { LDEB(1); DGifCloseFile( gft ); return -1;	}

	switch( grt )
	    {
	    case UNDEFINED_RECORD_TYPE:
		LDEB(grt); continue;

	    case SCREEN_DESC_RECORD_TYPE:
		LDEB(grt); continue;

	    case IMAGE_DESC_RECORD_TYPE:
		if  ( gotImage )
		    { LDEB(gotImage); break;	}
		gotImage= 1;

		if  ( DGifGetImageDesc( gft ) != GIF_OK )
		    { LDEB(1); DGifCloseFile( gft ); return -1; }
		bd->bdPixelsWide= gid->Width;
		bd->bdPixelsHigh= gid->Height;
		if  ( transparent >= 0 )
		    { bd->bdHasAlpha= 1;	}
		else{ bd->bdHasAlpha= 0;	}
		bd->bdUnit= BMunINCH;
		bd->bdXResolution= 72;
		bd->bdYResolution= 72;

		bd->bdBitsPerSample= 8;
		if  ( bd->bdHasAlpha )
		    { bd->bdBitsPerPixel= 16;	}
		else{ bd->bdBitsPerPixel= 8;	}
		bd->bdColorEncoding= BMcoRGB8PALETTE;
		bd->bdColorCount= 256;
		bd->bdRGB8Palette= (RGB8Color *)0;

		if  ( bmCalculateSizes( bd ) )
		    { LDEB(1); return -1;	}

		buffer= (unsigned char *)malloc( bd->bdBufferLength );
		if  ( ! buffer )
		    {
		    LLDEB(bd->bdBufferLength,buffer);
		    DGifCloseFile( gft ); return -1;
		    }

		bd->bdRGB8Palette= (RGB8Color *)
					malloc( 256* sizeof( RGB8Color ) );
		if  ( ! bd->bdRGB8Palette )
		    {
		    LLDEB(256,bd->bdRGB8Palette);
		    free( buffer ); DGifCloseFile( gft ); return -1;
		    }

		wide= bd->bdPixelsWide;
		bpr= bd->bdBytesPerRow;
		if  ( gid->Interlace )
		    {
		    for ( row= 0; row < bd->bdPixelsHigh; row += 8 )
			{
			if  ( bmGifGetPixels( gft, buffer+ row* bpr, wide ) !=
								    GIF_OK )
			    { LDEB(row); DGifCloseFile( gft ); return -1; }
			}
		    for ( row= 4; row < bd->bdPixelsHigh; row += 8 )
			{
			if  ( bmGifGetPixels( gft, buffer+ row* bpr, wide ) !=
								    GIF_OK )
			    { LDEB(row); DGifCloseFile( gft ); return -1; }
			}
		    for ( row= 2; row < bd->bdPixelsHigh; row += 4 )
			{
			if  ( bmGifGetPixels( gft, buffer+ row* bpr, wide ) !=
								    GIF_OK )
			    { LDEB(row); DGifCloseFile( gft ); return -1; }
			}
		    for ( row= 1; row < bd->bdPixelsHigh; row += 2 )
			{
			if  ( bmGifGetPixels( gft, buffer+ row* bpr, wide ) !=
								    GIF_OK )
			    { LDEB(row); DGifCloseFile( gft ); return -1; }
			}
		    }
		else{
		    for ( row= 0; row < bd->bdPixelsHigh; row++ )
			{
			if  ( bmGifGetPixels( gft, buffer+ row* bpr, wide ) !=
								    GIF_OK )
			    { LDEB(row); DGifCloseFile( gft ); return -1; }
			}
		    }

		if  ( gid->gidImageColorMap.gcmColorCount > 0 )
		    { gcm= &(gid->gidImageColorMap);		  }
		else{ gcm= &(gft->gftScreenDescriptor.gsdScreenColorMap); }

		bd->bdColorCount= gcm->gcmColorCount;

		rgb8From= gcm->gcmColors;
		rgb8To= bd->bdRGB8Palette;
		for ( row= 0; row < bd->bdColorCount;
					    rgb8From++, rgb8To++, row++ )
		    {
		    rgb8To->rgb8Red= rgb8From->rgb8Red;
		    rgb8To->rgb8Green= rgb8From->rgb8Green;
		    rgb8To->rgb8Blue= rgb8From->rgb8Blue;
		    rgb8To->rgb8Alpha= 255* ( row != transparent );
		    }

		cc= 0;
		if  ( bd->bdHasAlpha )
		    {
		    for ( row= 0; row < bd->bdPixelsHigh; row++ )
			{
			unsigned char *	to=   buffer+ row* bpr+ bpr-  1;
			unsigned char *	from= buffer+ row* bpr+ wide- 1;
			int		col;

			for ( col= 0; col < bd->bdPixelsWide; col++ )
			    {
			    if  ( *from == transparent )
				{ *(to--)= 0x00; }
			    else{ *(to--)= 0xff; }

			    if  ( *from >= cc )
				{ cc= *from+ 1;	}

			    *(to--)= *(from--);
			    }
			}
		    }
		else{
		    for ( row= 0; row < bd->bdPixelsHigh; row++ )
			{
			unsigned char *	from= buffer+ row* bpr+ bpr-  1;
			int		col;

			for ( col= 0; col < bd->bdPixelsWide; col++ )
			    {
			    if  ( *from >= cc )
				{ cc= *from+ 1;	}

			    from--;
			    }
			}
		    }

		bd->bdColorCount= cc;

		continue;

	    case EXTENSION_RECORD_TYPE:
		{
		int		extensionType;
		unsigned char	extension[256];
		int		got= 1;

		if  ( DGifGetExtension( gft, &extensionType, extension )
								    != GIF_OK )
		    { LDEB(1); DGifCloseFile( gft ); return -1; }

		switch( extensionType )
		    {
		    case GRAPHICS_EXT_FUNC_CODE:
			if  ( extension[1] & 0x1 )
			    { transparent= extension[4];	}
			break;

		    case COMMENT_EXT_FUNC_CODE:
			XDEB(extensionType);
			appDebug( "Comment=\"%.*s\"\n",
						extension[0], extension+ 1 );
			break;

		    case PLAINTEXT_EXT_FUNC_CODE:
			XDEB(extensionType); break;

		    case APPLICATION_EXT_FUNC_CODE:
			XDEB(extensionType);
			appDebug( "Application=\"%.8s\"\n", extension+ 1 );
			break;

		    default:
			XDEB(extensionType); break;
		    }

		while( got )
		    {
		    if  ( DGifGetExtensionNext( gft, &got, extension )
								!= GIF_OK )
			{ LDEB(1); DGifCloseFile( gft ); return -1; }
		    }
		}
		continue;
	    case TERMINATE_RECORD_TYPE:
		break;
	    }

	break;
	}

    DGifCloseFile( gft );

    if  ( ! bd->bdHasAlpha					&&
	  ! bmMakeMonochrome( bd, bd->bdRGB8Palette, buffer )	)
	{ bd->bdRGB8Palette= (RGB8Color *)0;	}

    /*
    *pPrivateFormat= privateFormat;
    */
    *pBuffer= buffer;

    return 0;
    }

int bmReadGifFile(	const char *		filename,
			unsigned char **	pBuffer,
			BitmapDescription *	bd,
			int *			pPrivateFormat,
			double *		pCompressionFactor )
    {
    SimpleInputStream *	sis;

    sis= sioInStdioOpen( filename );
    if  ( ! sis )
	{ SXDEB(filename,sis); return -1;	}

    if  ( bmGifReadGif( bd, pBuffer, sis ) )
	{ SDEB(filename); sioInClose( sis ); return -1;	}

    *pPrivateFormat= 87;
    
    sioInClose( sis );
    return 0;
    }

/************************************************************************/
/*									*/
/*  Write a GIF file.							*/
/*									*/
/************************************************************************/

int bmGifWriteGif(		const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos )
    {
    GifFileType *		gft;

    GifColorMap			gcm;

    const int			left= 0;
    const int			top= 0;
    const int			interlace= 0;

    unsigned int		row;
    const unsigned char *	from;

    int				bpcolor;

    int				transparentColor= -1;
    int				backgroundColor= 0;

    gft= EGifOpenFileHandle( sos );
    if  ( ! gft )
	{ XDEB(gft); return -1;	}

    bmGifInitGifColorMap( &gcm );

    /*  1  */
    switch( bd->bdColorEncoding )
	{
	case BMcoWHITEBLACK:
	case BMcoBLACKWHITE:
	    if  ( bmMakeGrayPalette( bd, &(gcm.gcmColorCount), gcm.gcmColors ) )
		{ LDEB(bd->bdBitsPerPixel); return -1;	}

	    gcm.gcmBitsPerPixel= bd->bdBitsPerPixel;

	    break;

	case BMcoRGB8PALETTE:
	    bpcolor= 1;
	    while( ( 1 << bpcolor ) < bd->bdColorCount )
		{ bpcolor++;	}

	    if  ( bpcolor > 8 )
		{
		LDEB(bpcolor);
		LLDEB(bd->bdColorEncoding,bd->bdBitsPerPixel);
		return -1;
		}

	    gcm.gcmBitsPerPixel= bpcolor;
	    gcm.gcmColorCount= 1 << gcm.gcmBitsPerPixel;

	    for ( row= 0; row < bd->bdColorCount; row++ )
		{
		gcm.gcmColors[row]= bd->bdRGB8Palette[row];

		if  ( transparentColor < 0			&&
		      bd->bdRGB8Palette[row].rgb8Alpha == 0	)
		    { transparentColor= backgroundColor= row;	}
		}

	    if  ( bd->bdHasAlpha && transparentColor < 0 )
		{
		transparentColor= bd->bdColorCount;

		if  ( gcm.gcmBitsPerPixel < 8			&&
		      transparentColor >= gcm.gcmColorCount	)
		    {
		    gcm.gcmBitsPerPixel++;
		    gcm.gcmColorCount *= 2;
		    }

		if  ( transparentColor >= gcm.gcmColorCount )
		    {
		    LDEB(transparentColor);
		    transparentColor= backgroundColor= gcm.gcmColorCount- 1;
		    }
		}

	    break;

	case BMcoRGB:
	    switch( bd->bdBitsPerPixel )
		{
		case 4:
		case 8:
		default:
		    LLDEB(bd->bdColorEncoding,bd->bdBitsPerPixel);
		    return -1;
		}

	default:
	    LLDEB(bd->bdColorEncoding,bd->bdBitsPerPixel);
	    return -1;
	}

    if  ( bd->bdHasAlpha )
	{ bmGifSetVersion( gft, "89a" );	}
    else{ bmGifSetVersion( gft, "87a" );	}

    if  ( EGifPutScreenDesc( gft, bd->bdPixelsWide, bd->bdPixelsHigh,
				    gcm.gcmBitsPerPixel, 0, &gcm ) != GIF_OK )
	{ LDEB(1); EGifCloseFile( gft ); return -1;	}

    if  ( bd->bdHasAlpha )
	{
	unsigned char	extension[4];

	extension[0]= 0x01;
	extension[1]= 0;
	extension[2]= 0;
	extension[3]= transparentColor;

	if  ( EGifPutExtension( gft, GRAPHICS_EXT_FUNC_CODE,
				sizeof(extension), extension ) != GIF_OK )
	    { LDEB(1); EGifCloseFile( gft ); return -1;	}
	}

    if  ( EGifPutImageDesc( gft, left, top,
				bd->bdPixelsWide, bd->bdPixelsHigh,
						interlace, &gcm ) != GIF_OK )
	{ LDEB(1); EGifCloseFile( gft ); return -1;	}

    if  ( bd->bdBitsPerPixel == 8 && ! bd->bdHasAlpha )
	{
	from= buffer;
	for ( row= 0; row < bd->bdPixelsHigh; row++ )
	    {
	    if  ( bmGifPutPixels( gft, (unsigned char *)from,
						bd->bdPixelsWide ) != GIF_OK )
		{ LDEB(row); EGifCloseFile( gft ); return -1; }

	    from += bd->bdBytesPerRow;
	    }
	}
    else{
	unsigned char *		line;

	line= (unsigned char *)malloc( bd->bdPixelsWide+ 7 );
	if  ( ! line )
	    { XDEB(line); EGifCloseFile( gft ); return -1; }

	switch( bd->bdBitsPerPixel )
	    {
	    case 1: case 2: case 4: case 8: case 16:
		for ( row= 0; row < bd->bdPixelsHigh; row++ )
		    {
		    from= buffer+ row* bd->bdBytesPerRow;

		    bmInflateTo8bit( line, from, bd,
					    transparentColor, bd->bdHasAlpha );

		    if  ( bmGifPutPixels( gft, line, bd->bdPixelsWide )
								    != GIF_OK )
			{
			LDEB(row);
			EGifCloseFile( gft ); free( line );
			return -1;
			}
		    }
		break;

	    default:
		LDEB(bd->bdBitsPerPixel);
		EGifCloseFile( gft ); free( line );
		return -1;
	    }

	free( line );
	}

    EGifCloseFile( gft );

    return 0;
    }

int bmWriteGifFile(	const char *			filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    {
    SimpleOutputStream *	sos;

    if  ( bmCanWriteGifFile( bd, privateFormat, compressionFactor ) )
	{ LDEB(1); return -1;	}

    sos= sioOutStdioOpen( filename );
    if  ( ! sos )
	{ SXDEB(filename,sos); return -1;	}

    if  ( bmGifWriteGif( bd, buffer, sos ) )
	{ SDEB(filename); sioOutClose( sos ); return -1;	}
    
    sioOutClose( sos );
    return 0;
    }

/************************************************************************/
/*  Can this bitmap be written in GIF?					*/
/************************************************************************/

int bmCanWriteGifFile(	const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    {
    if  ( bd->bdBitsPerPixel <= 8 )
	{ return 0;	}

    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE	&&
	  bd->bdHasAlpha				&&
	  bd->bdBitsPerPixel <= 16			)
	{ return 0;	}

    return -1;
    }
