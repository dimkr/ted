/************************************************************************/
/*									*/
/*  Management of the list table of a document.				*/
/*  Individual list levels.						*/
/*									*/
/*  O)	Word 7.0 == Word 95 compatibility				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<utilBase26.h>
#   include	<utilRoman.h>

#   include	<appDebugon.h>

#   include	"docListLevel.h"

void docInitDocumentListLevel(	DocumentListLevel *	dll )
    {
    dll->dllStartAt= 1;
    dll->dllNumberStyle= 0;
    dll->dllJustification= DOCllaLEFT;
    dll->dllFollow= DOCllfTAB;
    dll->dllPrevToDigits= 0;
    dll->dllNoRestart= 0;
    dll->dllPictureNumber= -1;
    dll->dllFontBias= 0;

    dll->dllText= (unsigned char *)0;
    dll->dllNumbers= (unsigned char *)0;
    dll->dllTemplateID= -1;
    dll->dllTextSize= 0;
    dll->dllNumberSize= 0;

    dll->dllFromOld= 0;			/*  O  */
    dll->dllUsePrevText= 0;		/*  O  */
    dll->dllUsePrevSpace= 0;		/*  O  */
    dll->dllIndent= 0;			/*  O  */
    dll->dllSpace= 0;			/*  O  */

    docInitTabStopList( &(dll->dllTabStopList) );
    dll->dllLeftIndentTwips= 0;
    dll->dllFirstIndentTwips= 0;
    PROPmaskCLEAR( &(dll->dllParaPropertyMask) );

    utilInitTextAttribute( &(dll->dllTextAttribute) );
    PROPmaskCLEAR( &(dll->dllTextAttributeMask) );
    }

void docCleanDocumentListLevel(	DocumentListLevel *	dll )
    {
    if  ( dll->dllText )
	{ free( dll->dllText ); }
    if  ( dll->dllNumbers )
	{ free( dll->dllNumbers ); }

    docCleanTabStopList( &(dll->dllTabStopList) );
    }

int docCopyDocumentListLevel(	DocumentListLevel *		to,
				const DocumentListLevel *	from,
				const int *			fontMap,
				const int *			colorMap )
    {
    int			rval= 0;

    unsigned char *	text= (unsigned char *)0;
    unsigned char *	numbers= (unsigned char *)0;
    TabStopList		tsl;
    const int		pixels= 0;

    int			changed= 0;

    docInitTabStopList( &tsl );

    if  ( docCopyTabStopList( &changed, &tsl,
					&(from->dllTabStopList), pixels ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( from->dllText )
	{
	text= (unsigned char *)malloc( from->dllTextSize+ 1 );
	if  ( ! text )
	    { XDEB(text); rval= -1; goto ready;	}

	memcpy( text, from->dllText, from->dllTextSize );
	text[from->dllTextSize]= '\0';
	}

    if  ( from->dllNumbers )
	{
	numbers= (unsigned char *)malloc( from->dllNumberSize+ 1 );
	if  ( ! numbers )
	    { XDEB(numbers); rval= -1; goto ready;	}

	memcpy( numbers, from->dllNumbers, from->dllNumberSize );
	numbers[from->dllNumberSize]= '\0';
	}

    if  ( to->dllText )
	{ free( to->dllText );	}
    if  ( to->dllNumbers )
	{ free( to->dllNumbers );	}
    docCleanTabStopList( &(to->dllTabStopList) );

    *to= *from;

    if  ( fontMap							&&
	  PROPmaskISSET( &(to->dllTextAttributeMask), TApropDOC_FONT_NUMBER ) )
	{
	to->dllTextAttribute.taFontNumber=
				fontMap[from->dllTextAttribute.taFontNumber];
	}

    if  ( colorMap							&&
	  PROPmaskISSET( &(to->dllTextAttributeMask), TApropDOC_FONT_NUMBER ) &&
	  from->dllTextAttribute.taTextColorNumber > 0			)
	{
	to->dllTextAttribute.taTextColorNumber=
			    colorMap[from->dllTextAttribute.taTextColorNumber];
	}

    /* steal */
    to->dllText= text; text= (unsigned char *)0;
    to->dllNumbers= numbers; numbers= (unsigned char *)0;
    to->dllTabStopList= tsl; docInitTabStopList( &tsl );

  ready:

    if  ( text )
	{ free( text );	}
    if  ( numbers )
	{ free( numbers );	}

    docCleanTabStopList( &tsl );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Set the relevant style attributes in a list level.			*/
/*									*/
/*  1)  Paragraph Properties.						*/
/*  2)  Text Attributes.						*/
/*									*/
/************************************************************************/

int documentListLevelSetStyle(	DocumentListLevel *		dll,
				const PropertyMask *		paraMask,
				const ParagraphProperties *	pp,
				const PropertyMask *		textMask,
				const TextAttribute *		ta )
    {
    /*  1  */
    PROPmaskCLEAR( &(dll->dllParaPropertyMask) );

    if  ( PROPmaskISSET( paraMask, PPpropTAB_STOPS ) )
	{
	const int	pixels= 0;
	int		changed= 0;

	if  ( docCopyTabStopList( &changed, &(dll->dllTabStopList),
					    &(pp->ppTabStopList), pixels ) )
	    { LDEB(1); return -1;	}

	PROPmaskADD( &(dll->dllParaPropertyMask), PPpropTAB_STOPS );
	}

    if  ( PROPmaskISSET( paraMask, PPpropLEFT_INDENT ) )
	{
	dll->dllLeftIndentTwips= pp->ppLeftIndentTwips;
	PROPmaskADD( &(dll->dllParaPropertyMask), PPpropLEFT_INDENT );
	}

    if  ( PROPmaskISSET( paraMask, PPpropFIRST_INDENT ) )
	{
	dll->dllFirstIndentTwips= pp->ppFirstIndentTwips;
	PROPmaskADD( &(dll->dllParaPropertyMask), PPpropFIRST_INDENT );
	}

    /*  2  */
    {
    PropertyMask	doneMask;

    PROPmaskCLEAR( &doneMask );

    dll->dllTextAttributeMask= *textMask;
    utilUpdateTextAttribute( &doneMask, &(dll->dllTextAttribute),
								ta, textMask );
    }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Update a component in the level text in a list.			*/
/*									*/
/*  idx is the index in dll->dllNumbers. The text before the number is	*/
/*  changed. The trailing text after the last number is handeled in	*/
/*  the idx == dll->dllNumberSize case.					*/
/*									*/
/*  1)  Determine stretch in dll->dllText				*/
/*  2)  More memory needed?						*/
/*  3)  Insert new value.						*/
/*  4)  Shift indirection via dll->dllNumbers.				*/
/*  5)  Remember size in dll->dllText.					*/
/*									*/
/************************************************************************/

int docListLevelSetText(	int *				pChanged,
				DocumentListLevel *		dll,
				const unsigned char *		text,
				int				idx )
    {
    int			nlen= strlen( (const char *)text );
    int			olen;

    int			from;
    int			upto;

    int			i;

    /*  1  */
    if  ( idx < 0 || idx > dll->dllNumberSize )
	{ LLDEB(idx,dll->dllNumberSize); return -1;	}

    if  ( idx == 0 )
	{ from= 1;				}
    else{ from= dll->dllNumbers[idx- 1]+ 1;	}

    if  ( idx == dll->dllNumberSize )
	{ upto= dll->dllTextSize;		}
    else{ upto= dll->dllNumbers[idx];		}

    olen= upto- from;

    if  ( nlen == olen && ! memcmp( dll->dllText+ from, text, olen ) )
	{ *pChanged= 0; return 0;	}

    /*  2  */
    if  ( olen < nlen )
	{
	unsigned char *	fresh;

	fresh= realloc( dll->dllText, dll->dllTextSize+ nlen- olen+ 1 );
	if  ( ! fresh )
	    { LXDEB(dll->dllTextSize+nlen-olen,fresh); return -1;	}
	dll->dllText= fresh;
	}

    /*  3  */
    memmove( dll->dllText+ from+ nlen,
			dll->dllText+ from+ olen, dll->dllTextSize- upto+ 1 );
    memcpy( dll->dllText+ from, text, nlen );

    /*  4  */
    for ( i= idx; i < dll->dllNumberSize; i++ )
	{ dll->dllNumbers[i] += nlen- olen;	}

    dll->dllTextSize += nlen- olen;

    /*  5  */
    dll->dllText[0]= dll->dllTextSize- 1;

    *pChanged= 1; return 0;
    }

int docListLevelSetNumber(	int *				pChanged,
				DocumentListLevel *		dll,
				int				number,
				int				idx )
    {
    /*  1  */
    if  ( idx < 0 || idx > dll->dllNumberSize )
	{ LLDEB(idx,dll->dllNumberSize); return -1;	}

    if  ( idx < dll->dllNumberSize			&&
	  dll->dllText[dll->dllNumbers[idx]] == number	)
	{ *pChanged= 0; return 0;	}

    if  ( idx == dll->dllNumberSize )
	{
	unsigned char *	fresh;

	fresh= realloc( dll->dllNumbers, dll->dllNumberSize+ 1+ 1 );
	if  ( ! fresh )
	    { LXDEB(dll->dllNumberSize,fresh); return -1;	}
	dll->dllNumbers= fresh;
	dll->dllNumbers[dll->dllNumberSize++]= dll->dllTextSize;
	dll->dllNumbers[dll->dllNumberSize]= '\0';

	fresh= realloc( dll->dllText, dll->dllTextSize+ 1+ 1 );
	if  ( ! fresh )
	    { LXDEB(dll->dllTextSize,fresh); return -1;	}
	dll->dllText= fresh;
	dll->dllText[dll->dllTextSize++]= number;
	dll->dllText[0]++;
	dll->dllText[dll->dllTextSize]= '\0';
	}

    dll->dllText[dll->dllNumbers[idx]]= number;

    *pChanged= 1; return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a reference to a level number in the format string of a list	*/
/*  level.								*/
/*									*/
/*  1)  Check argument.							*/
/*  2)  Allocate memory.						*/
/*  3)  Insert in format template.					*/
/*  4)  Insert in list of numbers.					*/
/*									*/
/************************************************************************/

int docListLevelInsertNumber(	DocumentListLevel *		dll,
				int				number,
				int				idx )
    {
    int			off;
    int			i;
    unsigned char *	fresh;

    /*  1  */
    if  ( idx < 0 || idx > dll->dllNumberSize )
	{ LLDEB(idx,dll->dllNumberSize); return -1;	}

    /*  2  */
    fresh= realloc( dll->dllNumbers, dll->dllNumberSize+ 1+ 1 );
    if  ( ! fresh )
	{ LXDEB(dll->dllNumberSize,fresh); return -1;	}
    dll->dllNumbers= fresh;

    fresh= realloc( dll->dllText, dll->dllTextSize+ 1+ 1 );
    if  ( ! fresh )
	{ LXDEB(dll->dllTextSize,fresh); return -1;	}
    dll->dllText= fresh;

    /*  3  */
    if  ( idx == dll->dllNumberSize )
	{ off= dll->dllTextSize;	}
    else{
	off= dll->dllNumbers[idx];
	if  ( off > 1 )
	    { off--;	}
	}

    memmove( dll->dllText+ off+ 1, dll->dllText+ off,
						dll->dllTextSize- off+ 1 );
    dll->dllText[off]= number;
    dll->dllTextSize++;
    dll->dllText[0]++;

    /*  4  */
    dll->dllNumberSize++;
    dll->dllNumbers[dll->dllNumberSize]= '\0';

    for ( i= dll->dllNumberSize- 1; i > idx; i-- )
	{ dll->dllNumbers[i]= dll->dllNumbers[i- 1]+ 1; }
    dll->dllNumbers[idx]= off;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Delete a reference to a level number from the format string of a	*/
/*  list level.								*/
/*									*/
/*  1)  Check argument.							*/
/*  2)  Remove the number from the text format.				*/
/*  3)  Remove the number from the references to the text format.	*/
/*									*/
/************************************************************************/

int docListLevelDeleteNumber(	DocumentListLevel *		dll,
				int				idx )
    {
    int			off;
    int			i;

    /*  1  */
    if  ( idx < 0 || idx >= dll->dllNumberSize )
	{ LLDEB(idx,dll->dllNumberSize); return -1;	}

    /*  2  */
    off= dll->dllNumbers[idx];
    memmove( dll->dllText+ off, dll->dllText+ off+ 1,
						dll->dllTextSize- off );

    dll->dllTextSize--;
    dll->dllText[dll->dllTextSize]= '\0';
    dll->dllText[0]--;

    /*  3  */
    dll->dllNumberSize--;
    for ( i= idx; i < dll->dllNumberSize; i++ )
	{ dll->dllNumbers[i]= dll->dllNumbers[i+ 1]- 1; }
    dll->dllNumbers[dll->dllNumberSize]= '\0';

    return 0;
    }

/************************************************************************/
/*									*/
/*  Format the value of a 'listtext' field.				*/
/*									*/
/*  I.E. the text of the bullet or paragraph number at the beginning	*/
/*	of a paragraph that gets one because it has a list reference.	*/
/*									*/
/************************************************************************/

int docListLevelFormatLevelNumber(
				unsigned char *			target,
				int				maxsize,
				int				val,
				int				format )
    {
    int		step= 0;

    if  ( maxsize < 1 )
	{ LDEB(maxsize); return -1;	}

    switch( format )
	{
	case DOCpnDEC:
	    if  ( maxsize < 11 )
		{ LDEB(maxsize); return -1;	}
	    sprintf( (char *)target, "%d", val );
	    step= strlen( (char *)target );
	    break;

	case DOCpnUCRM:
	    if  ( utilRomanString( (char *)target, maxsize, val, 1 ) )
		{ LDEB(val); return -1;	}
	    step= strlen( (char *)target );
	    break;

	case DOCpnLCRM:
	    if  ( utilRomanString( (char *)target, maxsize, val, 0 ) )
		{ LDEB(val); return -1;	}
	    step= strlen( (char *)target );
	    break;

	case DOCpnUCLTR:
	    if  ( val == 0 )
		{ strcpy( (char *)target, "+" ); }
	    else{
		if  ( utilBase26String( (char *)target, maxsize, val, 1 ) )
		    { LDEB(val); return -1;	}
		}
	    step= strlen( (char *)target );
	    break;

	case DOCpnLCLTR:
	    if  ( val == 0 )
		{ strcpy( (char *)target, "+" ); }
	    else{
		if  ( utilBase26String( (char *)target, maxsize, val, 0 ) )
		    { LDEB(val); return -1;	}
		}
	    step= strlen( (char *)target );
	    break;

	case 22:
	    if  ( maxsize < 11 )
		{ LDEB(maxsize); return -1;	}
	    sprintf( (char *)target, "%02d", val );
	    step= strlen( (char *)target );
	    break;

	case 23:
	    *(target++)= 0xb7;
	    target[0]= '\0';
	    step= 1;
	    break;

	case 255:
	    break;

	default:
	    LLDEB(format,val);
	    if  ( maxsize < 13 )
		{ LDEB(maxsize); return -1;	}
	    sprintf( (char *)target, "(%d)", val );
	    step= strlen( (char *)target );
	    break;
	}

    return step;
    }

int docListLevelFormatParagraphNumber(
				unsigned char *			target,
				int				maxsize,
				int *				offsets,
				int				maxoffsets,
				int				ilvl,
				const int *			numberPath,
				const int *			startPath,
				const int *			formatPath,
				const DocumentListLevel *	dll )
    {
    int		done= 0;
    int		textSize= 1;
    int		textUsed= 1;

    int		off;

    if  ( ! dll->dllText )
	{ XDEB(dll->dllText); return -1; }

    if  ( dll->dllTextSize > 0 )
	{ textSize= dll->dllText[0]+ 1;	}


    for ( off= 0; off < dll->dllNumberSize; off++ )
	{
	int	level;
	int	step;
	int	upto= dll->dllNumbers[off];

	if  ( upto >= textSize	||
	      upto < textUsed	)
	    { LLLDEB(upto,textSize,textUsed); return -1;	}
	if  ( done+ ( upto- textUsed ) >= maxsize )
	    { LLLDEB(done,upto- textUsed,maxsize); return -1;		}

	if  ( 2* off+ 0 < maxoffsets )
	    { offsets[2* off+ 0]= done;	}

	if  ( textUsed < upto )
	    {
	    memcpy( target, dll->dllText+ textUsed, upto- textUsed );
	    target += upto- textUsed; done += upto- textUsed; textUsed= upto;
	    }

	target[0]= '\0';

	level= dll->dllText[textUsed++];
	if  ( level > ilvl )
	    { LLDEB(ilvl,level); return -1;	}

	if  ( 2* off+ 1 < maxoffsets )
	    { offsets[2* off+ 1]= done;	}

	step= docListLevelFormatLevelNumber( target, maxsize- done,
				startPath[level]+ numberPath[level],
				formatPath[level] );
	if  ( step < 0 )
	    { LLDEB(level,step); return -1;	}

	target += step; done += step;
	}

    if  ( 2* off+ 0 < maxoffsets )
	{ offsets[2* off+ 0]= done;	}

    if  ( textUsed < textSize )
	{
	int	upto= textSize;

	if  ( done+ ( upto- textUsed ) >= maxsize )
	    { LLLDEB(done,upto- textUsed,maxsize); return -1;	}

	memcpy( target, dll->dllText+ textUsed, upto- textUsed );
	target += upto- textUsed; done += upto- textUsed;
	target[0]= '\0';
	}

    if  ( 2* off+ 1 < maxoffsets )
	{ offsets[2* off+ 1]= done;	}

    target[0]= '\0';
    return done;
    }

