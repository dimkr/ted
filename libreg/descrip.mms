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
		regsub.obj,\
		regexp.obj

all : timestamp.vms [-.lib]libreg.olb
    write sys$output "all done in [.libreg]"

timestamp.vms : config.h_vms
	 copy config.h_vms libregConfig.h
	 open/write file timestamp.vms
	 close file

[-.lib]libreg.olb :	$(OBS)
	library/create libreg.olb $(OBS)
	rename libreg.olb [-.lib]

regexp.obj : \
    regexp.c \
    libregConfig.h \
    [-.include]reg.h \
    regmagic.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h

regsub.obj : \
    regsub.c \
    libregConfig.h \
    [-.include]reg.h \
    regmagic.h \
    [-.include]appDebugon.h \
    [-.include]appDebug.h
