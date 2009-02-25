##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual -ansi

noinst_PROGRAMS = tlsopenssl_test

tlsopenssl_test_SOURCES = tlsopenssl_test.cpp
tlsopenssl_test_LDADD = ../../tlsopensslclient.o ../../tlsopensslserver.o ../../tlsopensslbase.o ../../gloox.o
tlsopenssl_test_CFLAGS = $(CPPFLAGS)
