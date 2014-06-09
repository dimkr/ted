/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<limits.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>
#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Manage fields.							*/
/*									*/
/************************************************************************/

#   define	FIC_COUNT	15

/************************************************************************/
/*									*/
/*  See whether the field instructions are to a hyperlink or to a	*/
/*  bookmark.								*/
/*									*/
/*  Ted only supports bookmarks that begin end end inside the same	*/
/*  paragraph.								*/
/*									*/
/************************************************************************/

int docGetBookmarkForPosition(	const BufferDocument *		bd,
				const DocumentPosition *	dp,
				DocumentSelection *		ds,
				int *				pBeginPart,
				int *				pEndPart,
				const char **			pMarkName,
				int *				pMarkSize )
    {
    BufferItem *	bi= dp->dpBi;
    int			beginStroff= 0;
    int			endStroff= 0;
    int			beginPart;
    int			endPart;
    TextParticule *	tp;

    DocumentField *	dfBegin= (DocumentField *)0;
    DocumentField *	dfEnd= (DocumentField *)0;

    int			beginObjectNumber= -1;

    if  ( docFindParticule( &beginPart, dp->dpBi, dp->dpStroff, 1 ) )
	{ LDEB(dp->dpStroff); return -1;	}

    tp= bi->biParaParticules+ beginPart;
    while( beginPart >= 0 )
	{
	if  ( tp->tpKind == DOCkindFIELDSTART	)
	    {
	    dfBegin= bd->bdFieldList.dflFields+ tp->tpObjectNumber;
	    if  ( dfBegin->dfKind == DOCfkBOOKMARK )
		{
		beginStroff= tp->tpStroff;
		beginObjectNumber= tp->tpObjectNumber;
		break;
		}
	    }

	tp--; beginPart--;
	}

    if  ( beginPart < 0 )
	{ return -1;	}

    if  ( docFindParticule( &endPart, dp->dpBi, dp->dpStroff, 0 ) )
	{ LDEB(dp->dpStroff); return -1;	}
    tp= bi->biParaParticules+ endPart;
    while( endPart < bi->biParaParticuleCount )
	{
	if  ( tp->tpKind == DOCkindFIELDEND		&&
	      tp->tpObjectNumber == beginObjectNumber	)
	    {
	    dfEnd= bd->bdFieldList.dflFields+ tp->tpObjectNumber;

	    if  ( dfEnd->dfKind != DOCfkBOOKMARK )
		{ LDEB(dfEnd->dfKind); return -1;	}

	    if  ( docFieldGetBookmark( dfEnd, pMarkName, pMarkSize ) )
		{ LDEB(1); return -1;	}

	    endStroff= tp->tpStroff;

	    docSetParaSelection( ds, bi,
		endStroff > beginStroff, beginStroff, endStroff- beginStroff );

	    *pBeginPart= beginPart;
	    *pEndPart= endPart;

	    return 0;
	    }

	tp++; endPart++;
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Extract the destination from a hyperlink field.			*/
/*									*/
/*  Return the file name and the bookmark.				*/
/*  The strings are NOT '\0' terminated.				*/
/*									*/
/************************************************************************/

int docFieldGetHyperlink(	const DocumentField *	df,
				const char **		pFileName,
				int *			pFileSize,
				const char **		pMarkName,
				int *			pMarkSize )
    {
    FieldInstructionsComponent	fic[FIC_COUNT];
    int				n;
    int				comp;
    unsigned char *		bytes= df->dfInstructions.mbBytes;

    char *			fileName= (char *)0;
    int				fileSize= 0;
    char *			markName= (char *)0;
    int				markSize= 0;

    if  ( df->dfKind != DOCfkHYPERLINK )
	{ return -1;	}

    n= docSplitFieldInstructions( &(df->dfInstructions), fic, FIC_COUNT );
    if  ( n < 2 )
	{ LDEB(n); return -1;	}

    comp= 0;
    if  ( fic[comp].ficSize != 9					||
	  memcmp( bytes+ fic[comp].ficOffset, "HYPERLINK", 9 )	)
	{ SDEB((char *)bytes); return -1;	}
    comp++;

    if  ( fic[comp].ficSize > 0				&&
	  ( fic[comp].ficSize != 2		||
	    bytes[fic[comp].ficOffset] != '\\'	)	)
	{
	fileName= (char *)bytes+ fic[comp].ficOffset;
	fileSize= fic[comp].ficSize;
	comp++;
	}

    if  ( comp < n						&&
	  fic[comp].ficSize == 2					&&
	  ! memcmp( bytes+ fic[comp].ficOffset, "\\l", 2 )	)
	{
	comp++;
	if  ( comp >= n )
	    { LLDEB(comp,n); return -1;	}

	markName= (char *)bytes+ fic[comp].ficOffset;
	markSize= fic[comp].ficSize;

	comp++;
	}

    while( comp < n )
	{ LSDEB(comp,(char *)bytes+ fic[comp].ficOffset); comp++; }

    *pFileName= fileName;
    *pFileSize= fileSize;
    *pMarkName= markName;
    *pMarkSize= markSize;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Extract the name from a bookmark field.				*/
/*									*/
/*  Return the bookmark name.						*/
/*  The string is NOT '\0' terminated.					*/
/*									*/
/************************************************************************/

int docFieldGetBookmark(	const DocumentField *	df,
				const char **		pMarkName,
				int *			pMarkSize )
    {
    FieldInstructionsComponent	fic[FIC_COUNT];
    int				n;
    int				comp;
    unsigned char *		bytes= df->dfInstructions.mbBytes;

    char *			markName= (char *)0;
    int				markSize= 0;

    if  ( df->dfKind != DOCfkBOOKMARK )
	{ return -1;	}

    n= docSplitFieldInstructions( &(df->dfInstructions), fic, FIC_COUNT );
    if  ( n < 2 )
	{ LDEB(n); return -1;	}

    comp= 0;
    if  ( fic[comp].ficSize != 8					||
	  memcmp( bytes+ fic[comp].ficOffset, "BOOKMARK", 8 )	)
	{ SDEB((char *)bytes); return -1;	}
    comp++;

    markName= (char *)bytes+ fic[comp].ficOffset;
    markSize= fic[comp].ficSize;
    comp++;

    while( comp < n )
	{ LSDEB(comp,(char *)bytes+ fic[comp].ficOffset); comp++; }

    *pMarkName= markName;
    *pMarkSize= markSize;
    return 0;
    }

int docFieldGetRef(		const DocumentField *	df,
				const char **		pMarkName,
				int *			pMarkSize )
    {
    FieldInstructionsComponent	fic[FIC_COUNT];
    int				n;
    int				comp;
    unsigned char *		bytes= df->dfInstructions.mbBytes;

    char *			markName= (char *)0;
    int				markSize= 0;

    if  ( df->dfKind != DOCfkREF )
	{ return -1;	}

    n= docSplitFieldInstructions( &(df->dfInstructions), fic, FIC_COUNT );
    if  ( n < 2 )
	{ LDEB(n); return -1;	}

    comp= 0;
    if  ( fic[comp].ficSize != 3					||
	  memcmp( bytes+ fic[comp].ficOffset, "REF", 3 )	)
	{ SDEB((char *)bytes); return -1;	}
    comp++;

    markName= (char *)bytes+ fic[comp].ficOffset;
    markSize= fic[comp].ficSize;
    comp++;

    while( comp < n						&&
	  fic[comp].ficSize == 2				)
	{
	if  ( ! memcmp( bytes+ fic[comp].ficOffset, "\\f", 2 )	)
	    { comp++; continue;	}
	if  ( ! memcmp( bytes+ fic[comp].ficOffset, "\\n", 2 )	)
	    { comp++; continue;	}
	if  ( ! memcmp( bytes+ fic[comp].ficOffset, "\\h", 2 )	)
	    { comp++; continue;	}
	if  ( ! memcmp( bytes+ fic[comp].ficOffset, "\\r", 2 )	)
	    { comp++; continue;	}

	break;
	}

    while( comp < n )
	{ LSDEB(comp,(char *)bytes+ fic[comp].ficOffset); comp++; }

    *pMarkName= markName;
    *pMarkSize= markSize;
    return 0;
    }

int docFieldGetPageref(		const DocumentField *	df,
				const char **		pMarkName,
				int *			pMarkSize )
    {
    FieldInstructionsComponent	fic[FIC_COUNT];
    int				n;
    int				comp;
    unsigned char *		bytes= df->dfInstructions.mbBytes;

    char *			markName= (char *)0;
    int				markSize= 0;

    if  ( df->dfKind != DOCfkPAGEREF )
	{ return -1;	}

    n= docSplitFieldInstructions( &(df->dfInstructions), fic, FIC_COUNT );
    if  ( n < 2 )
	{ LDEB(n); return -1;	}

    comp= 0;
    if  ( fic[comp].ficSize != 7					||
	  memcmp( bytes+ fic[comp].ficOffset, "PAGEREF", 7 )	)
	{ SDEB((char *)bytes); return -1;	}
    comp++;

    markName= (char *)bytes+ fic[comp].ficOffset;
    markSize= fic[comp].ficSize;
    comp++;

    if  ( comp < n						&&
	  fic[comp].ficSize == 2					&&
	  ! memcmp( bytes+ fic[comp].ficOffset, "\\h", 2 )	)
	{ comp++;	}

    while( comp < n )
	{ LSDEB(comp,(char *)bytes+ fic[comp].ficOffset); comp++; }

    *pMarkName= markName;
    *pMarkSize= markSize;
    return 0;
    }

int docFieldGetIncludePicture(	const DocumentField *	df,
				const char **		pFileName,
				int *			pFileSize )
    {
    FieldInstructionsComponent	fic[FIC_COUNT];
    int				n;
    int				comp;
    unsigned char *		bytes= df->dfInstructions.mbBytes;

    char *			fileName= (char *)0;
    int				fileSize= 0;

    if  ( df->dfKind != DOCfkINCLUDEPICTURE )
	{ return -1;	}

    n= docSplitFieldInstructions( &(df->dfInstructions), fic, FIC_COUNT );
    if  ( n < 2 )
	{ LDEB(n); return -1;	}

    comp= 0;
    if  ( fic[comp].ficSize != 14					||
	  memcmp( bytes+ fic[comp].ficOffset, "INCLUDEPICTURE", 14 )	)
	{ SDEB((char *)bytes); return -1;	}
    comp++;

    fileName= (char *)bytes+ fic[comp].ficOffset;
    fileSize= fic[comp].ficSize;
    comp++;

    while( comp < n )
	{ LSDEB(comp,(char *)bytes+ fic[comp].ficOffset); comp++; }

    *pFileName= fileName;
    *pFileSize= fileSize;
    return 0;
    }

int docGetHyperlinkForPosition(	const BufferDocument *		bd,
				const DocumentPosition *	dp,
				DocumentSelection *		ds,
				int *				pBeginPart,
				int *				pEndPart,
				const char **			pFileName,
				int *				pFileSize,
				const char **			pMarkName,
				int *				pMarkSize )
    {
    BufferItem *	bi= dp->dpBi;

    int			beginStroff= 0;
    int			endStroff= 0;
    int			beginPart;
    int			endPart;

    TextParticule *	tp;

    DocumentField *	dfEnd= (DocumentField *)0;

    int			beginObjectNumber= -1;

    const char *	fileName;
    int			fileSize;
    const char *	markName;
    int			markSize;

    if  ( docFindParticule( &beginPart, dp->dpBi, dp->dpStroff, 1 ) )
	{ LDEB(dp->dpStroff); return -1;	}
    tp= bi->biParaParticules+ beginPart;
    while( beginPart >= 0 )
	{
	if  ( tp->tpKind == DOCkindFIELDSTART )
	    {
	    DocumentField *	df;

	    df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;

	    if  ( ! docFieldGetHyperlink( df,
				&fileName, &fileSize, &markName, &markSize ) )
		{
		beginStroff= tp->tpStroff;
		beginObjectNumber= tp->tpObjectNumber;
		break;
		}
	    }

	tp--; beginPart--;
	}

    if  ( beginPart < 0 )
	{ return 1;	}

    if  ( docFindParticule( &endPart, dp->dpBi, dp->dpStroff, 0 ) )
	{ LDEB(dp->dpStroff); return -1;	}
    tp= bi->biParaParticules+ endPart;
    while( endPart < bi->biParaParticuleCount )
	{
	if  ( tp->tpKind == DOCkindFIELDEND		&&
	      tp->tpObjectNumber == beginObjectNumber	)
	    {
	    dfEnd= bd->bdFieldList.dflFields+ tp->tpObjectNumber;

	    if  ( dfEnd->dfKind != DOCfkHYPERLINK )
		{ LDEB(dfEnd->dfKind); return -1;	}

	    endStroff= tp->tpStroff;

	    docSetParaSelection( ds, bi,
		endStroff > beginStroff, beginStroff, endStroff- beginStroff );

	    *pBeginPart= beginPart;
	    *pEndPart= endPart;
	    *pFileName= fileName;
	    *pFileSize= fileSize;
	    *pMarkName= markName;
	    *pMarkSize= markSize;

	    return 0;
	    }

	tp++; endPart++;
	}

    return 1;
    }

int docFieldSetHyperlink(	DocumentField *			df,
				const unsigned char *		file,
				int				fileSize,
				const unsigned char *		markName,
				int				markSize )
    {
    int			size;
    char *		fresh;
    char *		to;

    size= 11;
    if  ( fileSize > 0 )
	{ size += 1+ fileSize+ 2;	}
    if  ( markName )
	{ size += 4+ markSize+ 2;	}

    fresh= to= (char *)malloc( size+ 1 );
    if  ( ! fresh )
	{ LXDEB(size,fresh); return -1;	}

    strcpy( to, " HYPERLINK " ); to += strlen( to );
    if  ( fileSize > 0 )
	{
	*(to++)= '"';
	memcpy( to, file, fileSize ); to += fileSize;
	strcpy( to, "\" " ); to += strlen( to );
	}

    if  ( markSize > 0 )
	{
	strcpy( to, "\\l \"" ); to += strlen( to );
	memcpy( to, markName, markSize ); to += markSize;
	strcpy( to, "\" " ); to += strlen( to );
	}

    size= strlen( fresh );

    if  ( docSetFieldInst( df, (unsigned char *)fresh, size ) )
	{ LDEB(size); free( fresh ); return -1;	}

    free( fresh ); return 0;
    }

static int docFieldSetSingleValueField(	DocumentField *		df,
					const char *		kindName,
					int			kindSize,
					const unsigned char *	valueName,
					int			valueSize )
    {
    int			size;
    char *		fresh;
    char *		to;

    size= 1+ kindSize+ 1 + valueSize+ 2;

    fresh= to= (char *)malloc( size+ 1 );
    if  ( ! fresh )
	{ LXDEB(size,fresh); return -1;	}

    *(to++)= ' ';
    memcpy( to, kindName, kindSize ); to += kindSize;
    *(to++)= ' ';
    *(to++)= '"';
    memcpy( to, valueName, valueSize ); to += valueSize;
    *(to++)= '"';

    *(to)= '\0';

    if  ( docSetFieldInst( df, (unsigned char *)fresh, size ) )
	{ LDEB(size); free( fresh ); return -1;	}

    free( fresh ); return 0;
    }

int docFieldSetBookmark(	DocumentField *			df,
				const unsigned char *		markName,
				int				markSize )
    {
    const char		kindName[]= "BOOKMARK";
    int			kindSize= sizeof(kindName)- 1;

    return docFieldSetSingleValueField( df,
				    kindName, kindSize, markName, markSize );
    }

int docFieldSetRef(		DocumentField *			df,
				const unsigned char *		markName,
				int				markSize )
    {
    const char		kindName[]= "REF";
    int			kindSize= sizeof(kindName)- 1;

    return docFieldSetSingleValueField( df,
				    kindName, kindSize, markName, markSize );
    }

int docFieldSetPageref(		DocumentField *			df,
				const unsigned char *		markName,
				int				markSize )
    {
    const char		kindName[]= "PAGEREF";
    int			kindSize= sizeof(kindName)- 1;

    return docFieldSetSingleValueField( df,
				    kindName, kindSize, markName, markSize );
    }

/************************************************************************/
/*									*/
/*  Extract the values from a symbol field.				*/
/*									*/
/*									*/
/************************************************************************/

int docFieldGetSymbol(		const DocumentField *	df,
				const char **		pFontName,
				int *			pFontSize,
				const char **		pSymbName,
				int *			pSymbSize,
				const char **		pSizeString,
				int *			pSizeSize )
    {
    FieldInstructionsComponent	fic[FIC_COUNT];
    int				n;
    int				comp;
    unsigned char *		bytes= df->dfInstructions.mbBytes;

    const char *		fontName= (const char *)0;
    int				fontSize= 0;
    const char *		symbName= (const char *)0;
    int				symbSize= 0;
    const char *		sizeString= (const char *)0;
    int				sizeSize= 0;

    if  ( df->dfKind != DOCfkSYMBOL )
	{ return -1;	}

    n= docSplitFieldInstructions( &(df->dfInstructions), fic, FIC_COUNT );
    if  ( n < 2 )
	{ LDEB(n); return -1;	}

    comp= 0;
    if  ( fic[comp].ficSize != 6				||
	  memcmp( bytes+ fic[comp].ficOffset, "SYMBOL", 6 )	)
	{ SDEB((char *)bytes); return -1;	}
    comp++;

    while( comp < n )
	{
	if  ( fic[comp].ficSize == 2				&&
	      ! memcmp( bytes+ fic[comp].ficOffset, "\\f", 2 )	)
	    {
	    fontName= (char *)bytes+ fic[comp+ 1].ficOffset;
	    fontSize= fic[comp+ 1].ficSize;
	    comp += 2; continue;
	    }

	if  ( fic[comp].ficSize == 2				&&
	      ! memcmp( bytes+ fic[comp].ficOffset, "\\h", 2 )	)
	    { comp++; continue;	}

	if  ( fic[comp].ficSize == 2				&&
	      ! memcmp( bytes+ fic[comp].ficOffset, "\\s", 2 )	)
	    {
	    sizeString= (char *)bytes+ fic[comp+ 1].ficOffset;
	    sizeSize= fic[comp+ 1].ficSize;
	    comp += 2; continue;
	    }

	if  ( bytes[fic[comp].ficOffset] != '\\' )
	    {
	    symbName= (char *)bytes+ fic[comp].ficOffset;
	    symbSize= fic[comp].ficSize;
	    comp++; continue;
	    }

	SDEB((char *)bytes);
	break;
	}

    while( comp < n )
	{ LSDEB(comp,(char *)bytes+ fic[comp].ficOffset); comp++; }

    if  ( ! symbName )
	{ XDEB(symbName); return -1;	}

    *pFontName= fontName;
    *pFontSize= fontSize;
    *pSymbName= symbName;
    *pSymbSize= symbSize;
    *pSizeString= sizeString;
    *pSizeSize= sizeSize;

    return 0;
    }

