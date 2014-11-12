
##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual

noinst_PROGRAMS = jinglecontent_test

jinglecontent_test_SOURCES = jinglecontent_test.cpp
jinglecontent_test_LDADD = ../../stanza.o ../../jid.o ../../tag.o ../../prep.o \
                ../../gloox.o \
                ../../iq.o ../../util.o ../../sha.o ../../base64.o \
                ../../jinglecontent.o ../../error.o ../../mutex.o \
                ../../jinglepluginfactory.o
jinglecontent_test_CFLAGS = $(CPPFLAGS)
