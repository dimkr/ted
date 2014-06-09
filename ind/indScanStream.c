#   include	"indConfig.h"

#   include	<indScanStream.h>
#   include	<sioGeneral.h>
#   include	<charnames.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Scan an input stream for unknown words. Optionally make guesses for	*/
/*  the unknown words.							*/
/*									*/
/*  1)  Skip inital white space and diacritical signs.			*/
/*  2)  End of stream.. ready.						*/
/*  3)  Collect the first character for the list of possibilities.	*/
/*  4)  Collect subsequent characters upto a space or diacritical.	*/
/*  5)  Count the possibilities in the list.				*/
/*  6)  If no possibilities are left.. it must be a misspelling.	*/
/*									*/
/************************************************************************/

int indScanStream(	SimpleInputStream *	sis,
			SpellCheckContext *	scc,
			void *			through )
    {
    int				rval= 0;

    SpellScanJob		ssj;
    SpellGuessContext		sgc;
    IndGuessList		igl;

    int				position= -1;
    int				acceptedPos= position;

    indInitSpellScanJob( &ssj );
    indInitGuessList( &igl );
    indInitSpellGuessContext( &sgc, &igl, scc );

    for (;;)
	{
	int		c;
	int		count;

	/*  1  */
	while( ( c= sioInGetCharacter( sis ) ) != EOF )
	    {
	    position++;
	    if  ( ( scc->sccCharKinds[ c ] & CHARisALNUM ) )
		{ break;	}
	    }

	/*  2  */
	if  ( c == EOF )
	    { goto ready;	}

	/*  3  */
	indAddCharacterToPossibilities( &ssj, c );

	if  ( indNewPossibility( &ssj, position, c ) )
	    { LDEB(position); rval= -1; goto ready;	}

	/*  4  */
	while( ( c= sioInGetCharacter( sis ) ) != EOF )
	    {
	    position++;
	    if  ( ! ( scc->sccCharKinds[ c ] & CHARisALNUM ) )
		{ break;	}

	    indAddCharacterToPossibilities( &ssj, c );
	    }

	/*  5  */
	count= indCountPossibilities( &ssj, scc, position, position,
							    c == EOF, c );

	/*  6  */
	if  ( count == 0 )
	    {
	    PossibleWord *	maxpw;

	    maxpw= indMaximalPossibility( &ssj );

	    if  ( ! maxpw )
		{ XDEB(maxpw); rval= -1; goto ready;	}
SDEB((char *)maxpw->pwForm);
	    }

	indRejectPossibilities( &acceptedPos, acceptedPos, &ssj );
	indAddCharacterToPossibilities( &ssj, c );
	}

  ready:

    indCleanGuessList( &igl );
    indCleanSpellGuessContext( &sgc );
    indCleanSpellScanJob( &ssj );

    return rval;
    }

