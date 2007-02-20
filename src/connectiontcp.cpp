/*
  Copyright (c) 2004-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#include "gloox.h"

#include "connectiontcp.h"
#include "dns.h"
#include "logsink.h"
#include "prep.h"

#ifdef __MINGW32__
#include <winsock.h>
#endif

#if !defined( WIN32 ) && !defined( _WIN32_WCE )
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/select.h>
# include <unistd.h>
#else
# include <winsock.h>
#endif

#include <time.h>

#include <string>

#ifndef _WIN32_WCE
# include <sstream>
#endif

namespace gloox
{

  ConnectionTCP::ConnectionTCP( ConnectionDataHandler *cdh, const LogSink& logInstance,
                                const std::string& server, unsigned short port )
    : ConnectionBase( cdh ),
      m_logInstance( logInstance ),
      m_buf( 0 ), m_server( Prep::idna( server ) ), m_port( port ), m_socket( -1 ),
      m_totalBytesIn( 0 ), m_totalBytesOut( 0 ),
      m_bufsize( 1024 ), m_cancel( true )
  {
    m_buf = (char*)calloc( m_bufsize + 1, sizeof( char ) );
  }

  ConnectionTCP::~ConnectionTCP()
  {
    cleanup();
    free( m_buf );
    m_buf = 0;
  }

  ConnectionError ConnectionTCP::connect()
  {
    m_state = StateConnecting;

    if( m_socket < 0 )
    {
      if( m_port == (unsigned short)-1 )
        m_socket = DNS::connect( m_server, m_logInstance );
      else
        m_socket = DNS::connect( m_server, m_port, m_logInstance );
    }

    if( m_socket < 0 )
    {
      switch( m_socket )
      {
        case -ConnConnectionRefused:
          m_logInstance.log( LogLevelError, LogAreaClassConnection, m_server + ": connection refused" );
          break;
        case -ConnDnsError:
          m_logInstance.log( LogLevelError, LogAreaClassConnection, m_server + ": host not found" );
          break;
        default:
          m_logInstance.log( LogLevelError, LogAreaClassConnection, "Unknown error condition" );
          break;
      }
      ConnectionError e = (ConnectionError)-m_socket;
      cleanup();
      return e;
    }
    else
    {
      m_state = StateConnected;
    }

    m_cancel = false;
    return ConnNoError;
  }

  void ConnectionTCP::disconnect( ConnectionError e )
  {
    m_disconnect = e;
    m_cancel = true;
  }

  bool ConnectionTCP::dataAvailable( int timeout )
  {
    fd_set fds;
    struct timeval tv;

    FD_ZERO( &fds );
    FD_SET( m_socket, &fds );

    tv.tv_sec = timeout / 1000;
    tv.tv_usec = timeout % 1000;

    if( select( m_socket + 1, &fds, 0, 0, timeout == -1 ? 0 : &tv ) >= 0 )
    {
      return FD_ISSET( m_socket, &fds ) ? true : false;
    }
    return false;
  }

  ConnectionError ConnectionTCP::recv( int timeout )
  {
    if( m_cancel )
    {
      ConnectionError e = m_disconnect;
      cleanup();
      return e;
    }

    if( m_socket < 0 )
      return ConnNotConnected;

    if( !dataAvailable( timeout ) )
    {
      return ConnNoError;
    }

    // optimize(?): recv returns the size. set size+1 = \0
    memset( m_buf, '\0', m_bufsize + 1 );
    int size = 0;
    {
#ifdef SKYOS
      size = ::recv( m_socket, (unsigned char*)m_buf, m_bufsize, 0 );
#else
      size = ::recv( m_socket, m_buf, m_bufsize, 0 );
#endif
    }

    if( size < 0 )
    {
      // error
      return ConnIoError;
    }
    else if( size == 0 )
    {
      // connection closed
      return ConnUserDisconnected;
    }

    std::string buf;
    buf.assign( m_buf, size );

    if( m_handler )
      m_handler->handleReceivedData( buf );
    return ConnNoError;
  }

  ConnectionError ConnectionTCP::receive()
  {
    if( m_socket < 0 )
      return ConnNotConnected;

    while( !m_cancel )
    {
      ConnectionError r = recv( 10 );
      if( r != ConnNoError )
      {
        return r;
      }
    }
    ConnectionError e = m_disconnect;
    cleanup();

    return e;
  }

  bool ConnectionTCP::send( const std::string& data )
  {
    if( data.empty() || ( m_socket < 0 ) )
      return false;

    size_t num = 0;
    size_t len = data.length();
    while( num < len )
    {
#ifdef SKYOS
      int sent = ::send( m_socket, (unsigned char*)(data.c_str()+num), len - num, 0 );
#else
      int sent = ::send( m_socket, (data.c_str()+num), len - num, 0 );
#endif
      if( sent == -1 )
        return false;

      num += sent;
    }

    return true;
  }

  void ConnectionTCP::cleanup()
  {
    if( m_socket >= 0 )
    {
#ifdef WIN32
      closesocket( m_socket );
#else
      close( m_socket );
#endif
      m_socket = -1;
    }
    m_state = StateDisconnected;
    m_disconnect = ConnNoError;
    m_cancel = true;
  }

}
