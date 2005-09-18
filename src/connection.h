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

#ifdef HAVE_ZLIB
#include <zlib.h>
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
       * Constructs a new Connection object.
       * You should not need to use thsi function directly.
       * @param parser A parser to feed with incoming data.
       * @param server A server to connect to.
       * @param port The port to connect to. The default of -1 means that SRV records will be used
       * to find out about the actual host:port.
       */
      Connection( Parser *parser, const std::string& server, int port = -1 );

      /**
       * Virtual destructor
       */
      virtual ~Connection();

      /**
       * Used to initiate the connection.
       * @return Returns the connection state.
       */
      ConnectionState connect();

      /**
       * Use this periodically to receive data from the socket and to feed the parser.
       * @param timeout The timeout to use for select. Default means blocking.
       * @return The state of the connection.
       */
      ConnectionError recv( int timeout = -1 );

      /**
       * Use this function to send a string of data over the wire. The function returns only after
       * all data has been send.
       * @param data The data to send.
       */
      void send( const std::string& data );

      /**
       * Use this function to put the connection into 'receive mode'.
       * @return Returns a value describing the disconnection reason.
       */
      ConnectionError receive();

      /**
       * Disconnects an established connection. NOOP if no active connection exists.
       * param e A ConnectionError decribing why the connection is terminated. Well, its not really an
       * error here, but...
       */
      void disconnect( ConnectionError e );

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

#ifdef HAVE_ZLIB
      /**
       * In case Zlib is available, this function can be used to enable stream compression
       * as defined in JEP-0138.
       * @param compression Whether to enable or disable stream compression.
       * @return Returns @b true if compression was successfully enabled, @b false otherwise.
       */
       bool setCompression( bool compression );
#endif

#ifdef HAVE_GNUTLS
      /**
       * Call this function to start a TLS handshake over an established connection.
       */
      bool tlsHandshake();

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

#ifdef HAVE_ZLIB
      std::string compress( const std::string& data );
      std::string decompress( const std::string& data );
      z_stream m_zdeflate;
      z_stream m_zinflate;
#endif

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
      ConnectionError m_disconnect;

      char *m_buf;
      std::string m_server;
      int m_port;
      int m_socket;
      bool m_cancel;
      bool m_secure;
      bool m_compression;

  };

};

#endif // CONNECTION_H__
