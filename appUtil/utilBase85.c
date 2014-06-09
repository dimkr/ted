/************************************************************************/
/*									*/
/*  Utilities to translate back and forth between binary and base85	*/
/*									*/
/************************************************************************/

#   include	"utilBase85.h"

const unsigned char	UTIL_Base85Digits[]=
			    "!\"#$%&'()*+,-./0123456789:;<=>?@"
			    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			    "[\\]^_`"
			    "abcdefghijklmnopqrstu";

const int *	utilBase85GetIndexArray( void )
    {
    static int UTIL_Base85IndexArray[256];

    if  ( UTIL_Base85IndexArray[0] == 0 )
	{
	int	i;

	for ( i= 0; i < 256; i++ )
	    { UTIL_Base85IndexArray[i]= 0xff;	}

	i= 0;
	while( UTIL_Base85Digits[i] )
	    { UTIL_Base85IndexArray[UTIL_Base85Digits[i]]= i; i++;	}
	}

    return UTIL_Base85IndexArray;
    }

int utilBase85Make(	int *			d,
			const unsigned char *	b,
			int			last )
    {
    int			i;
    unsigned long	v;

    v= 0;
    for ( i= 0; i < 4; i++ )
	{ v= 256* v+ b[i];	}

    if  ( v == 0 && ! last )
	{ d[0]= 'z'; return 1;	}

    for ( i= 0; i < 5; i++ )
	{ d[4-i]= UTIL_Base85Digits[v%85]; v /= 85; }

    return 5;
    }

