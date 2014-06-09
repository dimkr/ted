#   include	"bitmapConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	"bmintern.h"
#   include	<utilEndian.h>
#   include	<sioStdio.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Read an SGI Image file.						*/
/*									*/
/************************************************************************/

typedef struct SgiImageHeader
    {
    short	sihMagic;
    char	sihStorage;
    char	sihBytesPerPixel;
    int		sihDimensionCount;
    int		sihPixelsWide;
    int		sihPixelsHigh;
    int		sihPlaneCount;
    long	sihMinimumPixelValue;
    long	sihMaximumPixelValue;
    char	sihImageName[79+1];
    long	sihKindOfImage;
    } SgiImageHeader;

int bmReadSgiFile(	const char *		filename,
			unsigned char **	pBuf,
			BitmapDescription *	bd,
			int *			pPrivateFormat,
			double *		pCompressionFactor	)
    {
    SimpleInputStream *		sis;

    sis= sioInStdioOpen( filename );
    if  ( ! sis )
	{ SXDEB(filename,sis); return -1;	}

    LDEB(1); return -1;
    }

/************************************************************************/
/*									*/
/*  Write an SGI Image file.						*/
/*									*/
/************************************************************************/
int bmWriteSgiFile(	const char *			filename,
			const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				privateFormat,
			double				compressionFactor )
    { LDEB(-1); return -1; }
