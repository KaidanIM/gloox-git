include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../tag.cpp ../../dataformfield.cpp ../../util.cpp ../../gloox.cpp )

include( ../CMakeLists_common_after.txt )

