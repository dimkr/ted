/************************************************************************/
/*									*/
/*  Intermediary functionality for the page layout page.		*/
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
/*  Refresh the page layout tool.					*/
/*									*/
/************************************************************************/

void tedFormatToolRefreshPageLayoutTool(
				PageLayoutTool *		plt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const DocumentProperties *	dp )
    {
    BufferItem *			sectBi;

    sectBi= ds->dsBegin.dpBi;
    while( sectBi && sectBi->biLevel > DOClevSECT )
	{ sectBi= sectBi->biParent;	}

    if  ( ! sectBi )
	{ XDEB(sectBi); return;	}

    appPageLayoutPageRefresh( plt, pEnabled, is,
		    &(sectBi->biSectDocumentGeometry), &(dp->dpGeometry) );

    if  ( ds->dsSelectionScope.ssInExternalItem != DOCinBODY )
	{ *pEnabled= 0;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make a page layout page customized for Ted.				*/
/*									*/
/************************************************************************/

void tedFormatFillPageLayoutPage(
			PageLayoutTool *			plt,
			const PageLayoutPageResources *		plpr,
			InspectorSubject *			is,
			APP_WIDGET				pageWidget,
			const InspectorSubjectResources *	isr )
    {
    const int		doHdFt= 1;
    const int		doSelection= 1;

    appPageLayoutPageFillPage( plt, doHdFt, doSelection,
						plpr, is, pageWidget, isr );
    }


/************************************************************************/
/*									*/
/*  Finish the page layout page.					*/
/*									*/
/************************************************************************/

void tedFormatFinishPageLayoutPage(	PageLayoutTool *		plt,
					TedFormatTool *			tft,
					const PageLayoutPageResources *	plpr )
    {
    appPageLayoutPageFinishPage( plt, plpr );
    }

/************************************************************************/
/*									*/
/*  Clean the page layout tool.						*/
/*									*/
/************************************************************************/

void tedFormatCleanPageLayoutTool(	PageLayoutTool *	plt )
    {
    appCleanPaperChooser( &(plt->pltPaperChooser) );
    appCleanDrawingData( &(plt->pltDrawingData) );

    return;
    }
