##
## Process this file with automake to produce Makefile.in
##

CPPFLAGS = -g -fno-inline

noinst_LTLIBRARIES = libworker.la

libworker_la_SOURCES = worker.cpp

libworker_la_LIBADD = ../jlib/libclient.la
libworker_la_CFLAGS = $(CPPFLAGS)
