/************************************************************************/
/*									*/
/*  Common resources used by Rulers.					*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdio.h>
#   include	<string.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<appUnit.h>
#   include	<appRuler.h>

#   include	<appDebugon.h>

#   ifndef	M_LN10
#	define	M_LN10	2.30258509299404568402
#   endif

void appInitRulerData(	RulerData *	rd )
    {
    rd->rdFontName= (const char *)0;

    rd->rdSizeAcross= 0;
    rd->rdSizeAlong= 0;
    rd->rdFontHeight= 0;

    rd->rdMinUsed= 0;
    rd->rdMaxUsed= 0;
    rd->rdExtraAfterMaxUsed= 0;

    rd->rdDocumentC0= 0;
    rd->rdDocumentC1= 0;
    rd->rdVisibleC0= 0;
    rd->rdVisibleC1= 0;
    rd->rdRulerC1= 0;

    rd->rdFontStruct= (APP_FONT *)0;
    appInitVerticalXFont( &(rd->rdVerticalFont) );
    appInitDrawingData( &(rd->rdDrawingData) );
    rd->rdDrawingDataAllocated= 0;
#   ifdef USE_MOTIF
    rd->rdBackStipple= None;
#   endif

    return;
    }

void appCleanRulerData(	RulerData *	rd )
    {
    if  ( rd->rdFontStruct )
	{ appDrawFreeFont( &(rd->rdDrawingData), rd->rdFontStruct );	}

    appCleanVerticalXFont( &(rd->rdVerticalFont), &(rd->rdDrawingData) );

#   ifdef USE_MOTIF
    if  ( rd->rdBackStipple )
	{
	XFreePixmap( rd->rdDrawingData.addDisplay, rd->rdBackStipple );
	}
#   endif

    appCleanDrawingData( &(rd->rdDrawingData) );

    return;
    }

void appRulerDrawBackground(	RulerData *	rd,
				int		x,
				int		y,
				int		wide,
				int		high )
    {
    AppDrawingData *	add= &(rd->rdDrawingData);

    appDrawSetForegroundColor( add, &(add->addBackColor) );

#   ifdef USE_MOTIF
    if  ( rd->rdBackStipple )
	{ XSetFillStyle( add->addDisplay, add->addGc, FillStippled ); }
#   endif

    appDrawFillRectangle( add, x, y, wide, high );

#   ifdef USE_MOTIF
    if  ( rd->rdBackStipple )
	{ XSetFillStyle( add->addDisplay, add->addGc, FillSolid ); }
#   endif

    return;
    }

void appRulerAllocateDrawingData(	RulerData *		rd,
					int			vertical,
					int			fontHeight,
					double			magnification,
					APP_WIDGET		w )
    {
    AppDrawingData *	add= &(rd->rdDrawingData);
    char		name[128];

    rd->rdFontHeight= fontHeight;

    if  ( appSetDrawingDataForWidget( w, magnification, add ) )
	{ LDEB(1);	}

    appDrawSetLineAttributes( add,
			1, LINEstyleSOLID, LINEcapPROJECTING, LINEjoinMITER,
			(const unsigned char *)0, 0 );

    if  ( rd->rdFontName )
	{
	sprintf( name, rd->rdFontName, fontHeight );

	rd->rdFontStruct= appDrawOpenFont( add, name );
	}
    else{ strcpy( name, "(None)" );	}

    if  ( ! rd->rdFontStruct )
	{
	SXDEB(name,rd->rdFontStruct);
	sprintf( name, "-*-*-medium-r-*-*-%d-*-*-*-m-*-iso8859-*",
							    fontHeight );

	rd->rdFontStruct= appDrawOpenFont( add, name );
	}

    if  ( vertical							&&
	  rd->rdFontStruct						&&
	  appOpenVerticalXFont( &(rd->rdVerticalFont), add, name, -1,
						&(add->addBackColor) )	)
	{ SDEB(name);	}

    if  ( rd->rdFontStruct )
	{ appDrawSetFont( add, rd->rdFontStruct );	}
    else{ SXDEB(name,rd->rdFontStruct);			}

#   ifdef USE_MOTIF
    if  ( DefaultDepth( add->addDisplay, add->addScreen ) <= 2 )
	{
	int		i;

	for ( i= 0; i < 16; i++ )
	    {
	    memset( name+ 8* i+ 0, 0xaa, 4 );
	    memset( name+ 8* i+ 4, 0x55, 4 );
	    }

	rd->rdBackStipple= XCreateBitmapFromData( add->addDisplay,
					    add->addDrawable, name, 32, 32 );
	if  ( ! rd->rdBackStipple )
	    { XDEB(rd->rdBackStipple);	}

	appDrawBlackColor( add, &(add->addBackColor) );

	XSetStipple( add->addDisplay, add->addGc, rd->rdBackStipple );
	}
#   endif

    rd->rdDrawingDataAllocated= 1;

    return;
    }

int appRulerTextWidth(	RulerData *	rd,
			const char *	s,
			int		len )
    {
    if  ( rd->rdFontStruct )
	{
	AppDrawingData *	add= &(rd->rdDrawingData);

	int			wide;
	int			fontAscent;
	int			fontDescent;

	appDrawTextExtents( &wide, &fontAscent, &fontDescent, add,
						rd->rdFontStruct, s, len );

	return  wide;
	}
    else{ return  (6*rd->rdFontHeight*len)/10;			}
    }

void appRulerDrawVerticalString(	RulerData *	rd,
					int		x,
					int		y,
					char *		s,
					int		len )
    {
    AppDrawingData *	add= &(rd->rdDrawingData);

    appDrawVerticalString( &(rd->rdVerticalFont), add, x, y, s, len );

    return;
    }

/************************************************************************/
/*									*/
/*  Calculate suitable distances for Texts, tags, ticks			*/
/*									*/
/*  unitsPerTick= unitsPerTag/ticksPerTag;				*/
/*  screenUnitsPerTick= unitsPerTick* magnification.			*/
/*  pixelsPerTick= FAC* screenUnitsPerTick.				*/
/*									*/
/************************************************************************/

static double appRulerPerItem(	double		unitsPerItem )
    {
    double aa;
    double bb;

    aa= log( unitsPerItem )/ M_LN10;
    bb= (int)( aa+ 20 )- 20;

    if  ( aa- bb > log( 5.0 )/ M_LN10	)
	{ aa= bb+ 1;	}
    else{
	if  ( aa- bb > log( 2.0 )/ M_LN10	)
	    { aa= bb+ log( 5.0 )/ M_LN10;	}
	else{
	    if  ( aa- bb > 0.0			)
		{ aa= bb+ log( 2.0 )/ M_LN10;	}
	    }
	}

    return exp( M_LN10* aa );
    }

void appRulerCalculateIncrements(	RulerData *	rd,
					double		magnifiedPixelsPerTwip,
					double		magnification )
    {
    double		mmPerUnit;

    double		unitsPerText;
    double		unitsPerTag;
    double		unitsPerTick;

    int			tagsPerText;

    switch( rd->rdUnit )
	{
	case UNITtyMM:		mmPerUnit= 1.0;		break;
	case UNITtyCM:		mmPerUnit= 10.0;	break;
	case UNITtyINCH:	mmPerUnit= 24.4;	break;
	case UNITtyPOINTS:	mmPerUnit= 25.4/ 72.0;	break;
	case UNITtyPICAS:	mmPerUnit= 25.4/ 6.0;	break;
	default:
	    LDEB(rd->rdUnit);
	    rd->rdUnit= UNITtyMM;
	    mmPerUnit= 1.0;
	    break;
	}

    rd->rdTwipsPerUnit= appUnitToTwips( 1.0, rd->rdUnit );
    rd->rdMagnifiedPixelsPerTwip= magnifiedPixelsPerTwip;

    unitsPerText= appRulerPerItem( ( 30.0/ mmPerUnit )/ magnification );
    unitsPerTag=  appRulerPerItem( (  8.0/ mmPerUnit )/ magnification );
    unitsPerTick= appRulerPerItem( (  3.0/ mmPerUnit )/ magnification );

    tagsPerText= unitsPerText/ unitsPerTag+ 0.49;
    rd->rdTicksPerTag= unitsPerTag/ unitsPerTick+ 0.49;

    rd->rdTicksPerText= tagsPerText* rd->rdTicksPerTag;
    rd->rdUnitsPerTick= unitsPerText/ rd->rdTicksPerText;

    return;
    }

void appRulerTagText(	char *		to,
			int *		pWide,
			int *		pLen,
			RulerData *	rd,
			double		units )
    {
    char	scratch[30];
    char *	s;

    int		wide;
    int		len;

    sprintf ( scratch, "%12.3g", units );
    s= scratch; while( *s == ' ' ) { s++;	}
    len= strlen( s );

    wide= appRulerTextWidth( rd, s, len );

    strcpy( to, s ); *pWide= wide; *pLen= len;

    return;
    }

void appScrollHorizontalRuler(		RulerData *		rd,
					DocumentRectangle *	drClip,
					int			d )
    {
    int				wide= rd->rdMaxUsed+ 1;
    int				high= rd->rdSizeAcross;

    AppDrawingData *		add= &(rd->rdDrawingData);

    int				x0;

    rd->rdVisibleC0 += d;
    rd->rdVisibleC1 += d;

    x0= rd->rdMinUsed;

    drClip->drY0= 0;
    drClip->drY1= high;

    if  ( d > 0 )
	{
	if  ( wide > d )
	    {
	    appDrawMoveArea( add, d+ x0, 0, wide- d- x0, high, x0, 0 );

	    drClip->drX0= rd->rdVisibleC1- d;
	    drClip->drX1= rd->rdVisibleC1;
	    }
	else{
	    drClip->drX0= rd->rdVisibleC0;
	    drClip->drX1= rd->rdVisibleC1;
	    }
	}
    else{
	if  ( high+ d > 0 )
	    {
	    appDrawMoveArea( add, x0, 0, wide+ d- x0, high, x0- d, 0 );

	    drClip->drX0= rd->rdVisibleC0;
	    drClip->drX1= rd->rdVisibleC0- d;
	    }
	else{
	    drClip->drX0= rd->rdVisibleC0;
	    drClip->drX1= rd->rdVisibleC1;
	    }
	}

    return;
    }

void appScrollVerticalRuler(		RulerData *		rd,
					DocumentRectangle *	drClip,
					int			d )
    {
    int				high= rd->rdMaxUsed+ 1;
    int				wide= rd->rdSizeAcross;

    AppDrawingData *		add= &(rd->rdDrawingData);

    int				y0;

    rd->rdVisibleC0 += d;
    rd->rdVisibleC1 += d;

    y0= rd->rdMinUsed;

    drClip->drX0= 0;
    drClip->drX1= wide;

    if  ( d > 0 )
	{
	if  ( high > d )
	    {
	    appDrawMoveArea( add, 0, d+ y0, wide, high- d- y0, 0, y0 );

	    drClip->drY0= rd->rdVisibleC1- d;
	    drClip->drY1= rd->rdVisibleC1;
	    }
	else{
	    drClip->drY0= rd->rdVisibleC0;
	    drClip->drY1= rd->rdVisibleC1;
	    }
	}
    else{
	if  ( high+ d > 0 )
	    {
	    appDrawMoveArea( add, 0, y0, wide, high+ d- y0, 0, y0- d );

	    drClip->drY0= rd->rdVisibleC0;
	    drClip->drY1= rd->rdVisibleC0- d;
	    }
	else{
	    drClip->drY0= rd->rdVisibleC0;
	    drClip->drY1= rd->rdVisibleC1;
	    }
	}

    return;
    }
