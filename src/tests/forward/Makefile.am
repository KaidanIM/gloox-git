##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual 

noinst_PROGRAMS = forward_test

forward_test_SOURCES = forward_test.cpp
forward_test_LDADD = ../../jid.o ../../tag.o \
                        ../../logsink.o ../../prep.o ../../util.o \
                        ../../gloox.o ../../iq.o ../../stanza.o \
                        ../../error.o ../../dataformfieldcontainer.o \
                        ../../dataform.o ../../dataformfield.o \
                        ../../dataformitem.o ../../softwareversion.o \
                        ../../dataformreported.o
forward_test_CFLAGS = $(CPPFLAGS)
