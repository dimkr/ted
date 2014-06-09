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

#   include	"docBorderProperties.h"

/************************************************************************/
/*									*/
/*  Initialise Border Properties.					*/
/*									*/
/************************************************************************/

void docInitBorderProperties(	BorderProperties *	bp )
    {
    bp->bpColor= 0;
    bp->bpSpacingTwips= 0;
    bp->bpPenWideTwips= 15;
    bp->bpStyle= DOCbsNONE;
    bp->bpArt= 0;
    }

/************************************************************************/
/*									*/
/*  Get the thickness of a border.					*/
/*									*/
/************************************************************************/

int docBorderThick(		int *				pSpace,
				const BorderProperties *	bp )
    {
    int			space;
    int			thick;

    if  ( ! DOCisBORDER( bp ) )
	{
	thick= 0;
	space= 0;
	}
    else{
	thick= bp->bpPenWideTwips;

	if  ( bp->bpStyle == DOCbsTH )
	    { thick *= 2;	}

	space= bp->bpSpacingTwips;
	}

    *pSpace= space;
    return thick;
    }


/************************************************************************/
/*									*/
/*  1) Are the columns in two RowProperties 'the same' (Do they align?)	*/
/*  2) All column properties identical?					*/
/*									*/
/************************************************************************/

static void docBorderPropertyDifference(
				PropertyMask *			pBpUpdMask,
				const BorderProperties *	bpTo,
				const BorderProperties *	bpFrom,
				const PropertyMask *		bpSetMask,
				const int *			colorMap )
    {
    PropertyMask		bpChgMask;

    PROPmaskCLEAR( &bpChgMask );

    if  ( PROPmaskISSET( bpSetMask, BRDRpropCOLOR ) )
	{
	int		fromColor= bpFrom->bpColor;

	if  ( fromColor > 0 && colorMap )
	    { fromColor= colorMap[fromColor];	}

	if  ( bpTo->bpColor != fromColor )
	    { PROPmaskADD( &bpChgMask, BRDRpropCOLOR ); }
	}

    if  ( PROPmaskISSET( bpSetMask, BRDRpropSPACING ) )
	{
	if  ( bpTo->bpSpacingTwips != bpFrom->bpSpacingTwips )
	    { PROPmaskADD( &bpChgMask, BRDRpropSPACING ); }
	}

    if  ( PROPmaskISSET( bpSetMask, BRDRpropPEN_WIDE ) )
	{
	if  ( bpTo->bpPenWideTwips != bpFrom->bpPenWideTwips )
	    { PROPmaskADD( &bpChgMask, BRDRpropPEN_WIDE ); }
	}

    if  ( PROPmaskISSET( bpSetMask, BRDRpropSTYLE ) )
	{
	if  ( bpTo->bpStyle != bpFrom->bpStyle )
	    { PROPmaskADD( &bpChgMask, BRDRpropSTYLE ); }
	}

    if  ( PROPmaskISSET( bpSetMask, BRDRpropART ) )
	{
	if  ( bpTo->bpArt != bpFrom->bpArt )
	    { PROPmaskADD( &bpChgMask, BRDRpropART ); }
	}

    *pBpUpdMask= bpChgMask;
    return;
    }

void docUpdateBorderProperties(	PropertyMask *			pBpDoneMask,
				BorderProperties *		bpTo,
				const PropertyMask *		bpSetMask,
				const BorderProperties *	bpFrom )
    {
    PropertyMask		bpDoneMask;

    PROPmaskCLEAR( &bpDoneMask );

    if  ( PROPmaskISSET( bpSetMask, BRDRpropCOLOR ) )
	{
	if  ( bpTo->bpColor != bpFrom->bpColor )
	    {
	    bpTo->bpColor= bpFrom->bpColor;
	    PROPmaskADD( &bpDoneMask, BRDRpropCOLOR );
	    }
	}

    if  ( PROPmaskISSET( bpSetMask, BRDRpropSPACING ) )
	{
	if  ( bpTo->bpSpacingTwips != bpFrom->bpSpacingTwips )
	    {
	    bpTo->bpSpacingTwips= bpFrom->bpSpacingTwips;
	    PROPmaskADD( &bpDoneMask, BRDRpropSPACING );
	    }
	}

    if  ( PROPmaskISSET( bpSetMask, BRDRpropPEN_WIDE ) )
	{
	if  ( bpTo->bpPenWideTwips != bpFrom->bpPenWideTwips )
	    {
	    bpTo->bpPenWideTwips= bpFrom->bpPenWideTwips;
	    PROPmaskADD( &bpDoneMask, BRDRpropPEN_WIDE );
	    }
	}

    if  ( PROPmaskISSET( bpSetMask, BRDRpropSTYLE ) )
	{
	if  ( bpTo->bpStyle != bpFrom->bpStyle )
	    {
	    bpTo->bpStyle= bpFrom->bpStyle;
	    PROPmaskADD( &bpDoneMask, BRDRpropSTYLE );
	    }
	}

    if  ( PROPmaskISSET( bpSetMask, BRDRpropART ) )
	{
	if  ( bpTo->bpArt != bpFrom->bpArt )
	    {
	    bpTo->bpArt= bpFrom->bpArt;
	    PROPmaskADD( &bpDoneMask, BRDRpropART );
	    }
	}

    *pBpDoneMask= bpDoneMask;
    return;
    }

int docBordersDiffer(		const BorderProperties *	bpTo,
				const BorderProperties *	bpFrom,
				const int *			colorMap )
    {
    PropertyMask	bpSetMask;
    PropertyMask	bpChgMask;

    PROPmaskCLEAR( &bpChgMask );
    PROPmaskCLEAR( &bpSetMask );
    utilPropMaskFill( &bpSetMask, BRDRprop_COUNT );

    docBorderPropertyDifference( &bpChgMask, bpTo,
					    bpFrom, &bpSetMask, colorMap );

    if  ( ! utilPropMaskIsEmpty( &bpChgMask ) )
	{ return 1;	}

    return 0;
    }

void docCopyBorderProperties(	BorderProperties *		bpTo,
				const BorderProperties *	bpFrom,
				const int *			colorMap )
    {
    *bpTo= *bpFrom;

    if  ( ! DOCisBORDER( bpTo ) )
	{ bpTo->bpColor= 0;	}
    else{
	if  ( bpTo->bpColor > 0 && colorMap )
	    { bpTo->bpColor= colorMap[bpTo->bpColor];	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Expand the palette references in border properties.			*/
/*									*/
/************************************************************************/

void docExpandBorderProperties(	ExpandedBorderProperties *	ebp,
				const BorderProperties *	bp,
				const RGB8Color *		colors,
				int				colorCount )
    {
    if  ( bp->bpColor == 0 )
	{
	ebp->ebpColorExplicit= 0;
	bmInitRGB8Color( &(ebp->ebpColor) );
	}
    else{
	if  ( bp->bpColor >= colorCount	)
	    {
	    LLDEB(bp->bpColor,colorCount);

	    ebp->ebpColorExplicit= 0;
	    bmInitRGB8Color( &(ebp->ebpColor) );
	    }
	else{
	    ebp->ebpColorExplicit= 1;
	    ebp->ebpColor= colors[bp->bpColor];
	    }
	}

    ebp->ebpPenWideTwips= bp->bpPenWideTwips;
    ebp->ebpStyle= bp->bpStyle;
    ebp->ebpArt= bp->bpArt;
    ebp->ebpSpacingTwips= bp->bpSpacingTwips;

    return;
    }

/************************************************************************/
/*									*/
/*  Get the palette references in a border properties.			*/
/*									*/
/************************************************************************/

int docIndirectBorderProperties(
			PropertyMask *				pBpDoneMask,
			BorderProperties *			bpTo,
			const PropertyMask *			bpSetMask,
			const ExpandedBorderProperties *	ebpFrom,
			RGB8Color **				pColors,
			int *					pColorCount )
    {
    PropertyMask		bpDoneMask;

    PROPmaskCLEAR( &bpDoneMask );

    if  ( PROPmaskISSET( bpSetMask, BRDRpropCOLOR ) )
	{
	const int		maxColors= 256;
	const int		avoidZero= 1;
	int			color;

	if  ( ebpFrom->ebpColorExplicit )
	    {
	    color= bmInsertColor( pColors, pColorCount,
				avoidZero, maxColors, &(ebpFrom->ebpColor) );
	    if  ( color < 0 )
		{ LDEB(color); return -1;	}

	    color= color;
	    }
	else{ color= 0;	}

	if  ( bpTo->bpColor != color )
	    {
	    bpTo->bpColor= color;
	    PROPmaskADD( &bpDoneMask, BRDRpropCOLOR );
	    }
	}

    if  ( PROPmaskISSET( bpSetMask, BRDRpropSPACING ) )
	{
	if  ( bpTo->bpSpacingTwips != ebpFrom->ebpSpacingTwips )
	    {
	    bpTo->bpSpacingTwips= ebpFrom->ebpSpacingTwips;
	    PROPmaskADD( &bpDoneMask, BRDRpropSPACING );
	    }
	}

    if  ( PROPmaskISSET( bpSetMask, BRDRpropPEN_WIDE ) )
	{
	if  ( bpTo->bpPenWideTwips != ebpFrom->ebpPenWideTwips )
	    {
	    bpTo->bpPenWideTwips= ebpFrom->ebpPenWideTwips;
	    PROPmaskADD( &bpDoneMask, BRDRpropPEN_WIDE );
	    }
	}

    if  ( PROPmaskISSET( bpSetMask, BRDRpropSTYLE ) )
	{
	if  ( bpTo->bpStyle != ebpFrom->ebpStyle )
	    {
	    bpTo->bpStyle= ebpFrom->ebpStyle;
	    PROPmaskADD( &bpDoneMask, BRDRpropSTYLE );
	    }
	}

    if  ( PROPmaskISSET( bpSetMask, BRDRpropART ) )
	{
	if  ( bpTo->bpArt != ebpFrom->ebpArt )
	    {
	    bpTo->bpArt= ebpFrom->ebpArt;
	    PROPmaskADD( &bpDoneMask, BRDRpropART );
	    }
	}

    *pBpDoneMask= bpDoneMask;
    return 0;
    }


/************************************************************************/
/*									*/
/*  Initialize expanded border properties.				*/
/*									*/
/************************************************************************/

void docInitExpandedBorderProperties(	ExpandedBorderProperties *	ebp )
    {
    ebp->ebpColorExplicit= 0;

    bmInitRGB8Color( &(ebp->ebpColor) );

    ebp->ebpSpacingTwips= 0;
    ebp->ebpPenWideTwips= 15;
    ebp->ebpStyle= DOCbsNONE;
    ebp->ebpArt= 0;
    }

