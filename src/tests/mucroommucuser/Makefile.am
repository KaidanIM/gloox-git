##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual 

noinst_PROGRAMS = mucroommucuser_test

mucroommucuser_test_SOURCES = mucroommucuser_test.cpp
mucroommucuser_test_LDADD = ../../tag.o ../../stanza.o \
                        ../../gloox.o ../../iq.o ../../util.o \
			../../error.o ../../jid.o ../../prep.o \
			../../dataform.o ../../presence.o \
                        ../../message.o ../../dataformfieldcontainer.o ../../dataformreported.o \
			../../dataformitem.o ../../dataformfield.o \
			../../softwareversion.o ../../stanzaextensionfactory.o \
			../../delayeddelivery.o
mucroommucuser_test_CFLAGS = $(CPPFLAGS)
