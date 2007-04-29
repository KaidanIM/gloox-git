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

  SOCKS5BytestreamServer::SOCKS5BytestreamServer( const LogSink& logInstance, int port,
                                                  const std::string& ip )
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
      return ConnNoError;
    }
    else
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

  ConnectionBase* SOCKS5BytestreamServer::getConnection( const std::string& hash )
  {
    ConnectionMap::iterator it = m_connections.begin();
    for( ; it != m_connections.end(); ++it )
    {
      if( (*it).second.hash == hash )
      {
        ConnectionBase* conn = (*it).first;
        m_connections.erase( it );
        return conn;
      }
    }

    return 0;
  }

  void SOCKS5BytestreamServer::handleIncomingConnection( ConnectionBase* connection )
  {
    printf( "got new connection: %s\n", connection->server().c_str() );
    connection->registerConnectionDataHandler( this );
    ConnectionInfo ci;
    ci.state = StateUnnegotiated;
    m_connections[connection] = ci;
  }

  void SOCKS5BytestreamServer::handleReceivedData( const ConnectionBase* connection,
                                                   const std::string& data )
  {
      printf( "data recv: " );
      const char* x = data.c_str();
      for( unsigned int i = 0; i < data.length(); ++i )
        printf( "%02X ", (const char)x[i] );
      printf( "\n" );

    ConnectionMap::iterator it = m_connections.find( const_cast<ConnectionBase*>( connection ) );
    if( it == m_connections.end() )
      return;

    switch( (*it).second.state )
    {
      case StateDisconnected:
        (*it).first->disconnect();
        break;
      case StateUnnegotiated:
      {
        char c[2];
        c[0] = 0x05;
        c[1] = (char)0xFF;
        (*it).second.state = StateDisconnected;

        if( data.length() >= 3 && data[0] == 0x05 )
        {
          printf( "sizeof char: %d\n", sizeof( char ) );
          printf( "data.length() >= 3 && data[0] == 0x05\n" );
          unsigned int sz = ( data.length() - 2 < (unsigned int)data[1] )
                              ? ( data.length() - 2 )
                              : ( (unsigned int)data[1] );
          printf( "checking 2 to %d\n", sz );
          for( unsigned int i = 2; i < sz + 2; ++i )
          {
            printf( "checking data[%d]: %02X\n", i, data[i] );
            if( data[i] == 0x00 )
            {
              printf( "data[%d] is 0x00\n", i );
              c[1] = 0x00;
              (*it).second.state = StateAuthAccepted;
              break;
            }
          }
        }
        /*(*it).first->*/send( (*it).first, std::string( c, 2 ) );
        break;
      }
      case StateAuthmethodAccepted:
        // place to implement any future auth support
        break;
      case StateAuthAccepted:
      {
        std::string reply = data;
        if( reply.length() < 2 )
          reply.resize( 2 );

        reply[0] = 0x05;
        reply[1] = 0x01; // general SOCKS server failure
        (*it).second.state = StateDisconnected;

        if( data.length() == 47 && data[0] == 0x05 && data[1] == 0x01 && data[2] == 0x00
            && data[3] == 0x03 && data[4] == 0x28 && data[45] == 0x00 && data[46] == 0x00 )
        {
          const std::string hash = data.substr( 5, 40 );
          printf( "hash: %s\n", hash.c_str() );

          HashMap::const_iterator ith = m_hashes.begin();
          for( ; ith != m_hashes.end() && (*ith) != hash; ++ith )
            ;

          if( ith != m_hashes.end() )
          {
            reply[1] = 0x00;
            (*it).second.hash = hash;
            (*it).second.state = StateDestinationAccepted;
          }
          else
            printf( "hash not found\n" );
        }
        /*(*it).first->*/send( (*it).first, reply );
        break;
      }
      case StateDestinationAccepted:
      case StateActive:
        // should not happen
        break;
    }
  }

  void SOCKS5BytestreamServer::send( ConnectionBase* connection, const std::string& data )
  {
      printf( "s data sent: " );
      const char* x = data.c_str();
      for( unsigned int i = 0; i < data.length(); ++i )
        printf( "%02X ", (const char)x[i] );
      printf( "\n" );

    connection->send( data );
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
