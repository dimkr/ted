#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	<string.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Binary morphology operations.					*/
/*									*/
/************************************************************************/

typedef struct DilateJob
    {
    BitmapDescription		djBdOut;
    unsigned char *		djBufOut;
    unsigned char		djInvertMaskSel;
    const BitmapDescription *	djBdSel;
    const unsigned char *	djBufSel;
    int				djRowSelOrig;
    int				djColSelOrig;
    } DilateJob;

static void bmMorphoDilatePixel(	void *			voiddj,
					int			rowIn,
					int			colIn )
    
    {
    DilateJob *	dj= (DilateJob *)voiddj;

    bmDraw1BitImage( &(dj->djBdOut), dj->djBufOut,
					dj->djBdSel, dj->djBufSel,
					dj->djInvertMaskSel,
					rowIn- dj->djRowSelOrig,
					colIn- dj->djColSelOrig );

    return;
    }

/************************************************************************/
/*									*/
/*  Check the input of a morphology operation.				*/
/*									*/
/*  1)  Check the format of the input image.				*/
/*  2)  Check the format of the structuring element.			*/
/*									*/
/************************************************************************/

static int bmMorphoCheckInputFormats(	const BitmapDescription *	bdIn,
					const BitmapDescription *	bdSel )
    {
    /*  1  */
    switch( bdIn->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	    if  ( bdIn->bdBitsPerPixel != 1 )
		{
		LLDEB(bdIn->bdColorEncoding,bdIn->bdBitsPerPixel);
		return -1;
		}
	    break;

	default:
	    LLDEB(bdIn->bdColorEncoding,bdIn->bdBitsPerPixel);
	    return -1;
	}

    /*  2  */
    if  ( bdSel->bdColorEncoding != bdIn->bdColorEncoding )
	{
	LLDEB(bdSel->bdColorEncoding,bdIn->bdColorEncoding); 
	return -1;
	}

    if  ( bdSel->bdBitsPerPixel != 1 )
	{
	LLDEB(bdSel->bdColorEncoding,bdSel->bdBitsPerPixel);
	return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remember what to do.						*/
/*									*/
/************************************************************************/

static void bmMorphoStartDilateJob(
				DilateJob *			dj,
				const BitmapDescription *	bdSel,
				const unsigned char *		bufSel,
				int				rowOrig,
				int				colOrig )
    {
    bmInitDescription( &(dj->djBdOut) );

    dj->djBufOut= (unsigned char *)0;
    dj->djInvertMaskSel= 0x00;
    dj->djBdSel= bdSel;
    dj->djBufSel= bufSel;
    dj->djRowSelOrig= rowOrig;
    dj->djColSelOrig= colOrig;

    return;
    }

/************************************************************************/
/*									*/
/*  Dilate an image.							*/
/*									*/
/*  As a completely arbitrary decision, the '1' value in both the	*/
/*  input and the output have been chosen as the 'black' value of the	*/
/*  morphology books. In the most common WHITEBLACK case of bitmaps,	*/
/*  this means that the colors are swapped.				*/
/*									*/
/*  1)  Allocate output resources.					*/
/*  2)  Determine the mask to ignore the trailing bits in the input	*/
/*	image.								*/
/*  3)  Dilate the individual rows.					*/
/*									*/
/************************************************************************/

static int bmMorphoDilateLow(	DilateJob *			dj,
				BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				unsigned char			invertMaskIn )
    {
    int			rval= 0;

    /*  1  */
    if  ( bmCopyDescription( &(dj->djBdOut), bdIn ) )
	{ LDEB(1); rval= -1; goto ready;	}

    dj->djBufOut= malloc( dj->djBdOut.bdBufferLength+ 1 );
    if  ( ! dj->djBufOut )
	{
	LXDEB(dj->djBdOut.bdBufferLength,dj->djBufOut);
	rval= -1; goto ready;
	}

#   if	PARANOIA
    bdSelPara= bdSel;
    bdOutPara= &dj->djBdOut;
#   endif

    memset( dj->djBufOut, 0, dj->djBdOut.bdBufferLength+ 1 );

    bmForAll1Pixels( bdIn, bufIn, invertMaskIn,
					(void *)dj, bmMorphoDilatePixel );

    if  ( bmCopyDescription( bdOut, &(dj->djBdOut) ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  steal  */
    *pBufOut= dj->djBufOut;
    dj->djBufOut= (unsigned char *)0;

  ready:

    if  ( dj->djBufOut )
	{ free( dj->djBufOut );	}

    bmCleanDescription( &(dj->djBdOut) );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Dilate an image.							*/
/*									*/
/************************************************************************/

int bmMorphoDilate(		BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				const BitmapDescription *	bdSel,
				const unsigned char *		bufSel,
				int				rowOrig,
				int				colOrig )
    {
    const unsigned char	invertMaskIn= 0x00;
    DilateJob		dj;

    if  ( bmMorphoCheckInputFormats( bdIn, bdSel ) )
	{ LDEB(1); return -1;	}

    bmMorphoStartDilateJob( &dj, bdSel, bufSel, rowOrig, colOrig );

    return bmMorphoDilateLow( &dj, bdOut, bdIn, pBufOut, bufIn, invertMaskIn );
    }

/************************************************************************/
/*									*/
/*  Erode an image.							*/
/*									*/
/*  Use the fact that the erosion is the complement of the dilation of	*/
/*  the complement of the input with the mirrored structuring element.	*/
/*									*/
/************************************************************************/

int bmMorphoErode(		BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				const BitmapDescription *	bdSel,
				const unsigned char *		bufSel,
				int				rowOrig,
				int				colOrig )
    {
    int			rval= 0;
    const unsigned char	invertMaskIn= 0xff;
    DilateJob		dj;

    unsigned char *	bufRot= (unsigned char *)0;
    BitmapDescription	bdRot;

    int			i;
    unsigned char *	to;

    bmInitDescription( &bdRot );

    if  ( bmMorphoCheckInputFormats( bdIn, bdSel ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( bmUpsideDown( &bdRot, bdSel, &bufRot, bufSel, 180 ) )
	{ LDEB(180); rval= -1; goto ready;	}

    bmMorphoStartDilateJob( &dj, &bdRot, bufRot,
				bdSel->bdPixelsHigh- rowOrig- 1,
				bdSel->bdPixelsWide- colOrig- 1 );

    if  ( bmMorphoDilateLow( &dj, bdOut, bdIn, pBufOut, bufIn, invertMaskIn ) )
	{ XDEB(invertMaskIn); rval= -1; goto ready;	}

    to= *pBufOut;
    for ( i= 0; i < bdOut->bdBufferLength; i++, to++ )
	{ to[0]= ~to[0]; }

  ready:

    if  ( bufRot )
	{ free( bufRot );	}

    bmCleanDescription( &bdRot );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Return the 'Simple' structuring element.				*/
/*									*/
/************************************************************************/

int bmMorphoSetSimpleSe(	BitmapDescription *		bdOut,
				unsigned char **		pBufOut )
    {
    const int		size= 3;
    const int		wide= size;
    const int		high= size;

    int			bytesPerRow;
    int			bufferLength;

    unsigned char *	bufOut;

    int			row;
    unsigned char	last;

    bytesPerRow= ( wide+ 7 )/ 8;
    bufferLength= high* bytesPerRow;

    bufOut= malloc( bufferLength );
    if  ( ! bufOut )
	{ LXDEB(bufferLength,bufOut); return -1;	}

    last= 0xff;
    if  ( wide % 8 )
	{ last= ( 0xff << ( 8- wide % 8 ) ) & 0xff;	}

    for ( row= 0; row < high; row++ )
	{
	unsigned char *	to= bufOut+ row* bytesPerRow;
	int		byte;

	for ( byte= 0; byte < bytesPerRow- 1; to++, byte++ )
	    { *to= 0xff; }
	*to= last;
	}

    bmInitDescription( bdOut );

    bdOut->bdColorEncoding= BMcoWHITEBLACK;
    bdOut->bdPixelsWide= wide;
    bdOut->bdPixelsHigh= high;
    bdOut->bdBitsPerSample= 1;
    bdOut->bdSamplesPerPixel= 1;
    bdOut->bdBitsPerPixel= 1;
    bdOut->bdXResolution= 1;
    bdOut->bdYResolution= 1;
    bdOut->bdUnit= BMunPIXEL;
    bdOut->bdColorEncoding= BMcoWHITEBLACK;

    bdOut->bdBytesPerRow= bytesPerRow;
    bdOut->bdBufferLength= bufferLength;

    *pBufOut= bufOut;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Dilate/Erode an image using the 'Simple' structuring element.	*/
/*									*/
/*  For Erosion, use the approach of bmMorphoErode(), but with some	*/
/*  shortcuts to avoid double work.					*/
/*									*/
/************************************************************************/

int bmMorphoDilateSimple(	BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				int				ignoredInt )
    {
    int			rval= 0;

    BitmapDescription	bdSel;
    unsigned char *	bufSel= (unsigned char *)0;

    const int		rowOrig= 1;
    const int		colOrig= 1;

    bmInitDescription( &bdSel );

    if  ( bmMorphoSetSimpleSe( &bdSel, &bufSel ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( bmMorphoDilate( bdOut, bdIn, pBufOut, bufIn,
					&bdSel, bufSel, rowOrig, colOrig ) )
	{ LLDEB(rowOrig,colOrig); rval= -1; goto ready;	}

  ready:

    if  ( bufSel )
	{ free( bufSel );	}

    bmCleanDescription( &bdSel );

    return rval;
    }

int bmMorphoErodeSimple(	BitmapDescription *		bdOut,
				const BitmapDescription *	bdIn,
				unsigned char **		pBufOut,
				const unsigned char *		bufIn,
				int				ignoredInt )
    {
    int			rval= 0;
    const unsigned char	invertMaskIn= 0xff;
    DilateJob		dj;

    BitmapDescription	bdSel;
    unsigned char *	bufSel= (unsigned char *)0;

    const int		rowOrig= 1;
    const int		colOrig= 1;

    int			i;
    unsigned char *	to;

    bmInitDescription( &bdSel );

    if  ( bmMorphoSetSimpleSe( &bdSel, &bufSel ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( bmMorphoCheckInputFormats( bdIn, &bdSel ) )
	{ LDEB(1); rval= -1; goto ready;	}

    bmMorphoStartDilateJob( &dj, &bdSel, bufSel, rowOrig, colOrig );

    if  ( bmMorphoDilateLow( &dj, bdOut, bdIn, pBufOut, bufIn, invertMaskIn ) )
	{ XDEB(invertMaskIn); rval= -1; goto ready;	}

    to= *pBufOut;
    for ( i= 0; i < bdOut->bdBufferLength; i++, to++ )
	{ to[0]= ~to[0]; }

  ready:

    if  ( bufSel )
	{ free( bufSel );	}

    bmCleanDescription( &bdSel );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Make an element to recognize lines.					*/
/*									*/
/************************************************************************/

int bmMorphoLineElement(	BitmapDescription *		bdOut,
				unsigned char **		pBufOut,
				int				wide,
				int				high,
				int				x0,
				int				y0,
				int				x1,
				int				y1 )
    {
    int			bytesPerRow;
    int			bufferLength;

    unsigned char *	bufOut;

    if  ( x0 < 0 || x0 >= wide )
	{ LLDEB(x0,wide); return -1;	}
    if  ( x1 < 0 || x1 >= wide )
	{ LLDEB(x1,wide); return -1;	}

    if  ( y0 < 0 || y0 >= high )
	{ LLDEB(y0,high); return -1;	}
    if  ( y1 < 0 || y1 >= high )
	{ LLDEB(y1,high); return -1;	}

    bytesPerRow= ( wide+ 7 )/ 8;
    bufferLength= high* bytesPerRow;

    bufOut= malloc( bufferLength );
    if  ( ! bufOut )
	{ LXDEB(bufferLength,bufOut); return -1;	}

    bmInitDescription( bdOut );

    bdOut->bdColorEncoding= BMcoWHITEBLACK;
    bdOut->bdPixelsWide= wide;
    bdOut->bdPixelsHigh= high;
    bdOut->bdBitsPerSample= 1;
    bdOut->bdSamplesPerPixel= 1;
    bdOut->bdBitsPerPixel= 1;
    bdOut->bdXResolution= 1;
    bdOut->bdYResolution= 1;
    bdOut->bdUnit= BMunPIXEL;
    bdOut->bdColorEncoding= BMcoWHITEBLACK;

    bdOut->bdBytesPerRow= bytesPerRow;
    bdOut->bdBufferLength= bufferLength;

    memset( bufOut, 0x00, bufferLength );

    bmDrawLine( bufOut, bdOut, x0, y0, x1, y1, 1 );

    *pBufOut= bufOut;
    return 0;
    }

