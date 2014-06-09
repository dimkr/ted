/************************************************************************/
/*									*/
/*  The 'Format Tool':							*/
/*									*/
/*  An inpector with pages for the different formatting properties of	*/
/*  the document.							*/
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
/*  Resources for the format tool.					*/
/*									*/
/************************************************************************/

typedef struct TedFormatToolResources
    {
    InspectorSubjectResources	tttrSubjectResources[TEDtsi_COUNT];

    TablePageResources			tttrTablePageResources;
    RowPageResources			tttrRowPageResources;
    ColumnPageResources			tttrColumnPageResources;
    CellPageResources			tttrCellPageResources;
    ParagraphLayoutPageResources	tttrParaLayoutPageResources;
    TabsPageResources			tttrTabsPageResources;
    ParagraphOrnamentsPageResources	tttrParaOrnamentsPageResources;
    ListsPageResources			tttrListsPageResources;
    SectionPageResources		tttrSectPageResources;
    PageLayoutPageResources		tttrPageLayoutPageResources;
    HeaderFooterPageResources		tttrHeadFootPageResources;
    NotesPageResources			tttrNotesPageResources;
    AppFontToolResources		tttrFontToolResources;
    RgbChooserPageResources		tttrRgbChooserPageResources;
    LinkToolResources			tttrLinkToolResources;
    ImagePageResources			tttrImagePageResources;
    } TedFormatToolResources;

static void tedFormatFillPages(	const TedFormatToolResources *	tftr,
				TedFormatTool *			tft,
				AppInspector *			ai )
    {
    tedFormatFillTablePage( &(tft->tftTableTool),
			    &(tftr->tttrTablePageResources),
			    &(ai->aiSubjects[TEDtsiTABLE]),
			    ai->aiSubjects[TEDtsiTABLE].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiTABLE]) );

    tedFormatFillRowPage( &(tft->tftRowTool),
			    &(tftr->tttrRowPageResources),
			    ai, TEDtsiROW,
			    &(ai->aiSubjects[TEDtsiROW]),
			    ai->aiSubjects[TEDtsiROW].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiROW]) );

    tedFormatFillColumnPage( &(tft->tftColumnTool),
			    &(tftr->tttrColumnPageResources),
			    ai, TEDtsiCOLUMN,
			    &(ai->aiSubjects[TEDtsiCOLUMN]),
			    ai->aiSubjects[TEDtsiCOLUMN].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiCOLUMN]) );

    tedFormatFillCellPage( &(tft->tftCellTool),
			    &(tftr->tttrCellPageResources),
			    ai, TEDtsiCELL,
			    &(ai->aiSubjects[TEDtsiCELL]),
			    ai->aiSubjects[TEDtsiCELL].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiCELL]) );

    tedFormatFillParagraphLayoutPage( &(tft->tftParagraphLayoutTool),
			    &(tftr->tttrParaLayoutPageResources),
			    &(ai->aiSubjects[TEDtsiPARA_LAY]),
			    ai->aiSubjects[TEDtsiPARA_LAY].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiPARA_LAY]) );

    tedFormatFillTabsPage( &(tft->tftTabsTool),
			    &(tftr->tttrTabsPageResources),
			    &(ai->aiSubjects[TEDtsiTABS]),
			    ai->aiSubjects[TEDtsiTABS].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiTABS]) );

    tedFormatFillParagraphOrnamentsPage( &(tft->tftParagraphOrnamentsTool),
			    &(tftr->tttrParaOrnamentsPageResources),
			    ai, TEDtsiPARA_ORN,
			    &(ai->aiSubjects[TEDtsiPARA_ORN]),
			    ai->aiSubjects[TEDtsiPARA_ORN].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiPARA_ORN]) );

    tedFormatFillListsPage( &(tft->tftListsTool),
			    &(tftr->tttrListsPageResources),
			    &(ai->aiSubjects[TEDtsiLISTS]),
			    ai->aiSubjects[TEDtsiLISTS].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiLISTS]) );

    tedFormatFillSectionPage( &(tft->tftSectionTool),
			    &(tftr->tttrSectPageResources),
			    &(ai->aiSubjects[TEDtsiSECT]),
			    ai->aiSubjects[TEDtsiSECT].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiSECT]) );

    tedFormatFillPageLayoutPage( &(tft->tftPageLayoutTool),
			    &(tftr->tttrPageLayoutPageResources),
			    &(ai->aiSubjects[TEDtsiPAGE]),
			    ai->aiSubjects[TEDtsiPAGE].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiPAGE]) );

    tedFormatFillHeaderFooterPage( &(tft->tftHeaderFooterTool),
			    &(tftr->tttrHeadFootPageResources),
			    &(ai->aiSubjects[TEDtsiHEADFOOT]),
			    ai->aiSubjects[TEDtsiHEADFOOT].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiHEADFOOT]) );

    tedFormatFillNotesPage( &(tft->tftNotesTool),
			    &(tftr->tttrNotesPageResources),
			    &(ai->aiSubjects[TEDtsiNOTES]),
			    ai->aiSubjects[TEDtsiNOTES].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiNOTES]) );

    appFontToolFillPage( &(tft->tftFontTool),
			    &(tftr->tttrFontToolResources),
			    TEDtsiFONT,
			    &(ai->aiSubjects[TEDtsiFONT]),
			    ai->aiSubjects[TEDtsiFONT].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiFONT]) );

    appFontToolFillPage( &(tft->tftListFontTool),
			    &(tftr->tttrFontToolResources),
			    TEDtsiLISTFONT,
			    &(ai->aiSubjects[TEDtsiLISTFONT]),
			    ai->aiSubjects[TEDtsiLISTFONT].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiLISTFONT]) );

    tedFormatFillLinkPage( &(tft->tftLinkTool),
			    &(tftr->tttrLinkToolResources),
			    &(ai->aiSubjects[TEDtsiLINK]),
			    ai->aiSubjects[TEDtsiLINK].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiLINK]) );

    tedFormatFillImagePage( &(tft->tftImageTool),
			    &(tftr->tttrImagePageResources),
			    &(ai->aiSubjects[TEDtsiIMAGE]),
			    ai->aiSubjects[TEDtsiIMAGE].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiIMAGE]) );

    /**/
    appRgbChooserPageFillPage( &(tft->tftRgbPage),
			    &(tftr->tttrRgbChooserPageResources),
			    &(ai->aiSubjects[TEDtsiRGB]),
			    ai->aiSubjects[TEDtsiRGB].isPage,
			    &(tftr->tttrSubjectResources[TEDtsiRGB]) );

    /**/
    appInspectorSetRgbPage( ai, (void *)&(tft->tftRgbPage), TEDtsiRGB );

    return;
    }

/************************************************************************/
/*									*/
/*  The user destroys the format tool.					*/
/*									*/
/************************************************************************/

static void tedDestroyFormatTool(	void *	voidtft )
    {
    TedFormatTool *	tft= (TedFormatTool *)voidtft;

    EditApplication *	ea= tft->tftApplication;
    TedAppResources *	tar= (TedAppResources *)ea->eaResourceData;

    tedFormatCleanTableTool( &(tft->tftTableTool) );
    tedCleanRowTool( &(tft->tftRowTool) );
    tedCleanColumnTool( &(tft->tftColumnTool) );
    tedCleanCellTool( &(tft->tftCellTool) );
    tedFormatCleanParaLayoutTool( &(tft->tftParagraphLayoutTool) );
    tedFormatCleanParaTabsTool( &(tft->tftTabsTool) );
    tedCleanParaOrnamentsTool( &(tft->tftParagraphOrnamentsTool) );
    tedFormatCleanListsTool( &(tft->tftListsTool) );
    tedFormatCleanSectionTool( &(tft->tftSectionTool) );
    tedFormatCleanPageLayoutTool( &(tft->tftPageLayoutTool) );
    tedFormatCleanHeaderFooterTool( &(tft->tftHeaderFooterTool) );
    tedFormatCleanNotesTool( &(tft->tftNotesTool) );
    appFontChooserCleanPage( &(tft->tftFontTool) );
    appFontChooserCleanPage( &(tft->tftListFontTool) );
    tedFormatCleanLinkTool( &(tft->tftLinkTool) );
    tedFormatCleanImageTool( &(tft->tftImageTool) );

    appRgbChooserPageCleanPage( &(tft->tftRgbPage) );

    free( tft );

    tar->tarInspector= (void *)0;

    return;
    }

/************************************************************************/
/*									*/
/*  Called when the user switches to another page of the format tool.	*/
/*									*/
/************************************************************************/

static void tedFormatToolNotifySubject(	AppInspector *		ai,
					int			subjectFrom,
					int			subjectTo )
    {
    if  ( subjectFrom == TEDtsiRGB && subjectTo != TEDtsiRGB )
	{
	const int	enabled= 0;

	appEnableInspectorSubject( ai, TEDtsiRGB, enabled );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  make/show the format tool.						*/
/*									*/
/************************************************************************/

void tedShowFormatTool(	APP_WIDGET		option,
			EditApplication *	ea )
    {
    AppInspector *			ai;
    TedFormatTool *			tft;

    static TedFormatToolResources	tftr;
    static int				gotResources;

    TedAppResources *			tar;

    const char *		widgetName= "tedFormatTool";
    const char *		pixmapName= "tedtable";

    tar= (TedAppResources *)ea->eaResourceData;

    if  ( tar->tarInspector )
	{
	ai= tar->tarInspector;

	appShowShellWidget( ai->aiTopWidget );

	return;
	}

    if  ( ! gotResources )
	{
	tedFormatToolGetTableResourceTable( ea,
				&(tftr.tttrTablePageResources),
				&(tftr.tttrSubjectResources[TEDtsiTABLE]) );

	tedFormatToolGetRowResourceTable( ea,
				&(tftr.tttrRowPageResources),
				&(tftr.tttrSubjectResources[TEDtsiROW]) );

	tedFormatToolGetColumnResourceTable( ea,
				&(tftr.tttrColumnPageResources),
				&(tftr.tttrSubjectResources[TEDtsiCOLUMN]) );

	tedFormatToolGetCellResourceTable( ea,
				&(tftr.tttrCellPageResources),
				&(tftr.tttrSubjectResources[TEDtsiCELL]) );

	tedFormatToolGetParaLayoutResourceTable( ea,
				&(tftr.tttrParaLayoutPageResources),
				&(tftr.tttrSubjectResources[TEDtsiPARA_LAY]) );

	tedFormatToolGetTabsResourceTable( ea,
				&(tftr.tttrTabsPageResources),
				&(tftr.tttrSubjectResources[TEDtsiTABS]) );

	tedFormatToolGetParaOrnamentsResourceTable( ea,
				&(tftr.tttrParaOrnamentsPageResources),
				&(tftr.tttrSubjectResources[TEDtsiPARA_ORN]) );

	tedFormatToolGetListsResourceTable( ea,
				&(tftr.tttrListsPageResources),
				&(tftr.tttrSubjectResources[TEDtsiLISTS]) );

	tedFormatToolGetSectResourceTable( ea,
				&(tftr.tttrSectPageResources),
				&(tftr.tttrSubjectResources[TEDtsiSECT]) );

	appPageLayoutPageGetResourceTable( ea,
				&(tftr.tttrPageLayoutPageResources),
				&(tftr.tttrSubjectResources[TEDtsiPAGE]) );

	tedFormatToolGetHeaderFooterResourceTable( ea,
				&(tftr.tttrHeadFootPageResources),
				&(tftr.tttrSubjectResources[TEDtsiHEADFOOT]) );

	tedFormatToolGetNotesResourceTable( ea,
				&(tftr.tttrNotesPageResources),
				&(tftr.tttrSubjectResources[TEDtsiNOTES]) );

	tedFontToolGetResourceTable( ea,
				&(tftr.tttrFontToolResources),
				&(tftr.tttrSubjectResources[TEDtsiFONT]) );

	tedListFontToolGetResourceTable( ea,
				&(tftr.tttrSubjectResources[TEDtsiLISTFONT]) );

	tedLinkToolGetResourceTable( ea,
				&(tftr.tttrLinkToolResources),
				&(tftr.tttrSubjectResources[TEDtsiLINK]) );

	tedImageToolGetResourceTable( ea,
				&(tftr.tttrImagePageResources),
				&(tftr.tttrSubjectResources[TEDtsiIMAGE]) );

	appRgbChooserPageGetResourceTable( ea,
				&(tftr.tttrRgbChooserPageResources),
				&(tftr.tttrSubjectResources[TEDtsiRGB]) );

	gotResources= 1;
	}

    tft= (TedFormatTool *)malloc( sizeof(TedFormatTool) );
    if  ( ! tft )
	{ XDEB(tft); return;	}

    tft->tftApplication= ea;

    /******/
    tedInitColumnTool( &(tft->tftColumnTool) );
    tedInitCellTool( &(tft->tftCellTool) );
    tedInitParaOrnamentsTool( &(tft->tftParagraphOrnamentsTool) );
    tedInitRowTool( &(tft->tftRowTool) );
    tedFormatInitLinkTool( &(tft->tftLinkTool) );
    tedFormatInitImageTool( &(tft->tftImageTool) );

    /******/
    tft->tftTableTool.ttApplication= ea;
    tft->tftRowTool.rtApplication= ea;
    tft->tftColumnTool.ctApplication= ea;
    tft->tftCellTool.ctApplication= ea;
    tft->tftParagraphLayoutTool.ptApplication= ea;
    tft->tftTabsTool.ttApplication= ea;
    tft->tftParagraphOrnamentsTool.potApplication= ea;
    tft->tftListsTool.ltApplication= ea;
    tft->tftSectionTool.stApplication= ea;
    tft->tftPageLayoutTool.pltApplication= ea;
    tft->tftHeaderFooterTool.hftApplication= ea;
    tft->tftNotesTool.ntApplication= ea;
    tft->tftFontTool.afcApplication= ea;
    tft->tftListFontTool.afcApplication= ea;
    tft->tftLinkTool.ltApplication= ea;
    tft->tftImageTool.itApplication= ea;
    tft->tftRgbPage.rcpApplication= ea;

    ai= appMakeInspector( ea, option, pixmapName, widgetName,
				    tftr.tttrSubjectResources, TEDtsi_COUNT,
				    tedDestroyFormatTool, (void *)tft );

    tft->tftInspector= ai;
    ai->aiNotifySubject= tedFormatToolNotifySubject;

    /******/
    tft->tftTableTool.ttInspector= ai;
    tft->tftRowTool.rtInspector= ai;
    tft->tftColumnTool.ctInspector= ai;
    tft->tftCellTool.ctInspector= ai;
    tft->tftParagraphLayoutTool.ptInspector= ai;
    tft->tftTabsTool.ttInspector= ai;
    tft->tftParagraphOrnamentsTool.potInspector= ai;
    tft->tftListsTool.ltInspector= ai;
    tft->tftSectionTool.stInspector= ai;
    tft->tftPageLayoutTool.pltInspector= ai;
    tft->tftHeaderFooterTool.hftInspector= ai;
    tft->tftNotesTool.ntInspector= ai;
    tft->tftFontTool.afcInspector= ai;
    tft->tftListFontTool.afcInspector= ai;
    tft->tftLinkTool.ltInspector= ai;
    tft->tftImageTool.itInspector= ai;
    tft->tftRgbPage.rcpInspector= ai;

    /******/

    tedFormatFillPages( &tftr, tft, ai );

    /******/

    tft->tftFontTool.afcSetFont= tedFontToolSet;
    tft->tftListFontTool.afcSetFont= tedListFontToolSet;

    /******/
    /*tedFormatFillTableChoosers()*/
    tedFormatFillRowChoosers( &(tft->tftRowTool),
					    &(tftr.tttrRowPageResources) );
    tedColumnToolFillChoosers( &(tft->tftColumnTool),
					&(tftr.tttrColumnPageResources) );
    tedCellToolFillChoosers( &(tft->tftCellTool),
					&(tftr.tttrCellPageResources) );
    tedFormatFillParagraphLayoutChoosers( &(tft->tftParagraphLayoutTool),
				    &(tftr.tttrParaLayoutPageResources) );
    tedTabsToolFillChoosers( &(tft->tftTabsTool) );
    tedFormatFillParagraphOrnamentsChoosers( &(tft->tftParagraphOrnamentsTool),
				    &(tftr.tttrParaOrnamentsPageResources) );
    tedFormatFillListChoosers( &(tft->tftListsTool) );
    tedFormatFillSectionChoosers( &(tft->tftSectionTool),
					    &(tftr.tttrSectPageResources) );
    appPageLayoutPageFillChoosers( &(tft->tftPageLayoutTool),
					&(tftr.tttrPageLayoutPageResources) );
    tedFormatFillHeaderFooterChoosers( &(tft->tftHeaderFooterTool),
					&(tftr.tttrHeadFootPageResources) );
    tedFormatFillNotesChoosers( &(tft->tftNotesTool),
					&(tftr.tttrNotesPageResources) );
    appFontToolFillChoosers( &(tft->tftFontTool),
					&(tftr.tttrFontToolResources) );
    appFontToolFillChoosers( &(tft->tftListFontTool),
					&(tftr.tttrFontToolResources) );
    tedLinkToolFillChoosers( &(tft->tftLinkTool),
					&(tftr.tttrLinkToolResources) );
    /******/

    appFinishInspector( ai );

    tedFormatFinishRowPage( &(tft->tftRowTool), tft,
					    &(tftr.tttrRowPageResources) );
    tedFormatFinishParaLayoutPage( &(tft->tftParagraphLayoutTool), tft,
					&(tftr.tttrParaLayoutPageResources) );
    tedFormatFinishTabsPage( &(tft->tftTabsTool) );
    tedFormatFinishParaOrnamentsPage( &(tft->tftParagraphOrnamentsTool), tft,
				    &(tftr.tttrParaOrnamentsPageResources) );
    tedFormatFinishListPage( &(tft->tftListsTool) );
    tedFormatFinishSectionPage( &(tft->tftSectionTool), tft,
					    &(tftr.tttrSectPageResources) );
    tedFormatFinishPageLayoutPage( &(tft->tftPageLayoutTool), tft,
					&(tftr.tttrPageLayoutPageResources) );
    tedFormatFinishHeaderFooterPage( &(tft->tftHeaderFooterTool), tft,
					&(tftr.tttrHeadFootPageResources) );
    tedFormatFinishColumnPage( &(tft->tftColumnTool), tft,
					&(tftr.tttrColumnPageResources) );
    tedFormatFinishCellPage( &(tft->tftCellTool), tft,
					&(tftr.tttrCellPageResources) );
    tedFormatFinishNotesPage( &(tft->tftNotesTool), tft,
					&(tftr.tttrNotesPageResources) );
    appFontToolFinishPage( &(tft->tftFontTool),
					&(tftr.tttrFontToolResources) );
    appFontToolFinishPage( &(tft->tftListFontTool),
					&(tftr.tttrFontToolResources) );
    tedFinishLinkTool( &(tft->tftLinkTool),
					&(tftr.tttrLinkToolResources) );
    appRgbChooserPageFinishPage( &(tft->tftRgbPage),
					&(tftr.tttrRgbChooserPageResources) );

    tar->tarInspector= ai;

    return;
    }

/************************************************************************/
/*									*/
/*  Ask the table tool to adapt itself to the selection.		*/
/*									*/
/************************************************************************/

void tedFormatShowPagePage(	EditApplication *	ea )
    {
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    if  ( ! tar->tarInspector )
	{ XDEB(tar->tarInspector); return;	}

    appEnableInspectorSubject( tar->tarInspector, TEDtsiPAGE, 1 );

    appInspectorSelectSubject( tar->tarInspector, TEDtsiPAGE );

    return;
    }

void tedFormatShowLinkPage(	EditApplication *	ea )
    {
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    if  ( ! tar->tarInspector )
	{ XDEB(tar->tarInspector); return;	}

    appEnableInspectorSubject( tar->tarInspector, TEDtsiLINK, 1 );

    appInspectorSelectSubject( tar->tarInspector, TEDtsiLINK );

    return;
    }

static void tedFormatRefreshToolPages(	int *				enabled,
					int *				prefs,
					TedFormatTool *			tft,
					AppInspector *			ai,
					EditDocument *			ed,
					const DocumentSelection *	ds,
					const SelectionGeometry *	sg,
					const SelectionDescription *	sd )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    tedFormatToolRefreshTableTool( &(tft->tftTableTool),
				    enabled+ TEDtsiTABLE,
				    prefs+ TEDtsiTABLE,
				    ai->aiSubjects+ TEDtsiTABLE, ds );

    tedFormatToolRefreshColumnTool( &(tft->tftColumnTool),
				    enabled+ TEDtsiCOLUMN,
				    prefs+ TEDtsiCOLUMN,
				    ai->aiSubjects+ TEDtsiCOLUMN, ds );

    tedFormatToolRefreshRowTool( &(tft->tftRowTool),
				    enabled+ TEDtsiROW,
				    prefs+ TEDtsiROW,
				    ai->aiSubjects+ TEDtsiROW, ds );

    tedFormatToolRefreshCellTool( &(tft->tftCellTool),
				    enabled+ TEDtsiCELL,
				    prefs+ TEDtsiCELL,
				    ai->aiSubjects+ TEDtsiCELL, ds );

    tedFormatToolRefreshParaLayoutTool(  &(tft->tftParagraphLayoutTool),
				    enabled+ TEDtsiPARA_LAY,
				    prefs+ TEDtsiPARA_LAY,
				    ai->aiSubjects+ TEDtsiPARA_LAY, ds );

    tedFormatToolRefreshTabsTool( &(tft->tftTabsTool),
				    enabled+ TEDtsiTABS,
				    prefs+ TEDtsiTABS,
				    ai->aiSubjects+ TEDtsiTABS,
				    ds, &(bd->bdProperties) );

    tedFormatToolRefreshParaOrnamentsTool(  &(tft->tftParagraphOrnamentsTool),
				    enabled+ TEDtsiPARA_ORN,
				    prefs+ TEDtsiPARA_ORN,
				    ai->aiSubjects+ TEDtsiPARA_ORN, ds );

    tedFormatToolRefreshListTool(  &(tft->tftListsTool),
				    enabled+ TEDtsiLISTS,
				    prefs+ TEDtsiLISTS,
				    ai->aiSubjects+ TEDtsiLISTS,
				    ds, sd, bd );

    tedFormatToolRefreshSectionTool( &(tft->tftSectionTool),
				    enabled+ TEDtsiSECT,
				    prefs+ TEDtsiSECT,
				    ai->aiSubjects+ TEDtsiSECT, ds );

    tedFormatToolRefreshPageLayoutTool( &(tft->tftPageLayoutTool),
				    enabled+ TEDtsiPAGE,
				    prefs+ TEDtsiPAGE,
				    ai->aiSubjects+ TEDtsiPAGE,
				    ds, &(bd->bdProperties) );

    tedFormatToolRefreshHeaderFooterTool( &(tft->tftHeaderFooterTool),
				    enabled+ TEDtsiHEADFOOT,
				    prefs+ TEDtsiHEADFOOT,
				    ai->aiSubjects+ TEDtsiHEADFOOT,
				    ds, sd, sg, bd );

    tedFormatToolRefreshNotesTool( &(tft->tftNotesTool),
				    enabled+ TEDtsiNOTES,
				    prefs+ TEDtsiNOTES,
				    ai->aiSubjects+ TEDtsiNOTES,
				    ds, bd );

    tedRefreshFontTool( &(tft->tftFontTool),
				    enabled+ TEDtsiFONT,
				    prefs+ TEDtsiFONT,
				    ai->aiSubjects+ TEDtsiFONT,
				    ds, sd, ed );

    tedRefreshListFontTool( &(tft->tftListFontTool),
				    enabled+ TEDtsiLISTFONT,
				    prefs+ TEDtsiLISTFONT,
				    ai->aiSubjects+ TEDtsiLISTFONT,
				    ds, sd, bd );

    tedRefreshLinkTool( &(tft->tftLinkTool),
				    enabled+ TEDtsiLINK,
				    prefs+ TEDtsiLINK,
				    ai->aiSubjects+ TEDtsiLINK,
				    ds, sd, bd );

    tedRefreshImageTool( &(tft->tftImageTool),
				    enabled+ TEDtsiIMAGE,
				    prefs+ TEDtsiIMAGE,
				    ai->aiSubjects+ TEDtsiIMAGE,
				    ds, sd, bd );

    /*  no refresh  */
    enabled[TEDtsiRGB]= 0;

    return;
    }

void tedFormatToolAdaptToSelection( AppInspector *		ai,
				EditDocument *			ed,
				int				choosePage,
				const DocumentSelection *	ds,
				const SelectionGeometry *	sg,
				const SelectionDescription *	sd )
    {
    TedFormatTool *		tft= (TedFormatTool *)ai->aiTarget;

    int				subject;
    int				enabled[TEDtsi_COUNT];
    int				prefs[TEDtsi_COUNT];
    int				highest= -1;
    int				preferred= -1;

    for ( subject= 0; subject < TEDtsi_COUNT; subject++ )
	{
	enabled[subject]= 1;
	prefs[subject]= 5;
	}

    if  ( ! ds->dsBegin.dpBi )
	{ appEnableInspector( ai, 0 ); return; }
    else{
	tedFormatRefreshToolPages( enabled, prefs, tft, ai, ed, ds, sg, sd );

	if  ( ed->edFileReadOnly )
	    { appEnableInspector( ai, 0 );	}
	else{ appEnableInspector( ai, 1 );	}
	}

    for ( subject= 0; subject < TEDtsi_COUNT; subject++ )
	{
	appEnableInspectorSubject( ai, subject,
				    enabled[subject] != 0 );

	if  ( enabled[subject] && prefs[subject] > highest )
	    { preferred= subject; highest= prefs[subject]; }
	}

    if  ( ai->aiCurrentSubject < 0				||
	  choosePage						||
	  ! ai->aiSubjects[ai->aiCurrentSubject].isEnabled	)
	{ appInspectorSelectSubject( ai, preferred );	}
    }

