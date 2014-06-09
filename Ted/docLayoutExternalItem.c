/************************************************************************/
/*									*/
/*  Geometry calculations about external items.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<limits.h>
#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	"docLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Determine the box around a header or a footer.			*/
/*									*/
/************************************************************************/

int docGetExternalItemBox(	DocumentRectangle *		dr,
				const BufferItem *		bodySectBi,
				const ExternalItem *		ei,
				int				justUsed,
				int				page,
				BufferDocument *		bd,
				AppDrawingData *		add )
    {
    const DocumentProperties *	dp;
    const SectionProperties *	sp;
    const DocumentGeometry *	dgRef;

    const BufferItem *		externSectBi= ei->eiItem;

    DocumentRectangle		drBox;

    int				pageTopPixels;

    if  ( ! externSectBi )
	{ XDEB(externSectBi); return -1;	}

    pageTopPixels= page* add->addPageStepPixels;

    switch( externSectBi->biInExternalItem )
	{
	case DOCinSECT_HEADER:
	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:
	case DOCinSECT_FOOTER:
	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:

	    sp= &(bodySectBi->biSectProperties);
	    dgRef= &(sp->spDocumentGeometry);

	    drBox.drX0= X_PIXELS( add, dgRef->dgLeftMarginTwips );
	    drBox.drX1= X_PIXELS( add, dgRef->dgPageWideTwips-
						dgRef->dgRightMarginTwips );

	    if  ( justUsed )
		{
		drBox.drY0= pageTopPixels+
				Y_PIXELS( add, ei->eiY0UsedTwips );
		drBox.drY1= pageTopPixels+
				Y_PIXELS( add, ei->eiY1UsedTwips );
		}
	    else{
		drBox.drY0= pageTopPixels+
				Y_PIXELS( add, ei->eiY0ReservedTwips );
		drBox.drY1= pageTopPixels+
				Y_PIXELS( add, ei->eiY1ReservedTwips );
		}

	    *dr= drBox; return 0;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	    sp= &(bodySectBi->biSectProperties);
	    dgRef= &(sp->spDocumentGeometry);

	    drBox.drX0= X_PIXELS( add, dgRef->dgLeftMarginTwips );
	    drBox.drX1= X_PIXELS( add, dgRef->dgPageWideTwips-
						dgRef->dgRightMarginTwips );

	    {
	    DocumentPosition	dpTop;
	    DocumentPosition	dpBot;

	    int			lineTop;
	    int			lineBot;

	    int			partTop;
	    int			partBot;

	    const TextLine *	tlTop;
	    const TextLine *	tlBot;

	    const int		column= 0;

	    if  ( docGetFirstInColumnForItem( &dpTop, &lineTop, &partTop,
						ei->eiItem, page, column ) )
		{ LDEB(page); return -1;	}

	    if  ( docGetLastInColumnForItem( &dpBot, &lineBot, &partBot,
						ei->eiItem, page, column ) )
		{ LDEB(page); return -1;	}

	    tlTop= dpTop.dpBi->biParaLines+ lineTop;
	    tlBot= dpBot.dpBi->biParaLines+ lineBot;

	    drBox.drY0= TL_TOP_PIXELS( add, tlTop );
	    drBox.drY1= TL_BELOW_PIXELS( add, tlBot );
	    }

	    *dr= drBox; return 0;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    if  ( page != ei->eiPageFormattedFor )
		{ LLDEB(page,ei->eiPageFormattedFor);	}

	    dp= &(bd->bdProperties);
	    dgRef= &(dp->dpGeometry);

	    drBox.drX0= X_PIXELS( add, dgRef->dgLeftMarginTwips );
	    drBox.drX1= X_PIXELS( add, dgRef->dgPageWideTwips-
						dgRef->dgRightMarginTwips );

	    drBox.drY0= pageTopPixels+
				Y_PIXELS( add, ei->eiY0UsedTwips );
	    drBox.drY1= pageTopPixels+
				Y_PIXELS( add, ei->eiY1UsedTwips );

	    *dr= drBox; return 0;

	default:
	    LDEB(externSectBi->biInExternalItem);
	    return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Do a preliminary layout of an external item.			*/
/*  Inside the external item, geometry is correct. Some items are	*/
/*  however used in different positions and here we just calculate the	*/
/*  layout in order to use the size of the item in geometry		*/
/*  calculations about the document as a whole.				*/
/*									*/
/*  1)  Remove the bottom of the page master frame. This is the routine	*/
/*	that calculates it for future use.				*/
/*									*/
/************************************************************************/

int docExternalItemPrelayout(		ExternalItem *			ei,
					const DocumentGeometry *	dgRef,
					LayoutJob *			lj )
    {
    LayoutJob			headerLj;
    LayoutPosition		headerLp;

    int				high;
    int				y1;

    BlockFrame			bf;

    ei->eiPageFormattedFor= -1;

    if  ( ! ei->eiItem )
	{ return 0;	}

    docSectDelimitTables( ei->eiItem );

    ei->eiItem->biSectDocumentGeometry.dgPageWideTwips=
						dgRef->dgPageWideTwips;
    ei->eiItem->biSectDocumentGeometry.dgLeftMarginTwips=
						dgRef->dgLeftMarginTwips;
    ei->eiItem->biSectDocumentGeometry.dgRightMarginTwips=
						dgRef->dgRightMarginTwips;

    headerLp.lpPage= ei->eiItem->biTopPosition.lpPage;
    headerLp.lpColumn= 0;
    headerLp.lpPageYTwips= dgRef->dgHeaderPositionTwips;
    headerLp.lpAtTopOfColumn= 1; /* not really */

    headerLj= *lj;
    headerLj.ljChangedRectanglePixels= (DocumentRectangle *)0;
    headerLj.ljAdd= lj->ljAdd;
    headerLj.ljBd= lj->ljBd;
    headerLj.ljScreenFontList= lj->ljScreenFontList;
    headerLj.ljChangedItem= lj->ljChangedItem;
    headerLj.ljPosition= headerLp;

    docBlockFrameTwips( &bf, ei->eiItem, lj->ljBd,
					headerLp.lpPage, headerLp.lpColumn );
    bf.bfY0Twips= headerLp.lpPageYTwips;
    bf.bfYBelowShapes= headerLp.lpPageYTwips;

    /*  1  */
    bf.bfY1Twips= dgRef->dgPageHighTwips;

    if  ( docLayoutItemImplementation( ei->eiItem, &bf, &headerLj ) )
	{ LDEB(1); return -1;	}

    y1= ei->eiItem->biBelowPosition.lpPageYTwips;

    switch( ei->eiItem->biInExternalItem )
	{
	case DOCinSECT_HEADER:
	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:

	    ei->eiY0UsedTwips= dgRef->dgHeaderPositionTwips;
	    ei->eiY1UsedTwips= y1;
	    ei->eiY0ReservedTwips= ei->eiY0UsedTwips;
	    if  ( y1 > dgRef->dgTopMarginTwips )
		{ ei->eiY1ReservedTwips= y1;				}
	    else{ ei->eiY1ReservedTwips= dgRef->dgTopMarginTwips;	}
	    break;

	case DOCinSECT_FOOTER:
	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:

	    high= y1- dgRef->dgHeaderPositionTwips;
	    ei->eiY1UsedTwips= dgRef->dgPageHighTwips-
					    dgRef->dgFooterPositionTwips;
	    ei->eiY0UsedTwips= ei->eiY1UsedTwips- high;

	    ei->eiY1ReservedTwips= ei->eiY1UsedTwips;

	    if  ( ei->eiY0UsedTwips >
			dgRef->dgPageHighTwips- dgRef->dgBottomMarginTwips )
		{
		ei->eiY0ReservedTwips= dgRef->dgPageHighTwips-
						dgRef->dgBottomMarginTwips;
		}
	    else{
		ei->eiY0ReservedTwips= ei->eiY0UsedTwips;
		}
	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:

	    /*  temporarily, will be placed later on */
	    ei->eiY0UsedTwips= dgRef->dgHeaderPositionTwips;
	    ei->eiY1UsedTwips= y1;
	    break;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    /*  temporarily */
	    ei->eiY0UsedTwips= dgRef->dgHeaderPositionTwips;
	    ei->eiY1UsedTwips= y1;
	    break;

	default:
	    LDEB(ei->eiItem->biInExternalItem); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Do the preliminary layout of the headers and footers of a section	*/
/*									*/
/************************************************************************/

int docSectHeaderFooterPrelayout(	BufferItem *	sectBi,
					LayoutJob *	lj )
    {
    const SectionProperties *	sp= &(sectBi->biSectProperties);
    const DocumentGeometry *	dgSect= &(sp->spDocumentGeometry);


    sectBi->biSectHeader.eiY0ReservedTwips=
    sectBi->biSectFirstPageHeader.eiY0ReservedTwips=
    sectBi->biSectLeftPageHeader.eiY0ReservedTwips=
    sectBi->biSectRightPageHeader.eiY0ReservedTwips=
					dgSect->dgHeaderPositionTwips;

    sectBi->biSectHeader.eiY1ReservedTwips=
    sectBi->biSectFirstPageHeader.eiY1ReservedTwips=
    sectBi->biSectLeftPageHeader.eiY1ReservedTwips=
    sectBi->biSectRightPageHeader.eiY1ReservedTwips=
					dgSect->dgTopMarginTwips;

    if  ( docExternalItemPrelayout( &(sectBi->biSectHeader), dgSect, lj ) )
	{ LDEB(1); return -1;	}

    if  ( docExternalItemPrelayout( &(sectBi->biSectFirstPageHeader),
							    dgSect, lj ) )
	{ LDEB(1); return -1;	}

    if  ( docExternalItemPrelayout( &(sectBi->biSectLeftPageHeader),
							    dgSect, lj ) )
	{ LDEB(1); return -1;	}

    if  ( docExternalItemPrelayout( &(sectBi->biSectRightPageHeader),
							    dgSect, lj ) )
	{ LDEB(1); return -1;	}

    /**/

    sectBi->biSectFooter.eiY0ReservedTwips=
    sectBi->biSectFirstPageFooter.eiY0ReservedTwips=
    sectBi->biSectLeftPageFooter.eiY0ReservedTwips=
    sectBi->biSectRightPageFooter.eiY0ReservedTwips=
		dgSect->dgPageHighTwips- dgSect->dgBottomMarginTwips;

    sectBi->biSectFooter.eiY1ReservedTwips=
    sectBi->biSectFirstPageFooter.eiY1ReservedTwips=
    sectBi->biSectLeftPageFooter.eiY1ReservedTwips=
    sectBi->biSectRightPageFooter.eiY1ReservedTwips=
		dgSect->dgPageHighTwips- dgSect->dgFooterPositionTwips;

    if  ( docExternalItemPrelayout( &(sectBi->biSectFooter), dgSect, lj ) )
	{ LDEB(1); return -1;	}

    if  ( docExternalItemPrelayout( &(sectBi->biSectFirstPageFooter),
							    dgSect, lj ) )
	{ LDEB(1); return -1;	}

    if  ( docExternalItemPrelayout( &(sectBi->biSectLeftPageFooter),
							    dgSect, lj ) )
	{ LDEB(1); return -1;	}

    if  ( docExternalItemPrelayout( &(sectBi->biSectRightPageFooter),
							    dgSect, lj ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Reset page dependent layout administration of headers/footers etc	*/
/*  fo force a subsequent recalculation.				*/
/*									*/
/************************************************************************/

void docResetExternalItemLayout(	BufferDocument *	bd )
    {
    int		i;

    bd->bdEiFtnsep.eiPageFormattedFor= -1;
    bd->bdEiFtnsepc.eiPageFormattedFor= -1;
    bd->bdEiFtncn.eiPageFormattedFor= -1;

    bd->bdEiAftnsep.eiPageFormattedFor= -1;
    bd->bdEiAftnsepc.eiPageFormattedFor= -1;
    bd->bdEiAftncn.eiPageFormattedFor= -1;

    for ( i= 0; i < bd->bdItem.biChildCount; i++ )
	{
	BufferItem *	sectBi=  bd->bdItem.biChildren[i];

	sectBi->biSectHeader.eiPageFormattedFor= -1;
	sectBi->biSectFirstPageHeader.eiPageFormattedFor= -1;
	sectBi->biSectLeftPageHeader.eiPageFormattedFor= -1;
	sectBi->biSectRightPageHeader.eiPageFormattedFor= -1;

	sectBi->biSectFooter.eiPageFormattedFor= -1;
	sectBi->biSectFirstPageFooter.eiPageFormattedFor= -1;
	sectBi->biSectLeftPageFooter.eiPageFormattedFor= -1;
	sectBi->biSectRightPageFooter.eiPageFormattedFor= -1;
	}

    return;
    }

