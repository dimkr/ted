#   include	"bitmapConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	"bmintern.h"
#   include	<utilEndian.h>
#   include	<appDebugon.h>

/************************************************************************/
/*  Header of a PCX file.						*/
/************************************************************************/

typedef struct _PcxHeader		/* Offset   Description            */
{
    unsigned char	Id;		/*  00h     Manufacturer ID        */
    unsigned char	Version;	/*  01h     Version                */
    unsigned char	Format;		/*  02h     Encoding Scheme        */
    unsigned char	BitsPixelPlane;	/*  03h     Bits/Pixel/Plane       */
    unsigned short	Xmin;		/*  04h     X Start (upper left)   */
    unsigned short	Ymin;		/*  06h     Y Start (top)          */
    unsigned short	Xmax;		/*  08h     X End (lower right)    */
    unsigned short	Ymax;		/*  0Ah     Y End (bottom)         */
    unsigned short	Hdpi;		/*  0Ch     Horizontal Resolution  */
    unsigned short	Vdpi;		/*  0Eh     Vertical Resolution    */
    unsigned char	EgaPalette[48];	/*  10h     16-Color EGA Palette   */
    unsigned char	Reserved;	/*  40h     Reserved               */
    unsigned char	NumberOfPlanes;	/*  41h     Number of Color Planes */
    unsigned short	BytesLinePlane;	/*  42h     Bytes/Line/Plane       */
    unsigned short	PaletteInfo;	/*  44h     Palette Interpretation */
    unsigned short	HScreenSize;	/*  46h     Horizontal Screen Size */
    unsigned short	VScreenSize;	/*  48h     Vertical Screen Size   */
    unsigned char	Filler[54];	/*  4Ah     Reserved               */
} PCXHEADER;

/************************************************************************/
/*  Read a PCX file header.						*/
/************************************************************************/

static int bmReadPcxFileHeader(	PCXHEADER *	pch,
				FILE *		f	)
    {
    int			i;

    pch->Id= getc( f );
    pch->Version= getc( f );
    pch->Format= getc( f );
    pch->BitsPixelPlane= getc( f );

    pch->Xmin= utilGetLeInt16( f );
    pch->Ymin= utilGetLeInt16( f );
    pch->Xmax= utilGetLeInt16( f );
    pch->Ymax= utilGetLeInt16( f );
    pch->Hdpi= utilGetLeInt16( f );
    pch->Vdpi= utilGetLeInt16( f );

    for ( i= 0; i < 48; i++ )
	{ pch->EgaPalette[i]= getc( f ); }

    pch->Reserved= getc( f );
    pch->NumberOfPlanes= getc( f );

    pch->BytesLinePlane= utilGetLeInt16( f );
    pch->PaletteInfo= utilGetLeInt16( f );
    pch->HScreenSize= utilGetLeInt16( f );
    pch->VScreenSize= utilGetLeInt16( f );

    for ( i= 0; i < 54; i++ )
	{ (void)getc( f ); }

    return 0;
    }

/************************************************************************/
/*  Read a PCX row.							*/
/************************************************************************/
static int bmReadPcxRow(	unsigned char *	to,
				int		width,
				FILE *		f,
				unsigned char *	pValue,
				int *		pCount	)
    {
    int		col= 0;
    int		done= 0;

    int		c;

    unsigned char	value= *pValue;
    int			count= *pCount;

    do  {
	done += count;
	while( count > 0 && col < width )
	    { to[col++]= value; count--; }

	if  ( count > 0 )
	    { done -= count; break; }

	c= getc( f );
	if  ( ( c & 0xc0 ) == 0xc0 )
	    { count= c & 0x3f;	value= getc( f );	}
	else{ count= 1;		value= c;		}

	} while ( col < width );

    *pValue= value; *pCount= count; return done;
    }

/************************************************************************/
/*  Read a PCX file.							*/
/************************************************************************/
int bmReadPcxFile(	const char *		filename,
			unsigned char **	pBuffer,
			BitmapDescription *	bd,
			int *			pPrivateFormat,
			double *		pCompressionFactor	)
    {
    FILE *		f;
    PCXHEADER		pch;

    int			row;
    int			count;
    unsigned char	c;

    unsigned char *	buffer;

    f= fopen( filename, "rb");
    if  ( ! f )
	{ SDEB(filename); return -1;	}

    if  ( bmReadPcxFileHeader( &pch, f ) )
	{ SDEB(filename); fclose( f ); return -1;	}

    if  ( pch.Id != 0xa )
	{ SXDEB(filename,pch.Id); fclose( f ); return -1;	}

    if  ( pch.Format != 1 )
	{ SXDEB(filename,pch.Format); fclose( f ); return -1;	}

    bd->bdPixelsWide= pch.Xmax- pch.Xmin+ 1;
    bd->bdPixelsHigh= pch.Ymax- pch.Ymin+ 1;

    if  ( 0 && pch.Hdpi != 0 && pch.Vdpi != 0 )
	{
	bd->bdUnit= BMunINCH;
	bd->bdXResolution= pch.Hdpi;
	bd->bdYResolution= pch.Vdpi;
	}
    else{
	bd->bdUnit= BMunPIXEL;
	bd->bdXResolution= 1;
	bd->bdYResolution= 1;
	}

    bd->bdHasAlpha= 0;

    switch( pch.BitsPixelPlane )
	{
	case 1:
	    switch( pch.NumberOfPlanes )
		{
		case 1:
		    bd->bdBitsPerSample= 1;
		    bd->bdSamplesPerPixel= 1;
		    bd->bdBitsPerPixel= 1;
		    bd->bdColorEncoding= BMcoWHITEBLACK;
		    bd->bdBytesPerRow= ( bd->bdPixelsWide+ 7 )/8;
		    bd->bdBufferLength= bd->bdPixelsHigh* bd->bdBytesPerRow;
		    break;
		case 2:
		case 3:
		case 4:
		default:
		    LLDEB(pch.BitsPixelPlane,pch.NumberOfPlanes);
		    SDEB(filename); fclose( f ); return -1;
		}
	    break;
	case 2:
	    switch( pch.NumberOfPlanes )
		{
		case 1:
		case 4:
		default:
		    LLDEB(pch.BitsPixelPlane,pch.NumberOfPlanes);
		    SDEB(filename); fclose( f ); return -1;
		}
	    break;
	case 4:
	    switch( pch.NumberOfPlanes )
		{
		case 1:
		default:
		    LLDEB(pch.BitsPixelPlane,pch.NumberOfPlanes);
		    SDEB(filename); fclose( f ); return -1;
		}
	    break;
	case 8:
	    switch( pch.NumberOfPlanes )
		{
		case 1:
		    bd->bdBitsPerSample= 8;
		    bd->bdSamplesPerPixel= 3;
		    bd->bdBitsPerPixel= 8;
		    bd->bdColorEncoding= BMcoRGB8PALETTE;
		    bd->bdColorCount= 256;
		    bd->bdRGB8Palette= (RGB8Color *)0;
		    bd->bdBytesPerRow= bd->bdPixelsWide;
		    bd->bdBufferLength= bd->bdPixelsHigh* bd->bdBytesPerRow;

		    bd->bdRGB8Palette= (RGB8Color *)
					    malloc( 256* sizeof(RGB8Color) );
		    if  ( ! bd->bdRGB8Palette )
			{ XDEB(bd->bdRGB8Palette); fclose(f); return -1; }
		    break;
		case 3:
		default:
		    LLDEB(pch.BitsPixelPlane,pch.NumberOfPlanes);
		    SDEB(filename); fclose( f ); return -1;
		}
	    break;
	default:
	    LLDEB(pch.BitsPixelPlane,pch.NumberOfPlanes);
	    SDEB(filename); fclose( f ); return -1;
	}

    buffer= (unsigned char *)malloc( bd->bdBufferLength+ 1 );
    if  ( ! buffer )
	{ LLDEB(bd->bdBufferLength,buffer); fclose( f ); return -1;	}

    switch( pch.NumberOfPlanes )
	{
	case 1:
	    c= 0; count= 0;

	    for ( row= 0; row < bd->bdPixelsHigh; row++ )
		{
		int	ret;

		ret= bmReadPcxRow( buffer+ row* bd->bdBytesPerRow,
					pch.BytesLinePlane, f, &c, &count );

		if  ( ret != pch.BytesLinePlane )
		    { LLDEB(ret,pch.BytesLinePlane);	}

		if  ( ferror( f ) )
		    { SDEB(filename); return -1;	}
		}
	    break;
	case 2:
	case 3:
	case 4:
	default:
	    LLDEB(pch.BitsPixelPlane,pch.NumberOfPlanes);
	    SDEB(filename); fclose( f ); return -1;
	}

    if  ( pch.BitsPixelPlane == 8 && pch.NumberOfPlanes == 1 )
	{
	if  ( fseek( f, -769, SEEK_END ) < 0 )
	    { LSDEB(-769,filename); fclose(f); return -1; }

	c= getc( f );
	if  ( c != 0xc )
	    { XSDEB(c,filename); fclose(f); return -1; }

	for ( row= 0; row < 256; row++ )
	    {
	    bd->bdRGB8Palette[row].rgb8Red= getc( f );
	    bd->bdRGB8Palette[row].rgb8Green= getc( f );
	    bd->bdRGB8Palette[row].rgb8Blue= getc( f );

	    if  ( ferror( f ) )
		{ SDEB(filename); return -1;	}
	    }
	}

    *pPrivateFormat= 0;
    *pBuffer= buffer;

    fclose( f ); return 0;
    }

/************************************************************************/
/*  Write a PCX file.							*/
/************************************************************************/

int bmWritePcxFile(	const char *			filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    { LDEB(-1); return -1; }

int bmCanWritePcxFile( const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    { return -1; }
