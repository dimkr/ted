#   ifndef	UTIL_NUP_H
#   define	UTIL_NUP_H

#   include	<geo2DInteger.h>
#   include	<utilDocumentGeometry.h>
#   include	<utilAffineTransform.h>

/************************************************************************/
/*									*/
/*  Position the pages of a document on successive sheets of paper.	*/
/*									*/
/************************************************************************/

typedef struct NupSchema
    {
    AffineTransform2D		nsBaseTransform;
    int				nsNup;
    int				nsRows;
    int				nsCols;
    AffineTransform2D *		nsNupTransforms;
    int				nsXShift;
    } NupSchema;

/************************************************************************/
/*									*/
/*  Geometry specifications for a print job.				*/
/*									*/
/************************************************************************/

typedef struct PrintGeometry
    {
    DocumentGeometry	pgSheetGeometry;
    int			pgRotatePage90;
    int			pgCenterPageHorizontally;
    int			pgGridRows;
    int			pgGridCols;
    int			pgGridHorizontal;
    int			pgScalePagesToFit;

    int			pgPrintOddSides;
    int			pgPrintEvenSides;
    int			pgPrintSheetsReverse;
    int			pgPrintBookletOrder;

    int			pgFirstPage;
    int			pgLastPage;
    int			pgUsePostScriptFilters;
    int			pgUsePostScriptIndexedImages;
    int			pgSkipEmptyPages;
    int			pgSkipBlankPages;
    int			pgOmitHeadersOnEmptyPages;
    } PrintGeometry;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void utilInitPrintGeometry(	PrintGeometry *	pg );
extern void utilCleanPrintGeometry(	PrintGeometry *	pg );

extern void utilInitNupSchema(	NupSchema *		ns );
extern void utilCleanNupSchema(	NupSchema *		ns );

extern int utilNupFitPagesToSheet(
			int *				pFitWithoutRot,
			int *				pFitWithRot,
			const PrintGeometry *		pg,
			const DocumentGeometry *	dgPage );

extern int utilNupGetBaseTranform(
			AffineTransform2D *		pAt1Page,
			int *				pRotatePages,
			const PrintGeometry *		pg,
			const DocumentGeometry *	dgPage,
			double				fac );

extern void utilNupSheetBoundingBox(
			DocumentRectangle *		sheetBBox,
			const NupSchema *		ns,
			const DocumentGeometry *	dgPage,
			int				hasPageHeader,
			int				hasPageFooter );

extern int utilNupSetSchema(
			NupSchema *			ns,
			int				rotateSheetGrid,
			const AffineTransform2D *	at1Page,
			const PrintGeometry *		pg,
			double				fac,
			const DocumentGeometry *	dgPage );

extern void utilNupGetPageTranform(
			AffineTransform2D *		at,
			const NupSchema *		ns,
			int				page );

#   endif	/*  UTIL_NUP_H  */
