/************************************************************************/
/*									*/
/*  Manage a lookup to keep track of text attributes.			*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_TEXT_ATTRIBUTE_ADMIN_H
#   define	UTIL_TEXT_ATTRIBUTE_ADMIN_H

#   include	<utilTextAttribute.h>

typedef struct IntegerValueNode
    {
    unsigned int			ivnIsLeaf:1;
    unsigned int			ivnChildCount:31;

    union
	{
	struct IntegerValueNode *	u_ivnChildren;
	int				u_ivnReference;
	} ivn_u;
#   define ivnChildren	ivn_u.u_ivnChildren
#   define ivnReference	ivn_u.u_ivnReference

    } IntegerValueNode;

typedef struct TextAttributeList
    {
    int			talCount;
    TextAttribute **	talPages;
    IntegerValueNode	talValueNodes;
    } TextAttributeList;

typedef void (*TextAttributeFunction)(	const TextAttribute *	ta,
					int			n,
					void *			through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void utilInitIntegerValueNode(	IntegerValueNode *	tal );

extern void utilCleanIntegerValueNode(	IntegerValueNode *	tal );

extern void utilInitTextAttributeList(	TextAttributeList *	tal );

extern void utilCleanTextAttributeList(	TextAttributeList *	tal );

extern int utilTextAttributeNumber(	TextAttributeList *	tal,
					const TextAttribute *	ta );

extern void utilGetTextAttributeByNumber(
					TextAttribute *			ta,
					const TextAttributeList *	tal,
					int				n );

extern void utilForAllTextAttributes(
				const TextAttributeList *	tal,
				TextAttributeFunction		f,
				void *				through );

#   endif	/*  UTIL_TEXT_ATTRIBUTE_ADMIN_H	*/
