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

#include <string>

namespace gloox
{

  /**
   * @brief This is an implementation of a simple HTTP Proxying connection.
   *
   * Usage:
   *
   * @code
   * Client *c = new Client( ... );
   * c->setConnectionImpl(
   *       new ConnectionHTTPProxy( c,
   *                                new ConnectionTCP( c->logInstance(),
   *                                                   proxyHost, proxyPort ),
   *                                c->logInstance(), xmppHost, xmppPort ) );
   * @endcode
   *
   * Make sure to pass the proxy host/port to the transport connection (ConnectionTCP in this case),
   * and the XMPP host/port to the proxy connection.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.9
   */
  class GLOOX_API ConnectionHTTPProxy : public ConnectionBase, ConnectionDataHandler
  {
    public:
      /**
       * Constructs a new ConnectionHTTPProxy object.
       * You should not need to use this function directly.
       * @param cdh An ConnectionDataHandler-derived object that will handle incoming data.
       * @param connection A transport connection. It should be configured to connect to
       * the proxy host and port, @b not to the XMPP host. ConnectionHTTPProxy will own the
       * transport connection and delete it in its destructor.
       * @param logInstance The log target. Obtain it from ClientBase::logInstance().
       * @param server A server to connect to. This is the XMPP server's address, @b not the proxy.
       * @param port The port to connect to. This is the XMPP server's port, @b not the proxy's.
       * The default of -1 means that SRV records will be used to find out about the actual host:port.
       */
      ConnectionHTTPProxy( ConnectionDataHandler *cdh, ConnectionBase *connection,
                           const LogSink& logInstance,
                           const std::string& server, unsigned short port = -1 );

      /**
       * Virtual destructor
       */
      virtual ~ConnectionHTTPProxy();

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

      // reimplemented from ConnectionDataHandler
      virtual void handleReceivedData( const std::string& data );

      // reimplemented from ConnectionDataHandler
      virtual void handleConnect();

      // reimplemented from ConnectionDataHandler
      virtual void handleDisconnect( ConnectionError reason );

      /**
       * Returns current connection statistics.
       * @param totalIn The total number of bytes received.
       * @param totalOut The total number of bytes sent.
       */
      void getStatistics( int &totalIn, int &totalOut );

      /**
       * Sets the XMPP server to proxy to.
       * @param host The XMPP server hostname (IP address).
       * @param port The XMPP server port. The default of -1 means that SRV records will be used
       * to find out about the actual host:port.
       */
      void setServer( const std::string& host, unsigned short port = -1 )
        { m_server = host; m_port = port; }

   private:
      ConnectionHTTPProxy &operator= ( const ConnectionHTTPProxy& );

      ConnectionBase *m_connection;
      const LogSink& m_logInstance;

      std::string m_server;
      std::string m_proxyHandshakeBuffer;
      unsigned short m_port;
      int m_totalBytesIn;
      int m_totalBytesOut;
      bool m_proxied;

  };

}

#endif // CONNECTIONHTTPPROXY_H__
