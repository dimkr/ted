/************************************************************************/
/*									*/
/*  Row tool:								*/
/*									*/
/************************************************************************/

#   ifndef	TED_ROW_TOOL_H
#   define	TED_ROW_TOOL_H

/************************************************************************/
/*									*/
/*  Resources for the 'Row' page.					*/
/*									*/
/************************************************************************/

typedef struct RowPageResources
    {
    char *		rprHeightFree;
    char *		rprHeightAtLeast;
    char *		rprHeightExactly;

    char *		rprTopBorder;
    char *		rprBottomBorder;

    char *		rprCellMargin;
    char *		rprLeftIndent;

    char *		rprKeepOnPage;
    char *		rprIsTableHeader;

    BorderToolResources	rprBorderToolResources;
    } RowPageResources;

/************************************************************************/
/*									*/
/*  A row tool, i.e. the 'Row' page of the format tool.			*/
/*									*/
/************************************************************************/

typedef struct RowTool
    {
    EditApplication *		rtApplication;
    AppInspector *		rtInspector;
    const RowPageResources *	rtPageResources;

    RowProperties		rtPropertiesSet;
    RowProperties		rtPropertiesChosen;

    TableRectangle		rtTableRectangle;

    int				rtPageRight;
    int				rtPageLeftMargin;

    APP_WIDGET			rtNumberText;
    HeightChooser		rtHeightChooser;
    APP_WIDGET			rtCellMarginText;
    APP_WIDGET			rtLeftIndentText;

    BorderTool			rtTopBorderTool;
    BorderTool			rtBottomBorderTool;

    APP_WIDGET			rtKeepOnPageToggle;
    APP_WIDGET			rtIsTableHeaderToggle;
    } RowTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatToolGetRowResourceTable(
				EditApplication *		ea,
				RowPageResources *		spr,
				InspectorSubjectResources *	isr );

extern void tedFormatToolRefreshRowTool(
				RowTool *			rt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	bs );

extern void tedInitRowTool(	RowTool *	rt );
extern void tedCleanRowTool(	RowTool *	rt );

#   endif	/*  TED_ROW_TOOL_H */
