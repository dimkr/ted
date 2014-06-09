/************************************************************************/
/*									*/
/*  Buffer: Manipulation of item tree.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>

#   include	"docBuf.h"
#   include	"docEdit.h"

/************************************************************************/
/*									*/
/*  The current selection consisted of several paragraphs. An edit	*/
/*  operation was executed on the first one.. The rest is to be		*/
/*  deleted.								*/
/*									*/
/************************************************************************/

static void docDeleteEmptyParents(	EditOperation *		eo,
					int *			pSectsDeleted,
					BufferItem *		bi )
    {
    BufferDocument *	bd= eo->eoBd;
    int			sectionsDeleted= 0;

    while( bi && bi->biChildCount == 0 )
	{
	int		numberInParent= bi->biNumberInParent;
	BufferItem *	parent= bi->biParent;

	if  ( ! parent )
	    { LXDEB(bi->biNumberInParent,bi->biParent); break;	}

	if  ( bi->biLevel == DOClevSECT )
	    { sectionsDeleted++;	}

	docDeleteItem( bd, bi );

	if  ( parent				&&
	      parent->biLevel == DOClevROW	&&
	      parent->biRowHasTableParagraphs	)
	    {
	    if  ( docDeleteColumnsFromRow( &(parent->biRowProperties),
							numberInParent, 1 ) )
		{ LDEB(numberInParent);	}
	    }

	bi= parent;
	if  ( ! bi )
	    { break;	}

	if  ( bi->biLevel == DOClevSECT	&&
	      bi->biChildCount > 0		)
	    { docSectDelimitTables( bi );	}
	}

    *pSectsDeleted= sectionsDeleted;
    return;
    }

int docRemoveSelectionTail(	EditOperation *			eo,
				const DocumentSelection *	ds )
    {
    BufferItem *	lastParaBi;
    int			paraShift= 0;
    int			sectShift= 0;

    int			firstDeleted= -1;
    int			parentCount= 0;
    int			mergeTail= 0;

    lastParaBi= ds->dsEnd.dpBi;

    for (;;)
	{
	BufferItem *	parent= lastParaBi->biParent;
	BufferItem *	prevParaBi= docPrevParagraph( lastParaBi );

	int		sectionsDeleted= 0;
	int		firstParaDeleted= -1;
	int		paragraphsDeleted= 0;

	int		from;
	int		count;

	while( prevParaBi != ds->dsBegin.dpBi			&&
	       prevParaBi->biParent == lastParaBi->biParent	)
	    { prevParaBi= docPrevParagraph( prevParaBi );	}

	if  ( prevParaBi->biParent == parent )
	    { from= prevParaBi->biNumberInParent+ 1;	}
	else{
	    from= 0;

	    if  ( parentCount == 0					&&
		  lastParaBi->biNumberInParent < parent->biChildCount	)
		{ mergeTail= 1;	}
	    }

	count= lastParaBi->biNumberInParent- from+ 1;

	parentCount++;

	docEditDeleteItems( eo, &sectionsDeleted,
					&firstParaDeleted, &paragraphsDeleted,
					parent, from, count );

	if  ( parent->biLevel == DOClevROW	&&
	      parent->biRowHasTableParagraphs	)
	    {
	    if  ( docDeleteColumnsFromRow( &(parent->biRowProperties),
								from, count ) )
		{ LLDEB(from,count);	}
	    }

	sectShift -= sectionsDeleted;
	paraShift -= paragraphsDeleted;
	if  ( firstParaDeleted >= 0 )
	    {
	    if  ( firstDeleted < 0 || firstDeleted > firstParaDeleted )
		{ firstDeleted= firstParaDeleted;	}
	    }

	if  ( parent->biChildCount > 0 )
	    { docEditIncludeItemInReformatRange( eo, parent );	}

	sectionsDeleted= 0;
	docDeleteEmptyParents( eo, &sectionsDeleted, parent );

	sectShift -= sectionsDeleted;

	if  ( prevParaBi == ds->dsBegin.dpBi )
	    { break;	}

	lastParaBi= prevParaBi;
	if  ( ! lastParaBi )
	    { XDEB(lastParaBi); break;	}
	}

    if  ( mergeTail )
	{
	lastParaBi= docNextParagraph( ds->dsBegin.dpBi );
	if  ( lastParaBi )
	    {
	    int			sectionsDeleted;
	    BufferItem *	lastParent;

	    lastParent= lastParaBi->biParent;

	    if  ( docMergeGroupItems( ds->dsBegin.dpBi->biParent, lastParent ) )
		{ LDEB(mergeTail); return -1;	}

	    sectionsDeleted= 0;
	    docDeleteEmptyParents( eo, &sectionsDeleted, lastParent );
	    sectShift -= sectionsDeleted;
	    }
	}

    if  ( sectShift != 0 )
	{
	const int	paraNr= firstDeleted+ 1;
	const int	stroffFrom= 0;
	const int	stroffShift= 0;
	const int	paraShift= 0;

	eo->eoSectionsDeleted -= sectShift;

	docEditShiftReferences( eo, &(eo->eoSelectionScope),
				    paraNr, stroffFrom,
				    sectShift, paraShift, stroffShift );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Delete some buffer items but first close the objects in them.	*/
/*									*/
/*  1)  Preliminary administration for shifting the bottom of the range	*/
/*	of paragraphs that are to be reformatted.			*/
/*  2)  Include the whole of the parent in the range of items to be	*/
/*	reformatted.							*/
/*									*/
/************************************************************************/

void docEditDeleteItems(	EditOperation *		eo,
				int *			pSectionsDeleted,
				int *			pFirstParaDeleted,
				int *			pParagraphsDeleted,
				BufferItem *		parentBi,
				int			first,
				int			count )
    {
    int		i;

    int		firstParaDeleted= -1;
    int		bulletsDeleted= 0;
    int		sectionsDeleted= 0;
    int		paragraphsDeleted= 0;
    int		notesDeleted= 0;

    if  ( count > 0 )
	{
	DocumentPosition	dp;

	/*  1  */
	if  ( docFirstPosition( &dp, parentBi->biChildren[first] ) )
	    { LDEB(1);							}
	else{ firstParaDeleted= docNumberOfParagraph( dp.dpBi );	}
	}

    for ( i= first+ count- 1; i >= first; i-- )
	{
	docCleanItemObjects( &notesDeleted, &bulletsDeleted, &paragraphsDeleted,
					eo->eoBd, parentBi->biChildren[i],
					eo->eoVoidadd, eo->eoCloseObject );

	if  ( parentBi->biChildren[i]->biLevel == DOClevSECT )
	    { eo->eoSectionsDeleted++;	}
	}

    eo->eoNotesDeleted += notesDeleted;

    docDeleteItems( eo->eoBd, parentBi, first, count );

    if  ( parentBi->biLevel == DOClevDOC )
	{ sectionsDeleted= count;	}

    if  ( count > 0 )
	{
	const int		sectShift= -sectionsDeleted;
	const int		stroffFrom= 0;
	const int		stroffShift= 0;

	if  ( firstParaDeleted < 0 )
	    { LDEB(firstParaDeleted);	}
	else{
	    docEditShiftReferences( eo, &(eo->eoSelectionScope),
			    firstParaDeleted+ paragraphsDeleted, stroffFrom,
			    sectShift, -paragraphsDeleted, stroffShift );
	    }

	/*  Must be done by caller!
	docEditIncludeItemInReformatRange( eo, parentBi );
	*/
	}

    *pSectionsDeleted= sectionsDeleted;
    *pFirstParaDeleted= firstParaDeleted;
    eo->eoBulletsChanged += bulletsDeleted;
    *pParagraphsDeleted= paragraphsDeleted;
    return;
    }

