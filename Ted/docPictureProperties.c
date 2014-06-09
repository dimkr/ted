/************************************************************************/
/*									*/
/*  Buffer administration: Properties of embedded images.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<utilPropMask.h>

#   include	"docObjectProperties.h"

#   include	<appDebugon.h>

void docInitPictureProperties(	PictureProperties *	pip )
    {
    pip->pipType= DOCokUNKNOWN;
    pip->pipMapMode= -1;

    pip->pip_xWinExt= 0;
    pip->pip_yWinExt= 0;

    pip->pipTwipsWide= 0;
    pip->pipTwipsHigh= 0;

    pip->pipScaleXSet= 100;
    pip->pipScaleYSet= 100;
    pip->pipScaleXUsed= 100;
    pip->pipScaleYUsed= 100;

    pip->pipTopCropTwips= 0;
    pip->pipBottomCropTwips= 0;
    pip->pipLeftCropTwips= 0;
    pip->pipRightCropTwips= 0;

    pip->pipBliptag= 0;

    pip->pipMetafileBitmapBpp= 0;
    pip->pipBmUnitsPerInch= 0;
    pip->pipBmBitsPerPixel= 0;
    pip->pipBmPlanes= 0;
    pip->pipBmBytesPerRow= 0;

    pip->pipPictureIsWordArt= 0;
    pip->pipMetafileIsBitmap= 0;
    pip->pipPictIsScaled= 0;

    return;
    }

int docUpdPictureProperties(	PropertyMask *			pDoneMask,
				PictureProperties *		pipTo,
				const PropertyMask *		pipUpdMask,
				const PictureProperties *	pipFrom )
    {
    PropertyMask		doneMask;

    PROPmaskCLEAR( &doneMask );

    if  ( PROPmaskISSET( pipUpdMask, PIPpropTYPE ) )
	{
	if  ( pipTo->pipType != pipFrom->pipType )
	    {
	    pipTo->pipType= pipFrom->pipType;
	    PROPmaskADD( &doneMask, PIPpropTYPE );
	    }
	if  ( pipTo->pipMapMode != pipFrom->pipMapMode )
	    {
	    pipTo->pipMapMode= pipFrom->pipMapMode;
	    PROPmaskADD( &doneMask, PIPpropTYPE );
	    }
	}

    if  ( PROPmaskISSET( pipUpdMask, PIPpropPICX_WIN_EXT ) )
	{
	if  ( pipTo->pip_xWinExt != pipFrom->pip_xWinExt )
	    {
	    pipTo->pip_xWinExt= pipFrom->pip_xWinExt;
	    PROPmaskADD( &doneMask, PIPpropPICX_WIN_EXT );
	    }
	}

    if  ( PROPmaskISSET( pipUpdMask, PIPpropPICY_WIN_EXT ) )
	{
	if  ( pipTo->pip_yWinExt != pipFrom->pip_yWinExt )
	    {
	    pipTo->pip_yWinExt= pipFrom->pip_yWinExt;
	    PROPmaskADD( &doneMask, PIPpropPICY_WIN_EXT );
	    }
	}

    if  ( PROPmaskISSET( pipUpdMask, PIPpropPICTWIPS_WIDE ) )
	{
	if  ( pipTo->pipTwipsWide != pipFrom->pipTwipsWide )
	    {
	    pipTo->pipTwipsWide= pipFrom->pipTwipsWide;
	    PROPmaskADD( &doneMask, PIPpropPICTWIPS_WIDE );
	    }
	}

    if  ( PROPmaskISSET( pipUpdMask, PIPpropPICTWIPS_HIGH ) )
	{
	if  ( pipTo->pipTwipsHigh != pipFrom->pipTwipsHigh )
	    {
	    pipTo->pipTwipsHigh= pipFrom->pipTwipsHigh;
	    PROPmaskADD( &doneMask, PIPpropPICTWIPS_HIGH );
	    }
	}

    if  ( PROPmaskISSET( pipUpdMask, PIPpropPICSCALE_X ) )
	{
	if  ( pipTo->pipScaleXSet != pipFrom->pipScaleXSet )
	    {
	    pipTo->pipScaleXSet= pipFrom->pipScaleXSet;
	    PROPmaskADD( &doneMask, PIPpropPICSCALE_X );
	    }
	}

    if  ( PROPmaskISSET( pipUpdMask, PIPpropPICSCALE_Y ) )
	{
	if  ( pipTo->pipScaleYSet != pipFrom->pipScaleYSet )
	    {
	    pipTo->pipScaleYSet= pipFrom->pipScaleYSet;
	    PROPmaskADD( &doneMask, PIPpropPICSCALE_Y );
	    }
	}

    if  ( PROPmaskISSET( pipUpdMask, PIPpropPICCROP_TOP ) )
	{
	if  ( pipTo->pipTopCropTwips != pipFrom->pipTopCropTwips )
	    {
	    pipTo->pipTopCropTwips= pipFrom->pipTopCropTwips;
	    PROPmaskADD( &doneMask, PIPpropPICCROP_TOP );
	    }
	}

    if  ( PROPmaskISSET( pipUpdMask, PIPpropPICCROP_BOTTOM ) )
	{
	if  ( pipTo->pipBottomCropTwips != pipFrom->pipBottomCropTwips )
	    {
	    pipTo->pipBottomCropTwips= pipFrom->pipBottomCropTwips;
	    PROPmaskADD( &doneMask, PIPpropPICCROP_BOTTOM );
	    }
	}

    if  ( PROPmaskISSET( pipUpdMask, PIPpropPICCROP_LEFT ) )
	{
	if  ( pipTo->pipLeftCropTwips != pipFrom->pipLeftCropTwips )
	    {
	    pipTo->pipLeftCropTwips= pipFrom->pipLeftCropTwips;
	    PROPmaskADD( &doneMask, PIPpropPICCROP_LEFT );
	    }
	}

    if  ( PROPmaskISSET( pipUpdMask, PIPpropPICCROP_RIGHT ) )
	{
	if  ( pipTo->pipRightCropTwips != pipFrom->pipRightCropTwips )
	    {
	    pipTo->pipRightCropTwips= pipFrom->pipRightCropTwips;
	    PROPmaskADD( &doneMask, PIPpropPICCROP_RIGHT );
	    }
	}

    if  ( PROPmaskISSET( pipUpdMask, PIPpropBLIPTAG ) )
	{
	if  ( pipTo->pipBliptag != pipFrom->pipBliptag )
	    {
	    pipTo->pipBliptag= pipFrom->pipBliptag;
	    PROPmaskADD( &doneMask, PIPpropBLIPTAG );
	    }
	}

    if  ( PROPmaskISSET( pipUpdMask, PIPpropPICBPP ) )
	{
	if  ( pipTo->pipMetafileBitmapBpp != pipFrom->pipMetafileBitmapBpp )
	    {
	    pipTo->pipMetafileBitmapBpp= pipFrom->pipMetafileBitmapBpp;
	    PROPmaskADD( &doneMask, PIPpropPICBPP );
	    }
	}

    if  ( PROPmaskISSET( pipUpdMask, PIPpropWBMBITSPIXEL ) )
	{
	if  ( pipTo->pipBmBitsPerPixel != pipFrom->pipBmBitsPerPixel )
	    {
	    pipTo->pipBmBitsPerPixel= pipFrom->pipBmBitsPerPixel;
	    PROPmaskADD( &doneMask, PIPpropWBMBITSPIXEL );
	    }
	}

    if  ( PROPmaskISSET( pipUpdMask, PIPpropWBMPLANES ) )
	{
	if  ( pipTo->pipBmPlanes != pipFrom->pipBmPlanes )
	    {
	    pipTo->pipBmPlanes= pipFrom->pipBmPlanes;
	    PROPmaskADD( &doneMask, PIPpropWBMPLANES );
	    }
	}

    if  ( PROPmaskISSET( pipUpdMask, PIPpropWBMWIDTHBYTES ) )
	{
	if  ( pipTo->pipBmBytesPerRow != pipFrom->pipBmBytesPerRow )
	    {
	    pipTo->pipBmBytesPerRow= pipFrom->pipBmBytesPerRow;
	    PROPmaskADD( &doneMask, PIPpropWBMWIDTHBYTES );
	    }
	}

    if  ( PROPmaskISSET( pipUpdMask, PIPpropDEFSHP ) )
	{
	if  ( pipTo->pipPictureIsWordArt != pipFrom->pipPictureIsWordArt )
	    {
	    pipTo->pipPictureIsWordArt= pipFrom->pipPictureIsWordArt;
	    PROPmaskADD( &doneMask, PIPpropDEFSHP );
	    }
	}

    if  ( PROPmaskISSET( pipUpdMask, PIPpropPICBMP ) )
	{
	if  ( pipTo->pipMetafileIsBitmap != pipFrom->pipMetafileIsBitmap )
	    {
	    pipTo->pipMetafileIsBitmap= pipFrom->pipMetafileIsBitmap;
	    PROPmaskADD( &doneMask, PIPpropPICBMP );
	    }
	}

    if  ( PROPmaskISSET( pipUpdMask, PIPpropPICSCALED ) )
	{
	if  ( pipTo->pipPictIsScaled != pipFrom->pipPictIsScaled )
	    {
	    pipTo->pipPictIsScaled= pipFrom->pipPictIsScaled;
	    PROPmaskADD( &doneMask, PIPpropPICSCALED );
	    }
	}

    *pDoneMask= doneMask; return 0;
    }
