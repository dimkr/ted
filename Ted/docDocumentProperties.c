/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<utilPropMask.h>
#   include	<appDebugon.h>

#   include	"docDocumentProperties.h"

/************************************************************************/
/*									*/
/*  Initialise Document Properties.					*/
/*									*/
/************************************************************************/

void docInitDocumentProperties(	DocumentProperties *	dp )
    {
    dp->dpContainsTables= 0;
    dp->dpTabIntervalTwips= 720;
    dp->dpDefaultColor= 0;
    dp->dpAnsiCodepage= -1;

    dp->dpDefaultFont= -1;
    dp->dpDefaultFontDbch= -1;
    dp->dpDefaultFontLoch= -1;
    dp->dpDefaultFontHich= -1;
    dp->dpDefaultFontBi= -1;

    dp->dpStartPageNumber= 1;
    dp->dpFootEndNoteType= DPfetFOOT_ONLY;

    dp->dpFootnoteProperties.npStartNumber= 1;
    dp->dpFootnoteProperties.npPosition= FTN_POS_PAGE_BOTTOM;
    dp->dpFootnoteProperties.npRestart= FTN_RST_CONTINUOUS;
    dp->dpFootnoteProperties.npNumberStyle= FTNstyleNAR;

    dp->dpEndnoteProperties.npStartNumber= 1;
    dp->dpEndnoteProperties.npPosition= FTN_POS_DOC_END;
    dp->dpEndnoteProperties.npRestart= FTN_RST_CONTINUOUS;
    dp->dpEndnoteProperties.npNumberStyle= FTNstyleNRLC;

    dp->dpHasFacingPages= 0;
    dp->dpWidowControl= 0;
    dp->dpTwoOnOne= 0;
    dp->dpIsDocumentTemplate= 0;

    utilInitDocumentGeometry( &(dp->dpGeometry) );

    docInitFontList( &(dp->dpFontList) );
    docInitListTable( &(dp->dpListTable) );
    docInitListOverrideTable( &(dp->dpListOverrideTable) );

    dp->dpColors= (RGB8Color *)0;
    dp->dpColorCount= 0;

    dp->dpAuthors= (unsigned char **)0;
    dp->dpAuthorCount= 0;

    dp->dpGenerator= (unsigned char *)0;
    dp->dpTitle= (unsigned char *)0;
    dp->dpSubject= (unsigned char *)0;
    dp->dpKeywords= (unsigned char *)0;
    dp->dpDoccomm= (unsigned char *)0;
    dp->dpAuthor= (unsigned char *)0;
    dp->dpCompany= (unsigned char *)0;
    dp->dpHlinkbase= (unsigned char *)0;
    dp->dpFilename= (unsigned char *)0;

    docInitBorderProperties( &(dp->dpTopBorder) );
    docInitBorderProperties( &(dp->dpLeftBorder) );
    docInitBorderProperties( &(dp->dpRightBorder) );
    docInitBorderProperties( &(dp->dpBottomBorder) );
    docInitBorderProperties( &(dp->dpHeadBorder) );
    docInitBorderProperties( &(dp->dpFootBorder) );

    appInvalidateTime( &(dp->dpCreatim) );
    appInvalidateTime( &(dp->dpRevtim) );
    appInvalidateTime( &(dp->dpPrintim) );
    }

void docCleanDocumentProperties(	DocumentProperties *	dp )
    {
    int		i;

    docCleanFontList( &(dp->dpFontList) );
    docCleanListTable( &(dp->dpListTable) );
    docCleanListOverrideTable( &(dp->dpListOverrideTable) );

    if  ( dp->dpColors )
	{ free( dp->dpColors );	}

    for ( i= 0; i < dp->dpAuthorCount; i++ )
	{ free( dp->dpAuthors[i] );	}
    if  ( dp->dpAuthors )
	{ free( dp->dpAuthors );	}

    appInvalidateTime( &(dp->dpCreatim) );
    appInvalidateTime( &(dp->dpRevtim) );
    appInvalidateTime( &(dp->dpPrintim) );

    if  ( dp->dpGenerator )
	{ free( dp->dpGenerator );	}
    if  ( dp->dpTitle )
	{ free( dp->dpTitle );	}
    if  ( dp->dpSubject )
	{ free( dp->dpSubject );	}
    if  ( dp->dpKeywords )
	{ free( dp->dpKeywords );	}
    if  ( dp->dpDoccomm )
	{ free( dp->dpDoccomm );	}
    if  ( dp->dpAuthor )
	{ free( dp->dpAuthor );	}
    if  ( dp->dpCompany )
	{ free( dp->dpCompany );	}
    if  ( dp->dpHlinkbase )
	{ free( dp->dpHlinkbase );	}
    if  ( dp->dpFilename )
	{ free( dp->dpFilename );	}

    return;
    }

int docCopyDocumentProperties(	DocumentProperties *		to,
				const DocumentProperties *	from )
    {
    int			i;
    DocumentProperties	copy;

    copy= *from;

    docInitFontList( &(copy.dpFontList) );
    docInitListTable( &(copy.dpListTable) );
    docInitListOverrideTable( &(copy.dpListOverrideTable) );

    copy.dpColorCount= 0;
    copy.dpColors= (RGB8Color *)0;
    copy.dpAuthorCount= 0;
    copy.dpAuthors= (unsigned char **)0;

    copy.dpGenerator= (unsigned char *)0;
    copy.dpTitle= (unsigned char *)0;
    copy.dpSubject= (unsigned char *)0;
    copy.dpKeywords= (unsigned char *)0;
    copy.dpDoccomm= (unsigned char *)0;
    copy.dpAuthor= (unsigned char *)0;
    copy.dpCompany= (unsigned char *)0;
    copy.dpHlinkbase= (unsigned char *)0;
    copy.dpFilename= (unsigned char *)0;

    if  ( docCopyFontList( &(copy.dpFontList), &(from->dpFontList) ) )
	{ LDEB(1); docCleanDocumentProperties( &copy ); return -1;	}

    if  ( docCopyListTable( &(copy.dpListTable), &(from->dpListTable) ) )
	{ LDEB(1); docCleanDocumentProperties( &copy ); return -1;	}

    if  ( docCopyListOverrideTable( &(copy.dpListOverrideTable),
					    &(from->dpListOverrideTable) ) )
	{ LDEB(1); docCleanDocumentProperties( &copy ); return -1;	}

    /**/
    copy.dpColors= (RGB8Color *)
			malloc( from->dpColorCount* sizeof(RGB8Color) );
    if  ( ! copy.dpColors )
	{
	XDEB(copy.dpColors);
	docCleanDocumentProperties( &copy ); return -1;
	}
    for ( i= 0; i < from->dpColorCount; i++ )
	{ copy.dpColors[i]= from->dpColors[i]; }
    copy.dpColorCount= from->dpColorCount;

    /**/
    copy.dpAuthors= (unsigned char **)
		    malloc( from->dpAuthorCount* sizeof(unsigned char *) );
    if  ( ! copy.dpAuthors )
	{
	XDEB(copy.dpAuthors);
	docCleanDocumentProperties( &copy ); return -1;
	}
    for ( i= 0; i < from->dpAuthorCount; i++ )
	{ copy.dpAuthors[i]= (unsigned char *)0; }
    copy.dpAuthorCount= from->dpAuthorCount;
    for ( i= 0; i < from->dpAuthorCount; i++ )
	{
	copy.dpAuthors[i]= (unsigned char *)
				    strdup( (char *)from->dpAuthors[i] );
	if  ( ! copy.dpAuthors[i] )
	    {
	    XDEB(copy.dpAuthors[i]);
	    docCleanDocumentProperties( &copy ); return -1;
	    }
	}

    if  ( from->dpGenerator )
	{
	copy.dpGenerator= (unsigned char *)strdup( (char *)from->dpGenerator );
	if  ( ! copy.dpGenerator )
	    {
	    XDEB(copy.dpGenerator);
	    docCleanDocumentProperties( &copy ); return -1;
	    }
	}

    if  ( from->dpTitle )
	{
	copy.dpTitle= (unsigned char *)strdup( (char *)from->dpTitle );
	if  ( ! copy.dpTitle )
	    {
	    XDEB(copy.dpTitle);
	    docCleanDocumentProperties( &copy ); return -1;
	    }
	}

    if  ( from->dpSubject )
	{
	copy.dpSubject= (unsigned char *)strdup( (char *)from->dpSubject );
	if  ( ! copy.dpSubject )
	    {
	    XDEB(copy.dpSubject);
	    docCleanDocumentProperties( &copy ); return -1;
	    }
	}

    if  ( from->dpKeywords )
	{
	copy.dpKeywords= (unsigned char *)strdup( (char *)from->dpKeywords );
	if  ( ! copy.dpKeywords )
	    {
	    XDEB(copy.dpKeywords);
	    docCleanDocumentProperties( &copy ); return -1;
	    }
	}

    if  ( from->dpDoccomm )
	{
	copy.dpDoccomm= (unsigned char *)strdup( (char *)from->dpDoccomm );
	if  ( ! copy.dpDoccomm )
	    {
	    XDEB(copy.dpDoccomm);
	    docCleanDocumentProperties( &copy ); return -1;
	    }
	}

    if  ( from->dpAuthor )
	{
	copy.dpAuthor= (unsigned char *)strdup( (char *)from->dpAuthor );
	if  ( ! copy.dpAuthor )
	    {
	    XDEB(copy.dpAuthor);
	    docCleanDocumentProperties( &copy ); return -1;
	    }
	}

    if  ( from->dpCompany )
	{
	copy.dpCompany= (unsigned char *)strdup( (char *)from->dpCompany );
	if  ( ! copy.dpCompany )
	    {
	    XDEB(copy.dpCompany);
	    docCleanDocumentProperties( &copy ); return -1;
	    }
	}

    if  ( from->dpHlinkbase )
	{
	copy.dpHlinkbase= (unsigned char *)strdup( (char *)from->dpHlinkbase );
	if  ( ! copy.dpHlinkbase )
	    {
	    XDEB(copy.dpHlinkbase);
	    docCleanDocumentProperties( &copy ); return -1;
	    }
	}

    if  ( from->dpFilename )
	{
	copy.dpFilename= (unsigned char *)strdup( (char *)from->dpFilename );
	if  ( ! copy.dpFilename )
	    {
	    XDEB(copy.dpFilename);
	    docCleanDocumentProperties( &copy ); return -1;
	    }
	}

    docCleanDocumentProperties( to );
    *to= copy;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set the file name in the document properties.			*/
/*									*/
/************************************************************************/

int docPropertiesSetFilename(	DocumentProperties *	dp,
				const char *		filename )
    {
    char *	saved= (char *)0;

    if  ( filename )
	{
	saved= strdup( filename );
	if  ( ! saved )
	    { XDEB(saved); return -1;	}
	}

    if  ( dp->dpFilename )
	{ free( dp->dpFilename );	}

    dp->dpFilename= (unsigned char *)saved;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Change document properties and tell what has been changed.		*/
/*									*/
/************************************************************************/

int docUpdDocumentProperties(	PropertyMask *			pDoneMask,
				DocumentProperties *		dpTo,
				const PropertyMask *		updMask,
				const DocumentProperties *	dpFrom )
    {
    PropertyMask	doneMask;
    PropertyMask	dgMask;

    const int * const	colorMap= (const int *)0;

    PROPmaskCLEAR( &doneMask );

    PROPmaskCLEAR( &dgMask );
    utilPropMaskFill( &dgMask, DGprop_COUNT );
    utilPropMaskAnd( &dgMask, &dgMask, updMask );

    utilUpdDocumentGeometry( &(dpTo->dpGeometry), &(dpFrom->dpGeometry),
							&doneMask, &dgMask );

    if  ( PROPmaskISSET( updMask, DPpropSTART_PAGE ) )
	{
	if  ( dpTo->dpStartPageNumber != dpFrom->dpStartPageNumber )
	    {
	    dpTo->dpStartPageNumber= dpFrom->dpStartPageNumber;
	    PROPmaskADD( &doneMask, DPpropSTART_PAGE );
	    }
	}

    if  ( PROPmaskISSET( updMask, DPpropFACING_PAGES ) )
	{
	if  ( dpTo->dpHasFacingPages != dpFrom->dpHasFacingPages )
	    {
	    dpTo->dpHasFacingPages= dpFrom->dpHasFacingPages;
	    PROPmaskADD( &doneMask, DPpropFACING_PAGES );
	    }
	}

    docUpdNotesProperties( &doneMask, &(dpTo->dpFootnoteProperties),
				updMask, &(dpFrom->dpFootnoteProperties),
				DPpropFOOTNOTE_STARTNR,
				DPpropFOOTNOTE_POSITION,
				DPpropFOOTNOTE_RESTART,
				DPpropFOOTNOTE_STYLE );

    docUpdNotesProperties( &doneMask, &(dpTo->dpEndnoteProperties),
				updMask, &(dpFrom->dpEndnoteProperties),
				DPpropENDNOTE_STARTNR,
				DPpropENDNOTE_POSITION,
				DPpropENDNOTE_RESTART,
				DPpropENDNOTE_STYLE );

    if  ( PROPmaskISSET( updMask, DPpropDEFTAB ) )
	{
	if  ( dpTo->dpTabIntervalTwips != dpFrom->dpTabIntervalTwips )
	    {
	    dpTo->dpTabIntervalTwips= dpFrom->dpTabIntervalTwips;
	    PROPmaskADD( &doneMask, DPpropDEFTAB );
	    }
	}

    if  ( PROPmaskISSET( updMask, DPpropANSICPG ) )
	{
	if  ( dpTo->dpAnsiCodepage != dpFrom->dpAnsiCodepage )
	    {
	    dpTo->dpAnsiCodepage= dpFrom->dpAnsiCodepage;
	    PROPmaskADD( &doneMask, DPpropANSICPG );
	    }
	}

    if  ( PROPmaskISSET( updMask, DPpropTOP_BORDER ) )
	{
	if  ( docBordersDiffer( &(dpTo->dpTopBorder),
					&(dpFrom->dpTopBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(dpTo->dpTopBorder),
					&(dpFrom->dpTopBorder), colorMap );
	    PROPmaskADD( &doneMask, DPpropTOP_BORDER );
	    }
	}

    if  ( PROPmaskISSET( updMask, DPpropBOTTOM_BORDER ) )
	{
	if  ( docBordersDiffer( &(dpTo->dpBottomBorder),
					&(dpFrom->dpBottomBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(dpTo->dpBottomBorder),
					&(dpFrom->dpBottomBorder), colorMap );
	    PROPmaskADD( &doneMask, DPpropBOTTOM_BORDER );
	    }
	}

    if  ( PROPmaskISSET( updMask, DPpropLEFT_BORDER ) )
	{
	if  ( docBordersDiffer( &(dpTo->dpLeftBorder),
					&(dpFrom->dpLeftBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(dpTo->dpLeftBorder),
					&(dpFrom->dpLeftBorder), colorMap );
	    PROPmaskADD( &doneMask, DPpropLEFT_BORDER );
	    }
	}

    if  ( PROPmaskISSET( updMask, DPpropRIGHT_BORDER ) )
	{
	if  ( docBordersDiffer( &(dpTo->dpRightBorder),
					&(dpFrom->dpRightBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(dpTo->dpRightBorder),
					&(dpFrom->dpRightBorder), colorMap );
	    PROPmaskADD( &doneMask, DPpropRIGHT_BORDER );
	    }
	}

    if  ( PROPmaskISSET( updMask, DPpropHEAD_BORDER ) )
	{
	if  ( docBordersDiffer( &(dpTo->dpHeadBorder),
					&(dpFrom->dpHeadBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(dpTo->dpHeadBorder),
					&(dpFrom->dpHeadBorder), colorMap );
	    PROPmaskADD( &doneMask, DPpropHEAD_BORDER );
	    }
	}

    if  ( PROPmaskISSET( updMask, DPpropFOOT_BORDER ) )
	{
	if  ( docBordersDiffer( &(dpTo->dpFootBorder),
					&(dpFrom->dpFootBorder), colorMap ) )
	    {
	    docCopyBorderProperties( &(dpTo->dpFootBorder),
					&(dpFrom->dpFootBorder), colorMap );
	    PROPmaskADD( &doneMask, DPpropFOOT_BORDER );
	    }
	}


    *pDoneMask= doneMask;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Allocate a color.							*/
/*									*/
/************************************************************************/

int docAllocateDocumentColor(		DocumentProperties *	dp,
					const RGB8Color *	rgb8 )
    {
    const int			maxColors= 256;
    const int			avoidZero= 1;
    int				color;

    color= bmInsertColor( &(dp->dpColors), &(dp->dpColorCount),
						avoidZero, maxColors, rgb8 );
    if  ( color < 0 )
	{ LDEB(color); return -1;	}

    return color;
    }

/************************************************************************/
/*									*/
/*  As at least MS-Word 2000 crashes on lists without an oveeride,	*/
/*  and on the other hand staroffice/openoffice creates them under	*/
/*  certain circumstances: Make an empty override for lists without	*/
/*  overrides.								*/
/*									*/
/************************************************************************/

int docMakeOverrideForEveryList(	DocumentProperties *	dp )
    {
    const DocumentListTable *	dlt= &(dp->dpListTable);
    const DocumentList *	dl;

    int				li;

    ListOverrideTable *		lot= &(dp->dpListOverrideTable);

    dl= dlt->dltLists;
    for ( li= 0; li < dlt->dltListCount; dl++, li++ )
	{
	int			ov;
	const ListOverride *	lo;

	lo= lot->lotOverrides;
	for ( ov= 0; ov < lot->lotOverrideCount; lo++, ov++ )
	    {
	    if  ( lo->loIndex < 1 )
		{ continue;	}

	    if  ( lo->loListID == dl->dlListID )
		{ break; }
	    }

	if  ( ov >= lot->lotOverrideCount )
	    {
	    int			ret;
	    ListOverride	loNew;
	    int			idx;

	    const int * const	fontMap= (const int *)0;
	    const int * const	colorMap= (const int *)0;

	    docInitListOverride( &loNew );

	    loNew.loListID= dl->dlListID;
	    loNew.loListIndex= li;

	    idx= lot->lotOverrideCount;
	    if  ( idx < 1 )
		{ idx=  1;	}

	    ret= docListOverrideTableAddOverride( lot, &loNew, idx,
							fontMap, colorMap );

	    docCleanListOverride( &loNew );

	    if  ( ret < 0 )
		{ LDEB(ret); return -1;	}
	    }
	}

    return 0;
    }
