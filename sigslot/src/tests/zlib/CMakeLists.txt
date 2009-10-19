include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../compressionzlib.cpp ../../mutex.cpp )

add_executable( ${dirname}_perf ${perf_SRCS} ../../compressionzlib.cpp ../../mutex.cpp )
target_link_libraries( ${dirname}_perf ${LIBS} )

include( ../CMakeLists_common_after.txt )

