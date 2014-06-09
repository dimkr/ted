/************************************************************************/
/*									*/
/*  Utility functions for CGI programming.				*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	<unistd.h>

#   include	<appCgiIn.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Return an array of values with headers from the environment.	*/
/*									*/
/************************************************************************/

extern char **		environ;

static TaggedValueList * appCgiInGetHeaderList( void )
    {
    char **		e;
    int			count;

    TaggedValueList *	tvl;

    e= environ; count= 0;
    while( *e )
	{
	if  ( ! strncmp( *e, "HTTP_", 5 ) )
	    { count++;	}

	e++;
	}

    tvl= appTagvalAllocateList( count );
    if  ( ! tvl )
	{ LDEB(1); return (TaggedValueList *)0;	}

    e= environ; count= 0;
    while( *e )
	{
	if  ( ! strncmp( *e, "HTTP_", 5 ) )
	    {
	    char *	v= strchr( *e, '=' );

	    if  ( ! v )
		{ SDEB(*e);	}
	    else{
		char *			buf;
		TaggedValueList *	fresh;

		v++;

		buf= malloc( v- *e- 5 );
		memcpy( buf, *e+ 5, v- *e- 5 );
		buf[v- *e- 6]= '\0';

		fresh= appTagvalAddValue( tvl, buf, v, 1, 0 );
		if  ( ! fresh )
		    {
		    LLDEB(tvl->tvlCount,fresh);
		    appTagvalFreeValueList( tvl );
		    return fresh;
		    }

		tvl= fresh;
		}
	    }

	e++;
	}

    return tvl;
    }

/************************************************************************/
/*									*/
/*  Return an array of values with the standard environment variables.	*/
/*									*/
/************************************************************************/

static TaggedValueList * appCgiInGetEnvironmentList( void )
    {
    static char *	environmentVariables[]=
	{
	"SERVER_PROTOCOL",
	"SERVER_PORT",
	"REQUEST_METHOD",
	"PATH_INFO",
	"PATH_TRANSLATED",
	"SCRIPT_NAME",
	"QUERY_STRING",
	"REMOTE_HOST",
	"REMOTE_ADDR",
	"AUTH_TYPE",
	"REMOTE_USER",
	"REMOTE_IDENT",
	"CONTENT_TYPE",
	"CONTENT_LENGTH",

	"GATEWAY_INTERFACE",
	"SERVER_NAME",
	"SERVER_SOFTWARE",
	};

    TaggedValueList *	values;
    int			count= sizeof(environmentVariables)/sizeof(char *);

    values= (TaggedValueList *)malloc(
			sizeof(TaggedValueList)+ count* sizeof(TaggedValue) );
    if  ( ! values )
	{ LLDEB(count,values); return (TaggedValueList *)0;	}

    values->tvlCount= count;

    for ( count= 0; count < values->tvlCount; count++ )
	{
	values->tvlValues[count].tvName= environmentVariables[count];
	values->tvlValues[count].tvNameAllocated= 0;

	values->tvlValues[count].tvValue=
					getenv( environmentVariables[count] );
	values->tvlValues[count].tvValueAllocated= 0;
	}

    return values;
    }

/************************************************************************/
/*									*/
/*  Analyse a CGI Query string.						*/
/*									*/
/************************************************************************/

static TaggedValueList * appCgiInAnalyseQueryString(
					const char *	queryString )
    {
    int			consumed= 0;

    int			count= 0;
    TaggedValueList *	tvl= (TaggedValueList *)0;

    while( * queryString )
	{
	char *		pair= (char *)0;
	int		pos= 0;
	int		allocatedSize= 0;
	int		firstEqual= -1;
	int		c;
	const char *	from= queryString;

	TaggedValueList *	nw;

	if  ( *from == '&' )
	    { queryString= from+ 1; continue; }

	while( * from )
	    {
	    c= *(from++); consumed++;

	    if  ( c == '&' )
		{ break;	}
	    if  ( ! c )
		{
		XDEB(c);
		if  ( tvl )
		    { appTagvalFreeValueList( tvl );	}
		return (TaggedValueList *)0;
		}

	    if  ( consumed >= allocatedSize )
		{
		int	sz= ( 3* ( allocatedSize+ 1 ) )/ 2;
		char *	nwp= realloc( pair, sz+ 1 );

		if  ( ! nwp )
		    {
		    LLDEB(sz,nwp);
		    if  ( tvl )
			{ appTagvalFreeValueList( tvl );	}
		    return (TaggedValueList *)0;
		    }

		pair= nwp; allocatedSize= sz;
		}

	    if  ( c == '=' && firstEqual < 0 )
		{ firstEqual= pos; }

	    /*  No.. Only in the argument
	    if  ( c == '+' )
		{ pair[pos++]= ' '; continue;	}
	    */

	    if  ( c == '%' )
		{
		char		b[3];
		unsigned int	uc;

		b[0]= *(from++); consumed++;
		b[1]= *(from++); consumed++;
		b[2]= '\0';

		sscanf( b, "%x", &uc );

		pair[pos++]= uc; continue;
		}

	    pair[pos++]= c; continue;
	    }

	pair[pos]= '\0';

	if  ( ! tvl )
	    {
	    nw= appTagvalAllocateList( 1 );
	    if  ( ! nw )
		{ LLDEB(count,nw); return (TaggedValueList *)0; }

	    tvl= nw;
	    }

	if  ( firstEqual > 0 )
	    {
	    pair[firstEqual]= '\0';
	    nw= appTagvalAddValue( tvl, pair, pair+ firstEqual+ 1, 1, 0 );
	    }
	else{ nw= appTagvalAddValue( tvl, pair, (char *)0, 1, 0 );	}

	if  ( ! nw )
	    {
	    LDEB(nw);
	    appTagvalFreeValueList( tvl );
	    return (TaggedValueList *)0;
	    }

	tvl= nw;

	queryString= from;
	}

    return tvl;
    }

/************************************************************************/
/*									*/
/*  Read a chunk of data from a stream and reduce it to TaggedValue's.	*/
/*									*/
/************************************************************************/

static TaggedValueList * appCgiInReadValueList(	FILE *		from,
						int		contentLength )
    {
    TaggedValueList *	values= (TaggedValueList *)0;

    char *		buffer;

    if  ( contentLength == 0 )
	{ LDEB(contentLength); return (TaggedValueList *)0;	}

    buffer= malloc( contentLength+ 1 );
    if  ( ! buffer )
	{ XDEB(buffer); return (TaggedValueList *)0;	}

    if  ( fread( buffer, 1, contentLength, from ) != contentLength )
	{ LDEB(contentLength); free( buffer ); return (TaggedValueList *)0; }
    buffer[contentLength]= '\0';

    values= appCgiInAnalyseQueryString( buffer );

    free( buffer );

    return values;
    }

/************************************************************************/
/*									*/
/*  Analyse the value of the COOKIE header.				*/
/*									*/
/************************************************************************/

static TaggedValueList * appCgiInAnalyseCookieString(
					const char *	cookieString )
    {
    int			consumed= 0;

    int			count= 0;
    TaggedValueList *	tvl= (TaggedValueList *)0;

    while( * cookieString )
	{
	char *		pair= (char *)0;
	int		pos= 0;
	int		allocatedSize= 0;
	int		firstEqual= -1;
	int		c;
	const char *	from= cookieString;

	TaggedValueList *	nw;

	if  ( *from == ';' )
	    { cookieString= from+ 1; continue; }

	while( * from )
	    {
	    c= *(from++); consumed++;

	    if  ( c == ';' )
		{ break;	}
	    if  ( ! c )
		{
		XDEB(c);
		if  ( tvl )
		    { appTagvalFreeValueList( tvl );	}
		return (TaggedValueList *)0;
		}

	    if  ( consumed >= allocatedSize )
		{
		int	sz= ( 3* ( allocatedSize+ 1 ) )/ 2;
		char *	nwp= realloc( pair, sz+ 1 );

		if  ( ! nwp )
		    {
		    LLDEB(sz,nwp);
		    if  ( tvl )
			{ appTagvalFreeValueList( tvl );	}
		    return (TaggedValueList *)0;
		    }

		pair= nwp; allocatedSize= sz;
		}

	    if  ( c == '=' && firstEqual < 0 )
		{ firstEqual= pos; }

	    pair[pos++]= c; continue;
	    }

	pair[pos]= '\0';

	if  ( ! tvl )
	    {
	    nw= appTagvalAllocateList( 1 );
	    if  ( ! nw )
		{ LLDEB(count,nw); return (TaggedValueList *)0; }

	    tvl= nw;
	    }

	if  ( firstEqual > 0 )
	    {
	    pair[firstEqual]= '\0';
	    nw= appTagvalAddValue( tvl, pair, pair+ firstEqual+ 1, 1, 0 );
	    }
	else{ nw= appTagvalAddValue( tvl, pair, (char *)0, 1, 0 );	}

	if  ( ! nw )
	    {
	    LDEB(nw);
	    appTagvalFreeValueList( tvl );
	    return (TaggedValueList *)0;
	    }

	tvl= nw;

	cookieString= from;
	}

    return tvl;
    }

/************************************************************************/
/*									*/
/*  Retrieve the REQUEST_METOD field from the environment values.	*/
/*									*/
/************************************************************************/

static int appCgiInGetRequestMethod(	const char **		pValue,
					TaggedValueList *	tvl	)
    {
    const char *	s;
    int			n;

    int		res= appTagvalGetStringValue( &s, &n, tvl, "REQUEST_METHOD" );

    if  ( res )
	{ LDEB(res); return res;	}

    if  ( n )
	{ return 1;	}

    *pValue= s; return 0;
    }

/************************************************************************/
/*									*/
/*  Build the request struct.						*/
/*									*/
/************************************************************************/

extern CGIRequest * appCgiInGetRequest( void )
    {
    CGIRequest *	cgir= (CGIRequest *)malloc( sizeof(CGIRequest) );
    int			res;
    int			n;
    const char *	contentType;
    long		contentLength;

    if  ( ! cgir )
	{ XDEB(cgir); return cgir;	}

    cgir->cgirHeaderValues= (TaggedValueList *)0;
    cgir->cgirEnvironmentValues= (TaggedValueList *)0;
    cgir->cgirQueryValues= (TaggedValueList *)0;
    cgir->cgirCookies= (TaggedValueList *)0;
    cgir->cgirRequestMethod= (char *)0;
    cgir->cgirStdinUsed= 0;

    cgir->cgirEnvironmentValues= appCgiInGetEnvironmentList();
    if  ( ! cgir->cgirEnvironmentValues )
	{
	XDEB(cgir->cgirEnvironmentValues);
	appCgiInFreeRequest( cgir );
	return (CGIRequest *)0;
	}

    cgir->cgirHeaderValues= appCgiInGetHeaderList();
    if  ( ! cgir->cgirHeaderValues )
	{
	XDEB(cgir->cgirHeaderValues);
	appCgiInFreeRequest( cgir );
	return (CGIRequest *)0;
	}

    if  ( appCgiInGetRequestMethod( &cgir->cgirRequestMethod,
					    cgir->cgirEnvironmentValues ) )
	{
	LDEB(1);
	appCgiInFreeRequest( cgir );
	return (CGIRequest *)0;
	}

    if  ( ! strcmp( cgir->cgirRequestMethod, "POST" )	||
	  ! strcmp( cgir->cgirRequestMethod, "PUT" )	)
	{
	res= appTagvalGetStringValue( &contentType, &n,
			    cgir->cgirEnvironmentValues, "CONTENT_TYPE" );
	if  ( res || n )
	    {
	    LLDEB(res,n);
	    appCgiInFreeRequest( cgir );
	    return (CGIRequest *)0;
	    }

	res= appTagvalGetLongValue( &contentLength, &n,
			    cgir->cgirEnvironmentValues, "CONTENT_LENGTH" );
	if  ( res || n )
	    {
	    LLDEB(res,n);
	    appCgiInFreeRequest( cgir );
	    return (CGIRequest *)0;
	    }

	if  ( ! strcmp( contentType, "application/x-www-form-urlencoded" ) )
	    {
	    cgir->cgirStdinUsed= 1;

	    cgir->cgirQueryValues=
			    appCgiInReadValueList( stdin, (int)contentLength );

	    if  ( ! cgir->cgirQueryValues )
		{
		XDEB(cgir->cgirQueryValues);
		appCgiInFreeRequest( cgir );
		return (CGIRequest *)0;
		}
	    }
	}

    if  ( ! strcmp( cgir->cgirRequestMethod, "GET" )	)
	{
	const char *	queryString;

	res= appTagvalGetStringValue( &queryString, &n,
			    cgir->cgirEnvironmentValues, "QUERY_STRING" );

	if  ( res )
	    {
	    LLDEB(res,n);
	    appCgiInFreeRequest( cgir );
	    return (CGIRequest *)0;
	    }

	if  ( ! n && queryString[0] )
	    {
	    cgir->cgirQueryValues= appCgiInAnalyseQueryString( queryString );

	    if  ( ! cgir->cgirQueryValues )
		{
		XDEB(cgir->cgirQueryValues);
		appCgiInFreeRequest( cgir );
		return (CGIRequest *)0;
		}
	    }
	}

    {
    const char *	cookieString;

    res= appTagvalGetStringValue( &cookieString, &n,
					cgir->cgirHeaderValues, "COOKIE" );

    if  ( res < 0 )
	{
	LLDEB(res,n);
	appCgiInFreeRequest( cgir );
	return (CGIRequest *)0;
	}

    if  ( res == 0 && ! n && cookieString[0] )
	{
	cgir->cgirCookies= appCgiInAnalyseCookieString( cookieString );

	if  ( ! cgir->cgirCookies )
	    {
	    XDEB(cgir->cgirCookies);
	    appCgiInFreeRequest( cgir );
	    return (CGIRequest *)0;
	    }
	}
    }

    return cgir;
    }

extern void appCgiInFreeRequest( CGIRequest * cgir )
    {
    if  ( cgir->cgirHeaderValues )
        { appTagvalFreeValueList( cgir->cgirHeaderValues );	}
    if  ( cgir->cgirEnvironmentValues )
	{ appTagvalFreeValueList( cgir->cgirEnvironmentValues );	}
    if  ( cgir->cgirQueryValues )
	{ appTagvalFreeValueList( cgir->cgirQueryValues );	}

    free( cgir );

    return;
    }
