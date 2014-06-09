/************************************************************************/
/*									*/
/*  Border tool:							*/
/*									*/
/************************************************************************/

#   ifndef	TED_BORDER_TOOL_H
#   define	TED_BORDER_TOOL_H

#   include	<appColorChooser.h>

struct BorderTool;

typedef struct BorderTool
    {
    AppInspector *		btInspector;
    int				btSubjectPage;
    int				btWhich;

    APP_WIDGET			btFrame;
    APP_WIDGET			btPaned;

    APP_WIDGET			btOnOffToggle;
    APP_WIDGET			btWideText;

    APP_WIDGET			btStyleRow;
    APP_WIDGET			btStyleLabel;
    AppDrawnPulldown		btStylePulldown;
    AppDrawingData		btInplaceDrawingData;
    AppDrawingData		btPulldownDrawingData;
    int				btPulldownDrawingDataSet;

    APP_WIDGET			btColorRow;
    APP_WIDGET			btColorLabel;
    ColorChooser		btColorChooser;

    ExpandedBorderProperties	btPropertiesChosen;
    PropertyMask		btPropertiesChanged;
    } BorderTool;

typedef struct BorderToolResources
    {
    char *			btrWidth;
    char *			btrColor;
    char *			btrStyle;
    ColorChooserResources	btrColorChooserResources;
    } BorderToolResources;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedInitBorderTool(		BorderTool *		bt );
extern void tedCleanBorderTool(		BorderTool *		bt );

extern void tedBorderToolSetProperties(	BorderTool *			bt,
					const DocumentProperties *	dp,
					const BorderProperties *	bp );

extern int tedBorderToolGetProperties(	BorderProperties *	bp,
					PropertyMask *		pSetMask,
					const BorderTool *	bt,
					DocumentProperties *	dp );

extern void tedMakeBorderTool(	BorderTool *			bt,
				AppInspector *			ai,
				APP_WIDGET			page,
				const char *			title,
				const BorderToolResources *	btr,
				int				subjectPage,
				int				which );

extern void tedFinishBorderTool(	BorderTool *		bt );

extern void tedBorderSetExplicitColorChoice(
					BorderTool *		bt,
					const RGB8Color *	rgb8Set );

#   endif	/*  TED_BORDER_TOOL_H */
