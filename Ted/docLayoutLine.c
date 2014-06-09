/************************************************************************/
/*									*/
/*  Layout of a document. Do the layout inside lies of text.		*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<utilMatchFont.h>

#   include	<appImage.h>
#   include	"docLayout.h"
#   include	<appWinMeta.h>
#   include	<appMacPict.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Get descent/ascent for the font of a special particule.		*/
/*									*/
/************************************************************************/

static int docLayoutSpecialExtents(
				const BufferDocument *		bd,
				const BufferItem *		bi,
				int *				pWordAscent,
				int *				pWordDescent,
				const PostScriptFontList *	psfl,
				const TextParticule *		tp )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);
    const DocumentFontList *	dfl= &(dp->dpFontList);

    AfmFontInfo *		afi;
    int				encoding;
    TextAttribute		ta;

    int				sizeTwips;

    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

    afi= utilPsGetAfi( &encoding, dfl, psfl, &ta );
    if  ( ! afi )
	{ XDEB(afi); return -1;	}

    sizeTwips= 10* ta.taFontSizeHalfPoints;

    *pWordAscent= ( sizeTwips* afi->afiFontBBox.abbTop+ 500 ) / 1000;
    *pWordDescent= *pWordAscent- sizeTwips;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Calculate the layout of the next stretch of text upto the point	*/
/*  where it can be folded.						*/
/*									*/
/************************************************************************/

static int docLayoutSegmentedStringExtents(
				AfmBBox *		abbRet,
				const unsigned char *	s,
				const int *		segments,
				int			segmentCount,
				int			sizeTwips,
				int			withKerning,
				int			encoding,
				const AfmFontInfo *	afi )
    {
    int			seg;
    int			scapsSize= ( 8* sizeTwips )/ 10;

    int			x= 0;
    int			wide;

    AfmBBox		abb;
    int			done= 0;

    for ( seg= 0; seg < segmentCount; seg++ )
	{
	if  ( segments[2* seg+ 0] > 0 )
	    {
	    wide= psCalculateStringExtents( &abb, s, segments[2* seg+ 0],
				    sizeTwips, withKerning, encoding, afi );

	    if  ( done )
		{
		abbRet->abbRight= x+ abb.abbRight;

		if  ( abbRet->abbTop < abb.abbTop )
		    { abbRet->abbTop=  abb.abbTop;	}
		if  ( abbRet->abbBottom < abb.abbBottom )
		    { abbRet->abbBottom=  abb.abbBottom;	}
		}
	    else{ *abbRet= abb;	}

	    s += segments[2* seg+ 0];
	    x += wide;
	    done= 1;
	    }

	if  ( segments[2* seg+ 1] > 0 )
	    {
	    wide= psCalculateStringExtents( &abb, s, segments[2* seg+ 1],
				    scapsSize, withKerning, encoding, afi );

	    if  ( done )
		{
		abbRet->abbRight= x+ abb.abbRight;

		if  ( abbRet->abbTop < abb.abbTop )
		    { abbRet->abbTop=  abb.abbTop;	}
		if  ( abbRet->abbBottom < abb.abbBottom )
		    { abbRet->abbBottom=  abb.abbBottom;	}
		}
	    else{ *abbRet= abb;	}

	    s += segments[2* seg+ 1];
	    x += wide;
	    done= 1;
	    }
	}

    return x;
    }

static int docLayoutStringExtents(	int *			pWidth,
					int *			pDecWidth,
					AfmBBox *		abb,
					const TextAttribute *	ta,
					int			sizeTwips,
					int			encoding,
					const BufferDocument *	bd,
					const AfmFontInfo *	afi,
					const unsigned char *	printString,
					int			len )
    {
    unsigned char *		upperString= (unsigned char *)0;
    int *			segments= (int *)0;
    int				segmentCount= 0;

    int				rval= 0;
    const int			withKerning= 0;

    int				dec;
    int				d;

    int				width;
    int				decWidth;

    if  ( len > 0 && ( ta->taSmallCaps || ta->taCapitals ) )
	{
	if  ( docMakeCapsString( &upperString, &segments, &segmentCount,
						bd, ta, printString, len ) )
	    { LDEB(len); return -1;	}

	printString= upperString;
	}

    if  ( len > 0 && ta->taSmallCaps && ! ta->taCapitals )
	{
	width= docLayoutSegmentedStringExtents( abb, printString,
				    segments, segmentCount,
				    sizeTwips, withKerning, encoding, afi );
	if  ( width < 0 )
	    { LLDEB(ta->taFontNumber,width);	}
	}
    else{
	width= psCalculateStringExtents( abb, printString, len,
				    sizeTwips, withKerning, encoding, afi );
	if  ( width < 0 )
	    { LLDEB(ta->taFontNumber,width);	}
	}

    dec= -1;
    for ( d= 0; d < len; d++ )
	{
	if  ( printString[d] == '.' || printString[d] == ','	)
	    { dec= d;	}
	}

    if  ( dec >= 0 )
	{
	AfmBBox		abbDec;

	decWidth= psCalculateStringExtents( &abbDec, printString, dec,
				    sizeTwips, withKerning, encoding, afi );
	if  ( decWidth < 0 )
	    { LDEB(decWidth);	}
	}
    else{ decWidth= width;	}

    *pWidth= width;
    *pDecWidth= decWidth;

    if  ( upperString )
	{ free( upperString );	}
    if  ( segments )
	{ free( segments );	}

    return rval;
    }

static int docLayoutWord(	const BufferDocument *		bd,
				const BufferItem *		bi,
				int				part,
				int *				pX1,
				int *				pVisibleX1,
				int *				pWordAscent,
				int *				pWordDescent,
				const PostScriptFontList *	psfl,
				ParticuleData *			pd,
				int				particuleCount,
				int				x0 )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);
    const DocumentFontList *	dfl= &(dp->dpFontList);

    int				particuleAscent;
    int				particuleDescent;
    int				wordAscent= 0;
    int				wordDescent= 0;

    int				accepted;
    int				width;
    int				decWidth;

    int				len= 0;
    int				xFrom= x0;

    int				x1= x0;
    int				visibleX1= x0;

    const TextParticule *	tp= bi->biParaParticules+ part;
    const unsigned char *	from= bi->biParaString+ tp->tpStroff;
    int				textAttrNumber= tp->tpTextAttributeNumber;

    TextAttribute		taPr;
    TextAttribute		taNx;

    accepted= 0;
    while( accepted < particuleCount && tp->tpKind == DOCkindTEXT )
	{
	const AfmFontInfo *	afi;
	int			encoding;
	TextAttribute		ta;

	utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

	afi= utilPsGetAfi( &encoding, dfl, psfl, &ta );
	if  ( ! afi )
	    {
	    LDEB(psfl->psflFamilyCount);
	    LDEB(docNumberOfParagraph(bi));
	    LDEB(tp->tpTextAttributeNumber);
	    SXDEB((char *)bi->biParaString+tp->tpStroff,afi);

	    return -1;
	    }

	while( accepted < particuleCount			&&
	       tp->tpTextAttributeNumber == textAttrNumber	&&
	       tp->tpKind == DOCkindTEXT			)
	    {
	    AfmBBox		abb;
	    int			sizeTwips;

	    sizeTwips= 10* ta.taFontSizeHalfPoints;

	    if  ( ta.taSuperSub == DOCfontSUPERSCRIPT ||
		  ta.taSuperSub == DOCfontSUBSCRIPT   )
		{ sizeTwips= SUPERSUB_SIZE( sizeTwips ); }

	    len += tp->tpStrlen;

	    if  ( docLayoutStringExtents( &width, &decWidth, &abb,
			    &ta, sizeTwips, encoding, bd, afi, from, len ) )
		{ LDEB(1); return -1;	}

	    x1= xFrom+ width;
	    visibleX1= xFrom+ ( abb.abbRight- abb.abbLeft );

	    pd->pdAfi= afi;
	    pd->pdX0= x0;
	    pd->pdWidth= xFrom+ width- x0;
	    pd->pdDecWidth= xFrom+ decWidth- x0;
	    pd->pdVisibleWidth= xFrom+ ( abb.abbRight- abb.abbLeft )- x0;
	    pd->pdTabNumber= -1;
	    pd->pdTabKind= -1;
	    pd->pdTabPosition= 0;
	    pd->pdVisiblePixels= 0;
	    pd->pdWhiteUnits= 0;
	    pd->pdCorrectBy= 0;
	    x0= x1;

	    particuleAscent= abb.abbTop;
	    particuleDescent= abb.abbBottom;

	    /*
	    fontAscent= ( sizeTwips* afi->afiFontBBox.abbTop+ 500 ) / 1000;
	    fontDescent= fontAscent- sizeTwips;
	    */

	    if  ( wordAscent < particuleAscent )
		{ wordAscent=  particuleAscent;	}
	    if  ( wordDescent > particuleDescent )
		{ wordDescent=  particuleDescent; }

	    accepted++; part++; tp++; pd++;

	    if  ( ( len > 0 && from[len- 1] == ' ' )	||
		  accepted >= particuleCount		)
		{ break;	}

	    utilGetTextAttributeByNumber( &taNx, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

	    if  ( ! taNx.taFontIsSlanted	&&
		  ta.taFontIsSlanted		)
		{
		int		extra;

		if  ( ta.taFontSizeHalfPoints < taNx.taFontSizeHalfPoints )
		    { extra= ta.taFontSizeHalfPoints;		}
		else{ extra= taNx.taFontSizeHalfPoints;		}

		extra = (int)( -afi->afiTanItalicAngle* 10* extra );

		x0 += extra;
		}
	    }

	if  ( ( len > 0 && from[len- 1] == ' ' )	||
	      accepted >= particuleCount		)
	    { break;	}

	utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

	textAttrNumber= tp->tpTextAttributeNumber;
	from= bi->biParaString+ tp->tpStroff;
	xFrom= x0;
	len= 0;
	}

    if  ( part < bi->biParaParticuleCount )
	{
	utilGetTextAttributeByNumber( &taPr, &(bd->bdTextAttributeList),
						tp[-1].tpTextAttributeNumber );
	utilGetTextAttributeByNumber( &taNx, &(bd->bdTextAttributeList),
						tp[0 ].tpTextAttributeNumber );

	if  ( accepted < particuleCount	&&
	      ! taNx.taFontIsSlanted	&&
	      taPr.taFontIsSlanted	)
	    {
	    int				extra;
	    AfmFontInfo *		afi;
	    int				encoding;

	    const TextAttribute *	taExtra;

	    if  ( taPr.taFontSizeHalfPoints < taNx.taFontSizeHalfPoints )
		{ taExtra= &taPr; }
	    else{ taExtra= &taNx; }

	    afi= utilPsGetAfi( &encoding, dfl, psfl, taExtra );
	    if  ( ! afi )
		{ XDEB(afi); return -1;	}

	    extra= (int) ( -afi->afiTanItalicAngle*
					10* taExtra->taFontSizeHalfPoints );

	    x1 += extra;
	    }
	}

    *pWordAscent= wordAscent;
    *pWordDescent= wordDescent;
    *pX1= x1;
    *pVisibleX1= visibleX1;

    return accepted;
    }

/************************************************************************/
/*									*/
/*  Place as many particules on a line as possible, but stop on a tab.	*/
/*									*/
/************************************************************************/

#   define	PSfoundNOTHING		0
#   define	PSfoundTAB		1
#   define	PSfoundLINEFULL		2
#   define	PSfoundLINEBREAK	3
#   define	PSfoundPAGEBREAK	4
#   define	PSfoundBULLET_END	5

static int docPsLayoutText(	const BufferDocument *		bd,
				const BufferItem *		bi,
				int				part,
				int *				pFound,
				int *				pWordCount,
				int *				pX1,
				int *				pTextAscent,
				int *				pTextDescent,
				int *				pHasInline,
				const ParagraphFrame *		pf,
				const PostScriptFontList *	psfl,
				ParticuleData *			pd,
				int				particuleCount,
				int				acceptAtLeast,
				int				x0 )
    {
    int				x1TextLines= pf->pfX1TextLinesTwips;

    int				textAscent= 0;
    int				textDescent= 0;

    int				accepted;
    int				found= PSfoundNOTHING;
    int				wordCount= 0;
    int				hasInline= 0;

    const TextParticule *	tp= bi->biParaParticules+ part;

    accepted= 0;
    while( accepted < particuleCount && found == PSfoundNOTHING )
	{
	int		done;

	switch( tp->tpKind )
	    {
	    int			x1;
	    int			visibleX1;

	    int			wordAscent;
	    int			wordDescent;

	    case DOCkindTEXT:
	    textCase:
		done= docLayoutWord( bd, bi, part, &x1, &visibleX1,
						&wordAscent, &wordDescent,
						psfl, pd,
						particuleCount- accepted, x0 );
		if  ( done < 1 )
		    { LDEB(done); return -1;	}
			    
		if  ( visibleX1 >= x1TextLines		&&
		      accepted >= acceptAtLeast		)
		    {
		    *pFound= PSfoundLINEFULL;
		    *pWordCount= wordCount;
		    *pX1= x0;
		    *pTextAscent= textAscent;
		    *pTextDescent= textDescent;
		    return accepted;
		    }

		if  ( textAscent < wordAscent )
		    { textAscent=  wordAscent;		}
		if  ( textDescent > wordDescent )
		    { textDescent=  wordDescent;	}

		wordCount++;
		hasInline= 1;
		accepted += done; part += done; tp += done; pd += done;
		x0= x1;
		break;

	    case DOCkindFIELDSTART:
		done= 0;
		while( accepted < particuleCount	&&
		       tp->tpKind == DOCkindFIELDSTART	)
		    {
		    pd->pdAfi= (AfmFontInfo *)0;
		    pd->pdX0= x0;
		    pd->pdWidth= 0;
		    pd->pdDecWidth= 0;
		    pd->pdVisibleWidth= 0;
		    pd->pdTabNumber= -1;
		    pd->pdTabKind= -1;
		    pd->pdTabPosition= 0;
		    pd->pdVisiblePixels= 0;
		    pd->pdWhiteUnits= 0;
		    pd->pdCorrectBy= 0;

		    if  ( accepted < acceptAtLeast )
			{ acceptAtLeast++;	}

		    accepted++; part++; tp++; pd++;
		    }

		if  ( accepted < particuleCount		&&
		      tp->tpKind == DOCkindTEXT		)
		    { goto textCase;	}

		break;

	    case DOCkindTAB:
		hasInline= 1;
		found= PSfoundTAB;
		break;

	    case DOCkindNOTE:
	    case DOCkindFIELDEND:
	    case DOCkindXE:
	    case DOCkindTC:
		pd->pdAfi= (AfmFontInfo *)0;
		pd->pdX0= x0;
		pd->pdWidth= 0;
		pd->pdDecWidth= 0;
		pd->pdVisibleWidth= 0;
		pd->pdTabNumber= -1;
		pd->pdTabKind= -1;
		pd->pdTabPosition= 0;
		pd->pdVisiblePixels= 0;
		pd->pdWhiteUnits= 0;
		pd->pdCorrectBy= 0;

		accepted++; part++; tp++; pd++; break;

	    case DOCkindOBJECT:
		{
		InsertedObject *	io;
		int			width= 0;

		io= bi->biParaObjects+ tp->tpObjectNumber;

		if  ( io->ioInline )
		    {
		    docLayoutScaleObjectToFitParagraphFrame( io, pf );

		    width= ( io->ioScaleXUsed* io->ioTwipsWide )/ 100.0;

		    if  ( x0+ width >= x1TextLines		&&
			  accepted >= acceptAtLeast		)
			{
			*pFound= PSfoundLINEFULL;
			*pWordCount= wordCount;
			*pX1= x0;
			*pTextAscent= textAscent;
			*pTextDescent= textDescent;
			return accepted;
			}

		    wordAscent= ( io->ioScaleYUsed* io->ioTwipsHigh )/ 100.0;
		    if  ( textAscent < wordAscent )
			{ textAscent=  wordAscent;	}
		    hasInline= 1;
		    }

		pd->pdAfi= (AfmFontInfo *)0;
		pd->pdX0= x0;
		pd->pdWidth= width;
		pd->pdDecWidth= width;
		pd->pdVisibleWidth= width;
		pd->pdTabNumber= -1;
		pd->pdTabKind= -1;
		pd->pdTabPosition= 0;
		pd->pdVisiblePixels= 0;
		pd->pdWhiteUnits= 0;
		pd->pdCorrectBy= 0;
		x0 += width;

		accepted++; part++; tp++; pd++; break;
		}

	    case DOCkindCHFTNSEP:
	    case DOCkindCHFTNSEPC:
		{
		int	width= 2880;

		if  ( x0+ width >= x1TextLines		&&
		      accepted >= acceptAtLeast		)
		    {
		    *pFound= PSfoundLINEFULL;
		    *pWordCount= wordCount;
		    *pX1= x0;
		    *pTextAscent= textAscent;
		    *pTextDescent= textDescent;
		    return accepted;
		    }

		if  ( docLayoutSpecialExtents( bd, bi,
				    &wordAscent, &wordDescent, psfl, tp ) )
		    { LDEB(1); return -1;	}

		if  ( textAscent < wordAscent )
		    { textAscent=  wordAscent;		}
		if  ( textDescent > wordDescent )
		    { textDescent=  wordDescent;	}

		pd->pdAfi= (AfmFontInfo *)0;
		pd->pdX0= x0;
		pd->pdWidth= width;
		pd->pdDecWidth= width;
		pd->pdVisibleWidth= width;
		pd->pdTabNumber= -1;
		pd->pdTabKind= -1;
		pd->pdTabPosition= 0;
		pd->pdVisiblePixels= 0;
		pd->pdWhiteUnits= 0;
		pd->pdCorrectBy= 0;
		x0 += width;

		hasInline= 1;
		accepted++; part++; tp++; pd++; break;
		}

	    case DOCkindLINEBREAK:
	    case DOCkindPAGEBREAK:
	    case DOCkindCOLUMNBREAK:
		pd->pdAfi= (AfmFontInfo *)0;
		pd->pdX0= x0;
		pd->pdWidth= 0;
		pd->pdDecWidth= 0;
		pd->pdVisibleWidth= 0;
		pd->pdTabNumber= -1;
		pd->pdTabKind= -1;
		pd->pdTabPosition= 0;
		pd->pdVisiblePixels= 0;
		pd->pdWhiteUnits= 0;
		pd->pdCorrectBy= 0;

		if  ( tp->tpKind != DOCkindLINEBREAK	&&
		      ! bi->biParaInTable		)
		    { found= PSfoundPAGEBREAK;	}
		else{ found= PSfoundLINEBREAK;	}

		accepted++; part++; tp++; pd++; break;

	    default:
		LDEB(tp->tpKind); return -1;
	    }
	}

    *pFound= found;
    *pWordCount= wordCount;
    *pX1= x0;
    *pTextAscent= textAscent;
    *pTextDescent= textDescent;
    *pHasInline= hasInline;

    return accepted;
    }

/************************************************************************/
/*									*/
/*  Align a strech of text to the tab before it.			*/
/*									*/
/************************************************************************/

static int docLayoutAlignTextToTab(
				ParticuleData *		pdd,
				int			done,
				ParticuleData *		pdTab,
				int			tabKind,
				int *			pX1,
				const int		tabX0,
				const int		tabX1 )
    {
    int		visibleSinceTab= 0;
    int		shift= 0;

    int		x1= *pX1;

    if  ( done > 0 )
	{
	visibleSinceTab=
		    pdd[done-1].pdX0+ pdd[done-1].pdVisibleWidth- tabX0;
	}

    switch( tabKind )
	{
	case DOCtaLEFT:
	    shift= 0; return 0;

	case DOCtaDECIMAL:
	    if  ( done > 0 && pdTab )
		{
		int		d;

		for ( d= done- 1; d >= 0; d-- )
		    {
		    if  ( pdd[d].pdDecWidth <  pdd[d].pdWidth )
			{
			visibleSinceTab=
				    pdd[d].pdX0+ pdd[d].pdDecWidth- tabX0;
			break;
			}
		    }

		shift= tabX1- visibleSinceTab- tabX0;
		if  ( shift < 0 )
		    { LDEB(shift); shift= 0;	}
		else{ x1= tabX1;			}
		pdTab->pdWidth= shift;
		pdTab->pdVisibleWidth= 0;
		}
	    else{
		if  ( ! pdTab )
		    { LXDEB(done,pdTab);	}
		else{
		    x1= tabX1;
		    pdTab->pdWidth= tabX1- tabX0;
		    pdTab->pdVisibleWidth= 0;
		    }
		}
	    break;

	case DOCtaRIGHT:
	    if  ( done > 0 && pdTab )
		{
		/*  6  */
		shift= tabX1- visibleSinceTab- tabX0;
		if  ( shift < 0 )
		    { shift= 0;	}
		else{ x1= tabX1;			}
		if  ( pdTab )
		    {
		    pdTab->pdWidth= shift;
		    pdTab->pdVisibleWidth= 0;
		    }
		}
	    else{
		if  ( ! pdTab )
		    { LXDEB(done,pdTab);	}
		else{
		    x1= tabX1;
		    pdTab->pdWidth= tabX1- tabX0;
		    pdTab->pdVisibleWidth= 0;
		    }
		}
	    break;

	case DOCtaCENTER:
	    if  ( done > 0 && pdTab )
		{
		/*  6  */
		shift= tabX1- visibleSinceTab/ 2- tabX0;
		if  ( shift < 0 )
		    { shift= 0;	}
		else{ x1 += shift;			}
		pdTab->pdWidth= shift;
		pdTab->pdVisibleWidth= 0;
		}
	    else{
		if  ( pdTab )
		    {
		    x1= tabX1;
		    pdTab->pdWidth= tabX1- tabX0;
		    pdTab->pdVisibleWidth= 0;
		    }
		}
	    break;

	default:
	    LDEB(tabKind);
	    shift= 0; break;
	}

    if  ( shift > 0 )
	{
	int		i;

	for ( i= 0; i < done; i++ )
	    { pdd[i].pdX0 += shift; }
	}

    *pX1= x1;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Handle the bullet of a list as if it were aligned to a tab.		*/
/*									*/
/*  Do two things:							*/
/*  1)  Report the alignment of the bullet as the settings of a tab	*/
/*	before any text in the paragraph.				*/
/*  2)  Report the end of the bullet as a temporary limit for the text	*/
/*	formatter to alow for shifting the bullet text as a piece of	*/
/*	text between bullets.						*/
/*									*/
/************************************************************************/

static int docLayoutBulletAsTab(	int *			pParticuleUpto,
					int *			pX0,
					int *			pTabX0,
					int *			pTabX1,
					int *			pTabParticule,
					int *			pTabKind,
					const ParagraphFrame *	pf,
					const BufferItem *	paraBi,
					const BufferDocument *	bd )
    {
    ListOverride *		lo;
    DocumentList *		dl;
    ListNumberTreeNode *	root;
    const DocumentListLevel *	dll;

    int * const			startPath= (int *)0;
    int * const			formatPath= (int *)0;

    int				particuleUpto= *pParticuleUpto;
    int				x0= *pX0;
    int				tabX0= *pTabX0;
    int				tabX1= *pTabX1;
    int				tabParticule= *pTabParticule;
    int				tabKind= *pTabKind;

    if  ( docGetListOfParagraph( &lo, &root, &dl,
					paraBi->biParaListOverride, bd ) )
	{ LDEB(paraBi->biParaListOverride);	}
    else{
	if  ( docListGetFormatPath( startPath, formatPath, &dll,
				    paraBi->biParaListLevel, dl, lo ) )
	    { LDEB(paraBi->biParaListLevel);	}
	else{
	    switch( dll->dllJustification )
		{
		case DOCllaLEFT:
		    /*  b  */
		    tabKind= DOCtaLEFT;
		    break;

		case DOCllaRIGHT:
		    tabKind= DOCtaRIGHT;
		    tabParticule= 0;
		    tabX0= x0= pf->pfX0GeometryTwips;
		    tabX1= pf->pfX0FirstLineTwips;
		    break;

		case DOCllaCENTER:
		    tabKind= DOCtaCENTER;
		    tabParticule= 0;
		    tabX0= x0= pf->pfX0GeometryTwips;
		    tabX1= pf->pfX0FirstLineTwips;
		    break;

		default:
		    LDEB(dll->dllJustification);
		    break;
		}

	    if  ( dll->dllFollow != DOCllfTAB	&&
		  tabKind != DOCtaLEFT		)
		{
		int			fieldNr;
		int			partBegin;
		int			partEnd;
		int			stroffBegin;
		int			stroffEnd;

		if  ( docDelimitParaHeadField( &fieldNr,
					    &partBegin, &partEnd,
					    &stroffBegin, &stroffEnd,
					    paraBi, bd ) )
		    { LDEB(1);			}
		else{ particuleUpto= partEnd+ 1;	}
		}
	    }
	}

    *pParticuleUpto= particuleUpto;
    *pX0= x0;
    *pTabX0= tabX0;
    *pTabX1= tabX1;
    *pTabParticule= tabParticule;
    *pTabKind= tabKind;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Place as many particules on a line as possible. The caller can	*/
/*  require that a certain number of particules are accepted even if	*/
/*  these do not fit on the line. This is to prevent loops. A formatter	*/
/*  typically wants to be sure that at least one particule is accepted,	*/
/*  such that it can know that it actually advances.			*/
/*									*/
/*  a)  Paragraph numbers are aligned as if there is a tab at the	*/
/*	first line indent position. The kind of tab depends on the	*/
/*	alignment of the list level.					*/
/*  b)  As formatting starts at that position anyway, there is no need	*/
/*	to do anything special for left aligned bullets or paragraph	*/
/*	numbers.							*/
/*									*/
/*  1)  Negative shifts occur when the text to the right of the tab	*/
/*	does not fit.							*/
/*									*/
/************************************************************************/

static int docLayoutParticules( const BufferDocument *		bd,
				const BufferItem *		paraBi,
				int				part,
				int *				pFound,
				int *				pWordCount,
				int *				pLineAscent,
				int *				pLineDescent,
				int *				pHasInline,
				const ParagraphFrame *		pf,
				const PostScriptFontList *	psfl,
				ParticuleData *			pd,
				int				particuleCount,
				int				acceptAtLeast,
				int				x0 )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);
    const DocumentFontList *	dfl= &(dp->dpFontList);

    int				lineAscent= 0;
    int				lineDescent= 0;

    int				accepted;
    int				wordCount= 0;
    int				hasInline= 0;

    int				tabKind= DOCtaLEFT;
    int				tabX0= 0;
    int				tabX1= 0;
    int				tabParticule= -1;
    int				particuleUpto= particuleCount;

    const TextParticule *	tpp= paraBi->biParaParticules+ part;
    ParticuleData *		pdd= pd;
    TabStop			ts;

    TextAttribute		ta;

    /*  a  */
    if  ( part == 0 && paraBi->biParaListOverride > 0 )
	{
	if  ( docLayoutBulletAsTab( &particuleUpto,
					&x0, &tabX0, &tabX1,
					&tabParticule, &tabKind,
					pf, paraBi, bd ) )
	    { LDEB(1);	}
	}

    accepted= 0;
    while( accepted < particuleCount )
	{
	int		done;
	int		found;

	int		textAscent;
	int		textDescent;
	int		textWordCount;

	int		x1;
	AfmFontInfo *	afi;
	int		encoding;
	int		tab;

	ParticuleData *	pdTab= (ParticuleData *)0;
	if  ( tabParticule >= 0 )
	    { pdTab= pd+ tabParticule;	}

	if  ( accepted >= acceptAtLeast )
	    { acceptAtLeast= 0;	}

	done= docPsLayoutText( bd, paraBi, part, &found, &textWordCount, &x1,
				    &textAscent, &textDescent, &hasInline,
				    pf, psfl, pdd,
				    particuleUpto- accepted,
				    acceptAtLeast, x0 );
	if  ( done < 0 )
	    { LDEB(done); return -1;	}

	if  ( lineAscent < textAscent )
	    { lineAscent=  textAscent;	}
	if  ( lineDescent > textDescent )
	    { lineDescent=  textDescent;	}

	if  ( docLayoutAlignTextToTab( pdd, done, pdTab, tabKind,
							&x1, tabX0, tabX1 ) )
	    { LDEB(done);	}

	wordCount += textWordCount;
	accepted += done; part += done; tpp += done; pdd += done;
	x0= x1;

	if  ( particuleUpto != particuleCount	&&
	      found == PSfoundNOTHING		)
	    {
	    particuleUpto= particuleCount;
	    found= PSfoundBULLET_END;
	    }

	switch( found )
	    {
	    case PSfoundTAB:
	    case PSfoundBULLET_END:
		break;

	    case PSfoundNOTHING:
	    case PSfoundLINEFULL:

	    case PSfoundLINEBREAK:

	    case PSfoundPAGEBREAK:
		*pFound= found;
		*pWordCount= wordCount;
		*pLineAscent= lineAscent; *pLineDescent= lineDescent;
		*pHasInline= hasInline;
		return accepted;

	    default:
		LDEB(found); return -1;
	    }

	utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tpp->tpTextAttributeNumber );

	afi= utilPsGetAfi( &encoding, dfl, psfl, &ta );
	if  ( ! afi )
	    { XDEB(afi); return -1;	}

	tab= -1;
	if  ( docNextTabStop( &ts, &x1, &tab, &(paraBi->biParaTabStopList),
				pf->pfX0GeometryTwips, pf->pfX0TextLinesTwips,
				dp->dpTabIntervalTwips, x0 ) )
	    { LDEB(dp->dpTabIntervalTwips); x1= x0+ 720;	}

	if  ( x1 > pf->pfX1TextLinesTwips && accepted >= acceptAtLeast )
	    {
	    if  ( accepted < 1 )
		{ LLDEB(particuleCount,accepted);	}

	    *pFound= PSfoundLINEFULL;
	    *pWordCount= wordCount;
	    *pLineAscent= lineAscent; *pLineDescent= lineDescent;
	    *pHasInline= hasInline;
	    return accepted;
	    }

	if  ( tab >= 0 )
	    { tabKind= ts.tsAlignment;	}
	else{ tabKind= DOCtaLEFT;	}
	tabX0= x0;
	tabX1= x1;
	tabParticule= accepted;

	pdd->pdAfi= afi;
	pdd->pdX0= x0;
	pdd->pdWidth= x1- x0;
	pdd->pdDecWidth= x1- x0;
	pdd->pdVisibleWidth= 0;
	pdd->pdTabNumber= tab;
	pdd->pdTabKind= tabKind;
	pdd->pdTabPosition= x1;
	pdd->pdVisiblePixels= 0;
	pdd->pdWhiteUnits= 0;
	pdd->pdCorrectBy= 0;

	if  ( tabKind == DOCtaLEFT )
	    { x0= x1;	}

	accepted++; part++; tpp++; pdd++;
	}

    if  ( accepted < 1 )
	{ LLDEB(particuleCount,accepted);	}

    *pFound= PSfoundNOTHING;
    *pWordCount= wordCount;
    *pLineAscent= lineAscent; *pLineDescent= lineDescent;
    *pHasInline= hasInline;

    return accepted;
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

static void docPsJustifyLine(	const BufferItem *	bi,
				const TextParticule *	tp,
				ParticuleData *		pd,
				int			accepted,
				int			x1TestLines )
    {
    int				i;
    int				extra;
    int				totalWeight;
    int				step;
    int				left;

    ParticuleData *		pdd;
    const TextParticule *	tpp;

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
	    { LDEB(pd[left-1].pdTabKind); return;	}

	tp += left; pd += left; accepted -= left;
	}

    /*  2  */
    while( accepted > 0 && tp[accepted- 1].tpStrlen == 0 )
	{ accepted--;	}

    if  ( accepted < 2 )
	{ /* LDEB(accepted); */ return;	}

    extra= x1TestLines- pd[accepted- 1].pdX0- pd[accepted- 1].pdVisibleWidth;

    if  ( extra < 0 )
	{
	LLDEB(extra,x1TestLines);
	tpp= tp; pdd= pd;
	for ( i= 0; i < accepted; tpp++, pdd++, i++ )
	    {
	    appDebug( "%-4s: %5d..%5d \"%*.*s\"\n",
		    docKindStr( tpp->tpKind ),
		    pdd->pdX0, pdd->pdX0+ pdd->pdVisibleWidth,
		    tpp->tpStrlen, tpp->tpStrlen,
		    bi->biParaString+ tpp->tpStroff );
	    }

	extra= 0;
	}

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
	pdd->pdX0 += step;
	if  ( pdd->pdWhiteUnits > 0 )
	    {
	    pdd->pdWidth += pdd->pdCorrectBy;
	    step += pdd->pdCorrectBy;
	    }
	}

    pdd->pdX0 += step;

    return;
    }

/************************************************************************/
/*									*/
/*  Place as many particules on a line as possible.			*/
/*									*/
/*  1)  Accept at least one particule.					*/
/*  A)  It looks nicer when fields do not start at the end of the line.	*/
/*	For the formatting it self this is irrelevant, but when a field	*/
/*	opens on the line before its contents, the shading of the	*/
/*	selection looks silly.						*/
/*  9)  Make \page mean \line in headers and footers and inside tables.	*/
/*									*/
/************************************************************************/

int docLayoutLineBox(	const BufferDocument *		bd,
			TextLine *			tl,
			const BufferItem *		bi,
			int				part,
			const PostScriptFontList *	psfl,
			ParticuleData *			pd,
			const ParagraphFrame *		pf )
    {
    int				lineAscent;
    int				lineDescent;
    int				ascent;
    int				descent;
    int				below;
    int				xShift;
    int				hasInlineContent= 1;

    int				accepted;
    int				found;
    int				wordCount;

    int				x0;
    int				visibleX1Twips;

    const TextParticule *	tp= bi->biParaParticules+ part;
    unsigned char *		from;

    if  ( part == 0 )
	{ x0= pf->pfX0FirstLineTwips;	}
    else{ x0= pf->pfX0TextLinesTwips;	}

    from= bi->biParaString+ tp->tpStroff;

    /*  1  */
    accepted= docLayoutParticules( bd, bi, part, &found, &wordCount,
				    &lineAscent, &lineDescent,
				    &hasInlineContent,
				    pf, psfl, pd,
				    bi->biParaParticuleCount- part, 1, x0 );

    if  ( accepted <= 0 )
	{ LDEB(accepted); return -1;	}

    /*  A  */
    while( accepted > 1							&&
	   bi->biParaParticules[part+accepted-1].tpKind
						== DOCkindFIELDSTART	)
	{ accepted--;	}

    tl->tlWordCount= wordCount;

    ascent= bi->biParaAscentTwips;
    descent= -bi->biParaDescentTwips;
    below= -bi->biParaDescentTwips+ bi->biParaLeadingTwips;

    if  ( lineAscent > bi->biParaAscentTwips )
	{ ascent= lineAscent;	}
    if  ( descent < -lineDescent )
	{ descent= -lineDescent; }
    if  ( below < -lineDescent )
	{ below= -lineDescent; }

    tl->tlLineAscentTwips= ascent;
    tl->tlLineHeightTwips= ascent+ descent;
    tl->tlLineSpacingTwips= ascent+ below;

    if  ( bi->biParaLineSpacingTwips != 0			&&
	  ( part+ accepted < bi->biParaParticuleCount	||
	    bi->biParaLineSpacingIsMultiple		)	)
	{
	if  ( bi->biParaLineSpacingTwips > 0 )
	    {
	    if  ( tl->tlLineSpacingTwips < bi->biParaLineSpacingTwips )
		{ tl->tlLineSpacingTwips=  bi->biParaLineSpacingTwips; }
	    }
	else{ tl->tlLineSpacingTwips= -bi->biParaLineSpacingTwips; }
	}

    if  ( accepted == 0 )
	{ visibleX1Twips= x0;	}
    else{
	ParticuleData *	pdd= pd+ accepted- 1;

	visibleX1Twips= pdd->pdX0+ pdd->pdVisibleWidth;
	}

    switch( bi->biParaAlignment )
	{
	case DOCiaLEFT:
	    xShift= 0; break;
	case DOCiaRIGHT:
	    xShift= pf->pfX1TextLinesTwips- visibleX1Twips;
	    break;
	case DOCiaCENTERED:
	    xShift= ( pf->pfX1TextLinesTwips- visibleX1Twips )/ 2;
	    break;
	case DOCiaJUSTIFIED:
	    xShift= 0;
	    if  ( found == PSfoundLINEFULL )
		{
		docPsJustifyLine( bi, tp, pd, accepted,
						    pf->pfX1TextLinesTwips );
		}
	    break;
	default:
	    LDEB(bi->biParaAlignment); xShift= 0;
	    break;
	}

    if  ( xShift != 0 )
	{
	int		i;
	ParticuleData *	pdd= pd;

	for ( i= 0; i < accepted; pdd++, i++ )
	    { pdd->pdX0 += xShift;	}
	}

    /*  9 */
    if  ( found == PSfoundPAGEBREAK		&&
	  bi->biInExternalItem == DOCinBODY	&&
	  ! bi->biParaInTable			)
	{ tl->tlHasPageBreak= 1;	}
    else{ tl->tlHasPageBreak= 0;	}

    tl->tlHasInlineContent= hasInlineContent;

    return accepted;
    }

