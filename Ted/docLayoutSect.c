/************************************************************************/
/*									*/
/*  Layout of a section.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   include	<appImage.h>
#   include	"docLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Adjust the position of the top of a section, depending on the kind	*/
/*  of break								*/
/*									*/
/*  1)  Note that even and odd are swapped as from the users point of	*/
/*	view page numbers count from one.				*/
/*  2)  As this routine is only called for the body of the document,	*/
/*	page wraps can be performed unconditionally.			*/
/*									*/
/************************************************************************/

void docLayoutPlaceSectTop(		BufferItem *		sectBi,
					BlockFrame *		bf,
					LayoutJob *		lj )
    {
    if  ( sectBi->biSectBreakKind != DOCsbkNONE		||
	  ( sectBi->biParent			&&
	    sectBi->biNumberInParent == 0	)	)
	{
	if  ( sectBi->biNumberInParent == 0 )
	    {
	    lj->ljPosition.lpPage= 0;
	    lj->ljPosition.lpColumn= 0;
	    lj->ljPosition.lpAtTopOfColumn= 1;

	    docLayoutSectColumnTop( sectBi, lj->ljBd, &(lj->ljPosition), bf );
	    }
	else{
	    const int		belowText= 0;
	    LayoutPosition	lpBelowNotes;

#	    if 0
	    /*
	    Though this would place the section at the correct position, 
	    giving the initial position is the responsibility of the caller. 
	    The same applies for the then branch. As I did not want to 
	    retest that branch, I have not changed it. (MdD Oct 2004)
	    */

	    const BufferItem *	docBi;
	    const BufferItem *	prevSectBi;

	    docBi= sectBi->biParent;
	    prevSectBi= docBi->biChildren[sectBi->biNumberInParent- 1];

	    lj->ljPosition.lpPage= prevSectBi->biBelowPosition.lpPage;
	    lj->ljPosition.lpColumn= 0;
	    lj->ljPosition.lpAtTopOfColumn= 0;
#	    endif

	    if  ( BF_HAS_FOOTNOTES( bf )				&&
		  docLayoutFootnotesForColumn( &lpBelowNotes, bf,
				  belowText, &(lj->ljPosition), lj )	)
		{ LDEB(1); return;	}

	    docLayoutToNextColumn( sectBi, lj->ljBd, &(lj->ljPosition), bf );
	    }

	/*  1  */
	if  ( ( sectBi->biSectBreakKind == DOCsbkEVEN	&&
		! ( lj->ljPosition.lpPage % 2 )		)	||
	      ( sectBi->biSectBreakKind == DOCsbkODD	&&
		( lj->ljPosition.lpPage % 2 )		)	)
	    {
	    docLayoutToNextColumn( sectBi, lj->ljBd, &(lj->ljPosition), bf );
	    }
	}

    sectBi->biTopPosition= lj->ljPosition;

    if  ( sectBi->biNumberInParent == 0		&&
	  sectBi->biParent			)
	{ sectBi->biParent->biTopPosition= lj->ljPosition; }

    return;
    }

/************************************************************************/
/*									*/
/*  Do the layout of a section.						*/
/*									*/
/************************************************************************/

int docLayoutSectItem(			BufferItem *		sectBi,
					BlockFrame *		bf,
					LayoutJob *		lj )
    {
    BufferDocument *		bd= lj->ljBd;
    const DocumentProperties *	dp= &(bd->bdProperties);
    const NotesProperties *	npEndnotes= &(dp->dpEndnoteProperties);

    DocumentNote *		dn;

    const SectionProperties *	sp= &(sectBi->biSectProperties);
    const DocumentGeometry *	dgSect= &(sp->spDocumentGeometry);

    int				i;

    /**/
    docSectDelimitTables( sectBi );

    /**/

    if  ( docSectHeaderFooterPrelayout( sectBi, lj ) )
	{ LDEB(1); return -1;	}

    /**/

    dn= bd->bdNotes;
    for ( i= 0; i < bd->bdNoteCount; dn++, i++ )
	{
	ExternalItem *	ei= (&dn->dnExternalItem);

	if  ( dn->dnSectNr != sectBi->biNumberInParent )
	    { continue;	}
	if  ( ! ei->eiItem )
	    { continue;	}

	if  ( docExternalItemPrelayout( ei, dgSect, lj ) )
	    { LDEB(1); return -1;	}
	}

    /**/

    docLayoutPlaceSectTop( sectBi, bf, lj );

    for ( i= 0; i < sectBi->biChildCount; i++ )
	{
	if  ( docLayoutItemImplementation( sectBi->biChildren[i], bf, lj ) )
	    { LDEB(i); return -1;	}
	}

    if  ( sectBi->biChildCount > 0 )
	{ sectBi->biTopPosition=  sectBi->biChildren[0]->biTopPosition; }

    /**/

    if  ( sectBi->biInExternalItem == DOCinBODY		&&
	  npEndnotes->npPosition == FTN_POS_SECT_END	)
	{
	if  ( docLayoutEndnotesForSection( sectBi->biNumberInParent, bf, lj ) )
	    { LDEB(sectBi->biNumberInParent); return -1;	}
	}

    return 0;
    }

