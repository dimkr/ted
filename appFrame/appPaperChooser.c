/************************************************************************/
/*									*/
/*  Choose a paper format common functionality.				*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	<appGeoString.h>
#   include	<appPaper.h>

#   include	"appPaperChooser.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Update the text widget.						*/
/*									*/
/************************************************************************/

static void appPaperChooserShowWidthHeight(
				PaperChooser *			pc,
				const DocumentGeometry *	dg )
    {
    char			scratch[50];

    appGeoRectangleToString( scratch,
		    dg->dgPageWideTwips, dg->dgPageHighTwips, pc->pcUnitType );

    pc->pcProgrammatic++;
    appStringToTextWidget( pc->pcSizeText, scratch );
    pc->pcProgrammatic--;

    return;
    }

/************************************************************************/
/*									*/
/*  Extract a paper size from the Text widget.				*/
/*									*/
/************************************************************************/

int appPaperChooserGetSize(	PropertyMask *		pChgMask,
				PaperChooser *		pc,
				DocumentGeometry *	dg )
    {
    char *		s;
    DocumentGeometry	dgHere;

    PropertyMask	updMask;
    PropertyMask	chgMask;

    dgHere= *dg;

    PROPmaskCLEAR( &updMask );
    PROPmaskCLEAR( &chgMask );

    PROPmaskADD( &updMask, DGpropPAGE_WIDTH );
    PROPmaskADD( &updMask, DGpropPAGE_HEIGHT );

    s= appGetStringFromTextWidget( pc->pcSizeText );

    if  ( appGeoRectangleFromString( s, pc->pcUnitType,
				    &(dgHere.dgPageWideTwips),
				    &(dgHere.dgPageHighTwips) )		||
	  dgHere.dgPageWideTwips <= 0					||
	  dgHere.dgPageHighTwips <= 0					)
	{
	appFreeStringFromTextWidget( s );

	appRefuseTextValue( pc->pcSizeText );

	return -1;
	}

    appFreeStringFromTextWidget( s );

    utilUpdDocumentGeometry( dg, &dgHere, &chgMask, &updMask );

    *pChgMask= chgMask;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Adapt to geometry.							*/
/*									*/
/*  1)  Show the orientation in the radiogroup.				*/
/*  2)  Derive the values to show in the radiogroup and the size menu	*/
/*	from the rectangle.						*/
/*  3)  Set the orientation buttons and the paper size menu.		*/
/*									*/
/************************************************************************/

/*  1  */
static void appPaperChooserShowOrientation(	PaperChooser *	pc )
    {
    if  ( ! pc->pcPortraitRadio || ! pc->pcLandscapeRadio )
	{ XXDEB(pc->pcPortraitRadio,pc->pcLandscapeRadio); return;	}

    pc->pcProgrammatic++;

    if  ( pc->pcLandscapeChosen )
	{
	appGuiSetToggleState( pc->pcPortraitRadio,  0 );
	appGuiSetToggleState( pc->pcLandscapeRadio, 1 );
	}
    else{
	appGuiSetToggleState( pc->pcPortraitRadio,  1 );
	appGuiSetToggleState( pc->pcLandscapeRadio, 0 );
	}

    pc->pcProgrammatic--;

    return;
    }

/*  2  */
static void appPaperChooserWidgetSettings(
				int *				pLandscape,
				int *				pSizeNr,
				int				custom,
				const DocumentGeometry *	dg )
    {
    int		sizeNr;
    int		landscape;

    if  ( dg->dgPageWideTwips > dg->dgPageHighTwips )
	{
	sizeNr= appPaperGetBySize( dg->dgPageHighTwips, dg->dgPageWideTwips );

	landscape= 1;
	}
    else{
	sizeNr= appPaperGetBySize( dg->dgPageWideTwips, dg->dgPageHighTwips );

	landscape= 0;
	}

    if  ( sizeNr < 0 )
	{ sizeNr= custom;	}

    *pLandscape= landscape;
    *pSizeNr= sizeNr;

    return;
    }

/*  3  */
static void appPaperChooserSetWidgets(	PaperChooser *		pc,
					int			sizeNr,
					int			landscape )
    {
    if  ( pc->pcSizeChosen != sizeNr )
	{
	pc->pcSizeChosen= sizeNr;

	pc->pcProgrammatic++;
	appSetOptionmenu( &(pc->pcOptionmenu), pc->pcSizeChosen );
	pc->pcProgrammatic--;
	}

    if  ( pc->pcLandscapeChosen != landscape	)
	{
	pc->pcLandscapeChosen= landscape;

	if  ( pc->pcPortraitRadio	&&
	      pc->pcLandscapeRadio	)
	    { appPaperChooserShowOrientation( pc );	}
	}

    return;
    }

void appPaperChooserAdaptToGeometry(	PaperChooser *			pc,
					const DocumentGeometry *	dg )
    {
    int		sizeNr;
    int		landscape;

    pc->pcGeometryChosen= *dg;

    appPaperChooserShowWidthHeight( pc, &(pc->pcGeometryChosen) );

    appPaperChooserWidgetSettings( &landscape, &sizeNr,
			    pc->pcCustomPaperSize, &(pc->pcGeometryChosen) );

    appPaperChooserSetWidgets( pc, sizeNr, landscape );

    return;
    }

/************************************************************************/
/*									*/
/*  A paper size was chosen in the menu. As far as the paper chooser	*/
/*  is concerned no more needs to be done.				*/
/*									*/
/************************************************************************/

static APP_OITEM_CALLBACK_H( appPaperChooserSizeChosen, w, voidpc )
    {
    PaperChooser *	pc= (PaperChooser *)voidpc;

    int			changed= 0;
    int			i= -1;

    int			width;
    int			height;

    if  ( pc->pcProgrammatic )
	{ return;	}

    i= appGuiGetOptionmenuItemIndex( &(pc->pcOptionmenu), w );
    if  ( i < 0 || i >= pc->pcSizeOptionCount )
	{ LLDEB(i,pc->pcSizeOptionCount); return;	}

    pc->pcSizeChosen= i;

    if  ( i == pc->pcCustomPaperSize )
	{
	PropertyMask		updMask;
	DocumentGeometry	dgScratch;

	PROPmaskCLEAR( &updMask );

	if  ( appPaperChooserGetSize( &updMask, pc, &dgScratch ) )
	    { LDEB(1); return;	}

	width= dgScratch.dgPageWideTwips;
	height= dgScratch.dgPageHighTwips;

	if  ( width > height )
	    { pc->pcLandscapeChosen= 1;	}
	else{ pc->pcLandscapeChosen= 0;	}
	}
    else{
	if  ( appPaperGetInformation( i, &width, &height, (const char **)0 ) )
	    { return;	}

	if  ( pc->pcLandscapeChosen )
	    {
	    int		swap;

	    swap= height; height= width; width= swap;
	    }
	}

    if  ( pc->pcGeometryChosen.dgPageWideTwips != width		||
	  pc->pcGeometryChosen.dgPageHighTwips != height	)
	{ changed= 1;	}

    pc->pcGeometryChosen.dgPageWideTwips= width;
    pc->pcGeometryChosen.dgPageHighTwips= height;

    appPaperChooserShowWidthHeight( pc, &(pc->pcGeometryChosen) );

    if  ( changed && pc->pcCallback )
	{
	(*pc->pcCallback)( pc, pc->pcCallbackThrough,
						&(pc->pcGeometryChosen) );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make the widgets for a paper chooser.				*/
/*									*/
/************************************************************************/

static APP_TXTYPING_CALLBACK_H( appPaperChooserTypingCallback, w, voidpc )
    {
    PaperChooser *	pc= (PaperChooser *)voidpc;
    char *		s;

    DocumentGeometry	dgHere;

    if  ( pc->pcProgrammatic )
	{ return;	}

    s= appGetStringFromTextWidget( pc->pcSizeText );

    if  ( ! appGeoRectangleFromString( s, pc->pcUnitType,
				    &(dgHere.dgPageWideTwips),
				    &(dgHere.dgPageHighTwips) )		&&
	  dgHere.dgPageWideTwips > 0					&&
	  dgHere.dgPageHighTwips > 0					)
	{
	int		sizeNr;
	int		landscape;

	appPaperChooserWidgetSettings( &landscape, &sizeNr,
					    pc->pcCustomPaperSize, &dgHere );

	appPaperChooserSetWidgets( pc, sizeNr, landscape );
	}

    appFreeStringFromTextWidget( s );

    return;
    }

/************************************************************************/
/*									*/
/*  Callback when the user pushes return in the edit.			*/
/*									*/
/************************************************************************/

static APP_TXACTIVATE_CALLBACK_H( appPaperChooserGotValueCallback, w, voidpc )
    {
    PaperChooser *	pc= (PaperChooser *)voidpc;
    PropertyMask	updMask;

    PROPmaskCLEAR( &updMask );

    if  ( pc->pcProgrammatic )
	{ return;	}

    if  ( appPaperChooserGetSize( &updMask, pc, &(pc->pcGeometryChosen) ) )
	{ LDEB(1); return;	}

    if  ( ! utilPropMaskIsEmpty( &updMask ) )
	{
	appPaperChooserAdaptToGeometry( pc, &(pc->pcGeometryChosen) );

	if  ( pc->pcCallback )
	    {
	    (*pc->pcCallback)( pc, pc->pcCallbackThrough,
						&(pc->pcGeometryChosen) );
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  One of the orientation toggles was activated. As far as the paper	*/
/*  chooser is concerned no more needs to be done.			*/
/*									*/
/*  The contorted logic of this function is dictated by the fact that	*/
/*  GTK invokes callbacks for programmatic changes to the toggle	*/
/*  buttons. This is a curse we have to live with.			*/
/*									*/
/************************************************************************/

static void appPaperChooserOrientationChanged(	PaperChooser *	pc )
    {
    int	swap;

    swap= pc->pcGeometryChosen.dgPageWideTwips;
    pc->pcGeometryChosen.dgPageWideTwips=
				pc->pcGeometryChosen.dgPageHighTwips;
    pc->pcGeometryChosen.dgPageHighTwips= swap;

    appPaperChooserShowWidthHeight( pc, &(pc->pcGeometryChosen) );

    if  ( pc->pcCallback )
	{
	(*pc->pcCallback)( pc, pc->pcCallbackThrough,
						&(pc->pcGeometryChosen) );
	}

    return;
    }

static APP_TOGGLE_CALLBACK_H( appPaperChooserLandscapeToggled, w, voidpc, e )
    {
    PaperChooser *		pc= (PaperChooser *)voidpc;

    int				change= 0;
    int				set;

    if  ( pc->pcProgrammatic )
	{ return;	}

    set= appGuiGetToggleStateFromCallback( w, e );

    if  ( pc->pcLandscapeChosen )
	{
	if  ( ! set )
	    {
	    pc->pcProgrammatic++;
	    appGuiSetToggleState( pc->pcLandscapeRadio, 1 );
	    pc->pcProgrammatic--;
	    }
	}
    else{
	if  ( set )
	    {
	    pc->pcLandscapeChosen= 1;
	    pc->pcProgrammatic++;
	    appGuiSetToggleState( pc->pcPortraitRadio, 0 );
	    pc->pcProgrammatic--;
	    change= 1;
	    }
	}

    if  ( change )
	{ appPaperChooserOrientationChanged( pc );	}

    return;
    }

static APP_TOGGLE_CALLBACK_H( appPaperChooserPortraitToggled, w, voidpc, e )
    {
    PaperChooser *		pc= (PaperChooser *)voidpc;

    int				change= 0;
    int				set;

    if  ( pc->pcProgrammatic )
	{ return;	}

    set= appGuiGetToggleStateFromCallback( w, e );

    if  ( pc->pcLandscapeChosen )
	{
	if  ( set )
	    {
	    pc->pcLandscapeChosen= 0;
	    pc->pcProgrammatic++;
	    appGuiSetToggleState( pc->pcLandscapeRadio, 0 );
	    pc->pcProgrammatic--;
	    change= 1;
	    }
	}
    else{
	if  ( ! set )
	    {
	    pc->pcProgrammatic++;
	    appGuiSetToggleState( pc->pcPortraitRadio, 1 );
	    pc->pcProgrammatic--;
	    }
	}

    if  ( change )
	{ appPaperChooserOrientationChanged( pc );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make the widgets in a paper chooser and install the callback.	*/
/*									*/
/************************************************************************/

void appMakePaperChooserWidgets(	APP_WIDGET		parent,
					const char *		title,
					int			unitType,
					PaperChooser *		pc,
					PaperChooserCallback	callback,
					void *			through )
    {
    const int	textEnabled= 1;

    pc->pcUnitType= unitType;
    pc->pcCallback= callback;
    pc->pcCallbackThrough= through;

    appMakeColumnFrameInColumn( &(pc->pcFrame),
				    &(pc->pcVerticalColumn), parent, title );


    appMakeOptionmenuInColumn( &(pc->pcOptionmenu), pc->pcVerticalColumn );

    appMakeTextInColumn( &(pc->pcSizeText), pc->pcVerticalColumn,
							    0, textEnabled );

    appGuiSetGotValueCallbackForText( pc->pcSizeText,
				appPaperChooserGotValueCallback, (void *)pc );

    appGuiSetTypingCallbackForText( pc->pcSizeText,
				appPaperChooserTypingCallback, (void *)pc );
    
    return;
    }

/************************************************************************/
/*									*/
/*  Add Portrait/Landscape toggles to a paper chooser.			*/
/*									*/
/************************************************************************/

void appPaperChooserAddOrientationToggles( PaperChooser *	pc,
					const char *		portrait,
					const char *		landscape )
    {
    const int	heightResizable= 0;

    pc->pcOrientationRow= appMakeRowInColumn( pc->pcVerticalColumn, 2,
							    heightResizable );

    pc->pcPortraitRadio= appMakeToggleInRow( pc->pcOrientationRow,
		portrait, appPaperChooserPortraitToggled, (void *)pc, 0 );

    pc->pcLandscapeRadio= appMakeToggleInRow( pc->pcOrientationRow,
		landscape, appPaperChooserLandscapeToggled, (void *)pc, 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  Fill the list of paper sizes.					*/
/*									*/
/************************************************************************/

void appPaperChooserFillMenu(	PaperChooser *		pc,
				const char *		customLabel )
    {
    const char *	label;
    int			i;
    int			gotSome= 0;

    if  ( ! pc->pcSizeOptions )
	{
	i= 0;
	while( ! appPaperGetInformation( i, (int *)0, (int *)0, &label ) )
	    { i++; }

	pc->pcCustomPaperSize= i++;
	pc->pcSizeOptionCount= i;

	pc->pcSizeOptions= realloc( pc->pcSizeOptions,
				pc->pcSizeOptionCount* sizeof(APP_WIDGET) );
	if  ( ! pc->pcSizeOptions )
	    { LXDEB(pc->pcSizeOptionCount,pc->pcSizeOptions); return;	}

	for ( i= 0; i < pc->pcSizeOptionCount; i++ )
	    { pc->pcSizeOptions[i]= (APP_WIDGET)0;	}
	}

    appEmptyOptionmenu( &(pc->pcOptionmenu) );

    i= 0;
    while( ! appPaperGetInformation( i, (int *)0, (int *)0, &label ) )
	{
	pc->pcSizeOptions[i]= appAddItemToOptionmenu( &(pc->pcOptionmenu),
			    label, appPaperChooserSizeChosen, (void *)pc );

	if  ( i == 0 )
	    { gotSome= 1;	}

	i++;
	}

    pc->pcSizeOptions[i]= appAddItemToOptionmenu( &(pc->pcOptionmenu),
			customLabel, appPaperChooserSizeChosen, (void *)pc );
    if  ( i == 0 )
	{ gotSome= 1;	}

    if  ( gotSome )
	{
	appSetOptionmenu( &(pc->pcOptionmenu), 0 );
	pc->pcSizeChosen= 0;
	}
    else{
	appSetOptionmenu( &(pc->pcOptionmenu), -1 );
	}

    appOptionmenuRefreshWidth( &(pc->pcOptionmenu) );

    return;
    }

/************************************************************************/
/*									*/
/*  Initialize a fresh PaperChooser.					*/
/*									*/
/************************************************************************/

void appCleanPaperChooser(	PaperChooser *	pc )
    {
    if  ( pc->pcSizeOptions )
	{ free( pc->pcSizeOptions );	}
    }

void appInitPaperChooser(	PaperChooser *	pc )
    {
    pc->pcFrame= (APP_WIDGET)0;
    pc->pcVerticalColumn= (APP_WIDGET)0;

    appInitOptionmenu( &(pc->pcOptionmenu) );
    pc->pcSizeText= (APP_WIDGET)0;

    pc->pcOrientationRow= (APP_WIDGET)0;
    pc->pcPortraitRadio= (APP_WIDGET)0;
    pc->pcLandscapeRadio= (APP_WIDGET)0;

    pc->pcCustomPaperSize= -1;
    pc->pcSizeChosen= -1;
    pc->pcLandscapeChosen= -1;

    utilInitDocumentGeometry( &(pc->pcGeometryChosen) );

    pc->pcSizeOptions= (APP_WIDGET *)0;
    pc->pcSizeOptionCount= 0;

    pc->pcCallback= (PaperChooserCallback)0;
    pc->pcCallbackThrough= (void *)0;

    pc->pcProgrammatic= 0;

    return;
    }
