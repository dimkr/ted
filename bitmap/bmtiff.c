#   include	"bitmapConfig.h"

#   include	<stdlib.h>
#   include	"bmintern.h"
#   include	<tiffio.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Read a tiff file.							*/
/*									*/
/*  9)  Allocate an extra byte as the libtiff fax 3 code scribbles	*/
/*	beyonf the end of the buffer.					*/
/*									*/
/************************************************************************/

int bmReadTiffFile(	const char *		filename,
			unsigned char **	pBuffer,
			BitmapDescription *	bd,
			int *			pPrivateFormat,
			double *		pCompressionFactor	)
    {
    int				rval= 0;

    TIFF *			t= (TIFF *)0;
    unsigned int		col;

    unsigned char *		buffer= (unsigned char *)0;
    int				fileFormat;

    unsigned short		photometric;
    unsigned short		bitsPerSample;
    float			resolution;

    unsigned short		unsignedShort;
    unsigned short *		whatKind;
    unsigned long		unsignedLong;

    unsigned short *		redMap= (unsigned short *)0;
    unsigned short *		greenMap= (unsigned short *)0;
    unsigned short *		blueMap= (unsigned short *)0;

    unsigned short		planarConfig= 1;

    t= TIFFOpen( (char *)filename, "r" );
    if  ( ! t )
	{ SXDEB(filename,t); rval= -1; goto ready;	}

    if  ( TIFFGetField( t, TIFFTAG_COMPRESSION, &unsignedShort ) != 1 )
	{ SLDEB(filename,TIFFTAG_COMPRESSION); rval= -1; goto ready;	}
    fileFormat= unsignedShort;

    if  ( TIFFGetField( t, TIFFTAG_IMAGEWIDTH, &unsignedLong ) != 1 )
	{ SLDEB(filename,TIFFTAG_IMAGEWIDTH); rval= -1; goto ready;	}
    bd->bdPixelsWide= unsignedLong;

    if  ( TIFFGetField( t, TIFFTAG_IMAGELENGTH, &unsignedLong ) != 1 )
	{ SLDEB(filename,TIFFTAG_IMAGELENGTH); rval= -1; goto ready;	}
    bd->bdPixelsHigh= unsignedLong;

    if  ( TIFFGetField( t, TIFFTAG_BITSPERSAMPLE, &bitsPerSample ) != 1 )
	{ SLDEB(filename,TIFFTAG_BITSPERSAMPLE); rval= -1; goto ready; }
    if  ( bitsPerSample > 8 )
	{ LDEB(bitsPerSample); return -1;	}
    bd->bdBitsPerSample= bitsPerSample;

    if  ( TIFFGetField( t, TIFFTAG_SAMPLESPERPIXEL, &unsignedShort ) != 1 )
	{ SLDEB(filename,TIFFTAG_SAMPLESPERPIXEL); rval= -1; goto ready; }
    bd->bdSamplesPerPixel= unsignedShort;

    if  ( TIFFGetField( t, TIFFTAG_EXTRASAMPLES, &unsignedShort,
							 &whatKind ) != 1 )
	{ bd->bdHasAlpha= 0; }
    else{
	switch( unsignedShort )
	    {
	    case 0:
		bd->bdHasAlpha= 0;
		break;

	    case 1:
		if  ( whatKind[0] != EXTRASAMPLE_ASSOCALPHA	&&
		      whatKind[0] != EXTRASAMPLE_UNASSALPHA	)
		    { LLDEB(TIFFTAG_EXTRASAMPLES,whatKind[0]); return -1;}
		bd->bdHasAlpha= 1;
		break;

	    default:
		LLDEB(TIFFTAG_EXTRASAMPLES,unsignedShort);
		return -1;
	    }
	}

    if  ( TIFFGetField( t, TIFFTAG_PHOTOMETRIC, &photometric ) != 1 )
	{ SLDEB(filename,TIFFTAG_PHOTOMETRIC); rval= -1; goto ready;	}

    switch( photometric )
	{
	case PHOTOMETRIC_MINISWHITE:
	    bd->bdColorEncoding= BMcoBLACKWHITE;
	    bd->bdBitsPerPixel= bd->bdBitsPerSample* bd->bdSamplesPerPixel;
	    break;

	case PHOTOMETRIC_MINISBLACK:
	    bd->bdColorEncoding= BMcoWHITEBLACK;
	    bd->bdBitsPerPixel= bd->bdBitsPerSample* bd->bdSamplesPerPixel;
	    break;

	case PHOTOMETRIC_RGB:
	    bd->bdColorEncoding= BMcoRGB;
	    bd->bdBitsPerPixel= bd->bdBitsPerSample* bd->bdSamplesPerPixel;
	    break;

	case PHOTOMETRIC_PALETTE:
	    if  ( TIFFGetField( t, TIFFTAG_COLORMAP,
					&redMap, &greenMap, &blueMap ) != 1 )
		{ SLDEB(filename,TIFFTAG_COLORMAP); rval= -1; goto ready; }

	    if  ( bd->bdHasAlpha )
		{
		if  ( bd->bdSamplesPerPixel != 2 )
		    { LDEB(bd->bdSamplesPerPixel); rval= -1; goto ready; }
		}
	    else{
		if  ( bd->bdSamplesPerPixel != 1 )
		    { LDEB(bd->bdSamplesPerPixel); rval= -1; goto ready; }
		}

	    bd->bdColorCount= 1 << bitsPerSample;
	    bd->bdRGB8Palette= (RGB8Color *)
			    malloc( bd->bdColorCount* sizeof(RGB8Color) );

	    if  ( ! bd->bdRGB8Palette )
		{
		LLDEB(bitsPerSample,bd->bdRGB8Palette); 
		rval= -1; goto ready;
		}
	    for ( col= 0; col < bd->bdColorCount; col++ )
		{
		bd->bdRGB8Palette[col].rgb8Red= redMap[col]/256;
		bd->bdRGB8Palette[col].rgb8Green= greenMap[col]/256;
		bd->bdRGB8Palette[col].rgb8Blue= blueMap[col]/256;
		}

	    bd->bdBitsPerSample= 8;
	    bd->bdColorEncoding= BMcoRGB8PALETTE;

	    if  ( bd->bdHasAlpha )
		{
		bd->bdSamplesPerPixel= 4;
		bd->bdBitsPerPixel= 2* bitsPerSample;
		}
	    else{
		bd->bdSamplesPerPixel= 3;
		bd->bdBitsPerPixel=    bitsPerSample;
		}

	    break;
	default:
	    LDEB(photometric); rval= -1; goto ready;
	}

    bd->bdBytesPerRow= ( bd->bdBitsPerPixel* bd->bdPixelsWide + 7 )/ 8;
    bd->bdBufferLength= bd->bdBytesPerRow* bd->bdPixelsHigh;

    if  ( bd->bdSamplesPerPixel != 1 )
	{
	if  ( TIFFGetField( t, TIFFTAG_PLANARCONFIG, &planarConfig ) != 1 )
	    { LDEB(TIFFTAG_PLANARCONFIG); rval= -1; goto ready;	}
	if  ( planarConfig != 1 )
	    {
	    SLLDEB(filename,bd->bdSamplesPerPixel,planarConfig);
	    LLDEB(bd->bdBitsPerSample,bd->bdBitsPerPixel);
	    }
	}

    if  ( TIFFGetField( t, TIFFTAG_RESOLUTIONUNIT, &unsignedShort ) != 1 )
	{
	/* SLDEB(filename,TIFFTAG_RESOLUTIONUNIT); */
	bd->bdUnit= BMunPIXEL;
	bd->bdXResolution= 1;
	bd->bdYResolution= 1;
	}
    else{
	switch( unsignedShort )
	    {
	    case	RESUNIT_INCH:
		bd->bdUnit= BMunINCH;
		if  ( TIFFGetField( t, TIFFTAG_XRESOLUTION, &resolution ) != 1 )
		    { LDEB(TIFFTAG_XRESOLUTION); rval= -1; goto ready; }
		bd->bdXResolution= (int)resolution;
		if  ( TIFFGetField( t, TIFFTAG_YRESOLUTION, &resolution ) != 1 )
		    { LDEB(TIFFTAG_YRESOLUTION); rval= -1; goto ready; }
		bd->bdYResolution= (int)resolution;
		break;
	    case	RESUNIT_CENTIMETER:
		bd->bdUnit= BMunM;
		if  ( TIFFGetField( t, TIFFTAG_XRESOLUTION, &resolution ) != 1 )
		    { LDEB(TIFFTAG_XRESOLUTION); rval= -1; goto ready; }
		bd->bdXResolution= (int)100* resolution;
		if  ( TIFFGetField( t, TIFFTAG_YRESOLUTION, &resolution ) != 1 )
		    { LDEB(TIFFTAG_YRESOLUTION); rval= -1; goto ready; }
		bd->bdYResolution= 100* (int)resolution;
		break;
	    case	RESUNIT_NONE:
		bd->bdUnit= BMunPIXEL;
		if  ( TIFFGetField( t, TIFFTAG_XRESOLUTION, &resolution ) != 1 )
		    { LDEB(TIFFTAG_XRESOLUTION); resolution= 1;	}
		bd->bdXResolution= (int)resolution;
		if  ( TIFFGetField( t, TIFFTAG_YRESOLUTION, &resolution ) != 1 )
		    { LDEB(TIFFTAG_YRESOLUTION); resolution= 1;	}
		bd->bdYResolution= (int)resolution;
		break;
	    default:
		LDEB(unsignedShort); rval= -1; goto ready;
	    }

	if  ( bd->bdXResolution == 0	||
	      bd->bdYResolution == 0	)
	    {
	    LLDEB(bd->bdXResolution,bd->bdYResolution);
	    bd->bdUnit= BMunPIXEL;
	    bd->bdXResolution= 1;
	    bd->bdYResolution= 1;
	    }
	}

    /*  9  */
    buffer= (unsigned char *)malloc( bd->bdBufferLength+ 1 );
    if  ( ! buffer )
	{ XDEB(buffer); rval= -1; goto ready;	}

    if  ( bd->bdBitsPerPixel % 8 == 0 )
	{
	int		offset= 0;
	int		strip= 0;

	while( offset < bd->bdBufferLength )
	    {
	    int		done;

	    done= TIFFReadEncodedStrip( t, strip, buffer+ offset,
						bd->bdBufferLength- offset );
	    if  ( done < 1 )
		{ LDEB(done); free( buffer ); TIFFClose( t ); return -1; }

	    offset += done; strip++;
	    }
	}
    else{
	int	row;

	for ( row= 0; row < bd->bdPixelsHigh; row++ )
	    {
	    if  ( TIFFReadScanline( t, buffer+ row* bd->bdBytesPerRow,
								row, 0 ) < 0 )
		{ LDEB(row); free( buffer ); TIFFClose( t ); return -1; }
	    }
	}

    if  ( planarConfig != 1 )
	{
	unsigned char *		scratch;

	scratch= (unsigned char *)malloc( bd->bdBufferLength+ 1 );
	if  ( ! scratch )
	    { XDEB(scratch); rval= -1; goto ready;	}

	if  ( bmPlanarToChunky( scratch, buffer, bd ) )
	    { LDEB(1); free( scratch ); rval= -1; goto ready;	}

	free( buffer ); buffer= scratch;
	}

    *pBuffer= buffer; buffer= (unsigned char *)0; /* steal */
    *pPrivateFormat= fileFormat;

  ready:
    if  ( t )
	{ TIFFClose( t );	}
    if  ( buffer )
	{ free( buffer );	}

    return rval;
    }


int bmCanWriteTiffFile( const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    {
    switch( privateFormat )
	{
	case COMPRESSION_NONE:
	    break;
	case COMPRESSION_CCITTRLE:
	case COMPRESSION_CCITTFAX3:
	case COMPRESSION_CCITTFAX4:
	    if  ( bd->bdBitsPerPixel == 1 )
		{ break;	}
	    else{ return -1;	}
	case COMPRESSION_LZW:
	    return 0;
	case COMPRESSION_JPEG:
	    if  ( bd->bdBitsPerSample == 8 )
		{ break;	}
	    else{ return -1;	}
	case COMPRESSION_NEXT:
	    return -1;
	case COMPRESSION_CCITTRLEW:
	case COMPRESSION_PACKBITS:
	case COMPRESSION_THUNDERSCAN:
	    if  ( bd->bdBitsPerPixel == 1 )
		{ break;	}
	    else{ return -1;	}
	default:
	    LDEB(privateFormat); return -1;
	}

    switch( bd->bdColorEncoding )
	{
	case BMcoRGB8PALETTE:
	    switch( bd->bdBitsPerSample )
		{
		case 4:
		case 8:
		    break;
		default:
		    /* LDEB(bd->bdBitsPerSample); */ return -1;
		}
	    break;
	default:
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write a bitmap to a TIFF file.					*/
/*									*/
/************************************************************************/

int bmWriteTiffFile(	const char *			filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    {
    TIFF *			t;
    int				colorSpace= PHOTOMETRIC_MINISWHITE; /*bah*/
    unsigned long		rowsPerStrip;
    int				unit, xResolution, yResolution;
    int				row;
    int				i;

    t= TIFFOpen( (char *)filename, "w" );
    if  ( ! t )
	{ XDEB(t); return -1;	}

    if  ( ! TIFFSetField( t, TIFFTAG_COMPRESSION, privateFormat ) )
	{ LLDEB(TIFFTAG_COMPRESSION,privateFormat); return -1; }

    if  ( ! TIFFSetField( t, TIFFTAG_IMAGELENGTH, (long)bd->bdPixelsHigh ) )
	{ LLDEB(TIFFTAG_IMAGELENGTH,(long)bd->bdPixelsHigh); return -1; }

    if  ( ! TIFFSetField( t, TIFFTAG_IMAGEWIDTH, (long)bd->bdPixelsWide ) )
	{ LLDEB(TIFFTAG_IMAGEWIDTH,(long)bd->bdPixelsWide); return -1; }

    switch( bd->bdColorEncoding )
	{
	case BMcoBLACKWHITE:	colorSpace= PHOTOMETRIC_MINISWHITE;
				break;
	case BMcoWHITEBLACK:	colorSpace= PHOTOMETRIC_MINISBLACK;
				break;
	case BMcoRGB:	colorSpace= PHOTOMETRIC_RGB;
				break;
	case BMcoRGB8PALETTE: colorSpace= PHOTOMETRIC_PALETTE;
				break;
	default:
		LDEB(bd->bdColorEncoding); return -1;
	}

    if  ( ! TIFFSetField( t, TIFFTAG_PHOTOMETRIC, colorSpace ) )
	{ LLDEB(TIFFTAG_PHOTOMETRIC,colorSpace); return -1; }

    if  ( colorSpace == PHOTOMETRIC_PALETTE )
	{
	int			colorCountOut;
	int			colorCountIn;
	unsigned short *	redMap;
	unsigned short *	greenMap;
	unsigned short *	blueMap;

	switch( bd->bdBitsPerPixel )
	    {
	    case 1:
		colorCountIn= 2;
		colorCountOut= 16;
		break;

	    case 2:
		colorCountIn= 4;
		colorCountOut= 16;
		break;

	    case 4:
		colorCountIn= 16;
		colorCountOut= 16;
		break;

	    case 8:
		colorCountIn= 256;
		colorCountOut= 256;
		break;

	    case 16:
		if  ( bd->bdHasAlpha )
		    {
		    colorCountIn= 256;
		    colorCountOut= 256;
		    }
		else{
		    LDEB(bd->bdHasAlpha);
		    LLDEB(bd->bdBitsPerPixel,bd->bdBitsPerSample); return -1;
		    }
		break;

	    default:
		LLDEB(bd->bdBitsPerPixel,bd->bdBitsPerSample); return -1;
	    }

	if  ( bd->bdColorCount > colorCountIn )
	    { LLDEB(bd->bdColorCount,colorCountIn); return -1;	}

	/*
	if  ( colorCountIn != colorCountOut )
	    { LLDEB(colorCountIn,colorCountOut); LDEB(bd->bdBitsPerPixel); }
	*/

	redMap=
	    (unsigned short *)malloc( colorCountOut * sizeof(unsigned short) );
	greenMap=
	    (unsigned short *)malloc( colorCountOut * sizeof(unsigned short) );
	blueMap=
	    (unsigned short *)malloc( colorCountOut * sizeof(unsigned short) );

	if  ( ! redMap || ! greenMap || ! blueMap )
	    { LDEB(colorCountOut); return -1; }

	for ( i= 0; i < bd->bdColorCount; i++ )
	    {
	    redMap[i]=	 257* bd->bdRGB8Palette[i].rgb8Red;
	    greenMap[i]= 257* bd->bdRGB8Palette[i].rgb8Green;
	    blueMap[i]=	 257* bd->bdRGB8Palette[i].rgb8Blue;
	    }
	for ( ; i < colorCountOut; i++ )
	    {
	    redMap[i]=	 redMap[i-1];
	    greenMap[i]= greenMap[i-1];
	    blueMap[i]=	 blueMap[i-1];
	    }

	if  ( bd->bdHasAlpha )
	    {
	    if  ( ! TIFFSetField( t, TIFFTAG_BITSPERSAMPLE,
						    bd->bdBitsPerPixel/ 2 ) )
		{ LLDEB(TIFFTAG_BITSPERSAMPLE,bd->bdBitsPerSample); return -1; }
	    if  ( ! TIFFSetField( t, TIFFTAG_SAMPLESPERPIXEL, 2 ) )
		{ LLDEB(TIFFTAG_SAMPLESPERPIXEL,1); TIFFClose(t); return -1; }
	    }
	else{
	    if  ( ! TIFFSetField( t, TIFFTAG_BITSPERSAMPLE,
						    bd->bdBitsPerPixel ) )
		{ LLDEB(TIFFTAG_BITSPERSAMPLE,bd->bdBitsPerSample); return -1; }
	    if  ( ! TIFFSetField( t, TIFFTAG_SAMPLESPERPIXEL, 1 ) )
		{ LLDEB(TIFFTAG_SAMPLESPERPIXEL,1); TIFFClose(t); return -1; }
	    }

	if  ( TIFFSetField( t, TIFFTAG_COLORMAP,
				    redMap, greenMap, blueMap ) != 1 )
	    { LDEB(TIFFTAG_COLORMAP); TIFFClose(t); return -1;	}

	free( redMap ); free( greenMap ); free( blueMap );
	}
    else{
	if  ( ! TIFFSetField( t, TIFFTAG_BITSPERSAMPLE, bd->bdBitsPerSample ) )
	    { LLDEB(TIFFTAG_BITSPERSAMPLE,bd->bdBitsPerSample); return -1; }

	if  ( ! TIFFSetField( t, TIFFTAG_SAMPLESPERPIXEL,
						bd->bdSamplesPerPixel ) )
	    { LLDEB(TIFFTAG_SAMPLESPERPIXEL,bd->bdSamplesPerPixel); return -1; }
	}

    if  ( ! TIFFSetField( t, TIFFTAG_PLANARCONFIG, 1 ) )
	{ LLDEB(TIFFTAG_PLANARCONFIG,1); TIFFClose(t); return -1; }

					/********************************/
					/*  To make PC's happy, make	*/
					/*  strips that are a little	*/
					/*  smaller than 64K.		*/
					/*  JPEG compression works per	*/
					/*  strip. It requires that the	*/
					/*  height of a strip be a	*/
					/*  multiple of 8. An attempt	*/
					/*  is made to combine these	*/
					/*  restrictions.		*/
					/********************************/

    rowsPerStrip= 65000/ bd->bdBytesPerRow;
    if  ( rowsPerStrip % 8 )
	{ rowsPerStrip= 8* ( rowsPerStrip/ 8 );	}
    if  ( rowsPerStrip < 1 )
	{ rowsPerStrip= 1;	}

    if  ( ! TIFFSetField( t, TIFFTAG_ROWSPERSTRIP, rowsPerStrip ) )
	{ LLDEB(TIFFTAG_ROWSPERSTRIP,rowsPerStrip); TIFFClose(t); return -1; }

    if  ( bd->bdHasAlpha )
	{
	unsigned short	value= EXTRASAMPLE_ASSOCALPHA;

	if  ( ! TIFFSetField( t, TIFFTAG_EXTRASAMPLES, 1, &value ) )
	    {
	    LLDEB(colorSpace,bd->bdSamplesPerPixel);
	    LLDEB(TIFFTAG_EXTRASAMPLES,bd->bdHasAlpha); 
	    TIFFClose(t); return -1;
	    }
	}

    switch( bd->bdUnit )
	{
	case	BMunM:
	    unit= RESUNIT_CENTIMETER;
	    xResolution= ( bd->bdXResolution+ 50 )/ 100;
	    yResolution= ( bd->bdYResolution+ 50 )/ 100;
	    break;

	case	BMunINCH:
	    unit= RESUNIT_INCH;
	    xResolution= bd->bdXResolution;
	    yResolution= bd->bdYResolution;
	    break;

	case	BMunPOINT:
	    unit= RESUNIT_INCH;
	    xResolution= bd->bdXResolution* 72;
	    yResolution= bd->bdYResolution* 72;
	    break;

	case BMunPIXEL:
	    unit= RESUNIT_NONE;
	    xResolution= bd->bdXResolution;
	    yResolution= bd->bdYResolution;
	    break;

	default:
	    LDEB(bd->bdUnit);
	    unit= RESUNIT_NONE;
	    xResolution= bd->bdXResolution;
	    yResolution= bd->bdYResolution;
	    break;
	}

    if  ( ! TIFFSetField( t, TIFFTAG_RESOLUTIONUNIT, unit ) )
	{ LLDEB(TIFFTAG_RESOLUTIONUNIT, unit); return -1; }

    if  ( ! TIFFSetField( t, TIFFTAG_XRESOLUTION, (float)xResolution ) )
	{ LLDEB(TIFFTAG_XRESOLUTION, xResolution); return -1; }

    if  ( ! TIFFSetField( t, TIFFTAG_YRESOLUTION, (float)yResolution ) )
	{ LLDEB(TIFFTAG_YRESOLUTION, yResolution); return -1; }

    if  ( ! TIFFSetField( t, TIFFTAG_SOFTWARE, "appFrame:"
		    " Mark de Does,"
		    " mark@mdedoes.com" ) )
	{ LDEB(TIFFTAG_SOFTWARE); return -1; }

    for ( row= 0; row < bd->bdPixelsHigh; row++ )
	{
	if  ( TIFFWriteScanline( t, (char *) buffer+ row* bd->bdBytesPerRow,
								row, 0 ) < 0 )
	    { LDEB(row); return -1; }
	}

    TIFFClose( t );

    return 0;
    }
