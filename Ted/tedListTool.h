/************************************************************************/
/*									*/
/*  List tool:								*/
/*									*/
/*  The page on the format tool to edit list properties.		*/
/*									*/
/************************************************************************/

#   ifndef	TED_LIST_TOOL_H
#   define	TED_LIST_TOOL_H

# define LISTTOOLcountSTYLES	8

/************************************************************************/
/*									*/
/*  Resources for the 'Bullets and Numbers' page.			*/
/*									*/
/************************************************************************/

typedef struct ListsPageResources
    {
    char *		lprListLevel;
    char *		lprNumberStyle;
    char *		lprNumberFormat;

    char *		lprStartAt;
    char *		lprNoRestart;
    char *		lprJustify;
    char *		lprFollowedBy;
    char *		lprStyleOptionTexts[LISTTOOLcountSTYLES];
    char *		lprJustifyOptionTexts[DOClla_COUNT];
    char *		lprFollowOptionTexts[DOCllf_COUNT];

    char *		lprLevelFirstIndent;
    char *		lprLevelLeftIndent;

    char *		lprSelection;
    char *		lprSetList;
    char *		lprNewList;
    char *		lprSetLevel;
    char *		lprRemoveFromList;

    char *		lprDeleteLevelNumber;
    char *		lprInsertLevelNumber;
    char *		lprEditLevelText;
    } ListsPageResources;

/************************************************************************/
/*									*/
/*  A lists tool, i.e. the 'Bullets and Numbers' page of the format	*/
/*  tool.								*/
/*									*/
/************************************************************************/

typedef struct ListTool
    {
    EditApplication *		ltApplication;
    AppInspector *		ltInspector;
    const ListsPageResources *	ltPageResources;

    int				ltUnitType;

    DocumentList		ltListPropertiesSet;
    DocumentList		ltListPropertiesChosen;
    ListOverride		ltListOverrideSet;
    ListOverride		ltListOverrideChosen;
    int				ltCurrentLevel;
    int				ltCurrentParagraphNumber;
    int				ltTabIntervalTwips;

    int				ltHerePath[DLmaxLEVELS];
    int				ltPrevPath[DLmaxLEVELS];
    int				ltCurrPath[DLmaxLEVELS];
    int				ltFormatPath[DLmaxLEVELS];
    int				ltStartPath[DLmaxLEVELS];

    int				ltHereLevel;
    int				ltPrevLevel;
    int				ltFormatIndex;

    /**/
    APP_WIDGET			ltListLevelList;

    APP_WIDGET			ltSelectionFrame;
    APP_WIDGET			ltSelectionPaned;

    APP_WIDGET			ltListLevelFrame;
    APP_WIDGET			ltListLevelPaned;

    AppOptionmenu		ltNumberStyleOptionmenu;
    APP_WIDGET			ltNumberStyleItems[LISTTOOLcountSTYLES];

    APP_WIDGET			ltFormatLabel;
    APP_WIDGET			ltNumberFormatHBox;
    APP_WIDGET			ltNumberFormatHead;
    APP_WIDGET			ltNumberFormatCurrentLabel;
    APP_WIDGET			ltNumberFormatCurrentText;
    APP_WIDGET			ltNumberFormatLabelTail;
    APP_WIDGET			ltNumberFormatTextTail;
    APP_WIDGET			ltNumberFormatMenu;
    int				ltFormatEditable;

    APP_WIDGET			ltFirstIndentText;
    APP_WIDGET			ltLeftIndentText;
    APP_WIDGET			ltStartAtText;
    APP_WIDGET			ltNoRestartToggle;

    AppOptionmenu		ltJustifyOptionmenu;
    APP_WIDGET			ltJustifyItems[DOClla_COUNT];

    AppOptionmenu		ltFollowOptionmenu;
    APP_WIDGET			ltFollowItems[DOCllf_COUNT];

    APP_WIDGET			ltSetListButton;
    APP_WIDGET			ltNewListButton;
    APP_WIDGET			ltSetLevelButton;
    APP_WIDGET			ltRemoveFromListButton;
    } ListTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatToolGetListsResourceTable(
					EditApplication *		ea,
					ListsPageResources *		spr,
					InspectorSubjectResources *	isr );

extern void tedFormatToolRefreshListTool(
				ListTool *			lt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				BufferDocument *		bd );

extern void tedFormatFillListsPage( ListTool *			lt,
				const ListsPageResources *	lpr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources *	isr );

extern void tedFormatFillListChoosers(	ListTool *			lt );

extern void tedFormatFinishListPage(	ListTool *			lt );

extern void tedFormatCleanListsTool(	ListTool *			lt );

#   endif	/*  TED_LIST_TOOL_H */
