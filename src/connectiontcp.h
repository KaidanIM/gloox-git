/*
  Copyright (c) 2004-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef CONNECTIONTCP_H__
#define CONNECTIONTCP_H__

#include "gloox.h"
#include "connectionbase.h"
#include "logsink.h"

#include <string>

namespace gloox
{

  class Mutex;

  /**
   * @brief This is an implementation of a simple TCP connection.
   *
   * You should only need to use this class directly if you need access to some special feature, like
   * the raw socket(), or if you need HTTP proxy support (see @ref gloox::ConnectionHTTPProxy for more
   * information).
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.9
   */
  class GLOOX_API ConnectionTCP : public ConnectionBase
  {
    public:
      /**
       * Constructs an empty ConnectionTCP object. You need to call setServer() to make it useable.
       * @param logInstance The log target. Obtain it from ClientBase::logInstance().
       */
      ConnectionTCP( const LogSink& logInstance );

      /**
       * Constructs a new ConnectionTCP object.
       * @param logInstance The log target. Obtain it from ClientBase::logInstance().
       * @param server A server to connect to.
       * @param port The port to connect to. The default of -1 means that SRV records will be used
       * to find out about the actual host:port.
       * @note To properly use this object, you have to set a ConnectionDataHandler using
       * registerConnectionDataHandler(). This is not necessary if this object is
       * part of a 'connection chain', e.g. with ConnectionHTTPProxy.
       */
      ConnectionTCP( const LogSink& logInstance,
                     const std::string& server, int port = -1 );

      /**
       * Constructs a new ConnectionTCP object.
       * @param cdh An ConnectionDataHandler-derived object that will handle incoming data.
       * @param logInstance The log target. Obtain it from ClientBase::logInstance().
       * @param server A server to connect to.
       * @param port The port to connect to. The default of -1 means that SRV records will be used
       * to find out about the actual host:port.
       */
      ConnectionTCP( ConnectionDataHandler *cdh, const LogSink& logInstance,
                     const std::string& server, int port = -1 );

      /**
       * Virtual destructor
       */
      virtual ~ConnectionTCP();

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

      // reimplemented from ConnectionBase
      virtual ConnectionTCP* newInstance() const;

      /**
       * Gives access to the raw socket of this connection. Use it wisely. You can
       * select()/poll() it and use ConnectionTCP::recv( -1 ) to fetch the data.
       * @return The socket of the active connection, or -1 if no connection is established.
       */
      int socket() const { return m_socket; };

      /**
       * This function allows to set an existing socket with an established
       * connection to use in this connection. You will still need to call connect() in order to
       * negotiate the XMPP stream. You should not set a new socket after having called connect().
       * @param socket The existing socket.
       */
      void setSocket( int socket ) { m_socket = socket; };

    private:
      ConnectionTCP &operator= ( const ConnectionTCP & );
      void init();
      bool dataAvailable( int timeout = -1 );
      void cancel();

      const LogSink& m_logInstance;
      Mutex* m_sendMutex;
      Mutex* m_recvMutex;

      char *m_buf;
      int m_socket;
      int m_totalBytesIn;
      int m_totalBytesOut;
      const int m_bufsize;
      bool m_cancel;

  };

}

#endif // CONNECTIONTCP_H__
