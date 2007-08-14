
##
## Process this file with automake to produce Makefile.in
##

SUBDIRS = . tests examples

AM_CPPFLAGS = -Wall -pedantic -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith \
                -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual -ansi

lib_LTLIBRARIES = libgloox.la

libgloox_la_SOURCES = adhoc.cpp annotations.cpp base64.cpp bookmarkstorage.cpp \
	chatstatefilter.cpp client.cpp clientbase.cpp component.cpp compressionzlib.cpp \
	connectionbosh.cpp connectionhttpproxy.cpp connectionsocks5proxy.cpp connectiontcpbase.cpp \
	connectiontcpclient.cpp connectiontcpserver.cpp connectiontls.cpp dataform.cpp dataformbase.cpp \
	dataformfield.cpp dataformitem.cpp dataformreported.cpp delayeddelivery.cpp disco.cpp \
	dns.cpp flexoff.cpp gloox.cpp gpgencrypted.cpp gpgsigned.cpp \
	inbandbytestream.cpp inbandbytestreammanager.cpp instantmucroom.cpp jid.cpp lastactivity.cpp \
	logsink.cpp md5.cpp messageeventfilter.cpp messagefilter.cpp messagesession.cpp \
	mucmessagesession.cpp mucroom.cpp mutex.cpp nonsaslauth.cpp oob.cpp parser.cpp prep.cpp \
	privacyitem.cpp privacymanager.cpp privatexml.cpp registration.cpp rosteritem.cpp \
	rostermanager.cpp search.cpp sha.cpp simanager.cpp siprofileft.cpp socks5bytestream.cpp \
	socks5bytestreammanager.cpp socks5bytestreamserver.cpp stanza.cpp stanzaextensionfactory.cpp \
	tag.cpp tlsdefault.cpp tlsgnutlsbase.cpp tlsgnutlsclient.cpp \
	tlsgnutlsclientanon.cpp tlsgnutlsserveranon.cpp tlsopenssl.cpp tlsschannel.cpp \
	uniquemucroom.cpp vcard.cpp vcardmanager.cpp vcardupdate.cpp xdelayeddelivery.cpp \
	connectiontls.cpp

libgloox_la_LDFLAGS = -version-info 7:0:0 -no-undefined -no-allow-shlib-undefined
libgloox_la_LIBADD =
libgloox_la_CFLAGS = $(CPPFLAGS)

libglooxincludedir = $(includedir)/gloox
libglooxinclude_HEADERS = adhoccommandprovider.h      privacymanager.h        tag.h \
                            adhoc.h                   iqhandler.h             privatexml.h \
                            annotations.h             client.h                privatexmlhandler.h \
                            annotationshandler.h      component.h             registration.h \
                            bookmarkhandler.h         registrationhandler.h   jid.h \
                            bookmarkstorage.h         messagehandler.h        rosteritem.h \
                            clientbase.h              rosterlistener.h        taghandler.h \
                            connectionlistener.h      rostermanager.h         gloox.h \
                            disco.h                   presencehandler.h       subscriptionhandler.h \
                            discohandler.h            privacyitem.h           stanza.h \
                            disconodehandler.h        privacylisthandler.h    loghandler.h \
                            lastactivity.h            lastactivityhandler.h   flexoff.h \
                            flexoffhandler.h          dataform.h              dataformfield.h \
                            dataformitem.h            dataformbase.h          dataformreported.h \
                            macros.h                  logsink.h               messagesession.h \
                            messageeventhandler.h     messagesessionhandler.h chatstatehandler.h \
                            chatstatefilter.h         messageeventfilter.h    inbandbytestream.h \
                            inbandbytestreamhandler.h inbandbytestreamdatahandler.h \
                            inbandbytestreammanager.h messagefilter.h         vcard.h \
                            vcardmanager.h            vcardhandler.h          adhochandler.h \
                            search.h                  searchhandler.h         statisticshandler.h \
                            resource.h                mucroom.h               mucroomhandler.h \
                            mucroomconfighandler.h    parser.h                \
                            mucinvitationhandler.h    stanzaextension.h       oob.h \
                            vcardupdate.h             delayeddelivery.h       base64.h \
                            xdelayeddelivery.h        gpgencrypted.h          gpgsigned.h \
                            md5.h                     sha.h                   uniquemucroom.h \
                            instantmucroom.h          tlsbase.h               tlshandler.h \
                            connectionbase.h          connectiondatahandler.h compressiondatahandler.h \
                            compressionbase.h         connectiontcpclient.h   connectionhttpproxy.h \
                            tlsdefault.h              simanager.h             siprofilehandler.h \
                            sihandler.h               siprofileft.h           siprofilefthandler.h \
                            socks5bytestreammanager.h connectionsocks5proxy.h socks5bytestreamdatahandler.h \
                            socks5bytestreamhandler.h socks5bytestream.h      socks5bytestreamserver.h \
                            mutex.h                   mutexguard.h            connectiontcpbase.h \
                            connectiontcpserver.h     connectionhandler.h     connectionbosh.h \
                            connectiontls.h

noinst_HEADERS = compressionzlib.h connectiontls.h dns.h mucmessagesession.h \
	nonsaslauth.h prep.h stanzaextensionfactory.h tlsgnutlsbase.h tlsgnutlsclient.h \
	tlsgnutlsclientanon.h tlsgnutlsserveranon.h tlsopenssl.h tlsschannel.h

EXTRA_DIST = version.rc

test:
	make -C tests test

check: test

leaktest:
	make -C tests leaktest

leakcheck: leaktest
