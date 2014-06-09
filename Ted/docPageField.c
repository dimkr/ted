/************************************************************************/
/*									*/
/*  Calculate page number and ref fields.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<utilBase26.h>
#   include	<utilRoman.h>
#   include	"docBuf.h"
#   include	"docEvalField.h"

/************************************************************************/
/*									*/
/*  Evaluate page number related fields.				*/
/*									*/
/*  The function that evaluates a REF field is an honorary member of	*/
/*  this file.								*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Format a page number to be used in a certain paragraph.		*/
/*									*/
/************************************************************************/

static int docGetPageNumberOffset(	const BufferItem *	sectBi )
    {
    if  ( ! sectBi->biParent )
	{ XDEB(sectBi->biParent); return 0;	}

    while( sectBi->biNumberInParent > 0 )
	{
	if  ( sectBi->biSectRestartPageNumbers )
	    { break;	}

	sectBi= sectBi->biParent->biChildren[sectBi->biNumberInParent- 1];
	}

    if  ( sectBi->biSectRestartPageNumbers )
	{
	return sectBi->biTopPosition.lpPage- sectBi->biSectStartPageNumber;
	}
    else{ return sectBi->biTopPosition.lpPage; }
    }

static void docFormatPageNumber(	char *			target,
					int			targetSize,
					const BufferItem *	sectBi,
					int			pageNumber )
    {
    int			style= sectBi->biSectPageNumberStyle;

    if  ( targetSize < 20 )
	{ LDEB(targetSize); strcpy( target, "?" ); return;	}

    pageNumber -= docGetPageNumberOffset( sectBi );

    switch( style )
	{
	default:
	    LDEB(style);
	    /*FALLTHROUGH*/

	case DOCpgnDEC:
	    sprintf( target, "%d", pageNumber+ 1 );
	    break;

	case DOCpgnUCLTR:
	    if  ( utilBase26String( target, targetSize, pageNumber+ 1, 1 ) )
		{ LDEB(pageNumber); return ;	}
	    break;

	case DOCpgnLCLTR:
	    if  ( utilBase26String( target, targetSize, pageNumber+ 1, 0 ) )
		{ LDEB(pageNumber); return ;	}
	    break;

	case DOCpgnUCRM:
	    if  ( utilRomanString( target, targetSize, pageNumber+ 1, 1 ) )
		{ sprintf( target, "UCRM:%d", pageNumber+ 1 );	}
	    break;

	case DOCpgnLCRM:
	    if  ( utilRomanString( target, targetSize, pageNumber+ 1, 0 ) )
		{ sprintf( target, "lcrm:%d", pageNumber+ 1 );	}
	    break;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Return the value of a pageref field.				*/
/*									*/
/************************************************************************/

int docCalculatePagerefFieldString(	int *			pCalculated,
					int *			pNewSize,
					unsigned char *		target,
					int			targetSize,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			partCount,
					const DocumentField *	dfRef )
    {
    DocumentField *		dfMark;
    int				pageNumber;
    int				i;
    const BufferItem *		sectBi= (const BufferItem *)0;

    const char *		markName;
    int				markSize;

    int				n;

    if  ( docFieldGetPageref( dfRef, &markName, &markSize ) )
	{ LDEB(1); *pCalculated= 0; return 0;	}

    n= docGetBookmarkField( &dfMark, markName, markSize, &(bd->bdFieldList) );
    if  ( n < 0 )
	{ SLLDEB(markName,markSize,n); *pCalculated= 0; return 0;	}

    pageNumber= dfMark->dfPage;

    for ( i= 0; i < bd->bdItem.biChildCount; i++ )
	{
	sectBi= bd->bdItem.biChildren[i];
	if  ( sectBi->biBelowPosition.lpPage >= pageNumber )
	    { break;	}
	}

    if  ( i >= bd->bdItem.biChildCount )
	{ LDEB(1); *pCalculated= 0; return 0;	}

    docFormatPageNumber( (char *)target, targetSize, sectBi, pageNumber );

    *pNewSize= strlen( (char *)target );
    *pCalculated= 1;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Return the number of the current section.				*/
/*									*/
/************************************************************************/

int docCalculateSectionFieldString(	int *			pCalculated,
					int *			pNewSize,
					unsigned char *		target,
					int			targetSize,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			partCount,
					const DocumentField *	df )
    {
    const BufferItem *	sectBi;

    sectBi= paraBi;

    while( sectBi && sectBi->biLevel != DOClevSECT )
	{ sectBi= sectBi->biParent;	}
    if  ( ! sectBi )
	{ XDEB(sectBi); *pCalculated= 0; return 0;	}

    if  ( sectBi->biInExternalItem != DOCinBODY )
	{
	if  ( ! sectBi->biSectHeaderFooterUseForSectBi )
	    {
	    XDEB(sectBi->biSectHeaderFooterUseForSectBi);
	    *pCalculated= 0; return 0;
	    }

	sectBi= sectBi->biSectHeaderFooterUseForSectBi;
	}

    sprintf( (char *)target, "%d", sectBi->biNumberInParent+ 1 );
    *pNewSize= strlen( (char *)target );

    *pCalculated= 1; return 0;
    }

int docCalculateSectionPagesFieldString(int *			pCalculated,
					int *			pNewSize,
					unsigned char *		target,
					int			targetSize,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			partCount,
					const DocumentField *	df )
    {
    const BufferItem *	sectBi;

    sectBi= paraBi;

    while( sectBi && sectBi->biLevel != DOClevSECT )
	{ sectBi= sectBi->biParent;	}
    if  ( ! sectBi )
	{ XDEB(sectBi); *pCalculated= 0; return 0;	}

    if  ( sectBi->biInExternalItem != DOCinBODY )
	{
	if  ( ! sectBi->biSectHeaderFooterUseForSectBi )
	    {
	    XDEB(sectBi->biSectHeaderFooterUseForSectBi);
	    *pCalculated= 0; return 0;
	    }

	sectBi= sectBi->biSectHeaderFooterUseForSectBi;
	}

    sprintf( (char *)target, "%d",
	sectBi->biBelowPosition.lpPage- sectBi->biTopPosition.lpPage+ 1 );
    *pNewSize= strlen( (char *)target );

    *pCalculated= 1; return 0;
    }

/************************************************************************/
/*									*/
/*  Return the value of a page/numpages field.				*/
/*									*/
/************************************************************************/

int docCalculatePageFieldString(	int *			pCalculated,
					int *			pNewSize,
					unsigned char *		target,
					int			targetSize,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			partCount,
					const DocumentField *	df )
    {
    const BufferItem *	headerFooterBi;

    headerFooterBi= paraBi->biParent;
    headerFooterBi= headerFooterBi->biParent;
    headerFooterBi= headerFooterBi->biParent;

    if  ( ! headerFooterBi->biSectHeaderFooterUseForSectBi )
	{ XDEB(headerFooterBi->biSectHeaderFooterUseForSectBi); return -1; }

    docFormatPageNumber( (char *)target, targetSize,
			    headerFooterBi->biSectHeaderFooterUseForSectBi,
			    headerFooterBi->biSectHeaderFooterUseForPage );
    *pNewSize= strlen( (char *)target );

    *pCalculated= 1; return 0;
    }

int docCalculateNumpagesFieldString(	int *			pCalculated,
					int *			pNewSize,
					unsigned char *		target,
					int			targetSize,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			partCount,
					const DocumentField *	df )
    {
    if  ( targetSize < 20 )
	{ LDEB(targetSize); return -1;	}

    sprintf( (char *)target, "%d", bd->bdItem.biBelowPosition.lpPage+ 1 );
    *pNewSize= strlen( (char *)target );

    *pCalculated= 1; return 0;
    }

/************************************************************************/
/*									*/
/*  Return the value of a ref field.					*/
/*									*/
/************************************************************************/

int docCalculateRefFieldString(		int *			pCalculated,
					int *			pNewSize,
					unsigned char *		target,
					int			targetSize,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			partCount,
					const DocumentField *	dfRef )
    {
    const char *		markName;
    int				markSize;

    DocumentSelection		dsNew;

    int				len;

    if  ( docFieldGetRef( dfRef, &markName, &markSize ) )
	{ LDEB(1); *pCalculated= 0; return 0;	}

    docInitDocumentSelection( &dsNew );

    if  ( docFindBookmarkInDocument( &dsNew, bd, markName, markSize ) )
	{
	SLDEB(markName,markSize);

	strcpy( (char *)target, "<<?!?>>" );
	len= strlen( (char *)target );
	}
    else{
	int			beginMoved= 0;
	int			endMoved= 0;
	const unsigned char *	string;

	docConstrainSelectionToOneParagraph( &beginMoved, &endMoved, &dsNew );

	if  ( dsNew.dsEnd.dpStroff == dsNew.dsBegin.dpStroff )
	    { LLDEB(dsNew.dsEnd.dpStroff,dsNew.dsBegin.dpStroff);	}

	string= dsNew.dsBegin.dpBi->biParaString;
	len= dsNew.dsEnd.dpStroff- dsNew.dsBegin.dpStroff;

	if  ( len > targetSize )
	    { len=  targetSize;	}
	memcpy( target, string+ dsNew.dsBegin.dpStroff, len );
	target[len]= '\0';
	}

    *pNewSize= len;
    *pCalculated= 1;

    return 0;
    }

