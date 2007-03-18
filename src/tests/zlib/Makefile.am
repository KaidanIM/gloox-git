##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual -ansi

noinst_PROGRAMS = zlib_test

zlib_test_SOURCES = zlib_test.cpp
zlib_test_LDADD = ../../compressionzlib.o
zlib_test_CFLAGS = $(CPPFLAGS)
