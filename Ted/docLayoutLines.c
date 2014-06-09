/************************************************************************/
/*									*/
/*  Layout of a document.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	<appImage.h>
#   include	"docLayout.h"

#   include	<appDebugon.h>

#   define	SHOW_LINE_CHANGES	0

/************************************************************************/
/*									*/
/*  Layout successive lines of a paragraph.				*/
/*  Claim space above a particular line.				*/
/*									*/
/*  1)  Cope with the output of jade: Ignore enormous space before's	*/
/*	in footers.							*/
/*  2)  Do not claim 'space before' at the top of the page.		*/
/*									*/
/************************************************************************/

static void docAboveLine(	LayoutPosition *		lp,
				int				fromLinePos,
				const BufferItem *		paraBi,
				int				part,
				const DocumentGeometry *	dg,
				AppDrawingData *		add )
    {
    int			spaceAboveLineTwips= 0;

    /*  1  */
    if  ( part == 0					&&
	  ! fromLinePos					&&
	  paraBi->biInExternalItem == DOCinBODY		)
	{
	spaceAboveLineTwips= paraBi->biTopInsetTwips;

	/*  2  */
	if  ( lp->lpAtTopOfColumn )
	    { spaceAboveLineTwips -= paraBi->biParaSpaceBeforeTwips;	}
	}

    lp->lpPageYTwips += spaceAboveLineTwips;
    /* still at top */

    return;
    }

/************************************************************************/
/*									*/
/*  Layout successive lines of a paragraph.				*/
/*  Claim space below a particular line.				*/
/*									*/
/*  Also decide whether it will fit in the current formatting frame.	*/
/*									*/
/*  1)  Only use the space after paragraph property and the insets	*/
/*	of the table when the paragraph is in a table.			*/
/*  2)  Push the position for the next line down until below any shapes	*/
/*	on this line. Until we implement the various wrap modes, this	*/
/*	is the best we can do.						*/
/*									*/
/************************************************************************/

static int docBelowLine(	LayoutPosition *		lp,
				LayoutPosition *		lpBelowShapes,
				const TextLine *		tl,
				const BufferItem *		paraBi,
				int				partFrom,
				int				partUpto,
				const ParagraphFrame *		pf,
				const NotesReservation *	nrLine,
				AppDrawingData *		add )
    {
    const LayoutPosition *	lpLineTop= &(tl->tlTopPosition);

    int				spaceBelowLineTwips= 0;
    int				lineBottom;
    int				lineHeight;
    int				frameBottom;
    LayoutPosition		lpBelowLine;

    int				footnoteHeight;

    int				part;
    const TextParticule *	tp;

    if  ( partUpto == paraBi->biParaParticuleCount )
	{
	/*  1  */
	if  ( paraBi->biParaInTable )
	    {
	    if  ( paraBi->biNumberInParent ==
					paraBi->biParent->biChildCount- 1 )
		{
		const BufferItem *	bi= paraBi;

		while( bi )
		    {
		    spaceBelowLineTwips += bi->biBottomInsetTwips;

		    if  ( bi->biLevel == DOClevROW )
			{ break;	}

		    bi= bi->biParent;
		    }
		}
	    }
	}

    lpBelowLine= *lpLineTop;
    lpBelowLine.lpPageYTwips += tl->tlLineHeightTwips;
					/********************************/
					/*  But add spacing to find	*/
					/*  position for next line	*/
					/********************************/

    /*  2  */
    tp= paraBi->biParaParticules+ partFrom;
    for ( part= partFrom; part < partUpto; tp++, part++ )
	{
	const InsertedObject *	io;
	const DrawingShape *	ds;

	LayoutPosition		lpShapeTop;
	LayoutPosition		lpBelowShape;

	if  ( tp->tpKind != DOCkindOBJECT )
	    { continue;	}

	io=  paraBi->biParaObjects+ tp->tpObjectNumber;
	if  ( io->ioKind != DOCokDRAWING_SHAPE )
	    { continue;	}
	ds= io->ioDrawingShape;
	if  ( ! ds )
	    { XDEB(ds); continue;	}

	docShapePageY( &lpShapeTop, &lpBelowShape, ds, paraBi, lpLineTop, pf );

	docLayoutPushBottomDown( lpBelowShapes, &lpBelowShape );
	docLayoutPushBottomDown( &lpBelowLine, &lpBelowShape );
	}

    /* ?
    lineBottom= lpLineTop->lpPageYTwips+
				tl->tlLineHeightTwips+ spaceBelowLineTwips;
    */
    lineBottom= lpBelowLine.lpPageYTwips+ spaceBelowLineTwips;
    lineHeight= lineBottom- lpLineTop->lpPageYTwips;

    footnoteHeight= nrLine->nrFtnsepHeight+ nrLine->nrFootnoteHeight;
    frameBottom= pf->pfFrameY1Twips;
    if  ( pf->pfPageY1Twips- footnoteHeight < frameBottom )
	{ frameBottom= pf->pfPageY1Twips- footnoteHeight;	}

    if  ( pf->pfHasBottom				&&
	  lineBottom > frameBottom			&&
	  lineHeight < pf->pfPageHigh			&&
	  ( pf->pfStripHigh < 0			||
	    lineHeight < pf->pfStripHigh	)	)
	{
	*lp= *lpLineTop;
	lp->lpPageYTwips= pf->pfPageY1Twips+ 1;
	lp->lpAtTopOfColumn= 0;
	return 1;
	}

    *lp= *lpLineTop;
    lp->lpPageYTwips += tl->tlLineSpacingTwips;
    lp->lpAtTopOfColumn= 0;

    if  ( partUpto == paraBi->biParaParticuleCount )
	{
	spaceBelowLineTwips= paraBi->biBottomInsetTwips;

	lp->lpPageYTwips += spaceBelowLineTwips;
	lp->lpAtTopOfColumn= 0;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Calculations on line inserts.					*/
/*									*/
/************************************************************************/

static int docPlaceShape(	const BufferItem *	paraBi,
				const LayoutPosition *	lpLineTop,
				InsertedObject *	io,
				const ParagraphFrame *	pf,
				int			x0Twips )
    {
    DrawingShape *		ds= io->ioDrawingShape;
    int				x1;
    LayoutPosition		lpBelowShape;

    if  ( ! ds )
	{ XDEB(ds); return -1;	}

    docShapePageRectangle( &(io->ioY0Position), &lpBelowShape,
				&(io->ioX0Twips), &x1,
				ds, paraBi, lpLineTop, pf, x0Twips );

    docDrawingShapeInvalidateTextLayout( ds );

    return 0;
    }

static int docPlaceShapeY(	const BufferItem *	paraBi,
				const LayoutPosition *	lpLineTop,
				InsertedObject *	io,
				const ParagraphFrame *	pf )
    {
    DrawingShape *		ds= io->ioDrawingShape;
    LayoutPosition		lpBelowShape;

    docShapePageY( &(io->ioY0Position), &lpBelowShape,
						ds, paraBi, lpLineTop, pf );

    docDrawingShapeInvalidateTextLayout( ds );

    return 0;
    }

static int docPlaceLineInserts(	BufferDocument *	bd,
				const BufferItem *	paraBi,
				const TextLine *	tl,
				const TextParticule *	tp,
				const ParticuleData *	pd,
				int			accepted,
				const ParagraphFrame *	pf,
				const LayoutPosition *	lp )
    {
    int		i;

    for ( i= 0; i < accepted; tp++, pd++, i++ )
	{
	if  ( tp->tpKind == DOCkindFIELDSTART )
	    {
	    DocumentField *	df;

	    df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;

	    df->dfPage= lp->lpPage;
	    }

	if  ( tp->tpKind == DOCkindOBJECT )
	    {
	    InsertedObject *	io=  paraBi->biParaObjects+ tp->tpObjectNumber;

	    if  ( io->ioKind == DOCokDRAWING_SHAPE )
		{
		if  ( docPlaceShape( paraBi, &(tl->tlTopPosition),
							io, pf, pd->pdX0 ) )
		    { LDEB(1);	}
		}
	    }
	}

    return 0;
    }

static int docPlaceLineInsertsY(BufferDocument *	bd,
				const BufferItem *	paraBi,
				const TextLine *	tl,
				const TextParticule *	tp,
				int			accepted,
				const ParagraphFrame *	pf,
				const LayoutPosition *	lp )
    {
    int		i;

    for ( i= 0; i < accepted; tp++, i++ )
	{
	if  ( tp->tpKind == DOCkindFIELDSTART )
	    {
	    DocumentField *	df;

	    df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;

	    df->dfPage= lp->lpPage;
	    }

	if  ( tp->tpKind == DOCkindOBJECT )
	    {
	    InsertedObject *	io=  paraBi->biParaObjects+ tp->tpObjectNumber;

	    if  ( io->ioKind == DOCokDRAWING_SHAPE )
		{
		if  ( docPlaceShapeY( paraBi, &(tl->tlTopPosition), io, pf ) )
		    { LDEB(1);	}
		}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Layout successive lines of a paragraph.				*/
/*  Place a line on the page without reformatting its text. Just the	*/
/*  position changes, not the layout of the text inside the line.	*/
/*									*/
/*  0)  If, in a partial reformat of a paragraph, the number of		*/
/*	particules changes, the offsets in the lines that are only	*/
/*	placed must be adapted.						*/
/*									*/
/************************************************************************/

static int docPlace_Line(	TextLine *			resTl,
				NotesReservation *		pNrLine,
				LayoutPosition *		lpBelowShapes,
				const BufferItem *		paraBi,
				int				part,
				const ParagraphLayoutContext *	plc,
				const ParagraphFrame *		pf,
				const LayoutPosition *		lpTop,
				LayoutPosition *		lpBottom,
				const ScreenLayout *		sl )
    {
    BufferDocument *		bd= plc->plcBd;
    AppDrawingData *		add= plc->plcAdd;

    int				accepted;
    int				res;

    TextParticule *		tp= paraBi->biParaParticules+ part;

    TextLine			workTl;
    TextLine *			tl= &(workTl);

    LayoutPosition		lp;
    const SectionProperties *	sp;
    const DocumentGeometry *	dg;

    const int			fromLinePos= 0;

    NotesReservation		nrLine;

    lp= *lpTop;
    sp= &(paraBi->biParent->biParent->biParent)->biSectProperties;
    dg= &(sp->spDocumentGeometry);

    workTl= *resTl;

    /*  0  */
    tl->tlFirstParticule= part;

    if  ( tl->tlStroff != tp->tpStroff )
	{ LLDEB(tl->tlStroff,tp->tpStroff);	}

    docInitNotesReservation( &nrLine );

    /*  1  */
    docAboveLine( &lp, fromLinePos, paraBi, part, dg, add );

    tl->tlTopPosition= lp;
    accepted= tl->tlParticuleCount;

    if  ( accepted < 1 )
	{ LDEB(accepted); docListItem(0,paraBi); return -1; }

    if  ( tl->tlHasInlineContent )
	{
	docLayoutPushBottomDown( &(tl->tlTopPosition), lpBelowShapes );
	docLayoutPushBottomDown( &lp, lpBelowShapes );
	}

    if  ( docLayoutCollectParaFootnoteHeight( &nrLine,
					    tl->tlTopPosition.lpPage,
					    tl->tlTopPosition.lpColumn,
					    bd, paraBi, tl->tlFirstParticule,
					    tl->tlFirstParticule+ accepted ) )
	{ LDEB(1); return -1;	}

    res= docBelowLine( &lp, lpBelowShapes, tl, paraBi, part, part+ accepted,
							    pf, &nrLine, add );
    if  ( res < 0 )
	{ LDEB(res); return -1;	}
    if  ( res > 0 )
	{ *lpBottom= lp; return 0; }

    tl->tlX0Pixels= pf->pfX0Pixels;
    tl->tlX1Pixels= pf->pfX1Pixels;

    if  ( docPlaceLineInsertsY( bd, paraBi, tl, tp, accepted, pf, &lp ) )
	{ LDEB(accepted); }

    *lpBottom= lp;
    *pNrLine= nrLine;
    *resTl= *tl;

    return accepted;
    }

/************************************************************************/
/*									*/
/*  Layout successive lines of a paragraph.				*/
/*									*/
/*  1)  Cope with the output of sgmls: Ignore enormous space before's	*/
/*	in footers.							*/
/*									*/
/************************************************************************/

static int docLayout_Line(	TextLine *			resTl,
				NotesReservation *		pNrLine,
				LayoutPosition *		lpBelowShapes,
				const BlockFrame *		bf,
				int				fromLinePos,
				const BufferItem *		paraBi,
				int				part,
				ParticuleData *			pd,
				const ParagraphLayoutContext *	plc,
				const ParagraphFrame *		pf,
				const LayoutPosition *		lpTop,
				LayoutPosition *		lpBottom,
				const ScreenLayout *		sl )
    {
    BufferDocument *		bd= plc->plcBd;
    AppDrawingData *		add= plc->plcAdd;
    ScreenFontList *		sfl= plc->plcScreenFontList;

    int				accepted;
    int				res;

    TextParticule *		tp= paraBi->biParaParticules+ part;

    TextLine			workTl;
    TextLine *			tl= &(workTl);

    LayoutPosition		lp;
    const SectionProperties *	sp;
    const DocumentGeometry *	dg;

    NotesReservation		nrLine;

    lp= *lpTop;
    sp= &(paraBi->biParent->biParent->biParent)->biSectProperties;
    dg= &(sp->spDocumentGeometry);

    docInitTextLine( tl );
    tl->tlStroff= tp->tpStroff;
    tl->tlFirstParticule= part;

    docInitNotesReservation( &nrLine );

    /*  1  */
    docAboveLine( &lp, fromLinePos, paraBi, part, dg, add );

    tl->tlTopPosition= lp;
    accepted= docLayoutLineBox( bd, tl, paraBi, part,
					add->addPostScriptFontList, pd, pf );

    if  ( accepted < 1 )
	{ LDEB(accepted); return -1;	}

    if  ( tl->tlHasInlineContent )
	{
	docLayoutPushBottomDown( &(tl->tlTopPosition), lpBelowShapes );
	docLayoutPushBottomDown( &lp, lpBelowShapes );
	}

    if  ( docLayoutCollectParaFootnoteHeight( &nrLine,
					tl->tlTopPosition.lpPage,
					tl->tlTopPosition.lpColumn,
					bd, paraBi, tl->tlFirstParticule,
					tl->tlFirstParticule+ accepted ) )
	{ LDEB(1); return -1;	}

    res= docBelowLine( &lp, lpBelowShapes, tl, paraBi, part, part+ accepted,
							    pf, &nrLine, add );
    if  ( res < 0 )
	{ LDEB(res); return -1;	}
    if  ( res > 0 )
	{ *lpBottom= lp; return 0; }

    /* above
    tl->tlStroff= tp->tpStroff;
    tl->tlFirstParticule= part;
    */
    tl->tlStrlen=
	    tp[accepted-1].tpStroff+ tp[accepted-1].tpStrlen- tp->tpStroff;
    tl->tlParticuleCount= accepted;
    tl->tlFrameWidthTwips= pf->pfX1GeometryTwips- pf->pfX0GeometryTwips;
    tl->tlFrameX0Twips= pf->pfX0GeometryTwips;

    if  ( sl && sl->slLayoutLine					&&
	  (*sl->slLayoutLine)( tl, paraBi, bd, part, accepted, add, sfl,
							pf, pd ) < 0	)
	{ LDEB(accepted); return -1;	}

    tl->tlX0Pixels= pf->pfX0Pixels;
    tl->tlX1Pixels= pf->pfX1Pixels;

    if  ( docPlaceLineInserts( bd, paraBi, tl, tp, pd, accepted, pf, &lp ) )
	{ LDEB(accepted); }

    *lpBottom= lp;
    *pNrLine= nrLine;
    *resTl= *tl;

    return accepted;
    }

/************************************************************************/
/*									*/
/*  Layout a series of lines in a paragraph.				*/
/*									*/
/*  1)  Make sure that we have scratch space for the layout routines.	*/
/*  2)  As long as there are any particules to be placed/formapped	*/
/*  3)  Can the current line be reused?					*/
/*  4)  If so just place it at a new position.				*/
/*  5)  Otherwise recalculate layout.					*/
/*  6)  If the line does not fit on this page (column) stop.		*/
/*  7)  Cause the line to be redrawn if either it is reformatted, or it	*/
/*	is moved.							*/
/*  8)  Subtract the space needed for the footnotes in this line from	*/
/*	the space left on this page. (column)				*/
/*  9)  Insert into administration.					*/
/*  10) If the line ends in a page break, make sure nothing will fit on	*/
/*	this page (in this column) anymore.				*/
/*  11) Truncate the number of lines when the paragraph is completely	*/
/*	formatted.							*/
/*									*/
/************************************************************************/

int docLayoutLines(		ParagraphLayoutPosition *	plp,
				BlockFrame *			bf,
				const ParagraphLayoutContext *	plc,
				BufferItem *			paraBi )
    {
    ParagraphStripPosition *	psp= &(plp->plpProgress);
    int				part= psp->pspPart;

    int				done= 0;
    int				line= psp->pspLine;


    ParticuleData *		pd;

    LayoutPosition		lp;
    LayoutPosition		lpBelowShapes;

    lp= plp->plpPos;
    lpBelowShapes= lp;
    lpBelowShapes.lpPageYTwips= bf->bfYBelowShapes;

    /*  1  */
    if  ( docPsClaimParticuleData( paraBi, &pd ) )
	{ LDEB(paraBi->biParaParticuleCount); return -1;	}

    /*  2  */
    while( part < paraBi->biParaParticuleCount )
	{
	int			accepted;
	TextLine		boxLine;

	int			placeOldLine= 0;
	TextLine *		tlLine= (TextLine *)0;

	NotesReservation	nrLine;

	const ParagraphFrame *	pf= &(plp->plpFormattingFrame);
	DocumentRectangle *	drChanged= plc->plcChangedRectanglePixels;

	docInitNotesReservation( &nrLine );

	/*  3  */
	if  ( line < paraBi->biParaLineCount )
	    {
	    const TextParticule *	tp= paraBi->biParaParticules+ part;
	    int				frameWidthTwips;

	    frameWidthTwips= pf->pfX1GeometryTwips- pf->pfX0GeometryTwips;

	    tlLine= paraBi->biParaLines+ line;
	    placeOldLine= 1;

	    if  ( tlLine->tlFirstParticule+ tlLine->tlParticuleCount >
					paraBi->biParaParticuleCount	||
		  tlLine->tlStroff != tp->tpStroff			||
		  tlLine->tlFrameWidthTwips != bf->bfColumnWidthTwips	||
		  tlLine->tlFrameX0Twips != pf->pfX0GeometryTwips	)
		{ placeOldLine= 0;	}
	    }

	/*  4  */
	if  ( placeOldLine )
	    {
	    boxLine= *tlLine;

	    accepted= docPlace_Line( &boxLine, &nrLine, &lpBelowShapes,
					paraBi, part, plc, pf,
					&lp, &lp, &(plc->plcScreenLayout) );

#	    if SHOW_LINE_CHANGES
	    if  ( accepted > 0 )
		{ docListTextLine( 0, "P+", line, paraBi, &boxLine );	}
	    else{ appDebug( "%s\n", "p-" );				}
#	    endif
	    }
	else{
	    const int		fromLinePos= 0;

	    /*  5  */
	    accepted= docLayout_Line( &boxLine, &nrLine, &lpBelowShapes,
					bf, fromLinePos,
					paraBi, part, pd, plc, pf,
					&lp, &lp, &(plc->plcScreenLayout) );
#	    if SHOW_LINE_CHANGES
	    if  ( accepted > 0 )
		{ docListTextLine( 0, "L+", line, paraBi, &boxLine );	}
	    else{ appDebug( "%s\n", "l-" );				}
#	    endif
	    }

	if  ( accepted < 0 )
	    { LDEB(accepted); return -1;	}

	/*  6  */
	if  ( accepted == 0 )
	    { break;	}

	/*  7  */
	if  ( drChanged	)
	    {
	    DocumentRectangle		drBox;

	    drBox.drX0= pf->pfRedrawX0Pixels;
	    drBox.drX1= pf->pfRedrawX1Pixels;
	    drBox.drY0= TL_TOP_PIXELS( plc->plcAdd, &boxLine );
	    drBox.drY1= TL_BELOW_PIXELS( plc->plcAdd, &boxLine );

	    geoUnionRectangle( drChanged, drChanged, &drBox );

	    if  ( tlLine )
		{
		DocumentRectangle	drTl;

		drTl.drX0= pf->pfRedrawX0Pixels;
		drTl.drX1= pf->pfRedrawX1Pixels;
		drTl.drY0= TL_TOP_PIXELS( plc->plcAdd, tlLine );
		drTl.drY1= TL_BELOW_PIXELS( plc->plcAdd, tlLine );

		if  ( drTl.drX0 != drBox.drX0	||
		      drTl.drX1 != drBox.drX1	||
		      drTl.drY0 != drBox.drY0	||
		      drTl.drY1 != drBox.drY1	)
		    {
		    geoUnionRectangle( drChanged, drChanged, &drTl );
		    geoUnionRectangle( drChanged, drChanged, &drBox );

#		    if SHOW_LINE_CHANGES
		    appDebug( "EXPOSE [%4d..%4d x %4d..%4d]\n",
					    drChanged->drX0, drChanged->drX1,
					    drChanged->drY0, drChanged->drY1 );
#		    endif
		    }
		}
	    else{
		geoUnionRectangle( drChanged, drChanged, &drBox );

#		if SHOW_LINE_CHANGES
		appDebug( "EXPOSE [%4d..%4d x %4d..%4d]\n",
					    drChanged->drX0, drChanged->drX1,
					    drChanged->drY0, drChanged->drY1 );
#		endif
		}
	    }

	/*  8  */
	docLayoutReserveNoteHeight( &(plp->plpFormattingFrame), bf, &nrLine );

	if  ( bf->bfYBelowShapes < lpBelowShapes.lpPageYTwips )
	    { bf->bfYBelowShapes=  lpBelowShapes.lpPageYTwips; }

	/*  9  */
	if  ( line >= paraBi->biParaLineCount )
	    {
	    tlLine= docInsertTextLine( paraBi, -1 );
	    if  ( ! tlLine )
		{ XDEB(tlLine); return -1;		}
	    }
	else{
	    tlLine= paraBi->biParaLines+ line;
	    }

	*tlLine= boxLine;

	part += accepted; done += accepted; line++;

	/*  10  */
	if  ( boxLine.tlHasPageBreak )
	    {
	    lp.lpPageYTwips= plp->plpFormattingFrame.pfPageY1Twips+ 1;
	    lp.lpAtTopOfColumn= 0;
	    break;
	    }
	}

    psp->pspLine= line;
    plp->plpPos= lp;

    /*  11  */
    if  ( part >= paraBi->biParaParticuleCount	&&
	  paraBi->biParaLineCount > line	)
	{ paraBi->biParaLineCount=  line; }

    return done;
    }

