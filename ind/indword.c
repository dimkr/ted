#   include	"indConfig.h"

#   include	<ind.h>
#   include	"indlocal.h"
#   include	<charnames.h>
#   include	<appDebugon.h>

/************************************************************************/
/*  See whether a word is accepted.					*/
/*  As this is a utility routine for spelling checking, the following	*/
/*  attempts are made to accept it when it is not quite acceptable:	*/
/************************************************************************/

int indWRDget(	IND *			ind,
		int *			pWhatWasShifted,
		const unsigned char *	word,
		int			asPrefix,
		const unsigned char *	charKinds,
		const unsigned char *	charShifts )
    {
    static unsigned char *	copy;

    int				l;
    const unsigned char *	r;
    unsigned char *		s;
    int				accepted;
    int				tn;

    tn= indINDget( &accepted, ind, -1, word );
    if  ( tn >= 0 && ( accepted || asPrefix ) )
	{ *pWhatWasShifted= INDhASIS; return 0;	}

    l= strlen( (char *)word );
    copy= realloc( copy, l+ 2 );

    if  ( ! copy )
	{ LDEB(copy); return -1;	}

    strcpy( (char *)copy, (const char *)word );

    if  ( charKinds[ word[0] ] & CHARisUPPER )
	{
	if  ( word[0] == 'I' && word[1] == 'J' )
	    {
	    copy[0]= 'i'; copy[1]= 'j';
	    if  ( indINDget( &accepted, ind, -1, copy ) >= 0	&&
		  ( accepted || asPrefix )			)
		{ *pWhatWasShifted= INDhIJUP; return 0;	}
	    }
	else{
	    if  ( ! ( charKinds[ word[1] ] & CHARisUPPER ) )
		{
		copy[0]= charShifts[ word[0] ];
		if  ( indINDget( &accepted, ind, -1, copy ) >= 0	&&
		      ( accepted || asPrefix )			)
		    { *pWhatWasShifted= INDhFIRSTUP; return 0;	}
		}
	    }

	/************************************************/
	/*  Shift down.					*/
	/************************************************/
	r= word; s= copy;
	while( *r && ( charKinds[ *r ] & CHARisUPPER ) )
	    {
	    *s= charShifts[*r];
	    s++; r++;
	    }

	/************************************************/
	/*  Not completely in upper case		*/
	/************************************************/
	if  ( *r )
	    { return -1; }

	/************************************************/
	/*  Try shift down.				*/
	/************************************************/
	if  ( indINDget( &accepted, ind, -1, copy ) >= 0	&&
	      ( accepted || asPrefix )			)
	    { *pWhatWasShifted= INDhALLUP; return 0;	}

	/************************************************/
	/*  Try tail shifted down.			*/
	/************************************************/
	if  ( word[0] == 'I' && word[1] == 'J' )
	    {
	    copy[0]= word[0]; copy[1]= word[1];
	    if  ( indINDget( &accepted, ind, -1, copy ) >= 0	&&
		  ( accepted || asPrefix )			)
		{ *pWhatWasShifted= INDhIJTAILUP; return 0;	}
	    }
	else{
	    copy[0]= word[0];
	    if  ( indINDget( &accepted, ind, -1, copy ) >= 0	&&
		  ( accepted || asPrefix )			)
		{ *pWhatWasShifted= INDhTAILUP; return 0;	}
	    }
	}

    return -1;
    }

/************************************************************************/
/*  Generate guesses for a word.					*/
/************************************************************************/
int	indWRDguess(	IND *				ind,
			const unsigned char *		word,
			SpellGuessContext *		sgc,
			const GuessSubstitution *	typos,
			int				count,
			const unsigned char *		charKinds,
			const unsigned char *		charShifts )
    {
    int				l;
    static unsigned char *	copy;

    const unsigned char *	r;
    unsigned char *		s;

    l= strlen( (char *)word );
    copy= realloc( copy, l+ 2 );
    if  ( ! copy )
	{ LDEB(copy); return -1;	}
    strcpy( (char *)copy, (const char *)word );

    if  ( charKinds[ word[0] ] & CHARisUPPER )
	{
	if  ( word[0] == 'I' && word[1] == 'J' )
	    {
	    copy[0]= 'i'; copy[1]= 'j';
	    indINDguess( ind, copy, sgc, INDhIJUP,
				    typos, count, charKinds, charShifts );
	    }
	else{
	    if  ( ! ( charKinds[ word[1] ] & CHARisUPPER ) )
		{
		copy[0]= charShifts[ word[0] ];
		indINDguess( ind, copy, sgc, INDhFIRSTUP,
				    typos, count, charKinds, charShifts );
		}
	    }

	/************************************************/
	/*  Shift down.					*/
	/************************************************/
	r= word; s= copy;
	while( *r && ( charKinds[ *r ] & CHARisUPPER ) )
	    {
	    *s= charShifts[*r];
	    s++; r++;
	    }

	/************************************************/
	/*  Not completely in upper case		*/
	/************************************************/
	if  ( *r )
	    { return 0; }

	/************************************************/
	/*  Try shift down.				*/
	/************************************************/
	indINDguess( ind, copy, sgc, INDhALLUP,
				    typos, count, charKinds, charShifts );

	/************************************************/
	/*  Try tail shifted down.			*/
	/************************************************/
	if  ( word[0] == 'I' && word[1] == 'J' )
	    {
	    copy[0]= word[0]; copy[1]= word[1];
	    indINDguess( ind, copy, sgc, INDhIJTAILUP,
				    typos, count, charKinds, charShifts );
	    }
	else{
	    copy[0]= word[0];
	    indINDguess( ind, copy, sgc, INDhTAILUP,
				    typos, count, charKinds, charShifts );
	    }
	}
    else{
	indINDguess( ind, word, sgc, INDhASIS,
				    typos, count, charKinds, charShifts );
	}

    return 0;
    }

int	indShiftWord(	unsigned char *		copy,
			const unsigned char *	word,
			int			how,
			const unsigned char *	charKinds,
			const unsigned char *	charShifts )
    {
    int	i;

    strcpy( (char *)copy, (const char *)word );

    switch( how )
	{
	case	INDhASIS:
	    break;
	case	INDhFIRSTUP:
	    if  ( charKinds[ copy[0] ] & CHARisLOWER )
		{ copy[0]= charShifts[ copy[0] ]; }
	    break;
	case	INDhIJUP:
	    if  ( copy[0] == 'i' && copy[1] == 'j' )
		{ copy[0]= 'I'; copy[1]= 'J';	}
	    break;
	case	INDhALLUP:
	    i= 0;
	    while( copy[i] )
		{
		if  ( charKinds[ copy[i] ] & CHARisLOWER )
		    { copy[i]= charShifts[ copy[i] ]; }
		i++;
		}
	    break;
	case	INDhTAILUP:
	    i= 0;
	    while( copy[i] )
		{
		if  ( charKinds[ copy[i] ] & CHARisLOWER )
		    { copy[i]= charShifts[ copy[i] ]; }
		i++;
		}
	    break;
	case	INDhIJTAILUP:
	    if  ( copy[0] != 'I' || copy[1] != 'J' )
		{ SDEB((char *)copy); return -1;	}
	    i= 2;
	    while( copy[i] )
		{
		if  ( charKinds[ copy[i] ] & CHARisLOWER )
		    { copy[i]= charShifts[ copy[i] ]; }
		i++;
		}
	    break;
	default:
	    LDEB(how); return -1;
	}

    return 0;
    }
