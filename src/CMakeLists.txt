file( GLOB gloox_SRCS *.cpp )

add_library( gloox SHARED ${gloox_SRCS} )
include_directories( ${gloox_SOURCE_DIR} )
