/************************************************************************/
/*									*/
/*  Manipulate iten properties in a document.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>
#   include	<utilPropMask.h>

#   include	"docSectProperties.h"

/************************************************************************/
/*									*/
/*  Change section properties and tell what has been changed.		*/
/*									*/
/************************************************************************/

int docUpdSectProperties(	PropertyMask *			pSpDoneMask,
				SectionProperties *		to,
				const PropertyMask *		setMask,
				const SectionProperties *	from )
    {
    PropertyMask		doneMask;
    PropertyMask		dgMask;

    PROPmaskCLEAR( &doneMask );

    PROPmaskCLEAR( &dgMask );
    utilPropMaskFill( &dgMask, DGprop_COUNT );
    utilPropMaskAnd( &dgMask, &dgMask, setMask );

    utilUpdDocumentGeometry( &(to->spDocumentGeometry),
					&(from->spDocumentGeometry),
					&doneMask, &dgMask );

    if  ( PROPmaskISSET( setMask, SPpropSTYLE ) )
	{
	if  ( to->spStyle != from->spStyle )
	    {
	    to->spStyle= from->spStyle;
	    PROPmaskADD( &doneMask, SPpropSTYLE );
	    }
	}

    if  ( PROPmaskISSET( setMask, SPpropTITLEPG ) )
	{
	if  ( to->spHasTitlePage != from->spHasTitlePage )
	    {
	    to->spHasTitlePage= from->spHasTitlePage;
	    PROPmaskADD( &doneMask, SPpropTITLEPG );
	    }
	}

    if  ( PROPmaskISSET( setMask, SPpropBREAK_KIND ) )
	{
	if  ( to->spBreakKind != from->spBreakKind )
	    {
	    to->spBreakKind= from->spBreakKind;
	    PROPmaskADD( &doneMask, SPpropBREAK_KIND );
	    }
	}

    if  ( PROPmaskISSET( setMask, SPpropNUMBER_STYLE ) )
	{
	if  ( to->spPageNumberStyle != from->spPageNumberStyle )
	    {
	    to->spPageNumberStyle= from->spPageNumberStyle;
	    PROPmaskADD( &doneMask, SPpropNUMBER_STYLE );
	    }
	}

    if  ( PROPmaskISSET( setMask, SPpropPAGE_RESTART ) )
	{
	if  ( to->spRestartPageNumbers != from->spRestartPageNumbers )
	    {
	    to->spRestartPageNumbers= from->spRestartPageNumbers;
	    PROPmaskADD( &doneMask, SPpropPAGE_RESTART );
	    }
	}

    if  ( PROPmaskISSET( setMask, SPpropSTART_PAGE ) )
	{
	if  ( to->spStartPageNumber != from->spStartPageNumber )
	    {
	    to->spStartPageNumber= from->spStartPageNumber;
	    PROPmaskADD( &doneMask, SPpropSTART_PAGE );
	    }
	}

    if  ( PROPmaskISSET( setMask, SPpropCOLUMN_COUNT ) )
	{
	if  ( to->spColumnCount != from->spColumnCount )
	    {
	    to->spColumnCount= from->spColumnCount;
	    PROPmaskADD( &doneMask, SPpropCOLUMN_COUNT );
	    }
	}

    if  ( PROPmaskISSET( setMask, SPpropCOLUMN_SPACING ) )
	{
	if  ( to->spColumnSpacingTwips != from->spColumnSpacingTwips )
	    {
	    to->spColumnSpacingTwips= from->spColumnSpacingTwips;
	    PROPmaskADD( &doneMask, SPpropCOLUMN_SPACING );
	    }
	}

    if  ( PROPmaskISSET( setMask, SPpropCOLUMN_WIDTH ) )
	{
	if  ( to->spColumnWidthTwips != from->spColumnWidthTwips )
	    {
	    to->spColumnWidthTwips= from->spColumnWidthTwips;
	    PROPmaskADD( &doneMask, SPpropCOLUMN_WIDTH );
	    }
	}

    docUpdNotesProperties( &doneMask, &(to->spFootnoteProperties),
				setMask, &(from->spFootnoteProperties),
				SPpropFOOTNOTE_STARTNR,
				SPpropFOOTNOTE_POSITION,
				SPpropFOOTNOTE_RESTART,
				SPpropFOOTNOTE_STYLE );

    docUpdNotesProperties( &doneMask, &(to->spEndnoteProperties),
				setMask, &(from->spEndnoteProperties),
				SPpropENDNOTE_STARTNR,
				SPpropENDNOTE_POSITION,
				SPpropENDNOTE_RESTART,
				SPpropENDNOTE_STYLE );


    *pSpDoneMask= doneMask; return 0;
    }

void docSectPropertyDifference( PropertyMask *			pDiffMask,
				const SectionProperties *	sp1,
				const SectionProperties *	sp2,
				const PropertyMask *		cmpMask )
    {
    PropertyMask		diffMask;
    PropertyMask		dgMask;

    DocumentGeometry		dg;

    PROPmaskCLEAR( &diffMask );

    PROPmaskCLEAR( &dgMask );
    utilPropMaskFill( &dgMask, DGprop_COUNT );
    utilPropMaskAnd( &dgMask, &dgMask, cmpMask );

    dg= sp1->spDocumentGeometry;
    utilUpdDocumentGeometry( &dg, &(sp2->spDocumentGeometry),
							&diffMask, &dgMask );

    if  ( PROPmaskISSET( cmpMask, SPpropSTYLE ) )
	{
	if  ( sp1->spStyle != sp2->spStyle )
	    { PROPmaskADD( &diffMask, SPpropSTYLE ); }
	}

    if  ( PROPmaskISSET( cmpMask, SPpropTITLEPG ) )
	{
	if  ( sp1->spHasTitlePage != sp2->spHasTitlePage )
	    { PROPmaskADD( &diffMask, SPpropTITLEPG ); }
	}

    if  ( PROPmaskISSET( cmpMask, SPpropBREAK_KIND ) )
	{
	if  ( sp1->spBreakKind != sp2->spBreakKind )
	    { PROPmaskADD( &diffMask, SPpropBREAK_KIND ); }
	}

    if  ( PROPmaskISSET( cmpMask, SPpropNUMBER_STYLE ) )
	{
	if  ( sp1->spPageNumberStyle != sp2->spPageNumberStyle )
	    { PROPmaskADD( &diffMask, SPpropNUMBER_STYLE ); }
	}

    if  ( PROPmaskISSET( cmpMask, SPpropPAGE_RESTART ) )
	{
	if  ( sp1->spRestartPageNumbers != sp2->spRestartPageNumbers )
	    { PROPmaskADD( &diffMask, SPpropPAGE_RESTART ); }
	}

    if  ( PROPmaskISSET( cmpMask, SPpropSTART_PAGE ) )
	{
	if  ( sp1->spStartPageNumber != sp2->spStartPageNumber )
	    { PROPmaskADD( &diffMask, SPpropSTART_PAGE ); }
	}

    if  ( PROPmaskISSET( cmpMask, SPpropCOLUMN_COUNT ) )
	{
	if  ( sp1->spColumnCount != sp2->spColumnCount )
	    { PROPmaskADD( &diffMask, SPpropCOLUMN_COUNT ); }
	}

    if  ( PROPmaskISSET( cmpMask, SPpropCOLUMN_SPACING ) )
	{
	if  ( sp1->spColumnSpacingTwips != sp2->spColumnSpacingTwips )
	    { PROPmaskADD( &diffMask, SPpropCOLUMN_SPACING ); }
	}

    if  ( PROPmaskISSET( cmpMask, SPpropCOLUMN_WIDTH ) )
	{
	if  ( sp1->spColumnWidthTwips != sp2->spColumnWidthTwips )
	    { PROPmaskADD( &diffMask, SPpropCOLUMN_WIDTH ); }
	}

    docNotesPropertyDifference( &diffMask, &(sp1->spFootnoteProperties),
				cmpMask, &(sp2->spFootnoteProperties),
				SPpropFOOTNOTE_STARTNR,
				SPpropFOOTNOTE_POSITION,
				SPpropFOOTNOTE_RESTART,
				SPpropFOOTNOTE_STYLE );

    docNotesPropertyDifference( &diffMask, &(sp1->spEndnoteProperties),
				cmpMask, &(sp2->spEndnoteProperties),
				SPpropENDNOTE_STARTNR,
				SPpropENDNOTE_POSITION,
				SPpropENDNOTE_RESTART,
				SPpropENDNOTE_STYLE );

    *pDiffMask= diffMask;
    return;
    }

/************************************************************************/
/*									*/
/*  Clean, Initialize section properties.				*/
/*									*/
/************************************************************************/

void docCleanSectionProperties(	SectionProperties *	sp )
    {
    if  ( sp->spParagraphNumbers )
	{ free( sp->spParagraphNumbers );	}

    return;
    }

void docInitSectionProperties(	SectionProperties *	sp )
    {
    utilInitDocumentGeometry( &(sp->spDocumentGeometry) );

    sp->spStyle= 0;

    sp->spColumnCount= 1;
    sp->spColumnSpacingTwips= 720;
    sp->spColumnWidthTwips= 0;

    sp->spHasTitlePage= 0;
    sp->spBreakKind= DOCsbkPAGE;
    sp->spPageNumberStyle= DOCpgnDEC;
    sp->spRestartPageNumbers= 0;

    sp->spStartPageNumber= 0;

    sp->spFootnoteProperties.npStartNumber= 1;
    sp->spFootnoteProperties.npPosition= FTN_POS_PAGE_BOTTOM;
    sp->spFootnoteProperties.npRestart= FTN_RST_CONTINUOUS;
    sp->spFootnoteProperties.npNumberStyle= FTNstyleNAR;

    sp->spEndnoteProperties.npStartNumber= 1;
    sp->spEndnoteProperties.npPosition= FTN_POS_DOC_END;
    sp->spEndnoteProperties.npRestart= FTN_RST_CONTINUOUS;
    sp->spEndnoteProperties.npNumberStyle= FTNstyleNRLC;

    sp->spParagraphNumbers= (ParagraphNumber *)0;
    sp->spParagraphNumberCount= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Copy SectionProperties.						*/
/*									*/
/*  NOTE that the headers and footers or the paragraph numbers for	*/
/*  bulleted lists etc are not copied.					*/
/*									*/
/************************************************************************/

int docCopySectionProperties(	SectionProperties *		to,
				const SectionProperties *	from )
    {
    to->spDocumentGeometry= from->spDocumentGeometry;

    to->spStyle= from->spStyle;

    to->spColumnCount= from->spColumnCount;
    to->spColumnSpacingTwips= from->spColumnSpacingTwips;
    to->spColumnWidthTwips= from->spColumnWidthTwips;

    to->spHasTitlePage= from->spHasTitlePage;
    to->spBreakKind= from->spBreakKind;
    to->spPageNumberStyle= from->spPageNumberStyle;
    to->spRestartPageNumbers= from->spRestartPageNumbers;

    to->spStartPageNumber= from->spStartPageNumber;

    to->spFootnoteProperties= from->spFootnoteProperties;
    to->spEndnoteProperties= from->spEndnoteProperties;

    return 0;
    }

