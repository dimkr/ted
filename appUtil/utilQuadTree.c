/************************************************************************/
/*									*/
/*  Quad Tree implementation.						*/
/*									*/
/*  Some kind of balancing is achieved by dividing a rectangle in	*/
/*  quadrants. No attempt is made to balance the tree when the entries	*/
/*  are not evenly spaced.						*/
/*									*/
/************************************************************************/

#   include	"utilQuadTree.h"

#   include	<appDebugon.h>

static const int QMAP[]= { 2, 1, 0, 3 };

# define QUADRANT( x, y, xm, ym ) \
		    ( QMAP[ 2* ( (x) >= (xm) )+ ( (y) >= (ym) ) ] )

/************************************************************************/
/*									*/
/*  Debuugging routines.						*/
/*									*/
/************************************************************************/

const char * qtQuadrantStr(	int q )
    {
    static char	scratch[12];

    switch( q )
	{
	case QTquadNE: return "NE";
	case QTquadNW: return "NW";
	case QTquadSW: return "SW";
	case QTquadSE: return "SE";

	default:
	    sprintf( scratch, "%d", q );
	    break;
	}

    return scratch;
    }

const char * qtOctantStr(	int o )
    {
    static char	scratch[12];

    switch( o )
	{
	case QToctENE: return "ENE";
	case QToctNNE: return "NNE";
	case QToctNNW: return "NNW";
	case QToctWNW: return "WNW";
	case QToctWSW: return "WSW";
	case QToctSSW: return "SSW";
	case QToctSSE: return "SSE";
	case QToctESE: return "ESE";

	default:
	    sprintf( scratch, "%d", o );
	    break;
	}

    return scratch;
    }

/************************************************************************/
/*									*/
/*  Destroy a node.							*/
/*									*/
/************************************************************************/

static void qnfree( QuadNode *	qn )
    {
    int		i;

    for ( i= 0; i < QTquad_COUNT; i++ )
	{
	if  ( qn->qnChildren[i] )
	    { qnfree( qn->qnChildren[i] );	}
	}

    if  ( qn->qnValues )
	{ free( (void *)qn->qnValues );	}

    free( (void *)qn );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a tree.							*/
/*									*/
/*  NOTE the sanity check on the coordinates. It removes the necessity	*/
/*	to check everything in the other routines.			*/
/*									*/
/************************************************************************/

QuadTree * qtMakeTree(	const DocumentRectangle *	dr )
    {
    int		d;
    QuadTree *	rval;

    rval= (QuadTree *)malloc( sizeof(QuadTree) );
    if  ( ! rval )
	{ XDEB(rval); return (QuadTree *)0;	}

    rval->qtRectangle= *dr;
    rval->qtRootNode= (QuadNode *)0;

    d= dr->drX1- dr->drX0;
    if  ( d < dr->drY1- dr->drY0 )
	{ d=  dr->drY1- dr->drY0;	}

    rval->qtDiameter= 1;
    while( rval->qtDiameter <= d )
	{ rval->qtDiameter *= 2; }

    return rval;
    }

/************************************************************************/
/*									*/
/*  Destroy a tree.							*/
/*									*/
/************************************************************************/

void qtFreeTree(	QuadTree *	qt )
    {
    if  ( qt->qtRootNode )
	{ qnfree( qt->qtRootNode );	}

    free( (char *)qt );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a node.							*/
/*									*/
/************************************************************************/

static QuadNode * qnmake(	QuadNode *	parent,
				int		x,
				int		y )
    {
    int		i;
    QuadNode *	rval= (QuadNode *)malloc( sizeof(QuadNode) );

    if  ( ! rval )
	{ XDEB(rval); return (QuadNode *)0;	}


    rval->qnX= x;
    rval->qnY= y;
    rval->qn_parent= parent;

    for ( i= 0; i < QTquad_COUNT; i++ )
	{ rval->qnChildren[i]= (QuadNode *)0; }

    rval->qnValueCount= 0;
    rval->qnValues= (void **)0;

    return rval;
    }

/************************************************************************/
/*									*/
/*  Insert a new value into a tree.					*/
/*									*/
/************************************************************************/

# if 0
static void qnValidate(	const QuadNode *	qn )
    {
    int		i;

    for ( i= 0; i < QTquad_COUNT; i++ )
	{
	int			q;
	const QuadNode *	child= qn->qnChildren[i];

	if  ( ! child )
	    { continue;	}

	q= QUADRANT( child->qnX, child->qnY, qn->qnX, qn->qnY );
	if  ( q != i )
	    {
	    SSDEB(qtQuadrantStr(q),qtQuadrantStr(i));
	    LLLLDEB( child->qnX, child->qnY, qn->qnX, qn->qnY );
	    }

	qnValidate( child );
	}
    }
# endif

int qtPut(	QuadTree *	qt,
		int		x,
		int		y,
		void *		data )
    {
    int			x0= qt->qtRectangle.drX0;
    int			xp= x0+ qt->qtDiameter;
    int			y0= qt->qtRectangle.drY0;
    int			yp= y0+ qt->qtDiameter;

    int			xm= ( x0+ xp )/ 2;
    int			ym= ( y0+ yp )/ 2;

    QuadNode *		qn;

    void **		fresh;

    if  ( x <  qt->qtRectangle.drX0	||
	  x >= qt->qtRectangle.drX1	||
	  y <  qt->qtRectangle.drY0	||
	  y >= qt->qtRectangle.drY1	)
	{ LLLLDEB(x,y,qt->qtRectangle.drX1,qt->qtRectangle.drY1); return -1; }

    if  ( ! qt->qtRootNode )
	{
	qt->qtRootNode= qnmake( (QuadNode *)0, xm, ym );
	if  ( ! qt->qtRootNode )
	    { XDEB(qt->qtRootNode); return -1;	}
	}

    qn= qt->qtRootNode;

    while( xm != x0 || ym != y0 )
	{
	int	q= QUADRANT( x, y, qn->qnX, qn->qnY );

	/*
	if  ( qn->qnX != xm || qn->qnY != ym )
	    { LLLLDEB(xm,ym,qn->qnX,qn->qnY); return -1;	}
	*/

	if  ( x < xm )
	    { xp= xm; }
	else{ x0= xm; }
	if  ( y < ym )
	    { yp= ym; }
	else{ y0= ym; }

	xm= ( x0+ xp )/ 2;
	ym= ( y0+ yp )/ 2;

	if  ( ! qn->qnChildren[q] )
	    {
	    qn->qnChildren[q]= qnmake( qn, xm, ym );
	    if  ( ! qn->qnChildren[q] )
		{ XDEB(qn->qnChildren[q]); return -1;	}
	    }

	qn= qn->qnChildren[q];
	}

    /*
    if  ( qn->qnX != x || qn->qnY != y )
	{ LLLLDEB(x,y,qn->qnX,qn->qnY); return -1;	}
    */

    fresh= (void **)realloc( (void *)qn->qnValues,
				    (qn->qnValueCount +1)* sizeof(void *) );

    if  ( ! fresh )
	{ LXDEB(qn->qnValueCount,fresh); return -1;	}

    qn->qnValues= fresh;
    fresh[qn->qnValueCount++]= data;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Get a series of values stored on a Quad Tree.			*/
/*									*/
/************************************************************************/

int qtGetExact(	QuadTree *	qt,
		int		x,
		int		y,
		void ** const	pvals,
		int *		pnval )
    {
    int			x0= qt->qtRectangle.drX0;
    int			xp= x0+ qt->qtDiameter;
    int			y0= qt->qtRectangle.drY0;
    int			yp= y0+ qt->qtDiameter;

    int			xm= ( x0+ xp )/ 2;
    int			ym= ( y0+ yp )/ 2;

    QuadNode *		qn= qt->qtRootNode;

    while( qn && ( xm != x0 || ym != y0 ) )
	{
	int	q= QUADRANT( x, y, qn->qnX, qn->qnY );

	if  ( x < xm )
	    { xp= xm; }
	else{ x0= xm; }
	if  ( y < ym )
	    { yp= ym; }
	else{ y0= ym; }

	xm= ( x0+ xp )/ 2;
	ym= ( y0+ yp )/ 2;

	qn= qn->qnChildren[q];
	}

    if  ( ! qn )
	{ return 1;	}

    if  ( qn->qnX != x || qn->qnY != y )
	{ LLLLDEB(x,y,qn->qnX,qn->qnY); return 1;	}

    *pvals= qn->qnValues;
    *pnval= qn->qnValueCount;
    
    return 0;
    }

/************************************************************************/
/*									*/
/*  Do something with all stored values in a rectangle.			*/
/*									*/
/************************************************************************/

static const int QTSCAN[QToct_COUNT][QTquad_COUNT]=
    {
	{ QTquadNE, QTquadSE, QTquadNW, QTquadSW }, /*  QToctENE  */
	{ QTquadNE, QTquadNW, QTquadSE, QTquadSW }, /*  QToctNNE  */

	{ QTquadNW, QTquadNE, QTquadSW, QTquadSE }, /*  QToctNNW  */
	{ QTquadNW, QTquadSW, QTquadNE, QTquadSE }, /*  QToctWNW  */

	{ QTquadSW, QTquadNW, QTquadSE, QTquadNE }, /*  QToctWSW  */
	{ QTquadSW, QTquadSE, QTquadNW, QTquadNE }, /*  QToctSSW  */

	{ QTquadSE, QTquadSW, QTquadNE, QTquadNW }, /*  QToctSSE  */
	{ QTquadSE, QTquadNE, QTquadSW, QTquadNW }, /*  QToctESE  */
    };

static int qnGetNearest(	QuadNode *		qn,
				int 			level,
				const QuadNode **	pFound,
				long *			pD2,
				int			x,
				int			y,
				const void *		data )
    {
    int		rval= 1;
    int		i;

    int		q= QUADRANT( x, y, qn->qnX, qn->qnY );
    int		o;

    long	d2x;
    long	d2y;
    long	d2;

    long	d[4];
    const int *	c;

    d2= 0;
    d2x= x- qn->qnX; d2x= d2x* d2x;
    d2y= y- qn->qnY; d2y= d2y* d2y;
    d2= d2x+ d2y;

    d[0]= 0;
    d[3]= d2;

    switch( q )
	{
	case QTquadNE:
	    if  ( x- qn->qnX > y- qn->qnY )
		{
		o= QToctENE;
		d[1]= d2y; d[2]= d2x;
		}
	    else{
		o= QToctNNE;
		d[1]= d2x; d[2]= d2y;
		}
	    break;

	case QTquadNW:
	    if  ( qn->qnX- x < y- qn->qnY )
		{
		o= QToctNNW;
		d[1]= d2x; d[2]= d2y;
		}
	    else{
		o= QToctWNW;
		d[1]= d2y; d[2]= d2x;
		}
	    break;

	case QTquadSW:
	    if  ( qn->qnX- x > qn->qnY- y )
		{
		o= QToctWSW;
		d[1]= d2y; d[2]= d2x;
		}
	    else{
		o= QToctSSW;
		d[1]= d2x; d[2]= d2y;
		}
	    break;

	case QTquadSE:
	    if  ( x- qn->qnX < qn->qnY- y )
		{
		o= QToctSSE;
		d[1]= d2x; d[2]= d2y;
		}
	    else{
		o= QToctESE;
		d[1]= d2y; d[2]= d2x;
		}
	    break;

	default:
	    LDEB(q); return -1;
	}

    /*
    appDebug( "NODE %*s [%4d,%4d] %s[%s] %*s N=%d D=%9ld\n",
		    2* level, "", qn->qnX, qn->qnY,
		    qtQuadrantStr( q ), qtOctantStr( o ),
		    level<16?32-2*level:2, "",
		    qn->qnValueCount, d2 );
    */


    c= QTSCAN[o];
    for ( i= 0; i < QTquad_COUNT; i++ )
	{
	int		r;
	QuadNode *	child= qn->qnChildren[c[i]];

	if  ( ! child )
	    { continue;	}

	if  ( d[i] > *pD2 )
	    { continue;	}

	r= qnGetNearest( child, level+ 1, pFound, pD2, x, y, data );
	if  ( r < 0 )
	    { LLDEB(i,r); return -1;	}
	if  ( r == 0 )
	    { rval= 0;	}
	}

    if  ( d2 < *pD2					&&
	  qn->qnValueCount > 0				&&
	  ( qn->qnValueCount > 1 || qn->qnValues[0] != data )	)
	{
	/*
	appDebug( "GOT  %*s [%4d,%4d]         %*s N=%d D=%9ld (!)\n",
			2* level, "", qn->qnX, qn->qnY,
			level<16?32-2*level:2, "",
			qn->qnValueCount, d2 );

	*/

	*pFound= qn;
	*pD2= d2;
	rval= 0;
	}

    return rval;
    }

int qtGetNearest(	QuadTree *	qt,
			int		x,
			int		y,
			const void *	data,
			int *		pX,
			int *		pY,
			void * const **	pvals,
			int *		pnval )
    {
    int			rval= 0;
    long		d;
    long		d2;
    const int		level= 0;

    const QuadNode *	found= (const QuadNode *)0;

    if  ( ! qt->qtRootNode )
	{ return 1;	}

    if  ( x <  qt->qtRectangle.drX0	||
	  x >= qt->qtRectangle.drX1	||
	  y <  qt->qtRectangle.drY0	||
	  y >= qt->qtRectangle.drY1	)
	{ return 1;	}

    d2= 0;
    d= qt->qtRectangle.drX1- qt->qtRectangle.drX0; d2 += d* d;
    d= qt->qtRectangle.drY1- qt->qtRectangle.drY0; d2 += d* d;

    rval= qnGetNearest( qt->qtRootNode, level, &found, &d2, x, y, data );

    if  ( rval == 0 )
	{
	int	i;
	int	nval= found->qnValueCount;

	for ( i= 0; i < found->qnValueCount; i++ )
	    {
	    if  ( found->qnValues[i] == data )
		{
		nval--;

		if  ( found->qnValueCount > 1 && i != nval )
		    {
		    void *	dt;

		    dt= found->qnValues[i];
		    found->qnValues[i]= found->qnValues[nval];
		    found->qnValues[nval]= dt;
		    }

		break;
		}
	    }

	*pX= found->qnX;
	*pY= found->qnY;
	*pvals= found->qnValues;
	*pnval= nval;
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Perform an operation for all values in a certain rectangle.		*/
/*									*/
/************************************************************************/

static int qnForAllInRectangle(	const QuadNode *		qn,
				const DocumentRectangle *	dr,
				QuadForAllCall			fun,
				void *				through )
    {
    int		res;
    int		rval= 0;

    int		children[QTquad_COUNT];
    int		i;

    for ( i= 0; i < QTquad_COUNT; i++ )
	{ children[i]= 0; }

    children[QUADRANT( dr->drX0, dr->drY0, qn->qnX, qn->qnY )]= 1;
    children[QUADRANT( dr->drX1, dr->drY0, qn->qnX, qn->qnY )]= 1;
    children[QUADRANT( dr->drX0, dr->drY1, qn->qnX, qn->qnY )]= 1;
    children[QUADRANT( dr->drX1, dr->drY1, qn->qnX, qn->qnY )]= 1;

    for ( i= 0; i < QTquad_COUNT; i++ )
	{
	if  ( qn->qnChildren[i] && children[i] )
	    {
	    res= qnForAllInRectangle( qn->qnChildren[i], dr, fun, through );
	    if  ( res < 0 )
		{ LDEB(res); return -1;	}
	    if  ( res > 0 )
		{ rval= 1;	}
	    }
	}

    if  ( qn->qnX >= dr->drX0	&&
	  qn->qnY >= dr->drY0	&&
	  qn->qnX <= dr->drX1	&&
	  qn->qnY <= dr->drY1	)
	{
	for ( i= 0; i < qn->qnValueCount; i++ )
	    {
	    res= (*fun)( qn->qnX, qn->qnY, qn->qnValues[i], through );
	    if  ( res < 0 )
		{ LDEB(res); return -1;	}
	    if  ( res > 0 )
		{ rval= 1;	}
	    }
	}

    return rval;
    }

int qtForAllInRectangle(	const QuadTree *		qt,
				const DocumentRectangle *	dr,
				QuadForAllCall			fun,
				void *				through )
    {
    int		rval= 0;

    if  ( qt->qtRootNode						&&
	  geoIntersectRectangle( (DocumentRectangle *)0,
					    &(qt->qtRectangle), dr )	)
	{ rval= qnForAllInRectangle( qt->qtRootNode, dr, fun, through ); }

    return rval;
    }

