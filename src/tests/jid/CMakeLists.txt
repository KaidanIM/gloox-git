include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../jid.cpp ../../prep.cpp ../../gloox.cpp ../../util.cpp )

add_executable( ${dirname}_perf ${perf_SRCS} ../../jid.cpp ../../prep.cpp ../../gloox.cpp ../../util.cpp )
target_link_libraries( ${dirname}_perf ${LIBS} )

include( ../CMakeLists_common_after.txt )

