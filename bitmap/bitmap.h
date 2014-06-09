/************************************************************************/
/*									*/
/*  Description of a bitmap.						*/
/*									*/
/************************************************************************/

#   ifndef	BITMAP_H
#   define	BITMAP_H

#   include	<stdio.h>
#   include	<sioGeneral.h>
#   include	<bmcolor.h>

/************************************************************************/
/*									*/
/*  Color encodings.							*/
/*									*/
/************************************************************************/

typedef enum BitmapColorEncoding
    {
    BMcoBLACKWHITE= 0,
    BMcoWHITEBLACK,
    BMcoRGB,
    BMcoRGB8PALETTE,

    BMco_COUNT
    } BitmapColorEncoding;

#   define	BMcoILLEGALVALUE	BMco_COUNT

extern const char *	bmcoIntToString( int colorEncodingInt );
extern int		bmcoStringToInt( const char * colorEncodingString );
extern const char *	bmcoStrings[];

/************************************************************************/
/*									*/
/*  Units of measurement.						*/
/*									*/
/************************************************************************/

typedef enum BitmapResolution
    {
    BMunM= 0,
    BMunINCH,
    BMunPOINT,
    BMunPIXEL,

    BMun_COUNT
    } BitmapResolution;

#   define	BMunILLEGALVALUE	BMun_COUNT

#   define	POINTS_PER_M		2834.65
#   define	TWIPS_PER_M		(20*POINTS_PER_M)

extern const char *	bmunIntToString( int unitInt );
extern int		bmunStringToInt( const char * unitString );
extern const char *	bmunStrings[];

/************************************************************************/
/*									*/
/*  Description of a bitmap.						*/
/*									*/
/************************************************************************/

typedef struct	BitmapDescription
    {
					/********************************/
					/*  Number of bytes in an image	*/
					/*  buffer; Idem for one row.	*/
					/*  Rows are never divided over	*/
					/*  more than one byte.		*/
					/********************************/
    unsigned int	bdBufferLength;
    unsigned int	bdBytesPerRow;
					/********************************/
					/*  Height/width of an image	*/
					/********************************/
    unsigned int	bdPixelsWide;
    unsigned int	bdPixelsHigh;
					/********************************/
					/*  Description of how pixels	*/
					/*  are encoded.		*/
					/*  For direct images, all three*/
					/*  refer to the buffer.	*/
					/*  For palette type images,	*/
					/*  BitsPerSample,		*/
					/*  SamplesPerPixel refer to the*/
					/*  image; BitsPerPixel refers	*/
					/*  to the buffer.		*/
					/********************************/
    int			bdBitsPerSample;
    int			bdSamplesPerPixel;
    int			bdBitsPerPixel;
					/********************************/
					/*  Pixels per metric unit;	*/
					/*  The unit.			*/
					/********************************/
    int			bdXResolution;
    int			bdYResolution;
    unsigned char	bdUnit;
					/********************************/
					/*  How are colors encoded.	*/
					/********************************/
    unsigned char	bdColorEncoding;
					/********************************/
					/*  Does it have a transparency	*/
					/*  mask? If so, 0 in the data	*/
					/*  means transparent, the	*/
					/*  maximum value opaque. Values*/
					/*  between are not supported.	*/
					/********************************/
    unsigned char	bdHasAlpha;
					/********************************/
					/*  Palette.			*/
					/*  ColorCount is only relevant	*/
					/*  with a palette.		*/
					/********************************/
    unsigned int	bdColorCount;
    union
	{
	RGB8Color *	bdpRGB8Palette;
	} bdPaletteUninterpreted;

#   define	bdRGB8Palette	bdPaletteUninterpreted.bdpRGB8Palette

    } BitmapDescription;

/************************************************************************/
/*									*/
/*  Description of a bitmap file format.				*/
/*									*/
/************************************************************************/

typedef struct	BitmapFileType
    {
    int (*bftWrite)(	const char *			filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor );

    int (*bftCanWrite)( const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor );

    int (*bftRead)(	const char *		filename,
			unsigned char **	pBuffer,
			BitmapDescription *	bd,
			int *			pPrivateFormat,
			double *		pCompressionFactor	);

    char *	bftFileExtension;
    char *	bftFileFilter;
    char *	bftTypeId;
    char *	bftTypeDescription;
    } BitmapFileType;

typedef struct	BitmapFileFormat
    {
    char *		bffDescription;
    char *		bffId;
    int			bffPrivate;
    BitmapFileType *	bffFileType;
    } BitmapFileFormat;

/************************************************************************/
/*									*/
/*  Description of selection from a bitmap.				*/
/*									*/
/************************************************************************/

typedef struct	BitmapSelection
    {
    int		bsX0;
    int		bsY0;
    int		bsPixelsWide;
    int		bsPixelsHigh;
    } BitmapSelection;

/************************************************************************/
/*									*/
/*  Context for writing images to PostScript.				*/
/*									*/
/************************************************************************/

typedef struct BitmapPrinter
    {
    int				bpUseFilters;
    int				bpIndexedImages;
    SimpleOutputStream *	bpOutput;
    SimpleOutputStream *	bpHexed;
    SimpleOutputStream *	bpBase85;
    SimpleOutputStream *	bpFlate;
    } BitmapPrinter;

extern int bmPsOpenBitmapPrinter(
				BitmapPrinter *			bp,
				SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				useFilters,
				int				indexedImages );

extern void bmCloseBitmapPrinter(	BitmapPrinter *		bp );

/************************************************************************/
/*									*/
/*  Catalog of available of a bitmap file formats.			*/
/*									*/
/************************************************************************/

extern BitmapFileFormat	bmFileFormats[];
extern const int	bmNumberOfFileFormats;

extern BitmapFileType *	bmFileTypes[];
extern const int	bmNumberOfFileTypes;

/************************************************************************/
/*									*/
/*  For Reading/Writing from/to streams.				*/
/*									*/
/************************************************************************/

extern int bmWriteGifFile(	const char *			filename,
				const unsigned char *		buffer,
				const BitmapDescription *	bd,
				int				privateFormat,
				double				factor );

extern int bmCanWriteGifFile(	const BitmapDescription *	bd,
				int				privateFormat,
				double				factor );

extern int bmWriteJpegFile(	const char *			filename,
				const unsigned char *		buffer,
				const BitmapDescription *	bd,
				int				privateFormat,
				double				factor );

extern int bmCanWriteJpegFile(	const BitmapDescription *	bd,
				int				privateFormat,
				double				factor );

/************************************************************************/
/*									*/
/*  Write part of a bitmap to postscript.				*/
/*									*/
/************************************************************************/

extern int bmPsPrintBitmap(	SimpleOutputStream *		sos,
				int				level,
				double				xscale,
				double				yscale,
				int				ox,
				int				oy,
				int				x0,
				int				y0,
				int				wide,
				int				high,
				int				useFilters,
				int				indexedImages,
				const BitmapDescription *	bd,
				const unsigned char *		buffer );

extern int bmPsPrintBitmapImage( SimpleOutputStream *		sos,
				int				level,
				double				xscale,
				double				yscale,
				int				ox,
				int				oy,
				int				x0,
				int				y0,
				int				wide,
				int				high,
				int				useFilters,
				int				indexedImages,
				const BitmapDescription *	bd,
				const unsigned char *		buffer );

extern void bmPsWriteImageInstructions(
				SimpleOutputStream *		sos,
				const BitmapDescription *	bd,
				int				wide,
				int				high,
				const char *			source,
				int				indexedImages );

extern int bmPsWriteBitmapData(	BitmapPrinter *			bp,
				int				x0,
				int				y0,
				int				wide,
				int				high,
				const BitmapDescription *	bd,
				const unsigned char *		inputBuffer );

extern int bmPsRowStringSize(	const BitmapDescription *	bd,
				int				pixelsWide,
				int				indexedImages );

extern int bmWmfWriteWmf(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos );

extern int bmPngWritePng(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos );

extern int bmCanWritePngFile(	const BitmapDescription *	bd,
				int				privateFormat,
				double				fac );

extern int bmJpegWriteJfif(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos );

extern int bmXvWritePaste(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos );

extern int bmGifWriteGif(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sos );

extern int bmBmpReadDib(	BitmapDescription *	bd,
				unsigned char **	pBuffer,
				SimpleInputStream *	sis );

extern int bmJpegReadJfif(	BitmapDescription *	bd,
				unsigned char **	pBuffer,
				SimpleInputStream *	sis );

extern int bmPngReadPng(	BitmapDescription *	bd,
				unsigned char **	pBuffer,
				SimpleInputStream *	sis );

extern int bmXvReadPaste(	BitmapDescription *	bd,
				unsigned char **	pBuffer,
				SimpleInputStream *	sis );

extern int bmGifReadGif(	BitmapDescription *	bd,
				unsigned char **	pBuffer,
				SimpleInputStream *	sis );

extern int bmBmpSaveDib(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				int				bytesWritten,
				void *				voidsos );

typedef int (*bmReadBitmap)(	BitmapDescription *		pBd,
				unsigned char **		pBuffer,
				SimpleInputStream *		sis );

typedef int (*bmWriteBitmap)(	const BitmapDescription *	bd,
				const unsigned char *		buffer,
				SimpleOutputStream *		sis );

/************************************************************************/
/*  Routines.								*/
/************************************************************************/

extern void bmInitDescription(	BitmapDescription *	bd	);

extern void bmCleanDescription(	BitmapDescription *	bd	);

extern int bmCopyDescription(	BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn	);

extern int bmWrite(	const char *			filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				fileFormat,
			double				compressionFactor );

extern int bmCanWrite(	const BitmapDescription *	bd,
			int				fileFormat,
			double				compressionFactor );

extern int bmRead(	const char *		filename,
			unsigned char **	pBuffer,
			BitmapDescription *	bd,
			int *			pFileFormat,
			double *		pCompressionFactor );

extern int bmSelect(	BitmapDescription *		bdOut,
			unsigned char **		pBufOut,
			const BitmapDescription *	bdIn,
			const unsigned char *		bufIn,
			int				x0,
			int				y0,
			int				wide,
			int				high );

extern int bmComponents( void ***		pComponents,
			int *			pCount,
			const unsigned char *	buffer,
			const BitmapDescription * bd );

extern int bmComponentBitmap( unsigned char **		buffer,
			BitmapDescription *		bdout,
			BitmapSelection *		bs,
			const BitmapDescription *	bdin,
			const void *			component	);

extern int bmGroupBitmap( unsigned char **		buffer,
			BitmapDescription *		bdout,
			BitmapSelection *		bs,
			const BitmapDescription *	bdin,
			const void *			vbc	);

int bmgGroupBitmap(	unsigned char **		pBuffer,
			BitmapDescription *		bdout,
			BitmapSelection *		bs,
			const BitmapDescription *	bdin,
			const void *			vcg );

extern int bmGroups(	void ***			pGroups,
			void **				comps,
			int				ncomp,
			const BitmapDescription *	bd		);

extern void bmFreeGroups( void **			groups,
			int				numberOfGroups	);

/************************************************************************/
/*									*/
/*  Palette expansion for drawing.					*/
/*									*/
/************************************************************************/

int bmExpandRGB8Palette(	unsigned char *		to,
				const unsigned char *	from,
				int			pixelsWide,
				int			bitsPerColor,
				const RGB8Color *	palette,
				int			hasAlpha );

/************************************************************************/
/*									*/
/*  Bitmap transformations.						*/
/*									*/
/************************************************************************/

typedef void (*BM_PIX_FUN)(	void *				through,
				int				row,
				int				col );

typedef void (*BM_SEG_FUN)(	void *				through,
				int				x0,
				int				y0,
				int				x1,
				int				y1 );

typedef int (*BitmapTransformation)(BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				int				option );

extern int bmWhiteToTransparent(BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				int				ignoredInt );

extern int bmVerticalFlip(	BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				int				ignoredInt );

extern int bmHorizontalFlip(	BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				int				ignoredInt );

extern int bmUpsideDown(	BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				int				ignoredInt );

extern int bmRotate90(		BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				int				ignoredInt );

extern int bmColorReduce(	BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				int				colorCount );

extern int bmRotate(	BitmapDescription *		bdOut,
			const BitmapDescription *	bdIn,
			unsigned char **		pBufOut,
			const unsigned char *		bufIn,
			double				angle	);

extern int bmFilterSobel(	BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				int				ignoredInt );

extern int bmFilterLaplace(	BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				int				ignoredInt );

extern int bmFilterSmoothe(	BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				int				ignoredInt );

extern int bmFilterBlur(	BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				int				ignoredInt );

extern int bmFilterRelative(	BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				int				ignoredInt );

extern int bmFlipBytes(		unsigned char *	buffer,
				int		byteCount,
				int		bitsPerPixel );

int bmMapImageColors(		const BitmapDescription *	bd,
				const int *			map,
				unsigned char *			buffer );

extern int bmAverage(		BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				int				pixels );

extern int bmExpandPaletteImage( BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				int				ignoredInt );

extern int bmFillImage(		ColorAllocator *		ca,
				int				bitmapUnit,
				int				swapBitmapBytes,
				int				swapBitmapBits,
				int				dither,
				unsigned char *			bufferOut,
				const unsigned char *		bufferIn,
				const BitmapDescription *	bdOut,
				const BitmapDescription *	bdIn );

extern int bmTransparentImage(	BitmapDescription *		bdOut,
				unsigned char **		pBufOut,
				int				colorEncoding,
				int				wide,
				int				high );

extern int bmRGBImage(		BitmapDescription *		bdOut,
				unsigned char **		pBufOut,
				int				colorEncoding,
				int				r,
				int				g,
				int				b,
				int				wide,
				int				high );

extern int bmGammaCorrect(	BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				double				gammaValue );

extern int bmInvertImage(	BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				int				ignoredInt );

extern void bmImageSizeTwips(
			int *				pImageWideTwips,
			int *				pImageHighTwips,
			const BitmapDescription *	bd );

extern int bmCalculateSizes(	BitmapDescription *	bd );

extern int bmToGrayscale(
			BitmapDescription *		bdOut,
			const BitmapDescription *	bdIn,
			unsigned char **		pBufOut,
			const unsigned char *		bufferIn,
			int				ignoredInt );

extern int bmThreshold(	BitmapDescription *		bdOut,
			const BitmapDescription *	bdIn,
			unsigned char **		pBufOut,
			const unsigned char *		bufferIn,
			int				ignoredInt );

extern int bmMorphoDilateSimple(
			BitmapDescription *		bdOut,
			const BitmapDescription *	bdIn,
			unsigned char **		pBufOut,
			const unsigned char *		bufferIn,
			int				ignoredInt );

extern int bmMorphoErodeSimple(
			BitmapDescription *		bdOut,
			const BitmapDescription *	bdIn,
			unsigned char **		pBufOut,
			const unsigned char *		bufferIn,
			int				ignoredInt );

extern int bmMorphoDilate(	BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				const BitmapDescription *	bdSe,
				const unsigned char *		bufSe,
				int				rowOrig,
				int				colOrig );

extern int bmMorphoErode(	BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				const BitmapDescription *	bdSe,
				const unsigned char *		bufSe,
				int				rowOrig,
				int				colOrig );

extern int bmMorphoSetSimpleSe(	BitmapDescription *		bdOut,
				unsigned char **		pBufOut );

extern int bmMorphoLineElement(	BitmapDescription *		bdOut,
				unsigned char **		pBufOut,
				int				wide,
				int				high,
				int				x0,
				int				y0,
				int				x1,
				int				y1 );

extern void bmDraw1BitImage(	const BitmapDescription *	bdTo,
				unsigned char *			bufTo,
				const BitmapDescription *	bdIn,
				const unsigned char *		bufIn,
				unsigned char			invertMask,
				int				rowTo0,
				int				colTo0 );

extern void bmForAll1Pixels(	const BitmapDescription *	bdIn,
				const unsigned char *		bufIn,
				unsigned char			invertMaskIn,
				void *				through,
				BM_PIX_FUN			pixFun );

extern int bmFindLineSegments(	const BitmapDescription *	bdIn,
				const unsigned char *		bufIn,
				int				size,
				void *				through,
				BM_SEG_FUN			segFun );

extern int bmTextureMap(	ColorAllocator *		ca,
				int				swapBitmapUnit,
				int				swapBitmapBytes,
				int				swapBitmapBits,
				int				dither,
				unsigned char *			bufferOut,
				const unsigned char *		bufferIn,
				const BitmapDescription *	bdOut,
				const BitmapDescription *	bdIn,
				double				E_x,
				double				E_y,
				double				E_z,
				double				P_x,
				double				P_y,
				double				P_z,
				double				N_x,
				double				N_y,
				double				N_z,
				double				M_x,
				double				M_y,
				double				M_z );

extern int bmTextureMapInverse(	ColorAllocator *		ca,
				int				swapBitmapUnit,
				int				swapBitmapBytes,
				int				swapBitmapBits,
				int				dither,
				unsigned char *			bufferOut,
				const unsigned char *		bufferIn,
				const BitmapDescription *	bdOut,
				const BitmapDescription *	bdIn,
				double				E_x,
				double				E_y,
				double				E_z,
				double				P_x,
				double				P_y,
				double				P_z,
				double				N_x,
				double				N_y,
				double				N_z,
				double				M_x,
				double				M_y,
				double				M_z );

extern int bmSetColorAllocatorForImage(	ColorAllocator *		ca,
					const BitmapDescription *	bd );

extern int bmCountLinePixels(	const unsigned char *		buffer,
				const BitmapDescription *	bd,
				int				x0,
				int				y0,
				int				x1,
				int				y1 );

extern int bmDrawLine(		unsigned char *			buffer,
				const BitmapDescription *	bd,
				int				x0,
				int				y0,
				int				x1,
				int				y1,
				int				wide );

extern int bmDrawBox(		unsigned char *			buffer,
				const BitmapDescription *	bd,
				int				x0,
				int				y0,
				int				x1,
				int				y1,
				int				wide );

#   endif
