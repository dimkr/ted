/************************************************************************/
/*									*/
/*  Primitives for the page on a page layout tool etc.			*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<appDebugon.h>

#   include	<appFrame.h>
#   include	"appUnit.h"
#   include	<appGeoString.h>
#   include	<utilPs.h>

/************************************************************************/
/*									*/
/*  Draw a text like pattern between some margins.			*/
/*									*/
/************************************************************************/

static void appDrawTextPattern(	AppDrawingData *		add,
				int				numX1,
				int				numY1,
				int				textX0,
				int				textX1,
				int				textY0,
				int				textY1 )
    {
    int			y;

    y= textY0+ 2;
    while( y < textY1- 1 )
	{
	int	x;
	int	c;

	if  ( y <= numY1 )
	    { x= numX1+ 1;	}
	else{ x= textX0+ 1;	}

	c= (int)( 5+ (75.0* rand() )/ (RAND_MAX+ 1.0 ) );

	while( c > 0 )
	    {
	    int	l;

	    l= (int) ( (1.0* (textX1- textX0)* rand() )/ (RAND_MAX+ 1.0 ) );
	    l= 2+ l/10;

	    if  ( x+ l >= textX1- 1 )
		{
		y += 2;

		if  ( y <= numY1 )
		    { x= numX1+ 1;	}
		else{ x= textX0+ 1;	}

		if  ( y >= textY1- 1 )
		    { break;	}
		}

	    appDrawDrawRectangle( add, x, y, l, 0 );

	    x += l+ 2; c--;
	    }

	y += 4;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the margins of a page.						*/
/*									*/
/************************************************************************/

static void appDrawTextPage(	AppDrawingData *		add,
				int				textX0,
				int				textX1,
				int				textY0,
				int				textY1 )
    {
    appDrawSetForegroundColor( add, &(add->addBackColor) );
    appDrawDrawRectangle( add,
	    textX0- 1, textY0- 1, textX1- textX0+ 1, textY1- textY0+ 1 );

    appDrawSetForegroundBlack( add );
    appDrawTextPattern( add, textX0, textY0,
				    textX0, textX1, textY0, textY1 );

    return;
    }

static void appDrawNupTextPage(	AppDrawingData *		add,
				APP_FONT *			xfs,
				const char *			label,
				int				textX0,
				int				textX1,
				int				textY0,
				int				textY1 )
    {
    if  ( xfs )
	{
	int				numWide;
	int				numHigh;

	int				l= strlen( label );
	int				fontAscent;
	int				fontDescent;

	appDrawTextExtents( &numWide, &fontAscent, &fontDescent,
						    add, xfs, label, l );

	numHigh= fontAscent+ fontDescent;

	appDrawSetForegroundColor( add, &(add->addBackColor) );
	appDrawDrawRectangle( add,
		textX0- 1, textY0- 1, textX1- textX0+ 1, textY1- textY0+ 1 );

	appDrawSetForegroundBlack( add );


	appDrawDrawString( add, textX0+ 2, textY0+ fontAscent+ 3,
								label, l );

	appDrawTextPattern( add, textX0+ numWide+ 2, textY0+ numHigh+ 2,
				    textX0, textX1, textY0, textY1 );
	}
    else{
	appDrawTextPage( add, textX0, textX1, textY0, textY1 );

	}
    }

/************************************************************************/
/*									*/
/*  Draw the plane of the sheet of paper.				*/
/*									*/
/************************************************************************/

static void appDrawPageSheet(	int				sheetX0,
				int				sheetX1,
				int				sheetY0,
				int				sheetY1,
				AppDrawingData *		add )
    {
    appDrawSetForegroundWhite( add );
    appDrawFillRectangle( add, sheetX0, sheetY0,
					sheetX1- sheetX0, sheetY1- sheetY0 );

    appDrawSetForegroundBlack( add );
    appDrawDrawRectangle( add, sheetX0- 1, sheetY0- 1,
					sheetX1- sheetX0, sheetY1- sheetY0 );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a schematic view of a page.					*/
/*									*/
/*  1)  Perform calculations relative to the centre, to avoid assymetry	*/
/*	from rounding when margins are identical.			*/
/*									*/
/************************************************************************/

void appDrawPageDiagram(	APP_WIDGET			w,
				AppDrawingData *		add,
				double				widgetHighMm,
				const DocumentGeometry *	dg )
    {
    int				wide;
    int				high;
    double			fac;

    int				sheetX0;
    int				sheetX1;
    int				sheetY0;
    int				sheetY1;

    int				sheetWidePixels;
    int				sheetHighPixels;

    int				textX0;
    int				textX1;
    int				textY0;
    int				textY1;

    appDrawGetSizeOfWidget( &wide, &high, w );

    appDrawSetForegroundColor( add, &(add->addBackColor) );
    appDrawFillRectangle( add, 0, 0, wide, high );

    fac= ( 2.54* high )/( widgetHighMm* 20.0* 72.27 );

    sheetWidePixels= fac* dg->dgPageWideTwips;
    sheetHighPixels= fac* dg->dgPageHighTwips;

    sheetX0= wide/2- ( sheetWidePixels/ 2 );
    sheetY0= high/2- ( sheetHighPixels/ 2 );
    sheetX1= wide/2+ ( sheetWidePixels/ 2 );
    sheetY1= high/2+ ( sheetHighPixels/ 2 );

    appDrawPageSheet( sheetX0, sheetX1, sheetY0, sheetY1, add );

    /*  1  */
    textX0= wide/2- fac* ( dg->dgPageWideTwips/2- dg->dgLeftMarginTwips );
    textX1= wide/2+ fac* ( dg->dgPageWideTwips/2- dg->dgRightMarginTwips );
    textY0= high/2- fac* ( dg->dgPageHighTwips/2- dg->dgTopMarginTwips );
    textY1= high/2+ fac* ( dg->dgPageHighTwips/2- dg->dgBottomMarginTwips );

    appDrawTextPage( add, textX0, textX1, textY0, textY1 );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw an arrow to show the direction of the paper through the	*/
/*  printer.								*/
/*									*/
/************************************************************************/

#   define	HEADW	5
#   define	HEADL	5
#   define	STEMW	3
#   define	STEML	3

#   define	ARROWD	16

static void appDrawDirectionArrow(	AppDrawingData *	add,
					int			rotate,
					int			x0,
					int			y0 )
    {
    static APP_POINT	offsets[8]=
	{
	    { -STEMW,	+STEML,	},
	    { +STEMW,	+STEML,	},
	    { +STEMW,	0,	},
	    { +HEADW,	0,	},
	    { 0,	-HEADL,	},
	    { -HEADW,	0,	},
	    { -STEMW,	0,	},
	    { -STEMW,	+STEML,	},
	};

    APP_POINT	points[8];
    int		i;

    if  ( rotate )
	{
	for ( i= 0; i < 8; i++ )
	    {
	    points[i].x= x0- offsets[i].y;
	    points[i].y= y0- offsets[i].x;
	    }
	}
    else{
	for ( i= 0; i < 8; i++ )
	    {
	    points[i].x= x0+ offsets[i].x;
	    points[i].y= y0+ offsets[i].y;
	    }
	}

    appDrawSetForegroundWhite( add );

    appDrawFillPolygon( add, points, 8 );

    appDrawSetForegroundBlack( add );

    appDrawDrawLines( add, points, 8 );
    }

static void appDrawDirectionArrows(	AppDrawingData *	add,
					int			rotate,
					int			sheetX0,
					int			sheetX1,
					int			sheetY0,
					int			sheetY1 )
    {
    if  ( rotate )
	{
	appDrawDirectionArrow( add, rotate,
				( sheetX0+ sheetX1 )/ 2, sheetY0- ARROWD );
	appDrawDirectionArrow( add, rotate,
				( sheetX0+ sheetX1 )/ 2, sheetY1+ ARROWD );
	}
    else{
	appDrawDirectionArrow( add, rotate,
				sheetX0- ARROWD, ( sheetY0+ sheetY1 )/ 2 );
	appDrawDirectionArrow( add, rotate,
				sheetX1+ ARROWD, ( sheetY0+ sheetY1 )/ 2 );
	}
    }

/************************************************************************/
/*									*/
/*  Draw a schematic view of the distribution of pages on a sheet of	*/
/*  paper.								*/
/*									*/
/*  1)  When no scaling or moving is required: Use appDrawPageDiagram()	*/
/*	saves work, and is more focused at looking nice.		*/
/*  2)  The vertical orientation of X11 drawing is the opposite of the	*/
/*	mathematical convention that is used in PostScript. So swap	*/
/*	coordinates.							*/
/*  3)  As we rotate the sheet rather than the document pages, swap	*/
/*	coordinates.							*/
/*									*/
/************************************************************************/

void appDrawNupDiagram(	APP_WIDGET			w,
			AppDrawingData *		add,
			APP_FONT *			xfs,
			double				widgetHighMm,
			const DocumentGeometry *	dgPage,
			const PrintGeometry *		pg )
    {
    int				wide= 0;
    int				high= 0;
    double			fac;

    int				sheetX0;
    int				sheetX1;
    int				sheetY0;
    int				sheetY1;

    int				sheetWidePixels;
    int				sheetHighPixels;

    int				x0;
    int				x1;
    int				y0;
    int				y1;

    int				textX0;
    int				textX1;
    int				textY0;
    int				textY1;

    int				rotatePages;
    int				rotateSheet;

    int				textWideTwips;
    int				textHighTwips;

    const DocumentGeometry *	dgSheet= &(pg->pgSheetGeometry);

    NupSchema			ns;
    AffineTransform2D		atPage;

    utilInitNupSchema( &ns );

    /*  1  */
    if  ( pg->pgGridRows* pg->pgGridCols == 1			&&
	  dgSheet->dgPageWideTwips == dgPage->dgPageWideTwips	&&
	  dgSheet->dgPageHighTwips == dgPage->dgPageHighTwips	)
	{
	appDrawPageDiagram( w, add, widgetHighMm, dgPage );
	goto ready;
	}

    appDrawGetSizeOfWidget( &wide, &high, w );

    appDrawSetForegroundColor( add, &(add->addBackColor) );
    appDrawFillRectangle( add, 0, 0, wide, high );

    fac= ( 2.54* high )/( widgetHighMm* 20.0* 72.27 );

    sheetWidePixels= fac* dgSheet->dgPageWideTwips+ 0.49;
    sheetHighPixels= fac* dgSheet->dgPageHighTwips+ 0.49;

    if  ( utilNupGetBaseTranform( &atPage, &rotatePages, pg, dgPage, fac ) )
	{ LDEB(1); goto ready;	}

    rotateSheet= rotatePages;
    if  ( utilNupSetSchema( &ns, rotateSheet, &atPage, pg, fac, dgPage ) )
	{ LLDEB(pg->pgGridRows,pg->pgGridCols); goto ready;	}

    if  ( rotateSheet )
	{
	sheetX0= wide/2- ( sheetHighPixels/ 2 );
	sheetY0= high/2- ( sheetWidePixels/ 2 );
	sheetX1= wide/2+ ( sheetHighPixels/ 2 );
	sheetY1= high/2+ ( sheetWidePixels/ 2 );
	}
    else{
	sheetX0= wide/2- ( sheetWidePixels/ 2 );
	sheetY0= high/2- ( sheetHighPixels/ 2 );
	sheetX1= wide/2+ ( sheetWidePixels/ 2 );
	sheetY1= high/2+ ( sheetHighPixels/ 2 );
	}

    appDrawPageSheet( sheetX0, sheetX1, sheetY0, sheetY1, add );

    textWideTwips= dgPage->dgPageWideTwips-
				dgPage->dgLeftMarginTwips-
				dgPage->dgRightMarginTwips;
    textHighTwips= dgPage->dgPageHighTwips-
				dgPage->dgTopMarginTwips-
				dgPage->dgBottomMarginTwips;

    if  ( pg->pgGridRows* pg->pgGridCols == 1 )
	{
	utilNupGetPageTranform( &atPage, &ns, 0 );

	x0= AT2_X( dgPage->dgLeftMarginTwips,
				    dgPage->dgTopMarginTwips, &atPage )+ 0.49;
	y0= AT2_Y( dgPage->dgLeftMarginTwips,
				    dgPage->dgTopMarginTwips, &atPage )+ 0.49;

	x1= AT2_X( dgPage->dgPageWideTwips- dgPage->dgRightMarginTwips,
			dgPage->dgPageHighTwips- dgPage->dgBottomMarginTwips,
			&atPage )+ 0.49;
	y1= AT2_Y( dgPage->dgPageWideTwips- dgPage->dgRightMarginTwips,
			dgPage->dgPageHighTwips- dgPage->dgBottomMarginTwips,
			&atPage )+ 0.49;

	if  ( rotatePages )
	    {
	    /*  3  */
	    textX0= sheetX0+ y0;
	    textX1= sheetX0+ y1;
	    textY0= sheetY0+ x0;
	    textY1= sheetY0+ x1;
	    }
	else{
	    textX0= sheetX0+ x0+ 1;
	    textX1= sheetX0+ x1;
	    /*  2  */
	    textY0= sheetY1- y0;
	    textY1= sheetY1- y1;
	    }

	appDrawTextPage( add, textX0, textX1, textY0, textY1 );
	}
    else{
	int		i;
	char		scratch[20];

	for ( i= 0; i < pg->pgGridRows* pg->pgGridCols; i++ )
	    {
	    utilNupGetPageTranform( &atPage, &ns, i );

	    sprintf( scratch, "%d", i+ 1 );

	    x0= AT2_X( dgPage->dgLeftMarginTwips,
				    dgPage->dgTopMarginTwips, &atPage )+ 0.49;
	    y0= AT2_Y( dgPage->dgLeftMarginTwips,
				    dgPage->dgTopMarginTwips, &atPage )+ 0.49;

	    x1= AT2_X( dgPage->dgPageWideTwips- dgPage->dgRightMarginTwips,
			dgPage->dgPageHighTwips- dgPage->dgBottomMarginTwips,
			&atPage )+ 0.49;
	    y1= AT2_Y( dgPage->dgPageWideTwips- dgPage->dgRightMarginTwips,
			dgPage->dgPageHighTwips- dgPage->dgBottomMarginTwips,
			&atPage )+ 0.49;

	    if  ( rotatePages )
		{
		/*  3  */
		textX0= sheetX0+ y0;
		textX1= sheetX0+ y1;
		textY0= sheetY0+ x0;
		textY1= sheetY0+ x1;
		}
	    else{
		textX0= sheetX0+ x0+ 1;
		textX1= sheetX0+ x1;
		/*  2  */
		textY0= sheetY1- y0;
		textY1= sheetY1- y1;
		}

	    appDrawNupTextPage( add, xfs,
			    scratch, textX0, textX1, textY0, textY1 );
	    }
	}

    appDrawDirectionArrows( add, rotateSheet,
			    sheetX0, sheetX1, sheetY0, sheetY1 );

  ready:

    utilCleanNupSchema( &ns );

    return;
    }

/************************************************************************/
/*									*/
/*  Make the location for the schematic drawing of the page.		*/
/*									*/
/************************************************************************/

APP_WIDGET appMakePageDrawing(	APP_WIDGET			parent,
				EditApplication *		ea,
				int				mmHigh,
				APP_EVENT_HANDLER_T		redraw,
				void *				through )
    {
    APP_WIDGET	drawing;

    double	horPixPerMM;
    double	verPixPerMM;
    double	xfac;
    double	yfac;

    const int	wide= -1;
    const int	heightResizable= 1;
    int		high;

    appGetFactors( ea, &horPixPerMM, &verPixPerMM, &xfac, &yfac );

    high= ( mmHigh* verPixPerMM )+ 0.5;

    appGuiMakeDrawingAreaInColumn( &drawing, parent,
				wide, high, heightResizable, redraw, through );

    return drawing;
    }

