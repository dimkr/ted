/************************************************************************/
/*									*/
/*  Buffer administration routines. Functionality related to the item	*/
/*  tree.								*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docEdit.h"

#   define	VALIDATE_TREE	0

#if 0

Schema to illustrate the numbering of paragraphs and the 
administration that is kept for that purpose
=========================================================

bi->biLeftParagraphs is the number of paragraphs to the left 
of this item in its immediate parent plus the number of paragraph
descendants. I.E: For items with children, the number of paragraphs 
in the children is included in bi->biLeftParagraphs. A paragraph 
includes itself in its bi->biLeftParagraphs.

Numbers of the paragraphs and the value of bi->biLeftParagraphs:

    1     2     3     4     5     6     7     8     9    10    11    12
    +     +     +     +     +     +     +     +     +     +     +     +
    1     2     3     4     1     2     3     4     5     1     2     3
    |     |     |     |     |     |     |     |     |     |     |     |
    4-----+-----+-----+     9-----+-----+-----+-----+     12----+-----+
    |                       |                             |
    12----------------------+-----------------------------+
    |
    *

Deleting 8 yeilds:

    1     2     3     4     5     6     7           8     9    10    11
    +     +     +     +     +     +     +           +     +     +     +
    1     2     3     4     1     2     3           4     1     2     3
    |     |     |     |     |     |     |           |     |     |     |
    4-----+-----+-----+     8-----+-----+-----+-----+     11----+-----+
    |                       |                             |
    11----------------------+-----------------------------+
    |
    *

So after deleting/inserting paragraphs: Descend to the root of the 
tree. In every parent adapt bi->biLeftParagraphs of all direct 
children to the right of the child that we come from. Set
bi->biLeftParagraphs of the parent to the value in its right most 
child. Continue with the parent of the parent until we have reached 
the root.

NOTE: I am educated as a biologist. My trees have their root at the 
    bottom, not at the top like those of computer scientists that 
    turn the world upside down. (I must admit that the metaphor 
    of parents and descendants has the computer science orientation.)

#endif

/************************************************************************/
/*									*/
/*  Free a BufferItem.							*/
/*									*/
/************************************************************************/

void docCleanItem(	BufferDocument *	bd,
			BufferItem *		bi )
    {
    int			i;
    TextParticule *	tp;

    for ( i= bi->biChildCount- 1; i >= 0; i-- )
	{ docFreeItem( bd, bi->biChildren[i] ); }
    if  ( bi->biChildren )
	{ free( bi->biChildren );	}

    switch( bi->biLevel )
	{
	case DOClevDOC:
	    break;

	case DOClevSECT:
	    docCleanExternalItem( bd, &(bi->biSectHeader) );
	    docCleanExternalItem( bd, &(bi->biSectFirstPageHeader) );
	    docCleanExternalItem( bd, &(bi->biSectLeftPageHeader) );
	    docCleanExternalItem( bd, &(bi->biSectRightPageHeader) );

	    docCleanExternalItem( bd, &(bi->biSectFooter) );
	    docCleanExternalItem( bd, &(bi->biSectFirstPageFooter) );
	    docCleanExternalItem( bd, &(bi->biSectLeftPageFooter) );
	    docCleanExternalItem( bd, &(bi->biSectRightPageFooter) );

	    docCleanSectionProperties( &(bi->biSectProperties) );

	    break;

	case DOClevCELL:
	    break;

	case DOClevROW:
	    docCleanRowProperties( &(bi->biRowProperties) );
	    break;

	case DOClevPARA:
	    tp= bi->biParaParticules;
	    for ( i= 0; i < bi->biParaParticuleCount; tp++, i++ )
		{
		if  ( tp->tpKind == DOCkindFIELDSTART	||
		      tp->tpKind == DOCkindFIELDEND	)
		    {
		    if  ( tp->tpObjectNumber < 0			||
			  tp->tpObjectNumber >=
					bd->bdFieldList.dflFieldCount	)
			{
			LDEB(bi->biParaParticules[i].tpObjectNumber);
			LDEB(bd->bdFieldList.dflFieldCount);
			continue;
			}

		    docCleanField( bd->bdFieldList.dflFields+
							tp->tpObjectNumber );
		    docInitField( bd->bdFieldList.dflFields+
							tp->tpObjectNumber );
		    }

		if  ( tp->tpKind == DOCkindNOTE			&&
		      docDeleteNoteOfParticule( bd, bi, tp )	)
		    { LDEB(tp->tpStroff);	}
		}

	    for ( i= 0; i < bi->biParaObjectCount; i++ )
		{ docCleanObject( bd, bi->biParaObjects+ i ); }

	    if  ( bi->biParaString )
		{ free( bi->biParaString );	}
	    if  ( bi->biParaParticules )
		{ free( bi->biParaParticules );	}
	    if  ( bi->biParaObjects )
		{ free( bi->biParaObjects );	}
	    if  ( bi->biParaLines )
		{ free( bi->biParaLines );	}

	    docCleanParagraphProperties( &(bi->biParaProperties) );

	    break;

	default:
	    /*FALLTHROUGH*/
	case DOClevOUT:
	    LDEB(bi->biLevel);
	    break;
	}

    bi->biLevel= DOClevOUT;
    }

void docFreeItem(	BufferDocument *	bd,
			BufferItem *		bi )
    {
    docCleanItem( bd, bi );
    free( bi );
    }

/************************************************************************/
/*									*/
/*  Initialise a BufferItem.						*/
/*									*/
/************************************************************************/

void docInitItem(	BufferItem *		bi,
			BufferItem *		parent,
			const BufferDocument *	bd,
			int			numberInParent,
			int			level,
			int			inExternalItem )
    {
    bi->biChildren= (BufferItem **)0;
    bi->biChildCount= 0;
    bi->biLeftParagraphs= 0;

    switch( level )
	{
	case DOClevDOC:
	    break;

	case DOClevSECT:
	    docInitExternalItem( &(bi->biSectHeader) );
	    docInitExternalItem( &(bi->biSectFirstPageHeader) );
	    docInitExternalItem( &(bi->biSectLeftPageHeader) );
	    docInitExternalItem( &(bi->biSectRightPageHeader) );

	    docInitExternalItem( &(bi->biSectFooter) );
	    docInitExternalItem( &(bi->biSectFirstPageFooter) );
	    docInitExternalItem( &(bi->biSectLeftPageFooter) );
	    docInitExternalItem( &(bi->biSectRightPageFooter) );

	    docInitSectionProperties( &(bi->biSectProperties) );

	    if  ( bd )
		{ bi->biSectDocumentGeometry= bd->bdProperties.dpGeometry; }

	    docInitSelectionScope( &(bi->biSectSelectionScope) );

	    bi->biSectSelectionScope.ssInExternalItem= inExternalItem;

	    bi->biSectHeaderFooterUseForSectBi= (const BufferItem *)0;
	    bi->biSectHeaderFooterUseForPage= -1;

	    break;

	case DOClevCELL:
	    break;

	case DOClevROW:
	    bi->biRowTableHeaderRow= -1;
	    bi->biRowTableFirst= -1;
	    bi->biRowTablePast= -1;
	    bi->biRowPrecededByHeader= 0;

	    docInitRowProperties( &(bi->biRowProperties) );

	    docInitLayoutPosition( &(bi->biRowBelowAllPosition) );
	    docInitLayoutPosition( &(bi->biRowAboveHeaderPosition) );
	    break;

	case DOClevPARA:
	    bi->biParaStrlen= 0;
	    bi->biParaString= (unsigned char *)0;

	    bi->biParaParticuleCount= 0;
	    bi->biParaParticules= (TextParticule *)0;

	    bi->biParaLineCount= 0;
	    bi->biParaLines= (TextLine *)0;

	    bi->biParaObjectCount= 0;
	    bi->biParaObjects= (InsertedObject *)0;

	    bi->biParaAscentTwips= 0;
	    bi->biParaDescentTwips= 0;
	    bi->biParaLeadingTwips= 0;

	    bi->biParaBorderAboveParagraph= (const BorderProperties *)0;
	    bi->biParaBorderBelowParagraph= (const BorderProperties *)0;

	    docInitParagraphProperties( &(bi->biParaProperties) );

	    break;

	default:
	    bi->biLevel= DOClevOUT;
	    bi->biParent= (BufferItem *)0;
	    LDEB(level); return;
	}

    bi->biLevel= level;
    bi->biInExternalItem= inExternalItem;
    bi->biParent= parent;
    bi->biNumberInParent= numberInParent;

    docInitLayoutPosition( &(bi->biTopPosition) );
    docInitLayoutPosition( &(bi->biBelowPosition) );

    bi->biTopInsetTwips= 0;
    bi->biBottomInsetTwips= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  1)  Delete a series of items.					*/
/*  2)  Delete an item from its parent.					*/
/*									*/
/************************************************************************/

static void docSetSectHeadFootScopes(		BufferItem *	sectBi )
    {
    int		n= sectBi->biNumberInParent;

    if  ( sectBi->biSectHeader.eiItem )
	{
	sectBi->biSectHeader.eiItem->
			    biSectSelectionScope.ssSectNrExternalTo= n;
	}
    if  ( sectBi->biSectFirstPageHeader.eiItem )
	{
	sectBi->biSectFirstPageHeader.eiItem->
			    biSectSelectionScope.ssSectNrExternalTo= n;
	}
    if  ( sectBi->biSectLeftPageHeader.eiItem )
	{
	sectBi->biSectLeftPageHeader.eiItem->
			    biSectSelectionScope.ssSectNrExternalTo= n;
	}
    if  ( sectBi->biSectRightPageHeader.eiItem )
	{
	sectBi->biSectRightPageHeader.eiItem->
			    biSectSelectionScope.ssSectNrExternalTo= n;
	}

    if  ( sectBi->biSectFooter.eiItem )
	{
	sectBi->biSectFooter.eiItem->
			    biSectSelectionScope.ssSectNrExternalTo= n;
	}
    if  ( sectBi->biSectFirstPageFooter.eiItem )
	{
	sectBi->biSectFirstPageFooter.eiItem->
			    biSectSelectionScope.ssSectNrExternalTo= n;
	}
    if  ( sectBi->biSectLeftPageFooter.eiItem )
	{
	sectBi->biSectLeftPageFooter.eiItem->
			    biSectSelectionScope.ssSectNrExternalTo= n;
	}
    if  ( sectBi->biSectRightPageFooter.eiItem )
	{
	sectBi->biSectLeftPageFooter.eiItem->
			    biSectSelectionScope.ssSectNrExternalTo= n;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Paragraphs have been deleted from a group item.			*/
/*  Administration in the child array has been done. Fix		*/
/*  biLeftParagraphs in the item itself and its parents.		*/
/*									*/
/************************************************************************/

static void docParagraphsDeleted(	BufferItem *	bi,
					int		paragraphsDeleted )
    {
    while( bi->biParent )
	{
	int		first;
	int		f;

	first= bi->biNumberInParent;
	bi= bi->biParent;

	for ( f= first; f < bi->biChildCount; f++ )
	    { bi->biChildren[f]->biLeftParagraphs -= paragraphsDeleted;	}
	}

    bi->biLeftParagraphs -= paragraphsDeleted;

    return;
    }

/*  1  */
void docDeleteItems(	BufferDocument *	bd,
			BufferItem *		bi,
			int			first,
			int			count )
    {
    int		n;
    int		f;
    int		c;
    int		paragraphsDeleted= 0;

#   if VALIDATE_TREE
    if  ( docCheckItem( &(bd->bdItem) ) )
	{ LDEB(2); docListItem( 0, &(bd->bdItem) ); abort(); }
#   endif

    if  ( first > bi->biChildCount )
	{
	LLDEB(first,bi->biChildCount);
	first= bi->biChildCount;
	}

    if  ( first+ count > bi->biChildCount )
	{
	LLDEB(first+count,bi->biChildCount);
	count= bi->biChildCount- first;
	}

    if  ( count <= 0 )
	{ LDEB(count); return;	}

    if  ( count > 0 )
	{
	f= 0;
	if  ( first > 0 )
	    { f= bi->biChildren[first- 1]->biLeftParagraphs;	}

	c= bi->biChildren[first+ count- 1]->biLeftParagraphs;

	paragraphsDeleted= c- f;
	}

    n= first+ count- 1;
    while( n >= first )
	{ docFreeItem( bd, bi->biChildren[n] ); n--; }

    bi->biChildCount -= count;

    f= first;
    while( f < bi->biChildCount )
	{
	bi->biChildren[f]= bi->biChildren[f+ count];
	bi->biChildren[f]->biNumberInParent= f;
	bi->biChildren[f]->biLeftParagraphs -= paragraphsDeleted;

	if  ( bi->biChildren[f]->biLevel == DOClevSECT )
	    { docSetSectHeadFootScopes( bi->biChildren[f] ); }

	f++;
	}

    docParagraphsDeleted( bi, paragraphsDeleted );

#   if VALIDATE_TREE
    if  ( docCheckItem( &(bd->bdItem) ) )
	{ LDEB(2); docListItem( 0, &(bd->bdItem) ); abort(); }
#   endif

    return;
    }

/*  2  */
void docDeleteItem(	BufferDocument *	bd,
			BufferItem *		bi )
    {
    if  ( bi->biParent )
	{ docDeleteItems( bd, bi->biParent, bi->biNumberInParent, 1 );	}
    else{ docFreeItem( bd, bi );					}
    }

/************************************************************************/
/*									*/
/*  Paragraphs have been inserted into a group item.			*/
/*  Administration in the child array has been done. Fix		*/
/*  biLeftParagraphs in the item itself and its parents.		*/
/*									*/
/************************************************************************/

static void docParagraphsInserted(	BufferItem *	bi,
					int		paragraphsInserted )
    {
    while( bi->biParent )
	{
	int		first;
	int		f;

	first= bi->biNumberInParent;
	bi= bi->biParent;

	for ( f= first; f < bi->biChildCount; f++ )
	    { bi->biChildren[f]->biLeftParagraphs += paragraphsInserted; }
	}

    bi->biLeftParagraphs += paragraphsInserted;

    return;
    }

/************************************************************************/
/*									*/
/*  Add a new child to a parent.					*/
/*									*/
/************************************************************************/

BufferItem * docInsertItem(	const BufferDocument *	bd,
				BufferItem *		parent,
				int			n,
				ItemLevel		level )
    {
    int			i;

    int			newSize;

    BufferItem **	freshChildren;
    BufferItem *	bi;
    BufferItem *	rowBi;

    int			paragraphsInserted;

#   if VALIDATE_TREE
    if  ( docCheckItem( &(bd->bdItem) ) )
	{ LDEB(2); docListItem( 0, &(bd->bdItem) ); abort(); }
#   endif

    if  ( parent->biLevel != level- 1 )
	{
	SSDEB(docLevelStr(parent->biLevel),docLevelStr(level));
	return (BufferItem *)0;
	}

    switch( parent->biLevel )
	{
	case DOClevDOC:
	case DOClevSECT:
	case DOClevROW:
	case DOClevCELL:
	    break;

	default:
	    LDEB(parent->biLevel); return (BufferItem *)0;
	}

    if  ( n == -1 )
	{ n= parent->biChildCount;	}

    newSize= parent->biChildCount;

    if  ( newSize % 10 )
	{ newSize ++;		}
    else{ newSize += 10;	}

    newSize *= sizeof(BufferItem *);

    freshChildren= (BufferItem **)realloc( parent->biChildren, newSize );
    if  ( ! freshChildren )
	{ LXDEB(newSize,freshChildren); return (BufferItem *)0; }
    parent->biChildren= freshChildren;

    bi= (BufferItem *)malloc( sizeof(BufferItem) );
    if  ( ! bi )
	{ XDEB(bi); return bi;	}

    if  ( n == -1 )
	{ n= parent->biChildCount; }

    docInitItem( bi, parent, bd, n, level, parent->biInExternalItem );

    if  ( n == 0 )
	{ bi->biTopPosition= parent->biTopPosition;			}
    else{ bi->biTopPosition= freshChildren[n-1]->biBelowPosition;	}

    for ( i= parent->biChildCount; i > n; i-- )
	{
	freshChildren[i]= freshChildren[i-1];

	freshChildren[i]->biNumberInParent= i;

	if  ( freshChildren[i]->biLevel == DOClevSECT )
	    { docSetSectHeadFootScopes( freshChildren[i] ); }
	}

    freshChildren[n]= bi;
    parent->biChildCount++;

    if  ( level != DOClevPARA )
	{
	if  ( n > 0 )
	    { bi->biLeftParagraphs= freshChildren[n-1]->biLeftParagraphs; }

	return bi;
	}

    paragraphsInserted= 1;

    rowBi= bi;
    while( rowBi && rowBi->biLevel != DOClevROW )
	{ rowBi= rowBi->biParent;	}
    if  ( ! rowBi )
	{ XDEB(rowBi);						}
    else{ bi->biParaInTable= rowBi->biRowHasTableParagraphs;	}

    if  ( n > 0 )
	{ bi->biLeftParagraphs= freshChildren[n-1]->biLeftParagraphs; }

    for ( i= n; i < parent->biChildCount; i++ )
	{ parent->biChildren[i]->biLeftParagraphs += paragraphsInserted; }

    docParagraphsInserted( parent, paragraphsInserted );

#   if VALIDATE_TREE
    if  ( docCheckItem( &(bd->bdItem) ) )
	{ LDEB(2); docListItem( 0, &(bd->bdItem) ); abort(); }
#   endif

    return bi;
    }

/************************************************************************/
/*									*/
/*  Make an empty paragraph: Needed at several locations.		*/
/*									*/
/************************************************************************/

BufferItem * docInsertEmptyParagraph(
				BufferDocument *	bd,
				BufferItem *		bi,
				int			textAttributeNumber )
    {
    if  ( bi->biLevel < DOClevSECT )
	{ LDEB(bi->biLevel); return (BufferItem *)0;	}

    if  ( bi->biLevel < DOClevROW )
	{
	bi= docInsertItem( bd, bi, -1, DOClevROW );
	if  ( ! bi )
	    { XDEB(bi); return (BufferItem *)0;   }
	}

    if  ( bi->biLevel < DOClevCELL )
	{
	bi= docInsertItem( bd, bi, -1, DOClevCELL );
	if  ( ! bi )
	    { XDEB(bi); return (BufferItem *)0;   }
	}

    if  ( bi->biLevel < DOClevPARA )
	{
	bi= docInsertItem( bd, bi, -1, DOClevPARA );
	if  ( ! bi )
	    { XDEB(bi); return (BufferItem *)0;   }
	}
    else{
	bi= docInsertItem( bd, bi->biParent, -1, DOClevPARA );
	if  ( ! bi )
	    { XDEB(bi); return (BufferItem *)0;   }
	}

    if  ( ! docInsertTextParticule( bi, 0, 0, 0,
				    DOCkindTEXT, textAttributeNumber ) )
	{ LDEB(1); return (BufferItem *)0;	}

    if  ( docInflateTextString( bi, 0 ) )
	{ LDEB(1); return (BufferItem *)0;	}
    bi->biParaString[bi->biParaStrlen]= '\0';

    return bi;
    }

/************************************************************************/
/*									*/
/*  Copy a paragraph to make a new one.					*/
/*									*/
/*  1)  Copy paragraph item.						*/
/*  2)  Open a hole in references to the item to possibly receive some	*/
/*	to the fresh item.						*/
/*  3)  Do not copy a possible list text field from the original.	*/
/*  4)  Copy the contents of the original or insert a dummy particule.	*/
/*  5)  Finally adapt the properties of the target paragraph to those	*/
/*	of the original.						*/
/*									*/
/************************************************************************/

BufferItem * docCopyParaItem(	DocumentCopyJob *	dcj,
				EditOperation *		eo,
				const SelectionScope *	ssRoot,
				BufferItem *		biCellTo,
				int			n,
				BufferItem *		biParaFrom,
				int			inTable )
    {
    BufferItem *	biParaTo;

    const int		partTo= 0;
    int			partFrom= 0;

    int			particulesInserted= 0;
    int			charactersCopied= 0;

    PropertyMask	ppChgMask;
    PropertyMask	ppUpdMask;

    /*  1  */
    biParaTo= docInsertItem( dcj->dcjBdTo, biCellTo, n, DOClevPARA );
    if  ( ! biParaTo )
	{ XDEB(biParaTo); return biParaTo;	}

    biParaTo->biParaInTable= inTable;

    /*  2  */
    {
    int		paraNr;
    const int	stroff= 0;
    const int	sectShift= 0;
    const int	paraShift= 1;

    paraNr= docNumberOfParagraph( biParaTo );

    docEditShiftReferences( eo, ssRoot, paraNr, stroff,
				sectShift, paraShift, -stroff );
    }

    /*  3  */
    if  ( biParaFrom->biParaListOverride > 0 )
	{
	int		bulletFieldNr= -1;
	int		bulletPartBegin= -1;
	int		bulletPartEnd= -1;
	int		bulletStroffBegin= -1;
	int		bulletStroffEnd= -1;

	if  ( docDelimitParaHeadField( &bulletFieldNr,
			&bulletPartBegin, &bulletPartEnd,
			&bulletStroffBegin, &bulletStroffEnd,
			biParaFrom, dcj->dcjBdFrom ) )
	    { LDEB(1);	}

	if  ( partFrom <= bulletPartEnd )
	    { partFrom= bulletPartEnd+ 1;	}
	}

    /*  4  */
    if  ( partFrom < biParaFrom->biParaParticuleCount )
	{
	if  ( docCopyParticules( dcj, eo, biParaTo, biParaFrom, partTo,
			partFrom, biParaFrom->biParaParticuleCount- partFrom,
			&particulesInserted, &charactersCopied ) )
	    {
	    LDEB(biParaFrom->biParaParticuleCount);
	    docDeleteItem( dcj->dcjBdTo, biParaTo ); return (BufferItem *)0;
	    }
	}
    else{
	int			textAttributeNumberTo;
	const TextParticule *	tpFrom;

	tpFrom= biParaFrom->biParaParticules+
				    biParaFrom->biParaParticuleCount- 1;

	textAttributeNumberTo= docMapTextAttributeNumber( dcj,
					    tpFrom->tpTextAttributeNumber );
	if  ( textAttributeNumberTo < 0 )
	    { LDEB(textAttributeNumberTo); return (BufferItem *)0;	}

	if  ( ! docInsertTextParticule( biParaTo, 0, 0, 0,
					DOCkindTEXT, textAttributeNumberTo ) )
	    { LDEB(1); return (BufferItem *)0;	}

	if  ( docInflateTextString( biParaTo, 0 ) )
	    { LDEB(1); return (BufferItem *)0;	}
	biParaTo->biParaString[biParaTo->biParaStrlen]= '\0';
	}

    /*  5  */
    PROPmaskCLEAR( &ppChgMask );

    PROPmaskCLEAR( &ppUpdMask );
    utilPropMaskFill( &ppUpdMask, PPprop_COUNT );
    PROPmaskUNSET( &ppUpdMask, PPpropIN_TABLE );

    if  ( docEditUpdParaProperties( eo, &ppChgMask, biParaTo,
				&ppUpdMask, &(biParaFrom->biParaProperties),
				dcj->dcjColorMap, dcj->dcjListStyleMap ) )
	{ LDEB(1); return (BufferItem *)0;	}

    if  ( biParaTo->biParaListOverride > 0 )
	{ dcj->dcjBulletsCopied++;	}

    return biParaTo;
    }

BufferItem * docCopyRowItem(	DocumentCopyJob *	dcj,
				EditOperation *		eo,
				const SelectionScope *	ssRoot,
				BufferItem *		sectBiTo,
				int			n,
				BufferItem *		rowBiFrom,
				int			inTable )
    {
    BufferItem *	biRowTo;
    int			col;

    biRowTo= docInsertItem( dcj->dcjBdTo, sectBiTo, n, DOClevROW );
    if  ( ! biRowTo )
	{ XDEB(biRowTo); return biRowTo;	}

    if  ( docCopyRowProperties( &(biRowTo->biRowProperties),
			    &(rowBiFrom->biRowProperties), dcj->dcjColorMap ) )
	{ LDEB(1); return (BufferItem *)0;	}

    for ( col= 0; col < rowBiFrom->biChildCount; col++ )
	{
	BufferItem *	biCellFrom;
	BufferItem *	biCellTo;
	int		para;

	biCellFrom= rowBiFrom->biChildren[col];
	biCellTo= docInsertItem( dcj->dcjBdTo, biRowTo, col, DOClevCELL );

	if  ( ! biCellTo )
	    { XDEB(biCellTo); return (BufferItem *)0;	}

	for ( para= 0; para < biCellFrom->biChildCount; para++ )
	    {
	    BufferItem *	biParaFrom= biCellFrom->biChildren[para];

	    if  ( ! docCopyParaItem( dcj, eo, ssRoot,
				    biCellTo, para, biParaFrom, inTable ) )
		{ LDEB(para); return (BufferItem *)0;	}
	    }
	}

    return biRowTo;
    }

BufferItem * docCopySectItem(	DocumentCopyJob *	dcj,
				EditOperation *		eo,
				const SelectionScope *	ssRoot,
				BufferItem *		parentBiTo,
				int			n,
				BufferItem *		biSectFrom )
    {
    BufferItem *		sectBiTo;
    int				row;

    DocumentPosition		dp;

    if  ( parentBiTo )
	{
	if  ( parentBiTo->biInExternalItem != ssRoot->ssInExternalItem )
	    {
	    LDEB(parentBiTo->biInExternalItem);
	    LDEB(ssRoot->ssInExternalItem);
	    return (BufferItem *)0;
	    }

	sectBiTo= docInsertItem( dcj->dcjBdTo, parentBiTo, n, DOClevSECT );
	if  ( ! sectBiTo )
	    { XDEB(sectBiTo); return (BufferItem *)0;	}
	}
    else{
	n= 0;
	sectBiTo= (BufferItem *)malloc( sizeof(BufferItem) );

	if  ( ! sectBiTo )
	    { XDEB(sectBiTo); return (BufferItem *)0;	}

	docInitItem( sectBiTo, (BufferItem *)0, dcj->dcjBdTo, n,
				DOClevSECT, ssRoot->ssInExternalItem );
	}

    sectBiTo->biSectSelectionScope= *ssRoot;

    if  ( docCopySectionProperties( &(sectBiTo->biSectProperties),
					&(biSectFrom->biSectProperties) ) )
	{
	LDEB(1);
	docDeleteItem( dcj->dcjBdTo, sectBiTo );
	return (BufferItem *)0;
	}

    for ( row= 0; row < biSectFrom->biChildCount; row++ )
	{
	BufferItem *	rowBiFrom= biSectFrom->biChildren[row];
	BufferItem *	rowBiTo;

	rowBiTo= docCopyRowItem( dcj, eo, ssRoot,
					sectBiTo, row, rowBiFrom,
					rowBiFrom->biRowHasTableParagraphs );
	if  ( ! rowBiTo )
	    {
	    XDEB(rowBiTo);
	    docDeleteItem( dcj->dcjBdTo, sectBiTo );
	    return (BufferItem *)0;
	    }
	}

    if  ( docLastPosition( &dp, sectBiTo ) )
	{ LDEB(1);	}
    else{
	int		paraNr;
	const int	stroff= 0;
	const int	sectShift= 1;
	const int	paraShift= 0;

	paraNr= docNumberOfParagraph( dp.dpBi )+ 1;

	docEditShiftReferences( eo, ssRoot, paraNr, stroff,
				    sectShift, paraShift, -stroff );
	}

    return sectBiTo;
    }

/************************************************************************/
/*									*/
/*  Insert a new row in a table.					*/
/*									*/
/************************************************************************/

BufferItem * docInsertRowItem(	BufferDocument *	bd,
				BufferItem *		sectBi,
				int			n,
				const RowProperties *	rp,
				int			textAttributeNumber )
    {
    int				col;

    BufferItem *		rowBi= (BufferItem *)0;

    rowBi= docInsertItem( bd, sectBi, n, DOClevROW );
    if  ( ! rowBi )
	{ XDEB(rowBi); return (BufferItem *)0;	}

    for ( col= 0; col < rp->rpCellCount; col++ )
	{
	BufferItem *	paraBi;

	paraBi= docInsertEmptyParagraph( bd, rowBi, textAttributeNumber );
	if  ( ! paraBi )
	    {
	    XDEB(paraBi);
	    docDeleteItem( bd, rowBi );
	    return (BufferItem *)0;
	    }

	paraBi->biParaInTable= 1;
	rowBi->biRowHasTableParagraphs= 1;
	}

    if  ( docCopyRowProperties( &(rowBi->biRowProperties),
						    rp, (const int *)0 ) )
	{
	LDEB(1);
	docDeleteItem( bd, rowBi );
	return (BufferItem *)0;
	}

    return rowBi;
    }

/************************************************************************/
/*									*/
/*  Step in insert row actions: Copy attributes.			*/
/*									*/
/*  NOTE: Copying the text attribute numbers means that the rows MUST	*/
/*	be in the same document. The same applies for the color numbers	*/
/*	in the paragraph properties.					*/
/*									*/
/************************************************************************/

int docCopyRowColumnAttributes(	BufferItem *		rowBi,
				const BufferItem *	refRowBi )
    {
    int		colCount;
    int		col;

    if  ( rowBi->biChildCount < refRowBi->biChildCount )
	{ colCount= rowBi->biChildCount;	}
    else{ colCount= refRowBi->biChildCount;	}

    for ( col= 0; col < colCount; col++ )
	{
	DocumentPosition	dp;
	DocumentPosition	dpRef;

	TextParticule *		tp;
	const TextParticule *	tpRef;

	PropertyMask		ppChgMask;
	PropertyMask		ppUpdMask;

	const int * const	colorMap= (const int *)0;
	const int * const	listStyleMap= (const int *)0;

	if  ( docFirstPosition( &dp, rowBi->biChildren[col] ) )
	    { continue;	}
	if  ( docFirstPosition( &dpRef, refRowBi->biChildren[col] ) )
	    { continue;	}

	PROPmaskCLEAR( &ppChgMask );

	PROPmaskCLEAR( &ppUpdMask );
	utilPropMaskFill( &ppUpdMask, PPprop_COUNT );

	PROPmaskUNSET( &ppUpdMask, PPpropLISTOVERRIDE );
	PROPmaskUNSET( &ppUpdMask, PPpropOUTLINELEVEL );
	PROPmaskUNSET( &ppUpdMask, PPpropLISTLEVEL );

	if  ( docUpdParaProperties( &ppChgMask, &(dp.dpBi->biParaProperties),
				&ppUpdMask, &(dpRef.dpBi->biParaProperties),
				colorMap, listStyleMap ) )
	    { LDEB(1); return -1;	}

	tp= dp.dpBi->biParaParticules;
	tpRef= dpRef.dpBi->biParaParticules;

	tp->tpTextAttributeNumber= tpRef->tpTextAttributeNumber;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Split an item with childen.						*/
/*									*/
/*  1)  See whether there is a parent of the given level that can be	*/
/*	split.								*/
/*									*/
/************************************************************************/

static int docSplitGroupItemLow(	const BufferDocument *	bd,
					BufferItem **		pNewBi,
					BufferItem *		oldBi,
					int			n )
    {
    BufferItem *	newBi;
    int			i;
    int			prev;

    newBi= docInsertItem( bd, oldBi->biParent,
				oldBi->biNumberInParent, oldBi->biLevel );
    if  ( ! newBi )
	{ XDEB(newBi); return -1;	}

    switch( oldBi->biLevel )
	{
	case DOClevSECT:
	    if  ( docCopySectionProperties( &newBi->biSectProperties,
					    &oldBi->biSectProperties ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevCELL:
	    break;

	case DOClevROW:
	    if  ( oldBi->biRowHasTableParagraphs )
		{
		LLDEB(oldBi->biLevel,oldBi->biRowHasTableParagraphs);
		return -1;
		}

	    if  ( docCopyRowProperties( &(newBi->biRowProperties),
				&(oldBi->biRowProperties), (const int *)0 ) )
		{ LDEB(1); return -1;	}
	    break;

	default:
	    LDEB(oldBi->biLevel); return -1;
	}

    newBi->biChildren= (BufferItem **)malloc( n* sizeof(BufferItem *) );
    if  ( ! newBi->biChildren )
	{ XDEB(newBi->biChildren); return -1;	}

    for ( i= 0; i < n; i++ )
	{
	newBi->biChildren[i]= oldBi->biChildren[i];
	newBi->biChildren[i]->biParent= newBi;
	}

    prev= 0;
    if  ( newBi->biNumberInParent > 0 )
	{
	prev= newBi->biParent->biChildren[newBi->biNumberInParent-1]->
							biLeftParagraphs;
	}

    if  ( n == 0 )
	{ newBi->biLeftParagraphs= prev; }
    else{
	newBi->biLeftParagraphs= prev+ newBi->biChildren[n-1]->biLeftParagraphs;
	}

    newBi->biChildCount= n;
    oldBi->biChildCount -= n;

    prev= 0;
    for ( i= 0; i < oldBi->biChildCount; i++ )
	{
	BufferItem *	child= oldBi->biChildren[i+ n];

	oldBi->biChildren[i]= child;
	child->biNumberInParent -= n;

	if  ( oldBi->biChildren[i]->biLevel == DOClevPARA )
	    { prev++;	}
	else{
	    if  ( child->biChildCount > 0 )
		{
		prev += child->biChildren[child->biChildCount-1]->
							    biLeftParagraphs;
		}
	    }

	oldBi->biChildren[i]->biLeftParagraphs= prev;
	}

    *pNewBi= newBi; return 0;
    }

int docSplitGroupItem(	const BufferDocument *	bd,
			BufferItem **		pNewBi,
			BufferItem **		pParentBi,
			BufferItem *		parentBi,
			int			n,
			int			level )
    {
    BufferItem *	newBi= (BufferItem *)0;
    BufferItem *	splitBi;

#   if VALIDATE_TREE
    SDEB(docLevelStr(parentBi->biLevel));
    if  ( docCheckItem( &(bd->bdItem) ) )
	{ LDEB(2); docListItem( 0, &(bd->bdItem) ); abort(); }
#   endif

    splitBi= parentBi;
    while( splitBi && splitBi->biLevel != level- 1 )
	{ splitBi= splitBi->biParent; }
    if  ( ! splitBi )
	{
	SSXDEB(docLevelStr(parentBi->biLevel),docLevelStr(level),splitBi);
	return -1;
	}

    if  ( n < 0 || n >= parentBi->biChildCount )
	{ LLDEB(n,parentBi->biChildCount); return -1;	}

    for (;;)
	{
	if  ( n > 0 || parentBi->biLevel == level )
	    {
	    if  ( docSplitGroupItemLow( bd, &newBi, parentBi, n ) )
		{ LDEB(n); return -1;	}

	    if  ( parentBi->biLevel == level )
		{ break;	}
	    }

	n= parentBi->biNumberInParent;
	parentBi= parentBi->biParent;
	}

#   if VALIDATE_TREE
    SDEB(docLevelStr(parentBi->biLevel));
    if  ( docCheckItem( &(bd->bdItem) ) )
	{ LDEB(2); docListItem( 0, &(bd->bdItem) ); abort(); }
#   endif

    *pNewBi= newBi;
    *pParentBi= parentBi;
    return 0;
    }

int docMergeGroupItems(		BufferItem *	to,
				BufferItem *	from )
    {
    BufferItem **	freshChildren;
    int			f;
    int			t;
    int			left;
    int			prev;
    int			paragraphsMoved;

    if  ( to == from )
	{ XXDEB(to,from); return -1;	}
    if  ( to->biLevel != from->biLevel )
	{ LLDEB(to->biLevel,from->biLevel); return -1;	}

    if  ( from->biChildCount == 0 )
	{ return 0;	}

#   if VALIDATE_TREE
    SSDEB(docLevelStr(to->biLevel),docLevelStr(from->biLevel));
    if  ( docCheckRootItem( to ) )
	{ LDEB(2); docListRootItem( 0, to ); abort(); }
    if  ( docCheckRootItem( from ) )
	{ LDEB(2); docListRootItem( 0, from ); abort(); }
#   endif

    freshChildren= realloc( to->biChildren,
	    ( to->biChildCount+ from->biChildCount )* sizeof(BufferItem *) );
    if  ( ! freshChildren )
	{
	LLXDEB(to->biChildCount,from->biChildCount,freshChildren);
	return -1;
	}
    to->biChildren= freshChildren;

    if  ( from->biParent && from->biNumberInParent > 0 )
	{
	BufferItem *	prevBi;

	prevBi= from->biParent->biChildren[from->biNumberInParent- 1];
	paragraphsMoved= from->biLeftParagraphs- prevBi->biLeftParagraphs;
	}
    else{
	paragraphsMoved= from->biLeftParagraphs;
	}

    t= to->biChildCount;
    left= to->biLeftParagraphs; prev= 0;
    for ( f= 0; f < from->biChildCount; t++, f++ )
	{
	int	count;

	freshChildren[t]= from->biChildren[f];
	freshChildren[t]->biParent= to;
	freshChildren[t]->biNumberInParent= t;

	count= freshChildren[t]->biLeftParagraphs- prev;
	prev= freshChildren[t]->biLeftParagraphs;
	left += count;
	freshChildren[t]->biLeftParagraphs= left;

	if  ( freshChildren[t]->biLevel == DOClevSECT )
	    { docSetSectHeadFootScopes( freshChildren[t] ); }
	}

    to->biChildCount += from->biChildCount;
    from->biChildCount= 0;

    docParagraphsInserted( to, paragraphsMoved );
    docParagraphsDeleted( from, paragraphsMoved );

#   if VALIDATE_TREE
    SSDEB(docLevelStr(to->biLevel),docLevelStr(from->biLevel));
    if  ( docCheckRootItem( to ) )
	{ LDEB(2); docListRootItem( 0, to ); abort(); }
    if  ( docCheckRootItem( from ) )
	{ LDEB(2); docListRootItem( 0, from ); abort(); }
#   endif

    return 0;
    }

/************************************************************************/
/*									*/
/*  Return the number of a paragraph.					*/
/*									*/
/************************************************************************/

int docNumberOfParagraph(	const BufferItem *	bi )
    {
    int		n= 0;

    if  ( bi->biLevel != DOClevPARA )
	{ SDEB(docLevelStr(bi->biLevel)); return -1;	}

    n= bi->biLeftParagraphs;

    while( bi->biParent )
	{
	bi= bi->biParent;

	if  ( bi->biParent && bi->biNumberInParent > 0 )
	    {
	    if  ( bi->biNumberInParent >= bi->biParent->biChildCount )
		{ LLDEB(bi->biNumberInParent,bi->biParent->biChildCount); }

	    n += bi->biParent->biChildren[
				bi->biNumberInParent- 1]->biLeftParagraphs;
	    }
	}

    return n;
    }

BufferItem * docGetParagraphByNumber(	BufferItem *	bi,
					int		paraNr )
    {
    if  ( bi->biParent )
	{ XDEB(bi->biParent); return (BufferItem *)0;	}

    while( bi->biChildCount > 0 )
	{
	int		i;

	for ( i= 0; i < bi->biChildCount; i++ )
	    {
	    if  ( bi->biChildren[i]->biLeftParagraphs >= paraNr )
		{ break;	}
	    }

	if  ( i >= bi->biChildCount )
	    {
	    /* LLSDEB(paraNr,bi->biChildCount,docLevelStr(bi->biLevel)); */
	    return (BufferItem *)0;
	    }

	if  ( i > 0 )
	    { paraNr -= bi->biChildren[i-1]->biLeftParagraphs;	}

	bi= bi->biChildren[i];
	}

    if  ( bi->biLevel != DOClevPARA )
	{ SDEB(docLevelStr(bi->biLevel)); return (BufferItem *)0;	}

    if  ( paraNr != 1 )
	{ LDEB(paraNr); return (BufferItem *)0; }

    return bi;
    }

/************************************************************************/
/*									*/
/*  Change the kind of external item for a node and all its children.	*/
/*									*/
/************************************************************************/

void docSetExternalItemKind(	BufferItem *		bi,
				int			extItKind )
    {
    int		i;

    bi->biInExternalItem= extItKind;

    if  ( bi->biLevel == DOClevSECT )
	{ bi->biSectSelectionScope.ssInExternalItem= extItKind; }

    for ( i= 0; i < bi->biChildCount; i++ )
	{ docSetExternalItemKind( bi->biChildren[i], extItKind );	}

    return;
    }

