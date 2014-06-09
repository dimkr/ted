/************************************************************************/
/*									*/
/*  Common resources used by Rulers.					*/
/*									*/
/************************************************************************/

#   ifndef	APP_RULER_H
#   define	APP_RULER_H

#   include	<appDraw.h>

typedef struct RulerData
    {
    int				rdSizeAcross;
    int				rdSizeAlong;
    int				rdFontHeight;

    int				rdMinUsed;
    int				rdMaxUsed;
    int				rdExtraAfterMaxUsed;

    const char *		rdFontName;

				/****************************************/
				/*					*/
				/*  Pixel coordinate ranges for the	*/
				/*  ruler.				*/
				/*					*/
				/****************************************/
    int				rdDocumentC0;
    int				rdDocumentC1;
    int				rdVisibleC0;
    int				rdVisibleC1;
    int				rdRulerC1;

    APP_FONT *			rdFontStruct;
    VerticalXFont		rdVerticalFont;
    APP_BITMAP_MASK		rdBackStipple;
    AppDrawingData		rdDrawingData;
    int				rdDrawingDataAllocated;

    int				rdUnit;
    int				rdTicksPerTag;
    int				rdTicksPerText;
    double			rdUnitsPerTick;

    double			rdTwipsPerUnit;
    double			rdMagnifiedPixelsPerTwip;
    } RulerData;

typedef void (*DragHair)( void * voided, int from, int to, int asUpper );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appInitRulerData(		RulerData *	rd );

extern void appCleanRulerData(		RulerData *	rd );

extern void appRulerDrawBackground(	RulerData *	rd,
					int		x,
					int		y,
					int		wide,
					int		high );

extern void appRulerAllocateDrawingData( RulerData *	rd,
					int		vertical,
					int		fontHeight,
					double		magnification,
					APP_WIDGET	w );

extern int appRulerTextWidth(		RulerData *	rd,
					const char *	s,
					int		len );

extern void appRulerDrawVerticalString(	RulerData *	rd,
					int		x,
					int		y,
					char *		s,
					int		len );

extern void appRulerCalculateIncrements( RulerData *	rd,
					double		magnifiedPixelsPerTwip,
					double		magnification );

extern void appRulerTagText(		char *		to,
					int *		pWide,
					int *		pLen,
					RulerData *	rd,
					double		units );

extern void appScrollHorizontalRuler(	RulerData *		rd,
					DocumentRectangle *	drClip,
					int			d );

void appScrollVerticalRuler(		RulerData *		rd,
					DocumentRectangle *	drClip,
					int			d );

#   endif
