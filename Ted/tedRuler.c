#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"tedRuler.h"
#   include	<appFrame.h>

#   include	<appDebugon.h>

#   define	BACK_MARG(s)	((s)/3)
#   define	WHITE_TOP(s)	BACK_MARG(s)
#   define	WHITE_BOTTOM(s)	((s)-BACK_MARG(s))
#   define	WHITE_HEIGHT(s)	(WHITE_BOTTOM(s)-WHITE_TOP(s))

#   define	TAG_DIST(s)	1
#   define	TAG_TOP(s)	(WHITE_TOP(s)+TAG_DIST(s))
#   define	TAG_BOTTOM(s)	(WHITE_BOTTOM(s)-TAG_DIST(s))
#   define	TAG_HEIGHT(s)	(TAG_BOTTOM(s)-TAG_TOP(s))

#   define	FONT_HEIGHT(s)	(WHITE_HEIGHT(s)-2)
#   define	FONT_BOTTOM(s)	(TAG_BOTTOM(s)-1)

#   define	TICK_DIST(s)	((s)/12)
#   define	TICK_TOP(s)	(WHITE_TOP(s)+TICK_DIST(s))
#   define	TICK_BOTTOM(s)	(WHITE_BOTTOM(s)-TICK_DIST(s))
#   define	TICK_HEIGHT(s)	(TICK_BOTTOM(s)-TICK_TOP(s))

#   define	BUTTON_MARG		1

#   define	BUTTON_DIST(s)		(((s)/6)-1)
#   define	BUTTON_RECTH(s)		((s)/6)
#   define	BUTTON_TRIH(s)		((s)/6)
#   define	BUTTON_HWIDTH(s)	((s)/6)


#   define	HBUTTON_BASE(s)		BUTTON_DIST(s)
#   define	HBUTTON_APEX(s)		(HBUTTON_BASE(s)+ \
					    BUTTON_RECTH(s)+BUTTON_TRIH(s))

#   define	SBUTTON_BASE(s)		((s)-BUTTON_DIST(s))
#   define	SBUTTON_APEX(s)		(SBUTTON_BASE(s)- \
					    BUTTON_RECTH(s)-BUTTON_TRIH(s))

#   define	BUTTON_LEFT(v,s)	((v)-BUTTON_HWIDTH((s))-BUTTON_MARG)
#   define	BUTTON_RIGHT(v,s)	((v)+BUTTON_HWIDTH((s))+BUTTON_MARG)

#   define	TAB_HEIGHT(s)		((s)/6)
#   define	TAB_TOP(s)		(WHITE_BOTTOM(s)-TAB_HEIGHT(s))
#   define	TAB_WIDTH(s)		((s)/6)
#   define	TAB_THICK(s)		3

typedef struct TedTopRuler
    {
    RulerData		ttrRulerData;
    double		ttrMagnification;
					/********************************/
					/*  Describe an area that is	*/
					/*  not to be used.		*/
					/********************************/
    int			ttrColX0;
    int			ttrColX1;
					/********************************/
					/*  Describe the area that is	*/
					/*  accessible as values.	*/
					/********************************/

    int			ttrFirstIndent;		/*  pixels		*/
    int			ttrLeftIndent;		/*  pixels		*/
    int			ttrRightIndent;		/*  pixels		*/

    TabStopList		ttrTabStopList;
    int			ttrTabKind;		/************************/
						/*  alignment for fresh	*/
						/*  tabs.		*/
						/************************/
    int			ttrColumnCount;
    ColumnSeparator *	ttrColumns;
    } TedTopRuler;

typedef struct TopRulerDrag
    {
    int			trdValue;
    int *		trdV0Pointer;
    int *		trdV1Pointer;
    int *		trdYPointer;
    int *		trdChangedPointer;
    TedTopRuler *	trdTtr;
    APP_WIDGET		trdWidget;
    int			trdOc;
    int			trdX0;
    int			trdX1;
    void *		trdThrough;
    DragHair		trdDragHair;
    } TopRulerDrag;

void * tedMakeTopRuler(	int			height,
			double			magnifiedPixelsPerTwip,
			double			magnification,

			int			leftRulerWidth,
			int			rightRulerWidth,

			int			documentC0,
			int			documentC1,
			int			visibleC0,
			int			visibleC1,
			int			rulerC1,

			const char *		fontName,
			int			whatUnit )
    {
    TedTopRuler *		ttr;
    RulerData *			rd;

    ttr= (TedTopRuler *)malloc( sizeof( TedTopRuler ) );
    if  ( ! ttr )
	{ XDEB(ttr); return (void *)0;	}
    rd= &(ttr->ttrRulerData);

    appInitRulerData( &(ttr->ttrRulerData) );

    rd->rdUnit= whatUnit;
    rd->rdFontName= fontName;

    appRulerCalculateIncrements( rd, magnifiedPixelsPerTwip, magnification );

    rd->rdMinUsed= leftRulerWidth;
    rd->rdMaxUsed= 0;
    rd->rdExtraAfterMaxUsed= rightRulerWidth;

    rd->rdDocumentC0= documentC0;
    rd->rdDocumentC1= documentC1;
    rd->rdVisibleC0= visibleC0;
    rd->rdVisibleC1= visibleC1;
    rd->rdRulerC1= rulerC1;

    ttr->ttrMagnification= magnification;

    ttr->ttrColX0= documentC0;
    ttr->ttrColX1= documentC1;

    rd->rdSizeAcross= height;
    rd->rdSizeAlong= 0;

    docInitTabStopList( &(ttr->ttrTabStopList) );
    ttr->ttrTabKind= DOCtaLEFT;

    ttr->ttrColumnCount= 0;
    ttr->ttrColumns= (ColumnSeparator *)0;

    ttr->ttrFirstIndent= -1;
    ttr->ttrLeftIndent= -1;
    ttr->ttrRightIndent= -1;

    return (void *)ttr;
    }

void tedFreeHorizontalRuler(	void *		voidttr )
    {
    TedTopRuler *		ttr= (TedTopRuler *)voidttr;

    docCleanTabStopList( &(ttr->ttrTabStopList) );

    appCleanRulerData( &(ttr->ttrRulerData) );

    free( ttr );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the little button for the 'First Indent' of a paragraph	*/
/*									*/
/*	       v							*/
/*	4 ---- . ---- 0 = 5						*/
/*	|      .      |							*/
/*	|      .      |							*/
/*	3      .      1							*/
/*	 \     .     / 							*/
/*	  \    .    /  							*/
/*	   \   .   /   							*/
/*	    \  .  /    							*/
/*	     \ . /     							*/
/*	       2       							*/
/*									*/
/************************************************************************/

static void tedHangingButtonRect(	DocumentRectangle *	drButton,
					int			value,
					int			sizeAcross )
    {
    drButton->drX0= value- BUTTON_HWIDTH(sizeAcross)- 1;
    drButton->drX1= value+ BUTTON_HWIDTH(sizeAcross)+ 1;

    drButton->drY0= BUTTON_HWIDTH(sizeAcross)- 1;
    drButton->drY1= HBUTTON_APEX(sizeAcross)+ 1;

    return;
    }

static void tedRulerDrawHangingButton(	int			value,
					int			ox,
					int			sizeAcross,
					APP_BITMAP_MASK		stipple,
					AppDrawingData *	add )
    {
    APP_POINT	points[6];
    int		w= BUTTON_MARG;

    value -= ox;

    points[0].x= value+ BUTTON_HWIDTH(sizeAcross);
    points[0].y= HBUTTON_BASE(sizeAcross);

    points[1].x= points[0].x;
    points[1].y= HBUTTON_BASE(sizeAcross)+ BUTTON_RECTH(sizeAcross);

    points[2].x= value;
    points[2].y= HBUTTON_APEX(sizeAcross);

    points[3].x= value- BUTTON_HWIDTH(sizeAcross);
    points[3].y= points[1].y;

    points[4].x= points[3].x;
    points[4].y= points[0].y;

    points[5]= points[0];

    appDrawSetForegroundColor( add, &(add->addBackColor) );

#   ifdef USE_MOTIF
    if  ( stipple )
	{ XSetFillStyle( add->addDisplay, add->addGc, FillStippled ); }
#   endif

    appDrawFillPolygon( add, points, 6 );

#   ifdef USE_MOTIF
    if  ( stipple )
	{ XSetFillStyle( add->addDisplay, add->addGc, FillSolid ); }
#   endif

    appDrawSetForegroundColor( add, &(add->addForeColor) );
    appDrawDrawLines( add, points, 6 );

    if  ( ! stipple )
	{
	points[0].x -= w; points[0].y += (w+1)/2;
	points[1].x -= w;
	points[2].y -= w;
	points[3].x += w;
	points[4].x += w; points[4].y += w;
	points[5].x -= w; points[5].y += w;

	appDrawSetForegroundColor( add, &(add->addBottomColor) );
	appDrawDrawLines( add, points, 4 );
	appDrawSetForegroundColor( add, &(add->addTopColor) );
	appDrawDrawLines( add, points+ 3, 3 );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the little buttons for the 'Left, Right Indent' of a paragraph	*/
/*									*/
/*	       3   							*/
/*	     / . \     							*/
/*	    /  .  \    							*/
/*	   /   .   \   							*/
/*	  /    .    \  							*/
/*	 /     .     \ 							*/
/*	4      .      2							*/
/*	|      .      |							*/
/*	|      .      |							*/
/*  5 = 0 ---- . ---- 1							*/
/*	       v							*/
/*									*/
/************************************************************************/

static void tedStandingButtonRect(	DocumentRectangle *	drButton,
					int			value,
					int			sizeAcross )
    {
    drButton->drX0= value- BUTTON_HWIDTH(sizeAcross)- 1;
    drButton->drX1= value+ BUTTON_HWIDTH(sizeAcross)+ 1;

    drButton->drY0= SBUTTON_APEX(sizeAcross)- 1;
    drButton->drY1= SBUTTON_BASE(sizeAcross)+ 1;

    return;
    }

static void tedRulerDrawStandingButton(	int			value,
					int			ox,
					int			sizeAcross,
					APP_BITMAP_MASK		stipple,
					AppDrawingData *	add )
    {
    APP_POINT	points[6];
    int		w= BUTTON_MARG;

    value -= ox;

    points[0].x= value- BUTTON_HWIDTH(sizeAcross);
    points[0].y= SBUTTON_BASE(sizeAcross);

    points[1].x= value+ BUTTON_HWIDTH(sizeAcross);
    points[1].y= points[0].y;

    points[2].x= points[1].x;
    points[2].y= SBUTTON_BASE(sizeAcross)- BUTTON_RECTH(sizeAcross);

    points[3].x= value;
    points[3].y= SBUTTON_APEX(sizeAcross);

    points[4].x= points[0].x;
    points[4].y= points[2].y;

    points[5]= points[0];

    appDrawSetForegroundColor( add, &(add->addBackColor) );

#   ifdef USE_MOTIF
    if  ( stipple )
	{ XSetFillStyle( add->addDisplay, add->addGc, FillStippled ); }
#   endif

    appDrawFillPolygon( add, points, 6 );

#   ifdef USE_MOTIF
    if  ( stipple )
	{ XSetFillStyle( add->addDisplay, add->addGc, FillSolid ); }
#   endif

    appDrawSetForegroundColor( add, &(add->addForeColor) );
    appDrawDrawLines( add, points, 6 );

    if  ( ! stipple )
	{
	points[0].x += (w+1)/2; points[0].y -= w;
	points[1].x -= w; points[1].y -= w;
	points[2].x -= w;
	points[3].y += (w+1)/2;
	points[4].x += w;
	points[5].x += w; points[5].y -= w;

	appDrawSetForegroundColor( add, &(add->addBottomColor) );
	appDrawDrawLines( add, points, 4 );
	appDrawSetForegroundColor( add, &(add->addTopColor) );
	appDrawDrawLines( add, points+ 3, 3 );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the little holes for the table columns on the ruler.		*/
/*									*/
/*	3-------------2							*/
/*	|      .      |							*/
/*	|      .      |							*/
/*	|      .      |							*/
/*	|      .      |							*/
/*	|      .      |							*/
/*  4 = 0 ---- . ---- 1							*/
/*	       v							*/
/*									*/
/************************************************************************/

static void tedRulerColumnControlRect(	DocumentRectangle *	drButton,
					int			x0,
					int			x1,
					int			sizeAcross )
    {
    if  ( x1- x0 < BUTTON_HWIDTH( sizeAcross ) )
	{
	int	m= ( x1+ x0 )/ 2;

	x0= m- BUTTON_HWIDTH( sizeAcross )/2;
	x1= m+ BUTTON_HWIDTH( sizeAcross )/2;
	}

    drButton->drX0= x0- 1;
    drButton->drX1= x1+ 1;

    drButton->drY0= WHITE_TOP(sizeAcross)- 1;
    drButton->drY1= WHITE_BOTTOM(sizeAcross)+ 1;

    return;
    }

static void tedRulerDrawColumnControl(	int			x0,
					int			x1,
					int			ox,
					int			sizeAcross,
					APP_BITMAP_MASK		stipple,
					AppDrawingData *	add )
    {
    APP_POINT	points[5];
    int		w= BUTTON_MARG;

    x0 -= ox;
    x1 -= ox;

    if  ( x1- x0 < BUTTON_HWIDTH( sizeAcross ) )
	{
	int	m= ( x1+ x0 )/ 2;

	x0= m- BUTTON_HWIDTH( sizeAcross )/2;
	x1= m+ BUTTON_HWIDTH( sizeAcross )/2;
	}

    points[0].x= x0;
    points[0].y= WHITE_BOTTOM(sizeAcross);

    points[1].x= x1;
    points[1].y= points[0].y;

    points[2].x= points[1].x;
    points[2].y= WHITE_TOP(sizeAcross);

    points[3].x= points[0].x;
    points[3].y= points[2].y;

    points[4]= points[0];

    appDrawSetForegroundColor( add, &(add->addBackColor) );

#   ifdef USE_MOTIF
    if  ( stipple )
	{ XSetFillStyle( add->addDisplay, add->addGc, FillStippled ); }
#   endif

    appDrawFillPolygon( add, points, 5 );

#   ifdef USE_MOTIF
    if  ( stipple )
	{ XSetFillStyle( add->addDisplay, add->addGc, FillSolid ); }
#   endif

    appDrawSetForegroundColor( add, &(add->addForeColor) );
    appDrawDrawLines( add, points, 5 );

    if  ( ! stipple )
	{
	points[0].x += w; points[0].y -= w;
	points[1].x -= w; points[1].y -= w;
	points[2].x -= w; points[2].y += w;
	points[3].x += w; points[3].y += w;
	points[4].x += w; points[4].y -= w;

	appDrawSetForegroundColor( add, &(add->addTopColor) );
	appDrawDrawLines( add, points, 3 );
	appDrawSetForegroundColor( add, &(add->addBottomColor) );
	appDrawDrawLines( add, points+ 2, 2 );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a 'Tab' symbol.						*/
/*									*/
/************************************************************************/

static void tedTabSymbolRect(	DocumentRectangle *	drButton,
				int			x,
				int			y,
				int			sizeAcross )
    {
    int		thick= TAB_THICK(sizeAcross);
    int		tabWidth= TAB_WIDTH(sizeAcross);
    int		tabHeight= TAB_HEIGHT(sizeAcross);

    drButton->drX0= x+ (thick+1)/2- tabWidth;
    drButton->drX1= x- thick/2+ tabWidth;

    drButton->drY0= y- tabHeight- 1;
    drButton->drY1= y+ 1;

    return;
    }

static void tedDrawTabSymbol(	AppDrawingData *	add,
				int			x,
				int			ox,
				int			y,
				int			sizeAcross,
				int			kind )
    {
    int		thick= TAB_THICK(sizeAcross);
    int		tabWidth= TAB_WIDTH(sizeAcross);
    int		tabHeight= TAB_HEIGHT(sizeAcross);

    int		xx;

    x -= ox;

    /*  standing  */
    xx= x- thick/2;
    appDrawFillRectangle( add, xx, y- tabHeight, thick, tabHeight );

    if  ( kind == DOCtaLEFT		||
	  kind == DOCtaCENTER		||
	  kind == DOCtaDECIMAL		)
	{
	xx= x- thick/2;

	/*  laying right	*/
	appDrawFillRectangle( add, xx, y- thick, tabWidth, thick );
	}

    if  ( kind == DOCtaRIGHT		||
	  kind == DOCtaCENTER		||
	  kind == DOCtaDECIMAL		)
	{
	xx= x+ (thick+1)/2- tabWidth;

	/*  laying left	*/
	appDrawFillRectangle( add, xx, y- thick, tabWidth, thick );
	}

    if  ( kind == DOCtaDECIMAL	)
	{
	int	dec= ( thick+ 1 )/ 2;

	xx= x- thick/2+ tabWidth- dec;

	/*  dot right	*/
	appDrawFillRectangle( add, xx, y- tabHeight, dec, dec );
	}

    return;
    }

static void tedTabButtonRectangle(	DocumentRectangle *	drButton,
					int			x,
					int			y,
					int			sizeAcross )
    {
    tedTabSymbolRect( drButton, x, y, sizeAcross );

    drButton->drX0 -= 3;
    drButton->drX1 += 3;

    drButton->drY0 -= 5;
    drButton->drY1 += 0;

    return;
    }

static void tedDrawTabButton(	RulerData *	rd,
				int		x,
				int		y,
				int		sizeAcross,
				int		kind )
    {
    AppDrawingData *	add= &(rd->rdDrawingData);
    int			tw= TAB_WIDTH(sizeAcross);
    DocumentRectangle	dr;
    int			ox= 0;

    tedTabButtonRectangle( &dr, x, y, sizeAcross );


#   define xfr( x, y, w, h ) \
    appDrawFillRectangle( add, (x), (y), (w), (h) );

    /*  top, bottom */
    appDrawSetForegroundColor( add, &(add->addForeColor) );

    xfr(	dr.drX0,	dr.drY0,	dr.drX1- dr.drX0, 1 );
    xfr(	dr.drX0,	dr.drY1,	dr.drX1- dr.drX0, 1 );

    /*  left, right */
    xfr(	dr.drX0,	dr.drY0,	1, dr.drY1- dr.drY0+ 1 );
    xfr(	dr.drX1- 1,	dr.drY0,	1, dr.drY1- dr.drY0 );


    /*  top, left */
    appDrawSetForegroundColor( add, &(add->addTopColor) );

    xfr(	dr.drX0+ 1,	dr.drY0+ 1,	dr.drX1- dr.drX0- 2, 1 );
    xfr(	dr.drX0+ 1,	dr.drY0+ 1,	1, dr.drY1- dr.drY0- 1 );

    /*  bottom, right */
    appDrawSetForegroundColor( add, &(add->addBottomColor) );

    xfr(	dr.drX0+ 2,	dr.drY1- 1,	dr.drX1- dr.drX0- 3, 1 );
    xfr(	dr.drX1- 2,	dr.drY0+ 2,	1, dr.drY1- dr.drY0- 3 );

    appDrawSetForegroundColor( add, &(add->addForeColor) );

    tedDrawTabSymbol( add, x, ox, y- tw/2+ 1, sizeAcross, kind );
    }

/************************************************************************/
/*									*/
/*  Draw a horizontal ruler.						*/
/*									*/
/************************************************************************/

static void tedDrawTopRuler(	APP_WIDGET		w,
				TedTopRuler *		ttr,
				DocumentRectangle *	drClip,
				int			ox )
    {
    RulerData *		rd= &(ttr->ttrRulerData);
    AppDrawingData *	add= &(rd->rdDrawingData);
    int			sizeAcross= rd->rdSizeAcross;

    int			tick;
    double		units;

    int			tab;
    TabStop *		ts;

    int			whiteY= WHITE_TOP(sizeAcross);
    int			whiteH= WHITE_HEIGHT(sizeAcross);

    int			tagY= TAG_TOP(sizeAcross);
    int			tagH= TAG_HEIGHT(sizeAcross);

    int			tickY= TICK_TOP(sizeAcross);
    int			tickH= TICK_HEIGHT(sizeAcross);

    int			fontBottom= FONT_BOTTOM(sizeAcross);

    ColumnSeparator *	cs;

    DocumentRectangle	drWhite;
    DocumentRectangle	drButton;

    if  ( rd->rdSizeAlong == 0 )
	{
	int	high;

	appDrawGetSizeOfWidget( &(rd->rdSizeAlong), &high, w );

	rd->rdMaxUsed= rd->rdSizeAlong- rd->rdExtraAfterMaxUsed;
	}

    appRulerDrawBackground( &(ttr->ttrRulerData),
		    drClip->drX0- ox, drClip->drY0,
		    drClip->drX1- drClip->drX0, drClip->drY1- drClip->drY0 );

    drWhite.drX0= 0;
    drWhite.drX1= rd->rdRulerC1;
    drWhite.drY0= whiteY;
    drWhite.drY1= whiteY+ whiteH;

    if  ( geoIntersectRectangle( &drWhite, &drWhite, drClip ) )
	{
	int			x0= drWhite.drX0- ox;
	int			x1= drWhite.drX1- ox;

	appDrawSetForegroundWhite( add );

	if  ( x0 < rd->rdMinUsed )
	    { x0=  rd->rdMinUsed;	}
	if  ( x1 > rd->rdMaxUsed )
	    { x1=  rd->rdMaxUsed;	}

	appDrawFillRectangle( add,
		    x0, drWhite.drY0,
		    x1- x0+ 1, drWhite.drY1- drWhite.drY0 );
	}

    tedDrawTabButton( rd, rd->rdMinUsed, whiteY, sizeAcross, ttr->ttrTabKind );

    appDrawSetForegroundColor( add, &(add->addForeColor) );

    units= 0; tick= 0;
    for (;;)
	{
	int	twips= units* rd->rdTwipsPerUnit;
	int	iitem= rd->rdMagnifiedPixelsPerTwip* twips;

	if  ( iitem > rd->rdVisibleC1 )
	    { break;	}

	if  ( iitem < rd->rdVisibleC0 )
	    {
	    units += rd->rdUnitsPerTick; tick++;
	    continue;
	    }

	if  ( iitem != 0 && tick % rd->rdTicksPerText == 0 )
	    {
	    char	scratch[30];
	    int		len;
	    int		wide;

	    appRulerTagText( scratch, &wide, &len, rd, units );

	    if  ( iitem- ox- wide/2 >= rd->rdMinUsed	&&
		  iitem- ox+ wide/2 <  rd->rdMaxUsed	)
		{
		appDrawDrawString( add,
			    iitem- ox- wide/2, fontBottom, scratch, len );
		}
	    else{
		appDrawFillRectangle( add, iitem- ox, tagY, 1, tagH );
		}
	    }
	else{
	    if  ( tick % rd->rdTicksPerTag == 0 )
		{ appDrawFillRectangle( add, iitem- ox, tagY,  1, tagH );  }
	    else{ appDrawFillRectangle( add, iitem- ox, tickY, 1, tickH ); }
	    }

	units += rd->rdUnitsPerTick; tick++;
	continue;
	}

    cs= ttr->ttrColumns;
    for ( tab= 0; tab < ttr->ttrColumnCount; cs++, tab++ )
	{
	int	m= ( cs->csX0+ cs->csX1 )/ 2;

	if  ( m > rd->rdVisibleC1	||
	      m < rd->rdVisibleC0	)
	    { continue;	}

	tedRulerColumnControlRect( &drButton, cs->csX0, cs->csX1, sizeAcross );

	if  ( ! geoIntersectRectangle( &drButton, &drButton, drClip ) )
	    { continue;	}

	tedRulerDrawColumnControl( cs->csX0, cs->csX1, ox, sizeAcross,
			    rd->rdBackStipple, add );
	}

    if  ( ttr->ttrFirstIndent >= 0					&&
	  ttr->ttrFirstIndent >= rd->rdVisibleC0			&&
	  ttr->ttrFirstIndent <= rd->rdVisibleC1			)
	{
	tedHangingButtonRect( &drButton, ttr->ttrFirstIndent, sizeAcross );

	if  ( geoIntersectRectangle( &drButton, &drButton, drClip ) )
	    {
	    tedRulerDrawHangingButton( ttr->ttrFirstIndent, ox, sizeAcross,
				rd->rdBackStipple, add );
	    }
	}

    if  ( ttr->ttrLeftIndent >= 0					&&
	  ttr->ttrLeftIndent >= rd->rdVisibleC0			&&
	  ttr->ttrLeftIndent <= rd->rdVisibleC1			)
	{
	tedStandingButtonRect( &drButton, ttr->ttrLeftIndent, sizeAcross );

	if  ( geoIntersectRectangle( &drButton, &drButton, drClip ) )
	    {
	    tedRulerDrawStandingButton( ttr->ttrLeftIndent, ox, sizeAcross,
				rd->rdBackStipple, add );
	    }
	}

    if  ( ttr->ttrRightIndent >= 0					&&
	  ttr->ttrRightIndent >= rd->rdVisibleC0			&&
	  ttr->ttrRightIndent <= rd->rdVisibleC1			)
	{
	tedStandingButtonRect( &drButton, ttr->ttrRightIndent, sizeAcross );

	if  ( geoIntersectRectangle( &drButton, &drButton, drClip ) )
	    {
	    tedRulerDrawStandingButton( ttr->ttrRightIndent, ox, sizeAcross,
				rd->rdBackStipple, add );
	    }
	}

    appDrawSetForegroundColor( add, &(add->addForeColor) );

    ts= ttr->ttrTabStopList.tslTabStops;
    for ( tab= 0; tab < ttr->ttrTabStopList.tslTabStopCount; ts++, tab++ )
	{
	if  ( ts->tsPixels < rd->rdVisibleC0	||
	      ts->tsPixels > rd->rdVisibleC1	)
	    { continue;	}

	tedTabSymbolRect( &drButton, ts->tsPixels, whiteY+ whiteH, sizeAcross );

	if  ( ! geoIntersectRectangle( &drButton, &drButton, drClip ) )
	    { continue;	}

	tedDrawTabSymbol( add, ts->tsPixels, ox, whiteY+ whiteH,
						sizeAcross, ts->tsAlignment );
	}

    appDrawNoClipping( add );

    return;
    }

APP_EVENT_HANDLER_H( tedRedrawHorizontalRuler, w, voidttr, event )
    {
    TedTopRuler *	ttr= (TedTopRuler *)voidttr;
    RulerData *		rd= &(ttr->ttrRulerData);
    AppDrawingData *	add= &(rd->rdDrawingData);

    int			ox= rd->rdVisibleC0- rd->rdMinUsed;
    int			oy= 0;

    DocumentRectangle	drClip;

    if  ( ! rd->rdDrawingDataAllocated )
	{
	const int	vertical= 0;

	appRulerAllocateDrawingData( rd, vertical,
		    FONT_HEIGHT(rd->rdSizeAcross), ttr->ttrMagnification, w );
	}

    appCollectExposures( &drClip, add, ox, oy, event );

    tedDrawTopRuler( w, ttr, &drClip, ox );
    }

APP_EVENT_HANDLER_H( tedHorizontalRulerConfigure, w, voidttr, event )
    {
    TedTopRuler *	ttr= (TedTopRuler *)voidttr;
    RulerData *		rd= &(ttr->ttrRulerData);

    int			wide;
    int			high;

    if  ( appDrawGetSizeFromConfigureEvent( &wide, &high, w, event ) )
	{ return;	}

    rd->rdSizeAlong= wide;
    rd->rdMaxUsed= wide- rd->rdExtraAfterMaxUsed;

    return;
    }

/************************************************************************/
/*									*/
/*  Scroll the horizontal ruler.					*/
/*									*/
/*  1)  Stretch the rectangle to contain whole controls.		*/
/*									*/
/************************************************************************/

static void tedRulerChangeRect(	int *				pChanged,
				DocumentRectangle *		drRedraw,
				const DocumentRectangle *	drClip,
				int				d,
				const TedTopRuler *		ttr )
    {
    const RulerData *		rd= &(ttr->ttrRulerData);
    int				sizeAcross= rd->rdSizeAcross;
    int				whY= WHITE_TOP(sizeAcross);

    TabStop *			ts;
    ColumnSeparator *		cs;
    int				tab;

    int				changed= 0;

    DocumentRectangle		drButMore;
    DocumentRectangle		drButLess;
    DocumentRectangle		drScratch;

    *drRedraw= *drClip;

    tedHangingButtonRect( &drButMore, ttr->ttrFirstIndent+ d, sizeAcross );
    tedHangingButtonRect( &drButLess, ttr->ttrFirstIndent- d, sizeAcross );
    geoUnionRectangle( &drButMore, &drButMore, &drButLess );
    if  ( geoIntersectRectangle( &drScratch, drClip, &drButMore ) )
	{ geoUnionRectangle( drRedraw, drRedraw, &drButMore ); changed= 1; }

    tedStandingButtonRect( &drButMore, ttr->ttrLeftIndent+ d, sizeAcross );
    tedStandingButtonRect( &drButLess, ttr->ttrLeftIndent- d, sizeAcross );
    geoUnionRectangle( &drButMore, &drButMore, &drButLess );
    if  ( geoIntersectRectangle( &drScratch, drClip, &drButMore ) )
	{ geoUnionRectangle( drRedraw, drRedraw, &drButMore ); changed= 1; }

    tedStandingButtonRect( &drButMore, ttr->ttrRightIndent+ d, sizeAcross );
    tedStandingButtonRect( &drButLess, ttr->ttrRightIndent- d, sizeAcross );
    geoUnionRectangle( &drButMore, &drButMore, &drButLess );
    if  ( geoIntersectRectangle( &drScratch, drClip, &drButMore ) )
	{ geoUnionRectangle( drRedraw, drRedraw, &drButMore ); changed= 1; }

    tedTabButtonRectangle( &drButMore, rd->rdVisibleC0+ d, whY, sizeAcross );
    tedTabButtonRectangle( &drButLess, rd->rdVisibleC0- d, whY, sizeAcross );
    geoUnionRectangle( &drButMore, &drButMore, &drButLess );
    if  ( geoIntersectRectangle( &drScratch, drClip, &drButMore ) )
	{ geoUnionRectangle( drRedraw, drRedraw, &drButMore ); changed= 1; }

    cs= ttr->ttrColumns;
    for ( tab= 0; tab < ttr->ttrColumnCount; cs++, tab++ )
	{
	tedRulerColumnControlRect( &drButMore,
					cs->csX0+ d, cs->csX1+ d, sizeAcross );
	tedRulerColumnControlRect( &drButLess,
					cs->csX0- d, cs->csX1- d, sizeAcross );

	geoUnionRectangle( &drButMore, &drButMore, &drButLess );
	if  ( geoIntersectRectangle( &drScratch, drClip, &drButMore ) )
	    { geoUnionRectangle( drRedraw, drRedraw, &drButMore ); changed= 1; }
	}

    ts= ttr->ttrTabStopList.tslTabStops;
    for ( tab= 0; tab < ttr->ttrTabStopList.tslTabStopCount; ts++, tab++ )
	{
	int	y= whY+ WHITE_HEIGHT(sizeAcross);

	tedTabSymbolRect( &drButMore, ts->tsPixels+ d, y, sizeAcross );
	tedTabSymbolRect( &drButLess, ts->tsPixels- d, y, sizeAcross );

	geoUnionRectangle( &drButMore, &drButMore, &drButLess );
	if  ( geoIntersectRectangle( &drScratch, drClip, &drButMore ) )
	    { geoUnionRectangle( drRedraw, drRedraw, &drButMore ); changed= 1; }
	}

    *pChanged= changed;
    return;
    }

void tedScrollHorizontalRuler(	void *			voidttr,
				APP_WIDGET		w,
				int			d	)
    {
    TedTopRuler *		ttr= (TedTopRuler *)voidttr;
    RulerData *			rd= &(ttr->ttrRulerData);

    int				changed;

    DocumentRectangle		drClip;
    DocumentRectangle		drRedraw;

    int				ox;

    appScrollHorizontalRuler( rd, &drClip, d );

    ox= rd->rdVisibleC0- rd->rdMinUsed;

    changed= 0;
    tedRulerChangeRect( &changed, &drRedraw, &drClip, d, ttr );

    tedDrawTopRuler( w, ttr, &drRedraw, ox );

    if  ( d > 0 )
	{ drClip.drX0= rd->rdVisibleC0- 1; drClip.drX1= rd->rdVisibleC0+ 1; }
    else{ drClip.drX0= rd->rdVisibleC1- 1; drClip.drX1= rd->rdVisibleC1+ 1; }

    changed= 0;
    tedRulerChangeRect( &changed, &drRedraw, &drClip, d, ttr );

    if  ( changed )
	{ tedDrawTopRuler( w, ttr, &drRedraw, ox ); }
    }

void tedSetHorizontalRulerRange(	void *			voidttr,
					APP_WIDGET		w,
					int			docVisX0,
					int			docVisX1,
					int			docBackX1 )
    {
    TedTopRuler *		ttr= (TedTopRuler *)voidttr;
    RulerData *			rd= &(ttr->ttrRulerData);
    AppDrawingData *		add= &(rd->rdDrawingData);

    rd->rdVisibleC0= docVisX0;
    rd->rdVisibleC1= docVisX1;
    rd->rdMaxUsed= docBackX1;

    appExposeRectangle( add, 0, 0, 0, 0 );

    return;
    }

/************************************************************************/
/*									*/
/*  Let the user drag one of the controls on the top ruler.		*/
/*									*/
/*  0)  Refuse to move outside the permitted range, but do not jump	*/
/*	back into it: Some documents do have strange settings and a	*/
/*	forced correction confuses the user.				*/
/*  1)  Draw a 'hair' over the document that corresponds to the		*/
/*	position of the control.					*/
/*  2)  Until the mouse button is released, observe events.		*/
/*  3)  Merge as many mouse motion events as possible.			*/
/*  4)  If the event is a mouse motion or a button release, redraw the	*/
/*	old an new position of the control on the ruler. (Assume that	*/
/*	the buttons are wider than the tab marks.			*/
/*  5)  Let the document move the hair.					*/
/*  6)  When the mouse button is released, the loop ends.		*/
/*  7)  Let the toolkit dispatch all events that are not relevant for	*/
/*	this routine.							*/
/*									*/
/************************************************************************/

static void tedHorizontalRulerSetValue(	TopRulerDrag *		trd,
					int			newValue )
    {
    TedTopRuler *		ttr= trd->trdTtr;
    RulerData *			rd= &(ttr->ttrRulerData);
    int				oldValue= trd->trdValue;

    int				v0;
    int				v1;
    int				nv0;
    int				nv1;

    DocumentRectangle		drClip;

    const int			asUpper= 0;

    if  ( newValue == oldValue )
	{ return;	}

    trd->trdValue= newValue;
    *(trd->trdChangedPointer)= 1;

    v1= v0= *(trd->trdV0Pointer);
    nv1= nv0= *(trd->trdV0Pointer)+ newValue- oldValue;

    if  ( trd->trdV1Pointer )
	{
	v1= *(trd->trdV1Pointer);
	nv1= *(trd->trdV1Pointer)+ newValue- oldValue;
	}

    /*  0  */
    if  ( v0 >= trd->trdX0 && nv0 < trd->trdX0 )
	{ nv1 += trd->trdX0- nv0; nv0 += trd->trdX0- nv0; }
    if  ( v1 <= trd->trdX1 && nv1 > trd->trdX1 )
	{ nv0 -= nv1- trd->trdX1; nv1 -= nv1- trd->trdX1; }

    *(trd->trdV0Pointer)= nv0;
    if  ( trd->trdV1Pointer )
	{ *(trd->trdV1Pointer)= nv1;	}

    if  ( v0 < nv0 )
	{
	drClip.drX0= v0-  BUTTON_HWIDTH(rd->rdSizeAcross)- BUTTON_MARG;
	drClip.drX1= nv1+ BUTTON_HWIDTH(rd->rdSizeAcross)+ BUTTON_MARG;
	drClip.drY0= 0;
	drClip.drY1= rd->rdSizeAcross;
	}
    else{
	drClip.drX0= nv0- BUTTON_HWIDTH(rd->rdSizeAcross)- BUTTON_MARG;
	drClip.drX1= v1+  BUTTON_HWIDTH(rd->rdSizeAcross)+ BUTTON_MARG;
	drClip.drY0= 0;
	drClip.drY1= rd->rdSizeAcross;
	}

    tedDrawTopRuler( trd->trdWidget, ttr, &drClip, trd->trdOc );

    /*  5  */
    (*trd->trdDragHair)( trd->trdThrough, v0, nv0, asUpper );
    if  ( trd->trdV1Pointer )
	{
	(*trd->trdDragHair)( trd->trdThrough, v1, nv1, asUpper );
	}

    return;
    }

static APP_EVENT_HANDLER_H( tedHorizontalRulerMouseMove, w, vtrd, event )
    {
    TopRulerDrag *		trd= (TopRulerDrag *)vtrd;

    int				x;
    int				y;

    if  ( appGetCoordinatesFromMouseMoveEvent( &x, &y, w, event ) )
	{ return;	}

    tedHorizontalRulerSetValue( trd, x+ trd->trdOc );

    return;
    }

static APP_EVENT_HANDLER_H( tedHorizontalRulerMouseUp, w, vtrd, event )
    {
    TopRulerDrag *		trd= (TopRulerDrag *)vtrd;
    const int			asUpper= 0;

    int				x;
    int				y;

    int				button;
    int				upDown;
    int				seq;
    unsigned int		keyState;

    if  ( appGetCoordinatesFromMouseButtonEvent(
			&x, &y, &button, &upDown, &seq, &keyState, w, event ) )
	{ return;	}

    tedHorizontalRulerSetValue( trd, x+ trd->trdOc );

    (*trd->trdDragHair)( trd->trdThrough,
				    *(trd->trdV0Pointer), -1, asUpper );
    if  ( trd->trdV1Pointer )
	{
	(*trd->trdDragHair)( trd->trdThrough,
				    *(trd->trdV1Pointer), -1, asUpper );
	}

    *(trd->trdYPointer)= y;

    return;
    }

static void tedHorizontalRulerDragItem(	int *			pV0,
					int *			pV1,
					int *			pY,
					int *			pChanged,
					APP_WIDGET		w,
					EditApplication *	ea,
					APP_EVENT *		downEvent,
					TedTopRuler *		ttr,
					int			x0,
					int			x1,
					int			v,
					int			ox,
					void *			voided,
					DragHair		dragHair )
    {
    const int		asUpper= 0;
    TopRulerDrag	trd;

    trd.trdValue= v;
    trd.trdV0Pointer= pV0;
    trd.trdV1Pointer= pV1;
    trd.trdYPointer= pY;
    trd.trdChangedPointer= pChanged;
    trd.trdTtr= ttr;
    trd.trdWidget= w;
    trd.trdX0= x0;
    trd.trdX1= x1;
    trd.trdOc= ox;
    trd.trdThrough= voided;
    trd.trdDragHair= dragHair;

    /*  1  */
    (*trd.trdDragHair)( trd.trdThrough, -1, *(trd.trdV0Pointer), asUpper );
    if  ( trd.trdV1Pointer )
	{
	(*trd.trdDragHair)( trd.trdThrough, -1, *(trd.trdV1Pointer), asUpper );
	}

    appRunDragLoop( w, ea, downEvent,
				tedHorizontalRulerMouseUp,
				tedHorizontalRulerMouseMove,
				0, (APP_TIMER_CALLBACK)0,
				(void *)&trd );
    }

/************************************************************************/
/*									*/
/*  The user clicked on the top ruler, make her happy.			*/
/*									*/
/*  1)  Only accept 'left mouse down' events.				*/
/*  2)  If the click is on one of the tab marks, apparently the user	*/
/*	wants to move the tab.						*/
/*  3)  If the tab was dragged from the ruler, assume that the user	*/
/*	wants to delete it.						*/
/*  4)  Move first indent?						*/
/*  5)  Move left indent?						*/
/*  6)  Move right indent?						*/
/*  7)  Not on one of the controls.. If the click is on the white band,	*/
/*	assume she wants to set a new tab.				*/
/*  8)  Is it on one of the table column separators? Allow the user to	*/
/*	drag it.							*/
/*  9)  Is it on the button with the kind of thabs that is to be added?	*/
/*	Rotate the type.						*/
/*									*/
/************************************************************************/

static int tedCompareTabStopsPixels(	const void *	voidts1,
					const void *	voidts2	)
    {
    const TabStop *	ts1= (const TabStop *)voidts1;
    const TabStop *	ts2= (const TabStop *)voidts2;

    return ts1->tsPixels- ts2->tsPixels;
    }

/*  2  */
static void tedHorizontalRulerDragTab(	int *			pChanged,
					APP_WIDGET		w,
					EditApplication *	ea,
					APP_EVENT *		downEvent,
					TedTopRuler *		ttr,
					int			item,
					int			x,
					int			ox,
					void *			voided,
					DragHair		dragHair )
    {
    RulerData *		rd= &(ttr->ttrRulerData);
    AppDrawingData *	add= &(rd->rdDrawingData);
    int			sizeAcross= rd->rdSizeAcross;

    int			y;

    TabStop *		ts= ttr->ttrTabStopList.tslTabStops+ item;

    int			x0= ts->tsPixels;
    int			x1;
    int			dxAbs;

    tedHorizontalRulerDragItem( &(ts->tsPixels), (int *)0, &y, pChanged,
			w, ea, downEvent,
			ttr, ttr->ttrColX0, ttr->ttrColX1, x, ox,
			voided, dragHair );

    x1= ts->tsPixels;

    dxAbs= x1- x0;
    if  ( dxAbs < 0 )
	{ dxAbs= -dxAbs;	}

    /*  3  */
    if  ( dxAbs < 2* WHITE_HEIGHT(sizeAcross) )
	{
	if  ( y <  WHITE_TOP(sizeAcross)				||
	      y >  WHITE_TOP(sizeAcross)+ WHITE_HEIGHT(sizeAcross)	)
	    {
	    docDeleteTabFromList( &(ttr->ttrTabStopList), item );

	    appExposeRectangle( add, 0, 0, 0, 0 );
	    }
	}

    qsort( ttr->ttrTabStopList.tslTabStops, ttr->ttrTabStopList.tslTabStopCount,
				sizeof(TabStop), tedCompareTabStopsPixels );

    return;
    }

void tedHorizontalRulerTrackMouse(	int *			pFirstIndent,
					int *			pLeftIndent,
					int *			pRightIndent,
					TabStopList *		tsl,
					int *			pCsCount,
					ColumnSeparator **	pCs,
					int *			pProperty,
					APP_WIDGET		w,
					EditApplication *	ea,
					APP_EVENT *		downEvent,
					void *			voidttr,
					void *			voided,
					DragHair		dragHair )
    {
    TedTopRuler  *	ttr= (TedTopRuler *)voidttr;
    RulerData *		rd= &(ttr->ttrRulerData);
    AppDrawingData *	add= &(rd->rdDrawingData);
    int			sizeAcross= rd->rdSizeAcross;

    int			x;
    int			y;
    int			ox= rd->rdVisibleC0- rd->rdMinUsed;

    int			item;
    int			tabWidth= TAB_WIDTH(sizeAcross);

    int			whiteY= WHITE_TOP(sizeAcross);
    int			whiteH= WHITE_HEIGHT(sizeAcross);

    int			changed= 0;
    int			yy;

    DocumentRectangle	dr;

    int			button;
    int			upDown;
    int			seq;
    unsigned int	keyState;

    if  ( appGetCoordinatesFromMouseButtonEvent(
			&x, &y, &button, &upDown, &seq, &keyState, w, downEvent ) )
	{ return;	}

    if  ( upDown < 1 || button != 1 || seq > 1 )
	{ return;	}

    /*  2  */
    if  ( y >= whiteY		&&
	  y <  whiteY+ whiteH	)
	{
	TabStop *		ts;

	ts= ttr->ttrTabStopList.tslTabStops;
	for ( item= 0; item < ttr->ttrTabStopList.tslTabStopCount; ts++, item++ )
	    {
	    if  ( x+ ox >= ts->tsPixels			&&
		  x+ ox <  ts->tsPixels+ tabWidth	)
		{
		tedHorizontalRulerDragTab( &changed, w, ea, downEvent, ttr,
					    item, x+ ox, ox, voided, dragHair );

		if  ( changed )
		    {
		    *tsl= ttr->ttrTabStopList;
		    *pProperty= PPpropTAB_STOPS;
		    }
		else{ *pProperty= PPprop_NONE; }

		return;
		}
	    }
	}

    /*  4  */
    if  ( ttr->ttrFirstIndent >= 0					&&
	  y < HBUTTON_APEX(sizeAcross)					&&
	  x+ ox >= ttr->ttrFirstIndent- BUTTON_HWIDTH(sizeAcross)	&&
	  x+ ox <= ttr->ttrFirstIndent+ BUTTON_HWIDTH(sizeAcross)	)
	{
	tedHorizontalRulerDragItem(
			    &(ttr->ttrFirstIndent), (int *)0, &yy, &changed,
			    w, ea, downEvent, ttr,
			    ttr->ttrColX0, ttr->ttrColX1, x+ ox, ox,
			    voided, dragHair );

	if  ( changed )
	    {
	    *pFirstIndent= ttr->ttrFirstIndent;
	    *tsl= ttr->ttrTabStopList;
	    *pProperty= PPpropFIRST_INDENT;
	    }
	else{ *pProperty= PPprop_NONE; }
	return;
	}

    /*  5  */
    if  ( ttr->ttrLeftIndent >= 0					&&
	  y >= SBUTTON_APEX(sizeAcross)					&&
	  y <= SBUTTON_BASE(sizeAcross)					&&
	  x+ ox >= ttr->ttrLeftIndent- BUTTON_HWIDTH(sizeAcross)	&&
	  x+ ox <= ttr->ttrLeftIndent+ BUTTON_HWIDTH(sizeAcross)	)
	{
	tedHorizontalRulerDragItem(
				&ttr->ttrLeftIndent, (int *)0, &yy, &changed,
				w, ea, downEvent, ttr,
				ttr->ttrColX0, ttr->ttrColX1, x+ ox, ox,
				voided, dragHair );

	if  ( changed )
	    {
	    *pLeftIndent= ttr->ttrLeftIndent;
	    *tsl= ttr->ttrTabStopList;
	    *pProperty= PPpropLEFT_INDENT;
	    }
	else{ *pProperty= PPprop_NONE; }
	return;
	}

    /*  6  */
    if  ( ttr->ttrRightIndent >= 0					&&
	  y >= SBUTTON_APEX(sizeAcross)					&&
	  y <= SBUTTON_BASE(sizeAcross)					&&
	  x+ ox >= ttr->ttrRightIndent- BUTTON_HWIDTH(sizeAcross)	&&
	  x+ ox <= ttr->ttrRightIndent+ BUTTON_HWIDTH(sizeAcross)	)
	{
	tedHorizontalRulerDragItem(
				&ttr->ttrRightIndent, (int *)0, &yy, &changed,
				w, ea, downEvent, ttr,
				ttr->ttrColX0, ttr->ttrColX1, x+ ox, ox,
				voided, dragHair );

	if  ( changed )
	    {
	    *pRightIndent= ttr->ttrRightIndent;
	    *tsl= ttr->ttrTabStopList;
	    *pProperty= PPpropRIGHT_INDENT;
	    }
	else{ *pProperty= PPprop_NONE; }
	return;
	}

    /*  7  */
    if  ( y >= whiteY			&&
	  y <  whiteY+ whiteH		&&
	  x+ ox >= ttr->ttrColX0	&&
	  x+ ox <= ttr->ttrColX1	)
	{
	TabStop		tsNew;

	docInitTabStop( &tsNew );

	tsNew.tsPixels= x+ ox;
	tsNew.tsAlignment= ttr->ttrTabKind;

	item= docAddTabToListPixels( &(ttr->ttrTabStopList), &tsNew );
	if  ( item < 0 )
	    { LDEB(item); *pProperty= PPprop_NONE; return;	}

	appExposeRectangle( add, 0, 0, 0, 0 );

	tedHorizontalRulerDragTab( &changed, w, ea, downEvent, ttr,
					    item, x+ ox, ox, voided, dragHair );

	*tsl= ttr->ttrTabStopList;
	*pProperty= PPpropTAB_STOPS; return;
	}

    /*  8  */
    if  ( y >= whiteY			&&
	  y <  whiteY+ whiteH		)
	{
	for ( item= 0; item < ttr->ttrColumnCount; item++ )
	    {
	    if  ( x+ ox >= ttr->ttrColumns[item].csX0		&&
		  x+ ox <  ttr->ttrColumns[item].csX1		)
		{
		int		wide;
		int		x0;
		int		x1;

		wide= ttr->ttrColumns[item].csX1- ttr->ttrColumns[item].csX0;
		x0= rd->rdDocumentC0- wide;
		x1= rd->rdDocumentC1+ wide;

		if  ( item > 0 )
		    { x0= ttr->ttrColumns[item- 1].csX1; }
		if  ( item < ttr->ttrColumnCount- 1 )
		    { x1= ttr->ttrColumns[item+ 1].csX0; }

		tedHorizontalRulerDragItem(
				    &ttr->ttrColumns[item].csX0,
				    &ttr->ttrColumns[item].csX1,
				    &yy, &changed,
				    w, ea, downEvent, ttr,
				    x0, x1, x+ ox, ox,
				    voided, dragHair );

		if  ( changed )
		    {
		    *pCsCount= ttr->ttrColumnCount; *pCs= ttr->ttrColumns;
		    *pProperty= PPpropCOLUMNS;
		    }
		else{ *pProperty= PPprop_NONE;	}
		return;
		}
	    }
	}

    /*  9  */
    tedTabButtonRectangle( &dr, rd->rdMinUsed, whiteY, sizeAcross );

    if  ( x >= dr.drX0 && x <= dr.drX1	&&
	  y >= dr.drY0 && y <= dr.drY1	)
	{
	switch( ttr->ttrTabKind )
	    {
	    case DOCtaLEFT:	ttr->ttrTabKind= DOCtaRIGHT;	break;
	    case DOCtaRIGHT:	ttr->ttrTabKind= DOCtaCENTER;	break;
	    case DOCtaCENTER:	ttr->ttrTabKind= DOCtaDECIMAL;	break;
	    case DOCtaDECIMAL:	ttr->ttrTabKind= DOCtaLEFT;	break;
	    default:
		LDEB(ttr->ttrTabKind);
	    }

	appExposeRectangle( add,
			dr.drX0, dr.drY0,
			dr.drX1- dr.drX0+ 1, dr.drY1- dr.drY0+ 1 );
	}

    *pProperty= PPprop_NONE; return;
    }

/************************************************************************/
/*									*/
/*  Adapt an horizontal ruler to the paragraph.				*/
/*									*/
/************************************************************************/

void tedAdaptHorizontalRuler(	void *			voidttr,
				APP_WIDGET		w,
				int			documentC0,
				int			documentC1,
				int			firstIndent,
				int			leftIndent,
				int			rightIndent,
				int			rulerC1,
				const TabStopList *	tslSet )
    {
    TedTopRuler *	ttr= (TedTopRuler *)voidttr;
    RulerData *		rd= &(ttr->ttrRulerData);
    AppDrawingData *	add= &(rd->rdDrawingData);

    int			tabsChanged= 0;
    int			changed= 0;
    const int		pixels= 1;

    rd->rdDocumentC0= documentC0;
    rd->rdDocumentC1= documentC1;

    if  ( ttr->ttrFirstIndent != firstIndent )
	{ ttr->ttrFirstIndent=   firstIndent; changed= 1;	}

    if  ( ttr->ttrLeftIndent != leftIndent )
	{ ttr->ttrLeftIndent=   leftIndent; changed= 1;	}

    if  ( ttr->ttrRightIndent != rightIndent )
	{ ttr->ttrRightIndent=   rightIndent; changed= 1;	}

    if  ( rd->rdRulerC1 != rulerC1 )
	{ rd->rdRulerC1=   rulerC1; changed= 1;	}

    if  ( docCopyTabStopList( &tabsChanged, &(ttr->ttrTabStopList),
							tslSet, pixels ) )
	{ LDEB(1); return;	}
    if  ( tabsChanged )
	{ changed= 1;	}

#   ifdef USE_MOTIF
    if  ( changed && XtIsRealized( w ) )
#   endif
#   ifdef USE_GTK
    if  ( changed && GTK_WIDGET_REALIZED( w ) )
#   endif
	{
	const int	vertical= 0;

	if  ( ! rd->rdDrawingDataAllocated )
	    {
	    appRulerAllocateDrawingData( rd, vertical,
					FONT_HEIGHT(rd->rdSizeAcross),
					ttr->ttrMagnification, w );
	    }
	if  ( ! rd->rdDrawingDataAllocated )
	    { LDEB(1); return;	}

	appExposeRectangle( add, 0, 0, 0, 0 );
	}

    return;
    }

int tedSetRulerColumns(		APP_WIDGET		w,
				void *			voidttr,
				int			colX0,
				int			colX1,
				ColumnSeparator *	cs,
				int			csCount )
    {
    TedTopRuler *	ttr= (TedTopRuler *)voidttr;
    RulerData *		rd= &(ttr->ttrRulerData);
    AppDrawingData *	add= &(rd->rdDrawingData);
    int			changed= 0;
    int			i;

    ttr->ttrColX0= colX0;
    ttr->ttrColX1= colX1;

    if  ( csCount != ttr->ttrColumnCount )
	{
	if  ( csCount > ttr->ttrColumnCount )
	    {
	    ColumnSeparator *	fresh;

	    fresh= (ColumnSeparator *)
		realloc( ttr->ttrColumns, csCount* sizeof(ColumnSeparator) );
	    if  ( ! fresh )
		{ LXDEB(csCount,fresh); return -1;	}

	    ttr->ttrColumns= fresh;
	    fresh += ttr->ttrColumnCount;

	    while( ttr->ttrColumnCount < csCount )
		{
		fresh->csX0= 0;
		fresh->csX1= 0;

		ttr->ttrColumnCount++; fresh++;
		}
	    }

	ttr->ttrColumnCount= csCount;
	changed= 1;
	}

    for ( i= 0; i < csCount; i++ )
	{
	if  ( ttr->ttrColumns[i].csX0 != cs[i].csX0 )
	    { ttr->ttrColumns[i].csX0=   cs[i].csX0; changed= 1; }
	if  ( ttr->ttrColumns[i].csX1 != cs[i].csX1 )
	    { ttr->ttrColumns[i].csX1=   cs[i].csX1; changed= 1; }
	}

#   ifdef USE_MOTIF
    if  ( changed && XtIsRealized( w ) )
#   endif
#   ifdef USE_GTK
    if  ( changed && GTK_WIDGET_REALIZED( w ) )
#   endif
	{
	appExposeRectangle( add, 0, 0, 0, 0 );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Left Ruler..							*/
/*									*/
/************************************************************************/

typedef struct TedLeftRuler
    {
    RulerData		tlrRulerData;

    int			tlrPageStep;
    int			tlrTopPage;
    int			tlrBottomPage;

    const char *	tlrPageFormat;
    char *		tlrPageScratch;

    double		tlrMagnification;
    } TedLeftRuler;

void * tedMakeLeftRuler(	int			width,
				double			magnifiedPixelsPerTwip,
				double			magnification,

				int			minUnused,
				int			maxUnused,

				int			documentC0,
				int			documentC1,
				int			visibleC0,
				int			visibleC1,
				int			rulerC1,

				const char *		fontName,
				int			whatUnit,
				int			pageStep,
				const char *		pageFormat )
    {
    TedLeftRuler *		tlr;
    RulerData *			rd;

    int				l;
    const char *		s;

    tlr= (TedLeftRuler *)malloc( sizeof( TedLeftRuler ) );
    if  ( ! tlr )
	{ XDEB(tlr); return (void *)0;	}
    rd= &(tlr->tlrRulerData);

    tlr->tlrPageFormat= pageFormat;
    tlr->tlrPageStep= pageStep;
    tlr->tlrTopPage= -1;
    tlr->tlrBottomPage= -1;

    tlr->tlrMagnification= magnification;

    l= 0; s= tlr->tlrPageFormat;
    while( *s )
	{
	if  ( s[0] == '%' && s[1] == 'd' )
	    { l += 12;	}
	else{ l++;	}

	s++;
	}

    tlr->tlrPageScratch= malloc( l+ 1 );
    if  ( ! tlr->tlrPageScratch )
	{ LXDEB(l,tlr->tlrPageScratch);	}
    tlr->tlrPageScratch[0]= '\0';

    appInitRulerData( &(tlr->tlrRulerData) );

    rd->rdFontName= fontName;
    rd->rdUnit= whatUnit;
    appRulerCalculateIncrements( rd, magnifiedPixelsPerTwip, magnification );

    rd->rdMinUsed= minUnused;
    rd->rdMaxUsed= 0;
    rd->rdExtraAfterMaxUsed= maxUnused;

    rd->rdDocumentC0= documentC0;
    rd->rdDocumentC1= documentC1;
    rd->rdVisibleC0= visibleC0;
    rd->rdVisibleC1= visibleC1;
    rd->rdRulerC1= rulerC1;

    rd->rdSizeAcross= width;
    rd->rdSizeAlong= 0;

    return (void *)tlr;
    }

APP_EVENT_HANDLER_H( tedVerticalRulerConfigure, w, voidtlr, event )
    {
    TedLeftRuler *	tlr= (TedLeftRuler *)voidtlr;
    RulerData *		rd= &(tlr->tlrRulerData);

    int			wide;
    int			high;

    if  ( appDrawGetSizeFromConfigureEvent( &wide, &high, w, event ) )
	{ return;	}

    rd->rdSizeAlong= high;
    rd->rdMaxUsed= high- rd->rdExtraAfterMaxUsed;

    return;
    }

void tedSetLeftRulerRange(	void *			voidtlr,
				APP_WIDGET		w,
				int			docVisV0,
				int			docVisV1,
				int			docBackV1 )
    {
    TedLeftRuler *		tlr= (TedLeftRuler *)voidtlr;
    RulerData *			rd= &(tlr->tlrRulerData);
    AppDrawingData *		add= &(rd->rdDrawingData);

    rd->rdVisibleC0= docVisV0;
    rd->rdVisibleC1= docVisV1;
    rd->rdMaxUsed= docBackV1;

    tlr->tlrTopPage=	  rd->rdVisibleC0    / tlr->tlrPageStep;
    tlr->tlrBottomPage=	( rd->rdVisibleC1- 1)/ tlr->tlrPageStep;

#   ifdef USE_MOTIF
    if  ( XtIsRealized( w ) )
#   endif
#   ifdef USE_GTK
    if  ( GTK_WIDGET_REALIZED( w ) )
#   endif
	{
	const int	vertical= 1;

	if  ( ! rd->rdDrawingDataAllocated )
	    {
	    appRulerAllocateDrawingData( rd, vertical,
			2* rd->rdSizeAcross/ 3- 2, tlr->tlrMagnification, w );
	    }
	if  ( ! rd->rdDrawingDataAllocated )
	    { LDEB(1); return;	}

	appExposeRectangle( add, 0, 0, 0, 0 );
	}

    return;
    }

void tedScrollLeftRuler(	void *			voidtlr,
				APP_WIDGET		w,
				int			d	)
    {
    TedLeftRuler *		tlr= (TedLeftRuler *)voidtlr;
    RulerData *			rd= &(tlr->tlrRulerData);
    AppDrawingData *		add= &(rd->rdDrawingData);

    int				oldTopPage= tlr->tlrTopPage;
    int				oldBottomPage= tlr->tlrBottomPage;

    rd->rdVisibleC0 += d;
    rd->rdVisibleC1 += d;

    tlr->tlrTopPage=	  rd->rdVisibleC0    / tlr->tlrPageStep;
    tlr->tlrBottomPage=	( rd->rdVisibleC1- 1)/ tlr->tlrPageStep;

    if  ( tlr->tlrTopPage != oldTopPage		||
	  tlr->tlrBottomPage != oldBottomPage	)
	{ appExposeRectangle( add, 0, 0, 0, 0 );	}

    return;
    }

void tedAdaptLeftRuler(		void *		voidtlr,
				APP_WIDGET	w,
				int		visisbleC0,
				int		visisbleC1,
				int		pageStep )
    {
    TedLeftRuler *		tlr= (TedLeftRuler *)voidtlr;
    RulerData *			rd= &(tlr->tlrRulerData);
    AppDrawingData *		add= &(rd->rdDrawingData);

    rd->rdVisibleC0= visisbleC0;
    rd->rdVisibleC1= visisbleC1;

    tlr->tlrPageStep= pageStep;

    tlr->tlrTopPage=	  rd->rdVisibleC0    / tlr->tlrPageStep;
    tlr->tlrBottomPage=	( rd->rdVisibleC1- 1)/ tlr->tlrPageStep;

    appExposeRectangle( add, 0, 0, 0, 0 );
    }

void tedFreeVerticalRuler(	void *		voidtlr )
    {
    TedLeftRuler *		tlr= (TedLeftRuler *)voidtlr;

    if  ( tlr->tlrPageScratch )
	{ free( tlr->tlrPageScratch );	}

    appCleanRulerData( &(tlr->tlrRulerData) );

    free( tlr );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a vertical ruler.						*/
/*									*/
/************************************************************************/

static void tedRulerFormatPageNumber(	TedLeftRuler *		tlr,
					int *			pLen,
					int *			pWide,
					int			page )
    {
    RulerData *		rd= &(tlr->tlrRulerData);

    char		scratch[11];
    const char *	from;
    char *		to;

    int			len;
    int			wide;

    sprintf( scratch, "%d", page+ 1 );

    from= tlr->tlrPageFormat;
    to= tlr->tlrPageScratch;
    while( *from )
	{
	if  ( from[0] == '%' && from[1] == 'd' )
	    { strcpy( to, scratch ); to += strlen( to ); from += 2;	}
	else{ *(to++)= *(from++);					}
	}

    *to= '\0';

    len= strlen( tlr->tlrPageScratch );
    wide= appRulerTextWidth( rd, tlr->tlrPageScratch, len );

    *pLen= len; *pWide= wide; return;
    }

static void tedDrawVerticalRuler(	APP_WIDGET		w,
					TedLeftRuler *		tlr,
					DocumentRectangle *	drClip,
					int			oy )
    {
    RulerData *		rd= &(tlr->tlrRulerData);

    int			len;
    int			wide;

    int			x;

    if  ( ! rd->rdVerticalFont.vxfFontStruct )
	{ XDEB(rd->rdVerticalFont.vxfFontStruct); return;	}

    if  ( rd->rdSizeAlong == 0 )
	{
	appDrawGetSizeOfWidget( &wide, &(rd->rdSizeAlong), w );

	rd->rdMaxUsed= rd->rdSizeAlong- rd->rdExtraAfterMaxUsed;
	}

    x= rd->rdSizeAcross/8+ rd->rdVerticalFont.vxfFontStruct->ascent;

    tedRulerFormatPageNumber( tlr, &len, &wide, tlr->tlrTopPage );

    appRulerDrawVerticalString( rd, x, 1+ wide, tlr->tlrPageScratch, len );

    if  ( tlr->tlrBottomPage != tlr->tlrTopPage			&&
	  rd->rdSizeAlong > 4* wide				)
	{
	tedRulerFormatPageNumber( tlr, &len, &wide, tlr->tlrBottomPage );

	appRulerDrawVerticalString( rd,
			    x, rd->rdSizeAlong- 1, tlr->tlrPageScratch, len );
	}
    }

APP_EVENT_HANDLER_H( tedRedrawVerticalRuler, w, voidtlr, event )
    {
    TedLeftRuler *	tlr= (TedLeftRuler *)voidtlr;
    RulerData *		rd= &(tlr->tlrRulerData);
    AppDrawingData *	add= &(rd->rdDrawingData);

    int			ox= 0;
    int			oy= rd->rdVisibleC0- rd->rdMinUsed;

    DocumentRectangle	drClip;

    if  ( ! rd->rdDrawingDataAllocated )
	{
	const int	vertical= 1;

	appRulerAllocateDrawingData( rd, vertical,
			2* rd->rdSizeAcross/ 3- 2, tlr->tlrMagnification, w );
	}

    appCollectExposures( &drClip, add, ox, oy, event );

    tedDrawVerticalRuler( w, tlr, &drClip, oy );
    }
