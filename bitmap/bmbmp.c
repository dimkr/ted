#   include	"bitmapConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	"bmintern.h"
#   include	<sioHex.h>
#   include	<sioEndian.h>
#   include	<sioStdio.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Data structures.							*/
/*									*/
/************************************************************************/

/************************************************************************/
/*  Describes a BMP file.						*/
/************************************************************************/

#define COMPRESS_RGB        0L      /* No compression               */
#define COMPRESS_RLE8       1L      /* 8 bits per pixel compression */
#define COMPRESS_RLE4       2L      /* 4 bits per pixel compression */
#define BMP_ID              0x4d42  /* BMP "magic" number           */

/************************************************************************/
/*  Common information for all kinds of BMP files.			*/
/************************************************************************/
typedef struct BmpFileHeader
    {
			/************************************************/
			/*  Common to all variants.			*/
			/************************************************/
    short	bfhMagic;
    long	bfhFileSize;
    short	bfhReserved1;
    short	bfhReserved2;
    long	bfhBitmapDataOffset;
    long	bfhSizeOfRestOfHeader;	/*  It is the Type of file!	*/
			/************************************************/
			/*  Common to all variants.			*/
			/************************************************/
    long	bfhPixelsWide;
    long	bfhPixelsHigh;
    short	bfhPlanes;
    short	bfhBitsPerPixel;
			/************************************************/
			/*  Common to Windows 3.* and OS/2 2.*		*/
			/************************************************/
    long	bfhCompression;
    long	bfhBufferLength;
    long	bfhXResolution;
    long	bfhYResolution;
    long	bfhColorCount;
    long	bfhImportantColors;
    } BmpFileHeader;

/************************************************************************/
/*  Structures desribing ICO files.					*/
/************************************************************************/
typedef struct IcoFileHeader
    {
    short	ifhReserved;
    short	ifhType;
    short	ifhCount;
    } IcoFileHeader;

typedef struct IcoDirectoryEntry
    {
    unsigned char	ideWidth;
    unsigned char	ideHeight;
    unsigned char	ideColorCount;
    unsigned char	ideReserved;
    short		idePlanes;
    short		ideBitCount;
    long		ideBytesInRes;
    long		ideImageOffset;
    } IcoDirectoryEntry;

/************************************************************************/
/*									*/
/*  Read a 3 or 4 byte color table.					*/
/*									*/
/************************************************************************/

static int	bmBmpRead3ColorTable(	int			colorCount,
					SimpleInputStream *	sis,
					RGB8Color *		palette	)
    {
    int		i;

    for ( i= 0; i < colorCount; i++ )
	{
	/* or in the other order ? */
	palette[i].rgb8Blue=	sioInGetCharacter( sis );
	palette[i].rgb8Green=	sioInGetCharacter( sis );
	palette[i].rgb8Red=	sioInGetCharacter( sis );
	palette[i].rgb8Alpha=	255;
	}

    return 0;
    }

static int	bmBmpWrite3ColorTable(	int			colorCount,
					SimpleOutputStream *	sos,
					const RGB8Color *	palette	)
    {
    int		i;

    for ( i= 0; i < colorCount; i++ )
	{
	/* or in the other order ? */
	sioOutPutCharacter( palette[i].rgb8Blue, sos );
	sioOutPutCharacter( palette[i].rgb8Green, sos );
	sioOutPutCharacter( palette[i].rgb8Red, sos );
	}

    return 0;
    }

static int	bmBmpRead4ColorTable(	int			colorCount,
					SimpleInputStream *	sis,
					RGB8Color *		palette	)
    {
    int		i;

    for ( i= 0; i < colorCount; i++ )
	{
	palette[i].rgb8Blue=	sioInGetCharacter( sis );
	palette[i].rgb8Green=	sioInGetCharacter( sis );
	palette[i].rgb8Red=	sioInGetCharacter( sis );
	(void) sioInGetCharacter( sis );
	palette[i].rgb8Alpha=	255;
	}

    return 0;
    }

static int	bmBmpWrite4ColorTable(	int			colorCount,
					SimpleOutputStream *	sos,
					const RGB8Color *	palette	)
    {
    int		i;

    for ( i= 0; i < colorCount; i++ )
	{
	sioOutPutCharacter( palette[i].rgb8Blue, sos );
	sioOutPutCharacter( palette[i].rgb8Green, sos );
	sioOutPutCharacter( palette[i].rgb8Red, sos );
	sioOutPutCharacter( '\0', sos );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read a Microsoft BMP header.					*/
/*  1)  Read fields common to all formats.				*/
/*  2)  Set all distinguishing properties to nonsense, then dixede on	*/
/*	the file type.							*/
/*  3)  If it is not a direct color image, read palette.		*/
/*  4)  Assume reasonable values for missing fields.			*/
/*									*/
/************************************************************************/

static int bmpReadImageHeader(	BmpFileHeader *		bfh,
				SimpleInputStream *	sis,
				RGB8Color **		pPalette )
    {
    int			colorCount;
    RGB8Color *		palette= (RGB8Color *)0;
    int			paletteBytes= 0;

    /*  1  */
    bfh->bfhSizeOfRestOfHeader= sioEndianGetLeInt32( sis );

    switch( bfh->bfhSizeOfRestOfHeader )
	{
	case 12:		/*  OS/2 1.*	*/
	    bfh->bfhPixelsWide   = sioEndianGetLeInt16( sis );
	    bfh->bfhPixelsHigh   = sioEndianGetLeInt16( sis );
	    bfh->bfhPlanes       = sioEndianGetLeInt16( sis );
	    bfh->bfhBitsPerPixel = sioEndianGetLeInt16( sis );

	    if  ( bfh->bfhBitsPerPixel != 24 )
		{
		colorCount=
		    (1U << (bfh->bfhPlanes * bfh->bfhBitsPerPixel ));
		}
	    else{
		colorCount= 0;
		}

	    /*  4  */
	    bfh->bfhCompression= 0;
	    bfh->bfhBufferLength= 0;
	    bfh->bfhXResolution= POINTS_PER_M;
	    bfh->bfhYResolution= POINTS_PER_M;
	    bfh->bfhColorCount= colorCount;
	    bfh->bfhImportantColors= colorCount;

	    /*  3  */
	    if  ( colorCount > 0 )
		{
		palette= (RGB8Color *)malloc( colorCount * sizeof(RGB8Color) );

		if  ( ! palette )
		    { XDEB(palette); return -1;	}

		if  ( bmBmpRead3ColorTable( colorCount, sis, palette ) )
		    { XDEB(sis); free( palette ); return -1; }

		paletteBytes= 3* colorCount;
		}
	    break;
	case 40:		/*  Windows 3.*	*/
	    bfh->bfhPixelsWide		= sioEndianGetLeInt32( sis );
	    bfh->bfhPixelsHigh		= sioEndianGetLeInt32( sis );
	    bfh->bfhPlanes		= sioEndianGetLeInt16( sis );
	    bfh->bfhBitsPerPixel	= sioEndianGetLeInt16( sis );
	    bfh->bfhCompression		= sioEndianGetLeInt32( sis );
	    bfh->bfhBufferLength	= sioEndianGetLeInt32( sis );
	    bfh->bfhXResolution		= sioEndianGetLeInt32( sis );
	    bfh->bfhYResolution		= sioEndianGetLeInt32( sis );
	    bfh->bfhColorCount		= sioEndianGetLeInt32( sis );
	    bfh->bfhImportantColors	= sioEndianGetLeInt32( sis );

	    /*  3  */
	    if  ( bfh->bfhBitsPerPixel != 24 || bfh->bfhColorCount != 0 )
		{
		if  ( bfh->bfhColorCount == 0 )
		    {
		    colorCount=
			(1U << (bfh->bfhPlanes * bfh->bfhBitsPerPixel ));
		    }
		else{ colorCount= bfh->bfhColorCount;	}

		palette= (RGB8Color *)malloc( colorCount * sizeof(RGB8Color) );

		if  ( ! palette )
		    { XDEB(palette); return -1;	}

		if  ( bmBmpRead4ColorTable( colorCount, sis, palette ) )
		    { XDEB(sis); free( palette ); return -1; }

		paletteBytes= 4* colorCount;
		}
	    break;
	case 64:		/*  OS/2 2.0 */
	    bfh->bfhPixelsWide		= sioEndianGetLeInt32( sis );
	    bfh->bfhPixelsHigh		= sioEndianGetLeInt32( sis );
	    bfh->bfhPlanes		= sioEndianGetLeInt16( sis );
	    bfh->bfhBitsPerPixel	= sioEndianGetLeInt16( sis );
	    bfh->bfhCompression		= sioEndianGetLeInt32( sis );
	    bfh->bfhBufferLength	= sioEndianGetLeInt32( sis );
	    bfh->bfhXResolution		= sioEndianGetLeInt32( sis );
	    bfh->bfhYResolution		= sioEndianGetLeInt32( sis );
	    bfh->bfhColorCount		= sioEndianGetLeInt32( sis );
	    bfh->bfhImportantColors	= sioEndianGetLeInt32( sis );
	    sioEndianGetLeInt16( sis );
	    sioEndianGetLeInt16( sis );
	    sioEndianGetLeInt16( sis );
	    sioEndianGetLeInt16( sis );
	    sioEndianGetLeInt32( sis );
	    sioEndianGetLeInt32( sis );
	    sioEndianGetLeInt32( sis );
	    sioEndianGetLeInt32( sis );
      
	    /*  3  */
	    if  ( bfh->bfhBitsPerPixel != 24 || bfh->bfhColorCount != 0 )
		{
		if  ( bfh->bfhColorCount == 0 )
		    {
		    colorCount=
			(1U << (bfh->bfhPlanes * bfh->bfhBitsPerPixel ));
		    }
		else{ colorCount= bfh->bfhColorCount;	}

		palette= (RGB8Color *)malloc( colorCount * sizeof(RGB8Color) );

		if  ( ! palette )
		    { XDEB(palette); return -1;	}

		if  ( bmBmpRead4ColorTable( colorCount, sis, palette ) )
		    { XDEB(sis); free( palette ); return -1; }

		paletteBytes= 4* colorCount;
		}
	    break;
	default:
	    LDEB(bfh->bfhSizeOfRestOfHeader); return -1;
	}

    *pPalette= palette;

    return bfh->bfhSizeOfRestOfHeader+ paletteBytes;
    }

/************************************************************************/
/*									*/
/*  Read the file header as it appears in a BMP file.			*/
/*									*/
/************************************************************************/

static int bmpReadFileHeader(	BmpFileHeader *		bfh,
				SimpleInputStream *	sis,
				RGB8Color **		pPalette )
    {
    int			done;

    bfh->bfhMagic = sioEndianGetLeInt16( sis );

    if  ( bfh->bfhMagic != BMP_ID )
	{ LLDEB(bfh->bfhMagic,BMP_ID); return -1; }

    bfh->bfhFileSize= sioEndianGetLeInt32( sis );
    bfh->bfhReserved1= sioEndianGetLeInt16( sis );
    bfh->bfhReserved2= sioEndianGetLeInt16( sis );
    bfh->bfhBitmapDataOffset= sioEndianGetLeInt32( sis );

    done= bmpReadImageHeader( bfh, sis, pPalette );
    if  ( done < 0 )
	{ LDEB(done); return -1;	}

    return 2+ 4+ 2+ 2+ 4+ done;
    }

/************************************************************************/
/*									*/
/*  Read an uncompresse image from a BMP file.				*/
/*									*/
/************************************************************************/

static int bmBmpReadUncompressed(	SimpleInputStream *	sis,
					unsigned char *		buffer,
					int			pixelsHigh,
					int			bytesPerRow,
					int			bitsPerPixel )
    {
    int		row;
    int		col;
    int		done= 0;

    switch( bitsPerPixel )
	{
	case 1: case 2: case 4: case 8:
	    for ( row= 0; row < pixelsHigh; row++ )
		{
		unsigned char *	to;

		to= buffer+ ( pixelsHigh- row- 1 )* bytesPerRow;

		for ( col= 0; col < bytesPerRow; col++ )
		    { (*to++)= sioInGetCharacter( sis ); done++; }

		while( col % 4 )
		    { sioInGetCharacter( sis ); done++; col++;	}
		}
	    return done;
	case 24:
	    for ( row= 0; row < pixelsHigh; row++ )
		{
		unsigned char *	to;

		to= buffer+ ( pixelsHigh- row- 1 )* bytesPerRow;

		for ( col= 0; col < bytesPerRow; to += 3, done += 3, col += 3 )
		    {
		    to[2]= sioInGetCharacter( sis );
		    to[1]= sioInGetCharacter( sis );
		    to[0]= sioInGetCharacter( sis );
		    }

		while( col % 4 )
		    { sioInGetCharacter( sis ); done++; col++;	}
		}
	    return done;

	case 16:
	    /****************************/
	    /*  Data is big-endian!!	*/
	    /****************************/
	default:
	    LDEB(bitsPerPixel); return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Translate a microsoft header to our bitmap descriptors.		*/
/*									*/
/*  1)  Boolean to value.						*/
/*									*/
/************************************************************************/

static int bmpHeaderToDescription(	BitmapDescription *	bd,
					int *			pBytesPerRow,
					int *			pPrivateFormat,
					const BmpFileHeader *	bfh,
					int			assumeAlpha )
    {
    /*  1  */
    if  ( assumeAlpha )
	{ assumeAlpha= 1; }

    bd->bdPixelsWide= bfh->bfhPixelsWide;
    bd->bdPixelsHigh= bfh->bfhPixelsHigh;

    bd->bdBitsPerSample= 8;
    bd->bdSamplesPerPixel= 3+ assumeAlpha;

    if  ( bfh->bfhBitsPerPixel != 24 || bfh->bfhColorCount != 0 )
	{
	if  ( bfh->bfhColorCount == 0 )
	    {
	    bd->bdColorCount=
		( 1U << (bfh->bfhPlanes * bfh->bfhBitsPerPixel ));
	    }
	else{ bd->bdColorCount= bfh->bfhColorCount; }

	bd->bdColorEncoding= BMcoRGB8PALETTE;
	bd->bdBitsPerPixel= ( 1+ assumeAlpha )* bfh->bfhBitsPerPixel;
	}
    else{
	bd->bdColorEncoding= BMcoRGB;
	}

    bd->bdHasAlpha= assumeAlpha;

    if  ( bmCalculateSizes( bd ) )
	{ LDEB(1); return -1;	}

    *pBytesPerRow= ( bfh->bfhBitsPerPixel* bd->bdPixelsWide + 7 )/ 8;

    bd->bdUnit= BMunM;
    bd->bdXResolution= bfh->bfhXResolution;
    bd->bdYResolution= bfh->bfhYResolution;

    /********************************************************************/
    /*  Check people that even run microsoft systems in their brain.	*/
    /********************************************************************/
    if  ( bd->bdXResolution < 500 || bd->bdXResolution < 500 )
	{ bd->bdYResolution= bd->bdXResolution= 1; bd->bdUnit= BMunPIXEL; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Convert a bitmap description to a Bitmap header.			*/
/*									*/
/************************************************************************/

static int bmbmpDescriptionToHeader(	BmpFileHeader *		bfh,
					RGB8Color **		pPalette,
					int			privateFormat,
					int			hasAlpha,
					const BitmapDescription * bd )
    {
    static RGB8Color		BWPalette[256];
    RGB8Color *			palette= (RGB8Color *)0;

    bfh->bfhMagic= BMP_ID;
    bfh->bfhFileSize= 0;		/*  Set below.	*/
    bfh->bfhReserved1= 0;
    bfh->bfhReserved2= 0;
    bfh->bfhBitmapDataOffset= 0;	/*  Set below.	*/
    bfh->bfhSizeOfRestOfHeader= privateFormat;

    bfh->bfhPixelsWide= bd->bdPixelsWide;
    bfh->bfhPixelsHigh= bd->bdPixelsHigh;

    bfh->bfhPlanes= 1;
    bfh->bfhBitsPerPixel= 0;	/*  Set Below.	*/
    bfh->bfhCompression= 0;	/*  Set Below.	*/

    bfh->bfhBufferLength= 0;	/*  Set Below.	*/

    switch( bd->bdUnit )
	{
	case BMunM:
	    bfh->bfhXResolution= bd->bdXResolution;
	    bfh->bfhYResolution= bd->bdYResolution;
	    break;

	case BMunINCH:
	    bfh->bfhXResolution= 40* bd->bdXResolution;
	    bfh->bfhYResolution= 40* bd->bdYResolution;
	    break;

	case BMunPOINT:
	    bfh->bfhXResolution= POINTS_PER_M* bd->bdXResolution;
	    bfh->bfhYResolution= POINTS_PER_M* bd->bdYResolution;
	    break;

	case BMunPIXEL:
	    bfh->bfhXResolution= 0;
	    bfh->bfhYResolution= 0;
	    break;

	default:
	    LDEB(bd->bdUnit); return -1;
	}

    bfh->bfhColorCount= 0;	/*  Set Below.	*/
    bfh->bfhImportantColors= 0;	/*  Set Below.	*/

    switch( bd->bdColorEncoding )
	{
	int	colorCount;

	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	    if  ( bmMakeGrayPalette( bd, &colorCount, BWPalette ) )
		{ LDEB(bd->bdBitsPerPixel); return -1;	}
			
	    switch( bd->bdBitsPerPixel )
		{
		case 1:
		    bfh->bfhBitsPerPixel= bd->bdBitsPerPixel;
		    bfh->bfhColorCount= colorCount;
		    bfh->bfhCompression= 0;
		    palette= BWPalette;
		    break;

		case 2:
		case 4:
		    bfh->bfhBitsPerPixel= 4;
		    bfh->bfhColorCount= colorCount;
		    bfh->bfhCompression= 2;
		    palette= BWPalette;
		    break;

		case 8:
		    bfh->bfhBitsPerPixel= bd->bdBitsPerPixel;
		    bfh->bfhColorCount= colorCount;
		    bfh->bfhCompression= 1;
		    palette= BWPalette;
		    break;

		default:
		    LDEB(bd->bdBitsPerPixel); return -1;
		}
	    break;

	case BMcoRGB:
	    if  ( bd->bdSamplesPerPixel != 3	||
		  bd->bdBitsPerSample != 8		||
		  bd->bdBitsPerPixel != 24		)
		{
		LLDEB(bd->bdSamplesPerPixel,bd->bdBitsPerSample != 8);
		return -1;
		}
	    bfh->bfhBitsPerPixel= 24;
	    bfh->bfhColorCount= 0;
	    bfh->bfhCompression= 0;
	    break;

	case BMcoRGB8PALETTE:
	    switch( bd->bdBitsPerPixel )
		{
		case 16:
		    if  ( hasAlpha )
			{
			bfh->bfhBitsPerPixel= 8;
			bfh->bfhCompression= 1;
			}
		    else{
			LLDEB(bd->bdColorEncoding,bd->bdBitsPerPixel);
			return -1;
			}
		    bfh->bfhColorCount= bd->bdColorCount;
		    palette= bd->bdRGB8Palette;
		    break;
		case 8:
		    if  ( hasAlpha )
			{ bfh->bfhBitsPerPixel= 4; }
		    else{ bfh->bfhBitsPerPixel= 8; }
		    bfh->bfhColorCount= bd->bdColorCount;
		    bfh->bfhCompression= 1;
		    palette= bd->bdRGB8Palette;
		    break;
		case 4:
		    if  ( hasAlpha )
			{
			LLDEB(bd->bdColorEncoding,bd->bdBitsPerPixel);
			return -1;
			}
		    else{ bfh->bfhBitsPerPixel= 4; }
		    bfh->bfhColorCount= bd->bdColorCount;
		    bfh->bfhCompression= 2;
		    palette= bd->bdRGB8Palette;
		    break;
		default:
		    LLDEB(bd->bdColorEncoding,bd->bdBitsPerPixel);
		    return -1;
		}
		break;
	default:
	    LDEB(bd->bdColorEncoding); return -1;
	}

    *pPalette= palette; return 0;
    }

/************************************************************************/
/*									*/
/*  Read the data bytes from a Microsoft BMP image.			*/
/*									*/
/************************************************************************/

static int bmBmpReadImageBytes(	unsigned char **		pBuffer,
				SimpleInputStream *		sis,
				int				compression,
				int				bytesPerDataRow,
				const BitmapDescription *	bd )
    {
    int			done;
    int			bRead= 0;

    int			x;
    int			y;
    int			wide= bd->bdBytesPerRow;
    int			high= bd->bdPixelsHigh;

    int			c;
    int			n;
    unsigned char	v;

    unsigned char *	to;

    unsigned char *	buffer;

    buffer= malloc( bd->bdBufferLength+ 3 );
    if  ( ! buffer )
	{ LLDEB(bd->bdBufferLength,buffer); return -1;	}

    switch( compression )
	{
	case COMPRESS_RGB:
	    done= bmBmpReadUncompressed( sis, buffer,
					    high, wide, bd->bdBitsPerPixel );

	    if  ( done < 0 )
		{ LDEB(done); free( buffer ); return -1;	}

	    bRead += done;
	    break;
	case COMPRESS_RLE8:
	    memset( buffer, '\0', bd->bdBufferLength );

	    x= y= 0;
	    to= buffer+ x+ ( high- y- 1 )* wide;

	    while( y < high )
		{
		c= sioInGetCharacter( sis ); bRead++;

		if  ( c > 0 )
		    {
		    v= sioInGetCharacter( sis ); bRead++;
		    while( c > 0 )
			{ *(to++)= v; x++; c--;	}

		    continue;
		    }

		c= sioInGetCharacter( sis ); bRead++;

		switch( c )
		    {
		    case 0x00:
			x= 0; y++;
			to= buffer+ x+ ( high- y- 1 )* wide;
			continue;
		    case 0x01:
			break;
		    case 0x02:
			c= sioInGetCharacter( sis ); bRead++; x += c;
			c= sioInGetCharacter( sis ); bRead++; y += c;
			to= buffer+ x+ ( high- y- 1 )* wide;
			continue;
		    default:
			while( c > 1 )
			    {
			    *(to++)= sioInGetCharacter( sis ); bRead++; x++;
			    *(to++)= sioInGetCharacter( sis ); bRead++; x++;
			    c -= 2;
			    }
			if  ( c > 0 )
			    {
			    *(to++)= sioInGetCharacter( sis ); bRead++; x++;
			    sioInGetCharacter( sis ); bRead++;
			    }
			continue;
		    }

		break;
		}

	    break;
	case COMPRESS_RLE4:
	    memset( buffer, '\0', bd->bdBufferLength );
	    x= y= 0;  
	    to= buffer + x/2 + ( high- y- 1 )* wide;

	    while( y < high )
		{
		c= sioInGetCharacter( sis ); bRead++;

		if  ( c > 0 )
		    {
		    v= sioInGetCharacter( sis ); bRead++;

		    if  ( x % 2 )
			{
			unsigned char	vv;

			*(to++) |= ( v >> 4 ); c--;
			vv= ( v >> 4 ) | ( v << 4 );

			while( c > 1 )
			    { *(to++)= vv; x += 2; c -= 2;	}

			if  ( c > 0 )
			    { *(to)= v; x++; }
			}
		    else{
			while( c > 1 )
			    { *(to++)= v; x += 2; c -= 2;	}

			if  ( c > 0 )
			    { *(to)= v; x++; }
			}
		    continue;
		    }

		n= c= sioInGetCharacter( sis ); bRead++;
		switch( c )
		    {
		    case 0x00:
			x= 0; y++;
			to= buffer + x/2 + ( high- y- 1 )* wide;
			continue;
		    case 0x01:
			break;
		    case 0x02:
			c= sioInGetCharacter( sis ); bRead++; x += c;
			c= sioInGetCharacter( sis ); bRead++; y += c;
			to= buffer + x/2 + ( high- y- 1 )* wide;
			continue;
		    default:
			if  ( x % 2 )
			    {
			    v= sioInGetCharacter( sis ); bRead++;
			    *(to++) |= ( v >> 4 ); c--; x++;

			    while( c > 1 )
				{
				*(to)= ( v << 4 );
				v= sioInGetCharacter( sis ); bRead++;
				*(to++) |= ( v >> 4 );
				c -= 2; x += 2;
				}
			    if  ( c )
				{ sioInGetCharacter( sis ); bRead++;	}
			    }
			else{
			    while( c > 1 )
				{
				v= sioInGetCharacter( sis ); bRead++;
				*(to++)= v; c -= 2; x += 2;
				}
			    if  ( c )
				{ sioInGetCharacter( sis ); bRead++;	}
			    }

			if  ( ( n & 3 ) == 1 || ( n & 3 ) == 2 )
			    { sioInGetCharacter( sis ); bRead++; }
			continue;
		    }

		break;
		}
	    break;
	default:
	    LDEB(compression); return -1;
	}

    *pBuffer= buffer; return bRead;
    }

/************************************************************************/
/*									*/
/*  Read a Microsoft BMP file.						*/
/*									*/
/*  1)	Read and check header.						*/
/*  2)  Translate header to a BitmapDescription.			*/
/*  3)  Seek to data. ( We are supposed to be there ).			*/
/*									*/
/************************************************************************/

int bmReadBmpFile(	const char *		filename,
			unsigned char **	pBuffer,
			BitmapDescription *	bd,
			int *			pPrivateFormat,
			double *		pCompressionFactor )
    {
    SimpleInputStream *	sis;
    BmpFileHeader	bfh;
    RGB8Color *		palette= (RGB8Color *)0;
    unsigned char *	buffer;

    int			done;
    int			bytesRead= 0;
    int			bytesPerDataRow;

    sis= sioInStdioOpen( filename );
    if  ( ! sis )
	{ SDEB(filename); return -1;	}

    /*  1  */
    done= bmpReadFileHeader( &bfh, sis, &palette );
    if  ( done < 0 )
	{ LDEB(1); sioInClose( sis ); return -1;	}
    bytesRead += done;

    /*  2  */
    if  ( bmpHeaderToDescription( bd, &bytesPerDataRow, pPrivateFormat,
								    &bfh, 0 ) )
	{ LDEB(1); return -1;	}

    /*  3  */
    while( bytesRead < bfh.bfhBitmapDataOffset )
	{
	sioInGetCharacter( sis );
	bytesRead++;
	}

    if  ( bmBmpReadImageBytes( &buffer, sis,
				bfh.bfhCompression, bytesPerDataRow, bd ) < 0 )
	{
	LDEB(bd->bdBufferLength);

	if  ( palette )
	    { free( palette );	}

	sioInClose( sis ); return -1;
	}

    sioInClose( sis );

    *pBuffer= buffer;
    *pPrivateFormat= bfh.bfhSizeOfRestOfHeader;

    if  ( bfh.bfhColorCount > 0				&&
	  ! bmMakeMonochrome( bd, palette, buffer )	)
	{ return 0;	}

    if  ( bfh.bfhBitsPerPixel != 24 || bfh.bfhColorCount != 0 )
	{ bd->bdRGB8Palette= palette; }

    return 0;
    }

/************************************************************************/
/*  Write a Microsoft BMP header.					*/
/*  1)  Write fields common to all formats.				*/
/*  2)  If it is not a direct color image, write palette.		*/
/************************************************************************/

static int bmpWriteImageHeader(	const BmpFileHeader *	bfh,
				SimpleOutputStream *	sos,
				RGB8Color *		palette )
    {
    int		paletteBytes= 0;

    sioEndianPutLeInt32( bfh->bfhSizeOfRestOfHeader, sos );

    switch( bfh->bfhSizeOfRestOfHeader )
	{
	case 12:		/*  OS/2 1.*	*/
	    sioEndianPutLeInt16( bfh->bfhPixelsWide, sos );
	    sioEndianPutLeInt16( bfh->bfhPixelsHigh, sos );
	    sioEndianPutLeInt16( bfh->bfhPlanes, sos );
	    sioEndianPutLeInt16( bfh->bfhBitsPerPixel, sos );

	    /*  2  */
	    if  ( bfh->bfhColorCount > 0	&&
		  palette			)
		{
		if  ( bmBmpWrite3ColorTable( bfh->bfhColorCount,
							    sos, palette ) )
		    { XDEB(sos); return -1; }

		paletteBytes= 3* bfh->bfhColorCount;
		}
	    break;
	case 40:		/*  Windows 3.*	*/
	    sioEndianPutLeInt32( bfh->bfhPixelsWide, sos );
	    sioEndianPutLeInt32( bfh->bfhPixelsHigh, sos );
	    sioEndianPutLeInt16( bfh->bfhPlanes, sos );
	    sioEndianPutLeInt16( bfh->bfhBitsPerPixel, sos );
	    sioEndianPutLeInt32( bfh->bfhCompression, sos );
	    sioEndianPutLeInt32( bfh->bfhBufferLength, sos );
	    sioEndianPutLeInt32( bfh->bfhXResolution, sos );
	    sioEndianPutLeInt32( bfh->bfhYResolution, sos );
	    sioEndianPutLeInt32( bfh->bfhColorCount, sos );
	    sioEndianPutLeInt32( bfh->bfhImportantColors, sos );

	    /*  2  */
	    if  ( bfh->bfhColorCount > 0	&&
		  palette			)
		{
		if  ( bmBmpWrite4ColorTable( bfh->bfhColorCount,
							    sos, palette ) )
		    { XDEB(sos); return -1; }

		paletteBytes= 4* bfh->bfhColorCount;
		}
	    break;
	case 64:		/*  OS/2 2.0 */
	    sioEndianPutLeInt32( bfh->bfhPixelsWide, sos );
	    sioEndianPutLeInt32( bfh->bfhPixelsHigh, sos );
	    sioEndianPutLeInt16( bfh->bfhPlanes, sos );
	    sioEndianPutLeInt16( bfh->bfhBitsPerPixel, sos );
	    sioEndianPutLeInt32( bfh->bfhCompression, sos );
	    sioEndianPutLeInt32( bfh->bfhBufferLength, sos );
	    sioEndianPutLeInt32( bfh->bfhXResolution, sos );
	    sioEndianPutLeInt32( bfh->bfhYResolution, sos );
	    sioEndianPutLeInt32( bfh->bfhColorCount, sos );
	    sioEndianPutLeInt32( bfh->bfhImportantColors, sos );
	    sioEndianPutLeInt16( 0, sos );
	    sioEndianPutLeInt16( 0, sos );
	    sioEndianPutLeInt16( 0, sos );
	    sioEndianPutLeInt16( 0, sos );
	    sioEndianPutLeInt32( 0L, sos );
	    sioEndianPutLeInt32( 0L, sos );
	    sioEndianPutLeInt32( 0L, sos );
	    sioEndianPutLeInt32( 0L, sos );
      
	    /*  2  */
	    if  ( bfh->bfhColorCount > 0	&&
		  palette			)
		{
		if  ( bmBmpWrite4ColorTable( bfh->bfhColorCount,
							    sos, palette ) )
		    { XDEB(sos); return -1; }

		paletteBytes= 4* bfh->bfhColorCount;
		}
	    break;
	default:
	    LDEB(bfh->bfhSizeOfRestOfHeader); return -1;
	}

    return bfh->bfhSizeOfRestOfHeader+ paletteBytes;
    }

static int bmpWriteFileHeader(	const BmpFileHeader *	bfh,
				SimpleOutputStream *	sos,
				RGB8Color *		palette )
    {
    int		bytesWritten;

    /*  1  */
    sioEndianPutLeInt16( bfh->bfhMagic, sos );

    sioEndianPutLeInt32( bfh->bfhFileSize, sos );
    sioEndianPutLeInt16( bfh->bfhReserved1, sos );
    sioEndianPutLeInt16( bfh->bfhReserved2, sos );
    sioEndianPutLeInt32( bfh->bfhBitmapDataOffset, sos );

    bytesWritten= bmpWriteImageHeader( bfh, sos, palette );

    if  ( bytesWritten < 0 )
	{ LDEB(bytesWritten); return -1;	}

    return 2+ 4+ 2+ 2+ 4+ bytesWritten;
    }

/************************************************************************/
/*									*/
/*  Write different kinds of scanlines to a Microsoft BMP file.		*/
/*									*/
/*  1)	Count number of identical bytes.				*/
/*  2)	Repeating run.							*/
/*  3)	Count literal run.						*/
/*  3)	Literal run. (Pixel count must be at least 3)			*/
/*  2a)	If possible extend the literal to even length.			*/
/*  2b)	Literals should have even lenghth, pad if necessary.		*/
/*  3)	Repeating run.							*/
/*									*/
/************************************************************************/

static int bmBmpWriteScanLine(	SimpleOutputStream *	sos,
				int			pixelsPerByte,
				int			maxByteCount,
				const unsigned char *	buffer,
				int			npixels,
				int			nbytes	)
    {
    int			done= 0;
    int			byteCount;
    int			pixelCount;
    unsigned char	byteValue;

    while( npixels > 0 )
	{
	byteValue= buffer[0];
	byteCount= 1; pixelCount= pixelsPerByte;

	/*  1  */
	while( byteCount < nbytes		&&
	       byteCount < maxByteCount		&&
	       buffer[byteCount] == byteValue	)
	    { byteCount++; pixelCount += pixelsPerByte;	}

	if  ( pixelsPerByte == 2					&&
	      pixelCount == npixels -1					&&
	      ( buffer[byteCount] & 0xf0 ) == ( byteValue & 0xf0 )	)
	    { byteCount++; pixelCount++; }

	/*  2  */
	if  ( byteCount > 1 )
	    {
	    if  ( pixelCount > npixels )
		{ pixelCount=  npixels;	}

	    sioOutPutCharacter( pixelCount, sos );
	    sioOutPutCharacter( byteValue, sos );
	    buffer += byteCount;
	    nbytes -= byteCount; npixels -= pixelCount;
	    done += 2; continue;
	    }

	/*  3  */
	while( byteCount+ 1 < nbytes			&&
	       byteCount+ 1 < maxByteCount		&&
	       buffer[byteCount+1] != buffer[byteCount]	)
	    { byteCount++; pixelCount += pixelsPerByte;	}

	if  ( byteCount+ 1 == nbytes			&&
	      byteCount+ 1 < maxByteCount		)
	    { byteCount++; pixelCount += pixelsPerByte; }

	if  ( pixelCount > npixels )
	    { pixelCount=  npixels;	}

	switch( pixelCount )
	    {
	    int		i;

	    default:
		sioOutPutCharacter( 0, sos ); done++;
		sioOutPutCharacter( pixelCount, sos ); done++;
		for ( i= 0; i < byteCount; i++ )
		    { sioOutPutCharacter( *(buffer++), sos ); done++; }
		if  ( byteCount % 2 )
		    { sioOutPutCharacter( 0, sos ); done++; }
		nbytes -= byteCount; npixels -= pixelCount;
		continue;
	    case 1:
		sioOutPutCharacter( pixelCount, sos ); done++;
		sioOutPutCharacter( *(buffer++), sos ); done++;
		nbytes -= byteCount; npixels -= pixelCount;
		continue;
	    case 2:
		if  ( pixelsPerByte == 2 )
		    {
		    sioOutPutCharacter( pixelCount, sos ); done++;
		    sioOutPutCharacter( *(buffer++), sos ); done++;
		    nbytes -= byteCount; npixels -= pixelCount;
		    continue;
		    }
		if  ( pixelsPerByte == 1 )
		    {
		    sioOutPutCharacter( 1, sos ); done++;
		    sioOutPutCharacter( *(buffer++), sos ); done++;
		    sioOutPutCharacter( 1, sos ); done++;
		    sioOutPutCharacter( *(buffer++), sos ); done++;
		    nbytes -= byteCount; npixels -= pixelCount;
		    continue;
		    }
		LLDEB(pixelCount,pixelsPerByte); return -1;
	    case 0:
		LLDEB(pixelCount,pixelsPerByte); return -1;
	    }
	}

    sioOutPutCharacter( 0, sos );
    sioOutPutCharacter( 0, sos ); done += 2;

    return done;
    }

/************************************************************************/
/*									*/
/*  Write the image bytes of a BMP file.				*/
/*									*/
/************************************************************************/

static int bmBmpWriteImageBytes(const BitmapDescription *	bd,
				const unsigned char *		buffer,
				int				compression,
				SimpleOutputStream *		sos )
    {
    int				done;
    int				bytesWritten= 0;

    int				row;
    int				i;
    const unsigned char *	from;

    int				maxByteCount;
    int				bytesPerRow;
    unsigned char *		inflated= (unsigned char *)0;

    int				white= -1;

    switch( compression )
	{
	case 0:
	    for ( row= 0; row < bd->bdPixelsHigh; row++ )
		{
		from= buffer+ ( bd->bdPixelsHigh- row- 1 )* bd->bdBytesPerRow;

		if  ( bd->bdBitsPerPixel == 24 )
		    {
		    int		col;

		    for ( col= 0; col < bd->bdBytesPerRow; from += 3, col += 3 )
			{
			sioOutPutCharacter( from[2], sos );
			sioOutPutCharacter( from[1], sos );
			sioOutPutCharacter( from[0], sos );
			}
		    }
		else{
		    if  ( sioOutWriteBytes( sos, from, bd->bdBytesPerRow ) !=
							    bd->bdBytesPerRow )
			{ LDEB(bd->bdBytesPerRow); return -1; }
		    }

		bytesWritten += bd->bdBytesPerRow;

		for ( i = bd->bdBytesPerRow; i % 4; i++ )
		    { sioOutPutCharacter( '\0', sos ); bytesWritten++;	}
		}
	    break;
	case 1:
	case 2:
	    bytesPerRow= bd->bdBytesPerRow;

	    if  ( compression == 2		&&
		  bd->bdBitsPerPixel == 2	)
		{
		inflated= malloc( 2* bd->bdBytesPerRow );
		if  ( ! inflated )
		    { LXDEB(bd->bdBytesPerRow,inflated); return -1;	}

		bytesPerRow *= 2;
		}

	    if  ( compression == 1		&&
		  bd->bdHasAlpha		&&
		  bd->bdBitsPerPixel == 16	)
		{
		bytesPerRow= bytesPerRow/ 2;

		inflated= malloc( bytesPerRow );
		if  ( ! inflated )
		    { LXDEB(bd->bdBytesPerRow,inflated); return -1;	}

		for ( i= 0; i < bd->bdColorCount; i++ )
		    {
		    if  ( bd->bdRGB8Palette[i].rgb8Red == 255	&&
			  bd->bdRGB8Palette[i].rgb8Green == 255	&&
			  bd->bdRGB8Palette[i].rgb8Blue == 255	)
			{ white= i; break;	}
		    }

		if  ( white < 0 )
		    { LDEB(white); white= 0;	}
		}

	    maxByteCount= 255/ compression;
	    for ( row= 0; row < bd->bdPixelsHigh; row++ )
		{
		unsigned char *	to;

		from= buffer+ ( bd->bdPixelsHigh- row- 1 )* bd->bdBytesPerRow;

		if  ( compression == 2		&&
		      bd->bdBitsPerPixel == 2	)
		    {
		    to= inflated;

		    for ( i= 0; i < bd->bdBytesPerRow; to += 2, from++, i++ )
			{
			to[0]= ( ( from[0] & 0x30 ) >> 4 )	|
			       ( ( from[0] & 0xc0 ) >> 2 )	;
			to[1]= ( ( from[0] & 0x03 ) << 0 )	|
			       ( ( from[0] & 0x0c ) << 2 )	;
			}

		    from= inflated;
		    }

		if  ( compression == 1		&&
		      bd->bdHasAlpha		&&
		      bd->bdBitsPerPixel == 16	)
		    {
		    to= inflated;

		    for ( i= 0; i < bytesPerRow; from += 2, to++, i++ )
			{
			if  ( from[1] )
			    { *to= *from;	}
			else{ *to= white;	}
			}

		    from= inflated;
		    }

		done= bmBmpWriteScanLine( sos, compression, maxByteCount,
				from, bd->bdPixelsWide, bytesPerRow );

		if  ( done < 0 )
		    { LDEB(row); return -1; }

		bytesWritten += done;
		}

	    sioOutPutCharacter( 0, sos );
	    sioOutPutCharacter( 1, sos ); bytesWritten += 2;

	    if  ( inflated )
		{ free( inflated );	}

	    break;

	default:
	    LDEB(compression); return -1;
	}

    return bytesWritten;
    }

/************************************************************************/
/*									*/
/*  Write a Microsoft BMP file.						*/
/*									*/
/************************************************************************/

int bmWriteBmpFile(	const char *			filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    {
    SimpleOutputStream *	sos;
    BmpFileHeader		bfh;
    RGB8Color *			palette= (RGB8Color *)0;

    int				done;
    int				bytesWritten= 0;
    long			headerOffset= 0L;

    sos= sioOutStdioOpen( filename );
    if  ( ! sos )
	{ SDEB(filename); return -1;	}

    if  ( bmbmpDescriptionToHeader( &bfh, &palette,
					privateFormat, bd->bdHasAlpha, bd ) )
	{ SDEB(filename); sioOutClose( sos ); return -1;	}

    done= bmpWriteFileHeader( &bfh, sos, palette );
    if  ( done < 0 )
	{ SLDEB(filename,bytesWritten); sioOutClose( sos ); return -1; }

    bytesWritten += done;
    bfh.bfhBitmapDataOffset= bytesWritten;

    done= bmBmpWriteImageBytes( bd, buffer, bfh.bfhCompression, sos );
    if  ( done < 0 )
	{ SLDEB(filename,done); sioOutClose( sos ); return -1;	}

    bytesWritten += done;

    bfh.bfhFileSize= bytesWritten;
    bfh.bfhBufferLength= done;

    if  ( sioOutSeek( sos, headerOffset ) )
	{ SLDEB(filename,headerOffset); sioOutClose( sos ); return -1; }

    if  ( bmpWriteFileHeader( &bfh, sos, (RGB8Color *)0 ) < 0 )
	{ SLDEB(filename,headerOffset); sioOutClose( sos ); return -1; }

    sioOutClose( sos );

    return 0;
    }

int bmCanWriteBmpFile( const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    { return 0;	}

/************************************************************************/
/*									*/
/*  Read a Microsoft Ico file.						*/
/*									*/
/*  1)	Read and check header.						*/
/*  2)  Translate header to a BitmapDescription.			*/
/*  2a) Only use XOR part of image.					*/
/*  3)  Do not seek to data. It immediately follows.			*/
/*  4)  Read image.							*/
/*  5)  Combine with transparency mask.					*/
/*									*/
/************************************************************************/

int bmReadIcoFile(	const char *		filename,
			unsigned char **	pBuffer,
			BitmapDescription *	bd,
			int *			pPrivateFormat,
			double *		pCompressionFactor	)
    {
    int				rval= 0;
    SimpleInputStream *		sis= (SimpleInputStream *)0;
    unsigned char *		buffer= (unsigned char *)0;
    RGB8Color *			palette= (RGB8Color *)0;

    BmpFileHeader		bfh;
    IcoFileHeader		ifh;
    IcoDirectoryEntry		ide;

    int				alphaByteCount;
    unsigned char *		ab= (unsigned char *)0;
    unsigned char *		rb= (unsigned char *)0;

    int				row;

    unsigned char *	to;
    int			done;
    int			bytesPerDataRow;
    int			bytesRead= 0;

    sis= sioInStdioOpen( filename );
    if  ( ! sis )
	{ SDEB(filename); rval= -1; goto ready;	}

    ifh.ifhReserved= sioEndianGetLeInt16( sis );
    if  ( ifh.ifhReserved != 0 )
	{ LDEB(ifh.ifhReserved); rval= -1; goto ready;	}

    ifh.ifhType= sioEndianGetLeInt16( sis );
    if  ( ifh.ifhType != 1 )
	{ LDEB(ifh.ifhType); rval= -1; goto ready;	}

    ifh.ifhCount= sioEndianGetLeInt16( sis );
    if  ( ifh.ifhCount != 1 )
	{ SLDEB(filename,ifh.ifhCount);	}

    ide.ideWidth= sioInGetCharacter( sis );
    ide.ideHeight= sioInGetCharacter( sis );
    ide.ideColorCount= sioInGetCharacter( sis );

    ide.ideReserved= sioInGetCharacter( sis );
    if  ( ide.ideReserved != 0 )
	{ LDEB(ide.ideReserved); rval= -1; goto ready;	}

    ide.idePlanes= sioEndianGetLeInt16( sis );
    ide.ideBitCount= sioEndianGetLeInt16( sis );
    ide.ideBytesInRes= sioEndianGetLeInt32( sis );
    ide.ideImageOffset= sioEndianGetLeInt32( sis );

    bytesRead= 2+ 2+ 2+ 4* 1+ 2+ 2+ 4+ 4;

    while( bytesRead < ide.ideImageOffset )
	{
	if  ( sioInGetCharacter( sis ) < 0 )
	    { LDEB(ide.ideImageOffset); rval= -1; goto ready;	}

	bytesRead++;
	}

    /*  1  */
    done= bmpReadImageHeader( &bfh, sis, &palette );
    if  ( done < 0 )
	{ SLDEB(filename,done); rval= -1; goto ready;	}

    /*  2  */
    if  ( bmpHeaderToDescription( bd, &bytesPerDataRow, pPrivateFormat,
								&bfh, 1 ) )
	{ LDEB(1); rval= -1; goto ready;	}
    /*  2a  */
    bd->bdPixelsHigh /= 2;

    /*  3  LDEB(bfh.bfhBitmapDataOffset);  */

    buffer= malloc( bd->bdBufferLength+ 3 );
    if  ( ! buffer )
	{ LLDEB(bd->bdBufferLength,buffer); rval= -1; goto ready; }

    /*  4  */
    for ( row= 0; row < bd->bdPixelsHigh; row++ )
	{
	to= buffer+ ( bd->bdPixelsHigh- row- 1 )* bd->bdBytesPerRow;

	done= sioInReadBytes( sis, to, bytesPerDataRow );
	if  ( done != bytesPerDataRow )
	    {
	    LLDEB(done,bytesPerDataRow);
	    LLDEB(row,bd->bdPixelsHigh); LLDEB(bfh.bfhCompression,done);
	    rval= -1; goto ready;
	    }
	}

    alphaByteCount= ( bd->bdPixelsWide+ 7 )/8;
    ab= malloc( alphaByteCount );
    if  ( ! ab )
	{ LXDEB(alphaByteCount,ab); rval= -1; goto ready; }
    rb= malloc( bd->bdBytesPerRow );
    if  ( ! rb )
	{ LXDEB(rb,bd->bdBytesPerRow); rval= -1; goto ready; }

    if  ( bytesPerDataRow > bd->bdBytesPerRow )
	{
	LLDEB(bytesPerDataRow,bd->bdBytesPerRow);
	rval= -1; goto ready;
	}

    /*  5  */
    for ( row= 0; row < bd->bdPixelsHigh; row++ )
	{
	int		i;
	int		got;

	to= buffer+ ( bd->bdPixelsHigh- row- 1 )* bd->bdBytesPerRow;

	got= sioInReadBytes( sis, ab, alphaByteCount );
	if  ( got != alphaByteCount )
	    {
	    LLLDEB(row,got,bd->bdPixelsWide);
	    rval= -1; goto ready;
	    }

	memcpy( rb, to, bytesPerDataRow );

	for ( i= 0; i < alphaByteCount; i++ )
	    { ab[i]= ~ab[i];	}

	switch( bd->bdColorCount )
	    {
	    case 2:
		for ( i= 0; i < bytesPerDataRow; i++ )
		    {
		    *to= 0;
		    *to |= ( rb[i] & 0x80 )      | ( ab[i] & 0x80 ) >> 1;
		    *to |= ( rb[i] & 0x40 ) >> 1 | ( ab[i] & 0x40 ) >> 2;
		    *to |= ( rb[i] & 0x20 ) >> 2 | ( ab[i] & 0x20 ) >> 3;
		    *to |= ( rb[i] & 0x10 ) >> 3 | ( ab[i] & 0x10 ) >> 4;
		    *(++to)= 0;
		    *to |= ( rb[i] & 0x08 ) << 4 | ( ab[i] & 0x08 ) << 3;
		    *to |= ( rb[i] & 0x04 ) << 3 | ( ab[i] & 0x04 ) << 2;
		    *to |= ( rb[i] & 0x02 ) << 2 | ( ab[i] & 0x02 ) << 1;
		    *to |= ( rb[i] & 0x01 ) << 1 | ( ab[i] & 0x01 )     ;
		    to++;
		    }
		break;

	    case 16:
		for ( i= 0; i < bd->bdPixelsWide; i += 2 )
		    {
		    if  ( ( ab[i/8] << (i%8) ) & 0x80 )
			{ *(to++)= ( rb[i/2] & 0xf0 ) | 0x0f;	}
		    else{ *(to++)= 0x00;		}

		    if  ( ( ab[i/8] << (i%8) ) & 0x40 )
			{ *(to++)=   ( rb[i/2] << 4 ) | 0x0f;	}
		    else{ *(to++)= 0x00;			}
		    }
		break;

	    case 256:
		for ( i= 0; i < bd->bdPixelsWide; i++ )
		    {
		    *(to++)= rb[i];
		    if  ( ( ab[i/8] << (i%8) ) & 0x80 )
			{ *(to++)= 255;	}
		    else{ *(to++)= 0;	}
		    }
		break;

	    case 0:
		for ( i= 0; i < bd->bdPixelsWide; i++ )
		    {
		    *(to++)= rb[3* i+ 0];
		    *(to++)= rb[3* i+ 1];
		    *(to++)= rb[3* i+ 2];
		    if  ( ( ab[i/8] << (i%8) ) & 0x80 )
			{ *(to++)= 255;	}
		    else{ *(to++)= 0;	}
		    }
		break;

	    case 4:
	    case 8:
	    default:
		LDEB(bd->bdColorCount); rval= -1; goto ready;
	    }
	}

    *pBuffer= buffer; buffer= (unsigned char *)0; /* steal */
    *pPrivateFormat= bfh.bfhSizeOfRestOfHeader;

    if  ( bfh.bfhColorCount > 0				&&
	  ! bmMakeMonochrome( bd, palette, buffer )	)
	{ palette= (RGB8Color *)0; /* freed */	}
    else{
	if  ( bfh.bfhBitsPerPixel != 24 || bfh.bfhColorCount != 0 )
	    {
	    bd->bdRGB8Palette= palette; palette= (RGB8Color *)0; /* steal */
	    }
	}

  ready:

    if  ( palette )
	{ free( palette );	}
    if  ( rb )
	{ free( rb );	}
    if  ( ab )
	{ free( ab );	}
    if  ( buffer )
	{ free( buffer );	}
    if  ( sis )
	{ sioInClose( sis );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Extract an image from hexadecimal data, e.g. from a windows		*/
/*  metafile in a word processing document.				*/
/*									*/
/************************************************************************/

int bmBmpReadDib(	BitmapDescription *	pBd,
			unsigned char **	pBuffer,
			SimpleInputStream *	sis )
    {
    BmpFileHeader	bfh;

    int			privateFormat;
    int			bytesPerDataRow;
    RGB8Color *		palette= (RGB8Color *)0;

    unsigned char *	buffer;
    BitmapDescription	bd;

    int			done;
    int			bytesRead;

    done= bmpReadImageHeader( &bfh, sis, &palette );
    if  ( done < 0 )
	{ LDEB(done); sioInClose( sis ); return -1;	}
    bytesRead= done;

    if  ( bmpHeaderToDescription( &bd,
				&bytesPerDataRow, &privateFormat, &bfh, 0 ) )
	{ LDEB(1); return -1;	}

    done= bmBmpReadImageBytes( &buffer, sis,
				bfh.bfhCompression, bytesPerDataRow, &bd );
    if  ( done < 0 )
	{
	LDEB(bd.bdBufferLength);

	if  ( palette )
	    { free( palette );	}

	return -1;
	}

    bytesRead += done;

    if  ( bd.bdColorCount != 2				||
	  bd.bdColorEncoding != BMcoRGB8PALETTE		||
	  bmMakeMonochrome( &bd, palette, buffer )	)
	{
	if  ( bd.bdColorEncoding == BMcoRGB8PALETTE )
	    { bd.bdRGB8Palette= palette;	}
	}

    *pBd= bd; *pBuffer= buffer;

    return bytesRead;
    }

int bmBmpSaveDib(	const BitmapDescription *	bd,
			const unsigned char *		buffer,
			int				headerOffset,
			void *				voidsos )
    {
    SimpleOutputStream *	sos= (SimpleOutputStream *)voidsos;

    BmpFileHeader		bfh;
    RGB8Color *			palette= (RGB8Color *)0;

    int				done;
    int				bytesWritten= 0;

    if  ( bmbmpDescriptionToHeader( &bfh, &palette, 40, bd->bdHasAlpha, bd ) )
	{ LDEB(40); return -1;	}

    done= bmpWriteImageHeader( &bfh, sos, palette );
    if  ( done < 0 )
	{ LDEB(done); return -1;	}

    bytesWritten += done;
    bfh.bfhBitmapDataOffset= bytesWritten;

    done= bmBmpWriteImageBytes( bd, buffer, bfh.bfhCompression, sos );
    if  ( done < 0 )
	{ LDEB(done); return -1;	}

    bytesWritten += done;

    bfh.bfhFileSize= headerOffset+ bytesWritten;
    bfh.bfhBufferLength= done;

    if  ( sioOutSeek( sos, headerOffset ) )
	{ LDEB(headerOffset); return -1; }

    done= bmpWriteImageHeader( &bfh, sos, palette );
    if  ( done < 0 )
	{ LDEB(done); return -1;	}

    if  ( sioOutSeek( sos, headerOffset+ bytesWritten ) )
	{ LDEB(bytesWritten); return -1; }

    return bytesWritten;
    }

/************************************************************************/
/*  Write a Microsoft ICO file.						*/
/************************************************************************/
static int bmbmpWriteIcoDirectory(	IcoDirectoryEntry *	ide,
					SimpleOutputStream *	sos	)
    {
    sioOutPutCharacter( ide->ideWidth, sos );
    sioOutPutCharacter( ide->ideHeight, sos );
    sioOutPutCharacter( ide->ideColorCount, sos );
    sioOutPutCharacter( ide->ideReserved, sos );
    sioEndianPutLeInt16( ide->idePlanes, sos );
    sioEndianPutLeInt16( ide->ideBitCount, sos );
    sioEndianPutLeInt32( ide->ideBytesInRes, sos );
    sioEndianPutLeInt32( ide->ideImageOffset, sos );

    return 16;
    }

int bmWriteIcoFile(	const char *			filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    {
    SimpleOutputStream *	sos;
    BmpFileHeader		bfh;
    IcoFileHeader		ifh;
    IcoDirectoryEntry		ide;
    RGB8Color *			palette= (RGB8Color *)0;
    int				row;
    int				i;
    const unsigned char *	from;
    int				col;
    unsigned char		c;
    long			directoryOffset;
    int				bytesWritten;
    int				done;

    sos= sioOutStdioOpen( filename );
    if  ( ! sos )
	{ SDEB(filename); return -1;	}

    ifh.ifhReserved= 0;
    ifh.ifhType= 1;
    ifh.ifhCount= 1;

    sioEndianPutLeInt16( ifh.ifhReserved, sos );
    sioEndianPutLeInt16( ifh.ifhType, sos );
    sioEndianPutLeInt16( ifh.ifhCount, sos );

    bytesWritten= 2+ 2+ 2;

    directoryOffset= bytesWritten;

    ide.ideWidth= bd->bdPixelsWide;
    ide.ideHeight= bd->bdPixelsHigh;
    ide.ideColorCount= bd->bdColorCount;
    ide.ideReserved= 0;
    ide.idePlanes= 0;
    ide.ideBitCount= 0;
    ide.ideBytesInRes= 0;
    ide.ideImageOffset= 0;

    done= bmbmpWriteIcoDirectory( &ide, sos );
    if  ( done < 0 )
	{ SLDEB(filename,done); sioOutClose( sos ); return -1;	}
    bytesWritten += done;

    ide.ideImageOffset= ( ( bytesWritten + 1 )/ 2 )* 2;

    while( bytesWritten < ide.ideImageOffset )
	{
	sioOutPutCharacter( '\0', sos );
	bytesWritten++;
	}

    if  ( bmbmpDescriptionToHeader( &bfh, &palette, 40, bd->bdHasAlpha, bd ) )
	{ SDEB(filename); sioOutClose( sos ); return -1;	}

    bfh.bfhPixelsHigh *= 2;
    bfh.bfhCompression= 0;

    /**/ bfh.bfhXResolution= 0;
    /**/ bfh.bfhYResolution= 0;

    done= bmpWriteImageHeader( &bfh, sos, palette );
    if  ( done < 0 )
	{ SLDEB(filename,done); sioOutClose( sos ); return -1;	}

    bytesWritten += done;

    /**/ bfh.bfhColorCount= 0;
    bfh.bfhBitmapDataOffset= bytesWritten;

    if  ( bd->bdHasAlpha )
	{
	switch( bfh.bfhCompression )
	    {
	    case 0:
		switch( bd->bdBitsPerPixel )
		    {
		    case 8:
			for ( row= 0; row < bd->bdPixelsHigh; row++ )
			    {
			    from= buffer+ ( bd->bdPixelsHigh- row- 1 )*
							    bd->bdBytesPerRow;

			    for ( col= 0; col < bd->bdPixelsWide; col += 2 )
				{
				c=  ( ( from[0] & 0xf0 )	)	|
				    ( ( from[1] & 0xf0 ) >> 4	)	;

				sioOutPutCharacter( c, sos ); bytesWritten++;
				from += 2;
				}
			    }
			break;
		    default:
			LDEB(bd->bdBitsPerPixel); sioOutClose( sos ); return -1;
		    }
		break;
	    default:
		LDEB(bfh.bfhCompression); sioOutClose( sos ); return -1;
		break;
	    }

	switch( bd->bdBitsPerPixel )
	    {
	    case 8:
		for ( row= 0; row < bd->bdPixelsHigh; row++ )
		    {
		    from= buffer+ ( bd->bdPixelsHigh- row- 1 )*
							bd->bdBytesPerRow;

		    for ( col= 0; col < bd->bdPixelsWide; col += 8 )
			{
			c= 0;

			if  ( *(from++) & 0x0f ) { c |= 0x80;	}
			if  ( *(from++) & 0x0f ) { c |= 0x40;	}
			if  ( *(from++) & 0x0f ) { c |= 0x20;	}
			if  ( *(from++) & 0x0f ) { c |= 0x10;	}

			if  ( *(from++) & 0x0f ) { c |= 0x08;	}
			if  ( *(from++) & 0x0f ) { c |= 0x04;	}
			if  ( *(from++) & 0x0f ) { c |= 0x02;	}
			if  ( *(from++) & 0x0f ) { c |= 0x01;	}

			sioOutPutCharacter( ~c, sos ); bytesWritten++;
			}
		    }
		break;
	    default:
		LDEB(bd->bdBitsPerPixel); sioOutClose( sos ); return -1;
	    }
	}
    else{
	switch( bfh.bfhCompression )
	    {
	    case 0:
		for ( row= 0; row < bd->bdPixelsHigh; row++ )
		    {
		    from= buffer+ ( bd->bdPixelsHigh- row- 1 )*
							bd->bdBytesPerRow;

		    for ( i= 0; i <  bd->bdBytesPerRow; i++ )
			{
			sioOutPutCharacter( *(from++), sos );
			bytesWritten++;
			}
		    for ( i = bd->bdBytesPerRow; i % 4; i++ )
			{ sioOutPutCharacter( '\0', sos ); bytesWritten++; }
		    }
		break;
	    default:
		LDEB(bfh.bfhCompression); sioOutClose( sos ); return -1;
	    }

	for ( row= 0; row < bd->bdPixelsHigh; row++ )
	    {
	    for ( col= 0; col < bd->bdPixelsWide; col += 8 )
		{ sioOutPutCharacter( 0xff, sos ); bytesWritten++;	}
	    }
	}

    bfh.bfhFileSize= bytesWritten;
    bfh.bfhBufferLength= bfh.bfhFileSize- bfh.bfhBitmapDataOffset;

    ide.ideBytesInRes= bfh.bfhFileSize- ide.ideImageOffset;

    if  ( sioOutSeek( sos, ide.ideImageOffset ) )
	{ LDEB(ide.ideImageOffset); return -1;	}

    if  ( bmpWriteImageHeader( &bfh, sos, (RGB8Color *)0 ) < 0 )
	{ SDEB(filename); sioOutClose( sos ); return -1; }

    if  ( sioOutSeek( sos, directoryOffset ) )
	{ LDEB(ide.ideImageOffset); return -1;	}

    if  ( bmbmpWriteIcoDirectory( &ide, sos ) < 0 )
	{ SDEB(filename); sioOutClose( sos ); return -1;	}

    sioOutClose( sos );

    return 0;
    }

int bmCanWriteIcoFile( const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    {
    if  ( bd->bdPixelsWide != 16	&&
	  bd->bdPixelsWide != 32	&&
	  bd->bdPixelsWide != 64	)
	{ return -1;	}

    if  ( bd->bdPixelsHigh != 16	&&
	  bd->bdPixelsHigh != 32	&&
	  bd->bdPixelsHigh != 64	)
	{ return -1;	}

    switch( bd->bdColorEncoding )
	{
	case BMcoRGB8PALETTE:
	    if  ( bd->bdColorCount > 16 )
		{ return -1;	}
	    return 0;
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	    if  ( bd->bdBitsPerSample > 4 )
		{ return -1;	}
	    return 0;
	default:
	    return -1;
	}

    return 0;
    }
