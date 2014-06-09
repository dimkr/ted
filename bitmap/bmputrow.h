
#   ifndef	BM_PUT_ROW_H
#   define	BM_PUT_ROW_H

#   include	"bmintern.h"
#   include	"bmgetrow.h"

/************************************************************************/
/*									*/
/*  Store rows of data in an image.					*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Data structure for filling an image.				*/
/*									*/
/************************************************************************/

typedef struct FillJob
    {
    ColorAllocator *	fjColorAllocator;
    int			fjFrWide;
    int			fjToWide;
    int			fjDither;

    unsigned char *	fjScratch;
    ColorValue *	fjThisRow;
    ColorValue *	fjNextCor;
    ColorValue *	fjPrevRow;
    ColorValue *	fjNextRow;

    int			fjDc1[512];
    int			fjDc3[512];
    int			fjDc5[512];
    int			fjDc7[512];
    } FillJob;

typedef int (*PutScreenRow)(	unsigned char *		to,
				const FillJob *		fj,
				ColorValue *		val );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int bmGetPutRow(		PutScreenRow *			pPutRow,
				int *				pScratchSize,
				const ColorAllocator *		ca,
				int				swapBitmapUnit,
				int				swapBitmapBytes,
				int				swapBitmapBits,
				const BitmapDescription *	bdOut );

extern void bmInitFillJob(	FillJob *			fj );
extern void bmCleanFillJob(	FillJob *			fj );

extern int bmSetFillJob(	FillJob *			fj,
				ColorAllocator *		ca,
				int				frWide,
				int				toWide,
				int				scratchSize,
				int				dither );

#   endif	/*  BM_PUT_ROW_H	*/

