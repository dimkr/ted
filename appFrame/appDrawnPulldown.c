#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"appFrame.h"
#   include	"appSystem.h"
#   include	<appGeoString.h>

#   include	<appDebugon.h>

void appDrawnPulldownDrawArrow(		int *			pRestWide,
					int *			pRestHigh,
					APP_WIDGET		w,
					AppDrawingData *	add )
    {
    int		wide;
    int		high;

    int		x0;
    int		wi;
    int		m;
    int		d;

    APP_POINT	points[4];

    appDrawGetSizeOfWidget( &wide, &high, w );

    wi= ( 2* high )/ 3;
    x0= wide- wi;
    m= x0+ wi/ 2;
    d= wi/ 4;

    points[0].x= m- d;
    points[0].y= high/ 3;
    points[1].x= m+ d;
    points[1].y= high/ 3;
    points[2].x= m;
    points[2].y= ( 2* high )/ 3;

    points[3]= points[0];

    appDrawSetForegroundBlack( add );
    appDrawFillPolygon( add, points, 3 );

    *pRestWide= wide- wi;
    *pRestHigh= high;
    return;
    }

