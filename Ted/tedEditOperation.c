/************************************************************************/
/*									*/
/*  Editor functionality.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedLayout.h"
#   include	"tedEdit.h"

#   include	<appDebugon.h>

#   define	VALIDATE_TREE		0

/************************************************************************/
/*									*/
/*  Do the last steps that set an I-Bar selection after an editing	*/
/*  action.								*/
/*									*/
/*  1)  The order in which the size of the window is adapted and the	*/
/*	window is scrolled to the selection depends on the direction	*/
/*	of the change, as the scrollbars complain when the corrent	*/
/*	position is not in range.					*/
/*  2)  Do not place the IBar in a bullet or note number.		*/
/*									*/
/************************************************************************/

static int tedEditFinishIBarSelection2(
			    EditDocument *		ed,
			    EditOperation *		eo,
			    BufferItem *		paraBi,
			    int				stroff,
			    int				lastLine )
    {
    AppDrawingData *		add= &(ed->edDrawingData);

    int				scrolledX= 0;
    int				scrolledY= 0;

    /*  1  */
    if  ( add->addBackRect.drY1 > eo->eoOldBackY1 )
	{
	appDocSetScrollbarValues( ed );
	appSetShellConstraints( ed );
	}

    if  ( paraBi )
	{
	/*  2  */
	if  ( docParaHeadFieldKind( paraBi, eo->eoBd ) >= 0 )
	    {
	    int		fieldNr= -1;
	    int		partBegin= -1;
	    int		partEnd= -1;
	    int		stroffBegin= -1;
	    int		stroffEnd= -1;

	    if  ( ! docDelimitParaHeadField( &fieldNr, &partBegin, &partEnd,
						&stroffBegin, &stroffEnd,
						paraBi, eo->eoBd ) )
		{
		if  ( stroff < stroffEnd )
		    { stroff=  stroffEnd;	}
		}
	    }

	tedSetIBarSelection( ed, paraBi, stroff, lastLine,
						    &scrolledX, &scrolledY );
	}
    else{ XDEB(paraBi);	}


    /*  1  */
    if  ( add->addBackRect.drY1 < eo->eoOldBackY1 )
	{
	appDocSetScrollbarValues( ed );
	appSetShellConstraints( ed );
	}

    appDocExposeRectangle( ed, &(eo->eoChangedRect), scrolledX, scrolledY );

    tedStartCursorBlink( ed );

    return 0;
    }

int tedEditFinishIBarSelection(	EditDocument *			ed,
				EditOperation *			eo,
				BufferItem *			bi,
				int				stroff )
    {
    const int		lastLine= 0;

    if  ( tedEditRefreshLayout( eo, ed ) )
	{ LDEB(1); /* return -1; */	}

    return tedEditFinishIBarSelection2( ed, eo, bi, stroff, lastLine );
    }

/************************************************************************/
/*									*/
/*  Adapt to a formatting change.					*/
/*									*/
/************************************************************************/

static int tedEditFinishSelection2(	EditDocument *			ed,
					EditOperation *			eo,
					const DocumentSelection *	dsNew )
    {
    AppDrawingData *		add= &(ed->edDrawingData);

    int				scrolledX= 0;
    int				scrolledY= 0;

    int				lastLine= 0;

    if  ( docIsIBarSelection( dsNew ) )
	{
	const int	lastLine= 0;

	return tedEditFinishIBarSelection2( ed, eo,
		    dsNew->dsBegin.dpBi, dsNew->dsBegin.dpStroff, lastLine );
	}

    if  ( add->addBackRect.drY1 > eo->eoOldBackY1 )
	{
	appDocSetScrollbarValues( ed );
	appSetShellConstraints( ed );
	}

    tedSetSelection( ed, dsNew, lastLine, &scrolledX, &scrolledY );

    if  ( add->addBackRect.drY1 < eo->eoOldBackY1 )
	{
	appDocSetScrollbarValues( ed );
	appSetShellConstraints( ed );
	}

    appDocExposeRectangle( ed, &(eo->eoChangedRect), scrolledX, scrolledY );

    return 0;
    }


int tedEditFinishSelection(		EditDocument *			ed,
					EditOperation *			eo,
					const DocumentSelection *	dsNew )
    {
    if  ( tedEditRefreshLayout( eo, ed ) )
	{ LDEB(1); return -1;	}

    return tedEditFinishSelection2( ed, eo, dsNew );
    }

int tedEditFinishOldSelection(		EditDocument *			ed,
					EditOperation *			eo )
    {
    int				rval;

    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		dsNew;
    DocumentPosition		dpBegin;
    DocumentPosition		dpEnd;

    ExternalItem *		ei;
    BufferItem *		selRootBi;
    BufferItem *		bodySectBi;
    BufferItem *		bi;

    TableRectangle		tr;

    if  ( tedEditRefreshLayout( eo, ed ) )
	{ LDEB(1); return -1;	}

    if  ( docGetRootOfSelectionScope( &ei, &selRootBi, &bodySectBi,
					eo->eoBd, &(eo->eoSelectionScope) ) )
	{ LDEB(1); return -1;	}

    bi= docGetParagraphByNumber( selRootBi,
		    eo->eoSelectedRange.erStart.epParaNr );
    if  ( ! bi )
	{
	LXDEB(eo->eoSelectedRange.erStart.epParaNr,bi);
	return -1;
	}
    if  ( eo->eoSelectedRange.erStart.epStroff > bi->biParaStrlen )
	{
	LLDEB(eo->eoSelectedRange.erStart.epStroff,bi->biParaStrlen);
	if  ( docLastPosition( &dpBegin, bi ) )
	    { LDEB(1); return -1;	}
	}
    else{
	docSetDocumentPosition( &dpBegin, bi,
			    eo->eoSelectedRange.erStart.epStroff );
	}

    bi= docGetParagraphByNumber( selRootBi,
		    eo->eoSelectedRange.erEnd.epParaNr );
    if  ( ! bi )
	{
	LXDEB(eo->eoSelectedRange.erEnd.epParaNr,bi);
	return -1;
	}
    if  ( eo->eoSelectedRange.erEnd.epStroff > bi->biParaStrlen )
	{
	LLDEB(eo->eoSelectedRange.erEnd.epStroff,bi->biParaStrlen);
	if  ( docLastPosition( &dpEnd, bi ) )
	    { LDEB(1); return -1;	}
	}
    else{
	docSetDocumentPosition( &dpEnd, bi,
			    eo->eoSelectedRange.erEnd.epStroff );
	}

    docInitDocumentSelection( &dsNew );

    docSetRangeSelection( &dsNew, &dpBegin, &dpEnd, 1, -1, -1 );

    if  ( ! docSelectionInsideCell( &dsNew )	&&
	  ! docGetTableRectangle( &tr, &dsNew ) )
	{
	BufferItem *	selSectBi;

	if  ( docTableRectangleSelection( &dsNew, &selSectBi, bd, &tr ) )
	    { LDEB(1);	}
	}

    rval= tedEditFinishSelection2( ed, eo, &dsNew );

    td->tdCurrentTextAttribute= eo->eoSavedTextAttribute;
    td->tdCurrentTextAttributeNumber= eo->eoSavedTextAttributeNumber;

    return rval;
    }

int tedEditFinishEndOfInsert(	EditDocument *			ed,
				EditOperation *			eo,
				int				lastLine)
    {
    DocumentPosition		dpEnd;

    ExternalItem *		ei;
    BufferItem *		selRootBi;
    BufferItem *		bodySectBi;
    BufferItem *		bi;

    if  ( tedEditRefreshLayout( eo, ed ) )
	{ LDEB(1); return -1;	}

    if  ( docGetRootOfSelectionScope( &ei, &selRootBi, &bodySectBi,
					eo->eoBd, &(eo->eoSelectionScope) ) )
	{ LDEB(1); return -1;	}

    bi= docGetParagraphByNumber( selRootBi,
		    eo->eoSelectedRange.erEnd.epParaNr );
    if  ( ! bi )
	{
	LXDEB(eo->eoSelectedRange.erEnd.epParaNr,bi);
	return -1;
	}
    if  ( eo->eoSelectedRange.erEnd.epStroff > bi->biParaStrlen )
	{
	LLDEB(eo->eoSelectedRange.erEnd.epStroff,bi->biParaStrlen);
	if  ( docLastPosition( &dpEnd, bi ) )
	    { LDEB(1); return -1;	}
	}
    else{
	docSetDocumentPosition( &dpEnd, bi,
			    eo->eoSelectedRange.erEnd.epStroff );
	}

    return tedEditFinishIBarSelection2( ed, eo,
				    dpEnd.dpBi, dpEnd.dpStroff, lastLine );
    }

/************************************************************************/
/*									*/
/*  Adjust the place from where redrawing after editing is to begin.	*/
/*									*/
/*  E.G. because when the first particule of the line was split, also	*/
/*	reformat the previous line.					*/
/*									*/
/************************************************************************/

/*  B  */
void tedAdjustRedrawBegin(	EditDocument *			ed,
				EditOperation *			eo,
				int *				pLine )
    {
    AppDrawingData *		add= &(ed->edDrawingData);
    const DocumentRectangle *	drBack= &(add->addBackRect);
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    BufferItem *		paraBi;
    TextLine *			tl;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    int				part;
    const int			lastOne= 0;

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return;	}
    paraBi= ds.dsBegin.dpBi;

    tl= paraBi->biParaLines+ *pLine;

    if  ( *pLine == 0 )
	{
	if  ( ds.dsBegin.dpBi != ds.dsEnd.dpBi )
	    {
	    int		topPixels= TL_TOP_PIXELS( add, tl );

	    eo->eoChangedRect.drX0= drBack->drX0;
	    eo->eoChangedRect.drX1= drBack->drX1;

	    if  ( eo->eoChangedRect.drY0 > topPixels )
		{ eo->eoChangedRect.drY0=  topPixels; }
	    }

	docIncludePositionInReformat( eo, paraBi, tl->tlStroff );

	return;
	}

    if  ( docFindParticuleOfPosition( &part, &(ds.dsBegin), lastOne ) )
	{ LDEB(ds.dsBegin.dpStroff); return;	}

    if  ( part <= tl->tlFirstParticule		||
	  ds.dsBegin.dpBi != ds.dsEnd.dpBi	)
	{
	int		topPixels;

	(*pLine)--; tl--;

	topPixels= TL_TOP_PIXELS( add, tl );

	eo->eoChangedRect.drX0= drBack->drX0;
	eo->eoChangedRect.drX1= drBack->drX1;

	if  ( eo->eoChangedRect.drY0 > topPixels )
	    { eo->eoChangedRect.drY0=  topPixels; }

	docIncludePositionInReformat( eo, paraBi, tl->tlStroff );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  1)  Start an Edit Operation.					*/
/*  2)  Finish an Edit Operation.					*/
/*									*/
/*  The reference positions in the Start and Finish calls should be	*/
/*  the same position in the document from a logical point of view.	*/
/*									*/
/************************************************************************/

/*  1  */
int tedStartEditOperation(	EditOperation *		eo,
				DocumentSelection *	ds,
				SelectionGeometry *	sg,
				SelectionDescription *	sd,
				EditDocument *		ed,
				int			fullWidth )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    AppDrawingData *		add= &(ed->edDrawingData);

    ExternalItem *		ei;
    BufferItem *		selRootBi;
    BufferItem *		bodySectBi;

    const int			lastOne= 1;

#   if VALIDATE_TREE
    {
    const BufferDocument *	bd= td->tdDocument;
    LDEB(1);
    if  ( docCheckItem( &(bd->bdItem) ) )
	{ LDEB(2); docListItem( 0, &(bd->bdItem) ); abort(); }
    }
#   endif

    docInitEditOperation( eo );

    if  ( tedGetSelection( ds, sg, sd, td ) )
	{ LDEB(1); return -1;	}

    eo->eoSelectionScope= ds->dsSelectionScope;

    eo->eoSavedTextAttribute= td->tdCurrentTextAttribute;
    eo->eoSavedTextAttributeNumber= td->tdCurrentTextAttributeNumber;

    if  ( docGetRootOfSelectionScope( &ei, &selRootBi,
				&bodySectBi, bd, &(eo->eoSelectionScope) ) )
	{ LDEB(1); return -1;	}

    docIncludePositionInReformat( eo, ds->dsBegin.dpBi, ds->dsBegin.dpStroff );
    docIncludePositionInReformat( eo, ds->dsEnd.dpBi, ds->dsEnd.dpStroff );

    eo->eoSelectedRange= eo->eoReformatRange;

    eo->eoIBarSelectionOld= sd->sdIsIBarSelection;
    eo->eoMultiParagraphSelectionOld= ! sd->sdIsSingleParagraph;

    docIncludeRectangleInChange( eo, &(sg->sgRectangle) );
    eo->eoChangedRect.drX1= add->addBackRect.drX1;

    if  ( fullWidth )
	{ eo->eoChangedRect.drX0= add->addBackRect.drX0; }

    eo->eoOldBackY1= add->addBackRect.drY1;

    /**/
    eo->eoBd= bd;
    eo->eoScreenFontList= &(td->tdScreenFontList);
    eo->eoVoidadd= (void *)&(ed->edDrawingData);
    eo->eoCloseObject= tedCloseObject;

    /**/
    eo->eoInsParaBi= ds->dsEnd.dpBi;
    eo->eoInsStroff= ds->dsEnd.dpStroff;
    if  ( docFindParticule( &(eo->eoInsParticule),
				eo->eoInsParaBi, eo->eoInsStroff, lastOne ) )
	{ LLDEB(eo->eoInsStroff,eo->eoInsParaBi->biParaStrlen); return -1; }

    if  ( eo->eoInsStroff ==
		eo->eoInsParaBi->biParaParticules[eo->eoInsParticule].tpStroff )
	{ eo->eoInsAtPartHead= 1;	}
    else{ eo->eoInsAtPartHead= 0;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Widen redraw rectangle to whole BufferItems.			*/
/*									*/
/************************************************************************/

int tedEditIncludeItemInRedraw(	EditOperation *		eo,
				EditDocument *		ed,
				const BufferItem *	bi )
    {
    AppDrawingData *		add= &(ed->edDrawingData);
    DocumentRectangle		drLocal;

    drLocal.drX0= add->addBackRect.drX0;
    drLocal.drX1= add->addBackRect.drX1;
    drLocal.drY0= BI_TOP_PIXELS( add, bi )- 1;
    drLocal.drY1= BI_BELOW_PIXELS( add, bi )- 1;

    if  ( bi->biLevel == DOClevROW	&&
	  bi->biRowHasTableParagraphs	)
	{
	drLocal.drY1= LP_YPIXELS( add, &(bi->biRowBelowAllPosition) )- 1;
	}

    docIncludeRectangleInChange( eo, &drLocal );

    return 0;
    }

int tedEditIncludeRowsInRedraw(	EditOperation *		eo,
				EditDocument *		ed,
				const BufferItem *	sectBi,
				int			row0,
				int			row1 )
    {
    AppDrawingData *		add= &(ed->edDrawingData);
    const BufferItem *		rowBi;
    DocumentRectangle		drLocal;

    drLocal.drX0= add->addBackRect.drX0;
    drLocal.drX1= add->addBackRect.drX1;

    rowBi= sectBi->biChildren[row0];
    drLocal.drY0= BI_TOP_PIXELS( add, rowBi )- 1;

    rowBi= sectBi->biChildren[row1];
    drLocal.drY1= LP_YPIXELS( add, &(rowBi->biRowBelowAllPosition) )- 1;

    docIncludeRectangleInChange( eo, &drLocal );

    return 0;
    }

int tedEditRefreshLayout(	EditOperation *		eo,
				EditDocument *		ed )
    {
    EditRange *			er= &(eo->eoReformatRange);

    AppDrawingData *		add= &(ed->edDrawingData);

    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    RecalculateFields		rf;

    ExternalItem *		ei;
    BufferItem *		selRootBi;
    BufferItem *		bodySectBi;

    const SelectionScope *	ss= &(eo->eoSelectionScope);

    if  ( docGetRootOfSelectionScope( &ei, &selRootBi, &bodySectBi, bd, ss ) )
	{ LDEB(1); return -1;	}

    docInitRecalculateFields( &rf );

    rf.rfBd= bd;
    rf.rfVoidadd= (void *)add;
    rf.rfCloseObject= tedCloseObject;
    rf.rfUpdateFlags= eo->eoFieldUpdate;
    rf.rfFieldsUpdated= 0;

    if  ( eo->eoBulletsChanged > 0 )
	{ rf.rfUpdateFlags |= FIELDdoLISTTEXT; }

    if  ( eo->eoNotesDeleted > 0	||
	  eo->eoNotesAdded > 0		||
	  eo->eoSectionsDeleted > 0	||
	  eo->eoSectionsAdded > 0	)
	{
	int			changed= 0;

	docRenumberNotes( &changed, bd );

	if  ( changed )
	    { rf.rfUpdateFlags |= FIELDdoCHFTN;	}
	}

    if  ( rf.rfUpdateFlags )
	{
	rf.rfSelBegin= eo->eoSelectedRange.erStart;
	rf.rfSelEnd= eo->eoSelectedRange.erEnd;

	if  ( docRecalculateTextLevelFields( &rf, &(bd->bdItem) ) )
	    { LDEB(1);	}

	eo->eoSelectedRange.erStart= rf.rfSelBegin;
	eo->eoSelectedRange.erEnd= rf.rfSelEnd;
	}

    if  ( rf.rfFieldsUpdated > 0 )
	{
	DocumentSelection	dsLayout;

	if  ( docLayoutInvalidateRange( &dsLayout, selRootBi, er ) )
	    { LDEB(1); return -1;	}

	if  ( tedLayoutDocumentTree( td, add ) )
	    { LDEB(1); return -1;	}

	if  ( tedOpenItemObjects( &(bd->bdItem), bd, &(ed->edColors), add ) )
	    { LDEB(1); return -1;	}

	docIncludeRectangleInChange( eo, &(add->addBackRect) );
	}
    else{
	if  ( eo->eoNotesDeleted > 0	||
	      eo->eoNotesAdded > 0	||
	      eo->eoSectionsDeleted > 0	||
	      eo->eoSectionsAdded > 0	)
	    {
	    if  ( tedLayoutItem( &(bd->bdItem), bd, add, eo->eoScreenFontList,
						    &(eo->eoChangedRect) ) )
		{ LDEB(eo->eoNotesDeleted); return -1;	}

	    if  ( tedOpenItemObjects( &(bd->bdItem), bd,
						    &(ed->edColors), add ) )
		{ LDEB(1); return -1;	}
	    }
	else{
	    if  ( eo->eoParaAdjustParagraphNumber >= 0 )
		{
		int		paraNr= eo->eoParaAdjustParagraphNumber;
		BufferItem *	paraBi;

		if  ( er->erStart.epParaNr != paraNr )
		    { LLDEB(er->erStart.epParaNr,paraNr); }

		if  ( er->erEnd.epParaNr != paraNr )
		    { LLDEB(er->erEnd.epParaNr,paraNr); }

		paraBi= docGetParagraphByNumber( selRootBi, paraNr );

		if  ( tedAdjustParagraphLayout( eo, add, paraBi,
					    eo->eoParaAdjustFromLine,
					    eo->eoParaAdjustStroffShift,
					    eo->eoParaAdjustStroffUpto ) )
		    { LDEB(eo->eoParaAdjustFromLine); }

		if  ( tedOpenItemObjects( paraBi, bd, &(ed->edColors), add ) )
		    { LDEB(1); return -1;	}
		}
	    else{
		DocumentSelection	dsLayout;

		if  ( docLayoutInvalidateRange( &dsLayout,
							    selRootBi, er ) )
		    { LDEB(1); return -1;	}

		selRootBi= docGetSelectionRoot( &ei, &bodySectBi,
							    bd, &dsLayout );
		if  ( ! selRootBi )
		    { XDEB(selRootBi); return -1;	}

		while( selRootBi && selRootBi->biLevel > er->erBottomLevel )
		    { selRootBi= selRootBi->biParent;	}
		if  ( ! selRootBi )
		    { XLDEB(selRootBi,er->erBottomLevel); return -1;	}

		if  ( selRootBi->biInExternalItem != DOCinBODY	&&
		      ! selRootBi->biParent			)
		    {
		    int				page;

		    if  ( selRootBi->biLevel != DOClevSECT )
			{
			SDEB(docLevelStr(selRootBi->biLevel));
			return -1;
			}

		    page= selRootBi->biSectHeaderFooterUseForPage;
		    ei->eiPageFormattedFor= -1;

		    docLayoutExternalItem( ei, &(eo->eoChangedRect),
					    page, ei->eiY0UsedTwips,
					    bd, bodySectBi, add,
					    eo->eoScreenFontList,
					    tedInitLayoutExternalItem,
					    tedCloseObject );

		    if  ( tedOpenItemObjects( ei->eiItem, bd,
						    &(ed->edColors), add ) )
			{ LDEB(1); return -1;	}
		    }
		else{
		    if  ( tedLayoutItem( selRootBi,
					    bd, add, eo->eoScreenFontList,
					    &(eo->eoChangedRect) ) )
			{ LDEB(1);	}

		    if  ( tedOpenItemObjects( selRootBi, bd,
						    &(ed->edColors), add ) )
			{ LDEB(1); return -1;	}
		    }
		}
	    }
	}

    return 0;
    }

