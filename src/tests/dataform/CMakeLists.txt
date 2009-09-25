include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../tag.cpp ../../dataform.cpp ../../dataformfieldcontainer.cpp ../../dataformreported.cpp ../../dataformitem.cpp ../../dataformfield.cpp ../../gloox.cpp ../../util.cpp )

include( ../CMakeLists_common_after.txt )

