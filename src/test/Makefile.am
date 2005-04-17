##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -g -fno-inline

bin_PROGRAMS = connect feedertest workertest

connect_SOURCES = connect.cpp
connect_LDADD = ../jlib/libclient.la -lpthread
connect_CFLAGS = $(CPPFLAGS)


feedertest_SOURCES = feedertest.cpp
feedertest_LDADD = ../feeder/libfeeder.la -lpthread
feedertest_CFLAGS = $(CPPFLAGS)


workertest_SOURCES = workertest.cpp
workertest_LDADD = ../worker/libworker.la -lpthread
workertest_CFLAGS = $(CPPFLAGS)
