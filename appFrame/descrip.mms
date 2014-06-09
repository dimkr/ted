####
####
####    VMS style 'makefile'. This file might be obsoleted by 
####    the current source. Together with the Unix/Linux makefile.in,
####    this file might be a good starting point for future OpenVMS 
####    ports.
####
####    Author : Jouk Jansen (joukj@hrem.stm.tudelft.nl)
####
####

CC=cc/name=(as_is,short)/float=ieee\
	/include=([],[-.include],[-.appframe],[-.apputil],[-.bitmap],[-.ind])

OBS	=	\
		sioCopyPasteMotif.obj,\
		sioCopyPasteGtk.obj,\
		docExpandedTextAttribute.obj,\
		appZoomMenu.obj,\
		appXvCopyPaste.obj,\
		appWinMetaX11.obj,\
		appWinMetaPs.obj,\
		appWinMetaImg.obj,\
		appWinMeta.obj,\
		appTree.obj,\
		appTextUtil.obj,\
		appTextMotif.obj,\
		appTextGtk.obj,\
		appSymbolPicker.obj,\
		appSpellTool.obj,\
		appScrollDocument.obj,\
		appRuler.obj,\
		appRgbCube.obj,\
		appRgbChooserPage.obj,\
		appReally.obj,\
		appQuestion.obj,\
		appPutImageMotif.obj,\
		appPutImageGtk.obj,\
		appPrintDocument.obj,\
		appPrintDialog.obj,\
		appPaperChooser.obj

OBS1=appPageTool.obj,\
		appPageLayoutTool.obj,\
		appOptionmenuMotif.obj,\
		appOptionmenuGtk.obj,\
		appMetricRuler.obj,\
		appMenuMotif.obj,\
		appMenuGtk.obj,\
		appMenu.obj,\
		appMarginTool.obj,\
		appMain.obj,\
		appMailDialog.obj,\
		appMacPictX11.obj,\
		appMacPictPs.obj,\
		appMacPict.obj,\
		appListMotif.obj,\
		appListGtk.obj,\
		appLabelMotif.obj,\
		appLabelGtk.obj,\
		appInspectorMotif.obj,\
		appInspectorGtk.obj,\
		appInspector.obj,\
		appImage.obj,\
		appIconsMotif.obj,\
		appIconsGtk.obj,\
		appGuiUtil.obj,\
		appGuiMotif.obj,\
		appGuiGtkX.obj,\
		appGuiGtk.obj,\
		appGetImageMotif.obj,\
		appGetImageGtk.obj,\
		appFontTool.obj,\
		appFont.obj,\
		appFindTool.obj,\
		appFileChooserMotif.obj,\
		appFileChooserGtk.obj,\
		appFileChooser.obj,\
		appEventMotif.obj

OBS2=appEventGtk.obj,\
		appEncodingMenu.obj,\
		appDrawnPulldownMotif.obj,\
		appDrawnPulldownGtk.obj,\
		appDrawnPulldown.obj,\
		appDrawPage.obj,\
		appDrawMotif.obj,\
		appDrawGtk.obj,\
		appDocumentMotif.obj,\
		appDocumentGtk.obj,\
		appDocument.obj,\
		appDocMetricRuler.obj,\
		appDialogMotif.obj,\
		appDialogGtk.obj,\
		appCopyPasteMotif.obj,\
		appCopyPasteGtk.obj,\
		appColorMotif.obj,\
		appColorGtk.obj,\
		appColorChooser.obj

all : timestamp.vms [-.lib]appFrame.olb
    write sys$output "all done in [.appframe]"

timestamp.vms : config.h_vms
	 copy config.h_vms appFrameConfig.h
	 open/write file timestamp.vms
	 close file

[-.lib]appFrame.olb :	$(OBS) $(OBS1) $(OBS2)
	library/create appFrame.olb $(OBS)
	library appFrame.olb $(OBS1)
	library appFrame.olb $(OBS2)
	rename appFrame.olb [-.lib]

appColorChooser.obj : \
    appColorChooser.c \
    appFrameConfig.h \
    [-.appframe]appColorChooser.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appDraw.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.bitmap]bmcolor.h \
    appFrame.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appInspector.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appColorGtk.obj : \
    appColorGtk.c \
    appFrameConfig.h \
    appDraw.h \
    [-.appframe]appGuiBase.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.bitmap]bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appColorMotif.obj : \
    appColorMotif.c \
    appFrameConfig.h \
    appDraw.h \
    [-.appframe]appGuiBase.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.bitmap]bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appCopyPasteGtk.obj : \
    appCopyPasteGtk.c \
    appFrameConfig.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appCopyPasteMotif.obj : \
    appCopyPasteMotif.c \
    appFrameConfig.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appDialogGtk.obj : \
    appDialogGtk.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appSystem.h \
    [-.apputil]appGeoString.h

appDialogMotif.obj : \
    appDialogMotif.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appSystem.h \
    [-.apputil]appGeoString.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appDocMetricRuler.obj : \
    appDocMetricRuler.c \
    appFrameConfig.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.appframe]appMetricRuler.h \
    [-.appframe]appRuler.h \
    [-.apputil]appUnit.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appDocument.obj : \
    appDocument.c \
    appFrameConfig.h \
    [-.apputil]appSystem.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appDocumentGtk.obj : \
    appDocumentGtk.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appDocumentMotif.obj : \
    appDocumentMotif.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appDrawGtk.obj : \
    appDrawGtk.c \
    appFrameConfig.h \
    [-.apputil]geo2DInteger.h \
    appDraw.h \
    [-.appframe]appGuiBase.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.bitmap]bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appDrawMotif.obj : \
    appDrawMotif.c \
    appFrameConfig.h \
    [-.apputil]geo2DInteger.h \
    appDraw.h \
    [-.appframe]appGuiBase.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.bitmap]bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appDrawPage.obj : \
    appDrawPage.c \
    appFrameConfig.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appUnit.h \
    [-.apputil]appGeoString.h

appDrawnPulldown.obj : \
    appDrawnPulldown.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appSystem.h \
    [-.apputil]appGeoString.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appDrawnPulldownGtk.obj : \
    appDrawnPulldownGtk.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appSystem.h \
    [-.apputil]appGeoString.h

appDrawnPulldownMotif.obj : \
    appDrawnPulldownMotif.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appSystem.h \
    [-.apputil]appGeoString.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appEncodingMenu.obj : \
    appEncodingMenu.c \
    [-.appframe]appEncodingMenu.h \
    [-.appframe]appFrameConfig.h \
    [-.apputil]psFont.h \
    [-.apputil]docFont.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]utilFontEncoding.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appEventGtk.obj : \
    appEventGtk.c \
    appFrameConfig.h \
    [-.apputil]geo2DInteger.h \
    appDraw.h \
    [-.appframe]appGuiBase.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.bitmap]bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appEventMotif.obj : \
    appEventMotif.c \
    appFrameConfig.h \
    [-.apputil]geo2DInteger.h \
    appDraw.h \
    [-.appframe]appGuiBase.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.bitmap]bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appFileChooser.obj : \
    appFileChooser.c \
    appFrameConfig.h \
    [-.apputil]appSystem.h \
    [-.apputil]sioStdio.h \
    [-.apputil]sioGeneral.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    appFileChooser.h

appFileChooserGtk.obj : \
    appFileChooserGtk.c \
    appFrameConfig.h \
    [-.apputil]appSystem.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    appFileChooser.h

appFileChooserMotif.obj : \
    appFileChooserMotif.c \
    appFrameConfig.h \
    [-.apputil]appSystem.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    appFileChooser.h

appFindTool.obj : \
    appFindTool.c \
    appFrameConfig.h \
    [-.include]reg.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appFont.obj : \
    appFont.c \
    appFrameConfig.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]utilFontmap.h \
    appDraw.h \
    [-.appframe]appGuiBase.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.bitmap]bmcolor.h \
    [-.bitmap]bitmap.h \
    [-.apputil]sioGeneral.h

appFontTool.obj : \
    appFontTool.c \
    appFrameConfig.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    [-.apputil]appUnit.h \
    [-.apputil]appGeoString.h \
    [-.apputil]utilPropMask.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.appframe]appFontTool.h \
    [-.appframe]docExpandedTextAttribute.h \
    [-.appframe]appEncodingMenu.h \
    [-.appframe]appFrameConfig.h \
    [-.appframe]appColorChooser.h \
    appDraw.h

appGetImageGtk.obj : \
    appGetImageGtk.c \
    appFrameConfig.h \
    [-.apputil]utilEndian.h \
    [-.appframe]appImage.h \
    [-.bitmap]bitmap.h \
    [-.apputil]sioGeneral.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appDraw.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appGetImageMotif.obj : \
    appGetImageMotif.c \
    appFrameConfig.h \
    [-.apputil]utilEndian.h \
    [-.appframe]appImage.h \
    [-.bitmap]bitmap.h \
    [-.apputil]sioGeneral.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appDraw.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appGuiGtk.obj : \
    appGuiGtk.c \
    appFrameConfig.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appGuiGtkX.obj : \
    appGuiGtkX.c \
    appFrameConfig.h \
    [-.apputil]appSystem.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appGuiMotif.obj : \
    appGuiMotif.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appSystem.h \
    [-.apputil]appGeoString.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appGuiUtil.obj : \
    appGuiUtil.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appSystem.h \
    [-.apputil]appGeoString.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appIconsGtk.obj : \
    appIconsGtk.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    appIcons.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appIconsMotif.obj : \
    appIconsMotif.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    appIcons.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appImage.obj : \
    appImage.c \
    appFrameConfig.h \
    appImage.h \
    [-.bitmap]bitmap.h \
    [-.apputil]sioGeneral.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appDraw.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]utilPs.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appInspector.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appInspector.obj : \
    appInspector.c \
    appFrameConfig.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.appframe]appRgbChooserPage.h \
    [-.appframe]appRgbCube.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appInspectorGtk.obj : \
    appInspectorGtk.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appSystem.h \
    [-.apputil]appGeoString.h

appInspectorMotif.obj : \
    appInspectorMotif.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appSystem.h \
    [-.apputil]appGeoString.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appLabelGtk.obj : \
    appLabelGtk.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appSystem.h \
    [-.apputil]appGeoString.h

appLabelMotif.obj : \
    appLabelMotif.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appSystem.h \
    [-.apputil]appGeoString.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appListGtk.obj : \
    appListGtk.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appSystem.h \
    [-.apputil]appGeoString.h

appListMotif.obj : \
    appListMotif.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appSystem.h \
    [-.apputil]appGeoString.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appMacPict.obj : \
    appMacPict.c \
    appFrameConfig.h \
    [-.appframe]appMacPict.h \
    [-.bitmap]bitmap.h \
    [-.apputil]sioGeneral.h \
    [-.bitmap]bmcolor.h \
    [-.apputil]docFont.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.appframe]appDraw.h \
    [-.appframe]appGuiBase.h \
    [-.apputil]geo2DInteger.h \
    [-.appframe]appImage.h \
    [-.apputil]utilPs.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]sioEndian.h \
    [-.apputil]utilEndian.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appMacPictPs.obj : \
    appMacPictPs.c \
    appFrameConfig.h \
    [-.appframe]appMacPict.h \
    [-.bitmap]bitmap.h \
    [-.apputil]sioGeneral.h \
    [-.bitmap]bmcolor.h \
    [-.apputil]docFont.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.appframe]appDraw.h \
    [-.appframe]appGuiBase.h \
    [-.apputil]geo2DInteger.h \
    [-.appframe]appImage.h \
    [-.apputil]utilPs.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]sioEndian.h \
    [-.apputil]utilEndian.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appMacPictX11.obj : \
    appMacPictX11.c \
    appFrameConfig.h \
    [-.appframe]appMacPict.h \
    [-.bitmap]bitmap.h \
    [-.apputil]sioGeneral.h \
    [-.bitmap]bmcolor.h \
    [-.apputil]docFont.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.appframe]appDraw.h \
    [-.appframe]appGuiBase.h \
    [-.apputil]geo2DInteger.h \
    [-.appframe]appImage.h \
    [-.apputil]utilPs.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]sioEndian.h \
    [-.apputil]utilEndian.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appMailDialog.obj : \
    appMailDialog.c \
    appFrameConfig.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]sioSmtp.h \
    [-.apputil]appSystem.h

appMain.obj : \
    appMain.c \
    appFrameConfig.h \
    [-.apputil]appSystem.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appUnit.h \
    [-.apputil]appPaper.h \
    [-.apputil]appGeoString.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appMarginTool.obj : \
    appMarginTool.c \
    appFrameConfig.h \
    [-.apputil]utilPropMask.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    appMarginTool.h \
    [-.apputil]appUnit.h \
    [-.apputil]appGeoString.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appMenu.obj : \
    appMenu.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appMenuGtk.obj : \
    appMenuGtk.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appMenuMotif.obj : \
    appMenuMotif.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appMetricRuler.obj : \
    appMetricRuler.c \
    appFrameConfig.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.appframe]appMetricRuler.h \
    [-.appframe]appRuler.h \
    [-.apputil]appUnit.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appOptionmenuGtk.obj : \
    appOptionmenuGtk.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appSystem.h \
    [-.apputil]appGeoString.h

appOptionmenuMotif.obj : \
    appOptionmenuMotif.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appSystem.h \
    [-.apputil]appGeoString.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appPageLayoutTool.obj : \
    appPageLayoutTool.c \
    appFrameConfig.h \
    [-.apputil]appGeoString.h \
    [-.apputil]appUnit.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    appPageLayoutTool.h \
    [-.apputil]appPaper.h \
    [-.appframe]appPaperChooser.h \
    [-.appframe]appFrame.h \
    [-.appframe]appMarginTool.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appPageTool.obj : \
    appPageTool.c \
    appFrameConfig.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appUnit.h \
    [-.apputil]appGeoString.h \
    [-.apputil]appPaper.h \
    appPaperChooser.h \
    appMarginTool.h

appPaperChooser.obj : \
    appPaperChooser.c \
    appFrameConfig.h \
    [-.apputil]appGeoString.h \
    [-.apputil]appPaper.h \
    appPaperChooser.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appPrintDialog.obj : \
    appPrintDialog.c \
    appFrameConfig.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appUnit.h \
    [-.apputil]appGeoString.h \
    appPaperChooser.h \
    [-.apputil]appPaper.h

appPrintDocument.obj : \
    appPrintDocument.c \
    appFrameConfig.h \
    [-.apputil]appSystem.h \
    [-.apputil]sioPipe.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]sioFd.h \
    [-.apputil]sioStdout.h \
    [-.apputil]sioStdio.h \
    [-.apputil]appPaper.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appPutImageGtk.obj : \
    appPutImageGtk.c \
    appFrameConfig.h \
    appImage.h \
    [-.bitmap]bitmap.h \
    [-.apputil]sioGeneral.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appDraw.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appPutImageMotif.obj : \
    appPutImageMotif.c \
    appFrameConfig.h \
    appImage.h \
    [-.bitmap]bitmap.h \
    [-.apputil]sioGeneral.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appDraw.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appQuestion.obj : \
    appQuestion.c \
    appFrameConfig.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appReally.obj : \
    appReally.c \
    appFrameConfig.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appRgbChooserPage.obj : \
    appRgbChooserPage.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    appRgbChooserPage.h \
    [-.appframe]appFrame.h \
    [-.appframe]appRgbCube.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appRgbCube.obj : \
    appRgbCube.c \
    appFrameConfig.h \
    [-.appframe]appRgbCube.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appDraw.h \
    [-.appframe]appGuiBase.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilAffineTransform.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appRuler.obj : \
    appRuler.c \
    appFrameConfig.h \
    [-.apputil]appUnit.h \
    [-.appframe]appRuler.h \
    [-.appframe]appDraw.h \
    [-.appframe]appGuiBase.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.bitmap]bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appScrollDocument.obj : \
    appScrollDocument.c \
    appFrameConfig.h \
    [-.apputil]appSystem.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appSpellTool.obj : \
    appSpellTool.c \
    appFrameConfig.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    appSpellTool.h \
    [-.ind]ind.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appSystem.h \
    [-.include]charnames.h

appSymbolPicker.obj : \
    appSymbolPicker.c \
    appFrameConfig.h \
    [-.appframe]appSymbolPicker.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.appframe]docExpandedTextAttribute.h \
    [-.appframe]appEncodingMenu.h \
    [-.appframe]appFrameConfig.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appTextGtk.obj : \
    appTextGtk.c \
    appFrameConfig.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appSystem.h \
    [-.apputil]appGeoString.h

appTextMotif.obj : \
    appTextMotif.c \
    appFrameConfig.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appSystem.h \
    [-.apputil]appGeoString.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appTextUtil.obj : \
    appTextUtil.c \
    appFrameConfig.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.apputil]appSystem.h \
    [-.apputil]appGeoString.h

appTree.obj : \
    appTree.c \
    appFrameConfig.h \
    appTree.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appWinMeta.obj : \
    appWinMeta.c \
    appFrameConfig.h \
    [-.appframe]appWinMeta.h \
    [-.appframe]appGuiBase.h \
    [-.bitmap]bitmap.h \
    [-.apputil]sioGeneral.h \
    [-.bitmap]bmcolor.h \
    [-.apputil]docFont.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.appframe]appDraw.h \
    [-.apputil]geo2DInteger.h \
    [-.appframe]appImage.h \
    [-.apputil]utilPs.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]sioEndian.h \
    [-.apputil]utilEndian.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appWinMetaImg.obj : \
    appWinMetaImg.c \
    appFrameConfig.h \
    [-.appframe]appWinMeta.h \
    [-.appframe]appGuiBase.h \
    [-.bitmap]bitmap.h \
    [-.apputil]sioGeneral.h \
    [-.bitmap]bmcolor.h \
    [-.apputil]docFont.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.appframe]appDraw.h \
    [-.apputil]geo2DInteger.h \
    [-.appframe]appImage.h \
    [-.apputil]sioEndian.h \
    [-.apputil]utilEndian.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appWinMetaPs.obj : \
    appWinMetaPs.c \
    appFrameConfig.h \
    [-.appframe]appWinMeta.h \
    [-.appframe]appGuiBase.h \
    [-.bitmap]bitmap.h \
    [-.apputil]sioGeneral.h \
    [-.bitmap]bmcolor.h \
    [-.apputil]docFont.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.appframe]appDraw.h \
    [-.apputil]geo2DInteger.h \
    [-.appframe]appImage.h \
    [-.apputil]utilPs.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]sioEndian.h \
    [-.apputil]utilEndian.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appWinMetaX11.obj : \
    appWinMetaX11.c \
    appFrameConfig.h \
    [-.appframe]appWinMeta.h \
    [-.appframe]appGuiBase.h \
    [-.bitmap]bitmap.h \
    [-.apputil]sioGeneral.h \
    [-.bitmap]bmcolor.h \
    [-.apputil]docFont.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.appframe]appDraw.h \
    [-.apputil]geo2DInteger.h \
    [-.appframe]appImage.h \
    [-.apputil]utilPs.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]sioEndian.h \
    [-.apputil]utilEndian.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appXvCopyPaste.obj : \
    appXvCopyPaste.c \
    appFrameConfig.h \
    [-.appframe]sioXprop.h \
    [-.appframe]appGuiBase.h \
    [-.apputil]sioGeneral.h \
    [-.appframe]appImage.h \
    [-.bitmap]bitmap.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appDraw.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appZoomMenu.obj : \
    appZoomMenu.c \
    appFrameConfig.h \
    [-.appframe]appFrame.h \
    [-.appframe]appGuiBase.h \
    [-.appframe]appGuiResource.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.apputil]utilPrinter.h \
    [-.apputil]utilMailContent.h \
    [-.appframe]appIcons.h \
    [-.appframe]appTool.h \
    [-.appframe]appDraw.h \
    [-.bitmap]bmcolor.h \
    [-.appframe]appInspector.h \
    [-.appframe]appZoomMenu.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

docExpandedTextAttribute.obj : \
    docExpandedTextAttribute.c \
    appFrameConfig.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]utilTextAttribute.h \
    [-.appframe]docExpandedTextAttribute.h \
    [-.apputil]docFont.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.bitmap]bmcolor.h

sioCopyPasteGtk.obj : \
    sioCopyPasteGtk.c \
    appFrameConfig.h \
    [-.appframe]sioXprop.h \
    [-.appframe]appGuiBase.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]sioMemory.h \
    [-.apputil]utilMemoryBuffer.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

sioCopyPasteMotif.obj : \
    sioCopyPasteMotif.c \
    appFrameConfig.h \
    [-.appframe]sioXprop.h \
    [-.appframe]appGuiBase.h \
    [-.apputil]sioGeneral.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h
