/************************************************************************/
/*									*/
/*  Edit/Make headers and footers.					*/
/*  Jump back and forth between the body of a document and notes.	*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<limits.h>

#   include	"docLayout.h"
#   include	"tedApp.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Move the current selection to a certain header/Footer. If the	*/
/*  header or footer does not exist, make it.				*/
/*									*/
/************************************************************************/

static int TED_TryPageOffsets[]=
    {
     0,
     1,
    -1,
     2
    };

void tedAppEditHeaderFooter(		EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					int			which )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    AppDrawingData *		add;
    TedDocument *		td;
    BufferDocument *		bd;
    const DocumentProperties *	docp;

    ExternalItem *		ei;
    ExternalItem *		eiTry;
    BufferItem *		bodySectBi;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;
    SelectionScope *		ss;

    int				page= -1;
    int				pg;
    int				i;

    int				scrolledX= 0;
    int				scrolledY= 0;

    DocumentRectangle		drChanged;
    ScreenFontList *		sfl;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    add= &(ed->edDrawingData);
    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;
    docp= &(bd->bdProperties);
    sfl= &(td->tdScreenFontList);

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return;	}
    ss= &(ds.dsSelectionScope);

    drChanged= sg.sgRectangle;

    if  ( docGetHeaderFooter( &ei, &bodySectBi, &ds, bd, which ) )
	{ LDEB(which); return;	}

    if  ( ss->ssInExternalItem == which					&&
	  ss->ssSectNrExternalTo == bodySectBi->biNumberInParent	)
	{ return;	}

    if  ( ! ei->eiItem )
	{
	BufferItem *	paraBi;
	const int	noteIndex= -1;

	paraBi= docMakeExternalparagraph( bd, ei, bodySectBi,
			td->tdCurrentTextAttributeNumber, noteIndex, which );
	if  ( ! paraBi )
	    { XDEB(paraBi); return; }

	if  ( tedLayoutItem( bodySectBi, bd, add, sfl, &drChanged ) )
	    { LDEB(1); return;	}

	appDocumentChanged( ed, 1 );
	}

    switch( which )
	{
	int		isEmpty;

	case DOCinFIRST_HEADER:
	case DOCinFIRST_FOOTER:
	    page= bodySectBi->biTopPosition.lpPage;
	    break;

	case DOCinSECT_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:
	    for ( i= 0; i < sizeof(TED_TryPageOffsets)/sizeof(int); i++ )
		{
		pg= sg.sgBegin.pgTopPosition.lpPage+ TED_TryPageOffsets[i];

		if  ( pg < bodySectBi->biTopPosition.lpPage		||
		      pg > bodySectBi->biBelowPosition.lpPage	)
		    { continue;	}

		if  ( docWhatPageHeader( &eiTry, &isEmpty,
					    bodySectBi, pg, docp ) == which )
		    { page= pg; break;	}
		}
	    break;

	case DOCinSECT_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:
	    for ( i= 0; i < sizeof(TED_TryPageOffsets)/sizeof(int); i++ )
		{
		pg= sg.sgBegin.pgTopPosition.lpPage+ TED_TryPageOffsets[i];

		if  ( pg < bodySectBi->biTopPosition.lpPage		||
		      pg > bodySectBi->biBelowPosition.lpPage	)
		    { continue;	}

		if  ( docWhatPageFooter( &eiTry, &isEmpty,
					    bodySectBi, pg, docp ) == which )
		    { page= pg; break;	}
		}
	    break;

	default:
	    LDEB(which); return;
	}

    if  ( page < bodySectBi->biTopPosition.lpPage		||
	  page > bodySectBi->biBelowPosition.lpPage		)
	{
	TedAppResources *	tar= (TedAppResources *)ea->eaResourceData;

	appQuestionRunErrorDialog( ea, relative, option,
					    tar->tarHeadFootNotReachable );
	return;
	}

    ei->eiPageSelectedUpon= page;

    tedSelectItemHome( ed, ei->eiItem, &scrolledX, &scrolledY );

    tedAdaptToolsToSelection( ed );

    return;
    }

/************************************************************************/
/*									*/
/*  Delete a particular page Header/Footer from the first section of	*/
/*  the selection.							*/
/*									*/
/*  1)  Get the given header/footer.					*/
/*  2)  If it exists.. Delete it.					*/
/*  3)  If the current selection is inside the header/footer to be	*/
/*	deleted.. Move the selection to the body somewhere nearby.	*/
/*									*/
/************************************************************************/

void tedAppDeleteHeaderFooter(		EditApplication *	ea,
					int			which )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    TedDocument *		td;
    BufferDocument *		bd;

    ExternalItem *		ei= (ExternalItem *)0;
    BufferItem *		bodySectBi;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    int				scrolledX= 0;
    int				scrolledY= 0;

    const ScreenFontList *	sfl;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;
    sfl= &(td->tdScreenFontList);

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return;	}

    /*  1  */
    if  ( docGetHeaderFooter( &ei, &bodySectBi, &ds, bd, which ) )
	{ LDEB(which); return;	}

    /*  2  */
    if  ( ei && ei->eiItem )
	{
	AppDrawingData *	add= &(ed->edDrawingData);
	DocumentRectangle	drChanged= add->addBackRect;

	/*  3  */
	if  ( ds.dsBegin.dpBi->biInExternalItem == which )
	    {
	    int			docX;
	    int			docY;
	    int			part;
	    const int		lastLine= 0;

	    DocumentPosition	dp;
	    PositionGeometry	pg;

	    docX= sg.sgBegin.pgXPixels;
	    docY= sg.sgBegin.pgBaselinePixels;

	    if  ( tedFindPosition( &dp, &pg, &part, bd, &(bd->bdItem),
						    add, sfl, docX, docY ) )
		{ LLDEB(docX,docY); return;	}

	    tedSetSelectedPosition( ed, &dp, lastLine, &scrolledX, &scrolledY );

	    tedAdaptToolsToSelection( ed );
	    }

	docCleanExternalItem( bd, ei );
	docInitExternalItem( ei );

	appDocumentChanged( ed, 1 );

	appDocExposeRectangle( ed, &drChanged, scrolledX, scrolledY );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Jump to the foot/end note thet is selected in the body of the	*/
/*  document.								*/
/*									*/
/************************************************************************/

void tedAppEditNote(		EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    TedDocument *		td;
    BufferDocument *		bd;

    int				noteIndex;
    DocumentNote *		dn= (DocumentNote *)0;
    ExternalItem *		eiNote= (ExternalItem *)0;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    int				scrolledX= 0;
    int				scrolledY= 0;

    int				noteFieldNr= -1;
    unsigned char *		fixedText= (unsigned char *)0;
    const int			fixedTextSize= 0;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return;	}

    if  ( ds.dsSelectionScope.ssInExternalItem != DOCinBODY )
	{ LDEB(ds.dsSelectionScope.ssInExternalItem); return;	}

    noteIndex= docGetSelectedNote( &dn, &noteFieldNr,
					fixedText, fixedTextSize, bd, &ds );
    if  ( noteIndex < 0 )
	{ LDEB(noteIndex); return;	}

    eiNote= &(dn->dnExternalItem);

    if  ( ! eiNote->eiItem )
	{ XDEB(eiNote->eiItem); return;	}

    tedSelectItemHome( ed, eiNote->eiItem, &scrolledX, &scrolledY );

    tedAdaptToolsToSelection( ed );

    return;
    }

/************************************************************************/
/*									*/
/*  Jump to the foot/end note thet is selected in the body of the	*/
/*  document.								*/
/*									*/
/************************************************************************/

void tedAppGotoNoteRef(		EditApplication *	ea )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    TedDocument *		td;
    BufferDocument *		bd;

    int				noteIndex;
    DocumentNote *		dn= (DocumentNote *)0;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    int				scrolledX= 0;
    int				scrolledY= 0;

    int				noteFieldNr= -1;
    unsigned char *		fixedText= (unsigned char *)0;
    const int			fixedTextSize= 0;

    DocumentPosition		dpNoteref;
    const int			lastLine= 0;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return;	}

    if  ( ds.dsSelectionScope.ssInExternalItem != DOCinFOOTNOTE	&&
	  ds.dsSelectionScope.ssInExternalItem != DOCinENDNOTE	)
	{ LDEB(ds.dsSelectionScope.ssInExternalItem); return;	}

    noteIndex= docGetSelectedNote( &dn, &noteFieldNr,
					fixedText, fixedTextSize, bd, &ds );
    if  ( noteIndex < 0 )
	{ LDEB(noteIndex); return;	}

    if  ( docGetNotePosition( &dpNoteref, bd, dn ) )
	{ LDEB(1); return;	}

    tedSetIBarSelection( ed, dpNoteref.dpBi, dpNoteref.dpStroff, lastLine,
						    &scrolledX, &scrolledY );

    return;
    }

