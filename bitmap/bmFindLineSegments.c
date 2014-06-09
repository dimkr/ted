#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	<string.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Find line segments in an image.					*/
/*									*/
/************************************************************************/

typedef struct FindLineSegmentsJob
    {
    int				flsjPixelSize;

    int				flsjX0;
    int				flsjY0;
    int				flsjX1;
    int				flsjY1;

    BitmapDescription		flsjBdSel;
    unsigned char *		flsjBufSel;

    BitmapDescription		flsjBdSegs;
    unsigned char *		flsjBufSegs;

    void *			flsjThrough;
    BM_SEG_FUN			flsjSegFun;
    } FindLineSegmentsJob;

static void bmFoundLineSegment(		void *			voidflsj,
					int			row,
					int			col )
    
    {
    FindLineSegmentsJob *	flsj= (FindLineSegmentsJob *)voidflsj;

    (*flsj->flsjSegFun)( flsj->flsjThrough,
				flsj->flsjX0+ col, flsj->flsjY0+ row,
				flsj->flsjX1+ col, flsj->flsjY1+ row );
    return;
    }

/************************************************************************/
/*									*/
/*  Start a FindLineSegments job.					*/
/*									*/
/************************************************************************/

static void bmInitFindLineSegmentsJob(	FindLineSegmentsJob *	flsj )
    {
    flsj->flsjPixelSize= 0;

    flsj->flsjX0= 0;
    flsj->flsjY0= 0;
    flsj->flsjX1= 0;
    flsj->flsjY1= 0;

    bmInitDescription( &(flsj->flsjBdSel) );
    flsj->flsjBufSel= (unsigned char *)0;

    bmInitDescription( &(flsj->flsjBdSegs) );
    flsj->flsjBufSegs= (unsigned char *)0;

    flsj->flsjThrough= (void *)0;
    flsj->flsjSegFun= (BM_SEG_FUN)0;

    return;
    }

static int bmStartFindLineSegmentsJob(	FindLineSegmentsJob *	flsj,
					void *			through,
					BM_SEG_FUN		segFun,
					int			size,
					int			x0,
					int			y0,
					int			x1,
					int			y1 )
    {
    if  ( bmMorphoLineElement( &(flsj->flsjBdSel), &(flsj->flsjBufSel),
					    size, size, x0, y0, x1, y1 ) )
	{ LDEB(size); return -1;	}

    flsj->flsjPixelSize= size;

    flsj->flsjX0= x0;
    flsj->flsjY0= y0;
    flsj->flsjX1= x1;
    flsj->flsjY1= y1;

    flsj->flsjThrough= through;
    flsj->flsjSegFun= segFun;

    return 0;
    }

static void bmCleanFindLineSegmentsJob(	FindLineSegmentsJob *	flsj )
    {
    /**/
    bmCleanDescription( &(flsj->flsjBdSel) );

    if  ( flsj->flsjBufSel )
	{ free( flsj->flsjBufSel );	}

    /**/
    bmCleanDescription( &(flsj->flsjBdSegs) );

    if  ( flsj->flsjBufSegs )
	{ free( flsj->flsjBufSegs );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Find the line segments of a certain fixed length in an image.	*/
/*									*/
/*  This is done by eroding the image with structuring elements that	*/
/*  correspond to the possible line segments through the center of	*/
/*  a square with the given size.					*/
/*									*/
/************************************************************************/

static int bmRunFindLineSegmentsJob(	FindLineSegmentsJob *		flsj,
					const BitmapDescription *	bdIn,
					const unsigned char *		bufIn,
					void *				through,
					BM_SEG_FUN			segFun,
					int				size,
					int				x0,
					int				y0,
					int				x1,
					int				y1 )
    {
    const unsigned char	invertMaskSegs= 0x00;

    if  ( bmStartFindLineSegmentsJob( flsj, through, segFun,
						    size, x0, y0, x1, y1 ) )
	{ LDEB(size); return -1;	}

    if  ( bmMorphoErode( &(flsj->flsjBdSegs), bdIn,
			    &(flsj->flsjBufSegs), bufIn,
			    &(flsj->flsjBdSel), flsj->flsjBufSel,
			    ( size+ 1 )/ 2, ( size+ 1 )/ 2 ) )
	{ LDEB(1); return -1;	}

    bmForAll1Pixels( &(flsj->flsjBdSegs), flsj->flsjBufSegs,
			invertMaskSegs, (void *)flsj, bmFoundLineSegment );

    bmCleanFindLineSegmentsJob( flsj );
    bmInitFindLineSegmentsJob( flsj );

    return 0;
    }

int bmFindLineSegments(		const BitmapDescription *	bdIn,
				const unsigned char *		bufIn,
				int				size,
				void *				through,
				BM_SEG_FUN			segFun )
    {
    int				rval= 0;
    FindLineSegmentsJob		flsj;

    int				i;

    bmInitFindLineSegmentsJob( &flsj );

    if  ( bdIn->bdBitsPerPixel != 1 )
	{ LDEB(bdIn->bdBitsPerPixel); rval= -1; goto ready;	}

    for ( i= 0; i < size; i++ )
	{
	if  ( bmRunFindLineSegmentsJob( &flsj, bdIn, bufIn, through, segFun,
					size, i, 0, size- i- 1, size- 1 ) )
	    { LDEB(i); rval= -1; goto ready;	}

	if  ( bmRunFindLineSegmentsJob( &flsj, bdIn, bufIn, through, segFun,
					size, 0, i, size- 1, size- i- 1 ) )
	    { LDEB(i); rval= -1; goto ready;	}
	}

  ready:

    bmCleanFindLineSegmentsJob( &flsj );

    return rval;
    }

