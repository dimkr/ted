/************************************************************************/
/*									*/
/*  sioSmtp.[ch]:	Send an e-mail message using the SMTP protocol.	*/
/*  Author:		Robert A. Vonk.					*/
/*			( Adapted by Mark de Does to fit in the sio	*/
/*			framework. Also added elementary ESMTP AUTH	*/
/*			support.					*/
/*									*/
/*									*/
/*  SimpleOutputStream * sioOutSmtpOpen(char *	mailHost,		*/
/*					char *	mailPort,		*/
/*					char *	user,			*/
/*					char *	pass,			*/
/*					char *	From,			*/
/*					char *	To,			*/
/*					char *	Cc,			*/
/*					char *	Bcc,			*/
/*					char *	Subject,		*/
/*					char *	TypeSlashSubtype,	*/
/*					char *	mimeBoundary )		*/
/*	ARGUMENTS:							*/
/*	  mailHost:	The name of the SMTP server.			*/
/*			DEFAULT: "mailhost".				*/
/*	  mailPort:	The name of the service port for smtp.		*/
/*			DEFAULT: "smtp".				*/
/*	  user:		If not null, this is the user name to log in	*/
/*			into the smtp server. Only some AUTH variants	*/
/*			are supported.					*/
/*	  pass:		If not null, this is the password to log in	*/
/*			into the smtp server. Only some AUTH variants	*/
/*			are supported.					*/
/*			DEFAULT: no default: You must give a non empty	*/
/*	  From:		The e-mail address of the sender.		*/
/*			DEFAULT: no default: You must give a non empty	*/
/*			value.						*/
/*	  To:		The e-mail address(es) of the recipient(s).	*/
/*			DEFAULT: no default: You must give a non empty	*/
/*			value.						*/
/*	  Cc:		The e-mail address(es) of the persons that are	*/
/*			carbon copied.					*/
/*			DEFAULT: Do not send copies.			*/
/*	  Bcc:		The e-mail address(es) of the persons that are	*/
/*			blind carbon copied (do not appear in mail	*/
/*			headers).					*/
/*			DEFAULT: Do not send copies.			*/
/*	  Subject:	The subject of the mail message			*/
/*			DEFAULT: No subject.				*/
/*	  mimeType:	The MIME type of the mail message		*/
/*			DEFAULT: No mime type.				*/
/*	  mimeBoundary:	The MIME boundary used to separate parts of the	*/
/*			message.					*/
/*			DEFAULT: No mime boundary.			*/
/*	  through:	Pass through pointer to the error routine.	*/
/*	  complain:	Called to pass error information back to the	*/
/*			user. Only called when things go wrong.		*/
/*									*/
/*	RETURN VALUES:							*/
/*	  (SimpleOutputStream *)0:	On failure.			*/
/*	  (SimpleOutputStream *)ptr:	On success.			*/
/*									*/
/*									*/
/************************************************************************/

#   ifndef	SIO_SMTP_H
#   define	SIO_SMTP_H

#   include     <appSystem.h>
#   include     <sioGeneral.h>

extern SimpleOutputStream * sioOutSmtpOpen(	const char *	mailHost,
						const char *	mailPort,
						const char *	user,
						const char *	pass,
						const char *	from,
						const char *	to,
						const char *	cc,
						const char *	bcc,
						const char *	subject,
						const char *	typeSlashSub,
						const char *	mimeBoundary,
						void *		through,
						APP_COMPLAIN	complain );

extern char * sioSmtpGuessMailAddress( void );

#   endif
