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
		bmxwd.obj,\
		bmxv.obj,\
		bmxpm.obj,\
		bmxbm.obj,\
		bmtogray.obj,\
		bmtiff.obj,\
		bmthreshold.obj,\
		bmsqueeze.obj,\
		bmsgi.obj,\
		bmselect.obj,\
		bmreduce.obj,\
		bmputrow.obj,\
		bmpng.obj,\
		bmpgm.obj,\
		bmpcx.obj,\
		bmpalette.obj,\
		bmmorpho.obj,\
		bmmapcolors.obj,\
		bmjpeg.obj,\
		bmio.obj,\
		bminvert.obj,\
		bmgif.obj,\
		bmgetrow.obj,\
		bmgamma.obj,\
		bmformats.obj,\
		bmflip.obj,\
		bmfilter.obj,\
		bmeps.obj,\
		bmdraw.obj,\
		bmconvert.obj,\
		bmcolor.obj,\
		bmbmp.obj,\
		bmalpha.obj,\
		bmadm.obj,\
		bm_egif_lib.obj,\
		bm_dgif_lib.obj,\
		bmTextureMap.obj,\
		bmSegments.obj,\
		bmMakeImage.obj,\
		bmForAll.obj,\
		bmFindLineSegments.obj,\
		bmFillJob.obj,\
		bmDrawImage.obj,\
		bmClusterSegments.obj

all : timestamp.vms [-.lib]bitmap.olb
    write sys$output "all done in [.bitmap]"

timestamp.vms : config.h_vms
	 copy config.h_vms bitmapConfig.h
	 open/write file timestamp.vms
	 close file

[-.lib]bitmap.olb :	$(OBS)
	library/create bitmap.olb $(OBS)
	rename bitmap.olb [-.lib]

bmClusterSegments.obj : \
    bmClusterSegments.c \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmDrawImage.obj : \
    bmDrawImage.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmFillJob.obj : \
    bmFillJob.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    bmgetrow.h \
    bmputrow.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmFindLineSegments.obj : \
    bmFindLineSegments.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmForAll.obj : \
    bmForAll.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmMakeImage.obj : \
    bmMakeImage.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmSegments.obj : \
    bmSegments.c \
    bitmapConfig.h \
    bmSegments.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    bmintern.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmTextureMap.obj : \
    bmTextureMap.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    bmgetrow.h \
    bmputrow.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bm_dgif_lib.obj : \
    bm_dgif_lib.c \
    bm_gif_lib.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.apputil]sioBlocked.h \
    [-.apputil]sioLzw.h \
    [-.apputil]sioEndian.h \
    [-.apputil]utilEndian.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bm_egif_lib.obj : \
    bm_egif_lib.c \
    bm_gif_lib.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    [-.apputil]sioBlocked.h \
    [-.apputil]sioLzw.h \
    [-.apputil]sioEndian.h \
    [-.apputil]utilEndian.h

bmadm.obj : \
    bmadm.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmalpha.obj : \
    bmalpha.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmbmp.obj : \
    bmbmp.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.apputil]sioHex.h \
    [-.apputil]sioEndian.h \
    [-.apputil]utilEndian.h \
    [-.apputil]sioStdio.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmcolor.obj : \
    bmcolor.c \
    bitmapConfig.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmconvert.obj : \
    bmconvert.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    bmgetrow.h \
    bmputrow.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmdraw.obj : \
    bmdraw.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmeps.obj : \
    bmeps.c \
    bitmapConfig.h \
    [-.apputil]sioStdio.h \
    [-.apputil]sioGeneral.h \
    [-.apputil]sioHex.h \
    [-.apputil]sioBase85.h \
    [-.apputil]sioFlate.h \
    [-.apputil]sioLzw.h \
    bmintern.h \
    bitmap.h \
    bmcolor.h \
    [-.apputil]utilPs.h \
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

bmfilter.obj : \
    bmfilter.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    bmgetrow.h \
    bmputrow.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmflip.obj : \
    bmflip.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmformats.obj : \
    bmformats.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmgamma.obj : \
    bmgamma.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmgetrow.obj : \
    bmgetrow.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    bmgetrow.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmgif.obj : \
    bmgif.c \
    bitmapConfig.h \
    [-.apputil]sioStdio.h \
    [-.apputil]sioGeneral.h \
    bmintern.h \
    bitmap.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    bm_gif_lib.h

bminvert.obj : \
    bminvert.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmio.obj : \
    bmio.c \
    bitmapConfig.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmjpeg.obj : \
    bmjpeg.c \
    bitmapConfig.h \
    [-.apputil]sioStdio.h \
    [-.apputil]sioGeneral.h \
    bmintern.h \
    bitmap.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmmapcolors.obj : \
    bmmapcolors.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmmorpho.obj : \
    bmmorpho.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmpalette.obj : \
    bmpalette.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmpcx.obj : \
    bmpcx.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.apputil]utilEndian.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmpgm.obj : \
    bmpgm.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmpng.obj : \
    bmpng.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    [-.apputil]sioStdio.h \
    [-.apputil]utilEndian.h

bmputrow.obj : \
    bmputrow.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    bmputrow.h \
    bmgetrow.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmreduce.obj : \
    bmreduce.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmselect.obj : \
    bmselect.c \
    bitmapConfig.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmsgi.obj : \
    bmsgi.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.apputil]utilEndian.h \
    [-.apputil]sioStdio.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmsqueeze.obj : \
    bmsqueeze.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.apputil]sioHex.h \
    [-.apputil]sioEndian.h \
    [-.apputil]utilEndian.h \
    [-.apputil]sioStdio.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmthreshold.obj : \
    bmthreshold.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmtiff.obj : \
    bmtiff.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmtogray.obj : \
    bmtogray.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmxbm.obj : \
    bmxbm.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmxpm.obj : \
    bmxpm.c \
    bitmapConfig.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h

bmxv.obj : \
    bmxv.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.apputil]sioEndian.h \
    [-.apputil]utilEndian.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

bmxwd.obj : \
    bmxwd.c \
    bitmapConfig.h \
    bmintern.h \
    bitmap.h \
    [-.apputil]sioGeneral.h \
    bmcolor.h \
    [-.apputil]utilEndian.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h \
    XWDFile.h
