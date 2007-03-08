##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -g3 -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual -ansi

noinst_PROGRAMS = stanzaextensionfactory_test

stanzaextensionfactory_test_SOURCES = stanzaextensionfactory_test.cpp ../../tag.cpp ../../stanza.cpp \
 			../../jid.cpp ../../prep.cpp ../../stanzaextensionfactory.cpp ../../oob.cpp \
 			../../vcardupdate.cpp ../../delayeddelivery.cpp ../../xdelayeddelivery.cpp \
 			../../gpgsigned.cpp ../../gpgencrypted.cpp ../../main.cpp
stanzaextensionfactory_test_LDADD =
stanzaextensionfactory_test_CFLAGS = $(CPPFLAGS)
