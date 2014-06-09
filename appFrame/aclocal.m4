#####################################################################
##
##   Look for gtk headers and libraries.
##
#####################################################################

AC_DEFUN(AC_PATH_GTK,
[
    echo Checking for Gtk etc.

    GTK_CFLAGS=
    GTK_LIBS=

    GTK_HEADERS_FOUND=NO
    GTK_LIBS_FOUND=NO

    # gtk 1.*
    if  ( gtk-config --cflags ) > /dev/null 2>&1
    then
	GTK_CFLAGS=`gtk-config --cflags`
	GTK_HEADERS_FOUND=YES
    fi

    if  ( gtk-config --libs ) > /dev/null 2>&1
    then
	GTK_LIBS=`gtk-config --libs`
	GTK_LIBS_FOUND=YES
    fi

    # gtk 2.0
    if  ( pkg-config --cflags gtk+-2.0 ) > /dev/null 2>&1
    then
	GTK_CFLAGS=`pkg-config --cflags gtk+-2.0`
	GTK_HEADERS_FOUND=YES
    fi

    if  ( pkg-config --libs gtk+-2.0 ) > /dev/null 2>&1
    then
	GTK_LIBS=`pkg-config --libs gtk+-2.0`
	GTK_LIBS_FOUND=YES
    fi


    AC_SUBST(GTK_CFLAGS)dnl
    AC_SUBST(GTK_LIBS)dnl
])
#####################################################################
##
##   Look for libxpm
##
#####################################################################

AC_DEFUN(AC_PATH_XPM,
[
    echo Checking for libxpm...

    XPM_CFLAGS=
    XPM_LIBS=
    XPM_STATIC_REF=
    XPM_SHARED_REF=

    XPM_FOUND=0

    ac_xpm_includes=${ac_xpm_includes:-NO}
    ac_xpm_libraries=${ac_xpm_libraries:-NO}
    ac_xpm_static_lib=NO
    ac_xpm_shared_lib=NO

    if  test $ac_xpm_includes = "NO"
    then
	# Includes
	for ac_dir in			\
	    /usr/X11R6/include		\
	    /usr/X11/include		\
	    /usr/local/include		\
	    /usr/pkg/include		\
	    /usr/local/include/X11R6	\
	    /usr/apps/include		\
	    ../xpm
	do
	if  test -r "$ac_dir/X11/xpm.h"
	    then
		ac_xpm_includes=$ac_dir
		break
	    fi
	done
    fi

    if  test $ac_xpm_libraries = "NO"
    then
	# Libraries
	for ac_dir in				\
	    /usr/X11R6/lib			\
	    /usr/X11/lib			\
	    /usr/local/lib			\
	    /usr/pkg/lib			\
	    /usr/local/lib/X11R6		\
	    /usr/apps/lib			\
	    ../xpm/lib
	do
	found=no

	if  test -r "$ac_dir/libXpm.a"
	    then
		ac_xpm_libraries=$ac_dir
		ac_xpm_static_lib=$ac_dir/libXpm.a
		found=yes
		break
	    fi

	if  test -r "$ac_dir/libXpm.so"
	    then
		ac_xpm_libraries=$ac_dir
		ac_xpm_shared_lib=$ac_dir/libXpm.so
		found=yes
		break
	    fi

	if  test -r "$ac_dir/libXpm.dylib"
	    then
		ac_xpm_libraries=$ac_dir
		ac_xpm_shared_lib=$ac_dir/libXpm.dylib
		found=yes
		break
	    fi

	if  test $found = yes
	    then
		break
	    fi

	done
    fi

    #echo Includes : $ac_xpm_includes
    #echo Libraries: $ac_xpm_libraries

    if  test $ac_xpm_includes = NO
    then
	XPM_FOUND=0
	AC_DEFINE(XPM_FOUND,0)
    else
	XPM_FOUND=1
	AC_DEFINE(XPM_FOUND,1)

	case $ac_xpm_includes in
	    /usr/include)
		;;
	    *)
		XPM_CFLAGS=-I$ac_xpm_includes

		if  test "$XPM_CFLAGS" = "$X_CFLAGS"
		then
		    XPM_CFLAGS=
		fi
		;;
	esac
    fi

    if  test $ac_xpm_libraries != NO
    then
	XPM_LIBS="-L$ac_xpm_libraries"

	if  test "$XPM_LIBS" = "$X_LIBS"
	then
	    XPM_LIBS=
	fi
    fi

    if  test $ac_xpm_static_lib != NO
    then
	XPM_STATIC_REF="$ac_xpm_static_lib"
    else
	XPM_STATIC_REF="$XPM_LIBS -lXpm"
    fi

    XPM_SHARED_REF="$XPM_LIBS -lXpm"

    AC_SUBST(XPM_CFLAGS)dnl
    AC_SUBST(XPM_LIBS)dnl
    AC_SUBST(XPM_STATIC_REF)dnl
    AC_SUBST(XPM_SHARED_REF)dnl
    AC_SUBST(XPM_FOUND)dnl
])
#####################################################################
##
##   Look for Motif
##
#####################################################################

AC_DEFUN(AC_PATH_XM,
[
    echo Checking for Motif...

    XM_CFLAGS=
    XM_LIBS=
    XM_EXTRA_LIBS=
    XM_STATIC_REF=
    XM_SHARED_REF=

    MOTIF_HEADERS_FOUND=NO
    MOTIF_LIBS_FOUND=NO

    ac_xm_includes=${ac_xm_includes:-NO}
    ac_xm_libraries=${ac_xm_libraries:-NO}
    ac_xm_static_lib=NO
    ac_xm_shared_lib=NO

    if  test $ac_xm_includes = NO
    then
	# Includes
	for ac_dir in				\
	    /usr/X11R6/include			\
	    /usr/local/include			\
	    /usr/dt/include			\
	    /opt/SUNWmotif/include		\
	    /usr/include/Motif2.1		\
	    /usr/apps/include			\
	    /usr/pkg/include			\
	    /usr/local/LessTif/include		\
	    /usr/X11R6/LessTif/Motif1.2/include
	do
	if  test -r "$ac_dir/Xm/Xm.h"
	    then
		ac_xm_includes=$ac_dir
		break
	    fi
	done
    fi

    if  test $ac_xm_libraries = NO
    then
	# Libraries
	for ac_dir in				\
	    /usr/X11R6/lib			\
	    /usr/local/lib			\
	    /usr/dt/lib				\
	    /opt/SUNWmotif/lib			\
	    /usr/apps/lib			\
	    /usr/pkg/lib			\
	    /usr/local/LessTif/lib		\
	    /usr/X11R6/LessTif/Motif1.2/lib
	do
	found=no

	if  test -r "$ac_dir/libXm.a"
	    then
		ac_xm_libraries=$ac_dir
		ac_xm_static_lib=$ac_dir/libXm.a
		found=yes
	    fi

	if  test -r "$ac_dir/libXm.so"
	    then
		ac_xm_libraries=$ac_dir
		ac_xm_shared_lib=$ac_dir/libXm.so
		found=yes
	    fi

	if  test -r "$ac_dir/libXm.dylib"
	    then
		ac_xm_libraries=$ac_dir
		ac_xm_shared_lib=$ac_dir/libXm.dylib
		found=yes
	    fi

	if  test $found = yes
	    then
		break
	    fi
	done
    fi

    #echo Includes : $ac_xm_includes
    #echo Libraries: $ac_xm_libraries

    #  Too simple..
    #  AC_CHECK_LIB( Xp, XpStartPage, XM_EXTRA_LIBS="-lXp" )
    if  test -r $x_libraries/libXp.a
    then
	XM_EXTRA_LIBS="-lXp"
    else
	if  test -r $x_libraries/libXp.so
	then
	    XM_EXTRA_LIBS="-lXp"
	fi
    fi

    if  test $ac_xm_includes != NO
    then
	XM_CFLAGS=" -I$ac_xm_includes"
	MOTIF_HEADERS_FOUND=YES

	if  test "$XM_CFLAGS" = "$X_CFLAGS"
	then
	    XM_CFLAGS=
	fi
    fi

    if  test $ac_xm_libraries != NO
    then
	XM_LIBS=" -L$ac_xm_libraries"
	MOTIF_LIBS_FOUND=YES

	if  test "$XM_LIBS" = "$X_LIBS"
	then
	    XM_LIBS=
	fi
    fi

    # Some SGI IRIX versions
    if  test $ac_xm_includes = NO
    then
	if test -f /usr/include/Xm.h
	then
	    XM_CFLAGS=
	    MOTIF_HEADERS_FOUND=YES
	fi
    fi

    # Some SGI IRIX versions
    if  test $ac_xm_libraries = NO
    then
	if test -f /usr/lib/libXm.a
	then
	    XM_LIBS=
	    MOTIF_LIBS_FOUND=YES
	fi

	if test -f /usr/lib/libXm.so
	then
	    XM_LIBS=
	    MOTIF_LIBS_FOUND=YES
	fi
    fi

    if  test $ac_xm_static_lib != NO
    then
	XM_STATIC_REF="$ac_xm_static_lib"
    else
	XM_STATIC_REF="$XM_LIBS -lXm"
    fi

    #  Hack to cope with the linker on some BSD operating systems
    #  I do not think that this hack will be acceptable on the long 
    #  term.
    #  *   The hack is in the wrong position: It is in the Motif 
    #      configuration, whereas it should be in some more generic 
    #      environment.
    #  *   It bypasses the generic autoconf framework. This might 
    #      pose more issues than it removes.
    #

    if  test $ac_xm_libraries != NO
    then
	if  uname -s > /dev/null
	then
	    ac_os_xm=`uname -s`
	    case $ac_os_xm in
		FreeBsd|NetBsd|OpenBsd|FreeBSD|NetBSD|OpenBSD)
		    XM_EXTRA_LIBS="$XM_EXTRA_LIBS -Wl,-R$ac_xm_libraries"
		    XM_EXTRA_LIBS="$XM_EXTRA_LIBS -Wl,-R$x_libraries"
		    ;;
		*)
		    ;;
	    esac
	fi
    fi

    XM_SHARED_REF="$XM_LIBS -lXm"

    AC_SUBST(XM_CFLAGS)dnl
    AC_SUBST(XM_LIBS)dnl
    AC_SUBST(XM_STATIC_REF)dnl
    AC_SUBST(XM_SHARED_REF)dnl
    AC_SUBST(XM_EXTRA_LIBS)dnl
])
#####################################################################
##
##   Give the caller the possibility to interfere with install
##   locations.
##
#####################################################################

AC_DEFUN(AC_INSTALL_LOCATIONS,
[
    dnl Installation locations ..

    AC_PREFIX_DEFAULT("/usr/local")

    if  test x_$prefix = x_NONE
    then
	PREFIX='"'/usr/local'"'
	EXEX_PREFIX='"'/usr/local'"'
	BINDIR='"'/usr/local/bin'"'
    else
	PREFIX='"'$prefix'"'
	EXEX_PREFIX='"'$prefix'"'
	BINDIR='"'$prefix/bin'"'
    fi

    if  test x_$exec_prefix = x_NONE
    then
	: ok
    else
	EXEX_PREFIX='"'$exec_prefix'"'
	BINDIR='"'$exec_prefix/bin'"'
    fi

    case $bindir in
	''|x_NONE)
	    : ok
	    ;;
	\$\{*\}/bin)
	    x_var=`eval echo $bindir`
	    case $x_var in
		NONE/bin)
		    BINDIR='"'/usr/local/bin'"'
		    ;;
		*)
		    BINDIR='"'$x_var'"'
		    ;;
	    esac
	    unset x_var
	    ;;
	*)
	    BINDIR='"'$bindir'"'
	    ;;
    esac

    AC_DEFINE_UNQUOTED( PREFIX, $PREFIX )
    AC_DEFINE_UNQUOTED( EXEC_PREFIX, $EXEC_PREFIX )
    AC_DEFINE_UNQUOTED( BINDIR, $BINDIR )

    AC_SUBST(PREFIX)
    AC_SUBST(EXEC_PREFIX)
    AC_SUBST(BINDIR)
])
#####################################################################
##
##   Choose the GUI thet is used. Relies on settings from the
##   Motif and/or Gtk configuration macros.
##
#####################################################################

AC_DEFUN(AC_CHOOSE_GUI,
[
    echo Choosing a GUI...

    GUI=NONE
    NO_MOTIF=NO
    NO_GTK=NO

    AC_ARG_WITH( MOTIF,
	[  --with-MOTIF            Use Motif/LessTif user interface],
	[
	if  test $withval = yes
	then
	    echo Choosing MOTIF ...

	    if  test	x_$MOTIF_HEADERS_FOUND = x_YES	-a \
			    x_$MOTIF_LIBS_FOUND = x_YES
	    then
		if  test $GUI = NONE
		then
		    : ok
		else
		    echo '############' WARNING '############'
		    echo 'GUI=' $GUI
		fi
	    else
		echo '############' WARNING '############'
		echo 'MOTIF_HEADERS_FOUND=' $MOTIF_HEADERS_FOUND
		echo 'MOTIF_LIBS_FOUND=' $MOTIF_HEADERS_FOUND
	    fi
	    
	    GUI=MOTIF
	else
	    NO_MOTIF=YES
	fi
	]
    )

    AC_ARG_WITH( GTK,
	[  --with-GTK              Use Gtk+ user interface ],
	[
	if  test $withval = yes
	then
	    echo Choosing GTK ...

	    if  test	x_$GTK_HEADERS_FOUND = x_YES	-a \
			    x_$GTK_LIBS_FOUND = x_YES
	    then
		if  test $GUI = NONE
		then
		    : ok
		else
		    echo '############' WARNING '############'
		    echo 'GUI=' $GUI
		fi
	    else
		echo '############' WARNING '############'
		echo 'GTK_HEADERS_FOUND=' $GTK_HEADERS_FOUND
		echo 'GTK_LIBS_FOUND=' $GTK_HEADERS_FOUND
	    fi
	    
	    GUI=GTK
	else
	    NO_GTK=YES
	fi
	]
    )

    if  test	$GUI = NONE				-a \
		$NO_MOTIF = NO				-a \
		x_$MOTIF_HEADERS_FOUND = x_YES		-a \
		x_$MOTIF_LIBS_FOUND = x_YES
    then
	echo Found MOTIF
	GUI=MOTIF
    fi

    if  test	$GUI = NONE				-a \
		$NO_GTK = NO				-a \
		x_$GTK_HEADERS_FOUND = x_YES		-a \
		x_$GTK_LIBS_FOUND = x_YES
    then
	echo Found GTK
	GUI=GTK
    fi

    case $GUI in
    MOTIF)
	AC_SUBST(GUI)
	AC_DEFINE( USE_MOTIF )
	;;
    GTK)
	AC_SUBST(GUI)
	AC_DEFINE( USE_GTK )
	;;
    *)
	echo '############' WARNING '############'
	echo 'GUI=' $GUI
	;;
    esac
])
