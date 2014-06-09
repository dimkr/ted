/************************************************************************/
/*									*/
/*  RGB chooser page on an inspector.					*/
/*									*/
/************************************************************************/

#   ifndef	APP_RGB_CHOOSER_PAGE_H
#   define	APP_RGB_CHOOSER_PAGE_H

#   include	<appFrame.h>
#   include	<bmcolor.h>
#   include	<appRgbCube.h>

/************************************************************************/
/*									*/
/*  Resources for the RGB chooser page.					*/
/*									*/
/************************************************************************/

typedef struct RgbChooserPageResources
    {
    char *		rcprCancelText;

    char *		rcprRedText;
    char *		rcprGreenText;
    char *		rcprBlueText;
    } RgbChooserPageResources;

/************************************************************************/
/*									*/
/*  A RGB chooser tool. Select RGB colors by typing or clicking.	*/
/*									*/
/************************************************************************/

struct RgbChooserPage;

typedef void (*RgbChooserCallback)(	struct RgbChooserPage *	rcp,
					const RGB8Color *	rgb8New,
					void *			through );

typedef struct RgbChooserPage
    {
    EditApplication *			rcpApplication;
    AppInspector *			rcpInspector;
    const RgbChooserPageResources *	rcpPageResources;

    RGB8Color				rcpRGBSet;
    RGB8Color				rcpRGBChosen;

    int					rcpSubject;
    int					rcpProperty;

    APP_WIDGET				rcpDrawing;
    AppDrawingData			rcpDrawingData;
    int					rcpDrawingDataAllocated;

    APP_WIDGET				rcpRedToggle;
    APP_WIDGET				rcpGreenToggle;
    APP_WIDGET				rcpBlueToggle;

    APP_WIDGET				rcpRedText;
    APP_WIDGET				rcpGreenText;
    APP_WIDGET				rcpBlueText;

    APP_WIDGET				rcpCancelButton;

    RgbCube				rcpRgbCube;
    } RgbChooserPage;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void appRgbChooserPageFinishPage(
				RgbChooserPage *		rcp,
				const RgbChooserPageResources *	rcpr );

extern void appRgbChooserRefresh(
				RgbChooserPage *		rcp,
				int *				pEnabled,
				InspectorSubject *		is,
				const RGB8Color *		rgbSet );

extern void appRgbChooserPageFillPage(
				RgbChooserPage *		rcp,
				const RgbChooserPageResources *	plpr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void appRgbChooserPageGetResourceTable(
				EditApplication *		ea,
				RgbChooserPageResources *	rcpr,
				InspectorSubjectResources *	isr );

extern void appRgbChooserPageCleanPage(
				RgbChooserPage *		rcp );

extern void appRgbChooserSetContext(	RgbChooserPage *	rcp,
					int			subjectPage,
					int			property );

#   endif	/*  APP_RGB_CHOOSER_PAGE_H */
