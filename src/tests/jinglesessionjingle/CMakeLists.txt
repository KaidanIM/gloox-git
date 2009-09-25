include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../stanza.cpp ../../jid.cpp ../../tag.cpp ../../prep.cpp
 		../../gloox.cpp ../../stanzaextensionfactory.cpp
		../../iq.cpp ../../util.cpp ../../sha.cpp ../../base64.cpp
		../../jinglecontent.cpp ../../error.cpp ../../jingledtmf.cpp )

include( ../CMakeLists_common_after.txt )

