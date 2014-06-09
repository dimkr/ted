/******************************************************************************
* In order to make life a little bit easier when using the GIF file format,   *
* this library was written, and which does all the dirty work...              *
*                                                                             *
*                                        Written by Gershon Elber,  Jun. 1989 *
*                                        Hacks by Eric S. Raymond,  Sep. 1992 *
*******************************************************************************
* History:                                                                    *
* 14 Jun 89 - Version 1.0 by Gershon Elber.                                   *
*  3 Sep 90 - Version 1.1 by Gershon Elber (Support for Gif89, Unique names)  *
* 15 Sep 90 - Version 2.0 by Eric S. Raymond (Changes to suoport GIF slurp)   *
* 26 Jun 96 - Version 3.0 by Eric S. Raymond (Full GIF89 support)             * 
* 17 Dec 98 - Version 4.0 by Toshio Kuratomi (Fix extension writing code)     *
******************************************************************************/

#ifndef _GIF_LIB_H
#define _GIF_LIB_H

#   include	<sioGeneral.h>
#   include	<bmcolor.h>

/*************************/
/*************************/
/*************************/

# define EGifCloseFile bm_EGifCloseFile
# define EGifOpenFileHandle bm_EGifOpenFileHandle
# define EGifPutComment bm_EGifPutComment
# define EGifPutExtension bm_EGifPutExtension
# define EGifPutExtensionFirst bm_EGifPutExtensionFirst
# define EGifPutExtensionLast bm_EGifPutExtensionLast
# define EGifPutExtensionNext bm_EGifPutExtensionNext
# define EGifPutImageDesc bm_EGifPutImageDesc
# define EGifPutScreenDesc bm_EGifPutScreenDesc
# define EGifSpew bm_EGifSpew

# define DGifCloseFile bm_DGifCloseFile
# define DGifOpenFileHandle bm_DGifOpenFileHandle
# define DGifGetExtension bm_DGifGetExtension
# define DGifGetExtensionNext bm_DGifGetExtensionNext
# define DGifGetImageDesc bm_DGifGetImageDesc
# define bmGifGetRecordType bm_DGifGetRecordType

# define _GifError bm__GifError

/*************************/
/*************************/
/*************************/

#define	GIF_ERROR	0
#define GIF_OK		1

#define GIF87_STAMP	"GIF87a"        /* First chars in file - GIF stamp.  */
#define GIF89_STAMP	"GIF89a"        /* First chars in file - GIF stamp.  */

typedef	unsigned char	GifPixelType;
typedef unsigned char	GifByteType;

typedef struct GifColorMap
    {
    int		gcmColorCount;
    int		gcmBitsPerPixel;
    RGB8Color	gcmColors[256];
    } GifColorMap;

typedef struct GifImageDesc
    {
    int		Left;
    int		Top;
    int		Width;
    int		Height;
    int		Interlace;		/*  Sequential/Interlaced lines	*/
    GifColorMap	gidImageColorMap;
    } GifImageDesc;

typedef struct GifScreenDescriptor
    {
    int			gsdScreenWide;
    int			gsdScreenHigh;
    int			gsdScreenBitsPerPixel;

    unsigned char	gsdPackedFields;

    int			gsdScreenBackgroundColor;
    int			gsdScreenAspectRatio;

    GifColorMap		gsdScreenColorMap;
    } GifScreenDescriptor;

typedef struct GifFileType
    {
    char			gftVersionString[6+1];
    GifScreenDescriptor		gftScreenDescriptor;
    GifImageDesc		gftCurrentImageDescriptor;

    /**/
    SimpleOutputStream *	gftSos;
    SimpleOutputStream *	gftSosBlocked;
    SimpleOutputStream *	gftSosLzw;

    SimpleInputStream *		gftSis;
    SimpleInputStream *		gftSisBlocked;
    SimpleInputStream *		gftSisLzw;

    long			gftPixelCount;
				    /************************************/
				    /*  Number of pixels that has not	*/
				    /*  yet been written to the current	*/
				    /*  image.				*/
				    /*  OR				*/
				    /*  Number of pixels that has not	*/
				    /*  yet been read from the current	*/
				    /*  image.				*/
				    /************************************/
    } GifFileType;

typedef enum GifRecordType
    {
    UNDEFINED_RECORD_TYPE,
    SCREEN_DESC_RECORD_TYPE,
    IMAGE_DESC_RECORD_TYPE,		/* Begin with ',' */
    EXTENSION_RECORD_TYPE,		/* Begin with '!' */
    TERMINATE_RECORD_TYPE		/* Begin with ';' */
    } GifRecordType;

extern void bmGifInitGifColorMap(      GifColorMap *   gcm );

/******************************************************************************
*  GIF89 extension function codes                                             *
******************************************************************************/

#define COMMENT_EXT_FUNC_CODE		0xfe	/* comment */
#define GRAPHICS_EXT_FUNC_CODE		0xf9	/* graphics control */
#define PLAINTEXT_EXT_FUNC_CODE		0x01	/* plaintext */
#define APPLICATION_EXT_FUNC_CODE	0xff	/* application block */

/******************************************************************************
* O.K., here are the routines one can access in order to encode GIF file:     *
* (GIF_LIB file EGIF_LIB.C).						      *
******************************************************************************/

extern GifFileType *EGifOpenFileHandle(	SimpleOutputStream *	sos );

extern void bmGifSetVersion(		GifFileType *		gft,
					const char *		version );

int EGifPutScreenDesc(GifFileType *GifFile,
	int GifWidth, int GifHeight, int GifColorRes, int GifBackGround,
	const GifColorMap *gcm);
int EGifPutImageDesc(GifFileType *GifFile,
	int GifLeft, int GifTop, int Width, int GifHeight, int GifInterlace,
	const GifColorMap * gcm);
int bmGifPutPixels(GifFileType *GifFile, const GifPixelType *GifLine, int GifLineLen);
int EGifPutComment(GifFileType *GifFile, const char *GifComment);
int EGifPutExtensionFirst(GifFileType *GifFile, int GifExtCode, int GifExtLen,
                           const void * GifExtension);
int EGifPutExtensionNext(GifFileType *GifFile, int GifExtCode, int GifExtLen,
                           const void * GifExtension);
int EGifPutExtensionLast(GifFileType *GifFile, int GifExtCode, int GifExtLen,
                           const void * GifExtension);
int EGifPutExtension(GifFileType *GifFile, int GifExtCode, int GifExtLen,
							const void * GifExtension);

int EGifCloseFile(GifFileType *GifFile);

#define	E_GIF_ERR_OPEN_FAILED	1		/* And EGif possible errors. */
#define	E_GIF_ERR_WRITE_FAILED	2
#define E_GIF_ERR_HAS_SCRN_DSCR	3
#define E_GIF_ERR_HAS_IMAG_DSCR	4
#define E_GIF_ERR_NO_COLOR_MAP	5
#define E_GIF_ERR_DATA_TOO_BIG	6
#define E_GIF_ERR_NOT_ENOUGH_MEM 7
#define E_GIF_ERR_DISK_IS_FULL	8
#define E_GIF_ERR_CLOSE_FAILED	9
#define E_GIF_ERR_NOT_WRITEABLE	10

/******************************************************************************
* O.K., here are the routines one can access in order to decode GIF file:     *
* (GIF_LIB file DGIF_LIB.C).						      *
******************************************************************************/

GifFileType *DGifOpenFileHandle(	SimpleInputStream *	sis );
int bmGifGetRecordType(GifFileType *GifFile, GifRecordType *GifType);
int DGifGetImageDesc(GifFileType *GifFile);

extern int bmGifGetPixels(	GifFileType *	GifFile,
				GifPixelType *	GifLine,
				int		GifLineLen );

int DGifGetComment(GifFileType *GifFile, char *GifComment);
int DGifCloseFile(GifFileType *GifFile);

extern int DGifGetExtension(		GifFileType *	GifFile,
					int *		ExtCode,
					GifByteType	Extension[256] );

extern int DGifGetExtensionNext(	GifFileType *	GifFile,
					int *		pGot,
					GifByteType	Extension[256] );

#define	D_GIF_ERR_OPEN_FAILED	101		/* And DGif possible errors. */
#define	D_GIF_ERR_READ_FAILED	102
#define	D_GIF_ERR_NOT_GIF_FILE	103
#define D_GIF_ERR_NO_SCRN_DSCR	104
#define D_GIF_ERR_NO_IMAG_DSCR	105
#define D_GIF_ERR_NO_COLOR_MAP	106
#define D_GIF_ERR_WRONG_RECORD	107
#define D_GIF_ERR_DATA_TOO_BIG	108
#define D_GIF_ERR_NOT_ENOUGH_MEM 109
#define D_GIF_ERR_CLOSE_FAILED	110
#define D_GIF_ERR_NOT_READABLE	111
#define D_GIF_ERR_IMAGE_DEFECT	112
#define D_GIF_ERR_EOF_TOO_SOON	113

extern int _GifError;

#endif /* _GIF_LIB_H */
