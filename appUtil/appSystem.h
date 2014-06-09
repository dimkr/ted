/************************************************************************/
/*									*/
/*  Operating system related functionality.				*/
/*									*/
/************************************************************************/

#   ifndef	APP_SYSTEM_H
#   define	APP_SYSTEM_H

typedef int (*FILE_CALLBACK)	(		const char *	name,
						void *		through );

typedef void (*APP_COMPLAIN)(		void *		through,
					int		errorId,
					const char *	errorSubject );

/************************************************************************/
/*									*/
/*  Error numbers. (Can be transtaled to strings by the caller.)	*/
/*									*/
/************************************************************************/

#	define APP_SYSTEMeNOERROR		0
#	define APP_SYSTEMeNOMEM			1
#	define APP_SYSTEMeFROM			2
#	define APP_SYSTEMeRCPT			3
#	define APP_SYSTEMeSMTP			4
#	define APP_SYSTEMeWRITE			5
#	define APP_SYSTEMeREAD			6
#	define APP_SYSTEMeUNAME			7
#	define APP_SYSTEMeHOST			8
#	define APP_SYSTEMeSERV			9
#	define APP_SYSTEMeSOCK			10
#	define APP_SYSTEMeCONN			11
#	define APP_SYSTEMeBIND			12
#	define APP_SYSTEMeLISTEN		13
#	define APP_SYSTEMeACCEPT		14

#	define APP_SYSTEMeCOUNT			15

/************************************************************************/
/*									*/
/*  Operating system related functionality.. Declarations.		*/
/*									*/
/************************************************************************/

extern int appHomeDirectory(	char *		home,
				int		len );

extern int appCurrentDirectory(	char *		pwd,
				int		len );

extern int appTestDirectory(	const char *	dir );
extern int appTestFileWritable( const char *	file );
extern int appTestFileExists( const char *	file );
extern int appTestFileReadable( const char *	file );

extern int appMakeDirectory(	const char *	dir );

extern long appGetTimestamp( void );

extern int appMakeUniqueString(	char *		target,
				int		maxlen );

extern int appAbsoluteName(	char *		absolute,
				int		len,
				const char *	filename,
				int		relativeIsFile,
				const char *	nameRelativeTo );

extern int appRemoveFile(	const char *	filename );

extern int appForAllFiles(	const char *		dir,
				const char *		ext,
				void *			through,
				FILE_CALLBACK		callback );

extern int appOpenSocket(	const char *		hostName,
				const char *		portName,
				void *			through,
				APP_COMPLAIN		complain );
extern int appListenSocket(	const char *		portName,
				unsigned int *		pPort,
				void *			through,
				APP_COMPLAIN		complain );
extern int appAcceptSocket(	int			lfd,
				void *			through,
				APP_COMPLAIN		complain );

extern const char * appFileExtensionOfName(	const char *	filename );
extern const char * appRelativeName(		const char *	filename );

#   endif	/*  APP_SYSTEM_H	*/
