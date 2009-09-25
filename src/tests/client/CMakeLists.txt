include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../client.cpp ../../clientbase.cpp ../../connectiontcpbase.cpp ../../connectiontcpclient.cpp
			../../disco.cpp ../../parser.cpp ../../tag.cpp ../../stanza.cpp ../../base64.cpp ../../jid.cpp
			../../md5.cpp ../../tlsgnutlsclient.cpp
			../../tlsopensslclient.cpp ../../tlsschannel.cpp
			../../tlsgnutlsserver.cpp
			../../tlsopensslbase.cpp ../../tlsopensslserver.cpp
			../../logsink.cpp ../../messagesession.cpp ../../prep.cpp ../../compressionzlib.cpp
			../../dns.cpp ../../stanzaextensionfactory.cpp
			../../rostermanager.cpp ../../nonsaslauth.cpp ../../sha.cpp ../../dataform.cpp
			../../dataformfieldcontainer.cpp ../../dataformreported.cpp ../../dataformitem.cpp
			../../dataformfield.cpp
			../../rosteritem.cpp ../../privatexml.cpp ../../gloox.cpp ../../tlsgnutlsbase.cpp
			../../tlsdefault.cpp ../../tlsgnutlsclientanon.cpp ../../tlsgnutlsserveranon.cpp
			../../mutex.cpp ../../iq.cpp ../../presence.cpp ../../message.cpp ../../subscription.cpp
			../../util.cpp ../../error.cpp ../../capabilities.cpp ../../eventdispatcher.cpp
			../../softwareversion.cpp ../../connectiontls.cpp
			../../connectioncompression.cpp ../../compressiondefault.cpp )

include( ../CMakeLists_common_after.txt )

