/************************************************************************/
/*									*/
/*  Shading.								*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>
#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Derive rgb values for a solidly shaded cell.			*/
/*									*/
/************************************************************************/

int docGetSolidRgbShadeOfItem(	int *			pIsSolid,
				int *			pRed,
				int *			pGreen,
				int *			pBlue,
				const BufferDocument *	bd,
				const ItemShading *	is )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);

    RGB8Color			cf;
    RGB8Color			cb;

    int				fl= is->isLevel;
    int				bl= 10000- is->isLevel;

    int				r;
    int				g;
    int				b;

    cf.rgb8Red= 0;
    cf.rgb8Green= 0;
    cf.rgb8Blue= 0;

    cb.rgb8Red= 255;
    cb.rgb8Green= 255;
    cb.rgb8Blue= 255;

    if  ( is->isPattern != DOCspSOLID )
	{ LDEB(is->isPattern); return -1;	}

    if  ( is->isBackColor <= 0		&&
	  is->isForeColor <= 0		&&
	  is->isLevel == 0		)
	{ *pIsSolid= 0; return 0;	}

    if  ( is->isBackColor > 0			&&
	  is->isBackColor < dp->dpColorCount	)
	{ cb= dp->dpColors[is->isBackColor];	}

    if  ( is->isLevel > 0 )
	{
	if  ( is->isForeColor > 0			&&
	      is->isForeColor < dp->dpColorCount	)
	    { cf= dp->dpColors[is->isForeColor];	}
	}
    else{ cf= cb; }

    r= ( fl* cf.rgb8Red+ bl* cb.rgb8Red )/ 10000;
    g= ( fl* cf.rgb8Green+ bl* cb.rgb8Green )/ 10000;
    b= ( fl* cf.rgb8Blue+ bl* cb.rgb8Blue )/ 10000;

    if  ( r == 255 && g == 255 && b == 255 )
	{ *pIsSolid= 0;	}
    else{
	*pIsSolid= 1;
	*pRed= r;
	*pGreen= g;
	*pBlue= b;
	}

    return 0;
    }

