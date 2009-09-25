include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../tag.cpp ../../stanza.cpp ../../prep.cpp ../../stanzaextensionfactory.cpp
                        ../../gloox.cpp ../../message.cpp ../../util.cpp ../../error.cpp ../../jid.cpp
                        ../../iq.cpp ../../base64.cpp ../../dataformfieldcontainer.cpp
                        ../../dataform.cpp ../../dataformfield.cpp
                        ../../dataformitem.cpp ../../dataformreported.cpp
                        ../../softwareversion.cpp )

include( ../CMakeLists_common_after.txt )

