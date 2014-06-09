#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	<string.h>
#   include	<tiffio.h>
#   include	<appDebugon.h>

static BitmapFileType	bmTiffFile=
    {
    bmWriteTiffFile,
    bmCanWriteTiffFile,
    bmReadTiffFile,
    "tiff",
    "*.tiff",
    "tiffFile",
    "Tagged Image Format ( *.tiff )",
    };

static BitmapFileType	bmTifFile=
    {
    bmWriteTiffFile,
    bmCanWriteTiffFile,
    bmReadTiffFile,
    "tif",
    "*.tif",
    "tifFile",
    "Tagged Image Format ( *.tif )",
    };

static BitmapFileType	bmBmpFile=
    {
    bmWriteBmpFile,
    bmCanWriteBmpFile,
    bmReadBmpFile,
    "bmp",
    "*.bmp",
    "bmpFile",
    "Microsoft Bitmap ( *.bmp )",
    };

static BitmapFileType	bmIcoFile=
    {
    bmWriteIcoFile,
    bmCanWriteIcoFile,
    bmReadIcoFile,
    "ico",
    "*.ico",
    "icoFile",
    "Microsoft Icon ( *.ico )",
    };

static BitmapFileType	bmXwdFile=
    {
    NULL, /* bmWriteXwdFile, */
    NULL, /* bmCanWriteXwdFile, */
    bmReadXwdFile,
    "xwd",
    "*.xwd",
    "xwdFile",
    "X-Window Dump ( *.xwd )",
    };

static BitmapFileType	bmJpgFile=
    {
    bmWriteJpegFile,
    bmCanWriteJpegFile,
    bmReadJpegFile,
    "jpg",
    "*.jpg",
    "jpgFile",
    "JPEG ( *.jpg )",
    };

static BitmapFileType	bmJpegFile=
    {
    bmWriteJpegFile,
    bmCanWriteJpegFile,
    bmReadJpegFile,
    "jpeg",
    "*.jpeg",
    "jpegFile",
    "JPEG ( *.jpeg )",
    };

static BitmapFileType	bmGifFile=
    {
    bmWriteGifFile,
    bmCanWriteGifFile,
    bmReadGifFile,
    "gif",
    "*.gif",
    "gifFile",
    "Compuserve GIF ( *.gif )",
    };

static BitmapFileType	bmEpsFile=
    {
    bmWriteEpsFile,
    bmCanWriteEpsFile,
    NULL, /* bmReadEpsFile, */
    "eps",
    "*.eps",
    "epsFile",
    "Encapsulated Postscript ( *.eps )",
    };

static BitmapFileType	bmXbmFile=
    {
    bmWriteXbmFile,
    bmCanWriteXbmFile,
    bmReadXbmFile,
    "xbm",
    "*.xbm",
    "xbmFile",
    "X-Windows bitmap ( *.xbm )",
    };

static BitmapFileType	bmWmfFile=
    {
    bmWriteWmfFile,
    bmCanWriteWmfFile,
    NULL, /* bmReadWmfFile, */
    "wmf",
    "*.wmf",
    "wmfFile",
    "Windows Meta File ( *.wmf )",
    };

static BitmapFileType	bmPngFile=
    {
    bmWritePngFile,
    bmCanWritePngFile,
    bmReadPngFile,
    "png",
    "*.png",
    "pngFile",
    "Portable Network Graphics ( *.png )",
    };

static BitmapFileType	bmPcxFile=
    {
    bmWritePcxFile,
    bmCanWritePcxFile,
    bmReadPcxFile,
    "pcx",
    "*.pcx",
    "pcxFile",
    "Zsoft PC Paintbrush ( *.pcx )",
    };

static BitmapFileType	bmXpmFile=
    {
    bmWriteXpmFile,
    bmCanWriteXpmFile,
    bmReadXpmFile,
    "xpm",
    "*.xpm",
    "xpmFile",
    "X11 Pixmap ( *.xpm )",
    };

static BitmapFileType	bmPgmFile=
    {
    bmWritePbmFile,
    bmCanWritePbmFile,
    bmReadPbmFile,
    "pgm",
    "*.pgm",
    "pgmFile",
    "Portable Gray Map ( *.pgm )",
    };

static BitmapFileType	bmPbmFile=
    {
    bmWritePbmFile,
    bmCanWritePbmFile,
    bmReadPbmFile,
    "pbm",
    "*.pbm",
    "pbmFile",
    "Portable Bitmap ( *.pbm )",
    };

static BitmapFileType	bmPpmFile=
    {
    bmWritePbmFile,
    bmCanWritePbmFile,
    bmReadPbmFile,
    "ppm",
    "*.ppm",
    "ppmFile",
    "Portable Pixmap ( *.ppm )",
    };

static BitmapFileType	bmWbmpFile=
    {
    bmWriteWbmpFile,
    bmCanWriteWbmpFile,
    bmReadWbmpFile,
    "wbmp",
    "*.wbmp",
    "wbmpFile",
    "WAP Wireless Bitmap Format ( *.wbmp )",
    };

# if 0
static BitmapFileType	bmSgiFile=
    {
    bmWriteSgiFile,
    NULL,
    bmReadSgiFile,
    "sgi",
    "*.sgi",
    "sgiFile",
    "SGI Image File ( *.sgi )",
    };
# endif

BitmapFileType * bmFileTypes[]=
    {
    &bmPngFile,
    &bmJpegFile,
    &bmJpgFile,
    &bmBmpFile,
    &bmGifFile,
    &bmXwdFile,
    &bmEpsFile,
    &bmXbmFile,
    &bmIcoFile,
    &bmTiffFile,
    &bmTifFile,
    &bmPcxFile,
    &bmXpmFile,
    &bmPgmFile,
    &bmPbmFile,
    &bmPpmFile,
    &bmWbmpFile,
    &bmWmfFile,
    };

BitmapFileFormat	bmFileFormats[]=
    {
	{ "PNG  Portable Network Graphics", "png1File",
			    0,				&bmPngFile },
	{ "JPG  Independent JPEG Group (.jpg)", "jpg1File",
			    1,				&bmJpgFile },
	{ "JPEG  Independent JPEG Group (.jpeg)", "jpeg1File",
			    1,				&bmJpegFile },
	{ "BMP  Microsoft Windows 3.x bitmap", "bmp3File",
			    40,				&bmBmpFile },
	{ "GIF  Compuserve GIF", "gif87File",
			    87,				&bmGifFile },
	{ "XWD  X/11 Window dump", "xwd11File",
			    11,				&bmXwdFile },
	{ "XWD  X/10 Window dump", "xwd10File",
			    10,				&bmXwdFile },
	{ "EPS  Encapsulated Postscript", "eps1File",
			    1,				&bmEpsFile },
	{ "EPS  Encapsulated Postscript (Level 2)", "eps2File",
			    2,				&bmEpsFile },
	{ "XBM  X/11 Bitmap File", "xbm11File",
			    11,				&bmXbmFile },
	{ "ICO  Microsoft 3.x Windows Icon", "ico3File",
			    40,				&bmIcoFile },
	{ "TIFF  No compression", "tiffPlainFile",
			    COMPRESSION_NONE,		&bmTiffFile },
	{ "TIFF  CCITT run length", "tiffCcittRleFile",
			    COMPRESSION_CCITTRLE,	&bmTiffFile },
	{ "TIFF  Fax 3 format", "tiffFax3File",
			    COMPRESSION_CCITTFAX3,	&bmTiffFile },
	{ "TIFF  Fax 4 format", "tiffFax4File",
			    COMPRESSION_CCITTFAX4,	&bmTiffFile },
	{ "TIFF  Lempel-Ziv & Welch", "tiffLzwFile",
			    COMPRESSION_LZW,		&bmTiffFile },
	{ "TIFF  NeXT 2-bit RLE", "tiffNextFile",
			    COMPRESSION_NEXT,		&bmTiffFile },
	{ "TIFF  Run length, aligned", "tiffRleFile",
			    COMPRESSION_CCITTRLEW,	&bmTiffFile },
	{ "TIFF  Packbits", "tiffPackbitsFile",
			    COMPRESSION_PACKBITS,	&bmTiffFile },
	{ "TIFF  Thunderscan", "tiffThunderFile",
			    COMPRESSION_THUNDERSCAN,	&bmTiffFile },
	{ "TIFF  JPEG", "tiffJpegFile",
			    COMPRESSION_JPEG,		&bmTiffFile },
	{ "PCX  Zsoft PC Paintbrush file", "pcx1File",
			    0,				&bmPcxFile },
	{ "XPM  X11 Pixmap File", "xpm1File",
			    0,				&bmXpmFile },
	{ "PGM  Gray Map (Text)", "pgm2File",
			    2,				&bmPgmFile },
	{ "PGM  Gray Map (Raw)", "pgm5File",
			    5,				&bmPgmFile },
	{ "PBM  Bitmap (Text)", "pgm1File",
			    1,				&bmPbmFile },
	{ "PBM  Bitmap (Raw)", "pgm4File",
			    4,				&bmPbmFile },
	{ "PPM  Pixmap (Text)", "pgm3File",
			    3,				&bmPbmFile },
	{ "PPM  Pixmap (Raw)", "pgm6File",
			    6,				&bmPbmFile },
	{ "WBMP  Wireless Bitmap", "wbmp0File",
			    0,				&bmWbmpFile },
	{ "WMF  Windows Meta File", "wmfFile",
			    0,				&bmWmfFile },
	/*
	{ "SGI Image File", "sgiFile",
			    0,				&bmSgiFile },
	*/
    };

const int bmNumberOfFileFormats= sizeof(bmFileFormats)/sizeof(BitmapFileFormat);
const int bmNumberOfFileTypes= sizeof(bmFileTypes)/sizeof(BitmapFileType *);

