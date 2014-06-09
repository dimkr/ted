/************************************************************************/
/*									*/
/*  Unit types and conversions between units.				*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<string.h>

#   include	"appUnit.h"
#   include	<appDebugon.h>

int appUnitTypeInt(	const char *	unitTypeString )
    {
    if  ( ! strcmp( unitTypeString, "cm" ) )
	{ return UNITtyCM;	}

    if  ( ! strcmp( unitTypeString, "inch" )	||
	  ! strcmp( unitTypeString, "\"" )	)
	{ return UNITtyINCH;	}

    if  ( ! strcmp( unitTypeString, "points" )	||
	  ! strcmp( unitTypeString, "pt" )	)
	{ return UNITtyPOINTS;	}

    if  ( ! strcmp( unitTypeString, "picas" )	||
	  ! strcmp( unitTypeString, "pi" )	)
	{ return UNITtyPICAS;	}

    if  ( ! strcmp( unitTypeString, "mm" ) )
	{ return UNITtyMM;	}

    return -1;
    }

const char * appUnitTypeString(	int	unitTypeInt )
    {
    switch( unitTypeInt )
	{
	case UNITtyCM:		return "cm";
	case UNITtyINCH:	return "\"";
	case UNITtyPOINTS:	return "pt";
	case UNITtyPICAS:	return "pi";
	case UNITtyMM:		return "mm";
	default:
	    LDEB(unitTypeInt); return "?";
	}
    }

double appUnitFromTwips(	int	twips,
				int	unitTypeInt )
    {
    switch( unitTypeInt )
	{
	case UNITtyCM:		return ( 2.54* twips )/ ( 20.0* 72 );
	case UNITtyINCH:	return twips/ ( 20.0* 72 );
	case UNITtyPOINTS:	return twips/ 20.0;
	case UNITtyPICAS:	return twips/ ( 20.0* 12 );
	case UNITtyMM:		return ( 25.4* twips )/ ( 20.0* 72 );
	default:
	    LDEB(unitTypeInt); return -1;
	}
    }

double appUnitToTwips(	double	units,
			int	unitTypeInt )
    {
    switch( unitTypeInt )
	{
	case UNITtyCM:		return ( 20.0* 72* units )/ 2.54;
	case UNITtyINCH:	return 20.0* 72* units;
	case UNITtyPOINTS:	return 20.0* units;
	case UNITtyPICAS:	return 20.0* 12* units;
	case UNITtyMM:		return ( 20.0* 72* units )/ 25.4;
	default:
	    LDEB(unitTypeInt); return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Format a date according to an 'MS-Word' datetime picture		*/
/*									*/
/*  "sequence" (sequence value) values are not supported here.. They	*/
/*  are supposed to be inserted and properly escaped beforehand or to	*/
/*  be replaced later on.						*/
/*									*/
/*  It is the responsibility of the caller to cope with the		*/
/*  interference between the output of the strftime() substitutions and	*/
/*  the 'sequence' substitutins in the order in which they are applied.	*/
/*									*/
/************************************************************************/

int appWordFormatDate(			char *			to,
					int			maxlen,
					const struct tm *	tm,
					const char *		wordPicture )
    {
    int			l;
    const char *	from= wordPicture;
    const char *	frm;

    while( *from )
	{
	switch( *from )
	    {
	    int		count;

	    case 'M':
		count= 1;
		from++;
		while( count < 4 && from[0] == from[-1]	)
		    { from++; count++;	}
		switch( count )
		    {
		    case 1:
			l= strftime( to, maxlen+ 1, "%m", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			if  ( to[0] == '0' )
			    { memmove( to, to+ 1, l+ 1 ); }
			to += l; maxlen -= l;
			continue;

		    case 2:
			l= strftime( to, maxlen+ 1, "%m", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			to += l; maxlen -= l;
			continue;

		    case 3:
			l= strftime( to, maxlen+ 1, "%b", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			to += l; maxlen -= l;
			continue;

		    case 4:
			l= strftime( to, maxlen+ 1, "%B", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			to += l; maxlen -= l;
			continue;

		    default:
			LDEB(count); return -1;
		    }
		continue;

	    case 'd': case 'D':
		count= 1;
		from++;
		while( count < 4					&&
		       tolower( from[0] ) == tolower( from[-1] )	)
		    { from++; count++;	}
		switch( count )
		    {
		    case 1:
			l= strftime( to, maxlen+ 1, "%d", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			if  ( to[0] == '0' )
			    { memmove( to, to+ 1, l+ 1 ); }
			to += l; maxlen -= l;
			continue;

		    case 2:
			l= strftime( to, maxlen+ 1, "%d", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			to += l; maxlen -= l;
			continue;

		    case 3:
			l= strftime( to, maxlen+ 1, "%a", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			to += l; maxlen -= l;
			continue;

		    case 4:
			l= strftime( to, maxlen+ 1, "%A", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			to += l; maxlen -= l;
			continue;

		    default:
			LDEB(count); return -1;
		    }
		continue;

	    case 'y': case 'Y':
		count= 1;
		from++;
		while( count < 4					&&
		       tolower( from[0] ) == tolower( from[-1] )	)
		    { from++; count++;	}
		switch( count )
		    {
		    case 2:
			/* GCC: Shut Up! */
			frm= "%y";
			l= strftime( to, maxlen+ 1, frm, tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			to += l; maxlen -= l;
			continue;

		    case 4:
			l= strftime( to, maxlen+ 1, "%Y", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			to += l; maxlen -= l;
			continue;

		    default:
			LDEB(count); return -1;
		    }
		continue;

	    case 'H':
		count= 1;
		from++;
		while( count < 2 && from[0] == from[-1]	)
		    { from++; count++;	}
		switch( count )
		    {
		    case 1:
			l= strftime( to, maxlen+ 1, "%H", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			if  ( to[0] == '0' )
			    { memmove( to, to+ 1, l+ 1 ); }
			to += l; maxlen -= l;
			continue;

		    case 2:
			l= strftime( to, maxlen+ 1, "%H", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			to += l; maxlen -= l;
			continue;

		    default:
			LDEB(count); return -1;
		    }
		continue;

	    case 'h':
		count= 1;
		from++;
		while( count < 2 && from[0] == from[-1]	)
		    { from++; count++;	}
		switch( count )
		    {
		    case 1:
			l= strftime( to, maxlen+ 1, "%I", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			if  ( to[0] == '0' )
			    { memmove( to, to+ 1, l+ 1 ); }
			to += l; maxlen -= l;
			continue;

		    case 2:
			l= strftime( to, maxlen+ 1, "%I", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			to += l; maxlen -= l;
			continue;

		    default:
			LDEB(count); return -1;
		    }
		continue;

	    case 'm':
		count= 1;
		from++;
		while( count < 2 && from[0] == from[-1]	)
		    { from++; count++;	}
		switch( count )
		    {
		    case 1:
			l= strftime( to, maxlen+ 1, "%M", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			if  ( to[0] == '0' )
			    { memmove( to, to+ 1, l+ 1 ); }
			to += l; maxlen -= l;
			continue;

		    case 2:
			l= strftime( to, maxlen+ 1, "%M", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			to += l; maxlen -= l;
			continue;

		    default:
			LDEB(count); return -1;
		    }
		continue;

	    case 'A': case 'a':
		if  ( ! strcmp( from, "AM/PM" )	||
		      ! strcmp( from, "am/pm" ) )
		    {
		    l= strftime( to, maxlen+ 1, "%p", tm );
		    if  ( l < 1 )
			{ LDEB(l); return -1;	}
		    to += l; maxlen -= l; from += 5;
		    continue;
		    }
		
		if  ( ! strcmp( from, "A/P" )	||
		      ! strcmp( from, "a/p" ) )
		    {
		    l= strftime( to, maxlen+ 1, "%p", tm );
		    if  ( l < 1 )
			{ LDEB(l); return -1;	}
		    to += l; maxlen -= l; from += 3;
		    continue;
		    }
		
		goto defaultCase;

	    case '\'':
		from++;
		while( *from != '\'' )
		    {
		    if  ( ! *from )
			{ LDEB(1); return -1;	}

		    if  ( maxlen < 1 )
			{ LDEB(maxlen); return -1;	}
		    *(to++)= *(from++); maxlen--;
		    }

		from++; /* '\'' */
		continue;

	    case '`':
		if  ( maxlen < 1 )
		    { LDEB(maxlen); return -1;	}
		*(to++)= *(from++);
		while( *from != '\'' )
		    {
		    if  ( ! *from )
			{ LDEB(1); return -1;	}

		    if  ( maxlen < 1 )
			{ LDEB(maxlen); return -1;	}
		    *(to++)= *(from++); maxlen--;
		    }

		if  ( maxlen < 1 )
		    { LDEB(maxlen); return -1;	}
		*(to++)= *(from++);
		continue;

	    default: defaultCase:
		if  ( maxlen < 1 )
		    { LDEB(maxlen); return -1;	}
		*(to++)= *(from++); maxlen--;
		continue;
	    }
	}

    *to= '\0';

    return 0;
    }

/************************************************************************/
/*									*/
/*  Invalidate/Initialize a 'struct tm'.				*/
/*									*/
/************************************************************************/

void appInvalidateTime(		struct tm *	tm )
    {
    const time_t	t= 0;

    *tm= *localtime( &t );

    tm->tm_mday= 0;

    return;
    }

