/* tedConfig.h.  Generated automatically by configure.  */
#   ifndef TED_CONFIG_H
#   define TED_CONFIG_H

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
/* #undef WORDS_BIGENDIAN */

/* Define if you have the socket function.  */
#define HAVE_SOCKET 1

/* Define if you have the strerror function.  */
#define HAVE_STRERROR 1

/* Define if you have the uname function.  */
#define HAVE_UNAME 1

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <sys/time.h> header file.  */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1


#   define	MY_PLATFORM "Linux_i686"
#   define	MY_HOST_DATE "mdedoes Sun Jan 23 13:49:05 CET 2005"

/************************************************************************/
/*									*/
/*  Locations of application related files				*/
/*									*/
/*  DOCUMENT_DIR:	Location of the manual file.			*/
/*									*/
/************************************************************************/

#   define	PREFIX "/usr/local"

#   ifdef	PREFIX
#	define	PKGDIR	PREFIX
#   endif

#   ifndef	PKGDIR
#	define	PKGDIR	"/usr/local"
#   endif

#   ifdef __VMS
#	define	DOCUMENT_DIR	"TED$ROOT:[ted]"
#   endif

#   ifndef	DOCUMENT_DIR
#	define	DOCUMENT_DIR	PKGDIR "/Ted/"
#   endif

/************************************************************************/
/*									*/
/*  Choose a GUI. (only one!)						*/
/*									*/
/************************************************************************/

#   define	USE_MOTIF 1
/* #   undef	USE_GTK */

#   endif	/*	TED_CONFIG_H	*/
