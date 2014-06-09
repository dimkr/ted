/************************************************************************/
/*									*/
/*  Utility functionality for 'Inspector' like tools.			*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>

#   include	<appFrame.h>

#   include	<appRgbChooserPage.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Pull a particular subject of the inspector to front.		*/
/*									*/
/************************************************************************/

void appInspectorSelectSubject(	AppInspector *		ai,
				int			subject )
    {
    const int	andMenu= 1;

    appInspectorChoosePage( ai, andMenu, subject );

    ai->aiCurrentSubject= subject;
    }

/************************************************************************/
/*									*/
/*  An inspector tool must be destroyed.				*/
/*									*/
/************************************************************************/

static void appDestroyInspector(	AppInspector *	ai )
    {
    if  ( ai->aiDestroy )
	{ (*ai->aiDestroy)( ai->aiTarget );	}

    appDestroyShellWidget( ai->aiTopWidget );

    free( ai );

    return;
    }

static APP_CLOSE_CALLBACK_H( appInspectorCloseCall, w, voidai )
    {
    AppInspector *	ai= (AppInspector *)voidai;

    appDestroyInspector( ai );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the form with the two buttons.					*/
/*									*/
/************************************************************************/

void appInspectorMakeButtonRow(	APP_WIDGET *		pRow,
				APP_WIDGET		parent,
				APP_WIDGET *		pLeftButton,
				APP_WIDGET *		pRightButton,
				const char *		leftLabel,
				const char *		rightLabel,
				APP_BUTTON_CALLBACK_T	leftCallback,
				APP_BUTTON_CALLBACK_T	rightCallback,
				void *			through )
    {
    const int	heightResizable= 0;
    const int	showAsDefault= 0;

    APP_WIDGET	row= appMakeRowInColumn( parent, 2, heightResizable );

    APP_WIDGET	leftButton;
    APP_WIDGET	rightButton;

    appMakeButtonInRow( &leftButton, row, leftLabel, leftCallback, through,
							    0, showAsDefault );
    appMakeButtonInRow( &rightButton, row, rightLabel,rightCallback, through,
							    1, showAsDefault );

    *pRow= row;
    *pLeftButton= leftButton;
    *pRightButton= rightButton;

    return;
    }

void appInspectorMake3ButtonRow( APP_WIDGET *		pRow,
				APP_WIDGET		parent,
				APP_WIDGET *		pLeftButton,
				APP_WIDGET *		pMiddleButton,
				APP_WIDGET *		pRightButton,
				const char *		leftLabel,
				const char *		middleLabel,
				const char *		rightLabel,
				APP_BUTTON_CALLBACK_T	leftCallback,
				APP_BUTTON_CALLBACK_T	middleCallback,
				APP_BUTTON_CALLBACK_T	rightCallback,
				void *			through )
    {
    const int	heightResizable= 0;
    const int	showAsDefault= 0;

    APP_WIDGET	row= appMakeRowInColumn( parent, 3, heightResizable );

    APP_WIDGET	leftButton;
    APP_WIDGET	middleButton;
    APP_WIDGET	rightButton;

    appMakeButtonInRow( &leftButton, row, leftLabel, leftCallback, through,
							    0, showAsDefault );
    appMakeButtonInRow( &middleButton, row, middleLabel,middleCallback, through,
							    1, showAsDefault );
    appMakeButtonInRow( &rightButton, row, rightLabel,rightCallback, through,
							    2, showAsDefault );

    *pRow= row;
    *pLeftButton= leftButton;
    *pMiddleButton= middleButton;
    *pRightButton= rightButton;

    return;
    }

void appInspectorMakeToggleRow(		APP_WIDGET *		pRow,
					APP_WIDGET		parent,
					APP_WIDGET *		pLeftToggle,
					APP_WIDGET *		pRightToggle,
					const char *		leftText,
					const char *		rightText,
					APP_TOGGLE_CALLBACK_T	leftCallback,
					APP_TOGGLE_CALLBACK_T	rightCallback,
					void *			through )
    {
    const int	heightResizable= 0;
    APP_WIDGET	row= appMakeRowInColumn( parent, 2, heightResizable );

    APP_WIDGET	leftToggle;
    APP_WIDGET	rightToggle;

    const int	leftColumn= 0;
    const int	rightColumn= 1;

    leftToggle= appMakeToggleInRow( row, leftText, leftCallback,
						    through, leftColumn );
    rightToggle= appMakeToggleInRow( row, rightText, rightCallback,
						    through, rightColumn );

    *pRow= row;
    *pLeftToggle= leftToggle;
    *pRightToggle= rightToggle;

    return;
    }

void appInspectorMakeMenuRow(	APP_WIDGET *		pRow,
				AppOptionmenu *		aom,
				APP_WIDGET *		pLabel,
				APP_WIDGET		parent,
				const char *		labelText )
    {
    APP_WIDGET	row;
    APP_WIDGET	label;

    const int	labelColumn= 0;
    const int	labelColspan= 1;

    const int	menuColumn= labelColumn+ labelColspan;
    const int	menuColspan= 1;
    const int	heightResizable= 0;

    row= appMakeRowInColumn( parent, 2, heightResizable );

    /**************/
    appMakeLabelInRow( &label, row, labelColumn, labelColspan, labelText );

    /**************/
    appMakeOptionmenuInRow( aom, row, menuColumn, menuColspan );

    /**************/

    *pRow= row;
    *pLabel= label;

    return;
    }

/************************************************************************/
/*									*/
/*  Make the pages,							*/
/*  Fill the menu for the pages.					*/
/*									*/
/************************************************************************/

static void appInspectorFillSubjects(	InspectorSubjectResources *	isr,
					AppInspector *			ai )
    {
    int				subject;
    InspectorSubject *		is;

    is= ai->aiSubjects;
    for ( subject= 0; subject < ai->aiSubjectCount; is++, isr++, subject++ )
	{
	appMakeVerticalInspectorPage( &(is->isPage), &(is->isMenuitem),
						    ai, isr->isrSubjectName );
	is->isEnabled= 1;
	}
    }

/************************************************************************/
/*									*/
/*  Initialize an inspector subject.					*/
/*									*/
/************************************************************************/

static void appInitInspectorSubject(	InspectorSubject *	is )
    {
    is->isPage= (APP_WIDGET)0;
    is->isMenuitem= (APP_WIDGET)0;
    is->isPrivate= (void *)0;
    is->isEnabled= 1;

    is->isPrevButton= (APP_WIDGET)0;
    is->isNextButton= (APP_WIDGET)0;

    is->isSelectButton= (APP_WIDGET)0;
    is->isDeleteButton= (APP_WIDGET)0;

    is->isInsertButton= (APP_WIDGET)0;
    is->isAppendButton= (APP_WIDGET)0;

    is->isRevertButton= (APP_WIDGET)0;
    is->isApplyButton= (APP_WIDGET)0;

    is->isGotColor= (InspectorSubjectGotColor)0;
    }

/************************************************************************/
/*									*/
/*  Turn an inspector or one of its subjects on or off.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appLowerInspector, w, voidai )
    {
    AppInspector *	ai= (AppInspector *)voidai;
    APP_WIDGET		shell= ai->aiTopWidget;

    appGuiLowerShellWidget( shell );

    return;
    }

static APP_BUTTON_CALLBACK_H( appCloseInspector, w, voidai )
    {
    AppInspector *	ai= (AppInspector *)voidai;

    appDestroyInspector( ai );

    return;
    }

void appEnableInspector(	AppInspector *	ai,
				int		enabled )
    {
    appGuiEnableWidget( ai->aiPaned, enabled != 0 );

    return;
    }

void appEnableInspectorSubject(		AppInspector *		ai,
					int			subject ,
					int			enabled )
    {
    appInspectorEnablePage( ai, subject, enabled );

    ai->aiSubjects[subject].isEnabled= enabled != 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Fill an inspector menu.						*/
/*									*/
/************************************************************************/

void appFillInspectorMenu(	int			count,
				int			current,
				APP_WIDGET *		items,
				char * const *		texts,
				AppOptionmenu *		aom,
				APP_OITEM_CALLBACK_T	callBack,
				void *			target )
    {
    int			i;

    appEmptyOptionmenu( aom );

    for ( i= 0; i < count; i++ )
	{
	items[i]= appAddItemToOptionmenu( aom, texts[i], callBack, target );
	}

    appSetOptionmenu( aom, current );

    appOptionmenuRefreshWidth( aom );

    return;
    }

static AppConfigurableResource APP_InspectorResourceTable[]=
{
    APP_RESOURCE( "inspectorLowerInspector",
		offsetof(AppInspectorResources,airLowerText),
		"Lower" ),

    APP_RESOURCE( "inspectorCloseInspector",
		offsetof(AppInspectorResources,airCloseText),
		"Close" ),
};

AppInspector * appMakeInspector(    EditApplication *		ea,
				    APP_WIDGET			option,
				    const char *		pixmapName,
				    const char *		widgetName,
				    InspectorSubjectResources * isr,
				    int				subjectCount,
				    AppToolDestroy		destroy,
				    void *			through )
    {
    AppInspector *		ai;
    int				subject;
    InspectorSubject *		is;

    APP_BITMAP_IMAGE		iconPixmap;
    APP_BITMAP_MASK		iconMask;

    const int			userResizable= 0;

    APP_WIDGET			row;

    static AppInspectorResources	air;
    static int				gotResources;

    if  ( ! gotResources )
	{
	appGuiGetResourceValues( &gotResources, ea, (void *)&air,
					APP_InspectorResourceTable,
					sizeof(APP_InspectorResourceTable)/
					sizeof(AppConfigurableResource) );

	gotResources= 1;
	}

    if  ( appGetImagePixmap( ea, pixmapName, &iconPixmap, &iconMask )  )
	{ SDEB(pixmapName); return (AppInspector *)0; }

    ai= (AppInspector *)malloc( sizeof(AppInspector)+ 
				subjectCount* sizeof(InspectorSubject) );
    if  ( ! ai )
	{ LXDEB(subjectCount,ai); return ai;	}

    ai->aiApplication= ea;
    ai->aiCurrentSubject= -1;
    ai->aiSubjectCount= subjectCount;

    ai->aiDestroy= destroy;
    ai->aiTarget= through;
    ai->aiNotifySubject= (InspectorNotifySubject)0;

    ai->aiRgbSubjectNumber= -1;
    ai->aiRgbChooser= (void *)0; /* RgbChooserPage */

    is= ai->aiSubjects;
    for ( subject= 0; subject < ai->aiSubjectCount; is++, subject++ )
	{ appInitInspectorSubject( is );	}

    appMakeVerticalTool( &(ai->aiTopWidget), &(ai->aiPaned), ea,
				iconPixmap, iconMask,
				widgetName, userResizable,
				option, appInspectorCloseCall, (void *)ai );

    appMakeOptionmenuInColumn( &(ai->aiSubjectOptionmenu), ai->aiPaned );

    appGuiInsertSeparatorInColumn( &(ai->aiSeparator), ai->aiPaned );

    appInspectorMakePageParent( ai );

    appInspectorFillSubjects( isr, ai );

    appGuiInsertSeparatorInColumn( &(ai->aiSeparator2), ai->aiPaned );

    appInspectorMakeButtonRow( &row, ai->aiPaned,
		    &(ai->aiLowerButton), &(ai->aiCloseButton),
		    air.airLowerText, air.airCloseText,
		    appLowerInspector, appCloseInspector, (void *)ai );

    return ai;
    }

void appFinishInspector(	AppInspector *		ai )
    {
    appShowShellWidget( ai->aiTopWidget );

    appOptionmenuRefreshWidth( &(ai->aiSubjectOptionmenu) );

    return;
    }

/************************************************************************/
/*									*/
/*  Callback of a possible RGB chooser to an inspector.			*/
/*									*/
/************************************************************************/

void appInspectorGotColor(	AppInspector *			ai,
				int				subjectPage,
				int				property,
				const RGB8Color *		rgb8Set )
    {
    RgbChooserPage *	rcp;
    InspectorSubject *	is;

    if  ( ! ai->aiRgbChooser )
	{ XDEB(ai->aiRgbChooser); return;	}
    rcp= (RgbChooserPage *)ai->aiRgbChooser;

    if  ( ai->aiRgbSubjectNumber < 0			||
	  ai->aiRgbSubjectNumber >= ai->aiSubjectCount	)
	{ LLDEB(ai->aiRgbSubjectNumber,ai->aiSubjectCount); return;	}

    is= ai->aiSubjects+ subjectPage;

    if  ( subjectPage < 0 || subjectPage >= ai->aiSubjectCount )
	{ LLDEB(subjectPage,ai->aiSubjectCount); return;	}

    /**/
    if  ( rgb8Set )
	{
	if  ( ! is->isGotColor )
	    { XDEB(is->isGotColor);					}
	else{ (*is->isGotColor)( is->isPrivate, property, rgb8Set );	}
	}

    /**/
    appEnableInspectorSubject( ai, subjectPage, 1 );
    appInspectorSelectSubject( ai, subjectPage );

    /**/
    appEnableInspectorSubject( ai, ai->aiRgbSubjectNumber, 0 );
    appRgbChooserSetContext( rcp, -1, -1 );

    return;
    }

/************************************************************************/
/*									*/
/*  Switch to the RGB color chooser page.				*/
/*									*/
/************************************************************************/

void appInspectorSetRgbPage(		AppInspector *		ai,
					void *			vrcp,
					int			subjectNumber )
    {
    ai->aiRgbChooser= vrcp;
    ai->aiRgbSubjectNumber= subjectNumber;

    return;
    }

void appInspectorShowRgbPage(		AppInspector *		ai,
					int			fromSubject,
					int			fromProperty,
					const RGB8Color *	rgb8 )
    {
    RgbChooserPage *	rcp;
    InspectorSubject *	is;

    int			enabled= 1;

    if  ( ! ai->aiRgbChooser )
	{ XDEB(ai->aiRgbChooser); return;	}
    rcp= (RgbChooserPage *)ai->aiRgbChooser;

    if  ( ai->aiRgbSubjectNumber < 0			||
	  ai->aiRgbSubjectNumber >= ai->aiSubjectCount	)
	{ LLDEB(ai->aiRgbSubjectNumber,ai->aiSubjectCount); return;	}

    is= ai->aiSubjects+ ai->aiRgbSubjectNumber;

    appRgbChooserRefresh( rcp, &enabled, is, rgb8 );

    appRgbChooserSetContext( rcp, fromSubject, fromProperty );

    appEnableInspectorSubject( ai, ai->aiRgbSubjectNumber, 1 );

    appInspectorSelectSubject( ai, ai->aiRgbSubjectNumber );

    return;
    }

