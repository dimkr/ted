/************************************************************************/
/*									*/
/*  A Symbol picker.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>

#   include	<appSymbolPicker.h>
#   include	<appEncodingMenu.h>
#   include	<appDraw.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Resources for the Symbol Picker.					*/
/*									*/
/************************************************************************/

typedef struct AppSymbolPickerResources
    {
    char *	asprInsert;
    char *	asprLower;
    char *	asprClose;
    char *	asprFont;
    char *	asprNone;
    char *	asprEncodings[CHARSETidxCOUNT];
    } AppSymbolPickerResources;

static AppConfigurableResource APP_SymbolPickerResourceTable[]=
    {
    APP_RESOURCE( "symbolPickerFont",
		offsetof(AppSymbolPickerResources,asprFont),
		"Font, Encoding" ),
    APP_RESOURCE( "symbolPickerNone",
		offsetof(AppSymbolPickerResources,asprNone),
		"None" ),
    APP_RESOURCE( "symbolPickerInsert",
		offsetof(AppSymbolPickerResources,asprInsert),
		"Insert" ),
    APP_RESOURCE( "symbolPickerLower",
		offsetof(AppSymbolPickerResources,asprLower),
		"Lower" ),
    APP_RESOURCE( "symbolPickerClose",
		offsetof(AppSymbolPickerResources,asprClose),
		"Close" ),
    };

/************************************************************************/
/*									*/
/*  Represents a symbol picker.						*/
/*									*/
/************************************************************************/

typedef struct AppSymbolPicker
    {
    EditApplication *		aspApplication;
    unsigned int		aspCurrentDocumentId;

    AppSymbolPickerResources *	aspResources;

    APP_WIDGET			aspTopWidget;
    APP_WIDGET			aspMainWidget;

    APP_WIDGET			aspFontFrame;
    AppOptionmenu		aspFontOptionmenu;
    AppEncodingMenu		aspEncodingMenu;

    APP_WIDGET			aspSymbolDrawing;
    APP_WIDGET			aspInsertButton;
    APP_WIDGET			aspLowerButton;
    APP_WIDGET			aspCloseButton;

    AppToolDestroy		aspDestroy;
    SymbolPickerInsert		aspInsert;
    void *			aspTarget;

    int				aspSymbolSelected;

    DocumentFontList		aspDocumentFontList;
    TextAttribute		aspTextAttribute;
    int				aspFontFamilyChosen;

    APP_FONT *			aspFont;
    AppDrawingData		aspDrawingData;
    int				aspCellSizePixels;
    int				aspCellsWide;
    int				aspCellsHigh;

    APP_WIDGET *		aspFontFamilyOpts;
    int				aspFontFamilyOptCount;
    } AppSymbolPicker;

/************************************************************************/
/*									*/
/*  Adapt the symbol Picker to the current font.			*/
/*									*/
/*  9)  Also remember that no symbol has been selected yet, and that	*/
/*	for that reason, no symbol can be inserted.			*/
/*									*/
/************************************************************************/

static int appSymbolAdaptDrawingToFont(		AppSymbolPicker *	asp )
    {
    AppDrawingData *		add= &(asp->aspDrawingData);
    const DocumentFontList *	dfl= &(asp->aspDocumentFontList);
    TextAttribute *		ta= &(asp->aspTextAttribute);
    int				physicalFont;

    if  ( asp->aspFontFamilyChosen < 0 )
	{ LDEB(asp->aspFontFamilyChosen); return -1;	}

    if  ( asp->aspFont )
	{
	/*  No! managed by the drawing data
	appDrawFreeFont( add, asp->aspFont );
	*/
	asp->aspFont= (APP_FONT *)0;
	}

    physicalFont= appOpenScreenFont( add, dfl, ta );
    if  ( physicalFont < 0 )
	{ LDEB(physicalFont);	}
    else{
	const DrawScreenFontList *	apfl= &(add->addScreenFontList);
	DrawScreenFont *		apf;

	apf= apfl->apflFonts+ physicalFont;
	asp->aspFont= apf->apfFontStruct;

	appDrawSetFont( add, apf->apfFontStruct );
	}

    appExposeRectangle( add, 0, 0, 0, 0 );

    asp->aspSymbolSelected= -1;
    appGuiEnableWidget( asp->aspInsertButton, 0 );

    return 0;
    }

static int appSymbolAdaptToFamily(  AppSymbolPicker *	asp )
    {
    const DocumentFontList *	dfl= &(asp->aspDocumentFontList);

    if  ( asp->aspFontFamilyChosen < 0 )
	{ LDEB(asp->aspFontFamilyChosen); return -1;	}

    appEncodingMenuAdaptToFamilyEncodings( &(asp->aspEncodingMenu),
			    dfl->dflFamilies+ asp->aspFontFamilyChosen );

    if  ( appSymbolAdaptDrawingToFont( asp ) )
	{ LDEB(asp->aspFontFamilyChosen); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  A symbol picker must be destroyed.					*/
/*									*/
/************************************************************************/

static void appDestroySymbolPicker(	AppSymbolPicker *	asp )
    {

    if  ( asp->aspDestroy )
	{ (*asp->aspDestroy)( asp->aspTarget );	}

    /* No! managed by the drawing data
    if  ( asp->aspFont )
	{ appDrawFreeFont( &(asp->aspDrawingData), asp->aspFont );	}
    */

    docCleanFontList( &(asp->aspDocumentFontList) );

    appCleanDrawingData( &(asp->aspDrawingData) );

    if  ( asp->aspFontFamilyOpts )
	{ free( asp->aspFontFamilyOpts );	}

    appDestroyShellWidget( asp->aspTopWidget );

    free( (void *)asp );

    return;
    }

static APP_CLOSE_CALLBACK_H( appCloseSymbolPicker, w, voidasp )
    {
    AppSymbolPicker *	asp= (AppSymbolPicker *)voidasp;

    appDestroySymbolPicker( asp );

    return;
    }

/************************************************************************/
/*									*/
/*  'Insert' button has been pushed.					*/
/*									*/
/************************************************************************/

static void appSymbolPickerInsertSymbol(	AppSymbolPicker *	asp )
    {
    PropertyMask	setMask;

    PROPmaskCLEAR( &setMask );

    if  ( asp->aspSymbolSelected < 0 || ! asp->aspInsert )
	{ LXDEB(asp->aspSymbolSelected,asp->aspInsert); return;	}

    if  ( asp->aspFontFamilyChosen >= 0 )
	{ PROPmaskADD( &setMask, TApropDOC_FONT_NUMBER );	}

    (*asp->aspInsert)( asp->aspTarget, asp->aspSymbolSelected,
				    &(asp->aspTextAttribute), &setMask );

    return;
    }

static APP_BUTTON_CALLBACK_H( appSymbolInsertPushed, w, voidasp )
    {
    AppSymbolPicker *	asp= (AppSymbolPicker *)voidasp;

    appSymbolPickerInsertSymbol( asp );
    
    return;
    }

static APP_BUTTON_CALLBACK_H( appSymbolLowerPushed, w, voidasp )
    {
    AppSymbolPicker *	asp= (AppSymbolPicker *)voidasp;

    appGuiLowerShellWidget( asp->aspTopWidget );
    
    return;
    }

static APP_BUTTON_CALLBACK_H( appSymbolClosePushed, w, voidasp )
    {
    AppSymbolPicker *	asp= (AppSymbolPicker *)voidasp;

    appDestroySymbolPicker( asp );
    
    return;
    }

/************************************************************************/
/*									*/
/*  Redraw the symbol part of the symbol picker.			*/
/*									*/
/************************************************************************/

static void appSymbolDrawSymbol(	AppSymbolPicker *	asp,
					int			sym,
					int			x0,
					int			y0 )
    {
    AppDrawingData *			add= &(asp->aspDrawingData);
    char				scratch[2];

    int					wide;
    int					fontAscent;
    int					fontDescent;

    int					x;
    int					y;

    scratch[0]= sym;
    scratch[1]= '\0';

    appDrawTextExtents( &wide, &fontAscent, &fontDescent,
					    add, asp->aspFont, scratch, 1 );

    y= ( asp->aspCellSizePixels- fontAscent- fontDescent+ 1 )/ 2;
    y= y0+ y+ fontAscent;
    x= x0+ asp->aspCellSizePixels/ 2- wide/2;

    appDrawDrawString( add, x, y, scratch, 1 );

    return;
    }

static void appSymbolRectangle(		DocumentRectangle *	drSym,
					const AppSymbolPicker *	asp,
					int			row,
					int			col )
    {
    drSym->drX0= col* ( asp->aspCellSizePixels+ 1 )+ 1;
    drSym->drX1= drSym->drX0+ asp->aspCellSizePixels- 1;

    drSym->drY0= row* ( asp->aspCellSizePixels+ 1 )+ 1;
    drSym->drY1= drSym->drY0+ asp->aspCellSizePixels- 1;

    return;
    }

static APP_EVENT_HANDLER_H( appSymbolRedraw, w, voidasp, exposeEvent )
    {
    AppSymbolPicker *			asp= (AppSymbolPicker *)voidasp;
    AppDrawingData *			add= &(asp->aspDrawingData);

    DocumentRectangle			drClip;
    DocumentRectangle			drSym;
    DocumentRectangle			drIgn;

    int					col;
    int					row;
    int					sym;

    int					x0;
    int					y0;
    int					wide;
    int					high;

    const int				ox= 0;
    const int				oy= 0;

    if  ( ! asp->aspFont )
	{ XDEB(asp->aspFont); return;	}

    appCollectExposures( &drClip, add, ox, oy, exposeEvent );

    appDrawSetForegroundColor( add, &(add->addBackColor) );
    appDrawFillRectangle( add, drClip.drX0, drClip.drY0,
					    drClip.drX1- drClip.drX0+ 1,
					    drClip.drY1- drClip.drY0+ 1 );

    appDrawSetForegroundBlack( add );

    y0= 0;
    wide= 1;
    high= asp->aspCellsHigh* ( asp->aspCellSizePixels+ 1 )+ 1;

    for ( col= 0; col <= asp->aspCellsWide; col++ )
	{
	x0= col* ( asp->aspCellSizePixels+ 1 );

	appDrawFillRectangle( add, x0, y0, wide, high );
	}

    x0= 0;
    wide= asp->aspCellsWide* ( asp->aspCellSizePixels+ 1 )+ 1;
    high= 1;

    for ( row= 0; row <= asp->aspCellsHigh; row++ )
	{
	y0= row* ( asp->aspCellSizePixels+ 1 );

	appDrawFillRectangle( add, x0, y0, wide, high );
	}

    appDrawSetForegroundWhite( add );

    wide= high= asp->aspCellSizePixels;
    for ( row= 0; row < asp->aspCellsHigh; row++ )
	{
	for ( col= 0; col < asp->aspCellsWide; col++ )
	    {
	    appSymbolRectangle( &drSym, asp, row, col );

	    sym= row* asp->aspCellsWide+ col;

	    if  ( ! geoIntersectRectangle( &drSym, &drSym, &drClip )	||
		  ! appCharExistsInFont( asp->aspFont, sym )		||
		  sym == asp->aspSymbolSelected				)
		{ continue;	}

	    appDrawFillRectangle( add, drSym.drX0, drSym.drY0,
					    drSym.drX1- drSym.drX0+ 1,
					    drSym.drY1- drSym.drY0+ 1 );
	    }
	}

    appDrawSetForegroundBlack( add );

    wide= high= asp->aspCellSizePixels;
    for ( row= 0; row < asp->aspCellsHigh; row++ )
	{
	for ( col= 0; col < asp->aspCellsWide; col++ )
	    {
	    appSymbolRectangle( &drSym, asp, row, col );

	    sym= row* asp->aspCellsWide+ col;

	    if  ( ! geoIntersectRectangle( &drIgn, &drSym, &drClip )	||
		  ! appCharExistsInFont( asp->aspFont, sym )		||
		  sym == asp->aspSymbolSelected				)
		{ continue;	}

	    appSymbolDrawSymbol( asp, sym, drSym.drX0, drSym.drY0 );
	    }
	}

    sym= asp->aspSymbolSelected;
    if  ( sym >= 0						&&
	  appCharExistsInFont( asp->aspFont, sym )		)
	{
	row= sym/ asp->aspCellsWide;
	col= sym% asp->aspCellsWide;

	appSymbolRectangle( &drSym, asp, row, col );

	if  ( geoIntersectRectangle( &drIgn, &drSym, &drClip ) )
	    {
	    appDrawSetForegroundBlack( add );

	    appDrawFillRectangle( add, drSym.drX0, drSym.drY0,
					    drSym.drX1- drSym.drX0+ 1,
					    drSym.drY1- drSym.drY0+ 1 );

	    appDrawSetForegroundWhite( add );

	    appSymbolDrawSymbol( asp, sym, drSym.drX0, drSym.drY0 );
	    }
	}

    appDrawNoClipping( add );

    return;
    }

/************************************************************************/
/*									*/
/*  Handle mouse clicks.						*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( appSymbolMousePress, w, voidasp, downEvent )
    {
    AppSymbolPicker *	asp= (AppSymbolPicker *)voidasp;

    int			x;
    int			y;
    int			row;
    int			col;
    int			sym;
    int			sameAsPrevious;

    int			oldRow= -1;
    int			oldCol= -1;

    int			button;
    int			upDown;
    int			seq;
    unsigned int	keyState;

    DocumentRectangle	drExpose;

    sym= asp->aspSymbolSelected;
    if  ( sym >= 0 )
	{
	oldRow= sym/ asp->aspCellsWide;
	oldCol= sym% asp->aspCellsWide;
	}

    if  ( appGetCoordinatesFromMouseButtonEvent(
		    &x, &y, &button, &upDown, &seq, &keyState, w, downEvent ) )
	{ return;	}

    if  ( upDown < 1 || button != 1 )
	{ return;	}

    row= y/ ( asp->aspCellSizePixels+ 1 );
    col= x/ ( asp->aspCellSizePixels+ 1 );
    sym= row* asp->aspCellsWide+ col;

    if  ( ! appCharExistsInFont( asp->aspFont, sym ) )
	{ return;	}

    sameAsPrevious= asp->aspSymbolSelected == sym;
    asp->aspSymbolSelected= sym;

    appGuiEnableWidget( asp->aspInsertButton, 1 );

    if  ( oldRow >= 0 && oldCol >= 0 )
	{
	appSymbolRectangle( &drExpose, asp, oldRow, oldCol );

	appExposeRectangle( &(asp->aspDrawingData),
					    drExpose.drX0,
					    drExpose.drY0,
					    drExpose.drX1- drExpose.drX0+ 1,
					    drExpose.drY1- drExpose.drY0+ 1 );
	}

    appSymbolRectangle( &drExpose, asp, row, col );

    appExposeRectangle( &(asp->aspDrawingData),
					    drExpose.drX0,
					    drExpose.drY0,
					    drExpose.drX1- drExpose.drX0+ 1,
					    drExpose.drY1- drExpose.drY0+ 1 );

    if  ( sameAsPrevious && seq == 2 )
	{ appSymbolPickerInsertSymbol( asp );	}

    return;
    }

/************************************************************************/
/*									*/
/*  1)  A font family was selected by the user.				*/
/*  2)  A font encoding was selected by the user.			*/
/*									*/
/************************************************************************/

/*  1  */
static APP_OITEM_CALLBACK_H( appSymbolFontFamilyChosen, w, voidasp )
    {
    int				fam;
    AppSymbolPicker *		asp= (AppSymbolPicker *)voidasp;
    AppEncodingMenu *		aem= &(asp->aspEncodingMenu);
    const DocumentFontList *	dfl= &(asp->aspDocumentFontList);
    const DocumentFontFamily *	dff;
    int				changed= 0;

    TextAttribute *		ta= &(asp->aspTextAttribute);
    int				fontNumber= ta->taFontNumber;

    fam= appGuiGetOptionmenuItemIndex( &(asp->aspFontOptionmenu), w );
    if  ( fam < 0 || fam >= dfl->dflFamilyCount )
	{ LLDEB(fam,dfl->dflFamilyCount); return;	}

    dff= dfl->dflFamilies+ fam;

    if  ( appEncodingMenuAdaptToFamily( &fontNumber, aem, dff ) )
	{ return;	}

    if  ( ta->taFontNumber != fontNumber )
	{ ta->taFontNumber= fontNumber; changed= 1;	}

    if  ( changed )
	{
	asp->aspFontFamilyChosen= fam;

	if  ( appSymbolAdaptToFamily( asp ) )
	    { LDEB(fam);	}
	}

    return;
    }

/*  2  */
static APP_OITEM_CALLBACK_H( appSymbolFontEncodingChosen, w, voidasp )
    {
    int				charsetIdx;
    AppSymbolPicker *		asp= (AppSymbolPicker *)voidasp;
    int				changed= 0;

    charsetIdx= appGuiGetOptionmenuItemIndex(
			&(asp->aspEncodingMenu.aemEncodingOptionmenu), w );
    if  ( charsetIdx < 0 || charsetIdx >= CHARSETidxCOUNT )
	{ LLDEB(charsetIdx,CHARSETidxCOUNT); return;	}

    if  ( asp->aspFontFamilyChosen >= 0 )
	{
	AppEncodingMenu *		aem= &(asp->aspEncodingMenu);
	const DocumentFontList *	dfl= &(asp->aspDocumentFontList);
	const DocumentFontFamily *	dff;

	TextAttribute *			ta= &(asp->aspTextAttribute);
	int				fontNumber= ta->taFontNumber;

	dff= dfl->dflFamilies+ asp->aspFontFamilyChosen;

	if  ( appEncodingMenuAdaptToCharsetIndex( &fontNumber, aem, dff,
								charsetIdx ) )
	    { return;	}

	if  ( ta->taFontNumber != fontNumber )
	    { ta->taFontNumber= fontNumber; changed= 1; }

	if  ( changed )
	    {
	    if  ( appSymbolAdaptDrawingToFont( asp ) )
		{ LDEB(asp->aspFontFamilyChosen);	}
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make the font/encoding part of the symbol picker.			*/
/*									*/
/************************************************************************/

static void appSymbolMakeFontPart(	APP_WIDGET			parent,
					AppSymbolPickerResources *	aspr,
					AppSymbolPicker *		asp )
    {
    APP_WIDGET	row;

    const int	fontColumn= 0;
    const int	fontColspan= 1;
    const int	encodingColumn= fontColumn+ fontColspan;
    const int	encodingColspan= 1;
    const int	columnCount= encodingColumn+ encodingColspan;

    appMakeRowFrameInColumn( &(asp->aspFontFrame),
					    &row, parent,
					    columnCount, aspr->asprFont );

    appMakeOptionmenuInRow( &(asp->aspFontOptionmenu), row,
					    fontColumn, fontColspan );

    appMakeOptionmenuInRow( &(asp->aspEncodingMenu.aemEncodingOptionmenu),
					row, encodingColumn, encodingColspan );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the symbol part of the symbol picker.				*/
/*									*/
/************************************************************************/

static void appSymbolMakeSymbolPart(	APP_WIDGET		parent,
					double			xfac,
					AppSymbolPickerResources * aspr,
					AppSymbolPicker *	asp )
    {
    int				wide;
    int				high;

    const int		twipsSize= 20* 10;
    const int		heightResizable= 0;

    asp->aspCellSizePixels= 1.5* ( xfac* twipsSize )+ 0.5;
    asp->aspCellsWide= 32;
    asp->aspCellsHigh= 8;

    wide= asp->aspCellsWide* ( asp->aspCellSizePixels+ 1 )+ 1;
    high= asp->aspCellsHigh* ( asp->aspCellSizePixels+ 1 )+ 1;

    appGuiMakeDrawingAreaInColumn( &(asp->aspSymbolDrawing), parent,
		wide, high, heightResizable, appSymbolRedraw, (void *)asp );

    appDrawSetButtonPressHandler( asp->aspSymbolDrawing,
				    appSymbolMousePress, (void *)asp );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the button part of the symbol picker.				*/
/*									*/
/************************************************************************/

static APP_WIDGET appSymbolMakeButtonRow(
				    APP_WIDGET			parent,
				    AppSymbolPickerResources *	aspr,
				    AppSymbolPicker *		asp )
    {
    APP_WIDGET		row;
    const int		heightResizable= 0;
    const int		showAsDefault= 0;

    row= appMakeRowInColumn( parent, 3, heightResizable );

    appMakeButtonInRow( &(asp->aspInsertButton), row,
			    aspr->asprInsert, appSymbolInsertPushed,
			    (void *)asp, 0, showAsDefault );

    appMakeButtonInRow( &(asp->aspLowerButton), row,
			    aspr->asprLower, appSymbolLowerPushed,
			    (void *)asp, 1, showAsDefault );

    appMakeButtonInRow( &(asp->aspCloseButton), row,
			    aspr->asprClose, appSymbolClosePushed,
			    (void *)asp, 2, showAsDefault );

    return row;
    }

/************************************************************************/
/*									*/
/*  Fill the list of font families.					*/
/*									*/
/************************************************************************/

/*  1  */
static void appSymbolFillFontMenu(	AppSymbolPickerResources *	aspr,
					AppSymbolPicker *		asp )
    {
    int				fam;
    const DocumentFontList *	dfl= &(asp->aspDocumentFontList);
    const DocumentFontFamily *	dff;

    appEmptyOptionmenu( &(asp->aspFontOptionmenu) );

    if  ( asp->aspFontFamilyOptCount < dfl->dflFamilyCount+ 1 )
	{
	APP_WIDGET *	opts;

	opts= realloc( asp->aspFontFamilyOpts,
				(dfl->dflFamilyCount+ 1)* sizeof(APP_WIDGET) );
	if  ( ! opts )
	    { XDEB(opts); return;	}

	asp->aspFontFamilyOpts= opts;
	asp->aspFontFamilyOptCount= dfl->dflFamilyCount;
	}

    dff= dfl->dflFamilies;
    for ( fam= 0; fam < dfl->dflFamilyCount; dff++, fam++ )
	{
	asp->aspFontFamilyOpts[fam]= appAddItemToOptionmenu(
				    &(asp->aspFontOptionmenu),
				    dff->dffFamilyName,
				    appSymbolFontFamilyChosen, (void *)asp );
	}

    if  ( dfl->dflFamilyCount == 0 )
	{
	asp->aspFontFamilyOpts[0]= appAddItemToOptionmenu(
				&(asp->aspFontOptionmenu), aspr->asprNone,
				appSymbolFontFamilyChosen, (void *)asp );

	appGuiEnableWidget( asp->aspTopWidget, 0 );
	}
    else{ appGuiEnableWidget( asp->aspTopWidget, 1 );	}

    asp->aspFontFamilyChosen= 0;
    appSetOptionmenu( &(asp->aspFontOptionmenu), 0 );

    appOptionmenuRefreshWidth( &(asp->aspFontOptionmenu) );

    return;
    }


/************************************************************************/
/*									*/
/*  Make a Symbol Picker Tool.						*/
/*									*/
/************************************************************************/

void * appMakeSymbolPicker(	APP_WIDGET		symbolOption,
				EditApplication *	ea,
				const char *		widgetName,
				APP_BITMAP_IMAGE	iconPixmap,
				APP_BITMAP_MASK		iconMask,
				SymbolPickerInsert	insert,
				AppToolDestroy		destroy,
				void *			target )
    {
    AppSymbolPicker *			asp;
    
    APP_WIDGET				buttonForm;

    const int				userResizable= 0;

    double				horPixPerMM;
    double				verPixPerMM;
    double				xfac;
    double				yfac;

    static AppSymbolPickerResources	aspr;
    static int				gotResources;

    if  ( ! gotResources )
	{
	appEncodingMenuGetOptionTexts( aspr.asprEncodings, ea );

	appGuiGetResourceValues( &gotResources, ea, (void *)&aspr,
					APP_SymbolPickerResourceTable,
					sizeof(APP_SymbolPickerResourceTable)/
					sizeof(AppConfigurableResource) );
	}

    if  ( appPostScriptFontCatalog( ea ) )
	{ SDEB(ea->eaAfmDirectory);	}

    appGetFactors( ea, &horPixPerMM, &verPixPerMM, &xfac, &yfac );

    asp= (AppSymbolPicker *)malloc( sizeof(AppSymbolPicker) );
    if  ( ! asp )
	{ XDEB(asp); return (void *)0;	}

    asp->aspApplication= ea;
    asp->aspCurrentDocumentId= 0;
    docInitFontList( &(asp->aspDocumentFontList) );

    asp->aspResources= &aspr;

    appInitOptionmenu( &(asp->aspFontOptionmenu) );

    asp->aspTopWidget= (APP_WIDGET)0;
    asp->aspSymbolDrawing= (APP_WIDGET)0;
    asp->aspInsertButton= (APP_WIDGET)0;

    asp->aspDestroy= destroy;
    asp->aspInsert= insert;
    asp->aspTarget= target;

    asp->aspSymbolSelected= -1;
    asp->aspFontFamilyChosen= -1;
    asp->aspFontFamilyOpts= (APP_WIDGET *)0;
    asp->aspFontFamilyOptCount= 0;

    asp->aspFont= (APP_FONT *)0;
    appInitDrawingData( &(asp->aspDrawingData) );

    asp->aspDrawingData.addPostScriptFontList= &(ea->eaPostScriptFontList);

    utilInitTextAttribute( &(asp->aspTextAttribute) );

    appMakeVerticalTool( &(asp->aspTopWidget), &(asp->aspMainWidget), ea,
			    iconPixmap, iconMask, widgetName, userResizable,
			    symbolOption, appCloseSymbolPicker, (void *)asp );

    appSymbolMakeFontPart( asp->aspMainWidget, &aspr, asp );

    appSymbolMakeSymbolPart( asp->aspMainWidget, xfac, &aspr, asp );
    buttonForm= appSymbolMakeButtonRow( asp->aspMainWidget, &aspr, asp );

    appSymbolFillFontMenu( &aspr, asp );
    appEncodingMenuFillOptionmenu( aspr.asprEncodings,
				    appSymbolFontEncodingChosen, (void *)asp,
				    &(asp->aspEncodingMenu) );


#   ifdef USE_MOTIF
    XtRealizeWidget( asp->aspTopWidget );
#   endif

#   ifdef USE_GTK
    gtk_widget_realize( asp->aspSymbolDrawing );
#   endif

    appOptionmenuRefreshWidth( &(asp->aspFontOptionmenu) );
    appOptionmenuRefreshWidth( &(asp->aspEncodingMenu.aemEncodingOptionmenu) );

    appSetDrawingDataForWidget( asp->aspSymbolDrawing,
				ea->eaMagnification, &(asp->aspDrawingData) );

    appShowShellWidget( asp->aspTopWidget );

    return (void *)asp;
    }

/************************************************************************/
/*									*/
/*  Draw a symbol picker to front.					*/
/*									*/
/************************************************************************/

void appShowSymbolPicker(	void *	voidasp	)
    {
    AppSymbolPicker *		asp= (AppSymbolPicker *)voidasp;

    appShowShellWidget( asp->aspTopWidget );
    }

/************************************************************************/
/*									*/
/*  Adapt the symbol Picker to the current font family.			*/
/*									*/
/************************************************************************/

int appAdaptSymbolPickerToFontFamily(
				void *				voidasp,
				unsigned int			documentId,
				const DocumentFontList *	dflFrom,
				const TextAttribute *		taNew,
				const PropertyMask *		newMask )
    {
    AppSymbolPicker *		asp= (AppSymbolPicker *)voidasp;
    DocumentFontList *		dfl= &(asp->aspDocumentFontList);
    AppEncodingMenu *		aem= &(asp->aspEncodingMenu);
    int				rval= 0;
    const DocumentFont *	df= dfl->dflFonts;

    PropertyMask		doneMask;

    if  ( asp->aspCurrentDocumentId != documentId )
	{
	if  ( docCopyFontList( dfl, dflFrom ) )
	    { LDEB(1); return -1; }

	appSymbolFillFontMenu( asp->aspResources, asp );

	asp->aspCurrentDocumentId= documentId;
	}

    PROPmaskCLEAR( &doneMask );
    utilUpdateTextAttribute( &doneMask, &(asp->aspTextAttribute),
							    taNew, newMask );

    asp->aspTextAttribute.taFontSizeHalfPoints= 20;
    asp->aspTextAttribute.taFontIsBold= 0;
    asp->aspTextAttribute.taFontIsSlanted= 0;

    if  ( asp->aspTextAttribute.taFontNumber >= 0 )
	{
	df= dfl->dflFonts+ asp->aspTextAttribute.taFontNumber;
	asp->aspFontFamilyChosen= df->dfDocFamilyIndex;
	}

    if  ( asp->aspFontFamilyChosen < 0 )
	{
	asp->aspFontFamilyChosen= 0; 
	asp->aspTextAttribute.taFontNumber= 0;
	df= dfl->dflFonts+ asp->aspTextAttribute.taFontNumber;
	}

    if  ( appSymbolAdaptToFamily( asp ) )
	{ LDEB(asp->aspFontFamilyChosen); rval= -1;	}

    appSetOptionmenu( &(asp->aspFontOptionmenu),
					    asp->aspFontFamilyChosen );

    appEncodingMenuSetEncoding( aem,
			    dfl->dflFamilies+ asp->aspFontFamilyChosen,
			    df->dfCharsetIndex );

    return rval;
    }

void appEnableSymbolPicker(	void *	voidasp,
				int	enabled )
    {
    AppSymbolPicker *	asp= (AppSymbolPicker *)voidasp;

    appGuiEnableWidget( asp->aspMainWidget, enabled != 0 );

    return;
    }
