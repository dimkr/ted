/************************************************************************/
/*									*/
/*  Ted, Screen drawing and forcing drawing through			*/
/*  appExposeRectangle().						*/
/*									*/
/************************************************************************/

#   ifndef	TED_DRAW_H
#   define	TED_DRAW_H

#   include	"docDraw.h"

#   include	<appDebugon.h>

# define DRDEB(dr) APP_DEB(appDebug( "%s(%3d) %s=[%4d..%4d,%4d..%4d]\n", \
			     __FILE__, __LINE__, #dr, \
			     (dr)->drX0, (dr)->drX1, (dr)->drY0, (dr)->drY1 ))

#   define	LOG_REDRAWS	0
#   define	BLINK_IBAR	1

#   if		LOG_REDRAWS
#	define	BLINK_IBAR	0
#   endif

typedef struct ScreenDrawingData
    {
    RGB8Color			sddForeColor;
    RGB8Color			sddBackColor;
    RGB8Color			sddGridColor;
    RGB8Color			sddPageColor;

    int				sddOx;
    int				sddOy;
    AppColors *			sddColors;
    const ScreenFontList *	sddScreenFontList;

    ExternalItem *		sddSelRootEi;
    BufferItem *		sddSelRootBi;
    BufferItem *		sddSelRootBodySectBi;
    int				sddBoxFirstPage;
    int				sddBoxLastPage;
    } ScreenDrawingData;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedDrawDrawingShape(	const DocumentRectangle *	twipsRect,
				int				page,
				DrawingShape *			ds,
				DrawingContext *		dc,
				void *				vsdd );

extern int tedDrawTextReverse(	const BufferItem *		bi,
				int				line,
				const ParagraphFrame *		pf,
				void *				vsdd,
				DrawingContext *		dc,
				int				pShift,
				int				xShift,
				int				yShift );

extern int tedDrawTextSelected(	const BufferItem *		bi,
				int				line,
				const ParagraphFrame *		pf,
				void *				vsdd,
				DrawingContext *		dc,
				int				pShift,
				int				xShift,
				int				yShift );

extern int tedDrawTextLine(	const BufferItem *		bi,
				int				line,
				const ParagraphFrame *		pf,
				void *				vsdd,
				DrawingContext *		dc,
				int				pShift,
				int				xShift,
				int				yShift );

#   endif	/*  TED_DRAW_H	*/
