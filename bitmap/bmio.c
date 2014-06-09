#   include	"bitmapConfig.h"

#   include	<string.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	"bitmap.h"
#   include	<appDebugon.h>

int bmWrite(	const char *			filename,
		const unsigned char *		buffer,
		const BitmapDescription *	bd,
		int				fileFormat,
		double				compressionFactor )
    {
    if  ( ! bmFileFormats[fileFormat].bffFileType->bftWrite )
	{
	LLDEB(fileFormat,bmFileFormats[fileFormat].bffFileType->bftWrite);
	return -1;
	}

    return (*bmFileFormats[fileFormat].bffFileType->bftWrite)(
			filename,
			buffer,
			bd,
			bmFileFormats[fileFormat].bffPrivate,
			compressionFactor			);
    }

int bmCanWrite( const BitmapDescription *	bd,
		int				fileFormat,
		double				compressionFactor )
    {
    if  ( ! bmFileFormats[fileFormat].bffFileType->bftWrite )
	{ return -1;	}

    return (*bmFileFormats[fileFormat].bffFileType->bftCanWrite)(
			bd,
			bmFileFormats[fileFormat].bffPrivate,
			compressionFactor			);
    }

/************************************************************************/
/*									*/
/*  Read a bitmap/pixmap file. Derive the type from the file name.	*/
/*									*/
/************************************************************************/

int bmRead(	const char *		filename,
		unsigned char **	pBuffer,
		BitmapDescription *	bd,
		int *			pFileFormat,
		double *		pCompressionFactor )
    {
    int		fileType;
    int		privateType;
    int		fileFormat;
    char *	extension= strrchr( filename, '.' );

    if  ( ! extension )
	{ LDEB(extension); return -1; }
    extension++;

    for ( fileType= 0; fileType < bmNumberOfFileTypes; fileType++ )
	{
	if  ( ! bmFileTypes[fileType]->bftRead )
	    { continue;	}

	if  ( ! strcmp( extension, bmFileTypes[fileType]->bftFileExtension ) )
	    {
	    unsigned char *	buffer;

	    if  ( (*bmFileTypes[fileType]->bftRead)(	filename,
							&buffer,
							bd,
							&privateType,
							pCompressionFactor ) )
		{ LDEB(fileType); return -1;	}

	    for (	fileFormat= 0;
			fileFormat < bmNumberOfFileFormats;
			fileFormat++				)
		{
		if  ( bmFileFormats[fileFormat].bffFileType->bftRead	==
		      bmFileTypes[fileType]->bftRead			&&
		      bmFileFormats[fileFormat].bffPrivate	==
		      privateType					)
		    { *pBuffer= buffer; *pFileFormat= fileFormat; return 0; }
		}

	    LDEB(privateType); free( buffer ); return -1;
	    }
	}

    SDEB(extension); return -1;
    }
