/************************************************************************/
/*									*/
/*  Calculate 'Symbol' fields.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>
#   include	"docBuf.h"
#   include	"docEdit.h"
#   include	"docEvalField.h"

/************************************************************************/
/*									*/
/*  Format the value of a 'symbol' field.				*/
/*									*/
/************************************************************************/

int docCalculateSymbolFieldString(	int *			pCalculated,
					int *			pNewSize,
					unsigned char *		target,
					int			targetSize,
					BufferDocument *	bd,
					const BufferItem *	paraBi,
					int			part,
					int			partCount,
					const DocumentField *	df )
    {
    const char *	fontName;
    int			fontSize;
    const char *	symbName;
    int			symbSize;
    const char *	sizeString;
    int			sizeSize;

    char *		past;
    int			symb;

    if  ( docFieldGetSymbol( df, &fontName, &fontSize,
			    &symbName, &symbSize, &sizeString, &sizeSize ) )
	{ LDEB(1); *pCalculated= 0; return 0;	}

    if  ( symbSize < 1 )
	{ LDEB(symbSize); *pCalculated= 0; return 0;	}

    symb= strtol( symbName, &past, 0 );

    if  ( symb < 1 || symb > 255 )
	{ LDEB(symb); *pCalculated= 0; return 0;	}

    target[0]= symb;
    target[1]= '\0';

    *pNewSize= strlen( (char *)target );
    *pCalculated= 1;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Evaluate Footnote number fields.					*/
/*									*/
/************************************************************************/

int docRecalculateParaSymbolTextParticules(
				int *				pCalculated,
				BufferDocument *		bd,
				int *				pPartShift,
				int *				pStroffShift,
				BufferItem *			bi,
				int				part,
				int				partCount,
				DocumentField *			df,
				void *				voidadd,
				DOC_CLOSE_OBJECT		closeObject )
    {
    int				rval= 0;

    const char *		fontName;
    int				fontSize;
    const char *		symbName;
    int				symbSize;
    const char *		sizeString;
    int				sizeSize;

    char *			allocated= (char *)0;

    DocumentProperties *	dp= &(bd->bdProperties);

    if  ( docFieldGetSymbol( df, &fontName, &fontSize,
			    &symbName, &symbSize, &sizeString, &sizeSize ) )
	{ LDEB(1); *pCalculated= 0; goto ready;	}

    rval= docRecalculateParaStringTextParticules( pCalculated, bd,
			    pPartShift, pStroffShift, bi, part, partCount,
			    df, voidadd, closeObject );

    if  ( rval )
	{ LDEB(rval); goto ready;	}

    if  ( *pCalculated )
	{
	const int		charset= FONTcharsetDEFAULT;

	TextAttribute		taSet;
	PropertyMask		taSetMask;

	PROPmaskCLEAR( &taSetMask );
	utilInitTextAttribute( &taSet );

	if  ( fontSize > 0 )
	    {
	    int		fontNumber;

	    allocated= malloc( fontSize+ 1 );
	    if  ( ! allocated )
		{ LXDEB(fontSize,allocated); rval= -1; goto ready;	}

	    strncpy( allocated, fontName, fontSize )[fontSize]= '\0';

	    fontNumber= docGetFontByNameAndCharset(
				&(dp->dpFontList), allocated, charset );
	    if  ( fontNumber < 0 )
		{ SLDEB(allocated,fontNumber);	}
	    else{
		taSet.taFontNumber= fontNumber;
		PROPmaskADD( &taSetMask, TApropDOC_FONT_NUMBER );
		}
	    }

	if  ( sizeSize > 0 )
	    {
	    int		fontSizeHalfPoints;
	    char *	past;

	    fontSizeHalfPoints= strtol( sizeString, &past, 10 );

	    if  ( fontSizeHalfPoints > 0 )
		{ fontSizeHalfPoints *= 2;	}

	    taSet.taFontSizeHalfPoints= fontSizeHalfPoints;
	    PROPmaskADD( &taSetMask, TApropFONTSIZE );
	    }

	if  ( ! utilPropMaskIsEmpty( &taSetMask ) )
	    {
	    if  ( docChangeParticuleAttributes( (PropertyMask *)0, bd,
					bi, part+ 1,
					part+ 1+ partCount+ *pPartShift,
					&taSet, &taSetMask ) )
		{ LDEB(part); rval= -1; goto ready;	}
	    }
	}

  ready: 

    if  ( allocated )
	{ free( allocated );	}

    return rval;
    }
