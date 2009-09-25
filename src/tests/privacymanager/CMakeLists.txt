include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../jid.cpp ../../tag.cpp
                        ../../logsink.cpp ../../prep.cpp ../../util.cpp
                        ../../gloox.cpp ../../iq.cpp ../../stanza.cpp
                        ../../error.cpp ../../privacyitem.cpp )

include( ../CMakeLists_common_after.txt )

