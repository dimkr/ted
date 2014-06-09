#   include	"tedConfig.h"

#   include	<stdlib.h>

#   include	"docDraw.h"
#   include	"docLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Draw external items such as headers/footers/footnotes/endnotes.	*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Print/Draw Headers/Footers.						*/
/*									*/
/************************************************************************/

static int docDrawHeaderFooter(	BufferItem *		bodySectBi,
				void *			through,
				ExternalItem *		ei,
				DrawingContext *	dc,
				int			page )
    {
    AppDrawingData *	add= dc->dcDrawingData;
    ScreenFontList *	sfl= dc->dcScreenFontList;

    if  ( dc->dcClipRect )
	{
	BufferDocument *	bd= dc->dcDocument;

	const int		justUsed= 1;
	DocumentRectangle	drExtern;

	if  ( docGetExternalItemBox( &drExtern, bodySectBi, ei, justUsed,
							    page, bd, add ) )
	    { LDEB(1); return -1;	}

	if  ( ! geoIntersectRectangle( &drExtern, &drExtern, dc->dcClipRect ) )
	    { return 0;	}
	}

    if  ( page != ei->eiPageFormattedFor				&&
	  docLayoutExternalItem( ei, (DocumentRectangle *)0,
			page, ei->eiY0UsedTwips,
			dc->dcDocument, bodySectBi, add, sfl,
			dc->dcInitLayoutExternal, dc->dcCloseObject )	)
	{ LDEB(1); return -1;	}

    if  ( docDrawItem( ei->eiItem, through, dc ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int docDrawPageHeader(	BufferItem *			bodySectBi,
			void *				through,
			DrawingContext *		dc,
			int				page )
    {
    ExternalItem *		ei= (ExternalItem *)0;
    int				inExternalItem;
    int				isEmpty;
    BufferDocument *		bd= dc->dcDocument;
    DocumentProperties *	dp= &(bd->bdProperties);

    inExternalItem= docWhatPageHeader( &ei, &isEmpty, bodySectBi, page, dp );

    if  ( ! ei || ! ei->eiItem )
	{ return 0;	}

    if  ( docDrawHeaderFooter( bodySectBi, through, ei, dc, page ) )
	{ LDEB(page); return 1;	}

    if  ( docDrawShapesForPage( ei->eiItem, through, dc, page ) )
	{ LDEB(page);	}

    return 0;
    }

int docDrawPageFooter(	BufferItem *			bodySectBi,
			void *				through,
			DrawingContext *		dc,
			int				page )
    {
    ExternalItem *		ei= (ExternalItem *)0;
    int				inHeaderFooter;
    int				isEmpty;
    BufferDocument *		bd= dc->dcDocument;
    DocumentProperties *	dp= &(bd->bdProperties);

    inHeaderFooter= docWhatPageFooter( &ei, &isEmpty, bodySectBi, page, dp );

    if  ( ! ei || ! ei->eiItem )
	{ return 0;	}

    if  ( docDrawHeaderFooter( bodySectBi, through, ei, dc, page ) )
	{ LDEB(page); return 1;	}

    if  ( docDrawShapesForPage( ei->eiItem, through, dc, page ) )
	{ LDEB(page);	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw the notes on a page.						*/
/*									*/
/*  1)  Calculate the position of the separator.			*/
/*  2)  In clipped drawing.. Will the separator be visible anyway?	*/
/*  3)  If necassary recalculate the layout of the separator.		*/
/*  4)  Draw it.							*/
/*									*/
/************************************************************************/

static int docDrawNoteSeparator(void *			through,
				DrawingContext *	dc,
				const DocumentNote *	dnFirstNote,
				int			extItKind )
    {
    BufferDocument *		bd= dc->dcDocument;
    const ExternalItem *	eiFirstNote;
    ExternalItem *		eiNoteSep;
    int				y0Twips;

    int				ret;

    AppDrawingData *		add= dc->dcDrawingData;
    DocumentRectangle		drExtern;
    int				page;

    eiFirstNote= &(dnFirstNote->dnExternalItem);
    page= eiFirstNote->eiItem->biTopPosition.lpPage;

    /*  1  */
    ret= docNoteSeparatorRectangle( &drExtern, &eiNoteSep, &y0Twips,
					bd, dnFirstNote, extItKind, add );
    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}
    if  ( ret > 0 )
	{ return 0;	}

    /*  2  */
    if  ( dc->dcClipRect						&&
	  ! geoIntersectRectangle( &drExtern, &drExtern, dc->dcClipRect ) )
	{ return 0;	}

    /*  3  */
    if  ( page != eiNoteSep->eiPageFormattedFor				&&
	  docLayoutExternalItem( eiNoteSep, (DocumentRectangle *)0,
			page, y0Twips,
			dc->dcDocument, (BufferItem *)0,
			dc->dcDrawingData, dc->dcScreenFontList,
			dc->dcInitLayoutExternal, dc->dcCloseObject )	)
	{ LDEB(1); return -1;	}

    /*  4  */
    if  ( docDrawItem( eiNoteSep->eiItem, through, dc ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int docDrawFootnotesForColumn(	int				page,
				void *				through,
				DrawingContext *		dc )
    {
    BufferDocument *		bd= dc->dcDocument;
    int				i;
    DocumentNote *		dn;
    int				noteIndex;

    noteIndex= docGetFirstNoteFromPage( &dn, bd, page, DOCinFOOTNOTE );
    if  ( noteIndex < 0 )
	{ return 0;	}

    if  ( docDrawNoteSeparator( through, dc, dn, DOCinFTNSEP ) )
	{ LDEB(page);	}

    for ( i= noteIndex; i < bd->bdNoteCount; dn++, i++ )
	{
	ExternalItem *	ei= &(dn->dnExternalItem);

	if  ( dn->dnParaNr < 0 )
	    { continue;	}
	if  ( dn->dnReferringPage < page )
	    { LLLDEB(i,page,dn->dnReferringPage); continue;	}
	if  ( dn->dnReferringPage > page )
	    { break;	}

	if  ( dn->dnExternalItemKind != DOCinFOOTNOTE )
	    { continue;	}
	if  ( ! ei->eiItem )
	    { XDEB(ei->eiItem); continue;	}

	if  ( docDrawItem( ei->eiItem, through, dc ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw the notes belonging to a section immediately below its text.	*/
/*									*/
/*  1)  Are there any?							*/
/*  2)  Get separator.							*/
/*  3)  Draw the separator.						*/
/*  4)  Draw the endnotes of this section.				*/
/*									*/
/************************************************************************/

int docDrawEndnotesForSection(		int			sect,
					void *			through,
					DrawingContext *	dc )
    {
    BufferDocument *		bd= dc->dcDocument;
    BufferItem *		sectBi;

    DocumentNote *		dn;
    int				noteIndex;
    int				i;
    ExternalItem *		ei;

    LayoutPosition		lpHere;
    DocumentPosition		dp;

    const int			pShift= 0;

    /*  1  */
    if  ( sect >= 0 )
	{
	noteIndex= docGetFirstNoteOfSection( &dn, bd, sect, DOCinENDNOTE );

	if  ( noteIndex < 0 )
	    { return 0;	}

	if  ( docLastPosition( &dp, bd->bdItem.biChildren[sect] ) )
	    { LDEB(1); return -1;	}

	sectBi= bd->bdItem.biChildren[sect];
	}
    else{
	noteIndex= docGetFirstNoteOfDocument( &dn, bd, DOCinENDNOTE );     

	if  ( noteIndex < 0 )
	    { return 0;	}

	if  ( docLastPosition( &dp, &(bd->bdItem) ) )
	    { LDEB(1); return -1;	}

	sectBi= bd->bdItem.biChildren[bd->bdItem.biChildCount- 1];
	}

    lpHere= dp.dpBi->biBelowPosition;

    ei= &(dn->dnExternalItem);
    if  ( dc->dcLastPage >= 0					&&
	  ei->eiItem->biTopPosition.lpPage > dc->dcLastPage	)
	{ return 0;	}

    if  ( docDrawToPageOfItem( sectBi, sectBi, ei->eiItem,
					    through, &lpHere, dc, pShift ) )
	{ LDEB(1); return -1;	}

    /*  2  */
    if  ( dc->dcFirstPage < 0					||
	  ei->eiItem->biBelowPosition.lpPage >= dc->dcFirstPage	)
	{
	if  ( docDrawNoteSeparator( through, dc, dn, DOCinAFTNSEP ) )
	    { LDEB(sect);	}
	}

    /*  4  */
    for ( i= noteIndex; i < bd->bdNoteCount; dn++, i++ )
	{
	if  ( dn->dnParaNr < 0 )
	    { continue;	}
	if  ( sect >= 0 && dn->dnSectNr > sect )
	    { break;	}

	if  ( dn->dnExternalItemKind != DOCinENDNOTE )
	    { continue;	}

	ei= &(dn->dnExternalItem);
	if  ( dc->dcLastPage >= 0				&&
	      ei->eiItem->biTopPosition.lpPage > dc->dcLastPage	)
	    { return 0;	}

	if  ( docDrawToPageOfItem( sectBi, sectBi, ei->eiItem,
					    through, &lpHere, dc, pShift ) )
	    { LDEB(1); return -1;	}

	if  ( dc->dcFirstPage < 0					||
	      ei->eiItem->biBelowPosition.lpPage >= dc->dcFirstPage	)
	    {
	    if  ( docDrawItem( ei->eiItem, through, dc ) )
		{ LDEB(1); return -1;	}
	    }

	lpHere= ei->eiItem->biBelowPosition;
	}

    return 0;
    }

int docDrawEndnotesForDocument(		void *			through,
					DrawingContext *	dc )
    {
    return docDrawEndnotesForSection( -1, through, dc );
    }

