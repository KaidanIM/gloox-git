include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../tag.cpp ../../stanza.cpp ../../stanzaextensionfactory.cpp
 				../../jid.cpp ../../prep.cpp
				../../message.cpp ../../util.cpp
				../../gloox.cpp ../../chatstate.cpp )

include( ../CMakeLists_common_after.txt )

