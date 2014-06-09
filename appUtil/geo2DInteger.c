#   include	"appUtilConfig.h"

#   include	<stdio.h>
#   include	<string.h>
#   include	<stdlib.h>
#   include	<stddef.h>
#   include	<ctype.h>
#   include	<limits.h>
#   include	<math.h>

#   include	"geo2DInteger.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Intersect two line segments. Use the algorithm from:		*/
/*									*/
/*  Subject 1.03 in http://www.faqs.org/faqs/graphics/algorithms-faq.	*/
/*									*/
/*  Arguments:								*/
/*									*/
/************************************************************************/

int geo2DIIntersectSegments(	Point2DI *		p1,
				Point2DI *		p2,
				double *		pXab1,
				double *		pXab2,
				double *		pXcd1,
				double *		pXcd2,
				const Point2DI *	ab,
				const Point2DI *	cd )
    {
    double	numXab;
    double	xab;
    double	numXcd;
    double	xcd;
    double	divxxx;

    double	Ax= ab[0].p2diX;
    double	Ay= ab[0].p2diY;
    double	Bx= ab[1].p2diX;
    double	By= ab[1].p2diY;

    double	Cx= cd[0].p2diX;
    double	Cy= cd[0].p2diY;
    double	Dx= cd[1].p2diX;
    double	Dy= cd[1].p2diY;

    if  ( Ax == Bx  && Ay == By )
	{ FFDEB(Ax,Bx); FFDEB(Ay,By); return 0;	}
    if  ( Cx == Dx  && Cy == Dy )
	{ FFDEB(Cx,Dx); FFDEB(Cy,Dy); return 0;	}

    numXab= ( Ay- Cy )* ( Dx- Cx )- ( Ax- Cx )* ( Dy- Cy );
    numXcd= ( Ay- Cy )* ( Bx- Ax )- ( Ax- Cx )* ( By- Ay );
    divxxx= ( Bx- Ax )* ( Dy- Cy )- ( By- Ay )* ( Dx- Cx );

    if  ( divxxx == 0 )
	{
	if  ( numXab == 0 )
	    {
	    double		C_AB;
	    double		D_AB;
	    double		A_CD;
	    double		B_CD;

	    /*FFDEB(divxxx,numXab);*/

	    if  ( Ax == Bx ) /* collinear and vertical */
		{
		C_AB=	( Cy- Ay )/ ( By- Ay );
		D_AB=	( Dy- Ay )/ ( By- Ay );
		A_CD=	( Ay- Cy )/ ( Dy- Cy );
		B_CD=	( By- Cy )/ ( Dy- Cy );
		}
	    else{
		C_AB=	( Cx- Ax )/ ( Bx- Ax );
		D_AB=	( Dy- Ay )/ ( By- Ay );
		A_CD=	( Ax- Cx )/ ( Dx- Cx );
		B_CD=	( Bx- Cx )/ ( Dx- Cx );
		}

	    if  ( C_AB < 0.0 && D_AB < 0.0 )
		{ return 0;	}
	    if  ( C_AB > 1.0 && D_AB > 1.0 )
		{ return 0;	}

	    if  ( D_AB > C_AB )
		{
		if  ( D_AB == 0.0 )
		    {
		    p1->p2diX= Ax;
		    p1->p2diY= Ay;
		    *pXab1= 0.0;
		    *pXcd1= 1.0;
		    return 1;
		    }

		if  ( C_AB >= 0.0 )
		    {
		    p1->p2diX= Cx;
		    p1->p2diY= Cy;
		    *pXab1= C_AB;
		    *pXcd1= 0.0;
		    if  ( C_AB == 1.0 )
			{ return 1;	}
		    }
		else{
		    p1->p2diX= Ax;
		    p1->p2diY= Ay;
		    *pXab1= 0.0;
		    *pXcd1= A_CD;
		    }

		if  ( D_AB <= 1.0 )
		    {
		    p2->p2diX= Dx;
		    p2->p2diY= Dy;
		    *pXab2= D_AB;
		    *pXcd2= 1.0;
		    }
		else{
		    p2->p2diX= Bx;
		    p2->p2diY= By;
		    *pXab2= 1.0;
		    *pXcd2= B_CD;
		    }

		return 2;
		}
	    else{
		if  ( C_AB == 0.0 )
		    {
		    p1->p2diX= Ax;
		    p1->p2diY= Ay;
		    *pXab1= 0.0;
		    *pXcd1= 0.0;
		    return 1;
		    }

		if  ( D_AB >= 0.0 )
		    {
		    p1->p2diX= Dx;
		    p1->p2diY= Dy;
		    *pXab1= D_AB;
		    *pXcd1= 1.0;
		    if  ( D_AB == 1.0 )
			{ return 1;	}
		    }
		else{
		    p1->p2diX= Ax;
		    p1->p2diY= Ay;
		    *pXab1= 0.0;
		    *pXcd1= A_CD;
		    }

		if  ( C_AB <= 1.0 )
		    {
		    p2->p2diX= Cx;
		    p2->p2diY= Cy;
		    *pXab2= C_AB;
		    *pXcd2= 0.0;
		    }
		else{
		    p2->p2diX= Bx;
		    p2->p2diY= By;
		    *pXab2= 1.0;
		    *pXcd2= 1.0- B_CD;
		    }

		return 2;
		}
	    }
	else{ return 0;	}
	}

    xab= numXab/ divxxx;
    xcd= numXcd/ divxxx;

    if  ( xab < 0.0 || xab > 1.0 )
	{ return 0;	}
    if  ( xcd < 0.0 || xcd > 1.0 )
	{ return 0;	}

    p1->p2diX= p2->p2diX= Ax+ xab* ( Bx- Ax );
    p1->p2diY= p2->p2diY= Ay+ xab* ( By- Ay );
    *pXab1= xab;
    *pXcd1= xcd;
    /*
    p1->p2diX= p2->p2diX= Cx+ xcd* ( Cx- Dx );
    p1->p2diY= p2->p2diY= Cy+ xcd* ( Cy- Dy );
    */

    return 1;
    }

/************************************************************************/
/*									*/
/*  Calculate the distance of point 'c' to the line through points 'a'	*/
/*  and 'b'.								*/
/*									*/
/*  Distances to the left are positive.					*/
/*									*/
/************************************************************************/

double geo2DIDistanceToLine(	const Point2DI *	ab,
				const Point2DI *	c )
    {
    double	Ax= ab[0].p2diX;
    double	Ay= ab[0].p2diY;
    double	Bx= ab[1].p2diX;
    double	By= ab[1].p2diY;

    double	Cx= c[0].p2diX;
    double	Cy= c[0].p2diY;

    double	L2= ( Bx- Ax )* ( Bx- Ax )+ ( By- Ay )* ( By- Ay );
    /*
    double	Dr= ( Cx- Ax )* ( Bx- Ax )+ ( Cy- Ay )* ( By- Ay );
    double	r;
    */
    double	Ds= ( Ay- Cy )* ( Bx- Ax )+ ( Ax- Cx )* ( By- Ay );
    double	s;

    if  ( Ax == Bx  && Ay == By )
	{ FFDEB(Ax,Bx); FFDEB(Ay,By); return 0;	}

    s= Ds/ L2;

    return s* sqrt( L2 );
    }

/************************************************************************/
/*									*/
/*  Calculate the distance of the projection of point 'c' on the line	*/
/*  through points 'a'  and 'b' to point A.				*/
/*  and 'b'.								*/
/*									*/
/*  Distances in the direction of B are positive.			*/
/*									*/
/************************************************************************/

double geo2DIProjectionOnLine(	const Point2DI *	ab,
				const Point2DI *	c )
    {
    double	Ax= ab[0].p2diX;
    double	Ay= ab[0].p2diY;
    double	Bx= ab[1].p2diX;
    double	By= ab[1].p2diY;

    double	Cx= c[0].p2diX;
    double	Cy= c[0].p2diY;

    double	L2= ( Bx- Ax )* ( Bx- Ax )+ ( By- Ay )* ( By- Ay );

    double	Dr= ( Cx- Ax )* ( Bx- Ax )+ ( Cy- Ay )* ( By- Ay );
    double	r;
    /*
    double	Ds= ( Ay- Cy )* ( Bx- Ax )+ ( Ax- Cx )* ( By- Ay );
    double	s;
    */

    if  ( Ax == Bx  && Ay == By )
	{ FFDEB(Ax,Bx); FFDEB(Ay,By); return 0;	}

    r= Dr/ L2;

    return r* sqrt( L2 );
    }


/************************************************************************/
/*									*/
/*  See whether a point is in a polygon.				*/
/*									*/
/*  The number of intersections of the line to a point very far away is	*/
/*  counted. It should be odd.						*/
/*									*/
/*  1)  When the line is collinear with one of the sides of the		*/
/*      polygon, another point far away it tried.			*/
/*  2)  Skip zero length segments.					*/
/*  3)  Intersect segment with segment to point far away.		*/
/*  4)  If collinear, abort this attempt.. Another point far away is	*/
/*	chosen below.							*/
/*									*/
/************************************************************************/

int geo2DIPointInPolygon(	const Point2DI *	p,
				const Point2DI *	points,
				int			pointCount )
    {
    Point2DI		ab[2];

    Point2DI		p1;
    Point2DI		p2;

    int			attempts= 0;

    ab[1]= ab[0]= *p;
    ab[1].p2diX= INT_MAX;

    /*  1  */
    for (;;)
	{
	int			i;
	int			res;
	const Point2DI *	vp;
	int			crossings= 0;

	double			xab1;
	double			xab2;
	double			xvp1;
	double			xvp2;

	vp= points;
	for ( i= 0; i < pointCount; vp++, i++ )
	    {
	    /*  2  */
	    if  ( vp[0].p2diX == vp[1].p2diX	&&
		  vp[0].p2diY == vp[1].p2diY	)
		{ continue;	}

	    /*  3  */
	    res= geo2DIIntersectSegments( &p1, &p2,
					&xab1, &xab2, &xvp1, &xvp2, ab, vp );
	    if  ( res == 0 )
		{ continue;	}

	    /*  4  */
	    if  ( res != 1 )
		{ /*LDEB(res);*/ break;	}

	    /*  5  */
	    if  ( p1.p2diX == vp[0].p2diX		&&
		  p1.p2diY == vp[0].p2diY		)
		{
		if  ( p1.p2diX == p->p2diX	&&
		      p1.p2diY == p->p2diY		)
		    { return 1;	}

		/*FFDEB(p1.p2diX,p1.p2diY);*/
		if  ( attempts < 60 )
		    { break;	}
		}

	    if  ( p1.p2diX == vp[1].p2diX		&&
		  p1.p2diY == vp[1].p2diY		)
		{
		if  ( p1.p2diX == p->p2diX	&&
		      p1.p2diY == p->p2diY		)
		    { return 1;	}

		/*FFDEB(p1.p2diX,p1.p2diY);*/
		if  ( attempts < 60 )
		    { break;	}
		}

	    if  ( xvp1 < 0.001 || xvp1 > 0.999	||
		  xvp2 < 0.001 || xvp2 > 0.999	)
		{
		if  ( attempts < 60 )
		    { break;	}
		}

	    crossings++;
	    }

	if  ( i < pointCount )
	    { ab[1].p2diY += 100; attempts++; continue;	}

	if  ( crossings % 2 )
	    { return 1;	}
	else{ return 0;	}
	}
    }

/************************************************************************/
/*									*/
/*  Clip a line segment to a rectangle.					*/
/*									*/
/************************************************************************/

static int geo2DIFirstBorderIntersection(
					Point2DI *			pP,
					double *			pXp,
					Point2DI			p0,
					Point2DI			p1,
					const DocumentRectangle *	dr )
    {
    Point2DI	pa;
    Point2DI	pb;
    double	xpa;
    double	xpb;
    double	xsa;
    double	xsb;

    Point2DI	p;
    double	xp= 2;

    Point2DI	vp[2];
    Point2DI	side[2];

    vp[0]= p0;
    vp[1]= p1;

    side[0].p2diX= dr->drX0; side[0].p2diY= dr->drY0;
    side[1].p2diX= dr->drX1; side[1].p2diY= dr->drY0;
    if  ( geo2DIIntersectSegments( &pa, &pb, &xpa, &xpb, &xsa, &xsb,
						vp, side ) == 1 && xpa < xp )
	{ p= pa; xp= xpa;	}

    side[0].p2diX= dr->drX0; side[0].p2diY= dr->drY1;
    side[1].p2diX= dr->drX1; side[1].p2diY= dr->drY1;
    if  ( geo2DIIntersectSegments( &pa, &pb, &xpa, &xpb, &xsa, &xsb,
						vp, side ) == 1 && xpa < xp )
	{ p= pa; xp= xpa;	}

    side[0].p2diX= dr->drX0; side[0].p2diY= dr->drY0;
    side[1].p2diX= dr->drX0; side[1].p2diY= dr->drY1;
    if  ( geo2DIIntersectSegments( &pa, &pb, &xpa, &xpb, &xsa, &xsb,
						vp, side ) == 1 && xpa < xp )
	{ p= pa; xp= xpa;	}

    side[0].p2diX= dr->drX1; side[0].p2diY= dr->drY0;
    side[1].p2diX= dr->drX1; side[1].p2diY= dr->drY1;
    if  ( geo2DIIntersectSegments( &pa, &pb, &xpa, &xpb, &xsa, &xsb,
						vp, side ) == 1 && xpa < xp )
	{ p= pa; xp= xpa;	}

    if  ( xp > 1 )
	{ return 1;	}

    *pP= p; *pXp= xp; return 0;
    }

static int geo2DILastBorderIntersection( Point2DI *			pP,
					double *			pXp,
					Point2DI			p0,
					Point2DI			p1,
					const DocumentRectangle *	dr )
    {
    Point2DI	pa;
    Point2DI	pb;
    double	xpa;
    double	xpb;
    double	xsa;
    double	xsb;

    Point2DI	p;
    double	xp= -1;

    Point2DI	vp[2];
    Point2DI	side[2];

    vp[0]= p0;
    vp[1]= p1;

    side[0].p2diX= dr->drX0; side[0].p2diY= dr->drY0;
    side[1].p2diX= dr->drX1; side[1].p2diY= dr->drY0;
    if  ( geo2DIIntersectSegments( &pa, &pb, &xpa, &xpb, &xsa, &xsb,
						vp, side ) == 1 && xpa > xp )
	{ p= pa; xp= xpa;	}

    side[0].p2diX= dr->drX0; side[0].p2diY= dr->drY1;
    side[1].p2diX= dr->drX1; side[1].p2diY= dr->drY1;
    if  ( geo2DIIntersectSegments( &pa, &pb, &xpa, &xpb, &xsa, &xsb,
						vp, side ) == 1 && xpa > xp )
	{ p= pa; xp= xpa;	}

    side[0].p2diX= dr->drX0; side[0].p2diY= dr->drY0;
    side[1].p2diX= dr->drX0; side[1].p2diY= dr->drY1;
    if  ( geo2DIIntersectSegments( &pa, &pb, &xpa, &xpb, &xsa, &xsb,
						vp, side ) == 1 && xpa > xp )
	{ p= pa; xp= xpa;	}

    side[0].p2diX= dr->drX1; side[0].p2diY= dr->drY0;
    side[1].p2diX= dr->drX1; side[1].p2diY= dr->drY1;
    if  ( geo2DIIntersectSegments( &pa, &pb, &xpa, &xpb, &xsa, &xsb,
						vp, side ) == 1 && xpa > xp )
	{ p= pa; xp= xpa;	}

    if  ( xp < 0 )
	{ return 1;	}

    *pP= p; *pXp= xp; return 0;
    }

int geo2DIClipSegmentToRectangle(
				Point2DI			vpChanged[2],
				double *			pXp0,
				double *			pXp1,
				const Point2DI			vp[2],
				const DocumentRectangle *	dr )
    {
    double	xp0= 0.0;
    double	xp1= 1.0;

    Point2DI	p0= vp[0];
    Point2DI	p1= vp[1];

    if  ( ! geo2DIPointInBox( &(vp[0]), dr ) )
	{
	if  ( geo2DIFirstBorderIntersection( &p0, &xp0, vp[0], vp[1], dr ) )
	    { return 1;	}
	}

    if  ( ! geo2DIPointInBox( &(vp[1]), dr ) )
	{
	if  ( geo2DILastBorderIntersection( &p1, &xp1, vp[0], vp[1], dr ) )
	    { return 1;	}
	}

    vpChanged[0]= p0; *pXp0= xp0;
    vpChanged[1]= p1; *pXp1= xp1;

    return 0;
    }

