/************************************************************************/
/*									*/
/*  Draw color cubes.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdio.h>
#   include	<string.h>
#   include	<stdlib.h>

#   include	<math.h>

#   include	<appRgbCube.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  For scanning the faces of the individual color cubes.		*/
/*									*/
/************************************************************************/

typedef struct FindColor
    {
    int			fcFound;
    int			fcSplitCandidate;
    Point2DI		fcPoint;
    RGB8Color		fcRgbColor;
    } FindColor;

typedef void (*HANDLE_FACE)(		RgbCube *		rc,
					RgbColorBlock *		rcb,
					void *			through,
					int			square,
					double			scale,
					const double		xo[8],
					const double		yo[8],
					int			canSplit,
					const int		nodes[4] );

/************************************************************************/
/*									*/
/*  1/2 the fraction of a block that is opened to show the inside.	*/
/*									*/
/************************************************************************/

#   define	RGB_K		(18* 256)
#   define	RGB_GAP		80
#   define	RGB_MARG	(-10)

/************************************************************************/
/*									*/
/*  Init/Clean an rgb cube.						*/
/*									*/
/************************************************************************/

void appInitRgbCube(	RgbCube *	rc )
    {
    rc->rcRedSteps= 1;
    rc->rcGreenSteps= 1;
    rc->rcBlueSteps= 1;

    appInitColors( &(rc->rcColors) );

    rc->rcColorBlocks= (RgbColorBlock *)0;
    rc->rcColorBlockCount= 0;

    return;
    }

void appCleanRgbCube(	RgbCube *	rc )
    {
    appCleanColors( &(rc->rcColors) );
    }

/************************************************************************/
/*									*/
/*  Common geometry calculations.					*/
/*									*/
/************************************************************************/

static void appRgbCubeGeometry(	double *		pScale,
				int *			pSquare,
				const RgbCube *		rc,
				int			wide,
				int			high )
    {
    int			drsz;
    int			square;
    double		scale;

    drsz= 256+ 2* RGB_GAP+ 2* RGB_MARG;
    if  ( wide <= high )
	{ square= wide;	}
    else{ square= high;	}

    scale= ( 1.0* square )/ drsz;

    *pScale= scale;
    *pSquare= square;

    return;
    }

/************************************************************************/
/*									*/
/*  See whether a mouse click is in one of the faces of a cube.		*/
/*									*/
/************************************************************************/

static void appTryRgbColorFace(		RgbCube *		rc,
					RgbColorBlock *		rcb,
					void *			through,
					int			square,
					double			scale,
					const double		xo[8],
					const double		yo[8],
					int			canSplit,
					const int		nodes[4] )
    {
    FindColor *		fc= (FindColor *)through;

    Point2DI		points[5];

    points[0].p2diX= scale* xo[nodes[0]]+ square/ 2;
    points[1].p2diX= scale* xo[nodes[1]]+ square/ 2;
    points[2].p2diX= scale* xo[nodes[2]]+ square/ 2;
    points[3].p2diX= scale* xo[nodes[3]]+ square/ 2;
    points[4].p2diX= scale* xo[nodes[0]]+ square/ 2;

    points[0].p2diY= square/ 2- scale* yo[nodes[0]];
    points[1].p2diY= square/ 2- scale* yo[nodes[1]];
    points[2].p2diY= square/ 2- scale* yo[nodes[2]];
    points[3].p2diY= square/ 2- scale* yo[nodes[3]];
    points[4].p2diY= square/ 2- scale* yo[nodes[0]];

    if  ( geo2DIPointInPolygon( &(fc->fcPoint), points, 4 ) )
	{
	fc->fcFound= 1;
	fc->fcSplitCandidate= canSplit;
	fc->fcRgbColor= rcb->rcbRgbColor;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Redraw an RGB cube.							*/
/*									*/
/************************************************************************/

static int appRgbCubeAllocateBlockColor(	RgbCube *	rc,
						RgbColorBlock *	rcb )
    {
    const RGB8Color *	col= &(rcb->rcbRgbColor);

    if  ( appColorRgb( &(rcb->rcbAllocatorColor), &(rc->rcColors),
					    col->rgb8Red,
					    col->rgb8Green,
					    col->rgb8Blue ) )
	{ LDEB(1); return- 1;	}

    if  ( appColorRgb( &(rcb->rcbAllocatorColorX), &(rc->rcColors),
					    255- col->rgb8Red,
					    255- col->rgb8Green,
					    255- col->rgb8Blue ) )
	{ LDEB(1); return- 1;	}

    rcb->rcbColorAllocated= 1;

    return 0;
    }

static void appDrawRgbColorFace(	RgbCube *		rc,
					RgbColorBlock *		rcb,
					void *			through,
					int			square,
					double			scale,
					const double		xo[8],
					const double		yo[8],
					int			canSplit,
					const int		nodes[4] )
    {
    AppDrawingData *	add= (AppDrawingData *)through;
    int			linewidth;

    APP_POINT		points[5];

    points[0].x= scale* xo[nodes[0]]+ square/ 2;
    points[1].x= scale* xo[nodes[1]]+ square/ 2;
    points[2].x= scale* xo[nodes[2]]+ square/ 2;
    points[3].x= scale* xo[nodes[3]]+ square/ 2;

    points[0].y= square/ 2- scale* yo[nodes[0]];
    points[1].y= square/ 2- scale* yo[nodes[1]];
    points[2].y= square/ 2- scale* yo[nodes[2]];
    points[3].y= square/ 2- scale* yo[nodes[3]];

    points[4]= points[0];

    if  ( ! rcb->rcbColorAllocated )
	{
	if  ( appRgbCubeAllocateBlockColor( rc, rcb ) )
	    { LDEB(1); return;	}
	}

    appDrawSetForegroundColor( add, &(rcb->rcbAllocatorColor) );

    appDrawFillPolygon( add, points, 4 );

    if  ( rcb->rcbSelected )
	{ linewidth= 3; }
    else{ linewidth= 1;	}

    appDrawSetForegroundColor( add, &(rcb->rcbAllocatorColorX) );

    appDrawSetLineAttributes( add, linewidth,
				LINEstyleSOLID, LINEcapBUTT, LINEjoinMITER,
				(const unsigned char *)0, 0 );
    appDrawDrawLines( add, points, 4 );

    return;
    }

typedef struct BlockFace
    {
    const int *		bfNodes;
    int			bfOnOutside;
    int			bfDrawCandidate;
    int			bfSplitCandidate;
    } BlockFace;

static void appHandleRgbColorBlock(	double			k,
					int			square,
					double			scale,
					RgbCube *		rc,
					void *			through,
					HANDLE_FACE		handleFace,
					RgbColorBlock *		rcb )
    {
    /*
	  6----7
	 /:   /|
	4-+--5 |
	| 2..|.3
	|/   |/
	0----1

    Counter clockwise, seen from the outside:
    */

    static const int front [4]=	{ 0, 1, 5, 4 };
    static const int back  [4]=	{ 3, 2, 6, 7 };
    static const int left  [4]=	{ 2, 0, 4, 6 };
    static const int right [4]=	{ 1, 3, 7, 5 };
    static const int bottom[4]=	{ 2, 3, 1, 0 };
    static const int top   [4]=	{ 4, 5, 7, 6 };

    typedef enum fc { fr, ba, le, ri, bo, to } fc;

    BlockFace	faces[6]=
		    {
		       { front,		},
		       { back,		},
		       { left,		},
		       { right,		},
		       { bottom,	},
		       { top,		},
		    };

    double	xi[8];
    double	yi[8];
    double	zi[8];

    double	xp[8];
    double	yp[8];
    double	zp[8];

    double	xo[8];
    double	yo[8];

    int		i;

    for ( i= 0; i < 4; i++ )
	{
	zi[front [i]]= rcb->rcbZ0;
	zi[back  [i]]= rcb->rcbZ1;
	xi[left  [i]]= rcb->rcbX0;
	xi[right [i]]= rcb->rcbX1;
	yi[bottom[i]]= rcb->rcbY0;
	yi[top   [i]]= rcb->rcbY1;
	}

    faces[fr].bfOnOutside= rcb->rcbRgbColor.rgb8Blue == 0;	/*  front */
    faces[ba].bfOnOutside= rcb->rcbRgbColor.rgb8Blue == 255;	/*  back */
    faces[le].bfOnOutside= rcb->rcbRgbColor.rgb8Red == 0;	/*  left */
    faces[ri].bfOnOutside= rcb->rcbRgbColor.rgb8Red == 255;	/*  right */
    faces[bo].bfOnOutside= rcb->rcbRgbColor.rgb8Green == 0;	/*  bottom */
    faces[to].bfOnOutside= rcb->rcbRgbColor.rgb8Green == 255;	/*  top */


    for ( i= 0; i < 6; i++ )
	{
	faces[i].bfDrawCandidate= faces[i].bfOnOutside;
	faces[i].bfSplitCandidate=
			faces[i].bfOnOutside &&
			( rc->rcSplitColor != RCsplitNONE );
	}

    if  ( rc->rcSplitColor == RCsplitRED )
	{
	/*  left  */
	if  ( rcb->rcbRgbColor.rgb8Red == rc->rcSplitValues.rgb8Red )
	    { faces[le].bfDrawCandidate= 1;	}

	if  ( rcb->rcbRgbColor.rgb8Red == 0	)
	    { faces[le].bfSplitCandidate= 0;	}

	/*  right  */
	if  ( rcb->rcbRgbColor.rgb8Red == rc->rcBSplitValues.rgb8Red )
	    { faces[ri].bfDrawCandidate= 1; }

	if  ( rcb->rcbRgbColor.rgb8Red == 255	)
	    { faces[ri].bfSplitCandidate= 0;	}
	}

    if  ( rc->rcSplitColor == RCsplitGREEN )
	{
	/*  bottom  */
	if  ( rcb->rcbRgbColor.rgb8Green == rc->rcSplitValues.rgb8Green )
	    { faces[bo].bfDrawCandidate= 1;	}

	if  ( rcb->rcbRgbColor.rgb8Green == 0	)
	    { faces[bo].bfSplitCandidate= 0;	}

	/*  top  */
	if  ( rcb->rcbRgbColor.rgb8Green == rc->rcBSplitValues.rgb8Green )
	    { faces[to].bfDrawCandidate= 1;	}

	if  ( rcb->rcbRgbColor.rgb8Green == 255	)
	    { faces[to].bfSplitCandidate= 0;	}
	}

    if  ( rc->rcSplitColor == RCsplitBLUE )
	{
	/*  front  */
	if  ( rcb->rcbRgbColor.rgb8Blue == rc->rcSplitValues.rgb8Blue )
	    { faces[fr].bfDrawCandidate= 1;	}

	if  ( rcb->rcbRgbColor.rgb8Blue == 0	)
	    { faces[fr].bfSplitCandidate= 0;	}

	/*  back  */
	if  ( rcb->rcbRgbColor.rgb8Blue == rc->rcBSplitValues.rgb8Blue )
	    { faces[ba].bfDrawCandidate= 1;	}

	if  ( rcb->rcbRgbColor.rgb8Blue == 255	)
	    { faces[ba].bfSplitCandidate= 0;	}
	}

    for ( i= 0; i < 8; i++ )
	{
	xp[i]= AT3_X( xi[i], yi[i], zi[i], &(rc->rcAt) );
	yp[i]= AT3_Y( xi[i], yi[i], zi[i], &(rc->rcAt) );
	zp[i]= AT3_Z( xi[i], yi[i], zi[i], &(rc->rcAt) );
	}

    for ( i= 0; i < 8; i++ )
	{
	xo[i]= xp[i]/ ( ( k- zp[i] )/ k+ 1.0 );
	yo[i]= yp[i]/ ( ( k- zp[i] )/ k+ 1.0 );
	}

    for ( i= 0; i < 6; i++ )
	{
	const int *	nodes= faces[i].bfNodes;

	double		ax, bx;
	double		ay, by;
	double		nz;

	if  ( ! faces[i].bfDrawCandidate )
	    { continue;	}

	ax= xp[nodes[3]]- xp[nodes[0]];
	ay= yp[nodes[3]]- yp[nodes[0]];
	/* az= zp[nodes[3]]- zp[nodes[0]]; */

	bx= xp[nodes[1]]- xp[nodes[0]];
	by= yp[nodes[1]]- yp[nodes[0]];
	/* bz= zp[nodes[1]]- zp[nodes[0]]; */

	/*
	nx= ax* bz- az* bx;
	ny= az* bx- ax* bz;
	*/
	nz= ax* by- ay* bx;

	if  ( nz > 0 )
	    {
	    (*handleFace)( rc, rcb,
			    through, square, scale, xo, yo,
			    faces[i].bfSplitCandidate,
			    nodes );
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Redraw the whole RGB cube.						*/
/*									*/
/************************************************************************/

void appRedrawRgbCube(	RgbCube *			rc,
			int				wide,
			int				high,
			const DocumentRectangle *	drClip,
			AppDrawingData *		add )
    {
    double		k= RGB_K;

    int			i;
    RgbColorBlock *	rcb;

    int			square;
    double		scale;

    appRgbCubeGeometry( &scale, &square, rc, wide, high );

    appDrawSetForegroundColor( add, &(rc->rcBackColor) );

    appDrawFillRectangle( add, drClip->drX0, drClip->drY0,
					    drClip->drX1- drClip->drX0+ 1,
					    drClip->drY1- drClip->drY0+ 1 );

    if  ( rc->rcColorSelected >= 0 )
	{
	DocumentRectangle	drSel;

	drSel.drX0= ( 5* wide )/ 6;
	drSel.drY0= ( 5* high )/ 6;
	drSel.drX1= wide- 6;
	drSel.drY1= high- 6;

	if  ( wide > high )
	    { drSel.drY0= 6;	}
	else{ drSel.drX0= 6;	}

	if  ( geoIntersectRectangle( &drSel, &drSel, drClip ) )
	    {
	    if  ( ! rc->rcColorAllocated )
		{
		if  ( appColorRgb( &(rc->rcAllocatedColor), &(rc->rcColors),
					    rc->rcSelectedColor.rgb8Red,
					    rc->rcSelectedColor.rgb8Green,
					    rc->rcSelectedColor.rgb8Blue ) )
		    { LDEB(1); return;	}

		rc->rcColorAllocated= 1;
		}

	    appDrawSetForegroundColor( add, &(rc->rcAllocatedColor) );

	    appDrawFillRectangle( add, drSel.drX0, drSel.drY0,
						drSel.drX1- drSel.drX0+ 1,
						drSel.drY1- drSel.drY0+ 1 );
	    }
	}

    rcb= rc->rcColorBlocks;
    for ( i= 0; i < rc->rcColorBlockCount; rcb++, i++ )
	{
	appHandleRgbColorBlock( k, square, scale, rc,
				    (void *)add, appDrawRgbColorFace, rcb );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Find out what color is clicked.					*/
/*									*/
/************************************************************************/

int appRgbCubeFindColor(RGB8Color *			rgb8,
			int *				pOnOutside,
			RgbCube *			rc,
			int				mouseX,
			int				mouseY,
			int				wide,
			int				high )
    {
    double		k= RGB_K;

    int			i;
    RgbColorBlock *	rcb;

    int			square;
    double		scale;

    FindColor		fc;

    appRgbCubeGeometry( &scale, &square, rc, wide, high );

    fc.fcFound= 0;
    fc.fcPoint.p2diX= mouseX;
    fc.fcPoint.p2diY= mouseY;

    rcb= rc->rcColorBlocks;
    for ( i= 0; i < rc->rcColorBlockCount; rcb++, i++ )
	{
	appHandleRgbColorBlock( k, square, scale, rc,
				    (void *)&fc, appTryRgbColorFace, rcb );
	}

    if  ( ! fc.fcFound )
	{ return -1;	}

    *rgb8= fc.fcRgbColor;
    *pOnOutside= fc.fcSplitCandidate;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Calculate coordinates of an individual cube in the block.		*/
/*									*/
/************************************************************************/

static void appRgbColorBlockSetC(	double *	pC0,
					double *	pC1,
					int		c,
					int		split,
					int		splitValue,
					int		step )
    {
    const int		size= 256+ 2* RGB_GAP;

    const double	f= 100;
    const double	f0=  0;
    const double	f1= ( f- f0 );

    double		c0= c;
    double		c1= c +step;

    if  ( split )
	{
	if  ( c0 < splitValue )
	    { c0 -= RGB_GAP; c1 -= RGB_GAP; }
	else{ c0 += RGB_GAP; c1 += RGB_GAP; }
	}

    c0 -= size/ 2;
    c1 -= size/ 2;

    *pC0= ( f1* c0+ f0* c1 )/ f;
    *pC1= ( f1* c1+ f0* c0 )/ f;

    return;
    }

static void appPlaceRgbColorBlock(	RgbCube *	rc,
					RgbColorBlock *	rcb )
    {
    appRgbColorBlockSetC( &(rcb->rcbX0), &(rcb->rcbX1),
					rcb->rcbRgbColor.rgb8Red,
					rc->rcSplitColor == RCsplitRED,
					rc->rcSplitValues.rgb8Red,
					rc->rcRedStep );

    appRgbColorBlockSetC( &(rcb->rcbY0), &(rcb->rcbY1),
					rcb->rcbRgbColor.rgb8Green,
					rc->rcSplitColor == RCsplitGREEN,
					rc->rcSplitValues.rgb8Green,
					rc->rcGreenStep );

    appRgbColorBlockSetC( &(rcb->rcbZ0), &(rcb->rcbZ1),
					rcb->rcbRgbColor.rgb8Blue,
					rc->rcSplitColor == RCsplitBLUE,
					rc->rcSplitValues.rgb8Blue,
					rc->rcBlueStep );

    return;
    }

/************************************************************************/
/*									*/
/*  Sort the color blocks in the Z direction. As perspective is not	*/
/*  from a point of view very nearby, this works.			*/
/*									*/
/************************************************************************/

static int appRgbCubeZCmp(	const void *	vrcb1,
				const void *	vrcb2 )
    {
    const RgbColorBlock *	rcb1= (const RgbColorBlock *)vrcb1;
    const RgbColorBlock *	rcb2= (const RgbColorBlock *)vrcb2;

    if  ( rcb1->rcbZ < rcb2->rcbZ )
	{ return -1;	}
    if  ( rcb1->rcbZ > rcb2->rcbZ )
	{ return +1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Recalculate the blocks of an RGB Cube.				*/
/*									*/
/************************************************************************/

static void appRgbCubeCalculateBlocks(	RgbCube *	rc )
    {
    int			i;
    RgbColorBlock *	rcb;

    rcb= rc->rcColorBlocks;
    for ( i= 0; i < rc->rcColorBlockCount; rcb++, i++ )
	{
	appPlaceRgbColorBlock( rc, rcb );

	rcb->rcbZ=
	    AT3_Z( rcb->rcbX0, rcb->rcbY0, rcb->rcbZ0, &(rc->rcAt) );
	}

    qsort( rc->rcColorBlocks, rc->rcColorBlockCount,
				    sizeof(RgbColorBlock), appRgbCubeZCmp );

    rc->rcColorSelected= -1;
    rcb= rc->rcColorBlocks;
    for ( i= 0; i < rc->rcColorBlockCount; rcb++, i++ )
	{
	const RGB8Color *	rgb8= &(rcb->rcbRgbColor);

	int		rr= rgb8->rgb8Red/ rc->rcRedStep;
	int		gg= rgb8->rgb8Green/ rc->rcGreenStep;
	int		bb= rgb8->rgb8Blue/ rc->rcBlueStep;

	int		n;

	n= 0;
	n += rc->rcGreenSteps* rc->rcBlueSteps* rr;
	n += rc->rcBlueSteps* gg;
	n += bb;

	rc->rcColorBlocks[n].rcbMappedIndex= i;

	if  ( rcb->rcbSelected )
	    { rc->rcColorSelected= i;	}
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Rotate an RGB cube. i.e. Set a new affine transform.		*/
/*									*/
/************************************************************************/

void appRotateRgbCube(		RgbCube *		rc,
				int			mouseX,
				int			mouseY,
				int			wide,
				int			high )
    {
    AffineTransform3D	at;
    double		a_yz;
    double		a_xz;

    int			square;
    double		scale;

    double		d;

    appRgbCubeGeometry( &scale, &square, rc, wide, high );

    mouseX= mouseX- square/ 2;
    mouseY= mouseY- square/ 2;

    d= sqrt( 1.0* mouseX* mouseX+ 1.0* mouseY* mouseY );
    a_xz= -mouseX/ d;
    a_yz= +mouseY/ d;

    a_xz *= M_PI/2.0;
    a_yz *= M_PI/2.0;

    a_xz /= 10;
    a_yz /= 10;

    utilYZRotationAffineTransform3D( &at, a_yz );
    utilAffineTransform3DProduct( &(rc->rcAt), &at, &(rc->rcAt) );

    utilXZRotationAffineTransform3D( &at,  a_xz );
    utilAffineTransform3DProduct( &(rc->rcAt), &at, &(rc->rcAt) );

    appRgbCubeCalculateBlocks( rc );

    return;
    }

void appRgbCubeSetSplit(	RgbCube *		rc,
				int			splitColor,
				const RGB8Color *	splitValues )
    {
    int		n;

    switch( splitColor )
	{
	case RCsplitNONE:
	    rc->rcSplitColor= splitColor;
	    break;

	case RCsplitRED:
	    n= splitValues->rgb8Red/ rc->rcRedStep;
	    if  ( n < 1 )
		{ n= 1;	}
	    if  ( n >= rc->rcRedSteps )
		{ n=   rc->rcRedSteps- 1;	}

	    rc->rcSplitColor= splitColor;
	    rc->rcSplitValues.rgb8Red= ( 255* n )/ ( rc->rcRedSteps- 1 );
	    rc->rcBSplitValues.rgb8Red= ( 255* (n-1) )/ ( rc->rcRedSteps- 1 );
	    break;

	case RCsplitGREEN:
	    n= splitValues->rgb8Green/ rc->rcGreenStep;
	    if  ( n < 1 )
		{ n= 1;	}
	    if  ( n >= rc->rcGreenSteps )
		{ n=   rc->rcGreenSteps- 1;	}

	    rc->rcSplitColor= splitColor;
	    rc->rcSplitValues.rgb8Green= ( 255* n )/ ( rc->rcGreenSteps- 1 );
	    rc->rcBSplitValues.rgb8Green= ( 255* (n-1) )/
						    ( rc->rcGreenSteps- 1 );
	    break;

	case RCsplitBLUE:
	    n= splitValues->rgb8Blue/ rc->rcBlueStep;
	    if  ( n < 1 )
		{ n= 1;	}
	    if  ( n >= rc->rcBlueSteps )
		{ n=   rc->rcBlueSteps- 1;	}

	    rc->rcSplitColor= splitColor;
	    rc->rcSplitValues.rgb8Blue= ( 255* n )/ ( rc->rcBlueSteps- 1 );
	    rc->rcBSplitValues.rgb8Blue= ( 255* (n-1) )/ ( rc->rcBlueSteps- 1 );
	    break;

	default:
	    LDEB(splitColor); return;
	}

    appRgbCubeCalculateBlocks( rc );

    return;
    }

void appRgbCubeRefreshSplit(	RgbCube *		rc,
				const RGB8Color *	rgb8 )
    {
    RGB8Color	rgb8Set;

    rgb8Set= *rgb8;

    if  ( rc->rcSplitColor == RCsplitRED )
	{
	if  ( rgb8Set.rgb8Red == rc->rcSplitValues.rgb8Red	&&
	      rgb8Set.rgb8Red+ rc->rcRedStep <= 255		)
	    { rgb8Set.rgb8Red += rc->rcRedStep; }

	appRgbCubeSetSplit( rc, rc->rcSplitColor, &rgb8Set );
	return;
	}

    if  ( rc->rcSplitColor == RCsplitGREEN )
	{
	if  ( rgb8Set.rgb8Green == rc->rcSplitValues.rgb8Green	&&
	      rgb8Set.rgb8Green+ rc->rcGreenStep <= 255		)
	    { rgb8Set.rgb8Green += rc->rcGreenStep; }

	appRgbCubeSetSplit( rc, rc->rcSplitColor, &rgb8Set );
	return;
	}

    if  ( rc->rcSplitColor == RCsplitBLUE )
	{
	if  ( rgb8Set.rgb8Blue == rc->rcSplitValues.rgb8Blue	&&
	      rgb8Set.rgb8Blue+ rc->rcBlueStep <= 255		)
	    { rgb8Set.rgb8Blue += rc->rcBlueStep; }

	appRgbCubeSetSplit( rc, rc->rcSplitColor, &rgb8Set );
	return;
	}

    return;
    }

void appRgbCubeSelectColor(	RgbCube *		rc,
				const RGB8Color *	rgb8 )
    {
    int		rr= rgb8->rgb8Red/ rc->rcRedStep;
    int		gg= rgb8->rgb8Green/ rc->rcGreenStep;
    int		bb= rgb8->rgb8Blue/ rc->rcBlueStep;

    int		n;

    n= 0;
    n += rc->rcGreenSteps* rc->rcBlueSteps* rr;
    n += rc->rcBlueSteps* gg;
    n += bb;

    n= rc->rcColorBlocks[n].rcbMappedIndex;

    if  ( rc->rcColorSelected >= 0 )
	{ rc->rcColorBlocks[rc->rcColorSelected].rcbSelected= 0;	}

    rc->rcColorBlocks[n].rcbSelected= 1;

    rc->rcSelectedColor= *rgb8;
    rc->rcColorSelected= n;
    rc->rcColorAllocated= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Prepare an RGB cube for drawing.					*/
/*									*/
/************************************************************************/

int appPrepareRgbCube(	RgbCube *			rc,
			AppDrawingData *		add,
			int				redSteps,
			int				greenSteps,
			int				blueSteps )
    {
    int			rr;
    int			gg;
    int			bb;

    int			r= 84;
    int			g= 84;
    int			b= 84;

    int			i;

    int			count;
    RgbColorBlock *	fresh;

    if  ( appAllocateColors( add, &(rc->rcColors) ) )
	{ LDEB(1); return -1;	}

    if  ( appColorRgb( &(rc->rcBackColor), &(rc->rcColors), r, g, b ) )
	{ LLLDEB(r,g,b); return -1;	}

    rc->rcColorSelected= -1;
    rc->rcColorAllocated= 0;

    count= redSteps* greenSteps* blueSteps;

    fresh= realloc( rc->rcColorBlocks, count* sizeof( RgbColorBlock ) );
    if  ( ! fresh )
	{ LXDEB(count,fresh); return -1;	}

    rc->rcColorBlocks= fresh;
    rc->rcColorBlockCount= count;

    rc->rcRedSteps= redSteps;
    rc->rcGreenSteps= greenSteps;
    rc->rcBlueSteps= blueSteps;

    rc->rcRedStep= 255/ ( redSteps- 1 );
    rc->rcGreenStep= 255/ ( greenSteps- 1 );
    rc->rcBlueStep= 255/ ( blueSteps- 1 );

    rc->rcSplitValues.rgb8Red= rc->rcRedStep* ( 128/ rc->rcRedStep );
    rc->rcSplitValues.rgb8Green= rc->rcGreenStep* ( 128/ rc->rcGreenStep );
    rc->rcSplitValues.rgb8Blue= rc->rcBlueStep* ( 128/ rc->rcBlueStep );

    rc->rcSplitColor= RCsplitNONE;

    utilIdentityAffineTransform3D( &(rc->rcAt) );

    /**/
    i= 0; fresh= rc->rcColorBlocks;
    for ( rr= 0; rr < redSteps; rr++ )
	{
	r= ( 255* rr )/ ( redSteps- 1 );

	for ( gg= 0; gg < greenSteps; gg++ )
	    {
	    g= ( 255* gg )/ ( greenSteps- 1 );

	    for ( bb= 0; bb < blueSteps; bb++ )
		{
		b= ( 255* bb )/ ( blueSteps- 1 );

		if  ( i >= rc->rcColorBlockCount )
		    { LLDEB(i,rc->rcColorBlockCount); return -1;	}

		fresh->rcbRgbColor.rgb8Red= r;
		fresh->rcbRgbColor.rgb8Green= g;
		fresh->rcbRgbColor.rgb8Blue= b;
		fresh->rcbRgbColor.rgb8Alpha= 255;

		fresh->rcbColorAllocated= 0;
		fresh->rcbSelected= 0;

		fresh++; i++;
		}
	    }
	}

    /**/
    {
    double		a_yz_0= -asin( 1.0/ sqrt( 3.0 ) );
    AffineTransform3D	at;

    double		angleXZ= ( -3* M_PI )/ 4;
    double		angleYZ= a_yz_0;
    double		angleXY= (  0* M_PI )/ 6;

    utilXZRotationAffineTransform3D( &(rc->rcAt), angleXZ );

    utilYZRotationAffineTransform3D( &at, angleYZ );
    utilAffineTransform3DProduct( &(rc->rcAt), &at, &(rc->rcAt) );

    utilXYRotationAffineTransform3D( &at, angleXY );
    utilAffineTransform3DProduct( &(rc->rcAt), &at, &(rc->rcAt) );
    }

    /**/
    appRgbCubeCalculateBlocks( rc );

    return 0;
    }
