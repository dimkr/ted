/************************************************************************/
/*									*/
/*  Change the font of stretches of text.				*/
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

/************************************************************************/
/*									*/
/*  Change the TextAttributes E.G. the font of a stretch of text	*/
/*  particules.								*/
/*									*/
/************************************************************************/

int docChangeParticuleAttributes(	PropertyMask *		pTaAllMask,
					BufferDocument *	bd,
					BufferItem *		bi,
					int			part,
					int			partUpto,
					const TextAttribute *	taSet,
					const PropertyMask *	taSetMask )
    {
    TextParticule *		tp;
    int				i;

    tp= bi->biParaParticules+ part;
    for ( i= part; i < partUpto; tp++, i++ )
	{
	TextAttribute	ta;
	int		attributeNumber= tp->tpTextAttributeNumber;
	PropertyMask	doneMask;

	PROPmaskCLEAR( &doneMask );

	utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

	utilUpdateTextAttribute( &doneMask, &ta, taSet, taSetMask );

	if  ( ! utilPropMaskIsEmpty( &doneMask )	||
	      attributeNumber < 0		)
	    {
	    if  ( pTaAllMask )
		{ utilPropMaskOr( pTaAllMask, pTaAllMask, &doneMask );	}

	    attributeNumber= utilTextAttributeNumber(
					&(bd->bdTextAttributeList), &ta );
	    if  ( attributeNumber < 0 )
		{ LDEB(attributeNumber); return -1;	}

	    tp->tpTextAttributeNumber= attributeNumber;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Map text attribute numbers from one document to another document.	*/
/*									*/
/************************************************************************/

int docMapTextAttributeNumber(	DocumentCopyJob *	dcj,
				int			attributeNumberFrom )
    {
    TextAttribute		taTo;
    TextAttribute		taFrom;

    int			attributeNumberTo;

    if  ( attributeNumberFrom < 0 )
	{ LDEB(attributeNumberFrom); return attributeNumberFrom;	}

    if  ( dcj->dcjBdTo == dcj->dcjBdFrom )
	{ return attributeNumberFrom;	}

    if  ( attributeNumberFrom == dcj->dcjCurrentTextAttributeNumberFrom )
	{ return dcj->dcjCurrentTextAttributeNumberTo;	}

    utilGetTextAttributeByNumber( &taFrom,
				    &(dcj->dcjBdFrom->bdTextAttributeList),
				    attributeNumberFrom );

    taTo= taFrom;

    if  ( dcj->dcjBdTo != dcj->dcjBdFrom )
	{
	DocumentProperties *	dpTo= &(dcj->dcjBdTo->bdProperties);
	DocumentFontList *	dflTo= &(dpTo->dpFontList);

	taTo.taFontNumber= dcj->dcjFontMap[taFrom.taFontNumber];

	if  ( taTo.taFontNumber >= 0			&&
	      taTo.taFontNumber < dflTo->dflFontCount	)
	    { dflTo->dflFonts[taTo.taFontNumber].dfUsed= 1;	}

	if  ( taFrom.taTextColorNumber > 0 )
	    {
	    taTo.taTextColorNumber= dcj->dcjColorMap[taFrom.taTextColorNumber];
	    }
	}

    attributeNumberTo= utilTextAttributeNumber(
			    &(dcj->dcjBdTo->bdTextAttributeList), &taTo );

    if  ( attributeNumberTo < 0 )
	{ LDEB(attributeNumberTo);	}

    dcj->dcjCurrentTextAttributeNumberFrom= attributeNumberFrom;
    dcj->dcjCurrentTextAttributeNumberTo= attributeNumberTo;

    return attributeNumberTo;
    }
