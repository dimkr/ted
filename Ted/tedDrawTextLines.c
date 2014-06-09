/************************************************************************/
/*									*/
/*  Ted, Screen drawing and forcing drawing through			*/
/*  appExposeRectangle().						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"tedApp.h"
#   include	"tedLayout.h"
#   include	"tedDraw.h"

#   include	<appDebugon.h>

# define DRDEB(dr) APP_DEB(appDebug( "%s(%3d) %s=[%4d..%4d,%4d..%4d]\n", \
			     __FILE__, __LINE__, #dr, \
			     (dr)->drX0, (dr)->drX1, (dr)->drY0, (dr)->drY1 ))

/************************************************************************/
/*									*/
/*  Draw a text item.							*/
/*									*/
/*  1)  Tabs need not to be drawn.					*/
/*									*/
/************************************************************************/

static int tedDrawTab(	const BufferItem *	bi,
			const TextParticule *	tp,
			int			baseLine,
			ScreenDrawingData *	sdd,
			DrawingContext *	dc )
    {
    AppDrawingData *		add= dc->dcDrawingData;
    const DrawScreenFontList *	apfl= &(add->addScreenFontList);
    const BufferDocument *	bd= dc->dcDocument;
    const TabStopList *		tsl= &(bi->biParaTabStopList);
    const TabStop *		ts= tsl->tslTabStops+ tp->tpObjectNumber;
    const ScreenFontList *	sfl= sdd->sddScreenFontList;

    int				x0;
    int				x1;

    int				textAttr;
    int				screenFont;
    DrawScreenFont *		dsf;
    TextAttribute		ta;

    textAttr= tp->tpTextAttributeNumber;

    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList), textAttr );

    switch( ts->tsLeader )
	{
	case DOCtlNONE:
	    break;

	case DOCtlDOTS:

	    if  ( textAttr >= sfl->sflAttributeToScreenCount	||
		  sfl->sflAttributeToScreen[textAttr] < 0	)
		{ LDEB(textAttr); return -1;	}

	    screenFont= sfl->sflAttributeToScreen[textAttr];
	    dsf= apfl->apflFonts+ screenFont;

	    x0= tp->tpX0+ dsf->apfFullSizePixels/ 4;
	    x1= tp->tpX0+ tp->tpPixelsWide- dsf->apfFullSizePixels/ 2;

	    x0= 3* ( ( x0+ 2 )/ 3 );

	    if  ( x1 <= x0 )
		{ return 0;	}

	    if  ( ta.taFontIsBold )
		{
		static const unsigned char	boldDot[]= { 2, 1 };

		appDrawSetLineAttributes( add,
			2, LINEstyleON_OFF_DASH, LINEcapBUTT, LINEjoinMITER,
			boldDot, sizeof( boldDot ) );
		}
	    else{
		static const unsigned char	dot[]= { 1, 2 };

		appDrawSetLineAttributes( add,
			1, LINEstyleON_OFF_DASH, LINEcapBUTT, LINEjoinMITER,
			dot, sizeof( dot ) );
		}

	    docDrawSetColorNumber( dc, sdd, ta.taTextColorNumber );

	    appDrawDrawLine( add,
				    x0- sdd->sddOx, baseLine- sdd->sddOy,
				    x1- sdd->sddOx, baseLine- sdd->sddOy );

	    break;

	case DOCtlUNDERLINE:
	    if  ( textAttr >= sfl->sflAttributeToScreenCount	||
		  sfl->sflAttributeToScreen[textAttr] < 0	)
		{ LDEB(textAttr); return -1;	}

	    screenFont= sfl->sflAttributeToScreen[textAttr];
	    dsf= apfl->apflFonts+ screenFont;

	    x0= tp->tpX0+ dsf->apfFullSizePixels/ 4;
	    x1= tp->tpX0+ tp->tpPixelsWide- dsf->apfFullSizePixels/ 2;

	    if  ( x1 <= x0 )
		{ return 0;	}

	    if  ( ta.taFontIsBold )
		{
		appDrawSetLineAttributes( add,
				2, LINEstyleSOLID, LINEcapBUTT, LINEjoinMITER,
				(const unsigned char *)0, 0 );
		}
	    else{
		appDrawSetLineAttributes( add,
				1, LINEstyleSOLID, LINEcapBUTT, LINEjoinMITER,
				(const unsigned char *)0, 0 );
		}

	    docDrawSetColorNumber( dc, sdd, ta.taTextColorNumber );

	    appDrawDrawLine( add, x0- sdd->sddOx, baseLine- sdd->sddOy,
				    x1- sdd->sddOx, baseLine- sdd->sddOy );

	    break;

	case DOCtlHYPH:
	    if  ( textAttr >= sfl->sflAttributeToScreenCount	||
		  sfl->sflAttributeToScreen[textAttr] < 0	)
		{ LDEB(textAttr); return -1;	}

	    screenFont= sfl->sflAttributeToScreen[textAttr];
	    dsf= apfl->apflFonts+ screenFont;

	    x0= tp->tpX0+ dsf->apfFullSizePixels/ 4;
	    x1= tp->tpX0+ tp->tpPixelsWide- dsf->apfFullSizePixels/ 2;

	    x0= 7* ( ( x0+ 6 )/ 7 );

	    if  ( x1 <= x0 )
		{ return 0;	}

	    if  ( ta.taFontIsBold )
		{
		static const unsigned char	boldDash[]= { 4, 3 };

		appDrawSetLineAttributes( add,
			2, LINEstyleON_OFF_DASH, LINEcapBUTT, LINEjoinMITER,
			boldDash, sizeof( boldDash ) );
		}
	    else{
		static const unsigned char	dash[]= { 3, 4 };

		appDrawSetLineAttributes( add,
			1, LINEstyleON_OFF_DASH, LINEcapBUTT, LINEjoinMITER,
			dash, sizeof( dash ) );
		}

	    docDrawSetColorNumber( dc, sdd, ta.taTextColorNumber );

	    appDrawDrawLine( add,
				    x0- sdd->sddOx, baseLine- sdd->sddOy,
				    x1- sdd->sddOx, baseLine- sdd->sddOy );

	    break;

	    break;

	case DOCtlTHICK:
	    LDEB(ts->tsLeader);
	    break;
	case DOCtlEQUAL:
	    LDEB(ts->tsLeader);
	    break;
	default:
	    LDEB(ts->tsLeader);
	    break;
	}

    return 0;
    }

static void tedDrawParticuleUnderlines(	DrawingContext *	dc,
					ScreenDrawingData *	sdd,
					const TextParticule *	tp,
					int			drawn,
					const DrawScreenFont *	dsf,
					int			baseLine )
    {
    AppDrawingData *		add= dc->dcDrawingData;
    const BufferDocument *	bd= dc->dcDocument;
    int				i;

    i= 0;
    while( i < drawn )
	{
	TextAttribute	ta;
	int		x0;
	int		x1;
	int		y0;
	int		h;

	utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp[i].tpTextAttributeNumber );
	if  ( ! ta.taTextIsUnderlined )
	    { i++; continue;	}

	x1= x0= tp[i].tpX0;
	y0= baseLine+ dsf->apfUnderLinePositionPixels;
	h= dsf->apfUnderlineThicknessPixels;

	while( i < drawn )
	    {
	    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp[i].tpTextAttributeNumber );
	    if  ( ! ta.taTextIsUnderlined )
		{ break;	}

	    x1= tp[i].tpX0+ tp[i].tpPixelsWide;
	    i++;
	    }

	if  ( x1 > x0 )
	    {
	    docDrawSetColorNumber( dc, (void *)sdd, ta.taTextColorNumber );

	    appDrawFillRectangle( add,
			    x0- sdd->sddOx, y0- sdd->sddOy, x1- x0, h );
	    }
	}

    return;
    }

static void tedDrawParticuleStrikethrough(
					DrawingContext *	dc,
					ScreenDrawingData *	sdd,
					const TextParticule *	tp,
					int			drawn,
					const DrawScreenFont *	dsf,
					int			baseLine )
    {
    AppDrawingData *		add= dc->dcDrawingData;
    const BufferDocument *	bd= dc->dcDocument;
    int				i;

    i= 0;
    while( i < drawn )
	{
	TextAttribute	ta;
	int		x0;
	int		x1;
	int		y0;
	int		h;

	utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp[i].tpTextAttributeNumber );
	if  ( ! ta.taHasStrikethrough )
	    { i++; continue;	}

	x1= x0= tp[i].tpX0;
	if  ( dsf->apfXHeightPixels > 1 )
	    { y0= baseLine- dsf->apfXHeightPixels/ 2;	}
	else{ y0= baseLine- dsf->apfFullSizePixels/ 4;	}
	h= dsf->apfUnderlineThicknessPixels;

	while( i < drawn )
	    {
	    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp[i].tpTextAttributeNumber );
	    if  ( ! ta.taHasStrikethrough )
		{ break;	}

	    x1= tp[i].tpX0+ tp[i].tpPixelsWide;
	    i++;
	    }

	if  ( x1 > x0 )
	    {
	    docDrawSetColorNumber( dc, (void *)sdd, ta.taTextColorNumber );

	    appDrawFillRectangle( add,
			    x0- sdd->sddOx, y0- sdd->sddOy, x1- x0, h );
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a footnote separator.						*/
/*									*/
/************************************************************************/

static void tedDrawChftnsep(	DrawingContext *	dc,
				int			pShift,
				int			xShift,
				int			yShift,
				void *			vsdd,
				const TextParticule *	tp,
				const TextLine *	tl )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;
    const ScreenFontList *	sfl= sdd->sddScreenFontList;
    AppDrawingData *		add= dc->dcDrawingData;
    const DrawScreenFontList *	apfl= &(add->addScreenFontList);
    const BufferDocument *	bd= dc->dcDocument;

    int				baseLine;
    int				y;
    const DrawScreenFont *	dsf;

    int				textAttr;
    int				screenFont;
    TextAttribute		ta;

    baseLine= TL_BASE_PIXELS_SH( add, tl, pShift, yShift );

    appDrawSetLineAttributes( add,
			    1, LINEstyleSOLID, LINEcapBUTT, LINEjoinMITER,
			    (const unsigned char *)0, 0 );
    y= baseLine;

    textAttr= tp->tpTextAttributeNumber;
    if  ( textAttr >= sfl->sflAttributeToScreenCount	||
	  sfl->sflAttributeToScreen[textAttr] < 0	)
	{ LDEB(textAttr); return;	}

    screenFont= sfl->sflAttributeToScreen[textAttr];
    dsf= apfl->apflFonts+ screenFont;

    y -= ( dsf->apfXHeightPixels )/ 2;

    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

    docDrawSetColorNumber( dc, (void *)sdd, ta.taTextColorNumber );

    appDrawDrawLine( add,
	    tp->tpX0- sdd->sddOx, y- sdd->sddOy,
	    tp->tpX0+ tp->tpPixelsWide- sdd->sddOx, y- sdd->sddOy );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the individual segments of a smallcaps string.			*/
/*									*/
/************************************************************************/

static void tedDrawSegments(	DrawingContext *		dc,
				AppDrawingData *		add,
				int				x,
				int				y,
				const DrawScreenFont *		dsf,
				const unsigned char *		s,
				const int *			segments,
				int				segmentCount )
    {
    int				seg;

    int				wide;
    int				fontAscent;
    int				fontDescent;

    int				scaps= 0;

    for ( seg= 0; seg < segmentCount; seg++ )
	{
	if  ( segments[2* seg+ 0] > 0 )
	    {
	    if  ( scaps )
		{
		appDrawSetFont( add, dsf->apfFontStruct );
		scaps= 0;
		}

	    appDrawDrawString( add, x, y, (char *)s, segments[2* seg+ 0] );

	    appDrawTextExtents( &wide, &fontAscent, &fontDescent,
				    add, dsf->apfFontStruct,
				    (const char *)s, segments[2* seg+ 0] );

	    s += segments[2* seg+ 0];
	    x += wide;
	    }

	if  ( segments[2* seg+ 1] > 0 )
	    {
	    if  ( ! scaps )
		{
		appDrawSetFont( add, dsf->apfScapsFontStruct );
		scaps= 1;
		}

	    appDrawSetFont( add, dsf->apfScapsFontStruct );

	    appDrawDrawString( add, x, y, (char *)s, segments[2* seg+ 1] );

	    appDrawTextExtents( &wide, &fontAscent, &fontDescent,
				    add, dsf->apfScapsFontStruct,
				    (const char *)s, segments[2* seg+ 1] );

	    s += segments[2* seg+ 1];
	    x += wide;
	    }
	}

    if  ( scaps )
	{ dc->dcCurrentTextAttributeSet= 0;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a series of particules.					*/
/*									*/
/*  1)  Only explicit tabs need to be drawn: Implicit ones have no	*/
/*	leader.								*/
/*  2)  MS-Word draws underlined tabs!					*/
/*									*/
/************************************************************************/

static int tedDrawParticules(	DrawingContext *	dc,
				int			pShift,
				int			xShift,
				int			yShift,
				void *			vsdd,
				const BufferItem *	paraBi,
				int			part,
				int			count,
				const TextLine *	tl,
				int			baseLine )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;
    const ScreenFontList *	sfl= sdd->sddScreenFontList;
    AppDrawingData *		add= dc->dcDrawingData;
    const DrawScreenFontList *	apfl= &(add->addScreenFontList);
    const BufferDocument *	bd= dc->dcDocument;
    const TextParticule *	tp= paraBi->biParaParticules+ part;
    const TabStopList *		tsl= &(paraBi->biParaTabStopList);

    int				drawn;
    int				len;

    int				textAttr;
    int				screenFont;
    TextAttribute		ta;
    const DrawScreenFont *	dsf;

    /*  1  */
    switch( tp->tpKind )
	{ 
	case DOCkindTAB:
	    /*  1  */
	    if  ( tp->tpObjectNumber >= 0			&&
		  tp->tpObjectNumber < tsl->tslTabStopCount	)
		{
		if  ( tedDrawTab( paraBi, tp, baseLine, sdd, dc ) )
		    { LDEB(1);	}
		}

	    drawn= 1;

	    /*  2  */
	    textAttr= tp->tpTextAttributeNumber;
	    if  ( textAttr >= sfl->sflAttributeToScreenCount	||
		  sfl->sflAttributeToScreen[textAttr] < 0	)
		{ LDEB(textAttr); return -1;	}

	    screenFont= sfl->sflAttributeToScreen[textAttr];
	    dsf= apfl->apflFonts+ screenFont;

	    tedDrawParticuleUnderlines( dc, sdd, tp, drawn, dsf, baseLine );
	    tedDrawParticuleStrikethrough( dc, sdd, tp, drawn, dsf, baseLine );

	    return drawn;

	case DOCkindTEXT:
	    break;

	case DOCkindFIELDSTART:
	case DOCkindFIELDEND:
	case DOCkindXE:
	case DOCkindTC:
	case DOCkindLINEBREAK:
	case DOCkindPAGEBREAK:
	case DOCkindCOLUMNBREAK:
	case DOCkindNOTE:
	    return drawn= 1;

	case DOCkindOBJECT:
	    if  ( tedDrawObject( paraBi, tp, baseLine,
					    sdd->sddOx, sdd->sddOy, add ) )
		{ LDEB(1); return -1;	}

	    return drawn= 1;

	case DOCkindCHFTNSEP:
	case DOCkindCHFTNSEPC:
	    tedDrawChftnsep( dc, pShift, xShift, yShift, vsdd, tp, tl );

	    return drawn= 1;

	default:
	    LDEB(tp->tpKind); return -1;
	}

    {
    const int	separate= 1;

    docDrawDelimitString( &len, &drawn, &ta,
					bd, paraBi, part, count, separate );
    }

    {
    textAttr= tp->tpTextAttributeNumber;
    if  ( textAttr >= sfl->sflAttributeToScreenCount	||
	  sfl->sflAttributeToScreen[textAttr] < 0	)
	{ LLDEB(textAttr,sfl->sflAttributeToScreenCount); return -1;	}

    screenFont= sfl->sflAttributeToScreen[textAttr];
    dsf= apfl->apflFonts+ screenFont;
    }

    if  ( len > 0 )
	{
	const unsigned char *	printString= paraBi->biParaString+ tp->tpStroff;
	unsigned char *		upperString= (unsigned char *)0;
	int *			segments= (int *)0;
	int			segmentCount= 0;

	int			y= baseLine;

	if  ( ta.taSmallCaps || ta.taCapitals )
	    {
	    if  ( docMakeCapsString( &upperString, &segments, &segmentCount,
						bd, &ta, printString, len ) )
		{ LDEB(len); return -1;	}

	    printString= upperString;
	    }

	if  ( ta.taSuperSub == DOCfontSUPERSCRIPT )
	    { y -= ( 8* dsf->apfAscentPixels+ 15 )/ 16;		}

	if  ( ta.taSuperSub == DOCfontSUBSCRIPT )
	    { y += ( 6* dsf->apfDescentPixels+ 15 )/ 16;	}

	docDrawSetFont( dc, vsdd, textAttr, &ta );
	docDrawSetColorNumber( dc, (void *)sdd, ta.taTextColorNumber );

	if  ( ta.taSmallCaps && ! ta.taCapitals )
	    {
	    tedDrawSegments( dc, add, tp->tpX0- sdd->sddOx, y- sdd->sddOy,
				    dsf, printString, segments, segmentCount );
	    }
	else{
	    appDrawDrawString( add, tp->tpX0- sdd->sddOx, y- sdd->sddOy,
					    (char *)printString, len );
	    }

	/*
	appDebug( "[%3d,%3d] -- %.*s\n", y, tp->tpX0, len, printString );
	*/

	if  ( upperString )
	    { free( upperString );	}
	if  ( segments )
	    { free( segments );	}
	}

    tedDrawParticuleUnderlines( dc, sdd, tp, drawn, dsf, baseLine );
    tedDrawParticuleStrikethrough( dc, sdd, tp, drawn, dsf, baseLine );

    return drawn;
    }

int tedDrawTextLine(		const BufferItem *		bi,
				int				line,
				const ParagraphFrame *		pf,
				void *				vsdd,
				DrawingContext *		dc,
				int				pShift,
				int				xShift,
				int				yShift )
    {
    AppDrawingData *		add= dc->dcDrawingData;

    const TextLine *		tl= bi->biParaLines+ line;
    int				part= tl->tlFirstParticule;
    int				done;

    int				baseline;

    baseline= TL_BASE_PIXELS_SH( add, tl, pShift, yShift );

    done= 0;
    while( done < tl->tlParticuleCount )
	{
	int		drawn;

	drawn= tedDrawParticules( dc, pShift, xShift, yShift, vsdd, bi, part,
				tl->tlParticuleCount- done, tl, baseline );
	if  ( drawn < 1 )
	    { LDEB(drawn); return -1;	}

	done += drawn; part += drawn;
	}

    return done;
    }

/************************************************************************/
/*									*/
/*  Draw a rectangle to show that (part of) a line is selected.		*/
/*									*/
/*  1)  Delimit the line and calculate a preliminary rectangle for the	*/
/*	line.								*/
/*  2)  Table rectangle selections look nicer when we select the full	*/
/*	width of the table cell.					*/
/*  3)  Where to we start the selected rectangle? (left)		*/
/*  4)  Where to we end the selected rectangle? (right)			*/
/*  5)  Do the line and the selection overlap. The two separate tests	*/
/*	reject the situation where only the begin or end position of	*/
/*	the line is in the selection.					*/
/*	Either the begin of the selection must be before the end of the	*/
/*	line or the end of the selection must be after the end of the	*/
/*	line.								*/
/*  6)  Intersect with clipping rectangle.				*/
/*									*/
/************************************************************************/

static int tedLineRectangle(	DocumentRectangle *		drRedraw,
				int *				pInSelection,
				const BufferItem *		bi,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd,
				int				line,
				const ParagraphFrame *		pf )
    {
    const DocumentSelection *	ds= dc->dcDocumentSelection;
    const SelectionGeometry *	sg= dc->dcSelectionGeometry;
    const DocumentRectangle *	drClip= dc->dcClipRect;
    const ScreenFontList *	sfl= sdd->sddScreenFontList;

    DocumentSelection	dsLine;
    SelectionGeometry	sgLine;

    const int		lastLine= 0;
    DocumentRectangle	drLine;

    int			tableRectangle;
    int			cmp_SeLe;
    int			cmp_SbLb;
    int			cmp_SbLe;
    int			cmp_SeLb;

    int			partLineBegin;
    int			partLineEnd;

    int			inSelection;

    /*  1  */
    docLineSelection( &dsLine, &partLineBegin, &partLineEnd, bi, line );
    tedSelectionGeometry( &sgLine, &dsLine, lastLine,
				    dc->dcDocument, dc->dcDrawingData, sfl );

    if  ( sgLine.sgBegin.pgLine != line )
	{ LLDEB(sgLine.sgBegin.pgLine,line);	}
    if  ( sgLine.sgEnd.pgLine != line )
	{ LLDEB(sgLine.sgEnd.pgLine,line);	}

    drLine= sgLine.sgRectangle;

    /*  2  */
    tableRectangle= 0;
    if  ( ds->dsCol0 >= 0						&&
	  ds->dsCol1 >= 0						&&
	  ! docPositionsInsideCell( &(ds->dsBegin), &(ds->dsEnd) )	)
	{ tableRectangle= 1; }

    cmp_SbLb= docComparePositions( &(ds->dsBegin), &(dsLine.dsBegin) );
    cmp_SeLb= docComparePositions( &(ds->dsEnd), &(dsLine.dsBegin) );
    cmp_SbLe= docComparePositions( &(ds->dsBegin), &(dsLine.dsEnd) );
    cmp_SeLe= docComparePositions( &(ds->dsEnd), &(dsLine.dsEnd) );

    /*  3  */
    if  ( cmp_SbLb <= 0 || ( cmp_SbLb == 0 && tableRectangle ) )
	{
	drLine.drX0= pf->pfX0Pixels;	

	if  ( cmp_SbLb == 0 )
	    {
	    if  ( line == 0 )
		{ drLine.drX0= pf->pfX0FirstLinePixels;	}
	    else{ drLine.drX0= pf->pfX0TextLinesPixels;	}
	    }
	}
    else{ drLine.drX0= sg->sgBegin.pgXPixels;	}

    /*  4  */
    if  ( cmp_SeLe > 0 || ( cmp_SeLe == 0 && tableRectangle ) )
	{ drLine.drX1= pf->pfX1Pixels;		}
    else{ drLine.drX1= sg->sgEnd.pgXPixels;	}

    /*  5  */
    inSelection= 0;
    if  ( cmp_SbLe < 0 && cmp_SeLb > 0 )
	{ inSelection= 1;	}

    if  ( ds->dsEnd  .dpBi->biParaStrlen == 0	&&
	  cmp_SeLb == 0				)
	{ cmp_SeLb=  1;	}
    if  ( ds->dsBegin.dpBi->biParaStrlen == 0	&&
	  cmp_SbLe == 0				)
	{ cmp_SbLe= -1;	}
    if  ( cmp_SbLe < 0 && cmp_SeLb > 0 )
	{ inSelection= 1;	}

    /*  6  */
    if  ( ! drClip || geoIntersectRectangle( drRedraw, drClip, &drLine ) )
	{ *pInSelection= inSelection; return 1;	}
    else{ return 0;	}
    }

int tedDrawTextReverse(		const BufferItem *		bi,
				int				line,
				const ParagraphFrame *		pf,
				void *				vsdd,
				DrawingContext *		dc,
				int				pShift,
				int				xShift,
				int				yShift )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;
    AppDrawingData *		add= dc->dcDrawingData;

    DocumentRectangle		drRedraw;
    int				done;
    int				inSelection;

    if  ( tedLineRectangle( &drRedraw, &inSelection, bi, dc, sdd, line, pf ) )
	{
	drRedraw.drX0 -= sdd->sddOx;
	drRedraw.drX1 -= sdd->sddOx;
	drRedraw.drY0 -= sdd->sddOy;
	drRedraw.drY1 -= sdd->sddOy;

	appDrawSetClipRect( add, &drRedraw );

	done= tedDrawTextLine( bi, line, pf, vsdd, dc, pShift, xShift, yShift );
	}
    else{
	TextLine *	tl= bi->biParaLines+ line;

	done= tl->tlParticuleCount;
	}

    return done;
    }

int tedDrawTextSelected(	const BufferItem *		bi,
				int				line,
				const ParagraphFrame *		pf,
				void *				vsdd,
				DrawingContext *		dc,
				int				pShift,
				int				xShift,
				int				yShift )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;
    AppDrawingData *		add= dc->dcDrawingData;

    DocumentRectangle		drRedraw;
    int				done;
    int				inSelection;

    if  ( tedLineRectangle( &drRedraw, &inSelection, bi, dc, sdd, line, pf ) )
	{
	drRedraw.drX0 -= sdd->sddOx;
	drRedraw.drX1 -= sdd->sddOx;
	drRedraw.drY0 -= sdd->sddOy;
	drRedraw.drY1 -= sdd->sddOy;

	appDrawSetClipRect( add, &drRedraw );

	if  ( inSelection )
	    {
	    docDrawSetColorRgb( dc, (void *)sdd,
				sdd->sddBackColor.rgb8Red,
				sdd->sddBackColor.rgb8Green,
				sdd->sddBackColor.rgb8Blue );

	    appDrawFillRectangle( add,
		    drRedraw.drX0,
		    drRedraw.drY0,
		    drRedraw.drX1- drRedraw.drX0+ 1,
		    drRedraw.drY1- drRedraw.drY0+ 1 );
	    }

	done= tedDrawTextLine( bi, line, pf, vsdd, dc, pShift, xShift, yShift );
	}
    else{
	TextLine *	tl= bi->biParaLines+ line;

	done= tl->tlParticuleCount;
	}

    return done;
    }

