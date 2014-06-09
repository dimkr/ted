#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	<string.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Set one run in a bitmap.						*/
/*  These routines only work for 1 bit/pixel.				*/
/*									*/
/*  1)  End point inclusive.						*/
/*									*/
/************************************************************************/

static void bmDraw1Run1Bit(	unsigned char *	buffer,
				int		col0,
				int		col1 )
    {
    int			d;
    unsigned char	c1, c2;
    unsigned char *	b1= (unsigned char *)0;
    unsigned char *	b2= (unsigned char *)0;

    /*
    APP_DEB(appDebug( "bmDraw1Run(,col0=%d,col1=%d)\n", col0, col1 ));
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
	    { *b1 |= c1 & c2; }
	else{
	    *b1 |= c1;
	    }
	}

    if  ( b2 && b2 != b1 )
	{ *b2 |= c2; }

    col0 /= 8;
    col1 /= 8;

    b1= buffer+ col0;
    while( col0++ < col1 )
	{ *(b1++)= 0xff;	}

    return;
    }

static void bmDraw0Run1Bit(	unsigned char *	buffer,
				int		col0,
				int		col1 )
    {
    int			d;
    unsigned char	c1, c2;
    unsigned char *	b1= (unsigned char *)0;
    unsigned char *	b2= (unsigned char *)0;

    /*
    APP_DEB(appDebug( "bmDraw0Run(,col0=%d,col1=%d)\n", col0, col1 ));
    */

    /*  1  */
    col1++;

    d= ( col0 % 8 );
    if  ( d )
	{
	b1= buffer+ col0/ 8;

	col0= 8* ( col0/ 8 )+ 8;

	c1= 0xff >> d; c1= ~c1;
	}
    else{ c1= 0xff;	}

    d= ( col1 % 8 );
    if  ( d )
	{
	b2= buffer+ col1/ 8;

	col1= 8* ( col1/ 8 );

	c2= 0xff << ( 8- d ); c2= ~c2;
	}
    else{ c2= 0xff;	}

    if  ( b1 )
	{
	if  ( b1 == b2 )
	    { *b1 &= c1 | c2; }
	else{
	    *b1 &= c1;
	    }
	}

    if  ( b2 && b2 != b1 )
	{ *b2 &= c2; }

    col0 /= 8;
    col1 /= 8;

    b1= buffer+ col0;
    while( col0++ < col1 )
	{ *(b1++)= 0x00;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Fill a rectangular area.						*/
/*									*/
/************************************************************************/

int bmFillBlock(	unsigned char *		buffer,
			int			x0,
			int			y0,
			int			x1,
			int			y1,
			int			bytesPerRow )
    {
    int		sw;

    /*
    APP_DEB(appDebug( "bmFillBlock(,x0=%d,y0=%d,x1=%d,y1=%d,bpr=%d,)\n",
		x0, y0, x1, y1, bytesPerRow ));
    */

    if  ( x1 < x0 )
	{ sw= x1; x1= x0; x0= sw;	}
    if  ( y1 < y0 )
	{ sw= y1; y1= y0; y0= sw;	}

    buffer += y0* bytesPerRow;

    while( y0 <= y1 )
	{
	bmDraw1Run1Bit( buffer, x0, x1 );
	y0++; buffer += bytesPerRow;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Allocate and initialise a buffer on which something can be drawn	*/
/*									*/
/************************************************************************/

unsigned char *	bmBackgroundBuffer(	int	bufferLength,
					int	colorEncoding )
    {
    unsigned char *	buffer= malloc( bufferLength );

    if  ( ! buffer )
	{ LLDEB(bufferLength,buffer); return buffer;	}

    switch( colorEncoding )
	{
	case	BMcoBLACKWHITE:
	    memset( buffer, 0x00, bufferLength );
	    break;

	case	BMcoWHITEBLACK:
	    memset( buffer, 0xff, bufferLength );
	    break;

	default:
	    LDEB(colorEncoding);
	    free( buffer );
	    return (unsigned char *)0;
	}

    return buffer;
    }

/************************************************************************/
/*									*/
/*  Convenience routine: Draw a box.					*/
/*									*/
/************************************************************************/

int bmDrawBox(	unsigned char *			buffer,
		const BitmapDescription *	bd,
		int				x0,
		int				y0,
		int				x1,
		int				y1,
		int				wide )
    {
    if  ( bmDrawLine( buffer, bd, x0, y0+ wide/2, x1, y0+ wide/2, wide ) )
	{ LDEB(1); return -1; }

    if  ( bmDrawLine( buffer, bd, x0, y1- wide/2, x1, y1- wide/2, wide ) )
	{ LDEB(1); return -1; }

    if  ( bmDrawLine( buffer, bd, x0+ wide/2, y0, x0+ wide/2, y1, wide ) )
	{ LDEB(1); return -1; }

    if  ( bmDrawLine( buffer, bd, x1- wide/2, y0, x1- wide/2, y1, wide ) )
	{ LDEB(1); return -1; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  draw a line using Bressenham.					*/
/*									*/
/*  1)  Line is supposed to be to the right.				*/
/*									*/
/************************************************************************/

static void bmDrawBres1Line1Bit(unsigned char *		buffer,
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

    dx= x1- x0;
    dy= y1- y0;

    /*  1  */
    if  ( dx < 0 )
	{ LDEB(dx); return;	}

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
	    *buffer |= byteMask;

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
	    *buffer |= byteMask;

	    while( e > 0 )
		{
		byteMask >>= 1; x0++; e -= d2;

		if  ( ! ( x0 % 8 ) )
		    { buffer++; byteMask= 0x80;	}
		}

	    buffer += bytesPerRow; e += e2;
	    }
	}
    }

static void bmDrawBres0Line1Bit(unsigned char *		buffer,
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

    dx= x1- x0;
    dy= y1- y0;

    /*  1  */
    if  ( dx < 0 )
	{ LDEB(dx); return;	}

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
	    *buffer &= byteMask;

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
	    *buffer &= byteMask;

	    while( e > 0 )
		{
		byteMask >>= 1; byteMask |= 0x80; x0++; e -= d2;

		if  ( ! ( x0 % 8 ) )
		    { buffer++; byteMask= 0x7f;	}
		}

	    buffer += bytesPerRow; e += e2;
	    }
	}
    }


/************************************************************************/
/*									*/
/*  Draw a line segment.						*/
/*									*/
/*  1)  Make it vertical or to the right.				*/
/*									*/
/************************************************************************/

int bmDrawLine(	unsigned char *			buffer,
		const BitmapDescription *	bd,
		int				x0,
		int				y0,
		int				x1,
		int				y1,
		int				wide )
    {
    int		i;

    int		wx;
    int		wy;

    int		dx;
    int		dy;
    int		sy;

    int		e, d2, e2;

    void	(*drawRun)(	unsigned char *	_buffer,
				int		_x0,
				int		_y0 );

    void	(*drawLine)(	unsigned char *	_buffer,
				int		_x0,
				int		_y0,
				int		_x1,
				int		_y1,
				int		_bytesPerRow );

    switch( bd->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	    if  ( bd->bdBitsPerPixel != 1 )
		{
		LLDEB(bd->bdColorEncoding,bd->bdBitsPerPixel);
		return -1;
		}

	    drawLine= bmDrawBres1Line1Bit;
	    drawRun= bmDraw1Run1Bit;
	    break;

	case BMcoWHITEBLACK:
	    if  ( bd->bdBitsPerPixel != 1 )
		{
		LLDEB(bd->bdColorEncoding,bd->bdBitsPerPixel);
		return -1;
		}

	    drawLine= bmDrawBres0Line1Bit;
	    drawRun= bmDraw0Run1Bit;
	    break;

	default:
	    LDEB(bd->bdColorEncoding);
	    return -1;
	}

    /*  1  */
    if  ( x0 > x1 )
	{ i= x0; x0= x1; x1= i; i= y0; y0= y1; y1= i; }

    if  ( y1 == y0 )
	{
	if  ( wide > 1 )
	    {
	    y0 -= wide/2; y1= y0+ wide;
	    }

	buffer += y0* bd->bdBytesPerRow;
	while( y0 <= y1 )
	    {
	    (*drawRun)( buffer, x0, x1 );

	    y0++; buffer += bd->bdBytesPerRow;
	    }

	return 0;
	}

    if  ( x1 == x0 )
	{
	if  ( wide > 1 )
	    {
	    x0 -= wide/2; x1= x0+ wide;
	    }

	if  ( y0 > y1 )
	    { i= y0; y0= y1; y1= i;	}

	buffer += y0* bd->bdBytesPerRow;
	while( y0 <= y1 )
	    {
	    (*drawRun)( buffer, x0, x1 );

	    y0++; buffer += bd->bdBytesPerRow;
	    }

	return 0;
	}

    dx= x1- x0;
    dy= y1- y0;

    wx= wy= 1;
    if  ( wide != 1 )
	{
	sy= sqrt( (double)( dx*dx + dy*dy ) );
	wx= ( dy* wide )/ sy;
	wy= ( dx* wide )/ sy;
	}

    if  ( dy > 0 )
	{ sy= -1;			}
    else{ sy=  1; dy= -dy; wx= -wx;	}

    if  ( wx > wy )
	{
	if  ( wx == 0 )
	    { wx= 1;	}

	e= 2* wy- wx;
	d2= 2* wx;
	e2= 2* wy;

	x0 -= wx/2;
	x1 -= wx/2;
	y0 -= sy* wy/2;
	y1 -= sy* wy/2;

	for ( i= 0; i < wx; i++ )
	    {
	    (*drawLine)( buffer, x0, y0, x1, y1, bd->bdBytesPerRow );

	    while( e >= 0 )
		{
		y0 += sy; y1 += sy; e -= d2;
		(*drawLine)( buffer, x0, y0, x1, y1, bd->bdBytesPerRow );
		}
	    x0++; x1++; e += e2;
	    }
	}
    else{
	if  ( wy == 0 )
	    { wy= 1;	}

	e= 2* wx- wy;
	d2= 2* wy;
	e2= 2* wx;

	x0 -= wx/2;
	x1 -= wx/2;
	y0 -= wy/2;
	y1 -= wy/2;

	for ( i= 0; i < wy; i++ )
	    {
	    (*drawLine)( buffer, x0, y0, x1, y1, bd->bdBytesPerRow );

	    while( e >= 0 )
		{
		x0++; x1++; e -= d2;
		(*drawLine)( buffer, x0, y0, x1, y1, bd->bdBytesPerRow );
		}
	    y0++; y1++; e += e2;
	    }
	}

    return 0;
    }
