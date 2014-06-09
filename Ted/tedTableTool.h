/************************************************************************/
/*									*/
/*  'Table' page of the format tool.					*/
/*									*/
/************************************************************************/

#   ifndef	TED_TABLE_TOOL_H
#   define	TED_TABLE_TOOL_H

/************************************************************************/
/*									*/
/*  Resources for the 'Table' page.					*/
/*									*/
/************************************************************************/

typedef struct TablePageResources
    {
    char *		tprCellMargin;
    char *		tprLeftIndent;
    } TablePageResources;

/************************************************************************/
/*									*/
/*  A table tool, i.e. the 'Table' page of the format tool.		*/
/*									*/
/************************************************************************/

typedef struct TableTool
    {
    EditApplication *		ttApplication;
    AppInspector *		ttInspector;
    const TablePageResources *	ttPageResources;

    RowProperties		ttPropertiesSet;
    RowProperties		ttPropertiesChosen;

    TableRectangle		ttTableRectangle;

    int				ttPageRight;
    int				ttPageLeftMargin;

    APP_WIDGET			ttCellMarginText;
    APP_WIDGET			ttLeftIndentText;
    } TableTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatFillTablePage( TableTool *			tt,
				const TablePageResources *	tpr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void tedFormatToolGetTableResourceTable(
					EditApplication *		ea,
					TablePageResources *		tpr,
					InspectorSubjectResources *	isr );

extern void tedFormatToolRefreshTableTool(
				TableTool *			tt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	bs );

extern void tedFormatCleanTableTool(	TableTool *		tt );

#   endif	/*  TED_TABLE_TOOL_H */
