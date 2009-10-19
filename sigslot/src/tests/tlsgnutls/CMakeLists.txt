include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../tlsgnutlsclientanon.cpp ../../tlsgnutlsserveranon.cpp ../../tlsgnutlsbase.cpp ../../gloox.cpp )

include( ../CMakeLists_common_after.txt )

