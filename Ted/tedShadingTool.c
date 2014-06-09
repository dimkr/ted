/************************************************************************/
/*									*/
/*  A Shading Tool. (Used on the pages of the format tool).		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>

#   include	"tedApp.h"
#   include	"tedFormatTool.h"

#   include	<appDebugon.h>

#   define	SHOW_PATTERN	0

/************************************************************************/
/*									*/
/*  Show the shading level width in a shading tool.			*/
/*									*/
/************************************************************************/

static void tedShadingToolShowLevel(	ShadingTool *		st,
					const ItemShading *	is )
    {
    if  ( ! st->stLevelText )
	{ return;	}

    appEnableText( st->stLevelText, is->isPattern == DOCspSOLID );

    if  ( is->isPattern == DOCspSOLID )
	{
	appIntegerToTextWidget( st->stLevelText, is->isLevel/ 100 );
	}
    else{ appStringToTextWidget( st->stLevelText, "" );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Set the current shading in a shading tool.				*/
/*									*/
/************************************************************************/

void tedSetShadingTool(			ShadingTool *			st,
					const DocumentProperties *	dp,
					const ItemShading *		is )
    {
    ExpandedItemShading *	eis= &(st->stShadingChosen);
    const int			avoidZero= 1;

    tedShadingToolShowLevel( st, is );

    /**/
    docExpandItemShading( eis, is, dp->dpColors, dp->dpColorCount );

    /**/
    appColorChooserSuggestPalette( &(st->stBackColorChooser),
				avoidZero, dp->dpColors, dp->dpColorCount );
    appColorChooserSetColor( &(st->stBackColorChooser),
				    eis->eisBackColorExplicit,
				    &(eis->eisBackColor) );

    /**/
    if  ( st->stForeColorChooser.ccFilled )
	{
	appColorChooserSuggestPalette( &(st->stForeColorChooser),
				avoidZero, dp->dpColors, dp->dpColorCount );

	appColorChooserSetColor( &(st->stForeColorChooser),
				eis->eisForeColorExplicit,
				&(eis->eisForeColor) );
	}

    /**/
    if  ( st->stShowPattern )
	{
	appExposeRectangle( &(st->stPatternInplaceDrawingData), 0, 0, 0, 0 );
	}

    PROPmaskCLEAR( &(st->stShadingChanged) );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the widgets of a shading tool.					*/
/*									*/
/************************************************************************/

# if SHOW_PATTERN

static void tedDrawShadedBox(	AppDrawingData *		add,
				ShadingTool *			st,
				int				pattern,
				const DocumentRectangle *	drI )
    {
    if  ( pattern != DOCspSOLID )
	{
	tedDrawShadedRectangle( add, st->stPatternPixmaps, pattern,
			    drI->drX0, drI->drY0, drI->drX1, drI->drY1 );
	}

    appDrawDrawLine( add, drI->drX0, drI->drY0, drI->drX1, drI->drY0 );
    appDrawDrawLine( add, drI->drX1, drI->drY0, drI->drX1, drI->drY1 );
    appDrawDrawLine( add, drI->drX1, drI->drY1, drI->drX0, drI->drY1 );
    appDrawDrawLine( add, drI->drX0, drI->drY1, drI->drX0, drI->drY0 );

    return;
    }

static APP_EVENT_HANDLER_H( tedShadingRedrawInplace, w, voidst, exposeEvent )
    {
    ShadingTool *	st= (ShadingTool *)voidst;
    AppDrawingData *	add= &(st->stPatternInplaceDrawingData);

    int			wide;
    int			high;

    DocumentRectangle	drI;

    appDrawnPulldownDrawArrow( &wide, &high, w, add );

    drI.drX0= 3; drI.drX1= wide- 3;
    drI.drY0= 3; drI.drY1= high- 3;

    appDrawSetForegroundWhite( add );
    appDrawFillRectangle( add, drI.drX0, drI.drY0,
			    drI.drX1- drI.drX0+ 1, drI.drY1- drI.drY0+ 1 );

    appDrawSetForegroundBlack( add );

    tedDrawShadedBox( add, st, st->stShadingChosen.eisPattern, &drI );

    return;
    }

static APP_EVENT_HANDLER_H( tedShadingRedrawPulldown, w, voidst, exposeEvent )
    {
    ShadingTool *	st= (ShadingTool *)voidst;
    AppDrawingData *	add= &(st->stPatternPulldownDrawingData);

    int			inplaceWide;
    int			inplaceHigh;
    int			pulldownWide;
    int			pulldownHigh;

    const int		ox= 0;
    const int		oy= 0;
    DocumentRectangle	drClip;

    int			i;

    if  ( ! st->stPulldownDrawingDataSet )
	{
	appSetDrawingDataForWidget( st->stPatternPulldown.adpPulldownDrawing,
				    1.0, &(st->stPatternPulldownDrawingData) );

	st->stPulldownDrawingDataSet= 1;
	}

    appDrawGetSizeOfWidget( &pulldownWide, &pulldownHigh, w );
    appDrawGetSizeOfWidget( &inplaceWide, &inplaceHigh,
				    st->stPatternPulldown.adpInplaceDrawing );

    appCollectExposures( &drClip, add, ox, oy, exposeEvent );

    appDrawSetForegroundWhite( add );

    appDrawFillRectangle( add, drClip.drX0, drClip.drY0,
					    drClip.drX1- drClip.drX0+ 1,
					    drClip.drY1- drClip.drY0+ 1 );

    appDrawSetForegroundBlack( add );
    appDrawDrawRectangle( add, 0, 0, pulldownWide- 1, pulldownHigh- 1 );

    for ( i= DOCspSOLID+ 1; i < DOCsp_COUNT; i++ )
	{
	DocumentRectangle	drI;
	DocumentRectangle	drX;

	drI.drX0= 3;
	drI.drX1= pulldownWide- 3;
	drI.drY0= ( i+ 0 )* inplaceHigh+ 3;
	drI.drY1= ( i+ 1 )* inplaceHigh- 3;

	if  ( ! geoIntersectRectangle( &drX, &drI, &drClip ) )
	    { continue;	}

	tedDrawShadedBox( add, st, i, &drI );
	}

    return;
    }
# endif

/************************************************************************/
/*									*/
/*  The user typed something in the 'Level' text box.			*/
/*									*/
/************************************************************************/

static APP_TXTYPING_CALLBACK_H( tedShadingLevelTyped, w, voidst )
    {
    ShadingTool *	st= (ShadingTool *)voidst;

    PROPmaskADD( &(st->stShadingChanged), ISpropLEVEL );

    return;
    }

/************************************************************************/
/*									*/
/*  A shading pattern was chosen.					*/
/*									*/
/************************************************************************/

# if SHOW_PATTERN
static APP_EVENT_HANDLER_H( tedShadingClickedPulldown, w, voidst, mouseEvent )
    {
    ShadingTool *		st= (ShadingTool *)voidst;
    ExpandedItemShading *	eis= &(st->stShadingChosen);

    int			pattern;

    if  ( appGuiDrawnPulldownGetStrip( &pattern,
				&(st->stPatternPulldown), w, mouseEvent ) )
	{ return;	}

    if  ( pattern < 0 || pattern >= DOCsp_COUNT )
	{ LLDEB(pattern,DOCsp_COUNT); return;	}

    if  ( eis->eisPattern != pattern )
	{
	PropertyMask	isSetMask;

	PROPmaskCLEAR( &isSetMask );
	PROPmaskADD( &isSetMask, ISpropPATTERN );
	PROPmaskADD( &(st->stShadingChanged), ISpropPATTERN );

	eis->eisPattern= pattern;

	appExposeRectangle( &(st->stPatternInplaceDrawingData), 0, 0, 0, 0 );

	if  ( st->stCallback )
	    {
	    (*st->stCallback)( st, st->stTarget, &isSetMask, eis );
	    }
	}

    return;
    }
# endif

/************************************************************************/
/*									*/
/*  A background/foreground has been selected in one of the color	*/
/*  choosers of the the shading tool.					*/
/*									*/
/*  1)  Actually.. No color was selected but the user wants to use a	*/
/*	more detailed tool.						*/
/*  2)  Background chosen.						*/
/*  3)  Foreground chosen.						*/
/*									*/
/************************************************************************/

static void tedShadeColorChosen( ColorChooser *			cc,
				int				which,
				void *				voidst,
				int				choice,
				const RGB8Color *		rgb8Set )
    {
    ShadingTool *		st= (ShadingTool *)voidst;
    ExpandedItemShading *	eis= &(st->stShadingChosen);
    int				changed= 0;

    /*  1  */
    if  ( choice == CHOICEccMORE )
	{
	if  ( which == st->stBackWhich )
	    {
	    appInspectorShowRgbPage( st->stInspector,
			    st->stSubjectPage, which, &(eis->eisBackColor) );
	    return;
	    }

	if  ( which == st->stForeWhich )
	    {
	    appInspectorShowRgbPage( st->stInspector,
			    st->stSubjectPage, which, &(eis->eisForeColor) );
	    return;
	    }

	LLDEB(CHOICEccMORE,which);
	return;
	}

    /*  2  */
    if  ( which == st->stBackWhich )
	{
	appColorChooserColorChosen( &(st->stShadingChanged), &changed,
			&(eis->eisBackColor), &(eis->eisBackColorExplicit),
			rgb8Set, choice != CHOICEccDEFAULT, ISpropBACK_COLOR );

	return;
	}

    /*  3  */
    if  ( which == st->stForeWhich )
	{
	appColorChooserColorChosen( &(st->stShadingChanged), &changed,
			&(eis->eisForeColor), &(eis->eisForeColorExplicit),
			rgb8Set, choice != CHOICEccDEFAULT, ISpropFORE_COLOR );

	return;
	}

    LDEB(which); return;
    }

void tedShadeSetExplicitColorChoice(
				ShadingTool *			st,
				int				which,
				const RGB8Color *		rgb8Set )
    {
    const int			explicit= 1;
    ExpandedItemShading *	eis= &(st->stShadingChosen);
    int				changed= 0;

    /*  2  */
    if  ( which == st->stBackWhich )
	{
	appColorChooserColorChosen( &(st->stShadingChanged), &changed,
			&(eis->eisBackColor), &(eis->eisBackColorExplicit),
			rgb8Set, explicit, ISpropBACK_COLOR );

	if  ( changed )
	    {
	    appColorChooserSetColor( &(st->stBackColorChooser),
					    eis->eisBackColorExplicit,
					    &(eis->eisBackColor) );
	    }

	return;
	}

    /*  3  */
    if  ( which == st->stForeWhich )
	{
	appColorChooserColorChosen( &(st->stShadingChanged), &changed,
			&(eis->eisForeColor), &(eis->eisForeColorExplicit),
			rgb8Set, explicit, ISpropFORE_COLOR );

	if  ( changed && st->stForeColorChooser.ccFilled )
	    {
	    appColorChooserSetColor( &(st->stForeColorChooser),
					    eis->eisForeColorExplicit,
					    &(eis->eisForeColor) );
	    }

	return;
	}

    LDEB(which); return;
    }

/************************************************************************/
/*									*/
/*  Make a ShadingTool.							*/
/*									*/
/************************************************************************/

void tedInitShadingTool(		ShadingTool *		st )
    {
    int			i;

    st->stFrame= (APP_WIDGET)0;
    st->stPaned= (APP_WIDGET)0;

    st->stPatternLabel= (APP_WIDGET)0;
    st->stLevelLabel= (APP_WIDGET)0;
    st->stBackLabel= (APP_WIDGET)0;
    st->stForeLabel= (APP_WIDGET)0;

    st->stShowPattern= 0;
    appInitDrawnPulldown( &(st->stPatternPulldown) );

    appInitDrawingData( &(st->stPatternInplaceDrawingData) );
    appInitDrawingData( &(st->stPatternPulldownDrawingData) );

    for ( i= 0; i < DOCsp_COUNT; i++ )
	{ st->stPatternPixmaps[i]= (APP_BITMAP_IMAGE)0;	}

    st->stPulldownDrawingDataSet= 0;

    st->stLevelText= (APP_WIDGET)0;

    appInitColorChooser( &(st->stForeColorChooser) );
    appInitColorChooser( &(st->stBackColorChooser) );

    st->stCallback= (TedShadingToolCallback)0;
    st->stTarget= (void *)0;
    st->stInspector= (AppInspector *)0;

    st->stSubjectPage= -1;
    st->stBackWhich= -1;
    st->stBackWhich= -1;

    return;
    }

void tedCleanShadingTool(		ShadingTool *		st )
    {
    AppDrawingData *	add= &(st->stPatternInplaceDrawingData);

    int			i;

    for ( i= 0; i < DOCsp_COUNT; i++ )
	{
	if  ( st->stPatternPixmaps[i] )
	    { appDrawFreePixmap( add, st->stPatternPixmaps[i] );	}
	}

    appCleanDrawingData( &(st->stPatternInplaceDrawingData) );
    appCleanDrawingData( &(st->stPatternPulldownDrawingData) );

    appCleanColorChooser( &(st->stForeColorChooser) );
    appCleanColorChooser( &(st->stBackColorChooser) );

    return;
    }

void tedFormatMakeShadingTool(	ShadingTool *			st,
				AppInspector *			ai,
				APP_WIDGET			pageWidget,
				const char *			title,
				const ShadingToolResources *	str,
				int				subjectPage,
				int				backWhich,
				int				foreWhich,
				TedShadingToolCallback		callback,
				void *				target )
    {
    APP_WIDGET	row;

    /*
    const int	heightResizable= 0;
    const int	labelColumn= 0;
    const int	labelColspan= 1;
    */

    /*
    const int	textColumns= 6;
    */
    /*
    const int	valueColumn= 1;
    const int	valueColspan= 1;
    */

    /****/
    st->stCallback= callback;
    st->stTarget= target;
    st->stInspector= ai;

    st->stSubjectPage= subjectPage;
    st->stBackWhich= backWhich;
    st->stForeWhich= foreWhich;

    /****/
    title= (const char *)0;
    appMakeColumnFrameInColumn( &(st->stFrame),
					&(st->stPaned), pageWidget, title );

    /****/
#   if SHOW_PATTERN
    row= appMakeRowInColumn( st->stPaned, 2, heightResizable );

    appMakeLabelInRow( &(st->stPatternLabel), row,
				labelColumn, labelColspan, str->strPattern );
    appMakeDrawnPulldownInRow( &(st->stPatternPulldown),
					    tedShadingRedrawInplace,
					    tedShadingRedrawPulldown,
					    tedShadingClickedPulldown,
					    row, valueColumn, valueColspan,
					    (void *)st );
    st->stShowPattern= 1;
#   else
    st->stShowPattern= 0;
#   endif

    /****/
#   if 0
    appMakeLabelAndTextRow( &row, &(st->stLevelLabel), &(st->stLevelText),
				st->stPaned, str->strLevel, textColumns, 1 );
#   else
    st->stLevelLabel= (APP_WIDGET)0;
    st->stLevelText= (APP_WIDGET)0;
#   endif

    if  ( st->stLevelText )
	{
	appGuiSetTypingCallbackForText( st->stLevelText,
					tedShadingLevelTyped, (void *)st );
	}

    /****/
    appMakeLabelAndColorChooserRow( &row, &(st->stBackLabel),
			    &(st->stBackColorChooser),
			    st->stPaned, str->strBackColor,
			    &(str->strBackColorChooserResources),
			    tedShadeColorChosen, backWhich, (void *)st );

    /****/
#   if 0
    appMakeLabelAndColorChooserRow( &row, &(st->stForeLabel),
			    &(st->stForeColorChooser),
			    st->stPaned, str->strForeColor,
			    &(str->strForeColorChooserResources),
			    tedShadeColorChosen, foreWhich, (void *)st );
#   endif

    return;
    }

void tedFinishShadingTool(	ShadingTool *		st )
    {
    if  ( st->stShowPattern )
	{
	appSetDrawingDataForWidget( st->stPatternPulldown.adpInplaceDrawing,
				    1.0, &(st->stPatternInplaceDrawingData) );

	appGuiSetDrawnPulldownStrips( &(st->stPatternPulldown), DOCsp_COUNT );
	}

    /**/
    appFinishColorChooser( &(st->stBackColorChooser),
				    appGuiGetLabelFont( st->stBackLabel ) );

    if  ( st->stForeColorChooser.ccFilled )
	{
	appFinishColorChooser( &(st->stForeColorChooser) ,
				    appGuiGetLabelFont( st->stForeLabel ) );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Retrieve the shading from the shading tool.				*/
/*									*/
/*  The properties managed by the toggle and the various option menus	*/
/*  or pulldowns are already stored in st->stShadingChosen.		*/
/*  The typing callback sets the changed flag for the shading level.	*/
/*									*/
/*  Retrieve the shading level if its changed flag is set.		*/
/*									*/
/************************************************************************/

int tedShadingToolGetShading(	ExpandedItemShading *	eisSet,
				PropertyMask *		pSetMask,
				const ShadingTool *	st )
    {
    const ExpandedItemShading *	eisChosen= &(st->stShadingChosen);

    int				changed= 0;

    const int			minValue= 1;
    const int			adaptToMin= 0;
    const int			maxValue= 100;
    const int			adaptToMax= 0;

    int				level= eisChosen->eisLevel/ 100;

    PropertyMask		isDoneMask;

    if  ( st->stLevelText					&&
	  PROPmaskISSET( &(st->stShadingChanged), ISpropLEVEL )	&&
	  eisChosen->eisPattern == DOCspSOLID			)
	{
	int sav= level;

	if  ( appGetIntegerFromTextWidget( st->stLevelText, &level,
			    minValue, adaptToMin, maxValue, adaptToMax ) )
	    { return -1;	}

	if  ( level != sav )
	    { changed= 1;	}
	}

    docUpdateExpandedItemShading( &isDoneMask, eisSet,
					&(st->stShadingChanged), eisChosen );

    if  ( PROPmaskISSET( &(st->stShadingChanged), ISpropLEVEL )	&&
	  changed						)
	{ eisSet->eisLevel= 100* level; }

    *pSetMask= st->stShadingChanged;

    return 0;
    }

