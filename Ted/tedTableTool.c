/************************************************************************/
/*									*/
/*  Table page of the format tool.					*/
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
/*  Refresh the 'Table' page of the format tool.			*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshTablePage(	TableTool *	tt )
    {
    RowProperties *	rp= &(tt->ttPropertiesChosen);

    char		scratch[50];

    appGeoLengthToString( scratch, rp->rpHalfGapWidthTwips, UNITtyPOINTS );
    appStringToTextWidget( tt->ttCellMarginText, scratch );

    appGeoLengthToString( scratch, rp->rpLeftIndentTwips, UNITtyPOINTS );
    appStringToTextWidget( tt->ttLeftIndentText, scratch );
    }

void tedFormatToolRefreshTableTool(
			    TableTool *			tt,
			    int *			pEnabled,
			    int *			pPref,
			    InspectorSubject *		is,
			    const DocumentSelection *	ds )
    {
    const RowProperties *	rp;
    const DocumentGeometry *	dg;

    const BufferItem *		rowBi;
    const BufferItem *		sectBi;

    if  ( docGetTableRectangle( &(tt->ttTableRectangle), ds ) )
	{
	docInitTableRectangle( &(tt->ttTableRectangle) );
	*pEnabled= 0; return;
	}

    rowBi= ds->dsBegin.dpBi;
    rowBi= rowBi->biParent;
    rowBi= rowBi->biParent;
    sectBi= rowBi->biParent;

    dg= &(sectBi->biSectDocumentGeometry);

    tt->ttPageRight= dg->dgPageWideTwips-
			    dg->dgLeftMarginTwips- dg->dgRightMarginTwips;
    tt->ttPageLeftMargin= dg->dgLeftMarginTwips;

    rp= &(rowBi->biRowProperties);

    if  ( docCopyRowProperties( &(tt->ttPropertiesChosen),
						    rp, (const int *)0 ) )
	{ LDEB(1); return;	}
    if  ( docCopyRowProperties( &(tt->ttPropertiesSet),
						    rp, (const int *)0 ) )
	{ LDEB(1); return;	}

    tedFormatToolRefreshTablePage( tt );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons have been pushed.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTableChangeTablePushed, w, voidtt )
    {
    TableTool *			tt= (TableTool *)voidtt;
    RowProperties *		rp= &(tt->ttPropertiesChosen);
    const TableRectangle *	tr= &(tt->ttTableRectangle);

    int				width;
    int				res;

    PropertyMask		rpSetMask;
    PropertyMask		cpSetMask;

    PROPmaskCLEAR( &rpSetMask );
    PROPmaskCLEAR( &cpSetMask );

    res= tedFormatToolGetRowLeftIndent( rp, tt->ttPageLeftMargin,
					    &width, tt->ttLeftIndentText );
    if  ( res != 0 )
	{ return;	}
    rp->rpLeftIndentTwips= width;
    PROPmaskADD( &rpSetMask, RPpropLEFT_INDENT );

    res= tedFormatToolGetGapWidth( rp, &width, tt->ttCellMarginText );
    if  ( res != 0 )
	{ return;	}
    rp->rpHalfGapWidthTwips= width;
    PROPmaskADD( &rpSetMask, RPpropGAP_WIDTH );

    tedAppSetTableProperties( tt->ttApplication, tr,
					    &rpSetMask, &cpSetMask, rp );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatRevertTablePushed, w, voidtt )
    {
    TableTool *	tt= (TableTool *)voidtt;

    docCopyRowProperties( &(tt->ttPropertiesChosen),
				    &(tt->ttPropertiesSet), (const int *)0 );

    tedFormatToolRefreshTablePage( tt );

    return;
    }

/************************************************************************/
/*									*/
/*  Check the cell margin that was entered on the 'Table' page of the	*/
/*  format tool.							*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedTableCellMarginChanged, w, voidtt )
    {
    TableTool *		tt= (TableTool *)voidtt;
    RowProperties *	rp= &(tt->ttPropertiesChosen);

    int			value;

    if  ( ! tedFormatToolGetGapWidth( rp, &value, tt->ttCellMarginText ) )
	{
	char	scratch[50];

	appGeoLengthToString( scratch, value, UNITtyPOINTS );
	appStringToTextWidget( tt->ttCellMarginText, scratch );
	}
    }

/************************************************************************/
/*									*/
/*  Check the left indent that was entered on the 'Table' page of the	*/
/*  format tool.							*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( tedTableLeftIndentChanged, w, voidtt )
    {
    TableTool *		tt= (TableTool *)voidtt;
    RowProperties *	rp= &(tt->ttPropertiesChosen);

    int			value;

    if  ( ! tedFormatToolGetRowLeftIndent( rp, tt->ttPageLeftMargin, &value,
						    tt->ttLeftIndentText ) )
	{
	char	scratch[50];

	appGeoLengthToString( scratch, value, UNITtyPOINTS );
	appStringToTextWidget( tt->ttLeftIndentText, scratch );
	}
    }

/************************************************************************/
/*									*/
/*  Delete/Select Table buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedTableDeleteTable, w, voidtt )
    {
    TableTool *			tt= (TableTool *)voidtt;
    EditApplication *		ea= tt->ttApplication;
    EditDocument *		ed= ea->eaCurrentDocument;

    const TableRectangle *	tr= &(tt->ttTableRectangle);

    if  ( tedDeleteRowsFromTable( ed, tr->trRow00, tr->trRow11 ) )
	{ LLDEB(tr->trRow00,tr->trRow11); return;	}

    return;
    }

static APP_BUTTON_CALLBACK_H( tedTableSelectTable, w, voidtt )
    {
    TableTool *			tt= (TableTool *)voidtt;
    EditApplication *		ea= tt->ttApplication;
    EditDocument *		ed= ea->eaCurrentDocument;
    TedDocument *		td;

    TableRectangle		trSet= tt->ttTableRectangle;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    td= (TedDocument *)ed->edPrivateData;

    docExpandTableRectangleToWholeTable( &trSet );

    tedAppSetTableSelection( ed, &trSet );
    }

/************************************************************************/
/*									*/
/*  Make the 'Table' page of the format tool.				*/
/*									*/
/************************************************************************/

void tedFormatFillTablePage(	TableTool *			tt,
				const TablePageResources *	tpr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    APP_WIDGET	widthLabel;
    APP_WIDGET	leftLabel;

    APP_WIDGET	row= (APP_WIDGET )0;

    const int	textColumns= 10;

    /**/
    tt->ttPageResources= tpr;

    /**/
    docInitTableRectangle( &(tt->ttTableRectangle) );

    docInitRowProperties( &(tt->ttPropertiesSet) );
    docInitRowProperties( &(tt->ttPropertiesChosen) );

    /**************/
    appMakeLabelAndTextRow( &row, &leftLabel, &(tt->ttLeftIndentText),
			pageWidget, tpr->tprLeftIndent, textColumns, 1 );

    appGuiSetGotValueCallbackForText( tt->ttLeftIndentText,
				tedTableLeftIndentChanged, (void *)tt );

    /**************/
    appMakeLabelAndTextRow( &row, &widthLabel, &(tt->ttCellMarginText),
		    pageWidget, tpr->tprCellMargin, textColumns, 1 );

    appGuiSetGotValueCallbackForText( tt->ttCellMarginText,
				tedTableCellMarginChanged, (void *)tt );

    /**************/
    appInspectorMakeButtonRow( &row, pageWidget,
		&(is->isSelectButton), &(is->isDeleteButton),
		isr->isrSelectButtonText, isr->isrDeleteButtonText,
		tedTableSelectTable, tedTableDeleteTable, (void *)tt );

    appInspectorMakeButtonRow( &row, pageWidget,
		&(is->isRevertButton), &(is->isApplyButton),
		isr->isrRevert, isr->isrApplyToSubject,
		tedFormatRevertTablePushed, tedTableChangeTablePushed,
		(void *)tt );

    return;
    }

/************************************************************************/
/*									*/
/*  Clean the table tool.						*/
/*									*/
/************************************************************************/

void tedFormatCleanTableTool(	TableTool *	tt )
    {
    docCleanRowProperties( &(tt->ttPropertiesSet) );
    docCleanRowProperties( &(tt->ttPropertiesChosen) );

    return;
    }

/************************************************************************/
/*									*/
/*  Get Table tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedTableSubjectResourceTable[]=
    {
    APP_RESOURCE( "tableToolTable",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Table" ),
    APP_RESOURCE( "tableToolChangeTable",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Table" ),
    APP_RESOURCE( "tableToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    /*  no Next Table */
    /*  no Prev Table */
    APP_RESOURCE( "tableToolSelectTable",
		offsetof(InspectorSubjectResources,isrSelectButtonText),
		"Select Table" ),
    APP_RESOURCE( "tableToolDeleteTable",
		offsetof(InspectorSubjectResources,isrDeleteButtonText),
		"Delete Table" ),
    /*  no Insert Table */
    /*  no Append Table */
    };

static AppConfigurableResource TED_TedTableToolResourceTable[]=
    {
    /**/
    APP_RESOURCE( "tableToolCellMargin",
		    offsetof(TablePageResources,tprCellMargin),
		    "Cell Margin" ),
    APP_RESOURCE( "tableToolLeftIndent",
		    offsetof(TablePageResources,tprLeftIndent),
		    "Left Margin" ),
    };

void tedFormatToolGetTableResourceTable( EditApplication *		ea,
					TablePageResources *		tpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)tpr,
				TED_TedTableToolResourceTable,
				sizeof(TED_TedTableToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedTableSubjectResourceTable,
				sizeof(TED_TedTableSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
