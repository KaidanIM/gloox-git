dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.50)
AC_INIT
AC_CONFIG_SRCDIR([configure.ac])
dnl Every other copy of the package version number gets its value from here
AM_INIT_AUTOMAKE(mailmon,0.1)

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

PKG_CHECK_MODULES(IKSEMEL,iksemel >= 1.2,,exit)
AC_SUBST(IKSEMEL_CFLAGS)
AC_SUBST(IKSEMEL_LIBS)

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h strings.h errno.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE

dnl Generating makefiles.
AC_CONFIG_FILES([
Makefile
doc/Makefile
src/Makefile
src/jlib/Makefile
src/jlib/wrapper/Makefile
src/feeder/Makefile
src/worker/Makefile
src/test/Makefile
])
AC_OUTPUT
