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

#include "connectionbosh.h"
#include "dns.h"
#include "logsink.h"
#include "prep.h"
#include "base64.h"
#include "tag.h"
#include "util.h"

#include <string>
#include <stdlib.h>

#ifndef _WIN32_WCE
#include <sstream>
#endif

namespace gloox
{

  ConnectionBOSH::ConnectionBOSH( ConnectionBase* connection, const LogSink& logInstance,
                                  const std::string& boshHost, const std::string& xmppServer,
                                  int xmppPort )
    : ConnectionBase( 0 ),
      m_logInstance( logInstance ), m_parser( this ), m_boshHost( boshHost ), m_path( "/http-bind/" ),
      m_handler( 0 ), m_rid( 0 ), m_initialStreamSent( false ), m_openRequests( 0 ),
      m_maxOpenRequests( 2 ), m_wait( 30 ), m_hold( 2 ), m_streamRestart( false ),
      m_lastRequestTime( 0 ), m_minTimePerRequest( 0 ), m_bufferContentLength( -1 ),
      m_connMode( ModePipelining )
  {
    initInstance( connection, xmppServer, xmppPort );
  }

  ConnectionBOSH::ConnectionBOSH( ConnectionDataHandler* cdh, ConnectionBase* connection,
                                  const LogSink& logInstance, const std::string& boshHost,
                                  const std::string& xmppServer, int xmppPort )
    : ConnectionBase( cdh ),
      m_logInstance( logInstance ), m_parser( this ), m_boshHost( boshHost ), m_path( "/http-bind/" ),
      m_handler( cdh ), m_rid( 0 ),  m_initialStreamSent( false ), m_openRequests( 0 ),
      m_maxOpenRequests( 2 ), m_wait( 30 ), m_hold( 2 ), m_streamRestart( false ),
      m_lastRequestTime( 0 ), m_minTimePerRequest( 0 ), m_bufferContentLength( -1 ),
      m_connMode( ModePipelining )
  {
    initInstance( connection, xmppServer, xmppPort );
  }

  void ConnectionBOSH::initInstance( ConnectionBase* connection, const std::string& xmppServer,
                                     const int xmppPort )
  {
#warning FIXME: check return value
    prep::idna( xmppServer, m_server );
    m_port = xmppPort;
    if( m_port != -1 )
    {
      std::ostringstream strBOSHHost;
      strBOSHHost << m_boshHost << ":" << m_port;
      m_boshHost = strBOSHHost.str();
    }

    // drop this connection into our pool of available connections
//     printf( "Added initial connection to connection pool\n" );
//     printf( "Connections in pool: %d\n", m_connectionPool.size() );
    connection->registerConnectionDataHandler( this );
    m_connectionPool.push_back( connection );
//     printf( "Connections in pool: %d\n", m_connectionPool.size() );
  }

  ConnectionBOSH::~ConnectionBOSH()
  {
    util::clearList( m_activeConnections );
    util::clearList( m_connectionPool );
  }

  ConnectionBase* ConnectionBOSH::newInstance() const
  {
    ConnectionBase* pBaseConn = 0;

    if( !m_connectionPool.empty() )
    {
      pBaseConn = m_connectionPool.front()->newInstance();
    }
    else if( !m_activeConnections.empty() )
    {
      pBaseConn = m_activeConnections.front()->newInstance();
    }
    else
    {
      return 0;
    }

    return new ConnectionBOSH( m_handler, pBaseConn, m_logInstance,
                               m_boshHost, m_server, m_port );
  }

  ConnectionError ConnectionBOSH::connect()
  {
    ConnectionError eRet = ConnNotConnected;

    /* the following assumes that there is only one connection either in the connection pool or
     * in the queue of active connections */
    if( m_handler )
    {
      if( m_connMode == ModePipelining )
      {
        m_state = StateConnecting;
        m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH,
                           "bosh initiating connection to server" );
        if( m_activeConnections.empty() && m_connectionPool.size() > 0 )
        {
          m_activeConnections.push_back( m_connectionPool.front() );
          m_connectionPool.pop_front();
        }
        eRet = m_activeConnections.back()->connect();
      }
      else
      {
        m_state = StateConnecting;
        m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH,
                           "bosh initiating connection to server" );
//         printf( "Connections in pool: %d\n", m_connectionPool.size() );
        eRet = m_connectionPool.back()->connect();
      }
    }

    return eRet;
  }

  void ConnectionBOSH::disconnect()
  {
    if( ( m_connMode == ModePipelining && m_activeConnections.empty() )
        || ( m_connectionPool.empty() && m_activeConnections.empty() ) )
      return;
    if( m_state != StateDisconnected )
    {
      ++m_rid;

      std::ostringstream requestBody;

      requestBody << "<body ";
      requestBody << "rid='" << m_rid << "' ";
      requestBody << "sid='" << m_sid << "' ";
      requestBody << "type='terminal' ";
      requestBody << "xml:lang='en' ";
      requestBody << "xmlns='" << XMLNS_HTTPBIND << "'";
      if( m_sendBuffer.empty() )  // Make sure that any data in the send buffer gets sent
        requestBody << "/>";
      else
      {
        requestBody << ">" << m_sendBuffer << "</body>";
        m_sendBuffer = EmptyString;
      }
      sendRequest( requestBody.str(), true );

      m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh disconnection request sent" );
    }
    else
    {
      m_logInstance.log( LogLevelError, LogAreaClassConnectionBOSH,
                         "disconnecting from server in a non-graceful fashion" );
    }

    util::ForEach( m_activeConnections, &ConnectionBase::disconnect );
    util::ForEach( m_connectionPool, &ConnectionBase::disconnect );

    m_state = StateDisconnected;
    if( m_handler )
      m_handler->handleDisconnect( this, ConnUserDisconnected );
  }

  ConnectionError ConnectionBOSH::recv( int timeout )
  {
    ConnectionError eRet = ConnNoError;

    // If there are no open requests then the spec allows us to send an empty request...
    // (Some CMs do not obey this, it seems)
    if( ( m_openRequests == 0 || m_sendBuffer.size() > 0 ) && m_state == StateConnected )
    {
      m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH,
           "Sending empty request (or there is data in the send buffer)" );
      sendXML( m_sendBuffer );
    }

    if( !m_activeConnections.empty() )
    {
      // If this the first recv() after a stream restart, pass to gloox a stream:stream tag
      if( m_handler && m_streamRestart )
      {
        m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "sending spoofed <stream:stream>" );
        m_handler->
        handleReceivedData( this,
                            "<?xml version='1.0' ?>"
                            "<stream:stream xmlns:stream='http://etherx.jabber.org/streams'"
                            " xmlns='" + XMLNS_CLIENT +
                            "' version='" + XMPP_STREAM_VERSION_MAJOR + "." + XMPP_STREAM_VERSION_MINOR +
                            "' from='" + m_server + "' id ='" + m_sid + "' xml:lang='en'>" );
        m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "sent spoofed <stream:stream>" );
        m_streamRestart = false;
      }

      switch( m_connMode )
      {
        case ModePipelining:
          // Read from connection at back of queue
          m_activeConnections.back()->recv( timeout );
          break;
        case ModeLegacyHTTP:
        case ModePersistentHTTP:
        {
          // Read from connection at front of queue
          ConnectionError ce = m_activeConnections.front()->recv( timeout );
          if( ce != ConnNoError )
          {

//             printf( "Error %d occured during recv() %d\n", ce, m_activeConnections.size() );
            m_connectionPool.push_back( m_activeConnections.front() );
            m_activeConnections.pop_front();
            m_connectionPool.back()->disconnect();
          }
          break;
        }
        default:
          break;
      } //switch
    }
    else
    {
      if( m_state == StateConnecting )
      {
        switch( m_connMode )
        {
          case ModePipelining:
          case ModePersistentHTTP:
          case ModeLegacyHTTP:
            if( !m_connectionPool.empty() )
              return m_connectionPool.front()->recv( timeout );
          default: break;
        }
      }

      m_logInstance.log( LogLevelWarning, LogAreaClassConnectionBOSH, "No available connections open" );
      eRet = ConnNotConnected;
    }
    return eRet;
  }

  /* Chooses the appropriate connection, or opens a new one if necessary. Wraps xml in HTTP and sends. */
  bool ConnectionBOSH::sendRequest( const std::string& xml, bool ignoreRequestLimit )
  {

    if( !ignoreRequestLimit )
    {
      if( m_connMode == ModePipelining && m_activeConnections.empty() )
      {
        m_logInstance.log( LogLevelWarning, LogAreaClassConnectionBOSH,
                           "Pipelining selected, but no connection open." );
        return false;
      }
      else if( m_connectionPool.empty() && m_activeConnections.empty() )
      {
        m_logInstance.log( LogLevelWarning, LogAreaClassConnectionBOSH,
                           "No available connections to send on..." );
        return false;
      }
      else if( m_openRequests > 0 && m_openRequests >= m_maxOpenRequests )
      {
        m_logInstance.log( LogLevelWarning, LogAreaClassConnectionBOSH,
             "impossible to send request (too many requests already open)" );
        return false;
      }
    }
    std::ostringstream request;
    request << "POST " << m_path;

    if( m_connMode == ModeLegacyHTTP )
    {
      request << " HTTP/1.0\r\n";
      request << "Connection: close\r\n";
    }
    else
      request << " HTTP/1.1\r\n";

    request << "Host: " << m_boshHost << "\r\n";
    request << "Content-Type: text/xml; charset=utf-8\r\n";
    request << "Content-Length: " << xml.length() << "\r\n\r\n";
    request << xml << "\r\n";

    bool bSendData = false;

    switch( m_connMode )
    {
      case ModePipelining:
      {
        // With pipelining we only have one connection that is always in the active connections list
        bSendData = true;
      }
        break;
      case ModePersistentHTTP:
      case ModeLegacyHTTP:
      {
        if( !m_connectionPool.empty() )
        {
          // get a connection from the connection pool
          ConnectionBase* pConnection = m_connectionPool.front();
          m_connectionPool.pop_front();
          if( pConnection->state() == StateDisconnected )
          {
//             printf( "Connection from pool was disconnected... connecting...\n" );
            ConnectionError ce = pConnection->connect();
            if( ce != ConnNoError )
            {
//               printf( "An error occured while connecting: %d\n", ce );
              m_state = StateDisconnected;   // Assume the connection to the server is lost permanently
              disconnect();
            }
            // Now return, as the connection may not be established
            // We will get called again from handleConnect
            /* m_logInstance.log ( LogLevelDebug, LogAreaClassConnectionBOSH,
                                  "waiting for connection to be established" );
             * return false;	*/
          }
          while( pConnection->state() != StateConnected ) pConnection->recv( 200 );
//           printf( "Restoring a connection whose status is %d\n", pConnection->state() );
          // now stick it on the end of the list of active connections
          m_activeConnections.push_back( pConnection );
          bSendData = true;
        }
        else if( !m_activeConnections.empty() )
        {
          ConnectionBase* pConnection = m_activeConnections.back()->newInstance();
          pConnection->registerConnectionDataHandler( this );
//           if( pConnection->connect() != ConnNoError )
//             printf( "Failed to connect...\n" );  // TODO: Retry here
          m_activeConnections.push_back( pConnection );
          bSendData = true;
        }
      }
        break;
      default:
        break;
    }
    // if everything is ok send the data
    if( bSendData )
    {
      bool ce = m_activeConnections.back()->send( request.str() );
      if( ce )
      {
//         printf( "Request sent on %p (%s)\n", m_activeConnections.back(), request.str().c_str() );
        ++m_openRequests;
//         printf( "%d: Incrementing m_openRequests to %d (connections: %d)\n", (int)time( 0 ),
//                m_openRequests, m_activeConnections.size() );
        return true;
      }
      else
      {
#warning What to do in this case?
        printf( "Error while trying to send on socket (state: %d)\n", m_activeConnections.back()->state() );
      }
    }
    m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "Unable to send request\n" );
    return false;
  }

  /* Sends XML. Wraps data in a <body/> tag, and then passes to sendRequest(). */
  bool ConnectionBOSH::sendXML( const std::string& data )
  {
//     printf( "*SendXML(%s)\n", data.c_str() );
    std::ostringstream requestBody;

    if( m_state != StateConnected )
    {
      m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH,
           "Data sent before connection established (will be buffered)" );
      return false;
    }

    if( data.empty() )
    {
      if( (time( 0 ) - m_lastRequestTime) < m_minTimePerRequest && m_openRequests > 0 )
      {
        m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH,
             "too little time between requests, adding to send buffer" );
        return false;
      }
//       printf( "\n>>>>> %ld seconds since last empty request <<<<<\n", time( 0 ) - m_lastRequestTime );
      m_lastRequestTime = time( 0 );
      m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "sending empty request" );
    }

    ++m_rid;

    requestBody << "<body ";
    requestBody << "rid='" << m_rid << "' ";
    requestBody << "sid='" << m_sid << "' ";
    requestBody << "xmlns='" << XMLNS_HTTPBIND << "'";

    if( m_streamRestart )
    {
      requestBody << " xmpp:restart='true' to='" << m_server << "' xml:lang='en' xmlns:xmpp='"
                  << XMLNS_XMPP_BOSH << "' />";
      m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "restarting stream" );
    }
    else
    {
      requestBody << ">" << m_sendBuffer << data << "</body>";
      m_sendBuffer = EmptyString;
    }
    // Send a request. Force if we are not sending an empty request, or if there are no connections open
    if( m_state != StateDisconnected &&
       sendRequest( requestBody.str(), !data.empty() | m_activeConnections.empty() ) )
    {
      m_sendBuffer.clear();
    }
    else
    {
      m_rid--; // I think... (may need to rethink when acks are implemented)
      m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH,
           "Unable to send. Connection not complete, or too many open requests, so added to buffer.\n" );
      m_sendBuffer += data;
//       printf( "\n---------------Buffer---------------\n%s\n----------------EOB--------------\n",
//              m_sendBuffer.c_str() );
    }

    return true;
  }

  std::string ConnectionBOSH::getHTTPField( const std::string& field )
  {
    int fieldpos = m_bufferHeader.find( "\r\n" + field + ": " ) + field.length() + 4;

    return m_bufferHeader.substr( fieldpos, m_bufferHeader.find( "\r\n", fieldpos ) );
  }

  ConnectionError ConnectionBOSH::receive()
  {
    ConnectionError eRet = recv( 0 );

    return eRet;
  }

  bool ConnectionBOSH::send( const std::string& data )
  {

    if( m_state == StateDisconnected )
      return false;

//     printf( "\nTold to send: %s\n", data.c_str() );

    if( data.substr( 0, 2 ) == "<?" )
    {
      if( m_initialStreamSent )
      {
        m_streamRestart = true;
      }
      else
      {
        m_initialStreamSent = true;
        m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "initial <stream:stream> dropped" );
        return true;
      }
    }
    else if( data == "</stream:stream>" )
      return true;

    sendXML( data );

    return true;
  }

  void ConnectionBOSH::cleanup()
  {
    m_state = StateDisconnected;

    util::ForEach( m_activeConnections, &ConnectionBase::cleanup );
    util::ForEach( m_connectionPool, &ConnectionBase::cleanup );
  }

  void ConnectionBOSH::getStatistics( int& totalIn, int& totalOut )
  {
    util::ForEach( m_activeConnections, &ConnectionBase::getStatistics, totalIn, totalOut );
    util::ForEach( m_connectionPool, &ConnectionBase::getStatistics, totalIn, totalOut );
  }

  void ConnectionBOSH::handleReceivedData( const ConnectionBase* connection,
                                           const std::string& data )
  {
    m_buffer += data;

    if( m_bufferHeader.empty() ) // HTTP header not received yet?
    {
      std::string::size_type headerLength = m_buffer.find( "\r\n\r\n", 0 );
      if( headerLength != std::string::npos ) // We have a full header in the buffer?
      {
        m_bufferHeader = m_buffer.substr( 0, headerLength );
        m_buffer = m_buffer.substr( headerLength + 4 ); // Remove header from m_buffer, and \r\n\r\n
        m_bufferContentLength = atol( getHTTPField( "Content-Length" ).c_str() );
        std::string statusCode = m_bufferHeader.substr( 9, 3 );

        if( statusCode != "200" )
        {
          m_logInstance.log( LogLevelWarning, LogAreaClassConnectionBOSH,
               "received error via legacy HTTP status code: " + statusCode );
          m_state = StateDisconnected; // As per XEP, consider connection broken
          disconnect();
        }

        if( m_connMode != ModeLegacyHTTP && getHTTPField( "Connection" ) == "close"
            || m_bufferHeader.substr( 0, 8 ) == "HTTP/1.0" )
        {
          m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH,
               "server indicated lack of support for HTTP/1.1 - falling back to HTTP/1.0" );
          m_connMode = ModeLegacyHTTP;
        }
      }
    }

    if( (long)( m_buffer.length() ) >= m_bufferContentLength && !m_buffer.empty() ) // We have at least
                                                                                    // one full response
    {
//       printf( "Response length is %d but I think it is at least %ld\n", m_buffer.length(),
//               m_bufferContentLength );
      m_openRequests--;
//       printf( "Decrementing m_openRequests to %d\n", m_openRequests );
//       printf( "\n-----------FULL RESPONSE BUFFER---------------\n%s\n---------------END-------------\n",
//              m_buffer.c_str() );
      std::string xml = m_buffer.substr( 0, m_bufferContentLength );
      handleXMLData( connection, xml );
      m_buffer.erase( 0, m_bufferContentLength ); // Remove the handled response from the buffer,
                                                  // and reset variables for reuse
      m_bufferContentLength = -1;
      m_bufferHeader = EmptyString;
//       printf( "\n-----------FULL RESPONSE BUFFER (after handling)"
//               "---------------\n%s\n---------------END-------------\n",
//               m_buffer.c_str() );
      handleReceivedData( connection, EmptyString ); // In case there are more full responses in the buffer

      if( connection == m_activeConnections.front() )
      {
        switch( m_connMode )
        {
          case ModeLegacyHTTP:
          {
            ConnectionBase* pConn = m_activeConnections.front();
            m_activeConnections.pop_front();
            pConn->disconnect();
            pConn->cleanup(); // This is necessary
            m_connectionPool.push_back( pConn );
//             printf( "Disconnected connection %p and moved to pool\n", pConn );
          }
            break;
          case ModePersistentHTTP:
          {
            ConnectionBase* pConn = m_activeConnections.front();
            m_activeConnections.pop_front();
            m_connectionPool.push_back( pConn );
            break;
          }
          case ModePipelining:
          default:
            break;
        }
      }
      else
      {
        m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH,
             "data has been received on a connection that is not the oldest" );
      }
    }
  }

  void ConnectionBOSH::handleXMLData( const ConnectionBase* /*connection*/, std::string& data )
  {
//     printf( "On connection %p ", connection );
//     m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh received XML:\n" + data + "\n" );
    m_parser.feed( data );
  }

  void ConnectionBOSH::handleConnect( const ConnectionBase* /*connection*/ )
  {

    if( m_state == StateConnecting )
    {
      m_rid = rand() % 100000 + 1728679472;

      Tag requestBody( "body" );

      requestBody.addAttribute( "content", "text/xml; charset=utf-8" );
      requestBody.addAttribute( "hold", (long)m_hold );
      requestBody.addAttribute( "rid", (long)m_rid );
      requestBody.addAttribute( "ver", "1.6" );
      requestBody.addAttribute( "wait", (long)m_wait );
      requestBody.addAttribute( "ack", 0 );
      requestBody.addAttribute( "secure", "false" );
      requestBody.addAttribute( "route", "xmpp:" + m_server + ":5222" );
      requestBody.addAttribute( "xml:lang", "en" );
      requestBody.addAttribute( "xmpp:version", "1.0" );
      requestBody.addAttribute( "xmlns", XMLNS_HTTPBIND );
      requestBody.addAttribute( "xmlns:xmpp", XMLNS_XMPP_BOSH );
      requestBody.addAttribute( "to", m_server );

      m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh connection request sent" );
      sendRequest( requestBody.xml(), true );
    }
    else
    {
//       printf( "New TCP connection %p established\n", connection );
      /* if(!m_sendBuffer.empty())
       * {
       * m_logInstance.log ( LogLevelDebug, LogAreaClassConnectionBOSH, "sending request on new connection" );
       * sendXML(m_sendBuffer);
       * } */
    }
  }

  void ConnectionBOSH::handleDisconnect( const ConnectionBase* /*connection*/,
                                         ConnectionError /*reason*/ )
  {
    switch( m_connMode )
    {
      case ModePipelining:
        m_connMode = ModeLegacyHTTP; // Server seems not to support pipelining
        m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH,
             "connection closed - falling back to HTTP/1.0 connection method" );
        break;
      case ModeLegacyHTTP:
      case ModePersistentHTTP:
#warning do we need to do anything here?
//         printf( "A TCP connection %p was disconnected (reason: %d).\n", connection, reason );
        break;
    }

    /* m_state = StateDisconnected;
     * m_logInstance.log ( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh connection closed" );
     *
     * if ( m_handler )
     * m_handler->handleDisconnect ( this, reason ); */
  }

  void ConnectionBOSH::handleTag( Tag* tag )
  {
    if( tag->name() == "body" )
    {
      if( tag->hasAttribute( "sid" ) )
      {
        m_state = StateConnected;
        m_sid = tag->findAttribute( "sid" );

        if( tag->hasAttribute( "requests" ) )
        {
          int serverRequests = atoi( tag->findAttribute( "requests" ).c_str() );
          if( serverRequests < m_maxOpenRequests )
          {
            m_maxOpenRequests = serverRequests;
            m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH,
                               "bosh parameter 'requests' now set to " + tag->findAttribute( "requests" ) );
          }
        }
        if( tag->hasAttribute( "hold" ) )
        {
          int maxHold = atoi( tag->findAttribute( "hold" ).c_str() );
          if( maxHold < m_hold )
          {
            m_hold = maxHold;
            m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH,
                               "bosh parameter 'hold' now set to " + tag->findAttribute( "hold" ) );
          }
        }
        if( tag->hasAttribute( "wait" ) )
        {
          int maxWait = atoi( tag->findAttribute( "wait" ).c_str() );
          if( maxWait < m_wait )
          {
            m_wait = maxWait;
            m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH,
                               "bosh parameter 'wait' now set to " + tag-> findAttribute( "wait" )
                                   + " seconds" );
          }
        }
        if( tag->hasAttribute( "polling" ) )
        {
          int minTime = atoi( tag->findAttribute( "polling" ).c_str() );
          m_minTimePerRequest = minTime;
          m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH,
                             "bosh parameter 'polling' now set to " + tag-> findAttribute( "polling" )
                                 + " seconds" );
        }
        if( m_handler )
        {
          m_handler->handleConnect( this );
          m_handler->
          handleReceivedData( this, "<?xml version='1.0' ?>"
                              "<stream:stream xmlns:stream='http://etherx.jabber.org/streams' "
                              "xmlns='" + XMLNS_CLIENT
                              + "' version='" + XMPP_STREAM_VERSION_MAJOR + "." + XMPP_STREAM_VERSION_MINOR
                              + "' from='" + m_server + "' id ='" + m_sid + "' xml:lang='en'>" );
        }
      }
      if( tag->hasAttribute( "type" ) )
      {
        if( tag->findAttribute( "type" ) == "terminal" )
        {
          m_logInstance.log( LogLevelError, LogAreaClassConnectionBOSH,
                             "bosh connection closed by server: " + tag->findAttribute( "condition" ) );
          m_state = StateDisconnected;
          if( m_handler )
            m_handler->handleDisconnect( this, ConnStreamClosed );
          return;
        }
      }

      TagList stanzas = tag->children();
      TagList::const_iterator i;
      for( i = stanzas.begin(); i != stanzas.end(); ++i )
      {

        if( m_handler && *i )
        {
          m_handler->handleReceivedData( this, (*i)->xml() );
        }
      }
      ;
    }
  }

} // namespace gloox;
