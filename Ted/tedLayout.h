/************************************************************************/
/*									*/
/*  Header file for geometry/formatting/drawing related functionality	*/
/*									*/
/************************************************************************/

#   ifndef	TED_LAYOUT_H
#   define	TED_LAYOUT_H

#   include	"docBuf.h"
#   include	"docLayout.h"
#   include	"docScreenFontList.h"

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedTextWidth(		const DrawScreenFont *		apf,
					const TextAttribute *		ta,
					const BufferDocument *		bd,
					const AppDrawingData *		add,
					const unsigned char *		s,
					int				len );

extern void tedParagraphFramePixels( 	ParagraphFrame *	pf,
					const AppDrawingData *	add,
					const BufferItem *	bi );

extern int tedCheckPageOfSelectedExtItem(
				    int *			pChanged,
				    DocumentRectangle *		drChanged,
				    BufferDocument *		bd,
				    ExternalItem *		selRootEi,
				    AppDrawingData *		add,
				    ScreenFontList *		sfl );

extern int tedOpenScreenFont(	
			BufferDocument *		bd,
			AppDrawingData *		add,
			ScreenFontList *		sfl,
			int				attributeNumber );

extern int tedInitLayoutExternalItem(
				LayoutJob *			lj,
				ExternalItem *			ei,
				int				page );

#   endif	/*  TED_LAYOUT_H	*/
