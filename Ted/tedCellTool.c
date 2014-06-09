/************************************************************************/
/*									*/
/*  The 'Cell' page of the 'Ted' format tool.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>

#   include	<appGeoString.h>
#   include	<appUnit.h>

#   include	"tedApp.h"
#   include	"tedFormatTool.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Refresh the 'Cell' page of the format tool.				*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshCellPage(	CellTool *	ct )
    {
    const RowProperties *	rp= &(ct->ctRowPropertiesChosen);
    const TableRectangle *	tr= &(ct->ctTableRectangle);
    const CellProperties *	cp= &(rp->rpCells[tr->trCol0]);

    EditApplication *		ea= ct->ctApplication;
    EditDocument *		ed= ea->eaCurrentDocument;

    TedDocument *		td;
    BufferDocument *		bd;
    DocumentProperties *	dp;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;
    dp= &(bd->bdProperties);

    appIntegerToTextWidget( ct->ctRowText, tr->trRow0- tr->trRow00+ 1 );
    appIntegerToTextWidget( ct->ctColumnText, tr->trCol0+ 1 );

    if  ( tr->trCol0 == tr->trCol1	&&
	  tr->trRow0 == tr->trRow1	&&
	  ! rp->rpIsTableHeader		)
	{
	appEnableText( ct->ctColspanText, 1 );
	appEnableText( ct->ctRowspanText, 1 );

	appIntegerToTextWidget( ct->ctColspanText, tr->trCellColspan );
	appIntegerToTextWidget( ct->ctRowspanText, tr->trCellRowspan );
	}
    else{
	appEnableText( ct->ctColspanText, 0 );
	appEnableText( ct->ctRowspanText, 0 );

	appStringToTextWidget( ct->ctColspanText, "" );
	appStringToTextWidget( ct->ctRowspanText, "" );
	}

    tedBorderToolSetProperties( &(ct->ctTopBorderTool), dp,
						&(cp->cpTopBorder) );
    tedBorderToolSetProperties( &(ct->ctBottomBorderTool), dp,
						&(cp->cpBottomBorder) );
    tedBorderToolSetProperties( &(ct->ctLeftBorderTool), dp,
						&(cp->cpLeftBorder) );
    tedBorderToolSetProperties( &(ct->ctRightBorderTool), dp,
						&(cp->cpRightBorder) );

    tedSetShadingTool( &(ct->ctShadingTool), dp, &(cp->cpShading) );
    }

void tedFormatToolRefreshCellTool(
				CellTool *			ct,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds )
    {
    TableRectangle *		tr= &(ct->ctTableRectangle);

    const RowProperties *	rp;

    const BufferItem *		rowBi;
    const BufferItem *		cellBi;

    if  ( docGetTableRectangle( tr, ds )	||
	  tr->trRow1 != tr->trRow0		||
	  tr->trCol1 != tr->trCol0		)
	{ docInitTableRectangle( tr ); *pEnabled= 0; return; }

    cellBi= ds->dsBegin.dpBi;
    cellBi= cellBi->biParent;
    rowBi= cellBi->biParent;

    rp= &(rowBi->biRowProperties);

    if  ( docCopyRowProperties( &(ct->ctRowPropertiesChosen),
						    rp, (const int *)0 ) )
	{ LDEB(1); return;	}
    if  ( docCopyRowProperties( &(ct->ctRowPropertiesSet),
						    rp, (const int *)0 ) )
	{ LDEB(1); return;	}

    appGuiEnableWidget( is->isPrevButton, tr->trCol0 > 0 );
    appGuiEnableWidget( is->isNextButton, tr->trCol1 < tr->trCol11 );

    tedFormatToolRefreshCellPage( ct );

    *pEnabled= 1;
    return;
    }


/************************************************************************/
/*									*/
/*  The user typed 'Enter' in the rowspan/colspan text widgets.		*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedCellToolRowspanChanged, w, voidct )
    {
    CellTool *			ct= (CellTool *)voidct;
    const TableRectangle *	tr= &(ct->ctTableRectangle);

    int				minValue= 1;
    const int			adaptToMin= 0;
    int				maxValue;
    const int			adaptToMax= 1;

    int				rowspan= tr->trCellRowspan;

    maxValue= tr->trRow11- tr->trRow0+ 1;

    if  ( ! appGetIntegerFromTextWidget( ct->ctRowspanText, &rowspan,
			    minValue, adaptToMin, maxValue, adaptToMax ) )
	{ appIntegerToTextWidget( ct->ctRowspanText, rowspan );	}

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedCellToolColspanChanged, w, voidct )
    {
    CellTool *			ct= (CellTool *)voidct;
    const TableRectangle *	tr= &(ct->ctTableRectangle);

    int				minValue= 1;
    const int			adaptToMin= 0;
    int				maxValue;
    const int			adaptToMax= 1;

    int				colspan= tr->trCellColspan;

    maxValue= tr->trCol11- tr->trCol0+ 1;

    if  ( ! appGetIntegerFromTextWidget( ct->ctColspanText, &colspan,
			    minValue, adaptToMin, maxValue, adaptToMax ) )
	{ appIntegerToTextWidget( ct->ctColspanText, colspan );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons have been pushed.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTableChangeCellPushed, w, voidct )
    {
    CellTool *			ct= (CellTool *)voidct;
    RowProperties *		rp= &(ct->ctRowPropertiesChosen);
    TableRectangle *		tr= &(ct->ctTableRectangle);

    PropertyMask		rpSetMask;
    PropertyMask		cpSetMask;
    PropertyMask		bpSetMask;
    BorderProperties		bp;

    /**/
    EditApplication *		ea= ct->ctApplication;
    EditDocument *		ed= ea->eaCurrentDocument;

    TedDocument *		td;
    BufferDocument *		bd;
    DocumentProperties *	dp;

    int				minValue= 1;
    const int			adaptToMin= 0;
    int				maxValue;
    const int			adaptToMax= 0;

    int				rowspan= 0;
    int				colspan= 0;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;
    dp= &(bd->bdProperties);

    /**/
    docInitBorderProperties( &bp );

    PROPmaskCLEAR( &rpSetMask );
    PROPmaskCLEAR( &cpSetMask );

    /****/

    if  ( tr->trCol0 == tr->trCol1	&&
	  tr->trRow0 == tr->trRow1	&&
	  ! rp->rpIsTableHeader		)
	{
	maxValue= tr->trRow11- tr->trRow0+ 1;

	if  ( appGetIntegerFromTextWidget( ct->ctRowspanText, &rowspan,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	    { goto ready;	}

	maxValue= tr->trCol11- tr->trCol0+ 1;

	if  ( appGetIntegerFromTextWidget( ct->ctColspanText, &colspan,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	    { goto ready;	}

	if  ( tr->trCellRowspan != rowspan )
	    {
	    tr->trCellRowspan= rowspan;
	    PROPmaskADD( &cpSetMask, CLprop_ROWSPAN );
	    }

	if  ( tr->trCellColspan != colspan )
	    {
	    tr->trCellColspan= colspan;
	    PROPmaskADD( &cpSetMask, CLprop_COLSPAN );
	    }
	}

    /****/
    PROPmaskCLEAR( &bpSetMask );
    if  ( tedBorderToolGetProperties( &bp, &bpSetMask,
						&(ct->ctTopBorderTool), dp ) )
	{ goto ready;	}

    if  ( ! utilPropMaskIsEmpty( &bpSetMask ) )
	{
	docRowSetTopBorderInCols( rp, tr->trCol0, tr->trCol1, &bpSetMask, &bp );
	PROPmaskADD( &cpSetMask, CLpropTOP_BORDER );
	}

    /****/
    PROPmaskCLEAR( &bpSetMask );
    if  ( tedBorderToolGetProperties( &bp, &bpSetMask,
					    &(ct->ctBottomBorderTool), dp ) )
	{ goto ready;	}

    if  ( ! utilPropMaskIsEmpty( &bpSetMask ) )
	{
	docRowSetBottomBorderInCols( rp, tr->trCol0, tr->trCol1,
							    &bpSetMask, &bp );
	PROPmaskADD( &cpSetMask, CLpropBOTTOM_BORDER );
	}

    /****/
    PROPmaskCLEAR( &bpSetMask );
    if  ( tedBorderToolGetProperties( &bp, &bpSetMask,
						&(ct->ctLeftBorderTool), dp ) )
	{ goto ready;	}

    if  ( ! utilPropMaskIsEmpty( &bpSetMask ) )
	{
	docRowSetLeftBorderInCols( rp, tr->trCol0, tr->trCol1,
							    &bpSetMask, &bp );
	PROPmaskADD( &cpSetMask, CLpropLEFT_BORDER );
	}

    /****/
    PROPmaskCLEAR( &bpSetMask );
    if  ( tedBorderToolGetProperties( &bp, &bpSetMask,
					    &(ct->ctRightBorderTool), dp ) )
	{ goto ready;	}

    if  ( ! utilPropMaskIsEmpty( &bpSetMask ) )
	{
	docRowSetRightBorderInCols( rp, tr->trCol0, tr->trCol1,
							    &bpSetMask, &bp );
	PROPmaskADD( &cpSetMask, CLpropRIGHT_BORDER );
	}

    if  ( tedFormatToolGetShading( &cpSetMask, rp, tr->trCol0, tr->trCol1,
						dp, &(ct->ctShadingTool) ) )
	{ LDEB(1); goto ready;	}

    tedDocSetTableProperties( ed, &(ct->ctTableRectangle),
					    &rpSetMask, &cpSetMask, rp );

  ready:
    docCleanBorderProperties( &bp );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatRevertCellPushed, w, voidct )
    {
    CellTool *	ct= (CellTool *)voidct;

    docCopyRowProperties( &(ct->ctRowPropertiesChosen),
				&(ct->ctRowPropertiesSet), (const int *)0 );

    tedFormatToolRefreshCellPage( ct );

    return;
    }

/************************************************************************/
/*									*/
/*  Previous/Next Column/Row buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedCellPreviousColumn, w, voidct )
    {
    CellTool *		ct= (CellTool *)voidct;
    EditApplication *	ea= ct->ctApplication;
    EditDocument *	ed= ea->eaCurrentDocument;
    TedDocument *	td;

    TableRectangle	tr;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tr= ct->ctTableRectangle;
    td= (TedDocument *)ed->edPrivateData;

    if  ( docShiftTableRectangleByColumns( &tr, -1 ) )
	{ return;	}

    tedAppSetTableSelection( ed, &tr );
    }

/*
static APP_BUTTON_CALLBACK_H( tedTableSelectCell, w, voidct )
    {
    CellTool *		ct= (CellTool *)voidct;
    EditApplication *	ea= ct->ctApplication;
    EditDocument *	ed= ea->eaCurrentDocument;
    TedDocument *	td;

    TableRectangle	tr;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tr= ct->ctTableRectangle;
    td= (TedDocument *)ed->edPrivateData;

    tedAppSetTableSelection( ed, &tr );
    }
*/

static APP_BUTTON_CALLBACK_H( tedCellNextColumn, w, voidct )
    {
    CellTool *		ct= (CellTool *)voidct;
    EditApplication *	ea= ct->ctApplication;
    EditDocument *	ed= ea->eaCurrentDocument;
    TedDocument *	td;

    TableRectangle	tr;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tr= ct->ctTableRectangle;
    td= (TedDocument *)ed->edPrivateData;

    if  ( docShiftTableRectangleByColumns( &tr, +1 ) )
	{ return;	}

    tedAppSetTableSelection( ed, &tr );
    }

/************************************************************************/
/*									*/
/*  Callback from the general color chooser: a color was chosen.	*/
/*									*/
/************************************************************************/

static void tedCellToolGotColor(	void *			voidct,
					int 			which,
					const RGB8Color *	rgb8 )
    {
    CellTool *			ct= (CellTool *)voidct;

    switch( which )
	{
	case CLpropTOP_BORDER:
	    tedBorderSetExplicitColorChoice( &(ct->ctTopBorderTool), rgb8 );
	    break;

	case CLpropBOTTOM_BORDER:
	    tedBorderSetExplicitColorChoice( &(ct->ctBottomBorderTool), rgb8 );
	    break;

	case CLpropLEFT_BORDER:
	    tedBorderSetExplicitColorChoice( &(ct->ctLeftBorderTool), rgb8 );
	    break;

	case CLpropRIGHT_BORDER:
	    tedBorderSetExplicitColorChoice( &(ct->ctRightBorderTool), rgb8 );
	    break;

	case CLpropSHADE_BACK_COLOR:
	case CLpropSHADE_FORE_COLOR:
	    tedShadeSetExplicitColorChoice( &(ct->ctShadingTool),
							    which, rgb8 );
	    break;

	default:
	    LDEB(which); return;
	}
    }

/************************************************************************/
/*									*/
/*  Make a Cell tool, I.E. the 'Cell' page of the format tool.		*/
/*									*/
/************************************************************************/

void tedFormatFillCellPage(	CellTool *			ct,
				const CellPageResources *	cpr,
				AppInspector *			ai,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    APP_WIDGET	label;

    APP_WIDGET	row= (APP_WIDGET)0;

    const int	textColumns= 10;

    /**/
    ct->ctPageResources= cpr;

    is->isPrivate= ct;
    is->isGotColor= tedCellToolGotColor;

    /**/

    docInitTableRectangle( &(ct->ctTableRectangle) );

    docInitRowProperties( &(ct->ctRowPropertiesSet) );
    docInitRowProperties( &(ct->ctRowPropertiesChosen) );

    /**/

    appMakeLabelAndTextRow( &row, &label, &(ct->ctRowText),
			    pageWidget, cpr->cprRow, textColumns, 0 );
    appMakeLabelAndTextRow( &row, &label, &(ct->ctColumnText),
			    pageWidget, cpr->cprColumn, textColumns, 0 );

    /**/

    appMakeLabelAndTextRow( &row, &label, &(ct->ctRowspanText),
			    pageWidget, cpr->cprRowspan, textColumns, 1 );
    appMakeLabelAndTextRow( &row, &label, &(ct->ctColspanText),
			    pageWidget, cpr->cprColspan, textColumns, 1 );


    appGuiSetGotValueCallbackForText( ct->ctRowspanText,
				    tedCellToolRowspanChanged, (void *)ct );
    appGuiSetGotValueCallbackForText( ct->ctColspanText,
				    tedCellToolColspanChanged, (void *)ct );

    /**/
    tedMakeBorderTool( &(ct->ctTopBorderTool), ai, pageWidget,
		cpr->cprTopBorder, &(cpr->cprBorderToolResources),
		subjectPage, CLpropTOP_BORDER );

    tedMakeBorderTool( &(ct->ctBottomBorderTool), ai, pageWidget,
		cpr->cprBottomBorder, &(cpr->cprBorderToolResources),
		subjectPage, CLpropBOTTOM_BORDER );

    tedMakeBorderTool( &(ct->ctLeftBorderTool), ai, pageWidget,
		cpr->cprLeftBorder, &(cpr->cprBorderToolResources),
		subjectPage, CLpropLEFT_BORDER );

    tedMakeBorderTool( &(ct->ctRightBorderTool), ai, pageWidget,
		cpr->cprRightBorder, &(cpr->cprBorderToolResources),
		subjectPage, CLpropRIGHT_BORDER );

    /**/
    tedFormatMakeShadingTool( &(ct->ctShadingTool), ai, pageWidget,
		cpr->cprShading, &(cpr->cprShadingResources),
		subjectPage, CLpropSHADE_FORE_COLOR, CLpropSHADE_BACK_COLOR,
		(TedShadingToolCallback)0, (void *)ct );

    /**/
    appInspectorMakeButtonRow( &row, pageWidget,
		    &(is->isPrevButton), &(is->isNextButton),
		    isr->isrPrevButtonText, isr->isrNextButtonText,
		    tedCellPreviousColumn, tedCellNextColumn, ct );

    /* ??
    appInspectorMakeButtonRow( &row, pageWidget,
		    &(is->isSelectButton), &(is->isDeleteButton),
		    isr->isrSelectButtonText, isr->isrDeleteButtonText,
		    tedTableSelectColumn, tedTableDeleteColumn, ct );
    */

    appInspectorMakeButtonRow( &row, pageWidget,
		    &(is->isRevertButton), &(is->isApplyButton),
		    isr->isrRevert, isr->isrApplyToSubject,
		    tedFormatRevertCellPushed, tedTableChangeCellPushed,
		    (void *)ct );

    return;
    }

void tedCellToolFillChoosers(		CellTool *			ct,
					const CellPageResources *	cpr )
    {
    return;
    }

void tedFormatFinishCellPage(		CellTool *			ct,
					TedFormatTool *			tft,
					const CellPageResources *	cpr )
    {
    tedFinishBorderTool( &(ct->ctTopBorderTool) );
    tedFinishBorderTool( &(ct->ctBottomBorderTool) );
    tedFinishBorderTool( &(ct->ctLeftBorderTool) );
    tedFinishBorderTool( &(ct->ctRightBorderTool) );

    tedFinishShadingTool( &(ct->ctShadingTool) );
    }

/************************************************************************/
/*									*/
/*  Initialize/Clean a cell Tool.					*/
/*									*/
/************************************************************************/

void tedInitCellTool(	CellTool *	ct )
    {
    ct->ctApplication= (EditApplication *)0;
    ct->ctInspector= (AppInspector *)0;
    ct->ctPageResources= (const CellPageResources *)0;

    docInitTableRectangle( &(ct->ctTableRectangle) );

    ct->ctRowText= (APP_WIDGET)0;
    ct->ctColumnText= (APP_WIDGET)0;

    docInitRowProperties( &(ct->ctRowPropertiesSet) );
    docInitRowProperties( &(ct->ctRowPropertiesChosen) );

    tedInitBorderTool( &(ct->ctTopBorderTool) );
    tedInitBorderTool( &(ct->ctBottomBorderTool) );
    tedInitBorderTool( &(ct->ctLeftBorderTool) );
    tedInitBorderTool( &(ct->ctRightBorderTool) );

    tedInitShadingTool( &(ct->ctShadingTool) );

    return;
    }

void tedCleanCellTool(	CellTool *	ct )
    {
    docCleanRowProperties( &(ct->ctRowPropertiesSet) );
    docCleanRowProperties( &(ct->ctRowPropertiesChosen) );

    tedCleanBorderTool( &(ct->ctTopBorderTool) );
    tedCleanBorderTool( &(ct->ctBottomBorderTool) );
    tedCleanBorderTool( &(ct->ctLeftBorderTool) );
    tedCleanBorderTool( &(ct->ctRightBorderTool) );

    tedCleanShadingTool( &(ct->ctShadingTool) );

    return;
    }

/************************************************************************/
/*									*/
/*  Get row tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedCellSubjectResourceTable[]=
    {
    APP_RESOURCE( "tableToolCell",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Cell" ),
    APP_RESOURCE( "tableToolChangeCell",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Cell" ),
    APP_RESOURCE( "tableToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    APP_RESOURCE( "tableToolCellNextColumn",
		offsetof(InspectorSubjectResources,isrNextButtonText),
		"Next Column" ),
    APP_RESOURCE( "tableToolCellPreviousColumn",
		offsetof(InspectorSubjectResources,isrPrevButtonText),
		"Previous Column" ),
    APP_RESOURCE( "tableToolSelectCell",
		offsetof(InspectorSubjectResources,isrSelectButtonText),
		"Select Cell" ),
    };

static AppConfigurableResource TED_TedCellToolResourceTable[]=
    {
    APP_RESOURCE( "tableToolCellRow",
	    offsetof(CellPageResources,cprRow),
	    "Row" ),
    APP_RESOURCE( "tableToolCellColumn",
	    offsetof(CellPageResources,cprColumn),
	    "Column" ),

    /**/
    APP_RESOURCE( "tableToolCellTopBorder",
	    offsetof(CellPageResources,cprTopBorder),
	    "Top Border" ),
    APP_RESOURCE( "tableToolCellBottomBorder",
	    offsetof(CellPageResources,cprBottomBorder),
	    "Bottom Border" ),
    APP_RESOURCE( "tableToolCellLeftBorder",
	    offsetof(CellPageResources,cprLeftBorder),
	    "Left Border" ),
    APP_RESOURCE( "tableToolCellRightBorder",
	    offsetof(CellPageResources,cprRightBorder),
	    "Right Border" ),

    APP_RESOURCE( "tableToolCellColumnSpan",
	    offsetof(CellPageResources,cprColspan),
	    "Columns Occupied" ),
    APP_RESOURCE( "tableToolCellRowSpan",
	    offsetof(CellPageResources,cprRowspan),
	    "Rows Occupied" ),

    /**/
    APP_RESOURCE( "formatToolCellBorderWidth",
	    offsetof(CellPageResources, cprBorderToolResources.btrWidth),
	    "Width" ),
    APP_RESOURCE( "formatToolCellBorderStyle",
	    offsetof(CellPageResources, cprBorderToolResources.btrStyle),
	    "Style" ),
    APP_RESOURCE( "formatToolCellBorderColor",
	    offsetof(CellPageResources, cprBorderToolResources.btrColor),
	    "Color" ),
    APP_RESOURCE( "formatToolCellBorderColorChooserAutomatic",
	    offsetof(CellPageResources,cprBorderToolResources.
			    btrColorChooserResources.ccrAutomaticColor),
	    "Automatic" ),
    APP_RESOURCE( "formatToolCellBorderColorChooserMoreColors",
	    offsetof(CellPageResources,cprBorderToolResources.
			    btrColorChooserResources.ccrMoreColors),
	    "More Colors..." ),

    /**/
    APP_RESOURCE( "formatToolCellShading",
	    offsetof(CellPageResources,cprShading),
	    "Shading" ),

    APP_RESOURCE( "formatToolCellShadingPattern",
	    offsetof(CellPageResources,cprShadingResources.strPattern),
	    "Pattern" ),
    APP_RESOURCE( "formatToolCellShadingLevel",
	    offsetof(CellPageResources,cprShadingResources.strLevel),
	    "Level" ),
    APP_RESOURCE( "formatToolCellShadingForeColor",
	    offsetof(CellPageResources,cprShadingResources.strForeColor),
	    "Foreground" ),
    APP_RESOURCE( "formatToolCellShadingBackColor",
	    offsetof(CellPageResources,cprShadingResources.strBackColor),
	    "Background" ),

    /**/
    APP_RESOURCE( "formatToolCellShadingBackColorChooserNoBackground",
	    offsetof(CellPageResources,cprShadingResources.
			    strBackColorChooserResources.ccrAutomaticColor),
	    "No Background" ),
    APP_RESOURCE( "formatToolCellShadingBackColorChooserMoreColors",
	    offsetof(CellPageResources,cprShadingResources.
			    strBackColorChooserResources.ccrMoreColors),
	    "More Colors..." ),

    APP_RESOURCE( "formatToolCellShadingForeColorChooserAutomatic",
	    offsetof(CellPageResources,cprShadingResources.
			    strForeColorChooserResources.ccrAutomaticColor),
	    "Automatic" ),
    APP_RESOURCE( "formatToolCellShadingForeColorChooserMoreColors",
	    offsetof(CellPageResources,cprShadingResources.
			    strForeColorChooserResources.ccrMoreColors),
	    "More Colors..." ),

    /**/
    APP_RESOURCE( "tableToolCellVerticalAlignment",
	    offsetof(CellPageResources,cprVerticalAlignment),
	    "Vertical Alignment" ),
    };

void tedFormatToolGetCellResourceTable(	EditApplication *		ea,
					CellPageResources *		cpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)cpr,
				TED_TedCellToolResourceTable,
				sizeof(TED_TedCellToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedCellSubjectResourceTable,
				sizeof(TED_TedCellSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
