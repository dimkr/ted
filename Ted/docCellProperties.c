/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Initialise Cell properties.						*/
/*									*/
/************************************************************************/

void docInitCellProperties(	CellProperties *	cp )
    {
    cp->cpRightBoundaryTwips= 0;
    cp->cpRightBoundaryPixels= 0;

    docInitBorderProperties( &(cp->cpTopBorder) );
    docInitBorderProperties( &(cp->cpLeftBorder) );
    docInitBorderProperties( &(cp->cpRightBorder) );
    docInitBorderProperties( &(cp->cpBottomBorder) );

    docInitItemShading( &(cp->cpShading) );

    /**/
    cp->cpPreferredWidth= 0;
    cp->cpPreferredWidthUnit= TRautoNONE;

    cp->cpTopPadding= 0;
    cp->cpBottomPadding= 0;
    cp->cpLeftPadding= 0;
    cp->cpRightPadding= 0;

    cp->cpTopPaddingUnit= TRautoNONE;
    cp->cpBottomPaddingUnit= TRautoNONE;
    cp->cpLeftPaddingUnit= TRautoNONE;
    cp->cpRightPaddingUnit= TRautoNONE;

    cp->cpLeftInMergedRow= 0;
    cp->cpMergedWithLeft= 0;
    cp->cpTopInMergedColumn= 0;
    cp->cpMergedWithAbove= 0;
    cp->cpNoWrap= 0;
    cp->cpNoShading= 0;

    cp->cpTextVerticalAlignment= DOCtvaTOP;
    cp->cpTextFlow= CLflowTXLRTB;
    }

/************************************************************************/
/*									*/
/*  Change cell properties and tell what has been changed.		*/
/*									*/
/************************************************************************/

int docUpdCellProperties(	PropertyMask *			pCpChgPask,
				CellProperties *		cpTo,
				const PropertyMask *		cpUpdMask,
				const CellProperties *		cpFrom,
				const int *			colorMap )
    {
    PropertyMask		cpChgMask;
    PropertyMask		isUpdMask;

    PROPmaskCLEAR( &cpChgMask );

    if  ( PROPmaskISSET( cpUpdMask, CLpropRIGHT_BOUNDARY ) )
	{
	if  ( cpTo->cpRightBoundaryTwips != cpFrom->cpRightBoundaryTwips )
	    {
	    cpTo->cpRightBoundaryTwips= cpFrom->cpRightBoundaryTwips;
	    PROPmaskADD( &cpChgMask, CLpropRIGHT_BOUNDARY );
	    }
	}

    /**/
    if  ( PROPmaskISSET( cpUpdMask, CLpropTOP_BORDER ) )
	{
	if  ( docBordersDiffer( &(cpTo->cpTopBorder),
					&(cpFrom->cpTopBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(cpTo->cpTopBorder),
					&(cpFrom->cpTopBorder), colorMap );
	    PROPmaskADD( &cpChgMask, CLpropTOP_BORDER );
	    }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropBOTTOM_BORDER ) )
	{
	if  ( docBordersDiffer( &(cpTo->cpBottomBorder),
					&(cpFrom->cpBottomBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(cpTo->cpBottomBorder),
					&(cpFrom->cpBottomBorder), colorMap );
	    PROPmaskADD( &cpChgMask, CLpropBOTTOM_BORDER );
	    }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropLEFT_BORDER ) )
	{
	if  ( docBordersDiffer( &(cpTo->cpLeftBorder),
					&(cpFrom->cpLeftBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(cpTo->cpLeftBorder),
					&(cpFrom->cpLeftBorder), colorMap );
	    PROPmaskADD( &cpChgMask, CLpropLEFT_BORDER );
	    }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropRIGHT_BORDER ) )
	{
	if  ( docBordersDiffer( &(cpTo->cpRightBorder),
					&(cpFrom->cpRightBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(cpTo->cpRightBorder),
					&(cpFrom->cpRightBorder), colorMap );
	    PROPmaskADD( &cpChgMask, CLpropRIGHT_BORDER );
	    }
	}

    docShadingMaskFromCellMask( &isUpdMask, cpUpdMask );
    if  ( ! utilPropMaskIsEmpty( &isUpdMask ) )
	{
	PropertyMask	isChgMask;

	PROPmaskCLEAR( &isChgMask );

	docUpdateItemShading( &isChgMask, &(cpTo->cpShading),
				&isUpdMask, &(cpFrom->cpShading), colorMap );

	docShadingMaskToCellMask( &isChgMask, &isChgMask );
	utilPropMaskOr( &cpChgMask, &cpChgMask, &isChgMask );
	}


    /**/
    if  ( PROPmaskISSET( cpUpdMask, CLpropCLW_WIDTH ) )
	{
	if  ( cpTo->cpPreferredWidth != cpFrom->cpPreferredWidth )
	    {
	    cpTo->cpPreferredWidth= cpFrom->cpPreferredWidth;
	    PROPmaskADD( &cpChgMask, CLpropCLW_WIDTH );
	    }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropCLFTS_WIDTH ) )
	{
	if  ( cpTo->cpPreferredWidthUnit != cpFrom->cpPreferredWidthUnit )
	    {
	    cpTo->cpPreferredWidthUnit= cpFrom->cpPreferredWidthUnit;
	    PROPmaskADD( &cpChgMask, CLpropCLFTS_WIDTH );
	    }
	}

    /**/
    if  ( PROPmaskISSET( cpUpdMask, CLpropLEFT_IN_MERGED ) )
	{
	if  ( cpTo->cpLeftInMergedRow != cpFrom->cpLeftInMergedRow )
	    {
	    cpTo->cpLeftInMergedRow= cpFrom->cpLeftInMergedRow;
	    PROPmaskADD( &cpChgMask, CLpropLEFT_IN_MERGED );
	    }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropMERGED_WITH_LEFT ) )
	{
	if  ( cpTo->cpMergedWithLeft != cpFrom->cpMergedWithLeft )
	    {
	    cpTo->cpMergedWithLeft= cpFrom->cpMergedWithLeft;
	    PROPmaskADD( &cpChgMask, CLpropMERGED_WITH_LEFT );
	    }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropTOP_IN_MERGED ) )
	{
	if  ( cpTo->cpTopInMergedColumn != cpFrom->cpTopInMergedColumn )
	    {
	    cpTo->cpTopInMergedColumn= cpFrom->cpTopInMergedColumn;
	    PROPmaskADD( &cpChgMask, CLpropTOP_IN_MERGED );
	    }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropMERGED_WITH_ABOVE ) )
	{
	if  ( cpTo->cpMergedWithAbove != cpFrom->cpMergedWithAbove )
	    {
	    cpTo->cpMergedWithAbove= cpFrom->cpMergedWithAbove;
	    PROPmaskADD( &cpChgMask, CLpropMERGED_WITH_ABOVE );
	    }
	}

    /**/
    if  ( PROPmaskISSET( cpUpdMask, CLpropTEXT_VERT_ALIGN ) )
	{
	if  ( cpTo->cpTextVerticalAlignment != cpFrom->cpTextVerticalAlignment )
	    {
	    cpTo->cpTextVerticalAlignment= cpFrom->cpTextVerticalAlignment;
	    PROPmaskADD( &cpChgMask, CLpropTEXT_VERT_ALIGN );
	    }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropTEXT_FLOW ) )
	{
	if  ( cpTo->cpTextFlow != cpFrom->cpTextFlow )
	    {
	    cpTo->cpTextFlow= cpFrom->cpTextFlow;
	    PROPmaskADD( &cpChgMask, CLpropTEXT_FLOW );
	    }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropTOP_PADDING ) )
	{
	if  ( cpTo->cpTopPadding != cpFrom->cpTopPadding )
	    {
	    cpTo->cpTopPadding= cpFrom->cpTopPadding;
	    PROPmaskADD( &cpChgMask, CLpropTOP_PADDING );
	    }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropBOTTOM_PADDING ) )
	{
	if  ( cpTo->cpBottomPadding != cpFrom->cpBottomPadding )
	    {
	    cpTo->cpBottomPadding= cpFrom->cpBottomPadding;
	    PROPmaskADD( &cpChgMask, CLpropBOTTOM_PADDING );
	    }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropLEFT_PADDING ) )
	{
	if  ( cpTo->cpLeftPadding != cpFrom->cpLeftPadding )
	    {
	    cpTo->cpLeftPadding= cpFrom->cpLeftPadding;
	    PROPmaskADD( &cpChgMask, CLpropLEFT_PADDING );
	    }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropRIGHT_PADDING ) )
	{
	if  ( cpTo->cpRightPadding != cpFrom->cpRightPadding )
	    {
	    cpTo->cpRightPadding= cpFrom->cpRightPadding;
	    PROPmaskADD( &cpChgMask, CLpropRIGHT_PADDING );
	    }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropTOP_PADDING_UNIT ) )
	{
	if  ( cpTo->cpTopPaddingUnit != cpFrom->cpTopPaddingUnit )
	    {
	    cpTo->cpTopPaddingUnit= cpFrom->cpTopPaddingUnit;
	    PROPmaskADD( &cpChgMask, CLpropTOP_PADDING_UNIT );
	    }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropBOTTOM_PADDING_UNIT ) )
	{
	if  ( cpTo->cpBottomPaddingUnit != cpFrom->cpBottomPaddingUnit )
	    {
	    cpTo->cpBottomPaddingUnit= cpFrom->cpBottomPaddingUnit;
	    PROPmaskADD( &cpChgMask, CLpropBOTTOM_PADDING_UNIT );
	    }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropLEFT_PADDING_UNIT ) )
	{
	if  ( cpTo->cpLeftPaddingUnit != cpFrom->cpLeftPaddingUnit )
	    {
	    cpTo->cpLeftPaddingUnit= cpFrom->cpLeftPaddingUnit;
	    PROPmaskADD( &cpChgMask, CLpropLEFT_PADDING_UNIT );
	    }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropRIGHT_PADDING_UNIT ) )
	{
	if  ( cpTo->cpRightPaddingUnit != cpFrom->cpRightPaddingUnit )
	    {
	    cpTo->cpRightPaddingUnit= cpFrom->cpRightPaddingUnit;
	    PROPmaskADD( &cpChgMask, CLpropRIGHT_PADDING_UNIT );
	    }
	}

    *pCpChgPask= cpChgMask; return 0;
    }

/************************************************************************/
/*									*/
/*  Compare cell properties.						*/
/*									*/
/************************************************************************/

void docCellPropertyDifference(	PropertyMask *			pCpChgPask,
				const CellProperties *		cpTo,
				const CellProperties *		cpFrom,
				const PropertyMask *		cpUpdMask,
				const int *			colorMap )
    {
    PropertyMask		cpChgMask;
    PropertyMask		isUpdMask;

    PROPmaskCLEAR( &cpChgMask );

    if  ( PROPmaskISSET( cpUpdMask, CLpropRIGHT_BOUNDARY ) )
	{
	if  ( cpTo->cpRightBoundaryTwips != cpFrom->cpRightBoundaryTwips )
	    { PROPmaskADD( &cpChgMask, CLpropRIGHT_BOUNDARY ); }
	}

    /**/
    if  ( PROPmaskISSET( cpUpdMask, CLpropTOP_BORDER ) )
	{
	if  ( docBordersDiffer( &(cpTo->cpTopBorder),
					&(cpFrom->cpTopBorder), colorMap ) )
	    { PROPmaskADD( &cpChgMask, CLpropTOP_BORDER ); }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropBOTTOM_BORDER ) )
	{
	if  ( docBordersDiffer( &(cpTo->cpBottomBorder),
					&(cpFrom->cpBottomBorder), colorMap ) )
	    { PROPmaskADD( &cpChgMask, CLpropBOTTOM_BORDER ); }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropLEFT_BORDER ) )
	{
	if  ( docBordersDiffer( &(cpTo->cpLeftBorder),
					&(cpFrom->cpLeftBorder), colorMap ) )
	    { PROPmaskADD( &cpChgMask, CLpropLEFT_BORDER ); }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropRIGHT_BORDER ) )
	{
	if  ( docBordersDiffer( &(cpTo->cpRightBorder),
					&(cpFrom->cpRightBorder), colorMap ) )
	    { PROPmaskADD( &cpChgMask, CLpropRIGHT_BORDER ); }
	}

    /**/
    docShadingMaskFromCellMask( &isUpdMask, cpUpdMask );
    if  ( ! utilPropMaskIsEmpty( &isUpdMask ) )
	{
	PropertyMask	isChgMask;

	PROPmaskCLEAR( &isChgMask );

	docItemShadingDifference( &isChgMask, &(cpTo->cpShading),
				&(cpFrom->cpShading), &isUpdMask, colorMap );

	docShadingMaskToCellMask( &isChgMask, &isChgMask );
	utilPropMaskOr( &cpChgMask, &cpChgMask, &isChgMask );
	}

    /**/
    if  ( PROPmaskISSET( cpUpdMask, CLpropCLW_WIDTH ) )
	{
	if  ( cpTo->cpPreferredWidth != cpFrom->cpPreferredWidth )
	    { PROPmaskADD( &cpChgMask, CLpropCLW_WIDTH );	}
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropCLFTS_WIDTH ) )
	{
	if  ( cpTo->cpPreferredWidthUnit != cpFrom->cpPreferredWidthUnit )
	    { PROPmaskADD( &cpChgMask, CLpropCLFTS_WIDTH );	}
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropLEFT_IN_MERGED ) )
	{
	if  ( cpTo->cpLeftInMergedRow != cpFrom->cpLeftInMergedRow )
	    { PROPmaskADD( &cpChgMask, CLpropLEFT_IN_MERGED ); }
	}

    /**/
    if  ( PROPmaskISSET( cpUpdMask, CLpropMERGED_WITH_LEFT ) )
	{
	if  ( cpTo->cpMergedWithLeft != cpFrom->cpMergedWithLeft )
	    { PROPmaskADD( &cpChgMask, CLpropMERGED_WITH_LEFT ); }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropTOP_IN_MERGED ) )
	{
	if  ( cpTo->cpTopInMergedColumn != cpFrom->cpTopInMergedColumn )
	    { PROPmaskADD( &cpChgMask, CLpropTOP_IN_MERGED ); }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropMERGED_WITH_ABOVE ) )
	{
	if  ( cpTo->cpMergedWithAbove != cpFrom->cpMergedWithAbove )
	    {
	    PROPmaskADD( &cpChgMask, CLpropMERGED_WITH_ABOVE );
	    }
	}

    /**/
    if  ( PROPmaskISSET( cpUpdMask, CLpropTEXT_VERT_ALIGN ) )
	{
	if  ( cpTo->cpTextVerticalAlignment != cpFrom->cpTextVerticalAlignment )
	    { PROPmaskADD( &cpChgMask, CLpropTEXT_VERT_ALIGN ); }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropTEXT_FLOW ) )
	{
	if  ( cpTo->cpTextFlow != cpFrom->cpTextFlow )
	    { PROPmaskADD( &cpChgMask, CLpropTEXT_FLOW ); }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropTOP_PADDING ) )
	{
	if  ( cpTo->cpTopPadding != cpFrom->cpTopPadding )
	    { PROPmaskADD( &cpChgMask, CLpropTOP_PADDING ); }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropBOTTOM_PADDING ) )
	{
	if  ( cpTo->cpBottomPadding != cpFrom->cpBottomPadding )
	    { PROPmaskADD( &cpChgMask, CLpropBOTTOM_PADDING ); }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropLEFT_PADDING ) )
	{
	if  ( cpTo->cpLeftPadding != cpFrom->cpLeftPadding )
	    { PROPmaskADD( &cpChgMask, CLpropLEFT_PADDING ); }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropRIGHT_PADDING ) )
	{
	if  ( cpTo->cpRightPadding != cpFrom->cpRightPadding )
	    { PROPmaskADD( &cpChgMask, CLpropRIGHT_PADDING ); }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropTOP_PADDING_UNIT ) )
	{
	if  ( cpTo->cpTopPaddingUnit != cpFrom->cpTopPaddingUnit )
	    { PROPmaskADD( &cpChgMask, CLpropTOP_PADDING_UNIT ); }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropBOTTOM_PADDING_UNIT ) )
	{
	if  ( cpTo->cpBottomPaddingUnit != cpFrom->cpBottomPaddingUnit )
	    { PROPmaskADD( &cpChgMask, CLpropBOTTOM_PADDING_UNIT ); }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropLEFT_PADDING_UNIT ) )
	{
	if  ( cpTo->cpLeftPaddingUnit != cpFrom->cpLeftPaddingUnit )
	    { PROPmaskADD( &cpChgMask, CLpropLEFT_PADDING_UNIT ); }
	}

    if  ( PROPmaskISSET( cpUpdMask, CLpropRIGHT_PADDING_UNIT ) )
	{
	if  ( cpTo->cpRightPaddingUnit != cpFrom->cpRightPaddingUnit )
	    { PROPmaskADD( &cpChgMask, CLpropRIGHT_PADDING_UNIT ); }
	}

    *pCpChgPask= cpChgMask; return;
    }

void docCopyCellProperties(	CellProperties *	cpTo,
				const CellProperties *	cpFrom,
				const int *		colorMap )
    {
    PropertyMask		cpDoneMask;
    PropertyMask		cpSetMask;

    PROPmaskCLEAR( &cpDoneMask );
    PROPmaskCLEAR( &cpSetMask );

    utilPropMaskFill( &cpSetMask, CLprop_COUNT );

    docUpdCellProperties( &cpDoneMask, cpTo, &cpSetMask, cpFrom, colorMap );

    return;
    }

/************************************************************************/
/*									*/
/*  Translate an item shading property mask to a cell property mask	*/
/*									*/
/************************************************************************/

void docShadingMaskToCellMask(	PropertyMask *		cpPropMask,
				const PropertyMask *	isPropMask )
    {
    PropertyMask	pm;

    PROPmaskCLEAR( &pm );

    if  ( PROPmaskISSET( isPropMask, ISpropFORE_COLOR ) )
	{ PROPmaskADD( &pm, CLpropSHADE_FORE_COLOR );	}

    if  ( PROPmaskISSET( isPropMask, ISpropBACK_COLOR ) )
	{ PROPmaskADD( &pm, CLpropSHADE_BACK_COLOR );	}

    if  ( PROPmaskISSET( isPropMask, ISpropLEVEL ) )
	{ PROPmaskADD( &pm, CLpropSHADE_LEVEL );	}

    if  ( PROPmaskISSET( isPropMask, ISpropPATTERN ) )
	{ PROPmaskADD( &pm, CLpropSHADE_PATTERN );	}

    *cpPropMask= pm;
    return;
    }

void docShadingMaskFromCellMask(	PropertyMask *		isPropMask,
					const PropertyMask *	cpPropMask )
    {
    PropertyMask	pm;

    PROPmaskCLEAR( &pm );

    if  ( PROPmaskISSET( cpPropMask, CLpropSHADE_FORE_COLOR ) )
	{ PROPmaskADD( &pm, ISpropFORE_COLOR );	}

    if  ( PROPmaskISSET( cpPropMask, CLpropSHADE_BACK_COLOR ) )
	{ PROPmaskADD( &pm, ISpropBACK_COLOR );	}

    if  ( PROPmaskISSET( cpPropMask, CLpropSHADE_LEVEL ) )
	{ PROPmaskADD( &pm, ISpropLEVEL );	}

    if  ( PROPmaskISSET( cpPropMask, CLpropSHADE_PATTERN ) )
	{ PROPmaskADD( &pm, ISpropPATTERN );	}

    *isPropMask= pm;
    return;
    }

