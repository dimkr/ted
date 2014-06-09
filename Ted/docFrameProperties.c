/************************************************************************/
/*									*/
/*  Positioned Objects and Frames					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docFrameProperties.h"

/************************************************************************/
/*									*/
/*  Manage text frame properties.					*/
/*									*/
/************************************************************************/

void docInitTextFrameProperties(	FramePosition *	fp )
    {
    fp->fpFrameWidthTwips= 0;
    fp->fpFrameHeightTwips= 0;

    fp->fpFrameXTwips= 0;
    fp->fpFrameYTwips= 0;

    fp->fpDistanceFromTextTwipsLeft= 0;
    fp->fpDistanceFromTextTwipsRight= 0;
    fp->fpDistanceFromTextTwipsTop= 0;
    fp->fpDistanceFromTextTwipsBottom= 0;
    fp->fpDistanceFromTextSet= 0;

    fp->fpHorizontalFrameReference= HFRphCOL;
    fp->fpHorizontalFrameAlignment= HFAposXL;

    fp->fpVerticalFrameReference= VFRpvMRG;
    fp->fpVerticalFrameAlignment= VFAposYIL;
    fp->fpFrameLockedToParagraph= 0;

    fp->fpFrameWrapStyle= FWSwrapWRAP;
    fp->fpNoOverlap= 0;

    return;
    }

