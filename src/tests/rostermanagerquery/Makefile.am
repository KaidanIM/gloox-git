
##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual -ansi

noinst_PROGRAMS = rostermanagerquery_test

rostermanagerquery_test_SOURCES = rostermanagerquery_test.cpp
rostermanagerquery_test_LDADD = ../../tag.o ../../stanza.o \
                        ../../gloox.o ../../iq.o ../../util.o \
			../../error.o ../../jid.o ../../prep.o \
			../../dataform.o ../../presence.o \
                        ../../message.o ../../subscription.o ../../dataformfieldcontainer.o ../../dataformreported.o \
			../../dataformitem.o ../../dataformfield.o \
			../../softwareversion.o ../../stanzaextensionfactory.o \
			../../rosteritem.o \
			../../privacyitem.o
rostermanagerquery_test_CFLAGS = $(CPPFLAGS)
