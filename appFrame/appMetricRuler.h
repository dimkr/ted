/************************************************************************/
/*									*/
/*  Definitions relating to rulers.					*/
/*									*/
/************************************************************************/

#   include	<appFrame.h>
#   include	<appRuler.h>
#   include	<appUnit.h>
#   include	<geo2DInteger.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void * appMakeMetricRuler( int		sizeAcross,
				double		magnifiedPixelsPerTwip,
				double		magnification,

				int		minUnused,
				int		maxUnused,

				int		documentC0,
				int		documentC1,
				int		visibleC0,
				int		visibleC1,
				int		rulerC1,

				const char *	fontName,
				int		whatUnit );

extern void appFreeMetricRuler(	void *		voidmr );

extern APP_EVENT_HANDLER_H( appRedrawHorizontalMetricRuler, w, voidmr, event );
extern APP_EVENT_HANDLER_H( appRedrawVerticalMetricRuler, w, voidmr, event );

extern APP_EVENT_HANDLER_H( appHorizontalRulerConfigure, w, voidmr, event );
extern APP_EVENT_HANDLER_H( appVerticalRulerConfigure, w, voidmr, event );

extern void     appHorizontalRulerTrackMouse( int *		pX0pixels,
					int *			pX1pixels,
					int *			pChanged,
					APP_WIDGET		w,
					EditApplication *	ea,
					APP_EVENT *		downEvent,
					void *			voidmr,
					void *			voided,
					DragHair		dragHair );

extern void     appVerticalRulerTrackMouse( int *		pY0pixels,
					int *			pY1pixels,
					int *			pChanged,
					APP_WIDGET		w,
					EditApplication *	ea,
					APP_EVENT *		downEvent,
					void *			voidmr,
					void *			voided,
					DragHair		dragHair );

extern void appScrollHorMetricRuler(	void *			voidmr,
					APP_WIDGET		w,
					int			d	);

extern void appScrollVertMetricRuler(	void *			voidmr,
					APP_WIDGET		w,
					int			d	);

extern void appSetMetricRulerRange(	void *			voidmr,
					APP_WIDGET		w,
					int			docVisV0,
					int			docVisV1,
					int			docBackV1 );

extern void appSetMetricRulerValues(	APP_WIDGET	w,
					void *		voidmr,
					int		v0,
					int		v1 );

extern void appSetMetricRulerMagnification(	APP_WIDGET	w,
					void *		voidmr,
					double		magnifiedPixelsPerTwip,
					double		magnification );

extern void appAdaptMetricRuler(	APP_WIDGET	w,
					void *		voidmr,
					int		documentC1,
					int		rulerC1 );

extern void appDocSetMetricTopRuler(	EditDocument *	ed,
					int		topRulerHeight,
					double		magnification,
					int		unitInt,
					APP_EVENT_HANDLER_T	mouseDown );

extern void appDocSetMetricLeftRuler(	EditDocument *	ed,
					int		topRulerHeight,
					double		magnification,
					int		unitInt,
					APP_EVENT_HANDLER_T	mouseDown );

extern void appHorMetricRulerMoveHair(	void *		voided,
					int		from,
					int		to,
					int		asUpper	);

extern void appVertMetricRulerMoveHair(	void *		voided,
					int		from,
					int		to,
					int		asUpper	);

extern void appDrawRectangleSelection(	DocumentRectangle *	drClip,
					DocumentRectangle *	drSelected,
					AppDrawingData *	add,
					AppColors *		ac,
					int			ox,
					int			oy );

extern void appRemoveRectangleSelection( DocumentRectangle *	drVisible,
					DocumentRectangle *	drSelected,
					AppDrawingData *	add );

extern void appSetHorRectangleSelection( DocumentRectangle *	drVisible,
					DocumentRectangle *	drSelected,
					int			x0Screen,
					int			x1Screen,
					AppDrawingData *	add );

extern void appSetVertRectangleSelection( DocumentRectangle *	drVisible,
					DocumentRectangle *	drSelected,
					int			y0Screen,
					int			y1Screen,
					AppDrawingData *	add );

