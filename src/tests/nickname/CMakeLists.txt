include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../nickname.cpp ../../gloox.cpp ../../tag.cpp ../../util.cpp )

include( ../CMakeLists_common_after.txt )

