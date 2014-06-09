#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	<string.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Set one run in a bitmap.						*/
/*  These routines only work for 1 bit/pixel.				*/
/*									*/
/*  1)  End point inclusive.						*/
/*									*/
/************************************************************************/

static unsigned char BM_BitCounts[256];

static int bmCount1Run1Bit(	const unsigned char *	buffer,
				int			col0,
				int			col1 )
    {
    int				d;
    unsigned char		c1, c2;
    const unsigned char *	b1= (unsigned char *)0;
    const unsigned char *	b2= (unsigned char *)0;

    int				count= 0;

    /*
    APP_DEB(appDebug( "bmCount1Run(,col0=%d,col1=%d)\n", col0, col1 ));
    */

    /*  1  */
    col1++;

    d= ( col0 % 8 );
    if  ( d )
	{
	b1= buffer+ col0/ 8;

	col0= 8* ( col0/ 8 )+ 8;

	c1= 0xff >> d;
	}
    else{ c1= 0;	}

    d= ( col1 % 8 );
    if  ( d )
	{
	b2= buffer+ col1/ 8;

	col1= 8* ( col1/ 8 );

	c2= 0xff << ( 8- d );
	}
    else{ c2= 0;	}

    if  ( b1 )
	{
	if  ( b1 == b2 )
	    { count += BM_BitCounts[ *b1 & c1 & c2 ]; }
	else{
	    count += BM_BitCounts[ *b1 & c1 ];
	    }
	}

    if  ( b2 && b2 != b1 )
	{ count += BM_BitCounts[ *b2 & c2 ]; }

    col0 /= 8;
    col1 /= 8;

    b1= buffer+ col0;
    while( col0++ < col1 )
	{ count += BM_BitCounts[ *(b1++) ];	}

    return count;
    }

/************************************************************************/
/*									*/
/*  Count the number of bits set to 0 on a horizontal stretch.		*/
/*									*/
/*  1)  Switch to 'upto' semantics.					*/
/*  2)  If the beginning is not on a byte boundary, obtain a mask for	*/
/*	the tail of the byte. Exclude this byte from the count below	*/
/*  3)  If the end is not on a byte boundary, obtain a mask for	the	*/
/*	head of the byte. Exclude this byte from the count below	*/
/*  4)  Count bits in the head byte. Handle the situation where head	*/
/*	and tail refer to the same byte.				*/
/*  5)  Count bits in the tail byte. (Unless head and tail are the same)*/
/*  6)  Count bits in the bytes between head and tail.			*/
/*									*/
/************************************************************************/

static int bmCount0Run1Bit(	const unsigned char *	buffer,
				int			col0,
				int			col1 )
    {
    int				d;
    unsigned char		c1, c2;
    const unsigned char *	b1= (unsigned char *)0;
    const unsigned char *	b2= (unsigned char *)0;

    int				count= 0;

    /*  1  */
    col1++;

    /*  2  */
    d= ( col0 % 8 );
    if  ( d )
	{
	b1= buffer+ col0/ 8;

	col0= 8* ( col0/ 8 )+ 8;

	c1= 0xff >> d; c1= ~c1;
	}
    else{ c1= 0xff;	}

    /*  3  */
    d= ( col1 % 8 );
    if  ( d )
	{
	b2= buffer+ col1/ 8;

	col1= 8* ( col1/ 8 );

	c2= 0xff << ( 8- d ); c2= ~c2;
	}
    else{ c2= 0xff;	}

    /*  4  */
    if  ( b1 )
	{
	if  ( b1 == b2 )
	    { count += 8- BM_BitCounts[ *b1 | c1 | c2 ];	}
	else{ count += 8- BM_BitCounts[ *b1 | c1 ];		}
	}

    if  ( b2 && b2 != b1 )
	{ count += 8- BM_BitCounts[ *b2 | c2 ]; }

    col0 /= 8;
    col1 /= 8;

    b1= buffer+ col0;
    while( col0++ < col1 )
	{ count += 8- (int)BM_BitCounts[ *(b1++) ];	}

    return count;
    }

/************************************************************************/
/*									*/
/*  count a line using Bressenham.					*/
/*									*/
/*  1)  Line is supposed to be to the right.				*/
/*									*/
/************************************************************************/

static int bmCountBres1Line1Bit(
				const unsigned char *	buffer,
				int			x0,
				int			y0,
				int			x1,
				int			y1,
				int			bytesPerRow )
    {
    int			dx;
    int			dy;
    int			i;

    int			e, d2, e2;

    unsigned char	byteMask;

    int			count= 0;

    dx= x1- x0;
    dy= y1- y0;

    /*  1  */
    if  ( dx < 0 )
	{ LDEB(dx); return 0;	}

    buffer += y0* bytesPerRow+ x0/ 8;
    byteMask= 0x80 >> ( x0 % 8 );

    if  ( dy < 0 )
	{ dy = -dy; bytesPerRow= -bytesPerRow; }

    if  ( dx > dy )
	{
	e= 2* dy- dx;
	d2= 2* dx;
	e2= 2* dy;

	while( x0 <= x1 )
	    {
	    count += BM_BitCounts[ *buffer & byteMask ];

	    while( e >= 0 )
		{ buffer += bytesPerRow; e -= d2; }

	    byteMask >>= 1; x0++; e += e2;

	    if  ( ! ( x0 % 8 ) )
		{ buffer++; byteMask= 0x80;	}
	    }
	}
    else{
	e= 2* dx- dy;
	d2= 2* dy;
	e2= 2* dx;

	for ( i= 0; i <= dy; i++ )
	    {
	    count += BM_BitCounts[ *buffer & byteMask ];

	    while( e > 0 )
		{
		byteMask >>= 1; x0++; e -= d2;

		if  ( ! ( x0 % 8 ) )
		    { buffer++; byteMask= 0x80;	}
		}

	    buffer += bytesPerRow; e += e2;
	    }
	}

    return count;
    }

static int bmCountBres0Line1Bit(
				const unsigned char *	buffer,
				int			x0,
				int			y0,
				int			x1,
				int			y1,
				int			bytesPerRow )
    {
    int			dx;
    int			dy;
    int			i;

    int			e, d2, e2;

    unsigned char	byteMask;

    int			count= 0;

    dx= x1- x0;
    dy= y1- y0;

    /*  1  */
    if  ( dx < 0 )
	{ LDEB(dx); return 0;	}

    buffer += y0* bytesPerRow+ x0/ 8;
    byteMask= 0x80 >> ( x0 % 8 ); byteMask= ~byteMask;

    if  ( dy < 0 )
	{ dy = -dy; bytesPerRow= -bytesPerRow; }

    if  ( dx > dy )
	{
	e= 2* dy- dx;
	d2= 2* dx;
	e2= 2* dy;

	while( x0 <= x1 )
	    {
	    count += 8- BM_BitCounts[ *buffer | byteMask ];

	    while( e >= 0 )
		{ buffer += bytesPerRow; e -= d2; }

	    byteMask >>= 1; byteMask |= 0x80; x0++; e += e2;

	    if  ( ! ( x0 % 8 ) )
		{ buffer++; byteMask= 0x7f;	}
	    }
	}
    else{
	e= 2* dx- dy;
	d2= 2* dy;
	e2= 2* dx;

	for ( i= 0; i <= dy; i++ )
	    {
	    count += 8- BM_BitCounts[ *buffer | byteMask ];

	    while( e > 0 )
		{
		byteMask >>= 1; byteMask |= 0x80; x0++; e -= d2;

		if  ( ! ( x0 % 8 ) )
		    { buffer++; byteMask= 0x7f;	}
		}

	    buffer += bytesPerRow; e += e2;
	    }
	}

    return count;
    }


/************************************************************************/
/*									*/
/*  Draw a line segment.						*/
/*									*/
/*  1)  Make it vertical or to the right.				*/
/*									*/
/************************************************************************/

int bmCountLinePixels(	const unsigned char *		buffer,
			const BitmapDescription *	bd,
			int				x0,
			int				y0,
			int				x1,
			int				y1 )
    {
    int		i;
    int		count= 0;

    int		(*countRun)(	const unsigned char *	_buffer,
				int			_x0,
				int			_y0 );

    int		(*countLine)(	const unsigned char *	_buffer,
				int			_x0,
				int			_y0,
				int			_x1,
				int			_y1,
				int			_bytesPerRow );

    switch( bd->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	    if  ( bd->bdBitsPerPixel != 1 )
		{
		LLDEB(bd->bdColorEncoding,bd->bdBitsPerPixel);
		return -1;
		}

	    countLine= bmCountBres1Line1Bit;
	    countRun= bmCount1Run1Bit;
	    break;

	case BMcoWHITEBLACK:
	    if  ( bd->bdBitsPerPixel != 1 )
		{
		LLDEB(bd->bdColorEncoding,bd->bdBitsPerPixel);
		return -1;
		}

	    countLine= bmCountBres0Line1Bit;
	    countRun= bmCount0Run1Bit;
	    break;

	default:
	    LDEB(bd->bdColorEncoding);
	    return -1;
	}


    if  ( x0 < 0 || x0 >= bd->bdPixelsWide )
	{ LLDEB(x0,bd->bdPixelsWide); return -1; }
    if  ( x1 < 0 || x1 >= bd->bdPixelsWide )
	{ LLDEB(x1,bd->bdPixelsWide); return -1; }
    if  ( y0 < 0 || y0 >= bd->bdPixelsHigh )
	{ LLDEB(y0,bd->bdPixelsHigh); return -1; }
    if  ( y1 < 0 || y1 >= bd->bdPixelsHigh )
	{ LLDEB(y1,bd->bdPixelsHigh); return -1; }


    if  ( BM_BitCounts[1] == 0 )
	{
	int	pos;

	for ( pos= 0; pos < 256; pos++ )
	    {
	    unsigned int	p= pos;

	    BM_BitCounts[pos]= 0;

	    while( p )
		{
		if  ( p & 0x01 )
		    { BM_BitCounts[pos]++;	}

		p >>= 1;
		}
	    }
	}

    /*  1  */
    if  ( x0 > x1 )
	{ i= x0; x0= x1; x1= i; i= y0; y0= y1; y1= i; }

    if  ( y1 == y0 )
	{
	count += (*countRun)( buffer+ y0* bd->bdBytesPerRow, x0, x1 );
	return count;
	}

    if  ( x1 == x0 )
	{
	if  ( y1 < y0 )
	    { i= y0; y0= y1; y1= i; }

	buffer += y0* bd->bdBytesPerRow;

	while( y0 <= y1 )
	    {
	    count += (*countRun)( buffer, x0, x1 );

	    y0++; buffer += bd->bdBytesPerRow;
	    }

	return count;
	}

    count += (*countLine)( buffer, x0, y0, x1, y1, bd->bdBytesPerRow );

    return count;
    }
