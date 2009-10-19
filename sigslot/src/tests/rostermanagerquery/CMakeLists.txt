include( ../CMakeLists_common_before.txt )

add_executable( ${dirname}_test ${test_SRCS} ../../tag.cpp ../../stanza.cpp
                        ../../gloox.cpp ../../iq.cpp ../../util.cpp
			../../error.cpp ../../jid.cpp ../../prep.cpp
			../../dataform.cpp ../../presence.cpp
                        ../../message.cpp ../../subscription.cpp ../../dataformfieldcontainer.cpp ../../dataformreported.cpp
			../../dataformitem.cpp ../../dataformfield.cpp
			../../softwareversion.cpp ../../stanzaextensionfactory.cpp
			../../rosteritem.cpp
			../../privacyitem.cpp )

include( ../CMakeLists_common_after.txt )

