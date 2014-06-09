#   include	"bitmapConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<sioStdio.h>
#   include	"bmintern.h"
#   include	<time.h>

#   include	<jpeglib.h>
#   include	<setjmp.h>
#   include	"jerror.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Declare additional routines.					*/
/*									*/
/************************************************************************/

#   define INPUT_BUF_SIZE	4096
#   define OUTPUT_BUF_SIZE	4096

typedef struct my_error_mgr
    {
    struct jpeg_error_mgr pub;	/* "public" fields */
    jmp_buf setjmp_buffer;	/* for return to caller */
    } my_error_mgr;

static void my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_mgr * myerr = (my_error_mgr *) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

typedef struct BmJpegInputSource
    {
    struct jpeg_source_mgr	pub;		/* public fields	*/

    BitmapDescription		bjisBd;
    unsigned char *		bjisBitmapBuffer;
    SimpleInputStream *		bjisSis;
    int				bjisRowsReceived;

    JOCTET *			bjisReadBuffer;
    int				bjisAtBeginOfInput;
    } BmJpegInputSource;

static int read_JPEG_file (	BmJpegInputSource * bjis );

/************************************************************************/

typedef struct BmJpegOutputDestination
    {
    struct jpeg_destination_mgr	pub;		/* public fields	*/

    const BitmapDescription *	bjodBd;
    const unsigned char *	bjodBuffer;
    SimpleOutputStream *	bjodSos;

    unsigned char *		bjodScratchBuffer;
    JOCTET *			bjodWriteBuffer;
    } BmJpegOutputDestination;

typedef struct
    {
    struct jpeg_destination_mgr	pub;		/* public fields	*/
    BmJpegOutputDestination *	bjod;		/* destination stream	*/
    JOCTET *			buffer;		/* start of buffer	*/
    } my_destination_mgr;

typedef my_destination_mgr * my_dest_ptr;

static int write_JPEG_file (	BmJpegOutputDestination *	bjod );

/**/

static void my_error_exit (j_common_ptr cinfo);

/************************************************************************/
/*									*/
/*  Read  a JPEG File.							*/
/*									*/
/************************************************************************/

static void bmJpegStartInputSource(	j_decompress_ptr	cinfo )
    {
    BmJpegInputSource *		bjis= (BmJpegInputSource *)cinfo->src;

    bjis->bjisAtBeginOfInput= TRUE;

    return;
    }

static void bmJpegTerminateInputSource(	j_decompress_ptr	cinfo )
    {
    return;
    }

static boolean bmJpegFillInputBuffer(	j_decompress_ptr	cinfo )
    {
    BmJpegInputSource *		bjis= (BmJpegInputSource *)cinfo->src;
    size_t			nbytes;

    nbytes= sioInReadBytes( bjis->bjisSis,
				bjis->bjisReadBuffer, INPUT_BUF_SIZE );

    if  ( nbytes <= 0 )
	{
	if  ( bjis->bjisAtBeginOfInput )	/* Treat empty input	*/
						/* file as fatal error	*/
	    { ERREXIT( cinfo, JERR_INPUT_EMPTY );	}

	WARNMS(cinfo, JWRN_JPEG_EOF);

	/* Insert a fake EOI marker */
	bjis->bjisReadBuffer[0] = (JOCTET) 0xFF;
	bjis->bjisReadBuffer[1] = (JOCTET) JPEG_EOI;
	nbytes = 2;
	}

    bjis->pub.next_input_byte= bjis->bjisReadBuffer;
    bjis->pub.bytes_in_buffer= nbytes;
    bjis->bjisAtBeginOfInput= FALSE;

    return TRUE;
    }

static void bmJpegSkipInputData(	j_decompress_ptr	cinfo,
					long			num_bytes )
    {
    BmJpegInputSource *		bjis= (BmJpegInputSource *)cinfo->src;

    if  ( num_bytes > 0 )
	{
	while( num_bytes > (long) bjis->pub.bytes_in_buffer )
	    {
	    num_bytes -= (long) bjis->pub.bytes_in_buffer;
	    (void) bmJpegFillInputBuffer( cinfo );
	    }

	bjis->pub.next_input_byte += (size_t) num_bytes;
	bjis->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
    }

/************************************************************************/
/*									*/
/*  Copy file information to the communication data areas		*/
/*									*/
/************************************************************************/

static int bmJpegGetReadParameters( struct jpeg_decompress_struct * cinfo )
    {
    BmJpegInputSource *		bjis= (BmJpegInputSource *)cinfo->src;

    bjis->bjisBd.bdPixelsWide= cinfo->image_width;
    bjis->bjisBd.bdPixelsHigh= cinfo->image_height;
    bjis->bjisBd.bdBitsPerSample= cinfo->data_precision;

    switch( cinfo->density_unit )
	{
	case 0:
	    /* LDEB(cinfo->density_unit); */
	    if  ( cinfo->X_density < 5 || cinfo->Y_density < 5 )
		{
	      pixel_density:
		bjis->bjisBd.bdUnit= BMunPIXEL;
		bjis->bjisBd.bdXResolution= 1;
		bjis->bjisBd.bdYResolution= 1;
		}
	    else{
		bjis->bjisBd.bdUnit= BMunINCH;
		bjis->bjisBd.bdXResolution= cinfo->X_density;
		bjis->bjisBd.bdYResolution= cinfo->Y_density;
		}
	    break;

	case 1:
	    if  (  cinfo->X_density < 5 || cinfo->Y_density < 5 )
		{
		LLLDEB(cinfo->density_unit,cinfo->X_density,cinfo->Y_density);
		goto pixel_density;
		}

	    bjis->bjisBd.bdUnit= BMunINCH;
	    bjis->bjisBd.bdXResolution= cinfo->X_density;
	    bjis->bjisBd.bdYResolution= cinfo->Y_density;
	    break;

	case 2:
	    if  (  cinfo->X_density < 2 || cinfo->Y_density < 2 )
		{
		LLLDEB(cinfo->density_unit,cinfo->X_density,cinfo->Y_density);
		goto pixel_density;
		}

	    bjis->bjisBd.bdUnit= BMunM;
	    bjis->bjisBd.bdXResolution= 100* cinfo->X_density;
	    bjis->bjisBd.bdYResolution= 100* cinfo->Y_density;
	    break;

	default:
	    LDEB(cinfo->density_unit);
	    return -1;
	    break;
	}

    switch( cinfo->out_color_space )
	{
	case JCS_GRAYSCALE:
	    bjis->bjisBd.bdColorEncoding= BMcoWHITEBLACK;
	    bjis->bjisBd.bdSamplesPerPixel= 1;
	    bjis->bjisBd.bdBitsPerPixel= bjis->bjisBd.bdBitsPerSample;
	    break;

	case JCS_RGB:
	    if  ( cinfo->jpeg_color_space == JCS_GRAYSCALE )
		{
		cinfo->out_color_space= cinfo->jpeg_color_space;

		bjis->bjisBd.bdColorEncoding= BMcoWHITEBLACK;
		bjis->bjisBd.bdSamplesPerPixel= 1;
		bjis->bjisBd.bdBitsPerPixel= bjis->bjisBd.bdBitsPerSample;
		}
	    else{
		bjis->bjisBd.bdColorEncoding= BMcoRGB;
		bjis->bjisBd.bdSamplesPerPixel= 3;
		bjis->bjisBd.bdBitsPerPixel= 3* bjis->bjisBd.bdBitsPerSample;
		}
	    break;

	case JCS_UNKNOWN:
	case JCS_YCbCr:
	case JCS_YCCK:
	case JCS_CMYK:
	    LDEB(cinfo->out_color_space);
	    return -1;

	default:
	    LDEB(cinfo->out_color_space);
	    return -1;
	}

    bjis->bjisBd.bdBytesPerRow= (	bjis->bjisBd.bdPixelsWide*
					bjis->bjisBd.bdBitsPerPixel+ 7 )/8;
    bjis->bjisBd.bdBufferLength=	bjis->bjisBd.bdPixelsHigh*
					bjis->bjisBd.bdBytesPerRow;

    bjis->bjisBitmapBuffer= (unsigned char *)
				malloc( bjis->bjisBd.bdBufferLength );
    if  ( ! bjis->bjisBitmapBuffer )
	{
	LLDEB(bjis->bjisBd.bdBufferLength,bjis->bjisBitmapBuffer);
	return -1;
	}

    bjis->bjisRowsReceived= 0;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remember one row of pixels.						*/
/*									*/
/************************************************************************/

static int bmJpegRememberScanline( JSAMPARRAY			pixel_data,
				struct jpeg_decompress_struct * cinfo	)
    {
    BmJpegInputSource *		bjis= (BmJpegInputSource *)cinfo->src;

    unsigned char *		to;
    register JSAMPROW		ptr0;
    long			col;

    switch( cinfo->out_color_space )
	{
	case JCS_GRAYSCALE:
	    switch( bjis->bjisBd.bdBitsPerSample )
		{
		case 8:
		    to= bjis->bjisBitmapBuffer+
			bjis->bjisBd.bdBytesPerRow*
					    (bjis->bjisRowsReceived++);

		    ptr0 = pixel_data[0];

		    for ( col = 0; col < cinfo->image_width; col++ )
			{ *(to++)= GETJSAMPLE(*(ptr0++));	}
		    break;
		default:
		    LDEB(bjis->bjisBd.bdBitsPerSample); 
		    return -1;
		}
	    break;

	case JCS_RGB:
	    switch( bjis->bjisBd.bdBitsPerSample )
		{
		case 8:
		    to= bjis->bjisBitmapBuffer+
			bjis->bjisBd.bdBytesPerRow*
					    (bjis->bjisRowsReceived++);

		    ptr0 = pixel_data[0];

		    for ( col = 0; col < cinfo->image_width; col++ )
			{
			*(to++)= GETJSAMPLE(*(ptr0++));	/* red */
			*(to++)= GETJSAMPLE(*(ptr0++));	/* green */
			*(to++)= GETJSAMPLE(*(ptr0++));	/* blue */
			}
		    break;
		default:
		    LDEB(bjis->bjisBd.bdBitsPerSample); 
		    return -1;
		}
	    break;

	case JCS_UNKNOWN:
	case JCS_YCbCr:
	case JCS_YCCK:
	case JCS_CMYK:
	    LDEB(cinfo->out_color_space);
	    return -1;
	    break;

	default:
	    LDEB(cinfo->out_color_space);
	    return -1;
	    break;
	}

    return 0;
    }

int bmJpegReadJfif(	BitmapDescription *	bd,
			unsigned char **	pBuffer,
			SimpleInputStream *	sis )
    {
    BmJpegInputSource	bjis;

    bmInitDescription( &(bjis.bjisBd) );

    bjis.bjisBitmapBuffer= (unsigned char *)0;
    bjis.bjisSis= (SimpleInputStream *)0;
    bjis.bjisRowsReceived= 0;

    bjis.pub.init_source= bmJpegStartInputSource;
    bjis.pub.fill_input_buffer= bmJpegFillInputBuffer;
    bjis.pub.skip_input_data= bmJpegSkipInputData;
    bjis.pub.resync_to_restart= jpeg_resync_to_restart;
					/* use default method		*/
    bjis.pub.term_source= bmJpegTerminateInputSource;
    bjis.pub.bytes_in_buffer= 0;	/* forces fill_input_buffer	*/
					/* on first read		*/
    bjis.pub.next_input_byte = NULL;	/* until buffer loaded		*/

    bjis.bjisSis= sis;

    if  ( ! read_JPEG_file ( &bjis ) )
	{
	XDEB(sis);
	if  ( bjis.bjisBitmapBuffer )
	    { free( bjis.bjisBitmapBuffer );	}
	bmCleanDescription( &(bjis.bjisBd) );

	return -1;
	}

    *bd= bjis.bjisBd;
    *pBuffer= bjis.bjisBitmapBuffer;

    return 0;
    }

int bmReadJpegFile(	const char *		filename,
			unsigned char **	pBuffer,
			BitmapDescription *	bd,
			int *			pPrivateFormat,
			double *		pCompressionFactor )
    {
    SimpleInputStream *		sis;

    sis= sioInStdioOpen( filename );
    if  ( ! sis )
	{ SXDEB(filename,sis); return -1;	}

    if  ( bmJpegReadJfif( bd, pBuffer, sis ) )
	{ SDEB(filename); sioInClose( sis ); return -1; }

    *pPrivateFormat= 1;

    sioInClose( sis );

    return 0;
    }

static int read_JPEG_file(	BmJpegInputSource * bjis )
    {
    struct jpeg_decompress_struct	cinfo;
    struct my_error_mgr			jerr;

    JSAMPARRAY				buffer;		/* Output row buffer */
    int					row_stride;	/* physical row */
							/* width in output */
							/* buffer */

    cinfo.err= jpeg_std_error( &jerr.pub );
    jerr.pub.error_exit = my_error_exit;

    if  ( setjmp( jerr.setjmp_buffer ) )
	{
	jpeg_destroy_decompress( &cinfo );
	return 0;
	}

    jpeg_create_decompress( &cinfo );

    cinfo.src= (struct jpeg_source_mgr *)bjis;

    bjis->bjisReadBuffer= (JOCTET *)
      (*cinfo.mem->alloc_small) ((j_common_ptr) &cinfo, JPOOL_PERMANENT,
				  INPUT_BUF_SIZE * sizeof(JOCTET));

    (void) jpeg_read_header( &cinfo, TRUE );

    bmJpegGetReadParameters( &cinfo );

    jpeg_start_decompress( &cinfo );

    row_stride= cinfo.output_width * cinfo.output_components;
    buffer= (*cinfo.mem->alloc_sarray)
		    ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1 );

    while( cinfo.output_scanline < cinfo.output_height )
	{
	(void)jpeg_read_scanlines( &cinfo, buffer, 1 );

	bmJpegRememberScanline( buffer, &cinfo );
	}

    (void)jpeg_finish_decompress( &cinfo );

    jpeg_destroy_decompress( &cinfo );

    return 1;
    }

/************************************************************************/
/*									*/
/*  Can the file be written in JPEG format?				*/
/*									*/
/************************************************************************/

int bmCanWriteJpegFile(	const BitmapDescription *	bd,
			int				PrivateFormat,
			double				CompressionFactor )
    {
    if  ( bd->bdBitsPerSample != 8 )
	{ /* LDEB(bd->bdBitsPerSample); */ return -1;	}

		    /* colorspace of input image */
    switch( bd->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	case BMcoRGB:
	case BMcoRGB8PALETTE:
	    break;
	default:
	    /* LDEB(bd->bdColorEncoding); */ return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write JPEG File.							*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Output: derive the contents of the jpeg compressor struct from the	*/
/*  bitmap description.							*/
/*									*/
/************************************************************************/

static int bmJpegFillCompressor(	struct jpeg_compress_struct *	cinfo,
					const BitmapDescription *	bd )
    {
    if  ( bd->bdBitsPerSample != 8 )
	{ LDEB(bd->bdBitsPerSample); return -1;	}

    cinfo->image_width= bd->bdPixelsWide;
    cinfo->image_height= bd->bdPixelsHigh;

    switch( bd->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	    cinfo->input_components= 1;
	    cinfo->in_color_space= JCS_GRAYSCALE;
	    break;

	case BMcoRGB:
	case BMcoRGB8PALETTE:
	    cinfo->input_components= 3;
	    cinfo->in_color_space= JCS_RGB;
	    break;

	default:
	    LDEB(bd->bdColorEncoding);
	    return -1;
	}

    jpeg_set_defaults( cinfo );

    switch( bd->bdUnit )
	{
	case BMunINCH:
	    cinfo->density_unit= 1;
	    cinfo->X_density= bd->bdXResolution;
	    cinfo->Y_density= bd->bdYResolution;
	    break;

	case BMunM:
	    cinfo->density_unit= 2;
	    cinfo->X_density= ( bd->bdXResolution+ 50 )/ 100;
	    cinfo->Y_density= ( bd->bdYResolution+ 50 )/ 100;
	    break;

	case BMunPOINT:
	    cinfo->density_unit= 1;
	    cinfo->X_density= 72* bd->bdXResolution;
	    cinfo->Y_density= 72* bd->bdYResolution;
	    break;

	case BMunPIXEL:
	    break;

	default:
	    LDEB(bd->bdUnit);
	    /************************************/
	    /*  giving nothing works for jpeg.	*/
	    /************************************/
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Output: Handle one row to the jpeg compressor.			*/
/*									*/
/************************************************************************/

static int bmJpegCompressRow(	struct jpeg_compress_struct *	cinfo,
				BmJpegOutputDestination *	bjod )
    {
    JSAMPROW			row_pointer[1];
    const unsigned char *	from;
    unsigned char *		to;
    int				i;
    int				res;

    const BitmapDescription *	bd= bjod->bjodBd;
    const unsigned char *	buffer= bjod->bjodBuffer;

    from= &(buffer[cinfo->next_scanline* bd->bdBytesPerRow]);

    switch( bd->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	    to= bjod->bjodScratchBuffer;

	    if  ( bd->bdHasAlpha )
		{
		for ( i= 0; i < bd->bdPixelsWide; to++, from += 2, i++ )
		    { *to= 255- *from; }
		}
	    else{
		for ( i= 0; i < bd->bdPixelsWide; to++, from++, i++ )
		    { *to= 255- *from; }
		}

	    row_pointer[0]= (JSAMPROW)bjod->bjodScratchBuffer;
	    res= jpeg_write_scanlines( cinfo, row_pointer, 1 );
	    break;

	case BMcoRGB8PALETTE:
	    bmExpandRGB8Palette( bjod->bjodScratchBuffer, from,
				    bd->bdPixelsWide, bd->bdBitsPerPixel,
				    bd->bdRGB8Palette, bd->bdHasAlpha );

	    row_pointer[0]= (JSAMPROW)bjod->bjodScratchBuffer;
	    res= jpeg_write_scanlines( cinfo, row_pointer, 1 );
	    break;

	case BMcoWHITEBLACK:
	    if  ( bd->bdHasAlpha )
		{
		to= bjod->bjodScratchBuffer;

		for ( i= 0; i < bd->bdPixelsWide; i++ )
		    {
		    *(to++)= *(from++);
		    from++;
		    }

		row_pointer[0]= (JSAMPROW)bjod->bjodScratchBuffer;
		res= jpeg_write_scanlines( cinfo, row_pointer, 1 );
		}
	    else{
		row_pointer[0]= (JSAMPROW)from;
		res= jpeg_write_scanlines( cinfo, row_pointer, 1 );
		}
	    break;

	case BMcoRGB:
	    if  ( bd->bdHasAlpha )
		{
		to= bjod->bjodScratchBuffer;

		for ( i= 0; i < bd->bdPixelsWide; i++ )
		    {
		    if  ( from[3] >= 128 )
			{
			*(to++)= *(from++);
			*(to++)= *(from++);
			*(to++)= *(from++);
			from++;
			}
		    else{
			*(to++)= 255;
			*(to++)= 255;
			*(to++)= 255;
			from += 4;
			}
		    }

		row_pointer[0]= (JSAMPROW)bjod->bjodScratchBuffer;
		res= jpeg_write_scanlines( cinfo, row_pointer, 1 );
		}
	    else{
		row_pointer[0]= (JSAMPROW)from;
		res= jpeg_write_scanlines( cinfo, row_pointer, 1 );
		}
	    break;

	default:
	    LDEB(bd->bdColorEncoding); return -1;
	}

    if  ( res != 1 )
	{ LDEB(res); return -1;	}

    return 0;
    }

static void bmJpegInitDestination(	j_compress_ptr	cinfo )
    {
    BmJpegOutputDestination *	bjod= (BmJpegOutputDestination *)cinfo->dest;

    bjod->bjodWriteBuffer = (JOCTET *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
					  OUTPUT_BUF_SIZE * sizeof(JOCTET));

    bjod->pub.next_output_byte= bjod->bjodWriteBuffer;
    bjod->pub.free_in_buffer= OUTPUT_BUF_SIZE;

    return;
    }

static boolean bmJpegWriteOutputBuffer(	j_compress_ptr	cinfo )
    {
    BmJpegOutputDestination *	bjod= (BmJpegOutputDestination *)cinfo->dest;

    if  ( sioOutWriteBytes( bjod->bjodSos, bjod->bjodWriteBuffer,
			    OUTPUT_BUF_SIZE ) != (size_t) OUTPUT_BUF_SIZE )
	{ ERREXIT(cinfo, JERR_FILE_WRITE); }

    bjod->pub.next_output_byte= bjod->bjodWriteBuffer;
    bjod->pub.free_in_buffer= OUTPUT_BUF_SIZE;

    return TRUE;
    }

static void bmJpegFinishDestination(	j_compress_ptr	cinfo )
    {
    BmJpegOutputDestination *	bjod= (BmJpegOutputDestination *)cinfo->dest;
    size_t			datacount;

    datacount= OUTPUT_BUF_SIZE- bjod->pub.free_in_buffer;

    if  ( datacount > 0 )
	{
	if  ( sioOutWriteBytes( bjod->bjodSos, bjod->bjodWriteBuffer,
						    datacount ) != datacount )
	    { ERREXIT( cinfo, JERR_FILE_WRITE );	}
	}

    return;
    }

int bmJpegWriteJfif(	const BitmapDescription *	bd,
			const unsigned char *		buffer,
			SimpleOutputStream *		sos )
    {
    int				rval= 0;
    BmJpegOutputDestination	bjod;

    bjod.bjodBd= bd;
    bjod.bjodBuffer= buffer;
    bjod.bjodSos= sos;
    bjod.bjodScratchBuffer= (unsigned char *)0;

    bjod.pub.init_destination= bmJpegInitDestination;
    bjod.pub.empty_output_buffer= bmJpegWriteOutputBuffer;
    bjod.pub.term_destination= bmJpegFinishDestination;

    switch( bd->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	    bjod.bjodScratchBuffer= malloc( bd->bdBytesPerRow );
	    if  ( ! bjod.bjodScratchBuffer )
		{
		LXDEB(bd->bdBytesPerRow,bjod.bjodScratchBuffer);
		rval= -1; goto ready;
		}
	    break;

	case BMcoRGB8PALETTE:
	    bjod.bjodScratchBuffer= malloc( 3* bd->bdPixelsWide );
	    if  ( ! bjod.bjodScratchBuffer )
		{
		LXDEB(bd->bdBytesPerRow,bjod.bjodScratchBuffer);
		rval= -1; goto ready;
		}
	    break;

	case BMcoRGB:
	    if  ( bd->bdHasAlpha )
		{
		bjod.bjodScratchBuffer= malloc( 3* bd->bdPixelsWide );
		if  ( ! bjod.bjodScratchBuffer )
		    {
		    LXDEB(bd->bdBytesPerRow,bjod.bjodScratchBuffer);
		    rval= -1; goto ready;
		    }
		}
	    break;

	default:
	    break;
	}

    if  ( write_JPEG_file( &bjod ) )
	{ LDEB(1); rval= -1; goto ready; }

  ready:

    if  ( bjod.bjodScratchBuffer )
	{ free( bjod.bjodScratchBuffer );	}

    return rval;
    }

int bmWriteJpegFile(	const char *			filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    {
    SimpleOutputStream *	sos;

    sos= sioOutStdioOpen( filename );
    if  ( ! sos )
	{ SXDEB(filename,sos); return -1;	}

    if  ( bmJpegWriteJfif( bd, buffer, sos ) )
	{ SDEB(filename); sioOutClose( sos ); return -1; }

    sioOutClose( sos );

    return 0;
    }

static int
write_JPEG_file(	BmJpegOutputDestination *	bjod )
    {
    struct jpeg_compress_struct		cinfo;
    struct jpeg_error_mgr		jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

#   if 0
    jpeg_sio_dest( &cinfo, bjod );
#   else
    cinfo.dest= (struct jpeg_destination_mgr *)bjod;
#   endif

    if  ( bmJpegFillCompressor( &cinfo, bjod->bjodBd ) )
	{ LDEB(1); return -1;	}

#   if 0
    jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);
#   endif

    jpeg_start_compress( &cinfo, TRUE );

    while( cinfo.next_scanline < cinfo.image_height )
	{
	if( bmJpegCompressRow( &cinfo, bjod ) )
	  { LDEB(1); return -1;	}
	}

    jpeg_finish_compress( &cinfo );

    jpeg_destroy_compress( &cinfo );

    return 0;
    }

