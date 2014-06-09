/************************************************************************/
/*									*/
/*  Simple color chooser on an inspector page.				*/
/*									*/
/************************************************************************/

#   ifndef	APP_COLOR_CHOOSER_H
#   define	APP_COLOR_CHOOSER_H

#   include	<appGuiBase.h>
#   include	<appDraw.h>

struct ColorChooser;

typedef void (*ColorChooserCallback)(
				struct ColorChooser *		cc,
				int				which,
				void *				through,
				int				choice,
				const RGB8Color *		rgb8 );

typedef struct ColorChooserResources
    {
    char *	ccrAutomaticColor;
    char *	ccrMoreColors;
    } ColorChooserResources;

typedef struct ColorChooserPaletteColor
    {
    RGB8Color		ccpcRGB8Color;
    APP_COLOR_RGB	ccpcAllocatedColor;
    int			ccpcColorAllocated;
    int			ccpcStatus;
    } ColorChooserPaletteColor;

#   define	CCstatusFREE	0
#   define	CCstatusSYSTEM	1
#   define	CCstatusPALETTE	2

typedef struct ColorChooser
    {
    int					ccFilled;

    const ColorChooserResources *	ccResources;

    AppDrawnPulldown			ccPulldown;
    AppDrawingData			ccInplaceDrawingData;
    AppDrawingData			ccPulldownDrawingData;
    int					ccPulldownDrawingDataSet;
    APP_COLOR_RGB			ccPulldownBackgroundColor;

    AppColors				ccInplaceColors;
    AppColors				ccPulldownColors;

    int					ccInplaceColorAllocated;
    APP_COLOR_RGB			ccInplaceColor;
    APP_FONT *				ccTextFont;

    RGB8Color				ccColorChosen;
    int					ccColorExplicit;
    int					ccColorSet;

    ColorChooserCallback		ccCallback;
    void *				ccTarget;
    int					ccWhich;

    int					ccStripHigh;
    int					ccColumnWide;
    int					ccStrips;
    int					ccColumns;
    int					ccXShift;

    ColorChooserPaletteColor *		ccColors;
    int					ccColorCount;
    } ColorChooser;

typedef enum ColorChooserChoice
    {
    CHOICEccDEFAULT= 0,
    CHOICEccCOLOR,
    CHOICEccMORE,

    CHOICEcc_COUNT
    } ColorChooserChoice;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appColorChooserSetColor(	ColorChooser *		cc,
					int			explicit,
					const RGB8Color *	rgb8 );

extern void appColorChooserUnset(	ColorChooser *		cc );

extern void appColorChooserSuggestPalette(
					ColorChooser *		cc,
					int			avoidZero,
					const RGB8Color *	colors,
					int			colorCount );

extern void appMakeColorChooserInRow(
				ColorChooser *			cc,
				APP_WIDGET			row,
				int				col,
				const ColorChooserResources *	ccr,
				ColorChooserCallback		callback,
				int				which,
				void *				through );

extern void appInitColorChooser(	ColorChooser *		cc );
extern void appCleanColorChooser(	ColorChooser *		cc );

extern void appMakeLabelAndColorChooserRow(
				APP_WIDGET *			pRow,
				APP_WIDGET *			pLabel,
				ColorChooser *			cc,
				APP_WIDGET			column,
				const char *			labelText,
				const ColorChooserResources *	ccr,
				ColorChooserCallback		callback,
				int				which,
				void *				through );

extern void appFinishColorChooser(
				ColorChooser *			cc,
				APP_FONT *			textFont );

extern void appColorChooserColorChosen(	PropertyMask *		isSetMask,
					int *			pChanged,
					RGB8Color *		rgb8To,
					int *			pExplicit,
					const RGB8Color *	rgb8Set,
					int			explicit,
					int			which );

#   endif	/*  APP_COLOR_CHOOSER_H */
