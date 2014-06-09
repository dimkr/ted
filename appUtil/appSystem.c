/************************************************************************/
/*									*/
/*  Consult the operating system about certain things.			*/
/*									*/
/*  0)  mkdir() is not declared on linux, suppress warning.		*/
/*  1)  Note that we use shell commands to consult the file system.	*/
/*	this is the easiest way.					*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<sys/types.h>
#   include	<pwd.h>
#   include	<unistd.h>
#   include	<sys/stat.h>
#   include	<time.h>
#   include	<errno.h>
#   include	<netinet/in.h>
#   include	<unistd.h>
#   include	<netdb.h>
#   include	<sys/utsname.h>

#   include	<appSystem.h>

#   include	<appDebugon.h>

#   ifdef NeXT
#	include	<libc.h>
#   endif

    /*  0  */
#   ifdef __linux__
    extern int mkdir( const char * name, mode_t mode );
#   endif

#   define	USE_STAT	1
#   define	USE_ACCESS	1
#   define	USE_GETCWD	1

#   if  HAVE_SYS_SOCKET_H
#	include	<sys/socket.h>
#   endif

#   if  HAVE_DIRENT_H
#	define	USE_OPENDIR	1
#	include	<dirent.h>
#   endif

/************************************************************************/
/*									*/
/*  Determine the home directory of the user.				*/
/*									*/
/************************************************************************/

int appHomeDirectory(	char *		home,
			int		len )
    {
    struct passwd *	pwd;
    int			l;

    pwd= getpwuid( getuid() );
    if  ( ! pwd )
	{ LXDEB(getuid(),pwd); return -1;	}

    if  ( ! pwd->pw_dir )
	{ XDEB(pwd->pw_dir);	}

    l= strlen( pwd->pw_dir );
    if  ( l > len )
	{ SLDEB(pwd->pw_dir,len); return -1;	}

    strcpy( home, pwd->pw_dir ); return l;
    }

/************************************************************************/
/*									*/
/*  Determine the current directory.					*/
/*									*/
/************************************************************************/

int appCurrentDirectory(	char *		pwd,
				int		len )
    {
#   if USE_GETCWD
    if  ( ! getcwd( pwd, len ) )
	{ LSDEB(len,strerror(errno)); return -1; }

    len= strlen( pwd );

#   else
    {
    FILE *	f= popen( "pwd", "r" );

    if  ( ! f )
	{ XDEB(f); return -1;	}

    if  ( ! fgets( pwd, len, f ) )
	{ LDEB(1); pclose( f ); return -1;	}

    len= strlen( len );
    if  ( len < 1 || pwd[len -1] != '\n' )
	{ SDEB(len); return -1;	}

    pwd[len--]= '\0';

    pclose( f );
    }
#   endif

    return len;
    }

/************************************************************************/
/*									*/
/*  Check whether a directory exists.					*/
/*									*/
/************************************************************************/

#if USE_STAT
int appTestDirectory(	const char *	dir )
    {
    struct stat	st;

    if  ( stat( dir, &st ) )
	{
	if  ( errno != ENOENT )
	    { SSDEB(dir,strerror(errno)); }

	return -1;
	}

    if  ( ! S_ISDIR( st.st_mode ) )
	{ return -1;	}

    return 0;
    }
#else
int appTestDirectory(	const char *	dir )
    {
    char	scratch[1001];

    sprintf( scratch, "test -d '%s'", dir );

    if  ( system( scratch ) )
	{ return -1;	}

    return 0;
    }
#endif

#if USE_ACCESS
int appTestFileWritable( const char *	file )
    {
    if  ( access( file, W_OK ) )
	{ return -1;	}

    return 0;
    }
#else
int appTestFileWritable( const char *	file )
    {
    char	scratch[1001];

    sprintf( scratch, "test -w '%s'", file );

    if  ( system( scratch ) )
	{ return -1;	}

    return 0;
    }
#endif

#if USE_ACCESS
int appTestFileReadable( const char *	file )
    {
    if  ( access( file, R_OK ) )
	{ return -1;	}

    return 0;
    }
#else
int appTestFileReadable( const char *	file )
    {
    char	scratch[1001];

    sprintf( scratch, "test -w '%s'", file );

    if  ( system( scratch ) )
	{ return -1;	}

    return 0;
    }
#endif

#if USE_STAT
int appTestFileExists(	const char *	file )
    {
    struct stat	st;

    if  ( stat( file, &st ) )
	{
	if  ( errno != ENOENT )
	    { SSDEB(file,strerror(errno)); }

	return -1;
	}

    if  ( ! S_ISREG( st.st_mode ) )
	{ return -1;	}

    return 0;
    }
#else
int appTestFileExists( const char *	file )
    {
    char	scratch[1001];

    sprintf( scratch, "test -f '%s'", file );

    if  ( system( scratch ) )
	{ return -1;	}

    return 0;
    }
#endif

/************************************************************************/
/*									*/
/*  Make a Directory.							*/
/*									*/
/************************************************************************/

int appMakeDirectory(	const char *	dir )
    {
    if  ( mkdir( dir, 0777 ) )
	{ SSDEB(dir,strerror(errno)); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make a string that is as unique as possible.			*/
/*									*/
/************************************************************************/

long appGetTimestamp( void )
    {
    time_t	now;

    now= time( (time_t *)0 ); sleep( 1 );

    return now;
    }

int appMakeUniqueString(	char *		target,
				int		maxlen )
    {
    int				pid= getpid();
    time_t			now= time( (time_t *)0 );
    int				needed;

    static time_t		prevNow;
    static unsigned long	count;

#   if HAVE_UNAME
    struct utsname		un;
#   endif

#   ifdef USE_GETHOSTNAME
    char			node_scratch[256+1];
#   endif

    if  ( now == prevNow )
	{ needed= 26; count++;	}
    else{ needed= 17; count= 0;	}

    prevNow= now;

    if  ( maxlen < needed )
	{ LLDEB(maxlen,needed); return -1;	}

    if  ( needed == 26 )
	{
	sprintf( target, "%08lx.%08lx.%08lx",
				(unsigned long)now,
				(unsigned long)pid,
				(unsigned long)count );
	}
    else{
	sprintf( target, "%08lx.%08lx",
				(unsigned long)now,
				(unsigned long)pid );
	}

    maxlen -= needed; target += needed;

    if  ( maxlen > 10 )
	{
	char *			nodename;
	struct hostent *	hp;

#	if HAVE_UNAME
	if  ( uname( &un ) < 0 )
	    { LDEB(1); return -1;	}
	nodename= un.nodename;
#	endif

#	ifdef USE_GETHOSTNAME
	if  ( gethostname( node_scratch, sizeof(node_scratch)- 1 ) )
	    { LDEB(1); return (char *)0;		}

	node_scratch[sizeof(node_scratch)-1]= '\0';
	nodename= node_scratch;
#	endif

	if  ( ! nodename )
	    { XDEB(nodename); return -1;	}

	if  ( ! strchr( nodename, '.' ) )
	    {
	    hp= gethostbyname( nodename );
	    if  ( hp )
		{ nodename= hp->h_name; }
	    }

	if  ( strlen( nodename ) < maxlen )
	    {
	    *target= '@';
	    maxlen -= 1; target += 1;
	    strcpy( target, nodename );
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remove a file.							*/
/*									*/
/************************************************************************/

int appRemoveFile(	const char *	filename )
    {
    if  ( remove( filename ) )
	{ SSDEB(filename,strerror(errno)); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Call a function for all files matching a certain criterion.		*/
/*									*/
/************************************************************************/

#   define	FILEL		1000

#if USE_OPENDIR

int appForAllFiles(	const char *		dir,
			const char *		ext,
			void *			through,
			FILE_CALLBACK		callback )
    {
    int			rval= 0;
    DIR *		d= opendir( dir );
    struct dirent *	de;

    if  ( ! d )
	{ SSDEB(dir,strerror(errno)); return -1;	}

    de= readdir( d );
    while( de )
	{
	char *	dot= strrchr( de->d_name, '.' );

#	ifdef __VMS
	char *	semicolon= strrchr( de->d_name, ';' );

	if  ( semicolon )
	    { *semicolon= '\0';	}
#	endif

	if  ( ! strcmp( de->d_name, "." ) )
	    { de= readdir( d ); continue;	}
	if  ( ! strcmp( de->d_name, ".." ) )
	    { de= readdir( d ); continue;	}

	if  ( ! ext					||
	      ( dot				&&
	      ! strcmp( dot+ 1, ext )		)	)
	    {
	    char	scratch[FILEL+1];

#	    ifdef __VMS
	    sprintf( scratch, "%s%s",  dir, de->d_name );
#	    else
	    sprintf( scratch, "%s/%s", dir, de->d_name );
#	    endif

	    if  ( (*callback)( scratch, through )	)
		{ SDEB(scratch); rval= -1; break;	}
	    }

	de= readdir( d );
	}

    if  ( closedir( d ) )
	{ SSDEB(dir,strerror(errno)); rval= -1;	}

    return rval;
    }
#else

int appForAllFiles(	const char *		dir,
			const char *		ext,
			void *			through,
			FILE_CALLBACK		callback )
    {
    int			rval= 0;
    const char *	format;
    char		scratch[FILEL+1];
    FILE *		names;

    if  ( ext )
	{
	format= "( ls -d '%s'/*.'%s' '%s'/.*.'%s' ) 2>/dev/null";

	sprintf( scratch, format, dir, ext, dir, ext );
	names= popen( scratch, "r" );
	if  ( ! names )
	    { SXDEB(scratch,names); return -1;	}
	}
    else{
	format= "( ls -d '%s'/* '%s'/.*  ) 2>/dev/null";

	sprintf( scratch, format, dir, dir );
	names= popen( scratch, "r" );
	if  ( ! names )
	    { SXDEB(scratch,names); return -1;	}
	}

    while( fgets( scratch, FILEL, names ) )
	{
	int	len= strlen( scratch );

	if  ( len > 0 && scratch[len -1] == '\n' )
	    { scratch[len -1]= '\0';		}
	else{ SLDEB(scratch,len); continue;	}

	if  ( ! strcmp( scratch, "." ) )
	    { continue;	}
	if  ( ! strcmp( scratch, ".." ) )
	    { continue;	}

	if  ( (*callback)( scratch, through ) )
	    { SDEB(scratch); rval= -1; break;	}
	}

    if  ( pclose( names ) )
	{ LDEB(1); rval= -1;	}

    return rval;
    }

#endif

/************************************************************************/
/*									*/
/*  Open a socket							*/
/*									*/
/************************************************************************/

int appOpenSocket(	const char *		hostName,
			const char *		portName,
			void *			through,
			APP_COMPLAIN		complain )
    {
    struct sockaddr_in	sina;

    int			port;
    char		c;
    int			b0, b1, b2, b3;
    unsigned char *	mach= (unsigned char *)&(sina.sin_addr.s_addr);

    int			attempt;
    int			secondsBetweenRetries= 1;
    const int		retryCount= 5;

    memset( &sina, 0, sizeof(sina) );

    if  ( sscanf( hostName, "%d.%d.%d.%d%c", &b0, &b1, &b2, &b3, &c ) == 4 )
	{
	mach[0]= b0; mach[1]= b1; mach[2]= b2; mach[3]= b3;
	sina.sin_family= AF_INET;
	}
    else{
	struct hostent *	host= gethostbyname( hostName );

	if  ( ! host )
	    {
	    SDEB(strerror(errno));
	    (*complain)( through, APP_SYSTEMeHOST, hostName );
	    return -1;
	    }

	memcpy( &sina.sin_addr, host->h_addr, host->h_length );
	sina.sin_family= host->h_addrtype;
	}

    c= 0;
    if  ( sscanf( portName, "%d%c", &port, &c ) == 1 && c == 0 )
	{ sina.sin_port= htons( port ); }
    else{
	struct servent *	serv= getservbyname( portName, "tcp" );

	if  ( ! serv )
	    {
	    SDEB(strerror(errno));
	    (*complain)( through, APP_SYSTEMeSERV, portName );
	    return -1;
	    }

	sina.sin_port= serv->s_port;
	}

    for ( attempt= 0; attempt < retryCount; attempt++ )
	{
	int	fd= socket( AF_INET, SOCK_STREAM, 0 );

	if  ( fd < 0 )
	    {
	    SDEB(strerror(errno));
	    (*complain)( through, APP_SYSTEMeSOCK, strerror(errno) );
	    return -1;
	    }

	if  ( connect( fd, (struct sockaddr *)&sina, sizeof(sina) ) )
	    {
	    SDEB(strerror(errno));
	    (*complain)( through, APP_SYSTEMeCONN, hostName );

	    close( fd );

	    sleep( secondsBetweenRetries ); continue;
	    }

	return fd;
	}

    return -1;
    }


int appListenSocket(		const char *	portName,
				unsigned int *	pPort,
				void *		through,
				APP_COMPLAIN	complain )
    {
    struct sockaddr_in	si;
    unsigned int	sinlen= sizeof( struct sockaddr_in );
    int			fd;
    unsigned int	port;
    char		c;

    memset( &si, 0, sinlen );

    c= 0;
    if  ( sscanf( portName, "%u%c", &port, &c ) == 1 && c == 0 )
	{ si.sin_port= htons( port ); }
    else{
	struct servent *	serv= getservbyname( portName, "tcp" );

	if  ( ! serv )
	    {
	    SDEB(strerror(errno));
	    (*complain)( through, APP_SYSTEMeSERV, portName );
	    return -1;
	    }

	si.sin_port= serv->s_port;
	}

    if  ( (fd= socket( AF_INET, SOCK_STREAM, 0 )) < 0 )
	{
	SDEB(strerror(errno));
	(*complain)( through, APP_SYSTEMeSOCK, strerror(errno) );
	return -1;
	}

    si.sin_family= AF_INET;
    si.sin_addr.s_addr= INADDR_ANY;

    if  ( bind( fd, (struct sockaddr *)&si, sinlen ) < 0 )
	{
	SDEB(strerror(errno));
	(*complain)( through, APP_SYSTEMeBIND, strerror(errno) );
	close( fd );
	return -1;
	}

    if  ( getsockname( fd, (struct sockaddr *)&si, &sinlen ) < 0 )
	{
	(*complain)( through, APP_SYSTEMeBIND, strerror(errno) );
	close( fd );
	return -1;
	}

    if  ( listen( fd, 5 ) < 0 )
	{
	SDEB(strerror(errno));
	(*complain)( through, APP_SYSTEMeLISTEN, strerror(errno) );
	close( fd );
	return -1;
	}

    if  ( pPort )
	{ *pPort= ntohs( si.sin_port );			}

    return fd;
    }

int appAcceptSocket(		int		lfd,
				void *		through,
				APP_COMPLAIN	complain	)
    {
    struct sockaddr_in	si;
    unsigned int	sinlen= sizeof( struct sockaddr_in );
    int			fd;

    if  ( (fd= accept( lfd, (struct sockaddr *)&si, &sinlen )) < 0 )
	{
	SDEB(strerror(errno));
	(*complain)( through, APP_SYSTEMeACCEPT, strerror(errno) );
	}

    return fd;
    }
