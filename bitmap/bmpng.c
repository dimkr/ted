#   include	"bitmapConfig.h"

#   include	<stdlib.h>
#   include	"bmintern.h"
#   include	<png.h>
#   include	<appDebugon.h>
#   include	<sioStdio.h>
#   include	<utilEndian.h>

# if 1
#   ifndef png_jmpbuf
    /*  Works with older libs as well */
    # define png_jmpbuf( pngp ) pngp->jmpbuf
#   endif
# endif

/************************************************************************/
/*									*/
/*  Read a bitmap from a PNG file.					*/
/*									*/
/************************************************************************/

static int bpPngiToBitmap(	const png_structp		pngp,
				png_info *			pngi,
				BitmapDescription *		bd )
    {
    unsigned int	col;
    png_uint_32		res_x, res_y;
    int			unit_type= 0;

    bd->bdPixelsWide= png_get_image_width( pngp, pngi );
    bd->bdPixelsHigh= png_get_image_height( pngp, pngi );
    bd->bdHasAlpha= 0;

    switch( png_get_color_type( pngp, pngi ) )
	{
	case PNG_COLOR_TYPE_PALETTE:
	    {
	    int		num_palette;
	    png_colorp	palette;

	    png_get_PLTE( pngp, pngi, &palette, &num_palette );

	    bd->bdColorEncoding= BMcoRGB8PALETTE;
	    bd->bdColorCount= num_palette;
	    bd->bdBitsPerSample= 8;
	    bd->bdSamplesPerPixel= 3;
	    bd->bdBitsPerPixel= png_get_bit_depth( pngp, pngi );

	    bd->bdRGB8Palette= (RGB8Color *)
				malloc( bd->bdColorCount* sizeof(RGB8Color) );

	    if  ( ! bd->bdRGB8Palette )
		{ LLDEB(bd->bdColorCount,bd->bdRGB8Palette); return -1; }

	    for ( col= 0; col < bd->bdColorCount; col++ )
		{
		bd->bdRGB8Palette[col].rgb8Red= palette[col].red;
		bd->bdRGB8Palette[col].rgb8Green= palette[col].green;
		bd->bdRGB8Palette[col].rgb8Blue= palette[col].blue;
		bd->bdRGB8Palette[col].rgb8Alpha= 0;
		}
	    }
	    break;

	case PNG_COLOR_TYPE_RGB:
	    bd->bdColorEncoding= BMcoRGB;
	    bd->bdBitsPerSample= png_get_bit_depth( pngp, pngi );
	    bd->bdSamplesPerPixel= png_get_channels( pngp, pngi );
	    bd->bdBitsPerPixel= bd->bdSamplesPerPixel* bd->bdBitsPerSample;
	    break;

	case PNG_COLOR_TYPE_GRAY:
	    bd->bdColorEncoding= BMcoWHITEBLACK;
	    bd->bdBitsPerSample= png_get_bit_depth( pngp, pngi );
	    bd->bdSamplesPerPixel= png_get_channels( pngp, pngi );
	    bd->bdBitsPerPixel= bd->bdSamplesPerPixel* bd->bdBitsPerSample;
	    break;

	case PNG_COLOR_TYPE_RGB_ALPHA:
	    bd->bdHasAlpha= 1;
	    bd->bdColorEncoding= BMcoRGB;
	    bd->bdBitsPerSample= png_get_bit_depth( pngp, pngi );
	    bd->bdSamplesPerPixel= png_get_channels( pngp, pngi );
	    bd->bdBitsPerPixel= bd->bdSamplesPerPixel* bd->bdBitsPerSample;
	    break;

	case PNG_COLOR_TYPE_GRAY_ALPHA:
	    bd->bdHasAlpha= 1;
	    LDEB(png_get_color_type( pngp, pngi )); return -1;

	default:
	    LDEB(png_get_color_type( pngp, pngi )); return -1;
	}

    bd->bdBytesPerRow= png_get_rowbytes( pngp, pngi );
    bd->bdBufferLength= bd->bdBytesPerRow* bd->bdPixelsHigh;

    if  ( !  png_get_pHYs( pngp, pngi, &res_x, &res_y, &unit_type ) )
	{ unit_type= PNG_RESOLUTION_UNKNOWN;	}

    switch( unit_type )
	{
	case PNG_RESOLUTION_UNKNOWN:
	    bd->bdUnit= BMunPIXEL;
	    bd->bdXResolution= 1;
	    bd->bdYResolution= 1;
	    break;

	case PNG_RESOLUTION_METER:
	    bd->bdUnit= BMunM;
	    bd->bdXResolution= res_x;
	    bd->bdYResolution= res_y;
	    break;

	default:
	    LDEB(unit_type);
	    return -1;
	}

    return 0;
    }

static int bmPngReadContents(	png_info *		pngi,
				png_struct *		png,
				BitmapDescription *	bd,
				unsigned char **	pBuffer )
    {
    int				numberOfPasses;
    int				i;

    unsigned char *		buffer;

    numberOfPasses= 1;
    if  ( png_get_interlace_type( png, pngi ) )
	{ numberOfPasses= png_set_interlace_handling( png ); }

    if  ( png_get_color_type( png, pngi ) == PNG_COLOR_TYPE_RGB	&&
	  png_get_bit_depth( png, pngi ) == 16			)
	{
	const unsigned short	one= 1;
	const unsigned char *	testEndian= (const unsigned char *)&one;

	if  ( testEndian[0] )
	    { png_set_swap( png );	}
	}

    png_start_read_image( png );

    buffer= (unsigned char *)malloc( bd->bdBufferLength );
    if  ( ! buffer )
	{ LLDEB(bd->bdBufferLength,buffer); return -1;	}

    for ( i= 0; i < numberOfPasses; i++ )
	{
	unsigned int		row;

	for ( row= 0; row < bd->bdPixelsHigh; row++ )
	    {
	    unsigned char *	to= buffer+ row* bd->bdBytesPerRow;

	    png_read_rows( png, &to, NULL, 1 );
	    }
	}

    png_read_end( png, pngi );

    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE		&&
	  ! bd->bdHasAlpha					&&
	  ! bmMakeMonochrome( bd, bd->bdRGB8Palette, buffer )	)
	{ bd->bdRGB8Palette= (RGB8Color *)0;	}

    *pBuffer= buffer; return 0;
    }

int bmReadPngFile(	const char *		filename,
			unsigned char **	pBuffer,
			BitmapDescription *	bd,
			int *			pPrivateFormat,
			double *		pCompressionFactor )
    {
    SimpleInputStream *	sis;

    sis= sioInStdioOpen( filename );
    if  ( ! sis )
	{ SXDEB(filename,sis); return -1;	}

    if  ( bmPngReadPng( bd, pBuffer, sis ) )
	{ SDEB(filename); sioInClose( sis ); return -1; }

    sioInClose( sis );

    *pPrivateFormat= 0;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read an image from a PNG input stream.				*/
/*									*/
/************************************************************************/

static void bmReadPngBytes(	png_struct *	png,
				png_byte *	buffer,
				png_size_t	count )
    {
    SimpleInputStream *	sis;

    sis= (SimpleInputStream *)png_get_io_ptr( png );

    sioInReadBytes( sis, (unsigned char *)buffer, count );

    return;
    }

int bmPngReadPng(	BitmapDescription *	bd,
			unsigned char **	pBuffer,
			SimpleInputStream *	sis )
    {
    png_structp		pngp= (png_structp)0;
    png_infop		pngip= (png_infop)0;

    unsigned char *	buffer;

    pngp = png_create_read_struct( PNG_LIBPNG_VER_STRING, (void *)0,
				   (png_error_ptr)0, (png_error_ptr)0 );
    if  ( ! pngp )
	{ LDEB(1); return -1;	}

    pngip = png_create_info_struct( pngp );
    if  ( ! pngip )
	{
	LDEB(1);
	png_destroy_read_struct( &pngp, (png_infop *)0, (png_infop *)0 );
	return -1;
	}

    if  ( setjmp( png_jmpbuf( pngp ) ) )
	{ 
	LDEB(1);
	png_destroy_read_struct( &pngp, &pngip, (png_infop *)0 );
	return -1; 
	}

    png_init_io( pngp, (FILE *)0 );
    png_set_read_fn( pngp, (void *)sis, bmReadPngBytes );

    png_read_info( pngp, pngip );

    if  ( bpPngiToBitmap( pngp, pngip, bd ) )
	{
	LLDEB(bd->bdColorCount,bd->bdRGB8Palette); 
	png_destroy_read_struct( &pngp, &pngip, (png_infop *)0 );
	return -1;
	}

    if  ( bmPngReadContents( pngip, pngp, bd, &buffer ) )
	{
	LLDEB(bd->bdBufferLength,buffer);
	png_destroy_read_struct( &pngp, &pngip, (png_infop *)0 );
	return -1;
	}

    *pBuffer= buffer;

    png_destroy_read_struct( &pngp, &pngip, (png_infop *)0 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Can write a bitmap to a PNG file?					*/
/*  All images can be written to png.					*/
/*									*/
/************************************************************************/

int bmCanWritePngFile( const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    {
    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE	&&
	  bd->bdBitsPerPixel > 8			)
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write a bitmap to a PNG file.					*/
/*									*/
/************************************************************************/

static int bpPngiFromBitmap(	png_structp			png,
				png_info *			pngi,
				png_colorp *			pPalette,
				const BitmapDescription *       bd )
    {
    int			bit_depth;
    int			color_type;
    png_color_8		sig_bit;

    switch( bd->bdUnit )
	{
	case BMunM:
	    png_set_pHYs( png, pngi,
			    bd->bdXResolution,
			    bd->bdYResolution,
			    PNG_RESOLUTION_METER);
	    break;

	case BMunINCH:
	    png_set_pHYs( png, pngi,
			    (int)( 39.37* bd->bdXResolution ),
			    (int)( 39.37* bd->bdYResolution ),
			    PNG_RESOLUTION_METER);
	    break;

	case BMunPOINT:
	    png_set_pHYs( png, pngi,
			    POINTS_PER_M* bd->bdXResolution,
			    POINTS_PER_M* bd->bdYResolution,
			    PNG_RESOLUTION_METER);
	    break;

	case BMunPIXEL:
	    png_set_pHYs(png, pngi, 1, 1, PNG_RESOLUTION_UNKNOWN);
	    break;

	default:
	    LDEB(bd->bdUnit);
	    png_set_pHYs(png, pngi, 1, 1, PNG_RESOLUTION_UNKNOWN);
	    break;
	}

    switch( bd->bdColorEncoding )
	{
	int	i;

	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	    bit_depth= bd->bdBitsPerPixel;
	    if  ( bd->bdHasAlpha )
		{ color_type= PNG_COLOR_TYPE_GRAY_ALPHA;	}
	    else{ color_type= PNG_COLOR_TYPE_GRAY;		}
	    sig_bit.gray= bd->bdBitsPerSample;
	    break;

	case BMcoRGB:
	    bit_depth= bd->bdBitsPerSample;
	    if  ( bd->bdHasAlpha )
		{ color_type= PNG_COLOR_TYPE_RGB_ALPHA;	}
	    else{ color_type= PNG_COLOR_TYPE_RGB;	}
	    sig_bit.red= bd->bdBitsPerSample;
	    sig_bit.green= bd->bdBitsPerSample;
	    sig_bit.blue= bd->bdBitsPerSample;
	    break;

	case BMcoRGB8PALETTE:
	    bit_depth= bd->bdBitsPerPixel;
	    color_type= PNG_COLOR_TYPE_PALETTE;

	    if  ( bd->bdColorCount > PNG_MAX_PALETTE_LENGTH )
		{
		LLDEB(bd->bdColorCount,PNG_MAX_PALETTE_LENGTH);
		return -1;
		}

	    *pPalette= (png_color *)malloc( PNG_MAX_PALETTE_LENGTH*
						    sizeof( png_color ) );
	    if  ( ! *pPalette )
		{ XDEB(*pPalette); return -1;	}
	    sig_bit.red= bd->bdBitsPerSample;
	    sig_bit.green= bd->bdBitsPerSample;
	    sig_bit.blue= bd->bdBitsPerSample;
	    for ( i= 0; i < bd->bdColorCount; i++ )
		{
		(*pPalette)[i].red= bd->bdRGB8Palette[i].rgb8Red;
		(*pPalette)[i].green= bd->bdRGB8Palette[i].rgb8Green;
		(*pPalette)[i].blue= bd->bdRGB8Palette[i].rgb8Blue;
		}

	    png_set_PLTE( png, pngi, (*pPalette), bd->bdColorCount );
	    break;

	default:
	    LDEB(bd->bdColorEncoding);
	    return -1;
	}

    png_set_sBIT( png, pngi, &sig_bit );
    png_set_IHDR( png, pngi,
			bd->bdPixelsWide, bd->bdPixelsHigh,
			bit_depth, color_type,
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE,
			PNG_FILTER_TYPE_BASE );

    return 0;
    }

static void bmPngWriteContents(	png_structp			png,
				png_infop			pngi,
				const unsigned char *		buffer,
				const BitmapDescription *	bd )
    {
    int			row;
    unsigned char *	scratch= (unsigned char *)0;

    if  ( bd->bdColorEncoding == BMcoBLACKWHITE )
	{
	if  ( bd->bdBitsPerPixel == 1 )
	    { png_set_invert_mono( png );	}
	else{
	    scratch= (unsigned char *)malloc( bd->bdBytesPerRow );
	    if  ( ! scratch )
		{ LXDEB(bd->bdBytesPerRow,scratch);	}
	    }
	}

    if  ( png_get_color_type( png, pngi ) == PNG_COLOR_TYPE_RGB	&&
	  bd->bdBitsPerSample == 16				)
	{
	const unsigned short	one= 1;
	const unsigned char *	testEndian= (const unsigned char *)&one;

	/* Does not work!
	if  ( testEndian[0] )
	    { png_set_swap( png );	}
	*/
	if  ( testEndian[0] )
	    {
	    scratch= (unsigned char *)malloc( bd->bdBytesPerRow );
	    if  ( ! scratch )
		{ LXDEB(bd->bdBytesPerRow,scratch);	}
	    }
	}

    png_write_info( png, pngi );

    for ( row= 0; row < bd->bdPixelsHigh; row++ )
	{
	const unsigned char *	from= buffer+ row* bd->bdBytesPerRow;

	if  ( bd->bdColorEncoding == BMcoBLACKWHITE	&&
	      scratch					)
	    {
	    int			col;
	    unsigned char *	to= scratch;

	    for ( col= 0; col < bd->bdBytesPerRow; col++ )
		{ *(to++)= ~*(from++); }

	    from= scratch;
	    }

	if  ( png_get_color_type( png, pngi ) == PNG_COLOR_TYPE_RGB	&&
	      bd->bdBitsPerSample == 16					&&
	      scratch							)
	    {
	    int			col;
	    const BmUint16 *	fr= (const BmUint16 *)from;
	    unsigned char *	to= scratch;

	    for ( col= 0; col < bd->bdBytesPerRow; col += sizeof(BmUint16) )
		{
		utilEndianStoreBeInt16( *(fr++), to );
		to += sizeof(BmUint16);
		}

	    from= scratch;
	    }

	png_write_rows( png, (unsigned char **)&from, 1 );
	}

    png_write_end( png, pngi );

    if  ( scratch )
	{ free( scratch );	}

    return;
    }

int bmWritePngFile(	const char *			filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    {
    SimpleOutputStream *	sos;

    sos= sioOutStdioOpen( filename );
    if  ( ! sos )
	{ SDEB(filename); return -1;	}

    if  ( bmPngWritePng( bd, buffer, sos ) )
	{ SDEB(filename); sioOutClose( sos ); return -1; }

    sioOutClose( sos );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write a bitmap to a png stream.					*/
/*									*/
/************************************************************************/

static void bmPngWriteBytes(	png_struct *	png,
				png_byte *	buffer,
				png_size_t	count )
    {
    SimpleOutputStream *	sos;

    sos= (SimpleOutputStream *)png_get_io_ptr( png );

    if  ( sioOutWriteBytes( sos, (const unsigned char *)buffer, count )
								!= count )
	{ LDEB(count);	}

    return;
    }

static void bmPngFlushBytes(    png_struct *    png )
    { return;	}

int bmPngWritePng(		const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos )
    {
    int			rval= 0;
    png_structp		pngp= (png_structp)0;
    png_infop		pngip= (png_infop)0;
    png_colorp		palette= (png_colorp)0;

    pngp = png_create_write_struct( PNG_LIBPNG_VER_STRING, (void *)0,
				    (png_error_ptr)0, (png_error_ptr)0 );
    if  ( ! pngp )
	{ XDEB(pngp); rval= -1; goto ready;	}

    pngip = png_create_info_struct( pngp );
    if  ( ! pngip )
	{ XDEB(pngip); rval= -1; goto ready;	}

    /*
    As the info struct is built by libpng this is not needed:
    (The call will disappear from libpng in version 1.4)
    png_info_init( pngi ); 
    */

    if  ( setjmp( png_jmpbuf( pngp ) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    png_init_io( pngp, (FILE *)0 );
    png_set_write_fn( pngp, (void *)sos, bmPngWriteBytes, bmPngFlushBytes );

    if  ( bpPngiFromBitmap( pngp, pngip, &palette, bd ) )
	{
	LDEB(bd->bdColorEncoding);
	png_destroy_write_struct( &pngp, &pngip );
	return -1;
	}
    if  ( bpPngiFromBitmap( pngp, pngip, &palette, bd ) )
	{ LDEB(bd->bdColorEncoding); rval= -1; goto ready; }

    bmPngWriteContents( pngp, pngip, buffer, bd );

  ready:

    if  ( palette )
	{ free( palette );	}

    png_destroy_write_struct( &pngp, &pngip );

    return rval;
    }
