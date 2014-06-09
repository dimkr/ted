/************************************************************************/
/*									*/
/*  Get/Move/Set Selections.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Initialise a BufferPosition/BufferSelection.			*/
/*									*/
/************************************************************************/

void docInitSelectionScope(	SelectionScope *	ss )
    {
    ss->ssInExternalItem= DOCinUNKNOWN;
    ss->ssSectNrExternalTo= -1;
    ss->ssNoteArrayIndex= -1;
    }

void docInitDocumentPosition(	DocumentPosition *	dp )
    {
    dp->dpBi= (BufferItem *)0;
    dp->dpStroff= 0;

    return;
    }

void docInitDocumentSelection(	DocumentSelection *	ds )
    {
    docInitSelectionScope( &(ds->dsSelectionScope) );

    docInitDocumentPosition( &(ds->dsBegin) );
    docInitDocumentPosition( &(ds->dsEnd) );
    docInitDocumentPosition( &(ds->dsAnchor) );

    ds->dsCol0= -1;
    ds->dsCol1= -1;

    ds->dsDirection= 0;

    return;
    }

void docSetIBarSelection(	DocumentSelection *		ds,
				const DocumentPosition *	dp )
    {
    docSetSelectionScope( ds, dp->dpBi );

    ds->dsBegin= *dp;
    ds->dsEnd= *dp;
    ds->dsAnchor= *dp;

    ds->dsCol0= -1;
    ds->dsCol1= -1;
    ds->dsDirection= 0;

    return;
    }

void docSetRangeSelection(	DocumentSelection *		ds,
				const DocumentPosition *	dpFrom,
				const DocumentPosition *	dpTo,
				int				direction,
				int				col0,
				int				col1 )
    {
    docInitDocumentSelection( ds );

    docSetSelectionScope( ds, dpFrom->dpBi );

    ds->dsBegin= *dpFrom;
    ds->dsEnd= *dpTo;
    ds->dsDirection= direction;

    ds->dsCol0= col0;
    ds->dsCol1= col1;
    
    if  ( direction >= 0 )
	{ ds->dsAnchor= *dpFrom;	}
    else{ ds->dsAnchor= *dpTo;		}

    return;
    }

int docIsIBarSelection( const DocumentSelection *		ds )
    {
    if  ( ! ds->dsBegin.dpBi )
	{ return 0;	}

    if  ( ds->dsBegin.dpBi != ds->dsEnd.dpBi )
	{ return 0;	}

    if  ( ds->dsBegin.dpStroff != ds->dsEnd.dpStroff )
	{ return 0;	}

    return 1;
    }

int docIsParaSelection( const DocumentSelection *		ds )
    {
    if  ( ! ds->dsBegin.dpBi )
	{ return 0;	}

    if  ( ds->dsBegin.dpBi != ds->dsEnd.dpBi )
	{ return 0;	}

    return 1;
    }

void docInitPositionGeometry(	PositionGeometry *		pg )
    {
    pg->pgLine= 0;
    pg->pgAtLineHead= 0;
    pg->pgAtLineEnd= 0;

    pg->pgXPixels= -1;
    docInitLayoutPosition( &pg->pgTopPosition );
    pg->pgY1Pixels= -1;
    pg->pgBaselinePixels= -1;

    return;
    }

void docInitSelectionGeometry(	SelectionGeometry *		sg )
    {
    docInitRectangle( &(sg->sgRectangle) );

    docInitPositionGeometry( &(sg->sgBegin) );
    docInitPositionGeometry( &(sg->sgEnd) );
    docInitPositionGeometry( &(sg->sgAnchor) );
    }

/************************************************************************/
/*									*/
/*  Reduce a selection to one paragraph.				*/
/*									*/
/************************************************************************/

void docConstrainSelectionToOneParagraph( int *			pBeginMoved,
					int *			pEndMoved,
					DocumentSelection *	ds )
    {
    int		lenBegin;
    int		lenEnd;

    if  ( docIsParaSelection( ds ) )
	{ return;	}

    lenBegin= ds->dsBegin.dpBi->biParaStrlen- ds->dsBegin.dpStroff;
    lenEnd= ds->dsEnd.dpStroff;

    if  ( lenEnd > lenBegin )
	{
	ds->dsBegin.dpBi= ds->dsEnd.dpBi;
	ds->dsBegin.dpStroff= 0;
	ds->dsCol0= ds->dsCol1;

	if  ( ds->dsAnchor.dpBi != ds->dsBegin.dpBi )
	    { ds->dsAnchor= ds->dsBegin;	}

	*pBeginMoved= 1; *pEndMoved= 0; return;
	}
    else{
	ds->dsEnd.dpBi= ds->dsBegin.dpBi;
	ds->dsEnd.dpStroff= ds->dsEnd.dpBi->biParaStrlen;
	ds->dsCol1= ds->dsCol0;

	if  ( ds->dsAnchor.dpBi != ds->dsEnd.dpBi )
	    { ds->dsAnchor= ds->dsEnd;	}

	*pBeginMoved= 0; *pEndMoved= 1; return;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Return the union of two BufferSelections in the same paragraph.	*/
/*									*/
/************************************************************************/

void docUnionParaSelections(		DocumentSelection *		ds,
					const DocumentSelection *	ds1,
					const DocumentSelection *	ds2 )
    {
    DocumentSelection	dsNew;

    if  ( ds1->dsBegin.dpBi !=  ds1->dsEnd.dpBi )
	{ XXDEB(ds1->dsBegin.dpBi,ds1->dsEnd.dpBi); return;	}
    if  ( ds2->dsBegin.dpBi !=  ds2->dsEnd.dpBi )
	{ XXDEB(ds2->dsBegin.dpBi,ds2->dsEnd.dpBi); return;	}
    if  ( ds1->dsBegin.dpBi !=  ds2->dsEnd.dpBi )
	{ XXDEB(ds1->dsBegin.dpBi,ds2->dsEnd.dpBi); return;	}

    dsNew= *ds1;

    if  ( ds2->dsBegin.dpStroff < ds1->dsBegin.dpStroff )
	{ dsNew.dsBegin.dpStroff= ds2->dsBegin.dpStroff;	}
    else{ dsNew.dsBegin.dpStroff= ds1->dsBegin.dpStroff;	}

    /**/

    if  ( ds2->dsEnd.dpStroff > ds1->dsEnd.dpStroff )
	{ dsNew.dsEnd.dpStroff= ds2->dsEnd.dpStroff;	}
    else{ dsNew.dsEnd.dpStroff= ds1->dsEnd.dpStroff;	}

    /**/

    *ds= dsNew; return;
    }

/************************************************************************/
/*									*/
/*  Move to the first/last position.					*/
/*									*/
/************************************************************************/

int docFirstPosition(	DocumentPosition *	dp,
			BufferItem *		bi )
    {
    while( bi )
	{
	if  ( bi->biLevel == DOClevPARA )
	    {
	    dp->dpBi= bi;
	    dp->dpStroff= 0;

	    return 0;
	    }

	if  ( bi->biChildCount == 0 )
	    { /*LDEB(bi->biChildCount);*/ return -1;	}

	bi= bi->biChildren[0];
	}

    /*XDEB(bi);*/ return -1;
    }

int docLastPosition(	DocumentPosition *	dp,
			BufferItem *		bi )
    {
    while( bi )
	{
	if  ( bi->biLevel == DOClevPARA )
	    {
	    dp->dpBi= bi;
	    dp->dpStroff= bi->biParaStrlen;

	    return 0;
	    }

	if  ( bi->biChildCount == 0 )
	    { /*LDEB(bi->biChildCount);*/ return -1;	}

	bi= bi->biChildren[bi->biChildCount- 1];
	}

    /*XDEB(bi);*/ return -1;
    }

int docFirstDocumentPosition(	DocumentPosition *	dp,
				BufferDocument *	bd )
    {
    if  ( docFirstPosition( dp, &(bd->bdItem) ) )
	{ return -1;	}

    return 0;
    }

int docLastDocumentPosition(	DocumentPosition *	dp,
				BufferDocument *	bd )
    {
    if  ( docLastPosition( dp, &(bd->bdItem) ) )
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Move to the next/previous position.					*/
/*									*/
/************************************************************************/

static BufferItem * docNextItem(	BufferItem *	bi,
					int		level )
    {
    for (;;)
	{
	if  ( ! bi->biParent )
	    { return (BufferItem *)0;	}

	if  ( bi->biNumberInParent < bi->biParent->biChildCount- 1 )
	    {
	    bi= bi->biParent->biChildren[bi->biNumberInParent+ 1];

	    while( bi->biLevel < level	&&
		   bi->biChildCount > 0	)
		{ bi= bi->biChildren[0]; }

	    if  ( bi->biLevel == level )
		{ return bi;	}
	    }
	else{ bi= bi->biParent;	}
	}
    }

BufferItem *	docNextParagraph(	BufferItem *	bi )
    { return docNextItem( bi, DOClevPARA );	}

BufferItem *	docNextSection(	BufferItem *	bi )
    { return docNextItem( bi, DOClevSECT );	}

static BufferItem * docPrevItem(	BufferItem *	bi,
					int		level )
    {
    for (;;)
	{
	if  ( ! bi->biParent )
	    { return (BufferItem *)0;	}

	if  ( bi->biNumberInParent > 0 )
	    {
	    bi= bi->biParent->biChildren[bi->biNumberInParent- 1];

	    while( bi->biLevel < level	&&
		   bi->biChildCount > 0	)
		{ bi= bi->biChildren[bi->biChildCount- 1]; }

	    if  ( bi->biLevel == level )
		{ return bi;	}
	    }
	else{ bi= bi->biParent;	}
	}
    }

BufferItem *	docPrevParagraph(	BufferItem *	bi )
    { return docPrevItem( bi, DOClevPARA );	}

BufferItem *	docPrevSection(		BufferItem *	bi )
    { return docPrevItem( bi, DOClevSECT );	}

int docNextPosition(	DocumentPosition *	dp )
    {
    BufferItem *	bi= dp->dpBi;

    int			stroff= dp->dpStroff+ 1;

    while( bi )
	{
	if  ( stroff <= bi->biParaStrlen )
	    {
	    docSetDocumentPosition( dp, bi, stroff );
	    return 0;
	    }

	bi= docNextParagraph( bi );
	stroff= 0;

	if  ( bi && bi->biParaStrlen == 0 )
	    {
	    docSetDocumentPosition( dp, bi, stroff );
	    return 0;
	    }
	}

    return -1;
    }

int docPrevPosition(	DocumentPosition *	dp )
    {
    BufferItem *	bi= dp->dpBi;

    int			stroff= dp->dpStroff- 1;

    while( bi )
	{
	if  ( bi->biLevel == DOClevPARA		&&
	      stroff >= 0			)
	    {
	    docSetDocumentPosition( dp, bi, stroff );
	    return 0;
	    }

	bi= docPrevParagraph( bi );
	if  ( bi )
	    { stroff= bi->biParaStrlen; }
	}

    return -1;
    }

int docParaBegin(	DocumentPosition *	dp )
    {
    const int		stroff= 0;

    docSetDocumentPosition( dp, dp->dpBi, stroff );

    return 0;
    }

int docParaEnd(		DocumentPosition *	dp )
    {
    docSetDocumentPosition( dp, dp->dpBi, dp->dpBi->biParaStrlen );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Move to the Next/Previous word.					*/
/*									*/
/*  1)  Words start after a particule ending in a space or at the	*/
/*	beginning of a paragraph.					*/
/*									*/
/************************************************************************/

int docNextWord(	DocumentPosition *	dp )
    {
    BufferItem *		bi= dp->dpBi;
    int				stroff= dp->dpStroff;
    const unsigned char *	s;

    if  ( stroff == bi->biParaStrlen )
	{
	bi= docNextParagraph( bi );
	if  ( ! bi )
	    { return 1;	}

	stroff= 0;
	}

    s= bi->biParaString;

    while( stroff < bi->biParaStrlen && ! isspace( s[stroff] ) )
	{ stroff++;	}
    while( stroff < bi->biParaStrlen &&   isspace( s[stroff] ) )
	{ stroff++;	}

    docSetDocumentPosition( dp, bi, stroff );

    return 0;
    }

int docPrevWord(	DocumentPosition *	dp )
    {
    BufferItem *		bi= dp->dpBi;
    int				stroff= dp->dpStroff;
    const unsigned char *	s;

    if  ( stroff == 0 )
	{
	bi= docPrevParagraph( bi );
	if  ( ! bi )
	    { return 1;	}

	stroff= bi->biParaStrlen;
	}

    s= bi->biParaString;

    while( stroff > 0 &&   isspace( s[stroff- 1] ) )
	{ stroff--;	}
    while( stroff > 0 && ! isspace( s[stroff- 1] ) )
	{ stroff--;	}

    docSetDocumentPosition( dp, bi, stroff );

    return 0;
    }


/************************************************************************/
/*									*/
/*  Move one line up.							*/
/*									*/
/*  1)  Go to previous line of the paragraph or to the last line of the	*/
/*	last paragraph before this one that has any lines.		*/
/*  2)  In a clumsy attempt to implement vertical movement in tables,	*/
/*	try to stay in the same column of a table.			*/
/*									*/
/************************************************************************/

int docLineUp(		TextLine **		pTl,
			DocumentPosition *	dp,
			int			line )
    {
    TextLine *		tl;
    BufferItem *	bi= dp->dpBi;

    line--;

    /*  1  */
    while( bi )
	{
	if  ( bi->biParaLineCount == 0 )
	    { LDEB(bi->biParaLineCount);	}

	if  ( bi->biLevel == DOClevPARA		&&
	      line < bi->biParaLineCount	&& /* against crashes */
	      line >= 0				)
	    {
	    tl= bi->biParaLines+ line;

	    dp->dpBi= bi;
	    dp->dpStroff= tl->tlStroff;

	    if  ( pTl )
		{ *pTl= tl;	}
	    return 0;
	    }

	/*  2  */
	if  ( bi->biLevel == DOClevPARA		&&
	      bi->biParaInTable			&&
	      bi->biNumberInParent == 0		)
	    {
	    int			col;
	    int			row0;
	    int			row;
	    int			row1;

	    BufferItem *	sectBi;

	    if  ( docDelimitTable( bi, &sectBi, &col, &row0, &row, &row1 ) )
		{ LDEB(1); return -1;	}

	    if  ( row > row0 )
		{
		BufferItem *	rowBi= sectBi->biChildren[row-1];

		if  ( col < rowBi->biChildCount )
		    {
		    BufferItem *	cellBi= rowBi->biChildren[col];

		    bi= cellBi->biChildren[cellBi->biChildCount-1];
		    line= bi->biParaLineCount- 1;
		    continue;
		    }
		}
	    }

	bi= docPrevParagraph( bi );
	if  ( ! bi )
	    { break;	}
	line= bi->biParaLineCount- 1;
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Move one line down.							*/
/*									*/
/*  1)  Go to the next line of the paragraph or to the first line of	*/
/*	the first paragraph after this one that has any lines.		*/
/*  2)  In a clumsy attempt to implement vertical movement in tables,	*/
/*	try to stay in the same column of a table.			*/
/*									*/
/************************************************************************/

int docLineDown(	TextLine **		pTl,
			DocumentPosition *	dp,
			int			line )
    {
    TextLine *		tl;
    BufferItem *	bi= dp->dpBi;

    line++;

    /*  1  */
    while( bi )
	{
	if  ( bi->biLevel == DOClevPARA		&&
	      line < bi->biParaLineCount	&&
	      line >= 0				)   /*  against crashes  */
	    {
	    tl= bi->biParaLines+ line;

	    dp->dpBi= bi;
	    dp->dpStroff= tl->tlStroff;

	    if  ( pTl )
		{ *pTl= tl;	}
	    return 0;
	    }

	/*  2  */
	if  ( bi->biLevel == DOClevPARA					&&
	      bi->biParaInTable						&&
	      bi->biNumberInParent == bi->biParent->biChildCount- 1	)
	    {
	    int			col;
	    int			row0;
	    int			row;
	    int			row1;

	    BufferItem *	sectBi;

	    if  ( docDelimitTable( bi, &sectBi, &col, &row0, &row, &row1 ) )
		{ LDEB(1); return -1;	}

	    if  ( row < row1 )
		{
		BufferItem *	rowBi= sectBi->biChildren[row+1];

		if  ( col < rowBi->biChildCount )
		    {
		    BufferItem *	cellBi= rowBi->biChildren[col];

		    bi= cellBi->biChildren[0];
		    line= 0;
		    continue;
		    }
		}
	    }

	bi= docNextParagraph( bi );
	line= 0;
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Move to the beginning/end of a line					*/
/*									*/
/************************************************************************/

int docBeginOfLine(	DocumentPosition *	dp,
			int			wasAtHead )
    {
    BufferItem *	bi= dp->dpBi;

    int			line;
    TextLine *		tl;

    if  ( wasAtHead )
	{ return 0;	}

    tl= bi->biParaLines;
    for ( line= 0; line < bi->biParaLineCount; tl++, line++ )
	{
	int			stroff= tl->tlStroff;

	/*  1  */
	if  ( stroff+ tl->tlStrlen > dp->dpStroff	||
	      line == bi->biParaLineCount- 1		)
	    {
	    dp->dpBi= bi;
	    dp->dpStroff= stroff;

	    return 0;
	    }
	}

    LDEB(1); return 1;
    }

int docEndOfLine(	DocumentPosition *	dp,
			int			wasAtHead )
    {
    BufferItem *	bi= dp->dpBi;

    int			line;
    TextLine *		tl;

    tl= bi->biParaLines;
    for ( line= 0; line < bi->biParaLineCount; tl++, line++ )
	{
	int			stroff= tl->tlStroff;

	if  ( wasAtHead					&&
	      stroff+ tl->tlStrlen == dp->dpStroff	&&
	      line < bi->biParaLineCount- 1		)
	    { wasAtHead= 0; continue;	}

	if  ( stroff+ tl->tlStrlen >= dp->dpStroff )
	    {
	    dp->dpBi= bi;
	    dp->dpStroff= stroff+ tl->tlStrlen;

	    return 0;
	    }
	}

    LDEB(1); return 1;
    }

/************************************************************************/
/*									*/
/*  Compare two positions in a document.				*/
/*									*/
/*  Comparison is in terms of the reading order in the document.	*/
/*									*/
/************************************************************************/

int docCompareItemPositions(	const BufferItem *	bi1,
				const BufferItem *	bi2 )
    {
    while( bi1->biParent && bi1->biLevel > bi2->biLevel )
	{ bi1= bi1->biParent;	}

    while( bi2->biParent && bi2->biLevel > bi1->biLevel )
	{ bi2= bi2->biParent;	}

    if  ( bi1 == bi2 )
	{ return 0;	}

    while( bi1->biParent			&&
	   bi2->biParent			&&
	   ( bi1->biParent != bi2->biParent )	)
	{ bi1= bi1->biParent; bi2= bi2->biParent; }

    if  ( bi1->biParent && bi1->biParent == bi2->biParent )
	{
	if  ( bi1->biNumberInParent > bi2->biNumberInParent )
	    { return  1;	}

	if  ( bi1->biNumberInParent < bi2->biNumberInParent )
	    { return -1;	}

	return 0;
	}

    XXDEB(bi1->biParent,bi2->biParent);

    return 0;
    }

int docComparePositions(	const DocumentPosition *	dp1,
				const DocumentPosition *	dp2 )
    {
    const BufferItem *	bi1= dp1->dpBi;
    const BufferItem *	bi2= dp2->dpBi;

    if  ( bi1 == bi2 )
	{
	if  ( dp1->dpStroff > dp2->dpStroff )
	    { return  1;	}

	if  ( dp1->dpStroff < dp2->dpStroff )
	    { return -1;	}

	return 0;
	}

    return docCompareItemPositions( bi1, bi2 );
    }

/************************************************************************/
/*									*/
/*  Go to the top/bottom of a certain page.				*/
/*									*/
/*  1)  Sections that start on an odd/even page may skip a page.	*/
/*									*/
/************************************************************************/

int docGetFirstInColumnForItem(
			DocumentPosition *		dp,
			int *				pLineTop,
			int *				pPartTop,
			BufferItem *			bi,
			int				page,
			int				column )
    {
    int			i;
    const TextLine *	tl;

    while( bi && bi->biLevel != DOClevPARA )
	{
	/*  1  */
	if  ( bi->biTopPosition.lpPage > page )
	    {
	    /* LLDEB(bi->biTopPosition.lpPage,page); */
	    return 1;
	    }
	if  ( bi->biBelowPosition.lpPage < page )
	    {
	    /* LLDEB(bi->biTopPosition.lpPage,page); */
	    return 1;
	    }

	for ( i= 0; i < bi->biChildCount; i++ )
	    {
	    if  ( bi->biChildren[i]->biBelowPosition.lpPage >= page )
		{
		if  ( bi->biChildren[i]->biBelowPosition.lpPage > page )
		    { break;	}
		if  ( bi->biChildren[i]->biBelowPosition.lpColumn >= column )
		    { break;	}
		}
	    }

	if  ( i >= bi->biChildCount )
	    {
	    /*  NO! is possible e.g. when endnotes continue beyond the 
	        last page with body content.
	    for ( i= 0; i < bi->biChildCount; i++ )
		{ LLDEB(i,bi->biChildren[i]->biBelowPosition.lpPage); }
	    LLLDEB(bi->biTopPosition.lpPage,bi->biBelowPosition.lpPage,page);
	    return -1;
	    */
	    return 1;
	    }

	bi= bi->biChildren[i];
	}

    if  ( ! bi || bi->biLevel != DOClevPARA )
	{ XDEB(bi); return -1;	}

    tl= bi->biParaLines;
    for ( i= 0; i < bi->biParaLineCount; tl++, i++ )
	{
	if  ( tl->tlTopPosition.lpPage >= page )
	    {
	    if  ( tl->tlTopPosition.lpPage > page )
		{ break;	}
	    if  ( tl->tlTopPosition.lpColumn >= column )
		{ break;	}
	    }
	}

    if  ( i >= bi->biParaLineCount		||
	  tl->tlTopPosition.lpPage != page	||
	  tl->tlTopPosition.lpColumn != column	)
	{
	LDEB(docNumberOfParagraph(bi));
	LLDEB(i,bi->biParaLineCount);
	return -1;
	}

    dp->dpBi= bi;
    dp->dpStroff= tl->tlStroff;

    *pLineTop= tl- bi->biParaLines;
    *pPartTop= tl->tlFirstParticule;

    return 0;
    }

int docGetTopOfColumn(	DocumentPosition *		dp,
			int *				pLineTop,
			int *				pPartTop,
			BufferDocument *		bd,
			int				page,
			int				column )
    {
    return docGetFirstInColumnForItem( dp, pLineTop, pPartTop, &(bd->bdItem),
								page, column );
    }

int docGetLastInColumnForItem(	DocumentPosition *		dp,
				int *				pLineBot,
				int *				pPartBot,
				BufferItem *			bi,
				int				page,
				int				column )
    {
    int			i;
    const TextLine *	tl;

    while( bi && bi->biLevel != DOClevPARA )
	{
	if  ( bi->biTopPosition.lpPage > page )
	    { LLDEB(bi->biBelowPosition.lpPage,page); return -1;	}
	if  ( bi->biBelowPosition.lpPage < page )
	    { LLDEB(bi->biBelowPosition.lpPage,page); return -1;	}

	for ( i= bi->biChildCount- 1; i >= 0; i-- )
	    {
	    if  ( bi->biChildren[i]->biTopPosition.lpPage <= page )
		{
		if  ( bi->biChildren[i]->biTopPosition.lpPage < page )
		    { break;	}
		if  ( bi->biChildren[i]->biTopPosition.lpColumn <= column )
		    { break;	}
		}
	    }

	if  ( i < 0 )
	    {
	    for ( i= 0; i < bi->biChildCount; i++ )
		{ LLDEB(i,bi->biChildren[i]->biTopPosition.lpPage); }
	    LLLDEB(bi->biTopPosition.lpPage,bi->biBelowPosition.lpPage,page);
	    return -1;
	    }

	bi= bi->biChildren[i];
	}

    if  ( ! bi || bi->biLevel != DOClevPARA )
	{ XDEB(bi); return -1;	}

    tl= bi->biParaLines+ bi->biParaLineCount- 1;
    for ( i= bi->biParaLineCount- 1; i >= 0; tl--, i-- )
	{
	if  ( tl->tlTopPosition.lpPage <= page )
	    {
	    if  ( tl->tlTopPosition.lpPage < page )
		{ break;	}
	    if  ( tl->tlTopPosition.lpColumn <= column )
		{ break;	}
	    }
	}

    if  ( i >= bi->biParaLineCount		||
	  tl->tlTopPosition.lpPage != page	||
	  tl->tlTopPosition.lpColumn != column	)
	{ LLDEB(i,bi->biParaLineCount); return -1;	}

    dp->dpBi= bi;
    dp->dpStroff= tl->tlStroff+ tl->tlStrlen;

    *pLineBot= tl- bi->biParaLines;
    *pPartBot= tl->tlFirstParticule+ tl->tlParticuleCount- 1;

    return 0;
    }

int docGetBottomOfColumn(
			DocumentPosition *		dp,
			int *				pPartBot,
			BufferDocument *		bd,
			int				page,
			int				column )
    {
    int		lineBot;

    return docGetLastInColumnForItem( dp, &lineBot, pPartBot, &(bd->bdItem),
								page, column );
    }

/************************************************************************/
/*									*/
/*  Translate the result of a search in a paragraph to a selection.	*/
/*									*/
/************************************************************************/

void docSetParaSelection(	DocumentSelection *	ds,
				BufferItem *		paraBi,
				int			direction,
				int			stroff,
				int			length )
    {
    if  ( paraBi->biLevel != DOClevPARA )
	{ LDEB(paraBi->biLevel);	}

    docSetDocumentPosition( &(ds->dsBegin), paraBi, stroff );
    docSetDocumentPosition( &(ds->dsEnd), paraBi, stroff+ length );

    ds->dsDirection= direction;

    ds->dsCol0= ds->dsCol1= -1;

    if  ( direction >= 0 )
	{ ds->dsAnchor= ds->dsBegin;	}
    else{ ds->dsAnchor= ds->dsEnd;	}

    docSetSelectionScope( ds, paraBi );

    return;
    }

/************************************************************************/
/*									*/
/*  Derive the scope of a selection from a buffer item inside.		*/
/*									*/
/************************************************************************/

void docSetSelectionScope(	DocumentSelection *	ds,
				const BufferItem *	sectBi )
    {
    if  ( ! sectBi )
	{ XDEB(sectBi);	}

    while( sectBi && sectBi->biLevel != DOClevSECT )
	{ sectBi= sectBi->biParent;	}

    if  ( ! sectBi )
	{ XDEB(sectBi);	}
    else{ ds->dsSelectionScope= sectBi->biSectSelectionScope;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Get buffer positions for a text line.				*/
/*									*/
/************************************************************************/

void docLineSelection(	DocumentSelection *	dsLine,
			int *			pPartLineBegin,
			int *			pPartLineEnd,
			const BufferItem *	bi,
			int			line )
    {
    const TextLine *	tl= bi->biParaLines+ line;

    if  ( bi->biLevel != DOClevPARA )
	{ LLDEB(bi->biLevel,DOClevPARA); return;	}
    if  ( line < 0 || line >= bi->biParaLineCount )
	{ LLDEB(line,bi->biParaLineCount); return;	}

    docInitDocumentSelection( dsLine );

    dsLine->dsBegin.dpBi= (BufferItem *)bi;
    dsLine->dsBegin.dpStroff= tl->tlStroff;

    dsLine->dsEnd.dpBi= (BufferItem *)bi;
    dsLine->dsEnd.dpStroff= tl->tlStroff+ tl->tlStrlen;

    dsLine->dsAnchor= dsLine->dsBegin;
    dsLine->dsDirection= 1;

    dsLine->dsCol0= dsLine->dsCol1= -1;

    docSetSelectionScope( dsLine, bi );

    *pPartLineBegin= tl->tlFirstParticule;
    *pPartLineEnd= tl->tlFirstParticule+ tl->tlParticuleCount- 1;

    return;
    }

void docWordSelection(	DocumentSelection *		dsWord,
			int *				pIsObject,
			const DocumentPosition *	dpAround,
			int				partAround )
    {
    TextParticule *	tp;

    BufferItem *	paraBi= dpAround->dpBi;
    int			part;

    part= partAround;
    tp= paraBi->biParaParticules+ part;

    if  ( paraBi->biLevel != DOClevPARA )
	{ LLDEB(paraBi->biLevel,DOClevPARA); return;	}
    if  ( part < 0 || part >= paraBi->biParaParticuleCount )
	{ LLDEB(part,paraBi->biParaParticuleCount); return;	}

    while( part < paraBi->biParaParticuleCount- 1		&&
	   tp->tpStroff+ tp->tpStrlen <= dpAround->dpStroff	)
	{ tp++; part++;	}

    tp= paraBi->biParaParticules+ part;

    if  ( tp->tpStroff == dpAround->dpStroff	&&
	  part > 0				&&
	  tp[-1].tpKind == DOCkindOBJECT	)
	{
	docSetParaSelection( dsWord, paraBi, 1,
					tp[-1].tpStroff, tp[-1].tpStrlen );
	*pIsObject= 1;
	return;
	}

    if  ( tp->tpKind == DOCkindOBJECT )
	{
	docSetParaSelection( dsWord, paraBi, 1, tp->tpStroff, tp->tpStrlen );
	*pIsObject= 1;
	return;
	}
    else{
	int	partBegin= part;
	int	stroffBegin= tp->tpStroff;

	int	partEnd= part;
	int	stroffEnd= tp->tpStroff+ tp->tpStrlen;

	while( partBegin > 0				&&
	       tp[-1].tpKind == DOCkindTEXT		&&
	       paraBi->biParaString[tp->tpStroff- 1] != ' '	)
	    {
	    tp--; partBegin--;
	    stroffBegin= tp->tpStroff;
	    }

	tp= paraBi->biParaParticules+ part;
	while( partEnd < paraBi->biParaParticuleCount- 1	&&
	       tp[1].tpKind == DOCkindTEXT		&&
	       paraBi->biParaString[stroffEnd- 1] != ' '	)
	    {
	    tp++; partEnd++;
	    stroffEnd= tp->tpStroff+ tp->tpStrlen;
	    }

	while( stroffEnd > tp->tpStroff			&&
	       paraBi->biParaString[stroffEnd- 1] == ' '	)
	    { stroffEnd--; }

	docSetParaSelection( dsWord, paraBi, 1,
				    stroffBegin, stroffEnd- stroffBegin );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Translate a TableRectangle to a DocumentSelection.			*/
/*									*/
/************************************************************************/

int docTableRectangleSelection(	DocumentSelection *	ds,
				BufferItem **		pSelSectBi,
				BufferDocument *	bd,
				const TableRectangle *	tr )
    {
    DocumentSelection	dsNew;

    BufferItem *	selSectBi;
    BufferItem *	rowBi;
    BufferItem *	cellBi;

    ExternalItem *	ei;
    BufferItem *	bodySectBi;

    docInitDocumentSelection( &dsNew );

    /*******/

    selSectBi= docGetSelectionRoot( &ei, &bodySectBi, bd, ds );
    while( selSectBi && selSectBi->biLevel != DOClevSECT )
	{ selSectBi= selSectBi->biParent;	}
    if  ( ! selSectBi )
	{ XDEB(selSectBi); return -1;	}

    /*******/

    if  ( tr->trRow0 < 0			||
	  tr->trRow0 >= selSectBi->biChildCount	)
	{ LLDEB(tr->trRow0,selSectBi->biChildCount); return -1;	}
    rowBi= selSectBi->biChildren[tr->trRow0];

    if  ( tr->trCol0 < 0			||
	  tr->trCol0 >= rowBi->biChildCount	)
	{ LLDEB(tr->trCol0,rowBi->biChildCount); return -1;	}
    cellBi= rowBi->biChildren[tr->trCol0];

    if  ( docFirstPosition( &dsNew.dsBegin, cellBi ) )
	{ LDEB(0); return -1;	}

    /*******/

    if  ( tr->trRow1 < 0			||
	  tr->trRow1 >= selSectBi->biChildCount	)
	{ LLDEB(tr->trRow0,selSectBi->biChildCount); return -1;	}
    rowBi= selSectBi->biChildren[tr->trRow1];

    if  ( tr->trCol1 < 0			||
	  tr->trCol1 >= rowBi->biChildCount	)
	{ LLDEB(tr->trCol1,rowBi->biChildCount); return -1;	}
    cellBi= rowBi->biChildren[tr->trCol1];

    if  ( docLastPosition( &dsNew.dsEnd, cellBi ) )
	{ LDEB(0); return -1;	}

    /*******/

    dsNew.dsCol0= tr->trCol0;
    dsNew.dsCol1= tr->trCol1;
    dsNew.dsDirection= 1;
    dsNew.dsAnchor= dsNew.dsBegin;

    if  ( tr->trRow1 < tr->trRow0					||
	  ( tr->trRow1 == tr->trRow0 && tr->trCol1 < tr->trCol0 )	)
	{
	dsNew.dsDirection= -1;
	dsNew.dsAnchor= dsNew.dsEnd;
	}

    docSetSelectionScope( &dsNew, selSectBi );

    *ds= dsNew;
    *pSelSectBi= selSectBi;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Decide whether the selection moved to a different scope.		*/
/*									*/
/*  NOTE: Only use the scope fields of the old selection as deletions	*/
/*	or insertions of BufferItems may have invalidated the pointers.	*/
/*									*/
/************************************************************************/

int docSelectionSameScope(	const SelectionScope *	ssFrom,
				const SelectionScope *	ssTo )
    {
    if  ( ssTo->ssInExternalItem != ssFrom->ssInExternalItem )
	{ return 0;	}

    switch( ssTo->ssInExternalItem )
	{
	case DOCinBODY:

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    return 1;

	case DOCinSECT_HEADER:
	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:
	case DOCinSECT_FOOTER:
	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:

	    if  ( ssTo->ssSectNrExternalTo != ssFrom->ssSectNrExternalTo )
		{ return 0;	}

	    return 1;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	    /*  Implied by ssNoteArrayIndex test:
	    if  ( ssTo->ssSectNrExternalTo != ssFrom->ssSectNrExternalTo )
		{ return 0;	}
	    */
	    if  ( ssTo->ssNoteArrayIndex != ssFrom->ssNoteArrayIndex )
		{ return 0;	}

	    return 1;

	default:
	    LLDEB(ssTo->ssInExternalItem,ssFrom->ssInExternalItem);
	    return -1;
	}
    }

int docSelectionSameRoot(
			const DocumentSelection *	dsFrom,
			const BufferItem *		biTo )
    {
    const SelectionScope *	ssFrom= &(dsFrom->dsSelectionScope);
    const SelectionScope *	ssTo;

    while( biTo && biTo->biLevel != DOClevSECT )
	{ biTo= biTo->biParent;	}

    if  ( ! biTo )
	{ XDEB(biTo); return -1;	}
    ssTo= &(biTo->biSectSelectionScope);

    return docSelectionSameScope( ssFrom, ssTo );
    }

/************************************************************************/
/*									*/
/*  Implementation of a 'Select All' menu option.			*/
/*									*/
/************************************************************************/

int docSelectAll(		DocumentSelection *	ds,
				BufferDocument *	bd )
    {
    BufferItem *		selSectBi;

    ExternalItem *		ei;
    BufferItem *		bodySectBi;

    selSectBi= docGetSelectionRoot( &ei, &bodySectBi, bd, ds );
    if  ( ! selSectBi )
	{ selSectBi= &(bd->bdItem);	}

    while( selSectBi && selSectBi->biParent )
	{ selSectBi= selSectBi->biParent; }
    if  ( ! selSectBi )
	{ XDEB(selSectBi); return -1;	}

    if  ( docFirstPosition( &(ds->dsBegin), selSectBi ) )
	{ LDEB(1); return -1;	}
    if  ( docLastPosition( &(ds->dsEnd), selSectBi ) )
	{ LDEB(1); return -1;	}

    ds->dsAnchor= ds->dsBegin;
    ds->dsDirection= 1;

    docSetSelectionScope( ds, ds->dsBegin.dpBi );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Change the selection to cover whole sections.			*/
/*									*/
/*  Depending on the 'direction' argument:				*/
/*	<  0:	The previous one.					*/
/*	== 0:	The current one.					*/
/*	>  0:	The next one.						*/
/*									*/
/************************************************************************/

int docSelectWholeSection(	DocumentSelection *	ds,
				BufferDocument *	bd,
				int			direction )
    {
    BufferItem *		selSectBi;

    ExternalItem *		ei;
    BufferItem *		bodySectBi;

    selSectBi= docGetSelectionRoot( &ei, &bodySectBi, bd, ds );
    if  ( ! selSectBi )
	{ XDEB(selSectBi); return -1;	}

    while( selSectBi && selSectBi->biLevel != DOClevSECT )
	{ selSectBi= selSectBi->biParent; }
    if  ( ! selSectBi )
	{ XDEB(selSectBi); return -1;	}

    if  ( direction > 0 )
	{
	if  ( docLastPosition( &(ds->dsBegin), selSectBi ) )
	    { LDEB(1); return -1;	}
	if  ( docNextPosition( &(ds->dsBegin) ) )
	    { return 1;	}

	ds->dsEnd= ds->dsBegin;

	selSectBi= docGetSelectionRoot( &ei, &bodySectBi, bd, ds );
	if  ( ! selSectBi )
	    { XDEB(selSectBi); return -1;	}
	}

    if  ( direction < 0 )
	{
	if  ( docFirstPosition( &(ds->dsBegin), selSectBi ) )
	    { LDEB(1); return -1;	}
	if  ( docPrevPosition( &(ds->dsBegin) ) )
	    { return 1;	}

	ds->dsEnd= ds->dsBegin;

	selSectBi= docGetSelectionRoot( &ei, &bodySectBi, bd, ds );
	if  ( ! selSectBi )
	    { XDEB(selSectBi); return -1;	}
	}

    while( selSectBi && selSectBi->biLevel != DOClevSECT )
	{ selSectBi= selSectBi->biParent; }
    if  ( ! selSectBi )
	{ XDEB(selSectBi); return -1;	}

    if  ( docFirstPosition( &(ds->dsBegin), selSectBi ) )
	{ LDEB(1); return -1;	}
    if  ( docLastPosition( &(ds->dsEnd), selSectBi ) )
	{ LDEB(1); return -1;	}

    if  ( direction >= 0 )
	{ ds->dsAnchor= ds->dsBegin;	}
    else{ ds->dsAnchor= ds->dsEnd;	}

    if  ( direction >= 0 )
	{ ds->dsDirection=  1;	}
    else{ ds->dsDirection= -1;	}

    docSetSelectionScope( ds, ds->dsBegin.dpBi );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Is the selection exactly an object?					*/
/*									*/
/************************************************************************/

int docGetObjectSelection(	const DocumentSelection *	ds,
				int *				pPart,
				DocumentPosition *		dpObject,
				InsertedObject **		pIo )
    {
    if  ( ds->dsBegin.dpBi			&&
	  ds->dsEnd.dpBi == ds->dsBegin.dpBi	)
	{
	BufferItem *		paraBi= ds->dsBegin.dpBi;
	int			part;
	TextParticule *		tp;

	const int		lastOne= 1;

	if  ( docFindParticule( &part, ds->dsBegin.dpBi,
					    ds->dsBegin.dpStroff, lastOne ) )
	    { LDEB(ds->dsBegin.dpStroff); return -1;	}

	tp= paraBi->biParaParticules+ part;

	if  ( tp->tpKind == DOCkindOBJECT			&&
	      ds->dsBegin.dpStroff == tp->tpStroff		&&
	      ds->dsEnd.dpStroff == tp->tpStroff+ tp->tpStrlen	)
	    {
	    *pPart= part;
	    *dpObject= ds->dsBegin;
	    *pIo= paraBi->biParaObjects+ tp->tpObjectNumber;

	    return 0;
	    }
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Fill a DocumentPosition.						*/
/*									*/
/************************************************************************/

int docSetDocumentPosition(	DocumentPosition *	dp,
				BufferItem *		bi,
				int			stroff )
    {
    dp->dpBi= bi;
    dp->dpStroff= stroff;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find the particule number for a certain string position.		*/
/*									*/
/*  NOTE: This does not expect the paragraph to be formatted.		*/
/*									*/
/************************************************************************/

int docFindParticuleOfPosition(	int *				pPart,
				const DocumentPosition *	dp,
				int				lastOne )
    {
    return docFindParticule( pPart, dp->dpBi, dp->dpStroff, lastOne );
    }

int docFindParticule(		int *			pPart,
				const BufferItem *	bi,
				int			stroff,
				int			lastOne )
    {
    int				part= 0;

    const TextParticule *	tp;

    part= 0; tp= bi->biParaParticules+ part;
    while( part < bi->biParaParticuleCount	&&
	   tp->tpStroff+ tp->tpStrlen < stroff	)
	{ part++; tp++;	}

    if  ( part >= bi->biParaParticuleCount )
	{
	LLDEB(stroff,bi->biParaStrlen);
	LLDEB(part,bi->biParaParticuleCount);
	docListItem( 0, bi );
	return -1;
	}

    while( lastOne				&&
	   part < bi->biParaParticuleCount -1	&&
	   tp->tpStroff+ tp->tpStrlen == stroff	)
	{ part++; tp++;	}

    *pPart= part;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find the line number for a certain string particule Number.		*/
/*									*/
/*  NOTE: This does not expect the paragraph to be formatted. We just	*/
/*	fail because there are no lines. We do not crash however.	*/
/*									*/
/************************************************************************/

int docFindLineOfPosition(	int *				pLine,
				const DocumentPosition *	dp,
				int				lastOne )
    {
    const BufferItem *	bi= dp->dpBi;
    int			line= 0;

    const TextLine *	tl;

    line= 0; tl= bi->biParaLines+ line;
    while( line < bi->biParaLineCount			&&
	   tl->tlStroff+ tl->tlStrlen < dp->dpStroff	)
	{ line++; tl++;	}

    if  ( line >= bi->biParaLineCount )
	{
	LLDEB(line,bi->biParaLineCount);
	LLDEB(dp->dpStroff,bi->biParaParticuleCount);
	docListItem( 0, bi );
	return -1;
	}

    while( lastOne					&&
	   line < bi->biParaLineCount- 1		&&
	   tl->tlStroff+ tl->tlStrlen == dp->dpStroff	)
	{ line++; tl++;	}

    *pLine= line;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Describe a selection and its relevance for application tools.	*/
/*									*/
/************************************************************************/

void docDescribeSelection(	SelectionDescription *		sd,
				const DocumentSelection *	ds,
				const BufferDocument *		bd,
				unsigned int			documentId,
				int				documentRo )
    {
    const DocumentPosition *	dpBegin= &(ds->dsBegin);
    const DocumentPosition *	dpEnd= &(ds->dsEnd);

    int				i;
    int				part;

    sd->sdDocumentId= documentId;
    sd->sdIsSet= 1;
    sd->sdDocumentReadonly= documentRo;
    sd->sdIsIBarSelection= docIsIBarSelection( ds );
    sd->sdIsSingleParagraph= docPositionsInsideParagraph( dpBegin, dpEnd );
    sd->sdIsSingleCell= docPositionsInsideCell( dpBegin, dpEnd );

    sd->sdBeginInTable= dpBegin->dpBi->biParaInTable;
    sd->sdBeginInTableHeader= 0;
    sd->sdIsColSlice= 0;
    sd->sdIsRowSlice= 0;
    sd->sdIsTableSlice= 0;
    sd->sdIsTableRectangle= 0;
    sd->sdIsObjectSelection= 0;

    if  ( sd->sdBeginInTable )
	{
	const BufferItem *	rowBi= dpBegin->dpBi;

	while( rowBi && rowBi->biLevel != DOClevROW )
	    { rowBi= rowBi->biParent;	}

	if  ( rowBi				&&
	      rowBi->biRowHasTableParagraphs	&&
	      rowBi->biRowIsTableHeader		)
	    { sd->sdBeginInTableHeader= 1;	}
	}

    sd->sdInExternalItem= dpBegin->dpBi->biInExternalItem;
    if  ( dpBegin->dpBi->biInExternalItem != dpEnd->dpBi->biInExternalItem )
	{
	LLDEB(dpBegin->dpBi->biInExternalItem,dpEnd->dpBi->biInExternalItem);
	}

    sd->sdInDocumentBody= ( sd->sdInExternalItem == DOCinBODY );
    sd->sdInHeaderFooter= 0;

    for ( i= 0; i < PAGES__COUNT; i++ )
	{
	if  ( sd->sdInExternalItem == DOC_HeaderScopes[i] )
	    { sd->sdInHeaderFooter= 1; break;	}
	if  ( sd->sdInExternalItem == DOC_FooterScopes[i] )
	    { sd->sdInHeaderFooter= 1; break;	}
	}

    if  ( ! sd->sdIsIBarSelection )
	{
	TableRectangle		tr;

	if  ( ! docGetTableRectangle( &tr, ds ) )
	    {
	    if  ( ! docGetTableSliceSelection( &(sd->sdIsRowSlice),
					    &(sd->sdIsColSlice), &tr, ds ) )
		{ sd->sdIsTableSlice= 1;	}

	    sd->sdIsTableRectangle= 1;
	    }
	}

    if  ( documentRo )
	{
	sd->sdCanReplace= 0;
	}
    else{
	sd->sdCanReplace= ! sd->sdIsTableRectangle || sd->sdIsSingleCell;
	}

    sd->sdBeginInField= docPositionInField( &part, dpBegin, bd );

    sd->sdBeginInHyperlink= 0;
    sd->sdBeginInBookmark= 0;

    {
    DocumentSelection	dsReference;
    int			startPart;
    int			endPart;

    const char *	markName;
    int			markSize;

    const char *	fileName;
    int			fileSize;

    if  ( ! docGetHyperlinkForPosition( bd, dpBegin,
				&dsReference, &startPart, &endPart,
				&fileName, &fileSize, &markName, &markSize ) )
	{ sd->sdBeginInHyperlink= 1;	}


    if  ( ! docGetBookmarkForPosition( bd, dpBegin,
				&dsReference, &startPart, &endPart,
				&markName, &markSize ) )
	{ sd->sdBeginInBookmark= 1;	}
    }

    sd->sdIsListBullet= 0;
    if  ( sd->sdIsSingleParagraph		&&
	  dpBegin->dpBi->biParaListOverride > 0	)
	{
	int		fieldNr= -1;
	int		partBegin= -1;
	int		partEnd= -1;
	int		stroffBegin= -1;
	int		stroffEnd= -1;

	if  ( docDelimitParaHeadField( &fieldNr, &partBegin, &partEnd,
				&stroffBegin, &stroffEnd, dpBegin->dpBi, bd ) )
	    { LDEB(dpBegin->dpBi->biParaListOverride);	}
	else{
	    if  ( stroffBegin == dpBegin->dpStroff	&&
		  stroffEnd == dpEnd->dpStroff		)
		{ sd->sdIsListBullet= 1;	}
	    }
	}

    sd->sdHasLists= 0;
    sd->sdListOverride= -1;
    sd->sdListLevel= -1;
    sd->sdMultiList= 0;
    sd->sdMultiLevel= 0;
    sd->sdFirstListParaNr= -1;

    sd->sdHasLists= ! docFindListOfSelection(
				&(sd->sdListOverride), &(sd->sdListLevel),
				&(sd->sdMultiList), &(sd->sdMultiLevel),
				&(sd->sdFirstListParaNr), ds );

    {
    DocumentPosition	dpObject;
    InsertedObject *	io;

    if  ( ! docGetObjectSelection( ds, &part, &dpObject, &io ) )
	{ sd->sdIsObjectSelection= 1; }
    }

    return;
    }

void docInitSelectionDescription(	SelectionDescription *	sd )
    {
    sd->sdDocumentId= 0;

    sd->sdIsSet= 0;
    sd->sdDocumentReadonly= 0;
    sd->sdIsIBarSelection= 0;
    sd->sdIsSingleParagraph= 0;
    sd->sdIsSingleCell= 0;
    sd->sdBeginInTable= 0;
    sd->sdBeginInTableHeader= 0;
    sd->sdIsColSlice= 0;
    sd->sdIsRowSlice= 0;
    sd->sdIsTableSlice= 0;
    sd->sdIsTableRectangle= 0;
    sd->sdIsObjectSelection= 0;

    sd->sdCanReplace= 0;
    sd->sdInExternalItem= 0;
    sd->sdInDocumentBody= 0;
    sd->sdInHeaderFooter= 0;

    sd->sdBeginInField= 0;
    sd->sdBeginInHyperlink= 0;
    sd->sdBeginInBookmark= 0;

    sd->sdIsListBullet= 0;

    sd->sdHasLists= 0;
    sd->sdFirstListParaNr= 0;
    sd->sdListOverride= 0;
    sd->sdListLevel= 0;
    sd->sdMultiList= 0;
    sd->sdMultiLevel= 0;

    return;
    }
