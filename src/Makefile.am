##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -g -fno-inline

SUBDIRS = wrapper

lib_LTLIBRARIES = libgloox.la

libgloox_la_SOURCES = jclient.cpp thread.cpp jthread.cpp disco.cpp roster.cpp \
			adhoc.cpp privatexml.cpp prep.cpp registration.cpp
libgloox_la_LDFLAGS = -version-info 6:0:2 -no-undefined
libgloox_la_LIBADD = ./wrapper/libwrapper.la $(IKSEMEL_LIBS)
libgloox_la_CFLAGS = $(CPPFLAGS) $(IKSEMEL_CFLAGS)
