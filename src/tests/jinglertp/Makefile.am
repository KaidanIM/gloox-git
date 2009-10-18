
##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual

noinst_PROGRAMS = jinglertp_test

jinglertp_test_SOURCES = jinglertp_test.cpp
jinglertp_test_LDADD = ../../stanza.o ../../jid.o ../../tag.o ../../prep.o \
 		../../gloox.o ../../stanzaextensionfactory.o \
                ../../jingledescription.o ../../jinglertp.o \
		../../iq.o ../../util.o ../../sha.o ../../base64.o
jinglertp_test_CFLAGS = $(CPPFLAGS)
