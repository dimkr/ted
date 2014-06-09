/*
 * regcomp and regexec -- regsub and regerror are elsewhere
 * @(#)regexp.c	1.3 of 18 April 87
 *
 *	Copyright (c) 1986 by University of Toronto.
 *	Written by Henry Spencer.  Not derived from licensed software.
 *
 *	Permission is granted to anyone to use this software for any
 *	purpose on any computer system, and to redistribute it freely,
 *	subject to the following restrictions:
 *
 *	1. The author is not responsible for the consequences of use of
 *		this software, no matter how awful, even if they arise
 *		from defects in it.
 *
 *	2. The origin of this software must not be misrepresented, either
 *		by explicit claim or by omission.
 *
 *	3. Altered versions must be plainly marked as such, and must not
 *		be misrepresented as being the original software.
 *
 * NOTE that Mark de Does (M.de.Does@inter.nl.net) made many
 * modifications to this source code without changing the original 
 * approach.
 *
 * Beware that some of this code is subtly aware of the way operator
 * precedence is structured in regular expressions. Serious changes in
 * regular-expression syntax might require a total rethink.
 *
 */

#   include	"libregConfig.h"

#   include	<stdio.h>
#   include	<string.h>
#   include	<stdlib.h>
#   include	<ctype.h>
#   include	<reg.h>
#   include	"regmagic.h"

#   include	<appDebugon.h>

/*
 * The "internal use only" fields in regexp.h are present to pass info from
 * compile to execute that permits the execute phase to run lots faster on
 * simple cases.  They are:
 *
 * regstart	char that must begin a match; '\0' if none obvious
 * reganch	is the match anchored (at beginning-of-line only)?
 * rpRegmust	string (pointer into program) that match must include, or NULL
 * regmlen	length of rpRegmust string
 *
 * Regstart and reganch permit very fast decisions on suitable starting points
 * for a match, cutting down the work a lot.  Regmust permits fast rejection
 * of lines that cannot possibly match.  The rpRegmust tests are costly enough
 * that regcomp() supplies a rpRegmust only if the r.e. contains something
 * potentially expensive (at present, the only such thing detected is * or +
 * at the start of the r.e., which can involve a lot of backup).  Regmlen is
 * supplied because the test in regexec() needs it and regcomp() is computing
 * it anyway.
 */

/*
 * Structure for regexp "program".  This is essentially a linear encoding
 * of a nondeterministic finite-state machine (aka syntax charts or
 * "railroad normal form" in parsing technology).  Each node is an opcode
 * plus a "next" pointer, possibly plus an operand.  "Next" pointers of
 * all nodes except BRANCH implement concatenation; a "next" pointer with
 * a BRANCH on both ends of it is connecting two alternatives.  (Here we
 * have one of the subtle syntax dependencies:  an individual BRANCH (as
 * opposed to a collection of them) is never concatenated with anything
 * because of operator precedence.)  The operand of some types of node is
 * a literal string; for others, it is a node leading into a sub-FSM.  In
 * particular, the operand of a BRANCH node is the first node of the branch.
 * (NB this is *not* a tree structure:  the tail of the branch connects
 * to the thing following the set of BRANCHes.)  The opcodes are:
 */

/* definition	number	opnd?	meaning */
#define	END	0	/* no	End of program. */
#define	BOL	1	/* no	Match "" at beginning of line. */
#define	EOL	2	/* no	Match "" at end of line. */
#define	ANY	3	/* no	Match any one character. */
#define	ANYOF	4	/* str	Match any character in this string. */
#define	ANYBUT	5	/* str	Match any character not in this string. */
#define	BRANCH	6	/* node	Match this alternative, or the next... */
#define	BACK	7	/* no	Match "", "next" ptr points backward. */
#define	EXACTLY	8	/* str	Match this string. */
#define	NOTHING	9	/* no	Match empty string. */
#define	STAR	10	/* node	Match this (simple) thing 0 or more times. */
#define	PLUS	11	/* node	Match this (simple) thing 1 or more times. */
#define	OPEN	20	/* no	Mark this point in input as start of #n. */
			/*	OPEN+1 is number 1, etc. */
#define	CLOSE	30	/* no	Analogous to OPEN. */

/*
 * Opcode notes:
 *
 * BRANCH	The set of branches constituting a single choice are hooked
 *		together with their "next" pointers, since precedence prevents
 *		anything being concatenated to any individual branch.  The
 *		"next" pointer of the last BRANCH in a choice points to the
 *		thing following the whole choice.  This is also where the
 *		final "next" pointer of each individual branch points; each
 *		branch starts with the operand node of a BRANCH node.
 *
 * BACK		Normal "next" pointers all implicitly point forward; BACK
 *		exists to make loop structures possible.
 *
 * STAR,PLUS	'?', and complex '*' and '+', are implemented as circular
 *		BRANCH structures using BACK.  Simple cases (one character
 *		per match) are implemented with STAR and PLUS for speed
 *		and to minimize recursive plunges.
 *
 * OPEN,CLOSE	...are numbered at compile time.
 */

/*
 * A node is one char of opcode followed by two chars of "next" pointer.
 * "Next" pointers are stored as two 8-bit pieces, high order first.  The
 * value is a positive offset from the opcode of the node containing it.
 * An operand, if any, simply follows the node.  (Note that much of the
 * code generation knows about this implicit relationship.)
 *
 * Using two bytes for the "next" pointer is vast overkill for most things,
 * but allows patterns to get big without disasters.
 */
#define	OP(p)	(*(p))
#define	NEXT(p)	(((*((p)+1)&0377)<<8) + (*((p)+2)&0377))
#define	OPERAND(p)	((p) + 3)

/*
 * See regmagic.h for one further detail of program structure.
 */


/*
 * Utility definitions.
 */
#ifndef CHARBITS
#define	UCHARAT(p)	((int)*(unsigned char *)(p))
#else
#define	UCHARAT(p)	((int)*(p)&CHARBITS)
#endif

#define	ISMULT(c)	((c) == '*' || (c) == '+' || (c) == '?')
#define	META		"^$.[()|?+*\\"

/*
 * Flags to be passed up and down.
 */
#define	HASWIDTH	01	/* Known never to match null string. */
#define	SIMPLE		02	/* Simple enough to be STAR/PLUS operand. */
#define	SPSTART		04	/* Starts with * or +. */
#define	WORST		0	/* Worst case. */

typedef struct ExpressionParsingJob
    {
    int				epjParanthesisCount;
    int				epjEmitCode;

    long			epjSize;
    unsigned char *		epjTarget;
    const unsigned char *	epjSource;
    } ExpressionParsingJob;

/*
 * Static routine declarations
 */
static int regrepeat(	unsigned char *		p,
			unsigned char *		scan,
			unsigned char **	pPast );

static int regmatch(	unsigned char *		prog,
			unsigned char *		string,
			unsigned char **	pPast );

static unsigned char * regGetNextNode(	register unsigned char *p );

static unsigned char * regParseGetNextNode(	unsigned char *		p,
						ExpressionParsingJob *	epj );

static char * regprop( unsigned char *op );
static int regtry(	regProg *	prog,
			unsigned char *	string );

static void regoptail(	unsigned char *		p,
			unsigned char *		val,
			ExpressionParsingJob *	epj );

static void regtail(	unsigned char *		p,
			unsigned char *		val,
			ExpressionParsingJob *	epj );

static void regInsertNode(	unsigned char		op,
				unsigned char *		opnd,
				ExpressionParsingJob *	epj );

static void regc(	unsigned char		b,
			ExpressionParsingJob *	epj );

static unsigned char * regnode( unsigned char		op,
				ExpressionParsingJob *	epj );

static unsigned char * regAtomicExpression( int *			flagp,
				ExpressionParsingJob *	epj );

static unsigned char * regpiece( int *			flagp,
				ExpressionParsingJob *	epj );

static unsigned char * regbranch( int *			flagp,
				ExpressionParsingJob *	epj );

static unsigned char * reg(	int			paren,
				int *			flagp,
				ExpressionParsingJob *	epj );


/*
 - regcomp - compile a regular expression into internal code
 *
 * We can't allocate space until we know how big the compiled form will be,
 * but we can't compile it (and thus know how big it is) until we've got a
 * place to put the code.  So we cheat:  we compile it twice, once with code
 * generation turned off and size counting turned on, and once "for real".
 * This also means that we don't allocate space until we are sure that the
 * thing really will compile successfully, and we never have to move the
 * code and thus invalidate pointers into it.  (Note that it has to be in
 * one piece because free() must be able to free it all.)
 *
 * Beware that the optimization-preparation code in here knows about some
 * of the structure of the compiled regexp.
 */
static void regInitParsingJob(	ExpressionParsingJob *	epj )
    {
    epj->epjParanthesisCount= 1;
    epj->epjEmitCode= 0;
    epj->epjSize= 0;
    epj->epjTarget= (unsigned char *)0;
    }

regProg * regCompileEscaped( const unsigned char *	exp )
    {
    int				l;
    unsigned char *		scratch;
    regProg *			rval;

    const unsigned char *	from;
    unsigned char *		to;

    l= strlen( (const char *)exp );
    scratch= malloc( l+ 1 );
    if  ( ! scratch )
	{ LXDEB(l,scratch); return (regProg *)0;	}

    from= exp; to= scratch;
    while( *from )
	{
	if  ( *from == '*'	||
	      *from == '+'	||
	      *from == '?'	||
	      *from == '^'	||
	      *from == '$'	||
	      *from == '.'	||
	      *from == '['	||
	      *from == '('	||
	      *from == '|'	||
	      *from == ')'	||
	      *from == '\\'	)
	    { *(to++)= '\\';	}

	*(to++)= *(from++);
	}

    *to= '\0';

    rval= regCompile( scratch );
    if  ( ! rval )
	{ XDEB(rval);	}

    free( scratch );

    return rval;
    }

regProg * regCompile( const unsigned char *	exp )
{
    register regProg *		r;
    register unsigned char *	scan;
    register unsigned char *	longest;
    register int		len;
    int				flags;

    ExpressionParsingJob	epj;

    unsigned char		dummy;

    if  ( exp == NULL )
	{ XDEB(exp); return (regProg *)0;	}

    /* First pass: determine size, legality. */
    regInitParsingJob( &epj );
    epj.epjEmitCode= 0;
    epj.epjTarget= &dummy;
    epj.epjSource= exp;

    regc( MAGIC, &epj );
    if (reg(0, &flags, &epj ) == NULL)
	    return(NULL);

    /* Small enough for pointer-storage convention? */
    if  ( epj.epjSize+ sizeof(regProg)+ 50 >= 32767L )
	{ LDEB(epj.epjSize); return (regProg *)0;	}

    /* Allocate space. */
    r = (regProg *)malloc(sizeof(regProg) + (unsigned)epj.epjSize);
    if  ( ! r )
	{ LXDEB(epj.epjSize,r); return (regProg *)0;	}

    /* Second pass: emit code. */
    regInitParsingJob( &epj );
    epj.epjEmitCode= 1;
    epj.epjTarget= r->rpProgram;
    epj.epjSource= exp;

    regc(MAGIC, &epj );
    if  ( reg(0, &flags, &epj ) == NULL)
	{ free( r ); return(NULL);	}

    /* Dig out information for optimizations. */
    r->regstart = '\0';	/* Worst-case defaults. */
    r->reganch = 0;
    r->rpRegmust = NULL;
    r->regmlen = 0;
    scan = r->rpProgram+1;			/* First BRANCH. */
    if (OP(regGetNextNode(scan)) == END) {	/* Only one top-level choice. */
	    scan = OPERAND(scan);

	    /* Starting-point info. */
	    if (OP(scan) == EXACTLY)
		    r->regstart = *OPERAND(scan);
	    else if (OP(scan) == BOL)
		    r->reganch++;

	    /*
	     * If there's something expensive in the r.e., find the
	     * longest literal string that must appear and make it the
	     * rpRegmust.  Resolve ties in favor of later strings, since
	     * the regstart check works with the beginning of the r.e.
	     * and avoiding duplication strengthens checking.  Not a
	     * strong reason, but sufficient in the absence of others.
	     */
	    if (flags&SPSTART) {
		    longest = NULL;
		    len = 0;
		    for (; scan != NULL; scan = regGetNextNode(scan))
			    if (OP(scan) == EXACTLY && strlen((char *)OPERAND(scan)) >= len) {
				    longest = OPERAND(scan);
				    len = strlen((char *)OPERAND(scan));
			    }
		    r->rpRegmust = longest;
		    r->regmlen = len;
	    }
    }

    return(r);
}

/*
 - reg - regular expression, i.e. main body or parenthesized thing
 *
 * Caller must absorb opening parenthesis.
 *
 * Combining parenthesis handling with the base level of regular expression
 * is a trifle forced, but the need to tie the tails of the branches to what
 * follows makes it hard to avoid.
 *
 * paren: Parenthesized?
 *
 */

static unsigned char * reg(	int			paren,
				int *			flagp,
				ExpressionParsingJob *	epj )
    {
    register unsigned char *	ret;
    register unsigned char *	br;
    register unsigned char *	ender;
    register int		parno= -1;
    int				flags;

    *flagp = HASWIDTH;	/* Tentatively. */

    /* Make an OPEN node, if parenthesized. */
    if  ( paren )
	{
	if  ( epj->epjParanthesisCount >= NSUBEXP )
	    {
	    LLDEB( epj->epjParanthesisCount,NSUBEXP);
	    return (unsigned char *)0;
	    }

	parno= epj->epjParanthesisCount;
	epj->epjParanthesisCount++;
	ret= regnode( OPEN+parno, epj );
	}
    else{ ret = NULL; 	}

    /* Pick up the branches, linking them together. */
    br = regbranch( &flags, epj );
    if (br == NULL)
	    return(NULL);
    if (ret != NULL)
	    regtail( ret, br, epj );	/* OPEN -> first. */
    else
	    ret = br;
    if (!(flags&HASWIDTH))
	    *flagp &= ~HASWIDTH;
    *flagp |= flags&SPSTART;
    while (*epj->epjSource == '|') {
	    epj->epjSource++;
	    br = regbranch( &flags, epj );
	    if (br == NULL)
		    return(NULL);
	    regtail(ret, br, epj );	/* BRANCH -> BRANCH. */
	    if (!(flags&HASWIDTH))
		    *flagp &= ~HASWIDTH;
	    *flagp |= flags&SPSTART;
    }

    /* Make a closing node, and hook it on the end. */
    ender = regnode((paren) ? CLOSE+parno : END, epj );	
    regtail(ret, ender, epj );

    /* Hook the tails of the branches to the closing node. */
    for (br = ret; br != NULL; br = regParseGetNextNode(br,epj))
	    regoptail(br, ender,epj);

    /* Check for proper termination. */
    if  ( paren )
	{
	if  ( *epj->epjSource != ')' )
	    { LSDEB(paren,(char *)epj->epjSource); return (unsigned char *)0; }

	epj->epjSource++;
	}
    else{
	if  ( *epj->epjSource != '\0' )
	    { SDEB((char *)epj->epjSource); return (unsigned char *)0; }
	}

    return(ret);
    }

/*
 - regbranch - one alternative of an | operator
 *
 * Implements the concatenation operator.
 */
static unsigned char * regbranch(	int *			flagp,
					ExpressionParsingJob *	epj )
{
	register unsigned char *	ret;
	register unsigned char *	chain;
	register unsigned char *	latest;
	int				flags;

	*flagp = WORST;		/* Tentatively. */

	ret= regnode( BRANCH, epj );
	chain = NULL;
	while (*epj->epjSource != '\0' && *epj->epjSource != '|' && *epj->epjSource != ')') {
		latest = regpiece(&flags,epj);
		if (latest == NULL)
			return(NULL);
		*flagp |= flags&HASWIDTH;
		if (chain == NULL)	/* First piece. */
			*flagp |= flags&SPSTART;
		else
			regtail(chain, latest,epj);
		chain = latest;
	}
	if (chain == NULL)	/* Loop ran zero times. */
		(void) regnode( NOTHING, epj );

	return(ret);
}

/************************************************************************/
/*									*/
/*  regpiece - something followed by possible [*+?]			*/
/*									*/
/*  Note that the branching code sequences used for ? and the general	*/
/*  cases of * and + are somewhat optimized:  they use the same NOTHING	*/
/*  node as both the endmarker for their branch list and the body of	*/
/*  the last branch. It might seem that this node could be dispensed	*/
/*  with entirely, but the endmarker role is not redundant.		*/
/*									*/
/*  1)  Consume one atom.						*/
/*  2)  If it is not followed by a repetition operator, just return it.	*/
/*  3)  Things like ^ and $ are consumed by regAtomicExpression(). It	*/
/*	is absurd to repeat them: Check.				*/
/*  4)  ???								*/
/*									*/
/************************************************************************/

static unsigned char * regpiece(	int *			flagp,
					ExpressionParsingJob *	epj )
    {
    register unsigned char *	ret;
    register unsigned char	op;
    register unsigned char *	next;
    int				flags;

    /*  1  */
    ret= regAtomicExpression( &flags, epj );
    if  ( ret == NULL )
	{ XDEB(ret); return ret;	}

    /*  2  */
    op= *epj->epjSource;
    if  ( ! ISMULT( op ) )
	{ *flagp = flags; return ret;	}

    /*  3  */
    if  ( ! ( flags & HASWIDTH ) && op != '?' )
	{ CDEB(op); return (unsigned char *)0; }

    /*  4  */
    *flagp= (op != '+') ? (WORST|SPSTART) : (WORST|HASWIDTH);

    switch( op )
	{
	case '*':
	    if  ( flags & SIMPLE )
		{ regInsertNode( STAR, ret, epj ); break;	}

	    /* Emit x* as (x&|), where & means "self". */
	    regInsertNode( BRANCH, ret, epj );
							/* Either x */
	    regoptail(ret, regnode( BACK, epj ), epj );
							/* and loop back */
	    regoptail(ret, ret, epj );
	    regtail( ret, regnode( BRANCH, epj ), epj );
							/* or */
	    regtail( ret, regnode( NOTHING, epj ), epj );
							/* null. */
	    break;

	case '+':
	    if  ( flags & SIMPLE )
		{ regInsertNode( PLUS, ret, epj ); break;	}

	    /* Emit x+ as x(&|), where & means "self". */
	    next= regnode(BRANCH, epj );
							/* Either */
	    regtail(ret, next, epj );
	    regtail( regnode( BACK, epj ), ret, epj );
							/* loop back */
	    regtail(next, regnode(BRANCH, epj ), epj);
							/* or */
	    regtail(ret, regnode(NOTHING, epj ), epj );
							/* null. */
	    break;

	case '?':
	    /* Emit x? as (x|) */
	    regInsertNode(BRANCH, ret, epj );		
							/* Either x */
	    regtail(ret, regnode(BRANCH, epj  ), epj );
							/* or */
	    next = regnode(NOTHING, epj );
							/* null. */
	    regtail(ret, next, epj );
	    regoptail(ret, next, epj );

	    break;

	default:
	    SDEB((char *)epj->epjSource);
	    return (unsigned char *)0;
	}

    epj->epjSource++;
    if  ( ISMULT( *epj->epjSource ) )
	{
	SDEB((char *)epj->epjSource);
	return (unsigned char *)0;
	}

    return(ret);
    }

/************************************************************************/
/*									*/
/*  regFinishExactPattern().						*/
/*									*/
/*  Finish an exact pattern.						*/
/*									*/
/*  1)  Stop at the end of the input.					*/
/*  2)  Stop at meta-characters. Sometimes a metacharacter is an error,	*/
/*	sometimes they end the exact pattern. By refusing them all,	*/
/*	they will either be processed or rejected at the correct place.	*/
/*  3)  In stead of a backslash process the next character.		*/
/*  4)  Trailing backslashes are refused.				*/
/*  5)  Repetitions: *+? are not part of an exact pattern.		*/
/*  6)  Include the next charcter in the pattern.			*/
/*  7)  Terminate the pattern.						*/
/*  8)  Exact patterns match characters anyway. Only those with length	*/
/*	1 are simple.							*/
/*									*/
/************************************************************************/

static void regFinishExactPattern(	int *			flagp,
					ExpressionParsingJob *	epj )
    {
    int		len= 1;

    for (;;)
	{
	/*  1  */
	if  ( ! epj->epjSource[0] )
	    { break;	}

	/*  2  */
	if  ( strchr( "^$.[()|?+*", epj->epjSource[0] ) )
	    { break;	}

	/*  3  */
	if  ( epj->epjSource[0] == '\\' )
	    {
	    /*  4  */
	    if  ( ! epj->epjSource[1] )
		{ break;	}
	    /*  5  */
	    if  ( ISMULT( epj->epjSource[2] ) )
		{ break;	}

	    epj->epjSource++;
	    }

	/*  5  */
	if  ( ISMULT( epj->epjSource[1] ) )
	    { break;	}

	/*  6  */
	regc( *epj->epjSource++, epj ); len++;
	}

    /*  7  */
    regc( '\0', epj );

    /*  8  */
    if  ( len == 1 )
	{ *flagp |= HASWIDTH|SIMPLE;	}
    else{ *flagp |= HASWIDTH;		}

    return;
    }

/************************************************************************/
/*									*/
/*  regAtomicExpression().						*/
/*									*/
/*  Parse an atomic expression.						*/
/*									*/
/*  Optimization:  gobbles an entire sequence of ordinary characters so	*/
/*  that it can turn them into a single node, which is smaller to store	*/
/*  and faster to run.							*/
/*									*/
/************************************************************************/

static unsigned char * regAtomicExpression(	int *			flagp,
						ExpressionParsingJob *	epj )
{
    unsigned char *	ret;
    int			flags;

    *flagp = WORST;		/* Tentatively. */

    switch( *epj->epjSource )
	{
	case '^':
	    ret= regnode( BOL, epj );
	    epj->epjSource++;
	    break;

	case '$':
	    ret= regnode( EOL, epj );
	    epj->epjSource++;
	    break;

	case '.':
	    ret = regnode( ANY, epj );
	    *flagp |= HASWIDTH|SIMPLE;
	    epj->epjSource++;
	    break;

	case '[':
	    epj->epjSource++;
	    if  ( *epj->epjSource == '^') /* Complement of range. */
		{
		ret = regnode( ANYBUT, epj );
		epj->epjSource++;
		}
	    else{ ret = regnode( ANYOF, epj );	}

	    if  ( *epj->epjSource == ']'	||
		  *epj->epjSource == '-'	)
		{ regc( *epj->epjSource++, epj );	}

	    while( *epj->epjSource != '\0'	&&
		   *epj->epjSource != ']'	)
		{
		if  ( *epj->epjSource == '-' )
		    {
		    epj->epjSource++;

		    if  ( *epj->epjSource == ']'	||
			  *epj->epjSource == '\0'	)
			{ regc('-', epj );	}
		    else{
			int	class = UCHARAT(epj->epjSource-2)+1;
			int	classend = UCHARAT(epj->epjSource);

			if  ( class > classend+1 )
			    {
			    SDEB((char *)(epj->epjSource-2));
			    return (unsigned char *)0;
			    }

			for (; class <= classend; class++)
			    { regc(class, epj );	}

			epj->epjSource++;
			}
		    }
		else{ regc(*epj->epjSource++, epj );	}
		}

	    regc('\0', epj );
	    if  ( *epj->epjSource != ']' )
		{ SDEB((char *)epj->epjSource); return (unsigned char *)0; }

	    epj->epjSource++;
	    *flagp |= HASWIDTH|SIMPLE;
	    break;

	case '(':
	    epj->epjSource++;
	    ret= reg( 1, &flags, epj );
	    if (ret == NULL)
		    return(NULL);
	    *flagp |= flags&(HASWIDTH|SPSTART);
	    break;

	case '\0':
	case '|':
	case ')':
	    /* Supposed to be caught earlier. */
	    SDEB((char *)epj->epjSource);
	    return (unsigned char *)0;

	case '?':
	case '+':
	case '*':
	    SDEB((char *)epj->epjSource);
	    return (unsigned char *)0;

	case '\\':
	    if  ( epj->epjSource[1] == '\0' )
		{
		SDEB((char *)epj->epjSource);
		return (unsigned char *)0;
		}

	    epj->epjSource++;

	    /*FALLTHROUGH*/

	default:
	    ret= regnode( EXACTLY, epj );
	    regc( *epj->epjSource++, epj );

	    regFinishExactPattern( flagp, epj );
	    break;
	}

    return(ret);
}

/************************************************************************/
/*									*/
/*  regnode(): emit a node.						*/
/*									*/
/*  1)  If an insertion point is given, insert a node and a null 'next'	*/
/*	pointer. Otherwise just count the number of bytes that is	*/
/*	needed.								*/
/*  2)  Operand; Null "next" pointer.					*/
/*									*/
/************************************************************************/

static unsigned char * regnode( unsigned char		op,
				ExpressionParsingJob *	epj )
    {
    unsigned char *	retval= epj->epjTarget;

    if  ( ! epj->epjEmitCode )
	{ epj->epjSize += 3;	}
    else{
	*(epj->epjTarget++)= op;
	*(epj->epjTarget++)= '\0';
	*(epj->epjTarget++)= '\0';
	}

    return retval;
    }

/************************************************************************/
/*									*/
/*  regc(). Insert a singel byte in a compiled regular expression, or	*/
/*  if we are just counting.. Just count it.				*/
/*									*/
/************************************************************************/

static void regc(	unsigned char		b,
			ExpressionParsingJob *	epj )
    {
    if  ( ! epj->epjEmitCode )
	{ epj->epjSize++;		}
    else{ *(epj->epjTarget++)= b;	}

    return;
    }

/************************************************************************/
/*									*/
/*  regInsertNode() insert an operator in front of the already-emitted	*/
/*  operand. The routine is used to insert logic before a parsed atomic	*/
/*  regular expression to implement the repetitive operators +*?.	*/
/*									*/
/*  This means moving the operand right by three bytes.			*/
/*									*/
/*  1)  Insert node where the operand used to be.			*/
/*									*/
/************************************************************************/

static void regInsertNode(	unsigned char		op,
				unsigned char *		opnd,
				ExpressionParsingJob *	epj )
    {
    register unsigned char *	target= epj->epjTarget;
    register unsigned char *	src;
    register unsigned char *	dst;

    if  ( ! epj->epjEmitCode )
	{ epj->epjSize += 3;		}
    else{
	src= target;
	target += 3;
	dst= target;

	while( src > opnd )
	    { *(--dst)= *(--src);	}

	/*  1  */
	*(opnd++)= op;
	*(opnd++)= '\0';
	*(opnd++)= '\0';

	epj->epjTarget= target;
	}

    return;
    }

/*
 - regtail - set the next-pointer at the end of a node chain
 */
static void regtail(	unsigned char *		p,
			unsigned char *		val,
			ExpressionParsingJob *	epj )
{
    register unsigned char *	scan;
    register unsigned char *	temp;
    register int		offset;

    if  ( ! epj->epjEmitCode )
	{ return;	}

    /* Find last node. */
    scan= p;
    for (;;)
	{
	temp = regParseGetNextNode(scan,epj);
	if (temp == NULL)
		break;
	scan= temp;
	}

    if (OP(scan) == BACK)
	    offset = scan - val;
    else
	    offset = val - scan;
    *(scan+1) = (offset>>8)&0377;
    *(scan+2) = offset&0377;
}

/*
 - regoptail - regtail on operand of first argument; nop if operandless
 */
static void regoptail(	unsigned char *		p,
			unsigned char *		val,
			ExpressionParsingJob *	epj )
{
    /* "Operandless" and "op != BRANCH" are synonymous in practice. */
    if  ( p == NULL || ! epj->epjEmitCode || OP(p) != BRANCH )
	{ return;	}

    regtail( OPERAND(p), val, epj );
}

static unsigned char * regParseGetNextNode(	unsigned char *		p,
						ExpressionParsingJob *	epj )
{
    if  ( ! epj->epjEmitCode )
	{ return (unsigned char *)0;	}

    return regGetNextNode( p );
}

/*
 * regexec and friends
 */

/*
 * Global work variables for regexec().
 */
static unsigned char *regbol;		/* Beginning of input, for ^ check. */
static unsigned char **regstartp;	/* Pointer to rpStartp array. */
static unsigned char **regendp;		/* Ditto for rpEndp. */

#ifdef APP_DEBUG
int regnarrate = 0;
void regdump( regProg *r );
#endif

/************************************************************************/
/*									*/
/*  Find a mach of a 'regular expression' searching from left to right.	*/
/*									*/
/*  1)  Check for garbage input.					*/
/*  2)  Look for a "must appear" string.				*/
/*  3)  Remember the first position in the string.			*/
/*  4)  If the expression is of the form '^something', just try the	*/
/*	first position.							*/
/*									*/
/************************************************************************/

int regFindLeftToRight(	regProg *	prog,
			unsigned char *	string )
    {
    unsigned char *	s;

    /*  1  */
    if  ( UCHARAT(prog->rpProgram) != MAGIC )
	{ CCDEB(UCHARAT(prog->rpProgram),MAGIC); return 0;	}

    /*  2  */
    if  ( prog->rpRegmust )
	{
	s= (unsigned char *)strchr( (char *)string, prog->rpRegmust[0] );

	while( s )
	    {
	    if  ( ! strncmp( (char *)s,
				(char *)prog->rpRegmust, prog->regmlen ) )
		{ break;	}

	    s= (unsigned char *)strchr( (char *)s+ 1, prog->rpRegmust[0] );
	    }

	if  ( ! s )
	    { return 0;	}
	}

    /*  3  */
    regbol= string;

    /*  4  */
    if  ( prog->reganch )
	{ return( regtry( prog, string ) );	}

    /*  5  */
    if  ( prog->regstart )
	{
	s= (unsigned char *)strchr( (char *)string, prog->regstart );
	while( s )
	    {
	    if  ( regtry( prog, s ) )
		{ return 1;	}

	    s= (unsigned char *)strchr( (char *)s+ 1, prog->regstart );
	    }
	}
    else{
	s= string;
	do  {
	    if  ( regtry( prog, s ) )
		{ return 1;	}
	    } while( *(s++) );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find a mach of a 'regular expression' searching from right to left.	*/
/*									*/
/*  1)  Check for garbage input.					*/
/*  2)  Look for a "must appear" string.				*/
/*  3)  Remember the first position in the string.			*/
/*  4)  If the expression is of the form '^something', just try the	*/
/*	first position.							*/
/*									*/
/************************************************************************/

int regFindRightToLeft(	regProg *		prog,
			unsigned char *		string,
			int			len )
    {
    unsigned char *	s;

    /*  1  */
    if  ( UCHARAT(prog->rpProgram) != MAGIC )
	{ CCDEB(UCHARAT(prog->rpProgram),MAGIC); return 0;	}

    /*  2  */
    if  ( prog->rpRegmust )
	{
	s= (unsigned char *)strchr( (char *)string, prog->rpRegmust[0] );

	while( s )
	    {
	    if  ( ! strncmp( (char *)s,
				(char *)prog->rpRegmust, prog->regmlen ) )
		{ break;	}

	    s= (unsigned char *)strchr( (char *)s+ 1, prog->rpRegmust[0] );
	    }

	if  ( ! s )
	    { return 0;	}
	}

    /*  3  */
    regbol= string;

    /*  4  */
    if  ( prog->reganch )
	{ return( regtry( prog, string ) );	}

    /*  5  */
    if  ( prog->regstart )
	{
	s= string+ len- 1;
	do  {
	    if  ( *s == prog->regstart && regtry( prog, s ) )
		{ return 1;	}
	    s--;
	    } while( len-- > 0 );
	}
    else{
	s= string+ len- 1;
	do  {
	    if  ( regtry( prog, s ) )
		{ return 1;	}
	    s--;
	    } while( len-- > 0 );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  regtry:	try match at a specific point				*/
/*									*/
/************************************************************************/

static int regtry(	regProg *	prog,
			unsigned char *	string )
{
    int			i;
    unsigned char **	sp;
    unsigned char **	ep;

    unsigned char *	past;

    regstartp = prog->rpStartp;
    regendp = prog->rpEndp;

    sp = prog->rpStartp;
    ep = prog->rpEndp;

    for (i = NSUBEXP; i > 0; i--) {
	    *sp++ = NULL;
	    *ep++ = NULL;
    }

    if  ( regmatch( prog->rpProgram + 1, string, &past ) )
	{
	prog->rpStartp[0]= string;
	prog->rpEndp[0]= past;
	return 1;
	}
    else{ return 0;	}
}

/************************************************************************/
/*									*/
/*  regmatch: main matching routine					*/
/*									*/
/*  Conceptually the strategy is simple:  check to see whether the	*/
/*  current node matches, call self recursively to see whether the rest	*/
/*  matches and then act accordingly.  In practice we make some effort	*/
/*  to avoid recursion, in particular by going through "ordinary" nodes	*/
/*  (that do n't need to know whether the rest of the match failed) by	*/
/*  a loop instead of by recursion.					*/
/*									*/
/************************************************************************/

		/* 0 failure, 1 success */

static int regmatch(	unsigned char *		prog,
			unsigned char *		string,
			unsigned char **	pPast )
{
    register unsigned char *	scan;	/* Current node. */
    unsigned char *		next;	/* Next node. */

    scan = prog;

#   ifdef APP_DEBUG
    if (scan != NULL && regnarrate)
	    appDebug( "%s(\n", (char *)regprop(scan));
#   endif

    while (scan != NULL)
	{
#	ifdef APP_DEBUG
	if (regnarrate)
		appDebug( "%s...\n", regprop(scan));
#	endif

	next= regGetNextNode( scan );

	switch( OP(scan) )
	    {
	    case BOL:
		if  ( string != regbol )
		    { return 0;	}
		break;

	    case EOL:
		if  ( *string != '\0' )
		    { return 0;	}
		break;

	    case ANY:
		if  ( *string == '\0' )
		    { return 0;	}

		string++;
		break;

	    case EXACTLY:
		{
		    register int		len;
		    register unsigned char *	opnd;

		    opnd = OPERAND(scan);

		    /* Inline the first character, for speed. */
		    if  ( *opnd != *string )
			{ return 0;	}

		    len= strlen( (char *)opnd );
		    if  ( len > 1					&&
			  strncmp( (char *)opnd, (char *)string, len )	)
			{ return 0;	}

		    string += len;
		}
		break;

	    case ANYOF:
		if  ( *string == '\0' 					||
		      ! strchr( (char *)OPERAND(scan), *string )	)
		    { return 0;	}

		string++;
		break;

	    case ANYBUT:
		if  ( *string == '\0'				||
		      strchr( (char *)OPERAND(scan), *string )	)
		    { return 0;	}

		string++;
		break;

	    case NOTHING:
		break;
	    case BACK:
		break;

	    case OPEN+1: case OPEN+2: case OPEN+3: case OPEN+4: case OPEN+5:
	    case OPEN+6: case OPEN+7: case OPEN+8: case OPEN+9:
		{
		    register int		no;
		    register unsigned char *	save;

		    no= OP(scan) - OPEN;
		    save= string;

		    if  ( regmatch( next, string, &string ) )
			{
			/*
			 * Don't set rpStartp if some later
			 * invocation of the same parentheses
			 * already has.
			 */

			if  ( regstartp[no] == NULL )
			    { regstartp[no] = save;	}

			*pPast= string; return 1;
			}
		    else{ return 0;	}
		}
		break;

	    case CLOSE+1: case CLOSE+2: case CLOSE+3: case CLOSE+4:
	    case CLOSE+5: case CLOSE+6: case CLOSE+7: case CLOSE+8:
	    case CLOSE+9:
		{
		int		no;
		unsigned char *	save;

		no = OP(scan) - CLOSE;
		save = string;

		if  ( regmatch( next, string, &string ) )
		    {
		    /*
		     * Don't set rpEndp if some later
		     * invocation of the same parentheses
		     * already has.
		     */
		    if  ( regendp[no] == NULL )
			{ regendp[no]= save;	}

		    *pPast= string; return 1;
		    }
		else{ return 0;	}
		}
		break;

	    case BRANCH:
		{
		register unsigned char *	save;

		if  ( OP(next) != BRANCH )		/* No choice. */
		    { next = OPERAND(scan);	}	/* Avoid recursion. */
		else{
		    do  {
			save = string;
			if  ( regmatch( OPERAND(scan), string, &string ) )
			    { *pPast= string; return 1; }

			string= save;
			scan= regGetNextNode(scan);
			} while ( scan != NULL && OP(scan) == BRANCH );

		    return 0;
		    }
		}
		break;

	    case STAR:
	    case PLUS:
		{
		register unsigned char		nextch;
		register int			no;
		register unsigned char *	save;
		register int			min;

		/*
		 * Lookahead to avoid useless match attempts
		 * when we know what character comes next.
		 */
		nextch= '\0';
		if  ( OP(next) == EXACTLY )
		    { nextch = *OPERAND(next);	}

		min = (OP(scan) == STAR) ? 0 : 1;
		save = string;
		no= regrepeat( OPERAND(scan), string, &string );

		while( no >= min )
		    {
		    /* If it could work, try it. */
		    if  ( nextch == '\0' || *string == nextch )
			{
			if  ( regmatch( next, string, &string ) )
			    { *pPast= string; return 1;	}
			}

		    /* Couldn't or didn't -- back up. */
		    no--;
		    string= save + no;
		    }

		return 0;
		}

	case END:
		*pPast= string; return 1;	/* Success! */
		break;
	default:
		SDEB("memory corruption");
		return 0;
		break;
	}

	scan = next;
	}

    /*
     * We get here only if there's trouble -- normally "case END" is
     * the terminating point.
     */
    SDEB("corrupted pointers");
    return 0;
}

/************************************************************************/
/*									*/
/*  regrepeat: repeatedly match something simple, report how many times	*/
/*									*/
/************************************************************************/

static int regrepeat(	unsigned char *		p,
			unsigned char *		scan,
			unsigned char **	pPast )
    {
    int			count= 0;
    unsigned char *	opnd;

    opnd= OPERAND(p);

    switch (OP(p))
	{
	case ANY:
	    count = strlen( (char *)scan );
	    scan += count;
	    break;

	case EXACTLY:
	    while( *opnd == *scan )
		{ count++; scan++;	}
	    break;

	case ANYOF:
	    while( *scan && strchr( (char *)opnd, *scan ) )
		{ count++; scan++; }
	    break;

	case ANYBUT:
	    while( *scan && ! strchr( (char *)opnd, *scan ) )
		{ count++; scan++; }
	    break;

	default:		/* Oh dear.  Called inappropriately. */
	    SDEB("internal foulup");
	    count = 0;	/* Best compromise. */
	    break;
	}


    *pPast= scan; return count;
    }

/*
 - regGetNextNode - dig the "next" pointer out of a node
 */
static unsigned char * regGetNextNode(	register unsigned char *p )
{
	register int offset;

	offset = NEXT(p);
	if (offset == 0)
		return(NULL);

	if (OP(p) == BACK)
		return(p-offset);
	else
		return(p+offset);
}

#ifdef APP_DEBUG

/*
 - regdump - dump a regexp onto stdout in vaguely comprehensible form
 */
void regdump( regProg *r )
{
	register unsigned char *	s;
	register char op = EXACTLY;	/* Arbitrary non-END op. */
	register unsigned char *	next;


	s = r->rpProgram + 1;
	while (op != END) {	/* While that wasn't END last time... */
		op = OP(s);
		appDebug("%2ld%s", (long)(s-r->rpProgram), regprop(s));
		next = regGetNextNode(s);
		if (next == NULL)		/* Next ptr. */
			appDebug("(0)");
		else 
			appDebug("(%ld)", (long)((s-r->rpProgram)+(next-s)));
		s += 3;
		if (op == ANYOF || op == ANYBUT || op == EXACTLY) {
			/* Literal string, where present. */
			while (*s != '\0') {
				putchar(*s);
				s++;
			}
			s++;
		}
		putchar('\n');
	}

	/* Header fields of interest. */
	if (r->regstart != '\0')
		appDebug("start `%c' ", r->regstart);
	if (r->reganch)
		appDebug("anchored ");
	if (r->rpRegmust != NULL)
		appDebug("must have \"%s\"", r->rpRegmust);
	appDebug("\n");
}

/*
 - regprop - printable representation of opcode
 */
static char * regprop( unsigned char *op )
{
	char *		p= (char *)0;
	static char	buf[50];

	(void) strcpy(buf, ":");

	switch (OP(op)) {
	case BOL:
		p = "BOL";
		break;
	case EOL:
		p = "EOL";
		break;
	case ANY:
		p = "ANY";
		break;
	case ANYOF:
		p = "ANYOF";
		break;
	case ANYBUT:
		p = "ANYBUT";
		break;
	case BRANCH:
		p = "BRANCH";
		break;
	case EXACTLY:
		p = "EXACTLY";
		break;
	case NOTHING:
		p = "NOTHING";
		break;
	case BACK:
		p = "BACK";
		break;
	case END:
		p = "END";
		break;
	case OPEN+1:
	case OPEN+2:
	case OPEN+3:
	case OPEN+4:
	case OPEN+5:
	case OPEN+6:
	case OPEN+7:
	case OPEN+8:
	case OPEN+9:
		sprintf(buf+strlen(buf), "OPEN%d", OP(op)-OPEN);
		p = NULL;
		break;
	case CLOSE+1:
	case CLOSE+2:
	case CLOSE+3:
	case CLOSE+4:
	case CLOSE+5:
	case CLOSE+6:
	case CLOSE+7:
	case CLOSE+8:
	case CLOSE+9:
		sprintf(buf+strlen(buf), "CLOSE%d", OP(op)-CLOSE);
		p = NULL;
		break;
	case STAR:
		p = "STAR";
		break;
	case PLUS:
		p = "PLUS";
		break;
	default:
		SDEB("corrupted opcode");
		break;
	}
	if (p != NULL)
		(void) strcat(buf, p);
	return(buf);
}
#endif
