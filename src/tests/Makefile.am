##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -g3 -fno-inline

bin_PROGRAMS = libidn_test register_test

libidn_test_SOURCES = libidn_test.cpp
libidn_test_LDADD = -L/tmp/gloox/lib -lgloox -lpthread
libidn_test_CFLAGS = $(CPPFLAGS)

register_test_SOURCES = register_test.cpp
register_test_LDADD = -L/tmp/gloox/lib -lgloox -lpthread
register_test_CFLAGS = $(CPPFLAGS)
