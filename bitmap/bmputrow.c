#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	"bmputrow.h"
#   include	<string.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Scaling, Dithering, Antialiassing.					*/
/*									*/
/*  These are the routines that fill the amages from rows of data that	*/
/*  have been collected elesewhere.					*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Fill a Depth 8 image from rgb data.					*/
/*									*/
/************************************************************************/

static int scanFillRow_8_FromRGB(	unsigned char *		to,
					const FillJob *		fj,
					ColorValue *		val )
    {
    ColorAllocator *	ca= fj->fjColorAllocator;
    ColorValue *	cor= fj->fjNextCor+ 1;

    int			col;
    AllocatorColor	ac;

    long		r;
    long		g;
    long		b;
    int			n;

    int			e;
    int			d2;
    int			e2;

    if  ( fj->fjToWide <= fj->fjFrWide )
	{
	e2= 2* fj->fjFrWide;
	d2= 2* fj->fjToWide;
	e= e2- fj->fjToWide;

	r= g= b= n= 0;

	for ( col= 0; col < fj->fjToWide; cor++, col++ )
	    {
	    while( e >= 0 )
		{
		r += val->cvR;
		g += val->cvG;
		b += val->cvB;
		n += val->cvN;

		val++; e -= d2;
		}

	    e += e2;

	    if  ( n == 0 )
		{
		r= g= b= n= 0;

		if  ( fj->fjDither )
		    {
		    cor[ 0].cvR= cor[ 0].cvG= cor[ 0].cvB= 0;
		    cor[-1].cvR= cor[-1].cvG= cor[-1].cvB= 0;
		    cor[+1].cvR= cor[+1].cvG= cor[+1].cvB= 0;
		    }

		to++; continue;
		}

	    r= r/n; g= g/n; b= b/n;

	    if  ( fj->fjDither )
		{ r += cor->cvR; g += cor->cvG; b += cor->cvB;	}

	    if  ( r < 0 ) { r= 0;	}
	    if  ( g < 0 ) { g= 0;	}
	    if  ( b < 0 ) { b= 0;	}

	    if  ( r > 255 ) { r= 255;	}
	    if  ( g > 255 ) { g= 255;	}
	    if  ( b > 255 ) { b= 255;	}

	    switch( ca->caAllocationType )
		{
		int	c;

		case CA_INDEX_ALLOC:
		    bmColorRgbDirect( &ac, ca, r, g, b );
		    c= ac.acColorNumber;
		    if  ( ca->caColors[c].acAllocated == AC_UNALLOCATED )
			{
			if  ( (*ca->caSystemAllocator)( &ac, ca, r, g, b ) )
			    { LDEB(col); return -1;	}
			}
		    else{ ac= ca->caColors[c];		}
		    break;

		case CA_CALCULATED:
		    bmColorRgbDirect( &ac, ca, r, g, b );
		    break;

		case CA_ALLOCATOR:
		    if  ( (*ca->caSystemAllocator)( &ac, ca, r, g, b ) )
			{ LDEB(col); return -1;	}
		    break;

		default:
		    LDEB(ca->caAllocationType); return -1;
		}

	    *(to++)= ac.acColorNumber;

	    if  ( fj->fjDither )
		{
		r -= ac.acRed/ 256;	r += 256;
		g -= ac.acGreen/ 256;	g += 256;
		b -= ac.acBlue/ 256;	b += 256;

		cor[ 0].cvR= fj->fjDc5[r];
		cor[ 0].cvG= fj->fjDc5[g];
		cor[ 0].cvB= fj->fjDc5[b];
		cor[-1].cvR= fj->fjDc3[r];
		cor[-1].cvG= fj->fjDc3[g];
		cor[-1].cvB= fj->fjDc3[b];
		cor[+1].cvR= fj->fjDc1[r];
		cor[+1].cvG= fj->fjDc1[g];
		cor[+1].cvB= fj->fjDc1[b];

		r= fj->fjDc7[r]; g= fj->fjDc7[g]; b= fj->fjDc7[b];
		}
	    else{ r= g= b= 0; }

	    n= 0;
	    }
	}
    else{
	e2= 2* fj->fjToWide;
	d2= 2* fj->fjFrWide;
	e= e2- fj->fjFrWide;

	r= g= b= n= 0;

	for ( col= 0; col < fj->fjFrWide; val++, col++ )
	    {
	    while( e >= 0 )
		{
		r += val->cvR; g += val->cvG; b += val->cvB; n += val->cvN;

		if  ( fj->fjDither )
		    { r += cor->cvR; g += cor->cvG; b += cor->cvB;	}

		if  ( n == 0 )
		    {
		    r= g= b= n= 0;

		    if  ( fj->fjDither )
			{
			cor[ 0].cvR= cor[ 0].cvG= cor[ 0].cvB= 0;
			cor[-1].cvR= cor[-1].cvG= cor[-1].cvB= 0;
			cor[+1].cvR= cor[+1].cvG= cor[+1].cvB= 0;
			}

		    to++; continue;
		    }

		if  ( n > 1 )
		    { r /= n; g /= n; b /= n; }

		if  ( r < 0 ) { r= 0;		}
		if  ( g < 0 ) { g= 0;		}
		if  ( b < 0 ) { b= 0;		}

		if  ( r > 255 ) { r= 255;	}
		if  ( g > 255 ) { g= 255;	}
		if  ( b > 255 ) { b= 255;	}

		switch( ca->caAllocationType )
		    {
		    int	c;

		    case CA_INDEX_ALLOC:
			bmColorRgbDirect( &ac, ca, r, g, b );
			c= ac.acColorNumber;
			if  ( ca->caColors[c].acAllocated == AC_UNALLOCATED )
			    {
			    if  ( (*ca->caSystemAllocator)( &ac, ca, r, g, b ) )
				{ LDEB(col); return -1;	}
			    }
			else{ ac= ca->caColors[c];		}
			break;

		    case CA_CALCULATED:
			bmColorRgbDirect( &ac, ca, r, g, b );
			break;

		    case CA_ALLOCATOR:
			if  ( (*ca->caSystemAllocator)( &ac, ca, r, g, b ) )
			    { LDEB(col); return -1;	}
			break;

		    default:
			LDEB(ca->caAllocationType); return -1;
		    }

		*(to++)= ac.acColorNumber;

		if  ( fj->fjDither )
		    {
		    r -= ac.acRed/ 256;		r += 256;
		    g -= ac.acGreen/ 256;	g += 256;
		    b -= ac.acBlue/ 256;	b += 256;

		    cor[ 0].cvR= fj->fjDc5[r];
		    cor[ 0].cvG= fj->fjDc5[g];
		    cor[ 0].cvB= fj->fjDc5[b];
		    cor[-1].cvR= fj->fjDc3[r];
		    cor[-1].cvG= fj->fjDc3[g];
		    cor[-1].cvB= fj->fjDc3[b];
		    cor[+1].cvR= fj->fjDc1[r];
		    cor[+1].cvG= fj->fjDc1[g];
		    cor[+1].cvB= fj->fjDc1[b];

		    r= fj->fjDc7[r]; g= fj->fjDc7[g]; b= fj->fjDc7[b];
		    }
		else{ r= g= b= 0;	}

		n= 0; e -= d2; cor++;
		}

	    e += e2;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Fill a Depth 4 image from rgb data.					*/
/*									*/
/************************************************************************/

static int scanFillRow_4_FromRGB(	unsigned char *		to,
					const FillJob *		fj,
					ColorValue *		val )
    {
    const unsigned char *	from;
    int				col;

    memset( fj->fjScratch, 0x00, fj->fjToWide );

    if  ( scanFillRow_8_FromRGB( fj->fjScratch, fj, val ) )
	{ LDEB(1); return -1;	}

    from= fj->fjScratch;
    for ( col= 0; col < fj->fjToWide; to++, from += 2, col += 2 )
	{
	to[0]=	( ( from[0] & 0x0f ) << 4 )	|
		( ( from[1] & 0x0f ) << 0 )	;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Fill a Depth 2 image from rgb data.					*/
/*									*/
/************************************************************************/

static int scanFillRow_2_FromRGB(	unsigned char *		to,
					const FillJob *		fj,
					ColorValue *		val )
    {
    const unsigned char *	from;
    int				col;

    memset( fj->fjScratch, 0x00, fj->fjToWide );

    if  ( scanFillRow_8_FromRGB( fj->fjScratch, fj, val ) )
	{ LDEB(1); return -1;	}

    from= fj->fjScratch;
    for ( col= 0; col < fj->fjToWide; to++, from += 4, col += 4 )
	{
	to[0]=	( ( from[0] & 0x03 ) << 6 )	|
		( ( from[1] & 0x03 ) << 4 )	|
		( ( from[2] & 0x03 ) << 2 )	|
		( ( from[3] & 0x03 ) << 0 )	;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Fill a Depth 1 image from rgb data.					*/
/*									*/
/************************************************************************/

static int scanFillD1RgbRow_32SX(	unsigned char *		to,
					const FillJob *		fj,
					ColorValue *		val )
    {
    const unsigned char *	from;
    int				col;

    memset( fj->fjScratch, 0x00, fj->fjToWide );

    if  ( scanFillRow_8_FromRGB( fj->fjScratch, fj, val ) )
	{ LDEB(1); return -1;	}

    from= fj->fjScratch;
    for ( col= 0; col < fj->fjToWide; to += 4, from += 32, col += 32 )
	{
	to[3]=	( from[ 0]  << 7 )	|
		( from[ 1]  << 6 )	|
		( from[ 2]  << 5 )	|
		( from[ 3]  << 4 )	|
		( from[ 4]  << 3 )	|
		( from[ 5]  << 2 )	|
		( from[ 6]  << 1 )	|
		( from[ 7]  << 0 )	;

	to[2]=	( from[ 8]  << 7 )	|
		( from[ 9]  << 6 )	|
		( from[10]  << 5 )	|
		( from[11]  << 4 )	|
		( from[12]  << 3 )	|
		( from[13]  << 2 )	|
		( from[14]  << 1 )	|
		( from[15]  << 0 )	;

	to[1]=	( from[16]  << 7 )	|
		( from[17]  << 6 )	|
		( from[18]  << 5 )	|
		( from[19]  << 4 )	|
		( from[20]  << 3 )	|
		( from[21]  << 2 )	|
		( from[22]  << 1 )	|
		( from[23]  << 0 )	;

	to[0]=	( from[24]  << 7 )	|
		( from[25]  << 6 )	|
		( from[26]  << 5 )	|
		( from[27]  << 4 )	|
		( from[28]  << 3 )	|
		( from[29]  << 2 )	|
		( from[30]  << 1 )	|
		( from[31]  << 0 )	;
	}

    return 0;
    }

static int scanFillD1RgbRow_32SS(	unsigned char *		to,
					const FillJob *		fj,
					ColorValue *		val )
    {
    const unsigned char *	from;
    int				col;

    memset( fj->fjScratch, 0x00, fj->fjToWide );

    if  ( scanFillRow_8_FromRGB( fj->fjScratch, fj, val ) )
	{ LDEB(1); return -1;	}

    from= fj->fjScratch;
    for ( col= 0; col < fj->fjToWide; to += 4, from += 32, col += 32 )
	{
	to[3]=	( from[ 7]  << 7 )	|
		( from[ 6]  << 6 )	|
		( from[ 5]  << 5 )	|
		( from[ 4]  << 4 )	|
		( from[ 3]  << 3 )	|
		( from[ 2]  << 2 )	|
		( from[ 1]  << 1 )	|
		( from[ 0]  << 0 )	;

	to[2]=	( from[15]  << 7 )	|
		( from[14]  << 6 )	|
		( from[13]  << 5 )	|
		( from[12]  << 4 )	|
		( from[11]  << 3 )	|
		( from[10]  << 2 )	|
		( from[ 9]  << 1 )	|
		( from[ 8]  << 0 )	;

	to[1]=	( from[23]  << 7 )	|
		( from[22]  << 6 )	|
		( from[21]  << 5 )	|
		( from[20]  << 4 )	|
		( from[19]  << 3 )	|
		( from[18]  << 2 )	|
		( from[17]  << 1 )	|
		( from[16]  << 0 )	;

	to[0]=	( from[31]  << 7 )	|
		( from[30]  << 6 )	|
		( from[29]  << 5 )	|
		( from[28]  << 4 )	|
		( from[27]  << 3 )	|
		( from[26]  << 2 )	|
		( from[25]  << 1 )	|
		( from[24]  << 0 )	;
	}

    return 0;
    }

static int scanFillD1RgbRow_16SX(	unsigned char *		to,
					const FillJob *		fj,
					ColorValue *		val )
    {
    const unsigned char *	from;
    int				col;

    memset( fj->fjScratch, 0x00, fj->fjToWide );

    if  ( scanFillRow_8_FromRGB( fj->fjScratch, fj, val ) )
	{ LDEB(1); return -1;	}

    from= fj->fjScratch;
    for ( col= 0; col < fj->fjToWide; to += 2, from += 16, col += 16 )
	{
	to[1]=	( from[ 0]  << 7 )	|
		( from[ 1]  << 6 )	|
		( from[ 2]  << 5 )	|
		( from[ 3]  << 4 )	|
		( from[ 4]  << 3 )	|
		( from[ 5]  << 2 )	|
		( from[ 6]  << 1 )	|
		( from[ 7]  << 0 )	;

	to[0]=	( from[ 8]  << 7 )	|
		( from[ 9]  << 6 )	|
		( from[10]  << 5 )	|
		( from[11]  << 4 )	|
		( from[12]  << 3 )	|
		( from[13]  << 2 )	|
		( from[14]  << 1 )	|
		( from[15]  << 0 )	;
	}

    return 0;
    }

static int scanFillD1RgbRow_16SS(	unsigned char *		to,
					const FillJob *		fj,
					ColorValue *		val )
    {
    const unsigned char *	from;
    int				col;

    memset( fj->fjScratch, 0x00, fj->fjToWide );

    if  ( scanFillRow_8_FromRGB( fj->fjScratch, fj, val ) )
	{ LDEB(1); return -1;	}

    from= fj->fjScratch;
    for ( col= 0; col < fj->fjToWide; to += 2, from += 16, col += 16 )
	{
	to[1]=	( from[ 7]  << 7 )	|
		( from[ 6]  << 6 )	|
		( from[ 5]  << 5 )	|
		( from[ 4]  << 4 )	|
		( from[ 3]  << 3 )	|
		( from[ 2]  << 2 )	|
		( from[ 1]  << 1 )	|
		( from[ 0]  << 0 )	;

	to[0]=	( from[15]  << 7 )	|
		( from[14]  << 6 )	|
		( from[13]  << 5 )	|
		( from[12]  << 4 )	|
		( from[11]  << 3 )	|
		( from[10]  << 2 )	|
		( from[ 9]  << 1 )	|
		( from[ 8]  << 0 )	;
	}

    return 0;
    }

static int scanFillD1RgbRow_8X(		unsigned char *		to,
					const FillJob *		fj,
					ColorValue *		val )
    {
    const unsigned char *	from;
    int				col;

    memset( fj->fjScratch, 0x00, fj->fjToWide );

    if  ( scanFillRow_8_FromRGB( fj->fjScratch, fj, val ) )
	{ LDEB(1); return -1;	}

    from= fj->fjScratch;
    for ( col= 0; col < fj->fjToWide; to++, from += 8, col += 8 )
	{
	to[0]=	( from[ 0]  << 7 )	|
		( from[ 1]  << 6 )	|
		( from[ 2]  << 5 )	|
		( from[ 3]  << 4 )	|
		( from[ 4]  << 3 )	|
		( from[ 5]  << 2 )	|
		( from[ 6]  << 1 )	|
		( from[ 7]  << 0 )	;
	}

    return 0;
    }

static int scanFillD1RgbRow_8S(		unsigned char *		to,
					const FillJob *		fj,
					ColorValue *		val )
    {
    const unsigned char *	from;
    int				col;

    memset( fj->fjScratch, 0x00, fj->fjToWide );

    if  ( scanFillRow_8_FromRGB( fj->fjScratch, fj, val ) )
	{ LDEB(1); return -1;	}

    from= fj->fjScratch;
    for ( col= 0; col < fj->fjToWide; to++, from += 8, col += 8 )
	{
	to[0]=	( from[ 7]  << 7 )	|
		( from[ 6]  << 6 )	|
		( from[ 5]  << 5 )	|
		( from[ 4]  << 4 )	|
		( from[ 3]  << 3 )	|
		( from[ 2]  << 2 )	|
		( from[ 1]  << 1 )	|
		( from[ 0]  << 0 )	;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Fill a Depth 16 image from rgb data.				*/
/*									*/
/************************************************************************/

static int scanFillRow_16_FromRGB(	unsigned char *		cto,
					const FillJob *		fj,
					ColorValue *		val )
    {
    ColorAllocator *	ca= fj->fjColorAllocator;
    ColorValue *	cor= fj->fjNextCor+ 1;

    int			col;

    long		r;
    long		g;
    long		b;
    int			n;

    int			e;
    int			d2;
    int			e2;

    BmUint16 *		to= (BmUint16 *)cto;
    AllocatorColor	ac;

    if  ( fj->fjToWide <= fj->fjFrWide )
	{
	e2= 2* fj->fjFrWide;
	d2= 2* fj->fjToWide;
	e= e2- fj->fjToWide;

	r= g= b= n= 0;

	for ( col= 0; col < fj->fjToWide; col++ )
	    {
	    r= g= b= n= 0;

	    while( e >= 0 )
		{
		r += val->cvR;
		g += val->cvG;
		b += val->cvB;
		n += val->cvN;

		val++; e -= d2;
		}

	    e += e2;

	    if  ( n == 0 )
		{
		r= g= b= n= 0;

		if  ( fj->fjDither )
		    {
		    cor[ 0].cvR= cor[ 0].cvG= cor[ 0].cvB= 0;
		    cor[-1].cvR= cor[-1].cvG= cor[-1].cvB= 0;
		    cor[+1].cvR= cor[+1].cvG= cor[+1].cvB= 0;
		    }

		to++; continue;
		}

	    r= r/n; g= g/n; b= b/n;

	    if  ( r < 0 ) { r= 0;	}
	    if  ( g < 0 ) { g= 0;	}
	    if  ( b < 0 ) { b= 0;	}

	    if  ( r > 255 ) { r= 255;	}
	    if  ( g > 255 ) { g= 255;	}
	    if  ( b > 255 ) { b= 255;	}

	    switch( ca->caAllocationType )
		{
		int	c;

		case CA_INDEX_ALLOC:
		    bmColorRgbDirect( &ac, ca, r, g, b );
		    c= ac.acColorNumber;
		    if  ( ca->caColors[c].acAllocated == AC_UNALLOCATED )
			{
			if  ( (*ca->caSystemAllocator)( &ac, ca, r, g, b ) )
			    { LDEB(col); return -1;	}
			}
		    else{ ac= ca->caColors[c];		}
		    break;

		case CA_CALCULATED:
		    bmColorRgbDirect( &ac, ca, r, g, b );
		    break;

		default:
		    LDEB(ca->caAllocationType); return -1;
		}

	    *(to++)= ac.acColorNumber;
	    }
	}
    else{
	e2= 2* fj->fjToWide;
	d2= 2* fj->fjFrWide;
	e= e2- fj->fjFrWide;

	r= g= b= n= 0;

	for ( col= 0; col < fj->fjFrWide; val++, col++ )
	    {
	    while( e >= 0 )
		{
		r= val->cvR;
		g= val->cvG;
		b= val->cvB;
		n= val->cvN;

		if  ( n == 0 )
		    {
		    r= g= b= n= 0;

		    if  ( fj->fjDither )
			{
			cor[ 0].cvR= cor[ 0].cvG= cor[ 0].cvB= 0;
			cor[-1].cvR= cor[-1].cvG= cor[-1].cvB= 0;
			cor[+1].cvR= cor[+1].cvG= cor[+1].cvB= 0;
			}

		    to++; continue;
		    }

		if  ( n > 1 )
		    { r /= n; g /= n; b /= n; }

		if  ( r < 0 ) { r= 0;		}
		if  ( g < 0 ) { g= 0;		}
		if  ( b < 0 ) { b= 0;		}

		if  ( r > 255 ) { r= 255;	}
		if  ( g > 255 ) { g= 255;	}
		if  ( b > 255 ) { b= 255;	}

		switch( ca->caAllocationType )
		    {
		    int	c;

		    case CA_INDEX_ALLOC:
			bmColorRgbDirect( &ac, ca, r, g, b );
			c= ac.acColorNumber;
			if  ( ca->caColors[c].acAllocated == AC_UNALLOCATED )
			    {
			    if  ( (*ca->caSystemAllocator)( &ac, ca, r, g, b ) )
				{ LDEB(col); return -1;	}
			    }
			else{ ac= ca->caColors[c];		}
			break;

		    case CA_CALCULATED:
			bmColorRgbDirect( &ac, ca, r, g, b );
			break;

		    default:
			LDEB(ca->caAllocationType); return -1;
		    }

		*(to++)= ac.acColorNumber;

		e -= d2;
		}

	    e += e2;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Fill a Depth 24 image from rgb data.				*/
/*									*/
/************************************************************************/

static int scanFillD24RgbRow(	unsigned char *		to,
				const FillJob *		fj,
				ColorValue *		val )
    {
    /*
    ColorAllocator *	ca= fj->fjColorAllocator;
    */
    ColorValue *	cor= fj->fjNextCor+ 1;

    int			col;

    long		r;
    long		g;
    long		b;
    int			n;

    int			e;
    int			d2;
    int			e2;

    if  ( fj->fjToWide <= fj->fjFrWide )
	{
	e2= 2* fj->fjFrWide;
	d2= 2* fj->fjToWide;
	e= e2- fj->fjToWide;

	r= g= b= n= 0;

	for ( col= 0; col < fj->fjToWide; col++ )
	    {
	    r= g= b= n= 0;

	    while( e >= 0 )
		{
		r += val->cvR;
		g += val->cvG;
		b += val->cvB;
		n += val->cvN;

		val++; e -= d2;
		}

	    e += e2;

	    if  ( n == 0 )
		{
		r= g= b= n= 0;

		if  ( fj->fjDither )
		    {
		    cor[ 0].cvR= cor[ 0].cvG= cor[ 0].cvB= 0;
		    cor[-1].cvR= cor[-1].cvG= cor[-1].cvB= 0;
		    cor[+1].cvR= cor[+1].cvG= cor[+1].cvB= 0;
		    }

		to += 3; continue;
		}

	    r= r/n; g= g/n; b= b/n;

	    if  ( r < 0 ) { r= 0;	}
	    if  ( g < 0 ) { g= 0;	}
	    if  ( b < 0 ) { b= 0;	}

	    if  ( r > 255 ) { r= 255;	}
	    if  ( g > 255 ) { g= 255;	}
	    if  ( b > 255 ) { b= 255;	}

	    *(to++)= r; *(to++)= g; *(to++)= b;
	    }
	}
    else{
	e2= 2* fj->fjToWide;
	d2= 2* fj->fjFrWide;
	e= e2- fj->fjFrWide;

	r= g= b= n= 0;

	for ( col= 0; col < fj->fjFrWide; val++, col++ )
	    {
	    while( e >= 0 )
		{
		r= val->cvR;
		g= val->cvG;
		b= val->cvB;
		n= val->cvN;

		if  ( n == 0 )
		    {
		    r= g= b= n= 0;

		    if  ( fj->fjDither )
			{
			cor[ 0].cvR= cor[ 0].cvG= cor[ 0].cvB= 0;
			cor[-1].cvR= cor[-1].cvG= cor[-1].cvB= 0;
			cor[+1].cvR= cor[+1].cvG= cor[+1].cvB= 0;
			}

		    to += 3; continue;
		    }

		if  ( n > 1 )
		    { r /= n; g /= n; b /= n; }

		if  ( r < 0 ) { r= 0;		}
		if  ( g < 0 ) { g= 0;		}
		if  ( b < 0 ) { b= 0;		}

		if  ( r > 255 ) { r= 255;	}
		if  ( g > 255 ) { g= 255;	}
		if  ( b > 255 ) { b= 255;	}

		*(to++)= r; *(to++)= g; *(to++)= b;

		e -= d2;
		}

	    e += e2;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Fill a Depth 32 image from rgb data.				*/
/*									*/
/************************************************************************/

static int scanFillRow_32_FromRGB(	unsigned char *		cto,
					const FillJob *		fj,
					ColorValue *		val )
    {
    ColorAllocator *	ca= fj->fjColorAllocator;
    ColorValue *	cor= fj->fjNextCor+ 1;

    int			col;

    long		r;
    long		g;
    long		b;
    int			n;

    int			e;
    int			d2;
    int			e2;

    BmUint32 *		to= (BmUint32 *)cto;
    AllocatorColor	ac;

    if  ( fj->fjToWide <= fj->fjFrWide )
	{
	e2= 2* fj->fjFrWide;
	d2= 2* fj->fjToWide;
	e= e2- fj->fjToWide;

	r= g= b= n= 0;

	for ( col= 0; col < fj->fjToWide; cor++, col++ )
	    {
	    r= g= b= n= 0;

	    while( e >= 0 )
		{
		r += val->cvR;
		g += val->cvG;
		b += val->cvB;
		n += val->cvN;

		val++; e -= d2;
		}

	    if  ( n == 0 )
		{
		r= g= b= n= 0;

		if  ( fj->fjDither )
		    {
		    cor[ 0].cvR= cor[ 0].cvG= cor[ 0].cvB= 0;
		    cor[-1].cvR= cor[-1].cvG= cor[-1].cvB= 0;
		    cor[+1].cvR= cor[+1].cvG= cor[+1].cvB= 0;
		    }

		to++; continue;
		}

	    e += e2;

	    r= r/n; g= g/n; b= b/n;

	    if  ( r < 0 ) { r= 0;	}
	    if  ( g < 0 ) { g= 0;	}
	    if  ( b < 0 ) { b= 0;	}

	    if  ( r > 255 ) { r= 255;	}
	    if  ( g > 255 ) { g= 255;	}
	    if  ( b > 255 ) { b= 255;	}

	    switch( ca->caAllocationType )
		{
		int	c;

		case CA_INDEX_ALLOC:
		    bmColorRgbDirect( &ac, ca, r, g, b );
		    c= ac.acColorNumber;
		    if  ( ca->caColors[c].acAllocated == AC_UNALLOCATED )
			{
			if  ( (*ca->caSystemAllocator)( &ac, ca, r, g, b ) )
			    { LDEB(col); return -1;	}
			}
		    else{ ac= ca->caColors[c];		}
		    break;

		case CA_CALCULATED:
		    bmColorRgbDirect( &ac, ca, r, g, b );
		    break;

		default:
		    LDEB(ca->caAllocationType); return -1;
		}

	    *(to++)= ac.acColorNumber;
	    }
	}
    else{
	e2= 2* fj->fjToWide;
	d2= 2* fj->fjFrWide;
	e= e2- fj->fjFrWide;

	r= g= b= n= 0;

	for ( col= 0; col < fj->fjFrWide; val++, cor++, col++ )
	    {
	    while( e >= 0 )
		{
		r= val->cvR;
		g= val->cvG;
		b= val->cvB;
		n= val->cvN;

		if  ( n == 0 )
		    {
		    r= g= b= n= 0;

		    if  ( fj->fjDither )
			{
			cor[ 0].cvR= cor[ 0].cvG= cor[ 0].cvB= 0;
			cor[-1].cvR= cor[-1].cvG= cor[-1].cvB= 0;
			cor[+1].cvR= cor[+1].cvG= cor[+1].cvB= 0;
			}

		    to++; continue;
		    }

		if  ( n > 1 )
		    { r /= n; g /= n; b /= n; }

		if  ( r < 0 ) { r= 0;		}
		if  ( g < 0 ) { g= 0;		}
		if  ( b < 0 ) { b= 0;		}

		if  ( r > 255 ) { r= 255;	}
		if  ( g > 255 ) { g= 255;	}
		if  ( b > 255 ) { b= 255;	}

		switch( ca->caAllocationType )
		    {
		    int	c;

		    case CA_INDEX_ALLOC:
			bmColorRgbDirect( &ac, ca, r, g, b );
			c= ac.acColorNumber;
			if  ( ca->caColors[c].acAllocated == AC_UNALLOCATED )
			    {
			    if  ( (*ca->caSystemAllocator)( &ac, ca, r, g, b ) )
				{ LDEB(col); return -1;	}
			    }
			else{ ac= ca->caColors[c];		}
			break;

		    case CA_CALCULATED:
			bmColorRgbDirect( &ac, ca, r, g, b );
			break;

		    default:
			LDEB(ca->caAllocationType); return -1;
		    }

		*(to++)= ac.acColorNumber;

		e -= d2;
		}

	    e += e2;
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Determine the pixel output mechanism.				*/
/*									*/
/************************************************************************/

int bmGetPutRow(	PutScreenRow *			pPutRow,
			int *				pScratchSize,
			const ColorAllocator *		ca,
			int				swapBitmapUnit,
			int				swapBitmapBytes,
			int				swapBitmapBits,
			const BitmapDescription *	bdOut )
    {
    PutScreenRow	putRow= (PutScreenRow)0;
    int			scratchSize= 0;

    if  ( ca->caAllocationType == CA_CALCULATED )
	{
	switch( bdOut->bdBitsPerPixel )
	    {
	    case 32:
		putRow= scanFillRow_32_FromRGB;
		break;

	    case 24:
		putRow= scanFillD24RgbRow;
		break;

	    case 16:
		putRow= scanFillRow_16_FromRGB;
		break;

	    case 8:
		putRow= scanFillRow_8_FromRGB;
		break;

	    case 4:
		scratchSize= bdOut->bdPixelsWide+ 7;
		putRow= scanFillRow_4_FromRGB;
		break;

	    case 2:
		scratchSize= bdOut->bdPixelsWide+ 7;
		putRow= scanFillRow_2_FromRGB;
		break;

	    default:
		LDEB(bdOut->bdBitsPerPixel);
		return -1;
	    }
	}
    else{
	switch( bdOut->bdBitsPerPixel )
	    {
	    case 1:
		scratchSize= bdOut->bdPixelsWide+ 7;

		if  ( swapBitmapBytes )
		    {
		    switch( swapBitmapUnit )
			{
			case 16:
			    if  ( swapBitmapBits )
				{ putRow= scanFillD1RgbRow_16SX;	}
			    else{ putRow= scanFillD1RgbRow_16SS;	}
			break;

			case 32:
			    if  ( swapBitmapBits )
				{ putRow= scanFillD1RgbRow_32SX;	}
			    else{ putRow= scanFillD1RgbRow_32SS;	}
			break;

			default:
			    LDEB(swapBitmapUnit); return -1;
			}
		    }
		else{
		    if  ( swapBitmapBits )
			{ putRow= scanFillD1RgbRow_8S;		}
		    else{ putRow= scanFillD1RgbRow_8X;		}
		    }
		break;

	    case 2:
		scratchSize= bdOut->bdPixelsWide+ 7;
		putRow= scanFillRow_2_FromRGB;
		break;

	    case 4:
		scratchSize= bdOut->bdPixelsWide+ 7;
		putRow= scanFillRow_4_FromRGB;
		break;

	    case 8:
		putRow= scanFillRow_8_FromRGB;
		break;

	    case 16:
		putRow= scanFillRow_16_FromRGB;
		break;

	    default:
		LDEB(bdOut->bdBitsPerPixel);
		return -1;
	    }
	}

    *pPutRow= putRow;
    *pScratchSize= scratchSize;

    return 0;
    }

