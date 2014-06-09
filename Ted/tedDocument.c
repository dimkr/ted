#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	<sioStdio.h>
#   include	<appSystem.h>
#   include	<utilMatchFont.h>

#   include	<appSpellTool.h>

#   include	"tedApp.h"
#   include	"tedRuler.h"

#   include	"docLayout.h"
#   include	"docEdit.h"
#   include	"docFind.h"

#   include	<appDebugon.h>

#   ifdef USE_MOTIF
#	include	<X11/cursorfont.h>
#   endif

#   define VALIDATE_TREE	0

/************************************************************************/
/*									*/
/*  Ted, callbacks for the DrawingArea.					*/
/*									*/
/************************************************************************/

void tedDrawRectangle(	APP_WIDGET		w,
			EditDocument *		ed,
			DocumentRectangle *	drClip,
			int			ox,
			int			oy )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    AppDrawingData *		add= &(ed->edDrawingData);
    AppColors *			ac= &(ed->edColors);

    /*  2,3,4  */
    tedRedrawRectangle( w, td, drClip, add, ac, ox, oy );
    }

/************************************************************************/
/*									*/
/*  Scrolling callbacks.						*/
/*									*/
/************************************************************************/

void tedMoveObjectWindows(	EditDocument *		ed )
    {
    AppDrawingData *		add= &(ed->edDrawingData);
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    InsertedObject *		io;

    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    DocumentPosition		dpObject;
    PositionGeometry		pg;
    int				part;
    const int			lastOne= 1;

    docInitDocumentPosition( &dpObject );

    if  ( tedGetObjectSelection( td, &part, &dpObject, &io ) )
	{ LDEB(1); return;	}

    tedPositionGeometry( &pg, &dpObject,
				lastOne, bd, add, &(td->tdScreenFontList) );

    tedSetObjectWindows( ed, &pg, io, ox, oy );
    }

void tedDocHorizontalScrollbarCallback(	APP_WIDGET	w,
					void *		voided,
					void *		voidscbs )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    appDocHorizontalScrollbarCallback( w, voided, voidscbs );

    if  ( td->tdSelectionDescription.sdIsObjectSelection )
	{ tedMoveObjectWindows( ed );	}
    }

void tedDocVerticalScrollbarCallback(	APP_WIDGET	w,
					void *		voided,
					void *		voidscbs )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    appDocVerticalScrollbarCallback( w, voided, voidscbs );

    if  ( td->tdSelectionDescription.sdIsObjectSelection )
	{ tedMoveObjectWindows( ed );	}
    }

/************************************************************************/
/*									*/
/*  Last phase in building a document window.				*/
/*									*/
/*  1)  Set the position by hand, because the regular routine sends an	*/
/*	expose to a window that does not exist yet.			*/
/*  2)  Forget the name of document templates to force a save-as.	*/
/*	Also forget it is a template, to make sure that it is saved as	*/
/*	an ordinary document.						*/
/*									*/
/************************************************************************/

int tedFinishDocumentSetup(	EditDocument *		ed )
    {
    EditApplication *		ea= ed->edApplication;
    AppDrawingData *		add= &(ed->edDrawingData);
    AppColors *			ac= &(ed->edColors);

    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);

    const TedAppResources *	tar= (TedAppResources *)ea->eaResourceData;
    const char *		selColorName= tar->tarSelectionColor;
    const char *		xselColorName= tar->tarCopiedSelectionColor;
    /*
    char *			tableColorName= "gray80";
    */

    {
    DocumentPosition		dpFirst;

    /*  1  */
    if  ( ! docFirstDocumentPosition( &dpFirst, bd ) )
	{
	const int			lastOne= 1;
	const int			lastLine= 0;
	int				part;
	const TextParticule *		tp;
	ScreenFontList *		sfl;

	sfl= &(td->tdScreenFontList);

	if  ( tar->tarFindPattern					&&
	      ! tedFindSetPattern( ed,
			    tar->tarFindPattern, tar->tarFindRegex )	&&
	      ! docFindFindNextInDocument( &(td->tdDocumentSelection),
		    &dpFirst, bd,
		    docFindParaFindNext, (void *)td->tdFindProg )	)
	    {
	    dpFirst= td->tdDocumentSelection.dsBegin;
	    }
	else{
	    docSetIBarSelection( &(td->tdDocumentSelection), &dpFirst );
	    }

	tedSelectionGeometry( &(td->tdSelectionGeometry),
			     &(td->tdDocumentSelection), lastLine,
			     bd, add, &(td->tdScreenFontList) );


	if  ( docFindParticuleOfPosition( &part, &dpFirst, lastOne ) )
	    { LDEB(dpFirst.dpStroff); return -1;	}

	tp= dpFirst.dpBi->biParaParticules+ part;

	utilGetTextAttributeByNumber( &(td->tdCurrentTextAttribute),
						&(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );
	td->tdCurrentTextAttributeNumber= tp->tpTextAttributeNumber;

	td->tdCurrentScreenFont= -1;
	if  ( td->tdCurrentTextAttributeNumber <
	      sfl->sflAttributeToScreenCount )
	    {
	    td->tdCurrentScreenFont=
		sfl->sflAttributeToScreen[td->tdCurrentTextAttributeNumber];
	    }

	docDescribeSelection( &(td->tdSelectionDescription),
				    &(td->tdDocumentSelection),
				    bd, ed->edDocumentId, ed->edIsReadonly );
	}
    else{ docListItem( 0, &(bd->bdItem) );	}

    /*  2  */
    if  ( dp->dpIsDocumentTemplate )
	{
	appSetDocumentFilename( ed, (const char *)0 );
	dp->dpIsDocumentTemplate= 0;
	}

#   if VALIDATE_TREE
    LDEB(1);
    if  ( docCheckItem( &(bd->bdItem) ) )
	{ LDEB(2); docListItem( 0, &(bd->bdItem) ); abort();	}
#   endif

    }

    if  ( appFinishDocumentSetup( ea, ed ) )
	{ LDEB(1); return -1;	}

    if  ( appAllocateColors( add, ac ) )
	{ LDEB(1); return -1;	}

    if  ( ed->edColors.acAllocator.caDepth < 4 )
	{
	if  ( appColorRgb( &(td->tdFieldColor), &(ed->edColors), 0, 0, 0 ) )
	    { LDEB(1); return -1;	}

	td->tdSelColor.rgb8Red= 0;
	td->tdSelColor.rgb8Green= 0;
	td->tdSelColor.rgb8Blue= 0;

	td->tdCopiedSelColor.rgb8Red= 0;
	td->tdCopiedSelColor.rgb8Green= 0;
	td->tdCopiedSelColor.rgb8Blue= 0;

	td->tdTableColor.rgb8Red= 0;
	td->tdTableColor.rgb8Green= 0;
	td->tdTableColor.rgb8Blue= 0;
	}
    else{
	APP_COLOR_RGB	acSel;
	APP_COLOR_RGB	acCopiedSel;

	if  ( appColorNamed( &acSel, &ed->edColors, selColorName ) )
	    {
	    if  ( appColorFindRgb( &acSel, &(ed->edColors), 176, 196, 222 ) )
		{ SDEB(selColorName); return -1;	}
	    }

	if  ( appColorNamed( &acCopiedSel, &(ed->edColors), xselColorName ) )
	    {
	    if  ( appColorFindRgb( &acCopiedSel,
					    &(ed->edColors), 176, 176, 176 ) )
		{ SDEB(xselColorName); return -1;	}
	    }

	if  ( appColorRgb( &(td->tdFieldColor), &ed->edColors, 0, 0, 200 ) )
	    {
	    if  ( appColorFindRgb( &(td->tdFieldColor),
					       &(ed->edColors), 0, 0, 255 ) )
		{ LDEB(1); return -1;	}
	    }

#	ifdef USE_MOTIF
	td->tdSelColor.rgb8Red= acSel.red/ 256;
	td->tdSelColor.rgb8Green= acSel.green/ 256;
	td->tdSelColor.rgb8Blue= acSel.blue/ 256;

	td->tdCopiedSelColor.rgb8Red= acCopiedSel.red/ 256;
	td->tdCopiedSelColor.rgb8Green= acCopiedSel.green/ 256;
	td->tdCopiedSelColor.rgb8Blue= acCopiedSel.blue/ 256;

	td->tdTableColor.rgb8Red= add->addBackColor.red/ 256;
	td->tdTableColor.rgb8Green= add->addBackColor.green/ 256;
	td->tdTableColor.rgb8Blue= add->addBackColor.blue/ 256;
#	endif

#	ifdef USE_GTK
	td->tdSelColor.rgb8Red= acSel.red/ 256;
	td->tdSelColor.rgb8Green= acSel.green/ 256;
	td->tdSelColor.rgb8Blue= acSel.blue/ 256;

	td->tdCopiedSelColor.rgb8Red= acCopiedSel.red/ 256;
	td->tdCopiedSelColor.rgb8Green= acCopiedSel.green/ 256;
	td->tdCopiedSelColor.rgb8Blue= acCopiedSel.blue/ 256;

	td->tdTableColor.rgb8Red= add->addBackColor.red/ 256;
	td->tdTableColor.rgb8Green= add->addBackColor.green/ 256;
	td->tdTableColor.rgb8Blue= add->addBackColor.blue/ 256;
#	endif
	}

    if  ( tedOpenItemObjects( &(bd->bdItem), bd, &(ed->edColors),
						    &(ed->edDrawingData) ) )
	{ LDEB(1);	}

    td->tdDrawTableGrid= tar->tarShowTableGridInt;

    appGuiSetToggleItemState( td->tdDrawTableGridOption,
						td->tdDrawTableGrid >= 0 );

    if  ( ed->edIsReadonly )
	{ tedMakeDocumentReadonly( ed );	}

    if  ( tedHasSelection( td ) )
	{ tedAdaptToolsToSelection( ed );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Scroll to the selection the first time the document becomes visible	*/
/*									*/
/************************************************************************/

void tedDocumentFirstVisible(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    DocumentRectangle		dr= ed->edVisibleRect;

    int				pScrolledX= 0;
    int				pScrolledY= 0;

    if  ( td->tdSelectionGeometry.sgRectangle.drX1 > dr.drX1 )
	{
	int sh= td->tdSelectionGeometry.sgRectangle.drX1- dr.drX1;

	dr.drX0 += sh;
	dr.drX1 += sh;
	}
    if  ( td->tdSelectionGeometry.sgRectangle.drY1 > dr.drY1 )
	{
	int sh= td->tdSelectionGeometry.sgRectangle.drY1- dr.drY1;

	dr.drY0 += sh;
	dr.drY1 += sh;
	}

    appScrollToRectangle( ed, dr.drX0, dr.drY0, dr.drX1, dr.drY1,
						    &pScrolledX, &pScrolledY );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the document widget and the rulers.				*/
/*									*/
/*  1)  Calculate the different width of the margins.			*/
/*  2)  Insert a Form in the scrolled window.				*/
/*  3)  Insert the top ruler in the form.				*/
/*  4)  Insert the bottom ruler in the form.				*/
/*  5)  Insert the left ruler in the form.				*/
/*  6)  Insert the right ruler in the form.				*/
/*  7)  Insert the document widget in the form.				*/
/*									*/
/************************************************************************/

void tedFreeDocument(		void *			voidtd,
				int			format,
				AppDrawingData *	add )
    {
    TedDocument *	td= (TedDocument *)voidtd;

    tedCleanCursorBlink( td );

    if  ( td->tdDocument )
	{
	BufferDocument *	bd= td->tdDocument;
	int			noteCount= 0;
	int			bulletsDeleted= 0;
	int			paragraphCount= 0;

	docCleanItemObjects( &noteCount, &bulletsDeleted, &paragraphCount,
					    bd, &(bd->bdItem),
					    (void *)add, tedCloseObject );

	docFreeDocument( bd );
	}

    docCleanScreenFontList( &(td->tdScreenFontList) );

    utilCleanMemoryBuffer( &(td->tdCopiedSelection) );
    utilCleanMemoryBuffer( &(td->tdCopiedFont) );
    utilCleanMemoryBuffer( &(td->tdCopiedRuler) );

    appCleanBitmapImage( &(td->tdCopiedImage) );

    if  ( td->tdFindProg )
	{ free( td->tdFindProg );	}

    free( td );
    }

int tedMakeDocumentWidget(	EditApplication *	ea,
				EditDocument *		ed )
    {
    if  ( appMakeDocumentWidget( ea, ed ) )
	{ LDEB(1); return -1;	}

    if  ( ! ea->eaDocumentCursor )
	{
#	ifdef USE_MOTIF
	ea->eaDocumentCursor= XCreateFontCursor(
			    XtDisplay( ea->eaToplevel.atTopWidget), XC_xterm );
	if  ( ! ea->eaDocumentCursor )
	    { LDEB(ea->eaDocumentCursor);	}
#	endif

#	ifdef USE_GTK
	ea->eaDocumentCursor= gdk_cursor_new( GDK_XTERM );
	if  ( ! ea->eaDocumentCursor )
	    { XDEB(ea->eaDocumentCursor);	}
#	endif
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find out whether a document uses just one code page.		*/
/*									*/
/************************************************************************/

static int tedDetermineCodepage(	BufferDocument *	bd )
    {
    const DocumentFontList *	dfl= &(bd->bdProperties.dpFontList);
    const DocumentFont *	df;
    int				font;

    int				encoding= -1;
    int				encodingCount= 0;

    df= dfl->dflFonts;
    for ( font= 0; font < dfl->dflFontCount; df++, font++ )
	{
	if  ( ! df->dfUsed )
	    { continue;	}
	if  ( ! df->dfOfficeCharsetMapping )
	    { XDEB(df->dfOfficeCharsetMapping); continue;	}

	if  ( encoding < 0 )
	    {
	    encoding= df->dfOfficeCharsetMapping->ocmPsFontEncoding;
	    encodingCount= 1;
	    }
	else{
	    if  ( encoding != df->dfOfficeCharsetMapping->ocmPsFontEncoding )
		{ encodingCount++; }
	    }
	}

    if  ( encodingCount != 1 )
	{ return -1;	}

    return utilWindowsCodepageFromEncoding( encoding );
    }

/************************************************************************/
/*									*/
/*  Make a new empty document.						*/
/*									*/
/************************************************************************/

int tedNewDocument(	EditApplication *	ea,
			EditDocument *		ed,
			const char *		filename )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd;
    DocumentProperties *	dp;
    TextAttribute		ta;

    time_t			now;

    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;
    int				rval= 0;

    tedDetermineDefaultSettings( tar );

    if  ( appPostScriptFontCatalog( ea ) )
	{ SDEB(ea->eaAfmDirectory); return -1;	}

    bd= docNewFile( &ta, ea->eaDefaultFont,
				    DOCcharsetANSI, tar->tarDefaultAnsicpgInt,
				    &(ea->eaPostScriptFontList),
				    &(ea->eaDefaultDocumentGeometry) );
    if  ( ! bd )
	{ XDEB(bd); rval= -1; goto ready;	}

    ed->edFormat= TEDdockindRTF; /* rtf */
    td->tdDocument= bd;
    dp= &(bd->bdProperties);

    {
    int		l= 0;

    l += strlen( ea->eaNameAndVersion );
    l += 2;
    l += strlen( ea->eaReference );
    l += 2;
    l += 1;

    dp->dpGenerator= malloc( l );
    if  ( ! dp->dpGenerator )
	{ LXDEB(l,dp->dpGenerator);	}
    else{
	sprintf( (char *)dp->dpGenerator, "%s (%s);",
				ea->eaNameAndVersion, ea->eaReference );
	}
    }

    /*  2  */
    if  ( appPostScriptFontCatalog( ea ) )
	{ SDEB(ea->eaAfmDirectory); return -1;	}

    if  ( filename )
	{
	const char *	ext= appFileExtensionOfName( filename );

	if  ( ext && ! strcmp( ext, "rtf" ) )
	    { ed->edFormat= TEDdockindRTF;	}
	if  ( ext && ! strcmp( ext, "txt" ) )
	    { ed->edFormat= TEDdockindTEXT_SAVE_FOLDED;	}
	}

    if  ( ea->eaAuthor )
	{ dp->dpAuthor= (unsigned char *)strdup( ea->eaAuthor ); }

    now= time( (time_t *)0 );
    dp->dpCreatim= *localtime( &now );

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Perform the setup of the editing environment for a document.	*/
/*									*/
/*  8)  Determine the code page of the document.			*/
/*  9)  Do not use the 1252 codepage as an explicit setting: It is	*/
/*	almost certainly returned by a default 'Ted' installation, and	*/
/*	is almost impossible to get rid of.				*/
/*									*/
/************************************************************************/

int tedLayoutDocument(		void *				privateData,
				int				format,
				AppDrawingData *		add,
				const DocumentGeometry *	dgDef )
    {
    TedDocument *		td= (TedDocument *)privateData;
    BufferDocument *		bd= td->tdDocument;
    const DocumentProperties *	dp= &(bd->bdProperties);
    const DocumentGeometry *	dgDoc= &(dp->dpGeometry);

    int				noteNumbersChanged= 0;

    RecalculateFields		rf;

    docInitRecalculateFields( &rf );

    rf.rfBd= bd;
    rf.rfVoidadd= (void *)add;
    rf.rfCloseObject= tedCloseObject;
    rf.rfUpdateFlags= FIELDdoDOC_INFO|FIELDdoDOC_COMPLETE|FIELDdoCHFTN;
    rf.rfFieldsUpdated= 0;

    tedScreenRectangles( add, dgDoc );

    if  ( docRecalculateTextLevelFields( &rf, &(bd->bdItem) ) )
	{ LDEB(1); return -1;	}

    if  ( tedLayoutDocumentTree( td, add ) )
	{ LDEB(1); return -1;	}

    docRenumberNotes( &noteNumbersChanged, bd );

    rf.rfUpdateFlags= FIELDdoDOC_FORMATTED;
    rf.rfFieldsUpdated= 0;

    if  ( noteNumbersChanged )
	{ rf.rfUpdateFlags |= FIELDdoCHFTN;	}

    if  ( docRecalculateTextLevelFields( &rf, &(bd->bdItem) ) )
	{ LDEB(1); return -1;	}

    if  ( rf.rfFieldsUpdated > 0 && tedLayoutDocumentTree( td, add ) )
	{ LDEB(1); return -1;	}

    /*  8  */
    if  ( bd->bdProperties.dpAnsiCodepage < 0 )
	{
	bd->bdProperties.dpAnsiCodepage= tedDetermineCodepage( bd );

	/*  9  */
	if  ( bd->bdProperties.dpAnsiCodepage == 1252 )
	    { bd->bdProperties.dpAnsiCodepage= -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save a document.							*/
/*									*/
/************************************************************************/

int tedSaveDocument(	const void *		privateData,
			int			format,
			const char *		applicationId,
			const char *		documentTitle,
			const char *		filename )
    {
    TedDocument *		td= (TedDocument *)privateData;
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);
    SimpleOutputStream *	sos;

    const int			saveBookmarks= 1;

    const int			asMimeAggr= 0;

    time_t			now;

    sos= sioOutStdioOpen( filename );
    if  ( ! sos )
	{ /* SXDEB(filename,sos); */ return -1;	}

    switch( format )
	{
	case TEDdockindRTF:
	    now= time( (time_t *)0 );
	    dp->dpRevtim= *localtime( &now );

	    if  ( docRtfSaveDocument( sos, bd, (const DocumentSelection *)0,
							    saveBookmarks ) )
		{ SDEB(filename); sioOutClose( sos ); return -1;	}
	    break;

	case TEDdockindTEXT_OPEN:
	    LDEB(format);
	    /*FALLTHROUGH*/

	case TEDdockindTEXT_SAVE_FOLDED:
	    if  ( docPlainSaveDocument( sos, bd,
					    (const DocumentSelection *)0,
					    1, 1 ) )
		{ SDEB(filename); sioOutClose( sos ); return -1;	}
	    break;

	case TEDdockindTEXT_SAVE_WIDE:
	    if  ( docPlainSaveDocument( sos, bd,
					    (const DocumentSelection *)0,
					    0, 1 ) )
		{ SDEB(filename); sioOutClose( sos ); return -1;	}
	    break;

	case TEDdockindHTML_FILES:
	    if  ( docHtmlSaveDocument( sos, bd, asMimeAggr,
						(const char *)0, filename ) )
		{ SDEB(filename); sioOutClose( sos ); return -1;	}
	    break;

	default:
	    LDEB(format); return -1;
	}

    sioOutClose( sos );
    
    return 0;
    }


/************************************************************************/
/*									*/
/*  Allocate private data for a document.				*/
/*									*/
/************************************************************************/

void * tedMakePrivateData()
    {
    int			i;
    TedDocument *	td;

    td= (TedDocument *)malloc( sizeof(TedDocument) );
    if  ( ! td )
	{ XDEB(td); return (void *)0;	}

    docInitSelectionDescription( &(td->tdSelectionDescription) );

    td->tdDocument= (BufferDocument *)0;
    utilInitTextAttribute( &(td->tdCurrentTextAttribute) );
    td->tdCurrentTextAttributeNumber= -1;
    td->tdCurrentScreenFont= -1;

    docInitScreenFontList( &(td->tdScreenFontList) );

    td->tdFormatMenu= (APP_WIDGET)0;
    td->tdFormatMenuButton= (APP_WIDGET)0;
    td->tdFormatOneParaOption= (APP_WIDGET)0;

    td->tdCopyWidget= (APP_WIDGET)0;
    td->tdCutWidget= (APP_WIDGET)0;
    td->tdPasteWidget= (APP_WIDGET)0;

    td->tdInsertMenu= (APP_WIDGET)0;
    td->tdInsertMenuButton= (APP_WIDGET)0;
    td->tdInsPictOption= (APP_WIDGET)0;
    td->tdInsFileOption= (APP_WIDGET)0;
    td->tdInsSymbolOption= (APP_WIDGET)0;
    td->tdInsHyperlinkOption= (APP_WIDGET)0;
    td->tdInsBookmarkOption= (APP_WIDGET)0;
    td->tdInsInsertFootnoteOption= (APP_WIDGET)0;
    td->tdInsInsertEndnoteOption= (APP_WIDGET)0;
    td->tdInsInsertChftnsepOption= (APP_WIDGET)0;
    td->tdInsInsertTableOption= (APP_WIDGET)0;
    td->tdInsInsertPageNumberOption= (APP_WIDGET)0;
    td->tdInsInsertLineBreakOption= (APP_WIDGET)0;
    td->tdInsInsertPageBreakOption= (APP_WIDGET)0;
    td->tdInsInsertSectBreakOption= (APP_WIDGET)0;

    td->tdTableMenu= (APP_WIDGET)0;
    td->tdTableMenuButton= (APP_WIDGET)0;

    td->tdTabInsertTableOption= (APP_WIDGET)0;
    td->tdTabAddRowOption= (APP_WIDGET)0;
    td->tdTabAddColumnOption= (APP_WIDGET)0;

    td->tdSelectTableWidget= (APP_WIDGET)0;
    td->tdSelectRowWidget= (APP_WIDGET)0;
    td->tdSelectColumnOption= (APP_WIDGET)0;

    td->tdDeleteTableWidget= (APP_WIDGET)0;
    td->tdDeleteRowWidget= (APP_WIDGET)0;
    td->tdDeleteColumnOption= (APP_WIDGET)0;

    td->tdDrawTableGridOption= (APP_WIDGET)0;

    td->tdFontMenu= (APP_WIDGET)0;
    td->tdFontMenuButton= (APP_WIDGET)0;
    td->tdFontBoldOption= (APP_WIDGET)0;
    td->tdFontItalicOption= (APP_WIDGET)0;
    td->tdFontUnderlinedOption= (APP_WIDGET)0;
    td->tdFontSuperscriptOption= (APP_WIDGET)0;
    td->tdFontSubscriptOption= (APP_WIDGET)0;

    td->tdToolsMenu= (APP_WIDGET)0;
    td->tdToolsMenuButton= (APP_WIDGET)0;

    docInitDocumentSelection( &(td->tdDocumentSelection) );
    docInitSelectionGeometry( &(td->tdSelectionGeometry) );

    td->tdVisibleSelectionCopied= 0;
    td->tdCanReplaceSelection= 0;

    td->tdCopiedSelectionClosed= 0;
    utilInitMemoryBuffer( &(td->tdCopiedSelection) );
    utilInitMemoryBuffer( &(td->tdCopiedFont) );
    utilInitMemoryBuffer( &(td->tdCopiedRuler) );

    appInitBitmapImage( &(td->tdCopiedImage) );

    td->tdFindProg= (void *)0;

#   ifdef USE_MOTIF
    td->tdHideIBarId= (XtIntervalId)0;
    td->tdShowIBarId= (XtIntervalId)0;
#   endif
#   ifdef USE_GTK
    td->tdHideIBarId= 0;
    td->tdShowIBarId= 0;
#   endif

    td->tdObjectWindow= (APP_WINDOW)0;
    for ( i= 0; i < RESIZE_COUNT; i++ )
	{ td->tdObjectResizeWindows[i]= (APP_WINDOW)0;	}
    td->tdObjectResizeCorner= -1;
    td->tdObjectCornerMovedX= 0;
    td->tdObjectCornerMovedY= 0;
    td->tdScaleChangedX= 0;
    td->tdScaleChangedY= 0;

    td->tdDrawTableGrid= 1;

    return (void *)td;
    }

/************************************************************************/
/*									*/
/*  Keep track of focus.						*/
/*									*/
/************************************************************************/

APP_EVENT_HANDLER_H( tedObserveFocus, w, voided, event )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    EditApplication *	ea= ed->edApplication;

    int			inout= 0;

    if  ( appDrawGetInoutFromFocusEvent( &inout, w, event ) )
	{ return;	}

    if  ( inout > 0 )
	{
	if  ( ea->eaCurrentDocument != ed )
	    {
	    appSetCurrentDocument( ea, ed );

	    if  ( ea->eaFindTool && ed->edIsReadonly )
		{ appFindToolEnableReplace( ea->eaFindTool, 0 ); }

	    if  ( ea->eaSpellTool )
		{ appEnableSpellTool( ea->eaSpellTool, ! ed->edIsReadonly ); }

	    tedAdaptPageToolToDocument( ea, ed );

	    tedAdaptFormatToolToDocument( ed, 0 );
	    }

	if  ( tedHasIBarSelection( td ) )
	    { tedStartCursorBlink( ed );	}
	}

    if  ( inout < 0 )
	{ tedStopCursorBlink( ed ); }
    }

/************************************************************************/
/*									*/
/*  Print a document to PostScript.					*/
/*									*/
/************************************************************************/

int tedPrintDocument(	SimpleOutputStream *		sos,
			const PrintJob *		pj,
			const PrintGeometry *		pg )
    {
    EditApplication *		ea= pj->pjApplication;
    TedDocument *		td= (TedDocument *)pj->pjPrivateData;
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);

    time_t			now;

    RecalculateFields		rf;

    docInitRecalculateFields( &rf );

    rf.rfBd= bd;
    rf.rfVoidadd= (void *)pj->pjDrawingData;
    rf.rfCloseObject= tedCloseObject;
    rf.rfUpdateFlags= FIELDdoDOC_FORMATTED|FIELDdoDOC_COMPLETE|FIELDdoDOC_INFO;
    rf.rfFieldsUpdated= 0;

    now= time( (time_t *)0 );
    dp->dpPrintim= *localtime( &now );

    if  ( docRecalculateTextLevelFields( &rf, &(bd->bdItem) ) )
	{ LDEB(1); return -1;	}

    if  ( rf.rfFieldsUpdated > 0				&&
	  tedLayoutDocumentTree( td, pj->pjDrawingData )	)
	{ LDEB(1); return -1;	}

    if  ( docPsPrintDocument( sos, pj->pjTitle, ea->eaApplicationName,
				ea->eaReference, ea->eaFontDirectory,
				pj->pjDrawingData,
				td->tdDocument, pg,
				tedCloseObject ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

void tedSuggestNup(	PrintGeometry *	pg,
			void *		privateData )
    {
    TedDocument *		td= (TedDocument *)privateData;
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);

    if  ( dp->dpTwoOnOne )
	{
	pg->pgGridRows= 2;
	pg->pgGridCols= 1;
	pg->pgGridHorizontal= 1;
	pg->pgScalePagesToFit= 1;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Open a document.							*/
/*									*/
/*  1)  Open an input stream.						*/
/*  2)  Try to read as RTF.						*/
/*  3)  If this fails, try as plain text.				*/
/*									*/
/************************************************************************/

int tedOpenDocumentFile(	EditApplication *	ea,
				int *			pFormat,
				BufferDocument **	pBd,
				const char *		filename,
				APP_WIDGET		relative,
				APP_WIDGET		option )
    {
    const char *		ext;
    SimpleInputStream *		sis;

    BufferDocument *		bd;
    int				triedRtf= 0;

    int				resp;
    AppFileMessageResources *	afmr= &(ea->eaFileMessageResources);
    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    int				longestPara;

    tedDetermineDefaultSettings( tar );

    if  ( appPostScriptFontCatalog( ea ) )
	{ SDEB(ea->eaAfmDirectory); return -1;	}

    ext= appFileExtensionOfName( filename );

    /*  1  */
    sis= sioInStdioOpen( filename );
    if  ( ! sis )
	{
	appQuestionRunSubjectErrorDialog( ea, relative, option,
					filename, afmr->afmrFileNoAccess );

	return -1;
	}

    if  ( ext && ! strcmp( ext, "rtf" ) )
	{
	/*  2  */
	bd= docRtfReadFile( sis, &(ea->eaPostScriptFontList),
						tar->tarDefaultAnsicpgInt );

	sioInClose( sis );
	
	if  ( bd )
	    {
	    *pFormat= TEDdockindRTF; *pBd= bd;

	    utilFontlistSetPreferredEncodings( &(bd->bdProperties.dpFontList) );

	    if  ( docPropertiesSetFilename( &(bd->bdProperties), filename ) )
		{ LDEB(1);	}

	    return 0;
	    }
	else{
	    resp= appQuestionRunSubjectOkCancelDialog( ea,
				    relative, option,
				    filename, tar->tarFileNotRtf,
				    (char *)0, (char *)0 );

	    if  ( resp != AQDrespOK )
		{ return -1;	}

	    triedRtf= 1;
	    }
	}

    if  ( ext && ! strcmp( ext, "txt" ) )
	{
	bd= docPlainReadFile( sis, &longestPara,
				    DOCcharsetANSI, tar->tarDefaultAnsicpgInt,
				    &(ea->eaDefaultDocumentGeometry) );

	sioInClose( sis );

	if  ( bd )
	    {
	    *pBd= bd;

	    if  ( longestPara > 76 )
		{ *pFormat= TEDdockindTEXT_SAVE_WIDE;		}
	    else{ *pFormat= TEDdockindTEXT_SAVE_FOLDED;		}

	    if  ( docPropertiesSetFilename( &(bd->bdProperties), filename ) )
		{ LDEB(1);	}

	    return 0;
	    }
	else{ SXDEB(filename,bd); return -1;	}
	}

    /*  2  */
    if  ( ! triedRtf )
	{
	bd= docRtfReadFile( sis, &(ea->eaPostScriptFontList),
						tar->tarDefaultAnsicpgInt );
	if  ( bd )
	    {
	    *pFormat= TEDdockindRTF; *pBd= bd;
	    
	    utilFontlistSetPreferredEncodings( &(bd->bdProperties.dpFontList) );

	    if  ( docPropertiesSetFilename( &(bd->bdProperties), filename ) )
		{ LDEB(1);	}

	    return 0;
	    }

	resp= appQuestionRunSubjectOkCancelDialog( ea,
					ea->eaToplevel.atTopWidget, option,
					filename, tar->tarFileNotRtf,
					(char *)0, (char *)0 );

	if  ( resp != AQDrespOK )
	    { return -1;	}
	}

    sis= sioInStdioOpen( filename );
    if  ( ! sis )
	{ SXDEB(filename,sis); return -1;	}

    bd= docPlainReadFile( sis, &longestPara,
				    DOCcharsetANSI, tar->tarDefaultAnsicpgInt,
				    &(ea->eaDefaultDocumentGeometry) );

    sioInClose( sis );

    if  ( bd )
	{
	*pBd= bd;

	if  ( longestPara > 76 )
	    { *pFormat= TEDdockindTEXT_SAVE_WIDE;		}
	else{ *pFormat= TEDdockindTEXT_SAVE_FOLDED;		}

	if  ( docPropertiesSetFilename( &(bd->bdProperties), filename ) )
	    { LDEB(1);	}

	return 0;
	}

    SXDEB(filename,bd); return -1;
    }

