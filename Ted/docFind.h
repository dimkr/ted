/************************************************************************/
/*									*/
/*  Definitions for finding patterns in the document and spelling	*/
/*  checking.								*/
/*									*/
/************************************************************************/

#   ifndef	DOC_FIND_H
#   define	DOC_FIND_H

#   include	"docBuf.h"
#   include	<reg.h>


typedef int (*PARA_FIND_STRING)(
			    DocumentSelection *		ds,
			    BufferItem *		paraBi,
			    const BufferDocument *	bd,
			    const DocumentPosition *	dpFrom,
			    void *			through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docFindParaFindNext(	DocumentSelection *		ds,
				BufferItem *			paraBi,
				const BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				void *				through );

extern int docFindParaFindPrev(	DocumentSelection *		ds,
				BufferItem *			paraBi,
				const BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				void *				through );

extern int docSpellParaFindNext(
				DocumentSelection *		ds,
				BufferItem *			paraBi,
				const BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				void *				through );

extern int docFindFindNextInDocument(
				DocumentSelection *		ds,
				const DocumentPosition *	dpFrom,
				BufferDocument *		bd,
				PARA_FIND_STRING		findNext,
				void *				through );

extern int docFindFindPrevInDocument(
				DocumentSelection *		ds,
				const DocumentPosition *	dpFrom,
				BufferDocument *		bd,
				PARA_FIND_STRING		findPrev,
				void *				through );

extern int docFindFindNextInTree( DocumentSelection *		ds,
				const BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				PARA_FIND_STRING		findNext,
				void *				through );

#   endif
