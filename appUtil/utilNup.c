/************************************************************************/
/*									*/
/*  Manage the layout of document pages on sheets of paper.		*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<utilNup.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Basic bookkeeping.							*/
/*									*/
/************************************************************************/

void utilInitNupSchema(	NupSchema *	ns )
    {
    utilInitAffineTransform2D( &(ns->nsBaseTransform) );

    ns->nsNup= 1;
    ns->nsRows= 1;
    ns->nsCols= 1;
    ns->nsNupTransforms= (AffineTransform2D *)0;

    ns->nsXShift= 0;

    return;
    }

void utilCleanNupSchema(	NupSchema *	ns )
    {
    if  ( ns->nsNupTransforms )
	{ free( ns->nsNupTransforms );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Initialisation for 'nup' printing.					*/
/*									*/
/*  The transform returned is the transform to place a single page	*/
/*  in the upper left corner on the output sheet at the desired scale.	*/
/*									*/
/*  Without rotation the upper left corner of the document page is	*/
/*  moved to the uppper left corner of the sheet.			*/
/*  With rotation the upper right corner of the document page is	*/
/*  moved to the uppper left corner of the sheet. (And the upper left	*/
/*  corner of the document page goes to the lower left corner of the	*/
/*  sheet)								*/
/*									*/
/*  0)  Make an exception for the trivial situation where the page fits	*/
/*	on the sheet, to give the hach below a chance.			*/
/*  1)  Hack: When the page is higher than the sheet, shift by the page	*/
/*	size. This gives better results in case of misconfiguration.	*/
/*  2)  Calculate amounts to scale with and without rotation.		*/
/*  3)  scale without rotation.						*/
/*  4)  scale with rotation.						*/
/*									*/
/************************************************************************/

int utilNupFitPagesToSheet(
			int *				pFitWithoutRot,
			int *				pFitWithRot,
			const PrintGeometry *		pg,
			const DocumentGeometry *	dgPage )
    {
    int				fitsWithoutRot= 0;
    int				fitsWithRot= 0;

    const DocumentGeometry *	dgSheet= &(pg->pgSheetGeometry);
    int				cols= pg->pgGridCols;
    int				rows= pg->pgGridRows;
    double			cellWideTwips= dgSheet->dgPageWideTwips/ cols;
    double			cellHighTwips= dgSheet->dgPageHighTwips/ rows;

    if  ( dgPage->dgPageWideTwips <= cellWideTwips	&&
	  dgPage->dgPageHighTwips <= cellHighTwips	)
	{ fitsWithoutRot= 1;	}

    if  ( dgPage->dgPageWideTwips <= cellHighTwips	&&
	  dgPage->dgPageHighTwips <= cellWideTwips	)
	{ fitsWithRot= 1;	}

    *pFitWithoutRot= fitsWithoutRot;
    *pFitWithRot= fitsWithRot;

    if  ( ! fitsWithoutRot && ! fitsWithRot )
	{ return 1;	}

    return 0;
    }

int utilNupGetBaseTranform(
			AffineTransform2D *		pAt1Page,
			int *				pRotatePages,
			const PrintGeometry *		pg,
			const DocumentGeometry *	dgPage,
			double				fac )
    {
    double			axx;
    double			axy;
    double			ayx;
    double			ayy;
    int				fitsWithoutRot= 0;
    int				fitsWithRot= 0;

    double			tx= 0.0;
    double			ty= 0.0;

    int				rotatePages= 0;

    const DocumentGeometry *	dgSheet= &(pg->pgSheetGeometry);
    int				cols= pg->pgGridCols;
    int				rows= pg->pgGridRows;
    double			cellWideTwips= dgSheet->dgPageWideTwips/ cols;
    double			cellHighTwips= dgSheet->dgPageHighTwips/ rows;

    utilNupFitPagesToSheet( &fitsWithoutRot, &fitsWithRot, pg, dgPage );

    /*  0  */
    if  ( rows == 1		&&
	  cols == 1		&&
	  fitsWithoutRot	&&
	  ! pg->pgRotatePage90	)
	{
	/*  1  */
	if  ( dgPage->dgPageHighTwips > dgSheet->dgPageHighTwips )
	    { ty= dgPage->dgPageHighTwips;	}
	else{ ty= dgSheet->dgPageHighTwips;	}

	pAt1Page->at2Axx= fac* 1.0;
	pAt1Page->at2Axy= fac* 0.0;
	pAt1Page->at2Ayx= fac* 0.0;
	pAt1Page->at2Ayy= fac* -1.0;
	pAt1Page->at2Tx= fac* tx;
	pAt1Page->at2Ty= fac* ty;

	*pRotatePages= 0;
	return 0;
	}

    /*  0  */
    if  ( rows == 1		&&
	  cols == 1		&&
	  fitsWithRot		)
	{
	if  ( dgPage->dgPageWideTwips < dgSheet->dgPageHighTwips )
	    { ty= dgSheet->dgPageHighTwips- dgPage->dgPageWideTwips;	}

	pAt1Page->at2Axx= fac* 0.0;
	pAt1Page->at2Axy= fac* 1.0;
	pAt1Page->at2Ayx= fac* 1.0;
	pAt1Page->at2Ayy= fac* 0.0;
	pAt1Page->at2Tx= fac* tx;
	pAt1Page->at2Ty= fac* ty;

	*pRotatePages= 1;
	return 0;
	}

    /*  2  */
    axx= ( 1.0* cellWideTwips )/ ( 1.0* dgPage->dgPageWideTwips );
    axy= ( 1.0* cellHighTwips )/ ( 1.0* dgPage->dgPageWideTwips );
    ayx= ( 1.0* cellWideTwips )/ ( 1.0* dgPage->dgPageHighTwips );
    ayy= ( 1.0* cellHighTwips )/ ( 1.0* dgPage->dgPageHighTwips );

    if  ( axx > 1.0 )	{ axx= 1.0;	}
    if  ( axy > 1.0 )	{ axy= 1.0;	}
    if  ( ayx > 1.0 )	{ ayx= 1.0;	}
    if  ( ayy > 1.0 )	{ ayy= 1.0;	}

    /*  3  */
    if  ( axx > ayy )
	{ axx=  ayy; }

    /*  4  */
    if  ( axy > ayx )
	{ axy=  ayx; }

    if  ( axy > 1.02* axx )
	{ rotatePages= 1;	}
    else{ rotatePages= 0;	}

    if  ( ! pg->pgScalePagesToFit )
	{ axx= axy= 1.0;	}

    if  ( rotatePages )
	{
	ty= dgSheet->dgPageHighTwips- cellHighTwips;

	pAt1Page->at2Axx= fac* 0.0;
	pAt1Page->at2Axy= fac* axy;
	pAt1Page->at2Ayx= fac* axy;
	pAt1Page->at2Ayy= fac* 0.0;
	pAt1Page->at2Tx= fac* tx;
	pAt1Page->at2Ty= fac* ty;
	}
    else{
	ty= cellHighTwips;

	pAt1Page->at2Axx= fac* axx;
	pAt1Page->at2Axy= fac* 0.0;
	pAt1Page->at2Ayx= fac* 0.0;
	pAt1Page->at2Ayy= fac* -axx;
	pAt1Page->at2Tx= fac* tx;
	pAt1Page->at2Ty= fac* ty;
	}

    *pRotatePages= rotatePages; return 0;
    }

/************************************************************************/
/*									*/
/*  Determine the bounding box for one sheet.				*/
/*									*/
/************************************************************************/

void utilNupSheetBoundingBox(
			DocumentRectangle *		sheetBBox,
			const NupSchema *		ns,
			const DocumentGeometry *	dgPage,
			int				hasPageHeader,
			int				hasPageFooter )
    {
    int				x0;
    int				y0;
    int				x1;
    int				y1;

    DocumentRectangle		dr;

    int				i;

    const AffineTransform2D *	at;

    /**/
    if  ( ns->nsNup == 1 )
	{ at= &(ns->nsBaseTransform);	}
    else{ at= ns->nsNupTransforms;	}

    /**/
    x0= dgPage->dgLeftMarginTwips;
    x1= dgPage->dgPageWideTwips- dgPage->dgRightMarginTwips;

    if  ( hasPageHeader )
	{ y0= dgPage->dgHeaderPositionTwips;	}
    else{ y0= dgPage->dgTopMarginTwips;		}

    if  ( hasPageFooter )
	{ y1= dgPage->dgPageHighTwips- dgPage->dgFooterPositionTwips;	}
    else{ y1= dgPage->dgPageHighTwips- dgPage->dgBottomMarginTwips;	}

    for ( i= 0; i < ns->nsNup; at++, i++ )
	{
	dr.drX0= AT2_X( x0, y0, at );
	dr.drY0= AT2_Y( x0, y0, at );
	dr.drX1= AT2_X( x1, y1, at );
	dr.drY1= AT2_Y( x1, y1, at );

	dr.drX0 += ns->nsXShift;
	dr.drX1 += ns->nsXShift;

	docNormalizeRectangle( &dr, &dr );

	if  ( i == 0 )
	    { *sheetBBox= dr;					}
	else{ geoUnionRectangle( sheetBBox, sheetBBox, &dr );	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Get the transform for a certain page.				*/
/*									*/
/************************************************************************/

void utilNupGetPageTranform(
			AffineTransform2D *		at,
			const NupSchema *		ns,
			int				page )
    {
    if  ( ns->nsNup == 1 )
	{
	*at= ns->nsBaseTransform;
	}
    else{
	*at= ns->nsNupTransforms[page % ns->nsNup];
	}

    at->at2Tx += ns->nsXShift;

    return;
    }

/************************************************************************/
/*									*/
/*  Set the transform for the individual pages in a nup != 1 schema	*/
/*									*/
/*  Rows and columns are the 'reading order' of the sheet.		*/
/*  Realize that PostScript coordinates are oriented as we were taught	*/
/*  at school. I.E. with the Y axis pointing up.			*/
/*									*/
/************************************************************************/

static void utilNupSetSheetGrid(	NupSchema *		ns,
					const PrintGeometry *	pg,
					int			rotateGrid,
					double			fac )
    {
    const DocumentGeometry *	dgSheet= &(pg->pgSheetGeometry);
    int				cols= pg->pgGridCols;
    int				rows= pg->pgGridRows;
    const int			cellWideTwips= dgSheet->dgPageWideTwips/ cols;
    const int			cellHighTwips= dgSheet->dgPageHighTwips/ rows;

    int			row;
    int			col;

    if  ( ns->nsNup <= 1 || rows* cols != ns->nsNup )
	{ LLLDEB(rows,cols,ns->nsNup); return;	}

    if  ( rotateGrid )
	{
	AffineTransform2D *	at;

	at= ns->nsNupTransforms;
	for ( col= 0; col < cols; col++ )
	    {
	    double	x= col* cellWideTwips;

	    for ( row= rows- 1; row >= 0; at++, row-- )
		{
		double	y= -row* cellHighTwips;

		*at= ns->nsBaseTransform;

		at->at2Tx += fac* x;
		at->at2Ty += fac* y;
		}
	    }
	}
    else{
	AffineTransform2D *	at;

	if  ( pg->pgGridHorizontal )
	    {
	    at= ns->nsNupTransforms;
	    for ( row= 0; row < rows; row++ )
		{
		double	y= dgSheet->dgPageHighTwips- ( row+ 1 )* cellHighTwips;

		for ( col= 0; col < cols; at++, col++ )
		    {
		    double	x= col* cellWideTwips;

		    *at= ns->nsBaseTransform;

		    at->at2Tx += fac* x;
		    at->at2Ty += fac* y;
		    }
		}
	    }
	else{
	    at= ns->nsNupTransforms;
	    for ( col= 0; col < cols; col++ )
		{
		double	x= col* cellWideTwips;

		for ( row= 0; row < rows; at++, row++ )
		    {
		    double	y= dgSheet->dgPageHighTwips-
						    ( row+ 1 )* cellHighTwips;

		    *at= ns->nsBaseTransform;

		    at->at2Tx += fac* x;
		    at->at2Ty += fac* y;
		    }
		}
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Fill a Nup Schema.							*/
/*									*/
/************************************************************************/

int utilNupSetSchema(	NupSchema *			ns,
			int				rotateSheetGrid,
			const AffineTransform2D *	at1Page,
			const PrintGeometry *		pg,
			double				fac,
			const DocumentGeometry *	dgPage )
    {
    AffineTransform2D *		at;

    int				nup= pg->pgGridRows* pg->pgGridCols;

    ns->nsBaseTransform= *at1Page;

    if  ( nup == 1 )
	{
	if  ( pg->pgCenterPageHorizontally )
	    {
	    const DocumentGeometry *	dgSheet= &(pg->pgSheetGeometry);

	    if  ( rotateSheetGrid )
		{
		ns->nsXShift=
		    ( dgSheet->dgPageWideTwips- dgPage->dgPageHighTwips )/2;
		}
	    else{
		ns->nsXShift=
		    ( dgSheet->dgPageWideTwips- dgPage->dgPageWideTwips )/2;
		}

	    ns->nsXShift *= fac;
	    }

	ns->nsRows= pg->pgGridRows;
	ns->nsCols= pg->pgGridCols;
	ns->nsNup= nup;
	}
    else{
	if  ( pg->pgRotatePage90 || pg->pgCenterPageHorizontally )
	    { LLDEB(pg->pgRotatePage90,pg->pgCenterPageHorizontally);	}

	ns->nsXShift= 0;

	at= realloc( ns->nsNupTransforms, nup* sizeof( AffineTransform2D ) );
	if  ( ! at )
	    { XDEB(at); return -1;	}
	ns->nsNupTransforms= at;

	ns->nsRows= pg->pgGridRows;
	ns->nsCols= pg->pgGridCols;
	ns->nsNup= nup;

	utilNupSetSheetGrid( ns, pg, rotateSheetGrid, fac );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Initialize printing geometry.					*/
/*  (Not quite a geometry, now that the page ranges are added)		*/
/*									*/
/************************************************************************/

void utilInitPrintGeometry(	PrintGeometry *	pg )
    {
    utilInitDocumentGeometry( &(pg->pgSheetGeometry) );

    pg->pgRotatePage90= 0;
    pg->pgCenterPageHorizontally= 0;
    pg->pgGridRows= 1;
    pg->pgGridCols= 1;
    pg->pgGridHorizontal= 0;

    pg->pgScalePagesToFit= 0;

    pg->pgPrintOddSides= 1;
    pg->pgPrintEvenSides= 1;
    pg->pgPrintSheetsReverse= 0;
    pg->pgPrintBookletOrder= 0;

    pg->pgFirstPage= -1;
    pg->pgLastPage= -1;
    pg->pgUsePostScriptFilters= 1;
    pg->pgUsePostScriptIndexedImages= 1;
    pg->pgSkipEmptyPages= 0;
    pg->pgSkipBlankPages= 0;
    pg->pgOmitHeadersOnEmptyPages= 0;

    return;
    }

void utilCleanPrintGeometry(	PrintGeometry *	pg )
    {
    return;
    }
