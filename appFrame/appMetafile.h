/************************************************************************/
/*									*/
/*  Drawing of metafiles (WMF,PICT)					*/
/*									*/
/************************************************************************/

#   include	<appGuiBase.h>

#   include	<bitmap.h>
#   include	<utilDocFont.h>
#   include	<utilPostscriptFace.h>
#   include	<appDraw.h>
#   include	<appImage.h>
#   include	<utilPs.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appMetafileDrawRoundRectX11(	AppDrawingData *	add,
					int			x0,
					int			y0,
					int			x1,
					int			y1,
					int			w,
					int			h,
					int			fill,
					int			borders,
					APP_COLOR_RGB *		fillColor,
					APP_COLOR_RGB *		borderColor );

extern void appMetafilePolygonPathPs(	PrintingState *		ps,
					const APP_POINT *	points,
					int			count,
					const char *		operator );

extern void appMetafileRoundRectPathPs(	PrintingState *		ps,
					int			x0,
					int			y0,
					int			x1,
					int			y1,
					int			w,
					int			h,
					const char *		operator );

extern void appMetafileRectPathPs(	PrintingState *		ps,
					int			x0,
					int			y0,
					int			x1,
					int			y1 );

extern int appMetafileStartPatternFillPs(	PrintingState *		ps,
						const AppBitmapImage *	abi );

