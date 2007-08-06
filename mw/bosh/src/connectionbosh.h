/*
  Copyright (c) 2004-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef CONNECTIONHTTPPROXY_H__
#define CONNECTIONHTTPPROXY_H__

#include "gloox.h"
#include "connectionbase.h"
#include "logsink.h"
#include "taghandler.h"
#include "parser.h"

#include <string>
#include <deque>

namespace gloox
{

  /**
   * @brief This is an implementation of a BOSH (HTTP binding) connection.
   *
   * Usage:
   *
   * @code
   * Client *c = new Client( ... );
   * c->setConnectionImpl( new ConnectionBOSH( c,
   *                                new ConnectionTCP( c->logInstance(), ProxyHost, ProxyPort ),
   *                                c->logInstance(), xmppHost, xmppPort, BOSHHost ) );
   * @endcode
   *
   * Make sure to pass the BOSH connection manager's host/port to the transport connection (ConnectionTCP in this case),
   * and the XMPP server's host and port to the BOSH connection. You must also pass to BOSH the address of the BOSH server 
   * you are dealing with, this is used in the HTTP Host header.
   *
   * The reason why ConnectionBOSH doesn't manage its own ConnectionTCP is that it allows it
   * to be used with other transports (like IPv6 or chained SOCKS5/HTTP proxies).
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @author Matthew Wild <mwild1@gmail.com>
   * @since 0.9
   */
  class GLOOX_API ConnectionBOSH : public ConnectionBase, ConnectionDataHandler, TagHandler
  {
    public:
      /**
       * Constructs a new ConnectionBOSH object.
       * @param connection A transport connection. It should be configured to connect to
       * the proxy host and port, @b not to the XMPP host. ConnectionBOSH will own the
       * transport connection and delete it in its destructor.
       * @param logInstance The log target. Obtain it from ClientBase::logInstance().
       * @param boshHost The hostname of the BOSH connection manager
       * @param xmppServer A server to connect to. This is the XMPP server's address, @b not the proxy.
       * @param xmppPort The port to connect to. This is the XMPP server's port, @b not the connection manager's.
       * The default of -1 means that SRV records will be used to find out about the actual host:port.
       * @note To properly use this object, you have to set a ConnectionDataHandler using
       * registerConnectionDataHandler(). This is not necessary if this object is
       * part of a 'connection chain', e.g. with ConnectionSOCKS5Proxy.
       */
      ConnectionBOSH( ConnectionBase *connection, const LogSink& logInstance, const std::string& boshHost,
                           const std::string& xmppServer, int xmppPort = -1);

      /**
       * Constructs a new ConnectionBOSH object.
       * @param cdh An ConnectionDataHandler-derived object that will handle incoming data.
       * @param connection A transport connection. It should be configured to connect to
       * the proxy host and port, @b not to the XMPP host. ConnectionBOSH will own the
       * transport connection and delete it in its destructor.
       * @param logInstance The log target. Obtain it from ClientBase::logInstance().
       * @param boshHost The hostname of the BOSH connection manager
       * @param server A server to connect to. This is the XMPP server's address, @b not the proxy.
       * @param port The port to connect to. This is the XMPP server's port, @b not the proxy's.
       * The default of -1 means that SRV records will be used to find out about the actual host:port.
       */
      ConnectionBOSH( ConnectionDataHandler *cdh, ConnectionBase *connection,
                           const LogSink& logInstance, const std::string& boshHost,
                           const std::string& xmppServer, int xmppPort = -1 );

      /**
       * Virtual destructor
       */
      virtual ~ConnectionBOSH();

      enum ConnMode
      {
        ModeLegacyHTTP             = 0, // HTTP 1.0 connections, closed after receiving a response
        ModePersistentHTTP        = 1, // HTTP 1.1 connections, re-used after receiving a response
        ModePipelining                 = 3 // HTTP Pipelining (implies HTTP 1.1) a single connection is used
      };

      // reimplemented from ConnectionBase
      virtual ConnectionError connect();

      // reimplemented from ConnectionBase
      virtual ConnectionError recv( int timeout = -1 );

      // reimplemented from ConnectionBase
      virtual bool send( const std::string& data );

      // reimplemented from ConnectionBase
      virtual ConnectionError receive();

      // reimplemented from ConnectionBase
      virtual void disconnect();

      // reimplemented from ConnectionBase
      virtual void cleanup();

      // reimplemented from ConnectionBase
      virtual void getStatistics( int &totalIn, int &totalOut );

      // reimplemented from ConnectionDataHandler
      virtual void handleReceivedData( const ConnectionBase* connection, const std::string& data );

      // reimplemented from ConnectionDataHandler
      virtual void handleConnect( const ConnectionBase* connection );

      // reimplemented from ConnectionDataHandler
      virtual void handleDisconnect( const ConnectionBase* connection, ConnectionError reason );

      // reimplemented from ConnectionDataHandler
      virtual ConnectionBase* newInstance() const;

      /**
       * Sets the XMPP server to proxy to.
       * @param host The XMPP server hostname (IP address).
       * @param port The XMPP server port. The default of -1 means that SRV records will be used
       * to find out about the actual host:port.
       */
      void setServer( const std::string& xmppHost, unsigned short xmppPort = -1 )
        { m_server = xmppHost; m_port = xmppPort; }
  
        /**
       * Sets the path on the connection manager to request
       * @param path The path, for example "/http-bind/"
       */
      void setPath( const std::string& path ) { m_path = path; }
        
      /**
       * Sets the connection mode
       * @param mode The connection mode, ConnMode
       */
      void setMode(ConnMode mode) { m_connMode = mode; }
        
      // reimplemented from TagHandler
      virtual void handleTag(Tag* tag);
      
  protected:
      ConnectionBOSH &operator=( const ConnectionBOSH& );
      void initInstance(ConnectionBase* connection, const std::string& xmppServer, const int xmppPort);
      void handleXMLData(const ConnectionBase* connection, const std::string& data);
      bool sendRequest(const std::string& xml, bool ignoreRequestLimit = false);
      bool sendXML(const std::string& data);
      std::string GetHTTPField(const std::string& field);

      //ConnectionBase *m_connection;
      const LogSink& m_logInstance;
   
      Parser* m_parser; // Used for parsing XML section of responses
      std::string m_boshHost; // 
      std::string m_path; // The path part of the URL that we need to request
      ConnectionDataHandler* m_handler; // This is where data will be passed to when received

      std::string m_proxyServer;
      std::string m_proxyPort;

      // BOSH parameters
      long m_rid;
      std::string m_sid;
      
      bool m_initialStreamSent;
      int m_openRequests;
      int m_maxOpenRequests;
      int m_wait;
      int m_hold;
 
      bool m_streamRestart; // Set to true if we are waiting for an acknowledgement of a stream restart

      long m_lastRequestTime;
      long m_minTimePerRequest;
      
      std::string m_buffer; // Buffer of received data
      std::string m_bufferHeader; // HTTP header of data currently in buffer
      long m_bufferContentLength; // Length of the data in the current response
      
      std::string m_sendBuffer; // Data waiting to be sent

      std::deque<ConnectionBase *> m_activeConnections;
      std::deque<ConnectionBase *> m_connectionPool;
      ConnMode m_connMode;
  };

}

#endif // CONNECTIONHTTPPROXY_H__
