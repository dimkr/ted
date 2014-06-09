/****************************************************************/
/*								*/
/*  Utility functions for scanning documents for typographical	*/
/*  errors.							*/
/*								*/
/****************************************************************/

#   include	"indConfig.h"

#   include	<charnames.h>

#   include	"indlocal.h"
#   include	<utilFontEncoding.h>
#   include	<appDebugon.h>

/****************************************************************/
/*								*/
/*  Cleanup routine: Free a possibility list.			*/
/*								*/
/****************************************************************/

void	indCleanSpellScanJob(	SpellScanJob *	ssj	)
    {
    PossibleWord *	pw= ssj->ssjPossibleWords;

    while( pw )
	{
	PossibleWord *	next= pw->pwNext;

	free( (char *)pw );

	pw= next;
	}

    return;
    }

void indInitSpellScanJob(	SpellScanJob *	ssj )
    {
    ssj->ssjPossibleWords= (PossibleWord *)0;
    ssj->ssjPossibleWordCount= 0;

    return;
    }

void indInitSpellCheckContext(	SpellCheckContext *	scc )
    {
    scc->sccDictionaryPrefix= "";
    scc->sccStaticInd= (void *)0;
    scc->sccForgotInd= (void *)0;
    scc->sccLearntInd= (void *)0;

    return;
    }

void indCleanSpellCheckContext(	SpellCheckContext *	scc )
    {
    /*
    scc->sccDictionaryPrefix= "";
    */

    if  ( scc->sccStaticInd )
	{ indFree( scc->sccStaticInd );	}
    if  ( scc->sccForgotInd )
	{ indFree( scc->sccForgotInd );	}
    if  ( scc->sccLearntInd )
	{ indFree( scc->sccLearntInd );	}

    return;
    }


void indInitSpellGuessContext(	SpellGuessContext *	sgc,
				IndGuessList *		igl,
				SpellCheckContext *	scc )
    {
    sgc->sgcGuessList= igl;
    sgc->sgcCheckContext= scc;

    return;
    }

void indCleanSpellGuessContext(	SpellGuessContext *	sgc )
    {
    return;
    }

/************************************************************************/
/*									*/
/*  Start a new possibility.						*/
/*									*/
/************************************************************************/

int	indNewPossibility(	SpellScanJob *		ssj,
				int			position,
				int			firstCharacter )
    {
    PossibleWord *	next= ssj->ssjPossibleWords;
    PossibleWord *	pw= (PossibleWord *)malloc(sizeof(PossibleWord));

    if  ( ! pw )
	{ XDEB(pw); return -1;	}

    pw->pwStartAt= position;
    pw->pwInsertionPoint= 1;
    pw->pwRejectedAt= -1;
    pw->pwAcceptedAt= -1;
    pw->pwNext= next;
    pw->pwForm[0]= firstCharacter;
    pw->pwForm[1]= '\0';

    ssj->ssjPossibleWords= pw;

    return 0;
    }

/****************************************************************/
/*  Add a character to all the possibilities in a list.		*/
/****************************************************************/

void indAddCharacterToPossibilities(	SpellScanJob *	ssj,
					int		c	)
    {
    PossibleWord *	pw= ssj->ssjPossibleWords;

    while( pw )
	{
	if  ( pw->pwRejectedAt == -1 )
	    {
	    if  ( pw->pwInsertionPoint >= FORM_MAX )
		{
		pw->pwRejectedAt= pw->pwStartAt+ pw->pwInsertionPoint;
		}
	    else{
		pw->pwForm[pw->pwInsertionPoint++]= c;
		pw->pwForm[pw->pwInsertionPoint  ]= '\0';
		}
	    }

	pw= pw->pwNext;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  1)  Give a judgement on the validity of a word.			*/
/*									*/
/*  2)  In a list of possiblities, count the number that is not yet	*/
/*	rejected.							*/
/*									*/
/************************************************************************/

/*  1  */
static int indCheckWord(	const unsigned char *	word,
				SpellCheckContext *	scc,
				int			asPrefix )
    {
    int				accepted;
    int				ignoredHow;

    if  ( ! asPrefix && isdigit( word[0] ) )
	{ return 0;	}

    if  ( ! asPrefix && scc->sccForgotInd )
	{
	/*  a  */
	if  ( indGet( &accepted, scc->sccForgotInd, word ) >= 0	&&
	      accepted						)
	    { return -1;	}
	}

    /*  b  */
    if  ( ! indGetWord( &ignoredHow, scc->sccStaticInd, word, asPrefix,
				    scc->sccCharKinds, scc->sccCharShifts ) )
	{ return 0;	}

    /*  c  */
    if  ( scc->sccLearntInd &&
	  indGet( &accepted, scc->sccLearntInd, word ) >= 0	&&
	  ( accepted || asPrefix )				)
	{ return 0; }

    return -1;
    }

/*  2  */
int indCountPossibilities(	SpellScanJob *		ssj,
				SpellCheckContext *	scc,
				int			position,
				int			nextPosition,
				int			rejectPrefices,
				int			nextCharacter )
    {
    PossibleWord *	pw= ssj->ssjPossibleWords;
    int			count= 0;

    while( pw )
	{
	if  ( pw->pwRejectedAt == -1 )
	    {
	    int	rejectedAsWord;
	    int	rejectedAsPrefix;

	    rejectedAsWord= indCheckWord( pw->pwForm, scc, 0 );

	    /* LSLDEB(pw->pwStartAt,pw->pwForm,rejectedAsWord); */

	    if  ( rejectedAsWord )
		{
		rejectedAsPrefix= rejectPrefices ||
					indCheckWord( pw->pwForm, scc, 1 );
		if  ( rejectedAsPrefix )
		    { pw->pwRejectedAt= position;	}
		}
	    else{
		rejectedAsPrefix= 0;
		pw->pwAcceptedAt= position;
		}

	    if  ( ! rejectedAsPrefix )
		{
		pw->pwForm[pw->pwInsertionPoint  ]= nextCharacter;
		pw->pwForm[pw->pwInsertionPoint+1]= '\0';
		rejectedAsPrefix= indCheckWord( pw->pwForm, scc, 1 );
		/* SLDEB(pw->pwForm,rejected); */
		pw->pwForm[pw->pwInsertionPoint  ]= '\0';

		if  ( rejectedAsPrefix )
		    {
		    if  ( rejectedAsWord )
			{ pw->pwRejectedAt= position;		}
		    else{ pw->pwRejectedAt= nextPosition;	}
		    }
		}

	    if  ( ! rejectedAsWord || ! rejectedAsPrefix )
		{ count++;	}
	    }

	pw= pw->pwNext;
	}

    return count;
    }

/************************************************************************/
/*									*/
/*  Make a list of the possibilities.					*/
/*									*/
/************************************************************************/

void	indLogPossibilities(	SpellScanJob *	ssj	)
    {
    PossibleWord *	pw= ssj->ssjPossibleWords;

    while( pw )
	{
	appDebug( "\"%s\": Start %d Rejected %d Accepted %d\r\n",
			pw->pwForm,
			pw->pwStartAt, pw->pwRejectedAt, pw->pwAcceptedAt );

	pw= pw->pwNext;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Cleanup routine: Remove rejected possibilities.			*/
/*									*/
/************************************************************************/

void indRejectPossibilities(	int *			pAcceptedPos,
				int			acceptedPos,
				SpellScanJob *		ssj	)
    {
    PossibleWord *	pw= ssj->ssjPossibleWords;
    PossibleWord *	rval= pw;
    PossibleWord *	next;

    while( rval						&&
	   rval->pwRejectedAt != -1			&&
	   rval->pwRejectedAt >= rval->pwAcceptedAt	)
	{
	next= rval->pwNext;

	if  ( acceptedPos < rval->pwAcceptedAt )
	    { acceptedPos=  rval->pwAcceptedAt;	}

	free( (char *)rval );

	rval= next;
	}

    if  ( rval )
	{
	pw= rval;
	while( pw )
	    {
	    next= pw->pwNext;

	    while( next					&&
		   next->pwRejectedAt != -1			&&
		   next->pwRejectedAt >= next->pwAcceptedAt	)
		{
		next= next->pwNext;

		if  ( acceptedPos < rval->pwAcceptedAt )
		    { acceptedPos=  rval->pwAcceptedAt;	}

		free( (char *)pw->pwNext );
		pw->pwNext= next;
		}

	    pw= next;
	    }
	}

    ssj->ssjPossibleWords= rval;

    *pAcceptedPos= acceptedPos;

    return;
    }

/************************************************************************/
/*									*/
/*  Find a maximal possibility to reject. This is rather arbitrary.	*/
/*									*/
/************************************************************************/

PossibleWord * indMaximalPossibility(	SpellScanJob *	ssj	)
    {
    PossibleWord *	pw= ssj->ssjPossibleWords;
    PossibleWord *	rval= pw;

    if  ( ! rval )
	{ return rval;	}

    while( rval && rval->pwAcceptedAt >= 0 )
	{ rval= rval->pwNext; }

    if  ( ! rval )
	{ return rval; }

    pw= rval->pwNext;

    while( pw )
	{
	if  ( pw->pwAcceptedAt == -1 )
	    {
	    if  ( pw->pwStartAt < rval->pwStartAt )
		{ rval= pw; }
	    else{
		if  ( pw->pwStartAt == rval->pwStartAt	&&
		      pw->pwRejectedAt    >  rval->pwRejectedAt   	)
		    { rval= pw; }
		}
	    }

	pw= pw->pwNext;
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Initialise character classification.				*/
/*									*/
/************************************************************************/

void indSpellIso1CharacterKinds(	SpellCheckContext *	scc )
    {
    int		i;

    memset( scc->sccCharKinds, 0, 256 );
    for ( i= 0; i < 256; i++ )
	{ scc->sccCharShifts[i]= i; }

    utilSetLatin1CharacterKinds( scc->sccCharKinds, scc->sccCharShifts );

    return;
    }

void indSpellIso2CharacterKinds(	SpellCheckContext *	scc )
    {
    int		i;

    memset( scc->sccCharKinds, 0, 256 );
    for ( i= 0; i < 256; i++ )
	{ scc->sccCharShifts[i]= i; }

    utilSetLatin2CharacterKinds( scc->sccCharKinds, scc->sccCharShifts );

    return;
    }

void indSpellIso5CharacterKinds(	SpellCheckContext *	scc )
    {
    int		i;

    memset( scc->sccCharKinds, 0, 256 );
    for ( i= 0; i < 256; i++ )
	{ scc->sccCharShifts[i]= i; }

    utilSetLatin5CharacterKinds( scc->sccCharKinds, scc->sccCharShifts );

    return;
    }

void indSpellIso7CharacterKinds(	SpellCheckContext *	scc )
    {
    int		i;

    memset( scc->sccCharKinds, 0, 256 );
    for ( i= 0; i < 256; i++ )
	{ scc->sccCharShifts[i]= i; }

    utilSetLatin7CharacterKinds( scc->sccCharKinds, scc->sccCharShifts );

    return;
    }

void indSpellKoi8rCharacterKinds(	SpellCheckContext *	scc )
    {
    int		i;

    memset( scc->sccCharKinds, 0, 256 );
    for ( i= 0; i < 256; i++ )
	{ scc->sccCharShifts[i]= i; }

    utilSetKoi8rCharacterKinds( scc->sccCharKinds, scc->sccCharShifts );

    return;
    }
