/************************************************************************/
/*									*/
/*  Read a plain text file into a BufferDocument			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Read a document as a whole.						*/
/*									*/
/************************************************************************/

typedef struct PlainReadingContext
    {
    TextAttribute	prcTextAttribute;
    BufferItem *	prcParaItem;
    int			prcLongestParagraph;
    } PlainReadingContext;

static void docPlainInitReadingContext( PlainReadingContext *	prc )
    {
    utilInitTextAttribute( &(prc->prcTextAttribute) );

    prc->prcParaItem= (BufferItem *)0;
    prc->prcLongestParagraph= 0;
    }

/************************************************************************/

static int docPlainReadParaCharacter(	SimpleInputStream *	sis,
					int *			pC,
					TextAttribute *		pTa )
    {
    int		c= sioInGetCharacter( sis );
    int		cc;

    if  ( c == EOF )
	{ return 1;	}

    if  ( c == '\t' || c == '\n' )
	{
	*pC= c; pTa->taFontIsBold= 0; pTa->taTextIsUnderlined= 0;
	return 0;
	}

    cc= sioInGetCharacter( sis );
    if  ( cc != '\b' )
	{
	if  ( cc != EOF )
	    { sioInUngetLastRead( sis );	}

	*pC= c; pTa->taFontIsBold= 0; pTa->taTextIsUnderlined= 0;
	return 0;
	}
    else{
	cc= sioInGetCharacter( sis );

	if  ( cc == EOF || cc == '\n' || cc == '\t' )
	    {
	    if  ( cc != EOF )
		{ sioInUngetLastRead( sis );	}

	    *pC= c; pTa->taFontIsBold= 0; pTa->taTextIsUnderlined= 0;
	    return 0;
	    }

	if  ( c == '_' )
	    {
	    *pC= cc; pTa->taFontIsBold= 0; pTa->taTextIsUnderlined= 1;
	    return 0;
	    }

	if  ( c == cc )
	    {
	    *pC= cc; pTa->taFontIsBold= 1; pTa->taTextIsUnderlined= 0;
	    return 0;
	    }

	*pC= cc; pTa->taFontIsBold= 0; pTa->taTextIsUnderlined= 0;
	return 0;
	}
    }

static int docPlainReadParagraph(	BufferDocument *	bd,
					SimpleInputStream *	sis,
					PlainReadingContext *	prc )
    {
    int			c;
    BufferItem *	bi;

    TextAttribute	ta= prc->prcTextAttribute;
    int			textAttrNr;

    int			res;

    res= docPlainReadParaCharacter( sis, &c, &ta );

    if  ( res < 0 )
	{ LDEB(res); return res;	}
    if  ( res > 0 )
	{ return res;	}

    if  ( ! prc->prcParaItem )
	{
	DocumentPosition	dp;

	if  ( docFirstPosition( &dp, &(bd->bdItem) ) )
	    { LDEB(1);	}

	bi= dp.dpBi;
	prc->prcParaItem= bi;
	}
    else{
	bi= docInsertItem( bd, prc->prcParaItem->biParent, -1, DOClevPARA );
	if  ( ! bi )
	    { XDEB(bi); return -1;	}

	prc->prcParaItem= bi;
	}

    while( c != '\n' )
	{
	int		off= bi->biParaStrlen;
	TextAttribute	prevTa= ta;

	if  ( c == '\f' )
	    {
	    if  ( bi->biParaStrlen == 0 )
		{ bi->biParaStartsOnNewPage= 1;	}

	    res= docPlainReadParaCharacter( sis, &c, &ta );

	    if  ( res < 0 )
		{ LDEB(res); return res;	}
	    if  ( res > 0 )
		{ break;	}

	    continue;
	    }

	if  ( c == '\t' )
	    {
	    const int	atBegin= 0;

	    if  ( docSaveSpecialParticule( bd, bi, atBegin,
				    &(prc->prcTextAttribute), DOCkindTAB ) )
		{ LDEB(bi->biParaStrlen); return -1;	}

	    res= docPlainReadParaCharacter( sis, &c, &ta );

	    if  ( res < 0 )
		{ LDEB(res); return res;	}
	    if  ( res > 0 )
		{ break;	}

	    continue;
	    }

	while( c != ' ' )
	    {
	    if  ( c != '\r' )
		{
		if  ( docInflateTextString( bi, 1 ) )
		    { LDEB(bi->biParaStrlen); return -1;	}

		bi->biParaString[bi->biParaStrlen++]= c;
		bi->biParaString[bi->biParaStrlen  ]= '\0';
		}

	    res= docPlainReadParaCharacter( sis, &c, &ta );
	    if  ( res )
		{ break;	}

	    if  ( c == '\n'					||
		  c == '\t'					||
		  ! docEqualTextAttributes( &ta, &prevTa )	)
		{ break;	}
	    }

	if  ( res < 0 )
	    { LDEB(res); return res;	}

	if  ( ! res && docEqualTextAttributes( &ta, &prevTa ) )
	    {
	    while( c == ' ' )
		{
		if  ( docInflateTextString( bi, 1 ) )
		    { LDEB(bi->biParaStrlen); return -1;	}

		bi->biParaString[bi->biParaStrlen++]= c;
		bi->biParaString[bi->biParaStrlen  ]= '\0';

		res= docPlainReadParaCharacter( sis, &c, &ta );
		if  ( res )
		    { break;	}

		if  ( ! docEqualTextAttributes( &ta, &prevTa )	)
		    { break;	}
		}
	    }

	if  ( res < 0 )
	    { LDEB(res); return res;	}

	textAttrNr= docTextAttributeNumber( bd, &prevTa );
	if  ( textAttrNr < 0 )
	    { LDEB(textAttrNr); return -1;	}

	if  ( ! docInsertTextParticule( bi, -1,
			off, bi->biParaStrlen- off, DOCkindTEXT, textAttrNr ) )
	    { LDEB(bi->biParaParticuleCount); return -1;	}

	if  ( res > 0 )
	    { break;	}
	}

    if  ( bi->biParaParticuleCount == 0 )
	{
	textAttrNr= docTextAttributeNumber( bd, &(prc->prcTextAttribute) );

	if  ( ! docInsertTextParticule( bi, 0, 0, 0, DOCkindTEXT, textAttrNr ) )
	    { LDEB( bi->biParaParticuleCount); return -1;	}
	}

    if  ( prc->prcLongestParagraph < bi->biParaStrlen )
	{ prc->prcLongestParagraph=  bi->biParaStrlen;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read a plain text file.						*/
/*									*/
/*  1)  Assume 12 cpi and courier. ( 12 cpi .. 6pt wide .. 10 pt high. )*/
/*	But make it one less: 9 pt to match the rfc1234.txt make up.	*/
/*  2)  Tab every 8 characters. 12 cpi .. 8/12 inch= 960 twips.		*/
/*									*/
/************************************************************************/

BufferDocument * docPlainReadFile(
			SimpleInputStream *		sis,
			int *				pMxL,
			int				docCharset,
			int				ansiCodepage,
			const DocumentGeometry *	dg )
    {
    BufferDocument *	bd;

    PlainReadingContext	prc;

    docPlainInitReadingContext( &prc );

    bd= docNewFile( &(prc.prcTextAttribute),
				    "Courier,,,9",
				    docCharset, ansiCodepage,
				    (PostScriptFontList *)0, dg );
    if  ( ! bd )
	{ XDEB(bd); return bd;	}

    bd->bdProperties.dpTabIntervalTwips= 960;

#   if 0

    int			encoding= 0;
    AfmBBox		abb;
    int			wide;

    wide= psCalculateStringExtents( &abb, "00000000", 8,
				10* prc.prcTextAttribute.taFontSizeHalfPoints,
				0, encoding, afi );
    if  ( wide > 0 )
	{ bd->bdProperties.dpTabIntervalTwips= wide;	}

#   endif

    for (;;)
	{
	int	res;

	res= docPlainReadParagraph( bd, sis, &prc );

	if  ( res > 0 )
	    { break;	}
	if  ( res < 0 )
	    { LDEB(res); goto failed; }
	}

    *pMxL= prc.prcLongestParagraph;

    return bd;

  failed:

    docFreeDocument( bd );

    return (BufferDocument *)0;
    }
