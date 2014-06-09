/************************************************************************/
/*									*/
/*  Open/Close objects inserted in the document.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	<sioMemory.h>
#   include	<sioHex.h>
#   include	<sioPipe.h>
#   include	<appSystem.h>

#   include	<appDraw.h>
#   include	<appImage.h>
#   include	<appWinMeta.h>
#   include	<appMacPict.h>

#   include	"docLayout.h"
#   include	"tedApp.h"

#   include	<appDebugon.h>

int tedDrawObject(		const BufferItem *	bi,
				const TextParticule *	tp,
				int			baseline,
				int			ox,
				int			oy,
				AppDrawingData *	add )
    {
    InsertedObject *	io= bi->biParaObjects+ tp->tpObjectNumber;

    int			x0= tp->tpX0;
    int			y0= baseline- io->ioPixelsHigh;

    if  ( ! io->ioInline )
	{
	x0= X_PIXELS( add, io->ioX0Twips );
	y0= LP_YPIXELS( add, &(io->ioY0Position) );
	}

    if  ( io->ioKind == DOCokDRAWING_SHAPE )
	{ return 0;	}

    if  ( ( io->ioKind == DOCokPICTWMETAFILE	||
	    io->ioKind == DOCokMACPICT		||
	    io->ioKind == DOCokPICTPNGBLIP	||
	    io->ioKind == DOCokPICTJPEGBLIP	)	&&
	  io->ioPixmap					)
	{
	appDrawDrawPixmap( add, io->ioPixmap,
			0, 0, /* src: x,y */
			io->ioPixelsWide, io->ioPixelsHigh, /* wide, high */
			x0- ox, y0- oy ); /* dest: x,y */
		    
	return 0;
	}

    if  ( io->ioKind == DOCokOLEOBJECT			&&
	  io->ioResultKind == DOCokPICTWMETAFILE	&&
	  io->ioPixmap					)
	{
	appDrawDrawPixmap( add, io->ioPixmap,
			0, 0, /* src: x,y */
			io->ioPixelsWide, io->ioPixelsHigh, /* wide, high */
			x0- ox, y0- oy ); /* dest: x,y */
		    
	return 0;
	}

    if  ( io->ioKind == DOCokEPS_FILE			&&
	  io->ioPixmap					)
	{
	appDrawDrawPixmap( add, io->ioPixmap,
			0, 0, /* src: x,y */
			io->ioPixelsWide, io->ioPixelsHigh, /* wide, high */
			x0- ox, y0- oy ); /* dest: x,y */
		    
	return 0;
	}

    appDrawSetLineAttributes( add,
			    1, LINEstyleSOLID, LINEcapBUTT, LINEjoinMITER,
			    (const unsigned char *)0, 0 );

    appDrawDrawRectangle( add,
		tp->tpX0- ox, baseline- io->ioPixelsHigh- oy,
		io->ioPixelsWide, io->ioPixelsHigh );
    return 0;
    }

typedef int (*PLAY_META_X11)(	SimpleInputStream *	sis,
				void **			pPrivate,
				AppColors *		ac,
				AppDrawingData *	add,
				APP_BITMAP_IMAGE	pixmap,
				int			mapMode,
				int			xExt,
				int			yExt,
				int			pixelsWide,
				int			pixelsHigh,
				int			twipsWide,
				int			twipsHigh );

static int tedOpenMetafileObject(
				void **				pPrivate,
				APP_BITMAP_IMAGE *		pPixmap,
				const PictureProperties *	pip,
				int				pixelsWide,
				int				pixelsHigh,
				PLAY_META_X11			playMetaX11,
				AppColors *			ac,
				AppDrawingData *		add,
				const MemoryBuffer *		mb )
    {
    SimpleInputStream *	sisMem;
    SimpleInputStream *	sisMeta;

    void *		private= (void *)0;
    APP_BITMAP_IMAGE	pixmap= (APP_BITMAP_IMAGE)0;

    sisMem= sioInMemoryOpen( mb );
    if  ( ! sisMem )
	{ XDEB(sisMem); return -1;	}

    sisMeta= sioInHexOpen( sisMem );
    if  ( ! sisMeta )
	{ XDEB(sisMem); return -1;	}

    pixmap= appMakePixmap( add, pixelsWide, pixelsHigh );
    if  ( ! pixmap )
	{ XDEB(pixmap); return -1;	}

    if  ( (*playMetaX11)( sisMeta, &private, ac, add,
				    pixmap, pip->pipMapMode,
				    pip->pip_xWinExt, pip->pip_yWinExt,
				    pixelsWide, pixelsHigh,
				    pip->pipTwipsWide, pip->pipTwipsHigh ) )
	{ LDEB(1);	}

    sioInClose( sisMeta );
    sioInClose( sisMem );

    *pPrivate= private;
    *pPixmap= pixmap;
    return 0;
    }

static int tedOpenBitmapObject(	void **				pPrivate,
				APP_BITMAP_IMAGE *		pPixmap,
				int				pixelsWide,
				int				pixelsHigh,
				bmReadBitmap			readBitmap,
				AppColors *			ac,
				AppDrawingData *		add,
				const MemoryBuffer *		mb )
    {
    AppBitmapImage *	abi= (AppBitmapImage *)malloc( sizeof(AppBitmapImage) );

    SimpleInputStream *	sisMem;
    SimpleInputStream *	sisBitmap;

    APP_BITMAP_IMAGE	pixmap;

    if  ( ! abi )
	{ XDEB(abi); return -1;	}
    appInitBitmapImage( abi );

    sisMem= sioInMemoryOpen( mb );
    if  ( ! sisMem )
	{ XDEB(sisMem); return -1;	}

    sisBitmap= sioInHexOpen( sisMem );
    if  ( ! sisBitmap )
	{ XDEB(sisMem); return -1;	}

    if  ( (*readBitmap)( &abi->abiBitmap, &abi->abiBuffer, sisBitmap ) )
	{ LDEB(1); return -1;	}

    sioInClose( sisBitmap );
    sioInClose( sisMem );

    if  ( appImgMakePixmap( add, &pixmap, pixelsWide, pixelsHigh, ac, abi ) )
	{
	LDEB(1);
	appCleanBitmapImage( abi ); free( abi );
	return -1;
	}

    *pPrivate= (void *)abi;
    *pPixmap= pixmap;

    return 0;
    }

static int tedOpenEpsfObject(		InsertedObject *	io,
					AppColors *		ac,
					AppDrawingData *	add,
					const MemoryBuffer *	mb )
    {
#   if 0
    SimpleInputStream *		sisMem;
    SimpleInputStream *		sisEps;
    SimpleOutputStream *	sosGs;

    char			command[1000];

    static Atom			XA_GHOSTVIEW= None;

    if  ( XA_GHOSTVIEW == None )
	{
	XA_GHOSTVIEW= XInternAtom( add->addDisplay, "GHOSTVIEW", False );
	if  ( XA_GHOSTVIEW == None )
	    { XDEB(XA_GHOSTVIEW); return -1;	}
	}

    io->ioPixmap= appMakePixmap( add, io->ioPixelsWide, io->ioPixelsHigh );
    if  ( ! io->ioPixmap )
	{ XDEB(io->ioPixmap); return -1;	}

    sprintf( command, "0 0 %d %d %d %d %f %f",
				/* llx	*/ 0,
				/* lly	*/ 0,
				/* urx	*/ io->ioTwipsWide/ 20,
				/* ury	*/ io->ioTwipsHigh/ 20,
				/* xdpi	*/ 72.0,
				/* ydpi	*/ 72.0 );

    XChangeProperty( add->addDisplay, add->addDrawable,
			XA_GHOSTVIEW, XA_STRING, 8,
			PropModeReplace,
			(unsigned char *)command,
			sizeof(command) );

    XSync( add->addDisplay, False );

    sprintf( command,
	    "GHOSTVIEW='%ld %ld' gs -dQUIET -sDEVICE=x11 -dNOPAUSE -c quit -",
	    add->addDrawable, io->ioPixmap );
    sosGs= sioOutPipeOpen( command );

    sisMem= sioInMemoryOpen( mb );
    if  ( ! sisMem )
	{ XDEB(sisMem); return -1;	}

    sisEps= sioInHexOpen( sisMem );
    if  ( ! sisEps )
	{ XDEB(sisMem); return -1;	}

    for (;;)
	{
	int		got;

	got= sioInReadBytes( sisEps, command, sizeof( command ) );
	if  ( got < 1 )
	    { break;	}
	if  ( sioOutWriteBytes( sosGs, command, got ) != got )
	    { LDEB(got); break;	}
	}

    sioInClose( sisEps );
    sioInClose( sisMem );

    sioOutPrintf( sosGs, " showpage\n" );
    sioOutClose( sosGs );

#   endif
    return 0;
    }

static int tedOpenShapePixmaps(	DrawingShape *			ds,
				const DocumentRectangle *	twipsRect,
				AppColors *			ac,
				AppDrawingData *		add )
    {
    int				rval= 0;
    const PictureProperties *	pip= &(ds->dsPictureProperties);
    int				pixelsWide;
    int				pixelsHigh;

    pixelsWide= X_PIXELS( add, twipsRect->drX1 )-
		X_PIXELS( add, twipsRect->drX0 )+ 1;
    pixelsHigh= Y_PIXELS( add, twipsRect->drY1 )-
		Y_PIXELS( add, twipsRect->drY0 )+ 1;

    if  ( ds->dsChildCount > 0 )
	{
	int			child;
	double			xfac;
	double			yfac;

	DocumentRectangle	drHere= *twipsRect;

	if  ( DSflipHORIZONTAL( ds ) )
	    { int sw= drHere.drX0; drHere.drX0= drHere.drX1; drHere.drX1= sw; }

	if  ( DSflipVERTICAL( ds ) )
	    { int sw= drHere.drY0; drHere.drY0= drHere.drY1; drHere.drY1= sw; }

	xfac= ( 1.0* ( drHere.drX1- drHere.drX0 ) )/
			    ( ds->dsGroupRect.drX1- ds->dsGroupRect.drX0 );
	yfac= ( 1.0* ( drHere.drY1- drHere.drY0 ) )/
			    ( ds->dsGroupRect.drY1- ds->dsGroupRect.drY0 );

	for ( child= 0; child < ds->dsChildCount; child++ )
	    {
	    DrawingShape *	dsChild= ds->dsChildren[child];
	    DocumentRectangle	drChild;

	    drChild.drX0= drHere.drX0+ xfac* 
			( dsChild->dsRelRect.drX0- ds->dsGroupRect.drX0 );
	    drChild.drY0= drHere.drY0+ yfac*
			( dsChild->dsRelRect.drY0- ds->dsGroupRect.drY0 );
	    drChild.drX1= drHere.drX0+ xfac* 
			( dsChild->dsRelRect.drX1- ds->dsGroupRect.drX0 );
	    drChild.drY1= drHere.drY0+ yfac*
			( dsChild->dsRelRect.drY1- ds->dsGroupRect.drY0 );

	    if  ( tedOpenShapePixmaps( dsChild, &drChild, ac, add ) )
		{ LDEB(child); rval= -1;	}
	    }
	}

    switch( pip->pipType )
	{
	case DOCokUNKNOWN:
	    break;

	case DOCokPICTWMETAFILE:
	    if  ( ! ds->dsPrivate					&&
		  tedOpenMetafileObject( &(ds->dsPrivate), &(ds->dsPixmap), pip,
					pixelsWide, pixelsHigh,
					appMetaPlayWmfX11,
					ac, add, &(ds->dsPictureData) )	)
		{ LDEB(1); rval= -1; break;	}
	    break;

	case DOCokPICTEMFBLIP:
	    if  ( ! ds->dsPrivate					&&
		  tedOpenMetafileObject( &(ds->dsPrivate), &(ds->dsPixmap), pip,
					pixelsWide, pixelsHigh,
					appMetaPlayEmfX11,
					ac, add, &(ds->dsPictureData) )	)
		{ LDEB(1); rval= -1; break;	}
	    break;

	case DOCokMACPICT:
	    if  ( ! ds->dsPrivate					&&
		  tedOpenMetafileObject( &(ds->dsPrivate), &(ds->dsPixmap), pip,
					pixelsWide, pixelsHigh,
					appMacPictPlayFileX11,
					ac, add, &(ds->dsPictureData) )	)
		{ LDEB(1); rval= -1; break;	}
	    break;

	case DOCokPICTPNGBLIP:
	    if  ( ! ds->dsPrivate					&&
		  tedOpenBitmapObject( &(ds->dsPrivate), &(ds->dsPixmap),
				    pixelsWide, pixelsHigh,
				    bmPngReadPng,
				    ac, add, &(ds->dsPictureData) )	)
		{ LDEB(1); rval= -1; break;	}
	    break;

	case DOCokPICTJPEGBLIP:
	    if  ( ! ds->dsPrivate					&&
		  tedOpenBitmapObject( &(ds->dsPrivate), &(ds->dsPixmap),
					pixelsWide, pixelsHigh,
					bmJpegReadJfif,
					ac, add, &(ds->dsPictureData) )	)
		{ LDEB(1); rval= -1; break;	}
	    break;

	default:
	    LDEB(pip->pipType); rval= -1; break;
	}

    return rval;
    }

static int tedOpenObject(	InsertedObject *	io,
				BufferDocument *	bd,
				AppColors *		ac,
				AppDrawingData *	add )
    {
    PictureProperties *	pip= &(io->ioPictureProperties);

    if  ( io->ioKind != DOCokDRAWING_SHAPE )
	{
	if  ( io->ioTwipsWide < 1 || io->ioTwipsHigh < 1 )
	    { LLLDEB(io->ioKind,io->ioTwipsWide,io->ioTwipsHigh); return -1; }
	}

    switch( io->ioKind )
	{
	case DOCokPICTWMETAFILE:
	    if  ( ! io->ioPrivate					&&
		  tedOpenMetafileObject( &(io->ioPrivate), &(io->ioPixmap), pip,
					io->ioPixelsWide, io->ioPixelsHigh,
					appMetaPlayWmfX11,
					ac, add, &(io->ioObjectData) )	)
		{ LDEB(1); return 0;	}

	    if  ( io->ioPrivate )
		{ io->ioPictureProperties.pipMetafileIsBitmap= 1;	}

	    return 0;

	case DOCokPICTEMFBLIP:
	    if  ( ! io->ioPrivate					&&
		  tedOpenMetafileObject( &(io->ioPrivate), &(io->ioPixmap), pip,
					io->ioPixelsWide, io->ioPixelsHigh,
					appMetaPlayEmfX11,
					ac, add, &(io->ioObjectData) )	)
		{ LDEB(1); return 0;	}
	    return 0;

	case DOCokMACPICT:
	    if  ( ! io->ioPrivate					&&
		  tedOpenMetafileObject( &(io->ioPrivate), &(io->ioPixmap), pip,
					io->ioPixelsWide, io->ioPixelsHigh,
					appMacPictPlayFileX11,
					ac, add, &(io->ioObjectData) )	)
		{ LDEB(1); return 0;	}
	    return 0;

	case DOCokPICTPNGBLIP:
	    if  ( ! io->ioPrivate					&&
		  tedOpenBitmapObject( &(io->ioPrivate), &(io->ioPixmap),
				    io->ioPixelsWide, io->ioPixelsHigh,
				    bmPngReadPng,
				    ac, add, &(io->ioObjectData) )	)
		{ LDEB(1); return 0;	}
	    return 0;

	case DOCokPICTJPEGBLIP:
	    if  ( ! io->ioPrivate					&&
		  tedOpenBitmapObject( &(io->ioPrivate), &(io->ioPixmap),
					io->ioPixelsWide, io->ioPixelsHigh,
					bmJpegReadJfif,
					ac, add, &(io->ioObjectData) )	)
		{ LDEB(1); return 0;	}
	    return 0;

	case DOCokOLEOBJECT:
	    if  ( io->ioResultKind == DOCokPICTWMETAFILE )
		{
		if  ( ! io->ioPrivate					&&
		      tedOpenMetafileObject(
					&(io->ioPrivate), &(io->ioPixmap), pip,
					io->ioPixelsWide, io->ioPixelsHigh,
					appMetaPlayWmfX11,
					ac, add, &(io->ioResultData) )	)
		    { LDEB(1); return 0;	}

		if  ( io->ioPrivate )
		    { io->ioPictureProperties.pipMetafileIsBitmap= 1;	}

		return 0;
		}
	    return 0;

	case DOCokEPS_FILE:
	    if  ( ! io->ioPixmap					&&
		  tedOpenEpsfObject( io,
				    ac, add, &io->ioResultData )	)
		{ LDEB(1); return 0;	}

	    return 0;

	case DOCokDRAWING_SHAPE:
	    {
	    DrawingShape *		ds= io->ioDrawingShape;
	    const ShapeProperties *	sp= &(ds->dsShapeProperties);

	    if  ( ! ds )
		{ XDEB(ds); return 0;	}

	    if  ( tedOpenShapePixmaps( ds, &(sp->spRect), ac, add ) )
		{ LDEB(1); return 0;	}
	    }
	    return 0;

	default:
	    LDEB(io->ioKind); return 0;
	}

    return 0;
    }

static int tedOpenParaObjects(	BufferItem *		bi,
				BufferDocument *	bd,
				AppColors *		ac,
				AppDrawingData *	add )
    {
    int			part;
    TextParticule *	tp;

    tp= bi->biParaParticules;
    for ( part= 0; part < bi->biParaParticuleCount; tp++, part++ )
	{
	InsertedObject *	io;

	if  ( tp->tpKind != DOCkindOBJECT )
	    { continue;	}

	io= bi->biParaObjects+ tp->tpObjectNumber;

	if  ( tedOpenObject( io, bd, ac, add )	)
	    { LDEB(part); continue;	}
	}

    return 0;
    }

int tedOpenItemObjects(	BufferItem *		bi,
			BufferDocument *	bd,
			AppColors *		ac,
			AppDrawingData *	add )
    {
    int		i;

    switch( bi->biLevel )
	{
	case DOClevSECT:
	    for ( i= 0; i < bi->biChildCount; i++ )
		{
		if  ( tedOpenItemObjects( bi->biChildren[i], bd, ac, add ) )
		    { LDEB(i); return -1;	}
		}

	    if  ( bi->biSectHeader.eiItem				&&
		  tedOpenItemObjects( bi->biSectHeader.eiItem,
							bd, ac, add )	)
		{ LDEB(1); return -1;	}
	    if  ( bi->biSectFirstPageHeader.eiItem			&&
		  tedOpenItemObjects( bi->biSectFirstPageHeader.eiItem,
							bd, ac, add )	)
		{ LDEB(1); return -1;	}
	    if  ( bi->biSectLeftPageHeader.eiItem			&&
		  tedOpenItemObjects( bi->biSectLeftPageHeader.eiItem,
							bd, ac, add )	)
		{ LDEB(1); return -1;	}
	    if  ( bi->biSectRightPageHeader.eiItem			&&
		  tedOpenItemObjects( bi->biSectRightPageHeader.eiItem,
							bd, ac, add )	)
		{ LDEB(1); return -1;	}

	    if  ( bi->biSectFooter.eiItem				&&
		  tedOpenItemObjects( bi->biSectFooter.eiItem,
							bd, ac, add )	)
		{ LDEB(1); return -1;	}
	    if  ( bi->biSectFirstPageFooter.eiItem			&&
		  tedOpenItemObjects( bi->biSectFirstPageFooter.eiItem,
							bd, ac, add )	)
		{ LDEB(1); return -1;	}
	    if  ( bi->biSectLeftPageFooter.eiItem			&&
		  tedOpenItemObjects( bi->biSectLeftPageFooter.eiItem,
							bd, ac, add )	)
		{ LDEB(1); return -1;	}
	    if  ( bi->biSectRightPageFooter.eiItem			&&
		  tedOpenItemObjects( bi->biSectRightPageFooter.eiItem,
							bd, ac, add )	)
		{ LDEB(1); return -1;	}

	    break;

	case DOClevDOC:
	case DOClevROW:
	case DOClevCELL:
	    for ( i= 0; i < bi->biChildCount; i++ )
		{
		if  ( tedOpenItemObjects( bi->biChildren[i], bd, ac, add ) )
		    { LDEB(i); return -1;	}
		}
	    break;

	case DOClevPARA:
	    if  ( tedOpenParaObjects( bi, bd, ac, add ) )
		{ LDEB(0); return -1;	}
	    break;

	default:
	    LDEB(bi->biLevel); return -1;
	}


    return 0;
    }

static void tedCloseDrawingShape(	DrawingShape *		ds,
					AppDrawingData *	add )
    {
    int		child;

    for ( child= 0; child < ds->dsChildCount; child++ )
	{
	DrawingShape *	dsChild= ds->dsChildren[child];

	tedCloseDrawingShape( dsChild, add );
	}

    if  ( ds->dsPrivate )
	{
	AppBitmapImage *	abi= (AppBitmapImage *)ds->dsPrivate;

	if  ( ds->dsPixmap )
	    {
	    appDrawFreePixmap( add, ds->dsPixmap );
	    ds->dsPixmap= (APP_BITMAP_IMAGE)0;
	    }

	if  ( abi )
	    { appCleanBitmapImage( abi ); free( abi );	}

	ds->dsPrivate= (void *)0;
	}

    return;
    }

void tedCloseObject(		BufferDocument *	bd,
				BufferItem *		bi,
				TextParticule *		tp,
				void *			voidadd )
    {
    InsertedObject *	io;
    AppDrawingData *	add= (AppDrawingData *)voidadd;

    if  ( tp->tpKind != DOCkindOBJECT )
	{ return;	}

    io= bi->biParaObjects+ tp->tpObjectNumber;

    switch( io->ioKind )
	{
	case DOCokPICTWMETAFILE:
	case DOCokPICTPNGBLIP:
	case DOCokPICTJPEGBLIP:
	case DOCokOLEOBJECT:
	case DOCokMACPICT:
	    if  ( io->ioPrivate )
		{
		AppBitmapImage *	abi= (AppBitmapImage *)io->ioPrivate;

		if  ( io->ioPixmap )
		    {
		    appDrawFreePixmap( add, io->ioPixmap );
		    io->ioPixmap= (APP_BITMAP_IMAGE)0;
		    }

		if  ( abi )
		    { appCleanBitmapImage( abi ); free( abi );	}

		io->ioPrivate= (void *)0;
		}
	    break;

	case DOCokEPS_FILE:
	    if  ( io->ioPixmap )
		{
		appDrawFreePixmap( add, io->ioPixmap );
		io->ioPixmap= (APP_BITMAP_IMAGE)0;
		}

	    break;

	case DOCokDRAWING_SHAPE:
	    tedCloseDrawingShape( io->ioDrawingShape, add );
	    break;

	default:
	    LDEB(io->ioKind); break;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Called to resize an inserted object.				*/
/*									*/
/************************************************************************/

int tedReopenObject(	BufferDocument *	bd,
			BufferItem *		bi,
			TextParticule *		tp,
			AppColors *		ac,
			AppDrawingData *	add )
    {
    InsertedObject *		io= bi->biParaObjects+ tp->tpObjectNumber;

    tedCloseObject( bd, bi, tp, (void *)add );

    if  ( tedOpenObject( io, bd, ac, add ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static void tedScaleObjectToParagraph(	const BufferDocument *	bd,
					BufferItem *		paraBi,
					double			xfac,
					InsertedObject *	io )
    {
    ParagraphFrame		pf;

    const int			bottom= -1;
    const int			stripHigh= -1;

    PictureProperties *		pip= &(io->ioPictureProperties);

    BlockFrame			bf;

    docBlockFrameTwips( &bf, paraBi, bd,
				    paraBi->biTopPosition.lpPage,
				    paraBi->biTopPosition.lpColumn );

    docParagraphFrameTwips( &pf, &bf, bottom, stripHigh, paraBi );

    docLayoutScaleObjectToFitParagraphFrame( io, &pf );

    pip->pipScaleXUsed= io->ioScaleXUsed;
    pip->pipScaleYUsed= io->ioScaleYUsed;

    io->ioPixelsWide= TWIPStoPIXELS( xfac,
				( io->ioScaleXUsed* pip->pipTwipsWide )/ 100 );
    io->ioPixelsHigh= TWIPStoPIXELS( xfac,
				( io->ioScaleYUsed* pip->pipTwipsHigh )/ 100 );
    if  ( io->ioPixelsWide < 1 )
	{ io->ioPixelsWide=  1;	}
    if  ( io->ioPixelsHigh < 1 )
	{ io->ioPixelsHigh=  1;	}

    pip->pip_xWinExt= (int) ( ( 100000.0* pip->pipTwipsWide )/
						    ( 20* POINTS_PER_M ) );
    pip->pip_yWinExt= (int) ( ( 100000.0* pip->pipTwipsHigh )/
						    ( 20* POINTS_PER_M ) );

    return;
    }

static void tedScaleObjectToSelectedParagraph(
					EditDocument *		ed,
					InsertedObject *	io )
    {
    AppDrawingData *		add= &(ed->edDrawingData);
    double			xfac= add->addMagnifiedPixelsPerTwip;

    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return;	}

    tedScaleObjectToParagraph( bd, ds.dsBegin.dpBi, xfac, io );

    return;
    }

/************************************************************************/
/*									*/
/*  Save the picture of an object for Copy/Paste.			*/
/*									*/
/************************************************************************/

int tedSaveObjectPicture(	AppBitmapImage *	abiTo,
				InsertedObject *	io )
    {
    AppBitmapImage *	abiFrom= (AppBitmapImage *)0;
    unsigned char *	buffer;

    if  ( ( io->ioKind == DOCokPICTWMETAFILE	||
	    io->ioKind == DOCokMACPICT		||
	    io->ioKind == DOCokPICTPNGBLIP	||
	    io->ioKind == DOCokPICTJPEGBLIP	)	&&
	  io->ioPrivate					)
	{ abiFrom= (AppBitmapImage *)io->ioPrivate; }

    if  ( io->ioKind == DOCokOLEOBJECT			&&
	  io->ioResultKind == DOCokPICTWMETAFILE	&&
	  io->ioPrivate					)
	{ abiFrom= (AppBitmapImage *)io->ioPrivate; }

    if  ( ! abiFrom )
	{ LXXDEB(io->ioKind,io->ioPrivate,abiFrom); return -1;	}

    buffer= (unsigned char *)malloc( abiFrom->abiBitmap.bdBufferLength );
    if  ( ! buffer )
	{ LXDEB(abiFrom->abiBitmap.bdBufferLength,buffer); return -1; }

    if  ( bmCopyDescription( &(abiTo->abiBitmap), &(abiFrom->abiBitmap) ) )
	{ LDEB(1); free( buffer ); return -1;	}

    memcpy( buffer, abiFrom->abiBuffer, abiFrom->abiBitmap.bdBufferLength );
    abiTo->abiBuffer= buffer;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Used from the 'Paste' for images.					*/
/*									*/
/*  9)  Not needed as the document is marked as changed anyway.		*/
/*									*/
/************************************************************************/

InsertedObject * tedObjectMakeBitmapObject(
					EditDocument *		ed,
					BufferItem *		bi,
					AppBitmapImage *	abi )
    {
    AppDrawingData *		add= &(ed->edDrawingData);

    APP_BITMAP_IMAGE		pixmap;

    InsertedObject *		io;
    PictureProperties *		pip;

    MemoryBuffer		mb;
    SimpleOutputStream *	sosMem;
    SimpleOutputStream *	sosHex;

    int				objectNumber;
    int				includedAsBitmap= 0;

    const char *		typeId="";

    io= docClaimObject( &objectNumber, bi );
    if  ( ! io )
	{ XDEB(io); return (InsertedObject *)0;	}
    pip= &(io->ioPictureProperties);

    utilInitMemoryBuffer( &mb );

    sosMem= sioOutMemoryOpen( &mb );
    if  ( ! sosMem )
	{ XDEB(sosMem); return (InsertedObject *)0;	}

    sosHex= sioOutHexOpen( sosMem );
    if  ( ! sosHex )
	{ XDEB(sosHex); return (InsertedObject *)0;	}

    if  ( abi->abiFormat >= 0 )
	{ typeId= bmFileFormats[abi->abiFormat].bffFileType->bftTypeId; }

    if  ( abi->abiFormat >= 0			&&
	  ! includedAsBitmap			&&
	  ! strcmp( typeId, "pngFile" )		)
	{
	if  ( bmPngWritePng( &(abi->abiBitmap), abi->abiBuffer, sosHex ) )
	    { LDEB(1); return (InsertedObject *)0;	}

	io->ioKind= DOCokPICTPNGBLIP;
	pip->pipType= DOCokPICTPNGBLIP;
	includedAsBitmap= 1;
	}

    if  ( abi->abiFormat >= 0				&&
	  ! includedAsBitmap				&&
	  ( ! strcmp( typeId, "jpgFile" )	||
	    ! strcmp( typeId, "jpegFile" )	)	)
	{
	if  ( bmJpegWriteJfif( &abi->abiBitmap, abi->abiBuffer, sosHex ) )
	    { LDEB(1); return (InsertedObject *)0;	}

	io->ioKind= DOCokPICTJPEGBLIP;
	pip->pipType= DOCokPICTJPEGBLIP;
	includedAsBitmap= 1;
	}

    if  ( ! includedAsBitmap )
	{
	if  ( bmWmfWriteWmf( &(abi->abiBitmap), abi->abiBuffer, sosHex ) )
	    { LDEB(1); return (InsertedObject *)0;	}

	pip->pipMetafileIsBitmap= 1;
	pip->pipMetafileBitmapBpp= abi->abiBitmap.bdBitsPerPixel;

	io->ioKind= DOCokPICTWMETAFILE;
	pip->pipType= DOCokPICTWMETAFILE;
	pip->pipMapMode= 8;
	}

    sioOutClose( sosHex );
    sioOutClose( sosMem );

    io->ioObjectData= mb;

    bmImageSizeTwips( &(pip->pipTwipsWide), &(pip->pipTwipsHigh),
							&(abi->abiBitmap) );
    io->ioTwipsWide= pip->pipTwipsWide;
    io->ioTwipsHigh= pip->pipTwipsHigh;

    io->ioPrivate= abi;

    tedScaleObjectToSelectedParagraph( ed, io );

    if  ( appImgMakePixmap( add, &pixmap,
				    io->ioPixelsWide, io->ioPixelsHigh,
				    &(ed->edColors), abi ) )
	{ LDEB(1); return (InsertedObject *)0; }
    io->ioPixmap= pixmap;

    /*  9
    io->ioBliptag= appGetTimestamp();
    */

    return io;
    }

/************************************************************************/
/*									*/
/*  Callback of the 'Insert Picture' menu option.			*/
/*									*/
/************************************************************************/

static int tedObjectOpenPicture(	void *		voided,
					APP_WIDGET	relative,
					APP_WIDGET	option,
					const char *	filename )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    const char *		ext;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    int				objectNumber;
    InsertedObject *		io;

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return -1;	}

    ext= appFileExtensionOfName( filename );

    io= docClaimObject( &objectNumber, ds.dsBegin.dpBi );
    if  ( ! io )
	{ XDEB(io); return -1;	}

    if  ( ! ext || strcmp( ext, "wmf" ) )
	{
	if  ( docReadBitmapObject( io, filename ) )
	    { SDEB(filename); return -1;;	}
	}
    else{
	if  ( docReadMetafileObject( io, filename ) )
	    { SDEB(filename); return -1;;	}
	}

    tedScaleObjectToSelectedParagraph( ed, io );

    /*  9
    io->ioBliptag= appGetTimestamp();
    */

    if  ( tedOpenObject( io, bd, &(ed->edColors), &(ed->edDrawingData) ) )
	{ LDEB(1);	}

    if  ( tedReplaceSelectionWithObject( ed, io ) )
	{ SDEB(filename); return -1;	}

    return 0;
    }

void tedDocInsertPicture(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    static AppFileExtension *	openExtensions;
    static int			openExtensionCount;

    if  ( ! td->tdCanReplaceSelection )
	{ LDEB(td->tdCanReplaceSelection); return;	}

    if  ( openExtensionCount == 0 )
	{
	AppFileExtension *	afe;

	if  ( appImgMakeFileExtensions( &openExtensions,
					&openExtensionCount ) )
	    { LDEB(1); return;	}

	afe= (AppFileExtension *) realloc( openExtensions,
			(openExtensionCount+ 1)* sizeof(AppFileExtension) );
	if  ( ! afe )
	    { LXDEB(openExtensionCount,afe); return; }
	openExtensions= afe;

	afe += openExtensionCount;

	afe->afeId= "wmfFile";
	afe->afeFilter= "*.wmf";
	afe->afeDescription= "Windows Metafile ( *.wmf )";
	afe->afeExtension= "wmf";
	afe->afeUseFlags= APPFILE_CAN_OPEN;

	openExtensionCount++;
	}

    appRunOpenChooser( option, ed->edToplevel.atTopWidget,
			openExtensionCount, openExtensions, (char *)0,
			voided, tedObjectOpenPicture, ea );

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt (scale) the current image.					*/
/*									*/
/************************************************************************/

void tedObjectSetImageProperties(
				PropertyMask *			pipDoneMask,
				EditDocument *			ed,
				InsertedObject *		io,
				const DocumentPosition *	dpObject,
				int				partObject,
				const PositionGeometry *	pgObject,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pipFrom )
    {
    AppDrawingData *		add= &(ed->edDrawingData);
    double			xfac= add->addMagnifiedPixelsPerTwip;

    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    PictureProperties *		pipTo= &(io->ioPictureProperties);

    if  ( docUpdPictureProperties( pipDoneMask, pipTo, pipSetMask, pipFrom ) )
	{ LDEB(1); return;	}

    if  ( utilPropMaskIsEmpty( pipDoneMask ) )
	{ return;	}

    if  ( PROPmaskISSET( pipDoneMask, PIPpropPICSCALE_X ) )
	{
	io->ioScaleXSet= io->ioScaleXUsed=
			    pipTo->pipScaleXUsed= pipTo->pipScaleXSet;
	}
    if  ( PROPmaskISSET( pipDoneMask, PIPpropPICSCALE_Y ) )
	{
	io->ioScaleYSet= io->ioScaleYUsed=
			    pipTo->pipScaleYUsed= pipTo->pipScaleYSet;
	}

    docObjectAdaptToPictureGeometry( io, pipTo );

    tedScaleObjectToParagraph( bd, dpObject->dpBi, xfac, io );

    if  ( tedEditReopenObject( ed, partObject, dpObject, pgObject ) )
	{ LDEB(1);	}

    return;
    }

static void tedDocSetImageProperties(
				EditDocument *			ed,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pipFrom )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    InsertedObject *		io;
    DocumentPosition		dpObject;
    int				part;

    PropertyMask		pipDoneMask;

    PROPmaskCLEAR( &pipDoneMask );

    if  ( tedGetObjectSelection( td, &part, &dpObject, &io ) )
	{ LDEB(1); return;	}

    tedObjectSetImageProperties( &pipDoneMask, ed, io, &dpObject, part,
		    &(td->tdSelectionGeometry.sgBegin), pipSetMask, pipFrom );

    if  ( utilPropMaskIsEmpty( &pipDoneMask ) )
	{ return;	}

    tedAdaptToolsToSelection( ed );

    appDocumentChanged( ed, 1 );

    return;
    }

void tedAppSetImageProperties(	EditApplication *		ea,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pip )
    {
    EditDocument *	ed= ea->eaCurrentDocument;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    tedDocSetImageProperties( ed, pipSetMask, pip );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the eight blocks around a selected object.			*/
/*									*/
/************************************************************************/

void tedGetObjectRectangle(	DocumentRectangle *		drObject,
				APP_POINT *			xp,
				const InsertedObject *		io,
				const PositionGeometry *	pg,
				const TedDocument *		td )
    {
    drObject->drX0= pg->pgXPixels;
    drObject->drX1= drObject->drX0+ io->ioPixelsWide- 1;
    drObject->drY1= pg->pgBaselinePixels;
    drObject->drY0= drObject->drY1- io->ioPixelsHigh+ 1;

    switch( td->tdObjectResizeCorner )
	{
	case -1:
	    break;

	case RESIZE_BOTTOM_LEFT:
	case RESIZE_MIDDLE_LEFT:
	case RESIZE_TOP_LEFT:
	    drObject->drX0 += td->tdObjectCornerMovedX;
	    break;

	case RESIZE_BOTTOM_RIGHT:
	case RESIZE_MIDDLE_RIGHT:
	case RESIZE_TOP_RIGHT:
	    drObject->drX1 += td->tdObjectCornerMovedX;
	    break;

	case RESIZE_BOTTOM_MIDDLE:
	case RESIZE_TOP_MIDDLE:
	    break;

	default:
	    LDEB(td->tdObjectResizeCorner);
	    break;
	}

    if  ( drObject->drX1 <= drObject->drX0 )
	{ drObject->drX1= drObject->drX0+ 1;	}

    switch( td->tdObjectResizeCorner )
	{
	case -1:
	    break;

	case RESIZE_TOP_LEFT:
	case RESIZE_TOP_MIDDLE:
	case RESIZE_TOP_RIGHT:
	    drObject->drY0 += td->tdObjectCornerMovedY;
	    break;

	case RESIZE_BOTTOM_LEFT:
	case RESIZE_BOTTOM_MIDDLE:
	case RESIZE_BOTTOM_RIGHT:
	    drObject->drY1 += td->tdObjectCornerMovedY;
	    break;

	case RESIZE_MIDDLE_LEFT:
	case RESIZE_MIDDLE_RIGHT:
	    break;

	default:
	    LDEB(td->tdObjectResizeCorner);
	    break;
	}

    if  ( drObject->drY1 <= drObject->drY0 )
	{ drObject->drY1= drObject->drY0+ 1;	}

    if  ( xp )
	{
	int		xl, xm, xr;
	int		yb, ym, yt;

	xl= drObject->drX0;
	xm= ( drObject->drX0+ drObject->drX1 )/ 2- RESIZE_BLOCK/2;
	xr= drObject->drX1- RESIZE_BLOCK;

	yt= drObject->drY0;
	ym= ( drObject->drY0+ drObject->drY1 )/ 2- RESIZE_BLOCK/2;
	yb= drObject->drY1- RESIZE_BLOCK;

	xp[RESIZE_BOTTOM_LEFT].x= xl;
	xp[RESIZE_BOTTOM_LEFT].y= yb;

	xp[RESIZE_BOTTOM_MIDDLE].x= xm;
	xp[RESIZE_BOTTOM_MIDDLE].y= yb;

	xp[RESIZE_BOTTOM_RIGHT].x= xr;
	xp[RESIZE_BOTTOM_RIGHT].y= yb;

	xp[RESIZE_MIDDLE_LEFT].x= xl;
	xp[RESIZE_MIDDLE_LEFT].y= ym;

	xp[RESIZE_MIDDLE_RIGHT].x= xr;
	xp[RESIZE_MIDDLE_RIGHT].y= ym;

	xp[RESIZE_TOP_LEFT].x= xl;
	xp[RESIZE_TOP_LEFT].y= yt;

	xp[RESIZE_TOP_MIDDLE].x= xm;
	xp[RESIZE_TOP_MIDDLE].y= yt;

	xp[RESIZE_TOP_RIGHT].x= xr;
	xp[RESIZE_TOP_RIGHT].y= yt;
	}

    return;
    }

