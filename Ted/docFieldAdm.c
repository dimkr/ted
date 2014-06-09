/************************************************************************/
/*									*/
/*  Manage fields.							*/
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

/************************************************************************/
/*									*/
/*  Make a copy of a field.						*/
/*									*/
/************************************************************************/

DocumentField *	docClaimFieldCopy(	DocumentFieldList *	dfl,
					int *			pNr,
					const DocumentField *	dfFrom )
    {
    DocumentField *	dfTo;
    int			fieldNumber;

    DocumentField	dfSaved= *dfFrom;

    dfTo= docClaimField( &fieldNumber, dfl );
    if  ( ! dfTo )
	{ XDEB(dfTo); return (DocumentField *)0;	}

    dfTo->dfKind= dfSaved.dfKind;

    if  ( utilCopyMemoryBuffer( &(dfTo->dfData), &(dfSaved.dfData) ) 	||
	  utilCopyMemoryBuffer( &(dfTo->dfInstructions),
					&(dfSaved.dfInstructions) )	)
	{
	LDEB(1);
	docCleanField( dfTo );
	docInitField( dfTo );
	return (DocumentField *)0;
	}

    *pNr= fieldNumber; return dfTo;
    }

/************************************************************************/
/*									*/
/*  Management of fields.						*/
/*									*/
/************************************************************************/

void docCleanField(	DocumentField *		df )
    {
    utilCleanMemoryBuffer( &df->dfData );
    utilCleanMemoryBuffer( &df->dfInstructions );

    return;
    }

void docInitField(	DocumentField *	df )
    {
    utilInitMemoryBuffer( &df->dfData );
    utilInitMemoryBuffer( &df->dfInstructions );

    df->dfPage= -1;
    df->dfKind= DOCfkFREE;

    return;
    }

int docSetFieldInst(	DocumentField *		df,
			const unsigned char *	bytes,
			int			size )
    { return utilSetMemoryBuffer( &(df->dfInstructions), bytes, size ); }

int docAddToFieldInst(	DocumentField *		df,
			const unsigned char *	bytes,
			int			size )
    { return utilAddToMemoryBuffer( &df->dfInstructions, bytes, size ); }

DocumentField * docClaimField(	int *			pNr,
				DocumentFieldList *	dfl )
    {
    int			n;
    DocumentField *	df;

    df= (DocumentField *)realloc( dfl->dflFields,
			    (dfl->dflFieldCount+ 1)* sizeof(DocumentField) );
    if  ( ! df )
	{ XDEB(df); return (DocumentField *)0;	}
    dfl->dflFields= df;

    for ( n= 0; n < dfl->dflFieldCount; df++, n++ )
	{
	if  ( df->dfKind == DOCfkFREE )
	    { break;	}
	}

    docInitField( df );

    if  ( n == dfl->dflFieldCount )
	{ dfl->dflFieldCount++;	}

    *pNr= n; return df;
    }

void docInitFieldList(	DocumentFieldList *	dfl )
    {
    dfl->dflFields= (DocumentField *)0;
    dfl->dflFieldCount= 0;

    return;
    }

void docCleanFieldList(	DocumentFieldList *	dfl )
    {
    int			i;
    DocumentField *	df;

    df= dfl->dflFields;
    for ( i= 0; i < dfl->dflFieldCount; df++, i++ )
	{
	if  ( df->dfKind == DOCfkFREE )
	    { continue;	}

	docCleanField( df );
	}

    if  ( dfl->dflFields )
	{ free( dfl->dflFields );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Split field instructions in a vector of arguments.			*/
/*									*/
/************************************************************************/

int docSplitFieldInstructions(	const MemoryBuffer *		mb,
				FieldInstructionsComponent *	fic,
				int				maxComponent )
    {
    unsigned char *	bytes= mb->mbBytes;

    int			offset= 0;
    int			n= 0;

    for (;;)
	{
	int		size= 0;

	while( offset < mb->mbSize && *bytes == ' ' )
	    { offset++; bytes++;	}

	if  ( offset >= mb->mbSize )
	    { break;	}

	if  ( n >= maxComponent )
	    { LLDEB(n,maxComponent); return 0;	}

	if  ( bytes[0] == '\\'		&&
	      offset < mb->mbSize- 1	&&
	      bytes[1] != ' '		)
	    {
	    fic->ficOffset= offset;

	    offset += 2;  bytes += 2;
	    fic->ficSize= 2;
	    n++; fic++;
	    continue;
	    }

	if  ( bytes[0] == '"' )
	    {
	    offset++; bytes++;

	    if  ( bytes[0] == '"' )
		{ offset++; bytes++; continue; }

	    fic->ficOffset= offset;
	    while( offset < mb->mbSize && *bytes != '"' )
		{ size++; offset++; bytes++;	}

	    if  ( offset >= mb->mbSize )
		{ SDEB((char *)mb->mbBytes); return 0;	}

	    offset++; bytes++;
	    }
	else{
	    fic->ficOffset= offset;
	    while( offset < mb->mbSize && *bytes != ' ' )
		{ size++; offset++; bytes++;	}
	    }

	fic->ficSize= size;
	n++; fic++;
	}

    return n;
    }

/************************************************************************/
/*									*/
/*  Bring a bookmark in a generally accepted format.			*/
/*									*/
/************************************************************************/

void docAdaptBookmarkName(	int *		pLength,
				char *		markName )
    {
    int		len= 0;

    while( *markName )
	{
	if  ( len >= DOCmaxBOOKMARK )
	    { *markName= '\0'; break;	}

	if  ( ! isalnum( *markName ) )
	    { *markName= '_';	}

	len++; markName++;
	}

    *pLength= len; return;
    }

/************************************************************************/
/*									*/
/*  Look for a bookmark in the field list.				*/
/*									*/
/************************************************************************/

int docGetBookmarkField(	DocumentField **		pDf,
				const char *			markName,
				int				markSize,
				const DocumentFieldList *	dfl )
    {
    DocumentField *	df;
    int			i;

    df= dfl->dflFields;
    for ( i= 0; i < dfl->dflFieldCount; df++, i++ )
	{
	const char *	name;
	int		size;

	if  ( df->dfKind != DOCfkBOOKMARK		||
	      docFieldGetBookmark( df, &name, &size )	)
	    { continue;		}

	if  ( size == 0			||
	      size > DOCmaxBOOKMARK	)
	    { continue;	}

	if  ( size == markSize			&&
	      ! strncmp( markName, name, size )	)
	    { *pDf= df; return i;	}
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Initialize a field recalculate job.					*/
/*									*/
/************************************************************************/

extern void docInitRecalculateFields(   RecalculateFields *     rf )
    {
    rf->rfBd= (BufferDocument *)0;
    rf->rfVoidadd= (void *)0;
    rf->rfCloseObject= (DOC_CLOSE_OBJECT)0;
    rf->rfFieldsUpdated= 0;
    rf->rfUpdateFlags= 0;

    docInitEditPosition( &(rf->rfSelBegin) );
    docInitEditPosition( &(rf->rfSelEnd) );

    return;
    }
