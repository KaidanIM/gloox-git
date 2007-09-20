/*
 * Copyright (c) 2004-2007 by Jakob Schroeter <js@camaya.net>
 * This file is part of the gloox library. http://camaya.net/gloox
 *
 * This software is distributed under a license. The full license
 * agreement can be found in the file LICENSE in this distribution.
 * This software may not be copied, modified, sold or distributed
 * other than expressed in the named license agreement.
 *
 * This software is distributed without any warranty.
 */

#include "gloox.h"
#include "connectiontls.h"
#include "tlsdefault.h"

namespace gloox
{

  ConnectionTLS::ConnectionTLS( ConnectionBase* conn, ConnectionDataHandler* cdh, const LogSink& log )
    : ConnectionBase( this ),
      m_connection( conn ), m_log( log ), m_handshaked( false )
  {
    m_handler = cdh;
    if( conn )
      conn->registerConnectionDataHandler( this );
  }

  ConnectionTLS::~ConnectionTLS()
  {
    delete m_connection;
  }

/**************** ConnectionBase methods ****************/

  ConnectionError ConnectionTLS::connect()
  {
    if( !m_connection )
    {
      return ConnNotConnected;
    }
    else
    {
      m_tls = new TLSDefault( this, m_connection->server() );
      m_state = StateConnecting;
      return m_connection->connect();
    }
  }

  ConnectionError ConnectionTLS::recv( int timeout )
  {
    if( m_connection->state() == StateConnected )
    {
      return m_connection->recv( timeout );
    }
    else
    {
      m_log.log( LogLevelWarning, LogAreaClassConnectionTLS,
                 "Attempt to receive data on a connection that is not connected (or is connecting)" );
      return ConnNotConnected;
    }
  }

  bool ConnectionTLS::send( const std::string& data )
  {
    if( m_handshaked && m_state == StateConnected )
    {
      // m_log.log(LogLevelDebug, LogAreaClassConnectionTLS, "Encrypting data...");
      printf( "Encrypting data...\n----------------\n<%s>\n----------\n", data.c_str() );
      m_tls->encrypt( data );
      return true;
    }
    else
    {
      m_log.log( LogLevelWarning, LogAreaClassConnectionTLS,
                 "Attempt to send data on a connection that is not connected (or is connecting)" );
      return false;
    }
  }

  ConnectionError ConnectionTLS::receive()
  {
    ConnectionError e;

    while( ( e = recv( 500 ) ) == ConnNoError ) ;
    return e;
  }

  void ConnectionTLS::disconnect()
  {
    m_connection->disconnect();
    m_handshaked = false;
    delete m_tls;
    m_tls = 0;
    m_state = StateDisconnected;
  }

  void ConnectionTLS::cleanup()
  {
    m_connection->cleanup();
  }

  void ConnectionTLS::getStatistics( int& totalIn, int& totalOut )
  {
    m_connection->getStatistics( totalIn, totalOut );
  }

  ConnectionBase* ConnectionTLS::newInstance() const
  {
    ConnectionBase* pConn = m_connection->newInstance();
    ConnectionTLS* pTLS = new ConnectionTLS( pConn, m_handler, m_log );

    return (ConnectionBase*)pTLS;
  }

/************ ConnectionDataHandler methods *************/

  void ConnectionTLS::handleReceivedData( const ConnectionBase* /*connection*/, const std::string& data )
  {
    m_log.log( LogLevelDebug, LogAreaClassConnectionTLS, "Decrypting received data..." );
    m_tls->decrypt( data );
  }

  void ConnectionTLS::handleConnect( const ConnectionBase* /*connection*/ )
  {
    if( !m_handshaked )
    {
      m_log.log( LogLevelDebug, LogAreaClassConnectionTLS, "Beginning TLS handshake...." );
      m_tls->handshake(); // Initiate handshake
    }
  }

  void ConnectionTLS::handleDisconnect( const ConnectionBase* /*connection*/, ConnectionError reason )
  {
    if( m_handler )
      m_handler->handleDisconnect( this, reason );
    m_handshaked = false;
    m_state = StateDisconnected;
  }

/***************** TLSHandler methods *******************/

  void ConnectionTLS::handleEncryptedData( const TLSBase* /*tls*/, const std::string& data )
  {
    // m_log.log(LogLevelDebug, LogAreaClassConnectionTLS,
    printf( "Sending encrypted data...\n" );
    m_connection->send( data );
  }

  void ConnectionTLS::handleDecryptedData( const TLSBase* /*tls*/, const std::string& data )
  {
    if( m_handler )
    {
      m_log.log( LogLevelDebug, LogAreaClassConnectionTLS, "Handling decrypted data..." );
      m_handler->handleReceivedData( this, data );
    }
    else
    {
      m_log.log( LogLevelDebug, LogAreaClassConnectionTLS, "Data received and decrypted but no handler" );
    }
  }

  void ConnectionTLS::handleHandshakeResult( const TLSBase* /*tls*/, bool success, CertInfo& certinfo )
  {
    if( m_handshaked = success )
    {
      m_state = StateConnected;
      m_log.log( LogLevelDebug, LogAreaClassConnectionTLS, "TLS handshake succeeded" );
      if( m_handler )
        m_handler->handleConnect( this );
    }
    else
    {
      m_state = StateDisconnected;
      m_log.log( LogLevelWarning, LogAreaClassConnectionTLS, "TLS handshake failed" );
    }
  }

} // namespace gloox
