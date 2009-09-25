include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../tag.cpp ../../stanza.cpp
 				../../jid.cpp ../../prep.cpp ../../gloox.cpp
				../../message.cpp ../../util.cpp
				../../sha.cpp ../../base64.cpp ../../messageevent.cpp )

include( ../CMakeLists_common_after.txt )

