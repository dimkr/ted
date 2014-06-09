/************************************************************************/
/*									*/
/*  Various definitions relating to document geometry.			*/
/*									*/
/************************************************************************/

#   ifndef		GEO_2D_INTEGER_H
#   define		GEO_2D_INTEGER_H

typedef struct DocumentRectangle
    {
    int		drX0;
    int		drY0;
    int		drX1;
    int		drY1;
    } DocumentRectangle;

#   define RECTDEB(dr) appDebug( "%s(%3d): %s= [%d..%d x %d..%d]\n",	\
			    __FILE__, __LINE__, #dr,			\
			    (dr)->drX0, (dr)->drX1,			\
			    (dr)->drY0, (dr)->drY1 )

typedef struct Point2DI
    {
    int		p2diX;
    int		p2diY;
    } Point2DI;

# define geo2DIPointInBox( p, b ) \
		    ( (p)->p2diX >= (b)->drX0 && \
		      (p)->p2diX <= (b)->drX1 && \
		      (p)->p2diY >= (b)->drY0 && \
		      (p)->p2diY <= (b)->drY1 )

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitRectangle(	DocumentRectangle *		dr );

extern void geoUnionRectangle(	DocumentRectangle *		dr,
				const DocumentRectangle *	dr1,
				const DocumentRectangle *	dr2	);

extern void docNormalizeRectangle(
				DocumentRectangle *		drTo,
				const DocumentRectangle *	drFrom );

extern int geoIntersectRectangle(	DocumentRectangle *		dr,
					const DocumentRectangle *	dr1,
					const DocumentRectangle *	dr2 );

extern int geo2DIIntersectSegments(	Point2DI *		p1,
					Point2DI *		p2,
					double *		pXab1,
					double *		pXab2,
					double *		pXcd1,
					double *		pXcd2,
					const Point2DI *	ab,
					const Point2DI *	cd );

extern double geo2DIDistanceToLine(	const Point2DI *	ab,
					const Point2DI *	c );

extern double geo2DIProjectionOnLine(	const Point2DI *	ab,
					const Point2DI *	c );

extern int geo2DIPointInPolygon(	const Point2DI *	p,
					const Point2DI *	points,
					int			pointCount );

extern int geo2DIClipSegmentToRectangle(
				Point2DI			vpChanged[2],
				double *			pXp0,
				double *			pXp1,
				const Point2DI			vp[2],
				const DocumentRectangle *	dr );

#   endif	/*	GEO_2D_INTEGER_H */
