/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#ifndef CONNECTION_H__
#define CONNECTION_H__

#include "config.h"

#include "gloox.h"

#include <string>

#ifdef HAVE_GNUTLS
#include <gnutls/gnutls.h>
#include <gnutls/x509.h>
#endif

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
       * Use this function to send a string of data over the wire. The function returns only after
       * all data has been send.
       * @param data The data to send.
       */
      void send( const std::string& data );

      /**
       * Use this function to put the connection into 'receive mode'.
       */
      int receive();

      /**
       * Disconnects an established connection. NOOP if no active connection exists.
       */
      void disconnect();

      /**
       * Call this function to start a TLS handshake over an established connection.
       */
      bool tlsHandshake();

      /**
       * Use this function to determine whether an esatblished connection is encrypted.
       * @return @b True if the connection is encrypted, @b false otherwise.
       */
      bool isSecure() const { return m_secure; };

      /**
       * Returns the current connection state.
       * @return The state of the connection.
       */
      ConnectionState state() const { return m_state; };

      /**
       * Sets the state of the connection. This can be used to indicate successful authentication.
       * A parameter of 'STATE_DISCONNECTED' will not disconnect.
       * @param state The new connection state.
       */
      void setState( ConnectionState state ) { m_state = state; };

#ifdef HAVE_GNUTLS
      /**
       * Use this function to set a number of trusted root CA certificates. which shall be
       * used to verify a servers certificate.
       * @param cacerts A list of absolute paths to CA root certificate files in PEM format.
       */
      void setCACerts( const StringList& cacerts ) { m_cacerts = cacerts; };

      /**
       * This function is used to retrieve certificate and connection info of a encrypted connection.
       * @return Certificate information.
       */
      const CertInfo& fetchTLSInfo() const { return m_certInfo; };
#endif

    private:
      void cancel();
      void cleanup();

#ifdef HAVE_GNUTLS
      bool verifyAgainstCAs( gnutls_x509_crt_t cert, gnutls_x509_crt_t *CAList, int CAListSize );
      bool verifyAgainst( gnutls_x509_crt_t cert, gnutls_x509_crt_t issuer );

      gnutls_session_t m_session;
      gnutls_certificate_credentials m_credentials;

      StringList m_cacerts;
#endif

      static const int BUFSIZE = 1024;
      Parser *m_parser;
      ConnectionState m_state;
      CertInfo m_certInfo;

      char *m_buf;
      std::string m_server;
      int m_port;
      int m_socket;
      bool m_cancel;
      bool m_secure;

  };

};

#endif // CONNECTION_H__
