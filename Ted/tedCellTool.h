/************************************************************************/
/*									*/
/*  Cell tool:								*/
/*									*/
/************************************************************************/

#   ifndef	TED_CELL_TOOL_H
#   define	TED_CELL_TOOL_H

#   include	"tedBorderTool.h"
#   include	"tedShadingTool.h"

/************************************************************************/
/*									*/
/*  Resources for the 'Cell' page.					*/
/*									*/
/************************************************************************/

typedef struct CellPageResources
    {
    char *			cprRow;
    char *			cprColumn;

    char *			cprColspan;
    char *			cprRowspan;

    char *			cprWidth;
    char *			cprLeftBorder;
    char *			cprRightBorder;

    char *			cprTopBorder;
    char *			cprBottomBorder;
    char *			cprShadingPattern;
    char *			cprVerticalAlignment;

    char *			cprShading;
    ShadingToolResources	cprShadingResources;
    BorderToolResources		cprBorderToolResources;
    } CellPageResources;

/************************************************************************/
/*									*/
/*  A Cell tool, i.e. the 'Cell' page of the format tool.		*/
/*									*/
/************************************************************************/

typedef struct CellTool
    {
    EditApplication *		ctApplication;
    AppInspector *		ctInspector;
    const CellPageResources *	ctPageResources;

    TableRectangle		ctTableRectangle;

    APP_WIDGET			ctRowText;
    APP_WIDGET			ctColumnText;
    APP_WIDGET			ctColspanText;
    APP_WIDGET			ctRowspanText;

    RowProperties		ctRowPropertiesSet;
    RowProperties		ctRowPropertiesChosen;

    BorderTool			ctTopBorderTool;
    BorderTool			ctBottomBorderTool;
    BorderTool			ctLeftBorderTool;
    BorderTool			ctRightBorderTool;

    ShadingTool			ctShadingTool;
    } CellTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatToolRefreshCellTool(
				CellTool *			ct,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	bs );

extern void tedCellToolFillChoosers(	CellTool *			ct,
					const CellPageResources *	cpr );

extern void tedFormatFillCellPage( CellTool *			ct,
				const CellPageResources *	cpr,
				AppInspector *			ai,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void tedFormatToolGetCellResourceTable(
					EditApplication *		ea,
					CellPageResources *		cpr,
					InspectorSubjectResources *	isr );

extern void tedInitCellTool(	CellTool *			ct );
extern void tedCleanCellTool(	CellTool *			ct );

#   endif	/*  TED_CELL_TOOL_H */
