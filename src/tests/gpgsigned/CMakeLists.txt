include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../gpgsigned.cpp ../../tag.cpp ../../gloox.cpp ../../util.cpp )

include( ../CMakeLists_common_after.txt )

