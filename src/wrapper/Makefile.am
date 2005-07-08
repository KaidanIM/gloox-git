##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -g3 -fno-inline

noinst_LTLIBRARIES = libwrapper.la

libwrapper_la_SOURCES = stream.cpp tree.cpp parser.cpp

libwrapper_la_LIBADD = $(IKSEMEL_LIBS)
libwrapper_la_CFLAGS = $(CPPFLAGS) $(IKSEMEL_CFLAGS)

libglooxincludedir = $(includedir)/gloox/wrapper
libglooxinclude_HEADERS = stream.h parser.h tree.h