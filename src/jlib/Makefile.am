##
## Process this file with automake to produce Makefile.in
##

SUBDIRS = wrapper

noinst_LTLIBRARIES = libclient.la

libclient_la_SOURCES = jclient.cpp thread.cpp jthread.cpp result.cpp disco.cpp

libclient_la_LIBADD = ./wrapper/libwrapper.la $(IKSEMEL_LIBS)
libclient_la_CFLAGS = -g $(CFLAGS) $(IKSEMEL_CFLAGS)
