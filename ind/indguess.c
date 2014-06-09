#   include	"indConfig.h"

#   include	"indlocal.h"
#   include	<charnames.h>

#   include	<appDebugoff.h>

static int INDGindent;

/************************************************************************/
/*  Guess a series of strings that are stored on an automaton from an	*/
/*  approximation.							*/
/************************************************************************/

typedef int (*IND_HANDLE_GUESS) ( void *, const unsigned char *, int, int );

static int indINDGuessTail(	IND *				ind,
				int				start,
				const unsigned char *		tail,
				unsigned char *			copy,
				int				p,
				int				score,
				SpellGuessContext *		sgc,
				IND_HANDLE_GUESS		fun,
				int				how,
				const GuessSubstitution *	typos,
				int				count,
				const unsigned char *		charKinds,
				const unsigned char *		charShifts )
    {
    int			i, j;
    int			next, try;
    int			ignored;
    TrieLink *		link;
    TrieNode *		node;
    int			transitions;
    int			best= -1;
    int			rval;
    int			cost;

    if  ( score < 0 )
	{ return score;	}

    INDGindent++;

    /********************************************************************/
    /*  While the letters are accepted.					*/
    /********************************************************************/
    while( *tail )
	{
	APP_DEB(appDebug( "%*sGUESS (score %3d) %*s '%.*s%c%s'\n",
					    2*INDGindent, "",
					    score,
					    8- 2*INDGindent, "",
					    p, copy, '^', tail ));

	/****************************************/
	/*  0)  Try the list of typos.		*/
	/****************************************/
	for ( i= 0; i < count; i++ )
	    {
	    if  ( * tail == typos[i].gsFrom[0]		&&
		  typos[i].gsCost <= score		)
		{
		try= indINDstep( &ignored, ind, start, typos[i].gsTo );

		if  ( try >= 0 )
		    {
		    copy[p]= typos[i].gsTo[0];

		    for ( j= 1; j < typos[i].gsFromLength; j++ )
			{
			if  ( tail[j] != typos[i].gsFrom[j] )
			    { break;	}
			}
		    if  ( j == typos[i].gsFromLength )
			{

			for ( j= 1; j < typos[i].gsToLength; j++ )
			    {
			    try= indINDstep( &ignored, ind, try,
							typos[i].gsTo+ j );

			    if  ( try < 0 )
				{ break;	}

			    copy[p+j]= typos[i].gsTo[j];
			    }

			if  ( try >= 0 )
			    {
			    rval= indINDGuessTail( ind, try,
					tail+ typos[i].gsFromLength,
					copy, p+ typos[i].gsToLength,
					score- typos[i].gsCost, sgc,
						fun, how, typos, count,
						charKinds, charShifts );

			    if  ( rval > best )
				{ best= rval;	}
			    }
			}
		    }
		}
	    }

	next= indINDstep( &ignored, ind, start, tail );

	node= NODE( ind, start );
	transitions= node->tn_transitions;

	/****************************************/
	/*  1)  remove one letter.		*/
	/****************************************/
	if  ( tail[1] )
	    {
	    if  ( tail[0] == tail[1] )	{ cost=  3;	}
	    else			{ cost= 12;	}

	    if  ( score >= cost )
		{
		APP_DEB(appDebug( "%*sREMOVE   '%c' COST %d\n",
					    50,"#",
					    tail[0], cost ));

		try= indINDstep( &ignored, ind, start, tail+ 1 );
		if  ( try >= 0 )
		    {
		    copy[p]= tail[1];
		    rval= indINDGuessTail( ind, try, tail+ 2, copy, p+ 1,
					    score- cost, sgc,
					    fun, how, typos, count,
					    charKinds, charShifts );
		    if  ( rval > best )
			{ best= rval;	}
		    }
		}
	    }
	else{
	    cost= 12;

	    if  ( score >= cost )
		{
		APP_DEB(appDebug( "%*sTRUNCATE '%c' COST %d\n",
					    50,"#",
					    tail[0], cost ));

		if  ( NODE(ind,start)->tn_flags & TNfACCEPTS )
		    {
		    copy[p]= '\0';
		    APP_DEB(appDebug( "%*s>>>>> %4d: '%s'\n",
						INDGindent, "",
						score- cost, copy ));
		    (*fun)( sgc, copy, score- cost, how );
		    if  ( score - cost > best )
			{ best= score- cost;	}
		    }
		}
	    }

	/****************************************************************/
	/*  2)  insert one letter. Repeating the next in the tail is	*/
	/*      less expensive.						*/
	/*  3)  replace one letter. Changing case is inexpensive.	*/
	/*  4)  Changing two identical letters at the same time		*/
	/****************************************************************/
	for ( j= 0; j < (int)node->tn_ntrans; j++ )
	    {
	    link= LINK(ind,transitions+j);

	    copy[p]= link->tl_key;

	    /*  2  */
	    if  ( link->tl_key == tail[0] )	{ cost=  3;	}
	    else				{ cost= 12;	}
	    if  ( score >= cost )
		{
		try= indINDstep( &ignored, ind, link->tl_to, tail );
		if  ( try >= 0 )
		    {
		    APP_DEB(appDebug( "%*sINSERT   '%c' COST %d\n",
					    50,"#",
					    link->tl_key, cost ));

		    copy[p+1]= tail[0];
		    rval= indINDGuessTail( ind, try, tail+ 1, copy,
				p+ 2, score- cost, sgc,
				fun, how, typos, count,
				charKinds, charShifts );
		    if  ( rval > best )
			{ best= rval;	}
		    }
		}

	    /*  3  */
	    if  ( ( charKinds[*tail] & CHARisUPPER )	&&
		  copy[p] == charShifts[*tail]		)
		{ cost= 2;	}
	    else{
		if  ( ( charKinds[*tail] & CHARisLOWER )	&&
		      copy[p] == charShifts[*tail]		)
		    { cost=  2;	}
		else{ cost= 10;	}
		}

	    if  ( score >= cost )
		{
		APP_DEB(appDebug( "%*sREPLACE  '%c' WITH '%c' COST %d\n",
					    50,"#",
					    *tail, link->tl_key, cost ));

		rval= indINDGuessTail( ind, link->tl_to, tail+ 1, copy,
			    p+ 1, score- cost, sgc,
			    fun, how, typos, count,
			    charKinds, charShifts );
		if  ( rval > best )
		    { best= rval;	}

		/*  4  */
		if  ( tail[0] == tail[1] )
		    {
		    copy[p+1]= copy[p];
		    cost += 3;
		    if  ( score >= cost )
			{
			try= indINDstep( &ignored, ind, link->tl_to, copy+ p );
			if  ( try >= 0 )
			    {
			    copy[p+1]= copy[p];
			    rval= indINDGuessTail( ind, try, tail+ 2, copy,
				    p+ 2, score- cost, sgc,
				    fun, how, typos, count,
				    charKinds, charShifts );
			    if  ( rval > best )
				{ best= rval;	}
			    }
			}
		    }
		}
	    }

	/********************************/
	/*  transposition.		*/
	/********************************/
	cost= 10;
	if  ( tail[0] && tail[1] && score >= cost )
	    {
	    try= indINDstep( &ignored, ind, start, tail+ 1 );
	    if  ( try >= 0 )
		{
		try= indINDstep( &ignored, ind, try, tail );
		if  ( try >= 0 )
		    {
		    APP_DEB(appDebug( "%*sSWAP     '%c' AND  '%c' COST %d\n",
					    50,"#",
					    tail[0], tail[1], cost ));

		    copy[p]= tail[1]; copy[p+1]= tail[0];
		    rval= indINDGuessTail( ind, try, tail+ 2, copy,
			    p+ 2, score- cost, sgc,
			    fun, how, typos, count,
			    charKinds, charShifts );
		    if  ( rval > best )
			{ best= rval;	}
		    }
		}
	    }

	if  ( next < 0 )
	    { INDGindent--; return -1;			}
	else{ start= next; copy[p++]= *(tail++);	}
	}

    /************************************************************/
    /*  Add at end.						*/
    /************************************************************/
    cost= 10;
    if  ( score >= cost )
	{
	node= NODE( ind, start );
	transitions= node->tn_transitions;

	for ( j= 0; j < (int)node->tn_ntrans; j++ )
	    {
	    link= LINK(ind,transitions+j);

	    APP_DEB(appDebug( "%*sAPPEND   '%c' COST %d\n",
					    50,"#",
					    link->tl_key, cost ));
	    copy[p]= link->tl_key;

	    rval= indINDGuessTail( ind, link->tl_to, tail, copy,
			p+ 1, score- cost, sgc,
			fun, how, typos, count,
			charKinds, charShifts );
	    if  ( rval > best )
		{ best= rval;	}
	    }
	}

    INDGindent--;

    if  ( NODE(ind,start)->tn_flags & TNfACCEPTS )
	{
	copy[p]= '\0';
	APP_DEB(appDebug( "%*s>>>> %4d: '%s'\n", INDGindent+ 1, "",  score, copy ));
	(*fun)( sgc, copy, score, how );
	return score;
	}

    return best;
    }

/************************************************************************/

static int indSpelGuess(	void *			voidsgc,
				const unsigned char *	word,
				int			score,
				int			how )
    {
    SpellGuessContext *		sgc= (SpellGuessContext *)voidsgc;
    IndGuessList *		igl= sgc->sgcGuessList;
    SpellCheckContext *		scc= sgc->sgcCheckContext;

    int				accepted;
    unsigned char		copy[100];

    if  ( indShiftWord( copy, word, how,
				scc->sccCharKinds, scc->sccCharShifts ) )
	{ SLDEB((char *)word,how); return 0;	}

    if  ( scc->sccForgotInd					&&
	  indGet( &accepted, scc->sccForgotInd, copy ) >= 0	&&
	  accepted						)
	{ return 0;	}

    if  ( indAddGuess( igl, copy, score ) )
	{ SLDEB((char *)copy,score); return -1;	}

    return 0;
    }

int indINDguess(	IND *				ind,
			const unsigned char *		word,
			SpellGuessContext *		sgc,
			int				how,
			const GuessSubstitution *	typos,
			int				count,
			const unsigned char *		charKinds,
			const unsigned char *		charShifts )
    {
    int			l= strlen( (char *)word );
    unsigned char *	copy= malloc( 2* l+ 2 );

    SLDEB(word,count);

    if  ( ind->ind_start >= 0 )
	{
	indINDGuessTail( ind, ind->ind_start, word, copy, 0,
			(int)( 1.5* l+ 10 ),
			sgc, indSpelGuess, how, typos, count,
			charKinds, charShifts );
	}

    free( copy );

    return 0;
    }
