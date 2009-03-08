cmake_minimum_required( VERSION 2.6 )

set( GLOOX_ENABLE_MAINTAINER false CACHE BOOL "Enable Maintainer Mode" )
set( GLOOX_ENABLE_GETADDRINFO false CACHE BOOL "Enable getaddrinfo()" )
if( WIN32 )
        set( GLOOX_WITH_SCHANNEL false CACHE BOOL "Enable SChannel" )
endif( WIN32 )

set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/" )

if( GLOOX_ENABLE_MAINTAINER )
        set( CMAKE_CXX_FLAGS "-Wshadow -Werror" CACHE STRING "Maintainer Flags" FORCE )
endif( GLOOX_ENABLE_MAINTAINER )
set( CMAKE_CXX_FLAGS_DEBUG "-g3 -fno-inline -DDEBUG" CACHE STRING "Debug Flags" FORCE )

project( gloox )

find_package( GNUTLS )
find_package( OpenSSL )
find_package( ZLIB )
find_package( IDN )
find_package( Threads )

include( CheckFunctionExists )

## Check functions
check_function_exists( setsockopt SETSOCKOPT_EXISTS )
if(NOT WIN32)
#SET( CMAKE_REQUIRED_FLAGS "-fno-builtin" )
SET( CMAKE_REQUIRED_LIBRARIES "resolv" ) # FIXME
        check_function_exists( res_querydomain QUERYDOMAIN_EXISTS )
        check_function_exists( dn_skipname SKIPNAME_EXISTS )
        check_function_exists( res_query QUERY_EXISTS )
endif(NOT WIN32)

if( GLOOX_ENABLE_GETADDRINFO )
        check_function_exists( getaddrinfo GETADDRINFO_EXISTS )
        if( GETADDRINFO_EXISTS )
                set( HAVE_GETADDRINFO true )
        endif( GETADDRINFO_EXISTS )
endif( GLOOX_ENABLE_GETADDRINFO )

if( SETSOCKOPT_EXISTS )
        set( HAVE_SETSOCKOPT true )
endif( SETSOCKOPT_EXISTS )


## Zlib
if( ZLIB_FOUND )
        list( APPEND LIBS ${ZLIB_LIBRARIES} )
        list( APPEND INCLUDE_DIRS ${ZLIB_INCLUDE_DIR} )
        set( HAVE_ZLIB true )
endif( ZLIB_FOUND )

## SSL/TLS
if( GNUTLS_FOUND )
        list( APPEND LIBS ${GNUTLS_LIBRARIES} )
        list( APPEND INCLUDE_DIRS ${GNUTLS_INCLUDE_DIRS} )
        set( HAVE_GNUTLS true )
else( GNUTLS_FOUND )
        if( OPENSSL_FOUND )
                list( APPEND LIBS ${OPENSSL_LIBRARIES} )
                list( APPEND INCLUDE_DIRS ${OPENSSL_INCLUDE_DIR} )
                set( HAVE_OPENSSL true )
        endif( OPENSSL_FOUND )
endif( GNUTLS_FOUND )

## IDN
if( IDN_FOUND )
        add_definitions( ${IDN_DEFINITIONS} )
        list( APPEND LIBS ${IDN_LIBRARIES} )
        list( APPEND INCLUDE_DIRS ${IDN_INCLUDE_DIR} )
        set( HAVE_LIBIDN true )
endif( IDN_FOUND )

## Windows/Pthreads
if( WIN32 )
        list( APPEND LIBS "-lwsock32 -lgdi32 -ldnsapi" )
        if( GLOOX_WITH_SCHANNEL )
                list( LIBS APPEND "-lcrypt32 -lsecur32" )
        endif( GLOOX_WITH_SCHANNEL )
        set( HAVE_WINTLS true )
        set( HAVE_WINDNS_H true )
else( WIN32 )
        if( NOT CMAKE_USE_PTHREADS_INIT )
                message( FATAL_ERROR "we do need pthread support!" )
        endif( NOT CMAKE_USE_PTHREADS_INIT )
endif( WIN32 )

CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/config.h.cmake ${CMAKE_BINARY_DIR}/config.h )

add_subdirectory( src )
