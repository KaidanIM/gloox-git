include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../dataformreported.cpp ../../tag.cpp
		../../dataform.cpp ../../gloox.cpp ../../dataformfieldcontainer.cpp ../../dataformfield.cpp ../../dataformitem.cpp
		../../util.cpp )

include( ../CMakeLists_common_after.txt )

