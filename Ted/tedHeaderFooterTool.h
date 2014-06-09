/************************************************************************/
/*									*/
/*  Headers/Footers tool:						*/
/*									*/
/************************************************************************/

#   ifndef	TED_HEADER_FOOTER_TOOL_H
#   define	TED_HEADER_FOOTER_TOOL_H

/************************************************************************/
/*									*/
/*  A Header/Footer Tool. I.E. The header footer page on the 'Format	*/
/*  Tool'								*/
/*									*/
/************************************************************************/

typedef struct HeaderFooterPageResources
    {
    char *		hfprSectionHeaderText;
    char *		hfprSectionNumberText;
    char *		hfprTitlepgText;

    char *		hfprDocumentHeaderText;
    char *		hfprFacingpText;
    char *		hfprChangeDocText;
    char *		hfprRevertDocText;

    char *		hfprPagesHeaderText;
    char *		hfprPagesOptionTexts[PAGES__COUNT];

    char *		hfprDeleteHeaderText;
    char *		hfprEditHeaderText;
    char *		hfprDeleteFooterText;
    char *		hfprEditFooterText;
    } HeaderFooterPageResources;

typedef struct HeaderFooterTool
    {
    EditApplication *			hftApplication;
    AppInspector *			hftInspector;
    const HeaderFooterPageResources *	hftPageResources;

    int					hftSectionNumber;

    DocumentProperties			hftDocPropertiesChosen;
    DocumentProperties			hftDocPropertiesSet;

    SectionProperties			hftSectPropertiesChosen;
    SectionProperties			hftSectPropertiesSet;
    int					hftPagesChosen;
    unsigned char			hftPagesOptionsEnabled[PAGES__COUNT];

    APP_WIDGET				hftSectionFrame;
    APP_WIDGET				hftSectionPaned;
    APP_WIDGET				hftSectionNumberLabel;
    APP_WIDGET				hftSectionNumberText;
    APP_WIDGET				hftTitlepgToggle;

    APP_WIDGET				hftDocumentFrame;
    APP_WIDGET				hftDocumentPaned;
    APP_WIDGET				hftFacingpToggle;
    APP_WIDGET				hftRevertDocButton;
    APP_WIDGET				hftChangeDocButton;

    APP_WIDGET				hftPagesFrame;
    APP_WIDGET				hftPagesPaned;
    AppOptionmenu			hftPagesOptionmenu;
    APP_WIDGET				hftPagesItems[PAGES__COUNT];

    APP_WIDGET				hftDeleteHeaderButton;
    APP_WIDGET				hftEditHeaderButton;
    APP_WIDGET				hftDeleteFooterButton;
    APP_WIDGET				hftEditFooterButton;
    } HeaderFooterTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatToolRefreshHeaderFooterTool(
				HeaderFooterTool *		hft,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				const SelectionGeometry *	sg,
				BufferDocument *		bd );

extern void tedFormatToolGetHeaderFooterResourceTable( EditApplication * ea,
					HeaderFooterPageResources *	hfpr,
					InspectorSubjectResources *	isr );

extern void tedFormatCleanHeaderFooterTool(	HeaderFooterTool *	hft );

#   endif	/*  TED_HEADER_FOOTER_TOOL_H */
