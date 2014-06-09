#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	"bmgetrow.h"
#   include	"bmputrow.h"
#   include	<string.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Manage FillJob's for image conversion.				*/
/*									*/
/************************************************************************/

static void scanFillDitherTables(	FillJob *	fj )
    {
    int		i;

    for ( i= -256; i < 0; i++ )
	{
	fj->fjDc1[i+256]= -( ( 8- 1*i+ 8 )/ 16 );
	fj->fjDc3[i+256]= -( ( 8- 3*i+ 8 )/ 16 );
	fj->fjDc5[i+256]= -( ( 8- 5*i+ 8 )/ 16 );
	fj->fjDc7[i+256]= -( ( 8- 7*i+ 8 )/ 16 );
	}

    for ( i= 0; i < 256; i++ )
	{
	fj->fjDc1[i+256]= ( 1*i+ 8 )/ 16;
	fj->fjDc3[i+256]= ( 3*i+ 8 )/ 16;
	fj->fjDc5[i+256]= ( 5*i+ 8 )/ 16;
	fj->fjDc7[i+256]= ( 7*i+ 8 )/ 16;
	}
    }

void bmInitFillJob(	FillJob *	fj )
    {
    int		i;

    fj->fjColorAllocator= (ColorAllocator *)0;
    fj->fjFrWide= 0;
    fj->fjToWide= 0;

    fj->fjDither= 0;

    fj->fjScratch= (unsigned char *)0;
    fj->fjThisRow= (ColorValue *)0;
    fj->fjNextCor= (ColorValue *)0;

    fj->fjNextRow= (ColorValue *)0;
    fj->fjPrevRow= (ColorValue *)0;

    for ( i= 0; i < 512; i++ )
	{
	fj->fjDc1[i]= 0;
	fj->fjDc3[i]= 0;
	fj->fjDc5[i]= 0;
	fj->fjDc7[i]= 0;
	}

    return;
    }

void bmCleanFillJob(	FillJob *	fj )
    {
    if  ( fj->fjScratch )
	{ free( fj->fjScratch );	}

    if  ( fj->fjThisRow )
	{ free( fj->fjThisRow );	}

    if  ( fj->fjNextCor )
	{ free( fj->fjNextCor );	}

    if  ( fj->fjNextRow )
	{ free( fj->fjNextRow );	}

    if  ( fj->fjPrevRow )
	{ free( fj->fjPrevRow );	}

    return;
    }

int bmSetFillJob(		FillJob *			fj,
				ColorAllocator *		ca,
				int				frWide,
				int				toWide,
				int				scratchSize,
				int				dither )
    {
    fj->fjFrWide= frWide;
    fj->fjToWide= toWide;
    fj->fjColorAllocator= ca;

    if  ( scratchSize > 0 )
	{
	fj->fjScratch= malloc( scratchSize );
	if  ( ! fj->fjScratch )
	    { LXDEB(scratchSize,fj->fjScratch); return -1;	}
	}

    if  ( dither )
	{
	fj->fjDither= 1;
	scanFillDitherTables( fj );
	}

    /*  1  */
    fj->fjThisRow= (ColorValue *)malloc( ( frWide+ 9 )* sizeof(ColorValue) );
    if  ( ! fj->fjThisRow )
	{ LXDEB(frWide,fj->fjThisRow); return -1;	}

    fj->fjNextCor= (ColorValue *)malloc( ( toWide+ 9 )* sizeof(ColorValue) );
    if  ( ! fj->fjNextCor )
	{ LXDEB(toWide,fj->fjNextCor); return -1;	}

    fj->fjNextRow= (ColorValue *)malloc( ( frWide+ 9 )* sizeof(ColorValue) );
    if  ( ! fj->fjNextRow )
	{ LXDEB(frWide,fj->fjNextRow); return -1;	}

    fj->fjPrevRow= (ColorValue *)malloc( ( frWide+ 9 )* sizeof(ColorValue) );
    if  ( ! fj->fjPrevRow )
	{ LXDEB(frWide,fj->fjPrevRow); return -1;	}

    return 0;
    }

