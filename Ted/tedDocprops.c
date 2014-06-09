/************************************************************************/
/*  Dialog with document properties.					*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	"tedApp.h"

#   define	DRH_CM	4.5

/************************************************************************/
/*  Represents a mail dialog.						*/
/************************************************************************/
#   define	FILEL	400

typedef struct DocumentPropertyPageResources
    {
    const char *		dpprOkText;
    const char *		dpprCancelText;

    const char *		dpprGeneratorText;
    const char *		dpprTitleText;
    const char *		dpprAuthorText;
    const char *		dpprCompanyText;
    const char *		dpprSubjectText;
    const char *		dpprKeywordsText;
    const char *		dpprDoccommText;
    const char *		dpprHlinkbaseText;

    const char *		dpprCreatimText;
    const char *		dpprRevtimText;
    const char *		dpprPrintimText;

    const char *		dpprPageCountText;
    const char *		dpprParaCountText;
    const char *		dpprLineCountText;
    const char *		dpprWordCountText;
    const char *		dpprCharCountText;
    } DocumentPropertyPageResources;

typedef struct PropertyDialog
    {
    AppDialog			dpdDialog;

    APP_WIDGET			dpdOkButton;

    APP_WIDGET			dpdGeneratorWidget;
    APP_WIDGET			dpdTitleWidget;
    APP_WIDGET			dpdAuthorWidget;
    APP_WIDGET			dpdCompanyWidget;
    APP_WIDGET			dpdSubjectWidget;
    APP_WIDGET			dpdKeywordsWidget;
    APP_WIDGET			dpdDoccommWidget;
    APP_WIDGET			dpdHlinkbaseWidget;

    APP_WIDGET			dpdCreatimWidget;
    APP_WIDGET			dpdRevtimWidget;
    APP_WIDGET			dpdPrintimWidget;

    APP_WIDGET			dpdPageCountWidget;
    APP_WIDGET			dpdParaCountWidget;
    APP_WIDGET			dpdLineCountWidget;
    APP_WIDGET			dpdWordCountWidget;
    APP_WIDGET			dpdCharCountWidget;
    } PropertyDialog;

static PropertyDialog *	TED_PropertyDialog;

static AppConfigurableResource TED_PropertyDialogResourceTable[]=
    {
	APP_RESOURCE( "propDialogGenerator",
		    offsetof(DocumentPropertyPageResources,dpprGeneratorText),
		    "Generator" ),
	APP_RESOURCE( "propDialogTitle",
		    offsetof(DocumentPropertyPageResources,dpprTitleText),
		    "Title" ),
	APP_RESOURCE( "propDialogAuthor",
		    offsetof(DocumentPropertyPageResources,dpprAuthorText),
		    "Author" ),
	APP_RESOURCE( "propDialogCompany",
		    offsetof(DocumentPropertyPageResources,dpprCompanyText),
		    "Company" ),
	APP_RESOURCE( "propDialogSubject",
		    offsetof(DocumentPropertyPageResources,dpprSubjectText),
		    "Subject" ),
	APP_RESOURCE( "propDialogKeywords",
		    offsetof(DocumentPropertyPageResources,dpprKeywordsText),
		    "Keywords" ),
	APP_RESOURCE( "propDialogDoccom",
		    offsetof(DocumentPropertyPageResources,dpprDoccommText),
		    "Comment" ),
	APP_RESOURCE( "propDialogHlinkbase",
		    offsetof(DocumentPropertyPageResources,dpprHlinkbaseText),
		    "Links Relative to" ),

	APP_RESOURCE( "propDialogCreatim",
		    offsetof(DocumentPropertyPageResources,dpprCreatimText),
		    "Created" ),
	APP_RESOURCE( "propDialogRevtim",
		    offsetof(DocumentPropertyPageResources,dpprRevtimText),
		    "Last Changed" ),
	APP_RESOURCE( "propDialogPrintim",
		    offsetof(DocumentPropertyPageResources,dpprPrintimText),
		    "Last Printed" ),

	APP_RESOURCE( "propDialogPages",
		    offsetof(DocumentPropertyPageResources,dpprPageCountText),
		    "Pages" ),
	APP_RESOURCE( "propDialogParagraphs",
		    offsetof(DocumentPropertyPageResources,dpprParaCountText),
		    "Paragraphs" ),
	APP_RESOURCE( "propDialogLines",
		    offsetof(DocumentPropertyPageResources,dpprLineCountText),
		    "Lines" ),
	APP_RESOURCE( "propDialogWords",
		    offsetof(DocumentPropertyPageResources,dpprWordCountText),
		    "Words" ),
	APP_RESOURCE( "propDialogCharacters",
		    offsetof(DocumentPropertyPageResources,dpprCharCountText),
		    "Characters" ),

	APP_RESOURCE( "propDialogCancel",
		    offsetof(DocumentPropertyPageResources,dpprCancelText),
		    "Cancel" ),
	APP_RESOURCE( "propDialogOk",
		    offsetof(DocumentPropertyPageResources,dpprOkText),
		    "Ok" ),
    };

/************************************************************************/
/*									*/
/*  One of the Text widgets changed.					*/
/*									*/
/************************************************************************/

static APP_TXTYPING_CALLBACK_H( tedPropertyChanged, w, voiddpd )
    {
    PropertyDialog *	dpd= (PropertyDialog *)voiddpd;

    appGuiEnableWidget( dpd->dpdOkButton, 1 );

    appGuiSetDefaultButtonForDialog( &(dpd->dpdDialog), dpd->dpdOkButton );

    return;
    }

/************************************************************************/
/*									*/
/*  The buttons have been pushed.					*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedPropertiesCancelPushed, w, voiddpd )
    {
    PropertyDialog *	dpd= (PropertyDialog *)voiddpd;

    appGuiBreakDialog( &(dpd->dpdDialog), AQDrespCANCEL );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedPropertiesOkPushed, w, voiddpd )
    {
    PropertyDialog *	dpd= (PropertyDialog *)voiddpd;

    appGuiBreakDialog( &(dpd->dpdDialog), AQDrespOK );

    return;
    }

/************************************************************************/
/*									*/
/*  A Property Dialog must be destroyed.				*/
/*									*/
/************************************************************************/

static APP_CLOSE_CALLBACK_H( tedClosePropertyDialog, w, voiddpd )
    {
    PropertyDialog *	dpd= (PropertyDialog *)voiddpd;

    appGuiBreakDialog( &(dpd->dpdDialog), AQDrespCANCEL );

    return;
    }

static void tedPropsDialogMakePropertyRow(	APP_WIDGET*	pRow,
						APP_WIDGET *	pLabel,
						APP_WIDGET *	pText,
						APP_WIDGET	parent,
						PropertyDialog * dpd,
						const char *	labelText,
						int		textEnabled )
    {
    APP_WIDGET		row;
    APP_WIDGET		label;
    APP_WIDGET		text;

    const int		heightResizable= 0;
    const int		columnCount= 5;
    const int		labelColumn= 0;
    const int		labelColspan= 1;
    const int		textColumn= 1;
    const int		textColspan= 4;
    const int		textColumns= 50;

    row= appMakeRowInColumn( parent, columnCount, heightResizable );

    appMakeLabelInRow( &label, row, labelColumn, labelColspan, labelText );
    appMakeTextInRow( &text, row, textColumn, textColspan,
						textColumns, textEnabled );

    *pRow= row; *pLabel= label; *pText= text;

    if  ( textEnabled )
	{
	appGuiSetTypingCallbackForText( text,
					    tedPropertyChanged, (void *)dpd );
	}

    return;
    }

static void tedPropsDialogMakeTextForm(
				APP_WIDGET				parent,
				PropertyDialog *			dpd,
				const DocumentPropertyPageResources *	dppr )
    {
    APP_WIDGET		row;
    APP_WIDGET		label;

    const int		changeable= 1;
    const int		derived= 0;

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdTitleWidget),
			    parent, dpd, dppr->dpprTitleText, changeable );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdAuthorWidget),
			    parent, dpd, dppr->dpprAuthorText, changeable );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdCompanyWidget),
			    parent, dpd, dppr->dpprCompanyText, changeable );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdSubjectWidget),
			    parent, dpd, dppr->dpprSubjectText, changeable );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdKeywordsWidget),
			    parent, dpd, dppr->dpprKeywordsText, changeable );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdDoccommWidget),
			    parent, dpd, dppr->dpprDoccommText, changeable );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdHlinkbaseWidget),
			    parent, dpd, dppr->dpprHlinkbaseText, changeable );

    /**/
    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdGeneratorWidget),
			    parent, dpd, dppr->dpprGeneratorText, derived );

    /**/

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdCreatimWidget),
			    parent, dpd, dppr->dpprCreatimText, derived );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdRevtimWidget),
			    parent, dpd, dppr->dpprRevtimText, derived );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdPrintimWidget),
			    parent, dpd, dppr->dpprPrintimText, derived );

    /**/

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdPageCountWidget),
			    parent, dpd, dppr->dpprPageCountText, derived );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdParaCountWidget),
			    parent, dpd, dppr->dpprParaCountText, derived );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdLineCountWidget),
			    parent, dpd, dppr->dpprLineCountText, derived );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdWordCountWidget),
			    parent, dpd, dppr->dpprWordCountText, derived );

    tedPropsDialogMakePropertyRow( &row, &label, &(dpd->dpdCharCountWidget),
			    parent, dpd, dppr->dpprCharCountText, derived );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the form with the two buttons.					*/
/*									*/
/************************************************************************/

static APP_WIDGET tedPropsDialogMakeButtonRow(
				APP_WIDGET				parent,
				PropertyDialog *			dpd,
				const DocumentPropertyPageResources *	dppr )
    {
    APP_WIDGET	row;
    APP_WIDGET	cancelButton;
    const int	heightResizable= 0;

    row= appMakeRowInColumn( parent, 2, heightResizable );

    appMakeButtonInRow( &(dpd->dpdOkButton), row, dppr->dpprOkText,
				tedPropertiesOkPushed, (void *)dpd, 0, 1 );

    appMakeButtonInRow( &(cancelButton), row, dppr->dpprCancelText, 
			    tedPropertiesCancelPushed, (void *)dpd, 1, 0 );

    appGuiSetCancelButtonForDialog( &(dpd->dpdDialog), cancelButton );

    return row;
    }

/************************************************************************/
/*									*/
/*  Make a document properties dialog.					*/
/*									*/
/************************************************************************/

static PropertyDialog * tedMakePropertyDialog( EditDocument *	ed,
					APP_WIDGET		printOption,
					const char *		pixmapName )
    {
    EditApplication *	ea= ed->edApplication;
    PropertyDialog *	dpd;
    
    APP_WIDGET		paned;
    APP_WIDGET		separator;
    APP_WIDGET		buttonForm;

    APP_BITMAP_IMAGE	iconPixmap= (APP_BITMAP_IMAGE)0;
    APP_BITMAP_MASK	iconMask= (APP_BITMAP_MASK)0;

    static DocumentPropertyPageResources	dppr;
    static int					gotResources;

    if  ( appGetImagePixmap( ea, pixmapName, &iconPixmap, &iconMask )  )
	{ SDEB(pixmapName); return (PropertyDialog *)0;	}

    dpd= (PropertyDialog *)malloc( sizeof(PropertyDialog) );
    if  ( ! dpd )
	{ XDEB(dpd); return (PropertyDialog *)0;	}

    if  ( ! gotResources )
	{
	appGuiGetResourceValues( &gotResources, ea, (void *)&dppr,
				    TED_PropertyDialogResourceTable,
				    sizeof(TED_PropertyDialogResourceTable)/
				    sizeof(AppConfigurableResource) );
	}

    appMakeVerticalDialog( &(dpd->dpdDialog), &paned, ea,
						tedClosePropertyDialog,
						(APP_DESTROY_CALLBACK_T)0,
						(void *)dpd,
						"tedPropertyDialog" );

    appSetShellTitle( dpd->dpdDialog.adTopWidget,
				    printOption, ea->eaApplicationName );


    tedPropsDialogMakeTextForm( paned, dpd, &dppr );

    appGuiInsertSeparatorInColumn( &separator, paned );

    buttonForm= tedPropsDialogMakeButtonRow( paned, dpd, &dppr );

    appGuiShowDialog( &(dpd->dpdDialog), ed->edToplevel.atTopWidget );

    return dpd;
    }

/************************************************************************/

static int tedPropChangeProperty(	int *			pChanged,
					unsigned char **	pValue,
					APP_WIDGET		textWidget )
    {
    char *	oldValue= (char *)*pValue;
    char *	newValue;
    char *	saved= (char *)0;

    newValue= appGetStringFromTextWidget( textWidget );

    if  ( ! oldValue || ! oldValue[0] )
	{
	if  ( ! newValue[0] )
	    { appFreeStringFromTextWidget( newValue ); return 0;	}
	}
    else{
	if  ( ! strcmp( oldValue, newValue ) )
	    { appFreeStringFromTextWidget( newValue ); return 0;	}
	}

    if  ( newValue[0] )
	{
	saved= strdup( newValue );
	if  ( ! saved )
	    { XDEB(saved); return -1;	}
	}

    appFreeStringFromTextWidget( newValue );

    if  ( oldValue )
	{ free( oldValue );	}

    *pChanged= 1; *pValue= (unsigned char *)saved;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Show the 'Document Properties...' dialog.				*/
/*									*/
/*  1)	Make or just show it.						*/
/*  2)	Set the default content.					*/
/*									*/
/************************************************************************/

static void tedPropToWidget(	APP_WIDGET			w,
				const unsigned char *		prop )
    {
    if  ( prop )
	{ appStringToTextWidget( w, (char *)prop );	}
    else{ appStringToTextWidget( w, "" );		}

    return;
    }

static void tedDateToWidget(	APP_WIDGET			w,
				const struct tm *		tm )
    {
    if  ( tm->tm_mday == 0 )
	{ appStringToTextWidget( w, "" );	}
    else{
	char		scratch[40+1];
	const char *	frm;

	/*  GCC Shut Up! */
	frm= "%c";
	if  ( strftime( scratch, sizeof(scratch)- 1, frm, tm ) < 1 )
	    { LDEB(1); appStringToTextWidget( w, "?" );	}
	else{ appStringToTextWidget( w, scratch );	}
	}

    return;
    }

int tedRunPropertyDialog(	EditDocument *			ed,
				const DocumentStatistics *	ds,
				APP_WIDGET			option,
				const char *			pixmapName )
    {
    int				rval= 0;
    int				changed= 0;

    PropertyDialog *		dpd= TED_PropertyDialog;

    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);

    /*  1  */
    if  ( ! dpd )
	{
	dpd= TED_PropertyDialog= tedMakePropertyDialog( ed, option,
								pixmapName );
	if  ( ! dpd )
	    { XDEB(dpd); return -1;	}

	}
    else{
	appSetShellTitle( dpd->dpdDialog.adTopWidget,
					    option, ea->eaApplicationName );

	appGuiShowDialog( &(dpd->dpdDialog), ed->edToplevel.atTopWidget );
	}

    tedPropToWidget( dpd->dpdGeneratorWidget, dp->dpGenerator );
    tedPropToWidget( dpd->dpdTitleWidget, dp->dpTitle );
    tedPropToWidget( dpd->dpdAuthorWidget, dp->dpAuthor );
    tedPropToWidget( dpd->dpdCompanyWidget, dp->dpCompany );
    tedPropToWidget( dpd->dpdSubjectWidget, dp->dpSubject );
    tedPropToWidget( dpd->dpdKeywordsWidget, dp->dpKeywords );
    tedPropToWidget( dpd->dpdDoccommWidget, dp->dpDoccomm );
    tedPropToWidget( dpd->dpdHlinkbaseWidget, dp->dpHlinkbase );

    tedDateToWidget( dpd->dpdCreatimWidget, &(dp->dpCreatim) );
    tedDateToWidget( dpd->dpdRevtimWidget, &(dp->dpRevtim) );
    tedDateToWidget( dpd->dpdPrintimWidget, &(dp->dpPrintim) );

    appIntegerToTextWidget( dpd->dpdPageCountWidget, ds->dsPageCount );
    appIntegerToTextWidget( dpd->dpdParaCountWidget, ds->dsParagraphCount );
    appIntegerToTextWidget( dpd->dpdLineCountWidget, ds->dsLineCount );
    appIntegerToTextWidget( dpd->dpdWordCountWidget, ds->dsWordCount );
    appIntegerToTextWidget( dpd->dpdCharCountWidget, ds->dsCharacterCount );

    appGuiEnableWidget( dpd->dpdOkButton, 0 );

    appGuiRunDialog( &(dpd->dpdDialog), AQDrespNONE, ea );

    switch( dpd->dpdDialog.adResponse )
	{
	case AQDrespOK:
	    break;
	default:
	    LDEB(dpd->dpdDialog.adResponse);
	    /*FALLTHROUGH*/
	case AQDrespCANCEL:
	    appGuiHideDialog( &(dpd->dpdDialog) );
	    return -1;
	}

    appGuiHideDialog( &(dpd->dpdDialog) );

    if  ( tedPropChangeProperty( &changed, &(dp->dpTitle),
						dpd->dpdTitleWidget ) )
	{ LDEB(1); return -1;	}

    if  ( tedPropChangeProperty( &changed, &(dp->dpAuthor),
						dpd->dpdAuthorWidget ) )
	{ LDEB(1); return -1;	}

    if  ( tedPropChangeProperty( &changed, &(dp->dpCompany),
						dpd->dpdCompanyWidget ) )
	{ LDEB(1); return -1;	}

    if  ( tedPropChangeProperty( &changed, &(dp->dpSubject),
						dpd->dpdSubjectWidget ) )
	{ LDEB(1); return -1;	}

    if  ( tedPropChangeProperty( &changed, &(dp->dpKeywords),
						dpd->dpdKeywordsWidget ) )
	{ LDEB(1); return -1;	}

    if  ( tedPropChangeProperty( &changed, &(dp->dpDoccomm),
						dpd->dpdDoccommWidget ) )
	{ LDEB(1); return -1;	}

    if  ( tedPropChangeProperty( &changed, &(dp->dpHlinkbase),
						dpd->dpdHlinkbaseWidget ) )
	{ LDEB(1); return -1;	}

    if  ( changed )
	{ appDocumentChanged( ed, 1 ); }

    return rval;
    }
