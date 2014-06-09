/************************************************************************/
/*									*/
/*  Spell checking utility routines.					*/
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

#   include	<ind.h>
#   include	<charnames.h>

/************************************************************************/
/*									*/
/*  Find the first misspelling from a certain position in a paragraph.	*/
/*									*/
/*  1)	Skip initial particules: Find the first particule that starts	*/
/*	after the starting point for spell checking.			*/
/*  2)	Skip particules that can be included in a spell check range.	*/
/*  3)	If spell checking starts in the middle of a word, skip to the	*/
/*	end of the word.						*/
/*  4)	Do not extend the string beyond the beginning/end of a field.	*/
/*									*/
/*  TODO: Do not check the contents of fields that are not editable.	*/
/*									*/
/************************************************************************/

int docSpellParaFindNext(	DocumentSelection *		ds,
				BufferItem *			paraBi,
				const BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				void *				through )
    {
    SpellCheckContext *		scc= (SpellCheckContext *)through;
    SpellScanJob		ssj;
    int				stroff= dpFrom->dpStroff;
    int				acceptedPos= stroff;
    const unsigned char *	str= paraBi->biParaString+ stroff;

    const int			lastOne= 1;
    int				part;
    const TextParticule *	tp;

    int				stroffUpto;

    indInitSpellScanJob( &ssj );

    if  ( docFindParticule( &part, paraBi, stroff, lastOne ) )
	{ LDEB(stroff); return -1;	}

    tp= paraBi->biParaParticules+ part;

    /*  1  */
    while( part < paraBi->biParaParticuleCount	&&
	   tp->tpStroff <= stroff		)
	{ part++; tp++;	}

    /*  2  */
    while( part < paraBi->biParaParticuleCount	&&
	   ! DOCkindBETWEEN_TEXT( tp->tpKind )	)
	{ part++; tp++;	}

    /*  3  */
    if  ( stroff > 0						&&
	  ( scc->sccCharKinds[ str[ 0] ] & CHARisALNUM )	&&
	  ( scc->sccCharKinds[ str[-1] ] & CHARisALNUM )	)
	{
	/*  4  */
	if  ( part < paraBi->biParaParticuleCount )
	    { stroffUpto= tp->tpStroff;		}
	else{ stroffUpto= paraBi->biParaStrlen; }

	while( stroff < stroffUpto				&&
	       ( scc->sccCharKinds[ *str ] & CHARisALNUM )	)
	    { stroff++; str++; }
	}

    while( stroff < paraBi->biParaStrlen )
	{
	int		count;

	/*  1  */
	while( part < paraBi->biParaParticuleCount	&&
	       tp->tpStroff <= stroff		)
	    { part++; tp++;	}

	/*  2  */
	while( part < paraBi->biParaParticuleCount	&&
	       ! DOCkindBETWEEN_TEXT( tp->tpKind )	)
	    { part++; tp++;	}

	/*  4  */
	if  ( part < paraBi->biParaParticuleCount )
	    { stroffUpto= tp->tpStroff;		}
	else{ stroffUpto= paraBi->biParaStrlen; }

	while( stroff < stroffUpto				&&
	       ! ( scc->sccCharKinds[ *str ] & CHARisALNUM )	)
	    {
	    indAddCharacterToPossibilities( &ssj, *str );
	    stroff++; str++;
	    }

	if  ( stroff >= paraBi->biParaStrlen )
	    { break;	}
	if  ( stroff >= stroffUpto )
	    { continue;	}

	indAddCharacterToPossibilities( &ssj, *str );

	if  ( indNewPossibility( &ssj, stroff, *str ) )
	    { CDEB(*str); return -1;	}

	stroff++; str++;
	while( stroff < stroffUpto				&&
	       ( scc->sccCharKinds[*str] & CHARisALNUM )	)
	    {
	    indAddCharacterToPossibilities( &ssj, *str );
	    stroff++; str++;
	    }

	count= indCountPossibilities( &ssj, scc, stroff- 1, stroff,
				    stroff >= paraBi->biParaStrlen, *str );

	if  ( count == 0 )
	    {
	    PossibleWord *	maxpw;
	    const int		direction= 1;

	    maxpw= indMaximalPossibility( &ssj );

	    if  ( ! maxpw )
		{ XDEB(maxpw); return -1;	}

	    docSetParaSelection( ds, paraBi, direction,
						maxpw->pwStartAt,
						maxpw->pwInsertionPoint );
	    indCleanSpellScanJob( &ssj );

	    return 0;
	    }

	indRejectPossibilities( &acceptedPos, acceptedPos, &ssj );
	}

    indCleanSpellScanJob( &ssj );

    return 1;
    }

