include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../parser.cpp ../../tag.cpp ../../util.cpp ../../gloox.cpp )

include( ../CMakeLists_common_after.txt )

