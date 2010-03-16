##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual 

noinst_PROGRAMS = adhoccommandnote_test

adhoccommandnote_test_SOURCES = adhoccommandnote_test.cpp
adhoccommandnote_test_LDADD = ../../tag.o ../../stanza.o ../../gloox.o ../../iq.o ../../util.o \
			../../error.o ../../jid.o ../../prep.o \
			../../dataform.o ../../dataformfieldcontainer.o ../../dataformreported.o \
			../../dataformitem.o ../../dataformfield.o \
			../../softwareversion.o ../../stanzaextensionfactory.o ../../mutex.o
adhoccommandnote_test_CFLAGS = $(CPPFLAGS)
