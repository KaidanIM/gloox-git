##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual 

noinst_PROGRAMS = discoinfo_test

discoinfo_test_SOURCES = discoinfo_test.cpp
discoinfo_test_LDADD = ../../tag.o ../../stanza.o \
                        ../../gloox.o ../../iq.o ../../util.o \
			../../error.o ../../jid.o ../../prep.o \
			../../dataform.o ../../dataformfieldcontainer.o ../../dataformreported.o \
			../../dataformitem.o ../../dataformfield.o \
			../../softwareversion.o ../../stanzaextensionfactory.o
discoinfo_test_CFLAGS = $(CPPFLAGS)
