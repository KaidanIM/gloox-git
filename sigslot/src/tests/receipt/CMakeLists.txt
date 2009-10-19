include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../receipt.cpp ../../gloox.cpp ../../tag.cpp
                  ../../util.cpp ../../stanza.cpp ../../message.cpp
                  ../../jid.cpp ../../prep.cpp
                  ../../stanzaextensionfactory.cpp )

include( ../CMakeLists_common_after.txt )

