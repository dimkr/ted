/************************************************************************/
/*									*/
/*  Management of Border Properties.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docItemShading.h"

/************************************************************************/
/*									*/
/*  Initialise a shading.						*/
/*									*/
/************************************************************************/

void docInitItemShading(	ItemShading *	is )
    {
    is->isBackColor= 0;
    is->isForeColor= 0;

    is->isLevel= 0;
    is->isPattern= DOCspSOLID;
    }

void docInitExpandedItemShading(	ExpandedItemShading *	eis )
    {
    eis->eisForeColorExplicit= 0;
    eis->eisBackColorExplicit= 0;

    bmInitRGB8Color( &(eis->eisBackColor) );
    bmInitRGB8Color( &(eis->eisForeColor) );

    eis->eisLevel= 0;
    eis->eisPattern= DOCspSOLID;
    }

/************************************************************************/
/*									*/
/*  Compare/Update shadings.						*/
/*									*/
/*  9)  Handle colors last. Some of the processing depends on the other	*/
/*	properties.							*/
/*									*/
/************************************************************************/

void docItemShadingDifference(	PropertyMask *			pIsDoneMask,
				const ItemShading *		isTo,
				const ItemShading *		isFrom,
				const PropertyMask *		isSetMask,
				const int *			colorMap )
    {
    PropertyMask		isDoneMask;
    int				fromColor;

    PROPmaskCLEAR( &isDoneMask );

    if  ( PROPmaskISSET( isSetMask, ISpropLEVEL ) )
	{
	if  ( isTo->isLevel != isFrom->isLevel )
	    { PROPmaskADD( &isDoneMask, ISpropLEVEL );	}
	}

    if  ( PROPmaskISSET( isSetMask, ISpropPATTERN ) )
	{
	if  ( isTo->isPattern != isFrom->isPattern )
	    { PROPmaskADD( &isDoneMask, ISpropPATTERN );	}
	}

    /*  9  */
    if  ( PROPmaskISSET( isSetMask, ISpropBACK_COLOR ) )
	{
	fromColor= isFrom->isBackColor;

	if  ( isTo->isPattern == DOCspSOLID	&&
	      isTo->isLevel == 10000		)
	    { fromColor= 0;	}
	else{
	    if  ( fromColor > 0 && colorMap )
		{ fromColor= colorMap[fromColor];	}
	    }

	if  ( isTo->isBackColor != fromColor )
	    { PROPmaskADD( &isDoneMask, ISpropBACK_COLOR );	}
	}

    /*  9  */
    if  ( PROPmaskISSET( isSetMask, ISpropFORE_COLOR ) )
	{
	fromColor= isFrom->isForeColor;

	if  ( isTo->isPattern == DOCspSOLID	&&
	      isTo->isLevel == 0		)
	    { fromColor= 0;	}
	else{
	    if  ( fromColor > 0 && colorMap )
		{ fromColor= colorMap[fromColor];	}
	    }

	if  ( isTo->isForeColor != fromColor )
	    { PROPmaskADD( &isDoneMask, ISpropFORE_COLOR );	}
	}

    *pIsDoneMask= isDoneMask;
    return;
    }

void docUpdateItemShading(	PropertyMask *			pIsDoneMask,
				ItemShading *			isTo,
				const PropertyMask *		isSetMask,
				const ItemShading *		isFrom,
				const int *			colorMap )
    {
    PropertyMask		isDoneMask;
    int				fromColor;

    PROPmaskCLEAR( &isDoneMask );

    if  ( PROPmaskISSET( isSetMask, ISpropLEVEL ) )
	{
	if  ( isTo->isLevel != isFrom->isLevel )
	    {
	    isTo->isLevel= isFrom->isLevel;
	    PROPmaskADD( &isDoneMask, ISpropLEVEL );
	    }
	}

    if  ( PROPmaskISSET( isSetMask, ISpropPATTERN ) )
	{
	if  ( isTo->isPattern != isFrom->isPattern )
	    {
	    isTo->isPattern= isFrom->isPattern;
	    PROPmaskADD( &isDoneMask, ISpropPATTERN );
	    }
	}

    /*  9  */
    if  ( PROPmaskISSET( isSetMask, ISpropBACK_COLOR ) )
	{
	fromColor= isFrom->isBackColor;

	if  ( isTo->isPattern == DOCspSOLID	&&
	      isTo->isLevel == 10000		)
	    { fromColor= 0;	}
	else{
	    if  ( fromColor > 0 && colorMap )
		{ fromColor= colorMap[fromColor];	}
	    }

	if  ( isTo->isBackColor != fromColor )
	    {
	    isTo->isBackColor= fromColor;
	    PROPmaskADD( &isDoneMask, ISpropBACK_COLOR );
	    }
	}

    /*  9  */
    if  ( PROPmaskISSET( isSetMask, ISpropFORE_COLOR ) )
	{
	fromColor= isFrom->isForeColor;

	if  ( isTo->isPattern == DOCspSOLID	&&
	      isTo->isLevel == 0		)
	    { fromColor= 0;	}
	else{
	    if  ( fromColor > 0 && colorMap )
		{ fromColor= colorMap[fromColor];	}
	    }

	if  ( isTo->isForeColor != fromColor )
	    {
	    isTo->isForeColor= fromColor;
	    PROPmaskADD( &isDoneMask, ISpropFORE_COLOR );
	    }
	}

    *pIsDoneMask= isDoneMask;
    return;
    }

void docUpdateExpandedItemShading(
				PropertyMask *			pIsDoneMask,
				ExpandedItemShading *		eisTo,
				const PropertyMask *		isSetMask,
				const ExpandedItemShading *	eisFrom )
    {
    PropertyMask		isDoneMask;

    PROPmaskCLEAR( &isDoneMask );

    /**/
    if  ( PROPmaskISSET( isSetMask, ISpropBACK_COLOR ) )
	{
	if  ( eisTo->eisBackColorExplicit )
	    {
	    if  ( ! eisFrom->eisBackColorExplicit )
		{
		eisTo->eisBackColorExplicit= 0;
		PROPmaskADD( &isDoneMask, ISpropBACK_COLOR );
		}
	    else{
		if  ( bmRGB8ColorsDiffer( &(eisTo->eisBackColor),
						&(eisFrom->eisBackColor) ) )
		    {
		    eisTo->eisBackColor= eisFrom->eisBackColor;
		    PROPmaskADD( &isDoneMask, ISpropBACK_COLOR );
		    }
		}
	    }
	else{
	    if  ( eisFrom->eisBackColorExplicit )
		{
		eisTo->eisBackColorExplicit= 1;
		eisTo->eisBackColor= eisFrom->eisBackColor;
		PROPmaskADD( &isDoneMask, ISpropBACK_COLOR );
		}
	    }
	}

    /**/
    if  ( PROPmaskISSET( isSetMask, ISpropFORE_COLOR ) )
	{
	if  ( eisTo->eisForeColorExplicit )
	    {
	    if  ( ! eisFrom->eisForeColorExplicit )
		{
		eisTo->eisForeColorExplicit= 0;
		PROPmaskADD( &isDoneMask, ISpropFORE_COLOR );
		}
	    else{
		if  ( bmRGB8ColorsDiffer( &(eisTo->eisForeColor),
						&(eisFrom->eisForeColor) ) )
		    {
		    eisTo->eisForeColor= eisFrom->eisForeColor;
		    PROPmaskADD( &isDoneMask, ISpropFORE_COLOR );
		    }
		}
	    }
	else{
	    if  ( eisFrom->eisForeColorExplicit )
		{
		eisTo->eisForeColorExplicit= 1;
		eisTo->eisForeColor= eisFrom->eisForeColor;
		PROPmaskADD( &isDoneMask, ISpropFORE_COLOR );
		}
	    }
	}

    /**/
    if  ( PROPmaskISSET( isSetMask, ISpropLEVEL ) )
	{
	if  ( eisTo->eisLevel != eisFrom->eisLevel )
	    {
	    eisTo->eisLevel= eisFrom->eisLevel;
	    PROPmaskADD( &isDoneMask, ISpropLEVEL );
	    }
	}

    if  ( PROPmaskISSET( isSetMask, ISpropPATTERN ) )
	{
	if  ( eisTo->eisPattern != eisFrom->eisPattern )
	    {
	    eisTo->eisPattern= eisFrom->eisPattern;
	    PROPmaskADD( &isDoneMask, ISpropPATTERN );
	    }
	}

    *pIsDoneMask= isDoneMask;
    return;
    }


/************************************************************************/
/*									*/
/*  Expand the palette references in an item shading.			*/
/*									*/
/************************************************************************/

void docExpandItemShading(	ExpandedItemShading *	eis,
				const ItemShading *	is,
				const RGB8Color *	colors,
				int			colorCount )
    {
    if  ( is->isBackColor == 0 )
	{
	eis->eisBackColorExplicit= 0;
	bmInitRGB8Color( &(eis->eisBackColor) );
	}
    else{
	if  ( is->isBackColor < 0		||
	      is->isBackColor >= colorCount	)
	    {
	    LLDEB(is->isBackColor,colorCount);

	    eis->eisBackColorExplicit= 0;
	    bmInitRGB8Color( &(eis->eisBackColor) );
	    }
	else{
	    eis->eisBackColorExplicit= 1;
	    eis->eisBackColor= colors[is->isBackColor];
	    }
	}

    if  ( is->isForeColor == 0 )
	{
	eis->eisForeColorExplicit= 0;
	bmInitRGB8Color( &(eis->eisForeColor) );
	}
    else{
	if  ( is->isForeColor < 0		||
	      is->isForeColor >= colorCount	)
	    {
	    LLDEB(is->isForeColor,colorCount);

	    eis->eisForeColorExplicit= 0;
	    bmInitRGB8Color( &(eis->eisForeColor) );
	    }
	else{
	    eis->eisForeColorExplicit= 1;
	    eis->eisForeColor= colors[is->isForeColor];
	    }
	}

    eis->eisLevel= is->isLevel;
    eis->eisPattern= is->isPattern;

    return;
    }

/************************************************************************/
/*									*/
/*  Get the palette references in an item shading.			*/
/*									*/
/************************************************************************/

int docIndirectItemShading(	PropertyMask *			pDoneMask,
				ItemShading *			is,
				const PropertyMask *		setMask,
				const ExpandedItemShading *	eis,
				RGB8Color **			pColors,
				int *				pColorCount )
    {
    const int			maxColors= 256;
    const int			avoidZero= 1;
    int				color;

    PropertyMask		doneMask;

    PROPmaskCLEAR( &doneMask );

    /**/
    if  ( PROPmaskISSET( setMask, ISpropBACK_COLOR ) )
	{
	if  ( eis->eisBackColorExplicit )
	    {
	    color= bmInsertColor( pColors, pColorCount,
				avoidZero, maxColors, &(eis->eisBackColor) );
	    if  ( color < 0 )
		{ LDEB(color); return -1;	}
	    }
	else{ color= 0;	}

	if  ( is->isBackColor != color )
	    {
	    is->isBackColor= color;
	    PROPmaskADD( &doneMask, ISpropBACK_COLOR );
	    }
	}

    /**/
    if  ( PROPmaskISSET( setMask, ISpropFORE_COLOR ) )
	{
	if  ( eis->eisForeColorExplicit )
	    {
	    color= bmInsertColor( pColors, pColorCount,
				avoidZero, maxColors, &(eis->eisForeColor) );
	    if  ( color < 0 )
		{ LDEB(color); return -1;	}
	    }
	else{ color= 0;	}

	if  ( is->isForeColor != color )
	    {
	    is->isForeColor= color;
	    PROPmaskADD( &doneMask, ISpropFORE_COLOR );
	    }
	}

    /**/
    if  ( PROPmaskISSET( setMask, ISpropLEVEL ) )
	{
	if  ( is->isLevel != eis->eisLevel )
	    {
	    is->isLevel= eis->eisLevel;
	    PROPmaskADD( &doneMask, ISpropLEVEL );
	    }
	}

    /**/
    if  ( PROPmaskISSET( setMask, ISpropPATTERN ) )
	{
	if  ( is->isPattern != eis->eisPattern )
	    {
	    is->isPattern= eis->eisPattern;
	    PROPmaskADD( &doneMask, ISpropPATTERN );
	    }
	}

    *pDoneMask= doneMask;
    return 0;
    }

