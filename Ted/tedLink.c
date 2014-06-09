/************************************************************************/
/*									*/
/*  Ted: Field,Bookmark,Hyperlink related editing functionality.	*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"docFind.h"
#   include	"tedEdit.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Determine the area covered by a field or a bookmark.		*/
/*									*/
/************************************************************************/

static void tedIncludeFieldInRedraw(	EditOperation *		eo,
					AppDrawingData *	add,
					BufferItem *		paraBi,
					int			startPart,
					int			endPart )
    {
    BufferDocument *		bd= eo->eoBd;
    const ScreenFontList *	sfl= eo->eoScreenFontList;

    DocumentSelection		dsNew;
    SelectionGeometry		sgNew;

    const int			direction= 1;
    const int			lastLine= 0;

    int				length;

    docInitDocumentSelection( &dsNew );

    length=  paraBi->biParaParticules[endPart].tpStroff-
			    paraBi->biParaParticules[startPart].tpStroff;


    docSetParaSelection( &dsNew, paraBi, direction,
			paraBi->biParaParticules[startPart].tpStroff, length );

    tedSelectionGeometry( &sgNew, &dsNew, lastLine, bd, add, sfl );

    docIncludeRectangleInChange( eo, &(sgNew.sgRectangle) );

    return;
    }

/************************************************************************/
/*									*/
/*  Convert the current selection to a field or a bookmark.		*/
/*									*/
/************************************************************************/

static int tedLayoutFieldItem(	EditOperation *		eo,
				EditDocument *		ed,
				DocumentSelection *	dsField,
				unsigned int		whenMask )
    {
    AppDrawingData *		add= &(ed->edDrawingData);
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    BufferItem *		bi;
    ExternalItem *		ei;
    BufferItem *		bodySectBi;

    bi= docGetSelectionRoot( &ei, &bodySectBi, bd, dsField );
    if  ( ! bi )
	{ XDEB(bi); return -1;	}

    if  ( whenMask )
	{
	RecalculateFields		rf;

	docInitRecalculateFields( &rf );

	rf.rfBd= bd;
	rf.rfVoidadd= (void *)add;
	rf.rfCloseObject= tedCloseObject;
	rf.rfUpdateFlags= whenMask;
	rf.rfFieldsUpdated= 0;

	if  ( docRecalculateTextLevelFields( &rf, bi ) )
	    { XDEB(whenMask);	}
	}

    docEditIncludeItemInReformatRange( eo, bi );

    return 0;
    }

static int tedFinishSetField(	EditDocument *			ed,
				DocumentSelection *		dsField,
				EditOperation *			eo,
				unsigned int			whenMask )
    {
    if  ( tedLayoutFieldItem( eo, ed, dsField, whenMask ) )
	{ LDEB(1); return -1;	}

    tedEditFinishSelection( ed, eo, dsField );

    tedAdaptToolsToSelection( ed );

    appDocumentChanged( ed, 1 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remove the particules that delimit a link or bookmark and adjust	*/
/*  the selection to changed particule numbers.				*/
/*									*/
/*  0)  First clean the field.						*/
/*									*/
/*  1)  Determine the rectangle that is to be redrawn.			*/
/*  2)  Delete the particules from the paragraph.			*/
/*  3)  Adjust the selection.						*/
/*  4)  Redraw.								*/
/*									*/
/************************************************************************/

static void tedDeleteField(	EditDocument *		ed,
				BufferItem *		paraBi,
				int			startPart,
				int			endPart )
    {
    AppDrawingData *		add= &(ed->edDrawingData);
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		dsNew;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    TextParticule *		tp;
    DocumentField *		df;

    EditOperation		eo;

    const int			direction= 1;
    const int			col0= -1;
    const int			col1= -1;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 0 );

    /*  0  */
    tp= paraBi->biParaParticules+ startPart;
    df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;
    docCleanField( df ); docInitField( df );

    /*  1  */
    tedIncludeFieldInRedraw( &eo, add, paraBi, startPart, endPart );

    /*  2  */
    docDeleteParticules( paraBi, endPart, 1 );
    docDeleteParticules( paraBi, startPart, 1 );

    docEditIncludeItemInReformatRange( &eo, paraBi );

    /*  3  */
    docSetDocumentPosition( &(ds.dsBegin),
				ds.dsBegin.dpBi, ds.dsBegin.dpStroff );
    docSetDocumentPosition( &(ds.dsEnd),
				ds.dsEnd.dpBi, ds.dsEnd.dpStroff );

    docInitDocumentSelection( &dsNew );

    docSetRangeSelection( &dsNew, &(ds.dsBegin), &(ds.dsEnd),
						    direction, col0, col1 );

    /*  4  */
    tedEditFinishSelection( ed, &eo, &dsNew );

    return;
    }


/************************************************************************/
/*									*/
/*  Run the 'Link Dialog' from the 'Insert Link' menu option.		*/
/*									*/
/************************************************************************/

void tedDocInsertLink(	APP_WIDGET	option,
			void *		voided,
			void *		voidpbcs )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    EditApplication *		ea= ed->edApplication;

    tedShowFormatTool( td->tdToolsFormatToolOption, ea );

    tedAdaptFormatToolToDocument( ed, 0 );

    tedFormatShowLinkPage( ea );

    return;
    }

int tedDocSetHyperlink(	EditDocument *		ed,
			const char *		file,
			const char *		mark,
			int			asRef,
			int			asPageref )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentSelection		dsHyperlink;
    int				linkStartPart;
    int				linkEndPart;

    const char *		fileName= (const char *)0;
    int				fileSize= 0;
    int				newFileSize= strlen( file );
    const char *		markName= (const char *)0;
    int				markSize= 0;
    int				newMarkSize= 0;

    DocumentField *		dfHyper;

    char *			adaptedMark= (char *)0;

    EditOperation		eo;

    TextAttribute		taSet;
    PropertyMask		taSetMask;

    utilInitTextAttribute( &taSet );
    PROPmaskCLEAR( &taSetMask );

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 0 );

    if  ( mark )
	{
	adaptedMark= strdup( mark );
	if  ( ! adaptedMark )
	    { XDEB(adaptedMark); return -1;	}

	docAdaptBookmarkName( &newMarkSize, adaptedMark );
	}

    if  ( docGetHyperlinkForPosition( bd, &(ds.dsBegin),
				&dsHyperlink, &linkStartPart, &linkEndPart,
				&fileName, &fileSize, &markName, &markSize ) )
	{
	DocumentProperties *	dp= &(bd->bdProperties);

	DocumentSelection	dsRep;
	DocumentSelection	dsHyperlink;
	int			beginMoved= 0;
	int			endMoved= 0;

	unsigned int		whenMask= FIELDdoNOTHING;

	RGB8Color		blueColor;
	int			blueIndex;

	const int		avoidZero= 1;
	const int		maxColors= 255;

	bmInitRGB8Color( &blueColor );
	blueColor.rgb8Red= 0;
	blueColor.rgb8Green= 0;
	blueColor.rgb8Blue= 255;

	blueIndex= bmInsertColor( &(dp->dpColors), &(dp->dpColorCount),
					avoidZero, maxColors, &blueColor );
	if  ( blueIndex < 0 )
	    { LDEB(blueIndex);					}
	else{ PROPmaskADD( &taSetMask, TApropTEXT_COLOR );	}

	taSet.taTextColorNumber= blueIndex;
	taSet.taTextIsUnderlined= 1;

	PROPmaskADD( &taSetMask, TApropTEXTUNDERLINED );

	dsRep= ds;

	docConstrainSelectionToOneParagraph( &beginMoved, &endMoved, &dsRep );

	dsHyperlink= dsRep;

	if  ( asRef || asPageref )
	    {
	    const FieldKindInformation *	fki;
	    int					innerStartPart= -1;
	    int					innerEndPart= -1;
	    int					fieldNumber;
	    TextParticule *			tp;

	    if  ( tedEditReplaceSelection( &eo, &dsRep, ed,
					    (const unsigned char *)"?", 1 ) )
		{ LDEB(1); return -1;	}

	    dsHyperlink= dsRep;

	    if  ( asRef )
		{
		DocumentField *	dfRef;
		int		refStartPart;
		int		refEndPart;

		if  ( docSurroundTextSelectionByField( &dfRef, bd,
					&refStartPart, &refEndPart, &dsRep,
					&taSetMask, &taSet ) )
		    { LDEB(1); return -1;	}

		if  ( docFieldSetRef( dfRef,
				(unsigned char *)adaptedMark, newMarkSize ) )
		    { LDEB(newMarkSize); return -1;	}

		dfRef->dfKind= DOCfkREF;
		fki= DOC_FieldKinds+ dfRef->dfKind;
		whenMask |= fki->fkiCalculateWhen;

		dsRep.dsBegin= dsRep.dsEnd;
		innerStartPart= refStartPart;
		innerEndPart= refEndPart;
		}

	    if  ( asRef && asPageref )
		{
		TextParticule *		tp;
		int			partShift= 0;
		int			stroffShift= 0;

		tp= docParaSpecialParticule( &eo, dsRep.dsEnd.dpBi, DOCkindTAB,
				    innerEndPart+ 1,
				    dsRep.dsEnd.dpStroff,
				    &partShift, &stroffShift );
		if  ( ! tp )
		    { XDEB(tp); return -1;	}
		
		dsRep.dsEnd.dpStroff += stroffShift;
		dsRep.dsBegin= dsRep.dsEnd;
		innerEndPart++;

		docUnionParaSelections( &dsHyperlink, &dsHyperlink, &dsRep );
		}

	    if  ( asPageref )
		{
		DocumentField *	dfPageref;
		int		pageStartPart;
		int		pageEndPart;

		if  ( docSurroundTextSelectionByField( &dfPageref, bd,
					&pageStartPart, &pageEndPart, &dsRep,
					&taSetMask, &taSet ) )
		    {
		    LLDEB(innerStartPart,innerEndPart);
		    docListItem(0,dsRep.dsEnd.dpBi);
		    return -1;
		    }

		if  ( docFieldSetPageref( dfPageref,
				(unsigned char *)adaptedMark, newMarkSize ) )
		    { LDEB(newMarkSize); return -1;	}

		dfPageref->dfKind= DOCfkPAGEREF;
		fki= DOC_FieldKinds+ dfPageref->dfKind;
		whenMask |= fki->fkiCalculateWhen;

		dsRep.dsBegin= dsRep.dsEnd;
		if  ( innerStartPart < 0 )
		    { innerStartPart= pageStartPart;	}
		innerEndPart= pageEndPart;
		}

	    dfHyper= docClaimField( &fieldNumber, &(bd->bdFieldList) );
	    if  ( ! dfHyper )
		{ XDEB(dfHyper); return -1;	}

	    linkStartPart= innerStartPart;
	    linkEndPart= innerEndPart+ 1;
	    tp= docInsertTextParticule( ds.dsEnd.dpBi, linkEndPart,
			    dsRep.dsEnd.dpStroff, 0, DOCkindFIELDEND,
			    ds.dsEnd.dpBi->biParaParticules[
					innerEndPart].tpTextAttributeNumber );
	    if  ( ! tp )
		{ XDEB(tp); return -1;	}
	    tp->tpObjectNumber= fieldNumber;

	    tp= docInsertTextParticule( ds.dsBegin.dpBi, linkStartPart,
			    dsRep.dsBegin.dpStroff, 0, DOCkindFIELDSTART,
			    ds.dsBegin.dpBi->biParaParticules[
					innerStartPart].tpTextAttributeNumber );
	    if  ( ! tp )
		{ XDEB(tp); return -1;	}
	    tp->tpObjectNumber= fieldNumber;
	    }
	else{
	    /*  4  */
	    if  ( docSurroundTextSelectionByField( &dfHyper, bd,
				&linkStartPart, &linkEndPart, &dsHyperlink,
				&taSetMask, &taSet ) )
		{ LDEB(1); return -1;	}
	    }

	if  ( docFieldSetHyperlink( dfHyper, (unsigned char *)file, newFileSize,
				(unsigned char *)adaptedMark, newMarkSize ) )
	    { SSDEB(file,adaptedMark); return -1;	}

	dfHyper->dfKind= DOCfkHYPERLINK;

	/*  5  */
	if  ( tedFinishSetField( ed, &dsHyperlink, &eo, whenMask ) )
	    { LDEB(1); return -1;	}

	if  ( adaptedMark )
	    { free( adaptedMark );	}

	return 0;
	}
    else{
	BufferItem *	bi= ds.dsBegin.dpBi;

	dfHyper= bd->bdFieldList.dflFields;
	dfHyper += bi->biParaParticules[linkStartPart].tpObjectNumber;

	if  ( docFieldSetHyperlink( dfHyper,
				(unsigned char *)file, newFileSize,
				(unsigned char *)adaptedMark, newMarkSize ) )
	    { SSDEB(file,adaptedMark); return -1;	}

	appDocumentChanged( ed, 1 );

	if  ( adaptedMark )
	    { free( adaptedMark );	}

	return 0;
	}
    }

int tedAppSetHyperlink(	EditApplication *	ea,
			const char *		file,
			const char *		mark,
			int			asRef,
			int			asPageref )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocSetHyperlink( ed, file, mark, asRef, asPageref );
    }

int tedDocRemoveHyperlink(	EditDocument *		ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		dsHyperlink;
    int				startPart;
    int				endPart;

    const char *		fileName= (const char *)0;
    int				fileSize= 0;
    const char *		markName= (const char *)0;
    int				markSize= 0;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    tedGetSelection( &ds, &sg, &sd, td );

    if  ( docGetHyperlinkForPosition( bd, &(ds.dsBegin),
				&dsHyperlink, &startPart, &endPart,
				&fileName, &fileSize, &markName, &markSize ) )
	{ LDEB(1); return -1;	}
    else{
	DocumentProperties *	dp= &(bd->bdProperties);
	BufferItem *		paraBi= ds.dsBegin.dpBi;

	int			part;
	TextParticule *		tp;

	RGB8Color		blueColor;
	int			blueIndex;

	const int		avoidZero= 1;
	const int		maxColors= 255;

	bmInitRGB8Color( &blueColor );
	blueColor.rgb8Red= 0;
	blueColor.rgb8Green= 0;
	blueColor.rgb8Blue= 255;

	blueIndex= bmInsertColor( &(dp->dpColors), &(dp->dpColorCount),
					avoidZero, maxColors, &blueColor );
	if  ( blueIndex < 0 )
	    { LDEB(blueIndex); blueIndex= 0;			}

	tp= paraBi->biParaParticules+ startPart+ 1;
	for ( part= startPart+ 1; part < endPart; tp++, part++ )
	    {
	    TextAttribute	taHere;

	    utilGetTextAttributeByNumber( &taHere, &(bd->bdTextAttributeList),
						    tp->tpTextAttributeNumber );

	    if  ( taHere.taTextColorNumber == blueIndex		&&
		  taHere.taTextIsUnderlined == 1		)
		{
		int	num;

		taHere.taTextColorNumber= 0;
		taHere.taTextIsUnderlined= 0;

		num= utilTextAttributeNumber( &(bd->bdTextAttributeList),
								    &taHere );
		if  ( num < 0 )
		    { LDEB(num);			}
		else{ tp->tpTextAttributeNumber= num;	}
		}
	    }

	tedDeleteField( ed, paraBi, startPart, endPart );

	appDocumentChanged( ed, 1 );

	return 0;
	}
    }

int tedAppRemoveHyperlink(	EditApplication *	ea )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    return tedDocRemoveHyperlink( ed );
    }

/************************************************************************/
/*									*/
/*  Run the 'Bookmark Dialog' from the 'Insert Bookmark' menu option.	*/
/*									*/
/************************************************************************/

void tedDocInsertBookmark(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		dsBookmark;
    int				startPart;
    int				endPart;

    int				nameProposed= 0;
    char *			savedMark= (char *)0;

    const char *		markName= (const char *)0;
    int				markSize= 0;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    tedGetSelection( &ds, &sg, &sd, td );

    if  ( docGetBookmarkForPosition( bd, &(ds.dsBegin),
					    &dsBookmark, &startPart, &endPart,
					    &markName, &markSize ) )
	{
	BufferItem *	bi= ds.dsBegin.dpBi;
	int		stroff= ds.dsBegin.dpStroff;

	if  ( ! ed->edIsReadonly		&&
	      bi				&&
	      bi == ds.dsEnd.dpBi		)
	    {
	    unsigned char *	s= bi->biParaString+ stroff;
	    unsigned char *	e;
	    unsigned char *	p;

	    while( stroff < ds.dsEnd.dpStroff	&&
		   ! isalnum( *s )		)
		{ s++; stroff++;	}

	    p= e= s;
	    while( stroff < ds.dsEnd.dpStroff )
		{
		if  ( isalnum( *e ) )
		    { p= e+ 1;	}

		e++; stroff++;
		}

	    if  ( p- s > DOCmaxBOOKMARK )
		{ p= s+ DOCmaxBOOKMARK;	}

	    savedMark= (char *)malloc( DOCmaxBOOKMARK+ 1 );
	    if  ( savedMark )
		{
		DocumentField *	df;

		if  ( p- s > 3 )
		    { strncpy( savedMark, (char *)s, p- s )[p- s]= '\0'; }
		else{ strcpy( savedMark, "bkmk" ); p= s+ 4;		 }

		markName= savedMark;
		nameProposed= 1;

		docAdaptBookmarkName( &markSize, savedMark );

		if  ( docGetBookmarkField( &df, savedMark, markSize,
						&(bd->bdFieldList) ) >= 0 )
		    {
		    int		i;

		    if  ( markSize+ 8 > DOCmaxBOOKMARK )
			{ markSize= DOCmaxBOOKMARK- 8;	}

		    markSize += 8;

		    for ( i= 0; i < 100000000; i++ )
			{
			sprintf( savedMark+ markSize- 8, "%08d", i );

			if  ( docGetBookmarkField( &df,
						savedMark, markSize,
						&(bd->bdFieldList) ) < 0 )
			    { break;	}
			}
		    }
		}
	    }
	}

    tedRunBookmarkDialog( ea, ed, option, nameProposed, markName, markSize );

    if  ( savedMark )
	{ free( savedMark );	}

    tedAdaptToolsToSelection( ed );

    return;
    }

int tedSetBookmark(	EditDocument *		ed,
			const char *		mark )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		dsBookmark;
    int				startPart;
    int				endPart;

    const char *		markName= (const char *)0;
    int				markSize= 0;
    int				newMarkSize;

    DocumentField *		df;

    char *			adaptedMark= (char *)0;

    EditOperation		eo;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    TextAttribute		taSet;
    PropertyMask		taSetMask;

    utilInitTextAttribute( &taSet );
    PROPmaskCLEAR( &taSetMask );

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 0 );

    adaptedMark= strdup( mark );
    if  ( ! adaptedMark )
	{ XDEB(adaptedMark); return -1;	}

    docAdaptBookmarkName( &newMarkSize, adaptedMark );

    if  ( docGetBookmarkForPosition( bd, &(ds.dsBegin),
					    &dsBookmark, &startPart, &endPart,
					    &markName, &markSize ) )
	{
	DocumentSelection	dsRep;

	int			startPart;
	int			endPart;

	int			beginMoved= 0;
	int			endMoved= 0;

	dsRep= ds;

	docConstrainSelectionToOneParagraph( &beginMoved, &endMoved, &dsRep );

	markSize= strlen( adaptedMark );

	/*  4  */
	if  ( docSurroundTextSelectionByField( &df, bd,
						&startPart, &endPart, &dsRep,
						&taSetMask, &taSet ) )
	    { LDEB(1); return -1;	}

	if  ( docFieldSetBookmark( df,
			    (unsigned char *)adaptedMark, newMarkSize ) )
	    { SDEB(adaptedMark); return -1;	}

	df->dfKind= DOCfkBOOKMARK;

	/*  5  */
	if  ( tedFinishSetField( ed, &dsRep, &eo, FIELDdoNOTHING ) )
	    { LDEB(1); return -1;	}

	return 0;
	}
    else{
	BufferItem *	bi= ds.dsBegin.dpBi;

	df= bd->bdFieldList.dflFields;
	df += bi->biParaParticules[startPart].tpObjectNumber;

	if  ( docFieldSetBookmark( df,
				(unsigned char *)adaptedMark, newMarkSize ) )
	    { SDEB(adaptedMark); return -1;	}

	appDocumentChanged( ed, 1 );

	return 0;
	}
    }

int tedRemoveBookmark(	EditDocument *		ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		dsBookmark;
    int				startPart;
    int				endPart;

    const char *		markName= (const char *)0;
    int				markSize= 0;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    tedGetSelection( &ds, &sg, &sd, td );

    if  ( docGetBookmarkForPosition( bd, &(ds.dsBegin),
					    &dsBookmark, &startPart, &endPart,
					    &markName, &markSize ) )
	{ LDEB(1); return -1;	}
    else{
	BufferItem *	bi= ds.dsBegin.dpBi;

	tedDeleteField( ed, bi, startPart, endPart );

	appDocumentChanged( ed, 1 );

	return 0;
	}
    }

int tedCopyBookmarkAsLink(	EditDocument *		ed,
				int			asRef,
				int			asPageref,
				const char *		markName )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    int				markSize= strlen( markName );

    DocumentSelection		ds;

    docInitDocumentSelection( &ds );

    if  ( docFindBookmarkInDocument( &ds, td->tdDocument, markName, markSize ) )
	{ SDEB(markName); return -1;	}

    if  ( tedCopyAsLink( ed, &ds, asRef, asPageref,
				    ed->edFilename, markName, markSize ) )
	{ LDEB(1);	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Select a particular bookmark in a document.				*/
/*									*/
/************************************************************************/

int tedGoToBookmark(	EditDocument *		ed,
			const char *		markName,
			int			markSize )
    {
    DocumentSelection		ds;

    TedDocument *		td;

    int				scrolledX= 0;
    int				scrolledY= 0;

    int				beginMoved= 0;
    int				endMoved= 0;

    const int			lastLine= 0;

    td= (TedDocument *)ed->edPrivateData;

    docInitDocumentSelection( &ds );

    if  ( docFindBookmarkInDocument( &ds, td->tdDocument, markName, markSize ) )
	{ SDEB(markName); return -1;	}

    docConstrainSelectionToOneParagraph( &beginMoved, &endMoved, &ds );

    tedSetSelection( ed, &ds, lastLine, &scrolledX, &scrolledY );

    tedAdaptToolsToSelection( ed );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Replace the current selection of the document with a field.		*/
/*									*/
/*  2)  Replace the current selection with something arbitrary.		*/
/*  3)  Make the footnote number field.					*/
/*  4)  Finish by surrounding the replacement with the field.		*/
/*  5)  Final bookkeeping and redrawing.				*/
/*									*/
/************************************************************************/

static int tedDocReplaceSelectionWithField(
					int *			pStartPart,
					int *			pEndPart,
					DocumentSelection *	dsField,
					EditOperation *		eo,
					EditDocument *		ed,
					const char *		fieldInst,
					int			fieldKind,
					const PropertyMask *	taSetMask,
					const TextAttribute *	taSet )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		dsRep;

    DocumentField *		df;

    /*  2  */
    if  ( tedEditReplaceSelection( eo, &dsRep, ed,
					    (const unsigned char *)"?", 1 ) )
	{ LDEB(1); return -1;	}

    /*  3,4  */
    if  ( docSurroundTextSelectionByField( &df, bd,
					    pStartPart, pEndPart, &dsRep,
					    taSetMask, taSet ) )
	{ LDEB(1); return -1;	}

    if  ( docSetFieldInst( df,
			(unsigned char *)fieldInst, strlen( fieldInst ) ) )
	{ SDEB(fieldInst); return -1;	}

    *dsField= dsRep;
    df->dfKind= fieldKind;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a page number in the document.				*/
/*									*/
/*  1)  Force the header or footer that will receive the page number	*/
/*	to be reformatted the next time that it is drawn.		*/
/*  2)  Replace the selection with a PAGE field. As the replecement is	*/
/*	a page number that is recalculated every time the		*/
/*	header/footer is drawn, no field calculation is required.	*/
/*									*/
/************************************************************************/

void tedDocInsertPageNumber(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    const char *		fieldInst= "PAGE";

    ExternalItem *		ei;
    BufferItem *		bodySectBi;

    DocumentSelection		dsField;

    EditOperation		eo;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    TextAttribute		taSet;
    PropertyMask		taSetMask;

    int				startPart;
    int				endPart;

    utilInitTextAttribute( &taSet );
    PROPmaskCLEAR( &taSetMask );

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 0 );

    /*  1  */
    if  ( docGetHeaderFooter( &ei, &bodySectBi, &ds, bd,
				    ds.dsSelectionScope.ssInExternalItem ) )
	{ LDEB(ds.dsSelectionScope.ssInExternalItem); return;	}

    ei->eiPageFormattedFor= -1;

    /*  2  */
    if  ( tedDocReplaceSelectionWithField( &startPart, &endPart, &dsField,
						    &eo, ed, 
						    fieldInst, DOCfkPAGE,
						    &taSetMask, &taSet ) )
	{ SDEB(fieldInst); return;	}

    /*  5  */
    if  ( tedFinishSetField( ed, &dsField, &eo, FIELDdoNOTHING ) )
	{ LDEB(1); return;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Insert a footnote in the document.					*/
/*									*/
/*  1)  Change the selection to a reference to a note.			*/
/*  2)	Insert a note immediately after the reference.			*/
/*  3)	Preliminary layout of the footnote.				*/
/*  4)	Layout of the item thet receives the footnote. (Usually redoes	*/
/*	the layout of the footnote)					*/
/*  5)	Select the end of the freshly inserted footnote.		*/
/*									*/
/************************************************************************/

static void tedDocInsertNote(	APP_WIDGET	option,
				void *		voided,
				int		noteInExtIt,
				int		sepInExtIt )
    {
    EditDocument *		ed= (EditDocument *)voided;
    AppDrawingData *		add= &(ed->edDrawingData);
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    ScreenFontList *		sfl= &(td->tdScreenFontList);

    DocumentSelection		dsField;

    const char * const		fieldInst= "";
    const unsigned int		fieldUpdMask= FIELDdoCHFTN;

    DocumentNote *		dn;
    BufferItem *		bodySectBi;

    int				changed;

    EditOperation		eo;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    DocumentPosition		dpEnd;

    RecalculateFields		rf;

    TextAttribute		taSet;
    PropertyMask		taSetMask;

    int				startPart;
    int				endPart;

    utilInitTextAttribute( &taSet );
    PROPmaskCLEAR( &taSetMask );

    PROPmaskADD( &taSetMask, TApropSUPERSUB );
    taSet.taSuperSub= DOCfontSUPERSCRIPT;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 0 );

    if  ( ds.dsSelectionScope.ssInExternalItem != DOCinBODY )
	{ LDEB(ds.dsSelectionScope.ssInExternalItem); return;	}

    bodySectBi= ds.dsBegin.dpBi;
    while( bodySectBi && bodySectBi->biLevel != DOClevSECT )
	{ bodySectBi= bodySectBi->biParent;	}
    if  ( ! bodySectBi )
	{ XDEB(bodySectBi); return;	}

    /*  1  */
    if  ( tedDocReplaceSelectionWithField( &startPart, &endPart, &dsField,
						    &eo, ed,
						    fieldInst, DOCfkCHFTN,
						    &taSetMask, &taSet ) )
	{ SDEB(fieldInst); return;	}

    /*  2  */
    if  ( docMakeNote( &dn, bd, dsField.dsEnd.dpBi,
			    endPart, dsField.dsEnd.dpStroff, noteInExtIt ) )
	{ LDEB(1); return;	}

    if  ( docCheckNoteSeparatorItem( bd, sepInExtIt ) )
	{ LDEB(1); return;	}

    changed= 0;
    docRenumberNotes( &changed, bd );
    eo.eoNotesAdded++;

    if  ( changed )
	{
	docInitRecalculateFields( &rf );

	rf.rfBd= bd;
	rf.rfVoidadd= (void *)add;
	rf.rfCloseObject= tedCloseObject;
	rf.rfUpdateFlags= fieldUpdMask;
	rf.rfFieldsUpdated= 0;

	if  ( docRecalculateTextLevelFields( &rf, bodySectBi ) )
	    { XDEB(fieldUpdMask);	}
	}

    /*  3  */
    docInitRecalculateFields( &rf );

    rf.rfBd= bd;
    rf.rfVoidadd= (void *)add;
    rf.rfCloseObject= tedCloseObject;
    rf.rfUpdateFlags= fieldUpdMask;
    rf.rfFieldsUpdated= 0;

    if  ( docRecalculateTextLevelFields( &rf, dn->dnExternalItem.eiItem ) )
	{ XDEB(fieldUpdMask);	}

    /*  NO! Is just to setup the note.
    docEditIncludeItemInReformatRange( eo, dn->dnExternalItem.eiItem );
    */

    /*  Exclude from Edit operation: In another tree */
    if  ( ! eo.eoChangedRectSet )
	{ LDEB(eo.eoChangedRectSet);	}

    if  ( tedLayoutItem( dn->dnExternalItem.eiItem, bd, add, sfl,
							&(eo.eoChangedRect) ) )
	{ LDEB(1); return;	}

    /*  4  */
    if  ( tedLayoutFieldItem( &eo, ed, &dsField, fieldUpdMask ) )
	{ LDEB(1); return;	}

    /*  5  */
    if  ( docLastPosition( &dpEnd, dn->dnExternalItem.eiItem ) )
	{ LDEB(1); return;	}

    /*  6  */
    tedEditFinishIBarSelection( ed, &eo, dpEnd.dpBi, dpEnd.dpStroff );

    appDocumentChanged( ed, 1 );

    return;
    }

void tedDocInsertFootnote(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs )
    {
    tedDocInsertNote( option, voided, DOCinFOOTNOTE, DOCinFTNSEP );
    }

void tedDocInsertEndnote(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs )
    {
    tedDocInsertNote( option, voided, DOCinENDNOTE, DOCinAFTNSEP );
    }


/************************************************************************/
/*									*/
/*  Change the kind of note for the current selection.			*/
/*									*/
/************************************************************************/

static int tedEditSetNoteItemAuto(	EditOperation *		eo,
					AppDrawingData *	add,
					int *			pChanged,
					DocumentNote *		dn,
					BufferDocument *	bd )
    {
    DocumentPosition	dpHead;

    int			fieldNr= -1;
    int			partBegin= -1;
    int			partEnd= -1;
    int			stroffBegin= -1;
    int			stroffEnd= -1;

    int			changed= 0;

    int			txtAttrNr;

    if  ( docFirstPosition( &dpHead, dn->dnExternalItem.eiItem ) )
	{ LDEB(1); return -1;	}

    txtAttrNr= dpHead.dpBi->biParaParticules->tpTextAttributeNumber;

    if  ( ! docDelimitParaHeadField( &fieldNr, &partBegin, &partEnd,
						&stroffBegin, &stroffEnd,
						dpHead.dpBi, bd ) )
	{ return 0;	}
    else{
	unsigned char	fixedText[30+1];
	int		fixedLen;

	int		partShift= 0;
	int		stroffShift= 0;

	const int	stroffHead= 0;
	int		paraNr= docNumberOfParagraph( dpHead.dpBi );

	docNoteGetTextAtHead( fixedText, &fixedLen,
					sizeof(fixedText)- 1, dpHead.dpBi );

	if  ( docParaReplaceText( eo, dpHead.dpBi, paraNr, stroffHead,
					&partShift, &stroffShift, fixedLen,
					(const unsigned char *)0, 0, -1 ) )
	    { LDEB(1); return -1;	}

	if  ( docInsertParaHeadField( &fieldNr, &partBegin, &partEnd,
				    &stroffBegin, &stroffEnd,
				    dpHead.dpBi, bd, DOCfkCHFTN, txtAttrNr ) )
	    { LDEB(1);	}

	eo->eoNotesAdded++;
	eo->eoNotesDeleted++;
	eo->eoFieldUpdate |= FIELDdoCHFTN;
	changed= 1;
	}

    if  ( changed )
	{ *pChanged= 1;	}

    return 0;
    }

static int tedEditSetNoteReferenceAuto(	EditOperation *		eo,
					AppDrawingData *	add,
					int *			pChanged,
					DocumentNote *		dn,
					BufferDocument *	bd )
    {
    DocumentPosition	dpNoteref;

    int			fieldNr= -1;
    int			partBegin= -1;
    int			partEnd= -1;
    int			stroffBegin= -1;
    int			stroffEnd= -1;

    int			changed= 0;

    if  ( docGetNotePosition( &dpNoteref, bd, dn ) )
	{ LDEB(1); return -1;	}

    if  ( ! docDelimitNoteReference( &fieldNr, &partBegin, &partEnd,
				    &stroffBegin, &stroffEnd,
				    dpNoteref.dpBi, dpNoteref.dpStroff, bd ) )
	{ return 0;	}
    else{
	unsigned char		fixedText[30+1];
	int			fixedLen;

	TextAttribute		taSet;
	PropertyMask		taSetMask;
	int			textAttributeNumber;

	DocumentField *		df;
	DocumentSelection	dsField;

	docNoteGetTextBefore( fixedText, &fixedLen, &textAttributeNumber,
		    sizeof(fixedText)- 1, dpNoteref.dpBi, dpNoteref.dpStroff );

	docSetParaSelection( &dsField, dpNoteref.dpBi, 1,
				    dpNoteref.dpStroff- fixedLen, fixedLen );

	utilInitTextAttribute( &taSet );
	PROPmaskCLEAR( &taSetMask );

	utilGetTextAttributeByNumber( &taSet, &(bd->bdTextAttributeList),
							textAttributeNumber );

	PROPmaskADD( &taSetMask, TApropSUPERSUB );
	taSet.taSuperSub= DOCfontSUPERSCRIPT;

	if  ( docSurroundTextSelectionByField( &df, bd,
					    &partBegin, &partEnd,
					    &dsField, &taSetMask, &taSet ) )
	    { LDEB(1); return -1;	}

	df->dfKind= DOCfkCHFTN;

	eo->eoNotesAdded++;
	eo->eoNotesDeleted++;
	eo->eoFieldUpdate |= FIELDdoCHFTN;
	changed= 1;
	}

    if  ( changed )
	{ *pChanged= 1;	}

    return 0;
    }

static int tedEditSetNoteItemFixed(	EditOperation *		eo,
					AppDrawingData *	add,
					int *			pChanged,
					DocumentNote *		dn,
					BufferDocument *	bd,
					const unsigned char *	fixedTextSet )
    {
    DocumentPosition	dpHead;

    int			fieldNr= -1;
    int			partBegin= -1;
    int			partEnd= -1;
    int			stroffBegin= -1;
    int			stroffEnd= -1;

    int			changed= 0;

    int			txtAttrNr;

    int			partShift= 0;
    int			stroffShift= 0;

    int			fixedLenSet= strlen( (const char *)fixedTextSet );

    int			paraNr;

    if  ( docFirstPosition( &dpHead, dn->dnExternalItem.eiItem ) )
	{ LDEB(1); return -1;	}

    paraNr= docNumberOfParagraph( dpHead.dpBi );
    txtAttrNr= dpHead.dpBi->biParaParticules->tpTextAttributeNumber;

    if  ( docDelimitParaHeadField( &fieldNr, &partBegin, &partEnd,
						&stroffBegin, &stroffEnd,
						dpHead.dpBi, bd ) )
	{
	const int	stroffHead= 0;

	unsigned char	fixedText[30+1];
	int		fixedLen;

	docNoteGetTextAtHead( fixedText, &fixedLen,
					sizeof(fixedText)- 1, dpHead.dpBi );

	if  ( fixedLen == fixedLenSet					&&
	      ! strncmp( (char *)fixedText, (char *)fixedTextSet,
							fixedLen )	)
	    { return 0;	}

	if  ( docParaReplaceText( eo, dpHead.dpBi, paraNr, stroffHead,
				    &partShift, &stroffShift, fixedLen,
				    fixedTextSet, fixedLenSet, txtAttrNr ) )
	    { LDEB(1); return -1;	}

	changed= 1;
	}
    else{
	BufferDocument *	bd= eo->eoBd;
	DocumentField *		df;

	df= bd->bdFieldList.dflFields+ fieldNr;
	docCleanField( df ); docInitField( df );

	tedIncludeFieldInRedraw( eo, add, dpHead.dpBi, partBegin, partEnd );

	docDeleteParticules( dpHead.dpBi, partEnd, 1 );
	docDeleteParticules( dpHead.dpBi, partBegin, 1 );

	if  ( docParaReplaceText( eo, dpHead.dpBi, paraNr, stroffBegin,
				    &partShift, &stroffShift, 
				    stroffEnd- stroffBegin,
				    fixedTextSet, fixedLenSet, txtAttrNr ) )
	    { LDEB(1); return -1;	}

	changed= 1;
	}

    if  ( changed )
	{
	docInvalidateParagraphLayout( dpHead.dpBi );
	*pChanged= 1;
	}

    return 0;
    }

static int tedEditSetNoteReferenceFixed( EditOperation *	eo,
					AppDrawingData *	add,
					int *			pChanged,
					DocumentNote *		dn,
					BufferDocument *	bd,
					const unsigned char *	fixedTextSet )
    {
    DocumentPosition	dpNoteref;

    int			fieldNr= -1;
    int			partBegin= -1;
    int			partEnd= -1;
    int			stroffBegin= -1;
    int			stroffEnd= -1;

    int			changed= 0;

    int			partShift= 0;
    int			stroffShift= 0;

    int			fixedLenSet= strlen( (const char *)fixedTextSet );

    int			txtAttrNr;
    int			paraNr;

    if  ( docGetNotePosition( &dpNoteref, bd, dn ) )
	{ LDEB(1); return -1;	}

    paraNr= docNumberOfParagraph( dpNoteref.dpBi );

    if  ( docDelimitNoteReference( &fieldNr, &partBegin, &partEnd,
				    &stroffBegin, &stroffEnd,
				    dpNoteref.dpBi, dpNoteref.dpStroff, bd ) )
	{
	unsigned char	fixedText[30+1];
	int		fixedLen;

	docNoteGetTextBefore( fixedText, &fixedLen, &txtAttrNr,
		    sizeof(fixedText)- 1, dpNoteref.dpBi, dpNoteref.dpStroff );

	if  ( fixedLen == fixedLenSet					&&
	      ! strncmp( (char *)fixedText, (char *)fixedTextSet,
							fixedLen )	)
	    { return 0;	}

	if  ( docParaReplaceText( eo, dpNoteref.dpBi, paraNr,
				dpNoteref.dpStroff- fixedLen,
				&partShift, &stroffShift, dpNoteref.dpStroff,
				fixedTextSet, fixedLenSet, txtAttrNr ) )
	    { LDEB(1); return -1;	}

	changed= 1;
	}
    else{
	BufferDocument *	bd= eo->eoBd;
	DocumentField *		df;

	df= bd->bdFieldList.dflFields+ fieldNr;
	docCleanField( df ); docInitField( df );

	txtAttrNr= dpNoteref.dpBi->biParaParticules[partBegin+1].
						    tpTextAttributeNumber;

	tedIncludeFieldInRedraw( eo, add, dpNoteref.dpBi, partBegin, partEnd );

	docDeleteParticules( dpNoteref.dpBi, partEnd, 1 );
	docDeleteParticules( dpNoteref.dpBi, partBegin, 1 );

	if  ( docParaReplaceText( eo, dpNoteref.dpBi, paraNr, stroffBegin,
				    &partShift, &stroffShift, stroffEnd,
				    fixedTextSet, fixedLenSet, txtAttrNr ) )
	    { LDEB(1); return -1;	}

	changed= 1;
	}

    if  ( changed )
	{
	docInvalidateParagraphLayout( dpNoteref.dpBi );
	*pChanged= 1;
	}

    return 0;
    }

int tedChangeCurrentNote(	EditApplication *	ea,
				int			autoNumber,
				const unsigned char *	fixedTextSet,
				int			extItKind )
    {
    EditDocument *		ed= ea->eaCurrentDocument;
    AppDrawingData *		add= &(ed->edDrawingData);
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    int				noteIndex;
    DocumentNote *		dn= (DocumentNote *)0;
    ExternalItem *		eiNote= (ExternalItem *)0;

    EditOperation		eo;
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    int				noteFieldNr= -1;
    unsigned char		fixedText[30+1];

    int				changed= 0;

    tedStartEditOperation( &eo, &ds, &sg, &sd, ed, 1 );

    noteIndex= docGetSelectedNote( &dn, &noteFieldNr,
				fixedText, sizeof(fixedText)- 1, bd, &ds );
    if  ( noteIndex < 0 )
	{ LDEB(noteIndex); return -1;	}

    if  ( autoNumber )
	{
	if  ( tedEditSetNoteItemAuto( &eo, add, &changed, dn, bd ) )
	    { LDEB(1); return -1;	}

	if  ( tedEditSetNoteReferenceAuto( &eo, add, &changed, dn, bd ) )
	    { LDEB(1); return -1;	}
	}
    else{
	if  ( tedEditSetNoteItemFixed( &eo, add,
					    &changed, dn, bd, fixedTextSet ) )
	    { LDEB(1); return -1;	}

	if  ( tedEditSetNoteReferenceFixed( &eo, add,
					    &changed, dn, bd, fixedTextSet ) )
	    { LDEB(1); return -1;	}
	}

    if  ( dn->dnAutoNumber != autoNumber )
	{ dn->dnAutoNumber= autoNumber; changed= 1;	}

    if  ( dn->dnExternalItemKind != extItKind )
	{
	switch( extItKind )
	    {
	    case DOCinFOOTNOTE:
		if  ( docCheckNoteSeparatorItem( bd, DOCinFTNSEP ) )
		    { LLDEB(extItKind,DOCinFTNSEP); return -1;	}
		break;

	    case DOCinENDNOTE:
		if  ( docCheckNoteSeparatorItem( bd, DOCinAFTNSEP ) )
		    { LLDEB(extItKind,DOCinAFTNSEP); return -1;	}
		break;

	    default:
		LDEB(extItKind); return -1;
	    }

	eiNote= &(dn->dnExternalItem);

	if  ( ds.dsSelectionScope.ssInExternalItem == dn->dnExternalItemKind )
	    { ds.dsSelectionScope.ssInExternalItem= extItKind;	}

	dn->dnExternalItemKind= extItKind;
	dn->dnNoteNumber= 0;
	eiNote->eiPageFormattedFor= -1;

	if  ( ! eiNote->eiItem )
	    { XDEB(eiNote->eiItem); return -1;	}

	docSetExternalItemKind( eiNote->eiItem, dn->dnExternalItemKind );
	changed= 1;
	}

    if  ( changed )
	{
	eo.eoFieldUpdate |= FIELDdoCHFTN;
	eo.eoNotesDeleted++;
	eo.eoNotesAdded++;

	if  ( tedEditFinishOldSelection( ed, &eo ) )
	    { LDEB(1);	}

	appDocumentChanged( ed, 1 );
	}

    return 0;
    }

