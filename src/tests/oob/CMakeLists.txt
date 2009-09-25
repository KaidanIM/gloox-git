include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../oob.cpp ../../tag.cpp ../../gloox.cpp ../../iq.cpp ../../stanzaextensionfactory.cpp
                 ../../stanza.cpp ../../util.cpp ../../jid.cpp ../../prep.cpp )

include( ../CMakeLists_common_after.txt )

