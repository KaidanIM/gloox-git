/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef SOCKS5BYTESTREAMSERVER_H__
#define SOCKS5BYTESTREAMSERVER_H__

#include "macros.h"
#include "connectionhandler.h"
#include "logsink.h"

namespace gloox
{

  class ConnectionTCPServer;

  /**
   * @brief A server listening for SOCKS5 bytestreams.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.9
   */
  class GLOOX_API SOCKS5BytestreamServer : public ConnectionHandler, ConnectionDataHandler
  {
    public:
      /**
       * Constructs a new SOCKS5BytestreamServer.
       */
      SOCKS5BytestreamServer( const LogSink& logInstance, const std::string& ip, int port );

      /**
       * Destructor.
       */
      ~SOCKS5BytestreamServer();

      /**
       * Starts listening on the specified interface and port.
       * @return Returns @c ConnNoError on success, @c ConnIoError on failure.
       */
      ConnectionError listen();

      /**
       * Call this function repeatedly to check for incoming connections and to negotiate
       * them.
       * @param timeout The timeout to use for select in microseconds. Default of -1 means blocking.
       * @return The state of the listening socket.
       */
      ConnectionError recv( int timeout = -1 );

      /**
       * Stops listening and unbinds from the interface and port.
       */
      void stop();

      // re-implemented from ConnectionHandler
      virtual void handleIncomingConnection( ConnectionBase* connection );

      // re-implemented from ConnectionDataHandler
      virtual void handleReceivedData( const ConnectionBase* connection, const std::string& data );

      // re-implemented from ConnectionDataHandler
      virtual void handleConnect( const ConnectionBase* connection );

      // re-implemented from ConnectionDataHandler
      virtual void handleDisconnect( const ConnectionBase* connection, ConnectionError reason );

    private:
      enum NegotiationState
      {
        StateDisconnected,
        StateUnnegotiated,
        StateAuthmethodAccepted,
        StateAuthAccepted,
        StateDestinationAccepted,
        StateActive
      };

      struct ConnectionInfo
      {
        NegotiationState state;
        std::string hash;
      };

      typedef std::map<ConnectionBase*, ConnectionInfo> ConnectionMap;
      ConnectionMap m_connections;

      ConnectionTCPServer* m_tcpServer;

      const LogSink& m_logInstance;
      std::string m_ip;
      int m_port;

  };

}

#endif // SOCKS5BYTESTREAMSERVER_H__
