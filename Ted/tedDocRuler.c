/************************************************************************/
/*									*/
/*  Rulers, Ted specific functionality.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioMemory.h>

#   include	"tedLayout.h"

#   include	"tedApp.h"
#   include	"tedRuler.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Allow a user to change paragraph attributes with the top ruler.	*/
/*									*/
/************************************************************************/

static void tedDragVerticalHair(		void *		voided,
						int		from,
						int		to,
						int		asUpper )
    {
    EditDocument *		ed= (EditDocument *)voided;
    AppDrawingData *		add= &(ed->edDrawingData);
    int				ox= ed->edVisibleRect.drX0;

    int				high;

    high= ed->edVisibleRect.drY1- ed->edVisibleRect.drY0+ 1;

    if  ( from >= 0 )
	{ appExposeRectangle( add, from- ox, 0, 1, high ); }

    if  ( to >= 0 )
	{ appDrawFillRectangle( add, to- ox, 0, 1, high ); }

    return;
    }

/************************************************************************/
/*									*/
/*  Obtain the separator positions for a table.				*/
/*									*/
/************************************************************************/

static int tedGetColumns(	BufferItem *			bi,
				const AppDrawingData *		add,
				const BufferItem *		sectBi,
				ColumnSeparator **		pCs,
				int *				pCsCount )
    {
    const DocumentGeometry *	dg;

    static ColumnSeparator *	cs;
    ColumnSeparator *		fresh;

    BufferItem *		rowBi;
    CellProperties *		cp;

    int				i;
    int				x00Pixels;

    dg= &(sectBi->biSectDocumentGeometry);

    x00Pixels= X_PIXELS( add, dg->dgLeftMarginTwips );

    rowBi= bi->biParent->biParent;

    fresh= (ColumnSeparator *)realloc( cs, ( rowBi->biRowCellCount+ 1 )*
					sizeof( ColumnSeparator ) );
    if  ( ! fresh )
	{ LXDEB(rowBi->biRowCellCount,fresh); return -1;	}
    cs= fresh;

    fresh->csX0= rowBi->biRowLeftIndentPixels-
				    rowBi->biRowHalfGapWidthPixels;
    fresh->csX1= rowBi->biRowLeftIndentPixels+
				    rowBi->biRowHalfGapWidthPixels;
    fresh->csX0 += x00Pixels;
    fresh->csX1 += x00Pixels;

    fresh++; cp= rowBi->biRowCells;
    for ( i= 0; i < rowBi->biRowCellCount; fresh++, cp++, i++ )
	{
	fresh->csX0= cp->cpRightBoundaryPixels-
				    rowBi->biRowHalfGapWidthPixels;
	fresh->csX1= cp->cpRightBoundaryPixels+
				    rowBi->biRowHalfGapWidthPixels;

	fresh->csX0 += x00Pixels;
	fresh->csX1 += x00Pixels;
	}

    *pCs= cs; *pCsCount= rowBi->biRowCellCount+ 1; return 0;
    }

static void tedUpdateTableColumns(	EditDocument *		ed,
					BufferItem *		bi,
					double			xfac,
					double			magnification,
					int			csCountNew,
					ColumnSeparator *	csNew )
    {
    AppDrawingData *		add= &(ed->edDrawingData);

    int			csCount;
    ColumnSeparator *	cs;
    int			changed= 0;

    BufferItem *	sectBi;
    BufferItem *	rowBi;

    RowProperties *	rp;
    CellProperties *	cp;

    int			col;
    int			row;
    int			row0;
    int			row1;

    int			i;

    if  ( docDelimitTable( bi, &sectBi, &col, &row0, &row, &row1 ) )
	{ LDEB(1); return;	}

    if  ( tedGetColumns( bi, add, sectBi, &cs, &csCount ) )
	{ LDEB(1); return;	}

    if  ( csCountNew != csCount )
	{ LLDEB(csCountNew,csCount); return;	}

    rowBi= sectBi->biChildren[row0];
    rp= &(rowBi->biRowProperties);

    if  ( csNew[0].csX0 != cs[0].csX0 )
	{
	rp->rpLeftIndentPixels += ( csNew[0].csX0- cs[0].csX0 );
	rp->rpLeftIndentTwips= PIXELStoTWIPS( rp->rpLeftIndentPixels, xfac );
	changed= 1;
	}

    cp= rp->rpCells;
    for ( i= 1; i < csCount; cp++, i++ )
	{
	if  ( csNew[i].csX0 != cs[i].csX0 )
	    {
	    cp->cpRightBoundaryPixels += ( csNew[i].csX0- cs[i].csX0 );
	    cp->cpRightBoundaryTwips=
			    PIXELStoTWIPS( cp->cpRightBoundaryPixels, xfac );
	    changed= 1;
	    }
	}

    if  ( changed )
	{
	PropertyMask	rpSetMask;
	PropertyMask	cpSetMask;

	const int	col0= 0;
	const int	col1= csCount- 1;

	PROPmaskCLEAR( &rpSetMask );
	PROPmaskCLEAR( &cpSetMask );

	PROPmaskADD( &rpSetMask, RPpropLEFT_INDENT );
	PROPmaskADD( &cpSetMask, CLpropRIGHT_BOUNDARY );

	tedChangeTableLayout( ed, sectBi, row0, row1, col0, col1, row1,
						&rpSetMask, &cpSetMask, rp );

	tedAdaptToolsToSelection( ed );

	tedAdaptFormatToolToDocument( ed, 0 );

	appDocumentChanged( ed, 1 );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  The user wants something with the mouse on the horizontal ruler	*/
/*									*/
/************************************************************************/

static int tedGetRulerTwips(
			const AppDrawingData *		add,
			BufferItem *			bi,
			ParagraphProperties *		pp,
			BufferDocument *		bd,
			int				firstIndentPixels,
			int				leftIndentPixels,
			int				rightIndentPixels,
			const TabStopList *		tslNew )
    {
    ParagraphFrame		pf;
    double			xfac= add->addMagnifiedPixelsPerTwip;

    const int			bottom= -1;
    const int			stripHigh= -1;

    int				x0GeometryPixels;
    int				x1GeometryPixels;

    BlockFrame			bf;

    int				tabCount= tslNew->tslTabStopCount;
    const TabStop *		tabStops= tslNew->tslTabStops;

    docBlockFrameTwips( &bf, bi, bd, bi->biTopPosition.lpPage,
					    bi->biTopPosition.lpColumn );

    docParagraphFrameTwips( &pf, &bf, bottom, stripHigh, bi );
    tedParagraphFramePixels( &pf, add, bi );

    {
    TabStopList *	tsl= &(bi->biParaTabStopList);
    int			tab;
    TabStop *		ts= tsl->tslTabStops;

    for ( tab= 0; tab < tsl->tslTabStopCount; ts++, tab++ )
	{
	ts->tsPixels= X_PIXELS( add, pf.pfX0GeometryTwips+ ts->tsTwips );
	}
    }

    x0GeometryPixels= X_PIXELS( add, pf.pfX0GeometryTwips );
    x1GeometryPixels= X_PIXELS( add, pf.pfX1GeometryTwips );

    leftIndentPixels -= x0GeometryPixels;
    pp->ppLeftIndentTwips= PIXELStoTWIPS( leftIndentPixels, xfac );

    firstIndentPixels -= x0GeometryPixels;
    firstIndentPixels -= leftIndentPixels;
    pp->ppFirstIndentTwips= PIXELStoTWIPS( firstIndentPixels, xfac );

    rightIndentPixels= x1GeometryPixels- rightIndentPixels;
    pp->ppRightIndentTwips= PIXELStoTWIPS( rightIndentPixels, xfac );

    if  ( docRulerMergeTabs( &(pp->ppTabStopList), xfac, x0GeometryPixels,
						    tabCount, tabStops ) )
	{ LDEB(tabCount); return -1;	}

    return 0;
    }

static APP_EVENT_HANDLER_H( tedHorizontalRulerButtonDown, w, voided, downEvent )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    BufferItem *		bi;
    AppDrawingData *		add= &(ed->edDrawingData);

    int				leftIndentNew;
    int				firstIndentNew;
    int				rightIndentNew;
    TabStopList			tslNew;

    int				csCountNew= 0;
    ColumnSeparator *		csNew= (ColumnSeparator *)0;

    double			xfac= add->addMagnifiedPixelsPerTwip;
    double			magnification= ea->eaMagnification;

    int				prop= PPprop_NONE;

    PropertyMask		taSetMask;
    PropertyMask		ppChgMask;
    PropertyMask		ppUpdMask;
    PropertyMask		spUpdMask;

    ParagraphProperties		ppNew;
    TextAttribute		taSet;

    ParagraphFrame		pf;

    const int			bottom= -1;
    const int			stripHigh= -1;

    BlockFrame			bf;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int * const	colorMap= (const int *)0;
    const int * const	listStyleMap= (const int *)0;

    docInitTabStopList( &tslNew );

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return;	}
    bi= ds.dsBegin.dpBi;

    tslNew.tslTabStopCount= bi->biParaTabStopCount;

    PROPmaskCLEAR( &taSetMask );
    PROPmaskCLEAR( &ppUpdMask );
    PROPmaskCLEAR( &spUpdMask );

    docBlockFrameTwips( &bf, bi, bd, bi->biTopPosition.lpPage,
					    bi->biTopPosition.lpColumn );

    docParagraphFrameTwips( &pf, &bf, bottom, stripHigh, bi );
    tedParagraphFramePixels( &pf, add, bi );

    leftIndentNew= pf.pfX0TextLinesPixels;
    firstIndentNew= pf.pfX0FirstLinePixels;
    rightIndentNew= pf.pfX1TextLinesPixels;

    tedHorizontalRulerTrackMouse(
			    &firstIndentNew, &leftIndentNew, &rightIndentNew,
			    &tslNew,
			    &csCountNew, &csNew, &prop,
			    w, ea, downEvent, ed->edTopRuler,
			    (void *)ed, tedDragVerticalHair );

    switch( prop )
	{
	case PPprop_NONE:
	    return;

	case PPpropLEFT_INDENT:
	case PPpropFIRST_INDENT:
	    PROPmaskADD( &ppUpdMask, PPpropLEFT_INDENT );
	    PROPmaskADD( &ppUpdMask, PPpropFIRST_INDENT );
	    break;

	case PPpropRIGHT_INDENT:
	case PPpropTAB_STOPS:
	    PROPmaskADD( &ppUpdMask, prop );
	    break;

	case PPpropCOLUMNS:
	    tedUpdateTableColumns( ed, bi,
				    xfac, magnification, csCountNew, csNew );
	    return;
	default:
	    LDEB(prop); return;
	}

    docInitParagraphProperties( &ppNew );
    utilInitTextAttribute( &taSet );

    PROPmaskCLEAR( &ppChgMask );

    if  ( docUpdParaProperties( &ppChgMask, &ppNew, &ppUpdMask,
						    &(bi->biParaProperties),
						    colorMap, listStyleMap ) )
	{ LDEB(1); return;	}

    if  ( tedGetRulerTwips( add, bi, &ppNew, bd,
			    firstIndentNew, leftIndentNew, rightIndentNew,
			    &tslNew ) )
	{ LDEB(1); return;	}

    if  ( tedChangeSelectionProperties( ed,
				    &taSetMask, &taSet,
				    &ppUpdMask, &ppNew,
				    &spUpdMask, (SectionProperties *)0 ) )
	{ LDEB(1);	}

    docCleanParagraphProperties( &ppNew );

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt the horizontal ruler to the current paragraph.		*/
/*									*/
/************************************************************************/

void tedDocAdaptHorizontalRuler(	EditDocument *		ed,
					BufferItem *		bi )
    {
    if  ( ed->edTopRuler )
	{
	TedDocument *		td= (TedDocument *)ed->edPrivateData;
	BufferDocument *	bd= td->tdDocument;

	AppDrawingData *	add= &(ed->edDrawingData);

	int			leftIndent;
	int			firstIndent;
	int			rightIndent;

	ParagraphFrame		pf;

	int			x0GeometryPixels;
	int			x1GeometryPixels;

	const int		bottom= -1;
	const int		stripHigh= -1;

	BlockFrame		bf;

	docBlockFrameTwips( &bf, bi, bd, bi->biTopPosition.lpPage,
					    bi->biTopPosition.lpColumn );

	docParagraphFrameTwips( &pf, &bf, bottom, stripHigh, bi );
	tedParagraphFramePixels( &pf, add, bi );

	{
	TabStopList *	tsl= &(bi->biParaTabStopList);
	int		tab;
	TabStop *	ts= tsl->tslTabStops;

	for ( tab= 0; tab < tsl->tslTabStopCount; ts++, tab++ )
	    {
	    ts->tsPixels= X_PIXELS( add,
					pf.pfX0GeometryTwips+ ts->tsTwips );
	    }
	}

	x0GeometryPixels= X_PIXELS( add, pf.pfX0GeometryTwips );
	x1GeometryPixels= X_PIXELS( add, pf.pfX1GeometryTwips );

	leftIndent= pf.pfX0TextLinesPixels;
	firstIndent= pf.pfX0FirstLinePixels;
	rightIndent= pf.pfX1TextLinesPixels;

	{
	TabStopList *	tsl= &(bi->biParaTabStopList);

	tedAdaptHorizontalRuler( ed->edTopRuler, ed->edTopRulerWidget,
		    add->addDocRect.drX0, add->addDocRect.drX1,
		    firstIndent, leftIndent, rightIndent,
		    add->addPaperRect.drX1, tsl );
	}

	if  ( bi->biParaInTable )
	    {
	    int			csCount;
	    ColumnSeparator *	cs;

	    BufferItem *	sectBi;

	    int			col;
	    int			row;
	    int			row0;
	    int			row1;

	    if  ( docDelimitTable( bi, &sectBi, &col, &row0, &row, &row1 ) )
		{ LDEB(1); return;	}

	    if  ( tedGetColumns( bi, add, sectBi, &cs, &csCount ) )
		{ LDEB(1); return;	}

	    tedSetRulerColumns( ed->edTopRulerWidget, ed->edTopRuler,
					x0GeometryPixels,
					x1GeometryPixels,
					cs, csCount );
	    }
	else{
	    tedSetRulerColumns( ed->edTopRulerWidget, ed->edTopRuler,
					x0GeometryPixels,
					x1GeometryPixels,
					(ColumnSeparator *)0, 0 );
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make the ruler administration for the top ruler widget of a		*/
/*  document.								*/
/*									*/
/************************************************************************/

int tedDocSetTopRuler(	EditDocument *	ed )
    {
    EditApplication *		ea= ed->edApplication;
    AppDrawingData *		add= &(ed->edDrawingData);

    int				topRulerHeight= ed->edTopRulerHeightPixels;
    int				unitInt= ea->eaUnitInt;

    ed->edTopRuler= tedMakeTopRuler(
			topRulerHeight,		/*  sizeAcross		*/
			add->addMagnifiedPixelsPerTwip,
			add->addMagnification,	/*  magnification	*/

			ed->edLeftRulerWidthPixels,	/*  minUnused	*/
			ed->edRightRulerWidthPixels,	/*  maxUnused	*/

			add->addDocRect.drX0,
			add->addDocRect.drX1,
			ed->edVisibleRect.drX0,
			ed->edVisibleRect.drX1,

			add->addPaperRect.drX1,	/*  rulerC1		*/
			ea->eaRulerFont,
			unitInt );		/*  whatUnit		*/

    appDrawSetConfigureHandler( ed->edTopRulerWidget,
			tedHorizontalRulerConfigure, (void *)ed->edTopRuler );

    appDrawSetRedrawHandler( ed->edTopRulerWidget,
			tedRedrawHorizontalRuler, (void *)ed->edTopRuler );

    appDrawSetButtonPressHandler( ed->edTopRulerWidget,
				tedHorizontalRulerButtonDown, (void *)ed );

    return 0;
    }

int tedDocSetLeftRuler(	EditDocument *	ed )
    {
    EditApplication *		ea= ed->edApplication;
    AppDrawingData *		add= &(ed->edDrawingData);

    int				leftRulerWidth= ed->edLeftRulerWidthPixels;
    int				unitInt= ea->eaUnitInt;

    ed->edLeftRuler= tedMakeLeftRuler(
			leftRulerWidth,		/*  sizeAcross		*/
			add->addMagnifiedPixelsPerTwip,
			add->addMagnification,	/*  magnification	*/

			0, 0,			/*  {min,max}Unused	*/

			add->addDocRect.drY0,
			add->addDocRect.drY1,

			ed->edVisibleRect.drY0,
			ed->edVisibleRect.drY1,

			add->addPaperRect.drY1,	/*  rulerC1		*/
			ea->eaRulerFont,
			unitInt,		/*  whatUnit		*/
			add->addPageStepPixels,	/*  pageStep.		*/
			ea->eaPageNumberFormat );

    appDrawSetConfigureHandler( ed->edLeftRulerWidget,
			tedVerticalRulerConfigure, (void *)ed->edLeftRuler );

    appDrawSetRedrawHandler( ed->edLeftRulerWidget,
			tedRedrawVerticalRuler, (void *)ed->edLeftRuler );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Callbacks for the Copy/Paste ruler menu options.			*/
/*									*/
/*  1)  The ruler paste is a selection paste with the selection		*/
/*	constrained to the last position of the first paragraph in the	*/
/*	selection.							*/
/*  2)  Remember the bytes expecting that someone will ask for them.	*/
/*  3)  Tell that we have a ruler paste available.			*/
/*									*/
/************************************************************************/

APP_MENU_CALLBACK_H( tedDocFormatCopyRul, fontsOption, voided, e )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    SimpleOutputStream *	sos;

    const int			saveBookmarks= 0;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		dsPara;

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return;	}

    /*  1  */
    dsPara= ds;
    docLastPosition( &(dsPara.dsBegin), dsPara.dsBegin.dpBi );
    docSetIBarSelection( &dsPara, &(dsPara.dsBegin) );

    /*  2  */
    sos= sioOutMemoryOpen( &(td->tdCopiedRuler) );
    if  ( ! sos )
	{ XDEB(sos); return;	}

    if  ( docRtfSaveDocument( sos, td->tdDocument, &dsPara, saveBookmarks ) )
	{ LDEB(1); sioOutClose( sos ); return;	}

    if  ( sioOutClose( sos ) )
	{ LDEB(1); return;	}

    /*
    sos= sioOutStdioOpen( "/tmp/ruler.rtf" );
    docRtfSaveDocument( sos, td->tdDocument, &dsPara, saveBookmarks );
    sioOutClose( sos );
    */

    /*  2  */
    appDocOwnSelection( ed, "RTFRULER", TedRulerTargets, TedRulerTargetCount );
    }

APP_MENU_CALLBACK_H( tedDocFormatPasteRul, fontsOption, voided, e )
    {
    EditDocument *			ed= (EditDocument *)voided;

    appDocAskForPaste( ed, "RTFRULER" );
    }

/************************************************************************/
/*									*/
/*  Merge the paragraphs in the selection.				*/
/*									*/
/************************************************************************/
void tedDocFormatOnePara(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs )
    {
    EditDocument *			ed= (EditDocument *)voided;

    tedMergeParagraphsInSelection( ed );

    tedAdaptToolsToSelection( ed );

    appDocumentChanged( ed, 1 );
    }

