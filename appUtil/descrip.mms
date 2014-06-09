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
		utilTree.obj,\
		utilTextAttributeAdmin.obj,\
		utilTextAttribute.obj,\
		utilPs.obj,\
		utilPropMask.obj,\
		utilPrinter.obj,\
		utilPostscriptFontCatalog.obj,\
		utilPostscriptFace.obj,\
		utilNup.obj,\
		utilMemoryBuffer.obj,\
		utilMatchFont.obj,\
		utilMapFontEncoding.obj,\
		utilMD5.obj,\
		utilJenkinsPerfectHash.obj,\
		utilJenkinsHash.obj,\
		utilFontmap.obj,\
		utilFontEncoding.obj,\
		utilFileName.obj,\
		utilEndian.obj,\
		utilDocumentGeometry.obj,\
		utilDate.obj,\
		utilCgiEcho.obj,\
		utilBase85.obj,\
		utilBase64.obj,\
		utilAffineTransform.obj,\
		sioStdout.obj,\
		sioStdio.obj,\
		sioSmtp.obj,\
		sioQuotedPrintable.obj,\
		sioPrintf.obj

OBS1=sioPipe.obj,\
		sioPfb.obj,\
		sioMsLz77.obj,\
		sioMemory.obj,\
		sioLzw.obj,\
		sioHttp.obj,\
		sioHex.obj,\
		sioGeneral.obj,\
		sioFork.obj,\
		sioFlate.obj,\
		sioFd.obj,\
		sioEndian.obj,\
		sioDebug.obj,\
		sioCgiIn.obj,\
		sioBlocked.obj,\
		sioBase85.obj,\
		sioBase64.obj,\
		psGlyphs.obj,\
		psFont.obj,\
		geo2DInteger.obj,\
		docFont.obj,\
		appUrl.obj,\
		appUnit.obj,\
		appTagval.obj,\
		appSystem.obj,\
		appPaper.obj,\
		appGeoString.obj,\
		appGeo.obj,\
		appDebug.obj,\
		appCgiIn.obj

all : timestamp.vms [-.lib]apputil.olb
    write sys$output "all done in [.apputil]"

timestamp.vms : config.h_vms
	 copy config.h_vms appUtilConfig.h
	 open/write file timestamp.vms
	 close file

[-.lib]apputil.olb :	$(OBS) $(OBS1)
	library/create apputil.olb $(OBS)
	library apputil.olb $(OBS1)
	rename apputil.olb [-.lib]

appCgiIn.obj : \
    appCgiIn.c \
    appUtilConfig.h \
    [-.apputil]appCgiIn.h \
    [-.apputil]appTagval.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appDebug.obj : \
    appDebug.c \
    appUtilConfig.h \
    [-.include]appDebug.h

appGeo.obj : \
    appGeo.c \
    appUtilConfig.h \
    geo2DInteger.h

appGeoString.obj : \
    appGeoString.c \
    appUtilConfig.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    appUnit.h \
    appGeoString.h

appPaper.obj : \
    appPaper.c \
    appUtilConfig.h \
    appPaper.h \
    appGeoString.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appSystem.obj : \
    appSystem.c \
    appUtilConfig.h \
    [-.apputil]appSystem.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appTagval.obj : \
    appTagval.c \
    appUtilConfig.h \
    [-.apputil]appTagval.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appUnit.obj : \
    appUnit.c \
    appUtilConfig.h \
    appUnit.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

appUrl.obj : \
    appUrl.c \
    appUtilConfig.h \
    [-.apputil]appUrl.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

docFont.obj : \
    docFont.c \
    appUtilConfig.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    utilPropMask.h \
    docFont.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]utilTextAttribute.h \
    utilFontEncoding.h

geo2DInteger.obj : \
    geo2DInteger.c \
    appUtilConfig.h \
    geo2DInteger.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

psFont.obj : \
    psFont.c \
    appUtilConfig.h \
    docFont.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]utilTextAttribute.h \
    psFont.h \
    [-.apputil]docFont.h \
    [-.apputil]utilFontEncoding.h \
    appSystem.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

psGlyphs.obj : \
    psGlyphs.c \
    appUtilConfig.h \
    psFont.h \
    [-.apputil]docFont.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]utilFontEncoding.h \
    [-.include]charnames.h

sioBase64.obj : \
    sioBase64.c \
    appUtilConfig.h \
    sioBase64.h \
    [-.apputil]sioGeneral.h \
    utilBase64.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

sioBase85.obj : \
    sioBase85.c \
    appUtilConfig.h \
    sioBase85.h \
    [-.apputil]sioGeneral.h \
    utilBase85.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

sioBlocked.obj : \
    sioBlocked.c \
    appUtilConfig.h \
    sioBlocked.h \
    [-.apputil]sioGeneral.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

sioCgiIn.obj : \
    sioCgiIn.c \
    appUtilConfig.h \
    [-.apputil]sioCgiIn.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]appCgiIn.h \
    [-.apputil]appTagval.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

sioDebug.obj : \
    sioDebug.c \
    appUtilConfig.h \
    sioDebug.h \
    [-.apputil]sioGeneral.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

sioEndian.obj : \
    sioEndian.c \
    appUtilConfig.h \
    [-.apputil]sioEndian.h \
    [-.apputil]utilEndian.h \
    [-.apputil]sioGeneral.h

sioFd.obj : \
    sioFd.c \
    appUtilConfig.h \
    [-.apputil]sioFd.h \
    [-.apputil]sioGeneral.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

sioFlate.obj : \
    sioFlate.c \
    appUtilConfig.h \
    sioFlate.h \
    [-.apputil]sioGeneral.h \
    sioEndian.h \
    [-.apputil]utilEndian.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

sioFork.obj : \
    sioFork.c \
    appUtilConfig.h \
    sioFork.h \
    [-.apputil]sioGeneral.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

sioGeneral.obj : \
    sioGeneral.c \
    appUtilConfig.h \
    sioGeneral.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

sioHex.obj : \
    sioHex.c \
    appUtilConfig.h \
    sioHex.h \
    [-.apputil]sioGeneral.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

sioHttp.obj : \
    sioHttp.c \
    appUtilConfig.h \
    [-.apputil]sioHttp.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]appSystem.h \
    [-.apputil]utilMemoryBuffer.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

sioLzw.obj : \
    sioLzw.c \
    appUtilConfig.h \
    sioLzw.h \
    [-.apputil]sioGeneral.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

sioMemory.obj : \
    sioMemory.c \
    appUtilConfig.h \
    [-.apputil]sioMemory.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]utilMemoryBuffer.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

sioMsLz77.obj : \
    sioMsLz77.c \
    appUtilConfig.h \
    sioMsLz77.h \
    [-.apputil]sioGeneral.h \
    sioEndian.h \
    [-.apputil]utilEndian.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

sioPfb.obj : \
    sioPfb.c \
    appUtilConfig.h \
    sioPfb.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]sioEndian.h \
    [-.apputil]utilEndian.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

sioPipe.obj : \
    sioPipe.c \
    appUtilConfig.h \
    [-.apputil]sioPipe.h \
    [-.apputil]appSystem.h \
    [-.apputil]sioGeneral.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

sioPrintf.obj : \
    sioPrintf.c \
    appUtilConfig.h \
    sioGeneral.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

sioQuotedPrintable.obj : \
    sioQuotedPrintable.c \
    appUtilConfig.h \
    sioQuotedPrintable.h \
    [-.apputil]sioGeneral.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

sioSmtp.obj : \
    sioSmtp.c \
    appUtilConfig.h \
    [-.apputil]sioSmtp.h \
    [-.apputil]appSystem.h \
    [-.apputil]sioGeneral.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

sioStdio.obj : \
    sioStdio.c \
    appUtilConfig.h \
    [-.apputil]sioStdio.h \
    [-.apputil]sioGeneral.h \
    [-.include]appDebugoff.h \
    [-.include]appDebug.h

sioStdout.obj : \
    sioStdout.c \
    appUtilConfig.h \
    [-.apputil]sioStdout.h \
    [-.apputil]sioGeneral.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

utilAffineTransform.obj : \
    utilAffineTransform.c \
    utilAffineTransform.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

utilBase64.obj : \
    utilBase64.c \
    utilBase64.h

utilBase85.obj : \
    utilBase85.c \
    utilBase85.h

utilCgiEcho.obj : \
    utilCgiEcho.c \
    [-.apputil]utilCgiEcho.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]appTagval.h \
    [-.apputil]appCgiIn.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

utilDate.obj : \
    utilDate.c \
    [-.apputil]utilDate.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

utilDocumentGeometry.obj : \
    utilDocumentGeometry.c \
    appUtilConfig.h \
    utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    utilPropMask.h

utilEndian.obj : \
    utilEndian.c \
    appUtilConfig.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    utilEndian.h

utilFileName.obj : \
    utilFileName.c \
    appUtilConfig.h \
    [-.apputil]appSystem.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

utilFontEncoding.obj : \
    utilFontEncoding.c \
    appUtilConfig.h \
    utilFontEncoding.h \
    [-.include]charnames.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

utilFontmap.obj : \
    utilFontmap.c \
    [-.apputil]appUtilConfig.h \
    [-.apputil]utilFontmap.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

utilJenkinsHash.obj : \
    utilJenkinsHash.c \
    appUtilConfig.h \
    [-.apputil]utilJenkinsHash.h

utilJenkinsPerfectHash.obj : \
    utilJenkinsPerfectHash.c \
    appUtilConfig.h \
    utilJenkinsHash.h \
    utilJenkinsPerfectHash.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

utilMD5.obj : \
    utilMD5.c \
    utilMD5.h \
    [-.apputil]appUtilConfig.h \
    utilBase64.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

utilMapFontEncoding.obj : \
    utilMapFontEncoding.c \
    appUtilConfig.h \
    psFont.h \
    [-.apputil]docFont.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]utilFontEncoding.h \
    [-.include]charnames.h

utilMatchFont.obj : \
    utilMatchFont.c \
    appUtilConfig.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    utilPropMask.h \
    docFont.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]utilTextAttribute.h \
    psFont.h \
    [-.apputil]docFont.h \
    [-.apputil]utilFontEncoding.h

utilMemoryBuffer.obj : \
    utilMemoryBuffer.c \
    appUtilConfig.h \
    [-.apputil]utilMemoryBuffer.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

utilNup.obj : \
    utilNup.c \
    appUtilConfig.h \
    [-.apputil]utilNup.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]utilAffineTransform.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

utilPostscriptFace.obj : \
    utilPostscriptFace.c \
    appUtilConfig.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]docFont.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

utilPostscriptFontCatalog.obj : \
    utilPostscriptFontCatalog.c \
    appUtilConfig.h \
    docFont.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]utilTextAttribute.h \
    psFont.h \
    [-.apputil]docFont.h \
    [-.apputil]utilFontEncoding.h \
    appSystem.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

utilPrinter.obj : \
    utilPrinter.c \
    appUtilConfig.h \
    utilPrinter.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

utilPropMask.obj : \
    utilPropMask.c \
    [-.apputil]utilPropMask.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

utilPs.obj : \
    utilPs.c \
    appUtilConfig.h \
    [-.apputil]utilPs.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]geo2DInteger.h \
    [-.apputil]utilDocumentGeometry.h \
    [-.apputil]utilPropMask.h \
    [-.apputil]docFont.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]psFont.h \
    [-.apputil]utilFontEncoding.h \
    [-.apputil]utilPostscriptFace.h \
    [-.apputil]utilAffineTransform.h \
    [-.apputil]utilNup.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

utilTextAttribute.obj : \
    utilTextAttribute.c \
    appUtilConfig.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    utilPropMask.h \
    utilTextAttribute.h \
    [-.apputil]utilPropMask.h

utilTextAttributeAdmin.obj : \
    utilTextAttributeAdmin.c \
    appUtilConfig.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    utilTextAttributeAdmin.h \
    [-.apputil]utilTextAttribute.h \
    [-.apputil]utilPropMask.h

utilTree.obj : \
    utilTree.c \
    utilTree.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h
