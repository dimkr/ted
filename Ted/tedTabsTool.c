/************************************************************************/
/*									*/
/*  The 'Tabs' page of the format tool.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>
#   include	<limits.h>

#   include	<appGeoString.h>
#   include	<appUnit.h>

#   include	"tedApp.h"
#   include	"tedFormatTool.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Refresh the controls with the current tab stop.			*/
/*									*/
/************************************************************************/

static void tedTabToolRefreshCurrentTab(	TabsTool *	tt )
    {
    const TabStop *		ts= &(tt->ttTabStopValue);

    if  ( ts->tsTwips >= 0 )
	{
	char			scratch[50];

	appGeoLengthToString( scratch, ts->tsTwips, UNITtyPOINTS );
	appStringToTextWidget( tt->ttTabPositionText, scratch );
	}
    else{
	appStringToTextWidget( tt->ttTabPositionText, "" );
	}

    appSetOptionmenu( &(tt->ttAlignmentOptionmenu), ts->tsAlignment );

    appSetOptionmenu( &(tt->ttLeaderOptionmenu), ts->tsLeader );

    return;
    }

/************************************************************************/
/*									*/
/*  Select a certain tab in the list.					*/
/*									*/
/************************************************************************/

static void tedTabsToolSelectTab(	TabsTool *	tt,
					int		n )
    {
    ParagraphProperties *	pp= &(tt->ttParaPropertiesChosen);
    const TabStopList *		tsl= &(pp->ppTabStopList);

    if  ( n >= 0 && n >= tsl->tslTabStopCount )
	{ LLDEB(n,tsl->tslTabStopCount); n= -1;	}

    if  ( n >= 0 )
	{
	tt->ttTabStopValue= tsl->tslTabStops[n];
	tt->ttTabStopNumber= n;

	appGuiSelectPositionInListWidget( tt->ttTabPositionList, n );
	}
    else{
	docInitTabStop( &(tt->ttTabStopValue) );
	tt->ttTabStopValue.tsTwips= -1;
	tt->ttTabStopNumber= -1;

	appGuiRemoveSelectionFromListWidget( tt->ttTabPositionList );
	}

    tedTabToolRefreshCurrentTab( tt );

    return;
    }

/************************************************************************/
/*									*/
/*  Refresh the list of tabs and the currently selected tab.		*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshTabsPage(	TabsTool *	tt )
    {
    ParagraphProperties *	pp= &(tt->ttParaPropertiesChosen);
    const TabStopList *		tsl= &(pp->ppTabStopList);

    int				below= -1;
    int				above= -1;
    int				da= INT_MAX;
    int				db= INT_MAX;

    char			scratch[50];

    const TabStop *		ts;
    int				i;

    appGuiEmptyListWidget( tt->ttTabPositionList );

    ts= tsl->tslTabStops;
    for ( i= 0; i < tsl->tslTabStopCount; ts++, i++ )
	{
	appGeoLengthToString( scratch, ts->tsTwips, UNITtyPOINTS );
	appGuiAddValueToListWidget( tt->ttTabPositionList, scratch );

	if  ( ts->tsTwips <= tt->ttTabStopValue.tsTwips )
	    {
	    db= tt->ttTabStopValue.tsTwips- ts->tsTwips;
	    below= i;
	    }
	if  ( above < 0 && ts->tsTwips >= tt->ttTabStopValue.tsTwips )
	    {
	    da= ts->tsTwips- tt->ttTabStopValue.tsTwips;
	    above= i;
	    }
	}

    if  ( tsl->tslTabStopCount == 0 )
	{ tedTabsToolSelectTab( tt, -1 );		}
    else{
	if  ( da < db )
	    { tedTabsToolSelectTab( tt, above );	}
	else{ tedTabsToolSelectTab( tt, below );	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt the tabs tool to the current selection in the document.	*/
/*									*/
/************************************************************************/

void tedFormatToolRefreshTabsTool(
				TabsTool *			tt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const DocumentProperties *	dp )
    {
    const ParagraphProperties *		pp;

    PropertyMask			chgMask;
    PropertyMask			updMask;

    const int * const			colorMap= (const int *)0;
    const int * const			listStyleMap= (const int *)0;

    pp= &(ds->dsBegin.dpBi->biParaProperties);

    PROPmaskCLEAR( &updMask );
    PROPmaskADD( &updMask, PPpropTAB_STOPS );

    PROPmaskCLEAR( &chgMask );

    if  ( docUpdParaProperties( &chgMask, &(tt->ttParaPropertiesChosen),
				    &updMask, pp, colorMap, listStyleMap ) )
	{ LDEB(1); return ;	}

    PROPmaskCLEAR( &chgMask );

    if  ( docUpdParaProperties( &chgMask, &(tt->ttParaPropertiesSet),
				    &updMask, pp, colorMap, listStyleMap ) )
	{ LDEB(1); return ;	}

    /**/

    tedFormatToolRefreshTabsPage( tt );

    /**/

    PROPmaskCLEAR( &updMask );
    PROPmaskADD( &updMask, DPpropDEFTAB );

    PROPmaskCLEAR( &chgMask );

    if  ( docUpdDocumentProperties( &chgMask, &(tt->ttDocPropertiesChosen),
							    &updMask, dp ))
	{ LDEB(1); return ;	}

    PROPmaskCLEAR( &chgMask );

    if  ( docUpdDocumentProperties( &chgMask, &(tt->ttDocPropertiesSet),
							    &updMask, dp ))
	{ LDEB(1); return ;	}

    /**/

    {
    char		scratch[50];

    appGeoLengthToString( scratch, dp->dpTabIntervalTwips, UNITtyPOINTS );
    appStringToTextWidget( tt->ttTabDefaultText, scratch );
    }

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button on the tabs tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatTabsRevertParaPushed, w, voidtt )
    {
    TabsTool *		tt= (TabsTool *)voidtt;

    PropertyMask	chgMask;
    PropertyMask	updMask;

    const int * const	colorMap= (const int *)0;
    const int * const	listStyleMap= (const int *)0;

    PROPmaskCLEAR( &chgMask );

    PROPmaskCLEAR( &updMask );
    PROPmaskADD( &updMask, PPpropTAB_STOPS );

    docUpdParaProperties( &chgMask, &(tt->ttParaPropertiesChosen),
			&updMask, &(tt->ttParaPropertiesSet),
			colorMap, listStyleMap );

    tedFormatToolRefreshTabsPage( tt );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatTabsRevertDocPushed, w, voidtt )
    {
    TabsTool *			tt= (TabsTool *)voidtt;

    PropertyMask		chgMask;
    PropertyMask		updMask;

    PROPmaskCLEAR( &chgMask );

    PROPmaskCLEAR( &updMask );
    PROPmaskADD( &updMask, DPpropDEFTAB );

    docUpdDocumentProperties( &chgMask, &(tt->ttDocPropertiesChosen),
				    &updMask, &(tt->ttDocPropertiesSet) );

    {
    char			scratch[50];
    const DocumentProperties *	dp= &(tt->ttDocPropertiesChosen);

    appGeoLengthToString( scratch, dp->dpTabIntervalTwips, UNITtyPOINTS );
    appStringToTextWidget( tt->ttTabDefaultText, scratch );
    }

    return;
    }

/************************************************************************/
/*									*/
/*  Check whether a tab velue is too close to an existing tab value for	*/
/*  the current paragraph.						*/
/*									*/
/************************************************************************/

static int tedTabToolValueTooCloseToExisting(
				    const TabStopList *	tsl,
				    int			current,
				    int			value )
    {
    int				i;
    TabStop *			ts;

    ts= tsl->tslTabStops;
    for ( i= 0; i < tsl->tslTabStopCount; ts++, i++ )
	{
	if  ( i == current )
	    { continue;	}

	if  ( value >= ts->tsTwips- 200		&&
	      value <= ts->tsTwips+ 200		)
	    { return 1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Check user input.							*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedTabPositionChanged, w, voidtt )
    {
    TabsTool *			tt= (TabsTool *)voidtt;
    ParagraphProperties *	pp= &(tt->ttParaPropertiesChosen);
    const TabStopList *		tsl= &(pp->ppTabStopList);

    int				value;
    int				changed;

    if  ( tt->ttTabStopNumber < 0		||
	  tt->ttTabStopNumber >= tsl->tslTabStopCount	)
	{ value= 0;	}
    else{
	TabStop *	ts= tsl->tslTabStops+ tt->ttTabStopNumber;

	value= ts->tsTwips;
	}

    changed= 0;

    tedFormatValidateDimension( &value, &changed,
					    tt->ttTabPositionText, value );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedTabDeftabChanged, w, voidtt )
    {
    TabsTool *			tt= (TabsTool *)voidtt;
    DocumentProperties *	dp= &(tt->ttDocPropertiesChosen);

    int				value= dp->dpTabIntervalTwips;
    int				changed= 0;

    tedFormatValidateDimension( &value, &changed, tt->ttTabDefaultText, value );

    return;
    }

/************************************************************************/
/*									*/
/*  A tab was selected in the list of positions.			*/
/*									*/
/************************************************************************/

static APP_LIST_CALLBACK_H( tedTabsToolTabChosen, w, voidtt, voidlcs )
    {
    TabsTool *			tt= (TabsTool *)voidtt;
    ParagraphProperties *	pp= &(tt->ttParaPropertiesChosen);
    const TabStopList *		tsl= &(pp->ppTabStopList);

    int				position;

    position= appGuiGetPositionFromListCallback( w, voidlcs );

    /*  No: takes the role of 'Revert'
    if  ( position == tt->ttTabStopNumber )
	{ return;	}
    */

    if  ( position < 0 || position >= tsl->tslTabStopCount )
	{ LLDEB(position,tsl->tslTabStopCount);	}

    tt->ttTabStopValue= tsl->tslTabStops[position];
    tt->ttTabStopNumber= position;

    tedTabToolRefreshCurrentTab( tt );

    return;
    }

/************************************************************************/
/*									*/
/*  Previous/Next Paragraph buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatPrevPara, w, voidtt )
    {
    TabsTool *			tt= (TabsTool *)voidtt;
    EditApplication *		ea= tt->ttApplication;

    tedSelectWholeParagraph( ea, -1 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatNextPara, w, voidtt )
    {
    TabsTool *			tt= (TabsTool *)voidtt;
    EditApplication *		ea= tt->ttApplication;

    tedSelectWholeParagraph( ea, 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  Add a tab to the list.						*/
/*									*/
/*  1)  Retrieve the new position from the text widget.			*/
/*  2)  Value not too close to an existing tab stop?			*/
/*  3)  Insert in list.							*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTabToolAddTab, w, voidtt )
    {
    TabsTool *			tt= (TabsTool *)voidtt;
    EditApplication *		ea= tt->ttApplication;
    ParagraphProperties *	pp= &(tt->ttParaPropertiesChosen);
    const TabStopList *		tsl= &(pp->ppTabStopList);

    const TabsPageResources *	tpr= tt->ttPageResources;

    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    int				value;
    int				changed;

    PropertyMask		updMask;

    /*  1  */
    value= tt->ttTabStopValue.tsTwips; changed= 0;

    if  ( appGetLengthFromTextWidget( tt->ttTabPositionText,
					&value, &changed, UNITtyPOINTS,
					INT_MIN, 0, maxValue, adaptToMax ) )
	{ return;	}

    /*  2  */
    if  ( tedTabToolValueTooCloseToExisting( tsl, -1, value ) )
	{
	appQuestionRunErrorDialog( tt->ttApplication,
					tt->ttInspector->aiTopWidget,
					(APP_WIDGET)w,
					tpr->tprTooCloseMessage );
	return;
	}

    if  ( changed )
	{ tt->ttTabStopValue.tsTwips= value;	}

    /*  3  */
    changed= docParaAddTab( pp, &(tt->ttTabStopValue) );
    if  ( changed < 0 )
	{ LDEB(changed); return;	}

    PROPmaskCLEAR( &updMask );
    PROPmaskADD( &updMask, PPpropTAB_STOPS );

    if  ( tedAppChangeParagraphProperties( ea, &updMask, pp ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Change the tab that is currently selected in the list.		*/
/*  and update the selected paragraphs in the document accordingly.	*/
/*									*/
/*  2)  Retrieve the new position from the text widget.			*/
/*  3)  Value not too close to another tab stop?			*/
/*  4)  Delete the current one.						*/
/*  5)  Insert the new value in the correct position.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatChangeParaTabs, w, voidtt )
    {
    TabsTool *			tt= (TabsTool *)voidtt;
    EditApplication *		ea= tt->ttApplication;
    ParagraphProperties *	pp= &(tt->ttParaPropertiesChosen);
    const TabStopList *		tsl= &(pp->ppTabStopList);

    const TabsPageResources *	tpr= tt->ttPageResources;

    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    int				value;
    int				changed;

    PropertyMask		updMask;

    /*  2  */
    value= tt->ttTabStopValue.tsTwips; changed= 0;

    if  ( appGetLengthFromTextWidget( tt->ttTabPositionText,
					&value, &changed, UNITtyPOINTS,
					INT_MIN, 0, maxValue, adaptToMax ) )
	{ return;	}

    /*  3  */
    if  ( tedTabToolValueTooCloseToExisting( tsl, tt->ttTabStopNumber, value ) )
	{
	appQuestionRunErrorDialog( tt->ttApplication,
					tt->ttInspector->aiTopWidget,
					(APP_WIDGET)w,
					tpr->tprTooCloseMessage );
	return;
	}

    if  ( changed )
	{ tt->ttTabStopValue.tsTwips= value;	}

    /*  4  */
    if  ( tt->ttTabStopNumber >= 0			&&
	  tt->ttTabStopNumber < tsl->tslTabStopCount	)
	{ docParaDeleteTab( pp, tt->ttTabStopNumber );	}

    /*  5  */
    changed= docParaAddTab( pp, &(tt->ttTabStopValue) );
    if  ( changed < 0 )
	{ LDEB(changed); return;	}

    PROPmaskCLEAR( &updMask );
    PROPmaskADD( &updMask, PPpropTAB_STOPS );

    if  ( tedAppChangeParagraphProperties( ea, &updMask, pp ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Delete the tab that is currently selected in the list.		*/
/*									*/
/*  1)  Has a tab been selected?					*/
/*  2)  Delete it.							*/
/*  3)  Refresh list.							*/
/*  4)  If a tab remains select the one after it, or if it was the last	*/
/*	one, select the previous one.					*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTabToolDeleteTab, w, voidtt )
    {
    TabsTool *			tt= (TabsTool *)voidtt;
    EditApplication *		ea= tt->ttApplication;
    ParagraphProperties *	pp= &(tt->ttParaPropertiesChosen);
    const TabStopList *		tsl= &(pp->ppTabStopList);

    PropertyMask		updMask;

    /*  1  */
    if  ( tt->ttTabStopNumber < 0		||
	  tt->ttTabStopNumber >= tsl->tslTabStopCount	)
	{ LLDEB(tt->ttTabStopNumber,tsl->tslTabStopCount); return;	}

    /*  2  */
    docParaDeleteTab( pp, tt->ttTabStopNumber );

    if  ( tt->ttTabStopNumber < tsl->tslTabStopCount )
	{ tt->ttTabStopValue= tsl->tslTabStops[tt->ttTabStopNumber];	}

    PROPmaskCLEAR( &updMask );
    PROPmaskADD( &updMask, PPpropTAB_STOPS );

    if  ( tedAppChangeParagraphProperties( ea, &updMask, pp ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/
/*									*/
/*  2)  The 'Change' default tab distance for the document has been	*/
/*	pushed.								*/
/*									*/
/************************************************************************/

/*  2  */
static APP_BUTTON_CALLBACK_H( tedFormatTabsApplyDocPushed, w, voidtt )
    {
    TabsTool *			tt= (TabsTool *)voidtt;
    EditApplication *		ea= tt->ttApplication;
    DocumentProperties *	dp= &(tt->ttDocPropertiesChosen);

    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    int				value;
    int				changed;

    PropertyMask		updMask;

    PROPmaskCLEAR( &updMask );
    PROPmaskADD( &updMask, DPpropDEFTAB );

    value= dp->dpTabIntervalTwips;

    if  ( appGetLengthFromTextWidget( tt->ttTabDefaultText,
					&value, &changed, UNITtyPOINTS,
					INT_MIN, 0, maxValue, adaptToMax ) )
	{ return;	}

    if  ( changed )
	{ dp->dpTabIntervalTwips= value;	}

    if  ( tedAppSetDocumentProperties( ea, dp, &updMask ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/
/*									*/
/*  An alignment has been chosen in the alignment menu in the tab	*/
/*  properties frame.							*/
/*									*/
/************************************************************************/

static APP_OITEM_CALLBACK_H( tedTabAlignmentChosen, w, voidtt )
    {
    TabsTool *			tt= (TabsTool *)voidtt;
    TabStop *			ts= &(tt->ttTabStopValue);

    int				alignment;

    alignment= appGuiGetOptionmenuItemIndex( &(tt->ttAlignmentOptionmenu), w );
    if  ( alignment < 0 || alignment >= DOCta_COUNT )
	{ LLDEB(alignment,DOCta_COUNT); return;	}

    ts->tsAlignment= alignment;

    return;
    }

/************************************************************************/
/*									*/
/*  A kind of leader has been chosen in the leader menu in the tab	*/
/*  properties frame.							*/
/*									*/
/************************************************************************/

static APP_OITEM_CALLBACK_H( tedTabLeaderChosen, w, voidtt )
    {
    TabsTool *			tt= (TabsTool *)voidtt;
    TabStop *			ts= &(tt->ttTabStopValue);

    int				leader;

    leader= appGuiGetOptionmenuItemIndex( &(tt->ttLeaderOptionmenu), w );
    if  ( leader < 0 || leader >= DOCtl_COUNT )
	{ LLDEB(leader,DOCtl_COUNT); return;	}

    ts->tsLeader= leader;

    return;
    }

/************************************************************************/
/*									*/
/*  Make the tabs page of the format tool.				*/
/*									*/
/************************************************************************/

void tedFormatFillTabsPage(	TabsTool *			tt,
				const TabsPageResources *	tpr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources *	isr )
    {
    APP_WIDGET		everyLabel;
    APP_WIDGET		posLabel;
    APP_WIDGET		alignmentLabel;
    APP_WIDGET		leaderLabel;

    APP_WIDGET		row;

    const int		visibleItems= 8;

    const int		textColumns= 10;

    /**/
    tt->ttPageResources= tpr;

    /**/
    docInitDocumentProperties( &(tt->ttDocPropertiesSet) );
    docInitDocumentProperties( &(tt->ttDocPropertiesChosen) );

    docInitParagraphProperties( &(tt->ttParaPropertiesSet) );
    docInitParagraphProperties( &(tt->ttParaPropertiesChosen) );

    docInitTabStop( &(tt->ttTabStopValue) );
    tt->ttTabStopValue.tsTwips= -1;
    tt->ttTabStopNumber= -1;

    /**/
    appMakeColumnFrameInColumn( &(tt->ttTabDefaultFrame),
			    &(tt->ttTabDefaultPaned),
			    pageWidget, tpr->tprDefaultTabStopsLabel );

    appMakeLabelAndTextRow( &row, &everyLabel, &(tt->ttTabDefaultText),
			    tt->ttTabDefaultPaned,
			    tpr->tprDefaultTabStopsEvery, textColumns, 1 );

    appGuiSetGotValueCallbackForText( tt->ttTabDefaultText,
				    tedTabDeftabChanged, (void *)tt );

    appInspectorMakeButtonRow( &row, tt->ttTabDefaultPaned,
					&(tt->ttTabDefaultRevertButton),
					&(tt->ttTabDefaultApplyButton),
					tpr->tprDefaultTabStopsRevert,
					tpr->tprDefaultTabStopsApply,
					tedFormatTabsRevertDocPushed,
					tedFormatTabsApplyDocPushed, tt );
    /**/
    appGuiMakeListInColumn( &(tt->ttTabPositionList), pageWidget,
			    visibleItems, tedTabsToolTabChosen, (void *)tt );

    /**/
    appMakeColumnFrameInColumn( &(tt->ttTabPropertyFrame),
				    &(tt->ttTabPropertyPaned),
				    pageWidget, tpr->tprTabStop );

    /**/
    appMakeLabelAndTextRow( &row, &posLabel, &(tt->ttTabPositionText),
				    tt->ttTabPropertyPaned,
				    tpr->tprPosition, textColumns, 1 );

    appGuiSetGotValueCallbackForText( tt->ttTabPositionText,
				    tedTabPositionChanged, (void *)tt );
    /**/
    appInspectorMakeMenuRow( &row, &(tt->ttAlignmentOptionmenu),
				    &alignmentLabel, tt->ttTabPropertyPaned,
				    tpr->tprAlignment );

    /**/
    appInspectorMakeMenuRow( &row, &(tt->ttLeaderOptionmenu),
				    &leaderLabel, tt->ttTabPropertyPaned,
				    tpr->tprLeader );

    /**/
    appInspectorMakeButtonRow( &row, pageWidget,
		&(is->isPrevButton), &(is->isNextButton),
		isr->isrPrevButtonText, isr->isrNextButtonText,
		tedFormatPrevPara, tedFormatNextPara, tt );

    appInspectorMakeButtonRow( &row, pageWidget,
		&(is->isDeleteButton), &(is->isInsertButton),
		isr->isrDeleteButtonText, isr->isrInsertButtonText,
		tedTabToolDeleteTab, tedTabToolAddTab, tt );

    appInspectorMakeButtonRow( &row, pageWidget,
		&(is->isRevertButton), &(is->isApplyButton),
		isr->isrRevert, isr->isrApplyToSubject,
		tedFormatTabsRevertParaPushed, tedFormatChangeParaTabs, tt );

    return;
    }

/************************************************************************/
/*									*/
/*  Finish the tabs page.						*/
/*									*/
/************************************************************************/

void tedTabsToolFillChoosers(		TabsTool *			tt )
    {
    const TabsPageResources *	tpr= tt->ttPageResources;

    appFillInspectorMenu( DOCta_COUNT, DOCtaLEFT,
			tt->ttAlignmentItems, tpr->tprAlignmentOptionTexts,
			&(tt->ttAlignmentOptionmenu),
			tedTabAlignmentChosen, (void *)tt );

    appGuiEnableWidget( tt->ttAlignmentItems[DOCtaBAR], 0 );

    appFillInspectorMenu( DOCtl_COUNT, DOCtlNONE,
			tt->ttLeaderItems, tpr->tprLeaderOptionTexts,
			&(tt->ttLeaderOptionmenu),
			tedTabLeaderChosen, (void *)tt );

    appGuiEnableWidget( tt->ttLeaderItems[DOCtlTHICK], 0 );
    appGuiEnableWidget( tt->ttLeaderItems[DOCtlEQUAL], 0 );

    return;
    }

void tedFormatFinishTabsPage(		TabsTool *			tt )
    {
    appOptionmenuRefreshWidth( &(tt->ttAlignmentOptionmenu) );
    appOptionmenuRefreshWidth( &(tt->ttLeaderOptionmenu) );

    return;
    }

/************************************************************************/
/*									*/
/*  Clean allocated resources in a tabs tool.				*/
/*									*/
/************************************************************************/

void tedFormatCleanParaTabsTool(	TabsTool *			tt )
    {
    docCleanDocumentProperties( &(tt->ttDocPropertiesSet) );
    docCleanDocumentProperties( &(tt->ttDocPropertiesChosen) );
    docCleanParagraphProperties( &(tt->ttParaPropertiesSet) );
    docCleanParagraphProperties( &(tt->ttParaPropertiesChosen) );
    
    return;
    }

/************************************************************************/
/*									*/
/*  Get tabs tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedTabsSubjectResourceTable[]=
    {
    APP_RESOURCE( "formatToolTabs",
	offsetof(InspectorSubjectResources,isrSubjectName),
	"Tabs" ),
    APP_RESOURCE( "formatToolChangeTabs",
	offsetof(InspectorSubjectResources,isrApplyToSubject),
	"Change Tab" ),
    APP_RESOURCE( "tableToolRevert",
	offsetof(InspectorSubjectResources,isrRevert),
	"Revert" ),
    APP_RESOURCE( "formatToolTabsNextPara",
	offsetof(InspectorSubjectResources,isrNextButtonText),
	"Next Paragraph" ),
    APP_RESOURCE( "formatToolTabsPreviousPara",
	offsetof(InspectorSubjectResources,isrPrevButtonText),
	"Previous Paragraph" ),

    APP_RESOURCE( "formatToolTabsAddTab",
	offsetof(InspectorSubjectResources,isrInsertButtonText),
	"Add Tab" ),
    APP_RESOURCE( "formatToolTabsDeleteTab",
	offsetof(InspectorSubjectResources,isrDeleteButtonText),
	"Delete Tab" ),
    };

static AppConfigurableResource TED_TedTabsToolResourceTable[]=
    {
    /**/
    APP_RESOURCE( "formatToolDefaultTabStopsLabel",
	offsetof(TabsPageResources,tprDefaultTabStopsLabel),
	"Default Tab Stops" ),
    APP_RESOURCE( "formatToolDefaultTabStopsEvery",
	offsetof(TabsPageResources,tprDefaultTabStopsEvery),
	"Every" ),
    APP_RESOURCE( "formatToolDefaultTabStopsRevert",
	offsetof(TabsPageResources,tprDefaultTabStopsRevert),
	"Revert" ),
    APP_RESOURCE( "formatToolDefaultTabStopsApply",
	offsetof(TabsPageResources,tprDefaultTabStopsApply),
	"Apply to Document" ),

    /**/
    APP_RESOURCE( "formatToolTabPositions",
	offsetof(TabsPageResources,tprPositions),
	"Positions" ),

    APP_RESOURCE( "formatToolTabStop",
	offsetof(TabsPageResources,tprTabStop),
	"Tab Stop" ),

    APP_RESOURCE( "formatToolTabPosition",
	offsetof(TabsPageResources,tprPosition),
	"Position" ),

    APP_RESOURCE( "formatToolTabAlignment",
	offsetof(TabsPageResources,tprAlignment),
	"Alignment" ),
    /**/
    APP_RESOURCE( "formatToolTabAlignmentLeft",
	offsetof(TabsPageResources,tprAlignmentOptionTexts[DOCtaLEFT]),
	"Left" ),
    APP_RESOURCE( "formatToolTabAlignmentRight",
	offsetof(TabsPageResources,tprAlignmentOptionTexts[DOCtaRIGHT]),
	"Right" ),
    APP_RESOURCE( "formatToolTabAlignmentCenter",
	offsetof(TabsPageResources,tprAlignmentOptionTexts[DOCtaCENTER]),
	"Center" ),
    APP_RESOURCE( "formatToolTabAlignmentDecimal",
	offsetof(TabsPageResources,tprAlignmentOptionTexts[DOCtaDECIMAL]),
	"Decimal" ),
    APP_RESOURCE( "formatToolTabAlignmentBar",
	offsetof(TabsPageResources,tprAlignmentOptionTexts[DOCtaBAR]),
	"Bar" ),

    /**/
    APP_RESOURCE( "formatToolTabLeader",
	offsetof(TabsPageResources,tprLeader),
	"Leader" ),
    APP_RESOURCE( "formatToolTabLeaderNone",
	offsetof(TabsPageResources,tprLeaderOptionTexts[DOCtlNONE]),
	"None" ),
    APP_RESOURCE( "formatToolTabLeaderDots",
	offsetof(TabsPageResources,tprLeaderOptionTexts[DOCtlDOTS]),
	"Dots" ),
    APP_RESOURCE( "formatToolTabLeaderUnderline",
	offsetof(TabsPageResources,tprLeaderOptionTexts[DOCtlUNDERLINE]),
	"Line" ),
    APP_RESOURCE( "formatToolTabLeaderHyphens",
	offsetof(TabsPageResources,tprLeaderOptionTexts[DOCtlHYPH]),
	"Dashes" ),

    APP_RESOURCE( "formatToolTabLeaderThick",
	offsetof(TabsPageResources,tprLeaderOptionTexts[DOCtlTHICK]),
	"Thick Line" ),
    APP_RESOURCE( "formatToolTabLeaderEqual",
	offsetof(TabsPageResources,tprLeaderOptionTexts[DOCtlEQUAL]),
	"Double Line" ),

    /**/
    APP_RESOURCE( "formatToolTabTooCloseToOther",
	offsetof(TabsPageResources,tprTooCloseMessage),
	"Position is too close to that of an other tab" ),
    };

void tedFormatToolGetTabsResourceTable(	EditApplication *		ea,
					TabsPageResources *		tpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)tpr,
				TED_TedTabsToolResourceTable,
				sizeof(TED_TedTabsToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedTabsSubjectResourceTable,
				sizeof(TED_TedTabsSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
