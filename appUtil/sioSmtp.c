#   include	"appUtilConfig.h"

#   include	<sioSmtp.h>
#   include	<sioMemory.h>
#   include	<sioBase64.h>
#   include	<utilMemoryBuffer.h>

#   include	<stdlib.h>
#   include	<stdarg.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	<time.h>
#   include	<sys/time.h>
#   include	<sys/types.h>
#   include	<sys/socket.h>
#   include	<pwd.h>
#   include	<unistd.h>
#   include	<fcntl.h>
#   include	<errno.h>
#   include	<sys/utsname.h>
#   include	<netdb.h>

#   include	<appDebugon.h>

#   ifdef	NeXT
#	include	<libc.h>

#	if HAVE_UNAME
#	else
#	    define USE_GETHOSTNAME
#	endif
#   endif

#   ifdef __VMS
#	define	HAVE_PW_GECOS	0
#   else
#	define	HAVE_PW_GECOS	1
#   endif

/************************************************************************/
/*									*/
/*  Forward declarations						*/
/*									*/
/************************************************************************/

static int sioSmtpSayRecipients(	int			fd,
					const char *		recipients,
					void *			through,
					APP_COMPLAIN		complain );

static int mmWriteHeader(	int			fd,
				const char *		name,
				const char *		value,
				void *			through,
				APP_COMPLAIN		complain );

static int mmWrite(		int			fd,
				const char *		buf,
				unsigned int		len,
				void *			through,
				APP_COMPLAIN		complain );

static int mmWriteCRLF(		int			fd,
				const char *		buf,
				void *			through,
				APP_COMPLAIN		complain );

static int sioOutSmtpWriteBytes( void *			voidmc,
				const unsigned char *	buf,
				int			len );

static int sioOutSmtpSeek(	void *			voidmc,
				long			pos );

static int sioOutSmtpClose(	void *			voidmc );

static int mmReadCRLF(	int			fd,
			char *			buf,
			void *			through,
			APP_COMPLAIN		complain );

typedef struct MailContext
    {
    int			mcFd;
    int			mcAtBeginOfLine;
    void *		mcThrough;
    APP_COMPLAIN	mcComplain;
    } MailContext;

static char MMscratch[1024];

/************************************************************************/
/*									*/
/*  Abort an Smtp connection.						*/
/*									*/
/************************************************************************/

static void sioOutSmtpAbortMessage(	MailContext *		mc,
					void *			through,
					APP_COMPLAIN		complain )
    {
    if  ( mmWriteCRLF( mc->mcFd, "RSET", through, complain ) )
	{ LDEB(1); return;	}

    if  ( mmReadCRLF( mc->mcFd, MMscratch, through, complain ) )
	{ LDEB(1); return;	}

    if  ( mmWriteCRLF( mc->mcFd, "QUIT", through, complain ) )
	{ LDEB(1); return;	}

    if  ( mmReadCRLF( mc->mcFd, MMscratch, through, complain ) )
	{ LDEB(1); return;	}

    return;
    }


/************************************************************************/
/*									*/
/*  respond to a challenge from the mail server.			*/
/*									*/
/************************************************************************/

static int sioOutSmtpChallengeResponse(	MailContext *		mc,
					const char *		response,
					int			responseLen,
					void *			through,
					APP_COMPLAIN		complain )
    {
    int				rval= 0;

    SimpleOutputStream *	sosmb= (SimpleOutputStream *)0;
    SimpleOutputStream *	sosb64= (SimpleOutputStream *)0;
    MemoryBuffer		mb64;

    utilInitMemoryBuffer( &mb64 );

    sosmb= sioOutMemoryOpen( &mb64 );
    if  ( ! sosmb )
	{ XDEB(sosmb); rval= -1; goto ready; }
    sosb64= sioOutBase64Open( sosmb );
    if  ( ! sosb64 )
	{ XDEB(sosb64); rval= -1; goto ready; }

    sioOutWriteBytes( sosb64, (const unsigned char *)response, responseLen );

    sioOutClose( sosb64 ); sosb64= (SimpleOutputStream *)0;
    sioOutClose( sosmb ); sosmb= (SimpleOutputStream *)0;

    if  ( mmWriteCRLF( mc->mcFd, (char *)mb64.mbBytes, through, complain ) )
	{ LDEB(1); rval= -1;	}

  ready:
    if  ( sosb64 )
	{ sioOutClose( sosb64 );	}
    if  ( sosmb )
	{ sioOutClose( sosmb );		}

    utilCleanMemoryBuffer( &mb64 );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Open a connection to an SMTP server and send the mail headers	*/
/*									*/
/************************************************************************/

SimpleOutputStream * sioOutSmtpOpen(	const char *	mailHost,
					const char *	mailPort,
					const char *	user,
					const char *	pass,
					const char *	from,
					const char *	to,
					const char *	cc,
					const char *	bcc,
					const char *	subject,
					const char *	typeSlashSubtype,
					const char *	mimeBoundary,
					void *		through,
					APP_COMPLAIN	complain )
    {
    MailContext *		mc= (MailContext *)0;
    const char *		past;
    const char *		start;
    const char *		nodename= (const char *)0;

    int				haveEsmtp= 1;
    int				haveAuth= 0;
    int				haveAuthLogin= 0;
    int				haveAuthPlain= 0;

    SimpleOutputStream *	sos;

#   if HAVE_UNAME
    struct utsname		un;
#   endif

#   ifdef USE_GETHOSTNAME
    char			node_scratch[256+1];
#   endif

    if  ( ! from || ! from[0] )
	{
	XDEB(from);
	(*complain)( through, APP_SYSTEMeFROM, (char *)0 );
	goto mmError;
	}

    if  ( ! to || ! to[0] )
	{
	XDEB(to);
	(*complain)( through, APP_SYSTEMeRCPT, (char *)0 );
	goto mmError;
	}

    if  ( ! (mc= (MailContext *)malloc( sizeof( MailContext ) )) )
	{
	XDEB(mc);
	(*complain)( through, APP_SYSTEMeNOMEM, (char *)0 );
	goto mmError;
	}

    mc->mcFd= -1;
    mc->mcAtBeginOfLine= 1;
    mc->mcThrough= through;
    mc->mcComplain= complain;

    /********************************************************************/
    /*  Open connection							*/
    /********************************************************************/

    if  ( ! mailHost || ! mailHost[0] )
	{ mailHost= "mailhost";	}
    if  ( ! mailPort || ! mailPort[0] )
	{ mailPort= "smtp";	}

    mc->mcFd= appOpenSocket( mailHost, mailPort, through, complain );
    if  ( mc->mcFd < 0 )
	{ LDEB(mc->mcFd); goto mmError;	}

    do  {
	if  ( mmReadCRLF( mc->mcFd, MMscratch, through, complain ) )
	    { LDEB(1); goto mmError;				}

	if  ( strncmp( MMscratch, "220", 3 ) )
	    {
	    SDEB(MMscratch);
	    (void) (*complain)( through, APP_SYSTEMeSMTP, MMscratch );
	    goto mmError;
	    }
	} while( MMscratch[3] != ' ' );

    /********************************************************************/
    /*  Say hello							*/
    /********************************************************************/
#   if HAVE_UNAME
    if  ( uname( &un ) < 0 )
	{ LDEB(1); goto mmError;				}
    nodename= un.nodename;
#   endif

#   ifdef USE_GETHOSTNAME
    if  ( gethostname( node_scratch, sizeof(node_scratch)- 1 ) )
	{ LDEB(1); goto mmError;				}

    node_scratch[sizeof(node_scratch)-1]= '\0';
    nodename= node_scratch;
#   endif

    if  ( ! nodename )
	{ XDEB(nodename); goto mmError;	}

    sprintf( MMscratch, "EHLO %s", nodename );
    if  ( mmWriteCRLF( mc->mcFd, MMscratch, through, complain ) )
	{ LDEB(1); goto mmError;				}

    do  {
	char *	head;
	char *	tail;

	if  ( mmReadCRLF( mc->mcFd, MMscratch, through, complain ) )
	    { LDEB(1); goto mmError;				}

	if  ( strncmp( MMscratch, "250", 3 ) )
	    {
	    if  ( strncmp( MMscratch, "500", 3 )	&&
		  strncmp( MMscratch, "502", 3 )	&&
		  strncmp( MMscratch, "554", 3 )	)
		{
		SDEB(MMscratch);
		(void) (*complain)( through, APP_SYSTEMeSMTP, MMscratch );
		goto mmError;
		}
	    else{ haveEsmtp= 0;	}
	    }

	head= tail= MMscratch+ 4;
	while( *tail && ! isspace( *tail ) )
	    {
	    if  ( islower( *tail ) )
		{ *tail= toupper( *tail );	}
	    tail++;
	    }

	if  ( tail- head == 4 && ! strncmp( head, "AUTH", 4 ) )
	    {
	    haveAuth= 1;

	    while( *tail && isspace( *tail ) )
		{ tail++;	}
	    head= tail;
	    while( *head )
		{
		while( *tail && ! isspace( *tail ) )
		    {
		    if  ( islower( *tail ) )
			{ *tail= toupper( *tail );	}
		    tail++;
		    }

		if  ( tail- head == 5 && ! strncmp( head, "LOGIN", 5 ) )
		    { haveAuthLogin= 1;	}
		if  ( tail- head == 5 && ! strncmp( head, "PLAIN", 5 ) )
		    { haveAuthPlain= 1;	}

		while( *tail && isspace( *tail ) )
		    { tail++;	}
		head= tail;
		}
	    }

	} while( MMscratch[3] != ' ' );

    if  ( ! haveEsmtp )
	{
	sprintf( MMscratch, "HELO %s", nodename );
	if  ( mmWriteCRLF( mc->mcFd, MMscratch, through, complain ) )
	    { LDEB(1); goto mmError;				}

	do  {
	    if  ( mmReadCRLF( mc->mcFd, MMscratch, through, complain ) )
		{ LDEB(1); goto mmError;				}

	    if  ( strncmp( MMscratch, "250", 3 ) )
		{
		SDEB(MMscratch);
		(void) (*complain)( through, APP_SYSTEMeSMTP, MMscratch );
		goto mmError;
		}
	    } while( MMscratch[3] != ' ' );
	}

    /********************************************************************/
    /*  Login into the server.						*/
    /********************************************************************/
    if  ( user && pass && haveAuthLogin )
	{
	sprintf( MMscratch, "AUTH LOGIN" );
	if  ( mmWriteCRLF( mc->mcFd, MMscratch, through, complain ) )
	    { LDEB(1); goto mmError;				}

	for (;;)
	    {
	    if  ( mmReadCRLF( mc->mcFd, MMscratch, through, complain ) )
		{ LDEB(1); goto mmError;				}

	    if  ( ! strncmp( MMscratch, "235", 3 ) )
		{ break;	}

	    if  ( strncmp( MMscratch, "334", 3 ) )
		{
		SDEB(MMscratch);
		(void) (*complain)( through, APP_SYSTEMeSMTP, MMscratch );
		goto mmAbort;
		}

	    /*  Username: */
	    if  ( ! strcmp( MMscratch+ 4, "VXNlcm5hbWU6" ) )
		{
		if  ( sioOutSmtpChallengeResponse( mc,
						user, strlen( user ),
						through, complain ) )
		    { LDEB(1);  goto mmAbort;	}

		continue;
		}

	    /*  Password: */
	    if  ( ! strcmp( MMscratch+ 4, "UGFzc3dvcmQ6" ) )
		{
		if  ( sioOutSmtpChallengeResponse( mc,
						pass, strlen( pass ),
						through, complain ) )
		    { LDEB(1);  goto mmAbort;	}

		continue;
		}

	    SDEB(MMscratch); goto mmAbort;
	    }
	}

    /********************************************************************/
    /*  Say sender							*/
    /********************************************************************/
    if  ( (start= strchr( from, '<' )) && ( past= strchr( start, '>' ) ) )
	{ start++;						}
    else{ start= from; past= from+ strlen( from );		}

    sprintf( MMscratch, "MAIL FROM:<%.*s>", (int)( past- start ), start );
    if  ( mmWriteCRLF( mc->mcFd, MMscratch, through, complain ) )
	{ LDEB(1); goto mmAbort;				}

    do  {
	if  ( mmReadCRLF( mc->mcFd, MMscratch, through, complain ) )
	    { LDEB(1); goto mmAbort;				}

	if  ( strncmp( MMscratch, "250", 3 ) )
	    {
	    SDEB(MMscratch);
	    (void) (*complain)( through, APP_SYSTEMeSMTP, MMscratch );
	    goto mmAbort;
	    }
	} while( MMscratch[3] != ' ' );

    /********************************************************************/
    /*  Say recipient(s)						*/
    /********************************************************************/

    if  ( sioSmtpSayRecipients( mc->mcFd, to, through, complain ) )
	{ LDEB(1); goto mmAbort;	}

    if  ( cc								&&
	  cc[0]								&&
	  sioSmtpSayRecipients( mc->mcFd, cc, through, complain )	)
	{ LDEB(1); goto mmAbort;	}

    if  ( bcc								&&
	  bcc[0]							&&
	  sioSmtpSayRecipients( mc->mcFd, bcc, through, complain )	)
	{ LDEB(1); goto mmAbort;	}

    /********************************************************************/
    /*  Say DATA							*/
    /********************************************************************/
    if  ( mmWriteCRLF( mc->mcFd, "DATA", through, complain ) )
	{ LDEB(1); goto mmAbort;				}

    do  {
	if  ( mmReadCRLF( mc->mcFd, MMscratch, through, complain ) )
	    { LDEB(1); goto mmAbort;				}

	if  ( strncmp( MMscratch, "354", 3 ) )
	    {
	    SDEB(MMscratch);
	    (void) (*complain)( through, APP_SYSTEMeSMTP, MMscratch );
	    goto mmAbort;
	    }
	} while( MMscratch[3] != ' ' );

    /********************************************************************/
    /*  Write headers							*/
    /********************************************************************/
    if  ( mmWriteHeader( mc->mcFd, "From", from, through, complain )	||
	  mmWriteHeader( mc->mcFd, "To", to, through, complain )	||
	  mmWriteHeader( mc->mcFd, "Cc", cc, through, complain )	||
	  mmWriteHeader( mc->mcFd, "Subject", subject,
					    through, complain )		)
	{ LDEB(1); goto mmAbort;	}
	
    if  ( mimeBoundary )
	{
	const char *	mimeVersion= "MIME-Version";

	const char *	name= "Content-Type";
	int		len= strlen( name );
	const char *	name2= " boundary=\"";

	if  ( mmWriteHeader( mc->mcFd, mimeVersion, "1.0",
						through, complain )	||
	      mmWrite( mc->mcFd, name, len, through, complain )		||
	      mmWrite( mc->mcFd, ": ", 2, through, complain )		||
	      mmWrite( mc->mcFd, typeSlashSubtype,
					strlen( typeSlashSubtype ),
					through, complain )		||
	      mmWriteCRLF( mc->mcFd, ";", through, complain )		)
	    { LDEB(1); goto mmAbort;	}

	len= strlen( name2 );
	if  ( mmWrite( mc->mcFd, name2, len, through, complain )	||
	      mmWrite( mc->mcFd, mimeBoundary,
				strlen( mimeBoundary ),
				through, complain )			||
	      mmWriteCRLF( mc->mcFd, "\"",  through, complain )		)
	    { LDEB(1); goto mmAbort;	}
	}
    else{
	if  ( mmWriteHeader( mc->mcFd, "Content-Type", typeSlashSubtype,
						    through, complain )	)
	    { LDEB(1); goto mmAbort;	}
	}

    if  ( mmWriteCRLF( mc->mcFd, "\n", through, complain )		)
	{ LDEB(1); goto mmAbort;	}

    sos= sioOutOpen( (void *)mc, sioOutSmtpWriteBytes,
					    sioOutSmtpSeek, sioOutSmtpClose );

    if  ( ! sos )
	{ XDEB(sos); goto mmAbort;	}

    return sos;

  mmAbort:
    sioOutSmtpAbortMessage( mc, through, complain );

  mmError:

    if  ( mc )
	{
	if  ( mc->mcFd != -1 )
	    { close( mc->mcFd );				}
	free( (char *)mc );
	}

    return (SimpleOutputStream *)0;
    }

/************************************************************************/
/*									*/
/*  Refuse to seek.							*/
/*									*/
/************************************************************************/

static int sioOutSmtpSeek(	void *	voidmc,
				long	pos )
    { LDEB(pos); return -1;	}

/************************************************************************/
/*									*/
/*  Send the next part of the mail body					*/
/*									*/
/************************************************************************/

static int sioOutSmtpWriteBytes(	void *			voidmc,
					const unsigned char *	buf,
					int			len )
    {
    static char			crlf[]	= "\015\012";
    MailContext *		mc	= (MailContext *)voidmc;
    const unsigned char *	start	= buf;
    const unsigned char *	past;
    unsigned int		todo	= len;

    while( todo > 0 )
	{
	past= (const unsigned char *)memchr( start, '\n', todo );
	if  ( ! past )
	    { todo-= past- start;				}
	else{ past= start+ todo; todo= 0;			}

	if  ( past > start )
	    {
	    if  ( mmWrite( mc->mcFd, (const char *)start, past - start,
					mc->mcThrough, mc->mcComplain ) )
		{ LDEB(1); return -1;				}
	    }

	if  ( todo > 0 )
	    {
	    if  ( mmWrite( mc->mcFd, crlf, 2,
					mc->mcThrough, mc->mcComplain ) )
		{ LDEB(1); return -1;				}
	    todo--; past++;
	    mc->mcAtBeginOfLine= 1;
	    }
	else{
	    mc->mcAtBeginOfLine= 0;
	    }

	start= past;
	}

    return (int)len;
    }

/************************************************************************/
/*									*/
/*  Finish sending the mail body and close the connection		*/
/*									*/
/************************************************************************/

static int sioOutSmtpClose(	void *		voidmc )
    {
    int			rval= 0;
    MailContext *	mc= (MailContext *)voidmc;

    if  ( ! mc->mcAtBeginOfLine						&&
	  mmWriteCRLF( mc->mcFd, "\n", mc->mcThrough, mc->mcComplain )	)
	{ LDEB(1); rval= -1;	}

    if  ( ! rval							&&
	  mmWriteCRLF( mc->mcFd, ".\nQUIT\n", mc->mcThrough, mc->mcComplain ) )
	{ LDEB(1); rval= -1;	}

    close( mc->mcFd );
    free( (char *)mc );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Write a list of recipients to the SMTP connection			*/
/*									*/
/*  1)  Look for the end of this recipient.				*/
/*  2)  Distinguish between <name> and name.				*/
/*  3)  Tell the SMTP server about the recipient.			*/
/*  4)  Get its answer.							*/
/*									*/
/************************************************************************/

static int sioSmtpSayRecipients(	int			fd,
					const char *		recipients,
					void *			through,
					APP_COMPLAIN		complain )
    {
    int	ok= 0;

    while( recipients[0] == ' ' )
	{ recipients++;	}

    while( recipients[0] )
	{
	const char *		comma;
	const char *		start;
	const char *		past;

	/*  1  */
	comma= strchr( recipients, ',' );
	if  ( ! comma )
	    { comma= recipients+ strlen( recipients );	} /*lie*/

	/*  2  */
	start= recipients;
	while( *start && start < comma && *start != '<' )
	    { start++; }

	if  ( *start == '<' )
	    {
	    start++; past= start;
	    while( *past && past < comma && *past != '>' )
		{ past++; }
	    }
	else{ start= recipients; past= comma;	}

	/*  3  */
	sprintf( MMscratch, "RCPT TO:<%.*s>", (int)(past- start), start );
	if  ( mmWriteCRLF( fd, MMscratch, through, complain ) )
	    { LDEB(1); return -1;				}

	/*  4  */
	do  {
	    if  ( mmReadCRLF( fd, MMscratch, through, complain ) )
		{ LDEB(1); return -1;				}

	    if  ( ! strncmp( MMscratch, "250", 3 ) )
		{ ok++;						}
	    else{ (void) (*complain)( through, APP_SYSTEMeSMTP, MMscratch ); }
	    } while( MMscratch[3] != ' ' );

	if  ( *comma == ',' )
	    { comma++;	}

	recipients= comma;
	while( recipients[0] == ' ' )
	    { recipients++;	}
	}

    return (ok > 0)?0:-1;
    }	

/************************************************************************/
/*									*/
/*  SMTP connection read and write utilities				*/
/*									*/
/************************************************************************/

static int mmReadCRLF(	int			fd,
			char *			buf,
			void *			through,
			APP_COMPLAIN		complain )
    {
    static char smallbuf[2];
    int		nbytes= 0;

    for (;;)
	{
	int	done= read( fd, smallbuf, 1 );

	if  ( done != 1 )
	    {
	    LSDEB(done,strerror(errno));
	    (*complain)( through, APP_SYSTEMeREAD, (char *)0 );
	    return -1;
	    }

	nbytes += done;

	*buf= *smallbuf;

	if  ( nbytes >= 2 && *buf == 10 && *(buf-1) == 13 )
	    { *(buf-1)= 0; return 0;	}

	buf++;
	}
    }

static int mmWriteCRLF(	int			fd,
			const char *		buf,
			void *			through,
			APP_COMPLAIN		complain )
    {
    static char		crlf[]	= "\015\012";

    const char *	start	= buf;
    const char *	past;

    while( *start )
	{
	past= strchr( start, '\n' );
	if  ( ! past )
	    { past= start + strlen( start );			}

	if  ( past > start )
	    {
	    if  ( mmWrite( fd, start, past - start, through, complain ) )
		{ LDEB(1); return -1;				}
	    }
	if  ( mmWrite( fd, crlf, 2, through, complain ) )
	    { LDEB(1); return -1;				}

	start= (*past) ? past + 1 : past;
	}

    return 0;
    }

static int mmWriteHeader(	int			fd,
				const char *		name,
				const char *		value,
				void *			through,
				APP_COMPLAIN		complain )
    {
    unsigned int	len;

    if  ( ! value || ! value[0] )
	{ return 0;						}

    len= strlen( name );
    if  ( mmWrite( fd, name, len, through, complain )	||
	  mmWrite( fd, ": ", 2, through, complain )	||
	  mmWriteCRLF( fd, value, through, complain )	)
	{ LDEB(1); return -1;					}

    return 0;
    }

static int mmWrite(	int			fd,
			const char *		buf,
			unsigned int		len,
			void *			through,
			APP_COMPLAIN		complain )
    {
    int		done= write( fd, buf, len );

    if  ( done != (int)len )
	{
	LLDEB(len,done); SDEB(strerror(errno));
	(*complain)( through, APP_SYSTEMeWRITE, (char *)0 );
	return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Return a qualified guess about the callers mail address.		*/
/*									*/
/************************************************************************/

char *	sioSmtpGuessMailAddress( void )
    {
    static char *	rval;
    char *		to;

    const char *	domainname= (const char *)0;
    const char *	fullhost= (const char *)0;
    const char *	nodename= (const char *)0;
    int			l;

    int			didFullName= 0;

    struct passwd *	pwd;

#   if HAVE_UNAME
    struct utsname	un;
#   endif

#   ifdef USE_GETHOSTNAME
    char			node_scratch[256+1];
#   endif

    if  ( rval )
	{ return rval;	}

#   if HAVE_UNAME
    if  ( uname( &un ) < 0 )
	{ LDEB(1); return (char *)0;	}
    nodename= un.nodename;
#   endif

#   ifdef USE_GETHOSTNAME
    if  ( gethostname( node_scratch, sizeof(node_scratch)- 1 ) )
	{ LDEB(1); return (char *)0;		}

    node_scratch[sizeof(node_scratch)-1]= '\0';
    nodename= node_scratch;
#   endif

    if  ( ! nodename )
	{ XDEB(nodename); return (char *)0;	}

#   ifdef  UTSNAME_DOMAIN
    if  ( un.UTSNAME_DOMAIN[0] && strcmp( un.UTSNAME_DOMAIN, "(none)" ) )
	{ domainname= un.UTSNAME_DOMAIN;	}
#   endif

    if  ( ! domainname )
	{
	if  ( strchr( nodename, '.' ) )
	    { fullhost= nodename; }
	else{
	    struct hostent *	host= gethostbyname( nodename );

	    l= strlen( nodename );
	    if  ( host )
		{
		char **		a= host->h_aliases;

		if  ( ! strncmp( nodename, host->h_name, l )	&&
		      host->h_name[l] == '.'			)
		    { fullhost= host->h_name;	}
		else{
		    if  ( a ) while( *a )
			{
			if  ( ! strncmp( nodename, *a, l )	&&
			      (*a)[l] == '.'			)
			    { fullhost= *a; break;	}

			a++;
			}
		    }

		if  ( ! fullhost )
		    {
		    a= host->h_aliases;

		    if  ( strchr( host->h_name, '.' ) )
			{ fullhost= host->h_name;	}
		    else{
			if  ( a ) while( *a )
			    {
			    if  ( strchr( *a, '.' ) )
				{ fullhost= *a; break;	}

			    a++;
			    }
			}
		    }
		}

	    if  ( ! fullhost )
		{ fullhost= nodename;	}
	    }
	}

    if  ( ! domainname && ! fullhost )
	{ XXDEB(domainname,fullhost); return (char *)0;	}

    pwd= getpwuid( getuid() );
    if  ( ! pwd )
	{ LXDEB(getuid(),pwd); return (char *)0; }

    l= 0;
    l += strlen( pwd->pw_name );
#   if HAVE_PW_GECOS
    if  ( pwd->pw_gecos && *pwd->pw_gecos )
	{ l += strlen( pwd->pw_gecos )+ 3;	}
#   endif

    if  ( domainname )
	{
	l += strlen( nodename );
	l += 1;
	l += strlen( domainname );
	}
    else{ l += strlen( fullhost ); }

    rval= (char *)malloc( l+ 2 );
    if  ( ! rval )
	{ XDEB(rval); return (char *)0;	}

    to= rval; *to= '\0';
#   if HAVE_PW_GECOS
    if  ( pwd->pw_gecos && *pwd->pw_gecos )
	{
	char *	s;
	char *	firstAlnum;
	char *	pastAlnum;

	firstAlnum= pwd->pw_gecos;
	while( *firstAlnum && ! isalnum( *firstAlnum ) )
	    { firstAlnum++;	}
	s= pastAlnum= firstAlnum;
	while( *s )
	    {
	    if  ( isalnum( *s ) )
		{ pastAlnum= s+ 1;	}
	    s++;
	    }

	if  ( pastAlnum > firstAlnum )
	    {
	    memcpy( to, firstAlnum, pastAlnum- firstAlnum );
	    to +=  pastAlnum- firstAlnum;
	    strcpy( to, " <" ); to += 2;

	    didFullName= 1;
	    }

	}
#   endif
    strcpy( to, pwd->pw_name ); to += strlen( to );
    strcpy( to, "@" ); to += strlen( to );
    if  ( domainname )
	{
	strcpy( to, nodename ); to += strlen( to );
	strcpy( to, "." ); to += strlen( to );
	strcpy( to, domainname ); to += strlen( to );
	}
    else{ strcpy( to, fullhost ); to += strlen( to );	}

    if  ( didFullName )
	{ strcpy( to, ">" ); to += strlen( to ); }

    return rval;
    }
