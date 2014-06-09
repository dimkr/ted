/************************************************************************/
/*									*/
/*  Manage the string of text bytes in a paragraph.			*/
/*  (Try not to be too clever, this will be changed to unicode once.)	*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Make sure there is enough space in the string of a paragraph.	*/
/*									*/
/*  NOTE that paraBi->biParaStrlen is not updated. We just make sure	*/
/*	that there is enough memory.					*/
/*									*/
/************************************************************************/

int docInflateTextString(	BufferItem *    paraBi,
				int		by )
    {
    unsigned char *	freshString;
    int			newSize;

    if  ( paraBi->biLevel != DOClevPARA )
	{ LLDEB(paraBi->biLevel,DOClevPARA); return -1;	}

    if  ( paraBi->biParaStrlen % 50 )
	{ newSize= paraBi->biParaStrlen+ by+  1; }
    else{ newSize= paraBi->biParaStrlen+ by+ 51; }

    freshString= (unsigned char *)realloc( paraBi->biParaString, newSize );
    if  ( ! freshString )
	{ LXDEB(paraBi->biParaStrlen,freshString); return -1; }

    paraBi->biParaString= freshString;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Replace part of the string of a paragraph with a different string	*/
/*									*/
/************************************************************************/

int docParaStringReplace(		int *			pStroffShift,
					BufferItem *		paraBi,
					int			stroffBegin,
					int			stroffEnd,
					const unsigned char *	addedString,
					int			addedStrlen )
    {
    int		stroffShift;

    stroffShift= addedStrlen- stroffEnd+ stroffBegin;

    if  ( paraBi->biLevel != DOClevPARA )
	{ LLDEB(paraBi->biLevel,DOClevPARA); return -1;	}

    if  ( stroffShift > 0				&&
	  docInflateTextString( paraBi, stroffShift )	)
	{ LLDEB(paraBi->biParaStrlen,stroffShift); return -1;	}

    if  ( paraBi->biParaStrlen > 0 )
	{
	int		n= paraBi->biParaStrlen- stroffEnd+ 1;

	if  ( n < 0 )
	    { LDEB(n);	}
	else{
	    memmove( paraBi->biParaString+ stroffEnd+ stroffShift,
				    paraBi->biParaString+ stroffEnd, n );
	    }
	}

    memcpy( paraBi->biParaString+ stroffBegin, addedString, addedStrlen );

    if  ( stroffShift != 0 )
	{
	paraBi->biParaStrlen += stroffShift;
	paraBi->biParaString[paraBi->biParaStrlen]= '\0';
	}

    *pStroffShift= stroffShift;
    return 0;
    }

