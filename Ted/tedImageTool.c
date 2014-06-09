/************************************************************************/
/*									*/
/*  Image page of the format tool.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>
#   include	<limits.h>

#   include	<appGeoString.h>
#   include	<appUnit.h>

#   include	"tedApp.h"
#   include	"tedFormatTool.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Get/Set absolute values for the dimensions by translating from/to	*/
/*  the scale.								*/
/*									*/
/************************************************************************/

static void tedImageToolShowWideAbs(	ImageTool *	it )
    {
    EditApplication *		ea= it->itApplication;
    const PictureProperties *	pip= &(it->itPropertiesChosen);

    char			scratch[50];

    appGeoLengthToString( scratch,
		( pip->pipScaleXSet* pip->pipTwipsWide )/ 100, ea->eaUnitInt );
    appStringToTextWidget( it->itImageWideText, scratch );

    return;
    }

static void tedImageToolShowHighAbs(	ImageTool *	it )
    {
    EditApplication *		ea= it->itApplication;
    const PictureProperties *	pip= &(it->itPropertiesChosen);

    char			scratch[50];

    appGeoLengthToString( scratch,
		( pip->pipScaleYSet* pip->pipTwipsHigh )/ 100, ea->eaUnitInt );
    appStringToTextWidget( it->itImageHighText, scratch );

    return;
    }

static int tedImageToolGetWideAbs(	ImageTool *	it )
    {
    EditApplication *		ea= it->itApplication;
    PictureProperties *		pip= &(it->itPropertiesChosen);

    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;
    const int			minSize= 1;
    const int			adaptToMin= 0;

    int				value= 0;
    int				changed= 0;

    if  ( appGetLengthFromTextWidget( it->itImageWideText,
				&value, &changed, ea->eaUnitInt,
				minSize, adaptToMin, maxValue, adaptToMax ) )
	{ return -1;	}

    pip->pipScaleXSet= ( 100* value )/ pip->pipTwipsWide;

    return 0;
    }

static int tedImageToolGetHighAbs(	ImageTool *	it )
    {
    EditApplication *		ea= it->itApplication;
    PictureProperties *		pip= &(it->itPropertiesChosen);

    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;
    const int			minSize= 1;
    const int			adaptToMin= 0;

    int				value= 0;
    int				changed= 0;

    if  ( appGetLengthFromTextWidget( it->itImageHighText,
				&value, &changed, ea->eaUnitInt,
				minSize, adaptToMin, maxValue, adaptToMax ) )
	{ return -1;	}

    pip->pipScaleYSet= ( 100* value )/ pip->pipTwipsHigh;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Refresh the 'Image' page of the format tool.			*/
/*									*/
/************************************************************************/

static void tedFormatToolRefreshImagePage(	ImageTool *	it )
    {
    const PictureProperties *	pip= &(it->itPropertiesChosen);
    char			scratch[50];

    if  ( it->itPictureHasBitmap )
	{
	const BitmapDescription *	bd= &(it->itBitmapDescription);

	sprintf( scratch, "%d x %d", bd->bdPixelsWide, bd->bdPixelsHigh );
	appStringToTextWidget( it->itPixelsSizeText, scratch );
	}
    else{
	appStringToTextWidget( it->itPixelsSizeText, "" );
	}

    appIntegerToTextWidget( it->itByteSizeText, it->itTotalBytes );

    tedImageToolShowWideAbs( it );
    tedImageToolShowHighAbs( it );

    appIntegerToTextWidget( it->itXScaleText, pip->pipScaleXSet );
    appIntegerToTextWidget( it->itYScaleText, pip->pipScaleYSet );
    }

void tedRefreshImageTool(
			    ImageTool *				it,
			    int *				pEnabled,
			    int *				pPref,
			    InspectorSubject *			is,
			    const DocumentSelection *		ds,
			    const SelectionDescription *	sd,
			    BufferDocument *			bd )
    {
    int			part;
    DocumentPosition	dpObject;
    InsertedObject *	io;
    PictureProperties *	pip;

    if  ( ! sd->sdIsObjectSelection )
	{ *pEnabled= 0; return;	}

    if  ( docGetObjectSelection( ds, &part, &dpObject, &io ) )
	{ *pEnabled= 0; return;	}

    it->itPropertiesSet= io->ioPictureProperties;
    it->itPropertiesChosen= io->ioPictureProperties;
    pip= &(it->itPropertiesChosen);

    it->itTotalBytes= io->ioObjectData.mbSize;
    if  ( io->ioKind == DOCokOLEOBJECT			&&
	  io->ioResultKind == DOCokPICTWMETAFILE	)
	{ it->itTotalBytes= io->ioResultData.mbSize; }

    if  ( pip->pipType == DOCokDIBITMAP				||
	  pip->pipType == DOCokWBITMAP				||
	  pip->pipType == DOCokPICTPNGBLIP			||
	  pip->pipType == DOCokPICTJPEGBLIP			||
	  ( pip->pipType == DOCokPICTWMETAFILE &&
				    pip->pipMetafileIsBitmap )	)
	{
	AppBitmapImage *	abi= (AppBitmapImage *)io->ioPrivate;

	if  ( ! abi )
	    { XDEB(abi); *pEnabled= 0; return;	}

	bmCopyDescription( &(it->itBitmapDescription), &(abi->abiBitmap) );
	it->itPictureHasBitmap= 1;
	}
    else{
	bmCleanDescription( &(it->itBitmapDescription) );
	bmInitDescription( &(it->itBitmapDescription) );

	it->itPictureHasBitmap= 0;
	}

    tedFormatToolRefreshImagePage( it );

    it->itImageHighAbsChanged= 0;
    it->itImageWideAbsChanged= 0;

    *pPref += 3;
    *pEnabled= 1;
    return;
    }

/************************************************************************/
/*									*/
/*  Set/Revert buttons have been pushed.				*/
/*									*/
/************************************************************************/

static APP_BUTTON_CALLBACK_H( tedFormatChangeImagePushed, w, voidit )
    {
    ImageTool *			it= (ImageTool *)voidit;
    PictureProperties *		pip= &(it->itPropertiesChosen);

    PropertyMask		pipSetMask;

    int				minValue= 1;
    const int			adaptToMin= 0;
    int				maxValue= 100000;
    const int			adaptToMax= 1;

    PROPmaskCLEAR( &pipSetMask );

    if  ( it->itImageWideAbsChanged )
	{
	if  ( tedImageToolGetWideAbs( it ) )
	    { return;	}
	}
    else{
	if  ( appGetIntegerFromTextWidget( it->itXScaleText,
			    &(pip->pipScaleXSet),
			    minValue, adaptToMin, maxValue, adaptToMax ) )
	    { return;	}
	}

    if  ( it->itImageHighAbsChanged )
	{
	if  ( tedImageToolGetHighAbs( it ) )
	    { return;	}
	}
    else{
	if  ( appGetIntegerFromTextWidget( it->itYScaleText,
			    &(pip->pipScaleYSet),
			    minValue, adaptToMin, maxValue, adaptToMax ) )
	    { return;	}
	}

    if  ( pip->pipScaleXSet != it->itPropertiesSet.pipScaleXSet )
	{ PROPmaskADD( &pipSetMask, PIPpropPICSCALE_X ); }
    if  ( pip->pipScaleYSet != it->itPropertiesSet.pipScaleYSet )
	{ PROPmaskADD( &pipSetMask, PIPpropPICSCALE_Y ); }

    if  ( utilPropMaskIsEmpty( &pipSetMask ) )
	{ return;	}

    tedAppSetImageProperties( it->itApplication, &pipSetMask, pip );

    return;
    }

static APP_BUTTON_CALLBACK_H( tedFormatRevertImagePushed, w, voidit )
    {
    ImageTool *	it= (ImageTool *)voidit;

    it->itPropertiesChosen= it->itPropertiesSet;

    tedFormatToolRefreshImagePage( it );

    return;
    }

/************************************************************************/
/*									*/
/*  1)  User wants to validate the scale.. Get scale and display size.	*/
/*  2)  User wants to set the image size.. Get size  and display scale.	*/
/*  3)  Keep track of whether size or scale was last manipulated.	*/
/*									*/
/************************************************************************/

/*  1  */
static void tedImageToolWideRelToAbs(	ImageTool *	it )
    {
    PictureProperties *		pip= &(it->itPropertiesChosen);

    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;
    const int			minScale= 1;
    const int			adaptToMin= 0;

    if  ( appGetIntegerFromTextWidget( it->itXScaleText,
					    &(pip->pipScaleXSet),
					    minScale, adaptToMin,
					    maxValue, adaptToMax ) )
	{ return;	}

    tedImageToolShowWideAbs( it );

    return;
    }

/*  1  */
static void tedImageToolHighRelToAbs(	ImageTool *	it )
    {
    PictureProperties *		pip= &(it->itPropertiesChosen);

    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;
    const int			minScale= 1;
    const int			adaptToMin= 0;

    if  ( appGetIntegerFromTextWidget( it->itYScaleText,
					    &(pip->pipScaleYSet),
					    minScale, adaptToMin,
					    maxValue, adaptToMax ) )
	{ return;	}

    tedImageToolShowHighAbs( it );

    return;
    }

/*  2  */
static void tedImageToolWideAbsToRel(	ImageTool *	it )
    {
    PictureProperties *		pip= &(it->itPropertiesChosen);

    if  ( tedImageToolGetWideAbs( it ) )
	{ return;	}

    appIntegerToTextWidget( it->itXScaleText, pip->pipScaleXSet );

    return;
    }

/*  2  */
static void tedImageToolHighAbsToRel(	ImageTool *	it )
    {
    PictureProperties *		pip= &(it->itPropertiesChosen);

    if  ( tedImageToolGetHighAbs( it ) )
	{ return;	}

    appIntegerToTextWidget( it->itYScaleText, pip->pipScaleYSet );

    return;
    }

/*  1  */
static APP_TXACTIVATE_CALLBACK_H( tedImageXScaleChanged, w, voidit )
    { tedImageToolWideRelToAbs( (ImageTool *)voidit );	}

/*  1  */
static APP_TXACTIVATE_CALLBACK_H( tedImageYScaleChanged, w, voidit )
    { tedImageToolHighRelToAbs( (ImageTool *)voidit );	}

/*  2  */
static APP_TXACTIVATE_CALLBACK_H( tedImageWideChanged, w, voidit )
    { tedImageToolWideAbsToRel( (ImageTool *)voidit );	}

/*  2  */
static APP_TXACTIVATE_CALLBACK_H( tedImageHighChanged, w, voidit )
    { tedImageToolHighAbsToRel( (ImageTool *)voidit );	}

/*  3  */
static APP_TXTYPING_CALLBACK_H( tedImageToolWideAbsTyped, w, voidit )
    {
    ImageTool *			it= (ImageTool *)voidit;

    it->itImageWideAbsChanged= 1;
    return;
    }

/*  3  */
static APP_TXTYPING_CALLBACK_H( tedImageToolHighAbsTyped, w, voidit )
    {
    ImageTool *			it= (ImageTool *)voidit;

    it->itImageHighAbsChanged= 1;
    return;
    }

/*  3  */
static APP_TXTYPING_CALLBACK_H( tedImageToolWideRelTyped, w, voidit )
    {
    ImageTool *			it= (ImageTool *)voidit;

    it->itImageWideAbsChanged= 0;
    return;
    }

/*  3  */
static APP_TXTYPING_CALLBACK_H( tedImageToolHighRelTyped, w, voidit )
    {
    ImageTool *			it= (ImageTool *)voidit;

    it->itImageHighAbsChanged= 0;
    return;
    }

/************************************************************************/
/*									*/
/*  Make the 'Image' page of the format tool.				*/
/*									*/
/************************************************************************/

void tedFormatFillImagePage(	ImageTool *			it,
				const ImagePageResources *	ipr,
				InspectorSubject *		is,
				APP_WIDGET			pageWidget,
				const InspectorSubjectResources * isr )
    {
    const int	textColumns= 10;
    APP_WIDGET	row;

    /**/
    it->itPageResources= ipr;

    /**/
    docInitPictureProperties( &(it->itPropertiesSet) );
    docInitPictureProperties( &(it->itPropertiesChosen) );
    bmInitDescription( &(it->itBitmapDescription) );
    it->itPictureHasBitmap= 0;

    /**************/
    appMakeLabelAndTextRow( &(it->itPixelSizeRow),
			&(it->itPixelsSizeLabel), &(it->itPixelsSizeText),
			pageWidget, ipr->iprPixelSize, textColumns, 0 );

    appMakeLabelAndTextRow( &(it->itByteSizeRow),
			&(it->itByteSizeLabel), &(it->itByteSizeText),
			pageWidget, ipr->iprTotalBytes, textColumns, 0 );

    appMakeLabelAndTextRow( &(it->itImageWideRow),
			&(it->itImageWideLabel), &(it->itImageWideText),
			pageWidget, ipr->iprImageWide, textColumns, 1 );

    appMakeLabelAndTextRow( &(it->itImageHighRow),
			&(it->itImageHighLabel), &(it->itImageHighText),
			pageWidget, ipr->iprImageHigh, textColumns, 1 );

    appMakeLabelAndTextRow( &(it->itXScaleRow),
			&(it->itXScaleLabel), &(it->itXScaleText),
			pageWidget, ipr->iprXScale, textColumns, 1 );

    appMakeLabelAndTextRow( &(it->itYScaleRow),
			&(it->itYScaleLabel), &(it->itYScaleText),
			pageWidget, ipr->iprYScale, textColumns, 1 );

    appGuiSetGotValueCallbackForText( it->itXScaleText,
					tedImageXScaleChanged, (void *)it );
    appGuiSetGotValueCallbackForText( it->itYScaleText,
					tedImageYScaleChanged, (void *)it );
    appGuiSetTypingCallbackForText( it->itXScaleText,
					tedImageToolWideRelTyped, (void *)it );
    appGuiSetTypingCallbackForText( it->itYScaleText,
					tedImageToolHighRelTyped, (void *)it );

    appGuiSetGotValueCallbackForText( it->itImageWideText,
					tedImageWideChanged, (void *)it );
    appGuiSetGotValueCallbackForText( it->itImageHighText,
					tedImageHighChanged, (void *)it );
    appGuiSetTypingCallbackForText( it->itImageWideText,
					tedImageToolWideAbsTyped, (void *)it );
    appGuiSetTypingCallbackForText( it->itImageHighText,
					tedImageToolHighAbsTyped, (void *)it );

    /**************/

    appInspectorMakeButtonRow( &row, pageWidget,
		&(is->isRevertButton), &(is->isApplyButton),
		isr->isrRevert, isr->isrApplyToSubject,
		tedFormatRevertImagePushed, tedFormatChangeImagePushed,
		(void *)it );

    return;
    }

/************************************************************************/
/*									*/
/*  Clean the image tool.						*/
/*									*/
/************************************************************************/

void tedFormatInitImageTool(	ImageTool *	it )
    {
    docInitPictureProperties( &(it->itPropertiesSet) );
    docInitPictureProperties( &(it->itPropertiesChosen) );
    bmInitDescription( &(it->itBitmapDescription) );

    it->itImageHighAbsChanged= 0;
    it->itImageWideAbsChanged= 0;

    it->itPictureHasBitmap= 0;

    return;
    }

void tedFormatCleanImageTool(	ImageTool *	it )
    {
    bmCleanDescription( &(it->itBitmapDescription) );

    return;
    }

/************************************************************************/
/*									*/
/*  Get Image tool resources.						*/
/*									*/
/************************************************************************/

static AppConfigurableResource TED_TedImageSubjectResourceTable[]=
    {
    APP_RESOURCE( "imageToolImage",
		offsetof(InspectorSubjectResources,isrSubjectName),
		"Image" ),
    APP_RESOURCE( "imageToolChangeImage",
		offsetof(InspectorSubjectResources,isrApplyToSubject),
		"Apply to Image" ),
    APP_RESOURCE( "imageToolRevert",
		offsetof(InspectorSubjectResources,isrRevert),
		"Revert" ),
    /*  no Next Image */
    /*  no Prev Image */
    /*  no Select Image */
    /*  no Delete Image */
    /*  no Insert Image */
    /*  no Append Image */
    };

static AppConfigurableResource TED_TedImageToolResourceTable[]=
    {
    APP_RESOURCE( "imageToolPixelSize",
		    offsetof(ImagePageResources,iprPixelSize),
		    "Pixel Size" ),
    APP_RESOURCE( "imageToolTotalBytes",
		    offsetof(ImagePageResources,iprTotalBytes),
		    "Size in Bytes" ),

    APP_RESOURCE( "imageToolImageWide",
		    offsetof(ImagePageResources,iprImageWide),
		    "Image Wide" ),
    APP_RESOURCE( "imageToolImageHigh",
		    offsetof(ImagePageResources,iprImageHigh),
		    "Image High" ),

    APP_RESOURCE( "imageToolXScale",
		    offsetof(ImagePageResources,iprXScale),
		    "Horizontal Scale (%)" ),
    APP_RESOURCE( "imageToolYScale",
		    offsetof(ImagePageResources,iprYScale),
		    "Vertical Scale (%)" ),
    };

void tedImageToolGetResourceTable(	EditApplication *		ea,
					ImagePageResources *		ipr,
					InspectorSubjectResources *	isr )
    {
    static int	gotToolResources= 0;
    static int	gotSubjectResources= 0;

    if  ( ! gotToolResources )
	{
	appGuiGetResourceValues( &gotToolResources, ea, (void *)ipr,
				TED_TedImageToolResourceTable,
				sizeof(TED_TedImageToolResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    if  ( ! gotSubjectResources )
	{
	appGuiGetResourceValues( &gotSubjectResources, ea, (void *)isr,
				TED_TedImageSubjectResourceTable,
				sizeof(TED_TedImageSubjectResourceTable)/
				sizeof(AppConfigurableResource) );
	}

    return;
    }
