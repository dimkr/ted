#   include	"appUtilConfig.h"

#   include	<string.h>
#   include	"geo2DInteger.h"

void docInitRectangle(	DocumentRectangle *		dr )
    { dr->drX0= dr->drY0= dr->drX1= dr->drY1= 0; return; }

/************************************************************************/
/*									*/
/*  Remove the possible orientation of a rectangle: Make sure x0 <= x1	*/
/*  and y0 <= y1.							*/
/*									*/
/************************************************************************/

void docNormalizeRectangle(	DocumentRectangle *		drTo,
				const DocumentRectangle *	drFrom )
    {
    DocumentRectangle	dr;
    int			swap;

    dr= *drFrom;

    if  ( dr.drX1 < dr.drX0 )
	{ swap= dr.drX1; dr.drX1= dr.drX0; dr.drX0= swap; }

    if  ( dr.drY1 < dr.drY0 )
	{ swap= dr.drY1; dr.drY1= dr.drY0; dr.drY0= swap; }

    *drTo= dr;

    return;
    }

void geoUnionRectangle(	DocumentRectangle *		dr,
			const DocumentRectangle *	dr1,
			const DocumentRectangle *	dr2 )
    {
    if  ( dr1->drX0 < dr2->drX0 )
	{ dr->drX0= dr1->drX0;	}
    else{ dr->drX0= dr2->drX0;	}

    if  ( dr1->drY0 < dr2->drY0 )
	{ dr->drY0= dr1->drY0;	}
    else{ dr->drY0= dr2->drY0;	}

    if  ( dr1->drX1 < dr2->drX1 )
	{ dr->drX1= dr2->drX1;	}
    else{ dr->drX1= dr1->drX1;	}

    if  ( dr1->drY1 < dr2->drY1 )
	{ dr->drY1= dr2->drY1;	}
    else{ dr->drY1= dr1->drY1;	}
    }

int geoIntersectRectangle(	DocumentRectangle *		dr,
				const DocumentRectangle *	dr1,
				const DocumentRectangle *	dr2 )
    {
    DocumentRectangle	res= *dr1;

    if  ( dr1->drX1 < dr2->drX0 )
	{ return 0;	}

    if  ( dr2->drX1 < dr1->drX0 )
	{ return 0;	}

    if  ( dr1->drY1 < dr2->drY0 )
	{ return 0;	}

    if  ( dr2->drY1 < dr1->drY0 )
	{ return 0;	}

    if  ( ! dr )
	{ return 1;	}

    if  ( dr1->drX0 < dr2->drX0 )
	{ res.drX0= dr2->drX0;	}
    else{ res.drX0= dr1->drX0;	}

    if  ( dr1->drY0 < dr2->drY0 )
	{ res.drY0= dr2->drY0;	}
    else{ res.drY0= dr1->drY0;	}

    if  ( dr1->drX1 < dr2->drX1 )
	{ res.drX1= dr1->drX1;	}
    else{ res.drX1= dr2->drX1;	}

    if  ( dr1->drY1 < dr2->drY1 )
	{ res.drY1= dr1->drY1;	}
    else{ res.drY1= dr2->drY1;	}

    *dr= res; return 1;
    }

