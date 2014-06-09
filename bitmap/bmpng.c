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

static int bpPngiToBitmap(	const png_info *		pngi,
				BitmapDescription *		bd )
    {
    unsigned int	col;

    bd->bdPixelsWide= pngi->width;
    bd->bdPixelsHigh= pngi->height;
    bd->bdHasAlpha= 0;

    switch( pngi->color_type )
	{
	case PNG_COLOR_TYPE_PALETTE:
	    bd->bdColorEncoding= BMcoRGB8PALETTE;
	    bd->bdColorCount= pngi->num_palette;
	    bd->bdBitsPerSample= 8;
	    bd->bdSamplesPerPixel= 3;
	    bd->bdBitsPerPixel= pngi->bit_depth;

	    bd->bdRGB8Palette= (RGB8Color *)
				malloc( bd->bdColorCount* sizeof(RGB8Color) );

	    if  ( ! bd->bdRGB8Palette )
		{ LLDEB(bd->bdColorCount,bd->bdRGB8Palette); return -1; }

	    for ( col= 0; col < bd->bdColorCount; col++ )
		{
		bd->bdRGB8Palette[col].rgb8Red= pngi->palette[col].red;
		bd->bdRGB8Palette[col].rgb8Green= pngi->palette[col].green;
		bd->bdRGB8Palette[col].rgb8Blue= pngi->palette[col].blue;
		bd->bdRGB8Palette[col].rgb8Alpha= 0;
		}
	    break;

	case PNG_COLOR_TYPE_RGB:
	    bd->bdColorEncoding= BMcoRGB;
	    bd->bdBitsPerSample= pngi->bit_depth;
	    bd->bdSamplesPerPixel= pngi->channels;
	    bd->bdBitsPerPixel= pngi->pixel_depth;
	    break;

	case PNG_COLOR_TYPE_GRAY:
	    bd->bdColorEncoding= BMcoWHITEBLACK;
	    bd->bdBitsPerSample= pngi->bit_depth;
	    bd->bdSamplesPerPixel= pngi->channels;
	    bd->bdBitsPerPixel= pngi->pixel_depth;
	    break;

	case PNG_COLOR_TYPE_RGB_ALPHA:
	    bd->bdHasAlpha= 1;
	    bd->bdColorEncoding= BMcoRGB;
	    bd->bdBitsPerSample= pngi->bit_depth;
	    bd->bdSamplesPerPixel= pngi->channels;
	    bd->bdBitsPerPixel= pngi->pixel_depth;
	    break;

	case PNG_COLOR_TYPE_GRAY_ALPHA:
	    bd->bdHasAlpha= 1;
	    LDEB(pngi->color_type); return -1;

	default:
	    LDEB(pngi->color_type); return -1;
	}

    bd->bdBytesPerRow= pngi->rowbytes;
    bd->bdBufferLength= bd->bdBytesPerRow* bd->bdPixelsHigh;

    switch( pngi->phys_unit_type )
	{
	case PNG_RESOLUTION_UNKNOWN:
	    bd->bdUnit= BMunPIXEL;
	    bd->bdXResolution= 1;
	    bd->bdYResolution= 1;
	    break;

	case PNG_RESOLUTION_METER:
	    bd->bdUnit= BMunM;
	    bd->bdXResolution= pngi->x_pixels_per_unit;
	    bd->bdYResolution= pngi->y_pixels_per_unit;
	    break;

	default:
	    LDEB(pngi->phys_unit_type);
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
    if  ( pngi->interlace_type )
	{ numberOfPasses= png_set_interlace_handling( png ); }

    if  ( pngi->color_type == PNG_COLOR_TYPE_RGB	&&
	  pngi->bit_depth == 16				)
	{
	const unsigned short	one= 1;
	const unsigned char *	testEndian= (const unsigned char *)&one;

	if  ( testEndian[0] )
	    { png_set_swap( png );	}
	}

    png_start_read_image( png );
    /*
    png_read_update_info( png, pngi );
    */

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

    if  ( bpPngiToBitmap( pngip, bd ) )
	{
	LLLDEB(pngip->color_type,bd->bdColorCount,bd->bdRGB8Palette); 
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

static int bpPngiFromBitmap(	png_info *			pngi,
				const BitmapDescription *       bd )
    {
    png_info_init( pngi );
    pngi->width= bd->bdPixelsWide;
    pngi->height= bd->bdPixelsHigh;

    switch( bd->bdUnit )
	{
	case BMunM:
	    pngi->phys_unit_type= PNG_RESOLUTION_METER;
	    pngi->x_pixels_per_unit= bd->bdXResolution;
	    pngi->y_pixels_per_unit= bd->bdYResolution;
	    pngi->valid |= PNG_INFO_pHYs;
	    break;

	case BMunINCH:
	    pngi->phys_unit_type= PNG_RESOLUTION_METER;
	    pngi->x_pixels_per_unit= (int)( 39.37* bd->bdXResolution );
	    pngi->y_pixels_per_unit= (int)( 39.37* bd->bdYResolution );
	    pngi->valid |= PNG_INFO_pHYs;
	    break;

	case BMunPOINT:
	    pngi->phys_unit_type= PNG_RESOLUTION_METER;
	    pngi->x_pixels_per_unit= POINTS_PER_M* bd->bdXResolution;
	    pngi->y_pixels_per_unit= POINTS_PER_M* bd->bdYResolution;
	    pngi->valid |= PNG_INFO_pHYs;
	    break;

	case BMunPIXEL:
	    pngi->phys_unit_type= PNG_RESOLUTION_UNKNOWN;
	    pngi->x_pixels_per_unit= 1;
	    pngi->y_pixels_per_unit= 1;
	    break;

	default:
	    LDEB(bd->bdUnit);
	    pngi->phys_unit_type= PNG_RESOLUTION_UNKNOWN;
	    break;
	}

    switch( bd->bdColorEncoding )
	{
	int	i;

	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	    pngi->bit_depth= bd->bdBitsPerPixel;
	    if  ( bd->bdHasAlpha )
		{ pngi->color_type= PNG_COLOR_TYPE_GRAY_ALPHA;	}
	    else{ pngi->color_type= PNG_COLOR_TYPE_GRAY;	}
	    pngi->sig_bit.gray= bd->bdBitsPerSample;
	    break;

	case BMcoRGB:
	    pngi->bit_depth= bd->bdBitsPerSample;
	    if  ( bd->bdHasAlpha )
		{ pngi->color_type= PNG_COLOR_TYPE_RGB_ALPHA;	}
	    else{ pngi->color_type= PNG_COLOR_TYPE_RGB;		}
	    pngi->sig_bit.red= bd->bdBitsPerSample;
	    pngi->sig_bit.green= bd->bdBitsPerSample;
	    pngi->sig_bit.blue= bd->bdBitsPerSample;
	    break;

	case BMcoRGB8PALETTE:
	    pngi->bit_depth= bd->bdBitsPerPixel;
	    pngi->color_type= PNG_COLOR_TYPE_PALETTE;

	    pngi->valid |= PNG_INFO_PLTE;
	    pngi->palette= (png_color *)malloc( 256* sizeof( png_color ) );
	    if  ( ! pngi->palette )
		{ XDEB(pngi->palette); return -1;	}
	    pngi->num_palette= bd->bdColorCount;
	    pngi->sig_bit.red= bd->bdBitsPerSample;
	    pngi->sig_bit.green= bd->bdBitsPerSample;
	    pngi->sig_bit.blue= bd->bdBitsPerSample;
	    for ( i= 0; i < pngi->num_palette; i++ )
		{
		pngi->palette[i].red= bd->bdRGB8Palette[i].rgb8Red;
		pngi->palette[i].green= bd->bdRGB8Palette[i].rgb8Green;
		pngi->palette[i].blue= bd->bdRGB8Palette[i].rgb8Blue;
		}
	    break;

	default:
	    LDEB(bd->bdColorEncoding);
	    return -1;
	}

    pngi->interlace_type= 0;

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

    if  ( pngi->color_type == PNG_COLOR_TYPE_RGB	&&
	  bd->bdBitsPerSample == 16			)
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

	if  ( pngi->color_type == PNG_COLOR_TYPE_RGB	&&
	      bd->bdBitsPerSample == 16			&&
	      scratch					)
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

    if  ( bd->bdColorEncoding == BMcoRGB8PALETTE	&&
	  pngi->palette					)
	{ free( pngi->palette );	}

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
    png_structp		pngp= (png_structp)0;
    png_infop		pngip= (png_infop)0;

    pngp = png_create_write_struct( PNG_LIBPNG_VER_STRING, (void *)0,
				    (png_error_ptr)0, (png_error_ptr)0 );
    if  ( ! pngp )
	{ LDEB(1); return -1;	}

    pngip = png_create_info_struct( pngp );
    if  ( ! pngip )
	{
	LDEB(1);
	png_destroy_write_struct( &pngp, (png_infop *)0 );
	return -1;
	}

    if  ( setjmp( png_jmpbuf( pngp ) ) )
	{
	LDEB(1);
	png_destroy_write_struct( &pngp, &pngip );
	/*  Crashes:
	if  ( bd->bdColorEncoding == BMcoRGB8PALETTE	&&
	      pngip->palette				)
	    { free( pngip->palette );	}
	*/
	return -1;
	}

    png_init_io( pngp, (FILE *)0 );
    png_set_write_fn( pngp, (void *)sos, bmPngWriteBytes, bmPngFlushBytes );

    if  ( bpPngiFromBitmap( pngip, bd ) )
	{
	LDEB(bd->bdColorEncoding);
	png_destroy_write_struct( &pngp, &pngip );
	return -1;
	}

    bmPngWriteContents( pngp, pngip, buffer, bd );

    png_destroy_write_struct( &pngp, &pngip );
    return 0;
    }
