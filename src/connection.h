/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*/


#ifndef CONNECTION_H__
#define CONNECTION_H__

#include "gloox.h"

#include <string>

namespace gloox
{

  class Packet;
  class Parser;

  /**
   * This is an implementation of a TLS-aware connection handler.
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.4
   */
  class Connection
  {
    public:
      /**
       * Constructor.
       */
      Connection( Parser *parser, const std::string& server, int port = -1 );

      /**
       * Virtual destructor
       */
      virtual ~Connection();

      /**
       * Contains the actual thread implementation
       */
      int connect();

      /**
       *
       */
      void send( const std::string& data );

      /**
       *
       */
      int receive();

      /**
       * Disconnects an established connection. NOOP if no active connection exists.
       */
      void disconnect();

      /**
       * Call this function to start a TLS handshake over an established connection.
       * @return Whether the handshake was successful.
       */
      bool tlsHandshake();

      /**
       *
       */
      bool isSecure() const { return m_secure; };

      /**
       *
       */
      void startSASL( SaslMechanisms type, const std::string& username, const std::string& password );

      /**
       *
       */
      ConnectionState state() const { return m_state; };

      /**
       *
       */
      void setState( ConnectionState state ) { m_state = state; };

    private:
      void cancel();
      void cleanup();

      static const int BUFSIZE = 1024;
      Parser *m_parser;
      ConnectionState m_state;

      char *m_buf;
      std::string m_server;
      int m_port;
      int m_socket;
      bool m_cancel;
      bool m_secure;

  };

};

#endif // CONNECTION_H__
