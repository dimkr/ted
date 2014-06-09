/************************************************************************/
/*									*/
/*  Ted: Changing font and text attributes.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedApp.h"
#   include	"tedLayout.h"
#   include	<docExpandedTextAttribute.h>
#   include	<appSymbolPicker.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Change to font attributes of the current selection.			*/
/*									*/
/************************************************************************/

static void tedSetCurrentAttribute(
				TedDocument *			td,
				AppDrawingData *		add )
    {
    BufferDocument *		bd= td->tdDocument;
    ScreenFontList *		sfl= &(td->tdScreenFontList);

    td->tdCurrentTextAttributeNumber= utilTextAttributeNumber(
					    &(bd->bdTextAttributeList),
					    &(td->tdCurrentTextAttribute) );

    if  ( td->tdCurrentTextAttributeNumber < 0 )
	{ LDEB(td->tdCurrentTextAttributeNumber); return;	}

    td->tdCurrentScreenFont= tedOpenScreenFont( bd, add, sfl,
					    td->tdCurrentTextAttributeNumber );

    return;
    }

static void tedDocChangeSelectionAttribute(
					EditDocument *		ed,
					const PropertyMask *	taSetMask,
					const TextAttribute *	taSet )
    {
    AppDrawingData *		add= &(ed->edDrawingData);
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    PropertyMask		ppUpdMask;
    PropertyMask		spUpdMask;
    PropertyMask		changeMask;

    PROPmaskCLEAR( &ppUpdMask );
    PROPmaskCLEAR( &spUpdMask );

    PROPmaskCLEAR( &changeMask );

    utilUpdateTextAttribute( &changeMask,
			    &(td->tdCurrentTextAttribute), taSet, taSetMask );

    if  ( ! utilPropMaskIsEmpty( &changeMask )	||
	  td->tdCurrentScreenFont < 0		)
	{ tedSetCurrentAttribute( td, add );	}

    tedChangeSelectionProperties( ed,
			    taSetMask, taSet,
			    &ppUpdMask, (const ParagraphProperties *)0,
			    &spUpdMask, (const SectionProperties *)0 );

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt fonts tool to the current document.				*/
/*									*/
/************************************************************************/

static void tedAdaptSymbolPickerToDocument(	EditApplication *	ea,
						EditDocument *		ed )
    {
    if  ( ea->eaSymbolPicker )
	{
	TedDocument *			td= (TedDocument *)ed->edPrivateData;
	BufferDocument *		bd= td->tdDocument;
	const DocumentProperties *	dp= &(bd->bdProperties);
	const DocumentFontList *	dfl= &(dp->dpFontList);

	PropertyMask			doneMask;
	PropertyMask			taSetMask;
	TextAttribute			taSet;

	PROPmaskCLEAR( &doneMask );
	PROPmaskCLEAR( &taSetMask );
	utilInitTextAttribute( &taSet );

	if  ( tedGetDocumentAttributes( td, &taSetMask, &taSet ) )
	    { return;	}

	if  ( appAdaptSymbolPickerToFontFamily( ea->eaSymbolPicker,
					    ed->edDocumentId,
					    dfl, &taSet, &taSetMask ) )
	    { LDEB(1);	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  1)  Adapt fonts tool &c to the current values.			*/
/*  2)  Adapt fonts tool &c to the current selection.			*/
/*									*/
/************************************************************************/

/*  1  */
static void tedAdaptFontIndicatorsToValues(
					EditDocument *		ed,
					const PropertyMask *	taSetMask,
					TextAttribute *		taSet )
    {
    EditApplication *	ea= ed->edApplication;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    if  ( ea->eaSymbolPicker )
	{
	BufferDocument *		bd= td->tdDocument;
	const DocumentProperties *	dp= &(bd->bdProperties);
	const DocumentFontList *	dfl= &(dp->dpFontList);

	if  ( appAdaptSymbolPickerToFontFamily( ea->eaSymbolPicker,
						    ed->edDocumentId,
						    dfl, taSet, taSetMask ) )
	    { LDEB(1);	}
	}

    appGuiSetToggleItemState( td->tdFontBoldOption,
			    PROPmaskISSET( taSetMask, TApropFONTBOLD ) &&
			    taSet->taFontIsBold );

    appGuiSetToggleItemState( td->tdFontItalicOption,
			    PROPmaskISSET( taSetMask, TApropFONTSLANTED ) &&
			    taSet->taFontIsSlanted );

    appGuiSetToggleItemState( td->tdFontUnderlinedOption,
			    PROPmaskISSET( taSetMask, TApropTEXTUNDERLINED ) &&
			    taSet->taTextIsUnderlined );

    appGuiSetToggleItemState( td->tdFontSuperscriptOption,
			    PROPmaskISSET( taSetMask, TApropSUPERSUB ) &&
			    taSet->taSuperSub == DOCfontSUPERSCRIPT );

    appGuiSetToggleItemState( td->tdFontSubscriptOption,
			    PROPmaskISSET( taSetMask, TApropSUPERSUB ) &&
			    taSet->taSuperSub == DOCfontSUBSCRIPT );

    return;
    }

/*  2  */
void tedAdaptFontIndicatorsToSelection(	EditApplication *	ea,
					EditDocument *		ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    PropertyMask		doneMask;
    PropertyMask		taSetMask;
    TextAttribute		taSet;

    PROPmaskCLEAR( &doneMask );
    PROPmaskCLEAR( &taSetMask );
    utilInitTextAttribute( &taSet );

    if  ( tedGetDocumentAttributes( td, &taSetMask, &taSet ) )
	{ goto ready;	}

    tedAdaptFontIndicatorsToValues( ed, &taSetMask, &taSet );

  ready:

    return;
    }


/************************************************************************/
/*									*/
/*  One of the text attribute options has been pushed.			*/
/*									*/
/************************************************************************/

static void tedTextAttributePushed(	APP_WIDGET	attributeOption,
					void *		voided,
					void *		voidcbs,
					int		prop )
    {
    EditDocument *	ed= (EditDocument *)voided;
    int			set;

    TextAttribute	taSet;
    PropertyMask	taSetMask;

    PROPmaskCLEAR( &taSetMask );
    utilInitTextAttribute( &taSet );

    set= appGuiGetMenuToggleStateFromCallback( attributeOption, voidcbs );

    switch( prop )
	{
	case TApropFONTBOLD:
	    taSet.taFontIsBold= ( set != 0 );
	    PROPmaskADD( &taSetMask, prop );
	    break;

	case TApropFONTSLANTED:
	    taSet.taFontIsSlanted= ( set != 0 );
	    PROPmaskADD( &taSetMask, prop );
	    break;

	case TApropTEXTUNDERLINED:
	    taSet.taTextIsUnderlined= ( set != 0 );
	    PROPmaskADD( &taSetMask, prop );
	    break;

	default:
	    LDEB(prop);
	    return;
	}

    tedDocChangeSelectionAttribute( ed, &taSetMask, &taSet );

    return;
    }

int tedAppChangeSelectionAttributeString(	EditDocument *	ed,
						char *		attrString )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    DocumentProperties *	dp= &(bd->bdProperties);
    DocumentFontList *		dfl= &(dp->dpFontList);

    TextAttribute		taSet;
    ExpandedTextAttribute	etaSet;
    PropertyMask		doneMask;
    PropertyMask		taSetMask;

    EditApplication *		ea= ed->edApplication;

    int				rval= 0;

    PROPmaskCLEAR( &doneMask );
    PROPmaskCLEAR( &taSetMask );
    docInitExpandedTextAttribute( &etaSet );
    utilInitTextAttribute( &taSet );

    if  ( docExpandedAttributeFromString( &taSetMask, &etaSet,
							dfl, attrString ) )
	{ SDEB( attrString ); rval= -1; goto ready;	}

    docIndirectTextAttribute( &doneMask, &taSet, &etaSet, &taSetMask,
				    &(dp->dpColors), &(dp->dpColorCount) );

    tedDocChangeSelectionAttribute( ed, &taSetMask, &taSet );

    tedAdaptFontIndicatorsToSelection( ea, ed );

  ready:
    docCleanExpandedTextAttribute( &etaSet );

    return rval;
    }

void tedDocFontBold(		APP_WIDGET	option,
				void *		voided,
				void *		voidcbs	 )
    { tedTextAttributePushed( option, voided, voidcbs, TApropFONTBOLD ); }

void tedDocFontItalic(	APP_WIDGET	option,
			void *		voided,
			void *		voidcbs	 )
    {
    tedTextAttributePushed( option, voided, voidcbs, TApropFONTSLANTED );
    }

void tedDocFontUnderlined(	APP_WIDGET	option,
				void *		voided,
				void *		voidcbs	 )
    {
    tedTextAttributePushed( option, voided, voidcbs, TApropTEXTUNDERLINED );
    }

void tedDocFontSupersub(	APP_WIDGET		option,
				void *			voided,
				void *			voidcbs	 )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    PropertyMask	taSetMask;
    TextAttribute	taSet;
    int			set;

    PROPmaskCLEAR( &taSetMask );
    PROPmaskADD( &taSetMask, TApropSUPERSUB );

    utilInitTextAttribute( &taSet );

    set= appGuiGetMenuToggleStateFromCallback( option, voidcbs );

    if  ( set )
	{
	if  ( option == td->tdFontSubscriptOption )
	    { taSet.taSuperSub= DOCfontSUBSCRIPT;	}

	if  ( option == td->tdFontSuperscriptOption )
	    { taSet.taSuperSub= DOCfontSUPERSCRIPT;	}
	}
    else{ taSet.taSuperSub= DOCfontREGULAR;	}

    tedDocChangeSelectionAttribute( ed, &taSetMask, &taSet );

    appGuiSetToggleItemState( td->tdFontSuperscriptOption,
				PROPmaskISSET( &taSetMask, TApropSUPERSUB ) &&
				taSet.taSuperSub == DOCfontSUPERSCRIPT );

    appGuiSetToggleItemState( td->tdFontSubscriptOption,
				PROPmaskISSET( &taSetMask, TApropSUPERSUB ) &&
				taSet.taSuperSub == DOCfontSUBSCRIPT );

    return;
    }

/************************************************************************/
/*									*/
/*  Ted: The 'Set' button on the font tool was pushed.			*/
/*									*/
/************************************************************************/

void tedFontToolSet(		void *				voidea,
				const ExpandedTextAttribute *	etaSet,
				const PropertyMask *		taSetMask )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;
    TedDocument *		td;
    BufferDocument *		bd;
    DocumentProperties *	dp;
    DocumentFontList *		dfl;

    PropertyMask		doneMask;
    TextAttribute		taSet;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;
    dp= &(bd->bdProperties);
    dfl= &(dp->dpFontList);

    utilInitTextAttribute( &taSet );
    PROPmaskCLEAR( &doneMask );

    docIndirectTextAttribute( &doneMask, &taSet, etaSet, taSetMask,
				&(dp->dpColors), &(dp->dpColorCount) );

    tedDocChangeSelectionAttribute( ed, taSetMask, &taSet );

    tedAdaptFontIndicatorsToSelection( ea, ed );

    return;
    }

/************************************************************************/
/*									*/
/*  Show or create a symbol picker.					*/
/*									*/
/*  1)  Counts the font as used, so no need to do so here.		*/
/*									*/
/************************************************************************/

static void tedSymbolPickerInsert(
				void *			voidea,
				int			symbol,
				const TextAttribute *	taSet,
				const PropertyMask *	taSetMask )
    {
    unsigned char		fresh[2];

    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;
    AppDrawingData *		add;
    TedDocument *		td;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    td= (TedDocument *)ed->edPrivateData;
    add= &(ed->edDrawingData);

    fresh[0]= symbol;
    fresh[1]= '\0';

    if  ( tedHasSelection( td )		&&
	  td->tdCanReplaceSelection	)
	{
	if  ( PROPmaskISSET( taSetMask, TApropDOC_FONT_NUMBER ) )
	    {
	    PropertyMask	tadoneMask;

	    PROPmaskCLEAR( &tadoneMask );

	    if  ( taSet->taFontNumber < 0 )
		{ LDEB(taSet->taFontNumber); return;	}

	    td->tdCurrentTextAttribute.taFontNumber= taSet->taFontNumber;

	    tedSetCurrentAttribute( td, add );
	    }

	/*  1  */
	tedDocReplaceSelection( ed, fresh, 1 );
	}

    return;
    }

static void tedSymbolPickerClosed(	void *		voidea	)
    {
    EditApplication *	ea=	(EditApplication *)voidea;

    ea->eaSymbolPicker= (void *)0;
    }

void tedDocToolInsertSymbol(	APP_WIDGET	symbolOption,
				void *		voided,
				void *		voidcbs )
    {
    EditDocument *	ed= (EditDocument *)voided;
    TedDocument *	td= (TedDocument *)ed->edPrivateData;
    EditApplication *	ea= ed->edApplication;

    if  ( ! td->tdCanReplaceSelection )
	{ LDEB(td->tdCanReplaceSelection); return;	}

    if  ( ! ea->eaSymbolPicker )
	{
	const char *		pixmapName= "tedsymbol";
	APP_BITMAP_IMAGE	iconPixmap= (APP_BITMAP_IMAGE)0;
	APP_BITMAP_MASK		iconMask= (APP_BITMAP_MASK)0;

	if  ( appGetImagePixmap( ea, pixmapName, &iconPixmap, &iconMask )  )
	    { SDEB(pixmapName); return;	}

	ea->eaSymbolPicker= appMakeSymbolPicker( symbolOption, ea,
							"tedSymbolPicker",
							iconPixmap, iconMask,
							tedSymbolPickerInsert,
							tedSymbolPickerClosed,
							(void *)ea );

	tedAdaptSymbolPickerToDocument( ea, ed );
	}
    else{ appShowSymbolPicker( ea->eaSymbolPicker ); }

    return;
    }
