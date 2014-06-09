/************************************************************************/
/*									*/
/*  Color mangement on X11: Try to get around the limitations.		*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	"appDraw.h"

#   include	<appDebugon.h>

#   ifdef	USE_MOTIF

/************************************************************************/
/*									*/
/*  Visual class names for debugging.					*/
/*									*/
/************************************************************************/

const char * const APP_VisualClasses[]=
    {
    "StaticGray",
    "GrayScale",
    "StaticColor",
    "PseudoColor",
    "TrueColor",
    "DirectColor",
    };

/************************************************************************/
/*									*/
/*  Allocate approximate colors.					*/
/*									*/
/************************************************************************/

static int appColorAllocSystemColor(	AllocatorColor *	acRes,
					const AllocatorColor *	acScratch,
					AppColors *		acSys )
    {
    XColor		xc;

    xc.red= acScratch->acRed;
    xc.green= acScratch->acGreen;
    xc.blue= acScratch->acBlue;
    xc.flags= DoRed | DoGreen | DoBlue;

    if  ( ! XAllocColor( acSys->acDisplay, acSys->acColormap, &xc ) )
	{ return -1;	}

    acRes->acAllocated= AC_ALLOCATED;
    acRes->acRed= xc.red;
    acRes->acGreen= xc.green;
    acRes->acBlue= xc.blue;
    acRes->acColorNumber= xc.pixel;

    return 0;
    }

static int appColorRgb111(	AllocatorColor *	ac,
				ColorAllocator *	ca,
				unsigned int		r,
				unsigned int		g,
				unsigned int		b )
    {
    AllocatorColor	acScratch;
    AllocatorColor *	acRes;

    bmColorRgbDirect( &acScratch, ca, r, g, b );

    acRes= &(ca->ca222Colors[acScratch.acColorNumber]);

    if  ( acRes->acAllocated == AC_UNALLOCATED )
	{
	AppColors *	acSys= (AppColors *)ca->caSystemPrivate;

	if  ( appColorAllocSystemColor( acRes, &acScratch, acSys ) )
	    { LDEB(acScratch.acColorNumber); return -1;	}
	}

    *ac= *acRes;
    ac->acAllocated= AC_COPIED;

    return 0;
    }

static int appColorRgb222(	AllocatorColor *	ac,
				ColorAllocator *	ca,
				unsigned int		r,
				unsigned int		g,
				unsigned int		b )
    {
    AllocatorColor	acScratch;
    AllocatorColor *	acRes;

    bmColorRgbDirect( &acScratch, ca, r, g, b );

    acRes= &(ca->ca222Colors[acScratch.acColorNumber]);

    if  ( acRes->acAllocated == AC_UNALLOCATED )
	{
	AppColors *	acSys= (AppColors *)ca->caSystemPrivate;

	if  ( appColorAllocSystemColor( acRes, &acScratch, acSys ) )
	    { LDEB(acScratch.acColorNumber); return -1;	}
	}

    *ac= *acRes;
    ac->acAllocated= AC_COPIED;

    return 0;
    }

static int appColorRgb555(	AllocatorColor *	ac,
				ColorAllocator *	ca,
				unsigned int		r,
				unsigned int		g,
				unsigned int		b )
    {
    AllocatorColor	acScratch;
    AllocatorColor *	acRes;

    bmColorRgbDirect( &acScratch, ca, r, g, b );

    acRes= &(ca->caColors[acScratch.acColorNumber]);

    if  ( acRes->acAllocated == AC_UNALLOCATED )
	{
	AppColors *	acSys= (AppColors *)ca->caSystemPrivate;

	if  ( appColorAllocSystemColor( acRes, &acScratch, acSys ) )
	    {
	    if  ( bmFindNearestColorRgb( ac, ca, r, g, b ) )
		{ LDEB(1); return -1;	}

	    return 0;
	    }
	}

    *ac= *acRes;
    ac->acAllocated= AC_COPIED;

    return 0;
    }

static int appColorRgb332(	AllocatorColor *	ac,
				ColorAllocator *	ca,
				unsigned int		r,
				unsigned int		g,
				unsigned int		b )
    {
    AllocatorColor	acScratch;
    AllocatorColor *	acRes;

    bmColorRgbDirect( &acScratch, ca, r, g, b );

    acRes= &(ca->caColors[acScratch.acColorNumber]);

    if  ( acRes->acAllocated == AC_UNALLOCATED )
	{
	AppColors *	acSys= (AppColors *)ca->caSystemPrivate;

	if  ( appColorAllocSystemColor( acRes, &acScratch, acSys ) )
	    {
	    if  ( bmFindNearestColorRgb( ac, ca, r, g, b ) )
		{ LDEB(1); return -1;	}

	    return 0;
	    }
	}

    *ac= *acRes;
    ac->acAllocated= AC_COPIED;

    return 0;
    }

int appColorRgb(	XColor *		xc,
			AppColors *		acSys,
			unsigned int		r,
			unsigned int		g,
			unsigned int		b )
    {
    AllocatorColor	ac;
    ColorAllocator *	ca= &(acSys->acAllocator);

    if  ( acSys->acVisualClass ==	TrueColor	||
	  acSys->acVisualClass ==	DirectColor	)
	{
	bmColorRgbDirect( &ac, ca, r, g, b );
	}
    else{
	if  ( ! ca->caSystemAllocator )
	    { XDEB(ca->caSystemAllocator); return -1;	}

	if  ( (*ca->caSystemAllocator)( &ac, ca, r, g, b ) )
	    { LLLDEB(r,g,b); return -1;	}
	}

    xc->red= ac.acRed;
    xc->green= ac.acGreen;
    xc->blue= ac.acBlue;
    xc->pixel= ac.acColorNumber;

    return 0;
    }

int appColorNamed(	XColor *		xc,
			AppColors *		acSys,
			const char *		name )
    {
    XColor	exact;
    XColor	screen;

    if  ( ! XLookupColor( acSys->acDisplay, acSys->acColormap, name,
							&exact, &screen ) )
	{ SDEB(name); return -1;	}

    return appColorRgb( xc, acSys,
			exact.red/257, exact.green/257, exact.blue/257 );
    }

/************************************************************************/
/*									*/
/*  Allocate colors for an image.					*/
/*									*/
/************************************************************************/

static int appPixCmp(		const void *	vpix1,
				const void *	vpix2 )
    {
    Pixel	pix1= *(Pixel *)vpix1;
    Pixel	pix2= *(Pixel *)vpix2;

    if  ( pix1 > pix2 )
	{ return  1;	}
    if  ( pix1 < pix2 )
	{ return -1;	}

    return 0;
    }

static void appTry222Color(	Display *		display,
				Colormap		cmap,
				ColorAllocator *	ca,
				AppColors *		acSys,
				double			dists[64],
				Pixel			l )
    {
    XColor		xc;
    int			col222;

    xc.pixel= l;
    xc.red= xc.green= xc.blue= 0;

    XQueryColor( display, cmap, &xc );

    col222= C222( (int)(xc.red/256), (int)(xc.green/256), (int)(xc.blue/256) );

    if  ( ca->ca222Colors[col222].acAllocated == AC_ALLOCATED )
	{
	double		c;
	double		d= 0;

	c= xc.red-   (256*256-1)* ( ( ( col222 & 0x30 ) >> 4 ) / 3 ); d += c* c;
	c= xc.green- (256*256-1)* ( ( ( col222 & 0x0c ) >> 2 ) / 3 ); d += c* c;
	c= xc.blue-  (256*256-1)* ( ( ( col222 & 0x03 ) >> 0 ) / 3 ); d += c* c;

	if  ( sqrt( d ) > dists[col222] )
	    { return;	}
	}

    if  ( XAllocColor( display, cmap, &xc )	)
	{
	if  ( xc.pixel == l )
	    {
	    if  ( ca->ca222Colors[col222].acAllocated == AC_UNALLOCATED )
		{
		ca->ca222Colors[col222].acAllocated= AC_ALLOCATED;
		ca->ca222Colors[col222].acRed= xc.red;
		ca->ca222Colors[col222].acGreen= xc.green;
		ca->ca222Colors[col222].acBlue= xc.blue;
		ca->ca222Colors[col222].acColorNumber= xc.pixel;
		}
	    else{ XFreeColors( display, cmap, &(xc.pixel), 1, 0L ); }
	    }
	else{ XFreeColors( display, cmap, &(xc.pixel), 1, 0L ); }
	}
    }

int appAllocateColors(	AppDrawingData *	add,
			AppColors *		acSys )
    {
    Display *		display= add->addDisplay;
    int			screen= DefaultScreen( display );
    int			depth= DefaultDepth( display, screen );
    Colormap		cmap= DefaultColormap( display, screen );
    Visual *		vis= DefaultVisual( display, screen );

    XVisualInfo		visualInfo;
    XVisualInfo *	visuals;

    int			count;

    double		d[64];

    int			l, m, r;
    Pixel *		pxx;

    ColorAllocator *	ca= &(acSys->acAllocator);

    visualInfo.visualid= XVisualIDFromVisual( vis );
    visuals= XGetVisualInfo( display, VisualIDMask, &visualInfo, &count );
    if  ( count != 1 )
	{ LDEB(count); return -1;	}
    visualInfo= *visuals;
    XFree( visuals );

    acSys->acDisplay= display;
    acSys->acColormap= cmap;
    ca->caSystemPrivate= acSys;

#   ifdef __cplusplus
    acSys->acVisualClass= visualInfo.c_class;
#   else
    acSys->acVisualClass= visualInfo.class;
#   endif

    ca->caDepth= depth;

    switch( acSys->acVisualClass )
	{
	case StaticGray:
	case GrayScale:
	    ca->caAllocationType= CA_INDEX_ALLOC;
	    switch( ca->caDepth )
		{
		case 1: case 2: case 4: case 8:
		    break;
		default:
		    SLDEB(APP_VisualClasses[acSys->acVisualClass],ca->caDepth);
		}
	    break;

	case StaticColor:
	    SDEB(APP_VisualClasses[acSys->acVisualClass]);

	    ca->caAllocationType= CA_INDEX_ALLOC;
	    break;

	case PseudoColor:

	    ca->caAllocationType= CA_INDEX_ALLOC;
	    break;

	case TrueColor:
	case DirectColor:
	    ca->caAllocationType= CA_CALCULATED;

	    bmSetCalculatedShifts( ca, visualInfo.red_mask,
			    visualInfo.green_mask, visualInfo.blue_mask );

	    return 0;
	}

    switch( ca->caDepth )
	{
	case  1:
	    count= 2;
	    ca->caSystemAllocator= appColorRgb111;
	    bmSetCalculatedShifts( ca, 0x0004, 0x0002, 0x0001 );
	    break;

	case  2:
	    count= 4;
	    ca->caSystemAllocator= appColorRgb111;
	    bmSetCalculatedShifts( ca, 0x0004, 0x0002, 0x0001 );
	    break;

	case  4:
	    count= 16;
	    ca->caSystemAllocator= appColorRgb222;
	    bmSetCalculatedShifts( ca, 0x0030, 0x000c, 0x0003 );
	    break;

	case  8:
	    count= 256;
	    ca->caSystemAllocator= appColorRgb332;
	    bmSetCalculatedShifts( ca, 0x00e0, 0x001c, 0x0003 );
	    break;

	case 16:
	    count= 256* 256;
	    ca->caSystemAllocator= appColorRgb555;
	    bmSetCalculatedShifts( ca, 0x7c00, 0x03e0, 0x001f );
	    break;

	case 24:
	case 32:
	    return 0;

	default:
	    LDEB(depth); return -1;
	}

    for ( l= 0; l < 64; l++ )
	{ d[l]= 300000;	}

    if  ( bmAllocateAllocatorColors( ca, count ) )
	{ LDEB(count); return -1;	}

    XGrabServer( display );

    l= 0; r= count; m= ( l+ r )/2;
    pxx= (Pixel *)malloc( count* sizeof( Pixel ) );
    while( l < m )
	{
	if  ( ! XAllocColorCells( display, cmap, False,
						(unsigned long *)0, 0,
						pxx, (unsigned)m ) )
	    { r= m; }
	else{
	    XFreeColors( display, cmap, pxx, m, 0L );
	    l= m;
	    }

	m= ( l+ r )/2;
	}

    if  ( m > 0 )
	{
	if  ( ! XAllocColorCells( display, cmap, False,
					    (unsigned long *)0, 0,
					    pxx, (unsigned)m ) )
	    { LDEB(m); XUngrabServer( display ); return -1; }
	}

    XUngrabServer( display );

    qsort( pxx, m, sizeof(Pixel), appPixCmp );
    pxx[m]= count;

    l= 0; r= 0;
    if  ( m > 0 ) while( r <= m )
	{
	while( (unsigned)l < pxx[r] )
	    {
	    appTry222Color( display, cmap, ca, acSys, d, l );

	    l++;
	    }

	l++; r++;
	}
    else{
	if  ( count <= 256 )
	    {
	    for ( l= 0; l < count; l++ )
		{ appTry222Color( display, cmap, ca, acSys, d, l ); }
	    }
	}

    if  ( m > 0 )
	{
	XFreeColors( display, cmap, pxx, m, 0L ); }

    free( pxx );

    return 0;
    }

void appCleanColors(	AppColors *		acSys )
    {
    Display *		display= acSys->acDisplay;
    Colormap            cmap= acSys->acColormap;
    int			i;

    ColorAllocator *	ca= &(acSys->acAllocator);

    if  ( ! display )
	{ return;	}

    for ( i= 0; i < 64; i++ )
	{
	if  ( ca->ca222Colors[i].acAllocated == AC_ALLOCATED )
	    {
	    Pixel	pix= ca->ca222Colors[i].acColorNumber;

	    XFreeColors( display, cmap, &pix, 1, 0L );
	    ca->ca222Colors[i].acAllocated= AC_UNALLOCATED;
	    }
	}

    for ( i= 0; i < ca->caColorCount; i++ )
	{
	if  ( ca->caColors[i].acAllocated == AC_ALLOCATED )
	    {
	    Pixel	pix= ca->caColors[i].acColorNumber;

	    XFreeColors( display, cmap, &pix, 1, 0L );
	    ca->caColors[i].acAllocated= AC_UNALLOCATED;
	    }
	}

    bmCleanColorAllocator( ca );
    }

void appInitColors(	AppColors *	acSys )
    {
    ColorAllocator *	ca= &(acSys->acAllocator);

    bmInitColorAllocator( ca );

    acSys->acDisplay= (Display *)0;
    acSys->acColormap= None;
    }

int appColorFindRgb(	XColor *	xc,
			AppColors *	acSys,
			unsigned int	r,
			unsigned int	g,
			unsigned int	b )
    {
    ColorAllocator *	ca= &(acSys->acAllocator);
    AllocatorColor	acRet;

    if  ( bmFindNearestColorRgb( &acRet, ca, r, g, b ) )
	{ return -1;	}

    xc->red= acRet.acRed;
    xc->green= acRet.acGreen;
    xc->blue= acRet.acBlue;
    xc->pixel= acRet.acColorNumber;

    return 0;
    }

#   endif
