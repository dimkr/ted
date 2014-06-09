/* bitmapConfig.h.  Generated automatically by configure.  */
#   ifndef	BITMAP_CONFIG_H
#   define	BITMAP_CONFIG_H

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

#define SIZEOF_SHORT 2
#define SIZEOF_INT 4
#define SIZEOF_LONG 4
#define SIZEOF_UNSIGNED_SHORT 2
#define SIZEOF_UNSIGNED_INT 4
#define SIZEOF_UNSIGNED_LONG 4

#   if SIZEOF_UNSIGNED_SHORT == 2
	typedef unsigned short	BmUint16;
#   else
#	if SIZEOF_UNSIGNED_INT == 2
	typedef unsigned int	BmUint16;
#	else
	What else can be a 16 bit integer?
#	endif
#   endif

#   if SIZEOF_UNSIGNED_INT == 4
	typedef unsigned int	BmUint32;
#   else
#	if SIZEOF_UNSIGNED_LONG == 4
	typedef unsigned long	BmUint32;
#	else
	What else can be a 32 bit integer?
#	endif
#   endif

/************************************************/
/*  Libraries that are (Un)Available.		*/
/************************************************/
#   define	JPEG_FOUND 1
#   define	TIFF_FOUND 1
#   define	PNG_FOUND 1
#   define	XPM_FOUND 1

#   endif	/*  BITMAP_CONFIG_H  */
