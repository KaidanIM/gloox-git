##
## Process this file with automake to produce Makefile.in
##

SUBDIRS = . tests examples

AM_CPPFLAGS = -Wall -pedantic -pipe -W -Wfloat-equal -Wcast-align -Wsign-compare -Wpointer-arith \
                -Wswitch -Wunknown-pragmas -Wconversion -Wundef -Wcast-qual -ansi \
                -Wwrite-strings -Wformat-security -Wmissing-format-attribute

lib_LTLIBRARIES = libgloox.la

libgloox_la_SOURCES = adhoc.cpp \
			amp.cpp \
			annotations.cpp \
			attention.cpp \
			base64.cpp \
			bob.cpp \
			bookmarkstorage.cpp \
			capabilities.cpp \
			chatstate.cpp \
			chatstatefilter.cpp \
			clientbase.cpp \
			client.cpp \
			component.cpp \
			compressiondefault.cpp \
			compressionzlib.cpp \
			connectionbosh.cpp \
			connectioncompression.cpp \
			connectionhttpproxy.cpp \
			connectionsocks5proxy.cpp \
			connectiontcpbase.cpp \
			connectiontcpclient.cpp \
			connectiontcpserver.cpp \
			connectiontls.cpp \
			dataform.cpp \
			dataformfieldcontainer.cpp \
			dataformfield.cpp \
			dataformitem.cpp \
			dataformreported.cpp \
			delayeddelivery.cpp \
			disco.cpp \
			dns.cpp \
			error.cpp \
			eventdispatcher.cpp \
			featureneg.cpp \
			flexoff.cpp \
			gloox.cpp \
			gpgencrypted.cpp \
			gpgsigned.cpp \
			inbandbytestream.cpp \
			instantmucroom.cpp \
			iq.cpp \
			jid.cpp \
			jinglecontent.cpp \
			jingledtmf.cpp \
			jingleiceudp.cpp \
			jinglerawudp.cpp \
			jinglertp.cpp \
			jinglesession.cpp \
			lastactivity.cpp \
			logsink.cpp \
			md5.cpp \
			message.cpp \
			messageevent.cpp \
			messageeventfilter.cpp \
			messagefilter.cpp \
			messagesession.cpp \
			mucinvite.cpp \
			mucmessagesession.cpp \
			mucroom.cpp \
			mutex.cpp \
			nickname.cpp \
			nonsaslauth.cpp \
			oob.cpp \
			parser.cpp \
			prep.cpp \
			presence.cpp \
			privacyitem.cpp \
			privacymanager.cpp \
			privatexml.cpp \
			pubsubevent.cpp \
			pubsubitem.cpp \
			pubsubmanager.cpp \
			rap.cpp \
			receipt.cpp \
			registration.cpp \
			rosteritem.cpp \
			rostermanager.cpp \
			search.cpp \
			sha.cpp \
			shim.cpp \
			simanager.cpp \
			siprofileft.cpp \
			socks5bytestream.cpp \
			socks5bytestreammanager.cpp \
			socks5bytestreamserver.cpp \
			softwareversion.cpp \
			stanza.cpp \
			stanzaextensionfactory.cpp \
			subscription.cpp \
			tag.cpp \
			tlsdefault.cpp \
			tlsgnutlsbase.cpp \
			tlsgnutlsclientanon.cpp \
			tlsgnutlsclient.cpp \
			tlsgnutlsserveranon.cpp \
			tlsopenssl.cpp \
			tlsschannel.cpp \
			uniquemucroom.cpp \
			util.cpp \
			vcard.cpp \
			vcardmanager.cpp \
			vcardupdate.cpp \
			xhtmlim.cpp

libgloox_la_LDFLAGS = -version-info 8:0:0 -no-undefined -no-allow-shlib-undefined
libgloox_la_LIBADD =
libgloox_la_CFLAGS = $(CPPFLAGS)

libglooxincludedir = $(includedir)/gloox
libglooxinclude_HEADERS = adhoccommandprovider.h \
			adhoc.h \
			adhochandler.h \
			amp.h \
			annotations.h \
			annotationshandler.h \
			attention.h \
			base64.h \
			bob.h \
			bookmarkhandler.h \
			bookmarkstorage.h \
			bytestreamdatahandler.h \
			bytestream.h \
			bytestreamhandler.h \
			capabilities.h \
			chatstatefilter.h \
			chatstate.h \
			chatstatehandler.h \
			clientbase.h \
			client.h \
			component.h \
			compressionbase.h \
			compressiondatahandler.h \
			connectionbase.h \
			connectionbosh.h \
			connectioncompression.h \
			connectiondatahandler.h \
			connectionhandler.h \
			connectionhttpproxy.h \
			connectionlistener.h \
			connectionsocks5proxy.h \
			connectiontcpbase.h \
			connectiontcpclient.h \
			connectiontcpserver.h \
			connectiontls.h \
			dataformfieldcontainer.h \
			dataformfield.h \
			dataform.h \
			dataformitem.h \
			dataformreported.h \
			delayeddelivery.h \
			disco.h \
			discohandler.h \
			disconodehandler.h \
			error.h \
			eventdispatcher.h \
			event.h \
			eventhandler.h \
			featureneg.h \
			flexoff.h \
			flexoffhandler.h \
			gloox.h \
			gpgencrypted.h \
			gpgsigned.h \
			inbandbytestream.h \
			instantmucroom.h \
			iq.h \
			iq.h \
			iqhandler.h \
			jid.h \
			jinglecontent.h \
			jingledescription.h \
			jingledtmf.h \
			jingleiceudp.h \
			jingleplugin.h \
			jinglerawudp.h \
			jinglertp.h \
			jinglesession.h \
			jinglesessionhandler.h \
			jingletransport.h \
			lastactivity.h \
			lastactivityhandler.h \
			loghandler.h \
			logsink.h \
			macros.h \
			md5.h \
			messageeventfilter.h \
			messageevent.h \
			messageeventhandler.h \
			messagefilter.h \
			message.h \
			messagehandler.h \
			messagesession.h \
			messagesessionhandler.h \
			mucinvitationhandler.h \
			mucinvite.h \
			mucroomconfighandler.h \
			mucroom.h \
			mucroomhandler.h \
			mutexguard.h \
			mutex.h \
			nickname.h \
			oob.h \
			parser.h \
			presence.h \
			presencehandler.h \
			privacyitem.h \
			privacylisthandler.h \
			privacymanager.h \
			privatexml.h \
			privatexmlhandler.h \
			pubsubevent.h \
			pubsub.h \
			pubsubitem.h \
			pubsubmanager.h \
			pubsubresulthandler.h \
			rap.h \
			receipt.h \
			registration.h \
			registrationhandler.h \
			resource.h \
			rosteritem.h \
			rosterlistener.h \
			rostermanager.h \
			search.h \
			searchhandler.h \
			sha.h \
			shim.h \
			sihandler.h \
			simanager.h \
			siprofileft.h \
			siprofilefthandler.h \
			siprofilehandler.h \
			socks5bytestream.h \
			socks5bytestreammanager.h \
			socks5bytestreamserver.h \
			softwareversion.h \
			stanzaextension.h \
			stanza.h \
			statisticshandler.h \
			subscription.h \
			subscriptionhandler.h \
			tag.h \
			taghandler.h \
			tlsbase.h \
			tlsdefault.h \
			tlshandler.h \
			uniquemucroom.h \
			util.h \
			vcard.h \
			vcardhandler.h \
			vcardmanager.h \
			vcardupdate.h \
			xhtmlim.h

noinst_HEADERS = compressiondefault.h \
			compressionzlib.h \
			dns.h \
			nonsaslauth.h \
			mucmessagesession.h \
			prep.h \
			rosteritemdata.h \
			stanzaextensionfactory.h \
			tlsgnutlsbase.h \
			tlsgnutlsclient.h \
			tlsgnutlsclientanon.h \
			tlsgnutlsserveranon.h \
			tlsopenssl.h \
			tlsschannel.h

EXTRA_DIST = version.rc

test:
	make -C tests test

check: test

leaktest:
	make -C tests leaktest

leakcheck: leaktest

includetest:
	@for i in *.h; do \
	  echo -e "#include \"$$i\"\nint main() {}" >tmp.cpp; \
	  $(CXX) -I.. tmp.cpp 2>&1 | sed "s/^$$i://g" >$$i.test.log; \
	  test -s $$i.test.log && echo "INC $$i: failed" || rm $$i.test.log; \
	done
	@rm tmp.cpp
	@ls *.h.test.log &>/dev/null || echo "INC all: ok"
