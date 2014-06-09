/************************************************************************/
/*									*/
/*  sioHttp.[ch]:	Retrieve an URL.				*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include     <string.h>
#   include     <stdlib.h>

#   include     <errno.h>
#   include     <unistd.h>
#   include     <sys/socket.h>

#   include     <sioHttp.h>
#   include	<utilMemoryBuffer.h>

#   include     <appDebugon.h>

typedef enum CommunicationPhase
    {
    CP_Unknown= 0,
    CP_WriteHeader,
    CP_WriteBody,
    CP_ReadHeader,
    CP_ReadBody,

    CP__COUNT
    } CommunicationPhase;

typedef struct HttpConnection
    {
    int			hcFd;
    int			hcCommunicationPhase;
    unsigned char	hcPhases[CP__COUNT];

    MemoryBuffer	hcRequestHeaders;
    MemoryBuffer	hcRequestBody;

    unsigned char	hcHeaderBuffer[SIOsizBUF];
    int			hcInHeaderBuffer;
    int			hcUseHeaderBuffer;
    } HttpConnection;

#   define	HCphaseUNUSED		0x00
#   define	HCphaseUSED		0x01
#   define	HCphaseGOT_END		0x02
#   define	HCphaseEXHAUSTED	0x04
#   define	HCphaseCLOSED		0x08

/************************************************************************/
/*									*/
/*  The output stream. (PUT,POST)					*/
/*									*/
/************************************************************************/

static int sioHttpCloseHeaderOutput(       void *  voidhc )
    {
    int			rval= 0;
    HttpConnection *	hc= (HttpConnection *)voidhc;

    if  ( hc->hcCommunicationPhase == CP_WriteHeader )
	{
	hc->hcPhases[hc->hcCommunicationPhase++] |= HCphaseCLOSED;
	while( hc->hcCommunicationPhase < CP__COUNT			&&
	       ! (hc->hcPhases[hc->hcCommunicationPhase] & HCphaseUSED)	)
	    { hc->hcCommunicationPhase++;	}
	}
    else{ LDEB(hc->hcCommunicationPhase); rval= -1;	}

    return rval;
    }

static int sioOutHttpWriteHeaderBytes(	void *			voidhc,
					const unsigned char *	buffer,
					int			count )
    {
    HttpConnection *	hc= (HttpConnection *)voidhc;

    if  ( hc->hcCommunicationPhase == CP_WriteHeader )
	{
	if  ( utilAddToMemoryBuffer( &(hc->hcRequestHeaders), buffer, count ) )
	    { LDEB(count); return -1;	}

	return count;
	}
    else{ LDEB(hc->hcCommunicationPhase); return -1;	}
    }

/************************************************************************/
/*									*/
/*  The output stream. (PUT,POST)					*/
/*									*/
/************************************************************************/

static int sioHttpCloseBodyOutput(       void *  voidhc )
    {
    int			rval= 0;
    HttpConnection *	hc= (HttpConnection *)voidhc;

    if  ( hc->hcCommunicationPhase == CP_WriteBody )
	{
	int		done;

	char		scratch[30];
	const char *	cl= "Content-Length:";
	const char *	crlf= "\r\n";

	/******/

	sprintf( scratch, "%d", hc->hcRequestBody.mbSize );

	if  ( utilAddToMemoryBuffer( &(hc->hcRequestHeaders),
			(const unsigned char *)cl, strlen( cl ) ) )
	    { LDEB(1); return -1;	}

	if  ( utilAddToMemoryBuffer( &(hc->hcRequestHeaders),
			(const unsigned char *)scratch, strlen( scratch ) ) )
	    { LDEB(1); return -1;	}

	if  ( utilAddToMemoryBuffer( &(hc->hcRequestHeaders),
			(const unsigned char *)crlf, strlen( crlf ) ) )
	    { LDEB(1); return -1;	}

	if  ( utilAddToMemoryBuffer( &(hc->hcRequestHeaders),
			(const unsigned char *)crlf, strlen( crlf ) ) )
	    { LDEB(1); return -1;	}

	done= write( hc->hcFd, hc->hcRequestHeaders.mbBytes,
						hc->hcRequestHeaders.mbSize );
	if  ( done != hc->hcRequestHeaders.mbSize )
	    { LLDEB(hc->hcRequestHeaders.mbSize,done); return -1;	}

	utilCleanMemoryBuffer( &(hc->hcRequestHeaders) );
	utilInitMemoryBuffer( &(hc->hcRequestHeaders) );

	/******/

	done= write( hc->hcFd, hc->hcRequestBody.mbBytes,
					    hc->hcRequestBody.mbSize );
	if  ( done != hc->hcRequestBody.mbSize )
	    { LLDEB(hc->hcRequestBody.mbSize,done); return -1;	}

	if  ( shutdown( hc->hcFd, 1 ) )
	    { LSDEB(hc->hcFd,strerror(errno)); rval= -1;	}

	utilCleanMemoryBuffer( &(hc->hcRequestBody) );
	utilInitMemoryBuffer( &(hc->hcRequestBody) );

	/******/

	hc->hcPhases[hc->hcCommunicationPhase++] |= HCphaseCLOSED;
	while( hc->hcCommunicationPhase < CP__COUNT			&&
	       ! (hc->hcPhases[hc->hcCommunicationPhase] & HCphaseUSED)	)
	    { hc->hcCommunicationPhase++;	}
	}
    else{ LDEB(hc->hcCommunicationPhase); rval= -1;	}

    return rval;
    }

static int sioOutHttpWriteBodyBytes(	void *			voidhc,
					const unsigned char *	buffer,
					int			count )
    {
    HttpConnection *	hc= (HttpConnection *)voidhc;

    if  ( hc->hcCommunicationPhase == CP_WriteBody )
	{
	if  ( utilAddToMemoryBuffer( &(hc->hcRequestBody), buffer, count ) )
	    { LDEB(count); return -1;	}

	return count;
	}
    else{ LDEB(hc->hcCommunicationPhase); return -1;	}
    }

/************************************************************************/
/*									*/
/*  The header stream. (GET,HEAD)					*/
/*									*/
/************************************************************************/

static int sioHttpUseHeaderBuffer(	HttpConnection *	hc,
					unsigned char *		buffer,
					int			count )
    {
    int		use;

    use= hc->hcUseHeaderBuffer;
    if  ( use > count )
	{ use=  count;	}

    memcpy( buffer, hc->hcHeaderBuffer, use );

    if  ( use < hc->hcInHeaderBuffer )
	{
	memmove( hc->hcHeaderBuffer, hc->hcHeaderBuffer+ use,
						hc->hcInHeaderBuffer- use );
	}

    hc->hcInHeaderBuffer -= use;
    hc->hcUseHeaderBuffer -= use;

    return use;
    }

static int sioInHttpReadHeaderBytes(	void *		voidhc,
					unsigned char *	buffer,
					int		count )
    {
    HttpConnection *	hc= (HttpConnection *)voidhc;

    if  ( hc->hcCommunicationPhase == CP_ReadHeader )
	{
	int		done= 0;

	if  ( hc->hcPhases[CP_ReadHeader]
				    & (HCphaseEXHAUSTED|HCphaseCLOSED)	)
	    {
	    LXDEB(hc->hcCommunicationPhase,hc->hcPhases[CP_ReadHeader]);
	    return -1;
	    }

	while( done < count )
	    {
	    int		got;
	    int		crlf2;

	    if  ( hc->hcUseHeaderBuffer > 0 )
		{
		got= sioHttpUseHeaderBuffer( hc, buffer+ done, count- done );

		done += got; continue;
		}

	    if  ( hc->hcPhases[CP_ReadHeader] & HCphaseGOT_END )
		{
		hc->hcPhases[CP_ReadHeader] |= HCphaseEXHAUSTED;
		break;
		}

	    got= read( hc->hcFd,
			    hc->hcHeaderBuffer+ hc->hcInHeaderBuffer,
			    SIOsizBUF- hc->hcInHeaderBuffer );
	    if  ( got < 0 )
		{ LSDEB(hc->hcFd,strerror(errno)); got= 0;	}

	    if  ( done == 0 && got <= 0 )
		{ hc->hcPhases[CP_ReadHeader] |= HCphaseEXHAUSTED;	}
	    if  ( got <= 0 )
		{ break;	}

	    got= hc->hcInHeaderBuffer+ got;

	    for ( crlf2= 0; crlf2 < got- 4; crlf2++ )
		{
		if  ( ! memcmp( hc->hcHeaderBuffer+ crlf2, "\r\n\r\n", 4 ) )
		    { break;	}
		}

	    if  ( crlf2 < got- 4 )
		{
		hc->hcUseHeaderBuffer= crlf2+ 4;
		hc->hcPhases[CP_ReadHeader] |= HCphaseGOT_END;
		}
	    else{
		hc->hcUseHeaderBuffer= crlf2;
		}

	    hc->hcInHeaderBuffer= got;
	    }

	return done;
	}
    else{ LDEB(hc->hcCommunicationPhase); return -1;	}
    }

static int sioHttpCloseHeaderInput(       void *  voidhc )
    {
    int			rval= 0;
    HttpConnection *	hc= (HttpConnection *)voidhc;

    if  ( hc->hcCommunicationPhase == CP_ReadHeader )
	{
	while( ! ( hc->hcPhases[CP_ReadHeader] & HCphaseEXHAUSTED ) )
	    {
	    int			done;
	    unsigned char	b[SIOsizBUF];

	    done= sioInHttpReadHeaderBytes( voidhc, b, SIOsizBUF );
	    if  ( done < 0 )
		{ LDEB(done); return -1;	}
	    }

	hc->hcPhases[hc->hcCommunicationPhase++] |= HCphaseCLOSED;
	while( hc->hcCommunicationPhase < CP__COUNT			&&
	       ! (hc->hcPhases[hc->hcCommunicationPhase] & HCphaseUSED)	)
	    { hc->hcCommunicationPhase++;	}
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  The body stream. (GET,POST,etc)					*/
/*									*/
/************************************************************************/

static int sioHttpCloseBodyInput(       void *  voidhc )
    {
    int			rval= 0;
    HttpConnection *	hc= (HttpConnection *)voidhc;

    if  ( close( hc->hcFd ) )
	{ LSDEB(hc->hcFd,strerror(errno)); rval= -1;	}

    utilCleanMemoryBuffer( &(hc->hcRequestHeaders) );
    utilCleanMemoryBuffer( &(hc->hcRequestBody) );

    free( voidhc );

    return rval;
    }

static int sioInHttpReadBodyBytes(	void *		voidhc,
					unsigned char *	buffer,
					int		count )
    {
    HttpConnection *	hc= (HttpConnection *)voidhc;

    if  ( hc->hcCommunicationPhase == CP_ReadHeader )
	{
	if  ( sioHttpCloseHeaderInput( voidhc ) )
	    { LDEB(hc->hcCommunicationPhase); return -1;	}
	}

    if  ( hc->hcCommunicationPhase == CP_ReadBody )
	{
	int		done= 0;

	if  ( hc->hcPhases[CP_ReadBody]
				    & (HCphaseEXHAUSTED|HCphaseCLOSED)	)
	    {
	    LXDEB(hc->hcCommunicationPhase,hc->hcPhases[CP_ReadBody]);
	    return -1;
	    }

	while( done < count )
	    {
	    int		got;

	    if  ( hc->hcInHeaderBuffer > 0 )
		{
		hc->hcUseHeaderBuffer= hc->hcInHeaderBuffer;

		got= sioHttpUseHeaderBuffer( hc, buffer+ done, count- done );

		done += got;
		continue;
		}

	    got= read( hc->hcFd, buffer+ done, count- done );

	    if  ( got < 0 )
		{ LSDEB(hc->hcFd,strerror(errno)); got= 0;		}
	    if  ( done == 0 && got <= 0 )
		{ hc->hcPhases[CP_ReadBody] |= HCphaseEXHAUSTED;	}
	    if  ( got <= 0 )
		{ break;	}

	    done += got;
	    }

	return done;
	}
    else{ LDEB(hc->hcCommunicationPhase); return -1;	}
    }

/************************************************************************/
/*									*/
/*  Make a HTTP request and return streams to write bytes to the other	*/
/*  side, and then read bytes from the other side.			*/
/*									*/
/************************************************************************/

static void sioHttpInitConnection(	HttpConnection *	hc )
    {
    hc->hcFd= -1;
    hc->hcCommunicationPhase= CP_Unknown;
    memset( hc->hcPhases, HCphaseUNUSED, CP__COUNT );

    utilInitMemoryBuffer( &(hc->hcRequestHeaders) );
    utilInitMemoryBuffer( &(hc->hcRequestBody) );

    hc->hcHeaderBuffer[0]= '\0';

    hc->hcInHeaderBuffer= 0;
    hc->hcUseHeaderBuffer= 0;

    return;
    }

static int sioHttpMakeRequest(		int			fd,
					const char *		url,
					const char *		method )
    {
    char *			request= (char *)0;
    int				requestLength;

    requestLength= 0;
    requestLength += strlen( method )+ 1;	/*  "GET "		*/
    requestLength += strlen( url );		/*  url			*/
    requestLength += 9;				/*  " HTTP/1.0"		*/
    requestLength += 2;				/*  "\r\n"		*/

    request= malloc( requestLength+ 1 );
    if  ( ! request )
	{ XDEB(request); return -1;	}

    sprintf( request, "%s %s HTTP/1.0\r\n", method, url );

    if  ( write( fd, request, requestLength ) != requestLength )
	{
	LSDEB(requestLength,strerror(errno));
	free( request );
	return -1;
	}

    free( request );
    return 0;
    }

int sioHttpOpen(	SimpleInputStream **	pSisBody,
			SimpleInputStream **	pSisHeader,
			SimpleOutputStream **	pSosBody,
			SimpleOutputStream **	pSosHeader,
			const char *		host,
			const char *		port,
			const char *		url,
			const char *		method,
			void *			through,
			APP_COMPLAIN		complain )
    {
    int				fd= -1;
    HttpConnection *		hc= (HttpConnection *)0;

    SimpleInputStream *		sisBody= (SimpleInputStream *)0;
    SimpleInputStream *		sisHeader= (SimpleInputStream *)0;
    SimpleOutputStream *	sosBody= (SimpleOutputStream *)0;
    SimpleOutputStream *	sosHeader= (SimpleOutputStream *)0;

    int				openBodyIn= 0;
    int				openBodyOut= 0;

    if  ( ! port )
	{ port= "http";	}

    if  ( strcmp( method, "HEAD" ) )
	{
	if  ( ! pSisBody )
	    { SXDEB(method,pSisBody); goto failure;	}

	openBodyIn= 1;
	}

    if  ( ! strcmp( method, "PUT" )	||
	  ! strcmp( method, "POST" )	)
	{
	if  ( ! pSosBody )
	    { SXDEB(method,pSosBody); goto failure;	}

	openBodyOut= 1;
	}

    fd= appOpenSocket( host, port, through, complain );
    if  ( fd < 0 )
	{ LDEB(fd); goto failure;	}

    if  ( sioHttpMakeRequest( fd, url, method ) )
	{ LDEB(fd); goto failure;	}

    hc= (HttpConnection *)malloc( sizeof(HttpConnection) );
    if  ( ! hc )
	{ XDEB(hc); goto failure;	}
    sioHttpInitConnection( hc );

    hc->hcFd= fd;

    /******/

    if  ( pSisHeader )
	{
	sisHeader= sioInOpen( (void *)hc, sioInHttpReadHeaderBytes,
						sioHttpCloseHeaderInput );
	if  ( ! sisHeader )
	    { XDEB(sisHeader); goto failure;	}

	hc->hcPhases[CP_ReadHeader] |= HCphaseUSED;
	}

    /******/

    if  ( openBodyIn )
	{
	sisBody= sioInOpen( (void *)hc, sioInHttpReadBodyBytes,
						    sioHttpCloseBodyInput );
	if  ( ! sisBody )
	    { XDEB(sisBody); goto failure;	}

	hc->hcPhases[CP_ReadBody] |= HCphaseUSED;
	}

    /******/

    if  ( pSosHeader )
	{
	sosHeader= sioOutOpen( (void *)hc, sioOutHttpWriteHeaderBytes,
				    (SIOoutSEEK)0, sioHttpCloseHeaderOutput );
	if  ( ! sosHeader )
	    { XDEB(sosHeader); goto failure;	}

	hc->hcPhases[CP_WriteHeader] |= HCphaseUSED;
	}

    /******/

    if  ( openBodyOut )
	{
	sosBody= sioOutOpen( (void *)hc, sioOutHttpWriteBodyBytes,
					(SIOoutSEEK)0, sioHttpCloseBodyOutput );
	if  ( ! sosBody )
	    { XDEB(sosBody); goto failure;	}

	hc->hcPhases[CP_WriteBody] |= HCphaseUSED;
	}
    else{ hc->hcPhases[CP_ReadHeader] |= HCphaseUSED;	}

    /******/

    hc->hcPhases[hc->hcCommunicationPhase++] |= HCphaseCLOSED;
    while( hc->hcCommunicationPhase < CP__COUNT				&&
	   ! (hc->hcPhases[hc->hcCommunicationPhase] & HCphaseUSED)	)
	{ hc->hcCommunicationPhase++;	}

    /******/

    if  ( pSisBody )
	{ *pSisBody= sisBody;		}
    if  ( pSisHeader )
	{ *pSisHeader= sisHeader;	}
    if  ( pSosBody )
	{ *pSosBody= sosBody;		}
    if  ( pSosHeader )
	{ *pSosHeader= sosHeader;	}

    return 0;

  failure:
    LDEB(1);

    utilCleanMemoryBuffer( &(hc->hcRequestHeaders) );
    utilCleanMemoryBuffer( &(hc->hcRequestBody) );

    if  ( hc )
	{ free( hc );	}

    if  ( fd >= 0 && close( fd ) )
	{ LSDEB(fd,strerror(errno));	}

    return -1;
    }
