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

#   include	"docLayout.h"
#   include	"tedEdit.h"

#   include	<appDebugon.h>

#   define	SHOW_SELECTION_RANGE	0

/************************************************************************/
/*									*/
/*  Replace the selection in the document with new text.		*/
/*									*/
/*  b)  Replace the tail of the beginning paragraph with the new text.	*/
/*  c)  Merge the two paragraphs.					*/
/*  d)  Erase all paragraphs after the beginning of the selection	*/
/*	to, including the end.						*/
/*  e)  Update the paragraph buffer and the particule administration.	*/
/*									*/
/*  B)  The first particule of the line was split, also reformat the	*/
/*	previous line.							*/
/*	If paragraphs were merged, redraw the whole neighbourhood.	*/
/*  C)  - Recalculate the geometry from the start of the selection to	*/
/*	  the end of the paragraph.					*/
/*	- Redraw the affected part of the paragraph.			*/
/*  6)  Update the notion of current particule and current line.	*/
/*  7)  Redraw the I-Bar.						*/
/*									*/
/************************************************************************/

int tedEditReplaceSelection(	EditOperation *		eo,
				DocumentSelection *	dsReplacement,
				EditDocument *		ed,
				const unsigned char *	addedText,
				int			addedLength )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);
    DocumentFontList *		dfl= &(dp->dpFontList);

    int				stroffShift= 0;
    int				partShift= 0;

    BufferItem *		bi;

    int				line;
    int				stroff;

    TextLine *			tl;
    int				upto;

    DocumentSelection		dsRep;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;
    int				multiParagraph;

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return -1;	}

    docInitDocumentSelection( &dsRep );

    bi= ds.dsBegin.dpBi;
    stroff= ds.dsBegin.dpStroff;
    line= sg.sgBegin.pgLine;

    tl= bi->biParaLines+ line;
    upto= tl->tlStroff+ tl->tlStrlen;

    multiParagraph= ds.dsEnd.dpBi != ds.dsBegin.dpBi;

    /*  b,c,d,e  */
    if  ( docReplaceSelection( eo, &ds,
				    &partShift, &stroffShift,
				    addedText, addedLength,
				    td->tdCurrentTextAttributeNumber ) )
	{ LDEB(addedLength); return -1;	}

    if  ( addedLength > 0						&&
	  td->tdCurrentTextAttribute.taFontNumber >= 0			&&
	  td->tdCurrentTextAttribute.taFontNumber < dfl->dflFontCount	)
	{ dfl->dflFonts[td->tdCurrentTextAttribute.taFontNumber].dfUsed= 1; }

    /*  B  */
    tedAdjustRedrawBegin( ed, eo, &line );

    docCheckNoBreakAsLast( eo, bi );

    /*  C  */
    if  ( multiParagraph )
	{
	docEditIncludeItemInReformatRange( eo, bi );
	}
    else{
	docSetParagraphAdjust( eo, bi, line, stroffShift, upto+ addedLength );
	}

    docSetParaSelection( &dsRep, bi, addedLength > 0, stroff, addedLength );

    *dsReplacement= dsRep;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Merge the selection into one paragraph.				*/
/*									*/
/************************************************************************/

static int tedIsIndentationParticule(	const BufferItem *	bi,
					const TextParticule *	tp )
    {
    const unsigned char *	s= bi->biParaString+ tp->tpStroff;
    int				i;

    if  ( tp->tpKind == DOCkindTAB )
	{ return 1;	}

    if  ( tp->tpKind != DOCkindTEXT )
	{ return 0;	}

    for ( i= 0; i < tp->tpStrlen; i++ )
	{
	if  ( *(s++) != ' ' )
	    { return 0;	}
	}

    return 1;
    }

static void tedMergeParagraphsInSelectionLow(	EditDocument *	ed,
						int *		fieldMap )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    BufferItem *		biTo;
    BufferItem *		biFrom;

    int				endMoved= 0;

    DocumentCopyJob		dcj;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    docInitDocumentCopyJob( &dcj );

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    if  ( docSet1DocumentCopyJob( &dcj, bd ) )
	{ LDEB(1); goto ready;	}

    biTo= ds.dsBegin.dpBi;
    biFrom= docNextParagraph( biTo );
    if  ( ! biTo )
	{ XDEB(biTo); goto ready;	}

    endMoved= biTo->biParaStrlen;

    for (;;)
	{
	int	partFrom= 0;

	int	particulesInserted= 0;
	int	charactersCopied= 0;

	if  ( biFrom->biParaListOverride > 0 )
	    {
	    int		fieldNr= -1;
	    int		partBegin= -1;
	    int		partEnd= -1;
	    int		stroffBegin= -1;
	    int		stroffEnd= -1;

	    if  ( docDelimitParaHeadField( &fieldNr, &partBegin, &partEnd,
						&stroffBegin, &stroffEnd,
						biFrom, bd ) )
		{ LDEB(1);	}
	    else{
		if  ( partFrom <= partEnd )
		    { partFrom= partEnd+ 1;	}
		}
	    }

	while( partFrom < biFrom->biParaParticuleCount- 1		&&
	       tedIsIndentationParticule( biFrom,
				biFrom->biParaParticules+ partFrom )	)
	    { partFrom++; }

	if  ( partFrom < biFrom->biParaParticuleCount )
	    {
	    int		toCount= biTo->biParaParticuleCount;

	    if  ( toCount > 0						&&
		  biTo->biParaParticules[toCount-1].tpKind ==
							DOCkindTEXT	&&
		  biTo->biParaString[biTo->biParaStrlen-1] != ' '	)
		{
		if  ( docInflateTextString( biTo, 1 ) )
		    { LDEB(biTo->biParaStrlen); goto ready; }

		biTo->biParaString[biTo->biParaStrlen++]= ' ';
		biTo->biParaString[biTo->biParaStrlen  ]= '\0';
		biTo->biParaParticules[toCount- 1].tpStrlen++;
		endMoved++;
		}

	    if  ( docCopyParticules( &dcj, &eo,
			biTo, biFrom,
			biTo->biParaParticuleCount, partFrom,
			biFrom->biParaParticuleCount- partFrom,
			&particulesInserted, &charactersCopied ) )
		{ LDEB(biFrom->biParaParticuleCount); goto ready;	}
	    }

	if  ( biFrom == ds.dsEnd.dpBi )
	    {
	    endMoved -= biFrom->biParaParticules[partFrom].tpStroff;
	    break;
	    }

	endMoved += charactersCopied;

	biFrom= docNextParagraph( biFrom );
	if  ( ! biFrom )
	    { XDEB(biFrom); goto ready;	}
	}

    biTo= ds.dsBegin.dpBi;
    if  ( ! biTo )
	{ XDEB(biTo); goto ready;	}

    if  ( docRemoveSelectionTail( &eo, &ds ) )
	{ LDEB(1); goto ready;	}

    if  ( ed->edFormat == TEDdockindTEXT_SAVE_FOLDED )
	{ ed->edFormat= TEDdockindTEXT_SAVE_WIDE;	}

    docEditIncludeItemInReformatRange( &eo, biTo );

    eo.eoNotesAdded += dcj.dcjNotesCopied;
    eo.eoBulletsChanged += dcj.dcjBulletsCopied;

    {
    const int		direction= 1;
    const int		col0= -1;
    const int		col1= -1;

    DocumentSelection	dsNew;

    docInitDocumentSelection( &dsNew );

    docSetDocumentPosition( &(ds.dsEnd), biTo, ds.dsEnd.dpStroff+ endMoved );

    docSetRangeSelection( &dsNew, &(ds.dsBegin), &(ds.dsEnd),
					    direction, col0, col1 );

    if  ( tedEditFinishSelection( ed, &eo, &dsNew ) )
	{ LDEB(1);	}
    }

  ready:

    docCleanDocumentCopyJob( &dcj );

    return;
    }

void tedMergeParagraphsInSelection(	EditDocument *	ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    int *			fieldMap;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return;	}

    if  ( docIsParaSelection( &ds ) )
	{ LDEB(1); return; }

    fieldMap= docAllocateFieldMap( bd );
    if  ( ! fieldMap )
	{ XDEB(fieldMap); return;	}

    tedMergeParagraphsInSelectionLow( ed, fieldMap );

    free( fieldMap );

    return;
    }

/************************************************************************/
/*									*/
/*  Split a paragraph.							*/
/*									*/
/*  1)  Split in the buffer administration.				*/
/*  2)  Recalculate the geometry from the start of the selection to the	*/
/*	end of the paragraph.						*/
/*  3)  Redraw the affected part of the paragraph.			*/
/*  4)  Cause the new paragraph to be reformatted in this edit		*/
/*	operation.							*/
/*  5)  If the new paragraph is part of a numbered/bulleted list,	*/
/*	insert a list text field at the head.				*/
/*									*/
/************************************************************************/

int tedSplitParaContents(	EditOperation *			eo,
				BufferItem **			pNewBi,
				const DocumentPosition *	dp,
				EditDocument *			ed,
				int				splitLevel,
				int				onNewPage )
    {
    BufferItem *		newBi;

    AppDrawingData *		add= &(ed->edDrawingData);

    /*  1  */
    if  ( docSplitParaItemAtStroff( eo, &newBi, dp, splitLevel ) )
	{ LDEB(dp->dpStroff); return -1;	}

    newBi->biParaStartsOnNewPage= ! newBi->biParaInTable && onNewPage;
    
    /*  3  */
    eo->eoChangedRect.drX0= add->addBackRect.drX0;

    if  ( dp->dpBi->biParaBottomBorder.bpStyle != DOCbsNONE )
	{ dp->dpBi->biParaBottomBorder.bpStyle=   DOCbsNONE;	}

    if  ( newBi->biParaTopBorder.bpStyle != DOCbsNONE )
	{ newBi->biParaTopBorder.bpStyle=   DOCbsNONE;	}

    *pNewBi= newBi; return 0;
    }

void tedSplitParagraph(		EditDocument *		ed,
				int			onNewPage )
    {
    BufferItem *		newBi;

    DocumentSelection		dsRep;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			stroff= 0;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    if  ( tedEditReplaceSelection( &eo, &dsRep, ed, (unsigned char *)0, 0 ) )
	{ return;	}

    if  ( tedSplitParaContents( &eo, &newBi, &(dsRep.dsBegin),
						ed, DOClevPARA, onNewPage ) )
	{ LDEB(1); return;	}

    /*  7,8  */
    tedEditFinishIBarSelection( ed, &eo, newBi, stroff );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert an 'Object'.							*/
/*									*/
/************************************************************************/

int tedReplaceSelectionWithObject(	EditDocument *		ed,
					InsertedObject *	io )
    {
    TextParticule *		tp;

    int				stroffShift;
    int				partShift;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		dsRep;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 0 );

    if  ( tedEditReplaceSelection( &eo, &dsRep, ed,
					    (const unsigned char *)0, 0 ) )
	{ LDEB(1); return -1;	}

    tp= docParaSpecialParticule( &eo, eo.eoInsParaBi, DOCkindOBJECT,
					eo.eoInsParticule, eo.eoInsStroff,
					&partShift, &stroffShift );
    if  ( ! tp )
	{ LXDEB(eo.eoInsParticule,tp); return -1;	}

    tp->tpObjectNumber= eo.eoInsParaBi->biParaObjectCount++;

    /*  3,4,5  */
    docExtendParagraphAdjust( &eo, eo.eoInsParaBi, stroffShift );

    /*  6,7  */
    eo.eoInsStroff += stroffShift;
    tedEditFinishIBarSelection( ed, &eo, eo.eoInsParaBi, eo.eoInsStroff );

    appDocumentChanged( ed, 1 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  An object has been resized: Redo layout.				*/
/*									*/
/************************************************************************/

int tedEditReopenObject(	EditDocument *			ed,
				int				part,
				const DocumentPosition *	dpObj,
				const PositionGeometry *	pgObj )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    AppDrawingData *		add= &(ed->edDrawingData);
    double			xfac= add->addMagnifiedPixelsPerTwip;

    BufferItem *		bi= dpObj->dpBi;
    TextParticule *		tp= bi->biParaParticules+ part;
    InsertedObject *		io= bi->biParaObjects+ tp->tpObjectNumber;
    int				line= pgObj->pgLine;

    const int			stroffShift= 0;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    io->ioPixelsWide= TWIPStoPIXELS( xfac,
				( io->ioScaleXUsed* io->ioTwipsWide )/ 100 );
    tp->tpPixelsWide= io->ioPixelsWide;

    io->ioPixelsHigh= TWIPStoPIXELS( xfac,
				( io->ioScaleYUsed* io->ioTwipsHigh )/ 100 );

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 0 );

    tedAdjustRedrawBegin( ed, &eo, &line );

    if  ( tedReopenObject( bd, bi, tp, &(ed->edColors), add ) )
	{ LDEB(1); return -1;	}

    docSetParagraphAdjust( &eo, bi, line, stroffShift,
					dpObj->dpStroff+ 1+ stroffShift );

    if  ( tedEditFinishOldSelection( ed, &eo ) )
	{ LDEB(1);	}

    appDocumentChanged( ed, 1 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Replace the selection with something new.				*/
/*									*/
/*  1)  No selection... Just refuse.					*/
/*  2)  Replace nothing with nothing: Do nothing.			*/
/*	(Saves a lot of work, and avoids some combinations of special	*/
/*	cases)								*/
/*									*/
/************************************************************************/

void tedDocReplaceSelection(	EditDocument *		ed,
				const unsigned char *	word,
				int			len )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    DocumentSelection		dsRep;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			lastLine= 0;

    /*  1  */
    if  ( ! tedHasSelection( td ) )
	{ LDEB(1); return; }

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 0 );

    /*  2  */
    if  ( eo.eoIBarSelectionOld && len == 0 )
	{ return;	}

    if  ( tedEditReplaceSelection( &eo, &dsRep, ed, word, len ) )
	{ return;	}

    /*  6,7  */
    tedEditFinishEndOfInsert( ed, &eo, lastLine );

    appDocumentChanged( ed, 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  Delete the current paragraph from a document.			*/
/*									*/
/************************************************************************/

int tedDeleteCurrentParagraph(	EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    BufferItem *		paraBi;

    DocumentPosition		dpNew;
    BufferItem *		parentBi;
    int				childNumber;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );
    paraBi= ds.dsBegin.dpBi;

    docInitDocumentPosition( &dpNew );
    if  ( docLastPosition( &dpNew, paraBi ) 	||
	  docNextPosition( &dpNew )		)
	{
	docInitDocumentPosition( &dpNew );
	if  ( docFirstPosition( &dpNew, paraBi )	||
	      docPrevPosition( &dpNew )			)
	    { docInitDocumentPosition( &dpNew ); }
	}

    parentBi= paraBi->biParent;
    childNumber= paraBi->biNumberInParent;
    while( parentBi->biChildCount < 2			&&
	   parentBi->biParent				)
	{
	if  ( parentBi->biParent->biLevel == DOClevROW		&&
	      parentBi->biParent->biRowHasTableParagraphs	)
	    { break;	}

	if  ( parentBi->biLevel == DOClevSECT )
	    { break;	}

	childNumber= parentBi->biNumberInParent;
	parentBi= parentBi->biParent;
	}

    if  ( parentBi->biChildCount < 2 )
	{
	if  ( docFirstPosition( &dpNew, paraBi ) )
	    { LDEB(1); return -1;	}

	tedEditIncludeItemInRedraw( &eo, ed, paraBi );

	if  ( docEditMakeParagraphEmpty( paraBi, &eo ) )
	    { LDEB(1); return -1;	}

	docEditIncludeItemInReformatRange( &eo, paraBi );

	dpNew.dpBi= paraBi; dpNew.dpStroff= 0;
	}
    else{
	int		firstParaDeleted= -1;
	int		sectionsDeleted= 0;
	int		paragraphsDeleted= 0;

	tedEditIncludeItemInRedraw( &eo, ed, paraBi );

	docEditDeleteItems( &eo, &sectionsDeleted,
					&firstParaDeleted, &paragraphsDeleted,
					parentBi, childNumber, 1 );

	docEditIncludeItemInReformatRange( &eo, parentBi );
	}

    tedEditFinishIBarSelection( ed, &eo, dpNew.dpBi, dpNew.dpStroff );

    appDocumentChanged( ed, 1 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make sure that a Section contains an empty paragraph.		*/
/*									*/
/************************************************************************/

int tedSectionParagraph(
			EditOperation *			eo,
			BufferItem **			pParaBi,
			BufferItem *			sectBi,
			int				sectShift,
			const ParagraphProperties *	pp,
			int				textAttributeNumber )
    {
    BufferDocument *	bd= eo->eoBd;
    BufferItem *	paraBi;

    paraBi= docInsertEmptyParagraph( bd, sectBi, textAttributeNumber );
    if  ( ! paraBi )
	{ XDEB(paraBi); return -1;	}

    if  ( pp )
	{
	PropertyMask		ppChgMask;
	PropertyMask		ppUpdMask;

	const int * const	colorMap= (const int *)0;
	const int * const	listStyleMap= (const int *)0;

	PROPmaskCLEAR( &ppChgMask );

	PROPmaskCLEAR( &ppUpdMask );
	utilPropMaskFill( &ppUpdMask, PPprop_COUNT );
	PROPmaskUNSET( &ppUpdMask, PPpropIN_TABLE );

	if  ( docEditUpdParaProperties( eo, &ppChgMask, paraBi,
				    &ppUpdMask, pp, colorMap, listStyleMap ) )
	    { LDEB(1);	}
	}

    {
    int			paraNr= docNumberOfParagraph( paraBi );
    const int		paraShift= 1;
    const int		stroffFrom= 0;
    const int		stroffShift= 0;

    docEditShiftReferences( eo, &(sectBi->biSectSelectionScope),
				    paraNr, stroffFrom,
				    sectShift, paraShift, stroffShift );
    }

    *pParaBi= paraBi; return 0;
    }

/************************************************************************/
/*									*/
/*  Delete the current section.						*/
/*									*/
/************************************************************************/

int tedDeleteCurrentSection(	EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    BufferItem *		paraBi;
    BufferItem *		sectBi;

    DocumentPosition		dpNew;
    BufferItem *		parentBi;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    int				newPositionIsPast= 1;

    int				paraNrOld= -1;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );
    paraBi= ds.dsBegin.dpBi;

    sectBi= paraBi->biParent->biParent->biParent;
    parentBi= sectBi->biParent;

    docInitDocumentPosition( &dpNew );
    if  ( docLastPosition( &dpNew, sectBi ) 	||
	  docNextPosition( &dpNew )		)
	{
	newPositionIsPast= 0;

	docInitDocumentPosition( &dpNew );
	if  ( docFirstPosition( &dpNew, sectBi )	||
	      docPrevPosition( &dpNew )			)
	    { docInitDocumentPosition( &dpNew ); }
	}

    if  ( parentBi->biInExternalItem == DOCinBODY	&&
	  newPositionIsPast				)
	{ paraNrOld= docNumberOfParagraph( dpNew.dpBi );	}

    if  ( parentBi->biChildCount < 2 )
	{
	ParagraphProperties	pp;
	TextParticule *		tp= paraBi->biParaParticules;
	int			textAttributeNumber= tp->tpTextAttributeNumber;

	PropertyMask		ppChgMask;
	PropertyMask		ppUpdMask;

	int			firstParaDeleted= -1;
	int			sectionsDeleted= 0;
	int			paragraphsDeleted= 0;
	const int		sectShift= 0;

	const int * const	colorMap= (const int *)0;
	const int * const	listStyleMap= (const int *)0;

	docInitParagraphProperties( &pp );

	PROPmaskCLEAR( &ppChgMask );

	PROPmaskCLEAR( &ppUpdMask );
	utilPropMaskFill( &ppUpdMask, PPprop_COUNT );
	PROPmaskUNSET( &ppUpdMask, PPpropIN_TABLE );

	if  ( docUpdParaProperties( &ppChgMask, &pp,
				&ppUpdMask, &(paraBi->biParaProperties),
				colorMap, listStyleMap ) )
	    { LDEB(1); return -1;	}

	tedEditIncludeItemInRedraw( &eo, ed, sectBi );

	docEditDeleteItems( &eo, &sectionsDeleted,
					&firstParaDeleted, &paragraphsDeleted,
					sectBi, 0, sectBi->biChildCount );

	if  ( tedSectionParagraph( &eo, &paraBi, sectBi, sectShift,
						&pp, textAttributeNumber ) )
	    { LDEB(1); return -1;	}

	docEditIncludeItemInReformatRange( &eo, sectBi );

	docCleanParagraphProperties( &pp );

	if  ( docFirstPosition( &dpNew, paraBi ) )
	    { LDEB(1);	}
	}
    else{
	int		sectionsDeleted= 0;
	int		firstParaDeleted= -1;
	int		paragraphsDeleted= 0;

	tedEditIncludeItemInRedraw( &eo, ed, sectBi );

	docEditDeleteItems( &eo, &sectionsDeleted,
				    &firstParaDeleted, &paragraphsDeleted,
				    parentBi, sectBi->biNumberInParent, 1 );

	docEditIncludeItemInReformatRange( &eo, sectBi );
	}

    docEditIncludeItemInReformatRange( &eo, parentBi );

    tedEditFinishIBarSelection( ed, &eo, dpNew.dpBi, dpNew.dpStroff );

    appDocumentChanged( ed, 1 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a paragraph Before/After the selection, depending on the	*/
/*  value of the 'after' argument.					*/
/*									*/
/************************************************************************/

int tedInsertParagraph(	EditApplication *	ea,
			int			after )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    BufferItem *		paraBi;
    BufferItem *		newBi;

    int				textAttributeNumber;
    BufferItem *		parentBi;
    int				pos;

    PropertyMask		ppChgMask;
    PropertyMask		ppUpdMask;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpRef;

    int				paraNr;
    const int			paraShift= 1;

    const int * const		colorMap= (const int *)0;
    const int * const		listStyleMap= (const int *)0;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    if  ( after )
	{
	paraBi= ds.dsEnd.dpBi;
	if  ( ! paraBi )
	    { XDEB(paraBi); return -1;	}

	if  ( docLastPosition( &dpRef, paraBi ) )
	    { LDEB(1); docInitDocumentPosition( &dpRef );	}

	pos= paraBi->biNumberInParent+ 1;
	textAttributeNumber= paraBi->biParaParticules[
			paraBi->biParaParticuleCount-1].tpTextAttributeNumber;
	}
    else{
	paraBi= ds.dsBegin.dpBi;
	if  ( ! paraBi )
	    { XDEB(paraBi); return -1;	}

	if  ( docFirstPosition( &dpRef, paraBi ) )
	    { LDEB(1); docInitDocumentPosition( &dpRef );	}

	pos= paraBi->biNumberInParent;
	textAttributeNumber= paraBi->biParaParticules[0].tpTextAttributeNumber;
	}

    tedEditIncludeItemInRedraw( &eo, ed, paraBi );

    parentBi= paraBi->biParent;

    newBi= docInsertItem( bd, parentBi, pos, DOClevPARA );
    if  ( ! newBi )
	{ XDEB(newBi); return -1;	}

    paraNr= docNumberOfParagraph( newBi );

    {
    const int	sectShift= 0;
    const int	stroffFrom= 0;
    const int	stroffShift= 0;

    docEditShiftReferences( &eo, &(eo.eoSelectionScope),
				    paraNr+ 1, stroffFrom,
				    sectShift, paraShift, stroffShift );
    }

    PROPmaskCLEAR( &ppChgMask );

    PROPmaskCLEAR( &ppUpdMask );
    utilPropMaskFill( &ppUpdMask, PPprop_COUNT );

    if  ( docEditUpdParaProperties( &eo, &ppChgMask, newBi,
				    &ppUpdMask, &(paraBi->biParaProperties),
				    colorMap, listStyleMap ) )
	{ LDEB(1);	}

    if  ( ! docInsertTextParticule( newBi, 0, 0, 0,
					DOCkindTEXT, textAttributeNumber ) )
	{ LDEB(1); return -1;	}

    docEditIncludeItemInReformatRange( &eo, paraBi );
    docEditIncludeItemInReformatRange( &eo, newBi );

    tedEditFinishIBarSelection( ed, &eo, newBi, 0 );

    appDocumentChanged( ed, 1 );

    return 0;
    }

int tedInsertSection(	EditApplication *	ea,
			int			after )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    BufferItem *		paraBi;
    BufferItem *		sectBi;
    BufferItem *		newSectBi;
    BufferItem *		newParaBi;
    int				paraNr;

    int				textAttributeNumber;
    BufferItem *		parentBi;
    int				pos;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpRef;

    const int			sectShift= 1;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    if  ( after )
	{
	paraBi= ds.dsEnd.dpBi;
	if  ( ! paraBi || paraBi->biInExternalItem != DOCinBODY )
	    { XDEB(paraBi); return -1;	}

	sectBi= paraBi;
	while( sectBi->biLevel != DOClevSECT )
	    { sectBi= sectBi->biParent;	}
	if  ( ! sectBi )
	    { XDEB(sectBi); return -1;	}

	if  ( docLastPosition( &dpRef, sectBi ) )
	    { LDEB(1); docInitDocumentPosition( &dpRef );	}

	pos= sectBi->biNumberInParent+ 1;
	textAttributeNumber= paraBi->biParaParticules[
			paraBi->biParaParticuleCount-1].tpTextAttributeNumber;

	paraNr= docNumberOfParagraph( dpRef.dpBi )+ 1;
	}
    else{
	paraBi= ds.dsBegin.dpBi;
	if  ( ! paraBi || paraBi->biInExternalItem != DOCinBODY )
	    { XDEB(paraBi); return -1;	}

	sectBi= paraBi;
	while( sectBi->biLevel != DOClevSECT )
	    { sectBi= sectBi->biParent;	}
	if  ( ! sectBi )
	    { XDEB(sectBi); return -1;	}

	if  ( docFirstPosition( &dpRef, sectBi ) )
	    { LDEB(1); docInitDocumentPosition( &dpRef );	}

	pos= sectBi->biNumberInParent;
	textAttributeNumber= paraBi->biParaParticules[0].tpTextAttributeNumber;

	paraNr= docNumberOfParagraph( dpRef.dpBi );
	}

    tedEditIncludeItemInRedraw( &eo, ed, sectBi );

    parentBi= sectBi->biParent;

    newSectBi= docInsertItem( bd, parentBi, pos, DOClevSECT );
    if  ( ! newSectBi )
	{ XDEB(newSectBi); return -1;	}
    eo.eoSectionsAdded++;

    if  ( docCopySectionProperties( &(newSectBi->biSectProperties),
						&(sectBi->biSectProperties) ) )
	{ LDEB(1);	}

    if  ( tedSectionParagraph( &eo, &newParaBi, newSectBi, sectShift,
			&(paraBi->biParaProperties), textAttributeNumber ) )
	{ LDEB(1); return -1;	}

    docEditIncludeItemInReformatRange( &eo, sectBi );
    docEditIncludeItemInReformatRange( &eo, newSectBi );

    tedEditFinishIBarSelection( ed, &eo, newParaBi, 0 );

    appDocumentChanged( ed, 1 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a (line/page) break particule in the document.		*/
/*									*/
/*  TABs could have been inserted with this routine, but against	*/
/*  flashing, a more subtle approach is used for TABs.			*/
/*									*/
/************************************************************************/

static void tedEditInsertSpecialParticule(	EditDocument *	ed,
						int		kind,
						int		redoLayout )
    {
    TextParticule *		tp;

    int				stroffShift;
    int				partShift;

    DocumentPosition *		dp;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		dsRep;

    const int			lastLine= 1;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );
    dp= &(ds.dsBegin);

    if  ( tedEditReplaceSelection( &eo, &dsRep, ed,
					    (const unsigned char *)0, 0 ) )
	{ return;	}

    dp= &(dsRep.dsBegin);

    tp= docParaSpecialParticule( &eo, eo.eoInsParaBi, kind,
				    eo.eoInsParticule, eo.eoInsStroff,
				    &partShift, &stroffShift );
    if  ( ! tp )
	{ LXDEB(eo.eoInsParticule,tp); return;	}

    if  ( docCheckNoBreakAsLast( &eo, eo.eoInsParaBi ) )
	{ LDEB(1);	}

    /*  3,4,5  */
    if  ( redoLayout )
	{ docEditIncludeItemInReformatRange( &eo, eo.eoInsParaBi );	}
    else{ docExtendParagraphAdjust( &eo, eo.eoInsParaBi, stroffShift );	}

    /*  6,7  */
    tedEditFinishEndOfInsert( ed, &eo, lastLine );

    appDocumentChanged( ed, 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a line/page break in the document.				*/
/*									*/
/************************************************************************/

void tedDocInsertLineBreak(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs )
    {
    EditDocument *	ed= (EditDocument *)voided;
    const int		redoLayout= 1;

    tedEditInsertSpecialParticule( ed, DOCkindLINEBREAK, redoLayout );

    return;
    }

void tedDocInsertPageBreak(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs )
    {
    EditDocument *	ed= (EditDocument *)voided;
    const int		redoLayout= 1;

    tedEditInsertSpecialParticule( ed, DOCkindPAGEBREAK, redoLayout );

    return;
    }

void tedDocInsertChftnsep(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs )
    {
    EditDocument *	ed= (EditDocument *)voided;
    const int		redoLayout= 0;

    tedEditInsertSpecialParticule( ed, DOCkindCHFTNSEP, redoLayout );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a 'Tab'.							*/
/*									*/
/************************************************************************/

void tedEditReplaceSelectionWithTab( EditDocument *	ed )
    {
    const int		redoLayout= 0;

    tedEditInsertSpecialParticule( ed, DOCkindTAB, redoLayout );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a section break in the document.				*/
/*									*/
/************************************************************************/

void tedDocInsertSectBreak(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    BufferItem *		newBi;

    const int			onNewPage= 0;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpSplit;
    const int			sectShift= 1;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    dpSplit= ds.dsBegin;

    if  ( dpSplit.dpBi->biParaStrlen > 0			&&
	  dpSplit.dpStroff == dpSplit.dpBi->biParaStrlen	)
	{ docNextPosition( &dpSplit );	}

    if  ( dpSplit.dpStroff > 0 )
	{
	if  ( tedSplitParaContents( &eo, &newBi, &dpSplit,
						ed, DOClevSECT, onNewPage ) )
	    { LDEB(1); return;	}

	eo.eoSectionsAdded++;
	}
    else{
	BufferItem *		insBi;
	BufferItem *		aftBi;

	DocumentPosition	dpNew;

	int			paraNr= -1;

	int			n= dpSplit.dpBi->biNumberInParent;

	paraNr= docNumberOfParagraph( dpSplit.dpBi );

	if  ( docSplitGroupItem( bd, &insBi, &aftBi, dpSplit.dpBi->biParent,
							    n, DOClevSECT ) )
	    { LDEB(1); return;	}

	eo.eoSectionsAdded++;

	if  ( docFirstPosition( &dpNew, insBi ) )
	    {
	    int		textAttributeNumber;

	    textAttributeNumber=
		    dpSplit.dpBi->biParaParticules[0].tpTextAttributeNumber;

	    if  ( tedSectionParagraph( &eo, &newBi, insBi, sectShift,
					    &(dpSplit.dpBi->biParaProperties),
					    textAttributeNumber ) )
		{ LDEB(1); return;	}
	    }
	else{ newBi= dpNew.dpBi;	}

	if  ( aftBi )
	    {
	    if  ( ! docFirstPosition( &dpNew, aftBi ) )
		{ newBi= dpNew.dpBi;	}
	    }

	if  ( aftBi && aftBi->biParent )
	    { docEditIncludeItemInReformatRange( &eo, aftBi->biParent ); }
	else{ XDEB(aftBi);	}
	}

    /*  7,8  */
    tedEditFinishIBarSelection( ed, &eo, newBi, 0 );

    appDocumentChanged( ed, 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  Replace the current selection in a document with a bitmap image.	*/
/*									*/
/************************************************************************/

int tedReplaceSelectionWithBitmapImage(	EditDocument *		ed,
					AppBitmapImage *	abi )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    InsertedObject *		io;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition *		dp;

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return -1;	}
    dp= &(ds.dsBegin);

    io= tedObjectMakeBitmapObject( ed, dp->dpBi, abi );
    if  ( ! io )
	{ XDEB(io); return -1; }

    if  ( tedReplaceSelectionWithObject( ed, io ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

