include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../tag.cpp ../../stanza.cpp ../../base64.cpp
			../../prep.cpp ../../gloox.cpp
			../../iq.cpp ../../util.cpp
			../../sha.cpp ../../error.cpp ../../jid.cpp
			../../jinglecontent.cpp ../../jingledtmf.cpp )

include( ../CMakeLists_common_after.txt )

