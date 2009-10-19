include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../dns.cpp ../../asyncdns.cpp
			../../mutex.cpp ../../thread.cpp ../../util.cpp
			../../gloox.cpp ../../logsink.cpp  )

include( ../CMakeLists_common_after.txt )

