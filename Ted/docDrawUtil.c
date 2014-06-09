/************************************************************************/
/*									*/
/*  Drawing functionality: Utility functions.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>

#   include	"docDraw.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialise a DrawingContext						*/
/*									*/
/************************************************************************/

void docInitDrawingContext(	DrawingContext *	dc )
    {
    dc->dcCurrentTextAttributeSet= 0;
    utilInitTextAttribute( &(dc->dcCurrentTextAttribute) );

    dc->dcCurrentColorSet= 0;
    dc->dcCurrentColor.rgb8Red= 0;
    dc->dcCurrentColor.rgb8Green= 0;
    dc->dcCurrentColor.rgb8Blue= 0;
    dc->dcCurrentColor.rgb8Alpha= 0;

    dc->dcDrawingData= (AppDrawingData *)0;
    dc->dcScreenFontList= (ScreenFontList *)0;
    dc->dcDocument= (BufferDocument *)0;
    dc->dcClipRect= (DocumentRectangle *)0;
    dc->dcDocumentSelection= (DocumentSelection *)0;
    dc->dcSelectionGeometry= (SelectionGeometry *)0;
    dc->dcFirstPage= -1;
    dc->dcLastPage= -1;
    dc->dcDrawExternalItems= 0;
    dc->dcPostponeHeadersFooters= 0;
    dc->dcDocHasPageHeaders= 0;
    dc->dcDocHasPageFooters= 0;

    dc->dcDrawTableGrid= 0;

    dc->dcSetColorRgb= (SET_COLOR_RGB)0;
    dc->dcSetFont= (SET_FONT)0;
    dc->dcDrawShape= (DRAW_SHAPE)0;

    dc->dcDrawTextLine= (DRAW_TEXT_LINE)0;
    dc->dcDrawParaTop= (DRAW_PARA_TOP)0;
    dc->dcDrawParaBottom= (DRAW_PARA_BOTTOM)0;
    dc->dcDrawParaLeft= (DRAW_PARA_LEFT)0;
    dc->dcDrawParaRight= (DRAW_PARA_RIGHT)0;
    dc->dcDrawParaShade= (DRAW_PARA_SHADE)0;
    dc->dcDrawCellTop= (DRAW_CELL_TOP)0;
    dc->dcDrawCellBottom= (DRAW_CELL_BOTTOM)0;
    dc->dcDrawCellLeft= (DRAW_CELL_LEFT)0;
    dc->dcDrawCellRight= (DRAW_CELL_RIGHT)0;
    dc->dcDrawCellShade= (DRAW_CELL_SHADE)0;
    dc->dcFinishPage= (FINISH_PAGE)0;
    dc->dcStartPage= (START_PAGE)0;
    dc->dcParaFramePixels= (PARA_SIDES_PIXELS)0;

    dc->dcInitLayoutExternal= (INIT_LAYOUT_EXTERNAL)0;
    dc->dcCloseObject= (DOC_CLOSE_OBJECT)0;

    return;
    }

/************************************************************************/
/*									*/
/*  Cause subsequent drawing to be done in a certain color.		*/
/*									*/
/*  NOTE that background drawing with color= 0 is against the RTF idea	*/
/*	of the default background color: transparent.			*/
/*									*/
/************************************************************************/

void docDrawSetColorRgb(	DrawingContext *	dc,
				void *			through,
				int			r,
				int			g,
				int			b )
    {
    if  ( ! dc->dcCurrentColorSet		||
	  dc->dcCurrentColor.rgb8Red != r	||
	  dc->dcCurrentColor.rgb8Green != g	||
	  dc->dcCurrentColor.rgb8Blue != b	)
	{
	if  ( ! dc->dcSetColorRgb				||
	      (*dc->dcSetColorRgb)( dc, through, r, g, b )	)
	    { LLLDEB(r,g,b); return;	}

	dc->dcCurrentColor.rgb8Red= r;
	dc->dcCurrentColor.rgb8Green= g;
	dc->dcCurrentColor.rgb8Blue= b;
	dc->dcCurrentColorSet= 1;
	}

    return;
    }

void docDrawSetColorNumber(	DrawingContext *	dc,
				void *			through,
				int			colorNumber )
    {
    const BufferDocument *	bd= dc->dcDocument;
    const DocumentProperties *	dp= &(bd->bdProperties);
    RGB8Color			rgb8;

    rgb8.rgb8Red= 0;
    rgb8.rgb8Green= 0;
    rgb8.rgb8Blue= 0;

    if  ( colorNumber != 0 )
	{
	if  ( colorNumber < 1			||
	      colorNumber >= dp->dpColorCount	)
	    { LLDEB(colorNumber,dp->dpColorCount);	}
	else{ rgb8= dp->dpColors[colorNumber];		}
	}

    docDrawSetColorRgb( dc, through,
				rgb8.rgb8Red,
				rgb8.rgb8Green,
				rgb8.rgb8Blue );

    return;
    }

void docDrawSetFont(		DrawingContext *	dc,
				void *			through,
				int			textAttr,
				const TextAttribute *	newTa )
    {
    TextAttribute *	curTa= &(dc->dcCurrentTextAttribute);

    if  ( ! dc->dcCurrentTextAttributeSet				||
	  curTa->taFontNumber != newTa->taFontNumber			||
	  curTa->taFontSizeHalfPoints != newTa->taFontSizeHalfPoints	||
	  curTa->taFontIsBold != newTa->taFontIsBold			||
	  curTa->taFontIsSlanted != newTa->taFontIsSlanted		||
	  curTa->taSmallCaps != newTa->taSmallCaps			||
	  curTa->taSuperSub != newTa->taSuperSub			)
	{
	if  ( ! dc->dcSetFont					||
	      (*dc->dcSetFont)( dc, through, textAttr, newTa )	)
	    { LDEB(1); return;	}

	dc->dcCurrentTextAttributeSet= 1;
	*curTa= *newTa;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Delimit a stretch of text that can be drawn using one string	*/
/*  drawing instruction. For smallcaps, the routine is still usefull	*/
/*  but the stretch will not be drawn in one instruction.		*/
/*									*/
/************************************************************************/

void docDrawDelimitString(		int *			pLen,
					int *			pDrawn,
					TextAttribute *		ta,
					const BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			count,
					int			separate )
    {
    int				drawn;
    int				len;

    const TextParticule *	tp= paraBi->biParaParticules+ part;

    for ( drawn= 1; drawn < count; drawn++ )
	{
	if  ( tp[drawn].tpTextAttributeNumber !=
					    tp->tpTextAttributeNumber	||
	      tp[drawn].tpKind != DOCkindTEXT				)
	    { break;	}

	if  ( paraBi->biParaAlignment == DOCiaJUSTIFIED || separate )
	    { break;	}
	}

    len= tp[drawn-1].tpStroff+ tp[drawn-1].tpStrlen- tp->tpStroff;

    while( len > 0						&&
	   paraBi->biParaString[tp->tpStroff+ len- 1] == ' '	)
	{ len--;	}

    utilGetTextAttributeByNumber( ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

    *pLen= len;
    *pDrawn= drawn;

    return;
    }

