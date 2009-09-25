include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../tag.cpp ../../stanza.cpp ../../base64.cpp
			../../prep.cpp
			../../gloox.cpp
			../../iq.cpp ../../presence.cpp ../../message.cpp ../../subscription.cpp ../../util.cpp
			../../sha.cpp ../../error.cpp ../../jid.cpp ../../rosteritem.cpp ../../dataform.cpp
			../../dataformfieldcontainer.cpp ../../dataformreported.cpp ../../dataformitem.cpp
			../../dataformfield.cpp )

include( ../CMakeLists_common_after.txt )

