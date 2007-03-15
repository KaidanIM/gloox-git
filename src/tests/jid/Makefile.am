##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual -ansi

noinst_PROGRAMS = jid_test

jid_test_SOURCES = jid_test.cpp
jid_test_LDADD = ../../jid.o ../../prep.o
jid_test_CFLAGS = $(CPPFLAGS)
