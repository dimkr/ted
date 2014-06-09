#   include	"indConfig.h"

#   include	<ind.h>
#   include	"indlocal.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Make an empty finite automaton.					*/
/*									*/
/************************************************************************/

void *	indMake( void )
    { return (void *)indINDmake( 0 );	}

/************************************************************************/
/*									*/
/*  Read a finite automaton from file. If the read_only flag is set,	*/
/*  the file is memory mapped. This han an advantage on some operating	*/
/*  systems.								*/
/*									*/
/************************************************************************/

void *	indRead( filename, read_only )
const char *	filename;
int		read_only;
    { return (void *)indINDread(  filename, read_only );	}

/************************************************************************/
/*  Make automaton voidint accept word key. The number of the accepting	*/
/*  state is returned, or -1 on failure.				*/
/************************************************************************/
int	indPut(	void *			voidind,
		const unsigned char *	key )
    {
    IND *	ind= (IND *)voidind;

    if  ( ind->ind_magic != INDMAGIC )
	{ LDEB(ind->ind_magic); return -1;	}

    return indINDput( ind, key );
    }

/************************************************************************/
/*  Tell automaton voidind to no longer accept the word key. This is	*/
/*  done by making the state that accepted key a non final state.	*/
/************************************************************************/
int	indForget( voidind, key )
void *			voidind;
const unsigned char *	key;
    {
    IND *	ind= (IND *)voidind;

    if  ( ind->ind_magic != INDMAGIC )
	{ LDEB(ind->ind_magic); return -1;	}

    return indINDforget( ind, (unsigned char *)key );
    }

/************************************************************************/
/*  Find the state in automaton voidind to which key is a path.		*/
/*  returns the number of the state or -1 if no such state exists.	*/
/*  *paccpt is set to 0 if it is a non final state, to other values in	*/
/*  final states.							*/
/************************************************************************/
int	indGet(	int *			paccept,
		void *			voidind,
		const unsigned char *	key		)
    {
    IND *	ind= (IND *)voidind;

    if  ( ind->ind_magic != INDMAGIC )
	{ LDEB(ind->ind_magic); return -1;	}

    return indINDget( paccept, ind, -1, key );
    }

/************************************************************************/
/*  See whether automaton voidind accepts word key.			*/
/*  Certain case mapping are tried if the word is not accepted as is.	*/
/*  The way case is changed is reported in *phow.			*/
/*  The value of *phow is set to one of the INDhSOMETHING values from	*/
/*  ind.h.								*/
/************************************************************************/
int	indGetWord(	int *			pWhatWasShifted,
			void *			voidind,
			const unsigned char *	word,
			int			asPrefix,
			const unsigned char *	charKinds,
			const unsigned char *	charShifts )
    {
    IND *	ind= (IND *)voidind;

    if  ( ind->ind_magic != INDMAGIC )
	{ LDEB(ind->ind_magic); return -1;	}

    return indWRDget( ind, pWhatWasShifted, word, asPrefix,
						    charKinds, charShifts );
    }

/************************************************************************/
/*  free a finite automaton.						*/
/************************************************************************/
void	indFree( voidind )
void *	voidind;
    {
    IND *	ind= (IND *)voidind;

    if  ( ind->ind_magic != INDMAGIC )
	{ return;	}

    indINDfree( ind );
    }

/************************************************************************/
/*  Write an automaton to file.						*/
/************************************************************************/
int	indWrite( voidind, filename )
void *		voidind;
const char *	filename;
    {
    IND *	ind= (IND *)voidind;

    if  ( ind->ind_magic != INDMAGIC )
	{ LDEB(ind->ind_magic); return -1;	}

    return indINDwrite( ind, filename );
    }

/************************************************************************/
/*  Minimise an automaton.						*/
/************************************************************************/
void *	indMini( voidind )
void *	voidind;
    {
    IND *	ind= (IND *)voidind;

    if  ( ind->ind_magic != INDMAGIC )
	{ return (void *)0;	}

    return (void *)indINDmini( ind );
    }

/************************************************************************/
/*  Return an automaton that has the same properties as voidind.	*/
/*  Node numbers and transition numbers are given using a 'depth first'	*/
/*  scan of 'voidind'.							*/
/************************************************************************/
void *	indRenumber( voidind )
void *	voidind;
    {
    IND *	ind= (IND *)voidind;

    if  ( ind->ind_magic != INDMAGIC )
	{ return (void *)0;	}

    return (void *)indINDrenumber( ind );
    }

/************************************************************************/
/*  Make guesses for a certain word.					*/
/*  For every guess, (*fun)( something, <guess>, how ) is called.	*/
/************************************************************************/
int indGuess(	void *				voidind,
		const unsigned char *		word,
		SpellGuessContext *		sgc,
		int				how,
		const GuessSubstitution *	typos,
		int				count,
		const unsigned char *		charKinds,
		const unsigned char *		charShifts )
    {
    IND *	ind= (IND *)voidind;

    if  ( ind->ind_magic != INDMAGIC )
	{ LDEB(ind->ind_magic); return -1;	}

    return indINDguess( ind, word, sgc, how,
				typos, count, charKinds, charShifts );
    }

/************************************************************************/
/*  Make guesses for a certain word.					*/
/*  If the word starts with a capital, of consists of capitals only,	*/
/*  the guesses are based on the word or the tail of the word mapped	*/
/*  to lower case. The how argument to fun is used to report what was	*/
/*  mapped.								*/
/*  For every guess, (*fun)( something, <guess>, how ) is called.	*/
/************************************************************************/
int indGuessWord(	void *				voidind,
			const unsigned char *		word,
			SpellGuessContext *		sgc,
			const GuessSubstitution *	typos,
			int				count,
			const unsigned char *		charKinds,
			const unsigned char *		charShifts )
    {
    IND *	ind= (IND *)voidind;

    if  ( ind->ind_magic != INDMAGIC )
	{ LDEB(ind->ind_magic); return -1;	}

    return indWRDguess( ind, word, sgc, typos, count, charKinds, charShifts );
    }

/************************************************************************/
/*  Remember an integer in a state on an automaton.			*/
/************************************************************************/
int	indSetItem( voidind, tn, item )
void *	voidind;
int	tn;
int	item;
    {
    IND *	ind= (IND *)voidind;

    if  ( ind->ind_magic != INDMAGIC	||
	  tn < 0			||
	  tn >= ind->indAllocatedNodes		)
	{ LDEB(ind->ind_magic); return -1;	}

    return indITset( ind, tn, item );
    }

/************************************************************************/
/*  Retrieve the integers that are stored in a state of an automaton.	*/
/************************************************************************/
int	indGetItems( voidind, tn, pnitem, pitems )
void *	voidind;
int	tn;
int *	pnitem;
int **	pitems;
    {
    IND *	ind= (IND *)voidind;

    if  ( ind->ind_magic != INDMAGIC	||
	  tn < 0			||
	  tn >= ind->indAllocatedNodes		)
	{ LDEB(ind->ind_magic); return -1;	}

    return indITget( ind, tn, pnitem, pitems );
    }
