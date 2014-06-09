#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<appMetafile.h>
#   include	<utilPs.h>
#   include	<sioEndian.h>

#   include	<appDebugon.h>

void appMetafilePolygonPathPs(		PrintingState *		ps,
					const APP_POINT *	points,
					int			count,
					const char *		operator )
    {
    SimpleOutputStream *	sos= ps->psSos;

    int				done;
    char *			command;
    int				x0;
    int				y0;


    command= "bp"; x0= 0; y0= 0;

    for ( done= 0; done < count; done++ )
	{
	sioOutPrintf( sos, "%d %d %s",
			    points[done].x- x0,
			    points[done].y- y0,
			    command );

	if  ( done % 8 == 7 )
	    { sioOutPutCharacter( '\n', sos );		}
	else{ sioOutPutCharacter( ' ', sos );		}

	command= "rl"; x0= points[done].x; y0= points[done].y;
	}

    if  ( operator )
	{ sioOutPrintf( sos, "%s", operator );	}

    return;
    }

void appMetafileRoundRectPathPs(	PrintingState *		ps,
					int			x0,
					int			y0,
					int			x1,
					int			y1,
					int			w,
					int			h,
					const char *		operator )
    {
    SimpleOutputStream *	sos= ps->psSos;

    int				top;
    int				bottom;
    int				radius;

    int				swap;

    if  ( x1 < x0 )
	{ swap= x0; x0= x1; x1= swap; }
    if  ( y1 < y0 )
	{ swap= y0; y0= y1; y1= swap; }

    if  ( h < 0 )
	{ h= -h;	}
    if  ( w < 0 )
	{ w= -w;	}

    top=    w* ( y1- y0 )/ ( 2* h );
    bottom= w* ( y0- y1 )/ ( 2* h );
    radius= w/ 2;

    sioOutPrintf( sos, "%d %d bp\n", x0, bottom+ radius );
    sioOutPrintf( sos, "%d %d %d %d %d arct\n",
				    x0, top, x0+ radius, top, radius );
    sioOutPrintf( sos, "%d %d %d %d %d arct\n",
				    x1, top, x1, top- radius, radius );
    sioOutPrintf( sos, "%d %d %d %d %d arct\n",
				    x1, bottom, x1- radius, bottom, radius );
    sioOutPrintf( sos, "%d %d %d %d %d arct\n",
				    x0, bottom, x0, bottom+ radius, radius );

    if  ( operator )
	{ sioOutPrintf( sos, "%s", operator );	}

    return;
    }

void appMetafileRectPathPs(		PrintingState *		ps,
					int			x0,
					int			y0,
					int			x1,
					int			y1 )
    {
    int		dx= x1- x0;
    int		dy= y1- y0;

    sioOutPrintf( ps->psSos, "%d %d bp ",  x0,  y0 );
    sioOutPrintf( ps->psSos, "%d %d rl ",  dx,   0 );
    sioOutPrintf( ps->psSos, "%d %d rl ",   0,  dy );
    sioOutPrintf( ps->psSos, "%d %d rl ", -dx,   0 );

    sioOutPrintf( ps->psSos, "closepath\n" );

    return;
    }

/************************************************************************/
/*									*/
/*  Start a pattern fill.						*/
/*									*/
/************************************************************************/

int appMetafileStartPatternFillPs(	PrintingState *		ps,
					const AppBitmapImage *	abi )
    {
    SimpleOutputStream *	sos= ps->psSos;

    int				bytesPerRow;

    const int			useFilters= 0;
    const int			indexedImages= 0;
    BitmapPrinter		bp;

    bytesPerRow= bmPsRowStringSize( &(abi->abiBitmap),
						abi->abiBitmap.bdPixelsWide,
						indexedImages );
    if  ( bytesPerRow < 1 )
	{ LDEB(bytesPerRow); return -1;	}

    sioOutPrintf( sos, "currentfile %d string readhexstring\n",
				    abi->abiBitmap.bdPixelsHigh* bytesPerRow );

    bmPsOpenBitmapPrinter( &bp, sos, &(abi->abiBitmap),
						useFilters, indexedImages );

    if  ( bmPsWriteBitmapData( &bp, 0, 0,
				abi->abiBitmap.bdPixelsWide,
				abi->abiBitmap.bdPixelsHigh,
				&(abi->abiBitmap), abi->abiBuffer ) )
	{ LDEB(1); return -1;	}

    bmCloseBitmapPrinter( &bp );

    sioOutPrintf( sos, "%%\n" );
    sioOutPrintf( sos, "pop /fill-data exch store\n" );

    sioOutPrintf( sos, "/fill-wide %d store\n", abi->abiBitmap.bdPixelsWide );
    sioOutPrintf( sos, "/fill-high %d store\n", abi->abiBitmap.bdPixelsHigh );

    sioOutPrintf( sos, "/fill-cell " );
    sioOutPrintf( sos, "{ " );
    sioOutPrintf( sos, "gsave " );
    sioOutPrintf( sos, "%d %d scale\n" ,
			    abi->abiBitmap.bdPixelsWide,
			    abi->abiBitmap.bdPixelsHigh );

    sioOutPrintf( sos, "1 setgray 0 0 1 1 rectfill\n" );

    bmPsWriteImageInstructions( sos,
			    &(abi->abiBitmap),
			    abi->abiBitmap.bdPixelsWide,
			    abi->abiBitmap.bdPixelsHigh,
			    "{ /fill-data load } bind",
			    indexedImages );

    sioOutPrintf( sos, "grestore " );
    sioOutPrintf( sos, "} bind def\n" );

    return 0;
    }
