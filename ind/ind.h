#   ifndef	IND_H						/*  {{	*/
#   define	IND_H

#   include	<stdio.h>

#   define	INDhASIS	1	/*  knudde -> knudde		*/
#   define	INDhFIRSTUP	2	/*  Knudde -> knudde		*/
#   define	INDhIJUP	3	/*  IJsco -> ijsco		*/
#   define	INDhALLUP	4	/*  KNUDDE -> knudde		*/
#   define	INDhTAILUP	5	/*  EDAM -> Edam		*/
#   define	INDhIJTAILUP	6	/*  IJMUIDEN -> IJmuiden	*/

/************************************************************************/
/*									*/
/*  A list of Guesses.							*/
/*									*/
/************************************************************************/

typedef struct IndGuessScore
    {
    unsigned char *	igsWord;
    int			igsScore;
    } IndGuessScore;

typedef struct IndGuessList
    {
    int			iglGuessCount;
    IndGuessScore *	iglGuesses;
    } IndGuessList;

/************************************************************************/
/*									*/
/*  Scores for frequent substitutions: Helps making guesses.		*/
/*									*/
/************************************************************************/

typedef struct GuessSubstitution
    {
    unsigned char *	gsFrom;
    unsigned char *	gsTo;
    int			gsFromLength;
    int			gsToLength;
    int			gsCost;

    } GuessSubstitution;

/************************************************************************/
/*									*/
/*  A possible 'word' that is collected doring the scan of the input.	*/
/*									*/
/************************************************************************/
#   define	FORM_MAX	60

typedef struct PossibleWord
    {
    int				pwStartAt;
    int				pwInsertionPoint;
    int				pwRejectedAt;
    int				pwAcceptedAt;
    unsigned char		pwForm[FORM_MAX+2];

    struct PossibleWord *	pwNext;
    } PossibleWord;

typedef struct SpellScanJob
    {
    PossibleWord *	ssjPossibleWords;
    int			ssjPossibleWordCount;
    } SpellScanJob;

/************************************************************************/
/*  Operating environment for a checker.				*/
/************************************************************************/
typedef struct SpellCheckContext
    {
    char *		sccDictionaryPrefix;
    void *		sccStaticInd;
    void *		sccForgotInd;
    void *		sccLearntInd;
    unsigned char	sccCharKinds[256];
    unsigned char	sccCharShifts[256];
    } SpellCheckContext;

typedef struct SpellGuessContext
    {
    IndGuessList *	sgcGuessList;
    SpellCheckContext *	sgcCheckContext;
    } SpellGuessContext;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void *	indMake( void );
extern void *	indRead( const char * filename, int read_only );
extern int		indPut( void * ind, const unsigned char * key );
extern int		indForget( void * ind, const unsigned char * key );
extern int		indGet( int *, void * ind, const unsigned char * key );
extern void		indFree( void * ind );
extern int		indWrite( void * ind, const char * filename );
extern void *	indMini( void * ind );
extern void *	indRenumber( void * ind );

extern int indGuess(	void *				voidind,
			const unsigned char *		word,
			SpellGuessContext *		sgc,
			int				how,
			const GuessSubstitution *	typos,
			int				count,
			const unsigned char *	charKinds,
			const unsigned char *	charShifts );

extern int	indSetItem( void *, int, int );
extern int	indGetItems( void *, int, int *, int ** );

extern int	indGetWord(	int *			pWhatWasShifted,
				void *			voidind,
				const unsigned char *	word,
				int			asPrefix,
				const unsigned char *	charKinds,
				const unsigned char *	charShifts );

extern int indGuessWord(    void *				voidind,
			    const unsigned char *		word,
			    SpellGuessContext *			sgc,
			    const GuessSubstitution *		typos,
			    int					count,
			    const unsigned char *		charKinds,
			    const unsigned char *		charShifts );

extern int indShiftWord(	unsigned char *		copy,
				const unsigned char *	word,
				int			how,
				const unsigned char *	charKinds,
				const unsigned char *	charShifts );

extern int indAddGuess(		IndGuessList *		igl,
				const unsigned char *	word,
				int			score	);

extern void 	indCleanGuessList( IndGuessList *		igl );
extern void 	indEmptyGuessList( IndGuessList *		igl );
extern void 	indInitGuessList( IndGuessList *		igl );
extern void 	indSortGuesses(	IndGuessList *		igl );

extern void	indLogPossibilities(	SpellScanJob *	ssj	);

extern int	indNewPossibility(	SpellScanJob *	ssj,
					int		position,
					int		c	);

extern void indAddCharacterToPossibilities(	SpellScanJob *	ssj,
						int		c	);

extern int indCountPossibilities(	SpellScanJob *		ssj,
					SpellCheckContext *	scc,
					int			position,
					int			nextPosition,
					int			rejectPrefices,
					int			nextCharacter );

extern void indRejectPossibilities(	int *			pAcceptedPos,
					int			acceptedPos,
					SpellScanJob *		ssj	);

extern PossibleWord * indMaximalPossibility(	SpellScanJob *	ssj	);

extern int	indMoveWord(	void *			fromInd,
				void *			toInd,
				const unsigned char *	word );

extern int indReadPrivateDictionary(	FILE *		f,
					void **		pLearntInd,
					void **		pForgotInd	);

extern int indLearnWord(	FILE *			f,
				void *			learntInd,
				void *			forgotInd,
				const unsigned char *	word	);

extern int indForgetWord(	FILE *			f,
				void *			learntInd,
				void *			forgotInd,
				const unsigned char *	word	);

extern void indInitSpellScanJob(	SpellScanJob *	ssj );
extern void indCleanSpellScanJob(	SpellScanJob *	ssj );

extern void indInitSpellGuessContext(	SpellGuessContext *	sgc,
					IndGuessList *		igl,
					SpellCheckContext *	scc );
extern void indCleanSpellGuessContext(	SpellGuessContext *	sgc );

extern void indInitSpellCheckContext(  SpellCheckContext *     scc );
extern void indCleanSpellCheckContext(  SpellCheckContext *     scc );

extern void indSpellIso1CharacterKinds(	SpellCheckContext *	scc );
extern void indSpellIso2CharacterKinds(	SpellCheckContext *	scc );
extern void indSpellIso7CharacterKinds(	SpellCheckContext *	scc );
extern void indSpellIso5CharacterKinds(	SpellCheckContext *	scc );
extern void indSpellKoi8rCharacterKinds( SpellCheckContext *	scc );

#   endif /*	IND_H						    }}	*/
