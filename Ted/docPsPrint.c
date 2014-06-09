/************************************************************************/
/*									*/
/*  Print PostScript, include PDF marks to preserve links when the	*/
/*  PostScript is converted to PDF.					*/
/*									*/
/*  Though the Adobe Technical Note #5150: "pdfmark Reference Manual"	*/
/*  exhaustively documents the pdfmark functionality, the book:		*/
/*  Merz, Thomas, "Postscript & Acrobat/PDF", Springer-Verlag, Berlin	*/
/*  &c, 1996, ISBN 3-540-60854-0. gives a summary on page 289 that is	*/
/*  much easier to understand.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	<sioGeneral.h>
#   include	<appWinMeta.h>

#   include	"docDraw.h"
#   include	"docLayout.h"
#   include	"docPsPrint.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Cause subsequent drawing to be done in a certain color.		*/
/*									*/
/*  NOTE that background drawing with color= 0 is against the RTF idea	*/
/*	of the default background color: transparent.			*/
/*									*/
/************************************************************************/

static int docPsSetColorRgb(	DrawingContext *	dc,
				void *			vps,
				int			r,
				int			g,
				int			b )
    {
    PrintingState *		ps= (PrintingState *)vps;

    utilPsSetRgbColor( ps, r/ 255.0, g/ 255.0, b/ 255.0 );

    return 0;
    }

static int docPsSetFont(	DrawingContext *	dc,
				void *			vps,
				int			textAttr,
				const TextAttribute *	ta )
    {
    PrintingState *		ps= (PrintingState *)vps;

    utilPsSetFont( ps, "f", ta );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Printing of borders.						*/
/*									*/
/************************************************************************/

static void psPrintHorizontalBorder(
				const BorderProperties *	bpHor,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				DrawingContext *		dc,
				void *				vps,
				int				x0Twips,
				int				x1Twips,
				int				above,
				int				y )
    {
    PrintingState *	ps= (PrintingState *)vps;

    int			space;
    int			thick;

    if  ( ! DOCisBORDER( bpHor ) )
	{ return;	}

    if  ( bpLeft && DOCisBORDER( bpLeft ) )
	{
	thick= docBorderThick( &space, bpLeft );

	x0Twips -= space;
	x0Twips -= thick/ 2;
	}

    if  ( bpRight && DOCisBORDER( bpRight ) )
	{
	thick= docBorderThick( &space, bpRight );

	x1Twips += space;
	x1Twips -= ( thick+ 1 )/ 2;
	}

    thick= docBorderThick( &space, bpHor );

    if  ( above )
	{ y -= space+ thick;	}

    docDrawSetColorNumber( dc, (void *)ps, bpHor->bpColor );

    utilPsFillRectangle( ps, x0Twips, y, x1Twips- x0Twips+ 1, thick );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a vertical border. E.G. the left or right border of a table	*/
/*  cell.								*/
/*									*/
/*  1)  MS-Word usually draws the vertical borders between the		*/
/*	horizontal ones so subtract the width of the top border from	*/
/*	the top of the vertical one.					*/
/*  2)  MS-Word draws the bottom borders outside the cells so do not	*/
/*	subtract the width of the bottom border.			*/
/*									*/
/************************************************************************/

static void psPrintVerticalBorder(
				const BorderProperties *	bp,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpBottom,
				DrawingContext *		dc,
				void *				vps,
				int				x,
				int				y0,
				int				y1 )
    {
    PrintingState *	ps= (PrintingState *)vps;

    int			space;
    int			thick;

    if  ( ! DOCisBORDER( bp ) )
	{ return;	}

    /*  1  */
    if  ( bpTop && DOCisBORDER( bpTop ) )
	{
	thick= docBorderThick( &space, bpTop );

	y0 += -space+ thick;
	}

    /*  2
    if  ( bpBottom && DOCisBORDER( bpBottom ) )
	{
	thick= docBorderThick( &wide, bpBottom );

	y1 -= -space+ thick;
	}
    */

    thick= docBorderThick( &space, bp );

    x -= thick/ 2;

    docDrawSetColorNumber( dc, (void *)ps, bp->bpColor );
    utilPsFillRectangle( ps, x, y0, thick, y1- y0 );

    return;
    }

/************************************************************************/
/*									*/
/*  Write links to be picked up by the distiller.			*/
/*									*/
/************************************************************************/

static void psPrintString(	PrintingState *			ps,
				DrawingContext *		dc,
				int				xShift,
				int				yShift,
				int				baseLine,
				const ParticuleData *		pd,
				int				textAttr,
				const TextAttribute *		ta,
				const unsigned char *		s,
				int				len )
    {
    int		fontSizeTwips;

    fontSizeTwips= 10* ta->taFontSizeHalfPoints;

    if  ( len > 0 )
	{
	int			y;
	int			capHeight;
	int			xHeight;

	docDrawSetFont( dc, (void *)ps, textAttr, ta );
	docDrawSetColorNumber( dc, (void *)ps, ta->taTextColorNumber );

	utilPsPrintStringValue( ps, s, len );

	capHeight= ( fontSizeTwips* pd->pdAfi->afiCapHeight+ 500 )/ 1000;
	if  ( capHeight == 0 )
	    { capHeight= ( fontSizeTwips* pd->pdAfi->afiAscender+ 500 )/ 1000; }

	xHeight= ( fontSizeTwips* pd->pdAfi->afiXHeight+ 500 )/ 1000;
	if  ( xHeight == 0 )
	    { xHeight= ( fontSizeTwips+ 1 )/ 2; }

	y= baseLine;

	if  ( ta->taSuperSub == DOCfontSUPERSCRIPT )
	    {
	    int suprSizeTwips= SUPERSUB_SIZE( fontSizeTwips );

	    int	ascYF= ( fontSizeTwips* pd->pdAfi->afiAscender+ 500 )/ 1000;
	    int	ascYS= ( suprSizeTwips* pd->pdAfi->afiAscender+ 500 )/ 1000;

	    y -= ascYF- ascYS;
	    /* was y -= xHeight; */
	    }

	if  ( ta->taSuperSub == DOCfontSUBSCRIPT )
	    {
	    int subsSizeTwips= SUPERSUB_SIZE( fontSizeTwips );

	    int	descYF= ( fontSizeTwips* pd->pdAfi->afiDescender+ 500 )/ 1000;
	    int	descYS= ( subsSizeTwips* pd->pdAfi->afiDescender+ 500 )/ 1000;

	    y += descYS- descYF;
	    /* was y += ( 4* capHeight )/ 10; */
	    }

	sioOutPrintf( ps->psSos, " %d %d mvs\n", pd->pdX0+ xShift, y+ yShift );
	ps->psLastPageMarked= ps->psPagesPrinted;
	}

    return;
    }

static void psPrintSegment(	PrintingState *			ps,
				DrawingContext *		dc,
				int				textAttr,
				const TextAttribute *		ta,
				const unsigned char *		s,
				int				len )
    {
    docDrawSetFont( dc, (void *)ps, textAttr, ta );

    utilPsPrintStringValue( ps, s, len );

    sioOutPutString( "show ", ps->psSos );
    ps->psLastPageMarked= ps->psPagesPrinted;

    return;
    }

static void psPrintSegments(	PrintingState *			ps,
				DrawingContext *		dc,
				int				xShift,
				int				yShift,
				int				baseLine,
				const ParticuleData *		pd,
				int				textAttr,
				const TextAttribute *		ta,
				const unsigned char *		s,
				const int *			segments,
				int				segmentCount )
    {
    TextAttribute		taN= *ta;
    int				seg;

    taN.taSmallCaps= 0;

    if  ( segments[0] > 0 )
	{
	psPrintString( ps, dc, xShift, yShift, baseLine, pd,
					    textAttr, &taN, s, segments[0] );
	s += segments[0];

	psPrintSegment( ps, dc, textAttr, ta, s, segments[1] );
	s += segments[1];
	}
    else{
	psPrintString( ps, dc, xShift, yShift, baseLine, pd,
					    textAttr, ta, s, segments[1] );
	s += segments[1];
	}

    for ( seg= 1; seg < segmentCount; seg++ )
	{
	if  ( segments[2* seg+ 0] > 0 )
	    {
	    psPrintSegment( ps, dc, textAttr, &taN, s, segments[2* seg+ 0] );
	    s += segments[2* seg+ 0];
	    }

	if  ( segments[2* seg+ 1] > 0 )
	    {
	    psPrintSegment( ps, dc, textAttr, ta, s, segments[2* seg+ 1] );
	    s += segments[2* seg+ 1];
	    }
	}

    return;
    }

static void psPrintParticuleUnderlines(	DrawingContext *	dc,
					int			xShift,
					int			yShift,
					PrintingState *		ps,
					const TextParticule *	tp,
					const ParticuleData *	pd,
					int			drawn,
					int			fontSizeTwips,
					int			baseLine )
    {
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

	x1= x0= pd[i].pdX0;
	y0= baseLine- ( fontSizeTwips*
			pd[i].pdAfi->afiUnderlinePosition+ 500 )/ 1000;
	h= ( fontSizeTwips*
			pd[i].pdAfi->afiUnderlineThickness+ 500 )/ 1000;

	while( i < drawn )
	    {
	    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp[i].tpTextAttributeNumber );

	    if  ( ! ta.taTextIsUnderlined )
		{ break;	}

	    x1= pd[i].pdX0+ pd[i].pdWidth; i++;
	    }

	docDrawSetColorNumber( dc, (void *)ps, ta.taTextColorNumber );
	utilPsFillRectangle( ps,  x0+ xShift, y0+ yShift, x1- x0, h );
	}

    return;
    }

static void psPrintParticuleStrikethrough(
					DrawingContext *	dc,
					int			xShift,
					int			yShift,
					PrintingState *		ps,
					const TextParticule *	tp,
					const ParticuleData *	pd,
					int			drawn,
					int			fontSizeTwips,
					int			baseLine )
    {
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

	x1= x0= pd[i].pdX0;
	if  ( pd[i].pdAfi->afiXHeight > 1 )
	    {
	    y0= baseLine- ( fontSizeTwips*
				    pd[i].pdAfi->afiXHeight/ 2+ 500 )/ 1000;
	    }
	else{
	    y0= baseLine- ( fontSizeTwips/ 4 );
	    }

	h= ( fontSizeTwips*
			pd[i].pdAfi->afiUnderlineThickness+ 500 )/ 1000;

	while( i < drawn )
	    {
	    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp[i].tpTextAttributeNumber );

	    if  ( ! ta.taHasStrikethrough )
		{ break;	}

	    x1= pd[i].pdX0+ pd[i].pdWidth; i++;
	    }

	docDrawSetColorNumber( dc, (void *)ps, ta.taTextColorNumber );
	utilPsFillRectangle( ps, x0+ xShift, y0+ yShift, x1- x0, h );
	}

    return;
    }

static void psPrintChftnsep(	DrawingContext *	dc,
				int			xShift,
				int			yShift,
				PrintingState *		ps,
				const TextParticule *	tp,
				const ParticuleData *	pd,
				int			baseLine )
    {
    BufferDocument *		bd= dc->dcDocument;

    TextAttribute		ta;
    int				fontSizeTwips;

    int				xHeight;

    int				x0;
    int				x1;
    int				y0;
    int				h;

    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

    fontSizeTwips= 10* ta.taFontSizeHalfPoints;

    xHeight= ( fontSizeTwips+ 1 )/ 2;

    x0= pd->pdX0;
    x1= pd->pdX0+ 2800;
    y0= baseLine- xHeight/2- 15/2;
    h= 15;

    docDrawSetColorNumber( dc, (void *)ps, ta.taTextColorNumber );
    utilPsFillRectangle( ps, x0+ xShift, y0+ yShift, x1- x0, h );

    return;
    }

/************************************************************************/
/*									*/
/*  Print a series of particules with the same attributes.		*/
/*									*/
/************************************************************************/

static int psPrintParticules(	PrintingState *			ps,
				DrawingContext *		dc,
				int				xShift,
				int				yShift,
				const BufferItem *		bi,
				int				part,
				const DocumentFontList *	dfl,
				const PostScriptFontList *	psfl,
				const TextParticule *		tp,
				ParticuleData *			pd,
				int				count,
				int				baseLine,
				int				lineTop,
				int				lineHeight )
    {
    BufferDocument *		bd= dc->dcDocument;
    const TabStopList *		tsl= &(bi->biParaTabStopList);

    int				drawn;
    int				len;

    DocumentField *		df;

    TextAttribute		ta;
    int				fontSizeTwips;

    /*  1  */
    switch( tp->tpKind )
	{
	case DOCkindTAB:
	    if  ( pd->pdTabNumber >= 0				&&
		  pd->pdTabNumber < tsl->tslTabStopCount	)
		{
		if  ( docPsPrintTab( ps, dc, xShift, yShift, bi, tp, pd,
						    baseLine, lineHeight ) )
		    { LDEB(1);	}
		}

	    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

	    fontSizeTwips= 10* ta.taFontSizeHalfPoints;

	    drawn= 1;

	    psPrintParticuleUnderlines( dc, xShift, yShift, ps,
				    tp, pd, drawn, fontSizeTwips, baseLine );
	    psPrintParticuleStrikethrough( dc, xShift, yShift, ps,
				    tp, pd, drawn, fontSizeTwips, baseLine );
	    return drawn;

	case DOCkindTEXT:
	    break;

	case DOCkindFIELDSTART:
	    df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;

	    if  ( df->dfKind == DOCfkBOOKMARK )
		{
		const char *	refName;
		int		refSize;

		if  ( ! docFieldGetBookmark( df, &refName, &refSize ) )
		    {
		    utilPsDestPdfmark( ps, lineTop, refName, refSize );
		    }

		return drawn= 1;
		}

	    if  ( df->dfKind == DOCfkCHFTN )
		{
		const char *	refName;
		int		refSize;
		const char *	markName;
		int		markSize;

		int		noteIndex;
		DocumentNote *	dn;

		char		ref[25+1];
		char		def[25+1];

		int		cnt;

		cnt= docCountParticulesInField( bi, part,
						    bi->biParaParticuleCount );
		if  ( cnt < 0 )
		    { LDEB(cnt); }

		noteIndex= docGetNote( &dn, bd, bi, tp[cnt+1].tpStroff );
		if  ( noteIndex < 0 )
		    {
		    LLDEB(cnt,tp[cnt+1].tpStroff);
		    SLDEB(docExternalKindStr(bi->biInExternalItem),noteIndex);
		    }

		sprintf( ref, "_NREF_%d", noteIndex+ 1 );
		sprintf( def, "_NDEF_%d", noteIndex+ 1 );

		if  ( bi->biInExternalItem == DOCinBODY )
		    {
		    markName= def;
		    refName=  ref;
		    }
		else{
		    markName= ref;
		    refName=  def;
		    }

		markSize= strlen( markName );
		refSize= strlen( refName );

		utilPsDestPdfmark( ps, lineTop, refName, refSize );

		ps->psLinkFile= (char *)0; ps->psLinkFileSize= 0;
		ps->psLinkMark= markName; ps->psLinkMarkSize= markSize;

		ps->psInsideLink= 1;
		ps->psLinkParticulesDone= 0;
		ps->psLinkRectLeft= pd->pdX0;

		return drawn= 1;
		}

	    if  ( df->dfKind == DOCfkHYPERLINK )
		{
		if  ( ! docFieldGetHyperlink( df,
				&(ps->psLinkFile), &(ps->psLinkFileSize),
				&(ps->psLinkMark), &(ps->psLinkMarkSize) ) )
		    {
		    ps->psInsideLink= 1;
		    ps->psLinkParticulesDone= 0;
		    ps->psLinkRectLeft= pd->pdX0;
		    }

		return drawn= 1;
		}

	    return drawn= 1;

	case DOCkindFIELDEND:
	    if  ( ps->psInsideLink )
		{
		utilPsFlushLink( ps, pd->pdX0, pd->pdVisibleWidth,
						    lineTop, lineHeight );
		ps->psInsideLink= 0;
		}

	    return drawn= 1;

	case DOCkindXE:
	case DOCkindTC:
	case DOCkindLINEBREAK:
	case DOCkindPAGEBREAK:
	case DOCkindCOLUMNBREAK:
	case DOCkindNOTE:
	    return drawn= 1;

	case DOCkindOBJECT:
	    drawn= docPsPrintObject( ps, dc, xShift, yShift,
					bi->biParaObjects+ tp->tpObjectNumber,
					psfl, pd, baseLine );

	    if  ( drawn < 1 )
		{ LDEB(drawn);	}
	    return drawn;

	case DOCkindCHFTNSEP:
	case DOCkindCHFTNSEPC:
	    psPrintChftnsep( dc, xShift, yShift, ps, tp, pd, baseLine );
	    return 1;

	default:
	    LDEB(tp->tpKind); return -1;
	}

    {
    const int	separate= 0;

    docDrawDelimitString( &len, &drawn, &ta, bd, bi, part, count, separate );
    }

    fontSizeTwips= 10* ta.taFontSizeHalfPoints;

    if  ( len > 0 )
	{
	const unsigned char *	printString= bi->biParaString+ tp->tpStroff;
	unsigned char *		upperString= (unsigned char *)0;
	int *			segments= (int *)0;
	int			segmentCount= 0;
	int			textAttr= tp->tpTextAttributeNumber;

	if  ( ta.taSmallCaps || ta.taCapitals )
	    {
	    if  ( docMakeCapsString( &upperString, &segments, &segmentCount,
						bd, &ta, printString, len ) )
		{ LDEB(len); return -1;	}

	    printString= upperString;
	    }

	if  ( ta.taSmallCaps && ! ta.taCapitals )
	    {
	    psPrintSegments(  ps, dc, xShift, yShift, baseLine, pd,
			textAttr, &ta, printString, segments, segmentCount );
	    }
	else{
	    psPrintString( ps, dc, xShift, yShift, baseLine, pd,
			textAttr, &ta, printString, len );
	    }

	if  ( upperString )
	    { free( upperString );	}
	if  ( segments )
	    { free( segments );	}
	}

    psPrintParticuleUnderlines( dc, xShift, yShift, ps, tp, pd, drawn,
						    fontSizeTwips, baseLine );
    psPrintParticuleStrikethrough( dc, xShift, yShift, ps, tp, pd, drawn,
						    fontSizeTwips, baseLine );

    ps->psLinkParticulesDone += drawn;

    return drawn;
    }

/************************************************************************/
/*									*/
/*  Print one line of output.						*/
/*									*/
/************************************************************************/

static int psPrintTextLine(	PrintingState *			ps,
				DrawingContext *		dc,
				int				xShift,
				int				yShift,
				const DocumentFontList *	dfl,
				const PostScriptFontList *	psfl,
				const BufferItem *		bi,
				int				part,
				const TextParticule *		tp,
				int				particuleCount,
				ParticuleData *			pd,
				int				baseLine,
				int				lineTop,
				int				lineHeight )
    {
    int				done= 0;

    while( done < particuleCount )
	{
	int		drawn;

	drawn= psPrintParticules( ps, dc, xShift, yShift,
			bi, part, dfl, psfl, tp, pd,
			particuleCount- done, baseLine, lineTop, lineHeight );

	if  ( drawn < 1 )
	    { LDEB(drawn); return -1;	}

	done += drawn; tp += drawn; pd += drawn; part += drawn;
	}

    if  ( done > 0 && ps->psInsideLink )
	{
	const ParticuleData *	ppd= pd- 1;

	utilPsFlushLink( ps, ppd->pdX0, ppd->pdVisibleWidth,
						    lineTop, lineHeight );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Layout and print successive lines of a paragraph.			*/
/*									*/
/************************************************************************/

static int docPsPrintTextLine(	const BufferItem *		bi,
				int				line,
				const ParagraphFrame *		pf,
				void *				vps,
				DrawingContext *		dc,
				int				pShift,
				int				xShift,
				int				yShift )
    {
    BufferDocument *		bd= dc->dcDocument;
    const DocumentProperties *	dp= &(bd->bdProperties);
    PrintingState *		ps= (PrintingState *)vps;
    const PostScriptFontList *	psfl= dc->dcPostScriptFontList;

    const TextLine *		tl= bi->biParaLines+ line;
    int				part= tl->tlFirstParticule;
    const TextParticule *	tp= bi->biParaParticules+ part;

    int				accepted;
    TextLine			boxLine;
    int				baseline;

    ParticuleData *		pd;

    if  ( docPsClaimParticuleData( bi, &pd ) )
	{ LDEB(bi->biParaParticuleCount); return -1;	}

    boxLine= *tl;
    accepted= docLayoutLineBox( bd, &boxLine, bi, part, psfl, pd, pf );

    if  ( accepted < 1 )
	{ LDEB(accepted); return -1;	}

    if  ( ps->psInsideLink )
	{ ps->psLinkRectLeft= pd->pdX0;	}

    baseline= tl->tlTopPosition.lpPageYTwips+ tl->tlLineAscentTwips;

    psPrintTextLine( ps, dc, xShift, yShift, &(dp->dpFontList), psfl,
			bi, part, tp, accepted, pd,
			baseline, tl->tlTopPosition.lpPageYTwips,
			tl->tlLineHeightTwips );

    return accepted;
    }

static int docPsPrintItemShade(	const ItemShading *		is,
				void *				vps,
				struct DrawingContext *		dc,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				int				x0Twips,
				int				x1Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow )
    {
    PrintingState *		ps= (PrintingState *)vps;
    const BufferDocument *	bd= dc->dcDocument;

    if  ( is->isPattern == DOCspSOLID )
	{
	int		isSolid= 0;
	int		r= 0;
	int		g= 0;
	int		b= 0;

	if  ( docGetSolidRgbShadeOfItem( &isSolid, &r, &g, &b, bd, is ) )
	    { LDEB(1);	}

	if  ( isSolid )
	    {
	    int		thick;
	    int		space;

	    int		y0Twips= lpTop->lpPageYTwips;

	    /*  1  */
	    if  ( bpTop && DOCisBORDER( bpTop ) )
		{
		thick= docBorderThick( &space, bpTop );

		y0Twips -= -space;
		y0Twips += thick;
		}

	    if  ( bpLeft && DOCisBORDER( bpLeft ) )
		{
		thick= docBorderThick( &space, bpLeft );

		x0Twips -= space;
		x0Twips += thick/ 2+ 1;
		}

	    if  ( bpRight && DOCisBORDER( bpRight ) )
		{
		thick= docBorderThick( &space, bpRight );

		x1Twips += space;
		x1Twips -= thick/ 2;
		}

	    docDrawSetColorRgb( dc, (void *)ps, r, g, b );
	    utilPsFillRectangle( ps, 
				x0Twips, y0Twips,
				x1Twips- x0Twips,
				lpBelow->lpPageYTwips- y0Twips );

	    dc->dcCurrentColorSet= 0;
	    }
	}

    return 0;
    }

static int docPsPrintParaTop(	const BorderProperties *	bp,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				const ParagraphFrame *		pf,
				const LayoutPosition *		lpTop,
				void *				vps,
				DrawingContext *		dc )
    {
    int			x0Twips= pf->pfX0TextLinesTwips;
    const int		above= 0;

    if  ( x0Twips > pf->pfX0FirstLineTwips )
	{ x0Twips=  pf->pfX0FirstLineTwips;	}

    psPrintHorizontalBorder( bp, bpLeft, bpRight, dc,
					vps, x0Twips, pf->pfX1TextLinesTwips,
					above, lpTop->lpPageYTwips );

    return 0;
    }

static int docPsPrintParaBottom( const BorderProperties *	bp,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				const ParagraphFrame *		pf,
				const LayoutPosition *		lpBelow,
				void *				vps,
				DrawingContext *		dc )
    {
    int			x0Twips= pf->pfX0TextLinesTwips;
    const int		above= 1;

    if  ( x0Twips > pf->pfX0FirstLineTwips )
	{ x0Twips=  pf->pfX0FirstLineTwips;	}

    psPrintHorizontalBorder( bp, bpLeft, bpRight, dc,
					vps, x0Twips, pf->pfX1TextLinesTwips,
					above, lpBelow->lpPageYTwips );

    return 0;
    }

static int docPsPrintParaLeft(	const BorderProperties *	bpLeft,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpBottom,
				const ParagraphFrame *		pf,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow,
				void *				vps,
				DrawingContext *		dc )
    {
    int				x0Twips= pf->pfX0TextLinesTwips;

    if  ( x0Twips > pf->pfX0FirstLineTwips )
	{ x0Twips=  pf->pfX0FirstLineTwips;	}

    x0Twips -= bpLeft->bpSpacingTwips;

    psPrintVerticalBorder( bpLeft, bpTop, bpBottom, dc, vps, x0Twips,
			    lpTop->lpPageYTwips, lpBelow->lpPageYTwips );

    return 0;
    }

static int docPsPrintParaRight(	const BorderProperties *	bpRight,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpBottom,
				const ParagraphFrame *		pf,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow,
				void *				vps,
				DrawingContext *		dc )
    {
    int				x1Twips= pf->pfX1TextLinesTwips;

    int				thick;
    int				space;

    thick= docBorderThick( &space, bpRight );

    x1Twips += bpRight->bpSpacingTwips;
    x1Twips -= thick;

    psPrintVerticalBorder( bpRight, bpTop, bpBottom, dc, vps, x1Twips,
			    lpTop->lpPageYTwips, lpBelow->lpPageYTwips );

    return 0;
    }

static int docPsPrintParaShade(	const ParagraphProperties *	pp,
				const BorderProperties *	bpTop,
				void *				vps,
				struct DrawingContext *		dc,
				const ParagraphFrame *		pf,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow )
    {
    const BorderProperties * const	bpLeft= &(pp->ppLeftBorder);
    const BorderProperties * const	bpRight= &(pp->ppRightBorder);

    int					x0Twips= pf->pfX0TextLinesTwips;

    if  ( x0Twips > pf->pfX0FirstLineTwips )
	{ x0Twips=  pf->pfX0FirstLineTwips;	}

    return docPsPrintItemShade( &(pp->ppShading), vps, dc,
					bpTop, bpLeft, bpRight,
					x0Twips, pf->pfX1TextLinesTwips,
					lpTop, lpBelow );

    return 0;
    }

static int docPsPrintCellTop(	const BorderProperties *	bp,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				int				asGrid,
				int				x0Twips,
				int				x1Twips,
				void *				vps,
				DrawingContext *		dc,
				const LayoutPosition *		lpTop )
    {
    if  ( ! asGrid && DOCisBORDER( bp ) )
	{
	const int		above= 0;

	psPrintHorizontalBorder( bp, bpLeft, bpRight, dc,
			vps, x0Twips, x1Twips, above, lpTop->lpPageYTwips );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a cell bottom.							*/
/*									*/
/*  1)  MS-Word really does it this way: The same space is allocated	*/
/*	for the bottom border of one row ant the tob border of the	*/
/*	next.								*/
/*									*/
/************************************************************************/

static int docPsPrintCellBottom( const BorderProperties *	bp,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				int				asGrid,
				int				x0Twips,
				int				x1Twips,
				void *				vps,
				DrawingContext *		dc,
				const LayoutPosition *		lpBottom )
    {
    if  ( ! asGrid && DOCisBORDER( bp ) )
	{
	/*  1  */
	const int		above= 0;

	psPrintHorizontalBorder( bp, bpLeft, bpRight, dc,
			vps, x0Twips, x1Twips, above, lpBottom->lpPageYTwips );
	}

    return 0;
    }

static int docPsPrintCellLeft(	const BorderProperties *	bp,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpBottom,
				int				asGrid,
				void *				vps,
				DrawingContext *		dc,
				int				x0Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow )
    {
    if  ( ! asGrid && DOCisBORDER( bp ) )
	{
	psPrintVerticalBorder( bp, bpTop, bpBottom, dc, vps, x0Twips,
			    lpTop->lpPageYTwips, lpBelow->lpPageYTwips );
	}

    return 0;
    }

static int docPsPrintCellRight(	const BorderProperties *	bp,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpBottom,
				int				asGrid,
				void *				vps,
				DrawingContext *		dc,
				int				x1Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow )
    {
    if  ( ! asGrid && DOCisBORDER( bp ) )
	{
	psPrintVerticalBorder( bp, bpTop, bpBottom, dc, vps, x1Twips,
			    lpTop->lpPageYTwips, lpBelow->lpPageYTwips );
	}

    return 0;
    }

static int docPsPrintCellShade(	const CellProperties *		cp,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				void *				vps,
				struct DrawingContext *		dc,
				int				x0Twips,
				int				x1Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow )
    {
    return docPsPrintItemShade( &(cp->cpShading), vps, dc,
					bpTop, bpLeft, bpRight,
					x0Twips, x1Twips, lpTop, lpBelow );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Skip to the next page.						*/
/*									*/
/*  1)  If anything is printed on the page, and header/footer printing	*/
/*      is postponed, print headers and footers for this page.		*/
/*  2)  If omitHeadersOnEmptyPages is set, dc->dcPostponeHeadersFooters	*/
/*	is derived from it. So any header printing is covered by (1)	*/
/*  3)  If the page is completely empty, skip it.			*/
/*  4)  if the page is empty, but not have been so if headers and	*/
/*	footers were printed, emit it. Otherwise skip it. This sounds a	*/
/*	bit strange, but the customer is always right. Actually, it has	*/
/*	a purpose: In simple documents, you do not want to waste paper.	*/
/*	So you should not print empty pages. In more complicated	*/
/*	documents, you do not want to print pages with only a header	*/
/*	and a footer to prevent fraud. For pagination purposes, you do	*/
/*	want to emit the page however. Now in stead of make the user	*/
/*	say what she wants with a multitude of parameters, we		*/
/*	distinguish between simple and complicated documents by		*/
/*	assuming that a complicated document has headers and footers.	*/
/*									*/
/************************************************************************/

static int docPsFinishPage(	void *				vps,
				DrawingContext *		dc,
				BufferItem *			bodySectBi,
				int				page,
				int				asLast )
    {
    PrintingState *	ps= (PrintingState *)vps;

    int			pageWasMarked;
    int			pageIsMarked;
    int			pageHasHeader= dc->dcDocHasPageHeaders;
    int			pageHasFooter= dc->dcDocHasPageFooters;
    int			skip= 0;

    pageWasMarked= ps->psLastPageMarked >= ps->psPagesPrinted;

    /*
    sioOutPrintf( ps->psSos, "%% pageWasMarked= %d\n", pageWasMarked );
    sioOutPrintf( ps->psSos, "%% dcDocHasPageHeaders= %d\n",
					    dc->dcDocHasPageHeaders );
    sioOutPrintf( ps->psSos, "%% dcDocHasPageFooters= %d\n",
					    dc->dcDocHasPageFooters );
    */

    /*  1  */
    if  ( pageWasMarked			&&
	  dc->dcPostponeHeadersFooters	)
	{
	if  ( dc->dcDocHasPageHeaders				&&
	      docDrawPageHeader( bodySectBi, vps, dc, page )	)
	    { LLDEB(dc->dcDocHasPageHeaders,page);	}

	if  ( dc->dcDocHasPageFooters				&&
	      docDrawPageFooter( bodySectBi, vps, dc, page )	)
	    { LLDEB(dc->dcDocHasPageFooters,page);	}
	}

    /*  2  */

    pageIsMarked= ps->psLastPageMarked >= ps->psPagesPrinted;
    /*
    sioOutPrintf( ps->psSos, "%% pageIsMarked= %d\n", pageIsMarked );
    */

    /*  3  */
    if  ( ! pageIsMarked && ps->psPrintGeometry.pgSkipBlankPages )
	{ skip= 1;	}

    /*  4  */
    if  ( ! pageIsMarked )
	{
	const BufferDocument *		bd= dc->dcDocument;
	const DocumentProperties *	dp= &(bd->bdProperties);

	int				inExternalItem;
	ExternalItem *			ei;
	int				isEmpty;

	if  ( dc->dcDocHasPageHeaders )
	    {
	    inExternalItem= docWhatPageHeader( &ei, &isEmpty,
							bodySectBi, page, dp );
	    if  ( ! ei || ! ei->eiItem || isEmpty )
		{ pageHasHeader= 0;	}
	    }

	if  ( dc->dcDocHasPageFooters )
	    {
	    inExternalItem= docWhatPageFooter( &ei, &isEmpty,
							bodySectBi, page, dp );
	    if  ( ! ei || ! ei->eiItem || isEmpty )
		{ pageHasFooter= 0;	}
	    }
	}

    /*
    sioOutPrintf( ps->psSos, "%% pageHasHeader= %d\n", pageHasHeader );
    sioOutPrintf( ps->psSos, "%% pageHasFooter= %d\n", pageHasFooter );
    */

    if  ( ! pageIsMarked			&&
	  ps->psPrintGeometry.pgSkipEmptyPages	&&
	  ! pageHasHeader			&&
	  ! pageHasFooter			)
	{ skip= 1;	}


    if  ( skip )
	{ utilPsAbortPage( ps, page, asLast );	}
    else{ utilPsFinishPage( ps, page, asLast );	}

    return 0;
    }

static int docPsPrintStartPage(	void *				vps,
				const DocumentGeometry *	dgPage,
				DrawingContext *		dc,
				int				page )
    {
    PrintingState *	ps= (PrintingState *)vps;

    dc->dcCurrentTextAttributeSet= 0;
    dc->dcCurrentColorSet= 0;

    utilPsRefreshNupSchema( ps, dgPage );

    utilPsStartPage( ps, page );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Leave a trace in the PDF document information.			*/
/*									*/
/************************************************************************/

static void psSaveInfo(		const char *		tag,
				const unsigned char *	info,
				PrintingState *		ps )
    {
    if  ( ! info )
	{ return;	}

    sioOutPrintf( ps->psSos, "  %s (", tag );
    utilPsPrintString( ps, info, strlen( (const char *)info ) );
    sioOutPrintf( ps->psSos, ")\n" );
    }

static void psSaveDate(		const char *		tag,
				const struct tm *	tm,
				PrintingState *		ps )
    {
    char	scratch[40+1];

    if  ( tm->tm_mday == 0 )
	{ return;	}

    if  ( strftime( scratch, sizeof(scratch)- 1, "D:%Y%m%d%H%M", tm ) < 1 )
	{ LDEB(1); return;	}

    psSaveInfo( tag, (const unsigned char *)scratch, ps );

    return;
    }

/************************************************************************/
/*									*/
/*  Print a range of pages in a document.				*/
/*									*/
/************************************************************************/

static int docPsPrintPageRange(	PrintingState *		ps,
				DrawingContext *	dc,
				BufferItem *		docBi,
				int			firstPage,
				int			lastPage,
				int			asLast )
    {
    int			i;

    for ( i= 0; i < docBi->biChildCount; i++ )
	{
	if  ( docBi->biChildren[i]->biBelowPosition.lpPage >= firstPage )
	    { break;	}
	}

    if  ( i >= docBi->biChildCount )
	{ LDEB(i); return -1; }

    docPsPrintStartPage( (void *)ps,
	    &(docBi->biChildren[i]->biSectDocumentGeometry), dc, firstPage );

    if  ( ! dc->dcPostponeHeadersFooters )
	{
	docDrawPageHeader( docBi->biChildren[i], (void *)ps, dc, firstPage );
	}

    docDrawItem( docBi, (void *)ps, dc );

    if  ( lastPage < 0 )
	{ lastPage= docBi->biBelowPosition.lpPage;	}

    for ( i= docBi->biChildCount- 1; i >= 0; i-- )
	{
	if  ( docBi->biChildren[i]->biTopPosition.lpPage <= lastPage )
	    { break;	}
	}

    if  ( i < 0 )
	{ LDEB(i); return -1;	}

    docDrawFootnotesForColumn( lastPage, (void *)ps, dc );

    if  ( docDrawShapesForPage( docBi, (void *)ps, dc, lastPage ) )
	{ LDEB(1);	}

    if  ( ! dc->dcPostponeHeadersFooters )
	{
	docDrawPageFooter( docBi->biChildren[i], (void *)ps, dc, lastPage );
	}

    docPsFinishPage( (void *)ps, dc, docBi->biChildren[i],
						    lastPage, asLast );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Print a document.							*/
/*									*/
/************************************************************************/

int docPsPrintDocument(	SimpleOutputStream *		sos,
			const char *			title,
			const char *			applicationName,
			const char *			applicationReference,
			const char *			fontDirectory,
			AppDrawingData *		add,
			BufferDocument *		bd,
			const PrintGeometry *		pg,
			DOC_CLOSE_OBJECT		closeObject )
    {
    DocumentProperties *	dp= &(bd->bdProperties);
    DocumentGeometry *		dg= &(dp->dpGeometry);
    BufferItem *		docBi= &(bd->bdItem);

    PostScriptTypeList		pstl;

    DrawingContext		dc;
    PrintingState		ps;

    int				firstPage= pg->pgFirstPage;
    int				lastPage= pg->pgLastPage;

    INIT_LAYOUT_EXTERNAL	initLayoutExternal= (INIT_LAYOUT_EXTERNAL)0;

    const PostScriptFontList *	psfl= add->addPostScriptFontList;

    utilInitPostScriptFaceList( &pstl );
    pstl.pstlFontDirectory= fontDirectory;

    docInitDrawingContext( &dc );

    dc.dcPostScriptFontList= psfl;

    dc.dcSetColorRgb= docPsSetColorRgb;
    dc.dcSetFont= docPsSetFont;
    dc.dcDrawShape= docPsPrintDrawDrawingShape;

    dc.dcDrawTextLine= docPsPrintTextLine;
    dc.dcDrawParaTop= docPsPrintParaTop;
    dc.dcDrawParaBottom= docPsPrintParaBottom;
    dc.dcDrawParaLeft= docPsPrintParaLeft;
    dc.dcDrawParaRight= docPsPrintParaRight;
    dc.dcDrawParaShade= docPsPrintParaShade;
    dc.dcDrawCellTop= docPsPrintCellTop;
    dc.dcDrawCellBottom= docPsPrintCellBottom;
    dc.dcDrawCellLeft= docPsPrintCellLeft;
    dc.dcDrawCellRight= docPsPrintCellRight;
    dc.dcDrawCellShade= docPsPrintCellShade;
    dc.dcFinishPage= docPsFinishPage;
    dc.dcStartPage= docPsPrintStartPage;
    dc.dcInitLayoutExternal= initLayoutExternal;
    dc.dcCloseObject= closeObject;

    dc.dcDrawingData= add;
    dc.dcScreenFontList= (ScreenFontList *)0;
    dc.dcDocument= bd;

    dc.dcFirstPage= pg->pgFirstPage;
    dc.dcLastPage= pg->pgLastPage;
    dc.dcDrawExternalItems= 1;
    dc.dcPostponeHeadersFooters= 0;

    if  ( pg->pgOmitHeadersOnEmptyPages )
	{ dc.dcPostponeHeadersFooters= 1;	}

    appPsInitPrintingState( &ps );
    ps.psSos= sos;
    ps.psUsePostScriptFilters= pg->pgUsePostScriptFilters;
    ps.psUsePostScriptIndexedImages= pg->pgUsePostScriptIndexedImages;

    ps.psPrintGeometry.pgSkipEmptyPages= pg->pgSkipEmptyPages;
    ps.psPrintGeometry.pgSkipBlankPages= pg->pgSkipBlankPages;
    ps.psPrintGeometry.pgOmitHeadersOnEmptyPages=
					    pg->pgOmitHeadersOnEmptyPages;

    docInquireHeadersFooters( &(dc.dcDocHasPageHeaders),
				    &(dc.dcDocHasPageFooters), docBi );

    if  ( dp->dpTitle && dp->dpTitle[0] )
	{ title= (char *)dp->dpTitle;	}

    if  ( utilPsSetNupSchema( &ps, dg, pg, dc.dcDocHasPageHeaders,
						    dc.dcDocHasPageFooters ) )
	{ LDEB(1); return -1;	}

    if  ( docPsPrintGetDocumentFonts( bd, &pstl, psfl ) )
	{ LDEB(1); return -1;	}

    utilPsStartDSCDocument( &ps, &pstl,
			    title, applicationName, applicationReference );

    sioOutPrintf( sos, "%%%%BeginProlog\n" );

    sioOutPrintf( sos, "\n/mvs { moveto show } bind def\n" );

    docPsSaveTabLeaderProcedures( sos );

    appPsSetPdfmarkEmulation( sos );
    appPsSetRectfillEmulation( sos );
    appPsSetSelectfontEmulation( sos );

    appPsDefineEpsProcs( sos );

    appPsIncludeFonts( &ps, &pstl );

    appPsFontNames( &ps, &pstl, /*allFonts=*/ 0 );

    appMetaDefineProcsetPs( sos );
    docPsPrintShapeDefineProcsetPs( sos );

    sioOutPrintf( sos, "%%%%EndProlog\n" );
    sioOutPrintf( sos, "%%%%BeginSetup\n" );

    if  ( docHasDocumentInfo( dp ) )
	{
	unsigned char *			scratch;

	scratch= malloc( strlen( applicationName )+ 2+
					strlen( applicationReference )+ 2 );
	if  ( ! scratch )
	    { XDEB(scratch); return -1;	}
	sprintf( (char *)scratch,
			"%s: %s", applicationName, applicationReference );

	sioOutPrintf( sos, "[\n" );

	psSaveInfo( "/Title",		dp->dpTitle, &ps );
	psSaveInfo( "/Author",		dp->dpAuthor, &ps );
	psSaveInfo( "/Subject",		dp->dpSubject, &ps );
	psSaveInfo( "/Keywords",	dp->dpKeywords, &ps );
	psSaveInfo( "/Creator",		scratch, &ps );
	psSaveInfo( "/Producer",	scratch, &ps );

	psSaveDate( "/ModDate",		&(dp->dpRevtim), &ps );
	psSaveDate( "/CreationDate",	&(dp->dpCreatim), &ps );

	sioOutPrintf( sos, "/DOCINFO pdfmark\n\n" );

	free( scratch );
	}

    sioOutPrintf( sos, "%%%%EndSetup\n" );

    if  ( firstPage < 0 )
	{ firstPage= 0;	}

    if  ( pg->pgPrintOddSides		&&
	  pg->pgPrintEvenSides		&&
	  ! pg->pgPrintSheetsReverse	&&
	  ! pg->pgPrintBookletOrder	)
	{
	if  ( docPsPrintPageRange( &ps, &dc, docBi,
				    firstPage, lastPage, /* asLast */ 1 ) )
	    { LDEB(firstPage); return -1;	}
	}
    else{
	if  ( pg->pgPrintBookletOrder )
	    { LDEB(pg->pgPrintBookletOrder); }

	if  ( docPsPrintPageRange( &ps, &dc, docBi,
				    firstPage, lastPage, /* asLast */ 1 ) )
	    { LDEB(firstPage); return -1;	}
	}

    appPsCleanPrintingState( &ps );

    utilCleanPostScriptFaceList( &pstl );

    docResetExternalItemLayout( bd );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Booklet printing.							*/
/*									*/
/*  Document has n pages.						*/
/*  On the front (odd side) of sheet s left,right: page n-2s-1, 2s	*/
/*  On the back (even side) of sheet s left,right: page 2s-1, n-2s-2	*/
/*									*/
/************************************************************************/

