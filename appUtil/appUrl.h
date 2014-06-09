/************************************************************************/
/*									*/
/*  Url/Uri manipulation routines.					*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Procedure declarations.						*/
/*									*/
/************************************************************************/

extern int appUrlGetScheme(		char *		scheme,
					int		schemeLen,
					const char *	url );

extern int appUrlGetHttpHostPort(	char *		host,
					int		hostLen,
					char *		port,
					int		portLen,
					const char *	schemeSpecific );

extern int appUrlGetFileHost(		char *		host,
					int		hostLen,
					const char *	schemeSpecific );
