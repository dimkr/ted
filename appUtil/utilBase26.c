/************************************************************************/
/*									*/
/*  Unit types and conversions between units.				*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<string.h>

#   include	"utilBase26.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Convert a number to base 26. Like in Excel column headers.		*/
/*  1->A; 26->Z; 27->AA; 28->AB &c.					*/
/*									*/
/*  1)  Produce letters in the opposite order.				*/
/*  2)  Terminate.							*/
/*  3)  Swap.								*/
/*									*/
/************************************************************************/

static const char UTIL_Base26LowerDigits[]= "abcdefghijklnmopqrstuvwxyz";
static const char UTIL_Base26UpperDigits[]= "ABCDEFGHIJKLNMOPQRSTUVWXYZ";

int utilBase26String(	char *		target,
			int		maxlen,
			int		n,
			int		upper )
    {
    const char *	d;
    int			done= 0;
    int			i;

    if  ( n <= 0 )
	{ LDEB(n); return -1;	}

    if  ( upper )
	{ d= UTIL_Base26UpperDigits;	}
    else{ d= UTIL_Base26LowerDigits;	}

    n--;

    /*  1  */
    while( n >= 0 )
	{
	if  ( maxlen == 0 )
	    { LLDEB(n,maxlen); return -1;	}

	target[done++]= d[n % 26];
	maxlen--;

	n /= 26; n--;
	}

    /*  2  */
    target[done]= '\0';

    /*  3  */
    for ( i= 0; i < done/ 2; i++ )
	{
	char	s;

	s= target[i]; target[i]= target[done-i-1]; target[done-i-1]= s;
	}

    return 0;
    }

int utilBase26Int(	const char *	from )
    {
    int			rval= 0;
    const char *	dig;

    while( isspace( *from ) )
	{ from++;	}

    if  ( ! *from )
	{ SDEB(from); return -1;	}

    while( *from )
	{
	if  ( isupper( *from ) )
	    {
	    dig= strchr( UTIL_Base26UpperDigits, *from );
	    if  ( ! dig )
		{ SDEB(from); return -1;	}

	    rval= 26* rval+ ( dig- UTIL_Base26UpperDigits );
	    }
	else{
	    dig= strchr( UTIL_Base26LowerDigits, *from );
	    if  ( ! dig )
		{ SDEB(from); return -1;	}

	    rval= 26* rval+ ( dig- UTIL_Base26LowerDigits );
	    }

	rval++;
	from++;
	}

    return rval;
    }

