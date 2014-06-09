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
##   Find out whether there is a domain name in the struct utsname.
##   An what is is called
##
#####################################################################

AC_DEFUN(AC_STRUCT_UTSNAME_DOMAINNAME,
[
    dnl Is the domain in the utsname struct? What is it same..

    AC_TRY_LINK(
	[#include <sys/utsname.h>],
	[struct utsname u; return u.domainname;],
	AC_DEFINE(UTSNAME_DOMAIN,domainname),)
    AC_TRY_LINK(
	[#include <sys/utsname.h>],
	[struct utsname u; return u.__domainname;],
	AC_DEFINE(UTSNAME_DOMAIN,__domainname),)
])
