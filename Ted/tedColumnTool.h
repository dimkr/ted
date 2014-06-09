/************************************************************************/
/*									*/
/*  Column Tool.							*/
/*									*/
/************************************************************************/

#   ifndef	TED_COLUMN_TOOL_H
#   define	TED_COLUMN_TOOL_H

/************************************************************************/
/*									*/
/*  Resources for the 'Column' page.					*/
/*									*/
/************************************************************************/

typedef struct ColumnPageResources
    {
    char *		cprRow;
    char *		cprColumn;

    char *		cprWidth;
    char *		cprLeftBorder;
    char *		cprRightBorder;

    BorderToolResources	cprBorderToolResources;
    } ColumnPageResources;

/************************************************************************/
/*									*/
/*  A Column tool, i.e. the 'Column' page of the format tool.		*/
/*									*/
/************************************************************************/

typedef struct ColumnTool
    {
    EditApplication *		ctApplication;
    AppInspector *		ctInspector;
    const ColumnPageResources *	ctPageResources;

    TableRectangle		ctTableRectangle;

    int				ctPageRight;
    int				ctPageLeftMargin;

    RowProperties		ctPropertiesSet;
    RowProperties		ctPropertiesChosen;

    APP_WIDGET			ctNumberText;
    APP_WIDGET			ctWidthText;

    BorderTool			ctLeftBorderTool;
    BorderTool			ctRightBorderTool;
    } ColumnTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatToolRefreshColumnTool(
				    ColumnTool *		ct,
				    int *			pEnabled,
				    int *			pPref,
				    InspectorSubject *		is,
				    const DocumentSelection *	bs );

extern void tedInitColumnTool(	ColumnTool *	ct );
extern void tedCleanColumnTool(	ColumnTool *	ct );

#   endif	/*  TED_COLUMN_TOOL_H */
