/************************************************************************/
/*									*/
/*  Intermediary Routines for the page layout page.			*/
/*									*/
/************************************************************************/

#   ifndef	TED_PAGE_LAYOUT_TOOL_H
#   define	TED_PAGE_LAYOUT_TOOL_H

#   include	<appPageLayoutTool.h>

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatToolRefreshPageLayoutTool(
				PageLayoutTool *		plt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const DocumentProperties *	dp );

extern void tedFormatCleanPageLayoutTool(	PageLayoutTool *	plt );

#   endif	/*  TED_PAGE_LAYOUT_TOOL_H */
