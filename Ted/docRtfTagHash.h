/************************************************************************/
/*									*/
/*  Hash functions for tags.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_RTF_TAG_HASH_H
#   define	DOC_RTF_TAG_HASH_H

extern unsigned long docRtfTagHash(	const unsigned char *	key,
					int			len );

extern int docRtfTagIndex(		const unsigned char *	key,
					int			len );

extern unsigned long docRtfShapePropHash(
					const unsigned char *	key,
					int			len );

extern int docRtfShapePropIndex(	const unsigned char *	key,
					int			len );

#    endif	/*  DOC_RTF_TAG_HASH_H	*/
