##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual -ansi

noinst_PROGRAMS = clientbase_test

clientbase_test_SOURCES = clientbase_test.cpp
clientbase_test_LDADD = ../../clientbase.o ../../jid.o ../../connectiontcp.o \
			../../disco.o ../../parser.o ../../tag.o ../../stanza.o ../../base64.o \
			../../md5.o ../../tlsgnutlsclient.o ../../tlsopenssl.o ../../tlsschannel.o \
			../../logsink.o ../../messagesession.o ../../prep.o ../../compressionzlib.o \
			../../dns.o ../../stanzaextensionfactory.o ../../oob.o ../../gpgencrypted.o \
			../../gpgsigned.o ../../vcardupdate.o ../../delayeddelivery.o \
			../../xdelayeddelivery.o ../../main.o ../../tlsgnutlsbase.o
clientbase_test_CFLAGS = $(CPPFLAGS)
