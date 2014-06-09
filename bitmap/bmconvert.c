#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	"bmgetrow.h"
#   include	"bmputrow.h"
#   include	<string.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Scaling, Dithering, Antialiassing.					*/
/*									*/
/************************************************************************/

static int bmBuildImage(	FillJob *			fj,
				unsigned char *			bufferOut,
				const unsigned char *		bufferIn,
				const BitmapDescription *	bdOut,
				const BitmapDescription *	bdIn,
				GetSourceRow			getSource,
				PutScreenRow			putRow )
    {
    int				rval= 0;

    int				rowOut;
    int				rowIn;

    const unsigned char *	from;
    unsigned char *		to;

    int				toWide= bdOut->bdPixelsWide;
    int				toHigh= bdOut->bdPixelsHigh;
    int				frWide= bdIn->bdPixelsWide;
    int				frHigh= bdIn->bdPixelsHigh;

    int				e;
    int				d2;
    int				e2;

    if  ( toHigh <= frHigh )
	{
	e2= 2* frHigh;
	d2= 2* toHigh;
	e= e2- toHigh;

	bmInitColorRow( fj->fjNextCor+ 1, toWide );

	rowIn= 0;
	for ( rowOut= 0; rowOut < toHigh; rowOut++ )
	    {
	    bmInitColorRow( fj->fjThisRow+ 1, frWide );
	    to= bufferOut+ rowOut* bdOut->bdBytesPerRow;

	    while( e >= 0 )
		{
		from= bufferIn+ rowIn* bdIn->bdBytesPerRow;

		(*getSource)( fj->fjThisRow+ 1, 0,
					from, 0, bdIn->bdPixelsWide, bdIn );

		rowIn++; e -= d2;
		}

	    e += e2;

	    if  ( (*putRow)( to, fj, fj->fjThisRow+ 1 ) )
		{ LDEB(rowOut); rval= -1; goto ready;	}
	    }
	}
    else{
	e2= 2* toHigh;
	d2= 2* frHigh;
	e= e2- frHigh;

	rowOut= 0;
	for ( rowIn= 0; rowIn < frHigh; rowIn++ )
	    {
	    from= bufferIn+ rowIn* bdIn->bdBytesPerRow;

	    bmInitColorRow( fj->fjThisRow+ 1, frWide );
	    (*getSource)( fj->fjThisRow+ 1, 0,
					from, 0, bdIn->bdPixelsWide, bdIn );

	    while( e >= 0 )
		{
		to= bufferOut+ rowOut* bdOut->bdBytesPerRow;

		if  ( (*putRow)( to, fj, fj->fjThisRow+ 1 ) )
		    { LDEB(rowOut); rval= -1; goto ready; }

		e -= d2; rowOut++;
		}

	    e += e2;
	    }
	}

  ready:

    return rval;
    }

int bmFillImage(	ColorAllocator *		ca,
			int				swapBitmapUnit,
			int				swapBitmapBytes,
			int				swapBitmapBits,
			int				dither,
			unsigned char *			bufferOut,
			const unsigned char *		bufferIn,
			const BitmapDescription *	bdOut,
			const BitmapDescription *	bdIn )
    {
    int			rval= 0;

    PutScreenRow	putRow= (PutScreenRow)0;
    GetSourceRow	getRow= (GetSourceRow)0;
    int			scratchSize= 0;

    FillJob		fj;

    bmInitFillJob( &fj );

    /*  1  */
    if  ( bmGetPutRow( &putRow, &scratchSize, ca,
					    swapBitmapUnit, swapBitmapBytes,
					    swapBitmapBits, bdOut ) )
	{ LDEB(1); rval= -1; goto ready; }

    if  ( bmGetGetRow( &getRow, bdIn ) )
	{ LDEB(1); return -1; }

    if  ( bmSetFillJob( &fj, ca,
			    bdIn->bdPixelsWide, bdOut->bdPixelsWide,
			    scratchSize, dither ) )
	{ LDEB(scratchSize); rval= -1; goto ready;	}

    if  ( bmBuildImage( &fj, bufferOut, bufferIn, bdOut, bdIn,
							    getRow, putRow ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    bmCleanFillJob( &fj );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Make a color allocator for a certain kind of image.			*/
/*									*/
/************************************************************************/

int bmSetColorAllocatorForImage(	ColorAllocator *		ca,
					const BitmapDescription *	bd )
    {
    switch( bd->bdColorEncoding )
	{
	case BMcoRGB8PALETTE:
	    if  ( bmSetColorAllocatorForPaletteImage( ca, bd ) )
		{ LDEB(1); return -1;	}
	    return 0;

	case BMcoBLACKWHITE:
	    if  ( bmSetColorAllocatorForBWImage( ca, bd ) )
		{ LDEB(1); return -1;	}
	    return 0;

	case BMcoWHITEBLACK:
	    if  ( bmSetColorAllocatorForWBImage( ca, bd ) )
		{ LDEB(1); return -1;	}
	    return 0;

	case BMcoRGB:
	    switch( bd->bdBitsPerSample )
		{
		case 8:
		    bmSetCalculatedShifts( ca, 0xff0000, 0x00ff00, 0x0000ff );
		    ca->caAllocationType= CA_CALCULATED;
		    return 0;

		default:
		    LLDEB(bd->bdColorEncoding,bd->bdBitsPerSample);
		    return -1;
		}

	default:
	    LDEB(bd->bdColorEncoding); return -1;
	}

    return 0;
    }

