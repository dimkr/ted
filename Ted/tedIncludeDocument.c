/************************************************************************/
/*									*/
/*  Replace the selection in a document with another document.		*/
/*  ( Used with 'paste', 'insert file', 'undo', 'redo'. )		*/
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
/*  Replace the selection in a document with another document.		*/
/*  ( Used with 'paste', 'insert file', 'undo', 'redo'. )		*/
/*									*/
/*  B)  The first particule of the line was split, probably, part fits	*/
/*	on the previous line. Reformat from the previous particule.	*/
/*	If paragraphs were merged, redraw the whole neighbourhood.	*/
/*									*/
/*  1)  Replace the selection of the target with the text of those	*/
/*	particules at the beginning of the source that have the same	*/
/*	attributes.							*/
/*  2)  Insert the rest of the first paragraph of the source into the	*/
/*	target.								*/
/*  4)  If the insertion consists of more than one paragraph, split the	*/
/*	target paragraph.						*/
/*  5)  Insert the last particule of the insertion as text.		*/
/*									*/
/*  z)  Copy all paragraphs between the first and last (exclusive) of	*/
/*	the source to the target.					*/
/*									*/
/************************************************************************/

static int tedIncludeParagraphs(	EditOperation *		eo,
					DocumentCopyJob *	dcj,
					int *			pEndedInTable,
					int			startWithTable,
					BufferItem *		biFrom,
					DocumentPosition *	dpEndFrom )
    {
    BufferItem *		rowBi= (BufferItem *)0;
    int				inTable= 0;

    BufferDocument *		bdTo= dcj->dcjBdTo;

    if  ( ! startWithTable )
	{
	biFrom= docNextParagraph( biFrom );
	if  ( ! biFrom )
	    { XDEB(biFrom); return -1;	}

	if  ( biFrom == dpEndFrom->dpBi )
	    { return 0; }
	}

    for (;;)
	{
	BufferItem *	insBi;
	BufferItem *	aftBi;

	if  ( biFrom->biParaInTable )
	    {
	    DocumentPosition	dp;

	    BufferItem *	paraBiTo= eo->eoInsParaBi;
	    BufferItem *	cellBiTo= paraBiTo->biParent;
	    BufferItem *	rowBiTo= cellBiTo->biParent;
	    BufferItem *	rowBiFrom= biFrom->biParent->biParent;

	    inTable= 1;

	    if  ( paraBiTo->biNumberInParent != cellBiTo->biChildCount -1 )
		{
		if  ( docSplitGroupItem( bdTo, &insBi, &aftBi,
				cellBiTo, paraBiTo->biNumberInParent+ 1,
				DOClevCELL ) )
		    { LDEB(1); return -1;	}

		cellBiTo= paraBiTo->biParent;
		}

	    if  ( cellBiTo->biNumberInParent == rowBiTo->biChildCount -1 )
		{
		docEditIncludeItemInReformatRange( eo, cellBiTo );
		docEditIncludeItemInReformatRange( eo, insBi );
		}
	    else{
		if  ( docSplitGroupItem( bdTo, &insBi, &aftBi,
			    rowBiTo, cellBiTo->biNumberInParent+ 1,
			    DOClevROW ) )
		    { LDEB(1); return -1;	}

		docEditIncludeItemInReformatRange( eo, rowBiTo );
		docEditIncludeItemInReformatRange( eo, insBi );

		rowBiTo= insBi;
		}

	    rowBi= docCopyRowItem( dcj, eo, &(eo->eoSelectionScope),
			rowBiTo->biParent, rowBiTo->biNumberInParent+ 1,
			rowBiFrom, biFrom->biParaInTable );
	    if  ( ! rowBi )
		{ XDEB(rowBi); return -1;	}

	    docEditIncludeItemInReformatRange( eo, rowBi );

	    if  ( docLastPosition( &dp, rowBiFrom ) )
		{ LDEB(1); break;	}

	    if  ( biFrom == dpEndFrom->dpBi )
		{ break;	}

	    biFrom= docNextParagraph( dp.dpBi );
	    if  ( ! biFrom )
		{ break;	}

	    if  ( biFrom == dpEndFrom->dpBi && ! biFrom->biParaInTable )
		{ break;	}

	    if  ( docLastPosition( &dp, rowBi ) )
		{ LDEB(1); break;	}

	    eo->eoInsParaBi= dp.dpBi;
	    eo->eoInsParticule= eo->eoInsParaBi->biParaParticuleCount- 1;
	    eo->eoInsAtPartHead= 0;
	    eo->eoInsStroff= dp.dpStroff;
	    }
	else{
	    inTable= 0;

	    if  ( rowBi )
		{
		BufferItem *	cellBi;
		BufferItem *	childBi;

		insBi= docInsertItem( bdTo, rowBi->biParent,
				    rowBi->biNumberInParent+ 1, DOClevROW );
		if  ( ! insBi )
		    { XDEB(insBi); return -1;	}

		cellBi= docInsertItem( bdTo, insBi, 0, DOClevCELL );
		if  ( ! cellBi )
		    { XDEB(cellBi); return -1;	}

		childBi= docCopyParaItem( dcj, eo, &(eo->eoSelectionScope),
						cellBi, 0, biFrom, inTable );

		if  ( ! childBi )
		    { XDEB(childBi); return -1;	}

		eo->eoInsParaBi= childBi;
		eo->eoInsParticule= childBi->biParaParticuleCount- 1;
		eo->eoInsAtPartHead= 0;
		eo->eoInsStroff= childBi->biParaStrlen;
		}
	    else{
		insBi= docCopyParaItem( dcj, eo, &(eo->eoSelectionScope),
					eo->eoInsParaBi->biParent,
					eo->eoInsParaBi->biNumberInParent+ 1,
					biFrom, inTable );

		if  ( eo->eoInsParaBi->biParaInTable )
		    { insBi->biParaInTable= eo->eoInsParaBi->biParaInTable; }

		if  ( ! insBi )
		    { XDEB(insBi); return -1;	}

		eo->eoInsParaBi= insBi;
		eo->eoInsParticule= insBi->biParaParticuleCount- 1;
		eo->eoInsAtPartHead= 0;
		eo->eoInsStroff= insBi->biParaStrlen;
		}

	    docEditIncludeItemInReformatRange( eo, insBi );

	    biFrom= docNextParagraph( biFrom );
	    if  ( ! biFrom )
		{ XDEB(biFrom); return -1;	}

	    if  ( biFrom == dpEndFrom->dpBi )
		{ break;	}

	    rowBi= (BufferItem *)0;
	    }
	}

    eo->eoSelectedRange.erEnd.epParaNr= docNumberOfParagraph( eo->eoInsParaBi );
    eo->eoSelectedRange.erEnd.epStroff= eo->eoInsParaBi->biParaStrlen;

#   if SHOW_SELECTION_RANGE
    appDebug( "PARA   .. %3d(%3d) .. %3d(%3d)\n",
			    eo->eoSelectedRange.erStart.epParaNr,
			    eo->eoSelectedRange.erStart.epStroff,
			    eo->eoSelectedRange.erEnd.epParaNr,
			    eo->eoSelectedRange.erEnd.epStroff );
#   endif

    *pEndedInTable= inTable; return 0;
    }

/************************************************************************/
/*									*/
/*  Include the tail of a paragraph in another paragraph.		*/
/*									*/
/*  It is known by the caller that at the beginning if the series of	*/
/*  particules that is to be included, no particule merging is		*/
/*  possible/necessary.							*/
/*									*/
/*  1)  Inclusion at the end of the paragraph... No particule merging.	*/
/*  2)  If the last particule of the source, and the subsequent		*/
/*	particule of the target have the same attributes, let the	*/
/*	text replacement routine decide whether to merge them or not.	*/
/*  3)  Copy those particules that are to be copied 'as is.'		*/
/*  4)  Insert the last particule as text if this is desirable.		*/
/*									*/
/************************************************************************/

static int tedIncludeTail(	EditOperation *		eo,
				DocumentCopyJob *	dcj,
				const BufferItem *	biFrom,
				int			partFrom,
				int *			pStroffShift )
    {
    int			copyLastAsText;
    int			particulesCopied;

    TextParticule *	tpTo;
    TextParticule *	tpFrom;

    int			particulesInserted= 0;
    int			charactersCopied= 0;

    BufferItem *	biTo= eo->eoInsParaBi;
    int			paraNrTo= docNumberOfParagraph( biTo );
    int			partTo= eo->eoInsParticule;

    int			stroffFrom;

    if  ( ! eo->eoInsAtPartHead )
	{ partTo++;	}

    /*  1  */
    if  ( biTo->biParaStrlen == 0		||
	  partTo == biTo->biParaParticuleCount	)
	{ stroffFrom= biTo->biParaStrlen; copyLastAsText= 0;	}
    else{
	int		attributeNumberFrom;

	tpTo= biTo->biParaParticules+ partTo;
	tpFrom= biFrom->biParaParticules+ biFrom->biParaParticuleCount- 1;

	attributeNumberFrom= docMapTextAttributeNumber( dcj,
					    tpFrom->tpTextAttributeNumber );

	stroffFrom= tpTo->tpStroff;

	/*  2  */
	if  ( tpFrom->tpKind == DOCkindTEXT				&&
	      tpTo->tpKind == DOCkindTEXT				&&
	      attributeNumberFrom == tpTo->tpTextAttributeNumber	)
	    { copyLastAsText= 1;	}
	else{ copyLastAsText= 0;	}
	}

    /*  3  */
    particulesCopied= biFrom->biParaParticuleCount- partFrom- copyLastAsText;
    if  ( particulesCopied > 0 )
	{
	if  ( docCopyParticules( dcj, eo,
				    biTo, biFrom, partTo, partFrom,
				    particulesCopied,
				    &particulesInserted,
				    &charactersCopied ) )
	    { LDEB(1); return -1;	}

	eo->eoInsParticule += particulesCopied;
	}
    else{
	if  ( particulesCopied < 0 )
	    { LDEB(particulesCopied); particulesCopied= 0;	}
	}

    /*  4  */
    if  ( copyLastAsText > 0 )
	{
	int	textAttributeNumberFrom;

	tpTo= biTo->biParaParticules+ partTo+ particulesCopied;

	tpFrom= biFrom->biParaParticules+
				biFrom->biParaParticuleCount- copyLastAsText;

	textAttributeNumberFrom= docMapTextAttributeNumber( dcj,
					    tpFrom->tpTextAttributeNumber );
	if  ( textAttributeNumberFrom < 0 )
	    { LDEB(textAttributeNumberFrom); return -1;	}

	if  ( docParaReplaceText( eo, biTo, paraNrTo, tpTo->tpStroff,
		    &particulesInserted, &charactersCopied,
		    tpTo->tpStroff, biFrom->biParaString+ tpFrom->tpStroff,
		    tpFrom->tpStrlen, textAttributeNumberFrom ) )
	    { LDEB(1); return -1;	}

	/* NO! done by docParaReplaceText()
	eo->eoInsParticule += particulesInserted;
	*/
	}

    eo->eoSelectedRange.erEnd.epParaNr= paraNrTo;
    eo->eoSelectedRange.erEnd.epStroff= stroffFrom+ charactersCopied;

#   if SHOW_SELECTION_RANGE
    appDebug( "TAIL   .. %3d(%3d) .. %3d(%3d)\n",
			    eo->eoSelectedRange.erStart.epParaNr,
			    eo->eoSelectedRange.erStart.epStroff,
			    eo->eoSelectedRange.erEnd.epParaNr,
			    eo->eoSelectedRange.erEnd.epStroff );
#   endif

    *pStroffShift += charactersCopied;

    return 0;
    }

static int tedIncludeHead(
			EditOperation *			eo,
			DocumentCopyJob *		dcj,
			const DocumentSelection *	dsTo,
			const BufferItem *		biFrom,
			int *				pStroffShift )
    {
    BufferDocument *		bdFrom= dcj->dcjBdFrom;
    int				partFrom= 0;
    int				stroffFrom= 0;
    int				stroffUpto= 0;

    int				partShift= 0;
    int				partsCopied;
    int				particuleSplit= 0;

    int				includeHeadAsString= 0;
    int				attributeNumberTo;
    int				attributeNumberFrom;

    int				stroffShift= 0;

    {
    TextParticule * tpTo= eo->eoInsParaBi->biParaParticules+ eo->eoInsParticule;
    attributeNumberTo= tpTo->tpTextAttributeNumber;

    if  ( tpTo->tpKind == DOCkindTEXT )
	{ includeHeadAsString= 1;	}
    }

    attributeNumberFrom= biFrom->biParaParticules[0].tpTextAttributeNumber;

    if  ( biFrom->biParaListOverride > 0 )
	{
	int		bulletFieldNr= -1;
	int		bulletPartBegin= -1;
	int		bulletPartEnd= -1;
	int		bulletStroffBegin= -1;
	int		bulletStroffEnd= -1;

	if  ( ! docDelimitParaHeadField( &bulletFieldNr,
				    &bulletPartBegin, &bulletPartEnd,
				    &bulletStroffBegin, &bulletStroffEnd,
				    biFrom, bdFrom ) )
	    {
	    if  ( partFrom <= bulletPartEnd )
		{ partFrom= bulletPartEnd+ 1;	}
	    if  ( stroffFrom < bulletStroffEnd )
		{ stroffFrom= bulletStroffEnd;	}
	    }
	}

    if  ( eo->eoInsParaBi->biParaStrlen == 0 )
	{
	const int	stroffHead= 0;
	int		toInTable= eo->eoInsParaBi->biParaInTable;

	int		paraNrTo= docNumberOfParagraph( eo->eoInsParaBi );

	PropertyMask	ppChgMask;
	PropertyMask	ppUpdMask;

	{
	TextParticule *	tpTo;

	tpTo= eo->eoInsParaBi->biParaParticules+ eo->eoInsParticule;

	tpTo->tpTextAttributeNumber= docMapTextAttributeNumber( dcj,
							attributeNumberFrom );
	}

	PROPmaskCLEAR( &ppChgMask );

	PROPmaskCLEAR( &ppUpdMask );
	PROPmaskADD( &ppUpdMask, PPpropLEFT_INDENT );
	PROPmaskADD( &ppUpdMask, PPpropFIRST_INDENT );
	PROPmaskADD( &ppUpdMask, PPpropRIGHT_INDENT );
	PROPmaskADD( &ppUpdMask, PPpropALIGNMENT );
	PROPmaskADD( &ppUpdMask, PPpropTAB_STOPS );

	PROPmaskADD( &ppUpdMask, PPpropLISTOVERRIDE );
	PROPmaskADD( &ppUpdMask, PPpropLISTLEVEL );

	if  ( docEditUpdParaProperties( eo, &ppChgMask, eo->eoInsParaBi,
				    &ppUpdMask, &(biFrom->biParaProperties),
				    dcj->dcjColorMap, dcj->dcjListStyleMap ) )
	    { LDEB(1); }

	eo->eoInsParaBi->biParaInTable= toInTable;

	eo->eoInsParticule= eo->eoInsParaBi->biParaParticuleCount;
	eo->eoInsStroff= eo->eoInsParaBi->biParaStrlen;
	stroffShift= eo->eoInsStroff;
	includeHeadAsString= 1;

	docAdjustEditPositionOffsetB( &(eo->eoSelectedRange.erStart),
					paraNrTo, stroffHead, stroffShift );
	docAdjustEditPositionOffsetE( &(eo->eoSelectedRange.erEnd),
					paraNrTo, stroffHead, stroffShift );

#	if SHOW_SELECTION_RANGE
	appDebug( "HEAD0  .. %3d(%3d) .. %3d(%3d)\n",
			    eo->eoSelectedRange.erStart.epParaNr,
			    eo->eoSelectedRange.erStart.epStroff,
			    eo->eoSelectedRange.erEnd.epParaNr,
			    eo->eoSelectedRange.erEnd.epStroff );
#	endif
	}

    partsCopied= 0;
    if  ( includeHeadAsString )
	{
	const TextParticule *	tpFrom= biFrom->biParaParticules+ partFrom;
	/*  1  */
	stroffUpto= stroffFrom;
	attributeNumberFrom= -1;
	while( partFrom+ partsCopied < biFrom->biParaParticuleCount	&&
	       tpFrom->tpKind == DOCkindTEXT				)
	    {
	    attributeNumberFrom= docMapTextAttributeNumber( dcj,
						tpFrom->tpTextAttributeNumber );

	    if  ( attributeNumberFrom != attributeNumberTo )
		{ break;	}

	    stroffUpto= tpFrom->tpStroff+ tpFrom->tpStrlen;
	    partsCopied++; tpFrom++;
	    }
	}

    if  ( partsCopied > 0 )
	{
	int		charactersCopied;

	charactersCopied= stroffUpto- stroffFrom;

	if  ( attributeNumberTo < 0 )
	    { LDEB(attributeNumberTo); return -1;	}

	if  ( docReplaceSelection( eo, dsTo, &partShift, pStroffShift,
		    biFrom->biParaString+ stroffFrom, charactersCopied,
		    attributeNumberTo ) )
	    { LLDEB(stroffFrom,stroffUpto); return -1;	}
	}
    else{
	if  ( ! eo->eoIBarSelectionOld					&&
	      docReplaceSelection( eo, dsTo, &partShift, pStroffShift,
			(unsigned char *)0, 0, attributeNumberTo )	)
	    { LDEB(0); return -1;	}
	}

    if  ( eo->eoInsStroff >= eo->eoInsParaBi->biParaStrlen )
	{
	eo->eoInsParticule= eo->eoInsParaBi->biParaParticuleCount- 1;
	eo->eoInsAtPartHead= 0;
	}
    else{
	TextParticule * tpTo= eo->eoInsParaBi->biParaParticules+ eo->eoInsParticule;
	if  ( ! eo->eoInsAtPartHead				&&
	      tpTo->tpStroff+ tpTo->tpStrlen != eo->eoInsStroff	)
	    { particuleSplit= 1;	}

	if  ( partFrom+ partsCopied < biFrom->biParaParticuleCount	&&
	      particuleSplit						)
	    {
	    TextParticule *	tpOld;

	    if  ( docSplitTextParticule( &tpOld, &tpTo, eo->eoInsParaBi,
					eo->eoInsParticule, eo->eoInsStroff ) )
		{ LDEB(eo->eoInsParticule); return -1;	}

	    partShift++;
	    eo->eoInsParticule++;
	    eo->eoInsAtPartHead= 1;
	    }
	}

    if  ( partFrom+ partsCopied < biFrom->biParaParticuleCount )
	{
	if  ( tedIncludeTail( eo, dcj, biFrom, partFrom+ partsCopied,
							    pStroffShift ) )
	    { LLDEB(eo->eoInsParticule,partFrom); return -1;	}
	}

    return 0;
    }

static int tedIncludeDocument(	EditDocument *			edTo,
				EditOperation *			eo,
				DocumentCopyJob *		dcj,
				const DocumentSelection *	dsTo,
				const SelectionGeometry *	sgTo )
    {
    BufferDocument *		bdFrom= dcj->dcjBdFrom;

    int				stroffShift= 0;
    int				endedInTable= 0;

    DocumentPosition		dpBeginFrom;
    DocumentPosition		dpEndFrom;

    BufferItem *		biFrom;

    int				line;
    int				stroff;

    const int			lastLine= 0;

#   if SHOW_SELECTION_RANGE
    appDebug( "INCLUDE.. %3d(%3d) .. %3d(%3d)\n",
			    eo->eoSelectedRange.erStart.epParaNr,
			    eo->eoSelectedRange.erStart.epStroff,
			    eo->eoSelectedRange.erEnd.epParaNr,
			    eo->eoSelectedRange.erEnd.epStroff );
#   endif

    if  ( docFirstPosition( &dpBeginFrom, &(bdFrom->bdItem) ) )
	{ LDEB(1); return -1;	}
    if  ( docLastPosition( &dpEndFrom, &(bdFrom->bdItem) ) )
	{ LDEB(1); return -1;	}

    biFrom= dpBeginFrom.dpBi;

    stroff= dsTo->dsBegin.dpStroff;
    line= sgTo->sgBegin.pgLine;

    if  ( dpBeginFrom.dpBi != dpEndFrom.dpBi	||
	  bdFrom->bdFieldList.dflFieldCount > 0	)
	{
	int		inField;

	inField= docPositionInField( &(eo->eoInsParticule),
						&(dsTo->dsBegin), eo->eoBd );

	if  ( eo->eoInsStroff ==
		eo->eoInsParaBi->biParaParticules[eo->eoInsParticule].tpStroff )
	    { eo->eoInsAtPartHead= 1;	}
	else{ eo->eoInsAtPartHead= 0;	}

	if  ( inField )
	    { LDEB(inField); return -1;	}
	}
	  
    if  ( ! biFrom->biParaInTable )
	{
	tedAdjustRedrawBegin( edTo, eo, &line );

	if  ( tedIncludeHead( eo, dcj, dsTo, biFrom, &stroffShift ) )
	    { LDEB(1); return -1;	}
	}

    if  ( dpEndFrom.dpBi != biFrom )
	{
	BufferItem *	newBi;

	const int		particulesCopied= 0;
	TextParticule *		tpFrom;

	/*  4  */
	if  ( eo->eoInsAtPartHead )
	    {
	    if  ( docSplitParaItemAtPart( eo, &newBi,
			    eo->eoInsParaBi, eo->eoInsParticule, DOClevPARA ) )
		{ LDEB(eo->eoInsParticule); return -1; }
	    }
	else{
	    DocumentPosition	dpHere;

	    dpHere.dpBi= eo->eoInsParaBi;
	    dpHere.dpStroff= eo->eoSelectedRange.erEnd.epStroff;

	    if  ( docSplitParaItemAtStroff( eo, &newBi, &dpHere, DOClevPARA ) )
		{ LDEB(dpHere.dpStroff); return -1; }
	    }

	/*  C  */
	docEditIncludeItemInReformatRange( eo, eo->eoInsParaBi );

	if  ( tedIncludeParagraphs( eo, dcj, &endedInTable,
						    biFrom->biParaInTable,
						    biFrom, &dpEndFrom ) )
	    { LDEB(1); return -1;	}

	eo->eoInsParaBi= newBi;
	eo->eoInsStroff= 0;
	eo->eoInsParticule= 0;
	eo->eoInsAtPartHead= 1;

	if  ( eo->eoInsParaBi->biParaListOverride > 0 )
	    {
	    int		bulletFieldNr= -1;
	    int		bulletPartBegin= -1;
	    int		bulletPartEnd= -1;
	    int		bulletStroffBegin= -1;
	    int		bulletStroffEnd= -1;

	    if  ( docDelimitParaHeadField( &bulletFieldNr,
				    &bulletPartBegin, &bulletPartEnd,
				    &bulletStroffBegin, &bulletStroffEnd,
				    eo->eoInsParaBi, dcj->dcjBdTo ) )
		{ LDEB(1);	}

	    if  ( eo->eoInsParticule <= bulletPartEnd )
		{ eo->eoInsParticule= bulletPartEnd+ 1;	}
	    if  ( eo->eoInsStroff < bulletStroffEnd )
		{ eo->eoInsStroff= bulletStroffEnd;	}
	    }

	stroffShift= 0;

	biFrom= dpEndFrom.dpBi;
	tpFrom= biFrom->biParaParticules;

	if  ( ! endedInTable && biFrom->biParaStrlen > 0 )
	    {
	    int			partFrom= particulesCopied;
	    int			wasEmpty= newBi->biParaStrlen == 0;

	    PropertyMask	ppChgMask;
	    PropertyMask	ppUpdMask;

	    if  ( biFrom->biParaListOverride > 0 )
		{
		int		bulletFieldNr= -1;
		int		bulletPartBegin= -1;
		int		bulletPartEnd= -1;
		int		bulletStroffBegin= -1;
		int		bulletStroffEnd= -1;

		if  ( docDelimitParaHeadField( &bulletFieldNr,
					&bulletPartBegin, &bulletPartEnd,
					&bulletStroffBegin, &bulletStroffEnd,
					biFrom, bdFrom ) )
		    { LDEB(1);	}

		if  ( partFrom <= bulletPartEnd )
		    { partFrom= bulletPartEnd+ 1;	}
		}

	    if  ( tedIncludeTail( eo, dcj, biFrom, partFrom, &stroffShift ) )
		{ LDEB(partFrom); return -1;	}

	    PROPmaskCLEAR( &ppChgMask );

	    PROPmaskCLEAR( &ppUpdMask );
	    utilPropMaskFill( &ppUpdMask, PPprop_COUNT );
	    PROPmaskUNSET( &ppUpdMask, PPpropIN_TABLE );

	    if  ( wasEmpty &&
		  docEditUpdParaProperties( eo, &ppChgMask, newBi,
				    &ppUpdMask, &(biFrom->biParaProperties),
				    dcj->dcjColorMap, dcj->dcjListStyleMap ) )
		{ LDEB(wasEmpty);	}
	    }

	docEditIncludeItemInReformatRange( eo, newBi );
	}
    else{
	int	stroffEnd= eo->eoSelectedRange.erEnd.epStroff;

	/*  C  */
	docSetParagraphAdjust( eo, eo->eoInsParaBi,
					    line, stroffShift, stroffEnd );
	}

    eo->eoNotesAdded += dcj->dcjNotesCopied;
    eo->eoBulletsChanged += dcj->dcjBulletsCopied;

    if  ( tedEditFinishEndOfInsert( edTo, eo, lastLine ) )
	{ LDEB(1);	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Include a plain text document into the current document.		*/
/*									*/
/*  This is the implementation of 'Include File', but also of a paste	*/
/*  E.G. from a program that does not support pasting RTF such as	*/
/*  Xterm.								*/
/*									*/
/*  1)  Determine the text attributes of the target location.		*/
/*  2)  Merge the font of the current position n the TARGER document	*/
/*	into the font list of the SOURCE document. This sounds to be	*/
/*	the wrong direction, but it makes subsequent steps a lot	*/
/*	easier.								*/
/*  3)  Determine the attribute number in the source document of the	*/
/*	text attribute at the target position. This is possible as we	*/
/*	have just inserted the font into the source document.		*/
/*  4)  Patch the text attributes of the whole body of the source	*/
/*	document to those of the target position. The preparations	*/
/*	above have made it possible to represent them in the source	*/
/*	document.							*/
/*  5)  Make a mapping of the fonts in the source document to those in	*/
/*	the target document. [There is only one as we gave the whole	*/
/*	body the same text attribute.]					*/
/*  6)  Allocate memory to keep an administration of the fields that	*/
/*	have been copied from one document to the other.		*/
/*  7)  Actually include the source in the target.			*/
/*									*/
/************************************************************************/

int tedIncludePlainDocument(	EditDocument *		ed,
				BufferDocument *	bdFrom )
    {
    EditApplication *		ea= ed->edApplication;
    const PostScriptFontList *	psfl= &(ea->eaPostScriptFontList);

    TedDocument *		tdTo= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bdTo= tdTo->tdDocument;

    DocumentPosition		dpBeginTo;
    DocumentPosition		dpBeginFrom;
    BufferItem *		paraBi;

    int				textAttributeNumber;
    TextAttribute		ta;

    int				rval;

    DocumentCopyJob		dcj;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    docInitDocumentCopyJob( &dcj );

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    dpBeginTo= ds.dsBegin;
    if  ( ! dpBeginTo.dpBi				||
	  dpBeginTo.dpBi->biParaParticuleCount == 0	)
	{ LDEB(1); rval= -1; goto ready;	}

    /*  1  */
    textAttributeNumber= dpBeginTo.dpBi->biParaParticules[
				eo.eoInsParticule].tpTextAttributeNumber;

    utilGetTextAttributeByNumber( &ta, &(bdTo->bdTextAttributeList),
						    textAttributeNumber );

    /*  2  */
    {
    DocumentProperties *	dpFrom= &(bdFrom->bdProperties);
    DocumentFontList *		dflFrom= &(dpFrom->dpFontList);

    DocumentProperties *	dpTo= &(bdTo->bdProperties);
    DocumentFontList *		dflTo= &(dpTo->dpFontList);

    DocumentFont *		dfTo;

    int				to;

    dfTo= dflTo->dflFonts+ ta.taFontNumber;
    to= docMergeFontIntoFontlist( dflFrom, dfTo );
    if  ( to < 0 )
	{ LDEB(to); rval= -1; goto ready;	}
    ta.taFontNumber= to;

    if  ( ta.taTextColorNumber > 0 )
	{
	RGB8Color *	rgb8= dpTo->dpColors+ ta.taTextColorNumber;

	to= docAllocateDocumentColor( dpFrom, rgb8 );
	if  ( to < 0 )
	    { LDEB(to); rval= -1; goto ready;	}
	ta.taTextColorNumber= to;
	}
    }

    /*  3  */
    textAttributeNumber= utilTextAttributeNumber(
					&(bdFrom->bdTextAttributeList), &ta );
    if  ( textAttributeNumber < 0 )
	{ LDEB(textAttributeNumber); rval= -1; goto ready;	}

    /*  4  */
    if  ( docFirstPosition( &dpBeginFrom, &(bdFrom->bdItem) ) )
	{ LDEB(1); rval= -1; goto ready;	}
    paraBi= dpBeginFrom.dpBi;

    for (;;)
	{
	TextParticule *		tp;
	int			part;

	tp= paraBi->biParaParticules;

	for ( part= 0; part < paraBi->biParaParticuleCount; part++, tp++ )
	    { tp->tpTextAttributeNumber= textAttributeNumber; }

	paraBi= docNextParagraph( paraBi );
	if  ( ! paraBi )
	    { break;	}
	}

    /*  5,6  */
    if  ( docSet2DocumentCopyJob( &dcj, bdTo, bdFrom, psfl, ed->edFilename ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  7  */
    rval= tedIncludeDocument( ed, &eo, &dcj, &ds, &sg );
    if  ( rval )
	{ LDEB(rval);	}

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

int tedIncludeRtfDocument(	EditDocument *		ed,
				BufferDocument *	bdFrom )
    {
    EditApplication *		ea= ed->edApplication;
    const PostScriptFontList *	psfl= &(ea->eaPostScriptFontList);

    int				rval= 0;

    TedDocument *		tdTo= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bdTo= tdTo->tdDocument;

    DocumentCopyJob		dcj;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    docInitDocumentCopyJob( &dcj );

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    if  ( bdFrom->bdProperties.dpContainsTables )
	{
	if  ( ds.dsCol0 >= 0 )
	    { goto ready;	}
	if  ( ds.dsBegin.dpBi->biParaInTable )
	    { goto ready;	}
	if  ( ds.dsEnd.dpBi->biParaInTable )
	    { goto ready;	}
	}

    if  ( docSet2DocumentCopyJob( &dcj, bdTo, bdFrom, psfl, ed->edFilename ) )
	{ LDEB(1); rval= -1; goto ready;	}

    rval= tedIncludeDocument( ed, &eo, &dcj, &ds, &sg );
    if  ( rval )
	{ LDEB(rval);	}

  ready:

    docCleanDocumentCopyJob( &dcj );

    return rval;
    }

