#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	"appFrame.h"
#   include	"appSystem.h"
#   include	<appGeoString.h>

/************************************************************************/
/*									*/
/*  Get a 'Length', or an 'integer' from a text widget.			*/
/*									*/
/************************************************************************/

int appGetLengthFromTextWidget(		APP_WIDGET	w,
					int *		pValue,
					int *		pChanged,
					int		defaultUnit,
					int		minValue,
					int		adaptToMin,
					int		maxValue,
					int		adaptToMax )
    {
    int		value;

    char	scratch[50];
    char *	s;

    s= appGetStringFromTextWidget( w );
    if  ( appGeoLengthFromString( s, defaultUnit, &value )	||
	  ( ! adaptToMin && value < minValue )			||
	  ( ! adaptToMax && value > maxValue )			)
	{
	appFreeStringFromTextWidget( s );

	appRefuseTextValue( w ); return -1;
	}

    appGeoLengthToString( scratch, *pValue, defaultUnit );
    if  ( ! strcmp( scratch, s ) )
	{ value= *pValue;	}

    appFreeStringFromTextWidget( s );

    if  ( adaptToMin && value < minValue )
	{ value=  minValue;	}
    if  ( adaptToMax && value > maxValue )
	{ value=  maxValue;	}

    *pChanged= ( value != *pValue );
    *pValue= value;

    return 0;
    }

int appGetIntegerFromTextWidget(	APP_WIDGET	w,
					int *		pValue,
					int		minValue,
					int		adaptToMin,
					int		maxValue,
					int		adaptToMax )
    {
    int		value;

    char *	s;
    char *	past;

    s= appGetStringFromTextWidget( w );

    value= strtol( s, &past, 10 );

    if  ( past != s )
	{
	while( *past == ' ' )
	    { past++;	}
	}

    if  ( past == s						||
	  *past							||
	  ( ! adaptToMin && value < minValue )			||
	  ( ! adaptToMax && value > maxValue )			)
	{
	appFreeStringFromTextWidget( s );

	appRefuseTextValue( w ); return -1;
	}

    appFreeStringFromTextWidget( s );

    if  ( adaptToMin && value < minValue )
	{ value=  minValue;	}
    if  ( adaptToMax && value > maxValue )
	{ value=  maxValue;	}

    *pValue= value;

    return 0;
    }

int appGetDoubleFromTextWidget(		APP_WIDGET	w,
					double *	pValue,
					double		minValue,
					int		adaptToMin,
					double		maxValue,
					int		adaptToMax )
    {
    double	value;

    char *	s;
    char *	past;

    s= appGetStringFromTextWidget( w );

    value= strtod( s, &past );

    if  ( past != s )
	{
	while( *past == ' ' )
	    { past++;	}
	}

    if  ( past == s						||
	  *past							||
	  ( ! adaptToMin && value < minValue )			||
	  ( ! adaptToMax && value > maxValue )			)
	{
	appFreeStringFromTextWidget( s );

	appRefuseTextValue( w ); return -1;
	}

    appFreeStringFromTextWidget( s );

    if  ( value < minValue )
	{ value=  minValue;	}
    if  ( value > maxValue )
	{ value=  maxValue;	}

    *pValue= value;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert an integer in a text widget.					*/
/*									*/
/************************************************************************/

void appIntegerToTextWidget(		APP_WIDGET	w,
					int		n )
    {
    char	scratch[20];

    sprintf( scratch, "%d", n );
    appStringToTextWidget( w, scratch );

    return;
    }

void appDoubleToTextWidget(		APP_WIDGET	w,
					double		d )
    {
    char	scratch[30];

    sprintf( scratch, "%g", d );
    appStringToTextWidget( w, scratch );

    return;
    }
