/************************************************************************/
/*									*/
/*  Image property tool:						*/
/*									*/
/************************************************************************/

#   ifndef	TED_BITMAP_TOOL_H
#   define	TED_BITMAP_TOOL_H

#   include	<bitmap.h>
#   include	<appFrame.h>

/************************************************************************/
/*									*/
/*  Resources for the 'Image' page.					*/
/*									*/
/************************************************************************/

typedef struct ImagePageResources
    {
    char *		iprPixelSize;
    char *		iprTotalBytes;
    char *		iprImageWide;
    char *		iprImageHigh;
    char *		iprXScale;
    char *		iprYScale;
    } ImagePageResources;

/************************************************************************/
/*									*/
/*  A bitmap tool, i.e. the 'Image' page of the format tool.		*/
/*									*/
/************************************************************************/

typedef struct ImageTool
    {
    EditApplication *			itApplication;
    AppInspector *			itInspector;
    const ImagePageResources *		itPageResources;

    int					itPictureHasBitmap;
    int					itTotalBytes;
    BitmapDescription			itBitmapDescription;

    PictureProperties			itPropertiesChosen;
    PictureProperties			itPropertiesSet;

    int					itImageHighAbsChanged;
    int					itImageWideAbsChanged;

    APP_WIDGET				itPixelSizeRow;
    APP_WIDGET				itPixelsSizeLabel;
    APP_WIDGET				itPixelsSizeText;

    APP_WIDGET				itByteSizeRow;
    APP_WIDGET				itByteSizeLabel;
    APP_WIDGET				itByteSizeText;

    APP_WIDGET				itImageWideRow;
    APP_WIDGET				itImageWideLabel;
    APP_WIDGET				itImageWideText;

    APP_WIDGET				itImageHighRow;
    APP_WIDGET				itImageHighLabel;
    APP_WIDGET				itImageHighText;

    APP_WIDGET				itXScaleRow;
    APP_WIDGET				itXScaleLabel;
    APP_WIDGET				itXScaleText;

    APP_WIDGET				itYScaleRow;
    APP_WIDGET				itYScaleLabel;
    APP_WIDGET				itYScaleText;
    } ImageTool;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

void tedImageToolGetImageResourceTable(
				EditApplication *		ea,
				ImagePageResources *		ipr,
				InspectorSubjectResources *	isr );

extern void tedImagePageFillChoosers(
				ImageTool *			it,
				const ImagePageResources *	ipr );

extern void tedImageFinishImagePage(
				ImageTool *			it,
				const ImagePageResources *	ipr );

extern void tedFormatFillImagePage(
				ImageTool *			it,
				const ImagePageResources *	ipr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr );

extern void tedRefreshImageTool(
			    ImageTool *				it,
			    int *				pEnabled,
			    int *				pPref,
			    InspectorSubject *			is,
			    const DocumentSelection *		ds,
			    const SelectionDescription *	sd,
			    BufferDocument *			bd );

extern void tedFormatCleanImageTool(	ImageTool *	it );
extern void tedFormatInitImageTool(	ImageTool *	it );

extern void tedImageToolGetResourceTable(
				EditApplication *		ea,
				ImagePageResources *		ipr,
				InspectorSubjectResources *	isr );

#   endif	/*  TED_BITMAP_TOOL_H */
