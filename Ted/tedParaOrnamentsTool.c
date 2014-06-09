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
/*  Refresh the controls on the paragraph properties page.		*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshParaOrnamentsPage(
					ParagraphOrnamentsTool *	pot )
    {
    ParagraphProperties *	pp= &(pot->potPropertiesChosen);
    EditApplication *		ea= pot->potApplication;
    EditDocument *		ed= ea->eaCurrentDocument;

    TedDocument *		td;
    BufferDocument *		bd;
    DocumentProperties *	dp;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;
    dp= &(bd->bdProperties);

    tedBorderToolSetProperties( &(pot->potTopBorderTool), dp,
						&(pp->ppTopBorder) );
    tedBorderToolSetProperties( &(pot->potBottomBorderTool), dp,
						&(pp->ppBottomBorder) );
    tedBorderToolSetProperties( &(pot->potLeftBorderTool), dp,
						&(pp->ppLeftBorder) );
    tedBorderToolSetProperties( &(pot->potRightBorderTool), dp,
						&(pp->ppRightBorder) );

    tedSetShadingTool( &(pot->potShadingTool), dp, &(pp->ppShading) );

    return;
    }


void tedFormatToolRefreshParaOrnamentsTool(
				ParagraphOrnamentsTool *	pot,
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
    PROPmaskUNSET( &ppUpdMask, PPpropTAB_STOPS );

    PROPmaskCLEAR( &ppChgMask );

    if  ( docUpdParaProperties( &ppChgMask, &(pot->potPropertiesChosen),
					    &ppUpdMask, pp,
					    colorMap, listStyleMap ) )
	{ LDEB(1); return ;	}

    PROPmaskCLEAR( &ppChgMask );

    if  ( docUpdParaProperties( &ppChgMask, &(pot->potPropertiesSet),
					    &ppUpdMask, pp,
					    colorMap, listStyleMap ) )
	{ LDEB(1); return ;	}

    appGuiEnableWidget( is->isPrevButton,
				docPrevParagraph( bi ) != (BufferItem *)0 );
    appGuiEnableWidget( is->isNextButton,
				docNextParagraph( bi ) != (BufferItem *)0 );

    tedFormatToolRefreshParaOrnamentsPage( pot );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  The 'Revert' button on the paragraph tool has been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatParaRevertPushed, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;

    PropertyMask		ppChgMask;
    PropertyMask		ppUpdMask;

    const int * const		colorMap= (const int *)0;
    const int * const		listStyleMap= (const int *)0;

    PROPmaskCLEAR( &ppChgMask );

    PROPmaskCLEAR( &ppUpdMask );
    utilPropMaskFill( &ppUpdMask, PPprop_COUNT );

    docUpdParaProperties( &ppChgMask, &(pot->potPropertiesChosen),
			&ppUpdMask, &(pot->potPropertiesSet),
			colorMap, listStyleMap );

    tedFormatToolRefreshParaOrnamentsPage( pot );

    return;
    }

/************************************************************************/
/*									*/
/*  Previous/Next Paragraph buttons have been pushed.			*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatPrevPara, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;
    EditApplication *		ea= pot->potApplication;

    tedSelectWholeParagraph( ea, -1 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatNextPara, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;
    EditApplication *		ea= pot->potApplication;

    tedSelectWholeParagraph( ea, 1 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatSelectPara, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;
    EditApplication *		ea= pot->potApplication;

    tedSelectWholeParagraph( ea, 0 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatDeletePara, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;
    EditApplication *		ea= pot->potApplication;

    tedDeleteCurrentParagraph( ea );
    }

static APP_BUTTON_CALLBACK_H( tedFormatInsertPara, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;
    EditApplication *		ea= pot->potApplication;

    tedInsertParagraph( ea, 0 );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatAppendPara, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;
    EditApplication *		ea= pot->potApplication;

    tedInsertParagraph( ea, 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Change' button on the paragraph borders and shading page has	*/
/*  been pushed.							*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatChangePara, w, voidpot )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;
    ParagraphProperties *	pp= &(pot->potPropertiesChosen);

    PropertyMask		ppSetMask;
    PropertyMask		xxSetMask;

    /**/
    EditApplication *		ea= pot->potApplication;
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
    PROPmaskCLEAR( &ppSetMask );
    PROPmaskADD( &ppSetMask, PPpropTOP_BORDER );
    PROPmaskADD( &ppSetMask, PPpropBOTTOM_BORDER );
    PROPmaskADD( &ppSetMask, PPpropLEFT_BORDER );
    PROPmaskADD( &ppSetMask, PPpropRIGHT_BORDER );

    PROPmaskADD( &ppSetMask, PPpropSHADE_FORE_COLOR );
    PROPmaskADD( &ppSetMask, PPpropSHADE_BACK_COLOR );
    PROPmaskADD( &ppSetMask, PPpropSHADE_LEVEL );
    PROPmaskADD( &ppSetMask, PPpropSHADE_PATTERN );

    /**/
    PROPmaskCLEAR( &xxSetMask );
    if  ( tedBorderToolGetProperties( &(pp->ppTopBorder), &xxSetMask,
					    &(pot->potTopBorderTool), dp ) )
	{ return;	}
    PROPmaskCLEAR( &xxSetMask );
    if  ( tedBorderToolGetProperties( &(pp->ppBottomBorder), &xxSetMask,
					    &(pot->potBottomBorderTool), dp ) )
	{ return;	}
    PROPmaskCLEAR( &xxSetMask );
    if  ( tedBorderToolGetProperties( &(pp->ppLeftBorder), &xxSetMask,
					    &(pot->potLeftBorderTool), dp ) )
	{ return;	}
    PROPmaskCLEAR( &xxSetMask );
    if  ( tedBorderToolGetProperties( &(pp->ppRightBorder), &xxSetMask,
					    &(pot->potRightBorderTool), dp ) )
	{ return;	}

    {
    PropertyMask		isSetMask;

    ExpandedItemShading		eis;

    docInitExpandedItemShading( &eis );

    PROPmaskCLEAR( &isSetMask );

    if  ( tedShadingToolGetShading( &eis, &isSetMask,
						    &(pot->potShadingTool) ) )
	{ return;	}

    if  ( ! utilPropMaskIsEmpty( &isSetMask ) )
	{
	PropertyMask	isDoneMask;

	PROPmaskCLEAR( &isDoneMask );

	if  ( docIndirectItemShading( &isDoneMask,
				    &(pp->ppShading), &isSetMask, &eis,
				    &(dp->dpColors), &(dp->dpColorCount) ) )
	    { LDEB(1); return;	}

	docShadingMaskToParagraphMask( &isSetMask, &isSetMask );

	utilPropMaskOr( &ppSetMask, &ppSetMask, &isSetMask );
	}
    }

    /**/
    if  ( tedDocChangeParagraphProperties( ed, &ppSetMask, pp ) )
	{ LDEB(1);	}

    return;
    }

/************************************************************************/
/*									*/
/*  Callback from the general color chooser: a color was chosen.	*/
/*									*/
/************************************************************************/

static void tedParaOrnamentsGotColor(	void *			voidpot,
					int 			which,
					const RGB8Color *	rgb8 )
    {
    ParagraphOrnamentsTool *	pot= (ParagraphOrnamentsTool *)voidpot;

    switch( which )
	{
	case PPpropTOP_BORDER:
	    tedBorderSetExplicitColorChoice(
					&(pot->potTopBorderTool), rgb8 );
	    break;
	case PPpropBOTTOM_BORDER:
	    tedBorderSetExplicitColorChoice(
					&(pot->potBottomBorderTool), rgb8 );
	    break;
	case PPpropLEFT_BORDER:
	    tedBorderSetExplicitColorChoice(
					&(pot->potLeftBorderTool), rgb8 );
	    break;
	case PPpropRIGHT_BORDER:
	    tedBorderSetExplicitColorChoice(
					&(pot->potRightBorderTool), rgb8 );
	    break;

	case PPpropBOX_BORDER:
	    LDEB(which); return;
	    break;
	case PPpropBETWEEN_BORDER:
	    LDEB(which); return;
	    break;
	case PPpropBAR_BORDER:
	    LDEB(which); return;
	    break;

	case PPpropSHADE_BACK_COLOR:
	case PPpropSHADE_FORE_COLOR:
	    tedShadeSetExplicitColorChoice( &(pot->potShadingTool),
							    which, rgb8 );
	    break;

	default:
	    LDEB(which); return;
	}
    }

/************************************************************************/
/*									*/
/*  Make the paragraph layout page.					*/
/*									*/
/************************************************************************/

void tedFormatFillParagraphOrnamentsPage(
			ParagraphOrnamentsTool *		pot,
			const ParagraphOrnamentsPageResources *	popr,
			AppInspector *				ai,
			int					subjectPage,
			InspectorSubject *			is,
			APP_WIDGET				pageWidget,
			const InspectorSubjectResources *	isr )
    {
    APP_WIDGET		row;

    /**/
    pot->potPageResources= popr;
    is->isPrivate= pot;
    is->isGotColor= tedParaOrnamentsGotColor;

    /**/
    docInitParagraphProperties( &(pot->potPropertiesSet) );
    docInitParagraphProperties( &(pot->potPropertiesChosen) );

    /**/
    tedMakeBorderTool( &(pot->potTopBorderTool), ai, pageWidget,
	    popr->poprTopBorder, &(popr->poprBorderToolResources),
	    subjectPage, PPpropTOP_BORDER );
    tedMakeBorderTool( &(pot->potBottomBorderTool), ai, pageWidget,
	    popr->poprBottomBorder, &(popr->poprBorderToolResources),
	    subjectPage, PPpropBOTTOM_BORDER );
    tedMakeBorderTool( &(pot->potLeftBorderTool), ai, pageWidget,
	    popr->poprLeftBorder, &(popr->poprBorderToolResources),
	    subjectPage, PPpropLEFT_BORDER );
    tedMakeBorderTool( &(pot->potRightBorderTool), ai, pageWidget,
	    popr->poprRightBorder, &(popr->poprBorderToolResources),
	    subjectPage, PPpropRIGHT_BORDER );

    /**/
    tedFormatMakeShadingTool( &(pot->potShadingTool), ai, pageWidget,
		popr->poprShading, &(popr->poprShadingResources),
		subjectPage, PPpropSHADE_FORE_COLOR, PPpropSHADE_BACK_COLOR,
		(TedShadingToolCallback)0, (void *)pot );


    /**/
    appInspectorMakeButtonRow( &row, pageWidget,
			&(is->isPrevButton), &(is->isNextButton),
			isr->isrPrevButtonText, isr->isrNextButtonText,
			tedFormatPrevPara, tedFormatNextPara, pot );

    appInspectorMakeButtonRow( &row, pageWidget,
			&(is->isSelectButton), &(is->isDeleteButton),
			isr->isrSelectButtonText, isr->isrDeleteButtonText,
			tedFormatSelectPara, tedFormatDeletePara, pot );

    appInspectorMakeButtonRow( &row, pageWidget,
			&(is->isInsertButton), &(is->isAppendButton),
			isr->isrInsertButtonText, isr->isrAppendButtonText,
			tedFormatInsertPara, tedFormatAppendPara, pot );

    appInspectorMakeButtonRow( &row, pageWidget,
			&(is->isRevertButton), &(is->isApplyButton),
			isr->isrRevert, isr->isrApplyToSubject,
			tedFormatParaRevertPushed, tedFormatChangePara, pot );

    return;
    }

/************************************************************************/
/*									*/
/*  Finish the paragraph page.						*/
/*									*/
/************************************************************************/

void tedFormatFillParagraphOrnamentsChoosers(
			    ParagraphOrnamentsTool *			pot,
			    const ParagraphOrnamentsPageResources *	popr )
    {
    return;
    }

void tedFormatFinishParaOrnamentsPage(
			    ParagraphOrnamentsTool *			pot,
			    TedFormatTool *				tft,
			    const ParagraphOrnamentsPageResources *	popr )
    {
    tedFinishBorderTool( &(pot->potTopBorderTool) );
    tedFinishBorderTool( &(pot->potBottomBorderTool) );
    tedFinishBorderTool( &(pot->potLeftBorderTool) );
    tedFinishBorderTool( &(pot->potRightBorderTool) );

    tedFinishShadingTool( &(pot->potShadingTool) );

    return;
    }

/************************************************************************/
/*									*/
/*  Init/Clean the paragraph ornaments tool.				*/
/*									*/
/************************************************************************/

void tedInitParaOrnamentsTool(	ParagraphOrnamentsTool *	pot )
    {
    pot->potApplication= (EditApplication *)0;
    pot->potInspector= (AppInspector *)0;
    pot->potPageResources= (const ParagraphOrnamentsPageResources *)0;

    docInitParagraphProperties( &(pot->potPropertiesSet) );
    docInitParagraphProperties( &(pot->potPropertiesChosen) );

    tedInitBorderTool( &(pot->potTopBorderTool) );
    tedInitBorderTool( &(pot->potBottomBorderTool) );
    tedInitBorderTool( &(pot->potLeftBorderTool) );
    tedInitBorderTool( &(pot->potRightBorderTool) );
    
    tedInitShadingTool( &(pot->potShadingTool) );

    return;
    }

void tedCleanParaOrnamentsTool(	ParagraphOrnamentsTool *	pot )
    {
    docCleanParagraphProperties( &(pot->potPropertiesSet) );
    docCleanParagraphProperties( &(pot->potPropertiesChosen) );

    tedCleanBorderTool( &(pot->potTopBorderTool) );
    tedCleanBorderTool( &(pot->potBottomBorderTool) );
    tedCleanBorderTool( &(pot->potLeftBorderTool) );
    tedCleanBorderTool( &(pot->potRightBorderTool) );
    
    tedCleanShadingTool( &(pot->potShadingTool) );

    return;
    }

/************************************************************************/
/*									*/
/*  Get paragraph borders and shading tool resources.			*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedParagraphSubjectResourceTable[]=
    {
    APP_RESOURCE( "formatToolParaOrnaments",
	offsetof(InspectorSubjectResources,isrSubjectName),
	"Paragraph Borders and Shading" ),
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
    APP_RESOURCE( "formatToolParaTopBorder",
	offsetof(ParagraphOrnamentsPageResources,poprTopBorder),
	"Top Border" ),
    APP_RESOURCE( "formatToolParaBottomBorder",
	offsetof(ParagraphOrnamentsPageResources,poprBottomBorder),
	"Bottom Border" ),
    APP_RESOURCE( "formatToolParaLeftBorder",
	offsetof(ParagraphOrnamentsPageResources,poprLeftBorder),
	"Left Border" ),
    APP_RESOURCE( "formatToolParaRightBorder",
	offsetof(ParagraphOrnamentsPageResources,poprRightBorder),
	"Right Border" ),

    /**/
    APP_RESOURCE( "formatToolParaBorderWidth",
	offsetof(ParagraphOrnamentsPageResources,
					poprBorderToolResources.btrWidth),
	"Width" ),
    APP_RESOURCE( "formatToolParaBorderStyle",
	offsetof(ParagraphOrnamentsPageResources,
					poprBorderToolResources.btrStyle),
	"Style" ),
    APP_RESOURCE( "formatToolParaBorderColor",
	offsetof(ParagraphOrnamentsPageResources,
					poprBorderToolResources.btrColor),
	"Color" ),
    APP_RESOURCE( "formatToolParaBorderColorChooserAutomatic",
	offsetof(ParagraphOrnamentsPageResources,poprBorderToolResources.
			    btrColorChooserResources.ccrAutomaticColor),
	"Automatic" ),
    APP_RESOURCE( "formatToolParaBorderColorChooserMoreColors",
	offsetof(ParagraphOrnamentsPageResources,poprBorderToolResources.
			    btrColorChooserResources.ccrMoreColors),
	"More Colors..." ),

    /**/
    APP_RESOURCE( "formatToolParaShading",
	offsetof(ParagraphOrnamentsPageResources,poprShading),
	"Shading" ),

    APP_RESOURCE( "formatToolParaShadingPattern",
	offsetof(ParagraphOrnamentsPageResources,
					poprShadingResources.strPattern),
	"Pattern" ),
    APP_RESOURCE( "formatToolParaShadingLevel",
	offsetof(ParagraphOrnamentsPageResources,
					poprShadingResources.strLevel),
	"Level" ),
    APP_RESOURCE( "formatToolParaShadingForeColor",
	offsetof(ParagraphOrnamentsPageResources,
					poprShadingResources.strForeColor),
	"Foreground" ),
    APP_RESOURCE( "formatToolParaShadingBackColor",
	offsetof(ParagraphOrnamentsPageResources,
					poprShadingResources.strBackColor),
	"Background" ),
    /**/
    APP_RESOURCE( "formatToolParaShadingBackColorChooserNoBackground",
	offsetof(ParagraphOrnamentsPageResources,poprShadingResources.
			    strBackColorChooserResources.ccrAutomaticColor),
	"No Background" ),
    APP_RESOURCE( "formatToolParaShadingBackColorChooserMoreColors",
	offsetof(ParagraphOrnamentsPageResources,poprShadingResources.
			    strBackColorChooserResources.ccrMoreColors),
	"More Colors..." ),

    APP_RESOURCE( "formatToolParaShadingForeColorChooserAutomatic",
	offsetof(ParagraphOrnamentsPageResources,poprShadingResources.
			    strForeColorChooserResources.ccrAutomaticColor),
	"Automatic" ),
    APP_RESOURCE( "formatToolParaShadingForeColorChooserMoreColors",
	offsetof(ParagraphOrnamentsPageResources,poprShadingResources.
			    strForeColorChooserResources.ccrMoreColors),
	"More Colors..." ),

    };

void tedFormatToolGetParaOrnamentsResourceTable(
			    EditApplication *			ea,
			    ParagraphOrnamentsPageResources *	popr,
			    InspectorSubjectResources *		isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)popr,
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
