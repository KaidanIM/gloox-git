dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.50)
AC_INIT
AC_CONFIG_SRCDIR([configure.ac])
dnl Every other copy of the package version number gets its value from here
AM_INIT_AUTOMAKE(gloox,0.7)

dnl create a config.h file (Automake will add -DHAVE_CONFIG_H)
AM_CONFIG_HEADER(config.h)

AC_SUBST(VERSION)

ISODATE=`date +%Y-%m-%d`
AC_SUBST(ISODATE)

AC_CANONICAL_HOST

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AM_PROG_LIBTOOL

dnl Checks for libraries.

dnl Iksemel
AC_CHECK_HEADER(iksemel.h,
AC_CHECK_LIB(iksemel, iks_has_tls,
         [libiksemel=yes LIBS="${LIBS} -liksemel"], libiksemel=no),
         libiksemel=no)
if test "$libiksemel" == "no" ; then
  AC_MSG_ERROR([Libiksemel not found])
fi
AC_MSG_CHECKING([if Libiksemel is present])
AC_MSG_RESULT($libiksemel)


dnl Libidn
AC_ARG_WITH(libidn, AC_HELP_STRING([--with-libidn=[DIR]],
      [Support IDN (needs GNU Libidn)]),
      libidn=$withval, libidn=yes)
if test "$libidn" != "no"; then
  if test "$libidn" != "yes"; then
    LDFLAGS="${LDFLAGS} -L$libidn/lib"
    CPPFLAGS="${CPPFLAGS} -I$libidn/include"
  fi
  AC_CHECK_HEADER(idna.h,
  AC_CHECK_LIB(idn, stringprep_check_version,
           [libidn=yes LIBS="${LIBS} -lidn"], libidn=no),
           libidn=no)
fi
if test "$libidn" != "no" ; then
  AC_DEFINE(HAVE_LIBIDN, 1, [Define to 1 if you want IDN support.])
else
  AC_MSG_WARN([Libidn not found])
fi
AC_MSG_CHECKING([if Libidn should be used])
AC_MSG_RESULT($libidn)

dnl GnuTLS
AC_ARG_WITH(gnutls, AC_HELP_STRING([--with-gnutls=[DIR]],
      [Support TLS (needs GnuTLS)]),
      gnutls=$withval, gnutls=yes)
if test "$gnutls" != "no"; then
  if test "$gnutls" != "yes"; then
    LDFLAGS="${LDFLAGS} -L$gnutls/lib"
    CPPFLAGS="${CPPFLAGS} -I$gnutls/include"
  fi
  AC_CHECK_HEADER(gnutls/gnutls.h,
  AC_CHECK_LIB(gnutls, gnutls_check_version,
           [gnutls=yes LIBS="${LIBS} `libgnutls-config --libs`"], gnutls=no),
           gnutls=no)
fi
if test "$gnutls" != "no" ; then
  AC_DEFINE(HAVE_GNUTLS, 1, [Define to 1 if you want TLS support.])
else
  AC_MSG_WARN([GnuTLS not found])
fi
AC_MSG_CHECKING([if GnuTLS should be used])
AC_MSG_RESULT($gnutls)

dnl Zlib
AC_ARG_WITH(zlib, AC_HELP_STRING([--with-zlib=[DIR]],
      [Support Stream Compression (needs Zlib)]),
      zlib=$withval, zlib=yes)
if test "$zlib" != "no"; then
  if test "$zlib" != "yes"; then
    LDFLAGS="${LDFLAGS} -L$zlib/lib"
    CPPFLAGS="${CPPFLAGS} -I$zlib/include"
  fi
  AC_CHECK_HEADER(zlib.h,
  AC_CHECK_LIB(z, zlibVersion,
           [zlib=yes LIBS="${LIBS} -lz"], zlib=no),
           zlib=no)
fi
if test "$zlib" != "no" ; then
  AC_DEFINE(HAVE_ZLIB, 1, [Define to 1 if you want Stream Compression support.])
else
  AC_MSG_WARN([Zlib not found])
fi
AC_MSG_CHECKING([if Zlib should be used])
AC_MSG_RESULT($zlib)

dnl Debug
AC_ARG_WITH(debug, AC_HELP_STRING([--enable-debug=[yes|no]],
      [Enable debug output. Default: yes]),
      debug=$withval, debug=yes)
if test "$debug" != "no" ; then
  AC_DEFINE(DEBUG, 1, [Define to 1 if you want debug output.])
fi

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h strings.h errno.h arpa/nameser.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE

dnl Generating makefiles.
AC_CONFIG_FILES([
Makefile
gloox.pc
gloox-config
src/Makefile
src/tests/Makefile
])
AC_OUTPUT
