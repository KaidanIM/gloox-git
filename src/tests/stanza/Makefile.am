##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -g3 -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual -ansi

noinst_PROGRAMS = stanza_test

stanza_test_SOURCES = stanza_test.cpp ../../tag.cpp ../../stanza.cpp ../../jid.cpp \
			../../prep.cpp ../../stanzaextensionfactory.cpp ../../oob.cpp ../../vcardupdate.cpp \
 			../../delayeddelivery.cpp ../../xdelayeddelivery.cpp ../../gpgsigned.cpp \
 			../../gpgencrypted.cpp ../../main.cpp
stanza_test_LDADD =
stanza_test_CFLAGS = $(CPPFLAGS)
