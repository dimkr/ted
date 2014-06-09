#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	<string.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Draw images in another one.						*/
/*									*/
/************************************************************************/

# define PARANOIA 0

# if	PARANOIA

static const BitmapDescription * bdInPara;
static const BitmapDescription * bdToPara;

static unsigned char * bufToSav;
static const unsigned char * bufInSav;

static unsigned char oo[2000];
static unsigned char se[2000];
static unsigned char no[2000];

# define CHECK_RESULT() \
    \
    bmInflateTo8bit( no, bufToSav, bdToPara, 0, 0 ); \
    ocol= colTo0; \
    for ( col= colIn0; col < colInP; ocol++, col++ ) \
	{ \
	if  ( no[ocol] != ( oo[ocol] | se[col] ) ) \
	    { LLLDEB(no[ocol],oo[ocol],se[col]);LLDEB(ocol,col);abort();} \
	} \
    for ( ocol= 0; ocol < colTo0; ocol++ ) \
	{ \
	if  ( no[ocol] != oo[ocol] ) \
	    { LLLDEB(ocol,no[ocol],oo[ocol]);abort();} \
	} \
    for ( ocol= colTo0+ colInP- colIn0; \
				    ocol < bdToPara->bdPixelsWide; ocol++ ) \
	{ \
	if  ( no[ocol] != oo[ocol] ) \
	    { LLLDEB(ocol,no[ocol],oo[ocol]);abort();} \
	}

# define INIT_CHECK() \
    \
    int			ocol; \
\
    bufToSav= bufTo; \
    bufInSav= bufIn; \
\
    bmInflateTo8bit( oo, bufToSav, bdToPara, 0, 0 ); \
    bmInflateTo8bit( se, bufInSav, bdInPara, 0, 0 );

# endif


/************************************************************************/
/*									*/
/*  'Or'/'And' a shifted structuring element into the output buffer.	*/
/*									*/
/*  1)  Loop over the columns in the structuring element. NOTE that	*/
/*	the limits in the output are dictated by the size of the image	*/
/*	only, so we do not need to bother about masking off data to	*/
/*	protect bits in an output byte beyond colToP.			*/
/*									*/
/*  a)  Contribution of the first byte in the structuring element. As	*/
/*	its offset is left of the first output byte, only the left	*/
/*	shift is relevant.						*/
/*  b)  Contribution of all middle bytes in the structuring element.	*/
/*  c)  Contribution of the last byte in the strucuring element. Only	*/
/*	the right shift is relevant.					*/
/*									*/
/*  A)  Contribution of the first byte in the structuring element. As	*/
/*	its offset is right of the first output byte, only the right	*/
/*	shift is relevant for the first output byte. The left is for	*/
/*	the second output byte.						*/
/*  B)  Contribution of all middle bytes in the structuring element.	*/
/*  C)  Contribution of the last byte in the strucuring element. Only	*/
/*	the right shift is relevant.					*/
/*									*/
/************************************************************************/

typedef void (*OR_ROW)	(	unsigned char *		bufTo,
				const unsigned char *	bufIn,
				const int		colTo0,
				const int		colIn0,
				const int		colInP,
				int			shift0,
				int			shift1,
				unsigned char		inInvertMask,
				unsigned char		endMask );

/********************************/
/*				*/
/*  colIn0 % 8 > colTo0 % 8	*/
/*				*/
/********************************/

static void bmDrawOrRowRight(	unsigned char *		bufTo,
				const unsigned char *	bufIn,
				const int		colTo0,
				const int		colIn0,
				const int		colInP,
				int			shift0,
				int			shift1,
				unsigned char		inInvertMask,
				unsigned char		endMask )
    {
    int			col;
    unsigned char	mask;
    unsigned char	val;

#   if PARANOIA
    INIT_CHECK();
#   endif

    bufTo += colTo0/8;
    bufIn += colIn0/8;

    col= 8* ( colIn0/ 8 );
    mask= 0xff;

    if  ( col+ 8 > colInP )
	{ mask &= endMask; }

    /*  a  */
    val= bufIn[0] ^ inInvertMask;
    bufTo[0] |= ( val & mask ) << shift0;
    bufIn++, col += 8;

    /*  b  */
    for (; col+ 8 < colInP; bufIn++, col += 8 )
	{
	val= bufIn[0] ^ inInvertMask;

	bufTo[0] |= ( val        ) >> shift1;
	bufTo++;
	bufTo[0] |= ( val        ) << shift0;
	}

    /*  c  */
    if  ( col < colInP )
	{
	val= bufIn[0] ^ inInvertMask;

	if  ( col+ 8 > colInP )
	    { mask &= endMask; }

	bufTo[0] |= ( val & mask ) >> shift1;
	bufTo++;
	bufTo[0] |= ( val & mask ) << shift0;
	}

#   if PARANOIA
    CHECK_RESULT();
#   endif

    return;
    }

/********************************/
/*				*/
/*  colIn0 % 8 < colTo0 % 8	*/
/*				*/
/********************************/

static void bmDrawOrRowLeft(	unsigned char *		bufTo,
				const unsigned char *	bufIn,
				const int		colTo0,
				const int		colIn0,
				const int		colInP,
				int			shift0,
				int			shift1,
				unsigned char		inInvertMask,
				unsigned char		endMask )
    {
    int			col;
    unsigned char	mask;
    unsigned char	val;

#   if PARANOIA
    INIT_CHECK();
#   endif

    bufTo += colTo0/8;
    bufIn += colIn0/8;

    col= 8* ( colIn0/ 8 );
    mask= 0xff;

    if  ( col+ 8 > colInP )
	{ mask &= endMask; }

    /*  A  */
    val= bufIn[0] ^ inInvertMask;

    bufTo[0] |= ( val & mask ) >> shift0;
    bufTo++;
    bufTo[0] |= ( val & mask ) << shift1;
    bufIn++, col += 8;

    /*  B  */
    for (; col+ 8 < colInP; bufIn++, col += 8 )
	{
	val= bufIn[0] ^ inInvertMask;

	bufTo[0] |= ( val        ) >> shift0;
	bufTo++;
	bufTo[0] |= ( val        ) << shift1;
	}

    /*  C  */
    if  ( col < colInP )
	{
	val= bufIn[0] ^ inInvertMask;

	if  ( col+ 8 > colInP )
	    { mask &= endMask; }

	bufTo[0] |= ( val & mask ) >> shift0;
	bufTo++;
	bufTo[0] |= ( val & mask ) << shift1;
	}

#   if PARANOIA
    CHECK_RESULT();
#   endif

    return;
    }

/********************************/
/*				*/
/*  colIn0 % 8 == colTo0 % 8	*/
/*				*/
/********************************/

static void bmDrawOrRowEq(	unsigned char *		bufTo,
				const unsigned char *	bufIn,
				const int		colTo0,
				const int		colIn0,
				const int		colInP,
				int			ign_shift0,
				int			ign_shift1,
				unsigned char		inInvertMask,
				unsigned char		endMask )
    {
    int			col;
    unsigned char	mask;
    unsigned char	val;

#   if PARANOIA
    INIT_CHECK();
#   endif

    bufTo += colTo0/8;
    bufIn += colIn0/8;

    col= 8* ( colIn0/ 8 );
    mask= 0xff;

    if  ( col+ 8 > colInP )
	{ mask &= endMask; }

    /*  A  */
    val= bufIn[0] ^ inInvertMask;

    bufTo[0] |= ( val & mask );
    bufTo++;
    bufIn++, col += 8;

    /*  B  */
    for (; col+ 8 < colInP; bufIn++, col += 8 )
	{
	val= bufIn[0] ^ inInvertMask;

	bufTo[0] |= ( val        );
	bufTo++;
	}

    /*  C  */
    if  ( col < colInP )
	{
	val= bufIn[0] ^ inInvertMask;

	if  ( col+ 8 > colInP )
	    { mask &= endMask; }

	bufTo[0] |= ( val & mask );
	}

#   if PARANOIA
    CHECK_RESULT();
#   endif

    return;
    }

/************************************************************************/
/*									*/
/*  Insert an image in another one.					*/
/*									*/
/*  1)  Find the relevant rectangle on both images. It should not	*/
/*	protrude off one of the images.					*/
/*  2)  Determine how to mask the surplus bytes of the input image.	*/
/*  3)  Determine the best way to set the bits.				*/
/*  4)  Set the bits of the individual rows.				*/
/*									*/
/************************************************************************/

void bmDraw1BitImage(		const BitmapDescription *	bdTo,
				unsigned char *			bufTo,
				const BitmapDescription *	bdIn,
				const unsigned char *		bufIn,
				unsigned char			inInvertMask,
				int				rowTo0,
				int				colTo0 )
    {
    int			rowToP, colToP;
    int			rowIn0, colIn0;
    int			rowInP, colInP;

    int			rowTo;
    int			rowIn;

    int			shift0;
    int			shift1;

    unsigned char	endMask;

    OR_ROW		orRow= (OR_ROW)0;

    /*  1  */
    rowToP= rowTo0+ bdIn->bdPixelsHigh;
    colToP= colTo0+ bdIn->bdPixelsWide;

    rowIn0= 0;
    rowInP= bdIn->bdPixelsHigh;
    colIn0= 0;
    colInP= bdIn->bdPixelsWide;

    if  ( rowTo0 < 0 )
	{
	rowIn0 -= rowTo0;
	rowTo0=  0;
	}
    if  ( rowToP > bdTo->bdPixelsHigh )
	{
	rowInP -= rowToP- bdTo->bdPixelsHigh;
	rowToP= bdTo->bdPixelsHigh;
	}

    if  ( colTo0 < 0 )
	{
	colIn0 -= colTo0;
	colTo0=  0;
	}
    if  ( colToP > bdTo->bdPixelsWide )
	{
	colInP -= colToP- bdTo->bdPixelsWide;
	colToP= bdTo->bdPixelsWide;
	}

    /*  2  */
    endMask= 0xff;
    if  ( colInP % 8 )
	{ endMask= 0xff << ( 8- colInP % 8 );	}

    /*  3  */
    if  ( colIn0 % 8 > colTo0 % 8 )
	{
	shift0= colIn0 % 8- colTo0 % 8;
	shift1= 8- shift0;

	orRow= bmDrawOrRowRight;
	}
    else{
	shift0= colTo0 % 8- colIn0 % 8;
	shift1= 8- shift0;

	if  ( colIn0 % 8 < colTo0 % 8 )
	    { orRow= bmDrawOrRowLeft;	}
	else{ orRow= bmDrawOrRowEq;	}
	}

    /*
    LLLLDEB(rowTo0,rowToP,colTo0,colToP);
    LLLLDEB(rowIn0,rowInP,colIn0,colInP);
    */

    /*  4  */
    rowIn= rowIn0;
    for ( rowTo= rowTo0; rowTo < rowToP; rowIn++, rowTo++ )
	{
	unsigned char *		rowBufTo;
	const unsigned char *	rowBufIn;

	rowBufTo= bufTo+ rowTo* bdTo->bdBytesPerRow;
	rowBufIn= bufIn+ rowIn* bdIn->bdBytesPerRow;

	(*orRow)( rowBufTo, rowBufIn, colTo0, colIn0, colInP,
					shift0, shift1, inInvertMask, endMask );
	}

    return;
    }

