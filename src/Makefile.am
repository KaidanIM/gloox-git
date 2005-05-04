##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -g -fno-inline

SUBDIRS = wrapper

lib_LTLIBRARIES = libclient.la

libclient_la_SOURCES = jclient.cpp thread.cpp jthread.cpp disco.cpp roster.cpp adhoc.cpp \
			privatexml.cpp
libclient_la_LDFLAGS = -version-info 6:0:2 -no-undefined
libclient_la_LIBADD = ./wrapper/libwrapper.la $(IKSEMEL_LIBS)
libclient_la_CFLAGS = $(CPPFLAGS) $(IKSEMEL_CFLAGS)
