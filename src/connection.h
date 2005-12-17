/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef CONNECTION_H__
#define CONNECTION_H__

#ifdef WIN32
# include "../config.h.win"
#else
# include "config.h"
#endif

#include "gloox.h"

#include <string>

#if defined( HAVE_OPENSSL )
# define USE_OPENSSL
# include <openssl/ssl.h>
# define HAVE_TLS
#elif defined( HAVE_GNUTLS )
# define USE_GNUTLS
# include <gnutls/gnutls.h>
# include <gnutls/x509.h>
# define HAVE_TLS
#endif

#ifdef HAVE_ZLIB
# include <zlib.h>
#endif

namespace gloox
{

  class Packet;
  class Parser;

  /**
   * @brief This is an implementation of a TLS-aware connection handler.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.4
   */
  class GLOOX_EXPORT Connection
  {
    public:
      /**
       * Constructs a new Connection object.
       * You should not need to use this function directly.
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
       * all data has been sent.
       * @param data The data to send.
       */
      void send( const std::string& data );

      /**
       * Use this function to put the connection into 'receive mode'.
       * @return Returns a value indicating the disconnection reason.
       */
      ConnectionError receive();

      /**
       * Disconnects an established connection. NOOP if no active connection exists.
       * @param e A ConnectionError decribing why the connection is terminated. Well, its not really an
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

      /**
       * Gives access to the raw file descriptor of a connection. Use it wisely. Especially, you should not
       * ::recv() any data from it. There is no way to feed that back into the parser. You can
       * select()/poll() it and use Connection::recv( -1 ) to fetch the data.
       * @return The file descriptor of the active connection, or -1 if no connection is established.
       */
      int fileDescriptor();

#ifdef HAVE_ZLIB
      /**
       * In case Zlib is available, this function can be used to enable stream compression
       * as defined in JEP-0138.
       * @param compression Whether to enable or disable stream compression.
       * @return Returns @b true if compression was successfully enabled, @b false otherwise.
       */
       void setCompression( bool compression );

      /**
       * In case Zlib is available, this function is used to init or de-init stream compression. You must
       * call this before enabling compression using setCompression().
       * @param init Whether to init (@b true) or de-init (@b false) stream compression.
       * @return Returns @b true if compression was successfully initialized/de-initialized,
       * @b false otherwise.
       */
       bool initCompression( bool init );
#endif

#ifdef HAVE_TLS
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
      z_stream m_zinflate;
#endif

#if defined( USE_GNUTLS )

      bool verifyAgainstCAs( gnutls_x509_crt_t cert, gnutls_x509_crt_t *CAList, int CAListSize );
      bool verifyAgainst( gnutls_x509_crt_t cert, gnutls_x509_crt_t issuer );

      gnutls_session_t m_session;
      gnutls_certificate_credentials m_credentials;

#elif defined( USE_OPENSSL )
      SSL *m_ssl;
#endif

      StringList m_cacerts;

      Parser *m_parser;
      ConnectionState m_state;
      CertInfo m_certInfo;
      ConnectionError m_disconnect;

      char *m_buf;
      std::string m_server;
      int m_port;
      int m_socket;
      int m_compCount;
      int m_decompCount;
      int m_dataOutCount;
      int m_dataInCount;
      bool m_cancel;
      bool m_secure;
      bool m_compression;
      bool m_fdRequested;
      bool m_compInited;
  };

}

#endif // CONNECTION_H__
