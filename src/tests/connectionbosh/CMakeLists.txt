include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../connectionbosh.cpp ../../parser.cpp ../../tag.cpp ../../logsink.cpp
                            ../../gloox.cpp ../../prep.cpp ../../util.cpp )

include( ../CMakeLists_common_after.txt )

