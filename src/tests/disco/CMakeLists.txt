include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../tag.cpp ../../stanza.cpp
			../../prep.cpp
			../../gloox.cpp
			../../iq.cpp ../../util.cpp
			../../error.cpp ../../jid.cpp ../../dataform.cpp
			../../dataformfieldcontainer.cpp ../../dataformreported.cpp ../../dataformitem.cpp
			../../dataformfield.cpp ../../softwareversion.cpp )

include( ../CMakeLists_common_after.txt )

