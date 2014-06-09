/************************************************************************/
/*									*/
/*  Miscelaneous utility routines for managing private dictionaries.	*/
/*									*/
/************************************************************************/

#   include	"indConfig.h"

#   include	<ind.h>
#   include	<string.h>
#   include	<appDebugon.h>

/************************************************************************/
/*  Move a word from forgotten to learnt, or the other way round.	*/
/************************************************************************/
int	indMoveWord(	void *			fromInd,
			void *			toInd,
			const unsigned char *	word )
    {
    int				rval= 0;
    int				accepted;

    if  ( fromInd					&&
	  indGet( &accepted, fromInd, word ) >= 0	&&
	  accepted					)
	{
	if  ( indForget( fromInd, word ) )
	    { SDEB((char *)word); rval= -1;	}
	}

    if  ( indGet( &accepted, toInd, word ) < 0	||
	  ! accepted					)
	{
	if  ( indPut( toInd, word ) < 0 )
	    { SDEB((char *)word); rval= -1;	}
	}

    return rval;
    }

/************************************************************************/
/*  Read a private dictionary and store its contents in two indices.	*/
/************************************************************************/

#   define SPELBUFL	300

int indReadPrivateDictionary(	FILE *		f,
				void **		pLearntInd,
				void **		pForgotInd	)
    {
    void *		learntInd= *pLearntInd;
    void *		forgotInd= *pForgotInd;
    
    unsigned char	buf[SPELBUFL+ 1];

    int			rval= 0;

    if  ( ! learntInd )
	{
	learntInd= indMake();
	if  ( ! learntInd )
	    { XDEB( learntInd ); return -1;	}
	}

    if  ( ! forgotInd )
	{
	forgotInd= indMake();
	if  ( ! forgotInd )
	    { XDEB( forgotInd ); return -1;	}
	}

    while( fgets( (char *)buf, SPELBUFL, f ) == (char *)buf )
	{
	buf[strlen( (char *)buf )- 1]= '\0';

	switch( buf[0] )
	    {
	    case 'F':
		(void) indMoveWord( learntInd, forgotInd, buf+ 2 );
		break;
	    case 'L':
		(void) indMoveWord( forgotInd, learntInd, buf+ 2 );
		break;
	    default:
		SDEB((char *)buf); rval= -1; break;
	    }
	}

    if  ( ! rval )
	{
	*pLearntInd= learntInd;
	*pForgotInd= forgotInd;
	}

    return rval;
    }

/************************************************************************/
/*  Learn/Forget a word and store a record about this in the private	*/
/*  dictionary.								*/
/************************************************************************/
int indLearnWord(	FILE *			f,
			void *			learntInd,
			void *			forgotInd,
			const unsigned char *	word	)
    {
    fprintf( f, "%s %s\n", "L", word );

    if  ( indMoveWord( forgotInd, learntInd, word ) )
	{ SDEB((char *)word); return -1;	}

    return 0;
    }

int indForgetWord(	FILE *			f,
			void *			learntInd,
			void *			forgotInd,
			const unsigned char *	word	)
    {
    fprintf( f, "%s %s\n", "F", word );

    if  ( indMoveWord( learntInd, forgotInd, word ) )
	{ SDEB((char *)word); return -1;	}

    return 0;
    }

