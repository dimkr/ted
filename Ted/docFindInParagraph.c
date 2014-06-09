/************************************************************************/
/*									*/
/*  Find: Look for something inside paragraphs.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docFind.h"

#   include	<reg.h>
#   include	<charnames.h>

/************************************************************************/
/*									*/
/*  Find the first occurrence of a regular expression in a paragraph.	*/
/*									*/
/************************************************************************/

int docFindParaFindNext(	DocumentSelection *		ds,
				BufferItem *			bi,
				const BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				void *				through )
    {
    regProg *		prog= (regProg *)through;
    int			res;
    const int		direction= 1;

    if  (  bi->biParaStrlen == 0 )
	{ return 1;	}

    res= regFindLeftToRight( prog, bi->biParaString+ dpFrom->dpStroff );

    if  ( ! res )
	{ return 1;	}

    docSetParaSelection( ds, bi, direction,
				    prog->rpStartp[0]- bi->biParaString,
				    prog->rpEndp[0]- prog->rpStartp[0] );

    return 0;
    }

int docFindParaFindPrev(	DocumentSelection *		ds,
				BufferItem *			bi,
				const BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				void *				through )
    {
    regProg *		prog= (regProg *)through;
    int			res;
    const int		direction= -1;

    if  (  bi->biParaStrlen == 0 )
	{ return 1;	}

    res= regFindRightToLeft( prog, bi->biParaString, dpFrom->dpStroff );

    if  ( ! res )
	{ return 1;	}

    docSetParaSelection( ds, bi, direction,
				    prog->rpStartp[0]- bi->biParaString,
				    prog->rpEndp[0]- prog->rpStartp[0] );

    return 0;
    }

