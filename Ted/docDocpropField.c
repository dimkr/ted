/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>
#   include	"docBuf.h"
#   include	"docEvalField.h"

/************************************************************************/
/*									*/
/*  Evaluate fields to document properties.				*/
/*									*/
/************************************************************************/

#   define	FIC_COUNT	15

/************************************************************************/
/*									*/
/*  Return the value of one of the document dates.			*/
/*									*/
/************************************************************************/

static int docIsMergeformat(		const unsigned char *		bytes,
					FieldInstructionsComponent *	fic,
					int				n,
					int				comp )
    {
    if  ( comp < n-1						&&
	  fic[comp].ficSize == 2				&&
	  ! memcmp( bytes+ fic[comp].ficOffset, "\\*", 2 )	&&
	  fic[comp+ 1].ficSize == 11				)
	{
	const char *		mergeformat= "mergeformat";
	int			i;

	for ( i= 0; i < 11; i++ )
	    {
	    if  ( tolower( bytes[fic[comp+1].ficOffset+ i] ) != mergeformat[i] )
		{ break;	}
	    }

	if  ( i == 11 )
	    { return 1;	}
	}

    return 0;
    }

int docCalculateDocDateFieldString(	int *			pCalculated,
					int *			pNewSize,
					unsigned char *		target,
					int			targetSize,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			partCount,
					const DocumentField *	df )
    {
    FieldInstructionsComponent	fic[FIC_COUNT];
    int				n;
    int				comp;
    unsigned char *		bytes= df->dfInstructions.mbBytes;

    const DocumentProperties *	dp= &(bd->bdProperties);
    const struct tm *		tm;

    int				mergeFormat= 0;
    int				pictureComp= -1;
    const char *		defaultFormat= (const char *)0;

    time_t			t;
    struct tm			tmNow;

    n= docSplitFieldInstructions( &(df->dfInstructions), fic, FIC_COUNT );
    if  ( n < 1 )
	{ SLDEB((char *)bytes,n); return -1;	}

    for ( comp= 1; comp < n; comp++ )
	{
	if  ( docIsMergeformat( bytes, fic, n, comp ) )
	    { comp++; continue;	}

	if  ( comp < n-1					&&
	      fic[comp].ficSize == 2				&&
	      ! memcmp( bytes+ fic[comp].ficOffset, "\\@", 2 )	)
	    { pictureComp= comp+ 1; comp++; continue; }

	LSDEB(comp,(char *)bytes+ fic[comp].ficOffset);
	}

    if  ( targetSize < 40 )
	{ LDEB(targetSize); return -1;	}

    switch( df->dfKind )
	{
	case DOCfkCREATEDATE:
	    tm= &(dp->dpCreatim);
	    defaultFormat= "%c";
	    break;
	case DOCfkSAVEDATE:
	    tm= &(dp->dpRevtim);
	    defaultFormat= "%c";
	    break;
	case DOCfkPRINTDATE:
	    tm= &(dp->dpPrintim);
	    defaultFormat= "%c";
	    break;

	case DOCfkDATE:
	    t= time( (time_t *)0 );
	    tmNow= *localtime( &t );
	    tm= &tmNow;
	    defaultFormat= "%x";
	    break;
	case DOCfkTIME:
	    t= time( (time_t *)0 );
	    tmNow= *localtime( &t );
	    tm= &tmNow;
	    defaultFormat= "%X";
	    break;

	default:
	    LDEB(df->dfKind); *pCalculated= 0; return 0;
	}

    if  ( tm->tm_mday == 0 )
	{ /*LDEB(tm->tm_mday);*/ *pCalculated= 0; return 0;	}

    if  ( pictureComp >= 0 )
	{
	char		scratch[50+1];

	if  ( fic[pictureComp].ficSize > sizeof(scratch)- 1 )
	    {
	    LLDEB(fic[pictureComp].ficSize,sizeof(scratch));
	    *pCalculated= 0; return 0;
	    }
	memcpy( scratch, bytes+ fic[pictureComp].ficOffset,
						    fic[pictureComp].ficSize );
	scratch[fic[pictureComp].ficSize]= '\0';

	appWordFormatDate( (char *)target, targetSize, tm, scratch );
	*pNewSize= strlen( (char *)target );
	*pCalculated= 1; return 0;
	}

    if  ( mergeFormat )
	{
	if  ( strftime( (char *)target, targetSize, defaultFormat, tm ) < 1 )
	    { *pCalculated= 0; return 0;	}

	*pNewSize= strlen( (char *)target );
	*pCalculated= 1; return 0;
	}


    *pCalculated= 0; return 0;
    }

/************************************************************************/
/*									*/
/*  Return the value of one of the document properties.			*/
/*									*/
/************************************************************************/

int docCalculateDocStringFieldString(	int *			pCalculated,
					int *			pNewSize,
					unsigned char *		target,
					int			targetSize,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			partCount,
					const DocumentField *	df )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);
    unsigned char *		val;
    int				len;
    
    switch( df->dfKind )
	{
	case DOCfkAUTHOR:	val= dp->dpAuthor;	break;
	case DOCfkCOMMENTS:	val= dp->dpDoccomm;	break;
	case DOCfkDOCCOMM:	val= dp->dpDoccomm;	break;
	case DOCfkKEYWORDS:	val= dp->dpKeywords;	break;
	case DOCfkSUBJECT:	val= dp->dpSubject;	break;
	case DOCfkTITLE:	val= dp->dpTitle;	break;
	case DOCfkFILENAME:	val= dp->dpFilename;	break;

	default:
	    LDEB(df->dfKind); *pCalculated= 0; return 0;
	}

    if  ( ! val )
	{ val= (unsigned char *)"";	}
    len= strlen( (char *)val );

    if  ( len > targetSize )
	{ LLDEB(len,targetSize); *pCalculated= 0; return 0;	}

    strcpy( (char *)target, (char *)val );
    *pNewSize= len;

    *pCalculated= 1; return 0;
    }

/************************************************************************/
/*									*/
/*  Return the file name of the document.				*/
/*									*/
/*  The file name on the document proprties is assumed to be a full	*/
/*  absolute name. (From "/")						*/
/*									*/
/************************************************************************/

int docCalculateFilenameFieldString(	int *			pCalculated,
					int *			pNewSize,
					unsigned char *		target,
					int			targetSize,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			partCount,
					const DocumentField *	df )
    {
    FieldInstructionsComponent	fic[FIC_COUNT];
    int				n;
    unsigned char *		bytes= df->dfInstructions.mbBytes;

    const DocumentProperties *	dp= &(bd->bdProperties);

    int				fullPath= 0;
    unsigned char *		file= dp->dpFilename;
    unsigned char *		relative= (unsigned char *)0;

    int				size;
    int				comp;

    n= docSplitFieldInstructions( &(df->dfInstructions), fic, FIC_COUNT );
    if  ( n < 1 )
	{ LDEB(n); return -1;	}

    for ( comp= 1; comp < n; comp++ )
	{
	if  ( docIsMergeformat( bytes, fic, n, comp ) )
	    { comp++; continue;	}

	if  ( comp < n-1					&&
	      fic[comp].ficSize == 2				&&
	      ! memcmp( bytes+ fic[comp].ficOffset, "\\p", 2 )	)
	    { fullPath= 1; continue;	}

	LSDEB(comp,(char *)bytes+ fic[comp].ficOffset);
	}

    if  ( ! file )
	{ file= (unsigned char *)"";	}

    if  ( ! fullPath )
	{ relative= (unsigned char *)strrchr( (char *)file, '/' );	}

    if  ( fullPath || ! relative )
	{
	size= strlen( (char *)file );
	if  ( size > targetSize )
	    { LLDEB(size,targetSize); *pCalculated= 0; return 0; }

	memcpy( target, file, size );
	}
    else{
	relative++;

	size= strlen( (char *)relative );
	if  ( size > targetSize )
	    { LLDEB(size,targetSize); *pCalculated= 0; return 0; }

	memcpy( target, relative, size );
	}

    target[size]= '\0';
    *pNewSize= size;
    *pCalculated= 1;

    return 0;
    }

