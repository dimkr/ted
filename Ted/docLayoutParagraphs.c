/************************************************************************/
/*									*/
/*  Layout of a document. Layout of a series of paragraphs in a common	*/
/*  parent.								*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<appImage.h>
#   include	"docLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialize the formatting of a paragraph by determining its frame	*/
/*									*/
/************************************************************************/

static int docPsLayoutStartParagraph(
				int *				pToNextPage,
				const ParagraphLayoutContext *	plc,
				BufferItem *			paraBi,
				int				bottomTwips,
				int				stripHigh,
				const BlockFrame *		bf,
				ParagraphLayoutPosition *	plp )
    {
    const BufferDocument *	bd= plc->plcBd;
    const DocumentGeometry *	dg;
    const BufferItem *		sectBi= paraBi;

    const ScreenLayout *	sl= &(plc->plcScreenLayout);
    int				indentChanged= 0;

    while( sectBi && sectBi->biLevel != DOClevSECT )
	{ sectBi= sectBi->biParent;	}

    if  ( ! sectBi )
	{ XDEB(sectBi); return -1;	}

    if  ( paraBi->biParaListOverride > 0				&&
	  docAdaptParagraphToListLevel( &indentChanged, paraBi, bd )	)
	{ LDEB(1);		}

    dg= &(sectBi->biSectDocumentGeometry);

    /*  1  */
    if  ( paraBi->biInExternalItem == DOCinBODY			&&
	  ! paraBi->biParaInTable				&&
	  paraBi->biParaStartsOnNewPage				&&
	  ! plp->plpPos.lpAtTopOfColumn				)
	{ *pToNextPage= 1; return 0; }

    docParagraphFrameTwips( &(plp->plpFormattingFrame), bf,
					bottomTwips, stripHigh, paraBi );

    if  ( sl && sl->slScreenFrame )
	{
	(*sl->slScreenFrame)( &(plp->plpFormattingFrame),
						    plc->plcAdd, paraBi );
	}

    if  ( (*plc->plcStartParagraph)( plc->plcBd, paraBi, plc->plcAdd ) )
	{ LDEB(1); return -1;	}

    if  ( sl && sl->slStartParagraph					&&
	  (*sl->slStartParagraph)( paraBi,
		    &(plp->plpFormattingFrame),
		    plc->plcAdd, plc->plcScreenFontList, plc->plcBd )	)
	{ LDEB(1); return -1;	}

    *pToNextPage= 0;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Layout as much of a series of paragraph as fits on the current	*/
/*  page.								*/
/*									*/
/*  1)  When the paragraph starts on a new page, refuse to proceed on	*/
/*	the current one.						*/
/*  2)  When Widow/Orphan control is active and we could only place	*/
/*	one line on the page without finishing the paragraph. Force	*/
/*	the formatting of this paragraph to restart on the next page.	*/
/*  3)  When the paragraph is to be kept on one page, and it is not	*/
/*	possible to finish it on this page, Force the formatting of	*/
/*	paragraph to restart on the next page.				*/
/*  4)  When Widow/Orphan control is active and we finish the paragraph	*/
/*	by placing its last line on this page, skip back to the		*/
/*	penulimate line.						*/
/*  5)	NOTE the special case for 3 line paragraphs: The whole		*/
/*	paragraph is moved to the next page.				*/
/*									*/
/************************************************************************/

int docLayoutParagraphsInStrip(	ParagraphLayoutJob *		plj,
				BlockFrame *			bf,
				const ParagraphLayoutContext *	plc,
				BufferItem *			cellBi,
				int				paraUpto,
				int				bottomTwips,
				int				stripHigh )
    {
    int				changed= 0;
    ParagraphLayoutPosition *	plp= &(plj->pljPos);
    ParagraphStripPosition *	psp= &(plp->plpProgress);

    if  ( cellBi->biLevel != DOClevCELL )
	{ SDEB(docLevelStr(cellBi->biLevel)); return -1;	}

    while( psp->pspPara < paraUpto )
	{
	BufferItem *		paraBi= cellBi->biChildren[psp->pspPara];
	int			accepted;
	int			prevLine;
	LayoutPosition		lpBefore;

	lpBefore= plp->plpPos;

	if  ( psp->pspPart == 0 )
	    {
	    int		toNextPage= 0;

	    if  ( psp->pspPara == 0 )
		{ plp->plpPos.lpPageYTwips += cellBi->biTopInsetTwips; }

	    if  ( docPsLayoutStartParagraph( &toNextPage, plc, paraBi,
					bottomTwips, stripHigh, bf, plp ) )
		{ LDEB(1); return -1;	}

	    if  ( toNextPage )
		{ return 0;	}

	    paraBi->biTopPosition= plp->plpPos;
	    }

	prevLine= psp->pspLine;

	accepted= docLayoutLines( plp, bf, plc, paraBi );
	if  ( accepted < 0 )
	    { LDEB(accepted); return -1;	}

	/*  2  */
	if  ( paraBi->biParaWidowControl				&&
	      ! lpBefore.lpAtTopOfColumn				&&
	      psp->pspLine == 1						&&
	      psp->pspPart+ accepted < paraBi->biParaParticuleCount	&&
	      ! paraBi->biParaLines[0].tlHasPageBreak			)
	    {
	    psp->pspLine= 0;
	    psp->pspPart= 0;
	    break;
	    }

	/*  3  */
	if  ( ( paraBi->biParaKeepOnPage	||
	        paraBi->biParaKeepWithNext	)			&&
	      ! lpBefore.lpAtTopOfColumn				&&
	      psp->pspPart+ accepted < paraBi->biParaParticuleCount	&&
	      paraBi->biParaLineCount >= psp->pspLine			&&
	      psp->pspLine > 0						&&
	      ! paraBi->biParaLines[psp->pspLine- 1].tlHasPageBreak	)
	    {
	    psp->pspLine= 0;
	    psp->pspPart= 0;
	    break;
	    }

	psp->pspPart += accepted;

	if  ( psp->pspPart < paraBi->biParaParticuleCount		&&
	      ( accepted > 0 || ! lpBefore.lpAtTopOfColumn )		)
	    { break;	}

	/*  4  */
	if  ( paraBi->biParaWidowControl				&&
	      psp->pspLine > 1						&&
	      psp->pspLine- prevLine == 1				)
	    {
	    TextLine *		tl= paraBi->biParaLines+ prevLine- 1;

	    if  ( ! tl[0].tlHasPageBreak				&&
		  ! tl[0].tlTopPosition.lpAtTopOfColumn			&&
		  tl[0].tlTopPosition.lpPage < tl[1].tlTopPosition.lpPage )
		{
		/*  5  */
		if  ( paraBi->biParaLineCount == 3 )
		    {
		    BufferItem *	bi= paraBi;

		    psp->pspLine= 0;
		    psp->pspPart= 0;
		    plp->plpPos= tl[1].tlTopPosition;

		    while( bi->biNumberInParent > 0 )
			{
			bi= bi->biParent->biChildren[bi->biNumberInParent-1];
			if  ( ! bi->biParaKeepWithNext )
			    { break;	}

			psp->pspPara--;
			}
		    }
		else{
		    psp->pspLine= prevLine- 1;
		    psp->pspPart= tl->tlFirstParticule;
		    plp->plpPos= tl[1].tlTopPosition;
		    }

		continue;
		}
	    }

	(*plc->plcAdjustBottom)( &changed, paraBi, &(plp->plpPos), plc->plcAdd,
					    plc->plcChangedRectanglePixels );

	psp->pspLine= 0;
	psp->pspPart= 0;
	psp->pspPara++;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find a start point for the formatter to layout lines. A certain	*/
/*  work ahead is necessary to decide whether work on line layout can	*/
/*  be used or not. docPsAdvanceParagraphLayout() is used to decide	*/
/*  what work is final and what possibly is to be redone.		*/
/*									*/
/*  docPsAdvanceParagraphLayout() is called when a column is full. In	*/
/*  this case, everything that is not final yet needs to be moved to	*/
/*  the next page.							*/
/*									*/
/*  -   Insure that the constraints comming from Widow/Orphan control	*/
/*	are been obeyed.						*/
/*  -   Insure that paragraphs that should not be divided over pages	*/
/*	are not split.							*/
/*  -   Insure that paragraphs that should be with the next one as a	*/
/*	header really are with it. (Experimenting with Word shows that	*/
/*	paragraphs that should be with the next one should be		*/
/*	completely with it.)						*/
/*									*/
/*  When any of the above is violated, find a solution that respects	*/
/*  the requirements by pushing offending paragraphs or lines down.	*/
/*									*/
/*  docPsAdvanceParagraphLayout() shifts 'plp0' to the last position	*/
/*  that is not below 'plp1' that is not influenced by the layout of	*/
/*  position on 'plp1' or below. Everything before 'plp1' is formatted	*/
/*  and plp1->plpPos is the position where formatting would continue	*/
/*  if docPsAdvanceParagraphLayout() moved 'plp0' onto 'plp1'. (As it	*/
/*  often does).							*/
/*									*/
/*  REGULAR APPROACH							*/
/*  ================							*/
/*  a)  Avoid superfluous work and the handling of this exception	*/
/*	below: Commit an explit page break.				*/
/*  b)	If the current position is a legal position for a page break..	*/
/*	break here.							*/
/*									*/
/*  'Keep With Next' and 'Widow/Orphan Contol'				*/
/*  ==========================================				*/
/*  1)  Find the place to begin. Anything on a previous page is final:	*/
/*	It will not be shifted to the next page. (Paragraph)		*/
/*  2)	Idem: Line. The line found here might very well be irrelevant	*/
/*	because a different paragraph to commit is found below.		*/
/*  3)	Commit all paragraphs before the current one that do not have	*/
/*	the 'Keep with Next' flag.					*/
/*	flag.								*/
/*  4)	If the paragraph to be committed is before the current one	*/
/*	commit the position we have found. This can be:			*/
/*	-   The first line on the current page (from 2)			*/
/*	    [ This case is however excluded by the exception code (0)].	*/
/*	-   The head of a series of KeepWithNext paragraphs. (from 3)	*/
/*  5)	When this paragraph is to be kept together, restart from its	*/
/*	beginning.							*/
/*  6)	When Widow/Orphan control is active.. Do not commit the first	*/
/*	line before the whole paragraph is committed.			*/
/*  7)	The special cases of the last line of a paragraph with widow	*/
/*	orphan control and a paragraph with three lines are covered	*/
/*	in docLayoutParagraphsInStrip() at the end of a paragraph.	*/
/*	So no exceptions have to be made here. Also note the		*/
/*	preparations in docAdjustParaLayout().				*/
/*									*/
/*  8)	And finally when no exception applies, start from where we are	*/
/*	now.								*/
/*									*/
/************************************************************************/

static void docPsAdvanceParagraphLayout_x(
				ParagraphStripPosition *	psp0,
				const ParagraphStripPosition *	psp1,
				int				page,
				const BufferItem *		cellBi )
    {
    const BufferItem *		paraBi0;
    const BufferItem *		paraBi1;
    const TextLine *		tl;

    int				para;

    int				line0;

    paraBi0= cellBi->biChildren[psp0->pspPara];
    paraBi1= (const BufferItem *)0;

    /*  a  */
    if  ( psp1->pspPara < cellBi->biChildCount )
	{
	int		line1;

	paraBi1= cellBi->biChildren[psp1->pspPara];

	line1= psp1->pspLine;

	if  ( line1- 1 >= 0				&&
	      line1- 1 < paraBi1->biParaLineCount	)
	    {
	    const TextLine *	tl= paraBi1->biParaLines+ line1- 1;

	    if  ( tl->tlHasPageBreak )
		{
		*psp0= *psp1;
		return;
		}
	    }
	}

    /*  b  */
    if  ( paraBi1			&&
	  paraBi1->biParaKeepOnPage	&&
	  ! paraBi1->biParaKeepWithNext	&&
	  psp1->pspLine > 0		)
	{
	int		line= 1;

	if  ( paraBi1->biParaWidowControl )
	    { line++;	}

	if  ( psp1->pspLine >= line )
	    {
	    *psp0= *psp1;
	    return;
	    }
	}

    /*  1  */
    while( psp0->pspPara < psp1->pspPara )
	{
	if  ( paraBi0->biBelowPosition.lpPage >= page )
	    { break;	}

	psp0->pspPara++;
	psp0->pspLine= 0;
	psp0->pspPart= 0;
	paraBi0= cellBi->biChildren[psp0->pspPara];
	}

    /*  2  */
    if  ( psp0->pspPara < psp1->pspPara )
	{ line0= paraBi0->biParaLineCount;	}
    else{ line0= psp1->pspLine;			}

    tl= paraBi0->biParaLines+ psp0->pspLine;
    while( psp0->pspLine < line0 )
	{
	if  ( tl->tlTopPosition.lpPage >= page )
	    { break;	}

	psp0->pspLine++; tl++;
	}

    /*  3  */
    for ( para= psp0->pspPara; para < psp1->pspPara; para++ )
	{
	const BufferItem *	paraBi= cellBi->biChildren[para];

	if  ( ! paraBi->biParaKeepWithNext )
	    {
	    psp0->pspPara= para+ 1;
	    psp0->pspLine= 0;
	    psp0->pspPart= 0;
	    paraBi0= cellBi->biChildren[psp0->pspPara];
	    }
	}

    /*  4  */
    if  ( psp0->pspPara < psp1->pspPara )
	{ return;	}

    if  ( psp0->pspPara != psp1->pspPara )
	{ LLDEB(psp0->pspPara,psp1->pspPara );	}

    if  ( psp0->pspPara >= cellBi->biChildCount )
	{ return;	}

    /*  5  */
    if  ( paraBi0->biParaKeepOnPage	||
	  paraBi0->biParaKeepWithNext	)
	{
	psp0->pspLine= 0;
	psp0->pspPart= 0;

	return;
	}

    /*  6  */
    if  ( paraBi1			&&
	  paraBi1->biParaWidowControl	&&
	  psp1->pspLine == 1		&&
	  paraBi1->biParaLineCount >= 1	)
	{
	tl= paraBi1->biParaLines+ 0;

	if  ( tl->tlFirstParticule+ tl->tlParticuleCount <
					paraBi1->biParaParticuleCount	)
	    {
	    psp0->pspLine= 0;
	    psp0->pspPart= 0;

	    return;
	    }
	}

    /*  7  */

    /*  8  */
    *psp0= *psp1;

    return;
    }

void docPsAdvanceParagraphLayout(
				int *				pAdvanced,
				ParagraphStripPosition *	psp0,
				const ParagraphStripPosition *	psp00,
				const ParagraphStripPosition *	psp1,
				int				page,
				const BufferItem *		cellBi )
    {
    int		advanced= 0;

    docPsAdvanceParagraphLayout_x( psp0, psp1, page, cellBi );

    if  ( psp0->pspPara >    psp00->pspPara		||
	  ( psp0->pspPara == psp00->pspPara	&&
	    psp0->pspPart >  psp00->pspPart	)	)
	{ advanced= 1;	}

    *pAdvanced= advanced;
    }

/************************************************************************/
/*									*/
/*  Format the lines in a series of paragraphs. On the way keep an	*/
/*  administration on where to rebegin formatting at a page break.	*/
/*									*/
/************************************************************************/

int docLayoutParagraphs(	const ParagraphLayoutContext *	plc,
				BufferItem *			cellBi,
				BlockFrame *			bf,
				LayoutJob *			lj,
				ParagraphLayoutJob *		plj )
    {
    int				toNextPage= 0;

    const int			bottomTwips= -1;
    const int			stripHigh= -1;

    LayoutPosition		lpBefore;

    const ScreenLayout *	sl= &(plc->plcScreenLayout);

    lpBefore= plj->pljPos.plpPos;

    if  ( docLayoutParagraphsInStrip( plj, bf, plc, cellBi,
					    plj->pljParaUpto,
					    bottomTwips, stripHigh ) )
	{ LDEB(1); return -1;	}

    toNextPage= plj->pljPos.plpProgress.pspPara < plj->pljParaUpto;

    while( toNextPage )
	{
	int				advanced;
	ParagraphStripPosition		psp0;
	const int			belowText= 0;
	LayoutPosition			lpBelowNotes;

	if  ( BF_HAS_FOOTNOTES( bf )					&&
	      ! bf->bfFootnotesPlaced					&&
	      docLayoutFootnotesForColumn( &lpBelowNotes, bf,
			    belowText, &(plj->pljPos.plpPos), lj )	)
	    { LDEB(1); return -1;	}

	psp0= plj->pljPos0.plpProgress;

	docPsAdvanceParagraphLayout( &advanced,
					&psp0,
					&(plj->pljPos0.plpProgress),
					&(plj->pljPos.plpProgress),
					lpBefore.lpPage, cellBi );

	docLayoutToNextColumn( cellBi, plc->plcBd, &(plj->pljPos.plpPos), bf );

	if  ( advanced )
	    {
	    plj->pljPos0.plpProgress= psp0;
	    plj->pljPos.plpProgress= psp0;
	    }
	else{
	    plj->pljPos0.plpProgress= plj->pljPos.plpProgress;
	    }

	docParagraphStripFrame( sl, cellBi, plc->plcAdd,
				    bottomTwips, stripHigh, bf, plj );

	lpBefore= plj->pljPos.plpPos;

	if  ( docLayoutParagraphsInStrip( plj, bf, plc, cellBi,
					    plj->pljParaUpto,
					    bottomTwips, stripHigh ) )
	    { LDEB(1); return -1;	}

	toNextPage= plj->pljPos.plpProgress.pspPara < plj->pljParaUpto;
	}

    if  ( cellBi->biChildCount > 0 )
	{ cellBi->biTopPosition= cellBi->biChildren[0]->biTopPosition; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Calculate the formatting frame for a paragraph layout.		*/
/*									*/
/************************************************************************/

void docParagraphStripFrame(	const ScreenLayout *		sl,
				BufferItem *			cellBi,
				AppDrawingData *		add,
				int				bottomTwips,
				int				stripHigh,
				const BlockFrame *		bf,
				ParagraphLayoutJob *		plj )
					
    {
    BufferItem *		paraBi;

    paraBi= cellBi->biChildren[plj->pljPos.plpProgress.pspPara];

    docParagraphFrameTwips( &(plj->pljPos.plpFormattingFrame), bf,
					bottomTwips, stripHigh, paraBi );

    if  ( sl && sl->slScreenFrame )
	{
	(*sl->slScreenFrame)( &(plj->pljPos.plpFormattingFrame),
							    add, paraBi );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Initialize paragraph layout progress.				*/
/*									*/
/************************************************************************/

static void docPsInitParagraphLayoutPosition( ParagraphLayoutPosition *	plp )
    {
    plp->plpProgress.pspPara= 0;
    plp->plpProgress.pspLine= 0;
    plp->plpProgress.pspPart= 0;

    docInitLayoutPosition( &(plp->plpPos) );

    return;
    }

void docInitParagraphLayoutJob(	ParagraphLayoutJob *	plj )
    {
    plj->pljParaUpto= 0;

    docPsInitParagraphLayoutPosition( &(plj->pljPos) );
    docPsInitParagraphLayoutPosition( &(plj->pljPos0) );

    return;
    }

void docPsBeginParagraphLayoutProgress(	ParagraphLayoutJob *	plj,
					int			para,
					int			line,
					int			part,
					int			paraUpto,
					const LayoutPosition *	lp )
    {
    docInitParagraphLayoutJob( plj );

    plj->pljPos.plpProgress.pspPara= para;
    plj->pljPos.plpProgress.pspLine= line;
    plj->pljPos.plpProgress.pspPart= part;

    plj->pljParaUpto= paraUpto;

    plj->pljPos.plpPos= *lp;

    return;
    }

