include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../tag.cpp ../../stanza.cpp ../../prep.cpp
			../../gloox.cpp ../../message.cpp ../../util.cpp ../../error.cpp ../../jid.cpp
			../../iq.cpp ../../base64.cpp ../../sha.cpp )

include( ../CMakeLists_common_after.txt )

