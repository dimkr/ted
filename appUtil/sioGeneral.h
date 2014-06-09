/************************************************************************/
/*									*/
/*  Simplification of stdio.						*/
/*									*/
/************************************************************************/

#   ifndef	SIO_GENERAL_H
#   define	SIO_GENERAL_H


#   include	<stdio.h>		/*  for EOF.			*/

#   define	SIOsizBUF		1024

typedef int	(*SIOinREADBYTES)( void *, unsigned char *, int );
typedef int	(*SIOinCLOSE)( void * );

typedef struct SimpleInputStream
    {
    unsigned char	sisBuffer[SIOsizBUF];
    unsigned char *	sisP;
    int			sisN;
    void *		sisPrivate;

    long		sisReadUpto;

    SIOinREADBYTES	sisReadBytes;
    SIOinCLOSE		sisClose;
    } SimpleInputStream;

#   define	sioInGetCharacter(s)			\
		    ( (--((s)->sisN)) >= 0	?	\
		      *(((s)->sisP)++)		:	\
		      sioInFillBuffer(s)	)

typedef int	(*SIOoutWRITEBYTES)( void *, const unsigned char *, int );
typedef int	(*SIOoutCLOSE)( void * );
typedef int	(*SIOoutSEEK)( void *, long );

typedef struct SimpleOutputStream
    {
    unsigned char	sosBuffer[SIOsizBUF];
    unsigned char *	sosP;
    int			sosN;
    void *		sosPrivate;

    SIOoutWRITEBYTES	sosWriteBytes;
    SIOoutSEEK		sosSeek;
    SIOoutCLOSE		sosClose;
    } SimpleOutputStream;

#   define	sioOutPutCharacter(c,s)		\
		( ( (s)->sosN >= SIOsizBUF && sioOutFlushBuffer(s) ) ? -1 : \
		  ( *(((s)->sosP)++)= (c), (s)->sosN++, 0 ) )

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

/*  in  */

extern int sioInFillBuffer(	SimpleInputStream *	sis );

extern int sioInUngetLastRead(	SimpleInputStream *	sis );

extern SimpleInputStream * sioInOpen(	void *			specific,
					SIOinREADBYTES		readBytes,
					SIOinCLOSE		closeIt );

extern int sioInClose(	SimpleInputStream *		sis );

extern char * sioInGetString(	char *			s,
				int			size,
				SimpleInputStream *	sis );

extern int sioInReadBytes(	SimpleInputStream *	sis,
				unsigned char *		buf,
				int			count );

/*  out  */

extern int sioOutFlushBuffer(	SimpleOutputStream *	sos );

extern SimpleOutputStream * sioOutOpen(	void *			specific,
					SIOoutWRITEBYTES	writeBytes,
					SIOoutSEEK		seekTo,
					SIOoutCLOSE		closeIt );

extern int sioOutSeek(	SimpleOutputStream *		sos,
			long				pos );

extern int sioOutClose(	SimpleOutputStream *		sos );

extern int sioOutPutString(	const char *		s,
				SimpleOutputStream *	sos );

extern int sioOutWriteBytes(	SimpleOutputStream *	sos,
				const unsigned char *	buf,
				int			count );

#ifdef	__GNUC__
extern int sioOutPrintf(	SimpleOutputStream *	sos,
				const char *		format,
				... )
				__attribute__ (( format ( printf, 2, 3 ) ));
#else
extern int sioOutPrintf(	SimpleOutputStream *	sos,
				const char *		format,
				... );
#endif

#   endif
