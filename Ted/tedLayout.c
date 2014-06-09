/************************************************************************/
/*									*/
/*  Geometry calculations.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<limits.h>
#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	"tedApp.h"
#   include	"tedEdit.h"
#   include	"tedLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Calculate the width of a string on screen.				*/
/*									*/
/************************************************************************/

static int tedSegmentedWidth(	const AppDrawingData *		add,
				const DrawScreenFont *		dsf,
				const unsigned char *		s,
				const int *			segments,
				int				segmentCount )
    {
    int				seg;

    int				x= 0;

    int				wide;
    int				fontAscent;
    int				fontDescent;

    for ( seg= 0; seg < segmentCount; seg++ )
	{
	if  ( segments[2* seg+ 0] > 0 )
	    {
	    appDrawTextExtents( &wide, &fontAscent, &fontDescent,
				    add, dsf->apfFontStruct,
				    (const char *)s, segments[2* seg+ 0] );

	    s += segments[2* seg+ 0];
	    x += wide;
	    }

	if  ( segments[2* seg+ 1] > 0 )
	    {
	    appDrawTextExtents( &wide, &fontAscent, &fontDescent,
				    add, dsf->apfScapsFontStruct,
				    (const char *)s, segments[2* seg+ 1] );

	    s += segments[2* seg+ 1];
	    x += wide;
	    }
	}

    return x;
    }

int tedTextWidth(		const DrawScreenFont *		dsf,
				const TextAttribute *		ta,
				const BufferDocument *		bd,
				const AppDrawingData *		add,
				const unsigned char *		printString,
				int				len )
    {
    unsigned char *		upperString= (unsigned char *)0;
    int *			segments= (int *)0;
    int				segmentCount= 0;

    int				wide;

    if  ( ta->taSmallCaps || ta->taCapitals )
	{
	if  ( docMakeCapsString( &upperString, &segments, &segmentCount,
						bd, ta, printString, len ) )
	    { LDEB(len);		}
	else{ printString= upperString;	}
	}

    if  ( ta->taSmallCaps && ! ta->taCapitals )
	{
	wide= tedSegmentedWidth( add, dsf,
				    printString, segments, segmentCount );
	}
    else{
	int		fontAscent;
	int		fontDescent;

	appDrawTextExtents( &wide, &fontAscent, &fontDescent,
		    add, dsf->apfFontStruct, (const char *)printString, len );
	}

    if  ( upperString )
	{ free( upperString );	}
    if  ( segments )
	{ free( segments );	}

    return wide;
    }

static int tedVisibleTextWidth(	const DrawScreenFont *		dsf,
				const TextAttribute *		ta,
				const BufferDocument *		bd,
				const AppDrawingData *		add,
				const unsigned char *		printString,
				int				len )
    {
    while( len > 0 && printString[len- 1] == ' ' )
	{ len--;	}

    if  ( len == 0 )
	{ return 0;	}

    return tedTextWidth( dsf, ta, bd, add, printString, len );
    }

/************************************************************************/
/*									*/
/*  Place as many particules on a line as possible.			*/
/*									*/
/*  1)  Accept at least one particule.					*/
/*  9)  Predictable undefied behaviour.					*/
/*									*/
/************************************************************************/

static int tedLayoutParticules(	const BufferItem *		bi,
				const BufferDocument *		bd,
				AppDrawingData *		add,
				const ScreenFontList *		sfl,
				const ParagraphFrame *		ffPixels,
				TextParticule *			tp,
				ParticuleData *			pd,
				int				particuleCount,
				int				x0 )
    {
    int				accepted;
    int				wide;

    int				len= 0;
    int				xFrom= x0;
    const unsigned char *	from= bi->biParaString+ tp->tpStroff;

    DrawScreenFontList *	apfl= &(add->addScreenFontList);
    int				textAttr= tp->tpTextAttributeNumber;
    int				screenFont;
    DrawScreenFont *		dsf;

    if  ( textAttr >= sfl->sflAttributeToScreenCount	||
	  sfl->sflAttributeToScreen[textAttr] < 0	)
	{ LDEB(textAttr); return -1;	}

    screenFont= sfl->sflAttributeToScreen[textAttr];
    dsf= apfl->apflFonts+ screenFont;

    accepted= 0;
    while( accepted < particuleCount )
	{
	switch( tp->tpKind )
	    {
	    int		x1;

	    case DOCkindTEXT:
		while( accepted < particuleCount		&&
		       tp->tpTextAttributeNumber == textAttr	&&
		       tp->tpKind == DOCkindTEXT		)
		    {
		    TextAttribute	ta;

		    utilGetTextAttributeByNumber( &ta,
						&(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

		    len += tp->tpStrlen;
		    wide= tedTextWidth( dsf, &ta, bd, add, from, len );

		    pd->pdVisiblePixels= tedVisibleTextWidth(
						dsf, &ta, bd, add,
						bi->biParaString+ tp->tpStroff,
						tp->tpStrlen );
		    pd->pdWhiteUnits= xFrom+ wide- x0- pd->pdVisiblePixels;

		    tp->tpX0= x0;
		    tp->tpPixelsWide= xFrom+ wide- x0;
		    x0= xFrom+ wide;

		    accepted++; tp++; pd++; continue;
		    }

		break;

	    case DOCkindTAB:
		x1= TWIPStoPIXELS( add->addMagnifiedPixelsPerTwip,
							pd->pdTabPosition );

		pd->pdVisiblePixels= 0;
		pd->pdWhiteUnits= x1- x0;
		tp->tpObjectNumber= pd->pdTabNumber;

		tp->tpX0= x0;
		if  ( x1 > x0 )
		    { tp->tpPixelsWide= x1- x0;	}
		else{ tp->tpPixelsWide= 0;	}
		x0= x1;

		accepted++; tp++; pd++; break;

	    case DOCkindOBJECT:
		{
		const InsertedObject *	io;

		io= &(bi->biParaObjects[tp->tpObjectNumber]);

		if  ( io->ioInline )
		    { wide= io->ioPixelsWide;	}
		else{ wide= 0;			}
		}

		pd->pdVisiblePixels= wide;
		pd->pdWhiteUnits= 0;

		tp->tpX0= x0;
		tp->tpPixelsWide= wide;
		x0 += wide;

		accepted++; tp++; pd++; break;

	    case DOCkindCHFTNSEP:
	    case DOCkindCHFTNSEPC:
		wide= TWIPStoPIXELS( add->addMagnifiedPixelsPerTwip, 2880 );

		pd->pdVisiblePixels= wide;
		pd->pdWhiteUnits= 0;

		tp->tpX0= x0;
		tp->tpPixelsWide= wide;
		x0 += wide;

		accepted++; tp++; pd++; break;

	    case DOCkindNOTE:
	    case DOCkindFIELDSTART:
	    case DOCkindFIELDEND:
	    case DOCkindXE:
	    case DOCkindTC:
	    case DOCkindLINEBREAK:
	    case DOCkindPAGEBREAK:
	    case DOCkindCOLUMNBREAK:

		pd->pdVisiblePixels= 0; pd->pdWhiteUnits= 0;

		tp->tpX0= x0;
		tp->tpPixelsWide= 0;

		accepted++; tp++; pd++; break;

	    default:
		LDEB(tp->tpKind); return -1;
	    }

	if  ( accepted >= particuleCount )
	    { return accepted;	}

	textAttr= tp->tpTextAttributeNumber;

	if  ( textAttr >= sfl->sflAttributeToScreenCount	||
	      sfl->sflAttributeToScreen[textAttr] < 0		)
	    { LDEB(textAttr); return -1;	}

	screenFont= sfl->sflAttributeToScreen[textAttr];
	dsf= apfl->apflFonts+ screenFont;

	from= bi->biParaString+ tp->tpStroff;
	xFrom= x0;
	len= 0;
	}

    return accepted;
    }

/************************************************************************/
/*									*/
/*  Determine the height of a line.					*/
/*									*/
/************************************************************************/

static void tedLineHeight(	const BufferItem *	bi,
				const TextParticule *	tp,
				const AppDrawingData *	add,
				const ScreenFontList *	sfl,
				int			particuleCount,
				int *			pAscent,
				int *			pDescent )
    {
    const DrawScreenFontList *	apfl= &(add->addScreenFontList);
    int				textAttr= -1;
    int				screenFont= -1;
    DrawScreenFont *		dsf;
    int				ascent= 0;
    int				descent= 0;
    int				objectHeight= 0;

    InsertedObject *		io;

    if  ( particuleCount <= 0 )
	{ LDEB(particuleCount); return;	}

    while( particuleCount > 0 )
	{
	switch( tp->tpKind )
	    {
	    case DOCkindTEXT:
	    case DOCkindTAB:
	    case DOCkindCHFTNSEP:
	    case DOCkindCHFTNSEPC:
		textAttr= tp->tpTextAttributeNumber;
		if  ( textAttr >= sfl->sflAttributeToScreenCount	||
		      sfl->sflAttributeToScreen[textAttr] < 0		)
		    { LDEB(textAttr); return;	}

		screenFont= sfl->sflAttributeToScreen[textAttr];
		dsf= apfl->apflFonts+ screenFont;

		if  ( ascent < dsf->apfAscentPixels )
		    { ascent=  dsf->apfAscentPixels;	}
		if  ( descent < dsf->apfDescentPixels )
		    { descent=  dsf->apfDescentPixels; }
		break;

	    case DOCkindOBJECT:
		textAttr= tp->tpTextAttributeNumber;
		if  ( textAttr >= sfl->sflAttributeToScreenCount	||
		      sfl->sflAttributeToScreen[textAttr] < 0		)
		    { LDEB(textAttr); return;	}

		screenFont= sfl->sflAttributeToScreen[textAttr];
		dsf= apfl->apflFonts+ screenFont;

		if  ( ascent < dsf->apfAscentPixels )
		    { ascent=  dsf->apfAscentPixels;	}
		if  ( descent < dsf->apfDescentPixels )
		    { descent=  dsf->apfDescentPixels; }

		io= bi->biParaObjects+ tp->tpObjectNumber;

		if  ( io->ioInline )
		    {
		    if  ( objectHeight < io->ioPixelsHigh )
			{ objectHeight=  io->ioPixelsHigh;	}
		    }
		break;

	    case DOCkindNOTE:
	    case DOCkindFIELDSTART:
	    case DOCkindFIELDEND:
	    case DOCkindXE:
	    case DOCkindTC:
	    case DOCkindLINEBREAK:
	    case DOCkindPAGEBREAK:
	    case DOCkindCOLUMNBREAK:
		break;

	    default:
		LDEB(tp->tpKind); break;
	    }

	tp++; particuleCount--;
	}

    if  ( ascent < objectHeight )
	{ ascent=  objectHeight; }

    *pAscent= ascent;
    *pDescent= descent;
    }

/************************************************************************/
/*									*/
/*  Justify the particules in a line of text.				*/
/*									*/
/*  1)  Start justification after the last tab of the line. Justifying	*/
/*	after anything else than a left tab is ridiculous. Simply	*/
/*	refuse to.							*/
/*  2)  Ignore organisational particules such as the delimitation of	*/
/*	links and bookmarks at the end of the line.			*/
/*									*/
/************************************************************************/

static void tedJustifyLine(	const BufferItem *	bi,
				TextParticule *		tp,
				ParticuleData *		pd,
				int			accepted,
				int			x1TextLines )
    {
    TextParticule *	tpp;
    ParticuleData *	pdd;

    int			i;
    int			extra;
    int			totalWeight;
    int			step;
    int			left;

    /*  1  */
    left= 0;
    tpp= tp; pdd= pd;
    for ( i= 0; i < accepted- 1; tpp++, pdd++, i++ )
	{
	if  ( tpp->tpKind == DOCkindTAB )
	    { left= i+ 1;	}
	}

    if  ( left > 0 )
	{
	if  ( pd[left-1].pdTabKind != DOCtaLEFT )
	    { LLLDEB(left,accepted,pd[left-1].pdTabKind); return;	}

	tp += left; pd += left; accepted -= left;
	}

    /*  2  */
    while( accepted > 0 && tp[accepted- 1].tpStrlen == 0 )
	{ accepted--;	}

    if  ( accepted < 2 )
	{ /*LDEB(accepted);*/ return;	}

    extra= x1TextLines- tp[accepted- 1].tpX0- pd[accepted- 1].pdVisiblePixels;

    totalWeight= 0;
    tpp= tp; pdd= pd;
    for ( i= 0; i < accepted- 1; tpp++, pdd++, i++ )
	{
	pdd->pdWhiteUnits= 0;
	pdd->pdCorrectBy= 0;

	if  ( bi->biParaString[tpp->tpStroff+tpp->tpStrlen-1] == ' ' )
	    {
	    pdd->pdWhiteUnits=
		sqrt( (double)pdd[0].pdWidth+ (double)pdd[1].pdVisibleWidth );

	    totalWeight += pdd->pdWhiteUnits;
	    }
	}

    left= extra;
    tpp= tp; pdd= pd;
    for ( i= 0; i < accepted- 1; tpp++, pdd++, i++ )
	{
	if  ( pdd->pdWhiteUnits > 0 )
	    {
	    step= ( extra* pdd->pdWhiteUnits )/ totalWeight;
	    if  ( step > left )
		{ step= left;	}

	    pdd->pdCorrectBy += step;

	    left -= step;
	    }
	}

    tpp= tp; pdd= pd;
    for ( i= 0; i < accepted- 1; tpp++, pdd++, i++ )
	{
	if  ( pdd->pdWhiteUnits > 0 )
	    {
	    step= 1;
	    if  ( step > left )
		{ step= left;	}

	    pdd->pdCorrectBy += step;

	    left -= step;
	    }
	}

    step= 0;
    tpp= tp; pdd= pd;
    for ( i= 0; i < accepted- 1; tpp++, pdd++, i++ )
	{
	tpp->tpX0 += step;
	if  ( pdd->pdWhiteUnits > 0 )
	    {
	    tpp->tpPixelsWide += pdd->pdCorrectBy;
	    step += pdd->pdCorrectBy;
	    }
	}

    tpp->tpX0 += step;

    return;
    }

/************************************************************************/
/*									*/
/*  Recalculate the geometry from the start of the selection to the	*/
/*  end of the line.							*/
/*									*/
/*  'part'	is known to be the first particule of the line.		*/
/*									*/
/*  1)  Count the amount of white space on the line.			*/
/*  2)  Enough to squeeze white space?					*/
/*  3)  Remove white space in proportion to the amount of white space.	*/
/*  4)  (3) rounded down: Remove some more pixels.			*/
/*  5)  Actually move and shorten the particules.			*/
/*  6)  The last particule must not be squeezed: It can have white 	*/
/*	space that protrudes beyound the right margin.			*/
/*									*/
/************************************************************************/

static void tedSqueezeParticules(	TextParticule *		tp,
					ParticuleData *		pd,
					int			accepted,
					int			x0,
					int			x1 )
    {
    TextParticule *	tpp= tp+ accepted- 1;
    ParticuleData *	pdd= pd+ accepted- 1;
    int			i;
    int			whitePixels= 0;
    int			shortage= tpp->tpX0+ pdd->pdVisiblePixels- x1;
    int			xx0;
    int			done;

    /*  1  */
    for ( i= 0; i < accepted- 1; i++ )
	{
	if  ( pd[i].pdWhiteUnits > 0 )
	    { whitePixels += pd[i].pdWhiteUnits; }

	pd[i].pdCorrectBy= 0;
	}

    /*  2  */
    if  ( whitePixels < shortage )
	{
	/*
	LLDEB(whitePixels,shortage);
	LLLDEB(x0,x1,tpp->tpX0+ pdd->pdVisiblePixels);
	LLDEB(whitePixels,tpp->tpX0+ pdd->pdVisiblePixels-x1);

	for ( i= 0; i < accepted; i++ )
	    { docListParticule( 0, "SH", part+ i, bi, tp+ i ); }
	*/
	shortage= whitePixels;
	}

    /*  3  */
    done= 0;

    for ( i= 0; i < accepted- 1; i++ )
	{
	int		step;

	if  ( pd[i].pdWhiteUnits > 0 )
	    {
	    step= ( pd[i].pdWhiteUnits* shortage )/ whitePixels;

	    if  ( step > 0 && done < shortage )
		{
		if  ( step > shortage- done )
		    { step= shortage- done;	}

		pd[i].pdCorrectBy += step;
		done += step;
		}
	    }
	}

    /*  4  */
    for ( i= 0; i < accepted- 1 && done < shortage; i++ )
	{
	if  ( pd[i].pdWhiteUnits > pd[i].pdCorrectBy )
	    { pd[i].pdCorrectBy++; done++; }
	}

    if  ( done != shortage )
	{ LLDEB(done,shortage);	}

    /*  5  */
    xx0= x0; whitePixels= 0;
    for ( i= 0; i < accepted- 1; i++ )
	{
	tp[i].tpX0 -= whitePixels;

	if  ( pd[i].pdCorrectBy > 0 )
	    {
	    tp[i].tpPixelsWide -= pd[i].pdCorrectBy;
	    whitePixels += pd[i].pdCorrectBy;
	    }
	}

    /*  6  */
    tp[i].tpX0 -= whitePixels;
    /* No: tp[i].tpPixelsWide= x1- tp[i].tpX0; */

    return;
    }

/************************************************************************/
/*									*/
/*  Layout one line on screen, reconciling PostScript and X11 geometry.	*/
/*									*/
/************************************************************************/

static int tedLayoutLine(	const BufferItem *	bi,
				const BufferDocument *	bd,
				AppDrawingData *	add,
				const ScreenFontList *	sfl,
				const ParagraphFrame *	pf,
				TextParticule *		tp,
				ParticuleData *		pd,
				int			particuleCount,
				int			x0 )
    {
    int			rval= 0;

    int			past= 0;
    int			part= 0;
    int			x1;

    int			accepted;

    while( past < particuleCount )
	{
	int		squeeze= 0;
	int		includeTab= 0;
	int		j;

	TextParticule *	tpp;
	ParticuleData *	pdd;

	j= 0;
	while( past < particuleCount && tp[j].tpKind != DOCkindTAB )
	    { j++; past++;	}

	if  ( past < particuleCount )
	    { includeTab= 1; past++;	}

	accepted= tedLayoutParticules( bi, bd,
					add, sfl, pf, tp, pd, past- part, x0 );

	if  ( accepted != past- part )
	    { LLDEB(accepted,past- part); rval= -1; goto ready;	}

	tpp= tp+ accepted- 1;
	pdd= pd+ accepted- 1;

	if  ( includeTab )
	    {
	    x1= TWIPStoPIXELS( add->addMagnifiedPixelsPerTwip,
						    pdd->pdX0+ pdd->pdWidth );

	    if  ( tpp->tpX0+ pdd->pdVisiblePixels > x1	)
		{
		/*
		LLDEB(tpp->tpX0+ tpp->tpPixelsWide,x1);
		LLDEB(tpp->tpX0+ pdd->pdVisiblePixels,x1);
		docListParticule( 0, "WIDE", part, bi, tpp );
		*/

		squeeze= 1;
		}
	    else{
		if  ( tpp->tpX0+ tpp->tpPixelsWide != x1 )
		    {
		    if  ( tpp->tpX0 > x1 )
			{
			LLDEB(tpp->tpX0+ tpp->tpPixelsWide,x1);
			tpp->tpX0= x1;
			tpp->tpPixelsWide= 0;
			}
		    else{
			tpp->tpPixelsWide= x1- tpp->tpX0;
			}
		    }
		}
	    }
	else{
	    int x1p= tpp->tpX0+ pdd->pdVisiblePixels;

	    x1= pf->pfX1TextLinesPixels;

	    if  ( x1p > x1 )
		{
		/*
		LLDEB(tpp->tpX0+ tpp->tpPixelsWide,x1);
		LLDEB(tpp->tpX0+ pdd->pdVisiblePixels,x1);
		docListParticule( 0, "WIDE", part, bi, tpp );
		*/

		/*
		int w= x1- x0;
		int wp= x1p- x0;

		if  ( w > 50 && wp > ( 115* w )/ 100 )
		    { LLDEB(wp,w);	}
		*/

		squeeze= 1;
		}
	    }

	if  ( squeeze )
	    { tedSqueezeParticules( tp, pd, accepted, x0, x1 ); }

	part += accepted; tp += accepted; pd += accepted;
	x0= x1; past= part;
	}

  ready:

    return rval;
    }

static int tedLayoutScreenLine(	TextLine *			tl,
				const BufferItem *		bi,
				const BufferDocument *		bd,
				int				part,
				int				accepted,
				AppDrawingData *		add,
				const ScreenFontList *		sfl,
				const ParagraphFrame *		pf,
				ParticuleData *			pd )
    {
    int			xShift;
    int			x1;

    int			x0Pixels;
    int			ascentPixels;
    int			descentPixels;

    TextParticule *	tp= bi->biParaParticules+ part;
    double		xfac= add->addMagnifiedPixelsPerTwip;

    /*  Does not work for right aligned paragraph numbers where 
        the number is on the left hand side of the first line 
	indent. For cetered numbers, half of the number is 
	on the left.
    if  ( part == 0 )
	{ x0Pixels= pf->pfX0FirstLinePixels;	}
    else{ x0Pixels= pf->pfX0TextLinesPixels;	}
    */

    x0Pixels= TWIPStoPIXELS( xfac, pd->pdX0 );

    if  ( tedLayoutLine( bi, bd, add, sfl, pf, tp, pd, accepted, x0Pixels ) )
	{ LDEB(1); return -1;	}

    switch( bi->biParaAlignment )
	{
	case DOCiaLEFT:
	    xShift= 0;
	    break;

	case DOCiaRIGHT:
	    x1= tp[accepted-1].tpX0+ pd[accepted-1].pdVisiblePixels;
	    xShift= pf->pfX1TextLinesPixels- x1;
	    break;

	case DOCiaCENTERED:
	    /*
	    ( x1+ x0Pixels )/ 2+ xShift ==
					( pf->pfX1Pixels+ pf->pfX0Pixels )/ 2
	    */
	    x1= tp[accepted-1].tpX0+ pd[accepted-1].pdVisiblePixels;
	    xShift= ( pf->pfX1Pixels+ pf->pfX0Pixels- x1- x0Pixels )/ 2;
	    break;

	case DOCiaJUSTIFIED:
	    xShift= 0;
	    if  ( part+ accepted < bi->biParaParticuleCount )
		{
		tedJustifyLine( bi, tp, pd, accepted,
						pf->pfX1TextLinesPixels );
		}
	    break;
	default:
	    LDEB(bi->biParaAlignment); xShift= 0; break;
	}

    if  ( xShift > 0 )
	{
	int	i;

	for ( i= 0; i < accepted; i++ )
	    { tp[i].tpX0 += xShift; }
	}

    tedLineHeight( bi, bi->biParaParticules+ part, add, sfl, accepted,
					    &ascentPixels, &descentPixels );

    return tl->tlParticuleCount;
    }

/************************************************************************/
/*									*/
/*  Derive the frame for a paragraph from the page rectangle and the	*/
/*  paragraph properties.						*/
/*									*/
/*  For paragraphs inside a table cell, geometry is derived from the	*/
/*  table column.							*/
/*									*/
/************************************************************************/

void tedParagraphFramePixels( 	ParagraphFrame *	pf,
				const AppDrawingData *	add,
				const BufferItem *	paraBi )
    {
    double		xfac= add->addMagnifiedPixelsPerTwip;

    pf->pfX0TextLinesPixels= TWIPStoPIXELS( xfac, pf->pfX0TextLinesTwips );
    pf->pfX0FirstLinePixels= TWIPStoPIXELS( xfac, pf->pfX0FirstLineTwips );
    pf->pfX1TextLinesPixels= TWIPStoPIXELS( xfac, pf->pfX1TextLinesTwips );

    if  ( pf->pfX0TextLinesPixels < pf->pfX0FirstLinePixels )
	{ pf->pfX0Pixels= pf->pfX0TextLinesPixels;	}
    else{ pf->pfX0Pixels= pf->pfX0FirstLinePixels;	}

    pf->pfX1Pixels= pf->pfX1TextLinesPixels;

    pf->pfRedrawX0Pixels= TWIPStoPIXELS( xfac, pf->pfRedrawX0Twips );
    pf->pfRedrawX1Pixels= TWIPStoPIXELS( xfac, pf->pfRedrawX1Twips );

    return;
    }

static int tedLayoutStartScreenPara(	BufferItem *		paraBi,
					const ParagraphFrame *	pf,
					AppDrawingData *	add,
					ScreenFontList *	sfl,
					BufferDocument *	bd )
    {
    int			part;
    TextParticule *	tp;

    double		xfac= add->addMagnifiedPixelsPerTwip;

    if  ( tedOpenParaScreenFonts( bd, add, sfl,
				    paraBi, 0, paraBi->biParaParticuleCount ) )
	{ LDEB(1); return -1;	}

    tp= paraBi->biParaParticules;
    for ( part= 0; part < paraBi->biParaParticuleCount; tp++, part++ )
	{
	if  ( tp->tpKind != DOCkindOBJECT )
	    { continue;	}

	if  ( 1 )
	    {
	    InsertedObject *	io= paraBi->biParaObjects+ tp->tpObjectNumber;
	    PictureProperties *	pip= &(io->ioPictureProperties);

	    if  ( tp->tpObjectNumber < 0			||
		  tp->tpObjectNumber >= paraBi->biParaObjectCount	)
		{
		LLDEB(tp->tpObjectNumber,paraBi->biParaObjectCount);
		return -1;
		}

	    if  ( io->ioKind == DOCokMACPICT )
		{
		if  ( io->ioTwipsWide == 0 )
		    { io->ioTwipsWide= 20* pip->pip_xWinExt;	}
		if  ( io->ioTwipsHigh == 0 )
		    { io->ioTwipsHigh= 20* pip->pip_yWinExt;	}
		}

	    if  ( io->ioKind == DOCokDRAWING_SHAPE )
		{
		const DrawingShape *	ds= io->ioDrawingShape;

		if  ( ! ds )
		    { LXDEB(io->ioKind,io->ioDrawingShape);	}
		else{
		    const ShapeProperties *	sp= &(ds->dsShapeProperties);

		    if  ( io->ioTwipsWide == 0 )
			{
			io->ioTwipsWide= sp->spRect.drX1-
						    sp->spRect.drX0;
			}
		    if  ( io->ioTwipsHigh == 0 )
			{
			io->ioTwipsHigh= sp->spRect.drY1-
						    sp->spRect.drY0;
			}
		    }
		}

	    if  ( io->ioTwipsWide == 0 )
		{ io->ioTwipsWide= pip->pipTwipsWide;	}
	    if  ( io->ioTwipsHigh == 0 )
		{ io->ioTwipsHigh= pip->pipTwipsHigh;	}

	    docLayoutScaleObjectToFitParagraphFrame( io, pf );

	    io->ioPixelsWide= TWIPStoPIXELS( xfac,
				( io->ioScaleXUsed* io->ioTwipsWide )/ 100 );
	    io->ioPixelsHigh= TWIPStoPIXELS( xfac,
				(  io->ioScaleYUsed* io->ioTwipsHigh )/ 100 );
	    if  ( io->ioPixelsWide < 1 )
		{ io->ioPixelsWide=  1;	}
	    if  ( io->ioPixelsHigh < 1 )
		{ io->ioPixelsHigh=  1;	}
	    }
	}

    return 0;
    }

static int tedPlaceStartScreenPara(	BufferItem *		paraBi,
					const ParagraphFrame *	pf,
					AppDrawingData *	add,
					ScreenFontList *	sfl,
					BufferDocument *	bd )
    { return 0;	}

static int tedLayoutStartScreenRow(	BufferItem *		rowBi,
					AppDrawingData *	add,
					BufferDocument *	bd )
    {
    int			i;
    CellProperties *	cp;

    double		xfac= add->addMagnifiedPixelsPerTwip;

    rowBi->biRowHalfGapWidthPixels= TWIPStoPIXELS( xfac,
						rowBi->biRowHalfGapWidthTwips );
    rowBi->biRowLeftIndentPixels= TWIPStoPIXELS( xfac,
						rowBi->biRowLeftIndentTwips );

    cp= rowBi->biRowCells;
    for ( i= 0; i < rowBi->biRowCellCount; cp++, i++ )
	{
	cp->cpRightBoundaryPixels=
			    TWIPStoPIXELS( xfac, cp->cpRightBoundaryTwips );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find/Open the screen font belonging to an attribute number.		*/
/*									*/
/************************************************************************/

int tedOpenScreenFont(	BufferDocument *		bd,
			AppDrawingData *		add,
			ScreenFontList *		sfl,
			int				attributeNumber )
    {
    if  ( attributeNumber >= sfl->sflAttributeToScreenCount )
	{
	int *	fresh;

	fresh= (int *)realloc( sfl->sflAttributeToScreen,
				    (attributeNumber+ 1)* sizeof(int) );
	if  ( ! fresh )
	    { LXDEB(attributeNumber,fresh); return -1;	}
	sfl->sflAttributeToScreen= fresh;

	fresh += sfl->sflAttributeToScreenCount;
	while( sfl->sflAttributeToScreenCount < attributeNumber )
	    { sfl->sflAttributeToScreen[sfl->sflAttributeToScreenCount++]= -1; }

	sfl->sflAttributeToScreen[sfl->sflAttributeToScreenCount++]= -1;
	}

    if  ( sfl->sflAttributeToScreen[attributeNumber] < 0 )
	{
	const DocumentFontList *	dfl= &(bd->bdProperties.dpFontList);
	TextAttribute			ta;

	utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
							    attributeNumber );

	sfl->sflAttributeToScreen[attributeNumber]=
					appOpenScreenFont( add, dfl, &ta );

	if  ( sfl->sflAttributeToScreen[attributeNumber] < 0 )
	    {
	    LLDEB(attributeNumber,sfl->sflAttributeToScreen[attributeNumber]);
	    }
	}

    return sfl->sflAttributeToScreen[attributeNumber];
    }

int tedOpenParaScreenFonts(	BufferDocument *		bd,
				AppDrawingData *		add,
				ScreenFontList *		sfl,
				BufferItem *			paraBi,
				int				partFrom,
				int				partUpto )
    {
    int			part;
    TextParticule *	tp;

    tp= paraBi->biParaParticules+ partFrom;
    for ( part= partFrom; part < partUpto; tp++, part++ )
	{
	int	textAttr= tp->tpTextAttributeNumber;

	if  ( textAttr < sfl->sflAttributeToScreenCount &&
	      sfl->sflAttributeToScreen[textAttr] >= 0	)
	    { continue;	}

	if  ( tedOpenScreenFont( bd, add, sfl, textAttr ) < 0 )
	    { LLDEB(part,textAttr); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Various layout routines.						*/
/*									*/
/************************************************************************/

static void tedSetScreenLayoutFunctions(	LayoutJob *	lj )
    {
    lj->ljLayoutScreen.slStartRow= tedLayoutStartScreenRow;
    lj->ljLayoutScreen.slStartParagraph= tedLayoutStartScreenPara;
    lj->ljLayoutScreen.slScreenFrame= tedParagraphFramePixels;
    lj->ljLayoutScreen.slLayoutLine= tedLayoutScreenLine;

    lj->ljPlaceScreen.slStartRow= (START_ROW)0;
    lj->ljPlaceScreen.slStartParagraph= tedPlaceStartScreenPara;
    lj->ljPlaceScreen.slScreenFrame= tedParagraphFramePixels;
    lj->ljPlaceScreen.slLayoutLine= (LAYOUT_SCREEN_LINE)0;

    return;
    }

int tedLayoutItem(	BufferItem *		bi,
			BufferDocument *	bd,
			AppDrawingData *	add,
			ScreenFontList *	sfl,
			DocumentRectangle *	drChanged )
    {
    LayoutJob		lj;

    docPsInitLayoutJob( &lj );

    if  ( add->addForScreenDrawing )
	{ tedSetScreenLayoutFunctions( &lj );	}

    docItemLayoutStartPosition( &(lj.ljPosition), bi );

    lj.ljChangedRectanglePixels= drChanged;
    lj.ljAdd= add;
    lj.ljBd= bd;
    lj.ljScreenFontList= sfl;
    lj.ljChangedItem= bi;

    if  ( docLayoutItemAndParents( bi, &lj ) )
	{ LDEB(1); return -1;	}

    docPsCleanLayoutJob( &lj );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Calculate the layout of a page header.				*/
/*									*/
/************************************************************************/

int tedInitLayoutExternalItem(	LayoutJob *			lj,
				ExternalItem *			ei,
				int				page )
    {
    AppDrawingData *	add= lj->ljAdd;

    if  ( add->addForScreenDrawing )
	{ tedSetScreenLayoutFunctions( lj );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Redo the layout of a paragraph because its contents have changed	*/
/*  during editing.							*/
/*									*/
/************************************************************************/

int tedAdjustParagraphLayout(	EditOperation *			eo,
				AppDrawingData *		add,
				BufferItem *			bi,
				int				line,
				int				stroffShift,
				int				upto )
    {
    LayoutJob		lj;

    if  ( add->addDrawable )
	{ tedSetScreenLayoutFunctions( &lj );	}

    /*  bogus  */
    lj.ljPosition.lpPage= 0;
    lj.ljPosition.lpPageYTwips= 0;

    if  ( ! eo->eoChangedRectSet )
	{ LDEB(eo->eoChangedRectSet);	}

    lj.ljChangedRectanglePixels= &(eo->eoChangedRect);
    lj.ljAdd= add;
    lj.ljBd= eo->eoBd;
    lj.ljScreenFontList= eo->eoScreenFontList;
    lj.ljChangedItem= bi;

    if  ( docAdjustParaLayout( bi, line, stroffShift, upto, &lj ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Verify that the root of a selection is formatted for the current	*/
/*  page, if not format it.						*/
/*									*/
/************************************************************************/

static int tedCheckPageOfSelectedNoteSeparator(
					int *			pChanged,
					int *			pY0Twips,
					BufferDocument *	bd,
					AppDrawingData *	add,
					const ExternalItem *	selRootEi,
					int			noteItKind,
					int			sepItKind )
    {
    int			notesIndex;
    DocumentNote *	dnFirstNote;
    ExternalItem *	eiNoteSep;
    int			y0Twips;

    DocumentRectangle	drExtern;

    notesIndex= docGetFirstNoteOnPage( &dnFirstNote, bd, 
			selRootEi->eiPageSelectedUpon, noteItKind );
    if  ( notesIndex < 0 )
	{ LLDEB(selRootEi->eiPageSelectedUpon,notesIndex); return -1; }

    if  ( docNoteSeparatorRectangle( &drExtern, &eiNoteSep,
			    &y0Twips, bd, dnFirstNote, sepItKind, add ) )
	{ LDEB(1); return -1;	}

    *pY0Twips= y0Twips;
    return 0;
    }

int tedCheckPageOfSelectedExtItem(
				int *				pChanged,
				DocumentRectangle *		drChanged,
				BufferDocument *		bd,
				ExternalItem *			selRootEi,
				AppDrawingData *		add,
				ScreenFontList *		sfl )
    {
    int				y0Twips;
    BufferItem *		selRootBodySectBi= (BufferItem *)0;

    if  ( ! selRootEi->eiItem )
	{ XDEB(selRootEi->eiItem); return -1;	}

    switch( selRootEi->eiItem->biInExternalItem )
	{
	case DOCinBODY:
	    LDEB(selRootEi->eiItem->biInExternalItem);
	    return -1;

	case DOCinSECT_HEADER:
	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:

	case DOCinSECT_FOOTER:
	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:

	    if  ( selRootEi->eiItem )
		{
		BufferItem *		docBi= &(bd->bdItem);

		const SelectionScope *	ss;

		ss= &(selRootEi->eiItem->biSectSelectionScope);
		selRootBodySectBi= docBi->biChildren[ss->ssSectNrExternalTo];
		}

	    y0Twips= selRootEi->eiY0UsedTwips;
	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:
	    *pChanged= 0;
	    return 0;

	case DOCinFTNSEP:

	    if  ( selRootEi->eiPageSelectedUpon < 0 )
		{ LDEB(selRootEi->eiPageSelectedUpon); return -1;	}

	    if  ( selRootEi->eiPageFormattedFor ==
					    selRootEi->eiPageSelectedUpon )
		{ *pChanged= 0; return 0;	}

	    if  ( tedCheckPageOfSelectedNoteSeparator( pChanged, &y0Twips,
						bd, add, selRootEi,
						DOCinFOOTNOTE, DOCinFTNSEP ) )
		{ LDEB(1); return -1;	}

	    break;

	case DOCinFTNSEPC:
	case DOCinFTNCN:
	    LDEB(selRootEi->eiItem->biInExternalItem);
	    return -1;

	case DOCinAFTNSEP:

	    if  ( selRootEi->eiPageSelectedUpon < 0 )
		{ LDEB(selRootEi->eiPageSelectedUpon); return -1;	}

	    if  ( selRootEi->eiPageFormattedFor ==
					    selRootEi->eiPageSelectedUpon )
		{ *pChanged= 0; return 0;	}

	    if  ( tedCheckPageOfSelectedNoteSeparator( pChanged, &y0Twips,
						bd, add, selRootEi,
						DOCinENDNOTE, DOCinAFTNSEP ) )
		{ LDEB(1); return -1;	}

	    break;

	case DOCinAFTNSEPC:
	case DOCinAFTNCN:
	    LDEB(selRootEi->eiItem->biInExternalItem);
	    return -1;

	default:
	    LDEB(selRootEi->eiItem->biInExternalItem);
	    return -1;
	}

    if  ( selRootEi->eiPageSelectedUpon < 0 )
	{ LDEB(selRootEi->eiPageSelectedUpon); return -1;	}

    if  ( selRootEi->eiPageFormattedFor == selRootEi->eiPageSelectedUpon )
	{ *pChanged= 0; return 0;	}

    if  ( docLayoutExternalItem( selRootEi, drChanged,
		selRootEi->eiPageSelectedUpon, y0Twips,
		bd, selRootBodySectBi, add, sfl,
		tedInitLayoutExternalItem, tedCloseObject ) )
	{ LDEB(selRootEi->eiPageSelectedUpon); return -1; }

    *pChanged= 1; return 0;
    }

