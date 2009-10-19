include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../delayeddelivery.cpp ../../tag.cpp
		../../jid.cpp ../../prep.cpp ../../gloox.cpp ../../util.cpp )

include( ../CMakeLists_common_after.txt )

