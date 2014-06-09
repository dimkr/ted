/************************************************************************/
/*									*/
/*  Manage paragraph properties.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>

#   include	"docParaProperties.h"

/************************************************************************/
/*									*/
/*  1) Set a tab stop in a paragraph.					*/
/*  2) Remove a tab stop from the ruler of a paragraph.			*/
/*									*/
/************************************************************************/

/*  1  */
int docParaAddTab(		ParagraphProperties *	pp,
				const TabStop *		tsNew )
    { return docAddTabToList( &(pp->ppTabStopList), tsNew ); }

/*  2  */
void docParaDeleteTab(	ParagraphProperties *		pp,
			int				n )
    { docDeleteTabFromList( &(pp->ppTabStopList), n );	}

/************************************************************************/
/*									*/
/*  Manage paragraph properties.					*/
/*									*/
/************************************************************************/

void docCleanParagraphProperties(	ParagraphProperties *	pp )
    {
    docCleanTabStopList( &(pp->ppTabStopList) );
    }

void docInitParagraphProperties(	ParagraphProperties *	pp )
    {
    docInitTabStopList( &(pp->ppTabStopList) );

    pp->ppFirstIndentTwips= 0;
    pp->ppLeftIndentTwips= 0;
    pp->ppRightIndentTwips= 0;

    pp->ppSpaceBeforeTwips= 0;
    pp->ppSpaceAfterTwips= 0;
    pp->ppLineSpacingTwips= 0;

    pp->ppStyle= 0;
    pp->ppListOverride= 0;

    docInitBorderProperties( &(pp->ppTopBorder) );
    docInitBorderProperties( &(pp->ppLeftBorder) );
    docInitBorderProperties( &(pp->ppRightBorder) );
    docInitBorderProperties( &(pp->ppBottomBorder) );
    docInitBorderProperties( &(pp->ppBetweenBorder) );
    docInitBorderProperties( &(pp->ppBar) );

    docInitItemShading( &(pp->ppShading) );

    pp->ppOutlineLevel= 9;
    pp->ppListLevel= 0;

    pp->ppAlignment= DOCiaLEFT;

    pp->ppStartsOnNewPage= 0;
    pp->ppInTable= 0;
    pp->ppLineSpacingIsMultiple= 0;

    pp->ppKeepOnPage= 0;
    pp->ppKeepWithNext= 0;
    pp->ppWidowControl= 0;
    pp->ppHyphenateParagraph= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Change paragraph properties and tell what has been changed.		*/
/*									*/
/************************************************************************/

int docCopyParagraphProperties(	ParagraphProperties *		ppTo,
				const ParagraphProperties *	ppFrom )
    {
    const int * const	colorMap= (const int *)0;
    const int * const	listStyleMap= (const int *)0;

    PropertyMask	ppDoneMask;
    PropertyMask	ppSetMask;

    PROPmaskCLEAR( &ppDoneMask );
    utilPropMaskFill( &ppSetMask, PPprop_COUNT );

    return docUpdParaProperties( &ppDoneMask, ppTo,
				&ppSetMask, ppFrom, colorMap, listStyleMap );
    }

int docUpdParaProperties(	PropertyMask *			pDoneMask,
				ParagraphProperties *		ppTo,
				const PropertyMask *		ppUpdMask,
				const ParagraphProperties *	ppFrom,
				const int *			colorMap,
				const int *			listStyleMap )
    {
    PropertyMask		doneMask;
    PropertyMask		isUpdMask;

    PROPmaskCLEAR( &doneMask );

    if  ( PROPmaskISSET( ppUpdMask, PPpropSTYLE ) )
	{
	if  ( ppTo->ppStyle != ppFrom->ppStyle )
	    {
	    ppTo->ppStyle= ppFrom->ppStyle;
	    PROPmaskADD( &doneMask, PPpropSTYLE );
	    }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropLISTOVERRIDE ) )
	{
	int		listStyle= ppFrom->ppListOverride;

	if  ( listStyleMap )
	    { listStyle= listStyleMap[listStyle];	}

	if  ( ppTo->ppListOverride != listStyle )
	    {
	    ppTo->ppListOverride= listStyle;
	    PROPmaskADD( &doneMask, PPpropLISTOVERRIDE );
	    }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropIN_TABLE ) )
	{
	if  ( ppTo->ppInTable != ppFrom->ppInTable )
	    {
	    ppTo->ppInTable= ppFrom->ppInTable;
	    PROPmaskADD( &doneMask, PPpropIN_TABLE );
	    }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropFIRST_INDENT ) )
	{
	if  ( ppTo->ppFirstIndentTwips != ppFrom->ppFirstIndentTwips )
	    {
	    ppTo->ppFirstIndentTwips= ppFrom->ppFirstIndentTwips;
	    PROPmaskADD( &doneMask, PPpropFIRST_INDENT );
	    }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropLEFT_INDENT ) )
	{
	if  ( ppTo->ppLeftIndentTwips != ppFrom->ppLeftIndentTwips )
	    {
	    ppTo->ppLeftIndentTwips= ppFrom->ppLeftIndentTwips;
	    PROPmaskADD( &doneMask, PPpropLEFT_INDENT );
	    }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropRIGHT_INDENT ) )
	{
	if  ( ppTo->ppRightIndentTwips != ppFrom->ppRightIndentTwips )
	    {
	    ppTo->ppRightIndentTwips= ppFrom->ppRightIndentTwips;
	    PROPmaskADD( &doneMask, PPpropRIGHT_INDENT );
	    }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropALIGNMENT ) )
	{
	if  ( ppTo->ppAlignment != ppFrom->ppAlignment )
	    {
	    ppTo->ppAlignment= ppFrom->ppAlignment;
	    PROPmaskADD( &doneMask, PPpropALIGNMENT );
	    }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropTAB_STOPS ) )
	{
	const int	pixels= 0;
	int		changed= 0;

	if  ( docCopyTabStopList( &changed, &(ppTo->ppTabStopList),
					&(ppFrom->ppTabStopList), pixels ) )
	    { LDEB(1); return -1;	}

	if  ( changed )
	    { PROPmaskADD( &doneMask, PPpropTAB_STOPS );	}
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropNEWPAGE ) )
	{
	if  ( ppTo->ppStartsOnNewPage != ppFrom->ppStartsOnNewPage )
	    {
	    ppTo->ppStartsOnNewPage= ppFrom->ppStartsOnNewPage;
	    PROPmaskADD( &doneMask, PPpropNEWPAGE );
	    }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropWIDCTLPAR ) )
	{
	if  ( ppTo->ppWidowControl != ppFrom->ppWidowControl )
	    {
	    ppTo->ppWidowControl= ppFrom->ppWidowControl;
	    PROPmaskADD( &doneMask, PPpropWIDCTLPAR );
	    }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropKEEPN ) )
	{
	if  ( ppTo->ppKeepWithNext != ppFrom->ppKeepWithNext )
	    {
	    ppTo->ppKeepWithNext= ppFrom->ppKeepWithNext;
	    PROPmaskADD( &doneMask, PPpropKEEPN );
	    }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropKEEP ) )
	{
	if  ( ppTo->ppKeepOnPage != ppFrom->ppKeepOnPage )
	    {
	    ppTo->ppKeepOnPage= ppFrom->ppKeepOnPage;
	    PROPmaskADD( &doneMask, PPpropKEEP );
	    }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropSPACE_BEFORE ) )
	{
	if  ( ppTo->ppSpaceBeforeTwips != ppFrom->ppSpaceBeforeTwips )
	    {
	    ppTo->ppSpaceBeforeTwips= ppFrom->ppSpaceBeforeTwips;
	    PROPmaskADD( &doneMask, PPpropSPACE_BEFORE );
	    }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropSPACE_AFTER ) )
	{
	if  ( ppTo->ppSpaceAfterTwips != ppFrom->ppSpaceAfterTwips )
	    {
	    ppTo->ppSpaceAfterTwips= ppFrom->ppSpaceAfterTwips;
	    PROPmaskADD( &doneMask, PPpropSPACE_AFTER );
	    }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropLINE_SPACING_DIST ) )
	{
	if  ( ppTo->ppLineSpacingTwips != ppFrom->ppLineSpacingTwips )
	    {
	    ppTo->ppLineSpacingTwips= ppFrom->ppLineSpacingTwips;
	    PROPmaskADD( &doneMask, PPpropLINE_SPACING_DIST );
	    }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropLINE_SPACING_MULT ) )
	{
	if  ( ppTo->ppLineSpacingIsMultiple != ppFrom->ppLineSpacingIsMultiple )
	    {
	    ppTo->ppLineSpacingIsMultiple= ppFrom->ppLineSpacingIsMultiple;
	    PROPmaskADD( &doneMask, PPpropLINE_SPACING_MULT );
	    }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropTOP_BORDER ) )
	{
	if  ( docBordersDiffer( &(ppTo->ppTopBorder),
					&(ppFrom->ppTopBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(ppTo->ppTopBorder),
					&(ppFrom->ppTopBorder), colorMap );
	    PROPmaskADD( &doneMask, PPpropTOP_BORDER );
	    }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropBOTTOM_BORDER ) )
	{
	if  ( docBordersDiffer( &(ppTo->ppBottomBorder),
					&(ppFrom->ppBottomBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(ppTo->ppBottomBorder),
					&(ppFrom->ppBottomBorder), colorMap );
	    PROPmaskADD( &doneMask, PPpropBOTTOM_BORDER );
	    }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropLEFT_BORDER ) )
	{
	if  ( docBordersDiffer( &(ppTo->ppLeftBorder),
					&(ppFrom->ppLeftBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(ppTo->ppLeftBorder),
					&(ppFrom->ppLeftBorder), colorMap );
	    PROPmaskADD( &doneMask, PPpropLEFT_BORDER );
	    }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropRIGHT_BORDER ) )
	{
	if  ( docBordersDiffer( &(ppTo->ppRightBorder),
					&(ppFrom->ppRightBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(ppTo->ppRightBorder),
					&(ppFrom->ppRightBorder), colorMap );
	    PROPmaskADD( &doneMask, PPpropRIGHT_BORDER );
	    }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropBETWEEN_BORDER ) )
	{
	if  ( docBordersDiffer( &(ppTo->ppBetweenBorder),
					&(ppFrom->ppBetweenBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(ppTo->ppBetweenBorder),
					&(ppFrom->ppBetweenBorder), colorMap );
	    PROPmaskADD( &doneMask, PPpropBETWEEN_BORDER );
	    }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropBAR_BORDER ) )
	{
	if  ( docBordersDiffer( &(ppTo->ppBar),
					&(ppFrom->ppBar), colorMap ) )
	    {
	    docCopyBorderProperties( &(ppTo->ppBar),
					&(ppFrom->ppBar), colorMap );
	    PROPmaskADD( &doneMask, PPpropBAR_BORDER );
	    }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropOUTLINELEVEL ) )
	{
	if  ( ppTo->ppOutlineLevel != ppFrom->ppOutlineLevel )
	    {
	    ppTo->ppOutlineLevel= ppFrom->ppOutlineLevel;
	    PROPmaskADD( &doneMask, PPpropOUTLINELEVEL );
	    }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropLISTLEVEL ) )
	{
	if  ( ppTo->ppListLevel != ppFrom->ppListLevel )
	    {
	    ppTo->ppListLevel= ppFrom->ppListLevel;
	    PROPmaskADD( &doneMask, PPpropLISTLEVEL );
	    }
	}

    docShadingMaskFromParagraphMask( &isUpdMask, ppUpdMask );
    if  ( ! utilPropMaskIsEmpty( &isUpdMask ) )
	{
	PropertyMask	isChgMask;

	PROPmaskCLEAR( &isChgMask );

	docUpdateItemShading( &isChgMask, &(ppTo->ppShading),
				&isUpdMask, &(ppFrom->ppShading), colorMap );

	docShadingMaskToParagraphMask( &isChgMask, &isChgMask );
	utilPropMaskOr( &doneMask, &doneMask, &isChgMask );
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropHYPHPAR ) )
	{
	if  ( ppTo->ppHyphenateParagraph != ppFrom->ppHyphenateParagraph )
	    {
	    ppTo->ppHyphenateParagraph= ppFrom->ppHyphenateParagraph;
	    PROPmaskADD( &doneMask, PPpropHYPHPAR );
	    }
	}

    *pDoneMask= doneMask; return 0;
    }

/************************************************************************/
/*									*/
/*  Compare paragraph properties.					*/
/*									*/
/************************************************************************/

void docParaPropertyDifference(	PropertyMask *			pChgMask,
				const ParagraphProperties *	ppTo,
				const ParagraphProperties *	ppFrom,
				const PropertyMask *		ppUpdMask )
    {
    PropertyMask		diffMask;
    PropertyMask		isUpdMask;
    const int *			colorMap= (const int *)0;

    PROPmaskCLEAR( &diffMask );

    if  ( PROPmaskISSET( ppUpdMask, PPpropSTYLE ) )
	{
	if  ( ppTo->ppStyle != ppFrom->ppStyle )
	    { PROPmaskADD( &diffMask, PPpropSTYLE ); }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropLISTOVERRIDE ) )
	{
	if  ( ppTo->ppListOverride != ppFrom->ppListOverride )
	    { PROPmaskADD( &diffMask, PPpropLISTOVERRIDE ); }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropFIRST_INDENT ) )
	{
	if  ( ppTo->ppFirstIndentTwips != ppFrom->ppFirstIndentTwips )
	    { PROPmaskADD( &diffMask, PPpropFIRST_INDENT ); }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropLEFT_INDENT ) )
	{
	if  ( ppTo->ppLeftIndentTwips != ppFrom->ppLeftIndentTwips )
	    { PROPmaskADD( &diffMask, PPpropLEFT_INDENT ); }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropRIGHT_INDENT ) )
	{
	if  ( ppTo->ppRightIndentTwips != ppFrom->ppRightIndentTwips )
	    { PROPmaskADD( &diffMask, PPpropRIGHT_INDENT ); }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropALIGNMENT ) )
	{
	if  ( ppTo->ppAlignment != ppFrom->ppAlignment )
	    { PROPmaskADD( &diffMask, PPpropALIGNMENT ); }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropTAB_STOPS ) )
	{
	int	different;

	docCompareTabStopLists( &different, &(ppTo->ppTabStopList),
						&(ppFrom->ppTabStopList) );
	if  ( different )
	    { PROPmaskADD( &diffMask, PPpropTAB_STOPS );	}
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropNEWPAGE ) )
	{
	if  ( ppTo->ppStartsOnNewPage != ppFrom->ppStartsOnNewPage )
	    { PROPmaskADD( &diffMask, PPpropNEWPAGE ); }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropWIDCTLPAR ) )
	{
	if  ( ppTo->ppWidowControl != ppFrom->ppWidowControl )
	    { PROPmaskADD( &diffMask, PPpropWIDCTLPAR ); }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropKEEPN ) )
	{
	if  ( ppTo->ppKeepWithNext != ppFrom->ppKeepWithNext )
	    { PROPmaskADD( &diffMask, PPpropKEEPN ); }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropKEEP ) )
	{
	if  ( ppTo->ppKeepOnPage != ppFrom->ppKeepOnPage )
	    { PROPmaskADD( &diffMask, PPpropKEEP ); }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropSPACE_BEFORE ) )
	{
	if  ( ppTo->ppSpaceBeforeTwips != ppFrom->ppSpaceBeforeTwips )
	    { PROPmaskADD( &diffMask, PPpropSPACE_BEFORE ); }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropSPACE_AFTER ) )
	{
	if  ( ppTo->ppSpaceAfterTwips != ppFrom->ppSpaceAfterTwips )
	    { PROPmaskADD( &diffMask, PPpropSPACE_AFTER ); }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropLINE_SPACING_DIST ) )
	{
	if  ( ppTo->ppLineSpacingTwips != ppFrom->ppLineSpacingTwips )
	    { PROPmaskADD( &diffMask, PPpropLINE_SPACING_DIST ); }
	}
    if  ( PROPmaskISSET( ppUpdMask, PPpropLINE_SPACING_MULT ) )
	{
	if  ( ppTo->ppLineSpacingIsMultiple != ppFrom->ppLineSpacingIsMultiple )
	    { PROPmaskADD( &diffMask, PPpropLINE_SPACING_MULT ); }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropTOP_BORDER ) )
	{
	if  ( docBordersDiffer( &(ppTo->ppTopBorder),
					&(ppFrom->ppTopBorder), colorMap ) )
	    { PROPmaskADD( &diffMask, PPpropTOP_BORDER ); }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropBOTTOM_BORDER ) )
	{
	if  ( docBordersDiffer( &(ppTo->ppBottomBorder),
					&(ppFrom->ppBottomBorder), colorMap ) )
	    { PROPmaskADD( &diffMask, PPpropBOTTOM_BORDER ); }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropLEFT_BORDER ) )
	{
	if  ( docBordersDiffer( &(ppTo->ppLeftBorder),
					&(ppFrom->ppLeftBorder), colorMap ) )
	    { PROPmaskADD( &diffMask, PPpropLEFT_BORDER ); }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropRIGHT_BORDER ) )
	{
	if  ( docBordersDiffer( &(ppTo->ppRightBorder),
					&(ppFrom->ppRightBorder), colorMap ) )
	    { PROPmaskADD( &diffMask, PPpropRIGHT_BORDER ); }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropBETWEEN_BORDER ) )
	{
	if  ( docBordersDiffer( &(ppTo->ppBetweenBorder),
					&(ppFrom->ppBetweenBorder), colorMap ) )
	    { PROPmaskADD( &diffMask, PPpropBETWEEN_BORDER ); }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropBAR_BORDER ) )
	{
	if  ( docBordersDiffer( &(ppTo->ppBar), &(ppFrom->ppBar), colorMap ) )
	    { PROPmaskADD( &diffMask, PPpropBAR_BORDER ); }
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropOUTLINELEVEL ) )
	{
	if  ( ppTo->ppOutlineLevel != ppFrom->ppOutlineLevel )
	    { PROPmaskADD( &diffMask, PPpropOUTLINELEVEL );	}
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropLISTLEVEL ) )
	{
	if  ( ppTo->ppListLevel != ppFrom->ppListLevel )
	    { PROPmaskADD( &diffMask, PPpropLISTLEVEL );	}
	}

    docShadingMaskFromParagraphMask( &isUpdMask, ppUpdMask );
    if  ( ! utilPropMaskIsEmpty( &isUpdMask ) )
	{
	PropertyMask	isChgMask;

	PROPmaskCLEAR( &isChgMask );

	docItemShadingDifference( &isChgMask, &(ppTo->ppShading),
				&(ppFrom->ppShading), &isUpdMask, colorMap );

	docShadingMaskToParagraphMask( &isChgMask, &isChgMask );
	utilPropMaskOr( &diffMask, &diffMask, &isChgMask );
	}

    if  ( PROPmaskISSET( ppUpdMask, PPpropHYPHPAR ) )
	{
	if  ( ppTo->ppHyphenateParagraph != ppFrom->ppHyphenateParagraph )
	    { PROPmaskADD( &diffMask, PPpropHYPHPAR ); }
	}

    *pChgMask= diffMask;
    return;
    }

/************************************************************************/
/*									*/
/*  Translate an item shading property mask to a paragraph property	*/
/*  mask								*/
/*									*/
/************************************************************************/

void docShadingMaskToParagraphMask(	PropertyMask *		ppPropMask,
					const PropertyMask *	isPropMask )
    {
    PropertyMask	pm;

    PROPmaskCLEAR( &pm );

    if  ( PROPmaskISSET( isPropMask, ISpropFORE_COLOR ) )
	{ PROPmaskADD( &pm, PPpropSHADE_FORE_COLOR );	}

    if  ( PROPmaskISSET( isPropMask, ISpropBACK_COLOR ) )
	{ PROPmaskADD( &pm, PPpropSHADE_BACK_COLOR );	}

    if  ( PROPmaskISSET( isPropMask, ISpropLEVEL ) )
	{ PROPmaskADD( &pm, PPpropSHADE_LEVEL );	}

    if  ( PROPmaskISSET( isPropMask, ISpropPATTERN ) )
	{ PROPmaskADD( &pm, PPpropSHADE_PATTERN );	}

    *ppPropMask= pm;
    return;
    }

void docShadingMaskFromParagraphMask(	PropertyMask *		isPropMask,
					const PropertyMask *	ppPropMask )
    {
    PropertyMask	pm;

    PROPmaskCLEAR( &pm );

    if  ( PROPmaskISSET( ppPropMask, PPpropSHADE_FORE_COLOR ) )
	{ PROPmaskADD( &pm, ISpropFORE_COLOR );	}

    if  ( PROPmaskISSET( ppPropMask, PPpropSHADE_BACK_COLOR ) )
	{ PROPmaskADD( &pm, ISpropBACK_COLOR );	}

    if  ( PROPmaskISSET( ppPropMask, PPpropSHADE_LEVEL ) )
	{ PROPmaskADD( &pm, ISpropLEVEL );	}

    if  ( PROPmaskISSET( ppPropMask, PPpropSHADE_PATTERN ) )
	{ PROPmaskADD( &pm, ISpropPATTERN );	}

    *isPropMask= pm;
    return;
    }

