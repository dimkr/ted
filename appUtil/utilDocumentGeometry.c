#   include	"appUtilConfig.h"

#   include	<string.h>

#   include	"utilDocumentGeometry.h"
#   include	"utilPropMask.h"

/************************************************************************/
/*									*/
/*  Set default document geometry.					*/
/*									*/
/************************************************************************/

void utilInitDocumentGeometry(	DocumentGeometry *	dg )
    {
    dg->dgPageWideTwips= 12240;
    dg->dgPageHighTwips= 15840;

    dg->dgLeftMarginTwips= 1800;
    dg->dgTopMarginTwips= 1440;
    dg->dgRightMarginTwips= 1800;
    dg->dgBottomMarginTwips= 1440;

    dg->dgHeaderPositionTwips= 720;
    dg->dgFooterPositionTwips= 720;

    dg->dgGutterTwips= 0;
    }

/************************************************************************/
/*									*/
/*  Copy a document Geometry.						*/
/*									*/
/************************************************************************/

void utilUpdDocumentGeometry(	DocumentGeometry *		dgTo,
				const DocumentGeometry *	dgFr,
				PropertyMask *			pChanged,
				const PropertyMask *		updMask )
    {
    PropertyMask		changedMask;

    PROPmaskCLEAR( &changedMask );

    if  ( PROPmaskISSET( updMask, DGpropPAGE_WIDTH ) )
	{
	if  ( dgTo->dgPageWideTwips != dgFr->dgPageWideTwips )
	    {
	    dgTo->dgPageWideTwips= dgFr->dgPageWideTwips;
	    PROPmaskADD( &changedMask, DGpropPAGE_WIDTH );
	    }
	}

    if  ( PROPmaskISSET( updMask, DGpropPAGE_HEIGHT ) )
	{
	if  ( dgTo->dgPageHighTwips != dgFr->dgPageHighTwips )
	    {
	    dgTo->dgPageHighTwips= dgFr->dgPageHighTwips;
	    PROPmaskADD( &changedMask, DGpropPAGE_HEIGHT );
	    }
	}

    if  ( PROPmaskISSET( updMask, DGpropLEFT_MARGIN ) )
	{
	if  ( dgTo->dgLeftMarginTwips != dgFr->dgLeftMarginTwips )
	    {
	    dgTo->dgLeftMarginTwips= dgFr->dgLeftMarginTwips;
	    PROPmaskADD( &changedMask, DGpropLEFT_MARGIN );
	    }
	}

    if  ( PROPmaskISSET( updMask, DGpropRIGHT_MARGIN ) )
	{
	if  ( dgTo->dgRightMarginTwips != dgFr->dgRightMarginTwips )
	    {
	    dgTo->dgRightMarginTwips= dgFr->dgRightMarginTwips;
	    PROPmaskADD( &changedMask, DGpropRIGHT_MARGIN );
	    }
	}

    if  ( PROPmaskISSET( updMask, DGpropTOP_MARGIN ) )
	{
	if  ( dgTo->dgTopMarginTwips != dgFr->dgTopMarginTwips )
	    {
	    dgTo->dgTopMarginTwips= dgFr->dgTopMarginTwips;
	    PROPmaskADD( &changedMask, DGpropTOP_MARGIN );
	    }
	}

    if  ( PROPmaskISSET( updMask, DGpropBOTTOM_MARGIN ) )
	{
	if  ( dgTo->dgBottomMarginTwips != dgFr->dgBottomMarginTwips )
	    {
	    dgTo->dgBottomMarginTwips= dgFr->dgBottomMarginTwips;
	    PROPmaskADD( &changedMask, DGpropBOTTOM_MARGIN );
	    }
	}

    if  ( PROPmaskISSET( updMask, DGpropHEADER_POSITION ) )
	{
	if  ( dgTo->dgHeaderPositionTwips != dgFr->dgHeaderPositionTwips )
	    {
	    dgTo->dgHeaderPositionTwips= dgFr->dgHeaderPositionTwips;
	    PROPmaskADD( &changedMask, DGpropHEADER_POSITION );
	    }
	}

    if  ( PROPmaskISSET( updMask, DGpropFOOTER_POSITION ) )
	{
	if  ( dgTo->dgFooterPositionTwips != dgFr->dgFooterPositionTwips )
	    {
	    dgTo->dgFooterPositionTwips= dgFr->dgFooterPositionTwips;
	    PROPmaskADD( &changedMask, DGpropFOOTER_POSITION );
	    }
	}

    if  ( PROPmaskISSET( updMask, DGpropGUTTER ) )
	{
	if  ( dgTo->dgGutterTwips != dgFr->dgGutterTwips )
	    {
	    dgTo->dgGutterTwips= dgFr->dgGutterTwips;
	    PROPmaskADD( &changedMask, DGpropGUTTER );
	    }
	}

    *pChanged= changedMask;

    return;
    }

