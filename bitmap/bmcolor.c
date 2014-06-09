#   include	"bitmapConfig.h"

#   include	<bmcolor.h>
#   include	<string.h>
#   include	<stdlib.h>

#   include	<math.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Color administration.						*/
/*									*/
/************************************************************************/

void bmSetCalculatedShifts(	ColorAllocator *	ca,
				unsigned long		redMask,
				unsigned long		greenMask,
				unsigned long		blueMask )
    {
    ca->caRedPixelShift= 0;
    while( ! ( redMask & 0x1 ) )
	{ ca->caRedPixelShift++; redMask= redMask >> 1;	}
    ca->caRedMask= redMask;
    ca->caRedApproxShift= 8;
    while( redMask & 0x1 )
	{ ca->caRedApproxShift--; redMask= redMask >> 1;	}

    ca->caGreenPixelShift= 0;
    while( ! ( greenMask & 0x1 ) )
	{ ca->caGreenPixelShift++; greenMask= greenMask >> 1;	}
    ca->caGreenMask= greenMask;
    ca->caGreenApproxShift= 8;
    while( greenMask & 0x1 )
	{ ca->caGreenApproxShift--; greenMask= greenMask >> 1;	}

    ca->caBluePixelShift= 0;
    while( ! ( blueMask & 0x1 ) )
	{ ca->caBluePixelShift++; blueMask= blueMask >> 1;	}
    ca->caBlueMask= blueMask;
    ca->caBlueApproxShift= 8;
    while( blueMask & 0x1 )
	{ ca->caBlueApproxShift--; blueMask= blueMask >> 1;	}

    return;
    }

void bmCleanColorAllocator(	ColorAllocator *	ca )
    {
    if  ( ca->caSystemCleanup )
	{ (*ca->caSystemCleanup)( ca ); }

    if  ( ca->caColors )
	{ free( ca->caColors ); ca->caColors= (AllocatorColor *)0;	}

    ca->caColorCount= 0;

    return;
    }

void bmInitColorAllocator(	ColorAllocator *	ca )
    {
    int		i;

    ca->caColorCount= 0;
    ca->caColors= (AllocatorColor *)0;

    for ( i= 0; i < 64; i++ )
	{ ca->ca222Colors[i].acAllocated= AC_UNALLOCATED;	}

    ca->caAllocationType= CA_UNKNOWN;
    ca->caSystemPrivate= (void *)0;
    ca->caSystemAllocator= (SystemAllocator)0;
    ca->caSystemCleanup= (SystemCleanup)0;

    return;
    }

/************************************************************************/
/*									*/
/*  Find a color that is already allocated and that will do when	*/
/*  else helps.								*/
/*									*/
/************************************************************************/

int bmFindNearestColorRgb(	AllocatorColor *	acRet,
				const ColorAllocator *	ca,
				unsigned int		r,
				unsigned int		g,
				unsigned int		b )
    {
    int			d;
    int			col;

    for ( d= 0; d < 128; d++ )
	{
	const AllocatorColor *	ac;

	ac= ca->ca222Colors;
	for ( col= 0; col < 64; ac++, col++ )
	    {
	    if  ( ac->acAllocated == AC_UNALLOCATED )
		{ continue;	}

	    if  ( ac->acRed	> 256* ( r- d )		&&
		  ac->acRed	< 256* ( r+ d )		&&
		  ac->acGreen	> 256* ( g- d )		&&
		  ac->acGreen	< 256* ( g+ d )		&&
		  ac->acBlue	> 256* ( b- d )		&&
		  ac->acBlue	< 256* ( b+ d )		)
		{
		*acRet= *ac;
		acRet->acAllocated= AC_COPIED;
		return 0;
		}
	    }

	ac= ca->caColors;
	for ( col= 0; col < ca->caColorCount; ac++, col++ )
	    {
	    if  ( ac->acAllocated == AC_UNALLOCATED )
		{ continue;	}

	    if  ( ac->acRed	> 256* ( r- d )		&&
		  ac->acRed	< 256* ( r+ d )		&&
		  ac->acGreen	> 256* ( g- d )		&&
		  ac->acGreen	< 256* ( g+ d )		&&
		  ac->acBlue	> 256* ( b- d )		&&
		  ac->acBlue	< 256* ( b+ d )		)
		{
		*acRet= *ac;
		acRet->acAllocated= AC_COPIED;
		return 0;
		}
	    }
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Allocate the palette for palette type allocation.			*/
/*									*/
/************************************************************************/

int bmAllocateAllocatorColors(	ColorAllocator *	ca,
				int			count )
    {
    int			i;
    AllocatorColor *	fresh;

    fresh= (AllocatorColor *)realloc( ca->caColors,
					count* sizeof(AllocatorColor) );
    if  ( ! fresh )
	{ LXDEB(count,fresh); return -1;	}

    ca->caColors= fresh;
    ca->caColorCount= count;

    for ( i= 0; i < count; i++ )
	{
	ca->caColors[i].acColorNumber= 0;
	ca->caColors[i].acAllocated= AC_UNALLOCATED;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a color in a palette. (Or just return its index)		*/
/*									*/
/*  1)  As a HACK return the index of the nearest color if the palette	*/
/*	is full.							*/
/*									*/
/************************************************************************/

int bmInsertColor(	RGB8Color **		pColors,
			int *			pColorCount,
			int			avoidZero,
			int			maxSize,
			const RGB8Color *	rgb8 )
    {
    int		color;
    int		colorCount= *pColorCount;
    RGB8Color *	colors;
    int		extra= 0;

    avoidZero= ( avoidZero != 0 );

    colors= *pColors+ avoidZero;
    for ( color= avoidZero; color < colorCount; colors++, color++ )
	{
	if  ( ! bmRGB8ColorsDiffer( colors, rgb8 ) )
	    { return color;	}
	}

    /*  1  */
    if  ( maxSize > 0 && colorCount >= maxSize )
	{
	int		d= 257;
	int		i;

	LLDEB(colorCount,maxSize);

	color= 0;
	for ( i= 0; i < colorCount; i++ )
	    {
	    int		dd= 0;
	    int		ddd;

	    ddd= rgb8->rgb8Red- colors[i].rgb8Red;
	    if  ( ddd < 0 )
		{ ddd= -ddd;	}
	    if  ( dd < ddd )
		{ dd=  ddd;	}

	    ddd= rgb8->rgb8Green- colors[i].rgb8Green;
	    if  ( ddd < 0 )
		{ ddd= -ddd;	}
	    if  ( dd < ddd )
		{ dd=  ddd;	}

	    ddd= rgb8->rgb8Blue- colors[i].rgb8Blue;
	    if  ( ddd < 0 )
		{ ddd= -ddd;	}
	    if  ( dd < ddd )
		{ dd=  ddd;	}

	    if  ( dd < d )
		{ d= dd; color= i;	}
	    }

	return color;
	}

    if  ( avoidZero && colorCount == 0 )
	{ extra= 1;	}

    colors= realloc( *pColors, ( extra+ colorCount+ 1 )* sizeof(RGB8Color) );
    if  ( ! colors )
	{ LXDEB(colorCount,colors); return -1; }
    *pColors= colors;

    if  ( avoidZero && colorCount == 0 )
	{
	bmInitRGB8Color( colors+ colorCount );
	colorCount++;
	}

    color= colorCount;
    colors[colorCount++]= *rgb8;

    *pColorCount= colorCount;
    return color;
    }

/************************************************************************/
/*									*/
/*  Initialize an RGB8Color.						*/
/*									*/
/************************************************************************/

void bmInitRGB8Color(	RGB8Color *	rgb8 )
    {
    rgb8->rgb8Red= 0;
    rgb8->rgb8Green= 0;
    rgb8->rgb8Blue= 0;
    rgb8->rgb8Alpha= 255;

    return;
    }

/************************************************************************/
/*									*/
/*  RGB -> IHS conversion.						*/
/*									*/
/************************************************************************/

#   define	F_R	77
#   define	F_G	150
#   define	F_B	29

int bmRgbIntensity(	const RGB8Color *	rgb8 )
    {
    return ( F_R* rgb8->rgb8Red+ F_G* rgb8->rgb8Green+ F_B* rgb8->rgb8Blue )/ 
							    ( F_R+ F_G+ F_B );
    }

int bmRgbSaturation(	const RGB8Color *	rgb8 )
    {
    int		i;
    int		mi= 256;

    i= ( rgb8->rgb8Red+ rgb8->rgb8Green+ rgb8->rgb8Blue )/ 3;

    if  ( mi > rgb8->rgb8Red )
	{ mi=  rgb8->rgb8Red;	}
    if  ( mi > rgb8->rgb8Green )
	{ mi=  rgb8->rgb8Green;	}
    if  ( mi > rgb8->rgb8Blue )
	{ mi=  rgb8->rgb8Blue;	}

    return 255- mi/i;
    }

int bmRgbHue(	const RGB8Color *	rgb8 )
    {
    int		RmG= rgb8->rgb8Red- rgb8->rgb8Green;
    int		RmB= rgb8->rgb8Red- rgb8->rgb8Blue;
    int		GmB= rgb8->rgb8Green- rgb8->rgb8Blue;

    double	above;
    double	below;
    double	res;

    if  ( RmG == 0 && RmB == 0 )
	{ return 0;	}

    above= RmG+ RmB;
    below=  2.0* sqrt( RmG* RmG+ RmB* GmB );
    res= ( 255.0* acos( above/ below ) )/ ( 2* M_PI );

    if  ( below == 0 )
	{ FFDEB(above,below); return 0; }

    return res;
    }
