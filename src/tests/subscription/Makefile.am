##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual -ansi

noinst_PROGRAMS = subscription_test

subscription_test_SOURCES = subscription_test.cpp
subscription_test_LDADD = ../../tag.o ../../subscription.o ../../stanza.o ../../stanzaextensionfactory.o \
                ../../oob.o ../../gpgencrypted.o ../../gpgsigned.o ../../vcardupdate.o ../../delayeddelivery.o \
                ../../xdelayeddelivery.o ../../amp.o ../../jid.o ../../prep.o ../../gloox.o
subscription_test_CFLAGS = $(CPPFLAGS)
