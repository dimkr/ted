/************************************************************************/
/*									*/
/*  Shading tool:							*/
/*									*/
/************************************************************************/

#   ifndef	TED_SHADING_TOOL_H
#   define	TED_SHADING_TOOL_H

#   include <appGuiBase.h>
#   include <appColorChooser.h>
#   include "docBuf.h"

struct ShadingTool;

typedef void (*TedShadingToolCallback)(
				struct ShadingTool *		st,
				void *				through,
				const PropertyMask *		bpSetMask,
				const ExpandedItemShading *	eis );

typedef struct ShadingTool
    {
    APP_WIDGET			stFrame;
    APP_WIDGET			stPaned;

    APP_WIDGET			stPatternLabel;
    APP_WIDGET			stLevelLabel;
    APP_WIDGET			stBackLabel;
    APP_WIDGET			stForeLabel;

    int				stShowPattern;
    AppDrawnPulldown		stPatternPulldown;
    AppDrawingData		stPatternInplaceDrawingData;
    AppDrawingData		stPatternPulldownDrawingData;
    APP_BITMAP_IMAGE		stPatternPixmaps[DOCsp_COUNT];
    int				stPulldownDrawingDataSet;

    APP_WIDGET			stLevelText;
    ColorChooser		stBackColorChooser;
    ColorChooser		stForeColorChooser;

    ExpandedItemShading		stShadingChosen;
    PropertyMask		stShadingChanged;

    TedShadingToolCallback	stCallback;
    void *			stTarget;
    AppInspector *		stInspector;

    int				stSubjectPage;
    int				stBackWhich;
    int				stForeWhich;
    } ShadingTool;

typedef struct ShadingToolResources
    {
    char *			strPattern;
    char *			strLevel;
    char *			strBackColor;
    char *			strForeColor;

    ColorChooserResources	strBackColorChooserResources;
    ColorChooserResources	strForeColorChooserResources;
    } ShadingToolResources;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedSetShadingTool(		ShadingTool *			st,
					const DocumentProperties *	dp,
					const ItemShading *		is );

extern void tedFormatMakeShadingTool(
				ShadingTool *			st,
				AppInspector *			ai,
				APP_WIDGET			pageWidget,
				const char *			title,
				const ShadingToolResources *	str,
				int				subjectPage,
				int				foreWhich,
				int				backWhich,
				TedShadingToolCallback		callback,
				void *				through );

extern int tedShadingToolGetShading(
				ExpandedItemShading *		eisSet,
				PropertyMask *			pSetMask,
				const ShadingTool *		st );

extern void tedShadeSetExplicitColorChoice(
				ShadingTool *			st,
				int				which,
				const RGB8Color *		rgb8Set );

extern void tedInitShadingTool(		ShadingTool *		st );
extern void tedCleanShadingTool(	ShadingTool *		st );

extern void tedFinishShadingTool(	ShadingTool *		st );

#   endif	/*  TED_SHADING_TOOL_H */
