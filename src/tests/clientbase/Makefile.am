##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual -ansi

noinst_PROGRAMS = clientbase_test

clientbase_test_SOURCES = clientbase_test.cpp ../../clientbase.cpp ../../jid.cpp ../../connectiontcp.cpp \
			../../disco.cpp ../../parser.cpp ../../tag.cpp ../../stanza.cpp ../../base64.cpp \
			../../md5.cpp ../../tlsgnutls.cpp ../../tlsopenssl.cpp ../../tlsschannel.cpp \
			../../logsink.cpp ../../messagesession.cpp ../../prep.cpp ../../compressionzlib.cpp \
			../../dns.cpp ../../stanzaextensionfactory.cpp ../../oob.cpp ../../gpgencrypted.cpp \
			../../gpgsigned.cpp ../../vcardupdate.cpp ../../delayeddelivery.cpp \
			../../xdelayeddelivery.cpp
clientbase_test_LDADD =
clientbase_test_CFLAGS = $(CPPFLAGS)
