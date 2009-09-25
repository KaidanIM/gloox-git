include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../tag.cpp ../../stanza.cpp ../../prep.cpp ../../stanzaextensionfactory.cpp
			../../gloox.cpp ../../base64.cpp ../../util.cpp ../../sha.cpp
                        ../../jid.cpp ../../iq.cpp ../../error.cpp ../../softwareversion.cpp
                        ../../dataform.cpp ../../dataformfieldcontainer.cpp ../../dataformreported.cpp
                        ../../dataformitem.cpp ../../dataformfield.cpp )

include( ../CMakeLists_common_after.txt )

