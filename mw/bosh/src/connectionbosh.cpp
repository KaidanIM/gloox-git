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

  ConnectionBOSH::ConnectionBOSH( ConnectionBase *connection, const LogSink& logInstance, const std::string& boshHost,
                                            const std::string& xmppServer, int xmppPort )
    : ConnectionBase( 0 ), m_connection( connection ),
      m_logInstance( logInstance ), m_boshHost( boshHost), m_http11( false ), m_path("/http-bind/"), m_handler(NULL),
      m_initialStreamSent(false), m_openRequests(0), m_requests(2), m_wait(30), m_hold(2), m_streamRestart(false)
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
    if( m_connection )
      m_connection->registerConnectionDataHandler( this );
  }

  ConnectionBOSH::ConnectionBOSH( ConnectionDataHandler *cdh, ConnectionBase *connection,
                                            const LogSink& logInstance, const std::string& boshHost,
                                            const std::string& xmppServer, int xmppPort )
    : ConnectionBase( cdh ), m_connection( connection ),
      m_logInstance( logInstance ), m_boshHost( boshHost ), m_path("/http-bind/"), m_handler(cdh),
      m_initialStreamSent(false), m_openRequests(0), m_requests(2), m_wait(30), m_hold(2), m_streamRestart(false)
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
    m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh connection implementation set" );
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
    {
      if(m_openRequests < 1 && !m_sid.empty())
      {
        this->send("");
      }
      return m_connection->recv( timeout );
    }
    else
      return ConnNotConnected;
  }
  
  std::string ConnectionBOSH::GetHTTPField(const std::string& field)
  {
    int fieldpos = m_bufferHeader.find("\r\n" + field + ": ") + field.length() + 4;
    return m_bufferHeader.substr(fieldpos, m_bufferHeader.find("\r\n", fieldpos));
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
    
    std::ostringstream request;
    std::ostringstream requestBody;
    
    if(data.substr(0,2) == "<?")
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

    m_rid++;
    
    requestBody << "<body ";
    requestBody << "rid='" << m_rid << "' ";
    requestBody << "sid='" << m_sid << "' ";
    requestBody << "xmlns='http://jabber.org/protocol/httpbind' ";
    
    if(m_streamRestart)
    {
      requestBody << "xmpp:restart='true' to='" << m_server << "' xml:lang='en' xmlns:xmpp='urn:xmpp:xbosh' />";
      m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "restarting stream");
    }
    else
    {
      requestBody << ">" << data << "</body>";
    }
    
    
    request << "POST " << m_path << " HTTP/1.1\r\n";
    request << "Host: " << m_boshHost << "\r\n";
    request << "Content-Type: text/xml; charset=utf-8\r\n";
    request << "Content-Length: " << requestBody.str().length() << "\r\n\r\n";
    request << requestBody.str() << "\r\n";

    
    m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh sent:\n" + request.str() +"\n");
    m_connection->send(request.str());
    m_openRequests++;
    
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
	  m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh received:\n" + data + "\n");
    
    m_buffer += data;
    
    if(m_bufferHeader.empty()) // HTTP header not received yet?
    {
      std::string::size_type headerLength = m_buffer.find("\r\n\r\n", 0);
      if(headerLength != std::string::npos) 
      {
        m_bufferHeader = m_buffer.substr(0, headerLength);
        m_buffer = m_buffer.substr(headerLength + 4); // Remove header from m_buffer
        m_bufferContentLength = atol(GetHTTPField("Content-Length").c_str());
      }
    }
    
    if(m_buffer.length() >= m_bufferContentLength) // We have at least one full response
    {
      m_openRequests--;
      handleXMLData(connection, m_buffer.substr(0, m_bufferContentLength));
      m_buffer = m_buffer.substr(m_bufferContentLength); // Remove the handled response from the buffer, and reset variables for reuse
      m_bufferContentLength = -1;
      m_bufferHeader = "";
      handleReceivedData(connection, ""); // In case there are more full responses in the buffer
    }
  }
  
  void ConnectionBOSH::handleXMLData(const ConnectionBase* connection, const std::string& data)
  {
    if(m_handler && m_streamRestart)
     {
       m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "sending spoofed <stream:stream>");
       m_handler->handleReceivedData(this, "<?xml version='1.0' ?><stream:stream xmlns:stream='http://etherx.jabber.org/streams' xmlns='jabber:client' version='1.0' from='localhost' id ='" +m_sid+"' xml:lang='en' >");
       m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "sent spoofed <stream:stream>");
       m_streamRestart = false;
     }
    m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh received XML:\n" + data + "\n");
    m_parser->feed(data);
  }

  void ConnectionBOSH::handleConnect( const ConnectionBase* /*connection*/ )
  {
	  
    m_state = StateConnecting;
    
    m_rid = ( rand() % 100000 + 47289472);
    
    std::ostringstream connrequest;
    Tag requestBody("body");
    
    requestBody.addAttribute("content", "text/xml; charset=utf-8");
    requestBody.addAttribute("hold", m_hold);
    requestBody.addAttribute("rid", m_rid);
    requestBody.addAttribute("ver", "1.6");
    requestBody.addAttribute("wait", m_wait);
    requestBody.addAttribute("ack", 0);
    requestBody.addAttribute("xml:lang", "en");
    requestBody.addAttribute("xmlns", "http://jabber.org/protocol/httpbind");
    requestBody.addAttribute("to", m_server);
    
    connrequest << "POST " << m_path << " HTTP/1.1\r\n";
    connrequest << "Host: " << m_boshHost << "\r\n";
    connrequest << "Content-Type: text/xml; charset=utf-8\r\n";
    connrequest << "Content-Length: " << requestBody.xml().length() << "\r\n\r\n";
    connrequest << requestBody.xml() << "\r\n";
    
    m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh connection request sent" );
    m_openRequests++;
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
  
  void ConnectionBOSH::handleTag(Tag* tag)
  {
    if(tag->name() == "body")
    {
      if(tag->hasAttribute("sid"))
      {
        m_state = StateConnected;
        m_sid = tag->findAttribute("sid");
        if(tag->hasAttribute("hold"))
          m_hold = atoi(tag->findAttribute("hold").c_str());
        if(tag->hasAttribute("requests"))
        {
          int serverRequests = atoi(tag->findAttribute("requests").c_str());
          if(serverRequests < m_requests)
            m_requests = serverRequests;
        }
        if(tag->hasAttribute("hold"))
        {
          int serverRequests = atoi(tag->findAttribute("hold").c_str());
          if(serverRequests < m_hold)
            m_hold = serverRequests;
        }
        if(m_handler)
        {
          m_handler->handleConnect(this);
          m_handler->handleReceivedData(this, "<?xml version='1.0' ?><stream:stream xmlns:stream='http://etherx.jabber.org/streams' xmlns='jabber:client' version='1.0' from='localhost' id ='" +m_sid+"' xml:lang='en'>");
        }
      }
      if(tag->hasAttribute("type"))
      {
        if(tag->findAttribute("type") == "terminal")
        {
          m_logInstance.log( LogLevelDebug, LogAreaClassConnectionBOSH, "bosh connection closed by server: "+tag->findAttribute("condition"));
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
        m_handler->handleReceivedData(this, (*i)->xml());
      };
     }
  }
  

  
} // namespace gloox;
