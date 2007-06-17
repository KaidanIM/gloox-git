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

#include "connectionbosh.h"
#include "dns.h"
#include "logsink.h"
#include "prep.h"
#include "base64.h"
#include "tag.h"

#include <string>

#ifndef _WIN32_WCE
# include <sstream>
#endif

namespace gloox
{

  ConnectionBOSH::ConnectionBOSH( ConnectionBase *connection, const LogSink& logInstance, const std::string& boshHost,
                                            const std::string& xmppServer, int xmppPort )
    : ConnectionBase( 0 ), m_connection( connection ),
      m_logInstance( logInstance ), m_boshHost( boshHost), m_http11( false )
  {
    m_server = prep::idna( xmppServer );
    m_port = xmppPort;
    m_path = "/";
    if( m_connection )
      m_connection->registerConnectionDataHandler( this );
  }

  ConnectionBOSH::ConnectionBOSH( ConnectionDataHandler *cdh, ConnectionBase *connection,
                                            const LogSink& logInstance, const std::string& boshHost,
                                            const std::string& xmppServer, int xmppPort )
    : ConnectionBase( cdh ), m_connection( connection ),
      m_logInstance( logInstance ), m_boshHost( boshHost )
  {
    m_server = prep::idna( xmppServer );
    m_port = xmppPort;
    m_path = "/";
    if( m_connection )
      m_connection->registerConnectionDataHandler( this );
  }

  ConnectionBOSH::~ConnectionBOSH()
  {
    if( m_connection )
      delete m_connection;
  }

  ConnectionBase* ConnectionBOSH::newInstance() const
  {
    ConnectionBase* conn = m_connection ? m_connection->newInstance() : 0;
    return new ConnectionBOSH( m_handler, conn, m_logInstance, m_boshHost, m_server, m_port );
  }

  void ConnectionBOSH::setConnectionImpl( ConnectionBase* connection )
  {
    if( m_connection )
      delete m_connection;

    m_connection = connection;
  }

  ConnectionError ConnectionBOSH::connect()
  {
    if( m_connection && m_handler )
    {
      m_state = StateConnecting;
      m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh initiating connection to server" );
      return m_connection->connect();
    }

    return ConnNotConnected;
  }

  void ConnectionBOSH::disconnect()
  {
    m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh disconnected from server" );
    m_state = StateDisconnected;
    if( m_connection )
      m_connection->disconnect();
  }

  ConnectionError ConnectionBOSH::recv( int timeout )
  {
    if( m_connection )
      return m_connection->recv( timeout );
    else
      return ConnNotConnected;
  }

  ConnectionError ConnectionBOSH::receive()
  {
    if( m_connection )
      return m_connection->receive();
    else
      return ConnNotConnected;
  }

  bool ConnectionBOSH::send( const std::string& data )
  {
    if( !m_connection )
      return false;
    m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh sent" + data);
    return true;
  }

  void ConnectionBOSH::cleanup()
  {
    m_state = StateDisconnected;

    if( m_connection )
      m_connection->cleanup();
  }

  void ConnectionBOSH::getStatistics( int &totalIn, int &totalOut )
  {
    if( m_connection )
      m_connection->getStatistics( totalIn, totalOut );
    else
    {
      totalIn = 0;
      totalOut = 0;
    }
  }

  void ConnectionBOSH::handleReceivedData( const ConnectionBase* connection,
                                                const std::string& data )
  {
	  m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh received" + data);
  }

  void ConnectionBOSH::handleConnect( const ConnectionBase* /*connection*/ )
  {
	  
    std::ostringstream connrequest;
    Tag requestBody("body");
    
    requestBody.addAttribute("content", "text/xml");
    requestBody.addAttribute("charset", "utf-8");
    requestBody.addAttribute("hold", (int)m_hold); // Shouldn't there be a boolean variant of addAttribute?
    requestBody.addAttribute("rid", m_rid);
    requestBody.addAttribute("ver", "1.6");
    requestBody.addAttribute("wait", m_wait);
    requestBody.addAttribute("ack", 0);
    requestBody.addAttribute("xml:lang", "en");
    requestBody.addAttribute("xmlns", "http://jabber.org/protocol/httpbind");
    
    connrequest << "POST " << m_path << " HTTP/1.1\r\n";
    connrequest << "Host: " << m_server << "\r\n";
    connrequest << "Content-Type: text/xml; charset=utf-8\r\n";
    connrequest << "Content-Length: " << requestBody.xml().length() << "\r\n\r\n";
    connrequest << requestBody.xml() << "\r\n";
    
    m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh connection request sent" );
    m_connection->send(connrequest.str());  
  }

  void ConnectionBOSH::handleDisconnect( const ConnectionBase* /*connection*/,
                                              ConnectionError reason )
  {
    m_state = StateDisconnected;
    m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh connection closed" );

    if( m_handler )
      m_handler->handleDisconnect( this, reason );
  }

  
} // namespace gloox;
