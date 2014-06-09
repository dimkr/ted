/************************************************************************/
/*									*/
/*  Primitives for drawing metafiles (WMF,PICT) on X11.			*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<appDraw.h>

#   include	<appDebugon.h>

int appDrawPrepareRoundRect(		XArc			arcs[4],
					APP_SEGMENT		segments[4],
					APP_POINT		points[12],
					int			x0,
					int			y0,
					int			x1,
					int			y1,
					int			w,
					int			h )
    {
    int		swap;

    if  ( x1 < x0 )
	{ swap= x0; x0= x1; x1= swap; }
    if  ( y1 < y0 )
	{ swap= y0; y0= y1; y1= swap; }

    if  ( h < 0 )
	{ h= -h;	}
    if  ( w < 0 )
	{ w= -w;	}

    points[0].x= x0;
    points[0].y= y0+ h/ 2;

    arcs[0].x= x0;
    arcs[0].y= y0;
    arcs[0].width= w;
    arcs[0].height= h;
    arcs[0].angle1= 64*  180;
    arcs[0].angle2= 64*  -90;

    points[1].x= x0+ w/ 2;
    points[1].y= y0+ h/ 2;
    points[2].x= x0+ w/ 2;
    points[2].y= y0;

    segments[0].x1= x0+ w/2;
    segments[0].y1= y0;
    segments[0].x2= x1- w/2;
    segments[0].y2= y0;

    points[3].x= x1- w/ 2;
    points[3].y= y0;

    arcs[1].x= x1- w;
    arcs[1].y= y0;
    arcs[1].width= w;
    arcs[1].height= h;
    arcs[1].angle1= 64*   90;
    arcs[1].angle2= 64*  -90;

    points[4].x= x1- w/ 2;
    points[4].y= y0+ h/ 2;
    points[5].x= x1;
    points[5].y= y0+ w/ 2;

    segments[1].x1= x1;
    segments[1].y1= y0+ h/2;
    segments[1].x2= x1;
    segments[1].y2= y1- h/2;

    points[6].x= x1;
    points[6].y= y1- w/ 2;

    arcs[2].x= x1- w;
    arcs[2].y= y1- h;
    arcs[2].width= w;
    arcs[2].height= h;
    arcs[2].angle1= 64*    0;
    arcs[2].angle2= 64*  -90;

    points[7].x= x1- w/ 2;
    points[7].y= y1- h/ 2;
    points[8].x= x1- w/ 2;
    points[8].y= y1;

    segments[2].x1= x1- w/2;
    segments[2].y1= y1;
    segments[2].x2= x0+ w/2;
    segments[2].y2= y1;

    points[9].x= x0+ w/ 2;
    points[9].y= y1;

    arcs[3].x= x0;
    arcs[3].y= y1- h;
    arcs[3].width= w;
    arcs[3].height= h;
    arcs[3].angle1= 64*  -90;
    arcs[3].angle2= 64*  -90;

    points[10].x= x0+ w/ 2;
    points[10].y= y1- h/ 2;
    points[11].x= x0;
    points[11].y= y1- h/ 2;

    segments[3].x1= x0;
    segments[3].y1= y1- h/2;
    segments[3].x2= x0;
    segments[3].y2= y0+ h/2;

    return 0;
    }

int appDrawFillRoundRectX11(		AppDrawingData *	add,
					int			x0,
					int			y0,
					int			x1,
					int			y1,
					int			w,
					int			h )
    {
    XArc	arcs[4];
    APP_SEGMENT	segments[4];
    APP_POINT	points[12];

    appDrawPrepareRoundRect( arcs, segments, points, x0, y0, x1, y1, w, h );

    appDrawFillArcs( add, arcs, 4 );
    appDrawFillPolygon( add, points, 12 );

    return 0;
    }

int appDrawDrawRoundRectX11(		AppDrawingData *	add,
					int			x0,
					int			y0,
					int			x1,
					int			y1,
					int			w,
					int			h )
    {
    XArc	arcs[4];
    APP_SEGMENT	segments[4];
    APP_POINT	points[12];

    appDrawPrepareRoundRect( arcs, segments, points, x0, y0, x1, y1, w, h );

    appDrawDrawArcs( add, arcs, 4 );
    appDrawDrawSegments( add, segments, 4 );

    return 0;
    }

