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
		indword.obj,\
		indutil.obj,\
		indscan.obj,\
		indrenum.obj,\
		indnode.obj,\
		indmini.obj,\
		indlist.obj,\
		indlink.obj,\
		inditem.obj,\
		indind.obj,\
		indguess.obj,\
		indfront.obj,\
		inddeb.obj

all : timestamp.vms [-.lib]ind.olb
    write sys$output "all done in [.ind]"

timestamp.vms : config.h_vms
	 copy config.h_vms indConfig.h
	 open/write file timestamp.vms
	 close file

[-.lib]ind.olb :	$(OBS)
	library/create ind.olb $(OBS)
	rename ind.olb [-.lib]

inddeb.obj : \
    inddeb.c \
    indConfig.h \
    indlocal.h \
    ind.h \
    [-.apputil]utilEndian.h \
    [-.include]appDebugoff.h \
    [-.include]appDebug.h

indfront.obj : \
    indfront.c \
    indConfig.h \
    ind.h \
    indlocal.h \
    [-.apputil]utilEndian.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

indguess.obj : \
    indguess.c \
    indConfig.h \
    indlocal.h \
    ind.h \
    [-.apputil]utilEndian.h \
    [-.include]charnames.h \
    [-.include]appDebugoff.h \
    [-.include]appDebug.h

indind.obj : \
    indind.c \
    indConfig.h \
    indlocal.h \
    ind.h \
    [-.apputil]utilEndian.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

inditem.obj : \
    inditem.c \
    indConfig.h \
    indlocal.h \
    ind.h \
    [-.apputil]utilEndian.h \
    [-.include]appDebugoff.h \
    [-.include]appDebug.h

indlink.obj : \
    indlink.c \
    indConfig.h \
    indlocal.h \
    ind.h \
    [-.apputil]utilEndian.h \
    [-.include]appDebugoff.h \
    [-.include]appDebug.h

indlist.obj : \
    indlist.c \
    indConfig.h \
    ind.h \
    [-.include]appDebugoff.h \
    [-.include]appDebug.h

indmini.obj : \
    indmini.c \
    indConfig.h \
    indlocal.h \
    ind.h \
    [-.apputil]utilEndian.h

indnode.obj : \
    indnode.c \
    indConfig.h \
    indlocal.h \
    ind.h \
    [-.apputil]utilEndian.h \
    [-.include]appDebugoff.h \
    [-.include]appDebug.h

indrenum.obj : \
    indrenum.c \
    indConfig.h \
    indlocal.h \
    ind.h \
    [-.apputil]utilEndian.h \
    [-.include]appDebugoff.h \
    [-.include]appDebug.h

indscan.obj : \
    indscan.c \
    indConfig.h \
    [-.include]charnames.h \
    indlocal.h \
    ind.h \
    [-.apputil]utilEndian.h \
    [-.apputil]utilFontEncoding.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

indutil.obj : \
    indutil.c \
    indConfig.h \
    ind.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

indword.obj : \
    indword.c \
    indConfig.h \
    ind.h \
    indlocal.h \
    [-.apputil]utilEndian.h \
    [-.include]charnames.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h
