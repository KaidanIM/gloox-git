file( GLOB gloox_SRCS *.cpp )

add_library( gloox SHARED ${gloox_SRCS} )
target_link_libraries( gloox ${LIBS} )
include_directories( ${gloox_SOURCE_DIR} )

add_subdirectory( tests )
