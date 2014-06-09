/************************************************************************/
/*									*/
/*  Definitions relating to rulers.					*/
/*									*/
/************************************************************************/

#   include	<appUnit.h>

#   include	<appFrame.h>
#   include	<appRuler.h>

#   include	"docBuf.h"

typedef struct ColumnSeparator
    {
    int		csX0;
    int		csX1;
    } ColumnSeparator;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void * tedMakeTopRuler(	int			height,
				double			magnifiedPixelsPerTwip,
				double			magnification,

				int			leftRulerWidth,
				int			rightRulerWidth,
				int			docX0,
				int			docX1,

				int			minVisible,
				int			maxVisible,
				int			rulerC1,

				const char *		rulerFont,
				int			whatUnit );

extern void * tedMakeLeftRuler(	int			width,
				double			magnifiedPixelsPerTwip,
				double			magnification,

				int			minUnused,
				int			maxUnused,
				int			docY0,
				int			docY1,

				int			docVisY0,
				int			docVisY1,

				int			rulerC1,
				const char *		rulerFont,
				int			whatUnit,
				int			pageStep,
				const char *		tlrPageFormat );

extern void tedFreeHorizontalRuler(	void *		voidttr );
extern void tedFreeVerticalRuler(	void *		voidtlr );

extern APP_EVENT_HANDLER_H( tedRedrawHorizontalRuler, w, voidttr, event );
extern APP_EVENT_HANDLER_H( tedRedrawVerticalRuler, w, voidtlr, event );

extern void tedScrollHorizontalRuler(	void *			voidttr,
					APP_WIDGET		w,
					int			d	);

extern void tedSetHorizontalRulerRange(	void *		voidttr,
					APP_WIDGET	w,
					int		docVisX0,
					int		docVisX1,
					int		docBackX1 );

extern void tedAdaptHorizontalRuler(	void *			voidttr,
					APP_WIDGET		w,
					int			docX0,
					int			docX1,
					int			firstIndent,
					int			leftIndent,
					int			rightIndent,
					int			rulerC1,
					const TabStopList *	tslSet );

extern void tedHorizontalRulerTrackMouse( int *			pFirstIndent,
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
					DragHair		dragHair );

extern APP_EVENT_HANDLER_H( tedHorizontalRulerConfigure, w, voidttr, event );
extern APP_EVENT_HANDLER_H( tedVerticalRulerConfigure, w, voidtlr, event );

extern int tedSetRulerColumns(	APP_WIDGET		w,
				void *			voidttr,
				int			colX0,
				int			colX1,
				ColumnSeparator *	cs,
				int			csCount );

extern void tedSetLeftRulerRange(	void *			voidtr,
					APP_WIDGET		w,
					int			docVisV0,
					int			docVisV1,
					int			docBackV1 );

extern void tedScrollLeftRuler(		void *			voidtr,
					APP_WIDGET		w,
					int			d );

extern void tedAdaptLeftRuler(		void *		voidtlr,
					APP_WIDGET	w,
					int		visisbleC0,
					int		visisbleC1,
					int		pageStep );
