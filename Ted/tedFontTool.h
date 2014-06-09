/************************************************************************/
/*									*/
/*  Font tool:								*/
/*									*/
/************************************************************************/

#   ifndef	TED_FONT_TOOL_H
#   define	TED_FONT_TOOL_H

#   include	<appFontTool.h>
#   include	"tedApp.h"

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedRefreshFontTool(	AppFontChooser *		afc,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				EditDocument *			ed );

extern void tedRefreshListFontTool(
				AppFontChooser *		afc,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				BufferDocument *		bd );

extern void tedFontToolGetResourceTable(
				EditApplication *		ea,
				AppFontToolResources *		aftr,
				InspectorSubjectResources *	isr );

extern void tedListFontToolGetResourceTable(
				EditApplication *		ea,
				InspectorSubjectResources *	isr );

#   endif	/*  TED_FONT_TOOL_H */
