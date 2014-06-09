/************************************************************************/
/*									*/
/*  The 'Paragraph' page of the format tool.				*/
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
/*  Update the controls for SpaceAbove/SpaceBelow.			*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshSpaceAround(	APP_WIDGET	text,
						APP_WIDGET	toggle,
						int		space )
    {
    char		scratch[50];

    if  ( space == 0 )
	{
	appStringToTextWidget( text, "" );
	appEnableText( text, 0 );

	appGuiSetToggleState( toggle, 0 );
	}
    else{
	appGeoLengthToString( scratch, space, UNITtyPOINTS );
	appStringToTextWidget( text, scratch );
	appEnableText( text, 1 );

	appGuiSetToggleState( toggle, 1 );
	}
    }

static void tedFormatToolRefreshParagraphPage(	ParagraphLayoutTool *	plt )
    {
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    char		scratch[50];

    if  ( pp->ppListOverride > 0 )
	{
	appIntegerToTextWidget( plt->ptListLevelText, pp->ppListLevel+ 1 );

	appEnableText( plt->ptFirstIndentText, 0 );
	appEnableText( plt->ptLeftIndentText, 0 );
	}
    else{
	appStringToTextWidget( plt->ptListLevelText, "" );

	appEnableText( plt->ptFirstIndentText, 1 );
	appEnableText( plt->ptLeftIndentText, 1 );
	}

    appGeoLengthToString( scratch,
	    pp->ppLeftIndentTwips+ pp->ppFirstIndentTwips, UNITtyPOINTS );
    appStringToTextWidget( plt->ptFirstIndentText, scratch );

    appGeoLengthToString( scratch, pp->ppLeftIndentTwips, UNITtyPOINTS );
    appStringToTextWidget( plt->ptLeftIndentText, scratch );

    appGeoLengthToString( scratch, pp->ppRightIndentTwips, UNITtyPOINTS );
    appStringToTextWidget( plt->ptRightIndentText, scratch );

    tedFormatToolRefreshSpaceAround( plt->ptSpaceAboveText,
			    plt->ptSpaceAboveToggle, pp->ppSpaceBeforeTwips );

    tedFormatToolRefreshSpaceAround( plt->ptSpaceBelowText,
			    plt->ptSpaceBelowToggle, pp->ppSpaceAfterTwips );

    tedFormatRefreshHeightChooser( &(plt->ptLineDistChooser),
						pp->ppLineSpacingTwips );

    appSetOptionmenu( &(plt->ptAlignOptionmenu), pp->ppAlignment );

    appGuiSetToggleState( plt->ptOnNewPageToggle, pp->ppStartsOnNewPage );
    appGuiSetToggleState( plt->ptKeepOnPageToggle, pp->ppKeepOnPage );

    appGuiSetToggleState( plt->ptWidowControlToggle, pp->ppWidowControl );
    appGuiSetToggleState( plt->ptKeepWithNextToggle, pp->ppKeepWithNext );

    return;
    }


void tedFormatToolRefreshParaLayoutTool(
				ParagraphLayoutTool *		plt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds )
    {
    const ParagraphProperties *		pp;

    PropertyMask			ppChgMask;
    PropertyMask			ppUpdMask;

    BufferItem *			bi= ds->dsBegin.dpBi;

    const int * const			colorMap= (const int *)0;
    const int * const			listStyleMap= (const int *)0;

    pp= &(bi->biParaProperties);

    PROPmaskCLEAR( &ppUpdMask );
    utilPropMaskFill( &ppUpdMask, PPprop_COUNT );
    PROPmaskUNSET( &ppUpdMask, PPpropIN_TABLE );

    PROPmaskCLEAR( &ppChgMask );

    if  ( docUpdParaProperties( &ppChgMask, &(plt->ptPropertiesChosen),
				    &ppUpdMask, pp, colorMap, listStyleMap ) )
	{ LDEB(1); return ;	}

    PROPmaskCLEAR( &ppChgMask );

    if  ( docUpdParaProperties( &ppChgMask, &(plt->ptPropertiesSet),
				    &ppUpdMask, pp, colorMap, listStyleMap ) )
	{ LDEB(1); return ;	}

    appGuiEnableWidget( plt->ptOnNewPageToggle,
			bi->biInExternalItem == DOCinBODY &&
			! pp->ppInTable );

    appGuiEnableWidget( is->isPrevButton,
				docPrevParagraph( bi ) != (BufferItem *)0 );
    appGuiEnableWidget( is->isNextButton,
				docNextParagraph( bi ) != (BufferItem *)0 );

    tedFormatToolRefreshParagraphPage( plt );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button on the paragraph tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatParaRevertPushed, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;

    PropertyMask		ppChgMask;
    PropertyMask		ppUpdMask;

    const int * const		colorMap= (const int *)0;
    const int * const		listStyleMap= (const int *)0;

    PROPmaskCLEAR( &ppChgMask );

    PROPmaskCLEAR( &ppUpdMask );
    utilPropMaskFill( &ppUpdMask, PPprop_COUNT );

    docUpdParaProperties( &ppChgMask, &(plt->ptPropertiesChosen),
					&ppUpdMask, &(plt->ptPropertiesSet),
					colorMap, listStyleMap );

    tedFormatToolRefreshParagraphPage( plt );

    return;
    }

/************************************************************************/
/*									*/
/*  Check user input.							*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedParaFirstIndentChanged, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				lipfi;
    int				changed;

    lipfi=  pp->ppLeftIndentTwips+ pp->ppFirstIndentTwips;

    tedFormatValidateDimension( &lipfi, &changed,
					    plt->ptFirstIndentText, lipfi );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedParaLeftIndentChanged, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				value;
    int				changed;

    tedFormatValidateDimension( &value, &changed,
			    plt->ptLeftIndentText, pp->ppLeftIndentTwips );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedParaRightIndentChanged, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				value;
    int				changed;

    tedFormatValidateDimension( &value, &changed,
			    plt->ptRightIndentText, pp->ppRightIndentTwips );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedParaSpaceAboveChanged, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				value;
    int				changed;

    tedFormatValidateDimension( &value, &changed,
			    plt->ptSpaceAboveText, pp->ppSpaceBeforeTwips );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedParaSpaceBelowChanged, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				value;
    int				changed;

    tedFormatValidateDimension( &value, &changed,
			    plt->ptSpaceBelowText, pp->ppSpaceAfterTwips );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedParaLineDistanceChanged, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				value;
    int				changed;

    tedFormatValidateDimension( &value, &changed,
					    plt->ptLineDistChooser.hcText,
					    pp->ppLineSpacingTwips );

    return;
    }

/************************************************************************/
/*									*/
/*  Previous/Next Paragraph buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatPrevPara, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    EditApplication *		ea= plt->ptApplication;

    tedSelectWholeParagraph( ea, -1 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatNextPara, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    EditApplication *		ea= plt->ptApplication;

    tedSelectWholeParagraph( ea, 1 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatSelectPara, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    EditApplication *		ea= plt->ptApplication;

    tedSelectWholeParagraph( ea, 0 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatDeletePara, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    EditApplication *		ea= plt->ptApplication;

    tedDeleteCurrentParagraph( ea );
    }

static APP_BUTTON_CALLBACK_H( tedFormatInsertPara, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    EditApplication *		ea= plt->ptApplication;

    tedInsertParagraph( ea, 0 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatAppendPara, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    EditApplication *		ea= plt->ptApplication;

    tedInsertParagraph( ea, 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Change' button on the paragraph page has been pushed.		*/
/*									*/
/************************************************************************/

static int tedFormatToolGetSpaceAround(		int *		pValue,
						int *		pChanged,
						APP_WIDGET	text,
						APP_WIDGET	toggle,
						int		value )
    {
    int			state;
    int			changed= 0;

    const int		minValue= 1;
    const int		adaptToMin= 0;
    const int		maxValue= INT_MAX;
    const int		adaptToMax= 0;

    state= appGuiGetToggleState( toggle ) != 0;
    if  ( ! state )
	{ *pChanged= ( value != 0 ); *pValue= 0; return 0;	}

    state= value;
    if  ( appGetLengthFromTextWidget( text,
			    &state, &changed, UNITtyPOINTS,
			    minValue, adaptToMin, maxValue, adaptToMax ) )
	{ return -1;	}

    if  ( changed )
	{ value= state;	}

    *pChanged= changed; *pValue=value; return 0;
    }

static APP_BUTTON_CALLBACK_H( tedFormatChangePara, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    EditApplication *		ea= plt->ptApplication;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				li;
    int				lipfi;

    int				value;
    int				changed;

    PropertyMask		ppUpdMask;

    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    PROPmaskCLEAR( &ppUpdMask );
    utilPropMaskFill( &ppUpdMask, PPprop_COUNT );
    PROPmaskUNSET( &ppUpdMask, PPpropIN_TABLE );
    PROPmaskUNSET( &ppUpdMask, PPpropTAB_STOPS );
    PROPmaskUNSET( &ppUpdMask, PPpropTOP_BORDER );
    PROPmaskUNSET( &ppUpdMask, PPpropBOTTOM_BORDER );

    lipfi= pp->ppLeftIndentTwips+ pp->ppFirstIndentTwips;
    if  ( appGetLengthFromTextWidget( plt->ptFirstIndentText,
					&lipfi, &changed, UNITtyPOINTS,
					INT_MIN, 0, maxValue, adaptToMax ) )
	{ return;	}

    li= pp->ppLeftIndentTwips;
    if  ( appGetLengthFromTextWidget( plt->ptLeftIndentText,
					&li, &changed, UNITtyPOINTS,
					1, 1, maxValue, adaptToMax ) )
	{ return;	}

    pp->ppLeftIndentTwips= li;
    pp->ppFirstIndentTwips= lipfi- li;

    value= pp->ppRightIndentTwips;
    if  ( appGetLengthFromTextWidget( plt->ptRightIndentText,
					&value, &changed, UNITtyPOINTS,
					1, 1, maxValue, adaptToMax ) )
	{ return;	}
    pp->ppRightIndentTwips= value;

    value= pp->ppLineSpacingTwips;
    if  ( tedFormatToolGetHeight( &value, &(plt->ptLineDistChooser) ) )
	{ return;	}
    pp->ppLineSpacingTwips= value;

    value= pp->ppSpaceBeforeTwips;
    if  ( tedFormatToolGetSpaceAround( &value, &changed,
		    plt->ptSpaceAboveText, plt->ptSpaceAboveToggle, value ) )
	{ return;	}
    pp->ppSpaceBeforeTwips= value;

    value= pp->ppSpaceAfterTwips;
    if  ( tedFormatToolGetSpaceAround( &value, &changed,
		    plt->ptSpaceBelowText, plt->ptSpaceBelowToggle, value ) )
	{ return;	}
    pp->ppSpaceAfterTwips= value;

    if  ( tedAppChangeParagraphProperties( ea, &ppUpdMask, pp ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/
/*									*/
/*  One of the Space toggles was activated:				*/
/*									*/
/************************************************************************/

static APP_TOGGLE_CALLBACK_H( tedFormatSpaceAboveToggled, w, voidplt, voidtbcs )
    {
    ParagraphLayoutTool *		plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *		pp= &(plt->ptPropertiesChosen);

    int					set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( set )
	{
	if  ( plt->ptPropertiesSet.ppSpaceBeforeTwips != 0 )
	    {
	    pp->ppSpaceBeforeTwips=
			    plt->ptPropertiesSet.ppSpaceBeforeTwips;
	    }
	else{
	    EditDocument *	ed= plt->ptApplication->eaCurrentDocument;
	    int			lineHeight;

	    if  ( ! ed || tedGetParaLineHeight( &lineHeight, ed ) )
		{ XLDEB(ed,set);			}
	    else{ pp->ppSpaceBeforeTwips= lineHeight;	}
	    }
	}
    else{
	pp->ppSpaceBeforeTwips= 0;
	}

    tedFormatToolRefreshSpaceAround( plt->ptSpaceAboveText,
		    plt->ptSpaceAboveToggle, pp->ppSpaceBeforeTwips );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedFormatSpaceBelowToggled, w, voidplt, voidtbcs )
    {
    ParagraphLayoutTool *		plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *		pp= &(plt->ptPropertiesChosen);
    
    int					set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( set )
	{
	if  ( plt->ptPropertiesSet.ppSpaceAfterTwips != 0 )
	    {
	    pp->ppSpaceAfterTwips=
			    plt->ptPropertiesSet.ppSpaceAfterTwips;
	    }
	else{
	    EditDocument *	ed= plt->ptApplication->eaCurrentDocument;
	    int			lineHeight;

	    if  ( ! ed || tedGetParaLineHeight( &lineHeight, ed ) )
		{ XLDEB(ed,set);			}
	    else{ pp->ppSpaceAfterTwips= lineHeight;	}
	    }
	}
    else{
	pp->ppSpaceAfterTwips= 0;
	}

    tedFormatToolRefreshSpaceAround( plt->ptSpaceBelowText,
		    plt->ptSpaceBelowToggle, pp->ppSpaceAfterTwips );

    return;
    }

static APP_OITEM_CALLBACK_H( tedParaAlignmentChosen, w, voidplt )
    {
    ParagraphLayoutTool *		plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *		pp= &(plt->ptPropertiesChosen);

    int					alignment;

    alignment= appGuiGetOptionmenuItemIndex( &(plt->ptAlignOptionmenu), w );
    if  ( alignment < 0 || alignment >= DOCia_COUNT )
	{ LLDEB(alignment,DOCia_COUNT); return;	}

    pp->ppAlignment= alignment;

    return;
    }

/************************************************************************/
/*									*/
/*  A line Spacing was chosen with the menu.				*/
/*									*/
/************************************************************************/

static APP_OITEM_CALLBACK_H( tedParaLineDistChosen, w, voidplt )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    HeightChooser *		hc= &(plt->ptLineDistChooser);
    int				how;

    int				defaultValue;

    defaultValue= plt->ptPropertiesChosen.ppLineSpacingTwips;
    if  ( defaultValue == 0 )
	{ defaultValue= plt->ptPropertiesSet.ppLineSpacingTwips;	}
    if  ( defaultValue == 0 )
	{ defaultValue= 240;	}

    how= appGuiGetOptionmenuItemIndex( &(hc->hcOptionmenu), w );
    if  ( how < 0 || how >= HC__COUNT )
	{ LDEB(how); return;	}

    tedFormatHeightChosen( how, hc, defaultValue );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedParaOnNewPageToggled, w, voidplt, voidtbcs )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    pp->ppStartsOnNewPage= ( set != 0 ) && ! pp->ppInTable;

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedParaKeepOnPageToggled, w, voidplt, voidtbcs )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    pp->ppKeepOnPage= ( set != 0 );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedParaWidowControlToggled, w, voidplt, voidtbcs )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    pp->ppWidowControl= ( set != 0 );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedParaKeepWithNextToggled, w, voidplt, voidtbcs )
    {
    ParagraphLayoutTool *	plt= (ParagraphLayoutTool *)voidplt;
    ParagraphProperties *	pp= &(plt->ptPropertiesChosen);

    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    pp->ppKeepWithNext= ( set != 0 );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the paragraph layout page.					*/
/*									*/
/************************************************************************/

void tedFormatFillParagraphLayoutPage(
			ParagraphLayoutTool *			plt,
			const ParagraphLayoutPageResources *	plpr,
			InspectorSubject *			is,
			APP_WIDGET				pageWidget,
			const InspectorSubjectResources *	isr )
    {
    APP_WIDGET		firstLabel;
    APP_WIDGET		leftLabel;
    APP_WIDGET		rightLabel;
    APP_WIDGET		alignLabel;

    APP_WIDGET		row;

    const int		textColumns= 10;

    /**/
    plt->ptPageResources= plpr;

    /**/
    docInitParagraphProperties( &(plt->ptPropertiesSet) );
    docInitParagraphProperties( &(plt->ptPropertiesChosen) );

    /**/
    appMakeLabelAndTextRow( &row, &firstLabel, &(plt->ptListLevelText),
		    pageWidget, plpr->plprParaListLevel, textColumns, 0 );

    /**/
    appMakeLabelAndTextRow( &row, &firstLabel, &(plt->ptFirstIndentText),
		    pageWidget, plpr->plprParaFirstIndent, textColumns, 1 );

    appGuiSetGotValueCallbackForText( plt->ptFirstIndentText,
				    tedParaFirstIndentChanged, (void *)plt );
    /**/
    appMakeLabelAndTextRow( &row, &leftLabel, &(plt->ptLeftIndentText),
		    pageWidget, plpr->plprParaLeftIndent, textColumns, 1 );

    appGuiSetGotValueCallbackForText( plt->ptLeftIndentText,
				    tedParaLeftIndentChanged, (void *)plt );
    /**/
    appMakeLabelAndTextRow( &row, &rightLabel, &(plt->ptRightIndentText),
		    pageWidget, plpr->plprParaRightIndent, textColumns, 1 );

    appGuiSetGotValueCallbackForText( plt->ptRightIndentText,
				    tedParaRightIndentChanged, (void *)plt );
    /**/
    appInspectorMakeMenuRow( &row, &(plt->ptAlignOptionmenu),
		    &alignLabel, pageWidget, plpr->pprParaAlignment );

    /**/
    tedFormatMakeHeightRow( &row, (void *)plt, pageWidget,
		    &(plt->ptLineDistChooser), tedParaLineDistanceChanged );

    /**/
    appMakeToggleAndTextRow( &row, &(plt->ptSpaceAboveToggle),
		    &(plt->ptSpaceAboveText), pageWidget,
		    plpr->pprParaSpaceAbove,
		    tedFormatSpaceAboveToggled, (void *)plt,
		    textColumns, 1 );

    appGuiSetGotValueCallbackForText( plt->ptSpaceAboveText,
				    tedParaSpaceAboveChanged, (void *)plt );
    /**/
    appMakeToggleAndTextRow( &row, &(plt->ptSpaceBelowToggle),
		    &(plt->ptSpaceBelowText), pageWidget,
		    plpr->pprParaSpaceBelow,
		    tedFormatSpaceBelowToggled, (void *)plt,
		    textColumns, 1 );

    appGuiSetGotValueCallbackForText( plt->ptSpaceBelowText,
				    tedParaSpaceBelowChanged, (void *)plt );

    /**/
    appInspectorMakeToggleRow( &row, pageWidget,
		    &(plt->ptOnNewPageToggle), &(plt->ptKeepOnPageToggle),
		    plpr->pprOnNewPage, plpr->pprOnOnePage,
		    tedParaOnNewPageToggled,
		    tedParaKeepOnPageToggled, (void *)plt );

    /**/
    appInspectorMakeToggleRow( &row, pageWidget,
		    &(plt->ptWidowControlToggle), &(plt->ptKeepWithNextToggle),
		    plpr->pprWidctrl, plpr->pprKeepWithNext,
		    tedParaWidowControlToggled,
		    tedParaKeepWithNextToggled, (void *)plt );

    /**/
    appInspectorMakeButtonRow( &row, pageWidget,
			&(is->isPrevButton), &(is->isNextButton),
			isr->isrPrevButtonText, isr->isrNextButtonText,
			tedFormatPrevPara, tedFormatNextPara, plt );

    appInspectorMakeButtonRow( &row, pageWidget,
			&(is->isSelectButton), &(is->isDeleteButton),
			isr->isrSelectButtonText, isr->isrDeleteButtonText,
			tedFormatSelectPara, tedFormatDeletePara, plt );

    appInspectorMakeButtonRow( &row, pageWidget,
			&(is->isInsertButton), &(is->isAppendButton),
			isr->isrInsertButtonText, isr->isrAppendButtonText,
			tedFormatInsertPara, tedFormatAppendPara, plt );

    appInspectorMakeButtonRow( &row, pageWidget,
			&(is->isRevertButton), &(is->isApplyButton),
			isr->isrRevert, isr->isrApplyToSubject,
			tedFormatParaRevertPushed, tedFormatChangePara, plt );

    return;
    }

/************************************************************************/
/*									*/
/*  Finish the paragraph page.						*/
/*									*/
/************************************************************************/

void tedFormatFillParagraphLayoutChoosers(
			    ParagraphLayoutTool *			plt,
			    const ParagraphLayoutPageResources *	plpr )
    {
    tedFormatFillHeightChooser( &(plt->ptLineDistChooser),
					    tedParaLineDistChosen, plt,
					    plpr->pprParaLineDistFree,
					    plpr->pprParaLineDistAtLeast,
					    plpr->pprParaLineDistExactly );

    appFillInspectorMenu( DOCia_COUNT, DOCiaLEFT,
			plt->ptAlignItems, plpr->pprParaAlignMenuTexts,
			&(plt->ptAlignOptionmenu),
			tedParaAlignmentChosen, (void *)plt );

    return;
    }

void tedFormatFinishParaLayoutPage(
				ParagraphLayoutTool *			plt,
				TedFormatTool *				tft,
				const ParagraphLayoutPageResources *	plpr )
    {
    appOptionmenuRefreshWidth( &(plt->ptLineDistChooser.hcOptionmenu) );

    appOptionmenuRefreshWidth( &(plt->ptAlignOptionmenu) );

    return;
    }

/************************************************************************/
/*									*/
/*  Clean the paragraph tool.						*/
/*									*/
/************************************************************************/

void tedFormatCleanParaLayoutTool(	ParagraphLayoutTool *		plt )
    {
    docCleanParagraphProperties( &(plt->ptPropertiesSet) );
    docCleanParagraphProperties( &(plt->ptPropertiesChosen) );
    
    return;
    }

/************************************************************************/
/*									*/
/*  Get paragraph tool resources.					*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedParagraphSubjectResourceTable[]=
    {
    APP_RESOURCE( "formatToolParaLayout",
	offsetof(InspectorSubjectResources,isrSubjectName),
	"Paragraph Formatting" ),
    APP_RESOURCE( "formatToolChangePara",
	offsetof(InspectorSubjectResources,isrApplyToSubject),
	"Apply to Paragraph" ),
    APP_RESOURCE( "tableToolRevert",
	offsetof(InspectorSubjectResources,isrRevert),
	"Revert" ),
    APP_RESOURCE( "formatToolNextPara",
	offsetof(InspectorSubjectResources,isrNextButtonText),
	"Next Paragraph" ),
    APP_RESOURCE( "formatToolPreviousPara",
	offsetof(InspectorSubjectResources,isrPrevButtonText),
	"Previous Paragraph" ),
    APP_RESOURCE( "formatToolSelectPara",
	offsetof(InspectorSubjectResources,isrSelectButtonText),
	"Select Paragraph" ),
    APP_RESOURCE( "formatToolDeletePara",
	offsetof(InspectorSubjectResources,isrDeleteButtonText),
	"Delete Paragraph" ),
    APP_RESOURCE( "formatToolInsertPara",
	offsetof(InspectorSubjectResources,isrInsertButtonText),
	"Insert Paragraph" ),
    APP_RESOURCE( "formatToolAppendPara",
	offsetof(InspectorSubjectResources,isrAppendButtonText),
	"Append Paragraph" ),
    };

static AppConfigurableResource TED_TedParagraphToolResourceTable[]=
    {
    /**/
    APP_RESOURCE( "formatToolParaListLevel",
	offsetof(ParagraphLayoutPageResources,plprParaListLevel),
	"List Level" ),
    APP_RESOURCE( "formatToolParaFirstIndent",
	offsetof(ParagraphLayoutPageResources,plprParaFirstIndent),
	"First Line Indent" ),
    APP_RESOURCE( "formatToolParaLeftIndent",
	offsetof(ParagraphLayoutPageResources,plprParaLeftIndent),
	"Left Indent" ),
    APP_RESOURCE( "formatToolParaRightIndent",
	offsetof(ParagraphLayoutPageResources,plprParaRightIndent),
	"Right Indent" ),

    /**/
    APP_RESOURCE( "formatToolParaAlignment",
	offsetof(ParagraphLayoutPageResources,pprParaAlignment),
	"Alignment" ),
    APP_RESOURCE( "formatToolParaAlignLeft",
	offsetof(ParagraphLayoutPageResources,
				    pprParaAlignMenuTexts[DOCiaLEFT]),
	"Left" ),
    APP_RESOURCE( "formatToolParaAlignRight",
	offsetof(ParagraphLayoutPageResources,
				    pprParaAlignMenuTexts[DOCiaRIGHT]),
	"Right" ),
    APP_RESOURCE( "formatToolParaAlignCentered",
	offsetof(ParagraphLayoutPageResources,
				    pprParaAlignMenuTexts[DOCiaCENTERED]),
	"Centered" ),
    APP_RESOURCE( "formatToolParaAlignJustified",
	offsetof(ParagraphLayoutPageResources,
				    pprParaAlignMenuTexts[DOCiaJUSTIFIED]),
	"Justified" ),

    /**/
    APP_RESOURCE( "formatToolLineDistFree",
	offsetof(ParagraphLayoutPageResources,pprParaLineDistFree),
	"Spacing Free" ),
    APP_RESOURCE( "formatToolLineDistExactly",
	offsetof(ParagraphLayoutPageResources,pprParaLineDistExactly),
	"Spacing Exactly" ),
    APP_RESOURCE( "formatToolLineDistAtLeast",
	offsetof(ParagraphLayoutPageResources,pprParaLineDistAtLeast),
	"Spacing at Least" ),
    /**/
    APP_RESOURCE( "formatToolParaSpaceAbove",
	offsetof(ParagraphLayoutPageResources,pprParaSpaceAbove),
	"Space Above" ),
    APP_RESOURCE( "formatToolParaSpaceBelow",
	offsetof(ParagraphLayoutPageResources,pprParaSpaceBelow),
	"Space Below" ),

    /**/
    APP_RESOURCE( "formatToolParaOnNewPage",
	offsetof(ParagraphLayoutPageResources,pprOnNewPage),
	"Start on New Page" ),
    APP_RESOURCE( "formatToolParaOnOnePage",
	offsetof(ParagraphLayoutPageResources,pprOnOnePage),
	"Keep on One Page" ),
    /**/
    APP_RESOURCE( "formatToolParaWidctrl",
	offsetof(ParagraphLayoutPageResources,pprWidctrl),
	"Widow/Orphan Control" ),
    APP_RESOURCE( "formatToolParaKeepWithNext",
	offsetof(ParagraphLayoutPageResources,pprKeepWithNext),
	"Keep with Next" ),
    };

void tedFormatToolGetParaLayoutResourceTable(
					EditApplication *		ea,
					ParagraphLayoutPageResources *	plpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)plpr,
				TED_TedParagraphToolResourceTable,
				sizeof(TED_TedParagraphToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedParagraphSubjectResourceTable,
				sizeof(TED_TedParagraphSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
