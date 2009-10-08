##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual

noinst_PROGRAMS = md4_test

md4_test_SOURCES = md4_test.cpp
md4_test_LDADD = ../../md4.o
md4_test_CFLAGS = $(CPPFLAGS)
