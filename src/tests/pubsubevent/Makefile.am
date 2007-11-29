##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual -ansi

noinst_PROGRAMS = sepubsubevent_test

sepubsubevent_test_SOURCES = sepubsubevent_test.cpp
sepubsubevent_test_LDADD = ../../gloox.o ../../tag.o ../../jid.o ../../prep.o \
                           ../../util.o ../../error.o ../../pubsubevent.o \
                           ../../dataform.o ../../dataformfield.o \
                           ../../dataformfieldcontainer.o

sepubsubevent_test_CFLAGS = $(CPPFLAGS)
