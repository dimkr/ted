#   ifndef	APP_IMAGE_H
#   define	APP_IMAGE_H

#   include	<bitmap.h>

#   include	<appGuiBase.h>
#   include	<appDraw.h>

/************************************************************************/
/*									*/
/*  Used to store and manage a bitmap image.				*/
/*									*/
/************************************************************************/

typedef struct AppBitmapImage
    {
    BitmapDescription	abiBitmap;
    unsigned char *	abiBuffer;
    int			abiFormat;
    double		abiFactor;
    } AppBitmapImage;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appImgMakeImage(	AppDrawingData *	add,
				APP_IMAGE **		pPimage,
				int			toWide,
				int			toHigh,
				AppColors *		ac,
				const AppBitmapImage *	abi );

extern int appImgMakePixmap(	AppDrawingData *		add,
				APP_WINDOW *			pPixmap,
				int				toWide,
				int				toHigh,
				AppColors *			ac,
				const AppBitmapImage *		abi );

extern void appCleanBitmapImage(	AppBitmapImage *	abi );

extern void appInitBitmapImage( AppBitmapImage * abi );

extern int appImgPastePixmap(		AppBitmapImage *	abi,
					APP_WIDGET		w,
					APP_SELECTION_EVENT *	event );

extern int appImgGetXvClipboard(	BitmapDescription *	bd,
					unsigned char **	pBuffer,
					APP_WIDGET		w );

extern int appImgPutXvClipboard(	const BitmapDescription *	bd,
					const unsigned char *		buffer,
					APP_WIDGET			w );
#   endif
