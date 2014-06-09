/************************************************************************/
/*									*/
/*  Geometry Specification strings.					*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	"appUnit.h"
#   include	"appGeoString.h"

/************************************************************************/
/*									*/
/*  Represent a Length as a string.					*/
/*									*/
/************************************************************************/

static void appGeoLengthNumberToString(	char *		target,
					int		twips,
					int		unitInt )
    {
    double		units= appUnitFromTwips( twips, unitInt );

    if  ( (int)units == units || units > 100 || units < -100 )
	{
	if  ( units < 0 )
	    { sprintf( target, "%d", (int)( units- 0.4995 ) ); }
	else{ sprintf( target, "%d", (int)( units+ 0.4995 ) ); }
	}
    else{ sprintf( target, "%3.3g", units );			}

    return;
    }

void appGeoLengthToString(	char *		target,
				int		twips,
				int		unitInt )
    {
    const char *	unitString= appUnitTypeString( unitInt );

    char		numberString[40];
    char *		s;
    int			l;

    appGeoLengthNumberToString( numberString, twips, unitInt );

    s= numberString;
    while( *s == ' ' )
	{ s++;	}
    l= strlen( s );
    while( l > 0 && s[l-1] == ' ' )
	{ l--; s[l]= '\0';	}

    sprintf( target, "%s%s", s, unitString );

    return;
    }

/************************************************************************/
/*									*/
/*  Represent a Rectangle as a string.					*/
/*									*/
/************************************************************************/

void appGeoRectangleToString(	char *		target,
				int		widthTwips,
				int		heightTwips,
				int		unitInt )
    {
    const char *	unitString= appUnitTypeString( unitInt );

    char		widthString[40];
    char		heightString[40];
    char *		ws;
    char *		hs;
    int			l;

    appGeoLengthNumberToString( widthString, widthTwips, unitInt );
    ws= widthString;
    while( *ws == ' ' )
	{ ws++;	}
    l= strlen( ws );
    while( l > 0 && ws[l-1] == ' ' )
	{ l--; ws[l]= '\0';	}

    appGeoLengthNumberToString( heightString, heightTwips, unitInt );
    hs= heightString;
    while( *hs == ' ' )
	{ hs++;	}
    l= strlen( hs );
    while( l > 0 && hs[l-1] == ' ' )
	{ l--; hs[l]= '\0';	}

    sprintf( target, "%s x %s%s", ws, hs, unitString );

    return;
    }

/************************************************************************/
/*									*/
/*  Extract a Rectangle from a string.					*/
/*									*/
/************************************************************************/

int appGeoRectangleFromString(		const char *	s,
					int		defaultUnitInt,
					int *		pWidth,
					int *		pHeight )
    {
    double	width;
    double	height;
    char	scratch[100];
    int		got;

    int		unitInt;

    int		w_ival;
    int		w_nom;
    int		w_div;

    int		h_ival;
    int		h_nom;
    int		h_div;

    got= sscanf( s, "%lg x %lg%s", &width, &height, scratch );

    if  ( got == 2 )
	{
	*pWidth= (int)appUnitToTwips( width, defaultUnitInt );
	*pHeight= (int)appUnitToTwips( height, defaultUnitInt );
	return 0;
	}

    if  ( got == 3 )
	{
	unitInt= appUnitTypeInt( scratch );

	if  ( unitInt >= 0 )
	    {
	    *pWidth= (int)appUnitToTwips( width, unitInt );
	    *pHeight= (int)appUnitToTwips( height, unitInt );
	    return 0;
	    }
	}

    got= sscanf( s, "%d %d/%d x %d %d/%d%s",
				&w_ival, &w_nom, &w_div,
				&h_ival, &h_nom, &h_div, scratch );
    if  ( got == 6 )
	{
	width= w_ival+ (double)w_nom/(double)w_div;
	height= h_ival+ (double)h_nom/(double)h_div;

	*pWidth= (int)appUnitToTwips( width, defaultUnitInt );
	*pHeight= (int)appUnitToTwips( height, defaultUnitInt );
	return 0;
	}

    if  ( got == 7 )
	{
	width= w_ival+ (double)w_nom/(double)w_div;
	height= h_ival+ (double)h_nom/(double)h_div;
	unitInt= appUnitTypeInt( scratch );

	if  ( unitInt >= 0 )
	    {
	    *pWidth= (int)appUnitToTwips( width, unitInt );
	    *pHeight= (int)appUnitToTwips( height, unitInt );
	    return 0;
	    }
	}

    got= sscanf( s, "%lg x %d %d/%d%s",
				&width, &h_ival, &h_nom, &h_div, scratch );
    if  ( got == 4 )
	{
	height= h_ival+ (double)h_nom/(double)h_div;

	*pWidth= (int)appUnitToTwips( width, defaultUnitInt );
	*pHeight= (int)appUnitToTwips( height, defaultUnitInt );
	return 0;
	}

    if  ( got == 5 )
	{
	height= h_ival+ (double)h_nom/(double)h_div;
	unitInt= appUnitTypeInt( scratch );

	if  ( unitInt >= 0 )
	    {
	    *pWidth= (int)appUnitToTwips( width, unitInt );
	    *pHeight= (int)appUnitToTwips( height, unitInt );
	    return 0;
	    }
	}

    got= sscanf( s, "%d %d/%d x %lg%s",
				&w_ival, &w_nom, &w_div, &height, scratch );
    if  ( got == 4 )
	{
	width= w_ival+ (double)w_nom/(double)w_div;

	*pWidth= (int)appUnitToTwips( width, defaultUnitInt );
	*pHeight= (int)appUnitToTwips( height, defaultUnitInt );
	return 0;
	}

    if  ( got == 5 )
	{
	width= w_ival+ (double)w_nom/(double)w_div;
	unitInt= appUnitTypeInt( scratch );

	if  ( unitInt >= 0 )
	    {
	    *pWidth= (int)appUnitToTwips( width, unitInt );
	    *pHeight= (int)appUnitToTwips( height, unitInt );
	    return 0;
	    }
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Extract a Length from a string.					*/
/*									*/
/************************************************************************/

int appGeoLengthFromString(	const char *	s,
				int		defaultUnitInt,
				int *		pValue )
    {
    double	value;
    char	scratch[100];
    int		got;
    int		unitInt;

    int		ival;
    int		nomi;
    int		divi;

    got= sscanf( s, "%lg%s", &value, scratch );

    if  ( got == 1 )
	{
	*pValue= (int)appUnitToTwips( value, defaultUnitInt );
	return 0;
	}

    if  ( got == 2 )
	{
	unitInt= appUnitTypeInt( scratch );

	if  ( unitInt >= 0 )
	    {
	    *pValue= (int)appUnitToTwips( value, unitInt );
	    return 0;
	    }
	}

    got= sscanf( s, "%d %d/%d%s", &ival, &nomi, &divi, scratch );

    if  ( got == 3 )
	{
	value= ival+ (double)nomi/(double)divi;
	*pValue= (int)appUnitToTwips( value, defaultUnitInt );
	return 0;
	}


    if  ( got == 4 )
	{
	unitInt= appUnitTypeInt( scratch );

	if  ( unitInt >= 0 )
	    {
	    value= ival+ (double)nomi/(double)divi;
	    *pValue= (int)appUnitToTwips( value, unitInt );
	    return 0;
	    }
	}

    got= sscanf( s, "%d/%d%s", &nomi, &divi, scratch );

    if  ( got == 2 )
	{
	value= (double)nomi/(double)divi;
	*pValue= (int)appUnitToTwips( value, defaultUnitInt );
	return 0;
	}

    if  ( got == 3 )
	{
	unitInt= appUnitTypeInt( scratch );

	if  ( unitInt >= 0 )
	    {
	    value= (double)nomi/(double)divi;
	    *pValue= (int)appUnitToTwips( value, unitInt );
	    return 0;
	    }
	}

    return -1;
    }
