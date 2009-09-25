include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../stanza.cpp ../../jid.cpp ../../dataform.cpp ../../dataformfieldcontainer.cpp
 		../../dataformreported.cpp ../../dataformitem.cpp ../../dataformfield.cpp ../../tag.cpp ../../prep.cpp
 		../../gloox.cpp ../../stanzaextensionfactory.cpp ../../oob.cpp
		../../iq.cpp ../../util.cpp ../../sha.cpp ../../base64.cpp
		../../error.cpp )

include( ../CMakeLists_common_after.txt )

