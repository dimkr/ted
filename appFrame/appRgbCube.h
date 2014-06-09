/************************************************************************/
/*									*/
/*  Draw an RGB cube to allow the user to select colors.		*/
/*									*/
/************************************************************************/

#   ifndef	APP_RGB_CUBE_H
#   define	APP_RGB_CUBE_H

#   include	<bmcolor.h>
#   include	<appDraw.h>
#   include	<utilAffineTransform.h>

typedef struct RgbColorBlock
    {
    RGB8Color		rcbRgbColor;
    APP_COLOR_RGB	rcbAllocatorColor;
    APP_COLOR_RGB	rcbAllocatorColorX;
    int			rcbColorAllocated;

    int			rcbSelected;
    int			rcbMappedIndex;
			    /*  Calculated index, based on color values	*/
			    /*  does not work: blocks are sorted.	*/
			    /*  At the calculated position, remember	*/
			    /*  where the color now is.			*/

    double		rcbX0;
    double		rcbY0;
    double		rcbZ0;

    double		rcbX1;
    double		rcbY1;
    double		rcbZ1;

    double		rcbZ;
    } RgbColorBlock;

typedef struct RgbCube
    {
    int			rcRedSteps;
    int			rcGreenSteps;
    int			rcBlueSteps;

    int			rcRedStep;
    int			rcGreenStep;
    int			rcBlueStep;

    RGB8Color		rcSplitValues;
    RGB8Color		rcBSplitValues;
    int			rcSplitColor; /* enum */

    AffineTransform3D	rcAt;

    int			rcColorBlockCount;
    RgbColorBlock *	rcColorBlocks;

    AppColors		rcColors;
    APP_COLOR_RGB	rcBackColor;

    RGB8Color		rcSelectedColor;
    int			rcColorSelected;
    APP_COLOR_RGB	rcAllocatedColor;
    int			rcColorAllocated;
    } RgbCube;

typedef enum RgbCubeSplitColor
    {
    RCsplitNONE= 0,
    RCsplitRED,
    RCsplitGREEN,
    RCsplitBLUE,

    RCsplit_COUNT
    } RgbCubeSplitColor;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appInitRgbCube(	RgbCube *			rc );

extern void appCleanRgbCube(	RgbCube *			rc );

extern void appRedrawRgbCube(	RgbCube *			rc,
				int				wide,
				int				high,
				const DocumentRectangle *	drClip,
				AppDrawingData *		add );

extern int appPrepareRgbCube(	RgbCube *			rc,
				AppDrawingData *		add,
				int				redSteps,
				int				greenSteps,
				int				blueSteps );

extern void appRotateRgbCube(	RgbCube *		rc,
				int			mouseX,
				int			mouseY,
				int			wide,
				int			high );

extern int appRgbCubeFindColor(RGB8Color *		rgb8,
				int *			pOnOutside,
				RgbCube *		rc,
				int			mouseX,
				int			mouseY,
				int			wide,
				int			high );

extern void appRgbCubeSetSplit(	RgbCube *		rc,
				int			splitColor,
				const RGB8Color *	splitValues );

extern void appRgbCubeSelectColor(	RgbCube *		rc,
					const RGB8Color *	rgb8 );

extern void appRgbCubeRefreshSplit(	RgbCube *		rc,
					const RGB8Color *	rgb8 );

#   endif
