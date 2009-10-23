include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../clientbase.cpp ../../jid.cpp ../../connectiontcpclient.cpp ../../connectiontcpbase.cpp
			../../disco.cpp ../../parser.cpp ../../tag.cpp ../../stanza.cpp ../../base64.cpp
			../../md5.cpp ../../tlsgnutlsclient.cpp ../../tlsopensslclient.cpp
                        ../../tlsschannelbase.cpp ../../tlsschannelclient.cpp ../../tlsschannelserver.cpp
			../../tlsgnutlsserver.cpp
			../../tlsopensslbase.cpp ../../tlsopensslserver.cpp
			../../logsink.cpp ../../messagesession.cpp ../../prep.cpp ../../compressionzlib.cpp
			../../dns.cpp ../../stanzaextensionfactory.cpp
			../../gloox.cpp ../../tlsgnutlsbase.cpp ../../tlsdefault.cpp ../../dataform.cpp
			../../dataformfieldcontainer.cpp ../../dataformreported.cpp ../../dataformitem.cpp
			../../dataformfield.cpp
			../../tlsgnutlsclientanon.cpp ../../tlsgnutlsserveranon.cpp
                        ../../mutex.cpp
			../../iq.cpp ../../presence.cpp ../../message.cpp ../../subscription.cpp ../../util.cpp
			../../sha.cpp ../../error.cpp ../../eventdispatcher.cpp ../../softwareversion.cpp ../../connectiontls.cpp
			../../connectioncompression.cpp ../../compressiondefault.cpp )

include( ../CMakeLists_common_after.txt )

