/************************************************************************/
/*									*/
/*  Macintosh PICT File specific definitions.				*/
/*									*/
/************************************************************************/

#   include	<bitmap.h>
#   include	<utilDocFont.h>
#   include	<utilPostscriptFace.h>
#   include	<appDraw.h>
#   include	<appImage.h>
#   include	<sioGeneral.h>

#   include	<appMetafile.h>

/************************************************************************/
/*									*/
/*  Opcodes.								*/
/*									*/
/************************************************************************/

# define MACPICT_NOP			0x00
# define MACPICT_Clip			0x01
# define MACPICT_BkPat			0x02
# define MACPICT_TxFont			0x03
# define MACPICT_TxFace			0x04
# define MACPICT_TxMode			0x05
# define MACPICT_SpExtra		0x06
# define MACPICT_PnSize			0x07
# define MACPICT_PnMode			0x08
# define MACPICT_PnPat			0x09
# define MACPICT_FillPat		0x0a
# define MACPICT_OvSize			0x0b
# define MACPICT_Origin			0x0c
# define MACPICT_TxSize			0x0d
# define MACPICT_FgColor		0x0e
# define MACPICT_BkColor		0x0f
# define MACPICT_TxRatio		0x10
# define MACPICT_Version		0x11
# define MACPICT_BkPixPat		0x12
# define MACPICT_PnPixPat		0x13
# define MACPICT_FillPixPat		0x14
# define MACPICT_PnLocHFrac		0x15
# define MACPICT_ChExtra		0x16
# define MACPICT_RGBFgCol		0x1a
# define MACPICT_RGBBkCol		0x1b
# define MACPICT_HiliteMode		0x1c
# define MACPICT_HiliteColor		0x1d
# define MACPICT_DefHilite		0x1e
# define MACPICT_OpColor		0x1f
# define MACPICT_Line			0x20
# define MACPICT_LineFrom		0x21
# define MACPICT_ShortLine		0x22
# define MACPICT_ShortLineFrom		0x23
# define MACPICT_LongText		0x28
# define MACPICT_DHText			0x29
# define MACPICT_DVText			0x2a
# define MACPICT_DHDVText		0x2b
# define MACPICT_FontName		0x2c
# define MACPICT_LineJustify		0x2d
# define MACPICT_GlyphState		0x2e
# define MACPICT_FrameRect		0x30
# define MACPICT_PaintRect		0x31
# define MACPICT_EraseRect		0x32
# define MACPICT_invertRect		0x33
# define MACPICT_FillRect		0x34
# define MACPICT_FrameSameRect		0x38
# define MACPICT_PaintSameRect		0x39
# define MACPICT_EraseSameRect		0x3a
# define MACPICT_InvertSameRect		0x3b
# define MACPICT_FillSameRect		0x3c
# define MACPICT_FrameRRect		0x40
# define MACPICT_PaintRRect		0x41
# define MACPICT_EraseRRect		0x42
# define MACPICT_InvertRRect		0x43
# define MACPICT_FillRRect		0x44
# define MACPICT_FrameSameRRect		0x48
# define MACPICT_PaintSameRRect		0x49
# define MACPICT_EraseSameRRect		0x4a
# define MACPICT_InvertSameRRect	0x4b
# define MACPICT_FillSameRRect		0x4c
# define MACPICT_FrameOval		0x50
# define MACPICT_PaintOval		0x51
# define MACPICT_EraseOval		0x52
# define MACPICT_InvertOval		0x53
# define MACPICT_FillOval		0x54
# define MACPICT_FrameSameOval		0x58
# define MACPICT_PaintSameOval		0x59
# define MACPICT_EraseSameOval		0x5a
# define MACPICT_InvertSameOval		0x5b
# define MACPICT_FillSameOval		0x5c
# define MACPICT_FrameArc		0x60
# define MACPICT_PaintArc		0x61
# define MACPICT_EraseArc		0x62
# define MACPICT_InvertArc		0x63
# define MACPICT_FillArc		0x64
# define MACPICT_FrameSameArc		0x68
# define MACPICT_PaintSameArc		0x69
# define MACPICT_EraseSameArc		0x6a
# define MACPICT_InvertSameArc		0x6b
# define MACPICT_FillSameArc		0x6c
# define MACPICT_FramePoly		0x70
# define MACPICT_PaintPoly		0x71
# define MACPICT_erasePoly		0x72
# define MACPICT_invertPoly		0x73
# define MACPICT_fillPoly		0x74
# define MACPICT_frameSamePoly		0x78
# define MACPICT_paintSamePoly		0x79
# define MACPICT_eraseSamePoly		0x7a
# define MACPICT_invertSamePoly		0x7b
# define MACPICT_fillSamePoly		0x7c
# define MACPICT_FrameRgn		0x80
# define MACPICT_PaintRgn		0x81
# define MACPICT_EraseRgn		0x82
# define MACPICT_InvertRgn		0x83
# define MACPICT_FillRgn		0x84
# define MACPICT_FrameSameRgn		0x88
# define MACPICT_PaintSameRgn		0x89
# define MACPICT_EraseSameRgn		0x8a
# define MACPICT_InvertSameRgn		0x8b
# define MACPICT_FillSameRgn		0x8c
# define MACPICT_BitsRect		0x90
# define MACPICT_BitsRgn		0x91
# define MACPICT_PackBitsRect		0x98
# define MACPICT_PackBitsRgn		0x99
# define MACPICT_DirectBitsRect		0x9a
# define MACPICT_DirectBitsRgn		0x9b
# define MACPICT_ShortComment		0xa0
# define MACPICT_LongComment		0xa1

# define MACPICT_OpEndPic		0xff

# define MACPICT_CompressedQuickTime	0x8200
# define MACPICT_UncompressedQuickTime	0x8201

/************************************************************************/
/*									*/
/*  Picture Comments.							*/
/*									*/
/************************************************************************/

/* ?
# define PICTCMT_GroupBegin		0
# define PICTCMT_GroupEnd		1
*/

# define PICTCMT_Proprietary		100

# define PICTCMT_MacDrawBegin		130
# define PICTCMT_MacDrawEnd		131

# define PICTCMT_GroupedBegin		140
# define PICTCMT_GroupedEnd		141
# define PICTCMT_BitmapBegin		142
# define PICTCMT_BitmapEnd		143

# define PICTCMT_TextBegin		150
# define PICTCMT_TextEnd		151
# define PICTCMT_StringBegin		152
# define PICTCMT_StringEnd		153
# define PICTCMT_TextCenter		154
# define PICTCMT_LineLayoutOff		155
# define PICTCMT_LineLayoutOn		156
# define PICTCMT_ClientLineLayout	157

# define PICTCMT_PolyBegin		160
# define PICTCMT_PolyEnd		161
# define PICTCMT_PolyIgnore		163
# define PICTCMT_PolySmooth		164
# define PICTCMT_PolyClose		165

# define PICTCMT_ArrowAtEnd		170
# define PICTCMT_ArrowAtStart		171
# define PICTCMT_ArrowBothEnds		172
# define PICTCMT_ArrowEnd		173

# define PICTCMT_DashedLine		180
# define PICTCMT_DashedStop		181
# define PICTCMT_SetLineWidth		182

# define PICTCMT_PostScriptBegin	190
# define PICTCMT_PostScriptEnd		191
# define PICTCMT_PostScriptHandle	192
# define PICTCMT_PostScriptFile		193
# define PICTCMT_TextIsPostScript	194
# define PICTCMT_ResourcePS		195
# define PICTCMT_PSBeginNoSave		196

# define PICTCMT_SetGray		197

# define PICTCMT_RotateBegin		200
# define PICTCMT_RotateEnd		201
# define PICTCMT_RotateCenter		202

# define PICTCMT_FormsPrinting		210
# define PICTCMT_EndFormsPrinting	211

# define PICTCMT_CMBeginProfile		220
# define PICTCMT_CMEndProfile		221
# define PICTCMT_CMEnableMatching	222
# define PICTCMT_CMDisableMatching	223

/************************************************************************/
/*									*/
/*  Pen Modes.								*/
/*									*/
/************************************************************************/

/* ??
# define PENMODE_srcCopy	0
# define PENMODE_srcOr		1
# define PENMODE_srcXor		2
# define PENMODE_srcBic		3
# define PENMODE_notSrcCopy	4
# define PENMODE_notSrcOr	5
# define PENMODE_notSrcXor	6
# define PENMODE_notSrcBic	7
*/

# define PENMODE_patCopy	8
# define PENMODE_patOr		9
# define PENMODE_patXor		10
# define PENMODE_patBic		11
# define PENMODE_notPatCopy	12
# define PENMODE_notPatOr	13
# define PENMODE_notPatXor	14
# define PENMODE_notPatBic	15

/************************************************************************/
/*									*/
/*  Describes a macpict device.						*/
/*									*/
/************************************************************************/

typedef struct MacpictDevice
    {
    int				mdVersion;

    int				mdInX0;
    int				mdInY0;
    int				mdInX1;
    int				mdInY1;

    int				mdRectX0;
    int				mdRectY0;
    int				mdRectX1;
    int				mdRectY1;

    int				mdOvalX0;
    int				mdOvalY0;
    int				mdOvalX1;
    int				mdOvalY1;

    int				mdPolyX0;
    int				mdPolyY0;
    int				mdPolyX1;
    int				mdPolyY1;

    int				mdRRectX0;
    int				mdRRectY0;
    int				mdRRectX1;
    int				mdRRectY1;
    int				mdRRectOvalWide;
    int				mdRRectOvalHigh;

    int				mdArcX0;
    int				mdArcY0;
    int				mdArcX1;
    int				mdArcY1;
    int				mdArcA0;
    int				mdArcA1;

    int				mdOutWide;
    int				mdOutHigh;
    int				mdOutTwipsWide;
    int				mdOutTwipsHigh;

    int				mdPenMode;
    int				mdPenX;
    int				mdPenY;

    unsigned char		mdPenPattern[8];
    int				mdPenIsSolid;

    unsigned char		mdFillPattern[8];
    int				mdFillIsSolid;

    unsigned char		mdBackPattern[8];
    int				mdBackIsSolid;

    unsigned int		mdRedMax;
    unsigned int		mdGreenMax;
    unsigned int		mdBlueMax;

    RGB8Color			mdForeColor;
    RGB8Color			mdBackColor;
    RGB8Color			mdColorSet;
    APP_COLOR_RGB		mdDeviceColorSet;

    char *			mdFontName;
    TextAttribute		mdTextAttribute;
    int				mdPrivateFont;
    AfmFontInfo *		mdAfi;
    int				mdFontEncoding;

    int				mdStippleSet;
    int				mdFillStippled;
    APP_BITMAP_IMAGE		mdTilePixmap;

    unsigned char *		mdTextString;
    int				mdTextStringLength;
    APP_POINT *			mdPolyPoints;
    int				mdPolyPointCount;

    DocumentFontList		mdFontList;
    AppDrawingData		mdDrawingData;
    const PostScriptFontList *	mdPostScriptFontList;
    } MacpictDevice;

#   define	STIPPLE_SOLID	0
#   define	STIPPLE_PEN	1
#   define	STIPPLE_BACK	2
#   define	STIPPLE_FILL	3
#   define	STIPPLE_UNDEF	4

# define MD_X( x, md ) \
    ( ( (md)->mdOutWide* ( (x)-(md)->mdInX0 ) )/( (md)->mdInX1-(md)->mdInX0 ) )
# define MD_Y( y, md ) \
    ( ( (md)->mdOutHigh* ( (y)-(md)->mdInY0 ) )/( (md)->mdInY1-(md)->mdInY0 ) )

# define MD_W( w, md ) \
    ( ( (md)->mdOutWide* ( (w) ) )/( (md)->mdInX1-(md)->mdInX0 ) )
# define MD_H( h, md ) \
    ( ( (md)->mdOutHigh* ( (h) ) )/( (md)->mdInY1-(md)->mdInY0 ) )

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appMacPictInitDeviceHeader(	MacpictDevice *		md,
					int			outWide,
					int			outHigh,
					int			outTwipsWide,
					int			outTwipsHigh );

extern void appMacPictCleanDeviceHeader( MacpictDevice *	md );

extern int appMacPictGetDeviceHeader(	MacpictDevice *		md,
					SimpleInputStream *	sis );

extern int appMacPictGetOpcode(	unsigned int *			pOpcode,
				int *				pBytes,
				const MacpictDevice *		md,
				SimpleInputStream *		sis );

extern int appMacPictGetColor(	RGB8Color *		rgb8,
				const MacpictDevice *	md,
				SimpleInputStream *	sis );

extern int appMacPictGetPoly(		MacpictDevice *		md,
					SimpleInputStream *	sis );

extern int appMacPictReadPaletteColors(	BitmapDescription *	bd,
					int *			pBytesRead,
					const MacpictDevice *	md,
					int			colorCount,
					int			flags,
					SimpleInputStream *	sis );

extern int appMacPictReadPacBitsRow(
				unsigned char *			to,
				int *				pBytesRead,
				int				bytesPerRow,
				const MacpictDevice *		md,
				const BitmapDescription *	bd,
				SimpleInputStream *		sis );

extern int appMacPictPlayFileX11( SimpleInputStream *	sis,
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

extern int appMacPictPlayFilePs( PrintingState *		ps,
				SimpleInputStream *		sis,
				const PostScriptFontList *	psfl,
				int				useFilters,
				int				indexedImages,
				int				mapMode,
				int				xWinExt,
				int				yWinExt,
				int				twipsWide,
				int				twipsHigh );

extern int appMacPictListFontsPs( PostScriptTypeList *	psfl,
				SimpleInputStream *	sis,
				const PostScriptFontList * x_psfl,
				const char *		prefix,
				int			mapMode,
				int			xWinExt,
				int			yWinExt,
				int			twipsWide,
				int			twipsHigh );

extern int appMacPictGetCountAndString(	MacpictDevice *		md,
					int *			pCount,
					SimpleInputStream *	sis );

extern int appMacPictGetFontName(	MacpictDevice *		md,
					SimpleInputStream *	sis );

extern int appMacPictGetTxFont(		MacpictDevice *		md,
					SimpleInputStream *	sis );

extern int appMacPictGetTxFace(		MacpictDevice *		md,
					SimpleInputStream *	sis );

extern int appMacPictGetTxSize(		MacpictDevice *		md,
					SimpleInputStream *	sis );

extern int appMacPictGetArc(		MacpictDevice *		md,
					SimpleInputStream *	sis );

extern int appMacPictGetOval(		MacpictDevice *		md,
					SimpleInputStream *	sis );

extern int appMacPictGetOvSize(		MacpictDevice *		md,
					SimpleInputStream *	sis );

extern int appMacPictGetShortLine(	int *			pX0,
					int *			pY0,
					int *			pX1,
					int *			pY1,
					MacpictDevice *		md,
					SimpleInputStream *	sis );

extern int appMacPictGetShortLineFrom(	int *			pX0,
					int *			pY0,
					int *			pX1,
					int *			pY1,
					MacpictDevice *		md,
					SimpleInputStream *	sis );

extern int appMacPictGetLine(		int *			pX0,
					int *			pY0,
					int *			pX1,
					int *			pY1,
					MacpictDevice *		md,
					SimpleInputStream *	sis );

extern int appMacPictGetLineFrom(	int *			pX0,
					int *			pY0,
					int *			pX1,
					int *			pY1,
					MacpictDevice *		md,
					SimpleInputStream *	sis );

extern int appMacPictGetPattern(	MacpictDevice *		md,
					SimpleInputStream *	sis,
					int			unsetStipple,
					int *			pIsSolid,
					unsigned char		pattern[8] );

extern int appMacPictGetImage(		AppBitmapImage *	abi,
					int *			pY0Source,
					int *			pX0Source,
					int *			pY1Source,
					int *			pX1Source,
					int *			pY0Dest,
					int *			pX0Dest,
					int *			pY1Dest,
					int *			pX1Dest,
					MacpictDevice *		md,
					SimpleInputStream *	sis,
					int			packed,
					int			direct,
					int			clipRegion );

extern int appMacPictGetGlyphState(	MacpictDevice *		md,
					SimpleInputStream *	sis );

extern int appMacPictGetDHText(		int *			pCount,
					MacpictDevice *		md,
					SimpleInputStream *	sis );

extern int appMacPictGetDVText(		int *			pCount,
					MacpictDevice *		md,
					SimpleInputStream *	sis );

extern int appMacPictGetLongText(	int *			pCount,
					MacpictDevice *		md,
					SimpleInputStream *	sis );

extern int appMacPictGetDHDVText(	int *			pCount,
					MacpictDevice *		md,
					SimpleInputStream *	sis );

extern const char * appMacPictCommentName( int			n );

