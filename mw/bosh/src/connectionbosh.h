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
       * Sets the underlying transport connection. A possibly existing connection will be deleted.
       * @param connection The ConnectionBase to replace the current connection, if any.
       */
      void setConnectionImpl( ConnectionBase* connection );

      /**
       * Switches usage of HTTP/1.1 on or off.
       * @param http11 Set this to @b true to connect through a HTTP/1.1-only proxy, or @b false
       * to use HTTP/1.0. Defaults to HTTP/1.0 which should work with 99.9% of proxies.
       */
      void setHTTP11( bool http11 ) { m_http11 = http11; }
        
      // reimplemented from TagHandler
      virtual void handleTag(Tag* tag);

   private:
      ConnectionBOSH &operator=( const ConnectionBOSH& );
      void handleXMLData(const ConnectionBase* connection, const std::string& data);
      std::string GetHTTPField(const std::string& field);

      ConnectionBase *m_connection;
      const LogSink& m_logInstance;
   
      Parser* m_parser; // Used for parsing XML section of responses
      ConnectionDataHandler* m_handler;

      std::string m_path; // The path part of the URL that we need to request
      std::string m_proxyServer;
      std::string m_proxyPort;
      std::string m_boshHost; // 
   
      // BOSH parameters
      long m_rid;
      std::string m_sid;
      int m_hold;
      int m_wait;
      int m_requests;
   
      int m_openRequests;

      bool m_http11; // Persistent connections
      bool m_pipelining; // Multiple requests between responses (on a single connection)
   
      std::string m_buffer;
      std::string m_bufferHeader;
      long m_bufferContentLength;
      
      bool m_initialStreamSent;
      bool m_streamRestart;

  };

}

#endif // CONNECTIONHTTPPROXY_H__
