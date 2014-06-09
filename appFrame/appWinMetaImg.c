#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<appWinMeta.h>
#   include	<sioEndian.h>

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	<appDebugon.h>

#   ifndef	M_PI
#	define	M_PI	3.14159265358979323846
#   endif

# if 0
#    define	WMFDEB(x)	(x)
# else
#    define	WMFDEB(x)	/*nothing*/
# endif

# if 0
#    define	WMFLOG(x)	(x)
# else
#    define	WMFLOG(x)	/*nothing*/
# endif

/************************************************************************/
/*									*/
/*  Retrieve the bitmap that might be in a WMF.				*/
/*									*/
/************************************************************************/

static int appMetaBitmapImageImg(	AppBitmapImage **	pAbi,
					SimpleInputStream *	sis,
					int			expectBytes,
					int			srcX,
					int			srcY,
					int			srcXExt,
					int			srcYExt,
					int			dstX,
					int			dstY,
					int			dstXExt,
					int			dstYExt )
    {
    AppBitmapImage *	abi;

    if  ( srcX != 0 || srcY != 0 )
	{ LLDEB(srcX,srcY); return 0;	}

    if  ( appMetaBitmapImage( &abi, sis, expectBytes ) )
	{ LDEB(expectBytes); return -1;	}

    if  ( srcXExt != abi->abiBitmap.bdPixelsWide	||
	  srcYExt != abi->abiBitmap.bdPixelsHigh	)
	{
	LLDEB(srcXExt,abi->abiBitmap.bdPixelsWide);
	LLDEB(srcYExt,abi->abiBitmap.bdPixelsHigh);
	appCleanBitmapImage( abi ); free( abi );
	return 0;
	}

    if  ( *pAbi )
	{ appCleanBitmapImage( *pAbi ); free( *pAbi ); }

    *pAbi= abi;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Play the metafile records in a metafile.				*/
/*									*/
/************************************************************************/

static int appWinMetaPlayRecordsImg(	SimpleInputStream *	sis,
					AppBitmapImage **	pAbi )
    {
    AppBitmapImage *	abi= (AppBitmapImage *)0;

    for (;;)
	{
	long		rop;

	int		x0;
	int		y0;
	int		w0;
	int		h0;

	int		x1;
	int		y1;
	int		w1;
	int		h1;

	int		done;
	int		ignore;

	long		recordSize;
	int		function;

	recordSize= sioEndianGetLeInt32( sis );
	function= sioEndianGetLeInt16( sis );

	if  ( recordSize < 0 || function < 0 )
	    { LLDEB(recordSize,function); return -1;	}

	switch( function )
	    {
	    case WINMETA_StretchBlt:
		rop= sioEndianGetLeInt32( sis );
		h0= sioEndianGetLeInt16( sis );
		w0= sioEndianGetLeInt16( sis );
		y0= sioEndianGetLeInt16( sis );
		x0= sioEndianGetLeInt16( sis );
		h1= sioEndianGetLeInt16( sis );
		w1= sioEndianGetLeInt16( sis );
		y1= sioEndianGetLeInt16( sis );
		x1= sioEndianGetLeInt16( sis );

		WMFDEB(appDebug("StretchBlt([%d+%d,%d+%d]->[%d+%d,%d+%d],..)\n",
				x0,w0,y0,h0, x1,w1,y1,h1 ));

		if  ( appMetaBitmapImageImg( &abi, sis, 2*(recordSize-3-2-8*1),
					x0, y0, w0, h0, x1, y1, w1, h1 ) )
		    { LDEB(1); return -1; }

		continue;
	    case WINMETA_StretchDIBits:
		rop= sioEndianGetLeInt32( sis );
		(void) sioEndianGetLeInt16( sis );
		/*  source	*/
		h0= sioEndianGetLeInt16( sis );
		w0= sioEndianGetLeInt16( sis );
		y0= sioEndianGetLeInt16( sis );
		x0= sioEndianGetLeInt16( sis );
		/*  destination	*/
		h1= sioEndianGetLeInt16( sis );
		w1= sioEndianGetLeInt16( sis );
		y1= sioEndianGetLeInt16( sis );
		x1= sioEndianGetLeInt16( sis );

		WMFDEB(appDebug(
			"StretchDIBits([%d+%d,%d+%d]->[%d+%d,%d+%d],..)\n",
			x0,w0,y0,h0, x1,w1,y1,h1 ));

		if  ( appMetaBitmapImageImg( &abi, sis, 2*(recordSize-3-2-9*1),
					x0, y0, w0, h0, x1, y1, w1, h1 ) )
		    { LDEB(1); return -1; }

		continue;

	    case 0:
		if  ( recordSize != 3 )
		    { XLDEB(function,recordSize); return -1;	}
		break;

	    default:
		for ( done= 3; done < recordSize; done++ )
		    { ignore= sioEndianGetLeInt16( sis ); }
		continue;
	    }

	break;
	}

    *pAbi= abi;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Play the contents of a 'wmf' original windows metafile.		*/
/*									*/
/************************************************************************/

int appMetaPlayWmfImg(	BitmapDescription *	pBd,
			unsigned char **	pBuffer,
			SimpleInputStream *	sis )
    {
    int			fileType;
    int			headerSize;
    int			windowsVersion;
    long		fileSize;
    int			objectCount;
    long		maxRecordSize;
    int			parameterCount;

    AppBitmapImage *	abi= (AppBitmapImage *)0;

    if  ( sioInGetCharacter( sis ) == EOF )
	{ return -1;	}
    sioInUngetLastRead( sis );

    fileType= sioEndianGetLeInt16( sis );
    headerSize= sioEndianGetLeInt16( sis );
    windowsVersion= sioEndianGetLeInt16( sis );
    fileSize= sioEndianGetLeInt32( sis );
    objectCount= sioEndianGetLeInt16( sis );
    maxRecordSize= sioEndianGetLeInt32( sis );
    parameterCount= sioEndianGetLeInt16( sis );

    WMFDEB(appDebug( "IMG WMF ...\n" ));

    if  ( appWinMetaPlayRecordsImg( sis, &abi ) )
	{ LDEB(1); return -1; }

    if  ( ! abi )
	{ /*XDEB(abi);*/ return 1;	}

    *pBd= abi->abiBitmap;
    *pBuffer= abi->abiBuffer;

    free( abi );

    return 0;
    }

