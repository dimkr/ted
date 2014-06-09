/************************************************************************/
/*									*/
/*  PostScript printing.						*/
/*									*/
/************************************************************************/

#   include	<sioGeneral.h>
#   include	"docDraw.h"

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docPsPrintTab(	PrintingState *			ps,
				DrawingContext *		dc,
				int				xShift,
				int				yShift,
				const BufferItem *		bi,
				const TextParticule *		tp,
				ParticuleData *			pd,
				int				baseLine,
				int				lineHeight );

extern void docPsSaveTabLeaderProcedures(	SimpleOutputStream *	sos );

extern void docPsPrintShapeDefineProcsetPs(	SimpleOutputStream *    sos );

extern int docPsPrintDrawDrawingShape(
				const DocumentRectangle *	twipsRect,
				int				page,
				DrawingShape *			ds,
				DrawingContext *		dc,
				void *				vps );

extern int docPsPrintObject(	PrintingState *			ps,
				DrawingContext *		dc,
				int				xShift,
				int				yShift,
				InsertedObject *		io,
				const PostScriptFontList *	psfl,
				ParticuleData *			pd,
				int				baseLine );

extern int docPsPrintShapeImage( PrintingState *		ps,
				DrawingContext *		dc,
				DrawingShape *			ds,
				const PostScriptFontList *	psfl );

