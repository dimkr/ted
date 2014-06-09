/************************************************************************/
/*									*/
/*  Primitives for drawing metafiles (WMF,PICT) on X11.			*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<appMetafile.h>
#   include	<utilPs.h>
#   include	<sioEndian.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<appDebugon.h>

int appMetafileDrawRoundRectX11(	AppDrawingData *	add,
					int			x0,
					int			y0,
					int			x1,
					int			y1,
					int			w,
					int			h,
					int			fill,
					int			borders,
					APP_COLOR_RGB *		fillColor,
					APP_COLOR_RGB *		borderColor )
    {
    XArc	arcs[4];
    APP_SEGMENT	segments[4];
    APP_POINT	points[12];

    appDrawPrepareRoundRect( arcs, segments, points, x0, y0, x1, y1, w, h );

    if  ( fill )
	{
	appDrawSetForegroundColor( add, fillColor );

	appDrawFillArcs( add, arcs, 4 );
	appDrawFillPolygon( add, points, 12 );
	}

    if  ( borders )
	{
	appDrawSetForegroundColor( add, borderColor );

	appDrawDrawArcs( add, arcs, 4 );
	appDrawDrawSegments( add, segments, 4 );
	}

    return 0;
    }

