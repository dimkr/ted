/************************************************************************/
/*									*/
/*  The Column page of the format tool.					*/
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
/*  Refresh the 'Column' page of the format tool.			*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshColumnPage(	ColumnTool *	ct )
    {
    RowProperties *		rp= &(ct->ctPropertiesChosen);
    CellProperties *		cp;
    const TableRectangle *	tr= &(ct->ctTableRectangle);

    int				width;
    char			scratch[50];

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

    appIntegerToTextWidget( ct->ctNumberText, tr->trCol0+ 1 );

    if  ( tr->trCol0 == 0 )
	{
	width= rp->rpCells[tr->trCol0  ].cpRightBoundaryTwips-
						    rp->rpLeftIndentTwips;
	}
    else{
	width= rp->rpCells[tr->trCol0  ].cpRightBoundaryTwips-
			    rp->rpCells[tr->trCol0-1].cpRightBoundaryTwips;
	}

    appGeoLengthToString( scratch, width, UNITtyPOINTS );
    appStringToTextWidget( ct->ctWidthText, scratch );

    cp= rp->rpCells;

    tedBorderToolSetProperties( &(ct->ctLeftBorderTool), dp,
					&(cp[tr->trCol0].cpLeftBorder) );
    tedBorderToolSetProperties( &(ct->ctRightBorderTool), dp,
					&(cp[tr->trCol0].cpRightBorder) );
    }

void tedFormatToolRefreshColumnTool(
				ColumnTool *			ct,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds )
    {
    const RowProperties *	rp;
    const DocumentGeometry *	dg;

    const BufferItem *		rowBi;
    const BufferItem *		sectBi;

    TableRectangle *		tr= &(ct->ctTableRectangle);

    if  ( docGetTableRectangle( tr, ds )	||
	  tr->trCol1 != tr->trCol0		)
	{
	docInitTableRectangle( tr );
	*pEnabled= 0; return;
	}

    rowBi= ds->dsBegin.dpBi;
    rowBi= rowBi->biParent;
    rowBi= rowBi->biParent;
    sectBi= rowBi->biParent;

    dg= &(sectBi->biSectDocumentGeometry);

    ct->ctPageRight= dg->dgPageWideTwips-
			    dg->dgLeftMarginTwips- dg->dgRightMarginTwips;
    ct->ctPageLeftMargin= dg->dgLeftMarginTwips;

    rp= &(rowBi->biRowProperties);

    if  ( docCopyRowProperties( &(ct->ctPropertiesChosen),
						    rp, (const int *)0 ) )
	{ LDEB(1); return;	}
    if  ( docCopyRowProperties( &(ct->ctPropertiesSet),
						    rp, (const int *)0 ) )
	{ LDEB(1); return;	}

    appGuiEnableWidget( is->isPrevButton,
				ct->ctTableRectangle.trCol0 > 0 );
    appGuiEnableWidget( is->isNextButton,
				ct->ctTableRectangle.trCol1 <
				ct->ctTableRectangle.trCol11 );

    tedFormatToolRefreshColumnPage( ct );

    appGuiEnableWidget( is->isDeleteButton, rp->rpCellCount > 1 );

    *pEnabled= 1;
    return;
    }

static int tedFormatToolGetColumnWidth(	const RowProperties *	rp,
					int			col0,
					int			pageRight,
					int *			pValue,
					int *			pShift,
					APP_WIDGET		w )
    {
    int				width;
    int				changed;

    const CellProperties *	cp= &(rp->rpCells[col0]);
    int				rightRight;

    int				right;
    int				d;

    const int			minValue= 1;
    const int			adaptToMin= 0;
    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;
    
    right= cp->cpRightBoundaryTwips;
    if  ( col0 == 0 )
	{ width= right- rp->rpLeftIndentTwips;		}
    else{ width= right- cp[-1].cpRightBoundaryTwips;	}

    if  ( appGetLengthFromTextWidget( w, &width, &changed, UNITtyPOINTS,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	{ return -1; }

    if  ( col0 == 0 )
	{ right= rp->rpLeftIndentTwips+ width;		}
    else{ right= cp[-1].cpRightBoundaryTwips+ width;	}

    d= right- cp->cpRightBoundaryTwips;

    rightRight= rp->rpCells[rp->rpCellCount-1].cpRightBoundaryTwips;
    rightRight -= rp->rpHalfGapWidthTwips;

    if  ( rightRight+ d >= pageRight )
	{
	char		scratch[50];

	d= pageRight- rightRight;
	right= cp->cpRightBoundaryTwips+ d;

	if  ( col0 == 0 )
	    { width= right- rp->rpLeftIndentTwips;		}
	else{ width= right- cp[-1].cpRightBoundaryTwips;	}

	width -= 10;

	if  ( width <= 0 )
	    { appRefuseTextValue( w ); return -1;	}

	appGeoLengthToString( scratch, width, UNITtyPOINTS );
	appStringToTextWidget( w, scratch );
	appRefuseTextValue( w );
	return 1;
	}

    *pValue= width; *pShift= d; return 0;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons have been pushed.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTableChangeColumnPushed, w, voidct )
    {
    ColumnTool *		ct= (ColumnTool *)voidct;
    RowProperties *		rp= &(ct->ctPropertiesChosen);
    const TableRectangle *	tr= &(ct->ctTableRectangle);

    int				col;

    int				d;
    int				width;
    int				res;

    TableRectangle		trChange;

    BorderProperties		bp;

    PropertyMask		rpSetMask;
    PropertyMask		cpSetMask;
    PropertyMask		bpSetMask;

    /**/
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

    /**/

    PROPmaskCLEAR( &rpSetMask );
    PROPmaskCLEAR( &cpSetMask );

    docInitBorderProperties( &bp );

    /****/
    res= tedFormatToolGetColumnWidth( rp, tr->trCol0,
			    ct->ctPageRight, &width, &d, ct->ctWidthText );
    if  ( res != 0 )
	{ goto ready;	}
    PROPmaskADD( &cpSetMask, CLpropRIGHT_BOUNDARY );

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

    {
    CellProperties *		cp;

    cp= &(rp->rpCells[tr->trCol0]);
    for ( col= tr->trCol0; col < rp->rpCellCount; cp++, col++ )
	{ cp->cpRightBoundaryTwips += d; }
    }

    trChange= *tr;
    docExpandTableRectangleToWholeColumns( &trChange );

    tedAppSetTableProperties( ct->ctApplication, &trChange,
						&rpSetMask, &cpSetMask, rp );

  ready:
    docCleanBorderProperties( &bp );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatRevertColumnPushed, w, voidct )
    {
    ColumnTool *	ct= (ColumnTool *)voidct;

    docCopyRowProperties( &(ct->ctPropertiesChosen),
				    &(ct->ctPropertiesSet), (const int *)0 );

    tedFormatToolRefreshColumnPage( ct );

    return;
    }

/************************************************************************/
/*									*/
/*  Check a number as typed by the user.				*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedColumnWidthChanged, w, voidct )
    {
    ColumnTool *		ct= (ColumnTool *)voidct;
    RowProperties *		rp= &(ct->ctPropertiesChosen);
    const TableRectangle *	tr= &(ct->ctTableRectangle);

    int				d;
    int				width;

    if  ( ! tedFormatToolGetColumnWidth( rp, tr->trCol0,
			    ct->ctPageRight, &width, &d, ct->ctWidthText ) )
	{
	char	scratch[50];

	appGeoLengthToString( scratch, width, UNITtyPOINTS );

	appStringToTextWidget( ct->ctWidthText, scratch );
	}
    }

/************************************************************************/
/*									*/
/*  Previous/Next Column/Row buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedColumnPreviousColumn, w, voidct )
    {
    ColumnTool *	ct= (ColumnTool *)voidct;
    EditApplication *	ea= ct->ctApplication;
    EditDocument *	ed= ea->eaCurrentDocument;
    TedDocument *	td;

    TableRectangle	tr;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tr= ct->ctTableRectangle;
    td= (TedDocument *)ed->edPrivateData;

    docExpandTableRectangleToWholeColumns( &tr );

    if  ( docShiftTableRectangleByColumns( &tr, -1 ) )
	{ return;	}

    tedAppSetTableSelection( ed, &tr );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedTableSelectColumn, w, voidct )
    {
    ColumnTool *	ct= (ColumnTool *)voidct;
    EditApplication *	ea= ct->ctApplication;
    EditDocument *	ed= ea->eaCurrentDocument;
    TedDocument *	td;

    TableRectangle	tr;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tr= ct->ctTableRectangle;
    td= (TedDocument *)ed->edPrivateData;

    docExpandTableRectangleToWholeColumns( &tr );

    tedAppSetTableSelection( ed, &tr );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedColumnNextColumn, w, voidct )
    {
    ColumnTool *	ct= (ColumnTool *)voidct;
    EditApplication *	ea= ct->ctApplication;
    EditDocument *	ed= ea->eaCurrentDocument;
    TedDocument *	td;

    TableRectangle	tr;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tr= ct->ctTableRectangle;
    td= (TedDocument *)ed->edPrivateData;

    docExpandTableRectangleToWholeColumns( &tr );

    if  ( docShiftTableRectangleByColumns( &tr, +1 ) )
	{ return;	}

    tedAppSetTableSelection( ed, &tr );

    return;
    }

/************************************************************************/
/*									*/
/*  Delete Column/Row/Table buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTableDeleteColumn, w, voidct )
    {
    ColumnTool *		ct= (ColumnTool *)voidct;
    const TableRectangle *	tr= &(ct->ctTableRectangle);
    EditApplication *		ea= ct->ctApplication;
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    if  ( tedDeleteColumnsFromRows( ed,
		tr->trRow00, tr->trRow11, tr->trCol0, tr->trCol1 ) )
	{ LLDEB(tr->trRow00,tr->trRow11); return;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Insert/Append Column/Row buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTableInsertColumn, w, voidct )
    {
    ColumnTool *	ct= (ColumnTool *)voidct;
    EditApplication *	ea= ct->ctApplication;

    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedInsertColumnInTable( ed );

    appDocumentChanged( ed, 1 );
    }

static APP_BUTTON_CALLBACK_H( tedTableAppendColumn, w, voidct )
    {
    ColumnTool *	ct= (ColumnTool *)voidct;
    EditApplication *	ea= ct->ctApplication;

    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedAppendColumnToTable( ed );

    appDocumentChanged( ed, 1 );
    }

/************************************************************************/
/*									*/
/*  Callback from the general color chooser: a color was chosen.	*/
/*									*/
/************************************************************************/

static void tedColumnToolGotColor(	void *			voidct,
					int 			which,
					const RGB8Color *	rgb8 )
    {
    ColumnTool *			ct= (ColumnTool *)voidct;

    switch( which )
	{
	case CLpropLEFT_BORDER:
	    tedBorderSetExplicitColorChoice( &(ct->ctLeftBorderTool), rgb8 );
	    break;

	case CLpropRIGHT_BORDER:
	    tedBorderSetExplicitColorChoice( &(ct->ctRightBorderTool), rgb8 );
	    break;

	default:
	    LDEB(which); return;
	}
    }

/************************************************************************/
/*									*/
/*  Make a column tool, I.E. the 'Column' page of the format tool.	*/
/*									*/
/************************************************************************/

void tedFormatFillColumnPage(	ColumnTool *			ct,
				const ColumnPageResources *	cpr,
				AppInspector *			ai,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    APP_WIDGET	widthLabel;
    APP_WIDGET	columnLabel;

    APP_WIDGET	row= (APP_WIDGET)0;

    const int	textColumns= 10;

    /**/
    ct->ctPageResources= cpr;

    is->isPrivate= ct;
    is->isGotColor= tedColumnToolGotColor;

    /**/
    appMakeLabelAndTextRow( &row, &columnLabel, &(ct->ctNumberText),
			    pageWidget, isr->isrSubjectName, textColumns, 0 );

    appMakeLabelAndTextRow( &row, &widthLabel, &(ct->ctWidthText),
			pageWidget, cpr->cprWidth, textColumns, 1 );

    appGuiSetGotValueCallbackForText( ct->ctWidthText,
					tedColumnWidthChanged, (void *)ct );

    tedMakeBorderTool( &(ct->ctLeftBorderTool), ai, pageWidget,
		cpr->cprLeftBorder, &(cpr->cprBorderToolResources),
		subjectPage, CLpropLEFT_BORDER );

    tedMakeBorderTool( &(ct->ctRightBorderTool), ai, pageWidget,
		cpr->cprRightBorder, &(cpr->cprBorderToolResources),
		subjectPage, CLpropRIGHT_BORDER );

    appInspectorMakeButtonRow( &row, pageWidget,
		    &(is->isPrevButton), &(is->isNextButton),
		    isr->isrPrevButtonText, isr->isrNextButtonText,
		    tedColumnPreviousColumn, tedColumnNextColumn, ct );

    appInspectorMakeButtonRow( &row, pageWidget,
		    &(is->isSelectButton), &(is->isDeleteButton),
		    isr->isrSelectButtonText, isr->isrDeleteButtonText,
		    tedTableSelectColumn, tedTableDeleteColumn, ct );

    appInspectorMakeButtonRow( &row, pageWidget,
		    &(is->isInsertButton), &(is->isAppendButton),
		    isr->isrInsertButtonText, isr->isrAppendButtonText,
		    tedTableInsertColumn, tedTableAppendColumn, ct );

    appInspectorMakeButtonRow( &row, pageWidget,
		    &(is->isRevertButton), &(is->isApplyButton),
		    isr->isrRevert, isr->isrApplyToSubject,
		    tedFormatRevertColumnPushed, tedTableChangeColumnPushed,
		    (void *)ct );

    return;
    }

void tedColumnToolFillChoosers(	ColumnTool *			ct,
					const ColumnPageResources *	cpr )
    {
    return;
    }

void tedFormatFinishColumnPage(		ColumnTool *			ct,
					TedFormatTool *			tft,
					const ColumnPageResources *	cpr )
    {
    tedFinishBorderTool( &(ct->ctLeftBorderTool) );
    tedFinishBorderTool( &(ct->ctRightBorderTool) );
    }

/************************************************************************/
/*									*/
/*  Clean a column tool							*/
/*									*/
/************************************************************************/

void tedInitColumnTool(	ColumnTool *	ct )
    {
    ct->ctApplication= (EditApplication *)0;
    ct->ctInspector= (AppInspector *)0;
    ct->ctPageResources= (const ColumnPageResources *)0;

    docInitTableRectangle( &(ct->ctTableRectangle) );

    ct->ctPageRight= 0;
    ct->ctPageLeftMargin= 0;

    docInitRowProperties( &(ct->ctPropertiesSet) );
    docInitRowProperties( &(ct->ctPropertiesChosen) );

    ct->ctNumberText= (APP_WIDGET)0;
    ct->ctWidthText= (APP_WIDGET)0;

    tedInitBorderTool( &(ct->ctLeftBorderTool) );
    tedInitBorderTool( &(ct->ctRightBorderTool) );

    return;
    }

void tedCleanColumnTool(	ColumnTool *	ct )
    {
    docCleanRowProperties( &(ct->ctPropertiesSet) );
    docCleanRowProperties( &(ct->ctPropertiesChosen) );

    tedCleanBorderTool( &(ct->ctLeftBorderTool) );
    tedCleanBorderTool( &(ct->ctRightBorderTool) );

    return;
    }

/************************************************************************/
/*									*/
/*  Get column tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedColumnSubjectResourceTable[]=
    {
    APP_RESOURCE( "tableToolColumn",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Column" ),
    APP_RESOURCE( "tableToolChangeColumn",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Column" ),
    APP_RESOURCE( "tableToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    APP_RESOURCE( "tableToolNextColumn",
		offsetof(InspectorSubjectResources,isrNextButtonText),
		"Next Column" ),
    APP_RESOURCE( "tableToolPreviousColumn",
		offsetof(InspectorSubjectResources,isrPrevButtonText),
		"Previous Column" ),
    APP_RESOURCE( "tableToolSelectColumn",
		offsetof(InspectorSubjectResources,isrSelectButtonText),
		"Select Column" ),
    APP_RESOURCE( "tableToolDeleteColumn",
		offsetof(InspectorSubjectResources,isrDeleteButtonText),
		"Delete Column" ),
    APP_RESOURCE( "tableToolInsertColumn",
		offsetof(InspectorSubjectResources,isrInsertButtonText),
		"Insert Column" ),
    APP_RESOURCE( "tableToolAppendColumn",
		offsetof(InspectorSubjectResources,isrAppendButtonText),
		"Append Column" ),
    };

static AppConfigurableResource TED_TedColumnToolResourceTable[]=
    {
    APP_RESOURCE( "tableToolWidth",
		offsetof(ColumnPageResources,cprWidth),
		"Width" ),

    /**/
    APP_RESOURCE( "tableToolColumnLeftBorder",
		offsetof(ColumnPageResources,cprLeftBorder),
		"Left Border" ),
    APP_RESOURCE( "tableToolColumnRightBorder",
		offsetof(ColumnPageResources,cprRightBorder),
		"Right Border" ),

    /**/
    APP_RESOURCE( "formatToolColumnBorderWidth",
	    offsetof(ColumnPageResources, cprBorderToolResources.btrWidth),
	    "Width" ),
    APP_RESOURCE( "formatToolColumnBorderStyle",
	    offsetof(ColumnPageResources, cprBorderToolResources.btrStyle),
	    "Style" ),
    APP_RESOURCE( "formatToolColumnBorderColor",
	    offsetof(ColumnPageResources, cprBorderToolResources.btrColor),
	    "Color" ),
    APP_RESOURCE( "formatToolColumnBorderColorChooserAutomatic",
	    offsetof(ColumnPageResources,cprBorderToolResources.
			    btrColorChooserResources.ccrAutomaticColor),
	    "Automatic" ),
    APP_RESOURCE( "formatToolColumnBorderColorChooserMoreColors",
	    offsetof(ColumnPageResources,cprBorderToolResources.
			    btrColorChooserResources.ccrMoreColors),
	    "More Colors..." ),
    };

void tedFormatToolGetColumnResourceTable(EditApplication *		ea,
					ColumnPageResources *		cpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)cpr,
				TED_TedColumnToolResourceTable,
				sizeof(TED_TedColumnToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedColumnSubjectResourceTable,
				sizeof(TED_TedColumnSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
