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

#include "connectiontcpclient.h"
#include "dns.h"
#include "logsink.h"
#include "mutex.h"
#include "mutexguard.h"

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

#include <cstdlib>
#include <string>

#ifndef _WIN32_WCE
# include <sstream>
#endif

namespace gloox
{

  ConnectionTCPClient::ConnectionTCPClient( const LogSink& logInstance,
                                            const std::string& server, int port )
    : ConnectionTCPBase( logInstance, server, port )
  {
  }

  ConnectionTCPClient::ConnectionTCPClient( ConnectionDataHandler *cdh, const LogSink& logInstance,
                                            const std::string& server, int port )
    : ConnectionTCPBase( cdh, logInstance, server, port )
  {
  }


  ConnectionTCPClient::~ConnectionTCPClient()
  {
  }

  ConnectionTCPClient* ConnectionTCPClient::newInstance() const
  {
    return new ConnectionTCPClient( m_handler, m_logInstance, m_server, m_port );
  }

  ConnectionError ConnectionTCPClient::connect()
  {
    MutexGuard mg( m_sendMutex );

    if( !m_handler || m_socket >= 0 )
      return ConnNotConnected;

    if( m_state > StateDisconnected )
      return ConnNoError;

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
          m_logInstance.log( LogLevelError, LogAreaClassConnectionTCPClient,
                             m_server + ": connection refused" );
          break;
        case -ConnDnsError:
          m_logInstance.log( LogLevelError, LogAreaClassConnectionTCPClient,
                             m_server + ": host not found" );
          break;
        default:
          m_logInstance.log( LogLevelError, LogAreaClassConnectionTCPClient,
                             "Unknown error condition" );
          break;
      }
      m_handler->handleDisconnect( (ConnectionError)-m_socket );
      return (ConnectionError)-m_socket;
    }
    else
    {
      m_state = StateConnected;
    }

    m_cancel = false;
    m_handler->handleConnect();
    return ConnNoError;
  }

  ConnectionError ConnectionTCPClient::recv( int timeout )
  {
    MutexGuard mg( m_recvMutex );

    if( m_cancel || m_socket < 0 )
      return ConnNotConnected;

    if( !dataAvailable( timeout ) )
      return ConnNoError;

#ifdef SKYOS
    int size = ::recv( m_socket, (unsigned char*)m_buf, m_bufsize, 0 );
#else
    int size = ::recv( m_socket, m_buf, m_bufsize, 0 );
#endif

    mg.unlock();

    if( size <= 0 )
    {
      ConnectionError error = ( size ? ConnIoError : ConnStreamClosed );
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

}
