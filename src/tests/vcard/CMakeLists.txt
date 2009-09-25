include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../vcard.cpp ../../gloox.cpp ../../tag.cpp ../../util.cpp ../../iq.cpp
                   ../../stanzaextensionfactory.cpp ../../base64.cpp ../../stanza.cpp
                   ../../jid.cpp ../../prep.cpp )

include( ../CMakeLists_common_after.txt )

