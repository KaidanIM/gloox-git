##
## Process this file with automake to produce Makefile.in
##

AM_CPPFLAGS = -pedantic -Wall -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual -ansi

noinst_PROGRAMS = chatstatefilter_test

chatstatefilter_test_SOURCES = chatstatefilter_test.cpp
chatstatefilter_test_LDADD = ../../tag.o ../../stanza.o ../../stanzaextensionfactory.o \
 				../../jid.o ../../prep.o ../../seversion.o \
				../../message.o ../../util.o ../../disco.o \
				../../sha.o ../../base64.o ../../iq.o \
			../../clientbase.o ../../connectiontcpclient.o ../../connectiontcpbase.o \
			../../parser.o \
			../../md5.o ../../tlsgnutlsclient.o ../../tlsopenssl.o ../../tlsschannel.o \
			../../logsink.o ../../compressionzlib.o \
			../../dns.o \
			../../gloox.o ../../tlsgnutlsbase.o ../../tlsdefault.o \
			../../tlsgnutlsclientanon.o ../../tlsgnutlsserveranon.o ../../mutex.o \
			../../presence.o ../../subscription.o ../../error.o ../../chatstate.o
chatstatefilter_test_CPPFLAGS = $(CPPFLAGS)
