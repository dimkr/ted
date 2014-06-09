/************************************************************************/
/*									*/
/*  A tool to manage the style of notes in the document.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>
#   include	<limits.h>

#   include	<appGeoString.h>
#   include	"docEvalField.h"

#   include	"tedApp.h"
#   include	"tedFormatTool.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  What property values are relevant for footnotes/endnotes?		*/
/*									*/
/************************************************************************/

static const int TED_FootnoteRestarts[]=
    {
    FTN_RST_CONTINUOUS,
    FTN_RST_PER_SECTION,
    FTN_RST_PER_PAGE,
    };

static const int TED_FootnotePositions[]=
    {
    FTN_POS_BELOW_TEXT,
    FTN_POS_PAGE_BOTTOM,
    };

static const int TED_EndnoteRestarts[]=
    {
    FTN_RST_CONTINUOUS,
    FTN_RST_PER_SECTION,
    };

static const int TED_EndnotePositions[]=
    {
    FTN_POS_SECT_END,
    FTN_POS_DOC_END,
    };

/************************************************************************/
/*									*/
/*  Refresh the 'Notes' page of the format tool.			*/
/*									*/
/************************************************************************/

static void tedSetNotesMenu(	AppOptionmenu *		aom,
				int			val,
				APP_WIDGET *		options )
    {
    int		n= 0;
    int		i;

    for ( i= 0; i < val; i++ )
	{
	if  ( options[i] )
	    { n++;	}
	}

    appSetOptionmenu( aom, n );

    return;
    }

static void tedNotesRefreshNotesStartNumber(	NotePropertiesTool *	npt,
						const NotesProperties *	np )
    {
    if  ( np->npRestart == FTN_RST_CONTINUOUS )
	{
	appIntegerToTextWidget( npt->nptStartNumberText, np->npStartNumber );
	appEnableText( npt->nptStartNumberText, 1 );
	}
    else{
	appStringToTextWidget( npt->nptStartNumberText, "" );
	appEnableText( npt->nptStartNumberText, 0 );
	}

    return;
    }

static void tedNotesRefreshNotePropertiesTool(	NotePropertiesTool *	npt,
						const NotesProperties *	np )
    {
    tedNotesRefreshNotesStartNumber( npt, np );

    tedSetNotesMenu( &(npt->nptPositionOptionmenu),
					    np->npPosition,
					    npt->nptPositionOptions );
    tedSetNotesMenu( &(npt->nptRestartOptionmenu),
					    np->npRestart,
					    npt->nptRestartOptions );
    appSetOptionmenu( &(npt->nptStyleOptionmenu),
					    np->npNumberStyle );

    return;
    }

static void tedNotesToolRefreshDocWidgets(	NotesTool *	nt )
    {
    DocumentProperties *	dp= &(nt->ntPropertiesChosen);

    tedNotesRefreshNotePropertiesTool( &(nt->ntFootnotePropertiesTool),
						&(dp->dpFootnoteProperties) );

    tedNotesRefreshNotePropertiesTool( &(nt->ntEndnotePropertiesTool),
						&(dp->dpEndnoteProperties) );

    return;
    }

static void tedNotesToolRefreshNoteWidgets(	NotesTool *	nt )
    {
    int		enabled= 0;

    if  ( nt->ntNoteKindChosen == DOCinFOOTNOTE )
	{ enabled= 1;	}
    if  ( nt->ntNoteKindChosen == DOCinENDNOTE )
	{ enabled= 1;	}

    appGuiEnableWidget( nt->ntCurrentNoteFrame, enabled );

    appGuiEnableWidget( nt->ntToNoteRefButton, nt->ntInsideNote );
    appGuiEnableWidget( nt->ntToNoteButton, ! nt->ntInsideNote );

    appGuiSetToggleState( nt->ntFootnoteToggle,
				nt->ntNoteKindChosen == DOCinFOOTNOTE );

    appGuiSetToggleState( nt->ntEndnoteToggle,
				nt->ntNoteKindChosen == DOCinENDNOTE );

    appGuiSetToggleState( nt->ntFixedTextToggle, nt->ntFixedTextChosen );

    appEnableText( nt->ntFixedTextText, nt->ntFixedTextChosen );
    appStringToTextWidget( nt->ntFixedTextText, (char *)nt->ntNoteTextChosen );

    return;
    }

void tedFormatToolRefreshNotesTool(
				NotesTool *			nt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				BufferDocument *		bd )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);

    PropertyMask		chgMask;
    PropertyMask		updMask;

    int				enabled= 0;
    int				noteIndex= -1;
    int				noteFieldNr= -1;

    nt->ntInsideNote= 0;
    nt->ntNoteKindSet= DOCinBODY;
    nt->ntFixedTextSet= 0;

    nt->ntNoteTextSet[0]= '\0';
    nt->ntNoteTextChosen[0]= '\0';

    nt->ntNoteNumber= 0;

    if  ( ds )
	{
	DocumentNote *		dn= (DocumentNote *)0;

	noteIndex= docGetSelectedNote( &dn, &noteFieldNr,
						nt->ntNoteTextSet,
						sizeof(nt->ntNoteTextSet)- 1,
						bd, ds );
	if  ( noteIndex >= 0 )
	    {
	    nt->ntNoteKindSet= dn->dnExternalItemKind;
	    nt->ntNoteNumber= dn->dnNoteNumber;

	    if  ( ds->dsSelectionScope.ssInExternalItem == DOCinFOOTNOTE  ||
		  ds->dsSelectionScope.ssInExternalItem == DOCinENDNOTE   )
		{ nt->ntInsideNote= 1;	}

	    if  ( noteFieldNr < 0 )
		{ nt->ntFixedTextSet= 1;	}

	    (*pPref) += 4;
	    }
	}

    nt->ntNoteKindChosen=  nt->ntNoteKindSet;
    nt->ntFixedTextChosen=  nt->ntFixedTextSet;
    strcpy( (char *)nt->ntNoteTextChosen, (char *)nt->ntNoteTextSet );

    PROPmaskCLEAR( &updMask );

    PROPmaskADD( &updMask, DPpropFOOTNOTE_POSITION );
    PROPmaskADD( &updMask, DPpropFOOTNOTE_RESTART );
    PROPmaskADD( &updMask, DPpropFOOTNOTE_STYLE );
    PROPmaskADD( &updMask, DPpropFOOTNOTE_STARTNR );

    PROPmaskADD( &updMask, DPpropENDNOTE_POSITION );
    PROPmaskADD( &updMask, DPpropENDNOTE_RESTART );
    PROPmaskADD( &updMask, DPpropENDNOTE_STYLE );
    PROPmaskADD( &updMask, DPpropENDNOTE_STARTNR );

    PROPmaskCLEAR( &chgMask );

    if  ( docUpdDocumentProperties( &chgMask, &(nt->ntPropertiesChosen),
							&updMask, dp ) )
	{ LDEB(1); return;	}
    if  ( docUpdDocumentProperties( &chgMask, &(nt->ntPropertiesSet),
							&updMask, dp ) )
	{ LDEB(1); return;	}

    if  ( ! nt->ntFixedTextSet			&&
	  nt->ntNoteKindSet != DOCinBODY	)
	{
	if  ( docFormatChftnField( nt->ntNoteTextChosen,
					sizeof(nt->ntNoteTextChosen)- 1,
					&(nt->ntPropertiesChosen),
					nt->ntNoteNumber,
					nt->ntNoteKindChosen ) )
	    { LDEB(1); nt->ntNoteTextChosen[0]= '\0';	}

	strcpy( (char *)nt->ntNoteTextSet, (char *)nt->ntNoteTextChosen );
	}

    tedNotesToolRefreshDocWidgets( nt );
    tedNotesToolRefreshNoteWidgets( nt );

    if  ( ds->dsSelectionScope.ssInExternalItem == DOCinBODY		||
	  noteIndex >= 0						)
	{ enabled= 1;	}

    *pEnabled= enabled;
    return;
    }

/************************************************************************/
/*									*/
/*  Jump back and forth between a note and its reference.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedNotesToolToNoteRef, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;

    tedAppGotoNoteRef( nt->ntApplication );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedNotesToolToNote, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;

    tedAppEditNote( nt->ntApplication );

    return;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons for the document properties have been pushed.	*/
/*									*/
/************************************************************************/

static int tedNotesToolGetStartNumber(	NotePropertiesTool *	npt,
					NotesProperties *	np )
    {
    if  ( np->npRestart == FTN_RST_CONTINUOUS )
	{
	if  ( appGetIntegerFromTextWidget( npt->nptStartNumberText,
				    &(np->npStartNumber), 1, 0, INT_MAX, 0 ) )
	    { return -1;	}
	}

    return 0;
    }

static APP_TXACTIVATE_CALLBACK_H( tedNotesFootRestartChanged, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;
    DocumentProperties *	dp= &(nt->ntPropertiesChosen);
    NotePropertiesTool *	npt= &(nt->ntFootnotePropertiesTool);
    NotesProperties *		np= &(dp->dpFootnoteProperties);

    if  ( np->npRestart != FTN_RST_CONTINUOUS		||
	  tedNotesToolGetStartNumber( npt, np )		)
	{ return;	}

    if  ( ! nt->ntFixedTextChosen		&&
	  nt->ntNoteKindChosen == DOCinFOOTNOTE	)
	{
	if  ( docFormatChftnField( nt->ntNoteTextChosen,
					sizeof(nt->ntNoteTextChosen)- 1,
					&(nt->ntPropertiesChosen),
					nt->ntNoteNumber,
					nt->ntNoteKindChosen ) )
	    { LDEB(1); nt->ntNoteTextChosen[0]= '\0';	}
	else{
	    appStringToTextWidget( nt->ntFixedTextText,
					    (char *)nt->ntNoteTextChosen );
	    }
	}

    return;
    }

static APP_TXACTIVATE_CALLBACK_H( tedNotesEndRestartChanged, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;
    DocumentProperties *	dp= &(nt->ntPropertiesChosen);
    NotePropertiesTool *	npt= &(nt->ntEndnotePropertiesTool);
    NotesProperties *		np= &(dp->dpEndnoteProperties);

    if  ( np->npRestart != FTN_RST_CONTINUOUS		||
	  tedNotesToolGetStartNumber( npt, np )		)
	{ return;	}

    if  ( ! nt->ntFixedTextChosen		&&
	  nt->ntNoteKindChosen == DOCinENDNOTE	)
	{
	if  ( docFormatChftnField( nt->ntNoteTextChosen,
					sizeof(nt->ntNoteTextChosen)- 1,
					&(nt->ntPropertiesChosen),
					nt->ntNoteNumber,
					nt->ntNoteKindChosen ) )
	    { LDEB(1); nt->ntNoteTextChosen[0]= '\0';	}
	else{
	    appStringToTextWidget( nt->ntFixedTextText,
					    (char *)nt->ntNoteTextChosen );
	    }
	}

    return;
    }

static APP_BUTTON_CALLBACK_H( tedChangeNotesPushed, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;
    DocumentProperties *	dp= &(nt->ntPropertiesChosen);

    PropertyMask		dpUpdMask;
    PropertyMask		spUpdMask;

    SectionProperties		sp;

    docInitSectionProperties( &sp );

    PROPmaskCLEAR( &dpUpdMask );

    PROPmaskADD( &dpUpdMask, DPpropFOOTNOTE_POSITION );
    PROPmaskADD( &dpUpdMask, DPpropFOOTNOTE_RESTART );
    PROPmaskADD( &dpUpdMask, DPpropFOOTNOTE_STYLE );
    PROPmaskADD( &dpUpdMask, DPpropFOOTNOTE_STARTNR );

    PROPmaskADD( &dpUpdMask, DPpropENDNOTE_POSITION );
    PROPmaskADD( &dpUpdMask, DPpropENDNOTE_RESTART );
    PROPmaskADD( &dpUpdMask, DPpropENDNOTE_STYLE );
    PROPmaskADD( &dpUpdMask, DPpropENDNOTE_STARTNR );

    PROPmaskCLEAR( &spUpdMask );

    PROPmaskADD( &spUpdMask, SPpropFOOTNOTE_POSITION );
    PROPmaskADD( &spUpdMask, SPpropFOOTNOTE_RESTART );
    PROPmaskADD( &spUpdMask, SPpropFOOTNOTE_STYLE );
    PROPmaskADD( &spUpdMask, SPpropFOOTNOTE_STARTNR );

    /* No! PROPmaskADD( &spUpdMask, SPpropENDNOTE_POSITION ); */
    PROPmaskADD( &spUpdMask, SPpropENDNOTE_RESTART );
    PROPmaskADD( &spUpdMask, SPpropENDNOTE_STYLE );
    PROPmaskADD( &spUpdMask, SPpropENDNOTE_STARTNR );

    /**/

    if  ( tedAppChangeAllSectionProperties( nt->ntApplication,
							&spUpdMask, &sp ) )
	{ LDEB(1);	}

    /**/

    tedNotesToolGetStartNumber( &(nt->ntFootnotePropertiesTool),
						&(dp->dpFootnoteProperties) );

    tedNotesToolGetStartNumber( &(nt->ntEndnotePropertiesTool),
						&(dp->dpEndnoteProperties) );

    tedAppSetDocumentProperties( nt->ntApplication, dp, &dpUpdMask );

    /**/

    docCleanSectionProperties( &sp );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedRevertNotesPushed, w, voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;

    PropertyMask	chgMask;
    PropertyMask	updMask;

    PROPmaskCLEAR( &updMask );

    PROPmaskADD( &updMask, DPpropFOOTNOTE_POSITION );
    PROPmaskADD( &updMask, DPpropFOOTNOTE_RESTART );
    PROPmaskADD( &updMask, DPpropFOOTNOTE_STYLE );
    PROPmaskADD( &updMask, DPpropFOOTNOTE_STARTNR );

    PROPmaskADD( &updMask, DPpropENDNOTE_POSITION );
    PROPmaskADD( &updMask, DPpropENDNOTE_RESTART );
    PROPmaskADD( &updMask, DPpropENDNOTE_STYLE );
    PROPmaskADD( &updMask, DPpropENDNOTE_STARTNR );

    PROPmaskCLEAR( &chgMask );

    if  ( docUpdDocumentProperties( &chgMask, &(nt->ntPropertiesChosen),
					&updMask, &(nt->ntPropertiesSet) ) )
	{ LDEB(1); return;	}

    tedNotesToolRefreshDocWidgets( nt );

    return;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons for the current note have been pushed.		*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedChangeNotePushed, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;

    char *			fixedText= (char *)0;

    fixedText= appGetStringFromTextWidget( nt->ntFixedTextText );

    tedChangeCurrentNote( nt->ntApplication, ! nt->ntFixedTextChosen,
						(unsigned char *)fixedText,
						nt->ntNoteKindChosen );

    appFreeStringFromTextWidget( fixedText );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedRevertNotePushed, w, voidnt )
    {
    NotesTool *		nt= (NotesTool *)voidnt;

    nt->ntNoteKindChosen= nt->ntNoteKindSet;
    nt->ntFixedTextChosen= nt->ntFixedTextSet;

    strcpy( (char *)nt->ntNoteTextChosen, (char *)nt->ntNoteTextSet );

    tedNotesToolRefreshNoteWidgets( nt );

    return;
    }

/************************************************************************/
/*									*/
/*  One of the toggles to decide whether the current note is a footnote	*/
/*  or endnote has been toggled.					*/
/*									*/
/************************************************************************/

static APP_TOGGLE_CALLBACK_H( tedFootnoteToggled, w, voidnt, voidtbcs )
    {
    NotesTool *		nt= (NotesTool *)voidnt;

    int			set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( set && nt->ntNoteKindChosen != DOCinFOOTNOTE )
	{
	nt->ntNoteKindChosen= DOCinFOOTNOTE;

	if  ( ! nt->ntFixedTextChosen )
	    {
	    if  ( docFormatChftnField( nt->ntNoteTextChosen,
					    sizeof(nt->ntNoteTextChosen)- 1,
					    &(nt->ntPropertiesChosen),
					    nt->ntNoteNumber,
					    nt->ntNoteKindChosen ) )
		{ LDEB(1); nt->ntNoteTextChosen[0]= '\0';	}
	    }

	tedNotesToolRefreshNoteWidgets( nt );
	}

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedEndnoteToggled, w, voidnt, voidtbcs )
    {
    NotesTool *		nt= (NotesTool *)voidnt;

    int			set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( set && nt->ntNoteKindChosen != DOCinENDNOTE )
	{
	nt->ntNoteKindChosen= DOCinENDNOTE;
	
	if  ( ! nt->ntFixedTextChosen )
	    {
	    if  ( docFormatChftnField( nt->ntNoteTextChosen,
					    sizeof(nt->ntNoteTextChosen)- 1,
					    &(nt->ntPropertiesChosen),
					    nt->ntNoteNumber,
					    nt->ntNoteKindChosen ) )
		{ LDEB(1); nt->ntNoteTextChosen[0]= '\0';	}
	    }

	tedNotesToolRefreshNoteWidgets( nt );
	}

    return;
    }

static APP_TOGGLE_CALLBACK_H( tedFixedNoteTextToggled, w, voidnt, voidtbcs )
    {
    NotesTool *		nt= (NotesTool *)voidnt;

    int			set;

    set= appGuiGetToggleStateFromCallback( w, voidtbcs );

    if  ( nt->ntFixedTextChosen != set )
	{
	if  ( ! set )
	    {
	    if  ( docFormatChftnField( nt->ntNoteTextChosen,
					sizeof(nt->ntNoteTextChosen)- 1,
					&(nt->ntPropertiesChosen),
					nt->ntNoteNumber,
					nt->ntNoteKindChosen ) )
		{ LDEB(1); nt->ntNoteTextChosen[0]= '\0';	}
	    }

	nt->ntFixedTextChosen= set;
	}

    tedNotesToolRefreshNoteWidgets( nt );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the 'Notes' page of the format tool.				*/
/*									*/
/************************************************************************/

static void tedNotesMakeNotePropertiesTool(
				APP_WIDGET			pageWidget,
				const char *			title,
				NotesTool *			nt,
				NotePropertiesTool *		npt,
				const NotesPageResources *	npr,
				APP_TXACTIVATE_CALLBACK_T	callBack )
    {
    APP_WIDGET	rowLabel;
    APP_WIDGET	row= (APP_WIDGET )0;
    const int	textColumns= 6;


    appMakeColumnFrameInColumn( &(npt->nptFrame),
				    &(npt->nptPaned), pageWidget, title );

    appInspectorMakeMenuRow( &row, &(npt->nptPositionOptionmenu),
			    &rowLabel, npt->nptPaned, npr->nprPositionText );

    appInspectorMakeMenuRow( &row, &(npt->nptRestartOptionmenu),
			    &rowLabel, npt->nptPaned, npr->nprRestartText );

    appInspectorMakeMenuRow( &row, &(npt->nptStyleOptionmenu),
				&rowLabel, npt->nptPaned, npr->nprStyleText );

    appMakeLabelAndTextRow( &row, &rowLabel, &(npt->nptStartNumberText),
			npt->nptPaned, npr->nprFirstNumberText,
			textColumns, 1 );

    if  ( callBack )
	{
	appGuiSetGotValueCallbackForText( npt->nptStartNumberText,
						    callBack, (void *)nt );
	}

    return;
    }

void tedFormatFillNotesPage(	NotesTool *			nt,
				const NotesPageResources *	npr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    APP_WIDGET	row= (APP_WIDGET )0;

    const int	textColumns= 6;

    /**/
    nt->ntPageResources= npr;

    /**/
    docInitDocumentProperties( &(nt->ntPropertiesSet) );
    docInitDocumentProperties( &(nt->ntPropertiesChosen) );

    /**/
    appMakeColumnFrameInColumn( &(nt->ntCurrentNoteFrame),
			    &(nt->ntCurrentNotePaned),
			    pageWidget, npr->nprCurrentNoteText );

    appInspectorMakeToggleRow( &row, nt->ntCurrentNotePaned,
		    &(nt->ntFootnoteToggle), &(nt->ntEndnoteToggle),
		    npr->nprFootnoteText, npr->nprEndnoteText,
		    tedFootnoteToggled,
		    tedEndnoteToggled, (void *)nt );

    appMakeToggleAndTextRow( &row, &(nt->ntFixedTextToggle),
		    &(nt->ntFixedTextText), nt->ntCurrentNotePaned,
		    npr->nprFixedTextText,
		    tedFixedNoteTextToggled, (void *)nt,
		    textColumns, 1 );

    appInspectorMakeButtonRow( &row, nt->ntCurrentNotePaned,
		&(nt->ntToNoteRefButton), &(nt->ntToNoteButton),
		npr->nprToNoteRefText, npr->nprToNoteText,
		tedNotesToolToNoteRef, tedNotesToolToNote,
		(void *)nt );

    appInspectorMakeButtonRow( &row, nt->ntCurrentNotePaned,
		&(nt->ntRevertNoteButton), &(nt->ntChangeNoteButton),
		npr->nprRevertNoteText, npr->nprChangeNoteText,
		tedRevertNotePushed, tedChangeNotePushed,
		(void *)nt );

    /**************/

    tedNotesMakeNotePropertiesTool( pageWidget, npr->nprFootnotesText, nt,
					&(nt->ntFootnotePropertiesTool),
					npr, tedNotesFootRestartChanged );
    tedNotesMakeNotePropertiesTool( pageWidget, npr->nprEndnotesText, nt,
					&(nt->ntEndnotePropertiesTool),
					npr, tedNotesEndRestartChanged );

    /**************/

    appInspectorMakeButtonRow( &row, pageWidget,
		&(is->isRevertButton), &(is->isApplyButton),
		isr->isrRevert, isr->isrApplyToSubject,
		tedRevertNotesPushed, tedChangeNotesPushed,
		(void *)nt );

    return;
    }

/************************************************************************/
/*									*/
/*  Finish the Notes page.						*/
/*									*/
/************************************************************************/

static APP_OITEM_CALLBACK_H( tedFootnoteStyleChosen, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;
    DocumentProperties *	dp= &(nt->ntPropertiesChosen);

    NotePropertiesTool *	npt= &(nt->ntFootnotePropertiesTool);
    NotesProperties *		np= &(dp->dpFootnoteProperties);

    int				n;

    n= appGuiGetOptionmenuItemIndex( &(npt->nptStyleOptionmenu), w );
    if  ( n < 0 || n >= FTNstyle_COUNT )
	{ LDEB(n); return;	}

    np->npNumberStyle= n;

    if  ( ! nt->ntFixedTextChosen		&&
	  nt->ntNoteKindChosen == DOCinFOOTNOTE	)
	{
	if  ( docFormatChftnField( nt->ntNoteTextChosen,
					sizeof(nt->ntNoteTextChosen)- 1,
					&(nt->ntPropertiesChosen),
					nt->ntNoteNumber,
					nt->ntNoteKindChosen ) )
	    { LDEB(1); nt->ntNoteTextChosen[0]= '\0';	}
	else{
	    appStringToTextWidget( nt->ntFixedTextText,
					    (char *)nt->ntNoteTextChosen );
	    }
	}

    return;
    }

static APP_OITEM_CALLBACK_H( tedEndnoteStyleChosen, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;
    DocumentProperties *	dp= &(nt->ntPropertiesChosen);

    NotePropertiesTool *	npt= &(nt->ntEndnotePropertiesTool);
    NotesProperties *		np= &(dp->dpEndnoteProperties);

    int				n;

    n= appGuiGetOptionmenuItemIndex( &(npt->nptStyleOptionmenu), w );
    if  ( n < 0 || n >= FTNstyle_COUNT )
	{ LDEB(n); return;	}

    np->npNumberStyle= n;

    if  ( ! nt->ntFixedTextChosen		&&
	  nt->ntNoteKindChosen == DOCinENDNOTE	)
	{
	if  ( docFormatChftnField( nt->ntNoteTextChosen,
					sizeof(nt->ntNoteTextChosen)- 1,
					&(nt->ntPropertiesChosen),
					nt->ntNoteNumber,
					nt->ntNoteKindChosen ) )
	    { LDEB(1); nt->ntNoteTextChosen[0]= '\0';	}
	else{
	    appStringToTextWidget( nt->ntFixedTextText,
					    (char *)nt->ntNoteTextChosen );
	    }
	}

    return;
    }

static APP_OITEM_CALLBACK_H( tedFootnoteRestartChosen, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;
    DocumentProperties *	dp= &(nt->ntPropertiesChosen);

    NotePropertiesTool *	npt= &(nt->ntFootnotePropertiesTool);
    NotesProperties *		np= &(dp->dpFootnoteProperties);

    int				n;

    n= appGuiGetOptionmenuItemIndex( &(npt->nptRestartOptionmenu), w );
    if  ( n < 0 || n >= sizeof(TED_FootnoteRestarts)/sizeof(int) )
	{ LLDEB(n,sizeof(TED_FootnoteRestarts)/sizeof(int)); return;	}

    np->npRestart= TED_FootnoteRestarts[n];

    tedNotesRefreshNotesStartNumber( npt, np );

    if  ( ! nt->ntFixedTextChosen		&&
	  nt->ntNoteKindChosen == DOCinFOOTNOTE	)
	{
	if  ( docFormatChftnField( nt->ntNoteTextChosen,
					sizeof(nt->ntNoteTextChosen)- 1,
					&(nt->ntPropertiesChosen),
					nt->ntNoteNumber,
					nt->ntNoteKindChosen ) )
	    { LDEB(1); nt->ntNoteTextChosen[0]= '\0';	}
	else{
	    appStringToTextWidget( nt->ntFixedTextText,
					    (char *)nt->ntNoteTextChosen );
	    }
	}
    return;
    }

static APP_OITEM_CALLBACK_H( tedEndnoteRestartChosen, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;
    DocumentProperties *	dp= &(nt->ntPropertiesChosen);

    NotePropertiesTool *	npt= &(nt->ntEndnotePropertiesTool);
    NotesProperties *		np= &(dp->dpEndnoteProperties);

    int				n;

    n= appGuiGetOptionmenuItemIndex( &(npt->nptRestartOptionmenu), w );
    if  ( n < 0 || n >= sizeof(TED_EndnoteRestarts)/sizeof(int) )
	{ LLDEB(n,sizeof(TED_EndnoteRestarts)/sizeof(int)); return;	}

    np->npRestart= TED_EndnoteRestarts[n];

    tedNotesRefreshNotesStartNumber( npt, np );

    if  ( ! nt->ntFixedTextChosen		&&
	  nt->ntNoteKindChosen == DOCinENDNOTE	)
	{
	if  ( docFormatChftnField( nt->ntNoteTextChosen,
					sizeof(nt->ntNoteTextChosen)- 1,
					&(nt->ntPropertiesChosen),
					nt->ntNoteNumber,
					nt->ntNoteKindChosen ) )
	    { LDEB(1); nt->ntNoteTextChosen[0]= '\0';	}
	else{
	    appStringToTextWidget( nt->ntFixedTextText,
					    (char *)nt->ntNoteTextChosen );
	    }
	}

    return;
    }

static APP_OITEM_CALLBACK_H( tedFootnotePositionChosen, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;
    DocumentProperties *	dp= &(nt->ntPropertiesChosen);

    NotePropertiesTool *	npt= &(nt->ntFootnotePropertiesTool);
    NotesProperties *		np= &(dp->dpFootnoteProperties);

    int				n;

    n= appGuiGetOptionmenuItemIndex( &(npt->nptPositionOptionmenu), w );
    if  ( n < 0 || n >= sizeof(TED_FootnotePositions)/sizeof(int) )
	{ LLDEB(n,sizeof(TED_FootnotePositions)/sizeof(int)); return;	}

    np->npPosition= TED_FootnotePositions[n];

    return;
    }

static APP_OITEM_CALLBACK_H( tedEndnotePositionChosen, w, voidnt )
    {
    NotesTool *			nt= (NotesTool *)voidnt;
    DocumentProperties *	dp= &(nt->ntPropertiesChosen);

    NotePropertiesTool *	npt= &(nt->ntEndnotePropertiesTool);
    NotesProperties *		np= &(dp->dpEndnoteProperties);

    int				n;

    n= appGuiGetOptionmenuItemIndex( &(npt->nptPositionOptionmenu), w );
    if  ( n < 0 || n >= sizeof(TED_EndnotePositions)/sizeof(int) )
	{ LLDEB(n,sizeof(TED_EndnotePositions)/sizeof(int)); return;	}

    np->npPosition= TED_EndnotePositions[n];
    if  ( n < 0 || n >= FTN_POS__COUNT )
	{ LDEB(n); return;	}

    return;
    }

static void tedNotePropertiesToolFillStyleChooser(
				NotesTool *			nt,
				APP_OITEM_CALLBACK_T		callBack,
				NotePropertiesTool *		npt,
				const NotesPageResources *	npr )
    {
    int		i;

    appFillInspectorMenu( FTNstyle_COUNT, FTNstyleNAR,
		    npt->nptStyleOptions, npr->nprNumberStyleMenuTexts,
		    &(npt->nptStyleOptionmenu),
		    callBack, (void *)nt );

    for ( i= 0; i < FTN_POS__COUNT; i++ )
	{ npt->nptPositionOptions[i]= (APP_WIDGET)0;	}

    for ( i= 0; i < FTN_RST__COUNT; i++ )
	{ npt->nptRestartOptions[i]= (APP_WIDGET)0;	}

    return;
    }

static void tedNotePropertiesToolFillChoosers(
			NotesTool *			nt,
			NotePropertiesTool *		npt,
			const NotesPageResources *	npr,
			const int *			restarts,
			int				restartCount,
			APP_OITEM_CALLBACK_T		restartChosen,
			const int *			positions,
			int				positionCount,
			APP_OITEM_CALLBACK_T		positionChosen )
    {
    int		i;

    for ( i= 0; i < restartCount; i++ )
	{
	npt->nptRestartOptions[i]=
		appAddItemToOptionmenu( &(npt->nptRestartOptionmenu),
			npr->nprRestartMenuTexts[restarts[i]],
			restartChosen, (void *)nt );
	}

    for ( i= 0; i < positionCount; i++ )
	{
	npt->nptPositionOptions[i]=
		appAddItemToOptionmenu( &(npt->nptPositionOptionmenu),
			npr->nprPositionMenuTexts[positions[i]],
			positionChosen, (void *)nt );
	}

    return;
    }

void tedFormatFillNotesChoosers(	NotesTool *			nt,
					const NotesPageResources *	npr )
    {
    tedNotePropertiesToolFillStyleChooser( nt, tedFootnoteStyleChosen,
				    &(nt->ntFootnotePropertiesTool), npr );

    tedNotePropertiesToolFillStyleChooser( nt, tedEndnoteStyleChosen,
				    &(nt->ntEndnotePropertiesTool), npr );

    tedNotePropertiesToolFillChoosers( nt,
				&(nt->ntFootnotePropertiesTool), npr,

				TED_FootnoteRestarts,
				sizeof(TED_FootnoteRestarts)/sizeof(int),
				tedFootnoteRestartChosen,

				TED_FootnotePositions,
				sizeof(TED_FootnotePositions)/sizeof(int),
				tedFootnotePositionChosen );

    tedNotePropertiesToolFillChoosers( nt,
				&(nt->ntEndnotePropertiesTool), npr,

				TED_EndnoteRestarts,
				sizeof(TED_EndnoteRestarts)/sizeof(int),
				tedEndnoteRestartChosen,

				TED_EndnotePositions,
				sizeof(TED_EndnotePositions)/sizeof(int),
				tedEndnotePositionChosen );

    }

static void tedFinishNotePropertiesTool(
				NotePropertiesTool *		npt )
    {
    appOptionmenuRefreshWidth( &(npt->nptPositionOptionmenu) );
    appOptionmenuRefreshWidth( &(npt->nptRestartOptionmenu) );
    appOptionmenuRefreshWidth( &(npt->nptStyleOptionmenu) );

    return;
    }

void tedFormatFinishNotesPage(		NotesTool *			nt,
					TedFormatTool *			tft,
					const NotesPageResources *	npr )
    {
    tedFinishNotePropertiesTool( &(nt->ntFootnotePropertiesTool) );
    tedFinishNotePropertiesTool( &(nt->ntEndnotePropertiesTool) );
    }

/************************************************************************/
/*									*/
/*  Clean the notes tool.						*/
/*									*/
/************************************************************************/

void tedFormatCleanNotesTool(	NotesTool *	nt )
    {
    docCleanDocumentProperties( &(nt->ntPropertiesSet) );
    docCleanDocumentProperties( &(nt->ntPropertiesChosen) );

    return;
    }

/************************************************************************/
/*									*/
/*  Get notes tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedNotesSubjectResourceTable[]=
    {
    APP_RESOURCE( "formatToolNotes",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Notes" ),
    APP_RESOURCE( "formatToolChangeNotes",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Document" ),
    APP_RESOURCE( "tableToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    };

static AppConfigurableResource TED_TedNotesToolResourceTable[]=
    {
    APP_RESOURCE( "formatToolThisNote",
		offsetof(NotesPageResources,nprCurrentNoteText),
		"This Note" ),
    APP_RESOURCE( "formatToolFootnotes",
		offsetof(NotesPageResources,nprFootnotesText),
		"Footnotes" ),
    APP_RESOURCE( "formatToolEndnotes",
		offsetof(NotesPageResources,nprEndnotesText),
		"Endnotes" ),

    /**/
    APP_RESOURCE( "formatToolNoteIsFootnote",
		offsetof(NotesPageResources,nprFootnoteText),
		"Footnote" ),
    APP_RESOURCE( "formatToolNoteIsEndnote",
		offsetof(NotesPageResources,nprEndnoteText),
		"Endnote" ),

    APP_RESOURCE( "formatToolNoteHasFixedText",
		offsetof(NotesPageResources,nprFixedTextText),
		"Fixed Text" ),

    APP_RESOURCE( "formatToolToNoteRef",
		offsetof(NotesPageResources,nprToNoteRefText),
		"Find Note" ),
    APP_RESOURCE( "formatToolToNote",
		offsetof(NotesPageResources,nprToNoteText),
		"Edit Note" ),

    APP_RESOURCE( "formatToolRevertNote",
		offsetof(NotesPageResources,nprRevertNoteText),
		"Revert" ),
    APP_RESOURCE( "formatToolChangeNote",
		offsetof(NotesPageResources,nprChangeNoteText),
		"Apply to Note" ),

    /**/
    APP_RESOURCE( "formatToolNotesFirstNumber",
		offsetof(NotesPageResources,nprFirstNumberText),
		"First Number" ),
    APP_RESOURCE( "formatToolNotesPosition",
		offsetof(NotesPageResources,nprPositionText),
		"Position" ),
    APP_RESOURCE( "formatToolNotesRestart",
		offsetof(NotesPageResources,nprRestartText),
		"Numbering" ),
    APP_RESOURCE( "formatToolNotesNumberStyle",
		offsetof(NotesPageResources,nprStyleText),
		"Number Format" ),
    /**/
    APP_RESOURCE( "formatToolNoteNumberStyleNar",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[FTNstyleNAR]),
	    "1, 2, 3" ),
    APP_RESOURCE( "formatToolNoteNumberStyleNalc",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[FTNstyleNALC]),
	    "a, b, c" ),
    APP_RESOURCE( "formatToolNoteNumberStyleNauc",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[FTNstyleNAUC]),
	    "A, B, C" ),
    APP_RESOURCE( "formatToolNoteNumberStyleNrlc",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[FTNstyleNRLC]),
	    "i, ii, iii" ),
    APP_RESOURCE( "formatToolNoteNumberStyleNruc",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[FTNstyleNRUC]),
	    "I, II, III" ),
    APP_RESOURCE( "formatToolNoteNumberStyleNchi",
	    offsetof(NotesPageResources,nprNumberStyleMenuTexts[FTNstyleNCHI]),
	    "*, +, #, $" ),
    /**/
    APP_RESOURCE( "formatToolNoteNumberingContinuous",
	offsetof(NotesPageResources,nprRestartMenuTexts[FTN_RST_CONTINUOUS]),
	"Continuous" ),
    APP_RESOURCE( "formatToolNoteNumberingPerSection",
	offsetof(NotesPageResources,nprRestartMenuTexts[FTN_RST_PER_SECTION]),
	"Per Section" ),
    APP_RESOURCE( "formatToolNoteNumberingPerPage",
	offsetof(NotesPageResources,nprRestartMenuTexts[FTN_RST_PER_PAGE]),
	"Per Page" ),
    /**/
    APP_RESOURCE( "formatToolNotePositionSectEnd",
	offsetof(NotesPageResources,nprPositionMenuTexts[FTN_POS_SECT_END]),
	"End of Section" ),
    APP_RESOURCE( "formatToolNotePositionDocEnd",
	offsetof(NotesPageResources,nprPositionMenuTexts[FTN_POS_DOC_END]),
	"End of Document" ),
    APP_RESOURCE( "formatToolNotePositionBelowText",
	offsetof(NotesPageResources,nprPositionMenuTexts[FTN_POS_BELOW_TEXT]),
	"Below Text" ),
    APP_RESOURCE( "formatToolNotePositionPageBottom",
	offsetof(NotesPageResources,nprPositionMenuTexts[FTN_POS_PAGE_BOTTOM]),
	"Page Bottom" ),
    };

void tedFormatToolGetNotesResourceTable( EditApplication *		ea,
					NotesPageResources *		npr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)npr,
				TED_TedNotesToolResourceTable,
				sizeof(TED_TedNotesToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedNotesSubjectResourceTable,
				sizeof(TED_TedNotesSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
