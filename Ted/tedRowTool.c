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
/*  Refresh the row page.						*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshRowPage( RowTool *	rt )
    {
    const RowProperties *	rp= &(rt->rtPropertiesChosen);
    const CellProperties *	cp;
    const TableRectangle *	tr= &(rt->rtTableRectangle);
    char			scratch[50];

    EditApplication *		ea= rt->rtApplication;
    EditDocument *		ed= ea->eaCurrentDocument;

    TedDocument *		td;
    BufferDocument *		bd;
    DocumentProperties *	dp;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;
    dp= &(bd->bdProperties);

    appIntegerToTextWidget( rt->rtNumberText, tr->trRow0- tr->trRow00+ 1 );

    tedFormatRefreshHeightChooser( &(rt->rtHeightChooser),
							rp->rpHeightTwips );

    appGeoLengthToString( scratch, rp->rpHalfGapWidthTwips, UNITtyPOINTS );
    appStringToTextWidget( rt->rtCellMarginText, scratch );

    appGeoLengthToString( scratch, rp->rpLeftIndentTwips, UNITtyPOINTS );
    appStringToTextWidget( rt->rtLeftIndentText, scratch );

    cp= rp->rpCells;
    tedBorderToolSetProperties( &(rt->rtTopBorderTool), dp,
					&(cp[tr->trCol0].cpTopBorder) );
    tedBorderToolSetProperties( &(rt->rtBottomBorderTool), dp,
					&(cp[tr->trCol0].cpBottomBorder) );

    appGuiSetToggleState( rt->rtKeepOnPageToggle,
						rp->rpKeepOnOnePage );
    appGuiSetToggleState( rt->rtIsTableHeaderToggle,
						rp->rpIsTableHeader );
    /*
    appGuiSetToggleState( rt->rtAutofitToggle, rp->rpAutofit );
    */
    }

void tedFormatToolRefreshRowTool(
				RowTool *			rt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds )
    {
    TableRectangle *		tr= &(rt->rtTableRectangle);

    const RowProperties *	rp;
    const DocumentGeometry *	dg;

    const BufferItem *		rowBi;
    const BufferItem *		sectBi;

    if  ( docGetTableRectangle( tr, ds )	||
	  tr->trRow1 != tr->trRow0		)
	{ docInitTableRectangle( tr ); *pEnabled= 0; return; }

    rowBi= ds->dsBegin.dpBi;
    rowBi= rowBi->biParent;
    rowBi= rowBi->biParent;
    sectBi= rowBi->biParent;

    dg= &(sectBi->biSectDocumentGeometry);

    rt->rtPageRight= dg->dgPageWideTwips-
			    dg->dgLeftMarginTwips- dg->dgRightMarginTwips;
    rt->rtPageLeftMargin= dg->dgLeftMarginTwips;

    rp= &(rowBi->biRowProperties);

    if  ( docCopyRowProperties( &(rt->rtPropertiesChosen),
						    rp, (const int *)0 ) )
	{ LDEB(1); return;	}
    if  ( docCopyRowProperties( &(rt->rtPropertiesSet),
						    rp, (const int *)0 ) )
	{ LDEB(1); return;	}

    appGuiEnableWidget( is->isPrevButton,
				    tr->trRow0 > tr->trRow00 );
    appGuiEnableWidget( is->isNextButton,
				    tr->trRow1 < tr->trRow11 );

    appGuiEnableWidget( rt->rtIsTableHeaderToggle,
				rowBi->biInExternalItem == DOCinBODY	&&
				tr->trRow0 == tr->trRow00		&&
				tr->trRow1 == tr->trRow00		);

    tedFormatToolRefreshRowPage( rt );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  The 'Change' button of the row tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatChangeRowPushed, w, voidrt )
    {
    RowTool *			rt= (RowTool *)voidrt;
    RowProperties *		rp= &(rt->rtPropertiesChosen);

    TableRectangle		tr;

    int				width;
    int				changed;

    int				res;

    const int			minValue= 0;
    const int			adaptToMin= 0;
    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    BorderProperties		bp;

    PropertyMask		rpSetMask;
    PropertyMask		cpSetMask;
    PropertyMask		bpSetMask;

    /**/
    EditApplication *		ea= rt->rtApplication;
    EditDocument *		ed= ea->eaCurrentDocument;

    TedDocument *		td;
    BufferDocument *		bd;
    DocumentProperties *	dp;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;
    dp= &(bd->bdProperties);

    /**/
    PROPmaskCLEAR( &rpSetMask );
    PROPmaskCLEAR( &cpSetMask );

    PROPmaskADD( &rpSetMask, RPpropKEEP_ON_ONE_PAGE );
    PROPmaskADD( &rpSetMask, RPpropIS_TABLE_HEADER );
    /* No! not yet
    PROPmaskADD( &rpSetMask, RPpropKEEP_WITH_NEXT );
    */

    docInitBorderProperties( &bp );

    if  ( tedFormatToolGetHeight( &(rp->rpHeightTwips),
						&(rt->rtHeightChooser) ) )
	{ goto ready;	}
    PROPmaskADD( &rpSetMask, RPpropHEIGHT );

    width= rp->rpHalfGapWidthTwips;
    if  ( appGetLengthFromTextWidget( rt->rtCellMarginText,
				&width, &changed, UNITtyPOINTS,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	{ goto ready;	}
    rp->rpHalfGapWidthTwips= width;
    PROPmaskADD( &rpSetMask, RPpropGAP_WIDTH );

    res= tedFormatToolGetRowLeftIndent( rp, rt->rtPageLeftMargin, &width,
							rt->rtLeftIndentText );
    if  ( res != 0 )
	{ goto ready;	}
    rp->rpLeftIndentTwips= width;
    PROPmaskADD( &rpSetMask, RPpropLEFT_INDENT );

    tr= rt->rtTableRectangle;
    docExpandTableRectangleToWholeRows( &tr );

    /***/
    PROPmaskCLEAR( &bpSetMask );
    if  ( tedBorderToolGetProperties( &bp, &bpSetMask,
						&(rt->rtTopBorderTool), dp ) )
	{ goto ready;	}

    if  ( ! utilPropMaskIsEmpty( &bpSetMask ) )
	{
	docRowSetTopBorderInCols( rp, tr.trCol0, tr.trCol1, &bpSetMask, &bp );
	PROPmaskADD( &cpSetMask, CLpropTOP_BORDER );
	}

    /***/
    PROPmaskCLEAR( &bpSetMask );
    if  ( tedBorderToolGetProperties( &bp, &bpSetMask,
					    &(rt->rtBottomBorderTool), dp ) )
	{ goto ready;	}

    if  ( ! utilPropMaskIsEmpty( &bpSetMask ) )
	{
	docRowSetBottomBorderInCols( rp, tr.trCol0, tr.trCol1,
							&bpSetMask, &bp );
	PROPmaskADD( &cpSetMask, CLpropBOTTOM_BORDER );
	}

    /***/
    tedAppSetTableProperties( rt->rtApplication, &tr,
					    &rpSetMask, &cpSetMask, rp );

  ready:

    docCleanBorderProperties( &bp );

    return;
    }

/************************************************************************/
/*									*/
/*  The row height changed, check it.					*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedRowHeightChanged, w, voidrt )
    {
    RowTool *		rt= (RowTool *)voidrt;
    int			value;

    if  ( ! tedFormatToolGetHeight( &value, &(rt->rtHeightChooser) ) )
	{
	tedFormatRefreshHeightChooser( &(rt->rtHeightChooser), value );
	}
    }

static APP_TXACTIVATE_CALLBACK_H( tedRowCellMarginChanged, w, voidrt )
    {
    RowTool *		rt= (RowTool *)voidrt;
    RowProperties *	rp= &(rt->rtPropertiesChosen);

    int			width;

    if  ( ! tedFormatToolGetGapWidth( rp, &width, rt->rtCellMarginText ) )
	{
	char	scratch[50];

	appGeoLengthToString( scratch, width, UNITtyPOINTS );
	appStringToTextWidget( rt->rtCellMarginText, scratch );
	}
    }

static APP_TXACTIVATE_CALLBACK_H( tedRowLeftIndentChanged, w, voidrt )
    {
    RowTool *		rt= (RowTool *)voidrt;
    RowProperties *	rp= &(rt->rtPropertiesChosen);

    int			value;

    if  ( ! tedFormatToolGetRowLeftIndent( rp, rt->rtPageLeftMargin, &value,
						    rt->rtLeftIndentText ) )
	{
	char	scratch[50];

	appGeoLengthToString( scratch, value, UNITtyPOINTS );
	appStringToTextWidget( rt->rtLeftIndentText, scratch );
	}
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button of the row tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatRevertRowPushed, w, voidrt )
    {
    RowTool *	rt= (RowTool *)voidrt;

    docCopyRowProperties( &(rt->rtPropertiesChosen),
				&(rt->rtPropertiesSet), (const int *)0 );

    tedFormatToolRefreshRowPage( rt );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatPreviousRow, w, voidrt )
    {
    RowTool *		rt= (RowTool *)voidrt;
    EditApplication *	ea= rt->rtApplication;
    EditDocument *	ed= ea->eaCurrentDocument;
    TedDocument *	td;

    TableRectangle	tr;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tr= rt->rtTableRectangle;
    td= (TedDocument *)ed->edPrivateData;

    docExpandTableRectangleToWholeRows( &tr );

    if  ( docShiftTableRectangleByRows( &tr, -1 ) )
	{ return;	}

    tedAppSetTableSelection( ed, &tr );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatSelectRow, w, voidrt )
    {
    RowTool *		rt= (RowTool *)voidrt;
    EditApplication *	ea= rt->rtApplication;
    EditDocument *	ed= ea->eaCurrentDocument;
    TedDocument *	td;

    TableRectangle	tr;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tr= rt->rtTableRectangle;
    td= (TedDocument *)ed->edPrivateData;

    docExpandTableRectangleToWholeRows( &tr );

    tedAppSetTableSelection( ed, &tr );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatNextRow, w, voidrt )
    {
    RowTool *		rt= (RowTool *)voidrt;
    EditApplication *	ea= rt->rtApplication;
    EditDocument *	ed= ea->eaCurrentDocument;
    TedDocument *	td;

    TableRectangle	tr;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tr= rt->rtTableRectangle;
    td= (TedDocument *)ed->edPrivateData;

    docExpandTableRectangleToWholeRows( &tr );

    if  ( docShiftTableRectangleByRows( &tr, +1 ) )
	{ return;	}

    tedAppSetTableSelection( ed, &tr );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Delete' button of the row tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatDeleteRow, w, voidrt )
    {
    RowTool *		rt= (RowTool *)voidrt;
    EditApplication *	ea= rt->rtApplication;
    EditDocument *	ed= ea->eaCurrentDocument;
    TableRectangle *	tr= &(rt->rtTableRectangle);

    if  ( ! ed )
	{ XDEB(ed); return;	}

    if  ( tedDeleteRowsFromTable( ed, tr->trRow0, tr->trRow1 ) )
	{ LLDEB(tr->trRow0,tr->trRow1); return;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Insert/Append Row buttons have been pushed.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatInsertRow, w, voidrt )
    {
    RowTool *		rt= (RowTool *)voidrt;
    EditApplication *	ea= rt->rtApplication;

    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedInsertRowInTable( ed );

    appDocumentChanged( ed, 1 );
    }

static APP_BUTTON_CALLBACK_H( tedFormatAppendRow, w, voidrt )
    {
    RowTool *		rt= (RowTool *)voidrt;
    EditApplication *	ea= rt->rtApplication;
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedAppendRowToTable( ed );

    appDocumentChanged( ed, 1 );
    }

static APP_OITEM_CALLBACK_H( tedRowHeightChosen, w, voidrt )
    {
    RowTool *		rt= (RowTool *)voidrt;
    HeightChooser *	hc= &(rt->rtHeightChooser);
    int			how;

    int			defaultValue;

    defaultValue= rt->rtPropertiesChosen.rpHeightTwips;
    if  ( defaultValue == 0 )
	{ defaultValue= rt->rtPropertiesSet.rpHeightTwips;	}
    if  ( defaultValue == 0 )
	{ defaultValue= 240;	}

    how= appGuiGetOptionmenuItemIndex( &(hc->hcOptionmenu), w );
    if  ( how < 0 || how >= HC__COUNT )
	{ LDEB(how); return;	}

    tedFormatHeightChosen( how, hc, defaultValue );

    return;
    }

/************************************************************************/
/*									*/
/*  A toggle was activated.						*/
/*									*/
/************************************************************************/

static APP_TOGGLE_CALLBACK_H( tedRowToggleKeepOnPage, w, voidrt, voidtbcs )
    {
    RowTool *				rt= (RowTool *)voidrt;
    RowProperties *			rp= &(rt->rtPropertiesChosen);
    int					set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    rp->rpKeepOnOnePage= set;

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedRowToggleIsTableHeader, w, voidrt, voidtbcs )
    {
    RowTool *				rt= (RowTool *)voidrt;
    RowProperties *			rp= &(rt->rtPropertiesChosen);
    int					set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    rp->rpIsTableHeader= set;

    return;
    }

/************************************************************************/
/*									*/
/*  Callback from the general color chooser: a color was chosen.	*/
/*									*/
/************************************************************************/

static void tedRowToolGotColor(		void *			voidrt,
					int 			which,
					const RGB8Color *	rgb8 )
    {
    RowTool *			rt= (RowTool *)voidrt;

    switch( which )
	{
	case CLpropTOP_BORDER:
	    tedBorderSetExplicitColorChoice( &(rt->rtTopBorderTool), rgb8 );
	    break;

	case CLpropBOTTOM_BORDER:
	    tedBorderSetExplicitColorChoice( &(rt->rtBottomBorderTool), rgb8 );
	    break;

	default:
	    LDEB(which); return;
	}
    }

/************************************************************************/
/*									*/
/*  Make a row tool. I.E. the 'Row' page of the format tool.		*/
/*									*/
/************************************************************************/

void tedFormatFillRowPage(  RowTool *				rt,
			    const RowPageResources *		rpr,
			    AppInspector *			ai,
			    int					subjectPage,
			    InspectorSubject *			is,
			    APP_WIDGET				pageWidget,
			    const InspectorSubjectResources *	isr )
    {
    APP_WIDGET	rowLabel;
    APP_WIDGET	widthLabel;
    APP_WIDGET	leftLabel;

    APP_WIDGET	row;

    const int	textColumns= 10;

    /**/
    is->isPrivate= rt;
    is->isGotColor= tedRowToolGotColor;

    rt->rtPageResources= rpr;

    /**************/
    appMakeLabelAndTextRow( &row, &rowLabel, &(rt->rtNumberText),
			    pageWidget, isr->isrSubjectName, textColumns, 0 );

    /**************/
    tedFormatMakeHeightRow( &row, rt, pageWidget,
			&(rt->rtHeightChooser), tedRowHeightChanged );

    /**************/
    appMakeLabelAndTextRow( &row, &leftLabel, &(rt->rtLeftIndentText),
			    pageWidget, rpr->rprLeftIndent, textColumns, 1 );

    appGuiSetGotValueCallbackForText( rt->rtLeftIndentText,
				tedRowLeftIndentChanged, (void *)rt );

    /**************/
    appMakeLabelAndTextRow( &row, &widthLabel, &(rt->rtCellMarginText),
			    pageWidget, rpr->rprCellMargin, textColumns, 1 );

    appGuiSetGotValueCallbackForText( rt->rtCellMarginText,
				tedRowCellMarginChanged, (void *)rt );

    /**************/
    tedMakeBorderTool( &(rt->rtTopBorderTool), ai, pageWidget,
	    rpr->rprTopBorder, &(rpr->rprBorderToolResources),
	    subjectPage, CLpropTOP_BORDER );

    tedMakeBorderTool( &(rt->rtBottomBorderTool), ai, pageWidget,
	    rpr->rprBottomBorder, &(rpr->rprBorderToolResources),
	    subjectPage, CLpropBOTTOM_BORDER );

    /**************/
    appInspectorMakeToggleRow( &row, pageWidget,
	    &(rt->rtKeepOnPageToggle), &(rt->rtIsTableHeaderToggle),
	    rpr->rprKeepOnPage, rpr->rprIsTableHeader,
	    tedRowToggleKeepOnPage, tedRowToggleIsTableHeader, (void *)rt );

    appInspectorMakeButtonRow( &row, pageWidget,
	    &(is->isPrevButton), &(is->isNextButton),
	    isr->isrPrevButtonText, isr->isrNextButtonText,
	    tedFormatPreviousRow, tedFormatNextRow, (void *)rt );

    appInspectorMakeButtonRow( &row, pageWidget,
	    &(is->isSelectButton), &(is->isDeleteButton),
	    isr->isrSelectButtonText, isr->isrDeleteButtonText,
	    tedFormatSelectRow, tedFormatDeleteRow, (void *)rt );

    appInspectorMakeButtonRow( &row, pageWidget,
	    &(is->isInsertButton), &(is->isAppendButton),
	    isr->isrInsertButtonText, isr->isrAppendButtonText,
	    tedFormatInsertRow, tedFormatAppendRow, (void *)rt );

    appInspectorMakeButtonRow( &row, pageWidget,
	    &(is->isRevertButton), &(is->isApplyButton),
	    isr->isrRevert, isr->isrApplyToSubject,
	    tedFormatRevertRowPushed, tedFormatChangeRowPushed, (void *)rt );

    return;
    }

void tedFormatFillRowChoosers(		RowTool *			rt,
					const RowPageResources *	rpr )
    {
    tedFormatFillHeightChooser( &(rt->rtHeightChooser),
					    tedRowHeightChosen, (void *)rt,
					    rpr->rprHeightFree,
					    rpr->rprHeightAtLeast,
					    rpr->rprHeightExactly );
    }

void tedFormatFinishRowPage( 	RowTool *			rt,
				TedFormatTool *			tft,
				const RowPageResources *	rpr )
    {
    appOptionmenuRefreshWidth( &(rt->rtHeightChooser.hcOptionmenu) );

    tedFinishBorderTool( &(rt->rtTopBorderTool) );
    tedFinishBorderTool( &(rt->rtBottomBorderTool) );
    }

/************************************************************************/
/*									*/
/*  Clean a row tool.							*/
/*									*/
/************************************************************************/

void tedInitRowTool(	RowTool *	rt )
    {
    rt->rtApplication= (EditApplication *)0;
    rt->rtInspector= (AppInspector *)0;
    rt->rtPageResources= (const RowPageResources *)0;

    docInitRowProperties( &(rt->rtPropertiesSet) );
    docInitRowProperties( &(rt->rtPropertiesChosen) );

    docInitTableRectangle( &(rt->rtTableRectangle ) );

    rt->rtPageRight= 0;
    rt->rtPageLeftMargin= 0;

    rt->rtNumberText= (APP_WIDGET)0;
    tedInitHeightChooser( &(rt->rtHeightChooser) );
    rt->rtCellMarginText= (APP_WIDGET)0;
    rt->rtLeftIndentText= (APP_WIDGET)0;

    tedInitBorderTool( &(rt->rtTopBorderTool) );
    tedInitBorderTool( &(rt->rtBottomBorderTool) );

    rt->rtKeepOnPageToggle= (APP_WIDGET)0;
    rt->rtIsTableHeaderToggle= (APP_WIDGET)0;

    return;
    }

void tedCleanRowTool(	RowTool *	rt )
    {
    docCleanRowProperties( &(rt->rtPropertiesSet) );
    docCleanRowProperties( &(rt->rtPropertiesChosen) );

    tedCleanBorderTool( &(rt->rtTopBorderTool) );
    tedCleanBorderTool( &(rt->rtBottomBorderTool) );

    return;
    }

/************************************************************************/
/*									*/
/*  Get row tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedRowSubjectResourceTable[]=
    {
    APP_RESOURCE( "tableToolRow",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Row" ),
    APP_RESOURCE( "tableToolChangeRow",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Row" ),
    APP_RESOURCE( "tableToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    APP_RESOURCE( "tableToolNextRow",
		offsetof(InspectorSubjectResources,isrNextButtonText),
		"Next Row" ),
    APP_RESOURCE( "tableToolPreviousRow",
		offsetof(InspectorSubjectResources,isrPrevButtonText),
		"Previous Row" ),
    APP_RESOURCE( "tableToolSelectRow",
		offsetof(InspectorSubjectResources,isrSelectButtonText),
		"Select Row" ),
    APP_RESOURCE( "tableToolDeleteRow",
		offsetof(InspectorSubjectResources,isrDeleteButtonText),
		"Delete Row" ),
    APP_RESOURCE( "tableToolInsertRow",
		offsetof(InspectorSubjectResources,isrInsertButtonText),
		"Insert Row" ),
    APP_RESOURCE( "tableToolAppendRow",
		offsetof(InspectorSubjectResources,isrAppendButtonText),
		"Append Row" ),
    };

static AppConfigurableResource TED_TedRowToolResourceTable[]=
    {
    APP_RESOURCE( "tableToolHeightFree",
	    offsetof(RowPageResources,rprHeightFree),
	    "Height Free" ),
    APP_RESOURCE( "tableToolHeightExactly",
	    offsetof(RowPageResources,rprHeightExactly),
	    "Height Exactly" ),
    APP_RESOURCE( "tableToolHeightAtLeast",
	    offsetof(RowPageResources,rprHeightAtLeast),
	    "Height at Least" ),
    /**/
    APP_RESOURCE( "tableToolCellMargin",
	    offsetof(RowPageResources,rprCellMargin),
	    "Cell Margin" ),
    APP_RESOURCE( "tableToolLeftIndent",
	    offsetof(RowPageResources,rprLeftIndent),
	    "Left Margin" ),
    /**/
    APP_RESOURCE( "tableToolRowTopBorder",
	    offsetof(RowPageResources,rprTopBorder),
	    "Top Border" ),
    APP_RESOURCE( "tableToolRowBottomBorder",
	    offsetof(RowPageResources,rprBottomBorder),
	    "Bottom Border" ),

    APP_RESOURCE( "formatToolRowBorderColorChooserAutomatic",
	    offsetof(RowPageResources,rprBorderToolResources.
			    btrColorChooserResources.ccrAutomaticColor),
	    "Automatic" ),
    APP_RESOURCE( "formatToolRowBorderColorChooserMoreColors",
	    offsetof(RowPageResources,rprBorderToolResources.
			    btrColorChooserResources.ccrMoreColors),
	    "More Colors..." ),

    /**/
    APP_RESOURCE( "formatToolRowKeepOnPage",
	    offsetof(RowPageResources,rprKeepOnPage),
	    "Keep on One Page" ),
    APP_RESOURCE( "formatToolRowIsTableHeader",
	    offsetof(RowPageResources,rprIsTableHeader),
	    "Is Table Header" ),

    /**/
    APP_RESOURCE( "formatToolRowBorderWidth",
	    offsetof(RowPageResources, rprBorderToolResources.btrWidth),
	    "Width" ),
    APP_RESOURCE( "formatToolRowBorderStyle",
	    offsetof(RowPageResources, rprBorderToolResources.btrStyle),
	    "Style" ),
    APP_RESOURCE( "formatToolRowBorderColor",
	    offsetof(RowPageResources, rprBorderToolResources.btrColor),
	    "Color" ),
    };

void tedFormatToolGetRowResourceTable(	EditApplication *		ea,
					RowPageResources *		rpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)rpr,
				TED_TedRowToolResourceTable,
				sizeof(TED_TedRowToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedRowSubjectResourceTable,
				sizeof(TED_TedRowSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
