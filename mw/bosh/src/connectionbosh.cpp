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
#include <stdlib.h>

#ifndef _WIN32_WCE
# include <sstream>
#endif

namespace gloox
  {

  ConnectionBOSH::ConnectionBOSH( ConnectionBase *connection, const LogSink& logInstance, const std::string& boshHost, const std::string& xmppServer, int xmppPort )
      : ConnectionBase( 0 ), /*m_connection( connection ),*/
      m_logInstance( logInstance ), m_boshHost( boshHost), m_path("/http-bind/"), m_handler(NULL),
      m_initialStreamSent(false), m_openRequests(0), m_maxOpenRequests(20), m_wait(30), m_hold(2), m_streamRestart(false),
      m_lastRequestTime(0), m_minTimePerRequest(0), m_sendBuffer(""),
      m_connMode(ModePipelining)
  {
    m_server = prep::idna( xmppServer );
    m_port = xmppPort;

    if(m_port != -1)
      {
        std::ostringstream strBOSHHost;
        strBOSHHost << m_boshHost << ":" << m_port;
        m_boshHost = strBOSHHost.str();
      }
    m_parser = new Parser(this);
    //if( m_connection )
    //  m_connection->registerConnectionDataHandler( this );
    if(connection)
      {
        ConnectionBase * pConnection = connection->newInstance();

        if(m_connMode ==  ModePipelining)
          {
            // queue this connection so that it is ready for immediate use
            m_activeConnections.push_back(pConnection);

            m_activeConnections.back()->registerConnectionDataHandler( this );
          }
        else
          {
            // drop this connection into our pool of available connections
            m_connectionPool.push_back(pConnection);
          }
      }
  }

  ConnectionBOSH::ConnectionBOSH( ConnectionDataHandler *cdh, ConnectionBase *connection,
                                  const LogSink& logInstance, const std::string& boshHost,
                                  const std::string& xmppServer, int xmppPort )
      : ConnectionBase( cdh ),/* m_connection( connection ),*/
      m_logInstance( logInstance ), m_boshHost( boshHost ), m_path("/http-bind/"), m_handler(cdh),
      m_initialStreamSent(false), m_openRequests(0), m_maxOpenRequests(20), m_wait(30), m_hold(2), m_streamRestart(false),
      m_lastRequestTime(0), m_minTimePerRequest(0), m_sendBuffer(""),
      m_connMode(ModePipelining)
  {
    m_server = prep::idna( xmppServer );
    m_port = xmppPort;
    if(m_port != -1)
      {
        std::ostringstream strBOSHHost;
        strBOSHHost << m_boshHost << ":" << m_port;
        m_boshHost = strBOSHHost.str();
      }

    m_parser = new Parser(this);
    //if( m_connection )
    //  m_connection->registerConnectionDataHandler( this );
    if(connection)
      {
        ConnectionBase * pConnection = connection->newInstance();
        if(m_connMode ==  ModePipelining)
          {
            // queue this connection so that it is ready for immediate use
            m_activeConnections.push_back(pConnection);
            m_activeConnections.back()->registerConnectionDataHandler( this );
          }
        else
          {
            // drop this connection into our pool of available connections
            m_connectionPool.push_back(pConnection);
          }
      }
  }

  ConnectionBOSH::~ConnectionBOSH()
  {
    //if( m_connection )
    //  delete m_connection;
    if(!m_activeConnections.empty())
      {
        while(!m_activeConnections.empty())
          {
            ConnectionBase * pConnection = m_activeConnections.front();
            delete pConnection;
            m_activeConnections.pop_front();
          }
      }

    if(!m_connectionPool.empty())
      {
        while(!m_connectionPool.empty())
          {
            ConnectionBase * pConnection = m_connectionPool.front();
            delete pConnection;
            m_connectionPool.pop_front();
          }
      }
  }

  ConnectionBase* ConnectionBOSH::newInstance() const
    {
      //ConnectionBase* conn = m_connection ? m_connection->newInstance() : 0;
      //return new ConnectionBOSH( m_handler, conn, m_logInstance, m_boshHost, m_server, m_port );
      ConnectionBOSH * pNewConnection = new ConnectionBOSH( m_handler, /*conn*/NULL, m_logInstance, m_boshHost, m_server, m_port );

      pNewConnection->m_activeConnections = m_activeConnections;

      pNewConnection->m_connectionPool = m_connectionPool;

      return (pNewConnection);
    }


  ConnectionError ConnectionBOSH::connect()
  {
    // Perform connection to server

    /* //if( m_connection && m_handler )
     if( !m_activeConnections.empty() && m_handler )
     {
       m_state = StateConnecting;
       m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh initiating connection to server" );
       //return m_connection->connect();
       return m_activeConnections.front()->connect();
     }

     return ConnNotConnected;
    */
    ConnectionError eRet = ConnNotConnected;

    /* the following assumes that there is only one connection either in the connection pool or
       in the queue of active connections */
    if(m_handler)
      {
        if(m_connMode == ModePipelining)
          {
            m_state = StateConnecting;
            m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh initiating connection to server" );
            return m_activeConnections.back()->connect();
          }
        else
          {
            m_state = StateConnecting;
            m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh initiating connection to server" );
            return m_connectionPool.back()->connect();
          }
      }

    return eRet;
  }

  void ConnectionBOSH::disconnect()
  {
    m_state = StateDisconnected;

    //if(!m_connection)
    if(m_connMode == ModePipelining)
      {
        if(m_activeConnections.empty())
          return;
      }
    else
      {
        if(m_connectionPool.empty() && m_activeConnections.empty())
          return;
      }

    std::ostringstream connrequest;
    std::ostringstream requestBody;

    m_rid++;

    requestBody << "<body ";
    requestBody << "rid='" << m_rid << "' ";
    requestBody << "sid='" << m_sid << "' ";
    requestBody << "type='terminal' ";
    requestBody << "xml:lang='en' ";
    requestBody << "xmlns='http://jabber.org/protocol/httpbind'";
    if(m_sendBuffer.empty()) // Make sure that any data in the send buffer gets sent
      requestBody << "/>";
    else
      {
        requestBody << ">" << m_sendBuffer << "</body>";
        m_sendBuffer = "";
      }
    connrequest << "POST " << m_path << " HTTP/1.1\r\n";
    connrequest << "Host: " << m_boshHost << "\r\n";
    connrequest << "Content-Type: text/xml; charset=utf-8\r\n";
    connrequest << "Content-Length: " << requestBody.str().length() << "\r\n\r\n";
    connrequest << requestBody.str() << "\r\n";

    m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh disconnection request sent" );
    m_openRequests++;
    printf("Incrementing m_openRequests to %d\n", m_openRequests);
    //m_connection->send(connrequest.str());
    //m_connection->disconnect();
    if(m_connMode == ModePipelining)
      {
        m_activeConnections.back()->send(connrequest.str());
        if(!m_connMode & 1)
          m_activeConnections.back()->disconnect();
      }
    else
      {
        if(m_handler)
          m_handler->handleDisconnect(this, ConnUserDisconnected);
      }
  }

  ConnectionError ConnectionBOSH::recv( int timeout )
  {
    ConnectionError eRet = ConnNotConnected;

    switch(m_connMode)
      {
      case ModePipelining:
        // Read from connection at back of queue
        m_activeConnections.back()->recv(timeout);
        break;
      case ModeLegacyHTTP:
      case ModePersistentHTTP:
        // Read from connection at front of queue
        m_activeConnections.front()->recv(timeout);
        break;
      default:
        return eRet;
      }

    if(!m_activeConnections.empty())
      {
        if(m_handler && m_streamRestart)
          {
            m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "sending spoofed <stream:stream>");
            m_handler->handleReceivedData(this, "<?xml version='1.0' ?><stream:stream xmlns:stream='http://etherx.jabber.org/streams' xmlns='jabber:client' version='1.0' from='" + m_server + "' id ='" + m_sid + "' xml:lang='en' >");
            m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "sent spoofed <stream:stream>");
            m_streamRestart = false;
          }
        if(m_openRequests < 1 && !m_sid.empty())
          {
            this->send(m_sendBuffer);
            m_sendBuffer = "";
          }
        //return m_connection->recv( timeout );
        eRet = m_activeConnections.front()->recv( timeout );

        if((eRet == ConnNoError) && m_connMode != ModePipelining)
          {
            // remove this connection from the list of active connections and drop it into the
            // connection pool
            ConnectionBase * pConnection = m_activeConnections.front();
            m_connectionPool.push_back(pConnection);
            m_activeConnections.pop_front();
          }
      }

    return eRet;
  }

  /* Chooses the appropriate connection, or opens a new one if necessary. Wraps xml in HTTP and sends. */
  bool ConnectionBOSH::sendRequest(const std::string& xml)
  {

    if(m_connMode == ModePipelining)
      {
        if(m_activeConnections.empty()) // Pipelining, but no active connection
          return false;
      }
    else if(m_connMode == ModePersistentHTTP)
      {
        if(m_connectionPool.empty() && m_activeConnections.empty())
          return false;
      }
    else if(m_connMode == ModeLegacyHTTP && m_connectionPool.empty() && m_activeConnections.empty())
      {
        return false;
      }

    std::ostringstream request;
    request << "POST " << m_path << " HTTP/1.1\r\n";
    request << "Host: " << m_boshHost << "\r\n";
    if(m_connMode == ModeLegacyHTTP)
      request << "Connection: close\r\n";
    request << "Content-Type: text/xml; charset=utf-8\r\n";
    request << "Content-Length: " << xml.length() << "\r\n\r\n";
    request << xml << "\r\n";

    m_openRequests++;
    printf("%d: Incrementing m_openRequests to %d\n", (int)time(NULL), m_openRequests);

    // buffer the data
    m_sendBuffer += request.str();

    bool bSendData = false;

    switch (m_connMode)
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
        // check to see how many active connections there are...
        if(m_activeConnections.size() < m_maxOpenRequests)
          {
            // get a connection from the connection pool
            ConnectionBase * pConnection = m_connectionPool.front();
            m_connectionPool.pop_front();

            // now stick it on the end of the list of active connections
            m_activeConnections.push_back(pConnection);

            bSendData = true;
          }
      }
      break;
      default:
        break;
      }
    // if everything is ok send the data
    if(bSendData)
      {
        if(m_activeConnections.back()->send(m_sendBuffer))
          {
            m_sendBuffer.clear();
          }
      }

  }

  /* Sends XML. Wraps data in a <body/> tag, and then passes to sendRequest(). */
  bool ConnectionBOSH::sendXML(const std::string& data)
  {
    std::ostringstream requestBody;

    m_rid++;

    requestBody << "<body ";
    requestBody << "rid='" << m_rid << "' ";
    requestBody << "sid='" << m_sid << "' ";
    requestBody << "xmlns='http://jabber.org/protocol/httpbind'";

    if(m_streamRestart)
      {
        requestBody << " xmpp:restart='true' to='" << m_server << "' xml:lang='en' xmlns:xmpp='urn:xmpp:xbosh' />";
        m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "restarting stream");
      }
    else
      {
        requestBody << ">" << m_sendBuffer << data << "</body>";
        m_sendBuffer = "";
      }

    return sendRequest(requestBody.str());
  }


  std::string ConnectionBOSH::GetHTTPField(const std::string& field)
  {
    int fieldpos = m_bufferHeader.find("\r\n" + field + ": ") + field.length() + 4;
    return m_bufferHeader.substr(fieldpos, m_bufferHeader.find("\r\n", fieldpos));
  }

  ConnectionError ConnectionBOSH::receive()
  {
    //if( m_connection )
    //  return m_connection->receive();

    ConnectionError eRet = ConnNotConnected;

    if( !m_activeConnections.empty())
      {
        eRet = m_activeConnections.front()->receive();

        if((eRet = ConnNoError) && m_connMode != ModePipelining)
          {
            // remove this connection from the list of active connections and drop it into the
            // connection pool
            ConnectionBase * pConnection = m_activeConnections.front();
            m_connectionPool.push_back(pConnection);
            m_activeConnections.pop_front();
          }
      }

    return eRet;
  }


  bool ConnectionBOSH::send( const std::string& data )
  {

    if(m_state == StateDisconnected )
      return false;

    printf("\nTold to send: %s\n", data.c_str());

    if(m_openRequests >= m_maxOpenRequests)
      {
        m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "too many open requests, adding to send buffer");
        m_sendBuffer += data;
        return false;
      }
    else
      {
        printf("%d: m_openRequests == %d\n", (int)time(NULL), m_openRequests);
      }

    if(data.empty())
      {
        if((time(NULL) - m_lastRequestTime) < m_minTimePerRequest && m_openRequests > 0)
          {
            m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "too little time between requests, adding to send buffer");
            return false;
          }
        printf("\n>>>>> %ld seconds since last empty request <<<<<\n", time(NULL) - m_lastRequestTime);
        m_lastRequestTime = time(NULL);
        m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "sending empty request");
      }
    else if(data.substr(0, 2) == "<?")
      {
        if(m_initialStreamSent)
          {
            m_streamRestart = true;
          }
        else
          {
            m_initialStreamSent = true;
            m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "initial <stream:stream> dropped");
            return true;
          }
      }
    else if(data == "</stream:stream>")
      return true;

    sendXML(data);

    return true;
  }

  void ConnectionBOSH::cleanup()
  {
    m_state = StateDisconnected;

    //if( m_connection )
    //  m_connection->cleanup();
    for(uint i = 0; i < m_activeConnections.size(); i++)
      {
        m_activeConnections[i]->cleanup();
      }

    for(uint i = 0; i < m_connectionPool.size(); i++)
      {
        m_connectionPool[i]->cleanup();
      }
  }

  void ConnectionBOSH::getStatistics( int &totalIn, int &totalOut )
  {
    //if( m_connection )
    //  m_connection->getStatistics( totalIn, totalOut );

   // TODO...
    if( !m_activeConnections.empty() )
      m_activeConnections.back()->getStatistics( totalIn, totalOut );
    else
      {
        totalIn = 0;
        totalOut = 0;
      }
  }

  void ConnectionBOSH::handleReceivedData( const ConnectionBase* connection,
      const std::string& data )
  {
    m_buffer += data;

    if(m_bufferHeader.empty()) // HTTP header not received yet?
      {
        std::string::size_type headerLength = m_buffer.find("\r\n\r\n", 0);
        if(headerLength != std::string::npos) // We have a full header in the buffer?
          {
            m_bufferHeader = m_buffer.substr(0, headerLength);
            m_buffer = m_buffer.substr(headerLength + 4); // Remove header from m_buffer, and \r\n\r\n
            m_bufferContentLength = atol(GetHTTPField("Content-Length").c_str());
          }
      }

    if((long)(m_buffer.length()) >= m_bufferContentLength && !m_buffer.empty()) // We have at least one full response
      {
        printf("Response length is %d but I think it is at least %ld\n", m_buffer.length(), m_bufferContentLength);
        m_openRequests--;
        printf("Decrementing m_openRequests to %d\n", m_openRequests);
        printf("\n-----------FULL RESPONSE BUFFER---------------\n%s\n---------------END-------------\n", m_buffer.c_str());
        handleXMLData(connection, m_buffer.substr(0, m_bufferContentLength));
        m_buffer.erase(0, m_bufferContentLength); // Remove the handled response from the buffer, and reset variables for reuse
        m_bufferContentLength = -1;
        m_bufferHeader = "";
        printf("\n-----------FULL RESPONSE BUFFER (after handling)---------------\n%s\n---------------END-------------\n", m_buffer.c_str());
        handleReceivedData(connection, ""); // In case there are more full responses in the buffer

        if(connection == m_activeConnections.front())
          {
            switch(m_connMode)
              {
              case ModeLegacyHTTP:
              case ModePersistentHTTP:
              {
                ConnectionBase* pConn = m_activeConnections.front();
                m_activeConnections.pop_front();
                m_connectionPool.push_back(pConn);
                break;
              }
              case ModePipelining:
              default:
                break;
              }
          }
        else
          {
            m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "data has been received on a connection that is not the oldest" );
          }
      }
  }

  void ConnectionBOSH::handleXMLData(const ConnectionBase* /*connection*/, const std::string& data)
  {
    m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh received XML:\n" + data + "\n");
    m_parser->feed(data);
  }

  void ConnectionBOSH::handleConnect( const ConnectionBase* /*connection*/ )
  {

    m_state = StateConnecting;

    m_rid = ( rand() % 100000 + 47289472);

    Tag requestBody("body");

    requestBody.addAttribute("content", "text/xml; charset=utf-8");
    requestBody.addAttribute("hold", m_hold);
    requestBody.addAttribute("rid", m_rid);
    requestBody.addAttribute("ver", "1.6");
    requestBody.addAttribute("wait", m_wait);
    requestBody.addAttribute("ack", 0);
    requestBody.addAttribute("secure", "false");
    requestBody.addAttribute("route", "xmpp:" + m_server + ":5222");
    requestBody.addAttribute("xml:lang", "en");
    requestBody.addAttribute("xmpp:version", "1.0");
    requestBody.addAttribute("xmlns", "http://jabber.org/protocol/httpbind");
    requestBody.addAttribute("xmlns:xmpp", "urn:xmpp:xbosh");
    requestBody.addAttribute("to", m_server);

    m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh connection request sent" );
    printf("Incrementing m_openRequests to %d\n", m_openRequests);
    sendRequest( requestBody.xml() );
  }

  void ConnectionBOSH::handleDisconnect( const ConnectionBase* /*connection*/,
                                         ConnectionError reason )
  {
    switch(m_connMode)
      {
      case ModePipelining:
        // Disconnection is fatal to BOSH?
        break;
      case ModeLegacyHTTP:
      case ModePersistentHTTP:
        break;
      }

    // I think... try to reconnect.
    // If reconnection fails then disconnect the BOSH 'connection'
    // Add a number of retries?

    m_state = StateDisconnected;
    m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh connection closed" );

    if( m_handler )
      m_handler->handleDisconnect( this, reason );
  }

  void ConnectionBOSH::handleTag(Tag* tag)
  {
    if(tag->name() == "body")
      {
        if(tag->hasAttribute("sid"))
          {
            m_state = StateConnected;
            m_sid = tag->findAttribute("sid");

            if(tag->hasAttribute("requests"))
              {
                int serverRequests = atoi(tag->findAttribute("requests").c_str());
                if(serverRequests < m_maxOpenRequests)
                  {
                    m_maxOpenRequests = serverRequests;
                    m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh parameter 'requests' now set to " + tag->findAttribute("requests") );
                  }
              }
            if(tag->hasAttribute("hold"))
              {
                int maxHold = atoi(tag->findAttribute("hold").c_str());
                if(maxHold < m_hold)
                  {
                    m_hold = maxHold;
                    m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh parameter 'hold' now set to " + tag->findAttribute("hold") );
                  }
              }
            if(tag->hasAttribute("wait"))
              {
                int maxWait = atoi(tag->findAttribute("wait").c_str());
                if(maxWait < m_wait)
                  {
                    m_wait = maxWait;
                    m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh parameter 'wait' now set to " + tag->findAttribute("wait") + " seconds");
                  }
              }
            if(tag->hasAttribute("polling"))
              {
                int minTime = atoi(tag->findAttribute("polling").c_str());
                m_minTimePerRequest = minTime;
                m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh parameter 'polling' now set to " + tag->findAttribute("polling") + " seconds");
              }
            if(m_handler)
              {
                m_handler->handleConnect(this);
                m_handler->handleReceivedData(this, "<?xml version='1.0' ?><stream:stream xmlns:stream='http://etherx.jabber.org/streams' xmlns='jabber:client' version='1.0' from='localhost' id ='" + m_sid + "' xml:lang='en'>");
              }
          }
        if(tag->hasAttribute("type"))
          {
            if(tag->findAttribute("type") == "terminal")
              {
                m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh connection closed by server: " + tag->findAttribute("condition"));
                m_state = StateDisconnected;
                if(m_handler)
                  m_handler->handleDisconnect(this, ConnStreamClosed);
                return;
              }
          }

        Tag::TagList stanzas = tag->children();
        Tag::TagList::const_iterator i;
        for(i = stanzas.begin(); i != stanzas.end(); i++)
          {
//        printf("(*i) is %p, m_handler is %p\n", (void*)*i, (void*)m_handler);

            if(m_handler && *i)
              {
                m_handler->handleReceivedData(this, (*i)->xml());
              }
          };
      }
  }



} // namespace gloox;
