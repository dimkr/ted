/************************************************************************/
/*									*/
/*  Format tool:							*/
/*									*/
/************************************************************************/

#   ifndef	TED_FORMAT_TOOL_H
#   define	TED_FORMAT_TOOL_H

#   include	"tedParaLayoutTool.h"
#   include	"tedParaOrnamentsTool.h"
#   include	"tedRowTool.h"
#   include	"tedSectionTool.h"
#   include	"tedPageLayoutTool.h"
#   include	"tedNotesTool.h"
#   include	"tedHeaderFooterTool.h"
#   include	"tedTableTool.h"
#   include	"tedColumnTool.h"
#   include	"tedCellTool.h"
#   include	"tedTabsTool.h"
#   include	"tedListTool.h"
#   include	"tedFontTool.h"
#   include	"tedLinkTool.h"
#   include	"tedImageTool.h"

#   include	<appRgbChooserPage.h>

/************************************************************************/
/*									*/
/*  The different pages in a Format tool:				*/
/*									*/
/************************************************************************/

typedef enum ToolSubjectIndex
    {
    TEDtsiTABLE= 0,
    TEDtsiROW,
    TEDtsiCOLUMN,
    TEDtsiCELL,
    TEDtsiPARA_LAY,
    TEDtsiTABS,
    TEDtsiPARA_ORN,
    TEDtsiLISTS,
    TEDtsiSECT,
    TEDtsiPAGE,
    TEDtsiHEADFOOT,
    TEDtsiNOTES,
    TEDtsiFONT,
    TEDtsiLISTFONT,
    TEDtsiLINK,
    TEDtsiIMAGE,

    TEDtsiRGB,

    TEDtsi_COUNT

    } ToolSubjectIndex;

/************************************************************************/
/*									*/
/*  Represents a format tool.						*/
/*									*/
/************************************************************************/

typedef struct TedFormatTool
    {
    EditApplication *		tftApplication;
    AppInspector *		tftInspector;

    TableTool			tftTableTool;
    RowTool			tftRowTool;
    ColumnTool			tftColumnTool;
    CellTool			tftCellTool;
    ParagraphLayoutTool		tftParagraphLayoutTool;
    TabsTool			tftTabsTool;
    ParagraphOrnamentsTool	tftParagraphOrnamentsTool;
    ListTool			tftListsTool;
    SectionTool			tftSectionTool;
    PageLayoutTool		tftPageLayoutTool;
    HeaderFooterTool		tftHeaderFooterTool;
    NotesTool			tftNotesTool;
    AppFontChooser		tftFontTool;
    AppFontChooser		tftListFontTool;
    LinkTool			tftLinkTool;
    ImageTool			tftImageTool;

    RgbChooserPage		tftRgbPage;
    } TedFormatTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedFormatFinishRowPage( 	RowTool *			rt,
					TedFormatTool *			tft,
					const RowPageResources *	rpr );

extern void tedFormatFillRowPage( RowTool *			rt,
				const RowPageResources *	rpr,
				AppInspector *			ai,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void tedFormatFillRowChoosers(	RowTool *			rt,
					const RowPageResources *	rpr );

extern int tedFormatToolGetRowLeftIndent( const RowProperties *	rp,
					const int		pageLeftMargin,
					int *			pValue,
					APP_WIDGET		w );

extern int tedFormatToolGetGapWidth(	const RowProperties *	rp,
					int *			pValue,
					APP_WIDGET		w );

extern void tedFormatFillColumnPage(	ColumnTool *		ct,
				const ColumnPageResources *	cpr,
				AppInspector *			ai,
				int				subjectPage,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void tedFormatFillSectionPage(	SectionTool *		st,
				const SectionPageResources *	spr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void tedFormatFillSectionChoosers( SectionTool *			st,
					const SectionPageResources *	spr );

extern void tedFormatFinishSectionPage(	SectionTool *			st,
					TedFormatTool *			tft,
					const SectionPageResources *	spr );

extern void tedFormatFinishPageLayoutPage(	PageLayoutTool *	plt,
					TedFormatTool *			tft,
					const PageLayoutPageResources *	plpr );

extern void tedFormatFillNotesPage( NotesTool *			nt,
				const NotesPageResources *	npr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void tedFormatFillNotesChoosers(	NotesTool *			nt,
					const NotesPageResources *	npr );

extern void tedFormatFinishNotesPage(	NotesTool *			nt,
					TedFormatTool *			tft,
					const NotesPageResources *	npr );

extern int tedFormatValidateDimension(		int *		pNewValue,
						int *		pChanged,
						APP_WIDGET	w,
						int		oldValue );

extern void tedFormatFinishColumnPage(	ColumnTool *			ct,
					TedFormatTool *			tft,
					const ColumnPageResources *	cpr );

extern void tedColumnToolFillChoosers( ColumnTool *			ct,
					const ColumnPageResources *	cpr );

extern void tedFormatFinishCellPage(	CellTool *			ct,
					TedFormatTool *			tft,
					const CellPageResources *	cpr );

extern void tedFormatFillPageLayoutPage(
			PageLayoutTool *			plt,
			const PageLayoutPageResources *		plpr,
			InspectorSubject *			is,
			APP_WIDGET				pageWidget,
			const InspectorSubjectResources *	isr );

extern void tedFormatFillHeaderFooterPage(
			HeaderFooterTool *			hft,
			const HeaderFooterPageResources *	hfpr,
			InspectorSubject *			is,
			APP_WIDGET				pageWidget,
			const InspectorSubjectResources *	isr );

extern void tedFormatFillHeaderFooterChoosers( HeaderFooterTool *	hft,
				    const HeaderFooterPageResources *	hfpr );

extern void tedFormatFinishHeaderFooterPage( HeaderFooterTool *		hft,
				    TedFormatTool *			tft,
				    const HeaderFooterPageResources *	hfpr );

extern void tedFormatToolGetColumnResourceTable(
					EditApplication *		ea,
					ColumnPageResources *		cpr,
					InspectorSubjectResources *	isr );

extern void tedFormatToolGetTabsResourceTable(
					EditApplication *		ea,
					TabsPageResources *		tpr,
					InspectorSubjectResources *	isr );

extern void tedFormatFinishParaLayoutPage(
				ParagraphLayoutTool *			plt,
				TedFormatTool *				tft,
				const ParagraphLayoutPageResources *	plpr );

extern void tedFormatFinishParaOrnamentsPage(
				ParagraphOrnamentsTool *		pot,
				TedFormatTool *				tft,
				const ParagraphOrnamentsPageResources *	popr );

extern int tedFormatToolGetShading(	PropertyMask *		cpSetMask,
					RowProperties *		rp,
					int			col0,
					int			col1,
					DocumentProperties *	dp,
					ShadingTool *		st );

#   endif	/*  TED_FORMAT_TOOL_H */
