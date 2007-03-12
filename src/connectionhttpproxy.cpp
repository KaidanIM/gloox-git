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

#include "connectionhttpproxy.h"
#include "dns.h"
#include "logsink.h"
#include "prep.h"

#include <string>

#ifndef _WIN32_WCE
# include <sstream>
#endif

namespace gloox
{

  ConnectionHTTPProxy::ConnectionHTTPProxy( ConnectionDataHandler *cdh, ConnectionBase *connection,
                                            const LogSink& logInstance,
                                            const std::string& server, unsigned short port )
    : ConnectionBase( cdh ), m_connection( connection ),
      m_logInstance( logInstance ),
      m_server( Prep::idna( server ) ), m_port( port ),
      m_totalBytesIn( 0 ), m_totalBytesOut( 0 ), m_proxied( false )
  {
    if( m_connection )
      m_connection->registerConnectionDataHandler( this );
  }

  ConnectionHTTPProxy::~ConnectionHTTPProxy()
  {
    if( m_connection )
      delete m_connection;
  }

  ConnectionError ConnectionHTTPProxy::connect()
  {
    if( m_connection )
      return m_connection->connect();

    return ConnNotConnected;
  }

  void ConnectionHTTPProxy::disconnect()
  {
    if( m_connection )
      m_connection->disconnect();
  }

  ConnectionError ConnectionHTTPProxy::recv( int timeout )
  {
    if( m_connection )
      return m_connection->recv( timeout );
    else
      return ConnNotConnected;
  }

  ConnectionError ConnectionHTTPProxy::receive()
  {
    if( m_connection )
      return m_connection->receive();
    else
      return ConnNotConnected;
  }

  bool ConnectionHTTPProxy::send( const std::string& data )
  {
    if( m_connection )
      return m_connection->send( data );

    return false;
  }

  void ConnectionHTTPProxy::cleanup()
  {
    m_proxied = false;

    if( m_connection )
      m_connection->cleanup();
  }

  void ConnectionHTTPProxy::handleReceivedData( const std::string& data )
  {
    if( !m_proxied && m_handler )
    {
      m_proxyHandshakeBuffer += data;
      if( ( m_proxyHandshakeBuffer.substr( 0, 12 ) == "HTTP/1.0 200"
            || m_proxyHandshakeBuffer.substr( 0, 12 ) == "HTTP/1.1 200" )
          && m_proxyHandshakeBuffer.substr( m_proxyHandshakeBuffer.length() - 4 ) == "\r\n\r\n" )
      {
        m_proxyHandshakeBuffer = "";
        m_proxied = true;
        m_handler->handleConnect();
      }
      else if( m_proxyHandshakeBuffer.substr( 9, 3 ) == "407" )
        m_handler->handleDisconnect( ConnProxyAuthRequired );
      else if( m_proxyHandshakeBuffer.substr( 9, 3 ) == "403" ||
               m_proxyHandshakeBuffer.substr( 9, 3 ) == "404" )
        m_handler->handleDisconnect( ConnProxyAuthFailed );
    }
    else if( m_handler )
      m_handler->handleReceivedData( data );
  }

  void ConnectionHTTPProxy::handleConnect()
  {
    if( m_connection )
    {
      std::ostringstream os;
      os << "CONNECT " << m_server << ":" << m_port << " HTTP/1.0\r\n";
      os << "Host: " << m_server << "\r\n";
      os << "Content-Length: 0\r\n";
      os << "Proxy-Connection: Keep-Alive\r\n";
      os << "Pragma: no-cache\r\n";
      os << "\r\n";

      if( !m_connection->send( os.str() ) && m_handler )
        m_handler->handleDisconnect( ConnIoError );
    }
  }

  void ConnectionHTTPProxy::handleDisconnect( ConnectionError reason )
  {
    m_proxied = false;

    if( m_handler )
      m_handler->handleDisconnect( reason );
  }

}
