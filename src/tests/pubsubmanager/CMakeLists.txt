include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../gloox.cpp ../../tag.cpp ../../iq.cpp
				 ../../jid.cpp ../../prep.cpp
				 ../../stanza.cpp ../../util.cpp
                                 ../../error.cpp
				 ../../dataform.cpp
                                 ../../dataformfield.cpp
				 ../../dataformfieldcontainer.cpp
                                 ../../dataformitem.cpp
				 ../../dataformreported.cpp
				 ../../pubsubitem.cpp ../../shim.cpp
				 ../../mutex.cpp )

include( ../CMakeLists_common_after.txt )

