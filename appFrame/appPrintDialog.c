/************************************************************************/
/*									*/
/*  Print Dialog.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	<appFrame.h>
#   include	"appUnit.h"
#   include	"appGeoString.h"
#   include	"appPaperChooser.h"
#   include	"appPaper.h"

#   define	DRH_MM	40

/************************************************************************/
/*									*/
/*  Represents a print dialog.						*/
/*									*/
/************************************************************************/

#   define	FILEL	400

#   define	PRINTselAll_PAGES	0
#   define	PRINTselPAGE_RANGE	1
#   define	PRINTselCURRENT_PAGE	2

#   define	PRINTselCOUNT		3

typedef struct PlacementSpecification
    {
    int		psGridRows;
    int		psGridCols;
    int		psGridHorizontal;
    int		psScalePagesToFit;
    char *	psOptionText;
    } PlacementSpecification;

typedef enum PlacementSpecificationNumber
    {
    PSnum_1x1_ASIS= 0,
    PSnum_1x1_SCALED,
    PSnum_2x1_HOR,
    PSnum_1x2_HOR,
    PSnum_2x2_HOR,
    PSnum_2x2_VER,

    PSnum__COUNT
    } PlacementSpecificationNumber;

static PlacementSpecification	APP_Placements[PSnum__COUNT]=
    {
	/* R  C  H  F  "text"		*/
	{  1, 1, 0, 0, "1x1 As Is"	},
	{  1, 1, 1, 1, "1x1 Scale"	},
	{  2, 1, 1, 1, "2x1 Hor"	},
	{  1, 2, 1, 1, "1x2 Hor"	},
	{  2, 2, 1, 1, "2x2 Hor"	},
	{  2, 2, 0, 1, "2x2 Ver"	},
    };

typedef struct AppPrintDialogResources
    {
    char *			apdrPrinterText;
    char *			apdrNoneText;
    char *			apdrCancelText;
    char *			apdrPrintText;

    char *			apdrFaxText;
    char *			apdrPrintToFileText;
    char *			apdrVisiblePrinterCountText;
    char *			apdrPrintersUpText;
    char *			apdrPrintersDownText;

    char *			apdrPaperSizeText;
    char *			apdrCustomPaperSizeText;
    char *			apdrPlacementText;
    char *			apdrShowPlacementDrawingString;
    int				apdrShowPlacementDrawingInt;
    char *			apdrRotate90Text;
    char *			apdrCenterHText;

    char *			apdrSelectionText;
    char *			apdrAllPagesText;
    char *			apdrPageRangeText;
    char *			apdrCurrentPageText;
    char *			apdrFromPageText;
    char *			apdrToPageText;

    char *			apdrDefaultPrinter;

    PlacementSpecification	apdrPlacements[PSnum__COUNT];

    char *			apdrOddFacesOnly;
    char *			apdrEvenFacesOnly;
    char *			apdrFacesForward;
    char *			apdrFacesBackward;
    } AppPrintDialogResources;

static AppConfigurableResource APP_PrintDialogresourceTable[]=
{
    APP_RESOURCE( "printDialogPrinter",
	    offsetof(AppPrintDialogResources,apdrPrinterText),
	    "Printer" ),
    APP_RESOURCE( "printDialogNone",
	    offsetof(AppPrintDialogResources,apdrNoneText),
	    "None" ),
    APP_RESOURCE( "printDialogCancel",
	    offsetof(AppPrintDialogResources,apdrCancelText),
	    "Cancel" ),
    APP_RESOURCE( "printDialogPrint",
	    offsetof(AppPrintDialogResources,apdrPrintText),
	    "Print" ),
    APP_RESOURCE( "printDialogPrintToFile",
	    offsetof(AppPrintDialogResources,apdrPrintToFileText),
	    "Print to File" ),

    APP_RESOURCE( "printDialogFax",
	    offsetof(AppPrintDialogResources,apdrFaxText),
	    "Fax" ),
    APP_RESOURCE( "printDialogPaperSize",
	    offsetof(AppPrintDialogResources,apdrPaperSizeText),
	    "Paper Size" ),
    APP_RESOURCE( "printDialogCustomPaperSize",
	    offsetof(AppPrintDialogResources,apdrCustomPaperSizeText),
	    "Custom" ),

    /***/
    APP_RESOURCE( "printDialogPlacement",
	    offsetof(AppPrintDialogResources,apdrPlacementText),
	    "Placement" ),

    APP_RESOURCE( "printDialogShowPlacementDrawing",
	    offsetof(AppPrintDialogResources,apdrShowPlacementDrawingString),
	    "1" ),

    APP_RESOURCE( "printDialog_1_UpText",
	    offsetof(AppPrintDialogResources,
			apdrPlacements[PSnum_1x1_ASIS].psOptionText),
	    "1 Page/Sheet" ),
    APP_RESOURCE( "printDialog_1_ScaledText",
	    offsetof(AppPrintDialogResources,
			apdrPlacements[PSnum_1x1_SCALED].psOptionText),
	    "1 Page/Sheet Scale to Fit" ),
    APP_RESOURCE( "printDialog_2Row_UpText",
	    offsetof(AppPrintDialogResources,
			apdrPlacements[PSnum_2x1_HOR].psOptionText),
	    "2 Pages/Sheet Vertical" ),
    APP_RESOURCE( "printDialog_2Col_UpText",
	    offsetof(AppPrintDialogResources,
			apdrPlacements[PSnum_1x2_HOR].psOptionText),
	    "2 Pages/Sheet Horizontal" ),
    APP_RESOURCE( "printDialog_4H_UpText",
	    offsetof(AppPrintDialogResources,
			apdrPlacements[PSnum_2x2_HOR].psOptionText),
	    "4 Pages/Sheet Horizontal" ),
    APP_RESOURCE( "printDialog_4V_UpText",
	    offsetof(AppPrintDialogResources,
			apdrPlacements[PSnum_2x2_VER].psOptionText),
	    "4 Pages/Sheet Vertical" ),

    APP_RESOURCE( "printDialogRotate90",
	    offsetof(AppPrintDialogResources,apdrRotate90Text),
	    "Rotate 90\260" ), /* \260 is the degree sign */
    APP_RESOURCE( "printDialogCenterH",
	    offsetof(AppPrintDialogResources,apdrCenterHText),
	    "Center Horizontally" ),

    /***/
    APP_RESOURCE( "printDialogSelection",
	    offsetof(AppPrintDialogResources,apdrSelectionText),
	    "Selection" ),

    APP_RESOURCE( "printDialogAllPages",
	    offsetof(AppPrintDialogResources,apdrAllPagesText),
	    "All Pages" ),
    APP_RESOURCE( "printDialogPageRange",
	    offsetof(AppPrintDialogResources,apdrPageRangeText),
	    "Page Range" ),
    APP_RESOURCE( "printDialogCurrentPage",
	    offsetof(AppPrintDialogResources,apdrCurrentPageText),
	    "Current Page" ),

    APP_RESOURCE( "printDialogFromPage",
	    offsetof(AppPrintDialogResources,apdrFromPageText),
	    "From" ),
    APP_RESOURCE( "printDialogToPage",
	    offsetof(AppPrintDialogResources,apdrToPageText),
	    "To" ),

    APP_RESOURCE( "defaultPrinter",
	    offsetof(AppPrintDialogResources,apdrDefaultPrinter),
	    "" ),

    APP_RESOURCE( "printDialogPrinterCount",
	    offsetof(AppPrintDialogResources,apdrVisiblePrinterCountText),
	    "15" ),
    APP_RESOURCE( "printDialogPrintersUp",
	    offsetof(AppPrintDialogResources,apdrPrintersUpText),
	    "%d up" ),
    APP_RESOURCE( "printDialogPrintersDown",
	    offsetof(AppPrintDialogResources,apdrPrintersDownText),
	    "%d down" ),

    APP_RESOURCE( "printDialogOddFacesOnly",
	    offsetof(AppPrintDialogResources,apdrOddFacesOnly),
	    "Odd Faces Only" ),
    APP_RESOURCE( "printDialogEvenFacesOnly",
	    offsetof(AppPrintDialogResources,apdrEvenFacesOnly),
	    "Even Faces Only" ),

    APP_RESOURCE( "printDialogFacesForward",
	    offsetof(AppPrintDialogResources,apdrFacesForward),
	    "First to Last" ),
    APP_RESOURCE( "printDialogFacesBackward",
	    offsetof(AppPrintDialogResources,apdrFacesBackward),
	    "Last to First" ),
};

typedef struct AppPrintDialog
    {
    const AppPrintDialogResources *	apdResources;
    AppDialog				apdDialog;

    APP_WIDGET				apdPrintButton;
    APP_WIDGET				apdCancelButton;

    AppOptionmenu			apdPrinterOptionmenu;
    AppOptionmenu			apdPlacementOptionmenu;
    AppOptionmenu			apdSelectionOptionmenu;

    APP_WIDGET				apdCustomTransformRow;
    APP_WIDGET				apdRotate90Toggle;
    APP_WIDGET				apdCenterHToggle;

    int					apdUnitType;
    PaperChooser			apdPaperChooser;

    APP_WIDGET				apdFaxNumberText;

    APP_WIDGET				apdPageDrawing;
    AppDrawingData			apdDrawingData;
    APP_FONT *				apdFontStruct;

    APP_WIDGET				apdSelectionOptions[PRINTselCOUNT];
    APP_WIDGET				apdPageFromTextWidget;
    APP_WIDGET				apdPageToTextWidget;

    APP_WIDGET				apdFaceSelectionRow;
    APP_WIDGET				apdOddFacesOnlyToggle;
    APP_WIDGET				apdEvenFacesOnlyToggle;

    APP_WIDGET				apdFaceDirectionRow;
    APP_WIDGET				apdDirectionForwardToggle;
    APP_WIDGET				apdDirectionBackwardToggle;

    int					apdPrinterCount;
    int					apdPrintOptionCount;
    int					apdOptionNrChosen;
    int					apdVisiblePrinterCount;

    int					apdActivateFaxOptions;
    int					apdToFileOptionNr;
    int					apdFaxOptionNr;
    int					apdUpOptionNr;
    int					apdFirstPrinterOptionNr;
    int					apdDownOptionNr;
    int					apdFirstVisiblePrinter;

    DocumentGeometry			apdDocumentGeometry;
    DocumentGeometry			apdPrinterGeometry;
    APP_WIDGET				apdPlacementOptionWidgets[PSnum__COUNT];
    int					apdPlacementChosen;
    int					apdRotate90Chosen;
    int					apdCenterHChosen;

    int					apdPageCount;
    int					apdFirstSelectedPage;
    int					apdLastSelectedPage;
    int					apdSelectionChosen;
    int					apdFirstPageChosen;
    int					apdLastPageChosen;

    int					apdPrintOddFacesOnly;
    int					apdPrintEvenFacesOnly;
    int					apdPrintSheetsReverse;

    void *				apdTarget;

    APP_WIDGET				apdPrinterOptions[1]; 	/* LAST! */
    } AppPrintDialog;

/************************************************************************/
/*									*/
/*  Adapt the menu with printers to the currently selected range of	*/
/*  printers.								*/
/*									*/
/*  1)  All printers can be shown.. No need to offer browsing options	*/
/*  2)  Make the browsing options visible.				*/
/*  3)  Hide all printers before the fist visible one.			*/
/*  4)  Show the range of visible printers.				*/
/*  5)  Hide all printers after the last visible one.			*/
/*  6)  Enable/Disable the browsing options depending on whether there	*/
/*	are any printers before/after the current range.		*/
/*									*/
/************************************************************************/

static void appPrintDialogMenuRange(	AppPrintDialog *	apd )
    {
    int		i;
    int		gotBefore= 0;
    int		gotAfter= 0;

    /*  1  */
    if  ( apd->apdPrinterCount <= apd->apdVisiblePrinterCount )
	{
	if  ( apd->apdUpOptionNr >= 0 )
	    {
	    appOptionmenuItemSetVisibility(
			    apd->apdPrinterOptions[apd->apdUpOptionNr], 0 );
	    }

	if  ( apd->apdDownOptionNr >= 0 )
	    {
	    appOptionmenuItemSetVisibility(
			    apd->apdPrinterOptions[apd->apdDownOptionNr], 0 );
	    }

	return;
	}

    /*  2  */
    appOptionmenuItemSetVisibility(
			    apd->apdPrinterOptions[apd->apdUpOptionNr], 1 );
    appOptionmenuItemSetVisibility(
			    apd->apdPrinterOptions[apd->apdDownOptionNr], 1 );

    /*  3  */
    for ( i= 0; i < apd->apdFirstVisiblePrinter; i++ )
	{
	int	opt= apd->apdFirstPrinterOptionNr+ i;

	appOptionmenuItemSetVisibility( apd->apdPrinterOptions[opt], 0 );
	gotBefore= 1;
	}

    /*  4  */
    for ( i= apd->apdFirstVisiblePrinter;
	  i < apd->apdFirstVisiblePrinter+ apd->apdVisiblePrinterCount &&
	  i < apd->apdPrinterCount;
	  i++ )
	{
	int	opt= apd->apdFirstPrinterOptionNr+ i;

	appOptionmenuItemSetVisibility( apd->apdPrinterOptions[opt], 1 );
	}

    /*  5  */
    for ( i= apd->apdFirstVisiblePrinter+ apd->apdVisiblePrinterCount;
	  i < apd->apdPrinterCount;
	  i++ )
	{
	int	opt= apd->apdFirstPrinterOptionNr+ i;

	appOptionmenuItemSetVisibility( apd->apdPrinterOptions[opt], 0 );
	gotAfter= 1;
	}

    /*  6  */
    appGuiEnableWidget(
		    apd->apdPrinterOptions[apd->apdUpOptionNr], gotBefore );
    appGuiEnableWidget(
		    apd->apdPrinterOptions[apd->apdDownOptionNr], gotAfter );

    appOptionmenuRefreshWidth( &(apd->apdPrinterOptionmenu) );

    return;
    }

/************************************************************************/
/*									*/
/*  Show a range of pages in the Print Dialog.				*/
/*									*/
/************************************************************************/

static void appPrintDialogShowFromTo(	AppPrintDialog *	apd,
					int			fromPage,
					int			toPage,
					int			enabled )
    {
    appIntegerToTextWidget( apd->apdPageFromTextWidget, fromPage );
    appIntegerToTextWidget( apd->apdPageToTextWidget, toPage );

    appEnableText( apd->apdPageFromTextWidget, enabled );
    appEnableText( apd->apdPageToTextWidget, enabled );

    return;
    }

/************************************************************************/
/*									*/
/*  Fax number changed.							*/
/*									*/
/************************************************************************/

static void appPrintCheckFaxNumber(	AppPrintDialog *	apd )
    {
    char *	faxNumber;
    char *	s;

    s= faxNumber= appGetStringFromTextWidget( apd->apdFaxNumberText );

    while( *s == ' ' )
	{ s++;	}

    appGuiEnableWidget( apd->apdPrintButton, *s != '\0' );

    appFreeStringFromTextWidget( faxNumber );

    return;
    }

static APP_TXTYPING_CALLBACK_H( appPrintFaxChanged, w, voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;

    appPrintCheckFaxNumber( apd );

    return;
    }

/************************************************************************/
/*									*/
/*  A printer was chosen.						*/
/*									*/
/************************************************************************/

static void appPrinterAdaptToDestination(	AppPrintDialog *	apd )
    {
    if  ( apd->apdOptionNrChosen == apd->apdFaxOptionNr )
	{
	if  ( apd->apdFaxNumberText )
	    {
	    appEnableText( apd->apdFaxNumberText, 1 );

	    appPrintCheckFaxNumber( apd );
	    }
	}
    else{
	if  ( apd->apdFaxNumberText )
	    {
	    appEnableText( apd->apdFaxNumberText, 0 );
	    }

	appGuiEnableWidget( apd->apdPrintButton, 1 );

	appGuiSetDefaultButtonForDialog( &(apd->apdDialog),
						    apd->apdPrintButton );
	}
    }

static APP_OITEM_CALLBACK_H( appDestinationChosen, w, voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;
    short		n= -1;

    n= appGuiGetOptionmenuItemIndex( &(apd->apdPrinterOptionmenu), w );
    if  ( n < 0 || n >= apd->apdPrintOptionCount )
	{ LLDEB(n,apd->apdPrintOptionCount); return;	}

    apd->apdOptionNrChosen= n;

    appPrinterAdaptToDestination( apd );

    return;
    }

static APP_OITEM_CALLBACK_H( appPrintersUpChosen, w, voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;
    int			printerChosen;

    apd->apdFirstVisiblePrinter -= apd->apdVisiblePrinterCount;
    if  ( apd->apdFirstVisiblePrinter < 0 )
	{ apd->apdFirstVisiblePrinter= 0;	}

    printerChosen= apd->apdFirstVisiblePrinter;
    apd->apdOptionNrChosen= apd->apdFirstPrinterOptionNr+ printerChosen;

    appPrintDialogMenuRange( apd );
    appSetOptionmenu( &(apd->apdPrinterOptionmenu), apd->apdOptionNrChosen );

    return;
    }

static APP_OITEM_CALLBACK_H( appPrintersDownChosen, w, voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;
    int			printerChosen;

    apd->apdFirstVisiblePrinter += apd->apdVisiblePrinterCount;
    if  ( apd->apdFirstVisiblePrinter+ apd->apdVisiblePrinterCount >
						    apd->apdPrinterCount )
	{
	apd->apdFirstVisiblePrinter=
			apd->apdPrinterCount- apd->apdVisiblePrinterCount;
	if  ( apd->apdFirstVisiblePrinter < 0 )
	    { apd->apdFirstVisiblePrinter= 0;	}
	}

    printerChosen= apd->apdFirstVisiblePrinter;
    apd->apdOptionNrChosen= apd->apdFirstPrinterOptionNr+ printerChosen;

    appPrintDialogMenuRange( apd );
    appSetOptionmenu( &(apd->apdPrinterOptionmenu), apd->apdOptionNrChosen );

    return;
    }

/************************************************************************/
/*									*/
/*  Refresh placement drawing.						*/
/*									*/
/************************************************************************/

static void appPrintDialogRefreshPlacementDrawing(
					    AppPrintDialog *	apd )
    {
    AppDrawingData *	add= &(apd->apdDrawingData);

    if  ( ! apd->apdPageDrawing )
	{ return;	}

    appExposeRectangle( add, 0, 0, 0, 0 );
    }

/************************************************************************/
/*									*/
/*  Retrieve the print geometry that is currently set.			*/
/*									*/
/************************************************************************/

static int appPrintDialogPrintGeometry(
				    int *			pCustRelevant,
				    PrintGeometry *		pg,
				    AppPrintDialog *		apd )
    {
    int					customTransformRelevant= 1;

    const AppPrintDialogResources *	apdr= apd->apdResources;
    const PlacementSpecification *	ps;

    PropertyMask			updMask;

    if  ( appPaperChooserGetSize( &updMask,
				    &(apd->apdPaperChooser),
				    &(apd->apdPrinterGeometry) ) )
	{ return -1;	}

    ps= apdr->apdrPlacements+ apd->apdPlacementChosen;

    pg->pgSheetGeometry= apd->apdPrinterGeometry;

    pg->pgRotatePage90= apd->apdRotate90Chosen;
    pg->pgCenterPageHorizontally= apd->apdCenterHChosen;

    if  ( ps->psGridRows * ps->psGridCols != 1 || ps->psScalePagesToFit )
	{
	customTransformRelevant= 0;
	pg->pgRotatePage90= 0;
	pg->pgCenterPageHorizontally= 0;
	}

    pg->pgGridRows= ps->psGridRows;
    pg->pgGridCols= ps->psGridCols;
    pg->pgGridHorizontal= ps->psGridHorizontal;
    pg->pgScalePagesToFit= ps->psScalePagesToFit;

    pg->pgPrintOddSides= ! apd->apdPrintEvenFacesOnly;
    pg->pgPrintEvenSides= ! apd->apdPrintOddFacesOnly;
    pg->pgPrintSheetsReverse= apd->apdPrintSheetsReverse;
    pg->pgPrintBookletOrder= 0;

    *pCustRelevant= customTransformRelevant;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Adapt Custom Transform Row to other settings.			*/
/*									*/
/************************************************************************/

static void appPrintDialogRefreshCustomTransform(
				    AppPrintDialog *	apd )
    {
    int			customTransformRelevant= 1;
    PrintGeometry	pg;

    int			fitsWithRot= 0;
    int			fitsWithoutRot= 0;

    if  ( ! apd->apdCustomTransformRow )
	{ XDEB(apd->apdCustomTransformRow); return;	}

    if  ( appPrintDialogPrintGeometry( &customTransformRelevant, &pg, apd ) )
	{ LDEB(1);	}

    utilNupFitPagesToSheet( &fitsWithoutRot, &fitsWithRot,
					    &pg, &(apd->apdDocumentGeometry) );

    if  ( ! customTransformRelevant )
	{
	appGuiEnableWidget( apd->apdRotate90Toggle, 0 );
	appGuiEnableWidget( apd->apdCenterHToggle, 0 );

	appGuiSetToggleState( apd->apdRotate90Toggle, 0 );
	appGuiSetToggleState( apd->apdCenterHToggle, 0 );
	}
    else{
	int	fitsCentered;

	if  ( apd->apdRotate90Chosen )
	    { fitsCentered= fitsWithRot;	}
	else{ fitsCentered= fitsWithoutRot;	}

	appGuiEnableWidget( apd->apdRotate90Toggle, fitsWithRot );
	appGuiEnableWidget( apd->apdCenterHToggle, fitsCentered );

	appGuiSetToggleState( apd->apdRotate90Toggle,
				    fitsWithRot && apd->apdRotate90Chosen );
	appGuiSetToggleState( apd->apdCenterHToggle,
				    fitsCentered && apd->apdCenterHChosen );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  A placement was chosen.						*/
/*									*/
/************************************************************************/

static APP_OITEM_CALLBACK_H( appPrintDialogPlacementChosen, w, voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;
    int			i;

    i= appGuiGetOptionmenuItemIndex( &(apd->apdPlacementOptionmenu), w );
    if  ( i < 0 || i >= PSnum__COUNT )
	{ LLDEB(i,PSnum__COUNT); return;	}

    apd->apdPlacementChosen= i;

    if  ( apd->apdCustomTransformRow )
	{ appPrintDialogRefreshCustomTransform( apd );	}

    appPrintDialogRefreshPlacementDrawing( apd );

    return;
    }

/************************************************************************/
/*									*/
/*  A selection mode was chosen.					*/
/*									*/
/************************************************************************/

static APP_OITEM_CALLBACK_H( appSelectionChosen, w, voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;
    int			n;

#   ifdef USE_MOTIF
    XSetInputFocus( XtDisplay( apd->apdDialog.adTopWidget ),
				    XtWindow( apd->apdDialog.adTopWidget ),
				    RevertToNone, CurrentTime );
#   endif

    n= appGuiGetOptionmenuItemIndex( &(apd->apdSelectionOptionmenu), w );
    if  ( n < 0 || n >= PRINTselCOUNT )
	{ LLDEB(n,PRINTselCOUNT); return;	}

    switch( n )
	{
	case PRINTselAll_PAGES:
	    apd->apdFirstPageChosen= 0;
	    apd->apdLastPageChosen= apd->apdPageCount- 1;

	    appPrintDialogShowFromTo( apd, 1, apd->apdPageCount, 0 );
	    apd->apdSelectionChosen= n;
	    return;

	case PRINTselPAGE_RANGE:
	    appEnableText( apd->apdPageFromTextWidget, 1 );
	    appEnableText( apd->apdPageToTextWidget, 1 );
	    apd->apdSelectionChosen= n;

#	    ifdef USE_MOTIF
	    XmProcessTraversal( apd->apdPageFromTextWidget,
							XmTRAVERSE_CURRENT );
#	    endif
	    return;

	case PRINTselCURRENT_PAGE:
	    apd->apdFirstPageChosen= apd->apdFirstSelectedPage;
	    apd->apdLastPageChosen= apd->apdLastSelectedPage;

	    appPrintDialogShowFromTo( apd, apd->apdFirstSelectedPage+ 1,
					    apd->apdLastSelectedPage+ 1, 0 );
	    apd->apdSelectionChosen= n;
	    return;

	default:
	    LDEB(n); return;
	}
    }

/************************************************************************/
/*									*/
/*  The buttons have been pushed.					*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( appPrintDialogCancelPushed, w, voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;

    appGuiBreakDialog( &(apd->apdDialog), AQDrespCANCEL );

    return;
    }

static APP_BUTTON_CALLBACK_H( appPrintDialogPrintPushed, w, voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;

    if  ( apd->apdOptionNrChosen < 0	)
	{ LDEB(apd->apdOptionNrChosen); return;	}

    if  ( apd->apdSelectionChosen == PRINTselPAGE_RANGE )
	{
	int		from;
	int		to;

	if  ( appGetIntegerFromTextWidget( apd->apdPageFromTextWidget,
					&from, 1, 0, apd->apdPageCount, 0 ) )
	    { return;	}

	if  ( appGetIntegerFromTextWidget( apd->apdPageToTextWidget,
					&to, from, 0, apd->apdPageCount, 1 ) )
	    { return;	}

	apd->apdFirstPageChosen= from- 1;
	apd->apdLastPageChosen= to- 1;
	}

    appGuiBreakDialog( &(apd->apdDialog), AQDrespOK );

    return;
    }

/************************************************************************/
/*									*/
/*  A paper size was chosen.						*/
/*									*/
/************************************************************************/

static void appPrintDialogPaperRectChanged(
				    PaperChooser *		pc,
				    void *			voidapd,
				    const DocumentGeometry *	dg )
    {
    AppPrintDialog *		apd= (AppPrintDialog *)voidapd;

    apd->apdPrinterGeometry.dgPageWideTwips= dg->dgPageWideTwips;
    apd->apdPrinterGeometry.dgPageHighTwips= dg->dgPageHighTwips;

    if  ( apd->apdCustomTransformRow )
	{ appPrintDialogRefreshCustomTransform( apd );	}

    appPrintDialogRefreshPlacementDrawing( apd );

    return;
    }

/************************************************************************/
/*									*/
/*  The custom placement toggles were activated.			*/
/*									*/
/************************************************************************/

static APP_TOGGLE_CALLBACK_H( appPrintDialogRotate90Toggled, w, voidapd, e )
    {
    AppPrintDialog *		apd= (AppPrintDialog *)voidapd;
    int				set;

    set= appGuiGetToggleStateFromCallback( w, e );

    apd->apdRotate90Chosen= ( set != 0 );

    if  ( apd->apdCustomTransformRow )
	{ appPrintDialogRefreshCustomTransform( apd );	}

    appPrintDialogRefreshPlacementDrawing( apd );

    return;
    }

static APP_TOGGLE_CALLBACK_H( appPrintDialogCenterHToggled, w, voidapd, e )
    {
    AppPrintDialog *		apd= (AppPrintDialog *)voidapd;
    int				set;

    set= appGuiGetToggleStateFromCallback( w, e );

    apd->apdCenterHChosen= ( set != 0 );

    if  ( apd->apdCustomTransformRow )
	{ appPrintDialogRefreshCustomTransform( apd );	}

    appPrintDialogRefreshPlacementDrawing( apd );

    return;
    }

/************************************************************************/
/*									*/
/*  The Odd/Even pages only toggles have been activated.		*/
/*									*/
/************************************************************************/

static APP_TOGGLE_CALLBACK_H( appPrintDialogOddOnlyToggled, w, voidapd, e )
    {
    AppPrintDialog *		apd= (AppPrintDialog *)voidapd;
    int				set;

    set= appGuiGetToggleStateFromCallback( w, e );

    apd->apdPrintOddFacesOnly= ( set != 0 );

    if  ( set )
	{
	apd->apdPrintEvenFacesOnly= 0;
	appGuiSetToggleState( apd->apdEvenFacesOnlyToggle,
						apd->apdPrintEvenFacesOnly );
	}

    return;
    }

static APP_TOGGLE_CALLBACK_H( appPrintDialogEvenOnlyToggled, w, voidapd, e )
    {
    AppPrintDialog *		apd= (AppPrintDialog *)voidapd;
    int				set;

    set= appGuiGetToggleStateFromCallback( w, e );

    apd->apdPrintEvenFacesOnly= ( set != 0 );

    if  ( set )
	{
	apd->apdPrintOddFacesOnly= 0;
	appGuiSetToggleState( apd->apdOddFacesOnlyToggle,
						apd->apdPrintOddFacesOnly );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  The Forward/Backward pages only toggles have been activated.	*/
/*									*/
/************************************************************************/

static APP_TOGGLE_CALLBACK_H( appPrintDialogForwardToggled, w, voidapd, e )
    {
    AppPrintDialog *		apd= (AppPrintDialog *)voidapd;
    int				set;

    set= appGuiGetToggleStateFromCallback( w, e );

    if  ( apd->apdPrintSheetsReverse != ( set == 0 ) )
	{
	apd->apdPrintSheetsReverse= ( set == 0 );

	appGuiSetToggleState( apd->apdDirectionBackwardToggle,
						apd->apdPrintSheetsReverse );
	}

    return;
    }

static APP_TOGGLE_CALLBACK_H( appPrintDialogBackwardToggled, w, voidapd, e )
    {
    AppPrintDialog *		apd= (AppPrintDialog *)voidapd;
    int				set;

    set= appGuiGetToggleStateFromCallback( w, e );

    if  ( apd->apdPrintSheetsReverse != ( set != 0 ) )
	{
	apd->apdPrintSheetsReverse= ( set != 0 );

	appGuiSetToggleState( apd->apdDirectionForwardToggle,
						! apd->apdPrintSheetsReverse );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a schematic view of the page placement.			*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( appPrintDialogDrawPage, w, voidapd, exposeEvent )
    {
    AppPrintDialog *		apd= (AppPrintDialog *)voidapd;
    AppDrawingData *		add= &(apd->apdDrawingData);

    int				customTransformRelevant= 1;
    PrintGeometry		pg;

    if  ( ! apd->apdPageDrawing )
	{ XDEB(apd->apdPageDrawing); return;	}

    if  ( apd->apdPlacementChosen < 0			||
	  apd->apdPlacementChosen >= PSnum__COUNT	)
	{ LLDEB(apd->apdPlacementChosen,PSnum__COUNT); return;	}

    if  ( appPrintDialogPrintGeometry( &customTransformRelevant, &pg, apd ) )
	{ LDEB(1);	}

    appDrawNupDiagram( apd->apdPageDrawing, add, apd->apdFontStruct, DRH_MM,
					    &(apd->apdDocumentGeometry), &pg );
    }

/************************************************************************/
/*									*/
/*  Fill the list of printers.						*/
/*									*/
/*  1)  Empty option menu. It should already be empty.			*/
/*  2)  Insert print to file option.					*/
/*  3)  Insert fax option.						*/
/*  4)  Insert printers up option.					*/
/*  5)  Insert options for the actual printers.				*/
/*  6)  Insert printers down option.					*/
/*									*/
/************************************************************************/

static void appPrintDialogFillPrinterMenu(
				const AppPrintDialogResources *	apdr,
				AppPrintDialog *		apd,
				int				printerCount,
				int				defaultPrinter,
				PrintDestination *		pd )
    {
    int			opt;
    int			prt;

    int			optionChosen= -1;
    int			firstPrinter= -1;

    /*  1  */
    appEmptyOptionmenu( &(apd->apdPrinterOptionmenu) );

    opt= 0;

    /*  2  */
    {
    apd->apdPrinterOptions[opt]= appAddItemToOptionmenu(
					&(apd->apdPrinterOptionmenu),
					apdr->apdrPrintToFileText,
					appDestinationChosen, (void *)apd );

    if  ( optionChosen < 0 )
	{ optionChosen= opt;	}

    apd->apdToFileOptionNr= opt++;
    }

    /*  3  */
    {
    apd->apdPrinterOptions[opt]= appAddItemToOptionmenu(
					&(apd->apdPrinterOptionmenu),
					apdr->apdrFaxText,
					appDestinationChosen, (void *)apd );

    if  ( ! apd->apdActivateFaxOptions )
	{ appGuiEnableWidget( apd->apdPrinterOptions[opt], 0 ); }

    apd->apdFaxOptionNr= opt++;
    }

    /*  4  */
    {
    int		ltxt= strlen( apdr->apdrPrintersUpText );
    int		lval= strlen( apdr->apdrVisiblePrinterCountText );
    char *	scratch;

    scratch= malloc( ltxt+ lval+ 1 );
    if  ( ! scratch )
	{
	LLXDEB(ltxt,lval,scratch);
	apd->apdPrinterOptions[opt]= appAddItemToOptionmenu(
					&(apd->apdPrinterOptionmenu),
					apdr->apdrPrintersUpText,
					appPrintersUpChosen, (void *)apd );
	}
    else{
	int	pos;

	for ( pos= 0; pos < ltxt; pos++ )
	    {
	    if  ( apdr->apdrPrintersUpText[pos+ 0] == '%'	&&
		  apdr->apdrPrintersUpText[pos+ 1] == 'd'	)
		{ break;	}
	    }

	strncpy( scratch, apdr->apdrPrintersUpText, pos );
	strncpy( scratch+ pos, apdr->apdrVisiblePrinterCountText, lval );

	if  ( pos < ltxt )
	    { strcpy( scratch+ pos+ lval ,apdr->apdrPrintersUpText+ pos+ 2 ); }
	else{ strcpy( scratch+ pos+ lval ,apdr->apdrPrintersUpText+ pos );    }
	apd->apdPrinterOptions[opt]= appAddItemToOptionmenu(
					&(apd->apdPrinterOptionmenu),
					scratch,
					appPrintersUpChosen, (void *)apd );
	free( scratch );
	}

    apd->apdUpOptionNr= opt++;
    }

    /*  5  */
    apd->apdFirstPrinterOptionNr= opt;

    for ( prt= 0; prt < printerCount; pd++, opt++, prt++ )
	{
	apd->apdPrinterOptions[opt]=
			appAddItemToOptionmenu( &(apd->apdPrinterOptionmenu),
			pd->pdPrinterName, appDestinationChosen,
			(void *)apd );

	if  ( prt == defaultPrinter )
	    {
	    optionChosen= opt;
	    firstPrinter= prt;
	    }
	if  ( prt == 0 )
	    {
	    optionChosen= opt;
	    firstPrinter= prt;
	    }
	}

    /*  6  */
    {
    int		ltxt= strlen( apdr->apdrPrintersDownText );
    int		lval= strlen( apdr->apdrVisiblePrinterCountText );
    char *	scratch;

    scratch= malloc( ltxt+ lval+ 1 );
    if  ( ! scratch )
	{
	LLXDEB(ltxt,lval,scratch);
	apd->apdPrinterOptions[opt]= appAddItemToOptionmenu(
					&(apd->apdPrinterOptionmenu),
					apdr->apdrPrintersDownText,
					appPrintersDownChosen, (void *)apd );
	}
    else{
	int	pos;

	for ( pos= 0; pos < ltxt; pos++ )
	    {
	    if  ( apdr->apdrPrintersDownText[pos+ 0] == '%'	&&
		  apdr->apdrPrintersDownText[pos+ 1] == 'd'	)
		{ break;	}
	    }

	strncpy( scratch, apdr->apdrPrintersDownText, pos );
	strncpy( scratch+ pos, apdr->apdrVisiblePrinterCountText, lval );

	if  ( pos < ltxt )
	    { strcpy( scratch+ pos+ lval ,apdr->apdrPrintersDownText+ pos+ 2 ); }
	else{ strcpy( scratch+ pos+ lval ,apdr->apdrPrintersDownText+ pos );    }

	apd->apdPrinterOptions[opt]= appAddItemToOptionmenu(
					&(apd->apdPrinterOptionmenu),
					scratch,
					appPrintersDownChosen, (void *)apd );
	free( scratch );
	}

    apd->apdDownOptionNr= opt++;
    }

    apd->apdPrintOptionCount= opt;
    
    apd->apdPrinterCount= printerCount;
    apd->apdOptionNrChosen= optionChosen;
    apd->apdFirstVisiblePrinter= firstPrinter;

    appSetOptionmenu( &(apd->apdPrinterOptionmenu), apd->apdOptionNrChosen );
    appPrintDialogMenuRange( apd );

    appOptionmenuRefreshWidth( &(apd->apdPrinterOptionmenu) );
    }

/************************************************************************/
/*									*/
/*  Fill the list of possible page placements.				*/
/*									*/
/************************************************************************/

static void appPrintDialogFillPlacementMenu( AppPrintDialog *	apd )
    {
    const AppPrintDialogResources *	apdr= apd->apdResources;
    int					i;

    const PlacementSpecification *	ps;

    appEmptyOptionmenu( &(apd->apdPlacementOptionmenu) );

    ps= apdr->apdrPlacements;
    for ( i= 0; i < PSnum__COUNT; ps++, i++ )
	{
	apd->apdPlacementOptionWidgets[i]= appAddItemToOptionmenu(
				&(apd->apdPlacementOptionmenu),
				ps->psOptionText,
				appPrintDialogPlacementChosen, (void *)apd );
	}

    apd->apdPlacementChosen= 0;
    apd->apdRotate90Chosen= 0;
    apd->apdCenterHChosen= 0;

    appSetOptionmenu( &(apd->apdPlacementOptionmenu), 0 );

    appOptionmenuRefreshWidth( &(apd->apdPlacementOptionmenu) );
    }

/************************************************************************/
/*									*/
/*  Fill the list of selection options.					*/
/*									*/
/************************************************************************/

#   define	PRINTselAll_PAGES	0
#   define	PRINTselPAGE_RANGE	1
#   define	PRINTselCURRENT_PAGE	2

static void appPrintDialogFillSelectionMenu( AppPrintDialog *	apd )
    {
    const AppPrintDialogResources *	apdr= apd->apdResources;

    appEmptyOptionmenu( &(apd->apdSelectionOptionmenu) );

    apd->apdSelectionOptions[PRINTselAll_PAGES]= appAddItemToOptionmenu(
					    &(apd->apdSelectionOptionmenu),
					    apdr->apdrAllPagesText,
					    appSelectionChosen, (void *)apd );
    apd->apdSelectionOptions[PRINTselPAGE_RANGE]= appAddItemToOptionmenu(
					    &(apd->apdSelectionOptionmenu),
					    apdr->apdrPageRangeText,
					    appSelectionChosen, (void *)apd );
    apd->apdSelectionOptions[PRINTselCURRENT_PAGE]= appAddItemToOptionmenu(
					    &(apd->apdSelectionOptionmenu),
					    apdr->apdrCurrentPageText,
					    appSelectionChosen, (void *)apd );

    apd->apdSelectionChosen= PRINTselAll_PAGES;

    appSetOptionmenu( &(apd->apdSelectionOptionmenu),
						    apd->apdSelectionChosen );

    appOptionmenuRefreshWidth( &(apd->apdSelectionOptionmenu) );
    }

/************************************************************************/
/*									*/
/*  Protocol callback: The print dialog is closed through the window	*/
/*  manager. Interpret this as 'Cancel'					*/
/*									*/
/************************************************************************/

static APP_CLOSE_CALLBACK_H( appClosePrintDialog, w, voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;

    appGuiBreakDialog( &(apd->apdDialog), AQDrespCANCEL );

    return;
    }

static APP_DESTROY_CALLBACK_H( appDestroyPrintDialog, w, voidapd )
    {
    AppPrintDialog *	apd= (AppPrintDialog *)voidapd;

    appCleanPaperChooser( &(apd->apdPaperChooser) );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the frame for selecting a printer.				*/
/*									*/
/************************************************************************/

static APP_WIDGET appPrintDialogMakePrinterFrame(
				APP_WIDGET			parent,
				const AppPrintDialogResources *	apdr,
				AppPrintDialog *		apd )
    {
    APP_WIDGET	printerFrame;
    APP_WIDGET	frameColumn;

    APP_WIDGET	row;

    const int	heightResizable= 0;
    const int	textEnabled= 0;
    const int	textColumn= 1;
    const int	textColspan= 1;

    appMakeColumnFrameInColumn( &printerFrame, &frameColumn,
						parent, apdr->apdrPrinterText );

    row= appMakeRowInColumn( frameColumn, 2, heightResizable );

    appMakeOptionmenuInRow( &(apd->apdPrinterOptionmenu), row, 0, 1 );

    appMakeTextInRow( &(apd->apdFaxNumberText), row, textColumn,
					    textColspan, 15, textEnabled );

    appGuiSetTypingCallbackForText( apd->apdFaxNumberText,
					    appPrintFaxChanged, (void *)apd );

    return printerFrame;
    }

/************************************************************************/
/*									*/
/*  Make the frame for giving page placement.				*/
/*									*/
/************************************************************************/

static APP_WIDGET appPrintDialogMakePlacementFrame(
				APP_WIDGET			parent,
				EditApplication *		ea,
				const AppPrintDialogResources *	apdr,
				AppPrintDialog *		apd )
    {
    APP_WIDGET		placementFrame;
    APP_WIDGET		frameColumn;

    const int		heightResizable= 0;

    appMakeColumnFrameInColumn( &placementFrame, &frameColumn, parent,
						    apdr->apdrPlacementText );

    appMakeOptionmenuInColumn( &(apd->apdPlacementOptionmenu), frameColumn );

    apd->apdCustomTransformRow=
			appMakeRowInColumn( frameColumn, 2, heightResizable );
    apd->apdRotate90Toggle= appMakeToggleInRow( apd->apdCustomTransformRow,
				apdr->apdrRotate90Text,
				appPrintDialogRotate90Toggled, (void *)apd, 0 );
    apd->apdCenterHToggle= appMakeToggleInRow( apd->apdCustomTransformRow,
				apdr->apdrCenterHText,
				appPrintDialogCenterHToggled, (void *)apd, 1 );

    apd->apdPageDrawing= (APP_WIDGET)0;
    if  ( apdr->apdrShowPlacementDrawingInt >= 0 )
	{
	apd->apdPageDrawing= appMakePageDrawing( parent, ea, DRH_MM,
					appPrintDialogDrawPage, (void *)apd );
	}

    return placementFrame;
    }

/************************************************************************/
/*									*/
/*  Make the frame to select what pages are to be printed.		*/
/*									*/
/************************************************************************/

static APP_WIDGET appPrintDialogMakeSelectionFrame(
				APP_WIDGET			parent,
				const AppPrintDialogResources *	apdr,
				AppPrintDialog *		apd )
    {
    APP_WIDGET		selectionFrame;
    APP_WIDGET		frameColumn;

    APP_WIDGET		pageSelectionRow;
    APP_WIDGET		row;
    APP_WIDGET		leftColumn;
    APP_WIDGET		rightColumn;
    APP_WIDGET		fromLabel;
    APP_WIDGET		toLabel;

    const int		heightResizable= 0;
    const int		textEnabled= 0;
    const int		colspan= 1;

    const int		makeCollationWidgets= 0;

    appMakeColumnFrameInColumn( &selectionFrame, &frameColumn, parent,
						    apdr->apdrSelectionText );

    appMakeOptionmenuInColumn( &(apd->apdSelectionOptionmenu), frameColumn );

    /**/
    pageSelectionRow= appMakeRowInColumn( frameColumn, 2, heightResizable );

    appMakeColumnInRow( &leftColumn, pageSelectionRow, 0, colspan );
    appMakeColumnInRow( &rightColumn, pageSelectionRow, 1, colspan );

    appMakeLabelAndTextRow( &row, &fromLabel, &(apd->apdPageFromTextWidget),
			leftColumn, apdr->apdrFromPageText, 8, textEnabled );

    appMakeLabelAndTextRow( &row, &toLabel, &(apd->apdPageToTextWidget),
			rightColumn, apdr->apdrToPageText, 8, textEnabled );

    /**/
    apd->apdFaceSelectionRow= (APP_WIDGET)0;
    apd->apdOddFacesOnlyToggle= (APP_WIDGET)0;
    apd->apdEvenFacesOnlyToggle= (APP_WIDGET)0;

    if  ( makeCollationWidgets )
	{
	apd->apdFaceSelectionRow=
			appMakeRowInColumn( frameColumn, 2, heightResizable );
	apd->apdOddFacesOnlyToggle= appMakeToggleInRow(
			    apd->apdFaceSelectionRow,
			    apdr->apdrOddFacesOnly,
			    appPrintDialogOddOnlyToggled, (void *)apd, 0 );
	apd->apdEvenFacesOnlyToggle= appMakeToggleInRow(
			    apd->apdFaceSelectionRow,
			    apdr->apdrEvenFacesOnly,
			    appPrintDialogEvenOnlyToggled, (void *)apd, 1 );
	}

    /**/
    apd->apdFaceDirectionRow= (APP_WIDGET)0;
    apd->apdDirectionForwardToggle= (APP_WIDGET)0;
    apd->apdDirectionBackwardToggle= (APP_WIDGET)0;

    if  ( makeCollationWidgets )
	{
	apd->apdFaceDirectionRow=
			appMakeRowInColumn( frameColumn, 2, heightResizable );
	apd->apdDirectionForwardToggle= appMakeToggleInRow(
			    apd->apdFaceDirectionRow,
			    apdr->apdrFacesForward,
			    appPrintDialogForwardToggled, (void *)apd, 0 );
	apd->apdDirectionBackwardToggle= appMakeToggleInRow(
			    apd->apdFaceDirectionRow,
			    apdr->apdrFacesBackward,
			    appPrintDialogBackwardToggled, (void *)apd, 1 );
	}

    return selectionFrame;
    }

/************************************************************************/
/*									*/
/*  Make the form with the two buttons.					*/
/*									*/
/************************************************************************/

static APP_WIDGET appPrintDialogMakeButtonRow(
				APP_WIDGET			parent,
				const AppPrintDialogResources *	apdr,
				AppPrintDialog *		apd )
    {
    APP_WIDGET		row;
    const int		heightResizable= 0;

    row= appMakeRowInColumn( parent, 2, heightResizable );

    appMakeButtonInRow( &(apd->apdPrintButton), row, apdr->apdrPrintText,
			appPrintDialogPrintPushed, (void *)apd, 0, 1 );

    appMakeButtonInRow( &(apd->apdCancelButton), row, apdr->apdrCancelText,
			appPrintDialogCancelPushed, (void *)apd, 1, 0 );

    appGuiSetCancelButtonForDialog( &(apd->apdDialog),
						    apd->apdCancelButton );
    return row;
    }

/************************************************************************/
/*									*/
/*  make a page tool.							*/
/*									*/
/************************************************************************/

static AppPrintDialog * appMakePrintDialog( EditApplication *	ea,
					EditDocument *		ed,
					APP_WIDGET		printOption,
					const char *		pixmapName )
    {
    AppPrintDialog *	apd;
    
    APP_WIDGET		printerFrame;
    APP_WIDGET		selectionFrame;
    APP_WIDGET		paned;
    APP_WIDGET		buttonRow;

    int			i;

    int			high;
    int			wide;
    int			d;

    static int				gotResources;
    static AppPrintDialogResources	apdr;

    APP_BITMAP_IMAGE	iconPixmap= (APP_BITMAP_IMAGE)0;
    APP_BITMAP_MASK	iconMask= (APP_BITMAP_MASK)0;
    
    char		name[128];

    const double	magnification= 1.0;

    if  ( appGetImagePixmap( ea, pixmapName, &iconPixmap, &iconMask )  )
	{ SDEB(pixmapName); return (AppPrintDialog *)0;	}

    if  ( ! ea->eaPrintDestinationsCollected	&&
	  appGetPrintDestinations( ea )		)
	{ LDEB(1); return (AppPrintDialog *)0;	}

    apd= (AppPrintDialog *)malloc(
		    sizeof(AppPrintDialog)+
		    ( ea->eaPrintDestinationCount+ 4 )* sizeof(APP_WIDGET) );
    if  ( ! apd )
	{
	LXDEB(ea->eaPrintDestinationCount,apd);
	return (AppPrintDialog *)0;
	}

    appInitOptionmenu( &(apd->apdPrinterOptionmenu) );
    appInitOptionmenu( &(apd->apdPlacementOptionmenu) );
    appInitOptionmenu( &(apd->apdSelectionOptionmenu) );

    apd->apdCustomTransformRow= (APP_WIDGET)0;
    apd->apdRotate90Toggle= (APP_WIDGET)0;
    apd->apdCenterHToggle= (APP_WIDGET)0;
    apd->apdResources= &apdr;

    appInitDrawingData( &(apd->apdDrawingData) );

    if  ( ! gotResources )
	{
	for ( i= 0; i < PSnum__COUNT; i++ )
	    { apdr.apdrPlacements[i]= APP_Placements[i];	}

	appGuiGetResourceValues( &gotResources, ea, &apdr,
				    APP_PrintDialogresourceTable,
				    sizeof(APP_PrintDialogresourceTable)/
				    sizeof(AppConfigurableResource) );

	apdr.apdrShowPlacementDrawingInt= 0;
	if  ( apdr.apdrShowPlacementDrawingString )
	    {
	    if  ( ! strcmp( apdr.apdrShowPlacementDrawingString, "1" ) )
		{ apdr.apdrShowPlacementDrawingInt=  1;	}
	    if  ( ! strcmp( apdr.apdrShowPlacementDrawingString, "0" ) )
		{ apdr.apdrShowPlacementDrawingInt= -1;	}

	    if  ( apdr.apdrShowPlacementDrawingInt == 0 )
		{ SDEB(apdr.apdrShowPlacementDrawingString);	}
	    }
	}

    apd->apdVisiblePrinterCount= 0;
    if  ( apdr.apdrVisiblePrinterCountText )
	{
	char *	past= apdr.apdrVisiblePrinterCountText;

	apd->apdVisiblePrinterCount=
		strtol( apdr.apdrVisiblePrinterCountText, &past, 0 );

	while( *past == ' ' )
	    { past++;	}
	if  ( *past )
	    {
	    SDEB(apdr.apdrVisiblePrinterCountText);
	    apd->apdVisiblePrinterCount= 0;
	    }
	}
    if  ( apd->apdVisiblePrinterCount < 1 )
	{ apd->apdVisiblePrinterCount= 15;	}

    if  ( ea->eaFaxCommand && ea->eaFaxCommand[0] )
	{ apd->apdActivateFaxOptions= 1;	}
    else{ apd->apdActivateFaxOptions= 0;	}

    if  ( apdr.apdrDefaultPrinter && apdr.apdrDefaultPrinter[0] )
	{
	PrintDestination *	pd= ea->eaPrintDestinations;

	for ( i= 0; i < ea->eaPrintDestinationCount; pd++, i++ )
	    {
	    if  ( ! strcmp( pd->pdPrinterName, apdr.apdrDefaultPrinter ) )
		{ ea->eaDefaultPrintDestination= i; break;	}
	    }
	}

    apd->apdTarget= (void *)ea;
    apd->apdPrinterCount= 0;
    apd->apdPrintOptionCount= 0;
    apd->apdToFileOptionNr= -1;
    apd->apdFaxOptionNr= -1;
    apd->apdUnitType= ea->eaUnitInt;

    appInitPaperChooser( &(apd->apdPaperChooser) );

    appMakeVerticalDialog( &(apd->apdDialog), &paned, ea,
						appClosePrintDialog,
						appDestroyPrintDialog,
						(void *)apd,
						ea->eaPrintDialogName );

    appSetShellTitle( apd->apdDialog.adTopWidget,
					printOption, ea->eaApplicationName );

    printerFrame= appPrintDialogMakePrinterFrame( paned, &apdr, apd );

    appMakePaperChooserWidgets( paned, apdr.apdrPaperSizeText,
					    apd->apdUnitType,
					    &(apd->apdPaperChooser),
					    appPrintDialogPaperRectChanged,
					    (void *)apd );

    appPrintDialogMakePlacementFrame( paned, ea, &apdr, apd );
    selectionFrame= appPrintDialogMakeSelectionFrame( paned, &apdr, apd );
    buttonRow= appPrintDialogMakeButtonRow( paned, &apdr, apd );

    appPrintDialogFillPrinterMenu( &apdr, apd,
				    ea->eaPrintDestinationCount,
				    ea->eaDefaultPrintDestination,
				    ea->eaPrintDestinations );

    appPrintDialogFillPlacementMenu( apd );
    appPrintDialogFillSelectionMenu( apd );

    appPaperChooserFillMenu( &(apd->apdPaperChooser),
					    apdr.apdrCustomPaperSizeText );

    appGuiShowDialog( &(apd->apdDialog), ed->edToplevel.atTopWidget );

    appPaperChooserRetreshMenuWidth( &(apd->apdPaperChooser) );

    appOptionmenuRefreshWidth( &(apd->apdPrinterOptionmenu) );
    appOptionmenuRefreshWidth( &(apd->apdPlacementOptionmenu) );
    appOptionmenuRefreshWidth( &(apd->apdSelectionOptionmenu) );

    if  ( apd->apdPageDrawing )
	{
	AppDrawingData *	add= &(apd->apdDrawingData);

	if  ( appSetDrawingDataForWidget( apd->apdPageDrawing,
							magnification, add ) )
	    { LDEB(1);	}

	appDrawGetSizeOfWidget( &wide, &high, apd->apdPageDrawing );

	d= high/ 24;

	for ( i= 0; i <= d; i++ )
	    {
	    /**/
	    sprintf( name, ea->eaPrintDialogFont, high/ 12- i );
	    apd->apdFontStruct= appDrawOpenFont( add, name );

	    if  ( apd->apdFontStruct )
		{ appDrawSetFont( add, apd->apdFontStruct ); break; }

	    /**/
	    sprintf( name, ea->eaPrintDialogFont, high/ 12+ i );
	    apd->apdFontStruct= appDrawOpenFont( add, name );

	    if  ( apd->apdFontStruct )
		{ appDrawSetFont( add, apd->apdFontStruct ); break; }
	    }

	if  ( ! apd->apdFontStruct )
	    { SLXDEB(ea->eaPrintDialogFont,high/12,apd->apdFontStruct);	}
	}

    return apd;
    }

/************************************************************************/
/*									*/
/*  Show the 'Print...' dialog.						*/
/*									*/
/*  1)	Make or just show it.						*/
/*  2)	Default destination is set by appMakePrintDialog().		*/
/*  3)	Make Fax View by Martin Vermeer happy.				*/
/*									*/
/************************************************************************/

void appRunPrintDialog(			EditDocument *		ed,
					const DocumentGeometry * dgDocument,
					int			pageCount,
					int			firstSelected,
					int			lastSelected,
					APP_WIDGET		printOption,
					const char *		pixmapName )
    {
    EditApplication *			ea= ed->edApplication;
    AppPrintDialog *			apd= (AppPrintDialog *)ea->eaPrintDialog;
    const AppPrintDialogResources *	apdr;

    PrintJob				pj;
    PrintGeometry			pg;

    int					i;
    int					customTransformRelevant= 1;

    appPrintJobForEditDocument( &pj, ed );

    /*  1  */
    if  ( ! apd )
	{
	apd= appMakePrintDialog( ea, ed, printOption, pixmapName );

	if  ( ! apd )
	    { XDEB(apd); return;	}

	ea->eaPrintDialog= (void *)apd;

	/*  2  */

	/*  3  */
	if  ( apd->apdActivateFaxOptions )
	    {
	    const char *	faxNumber= getenv( "APP_FAX_TO" );

	    if  ( faxNumber )
		{
		appStringToTextWidget( apd->apdFaxNumberText, faxNumber );
		apd->apdOptionNrChosen= apd->apdFaxOptionNr;
		}
	    }
	}
    else{
	appSetShellTitle( apd->apdDialog.adTopWidget, printOption,
						    ea->eaApplicationName );

	appGuiShowDialog( &(apd->apdDialog), ed->edToplevel.atTopWidget );
	}

    apdr= apd->apdResources;

    apd->apdPrinterGeometry= ea->eaDefaultDocumentGeometry;
    apd->apdDocumentGeometry= *dgDocument;

    apd->apdPageCount= pageCount;
    apd->apdFirstSelectedPage= firstSelected;
    apd->apdLastSelectedPage= lastSelected;
    apd->apdFirstPageChosen= 0;
    apd->apdLastPageChosen= apd->apdPageCount- 1;
    apd->apdPrintOddFacesOnly= 0;
    apd->apdPrintEvenFacesOnly= 0;
    apd->apdPrintSheetsReverse= 0;

    utilInitPrintGeometry( &pg );
    appApplicationSettingsToPrintGeometry( &pg, ea );
    pg.pgSheetGeometry= apd->apdPrinterGeometry;

    if  ( ea->eaSuggestNup )
	{ (*ea->eaSuggestNup)( &pg, ed->edPrivateData );	}

    for ( i= 0; i < PSnum__COUNT; i++ )
	{
	if  ( apdr->apdrPlacements[i].psGridRows == pg.pgGridRows	&&
	      apdr->apdrPlacements[i].psGridCols == pg.pgGridCols	&&
	      apdr->apdrPlacements[i].psGridHorizontal ==
						pg.pgGridHorizontal	&&
	      apdr->apdrPlacements[i].psScalePagesToFit ==
						pg.pgScalePagesToFit	)
	    { apd->apdPlacementChosen= i; break; }
	}

    appSetOptionmenu( &(apd->apdPlacementOptionmenu),
						apd->apdPlacementChosen );

    apd->apdSelectionChosen= PRINTselAll_PAGES;
    appSetOptionmenu( &(apd->apdSelectionOptionmenu),
						apd->apdSelectionChosen );
    appPrintDialogShowFromTo( apd, 1, apd->apdPageCount, 0 );

    appGuiEnableWidget( apd->apdSelectionOptions[PRINTselCURRENT_PAGE],
			    ( apd->apdFirstSelectedPage >= 0	&&
			      apd->apdLastSelectedPage >= 0	)	);

    appSetOptionmenu( &(apd->apdPrinterOptionmenu),
						apd->apdOptionNrChosen );

    if  ( apd->apdOddFacesOnlyToggle )
	{
	appGuiSetToggleState( apd->apdOddFacesOnlyToggle,
				    apd->apdPrintOddFacesOnly );
	}
    if  ( apd->apdEvenFacesOnlyToggle )
	{
	appGuiSetToggleState( apd->apdEvenFacesOnlyToggle,
				    apd->apdPrintEvenFacesOnly );
	}

    if  ( apd->apdDirectionBackwardToggle )
	{
	appGuiSetToggleState( apd->apdDirectionBackwardToggle,
						apd->apdPrintSheetsReverse );
	}
    if  ( apd->apdDirectionForwardToggle )
	{
	appGuiSetToggleState( apd->apdDirectionForwardToggle,
						! apd->apdPrintSheetsReverse );
	}

    appPrinterAdaptToDestination( apd );

    appPaperChooserAdaptToGeometry( &(apd->apdPaperChooser),
						&(apd->apdPrinterGeometry) );

    if  ( apd->apdCustomTransformRow )
	{ appPrintDialogRefreshCustomTransform( apd );	}

    appGuiRunDialog( &(apd->apdDialog), AQDrespNONE, ea );

    if  ( apd->apdPlacementChosen < 0			||
	  apd->apdPlacementChosen >= PSnum__COUNT	)
	{
	LLDEB(apd->apdPlacementChosen,PSnum__COUNT);
	appGuiHideDialog( &(apd->apdDialog) );
	goto ready;
	}

    if  ( appPrintDialogPrintGeometry( &customTransformRelevant, &pg, apd ) )
	{ appGuiHideDialog( &(apd->apdDialog) ); goto ready; }

    switch( apd->apdDialog.adResponse )
	{
	case AQDrespOK:
	    pg.pgFirstPage= apd->apdFirstPageChosen;
	    pg.pgLastPage= apd->apdLastPageChosen;

	    if  ( apd->apdOptionNrChosen == apd->apdToFileOptionNr )
		{
		appDocPrintToFile( printOption, apd->apdDialog.adTopWidget,
						ed, &pg );
		}
	    else{
		if  ( apd->apdOptionNrChosen == apd->apdFaxOptionNr )
		    {
		    char *	faxNumber;
		    char *	s;

		    s= faxNumber=
			appGetStringFromTextWidget( apd->apdFaxNumberText );

		    while( *s == ' ' )
			{ s++;	}

		    if  ( ! *s )
			{ SDEB(faxNumber);	}
		    else{
			if  ( appFaxDocument( ed, s, &pg ) )
			    { SDEB(faxNumber);	}
			}

		    appFreeStringFromTextWidget( faxNumber );
		    }
		else{
		    int		printer;

		    printer=
			apd->apdOptionNrChosen- apd->apdFirstPrinterOptionNr;

		    if  ( appPrintDocument( printer, &pj, &pg ) )
			{ LDEB(apd->apdOptionNrChosen); }
		    }
		}

	    appGuiHideDialog( &(apd->apdDialog) );
	    goto ready;

	default:
	    LDEB(apd->apdDialog.adResponse);
	    goto error;

	case AQDrespCANCEL:
	error:
	    appGuiHideDialog( &(apd->apdDialog) );
	    goto ready;
	}

  ready:
    utilCleanPrintGeometry( &pg );
    return;
    }
