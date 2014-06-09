#   ifndef	BM_GETROW_H
#   define	BM_GETROW_H

#   include	"bmintern.h"

/************************************************************************/
/*									*/
/*  Collect rows of image data for further processing.			*/
/*									*/
/*  For historical, and for efficiency reasons, there is not really an	*/
/*  elegant general approach.						*/
/*									*/
/************************************************************************/

typedef struct ColorValue
    {
    long	cvR;
    long	cvG;
    long	cvB;
    int		cvN;
    } ColorValue;

typedef void (*GetSourceRow)(	ColorValue *			cv,
				int				col0Out,
				const unsigned char *		from,
				int				col0In,
				int				colPIn,
				const BitmapDescription *	bdIn );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void bmInitColorRow(	ColorValue *			cv,
				int				wide );

extern int bmGetGetRow(		GetSourceRow *			pGetRow,
				const BitmapDescription *	bdIn );

#   endif
