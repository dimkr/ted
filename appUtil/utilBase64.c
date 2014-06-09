/************************************************************************/
/*									*/
/*  Utilities to translate back and forth between binary and base64	*/
/*									*/
/************************************************************************/

#   include	"utilBase64.h"

const unsigned char	UTIL_Base64Digits[]=
				       "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				       "abcdefghijklmnopqrstuvwxyz"
				       "0123456789+/";

const int *	utilBase64GetIndexArray( void )
    {
    static int UTIL_Base64IndexArray[256];

    if  ( UTIL_Base64IndexArray[0] == 0 )
	{
	int	i;

	for ( i= 0; i < 256; i++ )
	    { UTIL_Base64IndexArray[i]= 0xff;	}

	i= 0;
	while( UTIL_Base64Digits[i] )
	    { UTIL_Base64IndexArray[UTIL_Base64Digits[i]]= i; i++;	}
	}

    return UTIL_Base64IndexArray;
    }
