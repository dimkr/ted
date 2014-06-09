/************************************************************************/
/*									*/
/*  A Font/Text attribute Selection Page on an inspector.		*/
/*									*/
/************************************************************************/

#   ifndef APP_FONT_TOOL_H
#   define APP_FONT_TOOL_H

#   include	<docExpandedTextAttribute.h>
#   include	<appFrame.h>
#   include	<appEncodingMenu.h>
#   include	<appColorChooser.h>

/************************************************************************/
/*									*/
/*  Represents a font chooser.						*/
/*									*/
/************************************************************************/

typedef struct AppFontToolResources
    {
    char *			aftrFamily;
    char *			aftrFace;
    char *			aftrSize;
    char *			aftrRevert;
    char *			aftrSet;
    char *			aftrTextColor;

    char *			aftrTextUnderlined;
    char *			aftrTextStrikethrough;

    char *			aftrSuperscript;
    char *			aftrSubscript;

    char *			aftrSmallcaps;
    char *			aftrCapitals;

    ColorChooserResources	aftrTextColorChooserResources;

    char *			aftrEncodings[CHARSETidxCOUNT];
    char *			aftrFaces[FONTface_COUNT];
    } AppFontToolResources;

typedef void (*FontChooserSetFont)(
				void *				target,
				const ExpandedTextAttribute *	etaSet,
				const PropertyMask *		taSetMask );

typedef struct AppFontChooser
    {
    EditApplication *		afcApplication;
    AppInspector *		afcInspector;
    int				afcSubjectPage;
    int				afcCurrentDocumentId;

    APP_WIDGET			afcFamilyList;
    APP_WIDGET			afcFaceList;
    APP_WIDGET			afcSizeList;
    APP_WIDGET			afcSizeText;

    APP_WIDGET			afcColorRow;
    APP_WIDGET			afcTextColorLabel;
    ColorChooser		afcTextColorChooser;

    APP_WIDGET			afcUnderlinedToggle;
    APP_WIDGET			afcStrikethroughToggle;

    APP_WIDGET			afcSuperscriptToggle;
    APP_WIDGET			afcSubscriptToggle;

    APP_WIDGET			afcSmallcapsToggle;
    APP_WIDGET			afcCapitalsToggle;

    APP_WIDGET			afcButtonRow;
    APP_WIDGET			afcRevertButton;
    APP_WIDGET			afcSetButton;

    APP_WIDGET			afcSampleDrawing;
    APP_WIDGET			afcX11Name;
    APP_WIDGET			afcPsName;

    AppEncodingMenu		afcEncodingMenu;

    int				afcFontFamilyChosen;
    int				afcFaceChosen;
    PropertyMask		afcChosenMask;
    ExpandedTextAttribute	afcTextAttributeChosen;

    PropertyMask		afcSetMask;
    ExpandedTextAttribute	afcTextAttributeSet;

    void *			afcTarget;

    FontChooserSetFont		afcSetFont;
    
    DocumentFontList		afcDocumentFontList;

    AppDrawingData		afcDrawingData;
    const PostScriptFontList *	afcPostScriptFontList;
    APP_FONT *			afcFont;
    char			afcChoiceText[120+1];
    char			afcX11ChoiceText[120+1];
    int				afcFaceMapFwd[FONTface_COUNT];
    int				afcFaceMapBck[FONTface_COUNT];
    AppFontToolResources	afcResources;
    } AppFontChooser;

/************************************************************************/
/*									*/
/*  Routine declarations						*/
/*									*/
/************************************************************************/

extern void appFontToolFillPage(AppFontChooser *		afc,
				const AppFontToolResources *	aftr,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void appFontToolGetResourceTable(EditApplication *		ea,
					AppFontToolResources *		aftr );

extern void appFontChooserCleanPage( AppFontChooser *	afc );

extern void appFontToolFillChoosers(
				AppFontChooser *		afc,
				const AppFontToolResources *	aftr );

extern void appFontToolFinishPage(
				AppFontChooser *		afc,
				const AppFontToolResources *	aftr );

extern int appFontExpandCurrentFont(
				AppFontChooser *		afc,
				const PropertyMask *		newMask,
				const TextAttribute *		taNew,
				unsigned int			documentId,
				const DocumentFontList *	dfl,
				const RGB8Color *		colors,
				int				colorCount );

#   endif /*	APP_FONT_TOOL_H  */
