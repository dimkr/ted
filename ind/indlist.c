#   include	"indConfig.h"

#   include	<string.h>
#   include	<stdlib.h>
#   include	<stdio.h>

#   include	<ind.h>
#   include	<appDebugoff.h>

/************************************************************************/
/*  Insert a guess in a guess list.					*/
/*  1)  If it is already there, just adapt score.			*/
/*  2)  Remember a copy of word.					*/
/*  3)  Store in list.							*/
/************************************************************************/
int	indAddGuess(	IndGuessList *		igl,
			const unsigned char *	word,
			int			score	)
    {
    int			i;
    unsigned int	sz;
    unsigned char *	saved;
    IndGuessScore *	fresh;

    /*  1  */
    for ( i= 0; i < igl->iglGuessCount; i++ )
	{
	if  ( ! strcmp( (char *)igl->iglGuesses[i].igsWord, (char *)word ) )
	    {
	    if  ( igl->iglGuesses[i].igsScore < score )
		{ igl->iglGuesses[i].igsScore=  score;	}

	    return 0;
	    }
	}

    /*  2  */
    sz= strlen( (char *)word )+ 1;
    saved= malloc( sz+ 1 );
    if  ( ! saved )
	{ LLDEB(sz,saved); return -1;	}
    strcpy( (char *)saved, (const char *)word );

    /*  3  */
    sz= ( igl->iglGuessCount+  1 )* sizeof(IndGuessScore);

    fresh= (IndGuessScore *)realloc( (void *)igl->iglGuesses, sz );

    if  ( ! fresh )
	{ LLDEB(sz,fresh); return -1;	}

    igl->iglGuesses= fresh;

    fresh += igl->iglGuessCount++;

    fresh->igsWord= saved;
    fresh->igsScore= score;

    return 0;
    }

void indCleanGuessList( IndGuessList *	igl )
    {
    int			i;

    for ( i= 0; i < igl->iglGuessCount; i++ )
	{
	if  ( igl->iglGuesses[i].igsWord )
	    { free( igl->iglGuesses[i].igsWord ); }
	}

    if  ( igl->iglGuesses )
	{ free( igl->iglGuesses ); }

    igl->iglGuessCount= 0;
    igl->iglGuesses= (IndGuessScore *)0;
    }

void indEmptyGuessList( IndGuessList *	igl )
    {
    int			i;

    for ( i= 0; i < igl->iglGuessCount; i++ )
	{
	if  ( igl->iglGuesses[i].igsWord )
	    { free( igl->iglGuesses[i].igsWord ); }
	}

    igl->iglGuessCount= 0;

    return;
    }

/************************************************************************/
/*  Sort a guess list.							*/
/************************************************************************/
static int indIgsCmp(	const void *	vigs1,
			const void *	vigs2	)
    {
    const IndGuessScore *	igs1= (const IndGuessScore *)vigs1;
    const IndGuessScore *	igs2= (const IndGuessScore *)vigs2;

    return igs2->igsScore- igs1->igsScore;
    }

void indSortGuesses( IndGuessList *	igl )
    {
    if  ( igl->iglGuessCount > 1 )
	{
	qsort(	(void *)igl->iglGuesses,
		igl->iglGuessCount, sizeof(IndGuessScore), indIgsCmp );
	}
    }

void indInitGuessList(	IndGuessList *	igl )
    {
    igl->iglGuessCount= 0;
    igl->iglGuesses= (IndGuessScore *)0;
    }
