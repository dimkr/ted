#   include	"appUtilConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	"utilX11FontNames.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Split an X11 fonts dir line.					*/
/*									*/
/*  Return 0 on success.						*/
/*  Return 1 on a line that can be skipped: The first line with the	*/
/*	count.								*/
/*  Return -1 on illegal input.						*/
/*									*/
/************************************************************************/

int utilX11SplitFontsDirLine(		char *		scratch,
					int *		pX11Start,
					int *		pX11Length,
					int		line )
    {
    int		digitsOnly= 1;
    char *	s= scratch;
    int		past;
    int		j;

    while( *s && ! isspace( *s ) )
	{
	if  ( ! isdigit( *s ) )
	    { digitsOnly= 0;	}
	s++;
	}

    if  ( ! *s )
	{
	if  ( line != 0 || ! digitsOnly )
	    { LSDEB(line,scratch); return -1;	}

	return 1;
	}

    *(s++)= '\0';
    while( isspace( *s ) )
	{ s++;	}
    if  ( ! *s )
	{
	if  ( line != 0 || ! digitsOnly )
	    { LSDEB(line,scratch); return -1;	}
	
	return 1;
	}

    past= -1;

    j= 0;
    while( s[j] )
	{
	if  ( s[j] == ' ' || s[j] == '\t' || s[j] == '\n' )
	    { past= j;	}
	else{ past= -1;	}

	j++;
	}

    if  ( past >= 0 )
	{ s[past]= '\0';	}
    else{ past= j;		}

    *pX11Start= s- scratch;
    *pX11Length= past;
    return 0;
    }

