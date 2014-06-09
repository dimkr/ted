#>---------------------------------------------------------------------------<
#
#  Compilation on VMS : type
#	    $ MMS
#	    in the main directory.
#
#  NOTE that some of the descrip.mms files  in subdirectories might 
#	    be out of sync with the C source: Compare them with the 
#	    makefile.in files and update the lists of file names and 
#	    the file dependencies.
#
#  Installation on VMS : choose a directory where to install the Ted
#	    distribution (i.e. disk:[editors.ted] )
#
#	    define the logical TED$ROOT as follows
#	       $ DEFINE/JOB/TRANS=(CONCEALED) TED$ROOT DISK:[EDITORS.TED.]
#	    Note that the . before the ] is essential!!!
#
#	    copy the file [.TEDPACKAGE]TEDBINDIST.TAR to
#	    TED$ROOT:[000000]
#	    Untar this file and set read-acess as required to all
#	    the resulting files
#
#	    define a "foreign" symbol 
#	       ted :== $disk:[editors.ted_distribution.ted]ted
#		       
#  Notes about the VMS version VMS : This version was tested with
#	    OpenVMS Alpha 7.3
#	    Decwindows 1.2-6
#	    Compaq C V6.5-001
#
#>---------------------------------------------------------------------------<
#
#  Jouk Jansen
#		 
#  joukj@hrem.stm.tudelft.nl
#
#  Technische Universiteit Delft        tttttttttt  uu     uu  ddddddd
#  Nationaal centrum voor HREM          tttttttttt  uu     uu  dd    dd
#  Rotterdamseweg 137                       tt      uu     uu  dd     dd
#  2628 AL Delft                            tt      uu     uu  dd     dd
#  Nederland                                tt      uu     uu  dd    dd
#  tel. 31-15-2781536                       tt       uuuuuuu   ddddddd
#
#>---------------------------------------------------------------------------<

all :
	if f$search("lib.dir") .eqs. "" then create/directory [.lib]
	set default [.appframe]
	$(MMS)$(MMSQUALIFIERS)
	set default [-.apputil]
	$(MMS)$(MMSQUALIFIERS)
	set default [-.bitmap]
	$(MMS)$(MMSQUALIFIERS)
	set default [-.ind]
	$(MMS)$(MMSQUALIFIERS)
	set default [-.libreg]
	$(MMS)$(MMSQUALIFIERS)
	set default [-.ted]
	$(MMS)$(MMSQUALIFIERS)
