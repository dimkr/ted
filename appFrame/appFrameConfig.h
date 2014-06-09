/* appFrameConfig.h.  Generated automatically by configure.  */
/* config.h.in.  Generated automatically from configure.in by autoheader.  */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
/* #undef WORDS_BIGENDIAN */

#define HAVE_SOCKET 1
#define HAVE_MKSTEMP 1
#define HAVE_STRERROR 1
#define HAVE_UNAME 1

#define HAVE_FCNTL_H 1
#define HAVE_SYS_TIME_H 1
#define HAVE_UNISTD_H 1

/************************************************************************/
/*									*/
/*  Locations of application related files				*/
/*									*/
/*  AFMDIR:	Location of the font files: something.afm files and the	*/
/*		optional PostScript to X11 font translation table.	*/
/*  INDDIR:	Location of the spell check dictionaries.		*/
/*									*/
/************************************************************************/

#   define	PREFIX "/usr/local"

#   ifdef	PREFIX
#	define	PKGDIR	PREFIX
#   endif

#   ifndef	PKGDIR
#	define	PKGDIR	"/usr/local"
#   endif

#   ifndef	AFMDIR
#	define	AFMDIR	PKGDIR "/afm"
#   endif

#   ifndef 	INDDIR
#	define	INDDIR	PKGDIR "/ind"
#   endif

/************************************************************************/
/*									*/
/*  Choose a GUI. (only one!)						*/
/*									*/
/************************************************************************/

#   define	USE_MOTIF 1
/* #   undef	USE_GTK */
