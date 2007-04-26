/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "socks5bytestream.h"
#include "socks5bytestreamdatahandler.h"
#include "clientbase.h"
#include "connectionbase.h"
#include "connectionsocks5proxy.h"
#include "sha.h"
#include "logsink.h"

#include <sstream>

namespace gloox
{

  SOCKS5Bytestream::SOCKS5Bytestream( SOCKS5BytestreamManager* manager, ConnectionBase* connection,
                                      LogSink& logInstance, const JID& initiator, const JID& target,
                                      const std::string& sid )
    : m_manager( manager ), m_connection( 0 ), m_socks5( 0 ), m_logInstance( logInstance ),
      m_socks5BytestreamDataHandler( 0 ), m_initiator( initiator ), m_target( target ),
      m_sid( sid ), m_open( false )
  {
    printf( "SOCKS5Bytestream::SOCKS5Bytestream\n" );

    setConnectionImpl( connection );
  }

  SOCKS5Bytestream::~SOCKS5Bytestream()
  {
    if( m_open )
      close();

    if( m_socks5 )
      delete m_socks5;
  }

  void SOCKS5Bytestream::setConnectionImpl( ConnectionBase* connection )
  {
    if( m_socks5 )
      delete m_socks5; // deletes m_connection as well

    m_connection = connection;

    printf( "socks5 host: %s%s%s\n", m_sid.c_str(), m_initiator.full().c_str(), m_target.full().c_str() );
    SHA sha;
    sha.feed( m_sid );
    sha.feed( m_initiator.full() );
    sha.feed( m_target.full() );
    sha.finalize();
    m_socks5 = new ConnectionSOCKS5Proxy( this, connection, m_logInstance, sha.hex(), 0 );
  }

  bool SOCKS5Bytestream::connect()
  {
    if( !m_connection || !m_socks5 || !m_manager )
      return false;

    printf( "SOCKS5Bytestream::connect\n" );

    StreamHostList::const_iterator it = m_hosts.begin();
    for( ; it != m_hosts.end(); ++it )
    {
      printf( "trying to connect to %s:%d\n", (*it).host.c_str(), (*it).port );
      m_connection->setServer( (*it).host, (*it).port );
      if( m_socks5->connect() == ConnNoError )
      {
        m_proxy = (*it).jid;
        return true;
      }
    }

    m_manager->acknowledgeStreamHost( false, JID(), std::string() );
    return false;
  }

  bool SOCKS5Bytestream::send( const std::string& data )
  {
    if( !m_open || !m_connection || !m_socks5 || !m_manager )
      return false;

    return m_socks5->send( data );
  }

  ConnectionError SOCKS5Bytestream::recv( int timeout )
  {
    if( !m_connection || !m_socks5 || !m_manager )
      return ConnNotConnected;

    return m_socks5->recv( timeout );
  }

  void SOCKS5Bytestream::closed()
  {
    m_open = false;

    if( m_socks5BytestreamDataHandler )
      m_socks5BytestreamDataHandler->handleSOCKS5Close( this );
  }

  void SOCKS5Bytestream::close()
  {
    m_open = false;
  }

  void SOCKS5Bytestream::handleReceivedData( const std::string& data )
  {
    if( !m_socks5BytestreamDataHandler )
      return;

    if( !m_open )
    {
      m_open = true;
      m_socks5BytestreamDataHandler->handleSOCKS5Open( this );
    }

//     if( !m_open && data.length() == 2 && data[0] == 0x05 && data[1] == 0x00 )
//     {
//       printf( "received acknowleding zero byte, stream is now open\n" );
//       m_open = true;
//       m_socks5BytestreamDataHandler->handleSOCKS5Open( this );
//       return;
//     }

    if( m_open )
      m_socks5BytestreamDataHandler->handleSOCKS5Data( this, data );
  }

  void SOCKS5Bytestream::handleConnect()
  {
    m_manager->acknowledgeStreamHost( true, m_proxy, m_sid );
  }

  void SOCKS5Bytestream::handleDisconnect( ConnectionError /*reason*/ )
  {
    if( m_socks5BytestreamDataHandler )
      m_socks5BytestreamDataHandler->handleSOCKS5Close( this );
  }

}
