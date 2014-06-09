/************************************************************************/
/*									*/
/*  An array of bytes and its (maximum) size.				*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_MEMORY_H
#   define	UTIL_MEMORY_H

typedef struct MemoryBuffer
    {
    int			mbSize;
    unsigned char *	mbBytes;
    } MemoryBuffer;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void utilInitMemoryBuffer(	MemoryBuffer *	mb );
extern void utilEmptyMemoryBuffer(	MemoryBuffer *	mb );
extern void utilCleanMemoryBuffer(	MemoryBuffer *	mb );

extern int utilSizeMemoryBuffer(	MemoryBuffer *	mb,
					int		size );

extern int utilSetMemoryBuffer(	MemoryBuffer *		mb,
				const unsigned char *	bytes,
				int			size );

extern int utilAddToMemoryBuffer(	MemoryBuffer *		mb,
					const unsigned char *	bytes,
					int			size );

extern int utilCopyMemoryBuffer(	MemoryBuffer *		mbTo,
					const MemoryBuffer *	mbFrom );

extern int utilMemoryCompareBuffers(	const MemoryBuffer *	mb1,
					const MemoryBuffer *	mb2 );

extern int utilMemoryAppendBuffer(	MemoryBuffer *		to,
					const MemoryBuffer *	from );

#   endif	/*  UTIL_MEMORY_H	*/
