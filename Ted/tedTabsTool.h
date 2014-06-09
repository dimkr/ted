/************************************************************************/
/*									*/
/*  Tabs tool:								*/
/*									*/
/*  The page on the format tool to edit tab stops.			*/
/*									*/
/************************************************************************/

#   ifndef	TED_TABS_TOOL_H
#   define	TED_TABS_TOOL_H

/************************************************************************/
/*									*/
/*  Resources for the 'Tabs' page.					*/
/*									*/
/************************************************************************/

typedef struct TabsPageResources
    {
    char *		tprDefaultTabStopsLabel;
    char *		tprDefaultTabStopsEvery;
    char *		tprDefaultTabStopsRevert;
    char *		tprDefaultTabStopsApply;

    char *		tprPositions;

    char *		tprTabStop;

    char *		tprPosition;

    char *		tprAlignment;
    char *		tprAlignmentOptionTexts[DOCta_COUNT];

    char *		tprLeader;
    char *		tprLeaderOptionTexts[DOCtl_COUNT];

    char *		tprTooCloseMessage;
    } TabsPageResources;

/************************************************************************/
/*									*/
/*  A tabs tool, i.e. the 'Tabs' page of the format tool.		*/
/*									*/
/************************************************************************/

typedef struct TabsTool
    {
    EditApplication *		ttApplication;
    AppInspector *		ttInspector;
    const TabsPageResources *	ttPageResources;

    int				ttUnitType;

    DocumentProperties		ttDocPropertiesSet;
    DocumentProperties		ttDocPropertiesChosen;

    ParagraphProperties		ttParaPropertiesSet;
    ParagraphProperties		ttParaPropertiesChosen;

    TabStop			ttTabStopValue;
    int				ttTabStopNumber;

    /**/
    APP_WIDGET			ttTabDefaultFrame;
    APP_WIDGET			ttTabDefaultPaned;
    APP_WIDGET			ttTabDefaultText;
    APP_WIDGET			ttTabDefaultRevertButton;
    APP_WIDGET			ttTabDefaultApplyButton;

    /**/
    APP_WIDGET			ttTabPositionList;

    APP_WIDGET			ttTabPropertyFrame;
    APP_WIDGET			ttTabPropertyPaned;

    APP_WIDGET			ttTabPositionText;

    AppOptionmenu		ttAlignmentOptionmenu;
    APP_WIDGET			ttAlignmentItems[DOCta_COUNT];

    AppOptionmenu		ttLeaderOptionmenu;
    APP_WIDGET			ttLeaderItems[DOCtl_COUNT];
    } TabsTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatToolGetTabsResourceTable(	EditApplication *	ea,
					TabsPageResources *		spr,
					InspectorSubjectResources *	isr );

extern void tedFormatToolRefreshTabsTool(
				TabsTool *			tt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	bs,
				const DocumentProperties *	dp );

extern void tedFormatFillTabsPage( TabsTool *			tt,
				const TabsPageResources *	tpr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources *	isr );

extern void tedTabsToolFillChoosers(	TabsTool *			tt );

extern void tedFormatFinishTabsPage(	TabsTool *			tt );

extern void tedFormatCleanParaTabsTool(	TabsTool *			tt );

#   endif	/*  TED_TABS_TOOL_H */
