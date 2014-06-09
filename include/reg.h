#   ifndef	REG_H
#   define	REG_H

/*
 * Definitions etc. for regexp(3) routines.
 *
 * Caveat:  this is V8 regexp(3) [actually, a reimplementation thereof],
 * not the System V one.
 */

#define NSUBEXP  10
typedef struct regProg
    {
    unsigned char *	rpStartp[NSUBEXP];
    unsigned char *	rpEndp[NSUBEXP];
    char		regstart;		/* Internal use only. */
    char		reganch;		/* Internal use only. */
    unsigned char *	rpRegmust;		/* Internal use only. */
    int			regmlen;		/* Internal use only. */
    unsigned char	rpProgram[1];		/* Unwarranted chumminess */
						/* with compiler. */
    } regProg;

extern regProg *regCompile( const unsigned char * );
extern regProg *regCompileEscaped( const unsigned char * );
extern void regSubstitute( regProg *, char *, char * );

extern int regFindLeftToRight(	regProg *	prog,
				unsigned char *	string );

extern int regFindRightToLeft(	regProg *	prog,
				unsigned char *	string,
				int		len );

#   endif
