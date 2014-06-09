
#   ifndef	APP_CGI_IN_H
#   define	APP_CGI_IN_H	1

#   include	<appTagval.h>

/************************************************************************/
/*									*/
/*  A full description of a CGI request.				*/
/*									*/
/************************************************************************/

typedef struct CGIRequest
    {
    TaggedValueList *	cgirHeaderValues;
    TaggedValueList *	cgirEnvironmentValues;
    TaggedValueList *	cgirQueryValues;
    TaggedValueList *	cgirCookies;
    const char *	cgirRequestMethod;
    int			cgirStdinUsed;
    } CGIRequest;

/************************************************************************/
/*									*/
/*  Procedure declarations.						*/
/*									*/
/************************************************************************/

extern CGIRequest *	appCgiInGetRequest( void );

extern void appCgiInFreeRequest( CGIRequest * cgir );

#   endif	/*  APP_CGI_IN_H  */
