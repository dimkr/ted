/************************************************************************/
/*									*/
/*  The 'Font Tool':							*/
/*									*/
/*  Most of the ont tool calls directly go to the AppFontTool.		*/
/*  the document.							*/
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
/*  Intermediary routine to refresh the text font tool.			*/
/*									*/
/************************************************************************/

void tedRefreshFontTool(	AppFontChooser *		afc,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				EditDocument *			ed )
    {
    const TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    const DocumentProperties *	dp= &(bd->bdProperties);
    const DocumentFontList *	dfl= &(dp->dpFontList);

    PropertyMask		updMask;

    TextAttribute		ta;

    if  ( sd->sdIsListBullet )
	{ *pEnabled= 0; return; }

    (*pPref)++;

    PROPmaskCLEAR( &updMask );

    utilInitTextAttribute( &ta );

    if  ( sd->sdIsIBarSelection )
	{
	ta= td->tdCurrentTextAttribute;
	utilPropMaskFill( &updMask, TAprop_COUNT );
	}
    else{
	docGetSelectionAttributes( bd, ds, &updMask, &ta );
	}

    if  ( appFontExpandCurrentFont( afc, &updMask, &ta,
				    ed->edDocumentId,
				    dfl, dp->dpColors, dp->dpColorCount ) )
	{ LDEB(1);	}

    *pEnabled= 1;

    return;
    }

/************************************************************************/
/*									*/
/*  Intermediary routine to refresh the list font tool.			*/
/*									*/
/************************************************************************/

void tedRefreshListFontTool(	AppFontChooser *		afc,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const SelectionDescription *	sd,
				BufferDocument *		bd )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);
    const DocumentFontList *	dfl= &(dp->dpFontList);

    ListOverride *		lo;
    DocumentList *		dl;
    ListNumberTreeNode *	root;

    int				startPath[DLmaxLEVELS+1];
    int				formatPath[DLmaxLEVELS+1];
    const DocumentListLevel *	dll= (const DocumentListLevel *)0;

    if  ( ! sd->sdHasLists		||
	  sd->sdListOverride < 1	||
	  sd->sdMultiList		||
	  sd->sdMultiLevel		)
	{ *pEnabled= 0; return; }

    if  ( docGetListOfParagraph( &lo, &root, &dl, sd->sdListOverride, bd ) )
	{ *pEnabled= 0; return; }

    docListGetFormatPath( startPath, formatPath, &dll,
						    sd->sdListLevel, dl, lo );

    if  ( appFontExpandCurrentFont( afc, &(dll->dllTextAttributeMask),
				    &(dll->dllTextAttribute),
				    sd->sdDocumentId, dfl,
				    dp->dpColors, dp->dpColorCount ) )
	{ LDEB(1);	}

    (*pPref)--;
    *pEnabled= 1;

    return;
    }

void tedFormatShowFontPage(	EditApplication *	ea )
    {
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    if  ( ! tar->tarInspector )
	{ XDEB(tar->tarInspector); return;	}

    appEnableInspectorSubject( tar->tarInspector, TEDtsiFONT, 1 );

    appInspectorSelectSubject( tar->tarInspector, TEDtsiFONT );

    return;
    }

/************************************************************************/
/*									*/
/*  Resources.								*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_FontToolSubjectResourceTable[]=
    {
    APP_RESOURCE( "fontToolFont",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Font" ),
    APP_RESOURCE( "fontToolSet",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Set" ),
    APP_RESOURCE( "fontToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    };

void tedFontToolGetResourceTable(	EditApplication *		ea,
					AppFontToolResources *		aftr,
					InspectorSubjectResources *	isr )
    {
    static int	gotSubjectResources= 0;

    appFontToolGetResourceTable( ea, aftr );

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_FontToolSubjectResourceTable,
				sizeof(TED_FontToolSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }

static AppConfigurableResource TED_ListFontToolSubjectResourceTable[]=
    {
    APP_RESOURCE( "fontToolListFont",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"List Font" ),
    APP_RESOURCE( "fontToolSetListFont",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Set" ),
    APP_RESOURCE( "fontToolRevertListFont",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    };

void tedListFontToolGetResourceTable(	EditApplication *		ea,
					InspectorSubjectResources *	isr )
    {
    static int	gotSubjectResources= 0;

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_ListFontToolSubjectResourceTable,
				sizeof(TED_ListFontToolSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
