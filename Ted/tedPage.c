/************************************************************************/
/*  Ted: interaction with the page layout tool.				*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	"tedApp.h"
#   include	"tedRuler.h"
#   include	"docLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Reformat and redisplay the document because of a major format	*/
/*  change. etc.							*/
/*									*/
/************************************************************************/

void tedRedoDocumentLayout(	EditDocument *		ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    AppDrawingData *		add= &(ed->edDrawingData);

    int				scrolledX= 0;
    int				scrolledY= 0;

    int				hasSelection= tedHasSelection( td );

    tedLayoutDocumentTree( td, add );

    appSetShellConstraints( ed );

    if  ( hasSelection )
	{ tedDelimitCurrentSelection( td, add );	}

    appDocSetScrollbarValues( ed );
    appSetShellConstraints( ed );

    if  ( hasSelection )
	{
	DocumentSelection	ds;
	SelectionGeometry	sg;
	SelectionDescription	sd;

	if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	    { LDEB(1); return;	}

	tedDocAdaptHorizontalRuler( ed, ds.dsBegin.dpBi );

	tedScrollToSelection( ed, &scrolledX, &scrolledY );

	if  ( td->tdSelectionDescription.sdIsObjectSelection )
	    { tedMoveObjectWindows( ed );	}
	}

    appExposeRectangle( add, 0, 0, 0, 0 );

    tedAdaptLeftRuler( ed->edLeftRuler, ed->edLeftRulerWidget,
			ed->edVisibleRect.drY0, ed->edVisibleRect.drY1,
			add->addPageStepPixels );

    return;
    }

/************************************************************************/
/*									*/
/*  Change the document geometry: Callback from the page layout tool	*/
/*  etc.								*/
/*									*/
/************************************************************************/

void tedSetPageLayout(	EditApplication *		ea,
			const DocumentGeometry *	dg,
			const PropertyMask *		setMask,
			int				wholeDocument )
    {
    AppDrawingData *		add;
    TedDocument *		td;
    BufferDocument *		bd;
    DocumentProperties *	dp;

    PropertyMask		changed;
    PropertyMask		dgUpdMask;

    int				i;

    EditDocument *		ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    PROPmaskCLEAR( &changed );
    PROPmaskCLEAR( &dgUpdMask );

    add= &(ed->edDrawingData);
    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;
    dp= &(bd->bdProperties);

    PROPmaskCLEAR( &dgUpdMask );
    utilPropMaskFill( &dgUpdMask, DGprop_COUNT );
    utilPropMaskAnd( &dgUpdMask, &dgUpdMask, setMask );

    if  ( wholeDocument )
	{
	utilUpdDocumentGeometry( &(dp->dpGeometry), dg, &changed, &dgUpdMask );

	for ( i= 0; i < bd->bdItem.biChildCount; i++ )
	    {
	    PropertyMask		sectChanged;
	    BufferItem *		sectBi= bd->bdItem.biChildren[i];

	    PROPmaskCLEAR( &sectChanged );

	    utilUpdDocumentGeometry( &(sectBi->biSectDocumentGeometry),
						    dg, &sectChanged, setMask );

	    utilPropMaskOr( &changed, &changed, &sectChanged );
	    }

	if  ( ! utilPropMaskIsEmpty( &changed ) )
	    {
	    appDocumentChanged( ed, 1 );

	    tedScreenRectangles( add, &(dp->dpGeometry) );

	    tedRedoDocumentLayout( ed );

	    tedAdaptFormatToolToDocument( ed, 0 );
	    }
	}
    else{
	SectionProperties		spNew;
	PropertyMask			doneMask;

	docInitSectionProperties( &spNew );
	PROPmaskCLEAR( &doneMask );

	utilUpdDocumentGeometry( &(spNew.spDocumentGeometry), dg,
							&doneMask, setMask );

	if  ( tedAppChangeSectionProperties( ea, setMask, &spNew ) )
	    { LDEB(1);	}

	docCleanSectionProperties( &spNew );
	}

    return;
    }

int tedAppSetDocumentProperties( EditApplication *		ea,
				const DocumentProperties *	dpNew,
				const PropertyMask *		updMask )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    AppDrawingData *		add;
    TedDocument *		td;
    BufferDocument *		bd;
    DocumentProperties *	dp;

    PropertyMask		changed;

    RecalculateFields		rf;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    PROPmaskCLEAR( &changed );

    docInitRecalculateFields( &rf );

    add= &(ed->edDrawingData);
    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;
    dp= &(bd->bdProperties);

    rf.rfBd= bd;
    rf.rfVoidadd= (void *)add;
    rf.rfCloseObject= tedCloseObject;
    rf.rfUpdateFlags= 0;
    rf.rfFieldsUpdated= 0;

    if  ( docUpdDocumentProperties( &changed, dp, updMask, dpNew ) )
	{ LDEB(1); return -1;	}

    if  ( ! utilPropMaskIsEmpty( &changed ) )
	{
	int		noteNumbersChanged= 0;

	if  ( PROPmaskISSET( &changed, DPpropFOOTNOTE_RESTART )	||
	      PROPmaskISSET( &changed, DPpropENDNOTE_RESTART )	)
	    { docRenumberNotes( &noteNumbersChanged, bd );	}

	if  ( noteNumbersChanged				||
	      PROPmaskISSET( &changed, DPpropFOOTNOTE_STYLE )	||
	      PROPmaskISSET( &changed, DPpropFOOTNOTE_STARTNR )	||
	      PROPmaskISSET( &changed, DPpropENDNOTE_STYLE )	||
	      PROPmaskISSET( &changed, DPpropENDNOTE_STARTNR )	)
	    { rf.rfUpdateFlags |= FIELDdoCHFTN;	}

	if  ( PROPmaskISSET( &changed, DPpropTITLE )	||
	      PROPmaskISSET( &changed, DPpropSUBJECT )	||
	      PROPmaskISSET( &changed, DPpropKEYWORDS )	||
	      PROPmaskISSET( &changed, DPpropDOCCOMM )	||
	      PROPmaskISSET( &changed, DPpropAUTHOR )	||
	      PROPmaskISSET( &changed, DPpropCOMPANY )	||
	      PROPmaskISSET( &changed, DPpropCREATIM )	||
	      PROPmaskISSET( &changed, DPpropREVTIM )	||
	      PROPmaskISSET( &changed, DPpropPRINTIM )	)
	    { rf.rfUpdateFlags |= FIELDdoDOC_INFO;	}

	if  ( rf.rfUpdateFlags != 0 )
	    {
	    if  ( docRecalculateTextLevelFields( &rf, &(bd->bdItem) ) )
		{ LDEB(1); return -1;	}
	    }

	appDocumentChanged( ed, 1 );

	tedScreenRectangles( add, &(dp->dpGeometry) );

	tedRedoDocumentLayout( ed );

	tedAdaptFormatToolToDocument( ed, 0 );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Adapt the page tool to a document.					*/
/*									*/
/************************************************************************/

void tedAdaptPageToolToDocument(	EditApplication *	ea,
					EditDocument *		ed )
    {
    TedDocument *		td;
    BufferDocument *		bd;
    DocumentProperties *	dp;

    if  ( ! ea->eaPageTool )
	{ return;	}

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;
    dp= &(bd->bdProperties);

    appPageToolSetProperties( ea->eaPageTool, &(dp->dpGeometry) );

    appEnablePageTool( ea->eaPageTool, ! ed->edIsReadonly );

    return;
    }

/************************************************************************/
/*									*/
/*  Derive private rectangles from document properties.			*/
/*									*/
/************************************************************************/

void tedScreenRectangles(	AppDrawingData *		add,
				const DocumentGeometry *	dg )
    {
    double			xfac= add->addMagnifiedPixelsPerTwip;
    double			yfac= add->addMagnifiedPixelsPerTwip;

    int				leftMarginPixels;
    int				rightMarginPixels;

#   if 0
    /*  1  */
    if  ( dg->dgLeftMarginTwips < MINMARG )
	{ dg->dgLeftMarginTwips= MINMARG;	}
    if  ( dg->dgTopMarginTwips < MINMARG )
	{ dg->dgTopMarginTwips= MINMARG;	}
    if  ( dg->dgRightMarginTwips < MINMARG )
	{ dg->dgRightMarginTwips= MINMARG;	}
    if  ( dg->dgBottomMarginTwips < MINMARG )
	{ dg->dgBottomMarginTwips= MINMARG;	}
#   endif

    leftMarginPixels= TWIPStoPIXELS( xfac, dg->dgLeftMarginTwips );
    rightMarginPixels= TWIPStoPIXELS( xfac, dg->dgRightMarginTwips );
    add->addBottomMarginPixels= TWIPStoPIXELS( xfac, dg->dgBottomMarginTwips );

    add->addBackRect.drX0= 0;
    add->addBackRect.drY0= 0;
    add->addBackRect.drX1= TWIPStoPIXELS( xfac, dg->dgPageWideTwips );
    add->addBackRect.drY1= TWIPStoPIXELS( yfac, dg->dgPageHighTwips );

    add->addPaperRect.drX0= 0;
    add->addPaperRect.drY0= 0;
    add->addPaperRect.drX1= TWIPStoPIXELS( xfac, dg->dgPageWideTwips );
    add->addPaperRect.drY1= TWIPStoPIXELS( yfac, dg->dgPageHighTwips );

    add->addDocRect.drX0= leftMarginPixels;
    add->addDocRect.drY0= TWIPStoPIXELS( xfac, dg->dgTopMarginTwips );
    add->addDocRect.drX1= add->addBackRect.drX1- rightMarginPixels;
    add->addDocRect.drY1= add->addBackRect.drY1- add->addBottomMarginPixels;

    add->addPageStepPixels= add->addPaperRect.drY1- add->addPaperRect.drY0+
							add->addPageGapPixels;

    return;
    }

/************************************************************************/
/*									*/
/*  (re)Calculate the layout of a whole document.			*/
/*									*/
/************************************************************************/

int tedLayoutDocumentTree(	TedDocument *		td,
				AppDrawingData *	add )
    {
    ScreenFontList *		sfl= &(td->tdScreenFontList);
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);

    DocumentRectangle		drChanged;

    /*  1  */
    if  ( dp->dpFontList.dflFontCount == 0 )
	{ LDEB(dp->dpFontList.dflFontCount); return -1;	}

    drChanged= add->addBackRect;

    if  ( tedLayoutItem( &(bd->bdItem), bd, add, sfl, &drChanged ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

