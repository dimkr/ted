/************************************************************************/
/*									*/
/*  A Border Tool. Used on the pages of the format tool.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>

#   include	"tedApp.h"
#   include	"tedFormatTool.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Show the border width in a border tool.				*/
/*									*/
/************************************************************************/

static void tedBorderToolShowWide(
				BorderTool *				bt,
				const ExpandedBorderProperties *	ebp )
    {
    appEnableText( bt->btWideText, ebp->ebpStyle != DOCbsNONE );

    if  ( ebp->ebpStyle != DOCbsNONE )
	{
	char	scratch[80];

	appGeoLengthToString( scratch, ebp->ebpPenWideTwips, UNITtyPOINTS );
	appStringToTextWidget( bt->btWideText, scratch );
	}
    else{ appStringToTextWidget( bt->btWideText, "" );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Conform to border settings from the application.			*/
/*									*/
/************************************************************************/

void tedBorderToolSetProperties(	BorderTool *			bt,
					const DocumentProperties *	dp,
					const BorderProperties *	bpSet )
    {
    ExpandedBorderProperties *	ebp= &(bt->btPropertiesChosen);
    int				set= 0;

    const int			avoidZero= 1;

    docExpandBorderProperties( ebp, bpSet, dp->dpColors, dp->dpColorCount );

    if  ( ebp->ebpStyle != DOCbsNONE )
	{ set= 1;	}

    appGuiSetToggleState( bt->btOnOffToggle, set );
    tedBorderToolShowWide( bt, ebp );

    if  ( bt->btStyleRow )
	{
	appExposeRectangle( &(bt->btInplaceDrawingData), 0, 0, 0, 0 );
	}

    appColorChooserSuggestPalette( &(bt->btColorChooser),
				avoidZero, dp->dpColors, dp->dpColorCount );
    appColorChooserSetColor( &(bt->btColorChooser),
				    ebp->ebpColorExplicit,
				    &(ebp->ebpColor) );

    PROPmaskCLEAR( &(bt->btPropertiesChanged) );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the widgets of a border tool.					*/
/*									*/
/************************************************************************/

static void tedDrawBorderStyle(	AppDrawingData *		add,
				BorderTool *			bt,
				int				style,
				const DocumentRectangle *	drI )
    {
    if  ( style != DOCbsNONE )
	{
	const int	minThick= 1;

	tedDrawHorizontalBorderLine( add, style, minThick,
			    drI->drX0,
			    drI->drX1, ( drI->drY0+ drI->drY1 )/ 2 );
	}

    /*  NO.. Confuses
    appDrawDrawLine( add, drI->drX0, drI->drY0, drI->drX1, drI->drY0 );
    appDrawDrawLine( add, drI->drX1, drI->drY0, drI->drX1, drI->drY1 );
    appDrawDrawLine( add, drI->drX1, drI->drY1, drI->drX0, drI->drY1 );
    appDrawDrawLine( add, drI->drX0, drI->drY1, drI->drX0, drI->drY0 );
    */

    return;
    }

static APP_EVENT_HANDLER_H( tedBorderToolRedrawInplace, w, voidbt, exposeEvent )
    {
    BorderTool *	bt= (BorderTool *)voidbt;
    AppDrawingData *	add= &(bt->btInplaceDrawingData);

    int			wide;
    int			high;

    DocumentRectangle	drI;

    appDrawnPulldownDrawArrow( &wide, &high, w, add );

    drI.drX0= 3; drI.drX1= wide- 3;
    drI.drY0= 3; drI.drY1= high- 3;

    appDrawSetForegroundWhite( add );

    appDrawFillRectangle( add, 0, 0, wide, high );

    appDrawSetForegroundBlack( add );

    tedDrawBorderStyle( add, bt, bt->btPropertiesChosen.ebpStyle, &drI );

    return;
    }

static APP_EVENT_HANDLER_H( tedBorderToolRedrawPulldown, w, voidbt, exposeEvent )
    {
    BorderTool *	bt= (BorderTool *)voidbt;
    AppDrawingData *	add= &(bt->btPulldownDrawingData);

    int			inplaceWide;
    int			inplaceHigh;
    int			pulldownWide;
    int			pulldownHigh;

    const int		ox= 0;
    const int		oy= 0;
    DocumentRectangle	drClip;

    int			i;

    if  ( ! bt->btPulldownDrawingDataSet )
	{
	appSetDrawingDataForWidget( bt->btStylePulldown.adpPulldownDrawing,
					1.0, &(bt->btPulldownDrawingData) );

	bt->btPulldownDrawingDataSet= 1;
	}

    appDrawGetSizeOfWidget( &pulldownWide, &pulldownHigh, w );
    appDrawGetSizeOfWidget( &inplaceWide, &inplaceHigh,
					bt->btStylePulldown.adpInplaceDrawing );

    appCollectExposures( &drClip, add, ox, oy, exposeEvent );

    appDrawSetForegroundWhite( add );

    appDrawFillRectangle( add, drClip.drX0, drClip.drY0,
					    drClip.drX1- drClip.drX0+ 1,
					    drClip.drY1- drClip.drY0+ 1 );

    appDrawSetForegroundBlack( add );
    appDrawDrawRectangle( add, 0, 0, pulldownWide- 1, pulldownHigh- 1 );

    for ( i= DOCbsNONE+ 1; i < DOCbs_COUNT; i++ )
	{
	DocumentRectangle	drI;
	DocumentRectangle	drX;

	drI.drX0= 3;
	drI.drX1= pulldownWide- 3;
	drI.drY0= ( i+ 0 )* inplaceHigh+ 3;
	drI.drY1= ( i+ 1 )* inplaceHigh- 3;

	if  ( ! geoIntersectRectangle( &drX, &drI, &drClip ) )
	    { continue;	}

	tedDrawBorderStyle( add, bt, i, &drI );
	}

    return;
    }


/************************************************************************/
/*									*/
/*  One of the border on/off toggles has been activated by a user.	*/
/*									*/
/************************************************************************/

static APP_TOGGLE_CALLBACK_H( tedBorderToolOnOffToggled, w, voidbt, voidtbcs )
    {
    BorderTool *		bt= (BorderTool *)voidbt;
    ExpandedBorderProperties *	ebp= &(bt->btPropertiesChosen);

    int			set;
    int			changed= 0;

    PropertyMask	bpSetMask;

    PROPmaskCLEAR( &bpSetMask );

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( set )
	{
	if  ( ebp->ebpStyle == DOCbsNONE )
	    {
	    ebp->ebpStyle= DOCbsS;
	    PROPmaskADD( &bpSetMask, BRDRpropSTYLE );
	    PROPmaskADD( &(bt->btPropertiesChanged), BRDRpropSTYLE );
	    changed= 1;
	    }

	if  ( ebp->ebpPenWideTwips == 0 )
	    {
	    ebp->ebpPenWideTwips= 15;
	    PROPmaskADD( &bpSetMask, BRDRpropPEN_WIDE );
	    PROPmaskADD( &(bt->btPropertiesChanged), BRDRpropPEN_WIDE );
	    changed= 1;
	    }
	}
    else{
	if  ( ebp->ebpStyle != DOCbsNONE )
	    {
	    ebp->ebpStyle= DOCbsNONE;
	    PROPmaskADD( &bpSetMask, BRDRpropSTYLE );
	    PROPmaskADD( &(bt->btPropertiesChanged), BRDRpropSTYLE );
	    changed= 1;
	    }
	}

    /*
    if  ( changed && bt->btCallback )
	{ (*bt->btCallback)( bt, bt->btTarget, &bpSetMask, bp );	}
    */

    tedBorderToolShowWide( bt, ebp );

    return;
    }

/************************************************************************/
/*									*/
/*  A border style was chosen.						*/
/*									*/
/*  1)  Determine strip.						*/
/*  2)  Value in range?							*/
/*  3)  If the value changed..						*/
/*  4)  Remember new value						*/
/*  5)  Provoke a redraw of the image					*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( tedBorderToolClickedPulldown, w, voidbt, mouseEvent )
    {
    BorderTool *		bt= (BorderTool *)voidbt;
    ExpandedBorderProperties *	ebp= &(bt->btPropertiesChosen);

    int			style;

    /*  1  */
    if  ( appGuiDrawnPulldownGetStrip( &style,
				&(bt->btStylePulldown), w, mouseEvent ) )
	{ return;	}

    /*  2  */
    if  ( style < 0 || style >= DOCbs_COUNT )
	{ LLDEB(style,DOCbs_COUNT); return;	}

    /*  3  */
    if  ( ebp->ebpStyle != style )
	{
	/*  4  */
	ebp->ebpStyle= style;
	PROPmaskADD( &(bt->btPropertiesChanged), BRDRpropSTYLE );

	/*  5  */
	appExposeRectangle( &(bt->btInplaceDrawingData), 0, 0, 0, 0 );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  1)  The user typed Something in the 'Width' text box.		*/
/*  2)  The user typed 'Enter' in the 'Width' text box.			*/
/*									*/
/************************************************************************/

/*  1  */
static APP_TXTYPING_CALLBACK_H( tedFormatBorderWidthTyped, w, voidbt )
    {
    BorderTool *	bt= (BorderTool *)voidbt;

    PROPmaskADD( &(bt->btPropertiesChanged), BRDRpropPEN_WIDE );

    return;
    }

/*  2  */
static APP_TXACTIVATE_CALLBACK_H( tedFormatBorderWidthChanged, w, voidbt )
    {
    BorderTool *		bt= (BorderTool *)voidbt;
    ExpandedBorderProperties *	ebp= &(bt->btPropertiesChosen);

    int			wide= 0;
    int			changed= 0;

    char		scratch[80];

    const int		minValue= 1;
    const int		adaptToMin= 0;
    const int		maxValue= 75;
    const int		adaptToMax= 0;

    PropertyMask	bpSetMask;

    PROPmaskCLEAR( &bpSetMask );

    wide= ebp->ebpPenWideTwips;
    if  ( appGetLengthFromTextWidget( bt->btWideText,
				&wide, &changed, UNITtyPOINTS,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	{ return;	}

    appGeoLengthToString( scratch, wide, UNITtyPOINTS );
    appStringToTextWidget( bt->btWideText, scratch );

    if  ( changed )
	{
	ebp->ebpPenWideTwips= wide;
	PROPmaskADD( &bpSetMask, BRDRpropPEN_WIDE );
	PROPmaskADD( &(bt->btPropertiesChanged), BRDRpropPEN_WIDE );
	}

    /*
    if  ( changed && bt->btCallback )
	{ (*bt->btCallback)( bt, bt->btTarget, &bpSetMask, bp ); }
    */

    return;
    }

/************************************************************************/
/*									*/
/*  A color for the border was chosen.					*/
/*									*/
/*  1)  Actually.. No color was selected but the user wants to use a	*/
/*	more detailed tool.						*/
/*  2)  color chosen.							*/
/*									*/
/************************************************************************/

static void tedBorderColorChosen(
				ColorChooser *			cc,
				int				which,
				void *				voidbt,
				int				choice,
				const RGB8Color *		rgb8Set )
    {
    BorderTool *		bt= (BorderTool *)voidbt;
    ExpandedBorderProperties *	ebp= &(bt->btPropertiesChosen);
    int				changed= 0;

    /*  1  */
    if  ( choice == CHOICEccMORE )
	{
	appInspectorShowRgbPage( bt->btInspector,
			    bt->btSubjectPage, which, &(ebp->ebpColor) );
	return;
	}

    /*  2  */
    appColorChooserColorChosen( &(bt->btPropertiesChanged), &changed,
			&(ebp->ebpColor), &(ebp->ebpColorExplicit),
			rgb8Set, choice != CHOICEccDEFAULT, BRDRpropCOLOR );

    return;
    }

void tedBorderSetExplicitColorChoice(	BorderTool *		bt,
					const RGB8Color *	rgb8Set )
    {
    const int			explicit= 1;
    ExpandedBorderProperties *	ebp= &(bt->btPropertiesChosen);
    int				changed= 0;

    appColorChooserColorChosen( &(bt->btPropertiesChanged), &changed,
				&(ebp->ebpColor), &(ebp->ebpColorExplicit),
				rgb8Set, explicit, BRDRpropCOLOR );

    if  ( changed )
	{
	appColorChooserSetColor( &(bt->btColorChooser),
				    ebp->ebpColorExplicit, &(ebp->ebpColor) );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make a BorderTool.							*/
/*									*/
/************************************************************************/

void tedInitBorderTool(		BorderTool *		bt )
    {
    bt->btInspector= (AppInspector *)0;
    bt->btSubjectPage= -1;
    bt->btWhich= -1;

    bt->btFrame= (APP_WIDGET)0;
    bt->btPaned= (APP_WIDGET)0;
    bt->btOnOffToggle= (APP_WIDGET)0;
    bt->btWideText= (APP_WIDGET)0;

    bt->btStyleRow= (APP_WIDGET)0;
    bt->btStyleLabel= (APP_WIDGET)0;
    appInitDrawnPulldown( &(bt->btStylePulldown) );

    appInitDrawingData( &(bt->btInplaceDrawingData) );
    appInitDrawingData( &(bt->btPulldownDrawingData) );

    bt->btColorRow= (APP_WIDGET)0;
    bt->btColorLabel= (APP_WIDGET)0;
    appInitColorChooser( &(bt->btColorChooser) );

    docInitExpandedBorderProperties( &(bt->btPropertiesChosen) );
    PROPmaskCLEAR( &(bt->btPropertiesChanged) );

    return;
    }

void tedCleanBorderTool(		BorderTool *		bt )
    {
    appCleanDrawingData( &(bt->btInplaceDrawingData) );
    appCleanDrawingData( &(bt->btPulldownDrawingData) );

    return;
    }

void tedMakeBorderTool(		BorderTool *			bt,
				AppInspector *			ai,
				APP_WIDGET			pageWidget,
				const char *			title,
				const BorderToolResources *	btr,
				int				subjectPage,
				int				which )
    {
    const int	heightResizable= 0;

    const int	toggleColumn= 0;
    const int	textColumn= toggleColumn+ 1;
    const int	textColspan= 1;

    const int	textColumns= 8;
    const int	textEnabled= 0;

    const int	labelColumn= 0;
    const int	labelColspan= 1;

    const int	valueColumn= 1;
    const int	valueColspan= 1;

    APP_WIDGET	row;

    /**/
    bt->btInspector= ai;
    bt->btSubjectPage= subjectPage;
    bt->btWhich= which;

    /**/
    docInitExpandedBorderProperties( &(bt->btPropertiesChosen) );

    appInitDrawingData( &(bt->btInplaceDrawingData) );
    appInitDrawingData( &(bt->btPulldownDrawingData) );
    bt->btPulldownDrawingDataSet= 0;

    /**/
    appMakeColumnFrameInColumn( &(bt->btFrame),
				&(bt->btPaned), pageWidget, (const char *)0 );
				/*
				&(bt->btPaned), pageWidget, title );
				*/

    /**/
    row= appMakeRowInColumn( bt->btPaned, 2, heightResizable );

    bt->btOnOffToggle= appMakeToggleInRow( row, title,
				    tedBorderToolOnOffToggled, (void *)bt, 0 );

    appMakeTextInRow( &(bt->btWideText), row, textColumn, textColspan,
						    textColumns, textEnabled );

    appGuiSetGotValueCallbackForText( bt->btWideText,
				    tedFormatBorderWidthChanged, (void *)bt );
    appGuiSetTypingCallbackForText( bt->btWideText,
				tedFormatBorderWidthTyped, (void *)bt );

    /**/
#   if 0
    bt->btStyleRow= appMakeRowInColumn( bt->btPaned, 2, heightResizable );
#   else
    bt->btStyleRow= (APP_WIDGET)0;
#   endif

    if  ( bt->btStyleRow )
	{
	appMakeLabelInRow( &(bt->btStyleLabel), bt->btStyleRow,
				labelColumn, labelColspan, btr->btrStyle );

	appMakeDrawnPulldownInRow( &(bt->btStylePulldown),
				    tedBorderToolRedrawInplace,
				    tedBorderToolRedrawPulldown,
				    tedBorderToolClickedPulldown,
				    bt->btStyleRow, valueColumn, valueColspan,
				    (void *)bt );
	}

    /****/
    appMakeLabelAndColorChooserRow( &(bt->btColorRow), &(bt->btColorLabel),
			    &(bt->btColorChooser),
			    bt->btPaned, btr->btrColor,
			    &(btr->btrColorChooserResources),
			    tedBorderColorChosen, which, (void *)bt );

    return;
    }

void tedFinishBorderTool(	BorderTool *		bt )
    {
    if  ( bt->btStyleRow )
	{
	appSetDrawingDataForWidget( bt->btStylePulldown.adpInplaceDrawing, 1.0,
						&(bt->btInplaceDrawingData) );

	appGuiSetDrawnPulldownStrips( &(bt->btStylePulldown), DOCbs_COUNT );
	}

    appFinishColorChooser( &(bt->btColorChooser),
				    appGuiGetLabelFont( bt->btColorLabel ) );

    return;
    }

/************************************************************************/
/*									*/
/*  Retrieve the border properties from the border tool.		*/
/*									*/
/*  The properties managed by the toggle and the various option menus	*/
/*  or pulldowns are already stored in bt->btPropertiesChosen.		*/
/*  The typing callback sets the changed flag for the pen width.	*/
/*									*/
/*  Retrieve the pen width if its changed flag is set.			*/
/*									*/
/************************************************************************/

int tedBorderToolGetProperties(	BorderProperties *	bpSet,
				PropertyMask *		pSetMask,
				const BorderTool *	bt,
				DocumentProperties *	dp )
    {
    const ExpandedBorderProperties *	ebpChosen= &(bt->btPropertiesChosen);

    int				changed= 0;

    const int			minValue= 1;
    const int			adaptToMin= 0;
    const int			maxValue= 75;
    const int			adaptToMax= 0;

    int				wide= ebpChosen->ebpPenWideTwips;

    PropertyMask		bpDoneMask;

    if  ( PROPmaskISSET( &(bt->btPropertiesChanged), BRDRpropPEN_WIDE )	&&
	  ebpChosen->ebpStyle != DOCbsNONE				)
	{
	if  ( appGetLengthFromTextWidget( bt->btWideText,
			    &wide, &changed, UNITtyPOINTS,
			    minValue, adaptToMin, maxValue, adaptToMax ) )
	    { return -1;	}
	}

    docIndirectBorderProperties( &bpDoneMask, bpSet,
				    &(bt->btPropertiesChanged), ebpChosen,
				    &(dp->dpColors), &(dp->dpColorCount) );
    if  ( PROPmaskISSET( &(bt->btPropertiesChanged), BRDRpropPEN_WIDE )	&&
	  changed							)
	{ bpSet->bpPenWideTwips= wide; }

    *pSetMask= bt->btPropertiesChanged;

    return 0;
    }

