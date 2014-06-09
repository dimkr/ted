/************************************************************************/
/*									*/
/*  List related editing and formatting changes.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"docLayout.h"
#   include	"tedEdit.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Change the properties of the list where the selection starts.	*/
/*									*/
/************************************************************************/

static int tedDocChangeCurrentList(	EditDocument *		ed,
					const DocumentList *	dlNew )
    {
    TedDocument *			td= (TedDocument *)ed->edPrivateData;
    BufferDocument *			bd= td->tdDocument;

    DocumentSelection			ds;
    SelectionGeometry			sg;
    SelectionDescription		sd;

    EditOperation			eo;
    const int				fullWidth= 1;

    ListOverride *			lo;
    DocumentList *			dl;
    ListNumberTreeNode *		root;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, fullWidth );

    if  ( ! sd.sdHasLists	||
	  sd.sdListOverride < 1	)
	{ LLDEB(sd.sdHasLists,sd.sdListOverride); return -1; }

    if  ( docGetListOfParagraph( &lo, &root, &dl, sd.sdListOverride, bd ) )
	{ LDEB(1); return -1;	}

    if  ( docCopyDocumentList( dl, dlNew, (int *)0, (int *)0 ) )
	{ LDEB(1); return -1;	}

    if  ( docApplyListRuler( root, dl, lo, bd ) )
	{ LDEB(1);	}

    eo.eoFieldUpdate |= FIELDdoLISTTEXT;

    tedEditRefreshLayout( &eo, ed );

    if  ( tedEditFinishOldSelection( ed, &eo ) )
	{ LDEB(1);	}

    tedAdaptToolsToSelection( ed );

    appDocumentChanged( ed, 1 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Ted: The 'Set' button on the list font tool was pushed.		*/
/*									*/
/************************************************************************/

void tedListFontToolSet(	void *				voidea,
				const ExpandedTextAttribute *	etaSet,
				const PropertyMask *		taSetMask )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;
    TedDocument *		td;
    BufferDocument *		bd;
    DocumentProperties *	dp;
    DocumentFontList *		dfl;

    EditOperation		eo;
    const int			fullWidth= 1;

    ListOverride *		lo;
    DocumentList *		dl;
    ListNumberTreeNode *	root;

    DocumentListLevel *		dll;

    PropertyMask		doneMask;
    PropertyMask		explicitMask;
    TextAttribute		taSet;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;
    dp= &(bd->bdProperties);
    dfl= &(dp->dpFontList);

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, fullWidth );

    if  ( ! sd.sdHasLists	||
	  sd.sdListOverride < 1	||
	  sd.sdMultiList	||
	  sd.sdMultiLevel	)
	{ LLDEB(sd.sdHasLists,sd.sdListOverride); return; }

    if  ( docGetListOfParagraph( &lo, &root, &dl, sd.sdListOverride, bd ) )
	{ LDEB(1); return;	}

    utilInitTextAttribute( &taSet );
    PROPmaskCLEAR( &doneMask );

    docIndirectTextAttribute( &doneMask, &taSet, etaSet, taSetMask,
				    &(dp->dpColors), &(dp->dpColorCount) );

    dll= &(dl->dlLevels[sd.sdListLevel]);

    utilPropMaskNot( &explicitMask, &(dll->dllTextAttributeMask) );
    utilPropMaskAnd( &explicitMask, taSetMask, taSetMask );

    PROPmaskCLEAR( &doneMask );
    utilUpdateTextAttribute( &doneMask, &(dll->dllTextAttribute),
							&taSet, taSetMask );
    utilPropMaskOr( &(dll->dllTextAttributeMask),
				    &(dll->dllTextAttributeMask), taSetMask );

    utilPropMaskOr( &doneMask, &doneMask, &explicitMask );

    if  ( utilPropMaskIsEmpty( &doneMask ) )
	{ return;	}

    eo.eoFieldUpdate |= FIELDdoLISTTEXT;

    tedEditRefreshLayout( &eo, ed );

    if  ( tedEditFinishOldSelection( ed, &eo ) )
	{ LDEB(1);	}

    tedAdaptToolsToSelection( ed );

    appDocumentChanged( ed, 1 );

    return;
    }

int tedAppChangeCurrentList(	EditApplication *		ea,
				const DocumentList *		dlNew )
    {
    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocChangeCurrentList( ed, dlNew );
    }

