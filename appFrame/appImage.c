/************************************************************************/
/*  Scan, main module.							*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	"appImage.h"
#   include	<appFrame.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Clean an image structure.						*/
/*									*/
/************************************************************************/

void appCleanBitmapImage(	AppBitmapImage *	abi )
    {
    if  ( abi->abiBuffer )
	{
	free( abi->abiBuffer );
	abi->abiBuffer= (unsigned char *)0;
	}

    bmCleanDescription( &abi->abiBitmap );
    bmInitDescription( &abi->abiBitmap );
    }

void appInitBitmapImage(	AppBitmapImage *	abi )
    {
    bmInitDescription( &abi->abiBitmap );
    abi->abiBuffer= (unsigned char *)0;
    abi->abiFormat= -1;
    abi->abiFactor= 10;
    }

/************************************************************************/
/*									*/
/*  Make the list of bitmap FileExtensions that can be opened/saved.	*/
/*									*/
/************************************************************************/

int appImgMakeFileExtensions(	AppFileExtension **	pAfeList,
				int *			pAfeCount )
    {
    int				i;
    AppFileExtension *		afe;
    int				afeCount;

    const BitmapFileFormat *	bff;
    BitmapFileType **		pbft;

    afeCount=  bmNumberOfFileTypes+ bmNumberOfFileFormats;
    afe= (AppFileExtension *)malloc( afeCount * sizeof(AppFileExtension) );
    if  ( ! afe )
	{ LXDEB(afeCount,afe); return -1;	}

    afeCount= 0;

    bff= bmFileFormats;
    for ( i= 0; i < bmNumberOfFileFormats; bff++, i++ )
	{
	unsigned	flags= APPFILE_IS_BASIC_TYPE;

	afe[afeCount].afeId= bff->bffId;
	afe[afeCount].afeFilter= bff->bffFileType->bftFileFilter;
	afe[afeCount].afeDescription= bff->bffDescription;
	afe[afeCount].afeExtension= bff->bffFileType->bftFileExtension;

	if  ( bff->bffFileType->bftRead )
	    { flags |= APPFILE_CAN_OPEN;	}
	if  ( bff->bffFileType->bftWrite )
	    { flags |= APPFILE_CAN_SAVE;	}

	afe[afeCount].afeUseFlags= flags;

	afeCount++;
	}

    pbft= bmFileTypes;
    for ( i= 0; i < bmNumberOfFileTypes; pbft++, i++ )
	{
	afe[afeCount].afeId= (*pbft)->bftTypeId;
	afe[afeCount].afeFilter= (*pbft)->bftFileFilter;
	afe[afeCount].afeDescription= (*pbft)->bftTypeDescription;
	afe[afeCount].afeExtension= (*pbft)->bftFileExtension;

	if  ( ! (*pbft)->bftRead )
	    { afe[afeCount].afeUseFlags= 0;			}
	else{ afe[afeCount].afeUseFlags= APPFILE_CAN_OPEN;	}

	afeCount++;
	}

    *pAfeList= afe;
    *pAfeCount= afeCount;

    return 0;
    }
