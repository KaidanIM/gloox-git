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
# include <winsock.h>
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

  ConnectionTCP::ConnectionTCP( const LogSink& logInstance,
                                const std::string& server, int port )
    : ConnectionBase( 0 ),
      m_logInstance( logInstance ),
      m_buf( 0 ), m_server( prep::idna( server ) ), m_port( port ), m_socket( -1 ),
      m_totalBytesIn( 0 ), m_totalBytesOut( 0 ),
      m_bufsize( 1024 ), m_cancel( true )
  {
    m_buf = (char*)calloc( m_bufsize + 1, sizeof( char ) );
  }

  ConnectionTCP::ConnectionTCP( ConnectionDataHandler *cdh, const LogSink& logInstance,
                                const std::string& server, int port )
    : ConnectionBase( cdh ),
      m_logInstance( logInstance ),
      m_buf( 0 ), m_server( prep::idna( server ) ), m_port( port ), m_socket( -1 ),
      m_totalBytesIn( 0 ), m_totalBytesOut( 0 ),
      m_bufsize( 1024 ), m_cancel( true )
  {
    m_buf = (char*)calloc( m_bufsize + 1, sizeof( char ) );
  }

  ConnectionTCP::~ConnectionTCP()
  {
    free( m_buf );
    m_buf = 0;
  }

  ConnectionError ConnectionTCP::connect()
  {
    m_state = StateConnecting;

    if( m_socket < 0 )
    {
      if( m_port == -1 )
        m_socket = DNS::connect( m_server, m_logInstance );
      else
        m_socket = DNS::connect( m_server, m_port, m_logInstance );
    }

    if( m_socket < 0 )
    {
      switch( m_socket )
      {
        case -ConnConnectionRefused:
          m_logInstance.log( LogLevelError, LogAreaClassConnectionTCP, m_server + ": connection refused" );
          break;
        case -ConnDnsError:
          m_logInstance.log( LogLevelError, LogAreaClassConnectionTCP, m_server + ": host not found" );
          break;
        default:
          m_logInstance.log( LogLevelError, LogAreaClassConnectionTCP, "Unknown error condition" );
          break;
      }
      if( m_handler )
        m_handler->handleDisconnect( (ConnectionError)-m_socket );
      return (ConnectionError)-m_socket;
    }
    else
    {
      m_state = StateConnected;
    }

    m_cancel = false;
    if( m_handler )
      m_handler->handleConnect();
    return ConnNoError;
  }

  void ConnectionTCP::disconnect()
  {
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

    return ( ( select( m_socket + 1, &fds, 0, 0, timeout == -1 ? 0 : &tv ) > 0 )
         && FD_ISSET( m_socket, &fds ) != 0 );
  }

  ConnectionError ConnectionTCP::recv( int timeout )
  {
    if( m_cancel || m_socket < 0 )
      return ConnNotConnected;

    if( !dataAvailable( timeout ) )
      return ConnNoError;

#ifdef SKYOS
    int size = ::recv( m_socket, (unsigned char*)m_buf, m_bufsize, 0 );
#else
    int size = ::recv( m_socket, m_buf, m_bufsize, 0 );
#endif

    if( size <= 0 )
    {
      ConnectionError error = size ? ConnIoError : ConnStreamClosed;
      if( m_handler )
        m_handler->handleDisconnect( error );
      return error;
    }

    m_buf[size] = '\0';

    m_totalBytesIn += size;
    if( m_handler )
      m_handler->handleReceivedData( std::string( m_buf, size ) );

    return ConnNoError;
  }

  ConnectionError ConnectionTCP::receive()
  {
    if( m_socket < 0 )
      return ConnNotConnected;

    ConnectionError err = ConnNoError;
    while( !m_cancel && ( err = recv( 10 ) ) == ConnNoError )
      ;
    return err == ConnNoError ? ConnNotConnected : err;
  }

  bool ConnectionTCP::send( const std::string& data )
  {
    if( data.empty() || ( m_socket < 0 ) )
      return false;

    int sent = 0;
    for( size_t num = 0, len = data.length(); sent != -1 && num < len; num += len )
    {
#ifdef SKYOS
      sent = ::send( m_socket, (unsigned char*)(data.c_str()+num), len - num, 0 );
#else
      sent = ::send( m_socket, (data.c_str()+num), len - num, 0 );
#endif
    }

    m_totalBytesOut += data.length();
    if( sent == -1 && m_handler )
      m_handler->handleDisconnect( ConnStreamClosed );

    return sent != -1;
  }

  void ConnectionTCP::getStatistics( int &totalIn, int &totalOut )
  {
    totalIn = m_totalBytesIn;
    totalOut = m_totalBytesOut;
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
    m_cancel = true;
  }

}
