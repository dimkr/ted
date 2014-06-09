/************************************************************************/
/*									*/
/*  Ted: Manipulation of font and text attributes.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedApp.h"
#   include	"tedEdit.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Get font properties of the current position.			*/
/*									*/
/************************************************************************/

static void docGetPositionAttributes(
				const BufferDocument *		bd,
				const TextParticule *		tp,
				const char **			pFamilyName,
				PropertyMask *			pUpdMask,
				TextAttribute *			pTaNew )
    {
    const DocumentFontList *	dfl= &(bd->bdProperties.dpFontList);
    DocumentFont *		df;

    TextAttribute		ta;

    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

    df= dfl->dflFonts+ ta.taFontNumber;

    *pFamilyName= df->dfName;

    *pTaNew= ta;

    PROPmaskCLEAR( pUpdMask );
    utilPropMaskFill( pUpdMask, TAprop_COUNT );

    return;
    }

/************************************************************************/
/*									*/
/*  Get the common text properties of the current selection.		*/
/*									*/
/*  1)  Get the last particule number for the beginning of the 		*/
/*	selection.							*/
/*  2)  If the paragraph is part of a list, exclude the bullet from	*/
/*	the selection.							*/
/*									*/
/************************************************************************/

void docGetSelectionAttributes(	BufferDocument *		bd,
				const DocumentSelection *	ds,
				PropertyMask *			pUpdMask,
				TextAttribute *			pTaNew )
    {
    const DocumentFontList *	dfl= &(bd->bdProperties.dpFontList);
    BufferItem *		paraBi;
    int				part;
    const TextParticule *	tp;


    int				psFamilyNumber= -1;
    const char *		familyName= (const char *)0;

    TextAttribute		taNew;
    PropertyMask		updMask;

    int				bulletFieldNr= -1;
    int				bulletPartBegin= -1;
    int				bulletPartEnd= -1;
    int				bulletStroffBegin= -1;
    int				bulletStroffEnd= -1;

    int				returnBullet= 0;

    paraBi= ds->dsBegin.dpBi;

    /*  1  */
    {
    const int			lastOne= 1;

    if  ( docFindParticuleOfPosition( &part, &(ds->dsBegin), lastOne ) )
	{
	LDEB(ds->dsBegin.dpStroff);
	PROPmaskCLEAR( pUpdMask );
	return;
	}
    }

    /*  2  */
    if  ( paraBi->biParaListOverride > 0 )
	{
	if  ( docDelimitParaHeadField( &bulletFieldNr,
			&bulletPartBegin, &bulletPartEnd,
			&bulletStroffBegin, &bulletStroffEnd, paraBi, bd ) )
	    { LDEB(1);	}

	if  ( part <= bulletPartEnd )
	    {
	    if  ( ! docSelectionInsideParagraph( ds )	||
		  ds->dsEnd.dpStroff > bulletStroffEnd	)
		{ part= bulletPartEnd+ 1;	}
	    else{ returnBullet= 1;		}
	    }
	}

    tp= paraBi->biParaParticules+ part;

    docGetPositionAttributes( bd, tp, &familyName, &updMask, &taNew );

    if  ( docIsIBarSelection( ds ) || returnBullet )
	{
	*pUpdMask= updMask;
	*pTaNew= taNew;
	return;
	}

    PROPmaskCLEAR( &updMask );

    for (;;)
	{
	int	col;

	col= paraBi->biParent->biNumberInParent;

	if  ( ds->dsCol0 < 0					||
	      ds->dsCol1 < 0					||
	      ( col >= ds->dsCol0 && col <= ds->dsCol1 )	)
	    {
	    while( part < paraBi->biParaParticuleCount )
		{
		PropertyMask	pm;
		PropertyMask	pmAll;

		TextAttribute	ta;
		DocumentFont *	df;

		if  ( paraBi == ds->dsEnd.dpBi			&&
		      tp->tpStroff >= ds->dsEnd.dpStroff	)
		    { break;	}

		utilGetTextAttributeByNumber( &ta,
						&(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );
		df= dfl->dflFonts+ ta.taFontNumber;

		PROPmaskCLEAR( &pm );

		PROPmaskCLEAR( &pmAll );
		utilPropMaskFill( &pmAll, TAprop_COUNT );

		utilAttributeDifference( &pm, &taNew, &ta, &pmAll );

		utilPropMaskOr( &updMask, &updMask, &pm );

		tp++; part++;
		}
	    }

	if  ( paraBi == ds->dsEnd.dpBi )
	    { break;	}

	paraBi= docNextParagraph( paraBi );
	if  ( ! paraBi )
	    { break;	}

	part= 0;
	tp= paraBi->biParaParticules+ part;

	/*  2  */
	if  ( paraBi->biParaListOverride > 0 )
	    {
	    if  ( docDelimitParaHeadField( &bulletFieldNr,
			&bulletPartBegin, &bulletPartEnd,
			&bulletStroffBegin, &bulletStroffEnd, paraBi, bd ) )
		{ LDEB(1);	}

	    part= bulletPartEnd+ 1;
	    tp= paraBi->biParaParticules+ part;
	    }
	}

    utilPropMaskNot( &updMask, &updMask );

    if  ( PROPmaskISSET( &updMask, TApropDOC_FONT_NUMBER ) )
	{
	DocumentFont *		df= dfl->dflFonts+ taNew.taFontNumber;

	familyName= df->dfName;
	psFamilyNumber= df->dfPsFamilyNumber;
	}
    else{
	familyName= (char *)0;
	psFamilyNumber= -1;
	}

    *pUpdMask= updMask;
    *pTaNew= taNew;

    return;
    }

