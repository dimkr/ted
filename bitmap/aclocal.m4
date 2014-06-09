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
##   Look for libpng
##
#####################################################################

AC_DEFUN(AC_PATH_PNG,
[
    echo Checking for libpng...

    PNG_CFLAGS=
    PNG_LIBS=
    PNG_FOUND=0
    PNG_STATIC_REF=
    PNG_SHARED_REF=

    ac_png_includes=${ac_png_includes:-NO}
    ac_png_libraries=${ac_png_libraries:-NO}
    ac_png_static_lib=NO
    ac_png_shared_lib=NO

    if  test $ac_png_includes = NO
    then
	# Includes
	for ac_dir in			\
	    /usr/include		\
	    /usr/local/include		\
	    /usr/pkg/include		\
	    /usr/X11R6/include		\
	    /usr/local/include/png	\
	    /usr/local/include/libpng	\
	    /usr/apps/include		\
	    /usr/sfw/include		\
	    /opt/sfw/include		\
	    ../libpng
	do
	if  test -r "$ac_dir/png.h"
	    then
		ac_png_includes=$ac_dir
		break
	    fi
	done
    fi

    if  test $ac_png_libraries = NO
    then
	# Libraries
	for ac_dir in			\
	    /usr/lib			\
	    /usr/local/lib		\
	    /usr/pkg/lib		\
	    /usr/X11R6/lib		\
	    /usr/local/lib/png		\
	    /usr/local/lib/libpng	\
	    /usr/apps/lib		\
	    /usr/sfw/lib		\
	    /opt/sfw/lib		\
	    ../libpng
	do
	found=no

	if  test -r "$ac_dir/libpng.a"
	    then
		ac_png_libraries=$ac_dir
		ac_png_static_lib=$ac_dir/libpng.a
		found=yes
		break
	    fi

	if  test -r "$ac_dir/libpng.so"
	    then
		ac_png_libraries=$ac_dir
		ac_png_shared_lib=$ac_dir/libpng.so
		found=yes
		break
	    fi

	if  test -r "$ac_dir/libpng.dylib"
	    then
		ac_png_libraries=$ac_dir
		ac_png_shared_lib=$ac_dir/libpng.dylib
		found=yes
		break
	    fi

	if  test $found = yes
	    then
		break
	    fi

	done
    fi

    #echo Includes : $ac_png_includes
    #echo Libraries: $ac_png_libraries

    if  test $ac_png_includes = NO
    then
	PNG_FOUND=0
	AC_DEFINE(PNG_FOUND,0)
    else
	PNG_FOUND=1
	AC_DEFINE(PNG_FOUND,1)

	case $ac_png_includes in
	    /usr/include)
		;;
	    *)
		PNG_CFLAGS=-I$ac_png_includes
		;;
	esac
    fi

    if  test $ac_png_libraries != NO
    then
	PNG_LIBS="-L$ac_png_libraries"
    fi

    if  test $ac_png_static_lib != NO
    then
	PNG_STATIC_REF="$ac_png_static_lib"
    else
	PNG_STATIC_REF="$PNG_LIBS -lpng"
    fi

    PNG_SHARED_REF="$PNG_LIBS -lpng"

    AC_SUBST(PNG_CFLAGS)dnl
    AC_SUBST(PNG_LIBS)dnl
    AC_SUBST(PNG_STATIC_REF)dnl
    AC_SUBST(PNG_SHARED_REF)dnl
    AC_SUBST(PNG_FOUND)dnl
])
#####################################################################
##
##   Look for zlib
##
#####################################################################

AC_DEFUN(AC_PATH_ZLIB,
[
    echo Checking for zlib...

    ZLIB_CFLAGS=
    ZLIB_LIBS=
    ZLIB_STATIC_REF=
    ZLIB_SHARED_REF=

    ZLIB_FOUND=0

    ac_zlib_includes=${ac_zlib_includes:-NO}
    ac_zlib_libraries=${ac_zlib_libraries:-NO}
    ac_zlib_static_lib=NO
    ac_zlib_shared_lib=NO

    if  test $ac_zlib_includes = "NO"
    then
	# Includes
	for ac_dir in		\
	    /usr/include		\
	    /usr/local/include	\
	    /usr/pkg/include	\
	    /usr/local/include/zlib	\
	    /usr/apps/include	\
	    ../zlib
	do
	if  test -r "$ac_dir/zlib.h"
	    then
		ac_zlib_includes=$ac_dir
		break
	    fi
	done
    fi

    if  test $ac_zlib_libraries = "NO"
    then
	# Libraries
	for ac_dir in		\
	    /usr/lib		\
	    /usr/local/lib		\
	    /usr/pkg/lib		\
	    /usr/local/lib/zlib	\
	    /usr/apps/lib		\
	    ../zlib
	do
	found=no

	if  test -r "$ac_dir/libz.a"
	    then
		ac_zlib_libraries=$ac_dir
		ac_zlib_static_lib=$ac_dir/libz.a
		found=yes
		break
	    fi

	if  test -r "$ac_dir/libz.so"
	    then
		ac_zlib_libraries=$ac_dir
		ac_zlib_shared_lib=$ac_dir/libz.so
		found=yes
		break
	    fi

	if  test -r "$ac_dir/libz.dylib"
	    then
		ac_zlib_libraries=$ac_dir
		ac_zlib_shared_lib=$ac_dir/libz.dylib
		found=yes
		break
	    fi

	if  test $found = yes
	    then
		break
	    fi

	done
    fi

    #echo Includes : $ac_zlib_includes
    #echo Libraries: $ac_zlib_libraries

    if  test $ac_zlib_includes = NO
    then
	ZLIB_FOUND=0
	AC_DEFINE(ZLIB_FOUND,0)
    else
	ZLIB_FOUND=1
	AC_DEFINE(ZLIB_FOUND,1)
	ZLIB_CFLAGS=-I$ac_zlib_includes

	if  test "$ZLIB_CFLAGS" = "-I/usr/include"
	then
	    ZLIB_CFLAGS=
	fi
    fi

    if  test $ac_zlib_libraries != NO
    then
	ZLIB_LIBS="-L$ac_zlib_libraries"
    fi

    if  test $ac_zlib_static_lib != NO
    then
	ZLIB_STATIC_REF="$ac_zlib_static_lib"
    else
	ZLIB_STATIC_REF="$ZLIB_LIBS -lz"
    fi

    ZLIB_SHARED_REF="$ZLIB_LIBS -lz"

    AC_SUBST(ZLIB_CFLAGS)dnl
    AC_SUBST(ZLIB_LIBS)dnl
    AC_SUBST(ZLIB_STATIC_REF)dnl
    AC_SUBST(ZLIB_SHARED_REF)dnl
    AC_SUBST(ZLIB_FOUND)dnl
])
#####################################################################
##
##   Look for libjpeg
##
#####################################################################

AC_DEFUN(AC_PATH_JPEG,
[
    echo Checking for libjpeg...

    JPEG_CFLAGS=
    JPEG_LIBS=
    JPEG_STATIC_REF=
    JPEG_SHARED_REF=

    JPEG_FOUND=0

    ac_jpeg_includes=${ac_jpeg_includes:-NO}
    ac_jpeg_libraries=${ac_jpeg_libraries:-NO}
    ac_jpeg_static_lib=NO
    ac_jpeg_shared_lib=NO

    if  test $ac_jpeg_includes = "NO"
    then
	# Includes
	for ac_dir in			\
	    /usr/local/include		\
	    /usr/include		\
	    /usr/pkg/include		\
	    /usr/local/include/jpeg	\
	    /usr/apps/include		\
	    /usr/sfw/include		\
	    /opt/sfw/include		\
	    ../jpeg
	do
	if  test -r "$ac_dir/jpeglib.h"
	    then
		ac_jpeg_includes=$ac_dir
		break
	    fi
	done
    fi

    if  test $ac_jpeg_libraries = "NO"
    then
	# Libraries
	for ac_dir in			\
	    /usr/local/lib		\
	    /usr/lib			\
	    /usr/pkg/lib		\
	    /usr/local/lib/jpeg		\
	    /usr/apps/lib		\
	    /usr/sfw/lib		\
	    /opt/sfw/lib		\
	    ../jpeg
	do
	found=no

	if  test -r "$ac_dir/libjpeg.a"
	    then
		ac_jpeg_libraries=$ac_dir
		ac_jpeg_static_lib=$ac_dir/libjpeg.a
		found=yes
	    fi

	if  test -r "$ac_dir/libjpeg.so"
	    then
		ac_jpeg_libraries=$ac_dir
		ac_jpeg_shared_lib=$ac_dir/libjpeg.so
		found=yes
	    fi

	if  test -r "$ac_dir/libjpeg.dylib"
	    then
		ac_jpeg_libraries=$ac_dir
		ac_jpeg_shared_lib=$ac_dir/libjpeg.dylib
		found=yes
	    fi

	if  test $found = yes
	    then
		break
	    fi

	done
    fi

    #echo Includes : $ac_jpeg_includes
    #echo Libraries: $ac_jpeg_libraries

    if  test $ac_jpeg_includes = NO
    then
	JPEG_FOUND=0
	AC_DEFINE(JPEG_FOUND,0)
    else
	JPEG_FOUND=1
	AC_DEFINE(JPEG_FOUND,1)

	case $ac_jpeg_includes in
	    /usr/include)
		;;
	    *)
		JPEG_CFLAGS=-I$ac_jpeg_includes
		;;
	esac
    fi

    if  test $ac_jpeg_libraries != NO
    then
	JPEG_LIBS="-L$ac_jpeg_libraries"
    fi

    if  test $ac_jpeg_static_lib != NO
    then
	JPEG_STATIC_REF="$ac_jpeg_static_lib"
    else
	JPEG_STATIC_REF="$JPEG_LIBS -ljpeg"
    fi

    JPEG_SHARED_REF="$JPEG_LIBS -ljpeg"

    AC_SUBST(JPEG_CFLAGS)dnl
    AC_SUBST(JPEG_LIBS)dnl
    AC_SUBST(JPEG_STATIC_REF)dnl
    AC_SUBST(JPEG_SHARED_REF)dnl
    AC_SUBST(JPEG_FOUND)dnl
])
#####################################################################
##
##   Look for libtiff
##
#####################################################################

AC_DEFUN(AC_PATH_TIFF,
[
    echo Checking for libtiff...

    TIFF_CFLAGS=
    TIFF_LIBS=
    TIFF_STATIC_REF=
    TIFF_SHARED_REF=

    TIFF_FOUND=0

    ac_tiff_includes=${ac_tiff_includes:-NO}
    ac_tiff_libraries=${ac_tiff_libraries:-NO}
    ac_tiff_static_lib=NO
    ac_tiff_shared_lib=NO

    if  test $ac_tiff_includes = "NO"
    then
	# Includes
	for ac_dir in			\
	    /usr/include		\
	    /usr/local/include		\
	    /usr/pkg/include		\
	    /usr/local/include/tiff	\
	    /usr/apps/include		\
	    /usr/sfw/include		\
	    /opt/sfw/include		\
	    ../tiff/libtiff
	do
	if  test -r "$ac_dir/tiffio.h"
	    then
		ac_tiff_includes=$ac_dir
		break
	    fi
	done
    fi

    if  test $ac_tiff_libraries = "NO"
    then
	# Libraries
	for ac_dir in			\
	    /usr/lib			\
	    /usr/local/lib		\
	    /usr/pkg/lib		\
	    /usr/local/lib/tiff		\
	    /usr/apps/lib		\
	    /usr/sfw/lib		\
	    /opt/sfw/lib		\
	    ../tiff/libtiff
	do
	found=no

	if  test -r "$ac_dir/libtiff.a"
	    then
		ac_tiff_libraries=$ac_dir
		ac_tiff_static_lib=$ac_dir/libtiff.a
		found=yes
		break
	    fi

	if  test -r "$ac_dir/libtiff.so"
	    then
		ac_tiff_libraries=$ac_dir
		ac_tiff_shared_lib=$ac_dir/libtiff.so
		found=yes
		break
	    fi

	if  test -r "$ac_dir/libtiff.dylib"
	    then
		ac_tiff_libraries=$ac_dir
		ac_tiff_shared_lib=$ac_dir/libtiff.dylib
		found=yes
		break
	    fi

	if  test $found = yes
	    then
		break
	    fi

	done
    fi

    #echo Includes : $ac_tiff_includes
    #echo Libraries: $ac_tiff_libraries

    if  test $ac_tiff_includes = NO
    then
	TIFF_FOUND=0
	AC_DEFINE(TIFF_FOUND,0)
    else
	TIFF_FOUND=1
	AC_DEFINE(TIFF_FOUND,1)

	case $ac_tiff_includes in
	    /usr/include)
		;;
	    *)
		TIFF_CFLAGS=-I$ac_tiff_includes
		;;
	esac
    fi

    if  test $ac_tiff_libraries != NO
    then
	TIFF_LIBS="-L$ac_tiff_libraries"
    fi

    if  test $ac_tiff_static_lib != NO
    then
	TIFF_STATIC_REF="$ac_tiff_static_lib"
    else
	TIFF_STATIC_REF="$TIFF_LIBS -ltiff"
    fi

    TIFF_SHARED_REF="$TIFF_LIBS -ltiff"

    AC_SUBST(TIFF_CFLAGS)dnl
    AC_SUBST(TIFF_LIBS)dnl
    AC_SUBST(TIFF_STATIC_REF)dnl
    AC_SUBST(TIFF_SHARED_REF)dnl
    AC_SUBST(TIFF_FOUND)dnl
])
