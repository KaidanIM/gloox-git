/*
 * Copyright (c) 2009 by Jakob Schroeter <js@camaya.net>
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
#include "connectioncompression.h"
#include "compressionbase.h"
#include "compressiondefault.h"

namespace gloox
{

  ConnectionCompression::ConnectionCompression( ConnectionDataHandler* cdh, ConnectionBase* conn, const LogSink& log )
    : ConnectionBase( cdh ),
      m_connection( conn ), m_log( log )
  {
    if( m_connection )
      m_connection->registerConnectionDataHandler( this );
  }

  ConnectionCompression::ConnectionCompression( ConnectionBase* conn, const LogSink& log )
    : ConnectionBase( 0 ),
      m_connection( conn ), m_log( log )
  {
    if( m_connection )
      m_connection->registerConnectionDataHandler( this );
  }

  ConnectionCompression::~ConnectionCompression()
  {
    delete m_connection;
    delete m_compression;
  }

  ConnectionError ConnectionCompression::connect()
  {
    if( !m_connection )
      return ConnNotConnected;

    m_compression = new CompressionDefault( this );
    if( !m_compression )
      return ConnCompressionNotAvailable;

    if( !m_compression->init() )
      return ConnCompressionFailed;

    m_state = StateConnected;

    return m_connection->connect();
 }

  ConnectionError ConnectionCompression::recv( int timeout )
  {
    if( m_connection->state() == StateConnected )
    {
      return m_connection->recv( timeout );
    }
    else
    {
//       m_log.log( LogLevelWarning, LogAreaClassConnectionCompression,
//                  "Attempt to receive data on a connection that is not connected (or is connecting)" );
      return ConnNotConnected;
    }
  }

  bool ConnectionCompression::send( const std::string& data )
  {
    if( m_state == StateConnected )
    {
      // m_log.log(LogLevelDebug, LogAreaClassConnectionCompression, "Encrypting data...");
//       printf( "Encrypting data...\n----------------\n<%s>\n----------\n", data.c_str() );
      m_compression->compress( data );
      return true;
    }
    else
    {
//       m_log.log( LogLevelWarning, LogAreaClassConnectionCompression,
//                  "Attempt to send data on a connection that is not connected (or is connecting)" );
      return false;
    }
  }

  ConnectionError ConnectionCompression::receive()
  {
    if( m_connection )
      return m_connection->receive();
    else
      return ConnNotConnected;
  }

  void ConnectionCompression::disconnect()
  {
    if( m_connection )
      m_connection->disconnect();

    cleanup();
  }

  void ConnectionCompression::cleanup()
  {
    if( m_connection )
      m_connection->cleanup();
    if( m_compression )
      m_compression->cleanup();
    delete m_compression;
    m_compression = 0;
    m_state = StateDisconnected;
  }

  void ConnectionCompression::getStatistics( int& totalIn, int& totalOut )
  {
    if( m_connection )
      m_connection->getStatistics( totalIn, totalOut );
  }

  ConnectionBase* ConnectionCompression::newInstance() const
  {
    ConnectionBase* newConn = 0;
    if( m_connection )
      newConn = m_connection->newInstance();
    return new ConnectionCompression( m_handler, newConn, m_log );
  }

  void ConnectionCompression::handleReceivedData( const ConnectionBase* /*connection*/, const std::string& data )
  {
//     m_log.log( LogLevelDebug, LogAreaClassConnectionCompression, "Decrypting received data..." );
    if( m_compression )
      m_compression->decompress( data );
  }

  void ConnectionCompression::handleConnect( const ConnectionBase* /*connection*/ )
  {
    if( m_handler )
      m_handler->handleConnect( this );
  }

  void ConnectionCompression::handleDisconnect( const ConnectionBase* /*connection*/, ConnectionError reason )
  {
    if( m_handler )
      m_handler->handleDisconnect( this, reason );

    cleanup();
  }

  void ConnectionCompression::handleCompressedData( const std::string& data )
  {
    // m_log.log(LogLevelDebug, LogAreaClassConnectionCompression,
//     printf( "Sending encrypted data...\n" );
    if( m_connection )
      m_connection->send( data );
  }

  void ConnectionCompression::handleDecompressedData( const std::string& data )
  {
    if( m_handler )
    {
//       m_log.log( LogLevelDebug, LogAreaClassConnectionCompression, "Handling decrypted data... \n" + data );
      m_handler->handleReceivedData( this, data );
    }
    else
    {
//       m_log.log( LogLevelDebug, LogAreaClassConnectionCompression, "Data received and decrypted but no handler" );
    }
  }

}
