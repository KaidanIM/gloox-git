include( FindZLIB )

file( GLOB gloox_SRCS *.cpp )

option( SETSOCKOPTS_EXISTS "We have setsockopts()" ON )

if( SETSOCKOPTS_EXISTS )
	write_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h "#define HAVE_SETSOCKOPT 1")
endif( SETSOCKOPTS_EXISTS )

if( ZLIB_FOUND )
    set( LIBS ${LIBS} ${ZLIB_LIBRARIES} )
    set( INCLUDE_DIRS ${INCLUDE_DIRS} ${ZLIB_INCLUDE_DIR} ) 
    write_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h "#define HAVE_ZLIB 1" APPEND )
endif( ZLIB_FOUND) 
add_library( gloox SHARED ${gloox_SRCS} )
target_link_libraries( gloox ${LIBS} )
include_directories( ${gloox_SOURCE_DIR} )
