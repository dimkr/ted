/************************************************************************/
/*									*/
/*  Print images to PostScript.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	<appImage.h>
#   include	<appWinMeta.h>
#   include	<appMacPict.h>
#   include	<sioMemory.h>
#   include	<sioHex.h>

#   include	"docDraw.h"
#   include	"docPsPrint.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Print a series of particules with the same attributes.		*/
/*									*/
/************************************************************************/

typedef int (*PLAY_METAFILE)(	PrintingState *			ps,
				SimpleInputStream *		sis,
				const PostScriptFontList *	psfl,
				int				useFilters,
				int				indexedImages,
				int				mapMode,
				int				xWinExt,
				int				yWinExt,
				int				twipsWide,
				int				twipsHigh );

static int docPsPrintMetafile(	PrintingState *			ps,
				const PictureProperties *	pip,
				const MemoryBuffer *		mb,
				const PostScriptFontList *	psfl,
				int				x0,
				int				baseline )
    {
    int				scaleX= pip->pipScaleXUsed;
    int				scaleY= pip->pipScaleYUsed;
    int				xWinExt= pip->pip_xWinExt;
    int				yWinExt= pip->pip_yWinExt;
    int				twipsWide= pip->pipTwipsWide;
    int				twipsHigh= pip->pipTwipsHigh;

    SimpleInputStream *		sisMem;
    SimpleInputStream *		sisMeta;

    PostScriptTypeList		pstl;

    int				y0;

    PLAY_METAFILE		playMetafile;

    utilInitPostScriptFaceList( &pstl );

    if  ( docPsListImageFonts( &pstl, pip, mb, psfl, "pf" ) )
	{ LDEB(1); return -1;	}

    switch( pip->pipType )
	{
	case DOCokPICTWMETAFILE:
	    playMetafile= appMetaPlayFilePs;
	    break;

	case DOCokMACPICT:
	    playMetafile= appMacPictPlayFilePs;
	    break;

	case DOCokPICTJPEGBLIP:
	case DOCokPICTPNGBLIP:
	default:
	    LDEB(pip->pipType); return 0;
	}

    sisMem= sioInMemoryOpen( mb );
    if  ( ! sisMem )
	{ XDEB(sisMem); return -1;	}

    sisMeta= sioInHexOpen( sisMem );
    if  ( ! sisMeta )
	{ XDEB(sisMem); sioInClose( sisMem ); return -1;	}

    y0= baseline- ( ( scaleY/100.0 )* twipsHigh );

    sioOutPrintf( ps->psSos, "100 dict begin\n" );

    appPsFontNames( ps, &pstl, /*allFonts=*/ 1 );

    sioOutPrintf( ps->psSos, "gsave %d %d translate %%{IMG\n", x0, y0 );

    if  ( scaleX != 100 || scaleY != 100 )
	{
	sioOutPrintf( ps->psSos, "%f %f scale\n", scaleX/100.0, scaleY/100.0 );
	}

    if  ( (*playMetafile)( ps, sisMeta, psfl,
				    ps->psUsePostScriptFilters,
				    ps->psUsePostScriptIndexedImages,
				    pip->pipMapMode,
				    xWinExt, yWinExt, twipsWide, twipsHigh ) )
	{ LDEB(1);	}

    sioOutPrintf( ps->psSos, "grestore end %%}IMG\n" );

    sioInClose( sisMeta );
    sioInClose( sisMem );

    utilCleanPostScriptFaceList( &pstl );

    ps->psLastPageMarked= ps->psPagesPrinted;

    return 0;
    }

static int psPrintIncludeEpsObject(	PrintingState *		ps,
					InsertedObject *	io,
					int			x0,
					int			baseLine )
    {
    SimpleInputStream *		sisMem;
    SimpleInputStream *		sisHex;

    char			line[512+2];

    sisMem= sioInMemoryOpen( &(io->ioResultData) );
    if  ( ! sisMem )
	{ XDEB(sisMem); return -1;	}

    sisHex= sioInHexOpen( sisMem );
    if  ( ! sisHex )
	{ XDEB(sisHex); sioInClose( sisMem ); return -1;	}

    appPsBeginEpsObject( ps, x0, baseLine,
			    io->ioScaleXUsed/ 100.0, io->ioScaleYUsed/ 100.0,
			    0, 0, io->ioTwipsWide, io->ioTwipsHigh,
			    io->ioObjectData.mbBytes );

    while( sioInGetString( line, 512+1, sisHex ) )
	{
	int		emit= 1;
	const char *	s= line;

	while( isspace( *s ) )
	    { s++;	}

	if  ( ! *s || *s == '%' )
	    { emit= 0;	}

	if  ( emit )
	    { sioOutPutString( line, ps->psSos ); }

	while( ! strchr( line, '\n' ) )
	    {
	    if  ( ! sioInGetString( line, 512+1, sisHex ) )
		{ break;	}

	    if  ( emit )
		{ sioOutPutString( line, ps->psSos ); }
	    }
	}

    sioInClose( sisHex );
    sioInClose( sisMem );

    appPsEndEpsObject( ps );

    ps->psLastPageMarked= ps->psPagesPrinted;

    return 0;
    }

static void psPrintObjectBox(	DrawingContext *	dc,
				int			xShift,
				int			yShift,
				PrintingState *		ps,
				const InsertedObject *	io,
				const ParticuleData *	pd,
				int			baseLine )
    {
    int		high;

    int		x0= pd->pdX0+ xShift;
    int		y0= baseLine+ yShift;

    docDrawSetColorRgb( dc, (void *)ps, 0, 0, 0 );

    high= ( io->ioScaleYUsed* io->ioTwipsHigh )/ 100;
    sioOutPrintf( ps->psSos, "%d %d moveto ",
			    x0, y0- high );
    sioOutPrintf( ps->psSos, "%d %d lineto ",
			    x0+ pd->pdWidth, y0- high );
    sioOutPrintf( ps->psSos, "%d %d lineto ",
			    x0+ pd->pdWidth, y0 );
    sioOutPrintf( ps->psSos, "%d %d lineto ", x0, y0 );
    sioOutPrintf( ps->psSos, "closepath stroke\n" );

    ps->psLastPageMarked= ps->psPagesPrinted;
    return;
    }

/************************************************************************/
/*									*/
/*  Print a bitmap image included in the document.			*/
/*									*/
/************************************************************************/

static int docPsPrintBitmapObject( PrintingState *		ps,
				DrawingContext *		dc,
				int				x0,
				int				baseLine,
				const AppBitmapImage *		abi,
				const PictureProperties *	pip )
    {
    const BitmapDescription *	bd;

    double		scaleX= pip->pipScaleXUsed/ 100.0;
    double		scaleY= pip->pipScaleYUsed/ 100.0;

    int			imageWideTwips;
    int			imageHighTwips;

    bd= &(abi->abiBitmap);

    bmImageSizeTwips( &imageWideTwips, &imageHighTwips, bd );

    if  ( imageWideTwips > 20 )
	{ scaleX= ( scaleX* pip->pipTwipsWide )/ imageWideTwips;	}
    if  ( imageHighTwips > 20 )
	{ scaleY= ( scaleY* pip->pipTwipsHigh )/ imageHighTwips;	}

    if  ( bmPsPrintBitmap( ps->psSos, 1,
			    20.0* scaleX, -20.0* scaleY,
			    x0, baseLine, 0, 0,
			    bd->bdPixelsWide, bd->bdPixelsHigh,
			    ps->psUsePostScriptFilters,
			    ps->psUsePostScriptIndexedImages,
			    bd, abi->abiBuffer ) )
	{ LDEB(1); return -1; }

    ps->psLastPageMarked= ps->psPagesPrinted;
    return 0;
    }

static int docPsPrintShapeBitmap(	bmReadBitmap		readBitmap,
					PrintingState *		ps,
					DrawingShape *		ds )
    {
    const int		x0= 0;
    const int		y0= 0;

    if  ( ! ds->dsPrivate )
	{
	if  ( docGetBitmap( readBitmap,
			    &(ds->dsPrivate), &(ds->dsPictureData) ) )
	    { XDEB(ds->dsPrivate);	}
	}

    if  ( ds->dsPrivate )
	{
	const AppBitmapImage *		abi;
	const BitmapDescription *	bd;

	abi= (AppBitmapImage *)ds->dsPrivate;
	bd= &(abi->abiBitmap);

	sioOutPrintf( ps->psSos, "gsave -0.5 0.5 translate\n" );

	if  ( bmPsPrintBitmap( ps->psSos, 1,
				1.0/ bd->bdPixelsWide,
				-1.0/ bd->bdPixelsHigh,
				x0, y0, 0, 0,
				bd->bdPixelsWide, bd->bdPixelsHigh,
				ps->psUsePostScriptFilters,
				ps->psUsePostScriptIndexedImages,
				bd, abi->abiBuffer ) )
	    { LDEB(1); return -1; }

	sioOutPrintf( ps->psSos, "grestore\n" );
	}

    ps->psLastPageMarked= ps->psPagesPrinted;
    return 0;
    }

int docPsPrintShapeImage(	PrintingState *			ps,
				DrawingContext *		dc,
				DrawingShape *			ds,
				const PostScriptFontList *	psfl )
    {
    const PictureProperties *	pip= &(ds->dsPictureProperties);

    const int			x0= 0;
    const int			y0= 0;

    switch( pip->pipType )
	{
	case DOCokPICTWMETAFILE:
	case DOCokMACPICT:
	    if  ( docPsPrintMetafile( ps, pip, &(ds->dsPictureData), psfl,
								    x0, y0 ) )
		{ LDEB(1); break;	}

	    dc->dcCurrentTextAttributeSet= 0;
	    dc->dcCurrentColorSet= 0;
	    return 0;

	case DOCokPICTJPEGBLIP:
	    if  ( docPsPrintShapeBitmap( bmJpegReadJfif, ps, ds ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOCokPICTPNGBLIP:
	    if  ( docPsPrintShapeBitmap( bmPngReadPng, ps, ds ) )
		{ LDEB(1); return -1;	}
	    break;

	default:
	    LDEB(pip->pipType); return 0;
	}

    return 0;
    }

int docPsPrintObject(		PrintingState *			ps,
				DrawingContext *		dc,
				int				xShift,
				int				yShift,
				InsertedObject *		io,
				const PostScriptFontList *	psfl,
				ParticuleData *			pd,
				int				baseLine )
    {
    const PictureProperties *	pip= &(io->ioPictureProperties);

    switch( io->ioKind )
	{
	case DOCokPICTWMETAFILE:
	case DOCokMACPICT:
	    if  ( docPsPrintMetafile( ps, pip, &(io->ioObjectData), psfl,
				    pd->pdX0+ xShift, baseLine+ yShift ) )
		{ LDEB(1); break;	}

	    dc->dcCurrentTextAttributeSet= 0;
	    dc->dcCurrentColorSet= 0;
	    ps->psLinkParticulesDone++;
	    return 1;

	case DOCokPICTJPEGBLIP:
	case DOCokPICTPNGBLIP:

	    if  ( ! io->ioPrivate )
		{
		if  ( docGetBitmapForObject( io ) )
		    { XDEB(io->ioPrivate);	}
		}

	    if  ( io->ioPrivate )
		{
		if  ( docPsPrintBitmapObject( ps, dc,
					    pd->pdX0+ xShift, baseLine+ yShift,
					    (AppBitmapImage *)io->ioPrivate,
					    &(io->ioPictureProperties) ) )
		    { LDEB(1); return -1;	}

		ps->psLinkParticulesDone++;
		return 1;
		}
	    break;

	case DOCokOLEOBJECT:
	    if  ( io->ioResultKind == DOCokPICTWMETAFILE )
		{
		if  ( docPsPrintMetafile( ps, pip, &(io->ioResultData), psfl,
					pd->pdX0+ xShift, baseLine+ yShift ) )
		    { LDEB(1); break;	}

		dc->dcCurrentTextAttributeSet= 0;
		dc->dcCurrentColorSet= 0;
		ps->psLinkParticulesDone++;
		return 1;
		}
	    break;

	case DOCokEPS_FILE:
	    if  ( psPrintIncludeEpsObject( ps, io,
			    pd->pdX0+ xShift, baseLine+ yShift ) )
		{ LDEB(1); break;	}

	    dc->dcCurrentTextAttributeSet= 0;
	    dc->dcCurrentColorSet= 0;
	    ps->psLinkParticulesDone++;
	    return 1;

	case DOCokDRAWING_SHAPE:
	    /*  Done in a separate loop from generic drawing code */
	    return 1;

	default:
	    LDEB(io->ioKind); return 0;
	}

    psPrintObjectBox( dc, xShift, yShift, ps, io, pd, baseLine );
    ps->psLinkParticulesDone++;
    return 1;
    }


