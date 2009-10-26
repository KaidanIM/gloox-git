include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../tlsschannelclient.cpp ../../tlsschannelserver.cpp ../../tlsschannelbase.cpp ../../gloox.cpp ../../mutex.cpp)

include( ../CMakeLists_common_after.txt )

