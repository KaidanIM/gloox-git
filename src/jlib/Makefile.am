##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -g -fno-inline

SUBDIRS = wrapper

noinst_LTLIBRARIES = libclient.la

libclient_la_SOURCES = jclient.cpp thread.cpp jthread.cpp result.cpp disco.cpp roster.cpp discoitem.cpp adhoc.cpp

libclient_la_LIBADD = ./wrapper/libwrapper.la $(IKSEMEL_LIBS)
libclient_la_CFLAGS = $(CPPFLAGS) $(IKSEMEL_CFLAGS)
