#   ifndef	BM_INTERN_H
#   define	BM_INTERN_H

#   include	<stdio.h>
#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<string.h>

#   include	"bitmap.h"

/************************************************************************/
/*									*/
/*  Byte Masks								*/
/*									*/
/************************************************************************/

extern unsigned char	Bmc1Masks[8];
extern unsigned char	Bmc7Masks[8];

/************************************************************************/
/*									*/
/*  Routines.								*/
/*									*/
/************************************************************************/

extern int bmReadTiffFile(	const char *		filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat,
				double *		pCompressionFactor );

extern int bmWriteTiffFile(	const char *		filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmCanWriteTiffFile( const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmReadBmpFile(	const char *		filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat,
				double *		pCompressionFactor );

extern int bmReadWbmpFile(	const char *		filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat,
				double *		pCompressionFactor );

extern int bmReadIcoFile(	const char *		filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat,
				double *		pCompressionFactor );

extern int bmCanWriteBmpFile( const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmCanWriteWbmpFile( const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmCanWriteIcoFile( const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmWriteBmpFile(	const char *		filename,
				const unsigned char *	buffer,
				const BitmapDescription * bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmWriteWbmpFile(	const char *		filename,
				const unsigned char *	buffer,
				const BitmapDescription * bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmWriteIcoFile(	const char *		filename,
				const unsigned char *	buffer,
				const BitmapDescription * bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmReadXwdFile(	const char *		filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat,
				double *		pCompressionFactor );

extern int bmWriteXwdFile(	const char *		filename,
				const unsigned char *	buffer,
				const BitmapDescription * bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmReadSgiFile(	const char *		filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat,
				double *		pCompressionFactor );

extern int bmWriteSgiFile(	const char *		filename,
				const unsigned char *	buffer,
				const BitmapDescription * bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmCanWriteGifFile( const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmReadGifFile(	const char *		filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat,
				double *		pCompressionFactor );

extern int bmWriteGifFile(	const char *		filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmCanWriteEpsFile( const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmWriteEpsFile(	const char *		filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );
extern int bmReadJpegFile(	const char *		filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat,
				double *		pCompressionFactor );

extern int bmCanWriteJpegFile( const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmWriteJpegFile(	const char *		filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmReadXbmFile(	const char *		filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat,
				double *		pCompressionFactor );

extern int bmCanWriteXbmFile( const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmWriteXbmFile(	const char *		filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmCanWriteWmfFile( const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmWriteWmfFile(	const char *		filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmReadPngFile(	const char *		filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat,
				double *		pCompressionFactor );

extern int bmCanWritePngFile( const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmWritePngFile(	const char *		filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmReadPcxFile(	const char *		filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat,
				double *		pCompressionFactor );

extern int bmCanWritePcxFile( const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmWritePcxFile(	const char *		filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmReadXpmFile(	const char *		filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat,
				double *		pCompressionFactor );

extern int bmCanWriteXpmFile( const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmWriteXpmFile(	const char *		filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmReadPbmFile(	const char *		filename,
				unsigned char **	pBuffer,
				BitmapDescription *	bd,
				int *			pPrivateFormat,
				double *		pCompressionFactor );

extern int bmCanWritePbmFile( const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmWritePbmFile(	const char *		filename,
				const unsigned char *	buffer,
				const BitmapDescription *	bd,
				int			privateFormat,
				double			compressionFactor );

extern int bmFillBlock(	unsigned char *		buffer,
			int			x0,
			int			y0,
			int			x1,
			int			y1,
			int			bytesPerRow );

extern unsigned char *	bmBackgroundBuffer(	int	bufferLength,
						int	colorEncoding	);

extern int bmMakeGrayPalette(	const BitmapDescription *	bd,
				int *				pColorCount,
				RGB8Color *			palette );

extern int bmInflateTo8bit(		unsigned char *			to,
					const unsigned char *		from,
					const BitmapDescription *	bd,
					int				trnsC,
					int				remA );

extern int bmInflateTo8bitGray(	
			unsigned char *			to,
			const unsigned char *		from,
			int				bitsPerPixel,
			int				pixelsWide );

extern int bmMakeMonochrome(		BitmapDescription *	bd,
					RGB8Color *		palette,
					unsigned char *		buffer );

extern int bmSetColorAllocatorForPaletteImage(
					ColorAllocator *		ca,
					const BitmapDescription *	bd );

extern int bmSetColorAllocatorForBWImage(
					ColorAllocator *		ca,
					const BitmapDescription *	bd );

extern int bmSetColorAllocatorForWBImage(
					ColorAllocator *		ca,
					const BitmapDescription *	bd );

extern int bmPlanarToChunky(		unsigned char *			to,
					const unsigned char *		from,
					const BitmapDescription *	bd );

#   endif /*  BM_INTERN_H	*/
