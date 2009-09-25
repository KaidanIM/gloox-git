include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../tag.cpp ../../stanza.cpp
                        ../../gloox.cpp ../../iq.cpp ../../util.cpp
			../../error.cpp ../../jid.cpp ../../prep.cpp
			../../dataform.cpp ../../presence.cpp
                        ../../message.cpp ../../dataformfieldcontainer.cpp ../../dataformreported.cpp
			../../dataformitem.cpp ../../dataformfield.cpp
			../../softwareversion.cpp ../../stanzaextensionfactory.cpp
			../../delayeddelivery.cpp )

include( ../CMakeLists_common_after.txt )

