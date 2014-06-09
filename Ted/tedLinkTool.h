/************************************************************************/
/*									*/
/*  Ted: Link tool.							*/
/*									*/
/************************************************************************/

#   ifndef	TED_LINK_TOOL_H
#   define	TED_LINK_TOOL_H

#   include	<appFrame.h>

/************************************************************************/
/*									*/
/*  User data for a link dialog.					*/
/*									*/
/************************************************************************/

typedef struct LinkToolResources
    {
    char *		lprFileText;
    char *		lprMarkText;

    char *		lprLinkAsText;
    char *		lprLinkAsItemTexts[LINKkind_COUNT];

    char *		lprFollowLinkText;
    char *		lprRemoveLinkText;
    char *		lprCancelText;
    } LinkToolResources;

typedef struct LinkTool
    {
    EditApplication *		ltApplication;
    AppInspector *		ltInspector;
    const LinkToolResources *	ltPageResources;

    APP_WIDGET			ltFileTextWidget;
    APP_WIDGET			ltMarkTextWidget;
    APP_WIDGET			ltMarkListWidget;

    APP_WIDGET			ltButtonRow;
    APP_WIDGET			ltSetLinkButton;
    APP_WIDGET			ltFollowLinkButton;
    APP_WIDGET			ltRemoveLinkButton;
    APP_WIDGET			ltCancelButton;

    APP_WIDGET			ltLinkAsFrame;
    APP_WIDGET			ltLinkAsPaned;
    AppOptionmenu		ltLinkAsOptionmenu;
    APP_WIDGET			ltLinkAsItems[LINKkind_COUNT];

    unsigned int		ltCurrentDocumentId;
    int				ltDocumentReadonly;
    int				ltLinkKind;
    char *			ltFileSet;
    char *			ltMarkSet;
    char *			ltFileChosen;
    char *			ltMarkChosen;
    } LinkTool;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatFillLinkPage(
				LinkTool *			lt,
				const LinkToolResources *	lpr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void tedFormatInitLinkTool(	LinkTool *		lt );
extern void tedFormatCleanLinkTool(	LinkTool *		lt );

extern void tedLinkToolGetResourceTable(
				EditApplication *		ea,
				LinkToolResources *		lpr,
				InspectorSubjectResources *	isr );

extern void tedLinkToolFillChoosers(	LinkTool *			lt,
					const LinkToolResources *	ltr );
extern void tedFinishLinkTool(		LinkTool *			lt,
					const LinkToolResources *	ltr );

extern void tedRefreshLinkTool(	LinkTool *			lt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				BufferDocument *		bd );

#   endif	/*  TED_LINK_TOOL_H  */
