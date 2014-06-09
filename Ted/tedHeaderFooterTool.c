/************************************************************************/
/*									*/
/*  A tool to manage the headers and footers of a Document/Section.	*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>
#   include	<limits.h>

#   include	"tedApp.h"
#   include	"tedFormatTool.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Set all the controls on the Header/Footer tool, to reflect the	*/
/*  section properties currently chosen.				*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshHeadFootPage(	HeaderFooterTool *	hft )
    {
    const SectionProperties *	sp= &(hft->hftSectPropertiesChosen);
    const DocumentProperties *	dp= &(hft->hftDocPropertiesChosen);

    int				i;

    appIntegerToTextWidget( hft->hftSectionNumberText,
						hft->hftSectionNumber+ 1 );

    appGuiSetToggleState( hft->hftTitlepgToggle, sp->spHasTitlePage );

    appGuiSetToggleState( hft->hftFacingpToggle, dp->dpHasFacingPages );

    for ( i= 0; i < PAGES__COUNT; i++ )
	{
	appGuiEnableWidget( hft->hftPagesItems[i],
					hft->hftPagesOptionsEnabled[i] );
	}

    if  ( hft->hftPagesChosen < 0				||
	  hft->hftPagesChosen >= PAGES__COUNT			||
	  ! hft->hftPagesOptionsEnabled[hft->hftPagesChosen]	)
	{
	for ( i= 0; i < PAGES__COUNT; i++ )
	    {
	    if  ( hft->hftPagesOptionsEnabled[i] )
		{
		appSetOptionmenu( &(hft->hftPagesOptionmenu), i );
		hft->hftPagesChosen= i;
		break;
		}
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt the Header/Footer tool to the current selection in the text.	*/
/*									*/
/************************************************************************/

void tedFormatToolRefreshHeaderFooterTool(
				HeaderFooterTool *		hft,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg,
				BufferDocument *		bd )
    {
    const DocumentProperties *	dpSet= &(bd->bdProperties);

    PropertyMask		changed;
    PropertyMask		update;

    BufferItem *		selSectBi;
    const SectionProperties *	spSet;

    int				whatHeaderFooter;

    PROPmaskCLEAR( &changed );

    selSectBi= ds->dsBegin.dpBi;
    while( selSectBi && selSectBi->biLevel > DOClevSECT )
	{ selSectBi= selSectBi->biParent;	}
    if  ( ! selSectBi )
	{ XDEB(selSectBi); return;	}

    whatHeaderFooter= selSectBi->biInExternalItem;
    if  ( whatHeaderFooter == DOCinBODY )
	{
	ExternalItem *	ei;
	int		isEmpty;
	int		page= sg->sgBegin.pgTopPosition.lpPage;

	whatHeaderFooter= docWhatPageHeader( &ei, &isEmpty,
						    selSectBi, page, dpSet );

	hft->hftSectionNumber= selSectBi->biNumberInParent;
	spSet= &(selSectBi->biSectProperties);
	}
    else{
	const SelectionScope *	ss= &(selSectBi->biSectSelectionScope);
	const BufferItem *	docBi= &(bd->bdItem);
	const BufferItem *	bodySectBi;

	if  ( ! sd->sdInHeaderFooter )
	    { *pEnabled= 0; return;	}

	bodySectBi= docBi->biChildren[ss->ssSectNrExternalTo];

	hft->hftSectionNumber= bodySectBi->biNumberInParent;
	spSet= &(bodySectBi->biSectProperties);

	(*pPref) += 2;
	}

    /**********/

    PROPmaskCLEAR( &update );
    PROPmaskADD( &update, DPpropFACING_PAGES );

    if  ( docUpdDocumentProperties( &changed, &(hft->hftDocPropertiesChosen),
							&update, dpSet ) )
	{ LDEB(1); return;	}
    if  ( docUpdDocumentProperties( &changed, &(hft->hftDocPropertiesSet),
							&update, dpSet ) )
	{ LDEB(1); return;	}

    PROPmaskCLEAR( &update );
    PROPmaskADD( &update, SPpropTITLEPG );

    if  ( docUpdSectProperties( &changed, &(hft->hftSectPropertiesChosen),
							&update, spSet ) )
	{ LDEB(1); return;	}
    if  ( docUpdSectProperties( &changed, &(hft->hftSectPropertiesSet),
							&update, spSet ) )
	{ LDEB(1); return;	}

    /**********/

    if  ( sd->sdInHeaderFooter )
	{
	int		i;

	for ( i= 0; i < PAGES__COUNT; i++ )
	    { hft->hftPagesOptionsEnabled[i]= 0; }

	appGuiEnableWidget( hft->hftSectionFrame, 0 );
	appGuiEnableWidget( hft->hftDocumentFrame, 0 );
	}
    else{
	hft->hftPagesOptionsEnabled[PAGES_FIRST_PAGE]=
					    spSet->spHasTitlePage;
	hft->hftPagesOptionsEnabled[PAGES_SUBSEQUENT_PAGES]=
					    spSet->spHasTitlePage	&&
					    ! dpSet->dpHasFacingPages;
	hft->hftPagesOptionsEnabled[PAGES_ALL_PAGES]=
					    ! spSet->spHasTitlePage	&&
					    ! dpSet->dpHasFacingPages;
	hft->hftPagesOptionsEnabled[PAGES_ODD_PAGES]=
					    dpSet->dpHasFacingPages;
	hft->hftPagesOptionsEnabled[PAGES_EVEN_PAGES]=
					    dpSet->dpHasFacingPages;

	appGuiEnableWidget( hft->hftSectionFrame, 1 );
	appGuiEnableWidget( hft->hftDocumentFrame, 1 );
	}

    switch( whatHeaderFooter )
	{
	case DOCinSECT_HEADER:
	case DOCinSECT_FOOTER:
	    if  ( spSet->spHasTitlePage )
		{ hft->hftPagesChosen= PAGES_SUBSEQUENT_PAGES;	}
	    else{ hft->hftPagesChosen= PAGES_ALL_PAGES;		}

	    hft->hftPagesOptionsEnabled[hft->hftPagesChosen]= 1;
	    appSetOptionmenu( &(hft->hftPagesOptionmenu), hft->hftPagesChosen );

	    break;

	case DOCinFIRST_HEADER:
	case DOCinFIRST_FOOTER:
	    hft->hftPagesChosen= PAGES_FIRST_PAGE;
	    hft->hftPagesOptionsEnabled[hft->hftPagesChosen]= 1;
	    appSetOptionmenu( &(hft->hftPagesOptionmenu), hft->hftPagesChosen );
	    break;

	case DOCinLEFT_HEADER:
	case DOCinLEFT_FOOTER:
	    hft->hftPagesChosen= PAGES_EVEN_PAGES;
	    hft->hftPagesOptionsEnabled[hft->hftPagesChosen]= 1;
	    appSetOptionmenu( &(hft->hftPagesOptionmenu), hft->hftPagesChosen );
	    break;

	case DOCinRIGHT_HEADER:
	case DOCinRIGHT_FOOTER:
	    hft->hftPagesChosen= PAGES_ODD_PAGES;
	    hft->hftPagesOptionsEnabled[hft->hftPagesChosen]= 1;
	    appSetOptionmenu( &(hft->hftPagesOptionmenu), hft->hftPagesChosen );
	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	case DOCinBODY:

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    *pEnabled= 0; return;

	default:
	    LDEB(whatHeaderFooter);
	    *pEnabled= 0; return;
	}

    /**********/

    tedFormatToolRefreshHeadFootPage( hft );

    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons have been pushed.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedHeadFootChangeDocPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    DocumentProperties *	dp= &(hft->hftDocPropertiesChosen);

    PropertyMask		update;

    PROPmaskCLEAR( &update );
    PROPmaskADD( &update, DPpropFACING_PAGES );

    tedAppSetDocumentProperties( hft->hftApplication, dp, &update );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedHeadFootChangeSectPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    SectionProperties *		sp= &(hft->hftSectPropertiesChosen);

    PropertyMask		update;

    PROPmaskCLEAR( &update );
    PROPmaskADD( &update, SPpropTITLEPG );

    if  ( tedAppChangeSectionProperties( hft->hftApplication, &update, sp ) )
	{ LDEB(1);	}

    return;
    }

static APP_BUTTON_CALLBACK_H( tedHeadFootRevertDocPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    DocumentProperties *	dp= &(hft->hftDocPropertiesChosen);

    PropertyMask		update;
    PropertyMask		changed;

    PROPmaskCLEAR( &changed );

    PROPmaskCLEAR( &update );
    PROPmaskADD( &update, DPpropFACING_PAGES );

    if  ( docUpdDocumentProperties( &changed, dp,
				    &update, &(hft->hftDocPropertiesSet) ) )
	{ LDEB(1); return;	}

    tedFormatToolRefreshHeadFootPage( hft );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedHeadFootRevertSectPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    SectionProperties *		sp= &(hft->hftSectPropertiesChosen);

    PropertyMask		update;
    PropertyMask		changed;

    PROPmaskCLEAR( &changed );

    PROPmaskCLEAR( &update );
    PROPmaskADD( &update, SPpropTITLEPG );

    if  ( docUpdSectProperties( &changed, sp,
				    &update, &(hft->hftSectPropertiesSet) ) )
	{ LDEB(1); return;	}

    tedFormatToolRefreshHeadFootPage( hft );

    return;
    }

/************************************************************************/
/*									*/
/*  The 'Titlepg' toggle was pushed.					*/
/*  The 'Facingp' toggle was pushed.					*/
/*									*/
/************************************************************************/

static APP_TOGGLE_CALLBACK_H( tedHeaderFooterToggleTitlepg, w, voidhft, voidtbcs )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    SectionProperties *		sp= &(hft->hftSectPropertiesChosen);
    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    sp->spHasTitlePage= ( set != 0 );

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedHeaderFooterToggleFacingp, w, voidhft, voidtbcs )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    DocumentProperties *	dp= &(hft->hftDocPropertiesChosen);
    int				set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    dp->dpHasFacingPages= ( set != 0 );

    return;
    }

/************************************************************************/
/*									*/
/*  A kind of pages was chosen in the optionmenu.			*/
/*									*/
/************************************************************************/

static APP_OITEM_CALLBACK_H( tedHeadFootPagesChosen, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;

    int				pages= -1;

    pages= appGuiGetOptionmenuItemIndex( &(hft->hftPagesOptionmenu), w );
    if  ( pages < 0 || pages >= PAGES__COUNT )
	{ LLDEB(pages,PAGES__COUNT); return;		}

    hft->hftPagesChosen= pages;

    return;
    }


/************************************************************************/
/*									*/
/*  Delete/Edit Headers/Footers.					*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedHeadFootDeleteHeaderPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;

    if  ( hft->hftPagesChosen < 0		||
	  hft->hftPagesChosen >= PAGES__COUNT	)
	{ LLDEB(hft->hftPagesChosen,PAGES__COUNT); return;	}

    tedAppDeleteHeaderFooter( hft->hftApplication,
				    DOC_HeaderScopes[hft->hftPagesChosen] );
    return;
    }

static APP_BUTTON_CALLBACK_H( tedHeadFootEditHeaderPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    APP_WIDGET			relative;

    if  ( hft->hftPagesChosen < 0		||
	  hft->hftPagesChosen >= PAGES__COUNT	)
	{ LLDEB(hft->hftPagesChosen,PAGES__COUNT); return;	}

    relative= hft->hftInspector->aiTopWidget;

    tedAppEditHeaderFooter( hft->hftApplication, relative,
				    hft->hftEditHeaderButton,
				    DOC_HeaderScopes[hft->hftPagesChosen] );
    return;
    }

static APP_BUTTON_CALLBACK_H( tedHeadFootDeleteFooterPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;

    if  ( hft->hftPagesChosen < 0		||
	  hft->hftPagesChosen >= PAGES__COUNT	)
	{ LLDEB(hft->hftPagesChosen,PAGES__COUNT); return;	}

    tedAppDeleteHeaderFooter( hft->hftApplication,
				    DOC_FooterScopes[hft->hftPagesChosen] );
    return;
    }

static APP_BUTTON_CALLBACK_H( tedHeadFootEditFooterPushed, w, voidhft )
    {
    HeaderFooterTool *		hft= (HeaderFooterTool *)voidhft;
    APP_WIDGET			relative;

    if  ( hft->hftPagesChosen < 0		||
	  hft->hftPagesChosen >= PAGES__COUNT	)
	{ LLDEB(hft->hftPagesChosen,PAGES__COUNT); return;	}

    relative= hft->hftInspector->aiTopWidget;

    tedAppEditHeaderFooter( hft->hftApplication, relative,
				    hft->hftEditFooterButton,
				    DOC_FooterScopes[hft->hftPagesChosen] );
    return;
    }

/************************************************************************/
/*									*/
/*  Make the 'HeaderFooter' page of the format tool.			*/
/*									*/
/************************************************************************/

void tedFormatFillHeaderFooterPage( HeaderFooterTool *		hft,
			    const HeaderFooterPageResources *	hfpr,
			    InspectorSubject *			is,
			    APP_WIDGET				pageWidget,
			    const InspectorSubjectResources *	isr )
    {
    APP_WIDGET	row= (APP_WIDGET )0;

    const int	heightResizable= 0;
    const int	textColumns= 5;

    /**/
    hft->hftPageResources= hfpr;

    /**/
    docInitDocumentProperties( &(hft->hftDocPropertiesSet) );
    docInitDocumentProperties( &(hft->hftDocPropertiesChosen) );
    docInitSectionProperties( &(hft->hftSectPropertiesSet) );
    docInitSectionProperties( &(hft->hftSectPropertiesChosen) );

    /**/

    appMakeColumnFrameInColumn( &(hft->hftSectionFrame),
			    &(hft->hftSectionPaned),
			    pageWidget, hfpr->hfprSectionHeaderText );

    appMakeLabelAndTextRow( &row, &(hft->hftSectionNumberLabel),
			    &(hft->hftSectionNumberText),
			    hft->hftSectionPaned, hfpr->hfprSectionNumberText,
			    textColumns, 0 );

    row= appMakeRowInColumn( hft->hftSectionPaned, 1, heightResizable );
    hft->hftTitlepgToggle= appMakeToggleInRow( row, hfpr->hfprTitlepgText,
				tedHeaderFooterToggleTitlepg, (void *)hft, 0 );

    appInspectorMakeButtonRow( &row, hft->hftSectionPaned,
		&(is->isRevertButton), &(is->isApplyButton),
		isr->isrRevert, isr->isrApplyToSubject,
		tedHeadFootRevertSectPushed, tedHeadFootChangeSectPushed,
		(void *)hft );

    /**************/

    appMakeColumnFrameInColumn( &(hft->hftDocumentFrame),
			    &(hft->hftDocumentPaned),
			    pageWidget, hfpr->hfprDocumentHeaderText );

    row= appMakeRowInColumn( hft->hftDocumentPaned, 1, heightResizable );
    hft->hftFacingpToggle= appMakeToggleInRow( row, hfpr->hfprFacingpText,
				tedHeaderFooterToggleFacingp, (void *)hft, 0 );

    appInspectorMakeButtonRow( &row, hft->hftDocumentPaned,
		&(hft->hftRevertDocButton), &(hft->hftChangeDocButton),
		hfpr->hfprRevertDocText, hfpr->hfprChangeDocText,
		tedHeadFootRevertDocPushed, tedHeadFootChangeDocPushed,
		(void *)hft );

    /**************/

    appMakeColumnFrameInColumn( &(hft->hftPagesFrame),
			    &(hft->hftPagesPaned),
			    pageWidget, hfpr->hfprPagesHeaderText );

    appMakeOptionmenuInColumn( &(hft->hftPagesOptionmenu),
						    hft->hftPagesPaned );

    appInspectorMakeButtonRow( &row, hft->hftPagesPaned,
		&(hft->hftDeleteHeaderButton), &(hft->hftEditHeaderButton),
		hfpr->hfprDeleteHeaderText, hfpr->hfprEditHeaderText,
		tedHeadFootDeleteHeaderPushed, tedHeadFootEditHeaderPushed,
		(void *)hft );

    appInspectorMakeButtonRow( &row, hft->hftPagesPaned,
		&(hft->hftDeleteFooterButton), &(hft->hftEditFooterButton),
		hfpr->hfprDeleteFooterText, hfpr->hfprEditFooterText,
		tedHeadFootDeleteFooterPushed, tedHeadFootEditFooterPushed,
		(void *)hft );

    /**************/

    return;
    }

/************************************************************************/
/*									*/
/*  Finish the Header/Footer page.					*/
/*									*/
/************************************************************************/

void tedFormatFillHeaderFooterChoosers( HeaderFooterTool *		hft,
				    const HeaderFooterPageResources *	hfpr )
    {
    appFillInspectorMenu( PAGES__COUNT, PAGES_ALL_PAGES,
			hft->hftPagesItems, hfpr->hfprPagesOptionTexts,
			&(hft->hftPagesOptionmenu),
			tedHeadFootPagesChosen, (void *)hft );

    hft->hftPagesChosen= PAGES_ALL_PAGES;

    return;
    }

void tedFormatFinishHeaderFooterPage( HeaderFooterTool *		hft,
				    TedFormatTool *			tft,
				    const HeaderFooterPageResources *	hfpr )
    {
    appOptionmenuRefreshWidth( &(hft->hftPagesOptionmenu) );

    return;
    }

/************************************************************************/
/*									*/
/*  Clean the header footer tool.					*/
/*									*/
/************************************************************************/

void tedFormatCleanHeaderFooterTool(	HeaderFooterTool *		hft )
    {
    docCleanDocumentProperties( &(hft->hftDocPropertiesChosen) );
    docCleanDocumentProperties( &(hft->hftDocPropertiesSet) );
    docCleanSectionProperties( &(hft->hftSectPropertiesChosen) );
    docCleanSectionProperties( &(hft->hftSectPropertiesSet) );

    return;
    }

/************************************************************************/
/*									*/
/*  Get Header/Footer tool resources.					*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedHeaderFooterSubjectResourceTable[]=
    {
    APP_RESOURCE( "formatToolHeaderFooter",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Headers and Footers" ),

    APP_RESOURCE( "formatToolHeadFootChangeSection",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Section" ),
    APP_RESOURCE( "formatToolHeadFootRevertSection",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    };

static AppConfigurableResource TED_TedHeaderFooterToolResourceTable[]=
    {
    APP_RESOURCE( "formatToolHeadFootSectionHeader",
		offsetof(HeaderFooterPageResources,hfprSectionHeaderText),
		"Section Properties" ),
    APP_RESOURCE( "formatToolHeadFootSectionNumber",
		offsetof(HeaderFooterPageResources,hfprSectionNumberText),
		"Section" ),
    APP_RESOURCE( "formatToolHeadFootTitlepgText",
		offsetof(HeaderFooterPageResources,hfprTitlepgText),
		"Different first Page" ),

    APP_RESOURCE( "formatToolHeadFootDocumentHeader",
		offsetof(HeaderFooterPageResources,hfprDocumentHeaderText),
		"Document Properties" ),
    APP_RESOURCE( "formatToolHeadFootFacingpText",
		offsetof(HeaderFooterPageResources,hfprFacingpText),
		"Different odd and even Pages" ),
    APP_RESOURCE( "formatToolHeadFootChangeDocument",
		offsetof(HeaderFooterPageResources,hfprChangeDocText),
		"Apply to Document" ),
    APP_RESOURCE( "formatToolHeadFootRevertDocument",
		offsetof(HeaderFooterPageResources,hfprRevertDocText),
		"Revert" ),

    APP_RESOURCE( "formatToolHeadFootPagesHeader",
		offsetof(HeaderFooterPageResources,hfprPagesHeaderText),
		"Pages in Section" ),

    APP_RESOURCE( "formatToolHeadFootFirstPage",
		offsetof(HeaderFooterPageResources,
				hfprPagesOptionTexts[PAGES_FIRST_PAGE]),
		"First Page" ),
    APP_RESOURCE( "formatToolHeadFootSubsequentPages",
		offsetof(HeaderFooterPageResources,
				hfprPagesOptionTexts[PAGES_SUBSEQUENT_PAGES]),
		"Subsequent Pages" ),
    APP_RESOURCE( "formatToolHeadFootAllPages",
		offsetof(HeaderFooterPageResources,
				hfprPagesOptionTexts[PAGES_ALL_PAGES]),
		"All Pages" ),
    APP_RESOURCE( "formatToolHeadFootOddPages",
		offsetof(HeaderFooterPageResources,
				hfprPagesOptionTexts[PAGES_ODD_PAGES]),
		"Odd Pages" ),
    APP_RESOURCE( "formatToolHeadFootEvenPages",
		offsetof(HeaderFooterPageResources,
				hfprPagesOptionTexts[PAGES_EVEN_PAGES]),
		"Even Pages" ),

    APP_RESOURCE( "formatToolHeadFootDeleteHeader",
		offsetof(HeaderFooterPageResources,hfprDeleteHeaderText),
		"Delete Header" ),
    APP_RESOURCE( "formatToolHeadFootEditHeader",
		offsetof(HeaderFooterPageResources,hfprEditHeaderText),
		"Edit Header" ),
    APP_RESOURCE( "formatToolHeadFootDeleteFooter",
		offsetof(HeaderFooterPageResources,hfprDeleteFooterText),
		"Delete Footer" ),
    APP_RESOURCE( "formatToolHeadFootEditFooter",
		offsetof(HeaderFooterPageResources,hfprEditFooterText),
		"Edit Footer" ),
    };

void tedFormatToolGetHeaderFooterResourceTable( EditApplication *	ea,
					HeaderFooterPageResources *	hfpr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)hfpr,
				TED_TedHeaderFooterToolResourceTable,
				sizeof(TED_TedHeaderFooterToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedHeaderFooterSubjectResourceTable,
				sizeof(TED_TedHeaderFooterSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
