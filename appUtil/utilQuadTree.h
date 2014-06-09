/************************************************************************/
/*									*/
/*  Quad Tree implementation.						*/
/*									*/
/*  Some kind of balancing is achieved by dividing a rectangle in	*/
/*  quadrants. No attempt is made to balance the tree when the entries	*/
/*  are not evenly spaced.						*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_QUAD_TREE_H
#   define	UTIL_QUAD_TREE_H

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<stddef.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	"geo2DInteger.h"

typedef enum Quadrant
    {
    QTquadNE,
    QTquadNW,
    QTquadSW,
    QTquadSE,

    QTquad_COUNT
    } Quadrant;

typedef enum Octant
    {
    QToctENE,
    QToctNNE,
    QToctNNW,
    QToctWNW,
    QToctWSW,
    QToctSSW,
    QToctSSE,
    QToctESE,

    QToct_COUNT
    } Octant;

typedef struct QuadNode
    {
    int			qnX;
    int			qnY;

    struct QuadNode *	qn_parent;
    struct QuadNode *	qnChildren[QTquad_COUNT];

    int			qnValueCount;
    void **		qnValues;
    } QuadNode;

typedef struct QuadTree
    {
    DocumentRectangle	qtRectangle;
    int			qtDiameter;
    QuadNode *		qtRootNode;
    } QuadTree;

typedef int (*QuadForAllCall)(	int		x,
				int		y,
				void *		data,
				void *		through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern QuadTree * qtMakeTree(	const DocumentRectangle *	dr );

extern void qtFreeTree(		QuadTree *	qt );

extern int qtPut(		QuadTree *	qt,
				int		x,
				int		y,
				void *		data );

extern int qtGetExact(		QuadTree *	qt,
				int		x,
				int		y,
				void ** const	pvals,
				int *		pnval );

extern int qtGetNearest(	QuadTree *	qt,
				int		x,
				int		y,
				const void *	data,
				int *		pX,
				int *		pY,
				void * const **	pvals,
				int *		pnval );

extern const char * qtQuadrantStr(	int q );
extern const char * qtOctantStr(	int q );

extern int qtForAllInRectangle(	const QuadTree *		qt,
				const DocumentRectangle *	dr,
				QuadForAllCall			fun,
				void *				through );

#   endif		/* UTIL_QUAD_TREE_H */
