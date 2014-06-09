/************************************************************************/
/*									*/
/*  Date manipulation.							*/
/*									*/
/*  These routines are totally naive with respect to the difference	*/
/*  between Julian and Gregorian dates. Everything is Gregorian.	*/
/*									*/
/*  Dates are relative to the hypothetical date 'Mar 1 0000', then	*/
/*  shifted to be relative to Jan 1, 1970.				*/
/*									*/
/************************************************************************/

#   include	<utilDate.h>

#   include	<appDebugon.h>

static int UTIL_DateMdays[]=
    {
    0,
    31,		/*  31, Mar	*/
    61,		/*  30, Apr	*/
    92,		/*  31, May	*/
    122,	/*  30, Jun	*/
    153,	/*  31, Jul	*/
    184,	/*  31, Aug	*/
    214,	/*  30, Sep	*/
    245,	/*  31, Oct	*/
    275,	/*  30, Nov	*/
    306,	/*  31, Dec	*/
    337,	/*  31, Jan	*/
    366,	/*  29, Feb	*/
    };

#   define	Y1		( 365 )
#   define	Y4		( 4* Y1+ 1 )
#   define	Y100		( 24* Y4+ 4* Y1 )
#   define	Y400		( 4* Y100+ 1 )
#   define	JAN_1_1970	( 719468 )

long utilDateGregorianDayNumber(	int	year,
					int	month,
					int	day )
    {
    long	rval= 0;

    month -= 3;

    while( month < 0 )
	{ month += 12; year--;	}
    while( month >= 12 )
	{ month -= 12; year++;	}

    rval += Y400* (year/400); year= year % 400;
    rval += Y100* (year/100); year= year % 100;
    rval += Y4  * (year/4  ); year= year % 4  ;
    rval += Y1  *  year;

    rval += UTIL_DateMdays[month];

    rval += day- 1;

    return rval- JAN_1_1970;
    }

void utilDateSplitGregorianDate(	long	day,
					int *	pYear,
					int *	pMonth,
					int *	pDay )
    {
    int		year= 0;
    int		month= 0;
    int		s;

    if  ( day < -JAN_1_1970 )
	{ LLDEB(day,JAN_1_1970);	}

    day += JAN_1_1970;

    while( day >= Y400 )		{ year += 400;	day -= Y400;	}

    s= 0;
    while( day >= Y100 && s < 3 )	{ year += 100;	day -= Y100; s++; }
    while( day >= Y4 )			{ year += 4;	day -= Y4;	}

    s= 0;
    while( day >= Y1 && s < 3 )		{ year += 1;	day -= Y1; s++;	}

    day++;
    while( UTIL_DateMdays[month+1] < day	&&
	   month < 12				)
	{ month++;	}
    day -= UTIL_DateMdays[month];

    month += 3;
    if  ( month > 12 )
	{ month -= 12; year++;	}

    *pYear= year;
    *pMonth= month;
    *pDay= day;

    return;
    }

# if 0

# include <time.h>

extern int qqq(void);

int qqq(void)
    {
    long	value;

    int		year;
    int		month;
    int		day;

    LDEB(utilDateGregorianDayNumber(1970,1,1));

    for ( value= -JAN_1_1970+ 1; value < 3670000; value++ )
	{
	long	value2;

	utilDateSplitGregorianDate( value, &year, &month, &day );

	value2= utilDateGregorianDayNumber( year, month, day );

	if  ( value2 != value )
	    {
	    LLLDEB(year,month,day);
	    LLDEB(value2,value);
	    return 1;
	    }
	}

    utilDateSplitGregorianDate( value, &year, &month, &day );
    LLLDEB(year,month,day);

    for ( value= -23000; value < 23000; value++ )
	{
	time_t		t= 24* 60* 60* value;
	struct tm *	tm= gmtime( &t );

	utilDateSplitGregorianDate( value, &year, &month, &day );

	if  ( tm->tm_year+ 1900 != year )
	    {
	    LLDEB(tm->tm_year,year);
	    LLLDEB(year,month,day);
	    LLDEB(t,value);
	    return 1;
	    }

	if  ( tm->tm_mon+ 1 != month )
	    {
	    LLDEB(tm->tm_mon,month);
	    LLLDEB(year,month,day);
	    LLDEB(t,value);
	    return 1;
	    }

	if  ( tm->tm_mday != day )
	    {
	    LLDEB(tm->tm_mday,day);
	    LLLDEB(year,month,day);
	    LLDEB(t,value);
	    return 1;
	    }
	}

    return 0;
    }

# endif
