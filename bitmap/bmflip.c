#   include	"bitmapConfig.h"

#   include	<utilAffineTransform.h>
#   include	<geo2DInteger.h>

#   include	"bmintern.h"

#   include	<string.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<appDebugon.h>

#   ifndef	M_PI
#	define	M_PI	3.14159265358979323846
#   endif

/************************************************************************/
/*									*/
/*  Flip a scan line consisting of 1 bit pixels				*/
/*									*/
/************************************************************************/

static unsigned char	bmFlipLeft[256];
static unsigned char	bmFlipRight[256];

static void bmFlip1Init( int n )
    {
    int		i;
    int		rshift= n % 8;
    int		lshift= 8- rshift;

    for ( i= 0; i < 256; i++ )
	{
	unsigned char value=	( i & 0x01 ) << 7	|
				( i & 0x02 ) << 5	|
				( i & 0x04 ) << 3	|
				( i & 0x08 ) << 1	|
				( i & 0x10 ) >> 1	|
				( i & 0x20 ) >> 3	|
				( i & 0x40 ) >> 5	|
				( i & 0x80 ) >> 7	;

	bmFlipLeft[i]=  value << lshift;
	bmFlipRight[i]= value >> rshift;
	}
    }

static void bmFlip2Init( int n )
    {
    int		i;
    int		rshift= 2* ( n % 4 );
    int		lshift= 8- rshift;

    for ( i= 0; i < 256; i++ )
	{
	unsigned char value=	( i & 0x03 ) << 6	|
				( i & 0x0c ) << 2	|
				( i & 0x30 ) >> 2	|
				( i & 0xc0 ) >> 6	;

	bmFlipLeft[i]=  value << lshift;
	bmFlipRight[i]= value >> rshift;
	}
    }

static void bmFlip4Init( int n )
    {
    int		i;
    int		rshift= 4* ( n % 2 );
    int		lshift= 8- rshift;

    for ( i= 0; i < 256; i++ )
	{
	unsigned char value=	( i & 0x0f ) << 4	|
				( i & 0xf0 ) >> 4	;

	bmFlipLeft[i]=  value << lshift;
	bmFlipRight[i]= value >> rshift;
	}
    }

/************************************************************************/
/*									*/
/*  Flip the bytes in a scan line.					*/
/*									*/
/************************************************************************/

int bmFlipBytes(		unsigned char *	buffer,
				int		byteCount,
				int		bitsPerPixel )
    {
    switch( bitsPerPixel )
	{
	case  1:
	case  2:
	case  4:
	    bmFlip1Init( 8/bitsPerPixel );
	    break;

	case  8:
	    return 0;

	case 24:
	default:
	    LDEB(bitsPerPixel); return -1;
	}

    while( byteCount-- > 0 )
	{ *buffer= bmFlipRight[*buffer]; buffer++;	}

    return 0;
    }


/************************************************************************/
/*									*/
/*  Flip a scan line consisting of 1 bit pixels				*/
/*									*/
/************************************************************************/

static void bmFlipBits(	unsigned char *		to,
			const unsigned char *	from,
			int			pixelsWide,
			int			pixelsPerByte )
    {
    int			i;
    int			count;

    count= pixelsWide/ pixelsPerByte;
    to += count;

    if  ( pixelsWide % pixelsPerByte )
	{
	*(to)= bmFlipLeft[*from];
	to--;

	for ( i= count; i; i-- )
	    {
	    *to=       bmFlipRight[*(from++)];
	    *(to--) |= bmFlipLeft [* from   ];
	    }
	}
    else{
	for ( i= count; i; i-- )
	    { *(--to)= bmFlipRight[*(from++)];	}
	}
    }

/************************************************************************/
/*									*/
/*  Flip a scan line consisting of 8 bit pixels				*/
/*									*/
/************************************************************************/

static void bmFlip8Bits(	unsigned char *		to,
				const unsigned char *	from,
				int			pixelsWide,
				int			ignored	)
    {
    int			i;

    to += pixelsWide- 1;

    for ( i= pixelsWide; i; i-- )
	{ *(to--)= *(from++); }
    }

/************************************************************************/
/*									*/
/*  Flip a scan line consisting of 24 bit pixels			*/
/*									*/
/************************************************************************/

static void bmFlip24Bits(	unsigned char *		to,
				const unsigned char *	from,
				int			pixelsWide,
				int			ignored	)
    {
    int			i;

    to += 3*pixelsWide- 3;

    for ( i= pixelsWide; i; i-- )
	{
	to[0]= from[0]; to[1]= from[1]; to[2]= from[2];
	to -= 3; from += 3;
	}
    }

/************************************************************************/
/*									*/
/*  Mirror bitmap images: Flip over horizontal axis.			*/
/*									*/
/************************************************************************/

int bmVerticalFlip(	BitmapDescription *		bdOut,
			const BitmapDescription *	bdIn,
			unsigned char **		pBufOut,
			const unsigned char *		bufIn,
			int				ignoredInt )
    {
    const unsigned char *	from;
    unsigned char *		to;
    unsigned int		row;
    unsigned char * 		bufOut;

    bufOut= (unsigned char *)malloc( bdIn->bdBufferLength );
    if  ( ! bufOut )
	{ LLDEB(bdIn->bdBufferLength,bufOut); return -1;	}

    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
	{
	from= bufIn+ row* bdIn->bdBytesPerRow;
	to= bufOut+ ( bdIn->bdPixelsHigh- row -1 )* bdIn->bdBytesPerRow;

	memcpy( to, from, bdIn->bdBytesPerRow );
	}

    bmCopyDescription( bdOut, bdIn ); *pBufOut= bufOut;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Mirror bitmap images: Rotate 180 degrees.				*/
/*									*/
/************************************************************************/

int bmUpsideDown(	BitmapDescription *		bdOut,
			const BitmapDescription *	bdIn,
			unsigned char **		pBufOut,
			const unsigned char *		bufIn,
			int				ignoredInt )
    {
    const unsigned char *	from;
    unsigned char *		to;
    unsigned int		row;
    int				pixelsPerByte= 8/ bdIn->bdBitsPerPixel;
    unsigned char * 		bufOut;
    void			(*flip)( unsigned char *,
					const unsigned char *, int, int );

    bufOut= (unsigned char *)malloc( bdIn->bdBufferLength );
    if  ( ! bufOut )
	{ LLDEB(bdIn->bdBufferLength,bufOut); return -1;	}

    switch( bdIn->bdBitsPerPixel )
	{
	case  1:
	    flip= bmFlipBits;	bmFlip1Init( bdIn->bdPixelsWide ); break;
	case  2:
	    flip= bmFlipBits;	bmFlip2Init( bdIn->bdPixelsWide ); break;
	case  4:
	    flip= bmFlipBits;	bmFlip4Init( bdIn->bdPixelsWide ); break;
	case  8:
	    flip= bmFlip8Bits;	break;
	case 24:
	    flip= bmFlip24Bits;	break;
	default:
	    LDEB(bdIn->bdBitsPerPixel); return -1;
	}

    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
	{
	from= bufIn+ row* bdIn->bdBytesPerRow;
	to= bufOut+ ( bdIn->bdPixelsHigh- row -1 )* bdIn->bdBytesPerRow;

	(*flip)( to, from, bdIn->bdPixelsWide, pixelsPerByte );
	}

    bmCopyDescription( bdOut, bdIn ); *pBufOut= bufOut;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Mirror bitmap images: Flip over vertical axis.			*/
/*									*/
/************************************************************************/

int bmHorizontalFlip(	BitmapDescription *		bdOut,
			const BitmapDescription *	bdIn,
			unsigned char **		pBufOut,
			const unsigned char *		bufIn,
			int				ignoredInt )
    {
    const unsigned char *	from;
    unsigned char *		to;
    unsigned int		row;
    int				pixelsPerByte= 8/ bdIn->bdBitsPerPixel;
    unsigned char * 		bufOut;
    void			(*flip)( unsigned char *,
					const unsigned char *, int, int );

    bufOut= (unsigned char *)malloc( bdIn->bdBufferLength );
    if  ( ! bufOut )
	{ LLDEB(bdIn->bdBufferLength,bufOut); return -1;	}

    switch( bdIn->bdBitsPerPixel )
	{
	case  1:
	    flip= bmFlipBits;	bmFlip1Init( bdIn->bdPixelsWide ); break;
	case  2:
	    flip= bmFlipBits;	bmFlip2Init( bdIn->bdPixelsWide ); break;
	case  4:
	    flip= bmFlipBits;	bmFlip4Init( bdIn->bdPixelsWide ); break;
	case  8:
	    flip= bmFlip8Bits;	break;
	case 24:
	    flip= bmFlip24Bits;	break;
	default:
	    LDEB(bdIn->bdBitsPerPixel); return -1;
	}

    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
	{
	from= bufIn+ row* bdIn->bdBytesPerRow;
	to= bufOut+ row* bdIn->bdBytesPerRow;
	(*flip)( to, from, bdIn->bdPixelsWide, pixelsPerByte );
	}

    bmCopyDescription( bdOut, bdIn ); *pBufOut= bufOut;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Rotate a bitmap by 0,90,180,270 degrees.				*/
/*									*/
/************************************************************************/

int bmRotate90(	BitmapDescription *		bdOut,
		const BitmapDescription *	bdIn,
		unsigned char **		pBufOut,
		const unsigned char *		bufIn,
		int				angle )
    {
    const unsigned char *	from;
    unsigned char *		to;
    unsigned int		row, col;
    unsigned char * 		bufOut;
    BitmapDescription		bd;

    int				s;
    int				shift;
    unsigned char		m= 0;
    unsigned char		mr;
    int				step;

    switch( angle )
	{
	case 0:
	    if  ( bmCopyDescription( &bd, bdIn ) )
		{ LDEB(1); return -1;	}

	    bufOut= (unsigned char *)malloc( bd.bdBufferLength );
	    if  ( ! bufOut )
		{ LLDEB(bd.bdBufferLength,bufOut); return -1;	}

	    memcpy( bufOut, bufIn, bd.bdBufferLength );

	    *bdOut= bd; *pBufOut= bufOut;
	    return 0;
	case 180:
	    return bmUpsideDown( bdOut, bdIn, pBufOut, bufIn, angle );

	case 270:
	case 90:
	    break;

	default:
	    LDEB(angle); return -1;
	}


    bmCopyDescription( &bd, bdIn );
    bd.bdPixelsWide= bdIn->bdPixelsHigh;
    bd.bdPixelsHigh= bdIn->bdPixelsWide;
    bd.bdBytesPerRow= ( bd.bdPixelsWide* bdIn->bdBitsPerPixel+ 7 )/ 8;
    bd.bdBufferLength= bd.bdPixelsHigh* bd.bdBytesPerRow;

    bufOut= (unsigned char *)malloc( bd.bdBufferLength );
    if  ( ! bufOut )
	{ LLDEB(bd.bdBufferLength,bufOut); return -1;	}

    switch( bdIn->bdBitsPerPixel )
	{
	case  4:
	    m |= 0x30;
	    /*FALLTHROUGH*/
	case  2:
	    m |= 0x40;
	    /*FALLTHROUGH*/
	case  1:
	    m |= 0x80;

	    memset( bufOut, 0, bd.bdBufferLength );
	    step= 8/ bdIn->bdBitsPerPixel;

	    if  ( angle == 90 )
		{
		for ( row= 0; row < bd.bdPixelsHigh; row++ )
		    {
		    int		toRow= bd.bdPixelsHigh- row- 1;

		    mr= m >> bdIn->bdBitsPerPixel* ( row % step );

		    for ( s= 0; s < row % step; s++ )
			{
			to= bufOut+ toRow* bd.bdBytesPerRow;
			from= bufIn+ row/step+ s* bdIn->bdBytesPerRow;

			shift= row % step- s;
			shift *= bdIn->bdBitsPerPixel;

			for ( col= s; col < bd.bdPixelsWide; col += step )
			    {
			    *(to++) |= ( *from & mr ) << shift;
			    from += step* bdIn->bdBytesPerRow;
			    }
			}

		    for ( s= row % step; s < step; s++ )
			{
			to= bufOut+ toRow* bd.bdBytesPerRow;
			from= bufIn+ row/step+ s* bdIn->bdBytesPerRow;

			shift= s- row % step;
			shift *= bdIn->bdBitsPerPixel;

			for ( col= s; col < bd.bdPixelsWide; col += step )
			    {
			    *(to++) |= ( *from & mr ) >> shift;
			    from += step* bdIn->bdBytesPerRow;
			    }
			}
		    }
		}
	    else{
		/*270*/
		for ( row= 0; row < bd.bdPixelsHigh; row++ )
		    {
		    mr= m >> bdIn->bdBitsPerPixel* ( row % step );

		    for ( s= 0; s < row % step; s++ )
			{
			int ri= bdIn->bdPixelsHigh- 1;

			to= bufOut+ row* bd.bdBytesPerRow;
			from= bufIn+ ( bdIn->bdPixelsHigh- 1 )*
							bdIn->bdBytesPerRow;
			from -= s* bdIn->bdBytesPerRow;
			ri -= s;
			from += row/ step;

			shift= row % step- s;
			shift *= bdIn->bdBitsPerPixel;

			for ( col= 0; col < bd.bdPixelsWide; col += step )
			    {
			    if  ( ri < 0 )
				{ break;	}

			    *(to++) |= ( *from & mr ) << shift;
			    from -= step* bdIn->bdBytesPerRow;
			    ri -= step;
			    }
			}

		    for ( s= row % step; s < step; s++ )
			{
			int ri= bdIn->bdPixelsHigh- 1;

			to= bufOut+ row* bd.bdBytesPerRow;
			from= bufIn+ ( bdIn->bdPixelsHigh- 1 )*
							bdIn->bdBytesPerRow;
			from -= s* bdIn->bdBytesPerRow;
			ri -= s;
			from += row/ step;

			shift= s- row % step;
			shift *= bdIn->bdBitsPerPixel;

			for ( col= 0; col < bd.bdPixelsWide; col += step )
			    {
			    if  ( ri < 0 )
				{ break;	}

			    *(to++) |= ( *from & mr ) >> shift;
			    from -= step* bdIn->bdBytesPerRow;
			    ri -= step;
			    }
			}
		    }
		}

	    break;
	case  8:
	    if  ( angle == 90 )
		{
		for ( row= 0; row < bd.bdPixelsHigh; row++ )
		    {
		    from= bufIn+ row;
		    to= bufOut+ ( bd.bdPixelsHigh- row- 1 )* bd.bdBytesPerRow;

		    for ( col= 0; col < bd.bdPixelsWide; col++ )
			{ *(to++)= *from; from += bdIn->bdBytesPerRow; }
		    }
		}
	    else{
		/*270*/
		for ( row= 0; row < bd.bdPixelsHigh; row++ )
		    {
		    from= bufIn+ row;
		    to= bufOut+ ( row+ 1 )* bd.bdBytesPerRow- 1;

		    for ( col= 0; col < bd.bdPixelsWide; col++ )
			{ *(to--)= *from; from += bdIn->bdBytesPerRow; }
		    }
		}
	    break;

	case 24:
	    if  ( angle == 90 )
		{
		for ( row= 0; row < bd.bdPixelsHigh; row++ )
		    {
		    from= bufIn+ 3* row;
		    to= bufOut+ ( bd.bdPixelsHigh- row- 1 )* bd.bdBytesPerRow;

		    for ( col= 0; col < bd.bdPixelsWide; col++ )
			{
			*(to++)= from[0]; *(to++)= from[1]; *(to++)= from[2];
			from += bdIn->bdBytesPerRow;
			}
		    }
		}
	    else{
		for ( row= 0; row < bd.bdPixelsHigh; row++ )
		    {
		    from= bufIn+ 3* row;
		    to= bufOut+ ( row+ 1 )* bd.bdBytesPerRow- 3;

		    for ( col= 0; col < bd.bdPixelsWide; col++ )
			{
			to[0]= from[0]; to[1]= from[1]; to[2]= from[2];
			to -= 3; from += bdIn->bdBytesPerRow;
			}
		    }
		}
	    break;
	default:
	    LDEB(bdIn->bdBitsPerPixel); return -1;
	}

    *bdOut= bd; *pBufOut= bufOut;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Shear operations.							*/
/*									*/
/************************************************************************/

typedef void (*ShearRow)(	unsigned char *			rowBufOut,
				const unsigned char *		rowBufIn,
				const BitmapDescription *	bdIn,
				const BitmapDescription *	bdOut,
				int				colShift,
				int				fl );

typedef void (*ShearCol)(	unsigned char *			imgBufOut,
				const unsigned char *		imgBufIn,
				const BitmapDescription *	bdIn,
				const BitmapDescription *	bdOut,
				int				col,
				int				rowShift,
				int				fl );

static void bmShearRow8Bit(	unsigned char *			rowBufOut,
				const unsigned char *		rowBufIn,
				const BitmapDescription *	bdIn,
				const BitmapDescription *	bdOut,
				int				colShift,
				int				fl )
    {
    int		fr= 255- fl;
    int		spp= bdIn->bdSamplesPerPixel;
    int		i;

    int		colIn0;
    int		colIn1;
    int		colIn;
    int		colTo0;

    int		prev[5];

    /*  1  */
    colIn0= 0;
    colIn1= bdIn->bdPixelsWide- 1;

    if  ( colShift < 0 )
	{ colIn0= -colShift; }
    colTo0= colIn0+ colShift;

    if  ( colIn1+ colShift >= bdOut->bdPixelsWide )
	{ colIn1= bdOut->bdPixelsWide- colShift- 1; }

    rowBufOut += spp* colShift;

    for ( i= 0; i < spp; i++ )
	{ prev[i]= rowBufIn[i]; }

    for ( colIn= colIn0; colIn <= colIn1; colIn++ )
	{
	const unsigned char *	f= rowBufIn;
	unsigned char *		t= rowBufOut;

	for ( i= 0; i < spp; i++ )
	    {
	    t[0]= ( ( fl* prev[i] )+ ( fr* f[0] ) )/ 255;
	    prev[i]= f[0];

	    f++; t++;
	    }

	rowBufOut += spp; rowBufIn += spp;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Shear one column in a one byte per sample image.			*/
/*									*/
/*  1)  For all rows of the input image.				*/
/*									*/
/************************************************************************/

static void bmShearCol8Bit(	unsigned char *			imgBufOut,
				const unsigned char *		imgBufIn,
				const BitmapDescription *	bdIn,
				const BitmapDescription *	bdOut,
				int				col,
				int				rowShift,
				int				fl )
    {
    int				fr= 255- fl;
    int				spp= bdIn->bdSamplesPerPixel;
    int				i;

    int				rowIn0;
    int				rowIn1;
    int				rowIn;
    int				rowTo0;
    int				prev[5];

    const unsigned char *	from;
    unsigned char *		to;

    for ( i= 0; i < spp; i++ )
	{ prev[i]= 0; }

    /*  1  */
    rowIn0= 0;
    rowIn1= bdIn->bdPixelsHigh- 1;

    if  ( rowShift < 0 )
	{ rowIn0= -rowShift; }
    rowTo0= rowIn0+ rowShift;

    if  ( rowIn1+ rowShift >= bdOut->bdPixelsHigh )
	{ rowIn1= bdOut->bdPixelsHigh- rowShift- 1; }

    from= imgBufIn+ rowIn0* bdIn->bdBytesPerRow+ spp* col;
    to= imgBufOut+ rowTo0* bdOut->bdBytesPerRow+ spp* col;

    /*  1  */
    for ( rowIn= rowIn0; rowIn <= rowIn1; rowIn++ )
	{
	const unsigned char *	f= from;
	unsigned char *		t= to;

	for ( i= 0; i < spp; i++ )
	    {
	    t[0]= ( ( fl* prev[i] )+ ( fr* f[0] ) )/ 255;
	    prev[i]= f[0];

	    f++; t++;
	    }

	from += bdIn->bdBytesPerRow;
	to += bdOut->bdBytesPerRow;
	}

    return;
    }

static int bmShearRows(		unsigned char *			bufOut,
				const BitmapDescription *	bdOut,
				int				shear,
				const DocumentRectangle *	drOut,
				const BitmapDescription *	bdIn,
				const unsigned char *		bufIn,
				ShearRow			shearRow )
    {
    int				row;

    int				fl;
    int				colShift;

    unsigned char *		rowBufOut;
    const unsigned char *	rowBufIn;

    if  ( bdOut->bdPixelsHigh != bdIn->bdPixelsHigh )
	{ LLDEB(bdOut->bdPixelsHigh,bdIn->bdPixelsHigh); return -1; }

    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
	{
	rowBufOut= bufOut+ row* bdOut->bdBytesPerRow;
	rowBufIn= bufIn+ row* bdIn->bdBytesPerRow;

	fl= ( 255* shear* row )/ (int)bdIn->bdPixelsHigh;
	colShift= ( shear* row )/ (int)bdIn->bdPixelsHigh;

	if  ( fl >= 0 )
	    { fl -= 255* colShift;		}
	else{ fl -= 255* colShift; fl += 255;	}

	colShift -= drOut->drX0;

	(*shearRow)( rowBufOut, rowBufIn, bdIn, bdOut, colShift, fl );
	}

    return 0;
    }

static int bmShearCols(		unsigned char *			bufOut,
				const BitmapDescription *	bdOut,
				int				shear,
				const DocumentRectangle *	drOut,
				const BitmapDescription *	bdIn,
				const unsigned char *		bufIn,
				ShearCol			shearCol )
    {
    int				col;

    int				fl;
    int				rowShift;

    if  ( bdOut->bdPixelsWide != bdIn->bdPixelsWide )
	{ LLDEB(bdOut->bdPixelsWide,bdIn->bdPixelsWide); return -1; }

    for ( col= 0; col < bdIn->bdPixelsWide; col++ )
	{
	fl= ( 255* shear* col )/ (int)bdIn->bdPixelsWide;
	rowShift= ( shear* col )/ (int)bdIn->bdPixelsWide;

	if  ( fl >= 0 )
	    { fl -= 255* rowShift;		}
	else{ fl -= 255* rowShift; fl += 255;	}

	rowShift -= drOut->drY0;

	(*shearCol)( bufOut, bufIn, bdIn, bdOut, col, rowShift, fl );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Apply an affine transform to the rectangle from the original image.	*/
/*									*/
/*  The routine used to determine the smallest rectangle that encloses	*/
/*  the result of an intermediate result in the rotation of an image.	*/
/*									*/
/************************************************************************/

static void bmTransformRectangle(
				BitmapDescription *		bd,
				Point2DI			to[4],
				DocumentRectangle *		drTo,
				const AffineTransform2D *	at,
				const Point2DI			from[4],
				const DocumentRectangle *	drFrom )
    {
    int		i;

    for ( i= 0; i < 4; i++ )
	{
	to[i].p2diX= AT2_X( from[i].p2diX- drFrom->drX0,
					    from[i].p2diY- drFrom->drY0, at );
	to[i].p2diY= AT2_Y( from[i].p2diX- drFrom->drX0,
					    from[i].p2diY- drFrom->drY0, at );
	}

    drTo->drX0= drTo->drX1= to[0].p2diX;
    drTo->drY0= drTo->drY1= to[0].p2diY;

    for ( i= 1; i < 4; i++ )
	{
	if  ( drTo->drX0 > to[i].p2diX )
	    { drTo->drX0=  to[i].p2diX;	}
	if  ( drTo->drY0 > to[i].p2diY )
	    { drTo->drY0=  to[i].p2diY;	}

	if  ( drTo->drX1 < to[i].p2diX )
	    { drTo->drX1=  to[i].p2diX;	}
	if  ( drTo->drY1 < to[i].p2diY )
	    { drTo->drY1=  to[i].p2diY;	}
	}

    bd->bdPixelsWide= drTo->drX1- drTo->drX0+ 1;
    bd->bdPixelsHigh= drTo->drY1- drTo->drY0+ 1;

    bmCalculateSizes( bd );

    return;
    }

/************************************************************************/
/*									*/
/*  Rotate a pixmap image over an arbitrary angle.			*/
/*									*/
/*  Use the algorithm outlined in:					*/
/*  PAETH, Alan: "A Fast Algorithm for General Raster Rotation" pp 179-	*/
/*  195 in: GLASSNER, Andrew S: "Graphics Gems", Academic Press,	*/
/*  Boston, 1990.							*/
/*									*/
/*  The two main advantages of the algorithm are fast and easy anti-	*/
/*  aliassing and the simple continuous loops that prevent holes in the	*/
/*  rendering.								*/
/*									*/
/*  Counterclockwise rotation of (x,y) onto (x',y') by an angle theta	*/
/*  is a multiplication of the vector my the matrix:			*/
/*	[  cos(theta) -sin(theta) ]					*/
/*	[  sin(theta)  cos(theta) ]					*/
/*  The matrix is orhogonal: the inverse is the transpose:		*/
/*	[  cos(theta)  sin(theta) ]					*/
/*	[ -sin(theta)  cos(theta) ]					*/
/*  It can be written as the product of three shears:			*/
/*	[ 1 a ] [ 1 0 ] [ 1 c ]   [ cos(theta) -sin(theta) ]		*/
/*	[ 0 1 ] [ b 1 ] [ 0 1 ] = [ sin(theta)  cos(theta) ]		*/
/*  Where:								*/
/*      a= -tan( theta/ 2 ).						*/
/*      b= sin( theta ).						*/
/*      c= -tan( theta/ 2 ).						*/
/*									*/
/*  1)  Find a theta <= 45 degrees.					*/
/*  2)  Initial X Shear.						*/
/*  3)  Subsequent Y Shear.						*/
/*  4)  Final X Shear.							*/
/*									*/
/************************************************************************/

int bmRotate(	BitmapDescription *		bdOut,
		const BitmapDescription *	bdIn,
		unsigned char **		pBufOut,
		const unsigned char *		bufIn,
		double				theta )
    {
    int				rval= 0;

    int				rot90;

    double			a;
    double			b;
    double			c;

    AffineTransform2D		C;
    AffineTransform2D		B;
    AffineTransform2D		A;

    Point2DI			rectIn[4];
    Point2DI			rectC[4];
    Point2DI			rectB[4];
    Point2DI			rectA[4];

    DocumentRectangle		drIn;
    DocumentRectangle		drC;
    DocumentRectangle		drB;
    DocumentRectangle		drA;

    int				shearC;
    int				shearB;
    int				shearA;

    BitmapDescription		bdC;
    BitmapDescription		bdB;
    BitmapDescription		bdA;

    unsigned char *		bufC= (unsigned char *)0;
    unsigned char *		bufB= (unsigned char *)0;
    unsigned char *		bufA= (unsigned char *)0;

    ShearRow			shearRow= (ShearRow)0;
    ShearCol			shearCol= (ShearCol)0;

    switch( bdIn->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	    switch( bdIn->bdBitsPerPixel )
		{
		case 8:
		    shearRow= bmShearRow8Bit;
		    shearCol= bmShearCol8Bit;
		    break;

		default:
		    LLDEB(bdIn->bdColorEncoding,bdIn->bdBitsPerPixel);
		    return -1;
		}
	    break;

	case BMcoRGB:
	    switch( bdIn->bdBitsPerSample )
		{
		case 8:
		    shearRow= bmShearRow8Bit;
		    shearCol= bmShearCol8Bit;
		    break;

		default:
		    LLDEB(bdIn->bdColorEncoding,bdIn->bdBitsPerPixel);
		    return -1;
		}
	    break;

	case BMcoRGB8PALETTE:
	    LDEB(bdIn->bdColorEncoding);
	    return -1;
	    break;

	default:
	    LDEB(bdIn->bdColorEncoding);
	    return -1;
	}

    bmInitDescription( &bdC );
    bmInitDescription( &bdB );
    bmInitDescription( &bdA );

    utilIdentityAffineTransform2D( &C );
    utilIdentityAffineTransform2D( &B );
    utilIdentityAffineTransform2D( &A );

    /*********/
    /*  0 1  */
    /*  3 2  */
    /*********/
    drIn.drX0= 0; drIn.drX1= bdIn->bdPixelsWide- 1;
    drIn.drY0= 0; drIn.drY1= bdIn->bdPixelsHigh- 1;

    rectIn[0].p2diX= 0;		rectIn[0].p2diY= 0;
    rectIn[1].p2diX= drIn.drX1; rectIn[1].p2diY= 0;
    rectIn[2].p2diX= drIn.drX1; rectIn[2].p2diY= drIn.drY1;
    rectIn[3].p2diX= 0;		rectIn[3].p2diY= drIn.drY1;

    /*  1  */
    while( theta < -M_PI/4 )
	{ theta += 2* M_PI;	}
    for ( rot90= 0; theta > M_PI/4; rot90++ )
	{ theta -= M_PI/2;	}
    rot90= rot90 % 4;

    c= -tan( theta/ 2.0 );
    b= sin( theta );
    a= -tan( theta/ 2.0 );

    C.at2Ayx= c;
    B.at2Axy= b;
    A.at2Ayx= a;

    /*  2  */
    bmCopyDescription( &bdC, bdIn );
    bmTransformRectangle( &bdC, rectC, &drC, &C, rectIn, &drIn );

    shearC= c* bdIn->bdPixelsHigh;

    bufC= malloc( bdC.bdBufferLength );
    if  ( ! bufC )
	{ LLDEB(bdC.bdBufferLength,bufC);	}
    memset( bufC, 0, bdC.bdBufferLength );

    if  ( bmShearRows( bufC, &bdC, shearC, &drC, bdIn, bufIn, shearRow ) )
	{ LDEB(1); rval= -1; goto ready; }

#   define	RET_C	0
#   if		RET_C
    bmCopyDescription( bdOut, &bdC );
    *pBufOut= bufC;
    bufC= (unsigned char *)0;
    goto ready;
#   endif
#   define	NO_C	0
#   if		NO_C
    bmCopyDescription(&bdC,bdIn);
    memcpy( bufC, bufIn, bdIn->bdBufferLength );
#   endif

    /*  3  */
    bmCopyDescription( &bdB, &bdC );
    bmTransformRectangle( &bdB, rectB, &drB, &B, rectC, &drC );

    shearB= b* bdC.bdPixelsWide;

    bufB= malloc( bdB.bdBufferLength );
    if  ( ! bufB )
	{ LLDEB(bdB.bdBufferLength,bufB);	}
    memset( bufB, 0, bdB.bdBufferLength );

    if  ( bmShearCols( bufB, &bdB, shearB, &drB, &bdC, bufC, shearCol ) )
	{ LDEB(1); rval= -1; goto ready; }

    free( bufC ); bufC= (unsigned char *)0;

#   define	RET_B	0
#   if		RET_B
    bmCopyDescription( bdOut, &bdB );
    *pBufOut= bufB;
    bufB= (unsigned char *)0;
    goto ready;
#   endif
#   define	NO_B	0
#   if		NO_B
    bmCopyDescription(&bdB,&bdC);
    memcpy( bufB, bufC, bdC.bdBufferLength );
#   endif

    /*  4  */
    bmCopyDescription( &bdA, &bdB );
    bmTransformRectangle( &bdA, rectA, &drA, &A, rectB, &drB );

    shearA= a* bdB.bdPixelsHigh;

    bufA= malloc( bdA.bdBufferLength );
    if  ( ! bufA )
	{ LLDEB(bdA.bdBufferLength,bufA);	}
    memset( bufA, 0, bdA.bdBufferLength );

    if  ( bmShearRows( bufA, &bdA, shearA, &drA, &bdB, bufB, shearRow ) )
	{ LDEB(1); rval= -1; goto ready; }

    free( bufB ); bufB= (unsigned char *)0;

    if  ( rot90 != 0 )
	{
	if  ( bmRotate90( bdOut, &bdA, pBufOut, bufA, 90* rot90 ) )
	    { LDEB(rot90); rval= -1; goto ready; }
	}
    else{
	/*  steal */
	bmCopyDescription( bdOut, &bdA );
	*pBufOut= bufA;
	bufA= (unsigned char *)0;
	}

  ready:

    if  ( bufC )
	{ free( bufC );	}
    if  ( bufB )
	{ free( bufB );	}
    if  ( bufA )
	{ free( bufA );	}

    bmCleanDescription( &bdC );
    bmCleanDescription( &bdB );
    bmCleanDescription( &bdA );

    return rval;
    }
