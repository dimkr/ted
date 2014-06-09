/************************************************************************/
/*									*/
/*  Simple color chooser on an inspector page.				*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>

#   include	<appColorChooser.h>
#   include	"appFrame.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Set the color of a ColorChooser.					*/
/*									*/
/************************************************************************/

void appColorChooserSetColor(	ColorChooser *		cc,
				int			explicit,
				const RGB8Color *	rgb8 )
    {
    int		changed= 0;

    if  ( ! cc->ccColorSet )
	{
	cc->ccColorSet= 1;
	changed= 1;
	}

    if  ( cc->ccColorExplicit )
	{
	if  ( explicit )
	    {
	    if  ( bmRGB8ColorsDiffer( &(cc->ccColorChosen), rgb8 ) )
		{
		cc->ccColorChosen= *rgb8;
		changed= 1;
		}
	    }
	else{
	    cc->ccColorExplicit= 0;
	    changed= 1;
	    }
	}
    else{
	if  ( explicit )
	    {
	    cc->ccColorExplicit= 1;
	    cc->ccColorChosen= *rgb8;
	    changed= 1;
	    }
	}

    if  ( changed )
	{
	cc->ccInplaceColorAllocated= 0;

	appExposeRectangle( &(cc->ccInplaceDrawingData), 0, 0, 0, 0 );
	}

    return;
    }

void appColorChooserUnset(	ColorChooser *	cc )
    {
    if  ( cc->ccColorSet )
	{
	cc->ccColorSet= 0;

	appExposeRectangle( &(cc->ccInplaceDrawingData), 0, 0, 0, 0 );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Redraw the 'Inplace' part of a Color Chooser.			*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( appColorChooserRedrawInplace, w, voidcc, exposeEvent )
    {
    ColorChooser *	cc= (ColorChooser *)voidcc;
    AppDrawingData *	add= &(cc->ccInplaceDrawingData);

    int			wide;
    int			high;

    DocumentRectangle	drI;

    appDrawnPulldownDrawArrow( &wide, &high, w, add );

    drI.drX0= 3; drI.drX1= wide- 3;
    drI.drY0= 3; drI.drY1= high- 3;

    if  ( ! cc->ccColorSet )
	{
	appDrawSetForegroundWhite( add );
	appDrawFillRectangle( add, drI.drX0, drI.drY0,
			    drI.drX1- drI.drX0+ 1, drI.drY1- drI.drY0+ 1 );

	appDrawSetForegroundBlack( add );

	appDrawDrawRectangle( add, drI.drX0, drI.drY0,
			    drI.drX1- drI.drX0+ 1, drI.drY1- drI.drY0+ 1 );

	appDrawDrawLine( add, drI.drX0, drI.drY0, drI.drX1, drI.drY1 );
	appDrawDrawLine( add, drI.drX0, drI.drY1, drI.drX1, drI.drY0 );

	return;
	}

    if  ( ! cc->ccColorExplicit )
	{
	const ColorChooserResources *	ccr= cc->ccResources;
	const int			row= 0;

	appDrawSetFont( add, cc->ccTextFont );

	appDrawSetForegroundWhite( add );
	appDrawFillRectangle( add, drI.drX0, drI.drY0,
			    drI.drX1- drI.drX0+ 1, drI.drY1- drI.drY0+ 1 );

	appDrawSetForegroundBlack( add );

	appDrawDrawString( add, 
		    cc->ccXShift+ 4, ( row+ 1 )* cc->ccStripHigh- 7,
		    ccr->ccrAutomaticColor, strlen( ccr->ccrAutomaticColor ) );

	return;
	}

    if  ( ! cc->ccInplaceColorAllocated )
	{
	if  ( appColorRgb( &(cc->ccInplaceColor), &(cc->ccInplaceColors),
						cc->ccColorChosen.rgb8Red,
						cc->ccColorChosen.rgb8Green,
						cc->ccColorChosen.rgb8Blue ) )
	    { LDEB(1); return;	}

	cc->ccInplaceColorAllocated= 1;
	}

    /**/
    appDrawSetForegroundColor( add, &(cc->ccInplaceColor) );

    appDrawFillRectangle( add, drI.drX0, drI.drY0,
			    drI.drX1- drI.drX0+ 1, drI.drY1- drI.drY0+ 1 );

    /**/
    appDrawSetForegroundBlack( add );

    appDrawDrawRectangle( add, drI.drX0, drI.drY0,
			    drI.drX1- drI.drX0+ 1, drI.drY1- drI.drY0+ 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  Redraw the 'Pulldown' part of a Color Chooser.			*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( appColorChooserRedrawPulldown, w, voidcc, exposeEvent )
    {
    ColorChooser *	cc= (ColorChooser *)voidcc;
    AppDrawingData *	add= &(cc->ccPulldownDrawingData);

    int			inplaceWide;
    int			inplaceHigh;
    int			pulldownWide;
    int			pulldownHigh;

    const int		ox= 0;
    const int		oy= 0;
    DocumentRectangle	drClip;

    int			row;
    int			col;

    int			currCol= -1;
    int			currRow= -1;

    if  ( ! cc->ccPulldownDrawingDataSet )
	{
	appSetDrawingDataForWidget( cc->ccPulldown.adpPulldownDrawing,
					1.0, &(cc->ccPulldownDrawingData) );

	if  ( appAllocateColors( add, &(cc->ccPulldownColors) ) )
	    { LDEB(1);	}

	cc->ccPulldownDrawingDataSet= 1;

	appColorRgb( &(cc->ccPulldownBackgroundColor),
				    &(cc->ccPulldownColors), 128, 128,128 );
	}

    appDrawGetSizeOfWidget( &pulldownWide, &pulldownHigh, w );
    appDrawGetSizeOfWidget( &inplaceWide, &inplaceHigh,
					cc->ccPulldown.adpInplaceDrawing );

    appCollectExposures( &drClip, add, ox, oy, exposeEvent );

    appDrawSetForegroundColor( add, &(cc->ccPulldownBackgroundColor) );

    appDrawFillRectangle( add, drClip.drX0, drClip.drY0,
					    drClip.drX1- drClip.drX0+ 1,
					    drClip.drY1- drClip.drY0+ 1 );

    appDrawSetForegroundBlack( add );
    appDrawDrawRectangle( add, 0, 0, pulldownWide- 1, pulldownHigh- 1 );

    /**/
    if  ( ! cc->ccTextFont )
	{ XDEB(cc->ccTextFont);	}
    else{
	const ColorChooserResources *	ccr= cc->ccResources;

	appDrawSetFont( add, cc->ccTextFont );

	row= 0;
	appDrawDrawString( add, 
		    cc->ccXShift+ 4, ( row+ 1 )* cc->ccStripHigh- 5,
		    ccr->ccrAutomaticColor, strlen( ccr->ccrAutomaticColor ) );

	row= cc->ccStrips- 1;
	appDrawDrawString( add, 
		    cc->ccXShift+ 4, ( row+ 1 )* cc->ccStripHigh- 8,
		    ccr->ccrMoreColors, strlen( ccr->ccrMoreColors ) );
	}

    for ( row= 1; row < cc->ccStrips- 1; row++ )
	{
	for ( col= 0; col < cc->ccColumns; col++ )
	    {
	    int				color;
	    ColorChooserPaletteColor *	ccpc;
	    const RGB8Color *		rgb8;

	    color= ( row- 1 )* cc->ccColumns+ col;
	    if  ( color < 0 || color >= cc->ccColorCount )
		{ LLDEB(color,cc->ccColorCount); continue;	}

	    ccpc= cc->ccColors+ color;
	    if  ( ccpc->ccpcStatus == CCstatusFREE )
		{ /*LDEB(ccpc->ccpcStatus);*/ continue;	}
	    rgb8= &(ccpc->ccpcRGB8Color);

	    if  ( ! ccpc->ccpcColorAllocated )
		{
		if  ( appColorRgb( &(ccpc->ccpcAllocatedColor),
					    &(cc->ccPulldownColors),
					    rgb8->rgb8Red,
					    rgb8->rgb8Green,
					    rgb8->rgb8Blue ) )
		    { LDEB(1); continue;	}

		ccpc->ccpcColorAllocated= 1;
		}

	    appDrawSetForegroundColor( add, &(ccpc->ccpcAllocatedColor) );

	    appDrawFillRectangle( add,
				    cc->ccXShift+ col* cc->ccColumnWide+ 3,
				    row* cc->ccStripHigh+ 3,
				    cc->ccColumnWide- 6,
				    cc->ccStripHigh- 6 );

	    if  ( cc->ccColorExplicit					&&
		  cc->ccColorSet					&&
		  rgb8->rgb8Red == cc->ccColorChosen.rgb8Red		&&
		  rgb8->rgb8Green == cc->ccColorChosen.rgb8Green	&&
		  rgb8->rgb8Blue == cc->ccColorChosen.rgb8Blue		)
		{ currCol= col; currRow= row;	}
	    }
	}

    /**/
    appDrawSetForegroundBlack( add );

    appDrawSetLineAttributes( add, 1,
				LINEstyleSOLID, LINEcapBUTT, LINEjoinMITER, 
				(const unsigned char *)0, 0 );

    for ( row= 1; row < cc->ccStrips- 1; row++ )
	{
	for ( col= 0; col < cc->ccColumns; col++ )
	    {
	    appDrawDrawRectangle( add,
				    cc->ccXShift+ col* cc->ccColumnWide+ 3,
				    row* cc->ccStripHigh+ 3,
				    cc->ccColumnWide- 6,
				    cc->ccStripHigh- 6 );
	    }
	}

    if  ( currRow >= 0 && currCol >= 0 )
	{
	appDrawSetLineAttributes( add, 3,
				LINEstyleSOLID, LINEcapBUTT, LINEjoinMITER, 
				(const unsigned char *)0, 0 );

	appDrawDrawRectangle( add, cc->ccXShift+ currCol* cc->ccColumnWide+ 3,
				    currRow* cc->ccStripHigh+ 3,
				    cc->ccColumnWide- 6,
				    cc->ccStripHigh- 6 );

	appDrawSetForegroundWhite( add );

	appDrawSetLineAttributes( add, 1,
				LINEstyleSOLID, LINEcapBUTT, LINEjoinMITER, 
				(const unsigned char *)0, 0 );

	appDrawDrawRectangle( add, cc->ccXShift+ currCol* cc->ccColumnWide+ 2,
				    currRow* cc->ccStripHigh+ 2,
				    cc->ccColumnWide- 4,
				    cc->ccStripHigh- 4 );

	}

    return;
    }

/************************************************************************/
/*									*/
/*  The user clicked in the ColorChooser puldown.			*/
/*									*/
/*  1)  Determine strip.						*/
/*  2)  Special cases..							*/
/*  3)  Determine color.						*/
/*  4)  Remember color							*/
/*  5)  Provoke a redraw of the image					*/
/*  6)  Tell target about the change.					*/
/*									*/
/************************************************************************/

static APP_EVENT_HANDLER_H( appColorChooserClickedPulldown, w, voidcc, mouseEvent )
    {
    ColorChooser *		cc= (ColorChooser *)voidcc;

    int				row;
    int				col;
    int				color;

    int				mouseX;
    int				mouseY;

    int				button;
    int				upDown;
    int				seq;
    unsigned int		keyState= 0;

    ColorChooserPaletteColor *	ccpc;

    /*  1  */
    if  ( appGuiDrawnPulldownGetStrip( &row,
					&(cc->ccPulldown), w, mouseEvent ) )
	{ return;	}

    /*  2  */
    if  ( row == 0 )
	{
	if  ( cc->ccColorExplicit )
	    {
	    cc->ccColorExplicit= 0;
	    appExposeRectangle( &(cc->ccInplaceDrawingData), 0, 0, 0, 0 );
	    }

	if  ( cc->ccCallback )
	    {
	    (*cc->ccCallback)( cc, cc->ccWhich, cc->ccTarget,
				    CHOICEccDEFAULT, (const RGB8Color *)0 );
	    }

	return;
	}

    if  ( row == cc->ccStrips- 1 )
	{
	if  ( cc->ccCallback )
	    {
	    (*cc->ccCallback)( cc, cc->ccWhich, cc->ccTarget,
				    CHOICEccMORE, (const RGB8Color *)0 );
	    }

	return;
	}

    if  ( appGetCoordinatesFromMouseButtonEvent( &mouseX, &mouseY,
					    &button, &upDown, &seq, &keyState,
					    w, mouseEvent ) )
	{ return;	}

    col= mouseX/ cc->ccColumnWide;
    if  ( col < 0 )
	{ col= 0;	}
    if  ( col >= cc->ccColumns )
	{ col= cc->ccColumns- 1;	}

    color= ( row- 1 )* cc->ccColumns+ col;
    if  ( color < 0 || color >= cc->ccColorCount )
	{ LLDEB(color,cc->ccColorCount); return;	}

    ccpc= cc->ccColors+ color;
    if  ( ccpc->ccpcStatus == CCstatusFREE )
	{ LDEB(ccpc->ccpcStatus); return;	}

    if  ( cc->ccCallback )
	{
	const int	explicit= 1;

	appColorChooserSetColor( cc, explicit, &(ccpc->ccpcRGB8Color) );

	(*cc->ccCallback)( cc, cc->ccWhich, cc->ccTarget,
				CHOICEccCOLOR, &(ccpc->ccpcRGB8Color) );
	}

    return;
    }


/************************************************************************/
/*									*/
/*  Suggest a palette for the color chooser.				*/
/*									*/
/************************************************************************/

static int appColorChooserFindColor(	const ColorChooser *	cc,
					const RGB8Color *	rgb8 )
    {
    int					i;
    const ColorChooserPaletteColor *	ccpc;

    ccpc= cc->ccColors;
    for ( i= 0; i < cc->ccColorCount; ccpc++, i++ )
	{
	if  ( ccpc->ccpcStatus == CCstatusFREE )
	    { continue;	}

	if  ( bmRGB8ColorsDiffer( &(ccpc->ccpcRGB8Color), rgb8 ) )
	    { continue;	}

	return i;
	}

    return -1;
    }

static int appColorChooserAllocateColor(	const ColorChooser *	cc,
						const RGB8Color *	rgb8,
						int			status )
    {
    int					i;
    ColorChooserPaletteColor *		ccpc;

    ccpc= cc->ccColors;
    for ( i= 0; i < cc->ccColorCount; ccpc++, i++ )
	{
	if  ( ccpc->ccpcStatus != CCstatusFREE )
	    { continue;	}

	ccpc->ccpcRGB8Color= *rgb8;
	ccpc->ccpcStatus= status;
	ccpc->ccpcColorAllocated= 0;

	return i;
	}

    return -1;
    }

static int appColorChooserFindNearest(	const RGB8Color *	colors,
					int			colorCount,
					int			dist,
					const RGB8Color *	rgb8 )
    {
    int					i;
    int					found= -1;
    int					d= 256;

    for ( i= 0; i < colorCount; colors++, i++ )
	{
	int	dd= 0;
	int	dc;

	dc= rgb8->rgb8Red- colors->rgb8Red;
	if  ( dc < 0 )
	    { dc= -dc;	}
	if  ( dd < dc )
	    { dd=  dc; }
	if  ( dd > dist || dd > d )
	    { continue;	}

	dc= rgb8->rgb8Green- colors->rgb8Green;
	if  ( dc < 0 )
	    { dc= -dc;	}
	if  ( dd < dc )
	    { dd=  dc; }
	if  ( dd > dist || dd > d )
	    { continue;	}

	dc= rgb8->rgb8Blue- colors->rgb8Blue;
	if  ( dc < 0 )
	    { dc= -dc;	}
	if  ( dd < dc )
	    { dd=  dc; }
	if  ( dd > dist || dd > d )
	    { continue;	}

	d= dd;
	found= i;

	if  ( d == 0 )
	    { break;	}
	}

    return found;
    }

/************************************************************************/
/*									*/
/*  Try to allocate a color for the palette.				*/
/*									*/
/************************************************************************/

static int appColorChooserTryColor(	ColorChooser *		cc,
					const RGB8Color *	colors,
					int			colorCount,
					int			dist,
					int			r,
					int			g,
					int			b )
    {
    int			found;
    int			status;
    RGB8Color		rgb8;

    rgb8.rgb8Red= r;
    rgb8.rgb8Green= g;
    rgb8.rgb8Blue= b;
    rgb8.rgb8Alpha= 255;

    status= CCstatusSYSTEM;

    found= appColorChooserFindNearest( colors, colorCount, dist, &rgb8 );
    if  ( found >= 0 && bmRGB8ColorsDiffer( &rgb8, &(colors[found]) ) )
	{
	rgb8= colors[found];
	status= CCstatusPALETTE;
	}

    if  ( appColorChooserFindColor( cc, &rgb8 ) >= 0 )
	{ return 0;	}

    if  ( appColorChooserAllocateColor( cc, &rgb8, status ) < 0 )
	{ return 1;	}

    return 0;
    }

static int appColorChooserCompareIntensity(	const void *	vccpc1,
						const void *	vccpc2 )
    {
    const ColorChooserPaletteColor *	ccpc1;
    const ColorChooserPaletteColor *	ccpc2;
    
    const RGB8Color *			rgb1;
    const RGB8Color *			rgb2;
    
    int					i1;
    int					i2;

    ccpc1= (const ColorChooserPaletteColor *)vccpc1;
    ccpc2= (const ColorChooserPaletteColor *)vccpc2;

    if  ( ccpc1->ccpcStatus == CCstatusFREE	&&
	  ccpc2->ccpcStatus == CCstatusFREE	)
	{ return  0;	}
    if  ( ccpc1->ccpcStatus != CCstatusFREE	&&
	  ccpc2->ccpcStatus == CCstatusFREE	)
	{ return -1;	}
    if  ( ccpc1->ccpcStatus == CCstatusFREE	&&
	  ccpc2->ccpcStatus != CCstatusFREE	)
	{ return  1;	}

    rgb1= &(ccpc1->ccpcRGB8Color);
    rgb2= &(ccpc2->ccpcRGB8Color);

    i1= bmRgbIntensity( rgb1 );
    i2= bmRgbIntensity( rgb2 );

    if  ( i1 > i2 )
	{ return  1;	}
    if  ( i1 < i2 )
	{ return -1;	}

    return 0;
    }

static int appColorChooserCompareHue(		const void *	vccpc1,
						const void *	vccpc2 )
    {
    const ColorChooserPaletteColor *	ccpc1;
    const ColorChooserPaletteColor *	ccpc2;

    const RGB8Color *			rgb1;
    const RGB8Color *			rgb2;
    
    int					h1;
    int					h2;

    ccpc1= (const ColorChooserPaletteColor *)vccpc1;
    ccpc2= (const ColorChooserPaletteColor *)vccpc2;

    if  ( ccpc1->ccpcStatus == CCstatusFREE	&&
	  ccpc2->ccpcStatus == CCstatusFREE	)
	{ return  0;	}
    if  ( ccpc1->ccpcStatus != CCstatusFREE	&&
	  ccpc2->ccpcStatus == CCstatusFREE	)
	{ return -1;	}
    if  ( ccpc1->ccpcStatus == CCstatusFREE	&&
	  ccpc2->ccpcStatus != CCstatusFREE	)
	{ return  1;	}

    rgb1= &(ccpc1->ccpcRGB8Color);
    rgb2= &(ccpc2->ccpcRGB8Color);

    if  ( rgb1->rgb8Red == rgb1->rgb8Green	&&
	  rgb1->rgb8Red == rgb1->rgb8Blue	)
	{
	if  ( bmRgbIntensity( rgb1 ) >= 128 )
	    { h1= 255;	}
	else{ h1= 0;	}
	}
    else{ h1= bmRgbHue( rgb1 ); }

    if  ( rgb2->rgb8Red == rgb2->rgb8Green	&&
	  rgb2->rgb8Red == rgb2->rgb8Blue	)
	{
	if  ( bmRgbIntensity( rgb2 ) >= 128 )
	    { h2= 255;	}
	else{ h2= 0;	}
	}
    else{ h2= bmRgbHue( rgb2 ); }

    if  ( h1 > h2 )
	{ return  1;	}
    if  ( h1 < h2 )
	{ return -1;	}

    return appColorChooserCompareIntensity( vccpc1, vccpc2 );
    }

static int appColorChooseMoreColors(	ColorChooser *		cc,
					const RGB8Color *	colors,
					int			colorCount,
					int *			rs,
					int			rc,
					int *			gs,
					int			gc,
					int *			bs,
					int			bc )
    {
    int	r;
    int	g;
    int	b;

    int	d= 30;

    for ( b= 0; b < bc; b++ )
	{
	for ( r= 0; r < rc; r++ )
	    {
	    for ( g= 0; g < gc; g++ )
		{
		if  ( appColorChooserTryColor( cc, colors, colorCount,
						    d, rs[r], gs[g], bs[b] ) )
		    { return 1;	}
		}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Suggest a palette for the color chooser.				*/
/*									*/
/*  1)  Remove colors that came from the previous palette.		*/
/*  2)  Make sure that we have at least some grays in the palette.	*/
/*  3)  Try to get the corners of the color cube.			*/
/*									*/
/************************************************************************/

void appColorChooserSuggestPalette(	ColorChooser *		cc,
					int			avoidZero,
					const RGB8Color *	colors,
					int			colorCount )
    {
    ColorChooserPaletteColor *	ccpc;

    int				useColors;
    int				i;

    const int			dist= 256/ 12;

    if  ( avoidZero && colorCount > 0 )
	{ colors++; colorCount--; }

    useColors= cc->ccColumns* ( cc->ccStrips- 2 );

    if  ( cc->ccColorCount < useColors )
	{
	ccpc= (ColorChooserPaletteColor *)realloc( cc->ccColors,
			    useColors* sizeof( ColorChooserPaletteColor ) );

	if  ( ! ccpc )
	    { LXDEB(useColors,ccpc); return;	}

	cc->ccColors= ccpc;
	ccpc += cc->ccColorCount;

	while( cc->ccColorCount < useColors )
	    {
	    ccpc->ccpcStatus= CCstatusFREE;
	    ccpc++; cc->ccColorCount++;
	    }
	}

    /*  1  */
    ccpc= cc->ccColors;
    for ( i= 0; i < cc->ccColorCount; ccpc++, i++ )
	{
	if  ( ccpc->ccpcStatus == CCstatusPALETTE )
	    { ccpc->ccpcStatus= CCstatusFREE;	}
	}

    /*  3  */
    if  ( appColorChooserTryColor( cc, colors, colorCount, dist, 0,0,0 ) )
	{ goto ready;	}
    if  ( appColorChooserTryColor( cc, colors, colorCount, dist, 255,255,255 ) )
	{ goto ready;	}
    if  ( appColorChooserTryColor( cc, colors, colorCount, dist, 255,0,0 ) )
	{ goto ready;	}
    if  ( appColorChooserTryColor( cc, colors, colorCount, dist, 0,255,0 ) )
	{ goto ready;	}
    if  ( appColorChooserTryColor( cc, colors, colorCount, dist, 0,0,255 ) )
	{ goto ready;	}
    if  ( appColorChooserTryColor( cc, colors, colorCount, dist, 0,255,255 ) )
	{ goto ready;	}
    if  ( appColorChooserTryColor( cc, colors, colorCount, dist, 255,0,255 ) )
	{ goto ready;	}
    if  ( appColorChooserTryColor( cc, colors, colorCount, dist, 255,255,0 ) )
	{ goto ready;	}

    /*  2  */
    for ( i= 0; i < 6; i++ )
	{
	int	c= ( 255* i )/ 5;

	if  ( appColorChooserTryColor( cc, colors, colorCount,
							dist, c, c, c ) )
	    { goto ready;	}
	}

    /*  4  */
    {
    int	bb0[]= { 0, 255 };
    int	bb1[]= { 0, 153, 255 };

    int	gg0[]= { 0, 153, 255 };
    int	gg1[]= { 0, 102, 204, 255 };

    int	rr0[]= { 0, 153, 255 };
    int	rr1[]= { 0, 102, 204, 255 };

    int	all[]= { 0, 53, 102, 153, 204, 255 };

    if  ( appColorChooseMoreColors( cc, colors, colorCount,
					    rr0, sizeof(rr0)/ sizeof(int),
					    gg0, sizeof(gg0)/ sizeof(int),
					    bb0, sizeof(bb0)/ sizeof(int) ) )
	{ goto ready;	}

    if  ( appColorChooseMoreColors( cc, colors, colorCount,
					    rr1, sizeof(rr1)/ sizeof(int),
					    gg1, sizeof(gg1)/ sizeof(int),
					    bb1, sizeof(bb1)/ sizeof(int) ) )
	{ goto ready;	}

    if  ( appColorChooseMoreColors( cc, colors, colorCount,
					    all, sizeof(all)/ sizeof(int),
					    all, sizeof(all)/ sizeof(int),
					    all, sizeof(all)/ sizeof(int) ) )
	{ goto ready;	}
    }

  ready:
    qsort( cc->ccColors, cc->ccColorCount, 
				    sizeof(ColorChooserPaletteColor),
				    appColorChooserCompareIntensity );

    for ( i= 0; i < cc->ccColorCount; i += cc->ccColumns )
	{
	qsort( cc->ccColors+ i, cc->ccColumns, 
				    sizeof(ColorChooserPaletteColor),
				    appColorChooserCompareHue );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make a ColorChooser.						*/
/*									*/
/************************************************************************/

void appCleanColorChooser(		ColorChooser *		cc )
    {
    appCleanDrawingData( &(cc->ccInplaceDrawingData) );
    appCleanDrawingData( &(cc->ccPulldownDrawingData) );

    if  ( cc->ccColors )
	{ free( cc->ccColors );	}

    return;
    }

void appFinishColorChooser(	ColorChooser *		cc,
				APP_FONT *		textFont )
    {
    AppDrawingData *	add= &(cc->ccInplaceDrawingData);

    int			wide;
    int			strips;

    appSetDrawingDataForWidget( cc->ccPulldown.adpInplaceDrawing, 1.0, add );

    if  ( appAllocateColors( add, &(cc->ccInplaceColors) ) )
	{ LDEB(1);	}

    appDrawGetSizeOfWidget( &wide, &(cc->ccStripHigh),
					    cc->ccPulldown.adpInplaceDrawing );

    cc->ccColumnWide= cc->ccStripHigh;
    strips= wide/ cc->ccStripHigh;
    cc->ccStrips= strips+ 2;
    cc->ccColumns= strips;

    cc->ccXShift= ( wide- cc->ccColumns* cc->ccColumnWide )/ 2;

    appGuiSetDrawnPulldownStrips( &(cc->ccPulldown), cc->ccStrips );

    /**/
    cc->ccTextFont= textFont;

    return;
    }

void appInitColorChooser(	ColorChooser *			cc )
    {
    cc->ccFilled= 0;;

    cc->ccResources= (const ColorChooserResources *)0;

    appInitDrawnPulldown( &(cc->ccPulldown) );

    appInitDrawingData( &(cc->ccInplaceDrawingData) );
    appInitDrawingData( &(cc->ccPulldownDrawingData) );
    cc->ccPulldownDrawingDataSet= 0;

    appInitColors( &(cc->ccInplaceColors) );
    appInitColors( &(cc->ccPulldownColors) );

    cc->ccInplaceColorAllocated= 0;
    cc->ccTextFont= (APP_FONT *)0;

    bmInitRGB8Color( &(cc->ccColorChosen) );
    cc->ccColorExplicit= 0;
    cc->ccColorSet= 0;

    cc->ccCallback= (ColorChooserCallback)0;
    cc->ccTarget= (void *)0;
    cc->ccWhich= -1;

    cc->ccStripHigh= 0;
    cc->ccColumnWide= 0;
    cc->ccStrips= 0;
    cc->ccColumns= 0;
    cc->ccXShift= 0;

    cc->ccColors= (ColorChooserPaletteColor *)0;
    cc->ccColorCount= 0;
    }

void appMakeColorChooserInRow(	ColorChooser *			cc,
				APP_WIDGET			row,
				int				col,
				const ColorChooserResources *	ccr,
				ColorChooserCallback		callback,
				int				which,
				void *				through )
    {
    const int	valueColumn= 1;
    const int	valueColspan= 1;

    /**/
    cc->ccResources= ccr;

    cc->ccCallback= callback;
    cc->ccWhich= which;
    cc->ccTarget= through;

    appMakeDrawnPulldownInRow( &(cc->ccPulldown),
					    appColorChooserRedrawInplace,
					    appColorChooserRedrawPulldown,
					    appColorChooserClickedPulldown,
					    row, valueColumn, valueColspan,
					    (void *)cc );

    /**/
    cc->ccFilled= 1;

    return;
    }

/************************************************************************/
/*									*/
/*  Make a row consisting of a label and a color chooser.		*/
/*									*/
/************************************************************************/

void appMakeLabelAndColorChooserRow(
				APP_WIDGET *			pRow,
				APP_WIDGET *			pLabel,
				ColorChooser *			cc,
				APP_WIDGET			column,
				const char *			labelText,
				const ColorChooserResources *	ccr,
				ColorChooserCallback		callback,
				int				which,
				void *				through )
    {
    APP_WIDGET		label;
    APP_WIDGET		row;

    const int		labelColumn= 0;
    const int		labelColspan= 1;
    const int		ccColumn= 1;

    const int		columnCount= 2;
    const int		heightResizable= 0;

    row= appMakeRowInColumn( column, columnCount, heightResizable );

    appMakeLabelInRow( &label, row, labelColumn, labelColspan, labelText );
    appMakeColorChooserInRow( cc, row, ccColumn,
					    ccr, callback, which, through );

    *pRow= row; *pLabel= label; return;
    }

/************************************************************************/
/*									*/
/*  Utility Routine: Update a color and a changed mask from a callback	*/
/*  of a color chooser.							*/
/*									*/
/************************************************************************/

void appColorChooserColorChosen(	PropertyMask *		isSetMask,
					int *			pChanged,
					RGB8Color *		rgb8To,
					int *			pExplicit,
					const RGB8Color *	rgb8Set,
					int			explicit,
					int			which )
    {
    if  ( explicit )
	{
	if  ( ! *pExplicit )
	    {
	    PROPmaskADD( isSetMask, which );
	    *pChanged= 1;

	    *pExplicit= 1;
	    *rgb8To= *rgb8Set;

	    return;
	    }
	else{
	    if  ( bmRGB8ColorsDiffer( rgb8To, rgb8Set ) )
		{
		PROPmaskADD( isSetMask, which );
		*pChanged= 1;

		*rgb8To= *rgb8Set;
		}
	    }
	}
    else{
	if  ( *pExplicit )
	    {
	    PROPmaskADD( isSetMask, which );
	    *pChanged= 1;

	    *pExplicit= 0;
	    }
	}

    return;
    }

