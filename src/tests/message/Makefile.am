##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual -ansi

noinst_PROGRAMS = message_test

message_test_SOURCES = message_test.cpp
message_test_LDADD = ../../tag.o ../../message.o ../../stanza.o ../../stanzaextensionfactory.o ../../oob.o \
                ../../gpgencrypted.o ../../gpgsigned.o ../../vcardupdate.o ../../delayeddelivery.o \
                ../../xdelayeddelivery.o ../../amp.o ../../jid.o ../../prep.o ../../gloox.o ../../util.o \
                ../../capabilities.o ../../sha.o ../../base64.o
message_test_CFLAGS = $(CPPFLAGS)
