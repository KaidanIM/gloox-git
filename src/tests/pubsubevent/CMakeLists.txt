include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../gloox.cpp ../../tag.cpp ../../jid.cpp ../../prep.cpp
                           ../../util.cpp ../../error.cpp ../../pubsubevent.cpp
                           ../../dataform.cpp ../../dataformfield.cpp
                           ../../dataformfieldcontainer.cpp ../../dataformreported.cpp
                           ../../dataformitem.cpp )

include( ../CMakeLists_common_after.txt )

