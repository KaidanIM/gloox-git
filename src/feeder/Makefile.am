##
## Process this file with automake to produce Makefile.in
##

CPPFLAGS = -g -fno-inline

noinst_LTLIBRARIES = libfeeder.la

libfeeder_la_SOURCES = feeder.cpp storage.cpp result.cpp
libfeeder_la_LIBADD = ../jlib/libclient.la
libfeeder_la_CFLAGS = $(CPPFLAGS)
