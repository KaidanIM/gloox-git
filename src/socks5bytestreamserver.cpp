/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "socks5bytestreamserver.h"
#include "connectiontcpserver.h"

namespace gloox
{

  SOCKS5BytestreamServer::SOCKS5BytestreamServer( const LogSink& logInstance, const std::string& ip,
                                                  int port )
    : m_tcpServer( 0 ), m_logInstance( logInstance ), m_ip( ip ), m_port( port )
  {
    m_tcpServer = new ConnectionTCPServer( this, m_logInstance, m_ip, m_port );
  }

  SOCKS5BytestreamServer::~SOCKS5BytestreamServer()
  {
    if( m_tcpServer )
      delete m_tcpServer;

    ConnectionMap::const_iterator it = m_connections.begin();
    for( ; it != m_connections.end(); ++it )
      delete (*it).first;
  }

  ConnectionError SOCKS5BytestreamServer::listen()
  {
    if( m_tcpServer )
      return m_tcpServer->connect();

    return ConnNotConnected;
  }

  ConnectionError SOCKS5BytestreamServer::recv( int timeout )
  {
    if( m_tcpServer )
    {
      ConnectionError ce = m_tcpServer->recv( timeout );
      if( ce != ConnNoError )
        return ce;

      ConnectionMap::const_iterator it = m_connections.begin();
      for( ; it != m_connections.end(); ++it )
      {
        (*it).first->recv( timeout );
      }
    }

    return ConnNotConnected;
  }

  void SOCKS5BytestreamServer::stop()
  {
    if( m_tcpServer )
    {
      m_tcpServer->disconnect();
      m_tcpServer->cleanup();
    }
  }

  void SOCKS5BytestreamServer::handleIncomingConnection( ConnectionBase* connection )
  {
    printf( "got new connection: %s\n", connection->server().c_str() );
    connection->registerConnectionDataHandler( this );
    m_connections[connection] = StateUnnegotiated;
  }

  void SOCKS5BytestreamServer::handleReceivedData( const ConnectionBase* connection,
                                                   const std::string& data )
  {
    ConnectionMap::const_iterator it = m_connections.find( const_cast<ConnectionBase*>( connection ) );
    if( it == m_connections.end() )
      return;

    switch( (*it).second )
    {
      case StateDisconnected:
        (*it).first->disconnect();
        break;
      case StateUnnegotiated:
      {
        if( data.length() < 3 || data[0] != 0x05 )
        {
          char c[2];
          c[0] = 0x05;
          c[1] = 0xFF;
          (*it).first->send( std::string( c ) );
          return;
        }
        unsigned int sz = ( data.length() - 2 < (unsigned int)data[1] )
                            ? ( data.length() - 2 )
                            : ( (unsigned int)data[1] );
        for( unsigned int i = 1; i < sz; ++i )
        {
        }
        break;
      }
      case StateAuthmethodAccepted:
        break;
      case StateAuthAccepted:
        break;
      case StateDestinationAccepted:
        break;
      case StateActive:
        break;
    }
  }

  void SOCKS5BytestreamServer::handleConnect( const ConnectionBase* /*connection*/ )
  {
    // should never happen, TCP connection is already established
  }

  void SOCKS5BytestreamServer::handleDisconnect( const ConnectionBase* connection,
                                                       ConnectionError /*reason*/ )
  {
    m_connections.erase( const_cast<ConnectionBase*>( connection ) );
    delete connection;
  }

}
