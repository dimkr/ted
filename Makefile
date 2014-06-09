####
####	Makefile for Ted
####	================
####
####	No explicit 'configure' is needed as it is done by the 'make' process
####	for the various targets.
####
####	Targets for external use are:
####	-----------------------------
####
####	compile:	Just build an executable in the 'Ted' directory.
####	private:	Install the Ted executable and the most essential
####			files for yourself only.
####	package:	Build a package. Must sometimes be run as root.
####	sysvpkg:	Build a Unix system V (Solaris) package. 
####	install:	Install the package. Must be run as root.
####	clean:		Cleanup rubbish from previous attempts.
####
####	compile.shared:	Compile, do not try to link statically.
####	package.shared:	Build a package, using the dynamically linked
####			executable.
####	install.shared:	Install the package. Must be run as root,
####			or another user with sufficient privileges to 
####			create the directories and files.
####			Installs the package just made, does not force
####			the package to be dynamically linked. This target
####			exists only for convenience.
####
####	Actively supported 'configure' options are:
####	-------------------------------------------
####
####		--prefix		Use another place than /usr/local
####					for installation. E.G.
####					--prefix=/opt/Ted
####		--with-MOTIF		Use the Motif gui toolkit. (The
####					default)
####		--with-GTK		Use the Gtk+ gui toolkit. (Still
####					experimental)
####
####
####
####	P.S.	To port to Compaq OpenVMS, use the descrip.mms file
####		It contains an explanatory note as well.
####
####	P.S.	I admit that this makefile is more like a shell script.
####

CONFIGURE_OPTIONS=--with-MOTIF

compile:	tedlibs		\
		Ted/Ted		\
		Ted/Ted.static
	:
	: Static executable Ted/Ted.static ready
	: *   To make an installation package
	:     you can now run 'make package'. Depending on the platform, 
	:     it might be that this must be done AS ROOT.
	: *   To install Ted for yourself only
	:     you can now run 'make private'

compile.shared:	tedlibs		\
		Ted/Ted
	:
	: Dynamic executable Ted/Ted ready
	: *   To make an installation package
	:     you can now run 'make package.shared' AS ROOT
	: *   To install Ted for yourself only
	:     you can now run 'make private'

####
####	Make library directory.
####

lib:
	mkdir lib

####
####	Build ted libraries
####

tedlibs: 	lib		\
		lib/bitmap.a	\
		lib/ind.a	\
		lib/reg.a	\
		lib/appUtil.a	\
		lib/appFrame.a

####
####	Compile the bitmap manipulation library
####

lib/bitmap.a: bitmap/makefile
	cd bitmap && $(MAKE)

bitmap/makefile: bitmap/makefile.in Makefile
	cd bitmap && ./configure $(CONFIGURE_OPTIONS)

####
####	Compile the spell checker library
####

lib/ind.a: ind/makefile
	cd ind && $(MAKE)

ind/makefile: ind/makefile.in Makefile
	cd ind && ./configure $(CONFIGURE_OPTIONS)

####
####	Compile the regular expression library
####

lib/reg.a: libreg/makefile
	cd libreg && $(MAKE)

libreg/makefile: libreg/makefile.in Makefile
	cd libreg && ./configure $(CONFIGURE_OPTIONS)

####
####	Compile the application utility library
####

lib/appUtil.a: appUtil/makefile
	cd appUtil && $(MAKE)

appUtil/makefile: appUtil/makefile.in Makefile
	cd appUtil && ./configure $(CONFIGURE_OPTIONS)

####
####	Compile the application framework library
####

lib/appFrame.a: appFrame/makefile
	cd appFrame && $(MAKE)

appFrame/makefile: appFrame/makefile.in Makefile
	cd appFrame && ./configure $(CONFIGURE_OPTIONS)

####
####	Compile and link Ted
####

Ted/Ted.static: tedlibs Ted/makefile
	cd Ted && $(MAKE) Ted.static

Ted/Ted: tedlibs Ted/makefile
	cd Ted && $(MAKE)

Ted/makefile: Ted/makefile.in Makefile
	cd Ted && ./configure $(CONFIGURE_OPTIONS)

####
####	Make a ready to install package
####	Must be run as root
####

package: tedPackage/makefile compile
	cd tedPackage && $(MAKE)
	:
	: Package ready.
	: To install Ted, you can now run 'make install' AS ROOT

package.shared: tedPackage/makefile compile.shared
	cd tedPackage && make package.shared
	:
	: Dynamically linked package ready.
	: To install Ted, you can now run 'make install' AS ROOT

tedPackage/makefile: tedPackage/makefile.in Makefile
	cd tedPackage && ./configure $(CONFIGURE_OPTIONS)

sysvpkg: tedPackage/makefile compile
	cd tedPackage && $(MAKE) sysvpkg
	:
	: UNIX system V Package ready.

####
####	Install Ted from the package just built
####

install: package
	sh ./tedPackage/installTed.sh

install.shared: package.shared
	sh ./tedPackage/installTed.sh

####
####	Install Ted for yourself
####

PRIVATE_FILES=\
	$(HOME)/afm/Helvetica.afm		\
	$(HOME)/afm/Courier.afm			\
	$(HOME)/afm/Symbol.afm			\
	$(HOME)/afm/Times-Roman.afm		\
	$(HOME)/Ted/TedDocument-en_US.rtf	\
	$(HOME)/Ted/Ted.ad.sample		\
	$(HOME)/ind/US_English.ind

$(HOME)/bin/Ted: Ted/Ted
	test -d $(HOME)/bin || mkdir $(HOME)/bin
	cp Ted/Ted.static $(HOME)/bin/Ted || cp Ted/Ted $(HOME)/bin/Ted

$(PRIVATE_FILES):
	h=`pwd` && cd && tar xvf $$h/tedPackage/TedBindist.tar

private: $(HOME)/bin/Ted $(PRIVATE_FILES)
	@echo ========= Updating $(HOME)/.Ted.properties:
	@echo Ted.documentFileName: $(HOME)/Ted/TedDocument-en_US.rtf | \
					    tee -a $(HOME)/.Ted.properties
	@echo Ted.afmDirectory: $(HOME)/afm | \
					    tee -a $(HOME)/.Ted.properties
	@echo Ted.spellToolSystemDicts: $(HOME)/ind | \
					    tee -a $(HOME)/.Ted.properties

####
####	Cleanup
####

clean:
	rm -fr	lib
	rm -f	*/config.cache */config.log */config.status
	rm -f	appUtil/appUtilConfig.h
	rm -f	appFrame/appFrameConfig.h
	rm -f	Ted/tedConfig.h
	rm -f	ind/indConfig.h
	rm -f	bitmap/bitmapConfig.h
	rm -f	*/makefile
	rm -f	*/*.o
	rm -f	Ted/Ted Ted/Ted.static
	rm -fr	tedPackage/scratch
	rm -f	tedPackage/*.gz
	rm -f	tedPackage/*.lsm
	rm -f	tedPackage/installTed.sh
	rm -f	tedPackage/README
	rm -f	tedPackage/package
	rm -f	tedPackage/package.shared

