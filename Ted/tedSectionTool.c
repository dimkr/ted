/************************************************************************/
/*									*/
/*  The Row page of the Format Tool.					*/
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
/*  Refresh the section page.						*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshPageRestart(	SectionTool *	st )
    {
    SectionProperties *		sp= &(st->stPropertiesChosen);

    if  ( sp->spRestartPageNumbers )
	{
	appIntegerToTextWidget( st->stPageRestartText,
						sp->spStartPageNumber+ 1 );
	appEnableText( st->stPageRestartText, 1 );

	appGuiSetToggleState( st->stPageRestartToggle, 1 );
	}
    else{
	appStringToTextWidget( st->stPageRestartText, "" );
	appEnableText( st->stPageRestartText, 0 );

	appGuiSetToggleState( st->stPageRestartToggle, 0 );
	}
    }

static void tedFormatToolRefreshSectionPage(	SectionTool *	st )
    {
    char	scratch[50];

    SectionProperties *		sp= &(st->stPropertiesChosen);

    appIntegerToTextWidget( st->stNumberText, st->stSectionNumber+ 1 );

    appSetOptionmenu( &(st->stNumberStyleOptionmenu), sp->spPageNumberStyle );

    appSetOptionmenu( &(st->stBreakKindOptionmenu), sp->spBreakKind );

    tedFormatToolRefreshPageRestart( st );

    appIntegerToTextWidget( st->stColumnsText, sp->spColumnCount );

    if  ( sp->spColumnCount > 1 )
	{
	appGeoLengthToString( scratch, sp->spColumnSpacingTwips, UNITtyPOINTS );
	appStringToTextWidget( st->stColumnSpacingText, scratch );

	appGeoLengthToString( scratch, sp->spColumnWidthTwips, UNITtyPOINTS );
	appStringToTextWidget( st->stColumnWidthText, scratch );
	}
    else{
	appStringToTextWidget( st->stColumnSpacingText, "" );
	appStringToTextWidget( st->stColumnWidthText, "" );
	}

    return;
    }

void tedFormatToolRefreshSectionTool(
				SectionTool *			st,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds )
    {
    BufferItem *		sectBi;

    sectBi= ds->dsBegin.dpBi;
    while( sectBi && sectBi->biLevel > DOClevSECT )
	{ sectBi= sectBi->biParent;	}

    if  ( ! sectBi )
	{ XDEB(sectBi); return;	}

    st->stSectionNumber= sectBi->biNumberInParent;

    if  ( docCopySectionProperties( &(st->stPropertiesChosen),
					    &(sectBi->biSectProperties) ))
	{ LDEB(1); return ;	}
    if  ( docCopySectionProperties( &(st->stPropertiesSet),
					    &(sectBi->biSectProperties) ) )
	{ LDEB(1); return ;	}

    appGuiEnableWidget( is->isPrevButton,
			docPrevSection( sectBi ) != (BufferItem *)0 );
    appGuiEnableWidget( is->isNextButton,
			docNextSection( sectBi ) != (BufferItem *)0 );

    tedFormatToolRefreshSectionPage( st );

    *pEnabled= ds->dsSelectionScope.ssInExternalItem == DOCinBODY;
    return;
    }

/************************************************************************/
/*									*/
/*  The 'Change' button of the row tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatChangeSectPushed, w, voidst )
    {
    SectionTool *		st= (SectionTool *)voidst;
    SectionProperties *		spNew= &(st->stPropertiesChosen);

    int				changed;

    PropertyMask		updMask;

    const int			minValue= 1;
    const int			adaptToMin= 0;
    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    spNew->spRestartPageNumbers=
		appGuiGetToggleState( st->stPageRestartToggle ) != 0;

    if  ( spNew->spRestartPageNumbers )
	{
	int			n;

	if  ( appGetIntegerFromTextWidget( st->stPageRestartText, &n,
							1, 0, INT_MAX, 0 ) )
	    { return;	}

	spNew->spStartPageNumber= n- 1;
	}

    if  ( appGetIntegerFromTextWidget( st->stColumnsText,
						    &(spNew->spColumnCount),
						    1, 0, INT_MAX, 0 ) )
	{ return;	}

    if  ( spNew->spColumnCount > 1 )
	{
	if  ( appGetLengthFromTextWidget( st->stColumnSpacingText,
		    &(spNew->spColumnSpacingTwips), &changed, UNITtyPOINTS,
		    minValue, adaptToMin, maxValue, adaptToMax ) )
	    { return;	}

	if  ( appGetLengthFromTextWidget( st->stColumnWidthText,
		    &(spNew->spColumnWidthTwips), &changed, UNITtyPOINTS,
		    minValue, adaptToMin, maxValue, adaptToMax ) )
	    { return;	}
	}

    PROPmaskCLEAR( &updMask );
    utilPropMaskFill( &updMask, SPprop_COUNT );

    if  ( tedAppChangeSectionProperties( st->stApplication, &updMask, spNew ) )
	{ LDEB(1);	}

    return;
    }

static APP_OITEM_CALLBACK_H( tedSectNumberStyleChosen, w, voidst )
    {
    SectionTool *			st= (SectionTool *)voidst;
    SectionProperties *			sp= &(st->stPropertiesChosen);

    int					style;

    style= appGuiGetOptionmenuItemIndex( &(st->stNumberStyleOptionmenu), w );
    if  ( style < 0 || style >= DOCpgn_COUNT )
	{ LLDEB(style,DOCpgn_COUNT); return;	}

    sp->spPageNumberStyle= style;

    return;
    }

static APP_OITEM_CALLBACK_H( tedSectBreakKindChosen, w, voidst )
    {
    SectionTool *			st= (SectionTool *)voidst;
    SectionProperties *			sp= &(st->stPropertiesChosen);

    int					breakKind;

    breakKind= appGuiGetOptionmenuItemIndex( &(st->stBreakKindOptionmenu), w );
    if  ( breakKind < 0 || breakKind >= DOCsbk_COUNT )
	{ LLDEB(breakKind,DOCsbk_COUNT); return;	}

    sp->spBreakKind= breakKind;

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedFormatPageRestartToggled, w, voidst, voidtbcs )
    {
    SectionTool *		st= (SectionTool *)voidst;
    SectionProperties *		sp= &(st->stPropertiesChosen);
    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    sp->spRestartPageNumbers= ( set != 0 );

    tedFormatToolRefreshPageRestart( st );

    return;
    }

/************************************************************************/
/*									*/
/*  Check user input.							*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedSectColSpacingChanged, w, voidst )
    {
    SectionTool *		st= (SectionTool *)voidst;
    SectionProperties *		sp= &(st->stPropertiesChosen);

    int				value;
    int				changed;

    value= sp->spColumnSpacingTwips;

    tedFormatValidateDimension( &value, &changed,
					    st->stColumnSpacingText, value );

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedSectColWidthChanged, w, voidst )
    {
    SectionTool *		st= (SectionTool *)voidst;
    SectionProperties *		sp= &(st->stPropertiesChosen);

    int				value;
    int				changed;

    value= sp->spColumnWidthTwips;

    tedFormatValidateDimension( &value, &changed,
					    st->stColumnWidthText, value );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button of the row tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatRevertSectPushed, w, voidst )
    {
    SectionTool *	st= (SectionTool *)voidst;

    docCopySectionProperties( &(st->stPropertiesChosen),
						    &(st->stPropertiesSet) );

    tedFormatToolRefreshSectionPage( st );

    return;
    }

/************************************************************************/
/*									*/
/*  Previous/Next Section buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatPrevSect, w, voidst )
    {
    SectionTool *		st= (SectionTool *)voidst;
    EditApplication *		ea= st->stApplication;

    tedSelectWholeSection( ea, -1 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatNextSect, w, voidst )
    {
    SectionTool *		st= (SectionTool *)voidst;
    EditApplication *		ea= st->stApplication;

    tedSelectWholeSection( ea, 1 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatSelectSect, w, voidst )
    {
    SectionTool *		st= (SectionTool *)voidst;
    EditApplication *		ea= st->stApplication;

    tedSelectWholeSection( ea, 0 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatDeleteSect, w, voidst )
    {
    SectionTool *		st= (SectionTool *)voidst;
    EditApplication *		ea= st->stApplication;

    tedDeleteCurrentSection( ea );
    }

static APP_BUTTON_CALLBACK_H( tedFormatInsertSect, w, voidst )
    {
    SectionTool *		st= (SectionTool *)voidst;
    EditApplication *		ea= st->stApplication;

    tedInsertSection( ea, 0 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatAppendSect, w, voidst )
    {
    SectionTool *		st= (SectionTool *)voidst;
    EditApplication *		ea= st->stApplication;

    tedInsertSection( ea, 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a section tool. I.E. the 'Section' page of the format tool.	*/
/*									*/
/************************************************************************/

void tedFormatFillSectionPage(	SectionTool *			st,
				const SectionPageResources *	spr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    APP_WIDGET	rowLabel;

    APP_WIDGET	row;

    const int	textColumns= 10;

    /**/

    st->stUnitType= appUnitTypeInt( spr->sprUnitType );
    if  ( st->stUnitType < 0 )
	{ SDEB(spr->sprUnitType); st->stUnitType= UNITtyINCH;	}

    /**/
    st->stPageResources= spr;

    /**/
    st->stSectionNumber= -1;

    docInitSectionProperties( &(st->stPropertiesSet) );
    docInitSectionProperties( &(st->stPropertiesChosen) );

    /**************/
    appMakeLabelAndTextRow( &row, &rowLabel, &(st->stNumberText),
			    pageWidget, isr->isrSubjectName, textColumns, 0 );

    /**************/
    appInspectorMakeMenuRow( &row,
			    &(st->stBreakKindOptionmenu),
			    &rowLabel, pageWidget, spr->sprBreakKind );
    /**************/
    appInspectorMakeMenuRow( &row,
			    &(st->stNumberStyleOptionmenu),
			    &rowLabel, pageWidget, spr->sprPageNumberStyle );
    /**************/
    appMakeToggleAndTextRow( &row, &(st->stPageRestartToggle),
			    &(st->stPageRestartText), pageWidget,
			    spr->sprNumberingRestarts,
			    tedFormatPageRestartToggled, (void *)st,
			    textColumns, 1 );

    /**************/
    appMakeLabelAndTextRow( &row, &rowLabel, &(st->stColumnsText),
			    pageWidget, spr->sprColumnCount, textColumns, 0 );

    appMakeLabelAndTextRow( &row, &rowLabel, &(st->stColumnSpacingText),
			    pageWidget, spr->sprColumnSpacing, textColumns, 0 );

    appMakeLabelAndTextRow( &row, &rowLabel, &(st->stColumnWidthText),
			    pageWidget, spr->sprColumnWidth, textColumns, 0 );

    appGuiSetGotValueCallbackForText( st->stColumnSpacingText,
					tedSectColSpacingChanged, (void *)st );

    appGuiSetGotValueCallbackForText( st->stColumnWidthText,
					tedSectColWidthChanged, (void *)st );

    /**************/
    appInspectorMakeButtonRow( &row, pageWidget,
	    &(is->isPrevButton), &(is->isNextButton),
	    isr->isrPrevButtonText, isr->isrNextButtonText,
	    tedFormatPrevSect, tedFormatNextSect, (void *)st );

    appInspectorMakeButtonRow( &row, pageWidget,
	    &(is->isSelectButton), &(is->isDeleteButton),
	    isr->isrSelectButtonText, isr->isrDeleteButtonText,
	    tedFormatSelectSect, tedFormatDeleteSect, (void *)st );

    appInspectorMakeButtonRow( &row, pageWidget,
	    &(is->isInsertButton), &(is->isAppendButton),
	    isr->isrInsertButtonText, isr->isrAppendButtonText,
	    tedFormatInsertSect, tedFormatAppendSect, (void *)st );

    appInspectorMakeButtonRow( &row, pageWidget,
	    &(is->isRevertButton), &(is->isApplyButton),
	    isr->isrRevert, isr->isrApplyToSubject,
	    tedFormatRevertSectPushed, tedFormatChangeSectPushed, (void *)st );

    return;
    }

/************************************************************************/
/*									*/
/*  Finish the section page.						*/
/*									*/
/************************************************************************/

void tedFormatFillSectionChoosers(	SectionTool *			st,
					const SectionPageResources *	spr )
    {
    appFillInspectorMenu( DOCpgn_COUNT, DOCpgnDEC,
			st->stNumberStyleItems, spr->sprNumberStyleMenuTexts,
			&(st->stNumberStyleOptionmenu),
			tedSectNumberStyleChosen, (void *)st );

    appFillInspectorMenu( DOCsbk_COUNT, DOCsbkPAGE,
			st->stBreakKindItems, spr->sprBreakKindMenuTexts,
			&(st->stBreakKindOptionmenu),
			tedSectBreakKindChosen, (void *)st );

    return;
    }

void tedFormatFinishSectionPage(	SectionTool *			st,
					TedFormatTool *			tft,
					const SectionPageResources *	spr )
    {
    appOptionmenuRefreshWidth( &(st->stNumberStyleOptionmenu) );

    appOptionmenuRefreshWidth( &(st->stBreakKindOptionmenu) );

    return;
    }

/************************************************************************/
/*									*/
/*  Clean a section tool						*/
/*									*/
/************************************************************************/

void tedFormatCleanSectionTool(		SectionTool *		st )
    {
    docCleanSectionProperties( &(st->stPropertiesSet) );
    docCleanSectionProperties( &(st->stPropertiesChosen) );

    return;
    }

/************************************************************************/
/*									*/
/*  Get section tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedSectionSubjectResourceTable[]=
    {
    APP_RESOURCE( "formatToolSect",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Section" ),
    APP_RESOURCE( "formatToolChangeSect",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Section" ),
    APP_RESOURCE( "tableToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    APP_RESOURCE( "formatToolNextSect",
		offsetof(InspectorSubjectResources,isrNextButtonText),
		"Next Section" ),
    APP_RESOURCE( "formatToolPreviousSect",
		offsetof(InspectorSubjectResources,isrPrevButtonText),
		"Previous Section" ),
    APP_RESOURCE( "formatToolSelectSect",
		offsetof(InspectorSubjectResources,isrSelectButtonText),
		"Select Section" ),
    APP_RESOURCE( "formatToolDeleteSect",
		offsetof(InspectorSubjectResources,isrDeleteButtonText),
		"Delete Section" ),
    APP_RESOURCE( "formatToolInsertSect",
		offsetof(InspectorSubjectResources,isrInsertButtonText),
		"Insert Section" ),
    APP_RESOURCE( "formatToolAppendSect",
		offsetof(InspectorSubjectResources,isrAppendButtonText),
		"Append Section" ),
    };

static AppConfigurableResource TED_TedSectionToolResourceTable[]=
    {
    /**/
    APP_RESOURCE( "unit",
		offsetof(SectionPageResources,sprUnitType),
		"inch" ),

    APP_RESOURCE( "formatToolSectNumberRestart",
		offsetof(SectionPageResources,sprNumberingRestarts),
		"Start with Page" ),
    /**/
    APP_RESOURCE( "formatToolSectPageNumberStyle",
	    offsetof(SectionPageResources,sprPageNumberStyle),
	    "Page Number Style" ),
    APP_RESOURCE( "formatToolSectNumberStyleDec",
	    offsetof(SectionPageResources,sprNumberStyleMenuTexts[DOCpgnDEC]),
	    "1, 2, 3" ),
    APP_RESOURCE( "formatToolSectNumberStyleUcrm",
	    offsetof(SectionPageResources,sprNumberStyleMenuTexts[DOCpgnUCRM]),
	    "I, II, III" ),
    APP_RESOURCE( "formatToolSectNumberStyleLcrm",
	    offsetof(SectionPageResources,sprNumberStyleMenuTexts[DOCpgnLCRM]),
	    "i, ii, iii" ),
    APP_RESOURCE( "formatToolSectNumberStyleUcltr",
	    offsetof(SectionPageResources,sprNumberStyleMenuTexts[DOCpgnUCLTR]),
	    "A, B, C" ),
    APP_RESOURCE( "formatToolSectNumberStyleLcltr",
	    offsetof(SectionPageResources,sprNumberStyleMenuTexts[DOCpgnLCLTR]),
	    "a, b, c" ),
    /**/
    APP_RESOURCE( "formatToolSectBreakKind",
	    offsetof(SectionPageResources,sprBreakKind),
	    "Begins" ),
    APP_RESOURCE( "formatToolSectBreakKindNone",
	    offsetof(SectionPageResources,sprBreakKindMenuTexts[DOCsbkNONE]),
	    "Below Previous" ),
    APP_RESOURCE( "formatToolSectBreakKindCol",
	    offsetof(SectionPageResources,sprBreakKindMenuTexts[DOCsbkCOL]),
	    "In Next Column" ),
    APP_RESOURCE( "formatToolSectBreakKindPage",
	    offsetof(SectionPageResources,sprBreakKindMenuTexts[DOCsbkPAGE]),
	    "On New Page" ),
    APP_RESOURCE( "formatToolSectBreakKindEven",
	    offsetof(SectionPageResources,sprBreakKindMenuTexts[DOCsbkEVEN]),
	    "On Even Page" ),
    APP_RESOURCE( "formatToolSectBreakKindOdd",
	    offsetof(SectionPageResources,sprBreakKindMenuTexts[DOCsbkODD]),
	    "On Odd Page" ),
    /**/
    APP_RESOURCE( "formatToolSectColumnCount",
	    offsetof(SectionPageResources,sprColumnCount),
	    "Columns" ),
    APP_RESOURCE( "formatToolSectColumnSpacing",
	    offsetof(SectionPageResources,sprColumnSpacing),
	    "Column Spacing" ),
    APP_RESOURCE( "formatToolSectColumnWidth",
	    offsetof(SectionPageResources,sprColumnWidth),
	    "Column Width" ),
    };

void tedFormatToolGetSectResourceTable(	EditApplication *		ea,
					SectionPageResources *		spr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)spr,
				TED_TedSectionToolResourceTable,
				sizeof(TED_TedSectionToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedSectionSubjectResourceTable,
				sizeof(TED_TedSectionSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
