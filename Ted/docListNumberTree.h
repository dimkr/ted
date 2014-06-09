/************************************************************************/
/*									*/
/*  Manage the numbers in bulleted and numbered lists.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LIST_NUMBER_TREE_H
#   define	DOC_LIST_NUMBER_TREE_H

typedef struct ListNumberTreeNode
    {
    int				lntnParagraphNumber;
    int				lntnLeafCount;
    struct ListNumberTreeNode *	lntnChildren;
    int				lntnChildCount;
    unsigned char		lntnIsLeaf;
    } ListNumberTreeNode;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docListNumberTreeInsertParagraph(
					ListNumberTreeNode *	root,
					int			ilvl0,
					int			paraNr );

extern int docListNumberTreeDeleteParagraph(
					ListNumberTreeNode *	root,
					int			paraNr );

extern int docListNumberTreeGetNumberPath(	
					int *			numberPath,
					ListNumberTreeNode *	root,
					int			ilvl,
					int			paraNr );

extern int docListNumberTreeGetPrevPath(int *			numberPath,
					int *			pLevel,
					ListNumberTreeNode *	root,
					int			paraNr );

extern void docFreeListNumberNodes(	ListNumberTreeNode *	nodes,
					int			count );

extern int docClaimListNumberTreeNodes(	ListNumberTreeNode **	pLntn,
					int *			pCount,
					int			count );

extern void docShiftListTreeReferences(	ListNumberTreeNode *	root,
					int			paraFrom,
					int			paraShift );

extern void docLogListNumberTreeNode(	int				indent,
					const ListNumberTreeNode *	lntn );

#   endif
