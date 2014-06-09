/************************************************************************/
/*									*/
/*  Windows Meta File specific definitions.				*/
/*									*/
/************************************************************************/

#   include	<appGuiBase.h>

#   include	<bitmap.h>
#   include	<utilDocFont.h>
#   include	<utilPostscriptFace.h>
#   include	<appDraw.h>
#   include	<appImage.h>
#   include	<sioGeneral.h>

#   include	<appMetafile.h>

#   define	WINMETA_Arc			0x0817
#   define	WINMETA_Chord			0x0830
#   define	WINMETA_Ellipse			0x0418
#   define	WINMETA_ExcludeClipRect		0x0415
#   define	WINMETA_FloodFill		0x0419
#   define	WINMETA_IntersectClipRect	0x0416
#   define	WINMETA_LineTo			0x0213
#   define	WINMETA_MoveTo			0x0214
#   define	WINMETA_OffsetClipRgn		0x0220
#   define	WINMETA_OffsetViewportOrg	0x0211
#   define	WINMETA_OffsetWindowOrg		0x020f
#   define	WINMETA_PatBlt			0x061d
#   define	WINMETA_Pie			0x081a
#   define	WINMETA_RealizePalette		0x0035
#   define	WINMETA_Rectangle		0x041b
#   define	WINMETA_ResizePalette		0x0319
#   define	WINMETA_RestoreDC		0x0127
#   define	WINMETA_RoundRect		0x061c
#   define	WINMETA_SaveDC			0x001e
#   define	WINMETA_ScaleViewportExt	0x0412
#   define	WINMETA_ScaleWindowExt		0x0400
#   define	WINMETA_SetBkColor		0x0201
#   define	WINMETA_SetBkMode		0x0102
#   define	WINMETA_SetMapMode		0x0103
#   define	WINMETA_SetMapperFlags		0x0231
#   define	WINMETA_SetPixel		0x041f
#   define	WINMETA_SetPolyFillMode		0x0106
#   define	WINMETA_SetROP2			0x0104
#   define	WINMETA_SetStretchBltMode	0x0107
#   define	WINMETA_SetTextAlign		0x012e
#   define	WINMETA_SetTextCharExtra	0x0108
#   define	WINMETA_SetTextColor		0x0209
#   define	WINMETA_SetTextJustification	0x020a
#   define	WINMETA_SetViewportExt		0x020e
#   define	WINMETA_SetViewportOrg		0x020d
#   define	WINMETA_SetWindowExt		0x020c
#   define	WINMETA_SetWindowOrg		0x020b

#   define	WINMETA_AnimatePalette		0x0436
#   define	WINMETA_BitBlt_2		0x0922
#   define	WINMETA_BitBlt			0x0940
#   define	WINMETA_CreateBrushIndirect	0x02fc
#   define	WINMETA_CreateFontIndirect	0x02fb
#   define	WINMETA_CreatePalette		0x00f7
#   define	WINMETA_CreatePatternBrush_2	0x01f9
#   define	WINMETA_CreatePatternBrush	0x0142
#   define	WINMETA_CreatePenIndirect	0x02fa
#   define	WINMETA_CreateRegion		0x06ff
#   define	WINMETA_DeleteObject		0x01f0
#   define	WINMETA_Escape			0x0626
#   define	WINMETA_ExtTextOut		0x0a32
#   define	WINMETA_Polygon			0x0324
#   define	WINMETA_PolyPolygon		0x0538
#   define	WINMETA_PolyLine		0x0325
#   define	WINMETA_SelectClipRgn		0x012c
#   define	WINMETA_SelectObject		0x012d
#   define	WINMETA_SelectPalette		0x0234
#   define	WINMETA_SetDIBitsToDevice	0x0d33
#   define	WINMETA_SetPaletteEntries	0x0037
#   define	WINMETA_StretchBlt_2		0x0b23
#   define	WINMETA_StretchBlt		0x0b41
#   define	WINMETA_StretchDIBits		0x0f43
#   define	WINMETA_TextOut			0x0521

/************************************************************************/
/*									*/
/*  1)  Various MapModes: Refer to petzold for an explanation.		*/
/*  2)  StrechBlt() modes.						*/
/*  3)  Font family styles.						*/
/*  4)  Polygon Fill Modes.						*/
/*									*/
/************************************************************************/

/*  1  */
#   define	MM_TEXT				1
#   define	MM_LOMETRIC			2
#   define	MM_HIMETRIC			3
#   define	MM_LOENGLISH			4
#   define	MM_HIENGLISH			5
#   define	MM_TWIPS			6
#   define	MM_ISOTROPIC			7
#   define	MM_ANISOTROPIC			8

/*  2  */
#   define	STRETCH_BLT_BLACKONWHITE	1
#   define	STRETCH_BLT_WHITEONBLACK	2
#   define	STRETCH_BLT_COLORONCOLOR	3

/*  3  */
#   define	FF_DONTCARE			0x00
#   define	FF_ROMAN			0x10
#   define	FF_SWISS			0x20
#   define	FF_MODERN			0x30
#   define	FF_SCRIPT			0x40
#   define	FF_DECORATIVE			0x50

/*  4  */
#   define	ALTERNATE			1
#   define	WINDING				2

/************************************************************************/
/*									*/
/*  Implementation of a metafile 'Object'				*/
/*									*/
/************************************************************************/

#   define TA_NOUPDATECP			0x0000
#   define TA_UPDATECP				0x0001
#   define TA_LEFT				0x0000
#   define TA_RIGHT				0x0002
#   define TA_CENTER				0x0006
#   define TA_TOP				0x0000
#   define TA_BOTTOM				0x0008
#   define TA_BASELINE				0x0018

#   define	BKMODE_OPAQUE		0
#   define	BKMODE_TRANSPARENT	1

typedef struct LogicalBrush
    {
    int			lbStyle;
#			define		BS_SOLID	0
#			define		BS_NULL		1
#			define		BS_HOLLOW	BS_NULL
#			define		BS_HATCHED	2
#			define		BS_PATTERN	3
#			define		BS_INDEXED	4
#			define		BS_DIBPATTERN	5
    RGB8Color		lbColor;
    int			lbHatch;
#			define HS_HORIZONTAL       0
#			define HS_VERTICAL         1
#			define HS_FDIAGONAL        2
#			define HS_BDIAGONAL        3
#			define HS_CROSS            4
#			define HS_DIAGCROSS        5

    APP_BITMAP_IMAGE	lbTilePixmap;
    } LogicalBrush;

typedef struct PatternBrush
    {
    int			pbType;		/*  == 5	*/
    int			pbUsage;	/*  == 0	*/
    AppBitmapImage	pbAbi;
    APP_BITMAP_IMAGE	pbTilePixmap;
    } PatternBrush;

typedef struct LogicalPen
    {
    int		lpStyle;
#		define		PS_SOLID	0
#		define		PS_DASH		1
#		define		PS_DOT		2
#		define		PS_DASHDOT	3
#		define		PS_DASHDOTDOT	4
#		define		PS_NULL		5
#		define		PS_INSIDEFRAME	6
    int		lpWidth;
    RGB8Color	lpColor;
    } LogicalPen;

typedef struct LogicalFont
    {
    int			lfHeight;
    int			lfWidth;
    int			lfEscapement;
    int			lfOrientation;
    int			lfWeight;
    unsigned char	lfItalic;
    unsigned char	lfUnderline;
    unsigned char	lfStrikeOut;
    unsigned char	lfCharSet;
    unsigned char	lfOutPrecision;
    unsigned char	lfClipPrecision;
    unsigned char	lfQuality;
    unsigned char	lfPitchAndFamily;
    char		lfFaceName[32];

    /************************************/
    /*  Implementation			*/
    /************************************/
    TextAttribute	lfTextAttribute;
    int			lfPrivateFont;
    } LogicalFont;

typedef struct LogicalPalette
    {
    int		lpVersion;
    int		lpColorCount;
    } LogicalPalette;

typedef union MetafileSpecific
    {
    LogicalBrush	mfsLogicalBrush;
    LogicalPen		mfsLogicalPen;
    LogicalFont		mfsLogicalFont;
    PatternBrush	mfsPatternBrush;
    } MetafileSpecific;

typedef struct MetaFileObject
    {
    int			mfoType;
    MetafileSpecific	mfoSpecific;
    } MetaFileObject;

#   define	MFtypeFREE		0
#   define	MFtypeBRUSH		1
#   define	MFtypePEN		2
#   define	MFtypeFONT		3
#   define	MFtypePALETTE		4
#   define	MFtypePATTERNBRUSH	5

#   define	mfoLogicalBrush		mfoSpecific.mfsLogicalBrush
#   define	mfoLogicalPen		mfoSpecific.mfsLogicalPen
#   define	mfoLogicalFont		mfoSpecific.mfsLogicalFont
#   define	mfoPatternBrush		mfoSpecific.mfsPatternBrush

/************************************************************************/
/*									*/
/*  Refer to Petzold, Charles: "Programming Windows 95", Microsoft	*/
/*  Press, Redmond, WA, 1996 pp138..					*/
/*									*/
/*  Terminology is extremely confusing: The window is the system of	*/
/*  'logical' user coordinates. The viewport is the sytem of 'hard'	*/
/*  device coordinates. Viewport=Device coordinates usually are in	*/
/*  pixels.								*/
/*									*/
/************************************************************************/

typedef struct DeviceCoordinateTransform
    {
    int					dct_xWinOrg;
    int					dct_yWinOrg;
    int					dct_xWinExt;
    int					dct_yWinExt;

    int					dct_xViewOrg;
    int					dct_yViewOrg;
    int					dct_xViewExt;
    int					dct_yViewExt;

    struct DeviceCoordinateTransform *	dctPushed;
    } DeviceCoordinateTransform;

# define DC_xWinOrg(dc)		((dc)->dcTransform.dct_xWinOrg)
# define DC_yWinOrg(dc)		((dc)->dcTransform.dct_yWinOrg)
# define DC_xWinExt(dc)		((dc)->dcTransform.dct_xWinExt)
# define DC_yWinExt(dc)		((dc)->dcTransform.dct_yWinExt)

# define DC_xViewOrg(dc)	((dc)->dcTransform.dct_xViewOrg)
# define DC_yViewOrg(dc)	((dc)->dcTransform.dct_yViewOrg)
# define DC_xViewExt(dc)	((dc)->dcTransform.dct_xViewExt)
# define DC_yViewExt(dc)	((dc)->dcTransform.dct_yViewExt)

# define DC_xViewport(x,dc) ( ( ( x-DC_xWinOrg(dc) )* DC_xViewExt(dc) ) / \
					    DC_xWinExt(dc)+ DC_xViewOrg(dc) )
# define DC_yViewport(y,dc) ( ( ( y-DC_yWinOrg(dc) )* DC_yViewExt(dc) ) / \
					    DC_yWinExt(dc)+ DC_yViewOrg(dc) )

# define DC_wViewport(w,dc) ( ( (w)* DC_xViewExt(dc) ) / DC_xWinExt(dc) )
# define DC_hViewport(h,dc) ( ( (h)* DC_yViewExt(dc) ) / DC_yWinExt(dc) )

typedef struct DeviceContext
    {
    int				dcMapMode;
    int				dcDrawBorders;
    int				dcFillInsides;
    int				dcROP2;

    int				dcClipX0;
    int				dcClipY0;
    int				dcClipX1;
    int				dcClipY1;

    int				dcFillHatched;
    int				dcFillPattern;

    DocumentFontList		x_dcFontList;
    const PostScriptFontList *	dcPostScriptFontList;
    AppDrawingData		dcDrawingData;

    LogicalPen			dcPen;
    LogicalBrush		dcBrush;
    LogicalFont			dcFont;

    RGB8Color			dcBkColor;
    RGB8Color			dcTextColor;
    int				dcBkMode;

    int				dcRedSet;
    int				dcGreenSet;
    int				dcBlueSet;
    TextAttribute		dcTextAttributeSet;
    LogicalPen			dcPenSet;

    int				dcExtraTextSpacing;
    int				dcJustificationAmount;
    int				dcJustificationSpaces;
    int				dcTextAlignment;
    long			dcStretchBltMode;

    int				dcX;
    int				dcY;

    unsigned char *		dcString;

    DeviceCoordinateTransform	dcTransform;
    int				dcTwipsWide;
    int				dcTwipsHigh;

    MetaFileObject *		dcObjects;
    int				dcObjectCount;

    APP_POINT *			dcPoints;

    AfmFontInfo *		dcAfi;
    int				dcFontEncoding;
    } DeviceContext;

# define DCX_xViewport(x,dcx)	(DC_xViewport(x,&(dcx->dcxDeviceContext)))
# define DCX_yViewport(y,dcx)	(DC_yViewport(y,&(dcx->dcxDeviceContext)))
# define DCX_wViewport(w,dcx)	(DC_wViewport(w,&(dcx->dcxDeviceContext)))
# define DCX_hViewport(h,dcx)	(DC_hViewport(h,&(dcx->dcxDeviceContext)))

typedef struct DeviceContextX11
    {
    DeviceContext	dcxDeviceContext;

    int			dcxPixelsWide;
    int			dcxPixelsHigh;

    APP_COLOR_RGB	dcxPenColor;
    APP_COLOR_RGB	dcxBrushColor;

    APP_COLOR_RGB	dcxBkColor;
    APP_COLOR_RGB	dcxTextColor;
    int			dcxFillTiled;
    int			dcxFillHatched;

    DrawScreenFont *	dcxPhysicalFont;
    } DeviceContextX11;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appMetaPlayWmfX11(	SimpleInputStream *	sis,
				void **			pPrivate,
				AppColors *		ac,
				AppDrawingData *	add,
				APP_WINDOW		pixmap,
				int			mapMode,
				int			xWinExt,
				int			yWinExt,
				int			pixelsWide,
				int			pixelsHigh,
				int			twipsWide,
				int			twipsHigh );

extern int appMetaPlayEmfX11(	SimpleInputStream *	sis,
				void **			pPrivate,
				AppColors *		ac,
				AppDrawingData *	add,
				APP_WINDOW		pixmap,
				int			mapMode,
				int			xExt,
				int			yExt,
				int			pixelsWide,
				int			pixelsHigh,
				int			twipsWide,
				int			twipsHigh );

extern int appMetaPlayFilePs(	PrintingState *			ps,
				SimpleInputStream *		sis,
				const PostScriptFontList *	psfl,
				int				useFilters,
				int				indexedImages,
				int				mapMode,
				int				xWinExt,
				int				yWinExt,
				int				twipsWide,
				int				twipsHigh );

extern void appMetaCleanDeviceContext(	DeviceContext *		dc );

extern int appMetaInitDeviceContext(
				DeviceContext *			dc,
				const PostScriptFontList *	psfl,
				int				objectCount,
				int				mapMode,
				int				xWinExt,
				int				yWinExt,
				int				xViewExt,
				int				yViewExt,
				int				twipsWide,
				int				twipsHigh );

extern void appMetaCleanObject( MetaFileObject *	mfo );

extern int appMeta_ExtTextOut(		SimpleInputStream *	sis,
					int			recordSize,
					int *			pX0,
					int *			pY0,
					int *			pCount,
					int *			pStyle,
					int *			pX1,
					int *			pY1,
					int *			pH1,
					int *			pW1,
					DeviceContext *		dc );

extern int appMeta_TextOut(	SimpleInputStream *	sis,
				int			recordSize,
				int *			pX0,
				int *			pY0,
				int *			pCount,
				DeviceContext *		dc );

extern int appMeta_GetCounts(	SimpleInputStream *	sis,
				int			recordSize,
				int *			pCount,
				int **			pCounts,
				DeviceContext *		dc );

extern int appMetaCreateBrushIndirect(	DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appMetaCreatePenIndirect(	DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appMetaCreateFontIndirect(	DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appMetaCreatePatternBrush(	DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appMetaCreatePalette(	DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appMetaGetColor(		SimpleInputStream *	sis,
					RGB8Color *		rgb8 );

extern int appMetaListFontsPs(	PostScriptTypeList *	psfl,
				SimpleInputStream *	sis,
				const PostScriptFontList * x_psfl,
				const char *		prefix,
				int			mapMode,
				int			xExt,
				int			yExt,
				int			twipsWide,
				int			twipsHigh );

extern int appMetaBitmapImage(		AppBitmapImage **	pAbi,
					SimpleInputStream *	sis,
					int			expectBytes );

extern int appMetaSetTextAlign(		DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appMetaDefineProcsetPs(	SimpleOutputStream *	sos );

extern int appMetaPlayWmfImg(	BitmapDescription *	pBd,
				unsigned char **	pBuffer,
				SimpleInputStream *	sis );

extern int appMetaSetMapMode(		DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appMetaOffsetWindowOrg(	DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appMetaSetWindowOrg(		DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appMetaSetWindowExt(		DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appMetaSetViewportOrg(	DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appMetaSetViewportExt(	DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appMetaScaleViewportExt(	DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appMetaScaleWindowExt(	DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appMetaSaveDC(		DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appMetaRestoreDC(		DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appMetaOffsetClipRgn(	DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appMetaExcludeClipRect(	DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appMetaSelectClipRgn(	DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appMetaIntersectClipRect(	DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appMetaSetBkColor(		DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appMetaSetTextColor(		DeviceContext *		dc,
					int			recordSize,
					SimpleInputStream *	sis );

extern int appWinMetaGetPoints(		DeviceContext *		dc,
					int			count,
					SimpleInputStream *	sis );

extern void appMetaX11SetWindowsLineStyle(
					AppDrawingData *	add,
					int *			pDraw,
					int			ps,
					int			lpW,
					int			width );

