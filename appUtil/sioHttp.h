/************************************************************************/
/*									*/
/*  sioHttp.[ch]:	Retrieve an URL.				*/
/*									*/
/************************************************************************/

#   ifndef	SIO_HTTP_H
#   define	SIO_HTTP_H

#   include     <sioGeneral.h>
#   include     <appSystem.h>

extern int sioHttpOpen(		SimpleInputStream **	pSisBody,
				SimpleInputStream **	pSisHeader,
				SimpleOutputStream **	pSosBody,
				SimpleOutputStream **	pSosHeader,
				const char *		host,
				const char *		port,
				const char *		url,
				const char *		method,
				void *			through,
				APP_COMPLAIN		complain );

#   endif
