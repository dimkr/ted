/************************************************************************/
/*									*/
/*  Ted: Handle user input.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedLayout.h"
#   include	"tedApp.h"
#   include	"tedRuler.h"

#   include	"appGuiKeys.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Just log events that pass by for debugging purposes.		*/
/*									*/
/*  NOTE the silly constuction to do away with the 'unused' compiler	*/
/*	 warning.							*/
/*									*/
/************************************************************************/

#   ifdef USE_MOTIF

static void tedLogEvent(	Widget		w,
				void *		voided,
				XEvent *	event,
				Boolean *	pRefused )
    {
    EditDocument *		ed= (EditDocument *)voided;

    appDebug( "EVENT \"%s\": %s\n",
			ed->edTitle, APP_X11EventNames[event->type] );

    *pRefused= 1;

    if  ( ! event )
	{ return;	}
    if  ( ! event )
	{ tedLogEvent( w, voided, event, pRefused );	}
    }

#   endif

/************************************************************************/
/*									*/
/*  Handle mouse clicks.						*/
/*									*/
/************************************************************************/

#   define	CLICK2_TO_CHANGE_ROOT	0

typedef struct DraggingContext
    {
    int				dcMouseX;
    int				dcMouseY;
    DocumentPosition		dcAnchorPosition;
    BufferItem *		dcRootItem;
    EditDocument *		dcEd;
    } DraggingContext;

/************************************************************************/
/*									*/
/*  Supposing te click is in a diffrent document tree, reformat that	*/
/*  tree for the context of the click to determine where the location	*/
/*  of the click in the document hierarchy.				*/
/*									*/
/************************************************************************/

static int tedLayoutSelectedRoot(	BufferDocument *	bd,
					AppDrawingData *	add,
					ScreenFontList *	sfl,
					ExternalItem *		eiSet,
					BufferItem *		rootBiSet,
					BufferItem *		bodySectBiSet,
					int			page )
    {
    DocumentPosition		dp;
    int				inHeadFoot;
    DocumentRectangle		drChanged;

    docInitDocumentPosition( &dp );

    inHeadFoot= rootBiSet->biInExternalItem;

    switch( inHeadFoot )
	{
	case DOCinBODY:
	case DOCinFOOTNOTE:
	case DOCinENDNOTE:
	    break;

	case DOCinSECT_HEADER:
	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:
	case DOCinSECT_FOOTER:
	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:

	    if  ( ! eiSet )
		{ XDEB(eiSet); return 1;	}
	    if  ( eiSet->eiPageFormattedFor != page )
		{
		if  ( docLayoutExternalItem( eiSet, &drChanged,
			    page, eiSet->eiY0UsedTwips,
			    bd, bodySectBiSet, add, sfl,
			    tedInitLayoutExternalItem, tedCloseObject ) )
		    { LDEB(page); return 1; }

		eiSet->eiPageSelectedUpon= page;
		}
	    break;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    if  ( ! eiSet )
		{ XDEB(eiSet); return 1;	}
	    if  ( eiSet->eiPageFormattedFor != page )
		{
		LLDEB(eiSet->eiPageFormattedFor,page);

		if  ( docLayoutExternalItem( eiSet, &drChanged,
			    page, eiSet->eiY0UsedTwips,
			    bd, bodySectBiSet, add, sfl,
			    tedInitLayoutExternalItem, tedCloseObject ) )
		    { LDEB(page); return 1; }
		}
	    break;

	default:
	    LDEB(rootBiSet->biInExternalItem);
	}

    if  ( eiSet )
	{ eiSet->eiPageSelectedUpon= page;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Select the current position of the mouse.				*/
/*									*/
/************************************************************************/

static int tedSelectMousePosition(
				unsigned int *			pKeyState,
				APP_WIDGET			w,
				EditDocument *			ed,
				DraggingContext *		dc,
				APP_EVENT *			downEvent )
    {
    AppDrawingData *		add= &(ed->edDrawingData);
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    ScreenFontList *		sfl= &(td->tdScreenFontList);
    BufferDocument *		bd= td->tdDocument;

    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    int				mouseX;
    int				mouseY;
    int				docX;
    int				docY;

    int				otherRoot= 0;

    int				scrolledX= 0;
    int				scrolledY= 0;

    int				button;
    int				upDown;
    int				seq;
    unsigned int		keyState= 0;

    DocumentSelection		dsOld;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    PositionGeometry		pgClick;
    int				partClick;
    DocumentSelection		dsWord;
    int				wordIsObject= 0;

    int				page;
    int				sectNr;
    ExternalItem *		eiSet= (ExternalItem *)0;
    BufferItem *		bodySectBiSet;
    BufferItem *		rootBiSet;

    if  ( tedGetSelection( &dsOld, &sg, &sd, td ) )
	{ LDEB(1); return 1;	}

    if  ( appGetCoordinatesFromMouseButtonEvent( &mouseX, &mouseY,
					    &button, &upDown, &seq, &keyState,
					    w, downEvent ) )
	{ LDEB(1); return 1;	}

    if  ( mouseX < -ox )
	{ mouseX=  -ox;	}
    if  ( mouseY < -oy )
	{ mouseY=  -oy;	}

    dc->dcMouseX= mouseX;
    dc->dcMouseY= mouseY;

    docX= mouseX+ ox;
    docY= mouseY+ oy;

    dc->dcRootItem= &(bd->bdItem);

    if  ( tedFindRootForPosition( &eiSet, &rootBiSet, &bodySectBiSet,
					    &sectNr, &page, ed, docX, docY ) )
	{ LLDEB(docX,docY); return 1;	}

    dc->dcRootItem= rootBiSet;

    otherRoot= ! docSelectionSameRoot( &dsOld, rootBiSet );

    if  ( otherRoot )
	{
#	if  CLICK2_TO_CHANGE_ROOT
	if  ( seq < 2 )
	    { return 1;	}
#	endif

	if  ( tedLayoutSelectedRoot( bd, add, sfl,
				    eiSet, rootBiSet, bodySectBiSet, page ) )
	    { LDEB(page); return 1;	}

#	if CLICK2_TO_CHANGE_ROOT
	{
	DocumentPosition	dp;
	PositionGeometry	pg;

	if  ( tedFindPosition( &dp, &pg, &part,
				bd, dc->dcRootItem, add, sfl, docX, docY ) )
	    { LLDEB(docX,docY); return 1;	}

	tedSetSelectedPosition( ed, &dp, pg.pgAtLineHead,
						    &scrolledX, &scrolledY );

	goto ready;
	}
#	endif

	appExposeRectangle( add, 0, 0, 0, 0 );
	}

    if  ( tedFindPosition( &(dc->dcAnchorPosition), &pgClick, &partClick,
				bd, dc->dcRootItem, add, sfl, docX, docY ) )
	{ LLDEB(docX,docY); return 1;	}

    docWordSelection( &dsWord, &wordIsObject,
					&(dc->dcAnchorPosition), partClick );

    if  ( seq > 1 )
	{
	DocumentSelection	dsNew;

	docInitDocumentSelection( &dsNew );

	docSetIBarSelection( &dsNew, &(dc->dcAnchorPosition) );

	if  ( seq > 2 )
	    {
	    int		partLineBegin;
	    int		partLineEnd;

	    docLineSelection( &dsNew, &partLineBegin, &partLineEnd,
				dc->dcAnchorPosition.dpBi, pgClick.pgLine );
	    }
	else{ dsNew= dsWord;	}

	tedSetSelection( ed, &dsNew, pgClick.pgAtLineHead,
						    &scrolledX, &scrolledY );

	goto ready;
	}

    if  ( ! otherRoot && ( keyState & KEY_SHIFT_MASK ) )
	{
	dc->dcAnchorPosition= dsOld.dsAnchor;

	if  ( tedExtendSelectionToXY( ed, dc->dcRootItem,
			&(dc->dcAnchorPosition), dc->dcMouseX, dc->dcMouseY ) )
	    { LDEB(1); return 1;	}

	goto ready;
	}

    if  ( docParaHeadFieldKind( dc->dcAnchorPosition.dpBi, bd ) >= 0 )
	{
	int		FieldNr= -1;
	int		partBegin= -1;
	int		partEnd= -1;
	int		stroffBegin= -1;
	int		stroffEnd= -1;

	if  ( ! docDelimitParaHeadField( &FieldNr, &partBegin, &partEnd,
					&stroffBegin, &stroffEnd,
					dc->dcAnchorPosition.dpBi, bd ) )
	    {
	    if  ( dc->dcAnchorPosition.dpStroff < stroffEnd )
		{ dc->dcAnchorPosition.dpStroff=  stroffEnd;	}
	    }
	}

    if  ( wordIsObject )
	{
	int			partObject;
	DocumentPosition	dpObject;
	InsertedObject *	io;

	if  ( ! docGetObjectSelection( &dsWord, &partObject, &dpObject, &io ) )
	    {
	    DocumentRectangle	drObject;
	    PositionGeometry	pgObject;
	    const int		lastOne= 1;
	    int			marg= 0;
	    int			wide;

	    tedPositionGeometry( &pgObject, &dpObject, lastOne, bd, add, sfl );

	    tedGetObjectRectangle( &drObject, (APP_POINT *)0,
							io, &pgObject, td );

	    wide= drObject.drX1- drObject.drX0+ 1;
	    if  ( wide > 10 )
		{ marg++;	}
	    if  ( wide > 20 )
		{ marg++;	}

	    if  ( mouseX >= drObject.drX0+ marg	&&
		  mouseX <= drObject.drX1- marg	)
		{
		tedSetSelection( ed, &dsWord, pgObject.pgAtLineHead,
						    &scrolledX, &scrolledY );
		goto ready;
		}
	    }
	}

    if  ( ! docIsIBarSelection( &dsOld )				||
	  ! docSamePosition( &(dsOld.dsBegin),
					&(dc->dcAnchorPosition) )	)
	{
	tedSetSelectedPosition( ed, &(dc->dcAnchorPosition),
						    pgClick.pgAtLineHead,
						    &scrolledX, &scrolledY );
	}

  ready:
    tedAdaptToolsToSelection( ed );

    dc->dcEd= ed;
    dc->dcMouseX= mouseX;
    dc->dcMouseY= mouseY;

    *pKeyState= keyState;
    return 0;
    }

#   define	TED_DRAG_INTERVAL	(150L)

static APP_EVENT_HANDLER_H( tedInputDragMouseMove, w, vdc, event )
    {
    DraggingContext *	dc= (DraggingContext *)vdc;
    EditDocument *	ed= dc->dcEd;

    int			ox= ed->edVisibleRect.drX0;
    int			oy= ed->edVisibleRect.drY0;

    int			mouseX;
    int			mouseY;

    if  ( appGetCoordinatesFromMouseMoveEvent( &mouseX, &mouseY, w, event ) )
	{ return;	}

    if  ( mouseX < -ox )
	{ mouseX=  -ox;	}
    if  ( mouseY < -oy )
	{ mouseY=  -oy;	}

    dc->dcMouseX= mouseX;
    dc->dcMouseY= mouseY;

    if  ( tedExtendSelectionToXY( dc->dcEd, dc->dcRootItem,
			&(dc->dcAnchorPosition), dc->dcMouseX, dc->dcMouseY ) )
	{ LDEB(1); return;	}

    return;
    }

static APP_EVENT_HANDLER_H( tedInputDragMouseUp, w, vdc, event )
    {
    DraggingContext *		dc= (DraggingContext *)vdc;
    EditDocument *		ed= dc->dcEd;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    docDescribeSelection( &(td->tdSelectionDescription),
				&(td->tdDocumentSelection),
				bd, ed->edDocumentId, ed->edIsReadonly );

    tedAdaptToolsToSelection( ed );

    return;
    }

static APP_TIMER_HANDLER( tedTick, voiddc )
    {
    DraggingContext *		dc= (DraggingContext *)voiddc;
    EditDocument *		ed= dc->dcEd;
    AppDrawingData *		add= &(ed->edDrawingData);

    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    int				mouseX;
    int				mouseY;

    int				scrolledX= 0;
    int				scrolledY= 0;

#   ifdef USE_MOTIF
    {
    Window		root;
    Window		child;
    int			rootX;
    int			rootY;
    int			winX;
    int			winY;
    unsigned int	mask;

    XQueryPointer( add->addDisplay, add->addDrawable,
						    &root, &child,
						    &rootX, &rootY,
						    &winX, &winY, &mask );
    mouseX= winX; mouseY= winY;
    }
#   endif

#   ifdef USE_GTK
    {
    gint		winX;
    gint		winY;
    GdkModifierType	mask;

    gdk_window_get_pointer( add->addDrawable, &winX, &winY, &mask );

    mouseX= winX; mouseY= winY;
    }
#   endif

    if  ( mouseX < -ox )
	{ mouseX=  -ox;	}
    if  ( mouseY < -oy )
	{ mouseY=  -oy;	}
    if  ( mouseX > add->addBackRect.drX1- ox )
	{ mouseX=  add->addBackRect.drX1- ox;	}
    if  ( mouseY > add->addBackRect.drY1- oy )
	{ mouseY=  add->addBackRect.drY1- oy;	}

    appScrollToRectangle( ed, mouseX+ ox, mouseY+ oy,
					mouseX+ ox, mouseY+ oy,
					&scrolledX, &scrolledY );

    if  ( scrolledX || scrolledY )
	{
	if  ( tedExtendSelectionToXY( ed, dc->dcRootItem,
			&(dc->dcAnchorPosition), dc->dcMouseX, dc->dcMouseY ) )
	    { LDEB(1);	}
	}

#   ifdef USE_MOTIF
    return;
#   endif
#   ifdef USE_GTK
    return 0;
#   endif
    }

static void tedButton1Pressed(	APP_WIDGET			w,
				EditDocument *			ed,
				APP_EVENT *			downEvent )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DraggingContext		dc;
    unsigned int		keyState;

    if  ( tedSelectMousePosition( &keyState, w, ed, &dc, downEvent ) )
	{ return;	}

    if  ( ed->edFileReadOnly || ( keyState & KEY_CONTROL_MASK ) )
	{
	DocumentSelection	dsHyperlink;
	int			startPart;
	int			endPart;

	const char *		fileName;
	int			fileSize;
	const char *		markName;
	int			markSize;

	if  ( ! docGetHyperlinkForPosition( bd, &(dc.dcAnchorPosition),
				&dsHyperlink, &startPart, &endPart,
				&fileName, &fileSize, &markName, &markSize ) )
	    {
	    tedDocFollowLink( (APP_WIDGET)0, ed,
				    fileName, fileSize, markName, markSize );

	    return;
	    }
	}

    appRunDragLoop( w, ed->edApplication, downEvent,
				tedInputDragMouseUp,
				tedInputDragMouseMove,
				TED_DRAG_INTERVAL, tedTick,
				(void *)&dc );

    return;
    }

/************************************************************************/
/*									*/
/*  Handle keyboard input.						*/
/*									*/
/*  a)  Handle miscelaneous keysyms as keysyms, even if they have a	*/
/*	string representation.						*/
/*									*/
/************************************************************************/

static void tedInputSetSelectedPosition(
				EditDocument *			ed,
				const DocumentPosition *	dp,
				int				lastLine )
    {
    int			scrolledX= 0;
    int			scrolledY= 0;

    tedSetSelectedPosition( ed, dp, lastLine, &scrolledX, &scrolledY );

    tedAdaptToolsToSelection( ed );

    return;
    }

static void tedInputExtendSelection(	EditDocument *			ed,
					const DocumentPosition *	dpSet )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return;	}

    tedExtendSelectionToPosition( ed, &(ds.dsAnchor), dpSet );

    tedAdaptToolsToSelection( ed );

    return;
    }

static void tedInputChangeSelection(
				EditDocument *			ed,
				unsigned int			keyState,
				const DocumentPosition *	dp,
				int				lastLine )
    {
    if  ( keyState & KEY_SHIFT_MASK )
	{
	tedInputExtendSelection( ed, dp );
	return;
	}

    tedInputSetSelectedPosition( ed, dp, lastLine );

    return;
    }

static void tedInputAvoidHeadField(	DocumentPosition *	dpNew,
					const BufferDocument *	bd )
    {
    int		fieldNr= -1;
    int		partBegin= -1;
    int		partEnd= -1;
    int		stroffBegin= -1;
    int		stroffEnd= -1;

    int	fieldKind= docParaHeadFieldKind( dpNew->dpBi, bd );
    if  ( fieldKind < 0 )
	{ return;	}

    if  ( docDelimitParaHeadField( &fieldNr, &partBegin, &partEnd,
				&stroffBegin, &stroffEnd, dpNew->dpBi, bd ) )
	{ LDEB(1);	}
    else{
	if  ( dpNew->dpStroff < stroffEnd )
	    { dpNew->dpStroff=  stroffEnd;	}
	}

    return;
    }

static int tedMoveRightOnKey(	DocumentPosition *	dpNew,
				const BufferDocument *	bd,
				int			keyState,
				const AppDrawingData *	add )
    {
    if  ( keyState & KEY_CONTROL_MASK )
	{
	if  ( docNextWord( dpNew ) )
	    { return -1;	}
	}
    else{
	if  ( docNextPosition( dpNew ) )
	    { return -1;	}
	}

    if  ( ! ( keyState & KEY_SHIFT_MASK ) )
	{ tedInputAvoidHeadField( dpNew, bd );	}

    return 0;
    }

static int tedMoveLeftOnKey(	DocumentPosition *	dpNew,
				const BufferDocument *	bd,
				int			keyState,
				const AppDrawingData *	add )
    {
    if  ( keyState & KEY_CONTROL_MASK )
	{
	if  ( docPrevWord( dpNew ) )
	    { return -1;	}
	}
    else{
	if  ( docPrevPosition( dpNew ) )
	    { return -1;	}
	}

    if  ( ! ( keyState & KEY_SHIFT_MASK )	&&
	  dpNew->dpBi->biParaListOverride > 0	)
	{
	int		fieldNr= -1;
	int		partBegin= -1;
	int		partEnd= -1;
	int		stroffBegin= -1;
	int		stroffEnd= -1;

	if  ( docDelimitParaHeadField( &fieldNr, &partBegin, &partEnd,
					&stroffBegin, &stroffEnd,
					dpNew->dpBi, bd ) )
	    { LDEB(1);	}
	else{
	    if  ( dpNew->dpStroff < stroffEnd )
		{
		dpNew->dpStroff= 0;

		if  ( docPrevPosition( dpNew ) )
		    { return -1;	}
		}
	    }
	}

    return 0;
    }

static void tedProcessKeyEvent(	EditDocument *		ed,
				APP_EVENT *		event,
				APP_KEY_VALUE		keySym,
				unsigned char		scratch[40],
				int			gotString,
				int			gotKey,
				unsigned int		state )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    const ScreenFontList *	sfl= &(td->tdScreenFontList);
    const AppDrawingData *	add= &(ed->edDrawingData);

    int				res;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return;	}

    if  ( gotString > 0 )
	{
	DrawScreenFont *	apf;

	if  ( ! td->tdCanReplaceSelection )
	    { return;	}

	if  ( td->tdCurrentScreenFont < 0				||
	      td->tdCurrentScreenFont >=
				add->addScreenFontList.apflFontCount	)
	    {
	    LDEB(td->tdCurrentScreenFont);
	    LDEB(add->addScreenFontList.apflFontCount);
	    return;
	    }

	apf= add->addScreenFontList.apflFonts+ td->tdCurrentScreenFont;

	if  ( appCharExistsInFont( apf->apfFontStruct, scratch[0] ) )
	    {
	    tedDocReplaceSelection( ed, scratch, gotString );
	    return;
	    }
	}

    if  ( gotKey == 0 )
	{ return;	}

    switch( keySym )
	{
	DocumentPosition		dpNew;
	const PositionGeometry *	pgRef;

#	ifdef KEY_ISO_Left_Tab
	case  KEY_ISO_Left_Tab:
	    if  ( ds.dsBegin.dpBi			&&
		  ds.dsBegin.dpBi->biParaInTable	&&
		  ! ( state & KEY_CONTROL_MASK )	)
		{ goto shiftTab;	}
	    else{ return;		}
#	endif

	case KEY_i:
	    if  ( state != KEY_CONTROL_MASK )
		{ goto unknown; }
	    /*FALLTHROUGH*/
	case KEY_Tab:
	    if  ( ds.dsBegin.dpBi			&&
		  ds.dsBegin.dpBi->biParaInTable	&&
		  ! ( state & KEY_CONTROL_MASK )	)
		{
		if  ( state & KEY_SHIFT_MASK )
		    {
		  shiftTab:
		    if  ( docFirstPosition( &dpNew,
					ds.dsBegin.dpBi->biParent )	||
			  docPrevPosition( &dpNew )			)
			{ return;	}
		    }
		else{
		    if  ( docLastPosition( &dpNew,
					ds.dsBegin.dpBi->biParent )	||
			  docNextPosition( &dpNew )			)
			{ return;	}
		    }

		{
		    const int	lastLine= 0;

		    tedInputSetSelectedPosition( ed, &dpNew, lastLine );
		}

		return;
		}

	    if  ( state & KEY_SHIFT_MASK )
		{ return;	}

	    if  ( ! td->tdCanReplaceSelection )
		{ return;	}

	    tedEditReplaceSelectionWithTab( ed );

	    return;

	case KEY_j: case KEY_m:
	    if  ( state != KEY_CONTROL_MASK )
		{ goto unknown; }
	    /*FALLTHROUGH*/
	case KEY_KP_Enter:
	case KEY_Return:
	    if  ( ! td->tdCanReplaceSelection )
		{ return;	}

	    tedSplitParagraph( ed,
			    state == KEY_CONTROL_MASK	&&
			    keySym != KEY_j		&&
			    keySym != KEY_m		);

	    appDocumentChanged( ed, 1 );

	    return;

	case KEY_KP_Delete:
	case KEY_Delete:

	    res= tedDeleteTableSliceSelection( ed );
	    if  ( res < 0 )
		{ LDEB(res); return;	}
	    if  ( res == 0 )
		{ return;		}

	    if  ( ! td->tdCanReplaceSelection )
		{ return;	}

	    if  ( sd.sdIsIBarSelection )
		{
		dpNew= ds.dsEnd;

		if  ( tedMoveRightOnKey( &dpNew, bd, state, add ) )
		    { return;	}

		if  ( ( ds.dsBegin.dpBi->biParaInTable	||
		        dpNew.dpBi->biParaInTable	)		&&
		      ! docPositionsInsideCell( &(ds.dsBegin), &dpNew ) )
		    {
		    const int		keyState= 0;
		    const int		lastLine= 1;

		    tedInputChangeSelection( ed, keyState, &dpNew, lastLine );
		    return;
		    }

		if  ( tedMoveRightOnKey( &(ds.dsEnd), bd, state, add ) )
		    { return;	}

		    {
		    int		scrolledX= 0;
		    int		scrolledY= 0;

		    tedSetSelection( ed, &ds, sg.sgBegin.pgAtLineHead,
						    &scrolledX, &scrolledY );
		    }
		}

	    tedDocReplaceSelection( ed, (const unsigned char *)0, 0 );

	    return;

	case KEY_BackSpace:

	    res= tedDeleteTableSliceSelection( ed );
	    if  ( res < 0 )
		{ LDEB(res); return;	}
	    if  ( res == 0 )
		{ return;		}

	    if  ( ! td->tdCanReplaceSelection )
		{ return;	}

	    if  ( sd.sdIsIBarSelection )
		{
		dpNew= ds.dsBegin;

		if  ( tedMoveLeftOnKey( &dpNew, bd, state, add ) )
		    { return;	}

		if  ( ( dpNew.dpBi->biParaInTable	||
		        ds.dsEnd.dpBi->biParaInTable	)		&&
		      ! docPositionsInsideCell( &dpNew, &(ds.dsEnd) )	)
		    {
		    const int		keyState= 0;
		    const int		lastLine= 1;

		    tedInputChangeSelection( ed, keyState, &dpNew, lastLine );
		    return;
		    }

		if  ( tedMoveLeftOnKey( &(ds.dsBegin), bd, state, add ) )
		    { return;	}

		    {
		    int		scrolledX= 0;
		    int		scrolledY= 0;

		    tedSetSelection( ed, &ds, sg.sgBegin.pgAtLineHead,
						    &scrolledX, &scrolledY );
		    }
		}

	    tedDocReplaceSelection( ed, (const unsigned char *)0, 0 );
	    return;

	case KEY_KP_Home:
	case KEY_Home:
	    if  ( ( state & KEY_CONTROL_MASK ) )
		{
		if  ( docFirstPosition( &dpNew, &(bd->bdItem) ) )
		    { return;	}
		}
	    else{
		if  ( ( state & KEY_SHIFT_MASK ) && ds.dsDirection >= 0 )
		    {
		    dpNew= ds.dsEnd;

		    if  ( docBeginOfLine( &dpNew, sg.sgEnd.pgAtLineHead ) )
			{ return;	}
		    }
		else{
		    dpNew= ds.dsBegin;

		    if  ( docBeginOfLine( &dpNew, sg.sgBegin.pgAtLineHead ) )
			{ return;	}
		    }
		}

	    tedInputAvoidHeadField( &dpNew, bd );

	    {
	    const int	lastLine= 1;

	    tedInputChangeSelection( ed, state, &dpNew, lastLine );
	    }
	    return;

	case KEY_KP_End:
	case KEY_End:
	    if  ( ( state & KEY_CONTROL_MASK ) )
		{
		if  ( docLastPosition( &dpNew, &(bd->bdItem) ) )
		    { return;	}
		}
	    else{
		if  ( ! ( state & KEY_SHIFT_MASK ) || ds.dsDirection >= 0 )
		    {
		    dpNew= ds.dsEnd;

		    if  ( docEndOfLine( &dpNew, sg.sgEnd.pgAtLineHead ) )
			{ return;	}
		    }
		else{
		    dpNew= ds.dsBegin;

		    if  ( docEndOfLine( &dpNew, sg.sgBegin.pgAtLineHead ) )
			{ return;	}
		    }
		}

	    {
	    const int	lastLine= 0;

	    tedInputChangeSelection( ed, state, &dpNew, lastLine );
	    }
	    return;

	case KEY_KP_Left:
	case KEY_Left:
	    if  ( ! state			&&
		  sd.sdIsIBarSelection		&&
		  ds.dsBegin.dpStroff > 0	&&
		  sg.sgBegin.pgAtLineHead	&&
		  ! sg.sgBegin.pgAtLineEnd	)
		{
		const int	lastLine= 0;

		tedInputChangeSelection( ed, state, &(ds.dsBegin), lastLine );
		return;
		}

	    if  ( ( state & KEY_SHIFT_MASK ) && ds.dsDirection >= 0 )
		{ dpNew= ds.dsEnd;	}
	    else{ dpNew= ds.dsBegin;	}

	    if  ( state != 0 || sd.sdIsIBarSelection )
		{
		if  ( tedMoveLeftOnKey( &dpNew, bd, state, add ) )
		    { return;	}
		}

	    {
	    const int	lastLine= 1;

	    tedInputChangeSelection( ed, state, &dpNew, lastLine );
	    }
	    return;

	case KEY_KP_Right:
	case KEY_Right:
	    if  ( ! state						&&
		  sd.sdIsIBarSelection					&&
		  ds.dsBegin.dpStroff < ds.dsBegin.dpBi->biParaStrlen	&&
		  sg.sgBegin.pgAtLineEnd				&&
		  ! sg.sgBegin.pgAtLineHead				)
		{
		const int	lastLine= 1;

		tedInputChangeSelection( ed, state, &ds.dsBegin, lastLine );
		return;
		}

	    if  ( ! ( state & KEY_SHIFT_MASK ) || ds.dsDirection >= 0 )
		{ dpNew= ds.dsEnd;	}
	    else{ dpNew= ds.dsBegin;	}

	    if  ( state != 0 || sd.sdIsIBarSelection )
		{
		if  ( tedMoveRightOnKey( &dpNew, bd, state, add ) )
		    { return;	}
		}

	    {
	    const int	lastLine= 0;

	    tedInputChangeSelection( ed, state, &dpNew, lastLine );
	    }
	    return;

	case KEY_KP_Up:
	case KEY_Up:
	    if  ( ! ( state & KEY_SHIFT_MASK ) || ds.dsDirection >= 0 )
		{ pgRef= &(sg.sgEnd); dpNew= ds.dsEnd;		}
	    else{ pgRef= &(sg.sgBegin); dpNew= ds.dsBegin;	}

	    if  ( state & KEY_CONTROL_MASK )
		{
		if  ( dpNew.dpStroff == 0 )
		    {
		    dpNew.dpBi= docPrevParagraph( dpNew.dpBi );
		    if  ( ! dpNew.dpBi )
			{ return;	}

		    if  ( docParaBegin( &dpNew ) )
			{ return;	}
		    }
		else{
		    if  ( docParaBegin( &dpNew ) )
			{ return;	}
		    }
		}
	    else{
		if  ( tedArrowUp( &dpNew, pgRef, bd, add, sfl ) )
		    { return;	}
		}

	    {
	    const int	lastLine= sg.sgBegin.pgAtLineHead;

	    tedInputChangeSelection( ed, state, &dpNew, lastLine );
	    }
	    return;

	case KEY_KP_Down:
	case KEY_Down:
	    if  ( ! ( state & KEY_SHIFT_MASK ) || ds.dsDirection >= 0 )
		{ pgRef= &(sg.sgEnd); dpNew= ds.dsEnd;		}
	    else{ pgRef= &(sg.sgBegin); dpNew= ds.dsBegin;	}

	    if  ( state & KEY_CONTROL_MASK )
		{
		if  ( dpNew.dpStroff < dpNew.dpBi->biParaStrlen		&&
		      ( state & KEY_SHIFT_MASK )			)
		    {
		    if  ( docParaEnd( &dpNew ) )
			{ return;	}
		    }
		else{
		    dpNew.dpBi= docNextParagraph( dpNew.dpBi );
		    if  ( ! dpNew.dpBi )
			{ return;	}

		    if  ( docParaBegin( &dpNew ) )
			{ return;	}
		    }
		}
	    else{
		if  ( tedArrowDown( &dpNew, pgRef, bd, add, sfl ) )
		    { return;	}
		}

	    {
	    const int	lastLine= sg.sgBegin.pgAtLineHead;

	    tedInputChangeSelection( ed, state, &dpNew, lastLine );
	    }
	    return;

	case KEY_KP_Prior:
	case KEY_Prior:
	    if  ( ( state & KEY_SHIFT_MASK ) && ds.dsDirection >= 0 )
		{ dpNew= ds.dsEnd;	}
	    else{ dpNew= ds.dsBegin;	}

	    if  ( dpNew.dpBi->biInExternalItem != DOCinBODY )
		{
		int		page= -1;

		if  ( ! docPrevSimilarRoot( &dpNew, &page, bd ) )
		    {
		    const int		lastLine= 0;
		    ExternalItem *	ei= (ExternalItem *)0;
		    BufferItem *	bodySectBi;

		    if  ( docGetExternalItem( &ei, &bodySectBi,
							bd, dpNew.dpBi ) )
			{ LDEB(1); return;	}

		    if  ( ei && ei->eiPageSelectedUpon < 0 && page >= 0 )
			{ ei->eiPageSelectedUpon= page;	}

		    tedInputAvoidHeadField( &dpNew, bd );

		    tedInputSetSelectedPosition( ed, &dpNew, lastLine );
		    }

		return;
		}

	    if  ( docPrevPosition( &dpNew )			&&
		  docFirstDocumentPosition( &dpNew, bd )	)
		{ return;	}

	    {
		PositionGeometry	pg;
		const int		lastLine= 1;
		int			partNew;
		int			lineNew;

		int			page;

		tedPositionGeometry( &pg, &dpNew, 0, bd, add, sfl );

		page= pg.pgTopPosition.lpPage;
		while( page >= 0 )
		    {
		    int		res;

		    res= docGetTopOfColumn( &dpNew, &lineNew, &partNew,
								bd, page, 0 );
		    if  ( res < 0 )
			{ LDEB(res); return;	}
		    if  ( res == 0 )
			{
			tedInputChangeSelection( ed, state, &dpNew, lastLine );
			return;
			}

		    page--;
		    }
	    }
	    return;

	case KEY_KP_Next:
	case KEY_Next:
	    if  ( ! ( state & KEY_SHIFT_MASK ) || ds.dsDirection >= 0 )
		{ dpNew= ds.dsEnd;	}
	    else{ dpNew= ds.dsBegin;	}

	    if  ( dpNew.dpBi->biInExternalItem != DOCinBODY )
		{
		int		page= -1;

		if  ( ! docNextSimilarRoot( &dpNew, &page, bd ) )
		    {
		    const int		lastLine= 0;
		    ExternalItem *	ei= (ExternalItem *)0;
		    BufferItem *	bodySectBi;

		    if  ( docGetExternalItem( &ei, &bodySectBi,
							bd, dpNew.dpBi ) )
			{ LDEB(1); return;	}

		    if  ( ei && ei->eiPageSelectedUpon < 0 && page >= 0 )
			{ ei->eiPageSelectedUpon= page;	}

		    tedInputAvoidHeadField( &dpNew, bd );

		    tedInputSetSelectedPosition( ed, &dpNew, lastLine );
		    }

		return;
		}

	    {
		PositionGeometry	pg;
		const int		lastLine= 1;
		int			partNew;
		int			lineNew;

		int			page;

		tedPositionGeometry( &pg, &dpNew, lastLine, bd, add, sfl );

		page= pg.pgTopPosition.lpPage+ 1;
		while( page < bd->bdItem.biBelowPosition.lpPage )
		    {
		    int		res;

		    res= docGetTopOfColumn( &dpNew, &lineNew, &partNew,
								bd, page, 0 );
		    if  ( res < 0 )
			{ LDEB(res); return;	}
		    if  ( res == 0 )
			{
			tedInputChangeSelection( ed, state, &dpNew, lastLine );
			return;
			}

		    page++;
		    }
	    }
	    return;

#	if 0
	case KEY_c:
	    /************************************************************/
	    /*  Usually this key is intercepted by the 'Edit' menu.	*/
	    /************************************************************/
	    if  ( state != KEY_CONTROL_MASK )
		{ goto defaultCase;	}

	    tedDocCopy( ed, (XEvent *)keyEvent );
	    return;
	case KEY_x:
	    /************************************************************/
	    /*  Usually this key is intercepted by the 'Edit' menu.	*/
	    /************************************************************/
	    if  ( state != KEY_CONTROL_MASK )
		{ goto defaultCase;	}

	    tedDocCut( ed, (XEvent *)keyEvent );
	    return;
	case KEY_v:
	    /************************************************************/
	    /*  Usually this key is intercepted by the 'Edit' menu.	*/
	    /************************************************************/
	    if  ( state != KEY_CONTROL_MASK )
		{ goto defaultCase;	}

	    appDocAskForPaste( ed, "PRIMARY", keyEvent->time );
	    return;
#	endif

#	if 0
	case KEY_y:
	    docListItem(0,ds.dsBegin.dpBi);
	    break;
#	endif

	case KEY_Shift_L:
	case KEY_Shift_R:
	case KEY_Alt_L:
	case KEY_Alt_R:
	case KEY_Control_L:
	case KEY_Control_R:
	case KEY_Caps_Lock:
	case KEY_Insert:
	case KEY_KP_Insert:
	case KEY_Num_Lock:
#	ifdef XK_ISO_Level3_Shift
	case XK_ISO_Level3_Shift:
#	endif

	    return;

	default: unknown:
#	    ifdef USE_GTK
#		if GTK_MAJOR_VERSION < 2
		gtk_accel_group_activate( ed->edToplevel.atAccelGroup,
							    keySym, state );
#		else
		gtk_accel_groups_activate(
			    G_OBJECT( ed->edToplevel.atAccelGroup ),
							    keySym, state );
#		endif
#	    endif

#	    ifdef APP_DEBUG
#	    ifdef USE_MOTIF
	    appDebug( "INPUT \"%s\": %s (%s%s%s%s%s%s%s%s<Key>%s) \"%.*s\"\n",
			ed->edTitle, APP_X11EventNames[event->type],
			(state&KEY_SHIFT_MASK)?"Shift":"",
			(state&KEY_CONTROL_MASK)?"Ctrl":"",
			(state&LockMask)?"Lock":"",
			(state&Mod1Mask)?"Mod1":"",
			(state&Mod2Mask)?"Mod2":"",
			(state&Mod3Mask)?"Mod3":"",
			(state&Mod4Mask)?"Mod4":"",
			(state&Mod5Mask)?"Mod5":"",
			XKeysymToString( keySym ), gotString, scratch );
#	    endif
#	    endif
	}

    return;
    }

APP_EVENT_HANDLER_H( tedKeyPressed, w, voided, keyEvent )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    APP_KEY_VALUE		keySym;
    unsigned char		scratch[40];
    int				gotString;
    int				gotKey;
    unsigned int		state;

    if  ( ! tedHasSelection( td ) )
	{
#	ifdef USE_MOTIF
	const AppDrawingData *	add= &(ed->edDrawingData);

	XBell( add->addDisplay, 0 );
#	endif

#	ifdef USE_GTK
	gdk_beep();
#	endif

	return;
	}

    appGuiGetStringFromKeyboardEvent( ed->edInputContext, w, keyEvent,
				&gotString, &gotKey, &state,
				scratch, sizeof(scratch), &keySym );

    tedStopCursorBlink( ed );

    if  ( gotString > 0 || gotKey > 0 )
	{
	tedProcessKeyEvent( ed, keyEvent,
				keySym, scratch, gotString, gotKey, state );
	}

    if  ( tedHasIBarSelection( td ) )
	{ tedStartCursorBlink( ed );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Handle mouse button down events for the document widget.		*/
/*									*/
/************************************************************************/

static void tedButton3Pressed(	APP_WIDGET			w,
				EditDocument *			ed,
				APP_EVENT *			downEvent )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    const ScreenFontList *	sfl= &(td->tdScreenFontList);
    const AppDrawingData *	add= &(ed->edDrawingData);

    int				ox= ed->edVisibleRect.drX0;
    int				oy= ed->edVisibleRect.drY0;

    int				mouseX;
    int				mouseY;
    int				docX;
    int				docY;

    int				otherRoot= 0;

    int				button;
    int				upDown;
    int				seq;
    unsigned int		keyState= 0;

    DraggingContext		dc;

    int				selectMousePosition= 0;

    DocumentSelection		dsOld;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    int				page;
    int				sectNr;
    ExternalItem *		eiSet= (ExternalItem *)0;
    BufferItem *		bodySectBiSet;
    BufferItem *		rootBiSet;

    if  ( tedGetSelection( &dsOld, &sg, &sd, td ) )
	{ LDEB(1); return;	}

    if  ( appGetCoordinatesFromMouseButtonEvent( &mouseX, &mouseY,
					    &button, &upDown, &seq, &keyState,
					    w, downEvent ) )
	{ LDEB(1); return;	}

    if  ( mouseX < -ox )
	{ mouseX=  -ox;	}
    if  ( mouseY < -oy )
	{ mouseY=  -oy;	}

    docX= mouseX+ ox;
    docY= mouseY+ oy;

    if  ( tedFindRootForPosition( &eiSet, &rootBiSet, &bodySectBiSet,
					    &sectNr, &page, ed, docX, docY ) )
	{ LLDEB(docX,docY); return;	}

    otherRoot= ! docSelectionSameRoot( &dsOld, rootBiSet );

    if  ( otherRoot )
	{ selectMousePosition= 1;	}
    else{
	DocumentPosition	dpClick;
	PositionGeometry	pgClick;
	int			partClick;

	int			cmp1;
	int			cmp2;

	if  ( tedFindPosition( &dpClick, &pgClick, &partClick,
					bd, rootBiSet, add, sfl, docX, docY ) )
	    { LLDEB(docX,docY); return;	}

	cmp1= docComparePositions( &dpClick, &(dsOld.dsBegin) );
	cmp2= docComparePositions( &dpClick, &(dsOld.dsEnd) );

	if  ( cmp1 < 0 || cmp2 > 0 )
	    { selectMousePosition= 1;	}
	}

    if  ( selectMousePosition )
	{
	if  ( tedSelectMousePosition( &keyState, w, ed, &dc, downEvent ) )
	    { return;	}
	}

    tedShowFormatTool( td->tdToolsFormatToolOption, ed->edApplication );

    tedAdaptFormatToolToDocument( ed, 1 );

    return;
    }

APP_EVENT_HANDLER_H( tedMouseButtonPressed, w, voided, downEvent )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    tedStopCursorBlink( ed );

#   ifdef USE_MOTIF
    switch( downEvent->xbutton.button )
	{
	case Button1:
	    if  ( ! ed->edFileReadOnly		&&
		  downEvent->xbutton.subwindow	)
		{
		tedObjectDrag( w, ed, downEvent );
		return;
		}

	    tedButton1Pressed( w, ed, downEvent );
	    break;
	case Button2:
	    appDocAskForPaste( ed, "PRIMARY" );
	    break;
	case Button3:
	    tedButton3Pressed( w, ed, downEvent );
	    break;

	case Button4:
	    appMouseWheelUp( ed );
	    break;

	case Button5:
	    appMouseWheelDown( ed );
	    break;

	default:
	    LDEB(downEvent->xbutton.button);
	    break;
	}
#   endif

#   ifdef USE_GTK
    switch( downEvent->button.button )
	{
	int	res;

	case 1:
	    if  ( ! ed->edFileReadOnly )
		{
		res= tedObjectDrag( w, ed, downEvent );
		if  ( res < 1 )
		    { return;	}
		}

	    tedButton1Pressed( w, ed, downEvent );
	    break;
	case 2:
	    appDocAskForPaste( ed, "PRIMARY" );
	    break;
	case 3:
	    tedButton3Pressed( w, ed, downEvent );
	    break;

	case 4:
	    appMouseWheelUp( ed );
	    break;

	case 5:
	    appMouseWheelDown( ed );
	    break;

	default:
	    LDEB(downEvent->button.button);
	    break;
	}
#   endif

    if  ( tedHasIBarSelection( td ) )
	{ tedStartCursorBlink( ed );	}

    return;
    }

