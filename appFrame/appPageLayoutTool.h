/************************************************************************/
/*									*/
/*  Page Layout page on an inspector:					*/
/*									*/
/************************************************************************/

#   ifndef	APP_PAGE_LAYOUT_TOOL_H
#   define	APP_PAGE_LAYOUT_TOOL_H

#   include	<appPaper.h>
#   include	<appPaperChooser.h>
#   include	<appMarginTool.h>

/************************************************************************/
/*									*/
/*  Resources for the 'Page Layout' page.				*/
/*									*/
/************************************************************************/

typedef struct PageLayoutPageResources
    {
    char *		plprUnitType;

    char *		plprPage;
    char *		plprCustomPageSizeText;
    char *		plprLandscapeText;
    char *		plprPortraitText;

    MarginToolResources	plprMarginToolResources;
    char *		plprPositionsText;
    char *		plprHeaderPositionText;
    char *		plprFooterPositionText;

    char *		plprRevertSelectionText;
    char *		plprChangeSelectionText;
    } PageLayoutPageResources;

/************************************************************************/
/*									*/
/*  A page layout tool, i.e. the 'Page Layout' page of the format tool.	*/
/*									*/
/************************************************************************/

typedef struct PageLayoutTool
    {
    EditApplication *			pltApplication;
    AppInspector *			pltInspector;
    const PageLayoutPageResources *	pltPageResources;

    int					pltManageHeadersFooters;
    int					pltManageSelection;

    int					pltUnitType;

    DocumentGeometry			pltGeometrySetSelection;
    DocumentGeometry			pltGeometrySetDocument;
    DocumentGeometry			pltGeometryChosen;

    APP_WIDGET				pltPageDrawing;
    PaperChooser			pltPaperChooser;
    AppMarginTool			pltMarginTool;
    AppDrawingData			pltDrawingData;
    int					pltDrawingDataAllocated;

    APP_WIDGET				pltHeaderFooterFrame;
    APP_WIDGET				pltHeaderFooterInside;
    APP_WIDGET				pltHeaderFooterLeftColumn;
    APP_WIDGET				pltHeaderFooterRightColumn;
    APP_WIDGET				pltHeaderPositionText;
    APP_WIDGET				pltFooterPositionText;

    APP_WIDGET				pltRevertSelectionWidget;
    APP_WIDGET				pltChangeSelectionWidget;
    APP_WIDGET				pltChangeDocumentWidget;
    } PageLayoutTool;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void appPageLayoutPageGetResourceTable(
				EditApplication *		ea,
				PageLayoutPageResources *	plpr,
				InspectorSubjectResources *	isr );

extern void appPageLayoutPageRefresh(
				PageLayoutTool *		plt,
				int *				pEnabled,
				InspectorSubject *		is,
				const DocumentGeometry *	dgSelection,
				const DocumentGeometry *	dgDoc );

extern void appPageLayoutPageFillPage(
				PageLayoutTool *		plt,
				int				doHdFt,
				int				doSelection,
				const PageLayoutPageResources *	plpr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void appPageLayoutPageFillChoosers(
				PageLayoutTool *		plt,
				const PageLayoutPageResources *	plpr );

extern void appPageLayoutPageFinishPage(
				PageLayoutTool *		plt,
				const PageLayoutPageResources *	plpr );

#   endif	/*  APP_PAGE_LAYOUT_TOOL_H */
