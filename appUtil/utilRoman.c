/************************************************************************/
/*									*/
/*  Unit types and conversions between units.				*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<string.h>

#   include	"utilRoman.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Convert a number to roman notation.					*/
/*									*/
/************************************************************************/

typedef struct RomanPiece
    {
    int			rpValue;
    const char *	rpString;
    int			rpStrlen;
    } RomanPiece;

static const RomanPiece	APP_RomanPieces[]=
    {
	{	1000,	"m",	1	},
	{	900,	"cm",	2	},
	{	500,	"d",	1	},
	{	400,	"cd",	2	},
	{	100,	"c",	1	},
	{	 90,	"xc",	2	},
	{	 50,	"l",	1	},
	{	 40,	"xl",	2	},
	{	 10,	"x",	1	},
	{	 9,	"ix",	2	},
	{	 5,	"v",	1	},
	{	 4,	"iv",	2	},
	{	 1,	"i",	1	}
    };

int utilRomanString(	char *		to,
			int		maxlen,
			int		n,
			int		upper )
    {
    int			i;
    const RomanPiece *	rp;

    if  ( n <= 0 || n > 3500 )
	{ LDEB(n); return -1;	}

    rp= APP_RomanPieces;
    for ( i= 0; i < sizeof(APP_RomanPieces)/sizeof(RomanPiece); rp++, i++ )
	{
	while( n >= rp->rpValue )
	    {
	    if  ( rp->rpStrlen > maxlen )
		{ LLDEB(rp->rpStrlen,maxlen); return -1;	}

	    strcpy( to, rp->rpString );
	    if  ( upper )
		{
		while( *to )
		    { *to= toupper( *to ); to++;	}
		}
	    else{ to += rp->rpStrlen;	}

	    maxlen -= rp->rpStrlen; n -= rp->rpValue;
	    }
	}

    return 0;
    }

