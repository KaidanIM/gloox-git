include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../gloox.cpp ../../tag.cpp
                  ../../util.cpp ../../stanza.cpp ../../message.cpp
                  ../../jid.cpp ../../prep.cpp
                  ../../stanzaextensionfactory.cpp ../../iq.cpp )

include( ../CMakeLists_common_after.txt )

