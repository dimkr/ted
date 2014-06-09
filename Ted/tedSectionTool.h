/************************************************************************/
/*									*/
/*  Format tool:							*/
/*									*/
/************************************************************************/

#   ifndef	TED_SECTION_TOOL_H
#   define	TED_SECTION_TOOL_H

/************************************************************************/
/*									*/
/*  Resources for the 'Section; page.					*/
/*									*/
/************************************************************************/

typedef struct SectionPageResources
    {
    char *		sprUnitType;

    char *		sprNumberingRestarts;

    char *		sprPageNumberStyle;
    char *		sprNumberStyleMenuTexts[DOCpgn_COUNT];

    char *		sprBreakKind;
    char *		sprBreakKindMenuTexts[DOCsbk_COUNT];

    char *		sprColumnCount;
    char *		sprColumnSpacing;
    char *		sprColumnWidth;
    } SectionPageResources;

/************************************************************************/
/*									*/
/*  A section tool, i.e. the 'Section' page of the format tool.		*/
/*									*/
/************************************************************************/

typedef struct SectionTool
    {
    EditApplication *			stApplication;
    AppInspector *			stInspector;
    const SectionPageResources *	stPageResources;

    int					stUnitType;

    SectionProperties			stPropertiesSet;
    SectionProperties			stPropertiesChosen;

    int					stSectionNumber;

    APP_WIDGET				stNumberText;

    APP_WIDGET				stPageRestartToggle;
    APP_WIDGET				stPageRestartText;

    AppOptionmenu			stNumberStyleOptionmenu;
    APP_WIDGET				stNumberStyleItems[DOCpgn_COUNT];

    AppOptionmenu			stBreakKindOptionmenu;
    APP_WIDGET				stBreakKindItems[DOCsbk_COUNT];

    APP_WIDGET				stColumnsText;
    APP_WIDGET				stColumnSpacingText;
    APP_WIDGET				stColumnWidthText;
    } SectionTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatToolGetSectResourceTable(	EditApplication *	ea,
					SectionPageResources *		spr,
					InspectorSubjectResources *	isr );

extern void tedFormatToolRefreshSectionTool(
				SectionTool *			st,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	bs );

extern void tedFormatCleanSectionTool(		SectionTool *		st );

#   endif	/*  TED_SECTION_TOOL_H */
