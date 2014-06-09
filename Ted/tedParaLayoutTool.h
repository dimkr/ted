/************************************************************************/
/*									*/
/*  Paragraph layout tool:						*/
/*									*/
/************************************************************************/

#   ifndef	TED_PARA_LAYOUT_TOOL_H
#   define	TED_PARA_LAYOUT_TOOL_H

#   include	"tedBorderTool.h"
#   include	"tedShadingTool.h"
#   include	"tedHeightTool.h"

/************************************************************************/
/*									*/
/*  Resources for the 'Paragraph Formatting' page.			*/
/*									*/
/************************************************************************/

typedef struct ParagraphLayoutPageResources
    {
    char *			plprParaListLevel;
    char *			plprParaFirstIndent;
    char *			plprParaLeftIndent;
    char *			plprParaRightIndent;

    char *			pprParaSpaceAbove;
    char *			pprParaSpaceBelow;

    char *			pprOnNewPage;
    char *			pprOnOnePage;

    char *			pprWidctrl;
    char *			pprKeepWithNext;

    char *			pprParaLineDistFree;
    char *			pprParaLineDistAtLeast;
    char *			pprParaLineDistExactly;

    char *			pprParaAlignment;
    char *			pprParaAlignMenuTexts[DOCia_COUNT];
    } ParagraphLayoutPageResources;

/************************************************************************/
/*									*/
/*  A paragraph tool, i.e. the 'Paragraph' page of the format tool.	*/
/*									*/
/************************************************************************/

typedef struct ParagraphLayoutTool
    {
    EditApplication *				ptApplication;
    AppInspector *				ptInspector;
    const ParagraphLayoutPageResources *	ptPageResources;

    ParagraphProperties				ptPropertiesSet;
    ParagraphProperties				ptPropertiesChosen;

    APP_WIDGET					ptListLevelText;
    APP_WIDGET					ptFirstIndentText;
    APP_WIDGET					ptLeftIndentText;
    APP_WIDGET					ptRightIndentText;

    APP_WIDGET					ptOnNewPageToggle;
    APP_WIDGET					ptKeepOnPageToggle;

    APP_WIDGET					ptWidowControlToggle;
    APP_WIDGET					ptKeepWithNextToggle;

    APP_WIDGET					ptSpaceAboveToggle;
    APP_WIDGET					ptSpaceAboveText;

    APP_WIDGET					ptSpaceBelowToggle;
    APP_WIDGET					ptSpaceBelowText;

    HeightChooser				ptLineDistChooser;

    AppOptionmenu				ptAlignOptionmenu;
    APP_WIDGET					ptAlignItems[DOCia_COUNT];
    } ParagraphLayoutTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

void tedFormatToolGetParaLayoutResourceTable(
					EditApplication *		ea,
					ParagraphLayoutPageResources *	plpr,
					InspectorSubjectResources *	isr );

extern void tedFormatFillParagraphLayoutChoosers(
				ParagraphLayoutTool *			plt,
				const ParagraphLayoutPageResources *	plpr );

extern void tedFormatFillParagraphLayoutPage(
				ParagraphLayoutTool *			plt,
				const ParagraphLayoutPageResources *	ppr,
				InspectorSubject *			is,
				APP_WIDGET				pgW,
				const InspectorSubjectResources *	isr );

extern void tedFormatToolRefreshParaLayoutTool(
				ParagraphLayoutTool *		plt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	bs );

extern void tedFormatCleanParaLayoutTool(
				ParagraphLayoutTool *		plt );

#   endif	/*  TED_PARA_LAYOUT_TOOL_H */
