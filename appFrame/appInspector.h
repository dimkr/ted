/************************************************************************/
/*									*/
/*  Application inspector with different subject pages.			*/
/*									*/
/************************************************************************/

#   ifndef	APP_INSPECTOR_H
#   define	APP_INSPECTOR_H

#   include	<stdio.h>

#   include	<bmcolor.h>

#   include	<appGuiBase.h>
#   include	<appGuiResource.h>

/************************************************************************/
/*									*/
/*  Facilities to make property inspectors.				*/
/*									*/
/************************************************************************/

struct AppInspector;

typedef void (*InspectorSubjectGotColor)(
			void *				through,
			int				property,
			const RGB8Color *		rgb8 );

typedef void (*InspectorNotifySubject)(
			struct AppInspector *		ai,
			int				from,
			int				to );

typedef struct InspectorSubject
    {
    APP_WIDGET			isPage;
    APP_WIDGET			isMenuitem;
    void *			isPrivate;
    int				isEnabled;

    APP_WIDGET			isPrevButton;
    APP_WIDGET			isNextButton;

    APP_WIDGET			isSelectButton;
    APP_WIDGET			isDeleteButton;

    APP_WIDGET			isInsertButton;
    APP_WIDGET			isAppendButton;

    APP_WIDGET			isRevertButton;
    APP_WIDGET			isApplyButton;

    InspectorSubjectGotColor	isGotColor;
    } InspectorSubject;

typedef struct AppInspector
    {
    struct EditApplication *	aiApplication;

    APP_WIDGET			aiTopWidget;
    APP_WIDGET			aiPaned;

    AppOptionmenu		aiSubjectOptionmenu;

    APP_WIDGET			aiSeparator;
    APP_WIDGET			aiPageParent;
    APP_WIDGET			aiSeparator2;

    APP_WIDGET			aiLowerButton;
    APP_WIDGET			aiCloseButton;

    InspectorNotifySubject	aiNotifySubject;

    void *			aiRgbChooser;		/*  RgbChooserPage */
    int				aiRgbSubjectNumber;

    AppToolDestroy		aiDestroy;
    void *			aiTarget;

    int				aiSubjectCount;
    int				aiCurrentSubject;

    InspectorSubject		aiSubjects[1];		/*  LAST !!	*/
    } AppInspector;

typedef struct AppInspectorResources
    {
    char *	airLowerText;
    char *	airCloseText;
    } AppInspectorResources;

typedef struct InspectorSubjectResources
    {
    char *		isrSubjectName;
    char *		isrApplyToSubject;
    char *		isrRevert;

    char *		isrNextButtonText;
    char *		isrPrevButtonText;

    char *		isrSelectButtonText;
    char *		isrDeleteButtonText;

    char *		isrInsertButtonText;
    char *		isrAppendButtonText;
    } InspectorSubjectResources;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appInspectorSelectSubject(	AppInspector *		ai,
					int			subject );

extern void appFinishInspector(		AppInspector *		ai );

extern void appEnableInspector(		AppInspector *		ai,
					int			enabled );

extern void appEnableInspectorSubject(	AppInspector *		ai,
					int			subject ,
					int			enabled );

extern void appInspectorGotColor(
				AppInspector *			ai,
				int				subjectPage,
				int				property,
				const RGB8Color *		rgb8 );

extern void appInspectorShowRgbPage(	AppInspector *		ai,
					int			fromSubject,
					int			fromProperty,
					const RGB8Color *	rgb8 );

extern void appInspectorSetRgbPage(	AppInspector *		ai,
					void *			vrcp,
					int			subjectNumber );

#   endif
