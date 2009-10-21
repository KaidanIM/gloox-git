/*
 * Copyright (c) 2007-2009 by Jakob Schroeter <js@camaya.net>
 * This file is part of the gloox library. http://camaya.net/gloox
 *
 * This software is distributed under a license. The full license
 * agreement can be found in the file LICENSE in this distribution.
 * This software may not be copied, modified, sold or distributed
 * other than expressed in the named license agreement.
 *
 * This software is distributed without any warranty.
 */

#ifndef CONNECTIONTLS_H__
#define CONNECTIONTLS_H__

#include "gloox.h"
#include "logsink.h"
#include "connectionbase.h"
#include "sigslot.h"
#include "tlsdefault.h"

#include <string>

namespace gloox
{

  /**
   * @brief This is an implementation of a TLS/SSL connection.
   *
   * You should not need to use this function directly. However,
   * you can use it to connect to the legacy Jabber SSL port,
   * 5223.
   *
   * Usage:
   * @code
   * Client *c = new Client( ... );
   * c->setConnectionImpl( new ConnectionTLS( new ConnectionTCP( c->logInstance(), server, 5223 ),
   *                                          c->logInstance()) );
   * @endcode
   *
   * Due to the need for handshaking data to be sent/received before the connection is fully
   * established, be sure not to use the connection until ConnectionDataHandler::handleConnect()
   * of the specified ConnectionDataHandler is called.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @author Matthew Wild <mwild1@gmail.com>
   * @since 1.0
   */

  class GLOOX_API ConnectionTLS : public ConnectionBase, public has_slots<>
  {
    public:
      /**
       * Constructs a new ConnectionTLS object.
       * @param conn A transport connection. It should be configured to connect to
       * the server and port you wish to make the encrypted connection to.
       * ConnectionTLS will own the transport connection and delete it in its destructor.
       * @param log The log target. Obtain it from ClientBase::logInstance().
       */
      ConnectionTLS( ConnectionBase* conn, const LogSink& log );

      /**
       * Virtual Destructor.
       */
      virtual ~ConnectionTLS();

      /**
       * Use this function to set a number of trusted root CA certificates which shall be
       * used to verify a servers certificate.
       * @param cacerts A list of absolute paths to CA root certificate files in PEM format.
       * @note This function is a wrapper for TLSBase::setCACerts().
       */
      void setCACerts( const StringList& cacerts )
      {
        m_cacerts = cacerts;
      }

      /**
       * This function is used to retrieve certificate and connection info of a encrypted connection.
       * @return Certificate information.
       * @note This funcztion is a wrapper around TLSBase::fetchTLSInfo().
       */
      const CertInfo& fetchTLSInfo() const { return m_certInfo; }

      /**
       * Use this function to set the user's certificate and private key. The certificate will
       * be presented to the server upon request and can be used for SASL EXTERNAL authentication.
       * The user's certificate file should be a bundle of more than one certificate in PEM format.
       * The first one in the file should be the user's certificate, each cert following that one
       * should have signed the previous one.
       * @note These certificates are not necessarily the same as those used to verify the server's
       * certificate.
       * @param clientKey The absolute path to the user's private key in PEM format.
       * @param clientCerts A path to a certificate bundle in PEM format.
       * @note This function is a wrapper around TLSBase::setClientCert().
       */
      void setClientCert( const std::string& clientKey, const std::string& clientCerts )
      {
        m_clientKey = clientKey;
        m_clientCerts = clientCerts;
      }

      /**
       * Sets the transport connection.
       * @param connection The transport connection to use.
       */
      void setConnectionImpl( ConnectionBase* connection );

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
      virtual void getStatistics( long int& totalIn, long int& totalOut );

      /**
       * Reimplement this function to receive encrypted data from a TLSBase implementation.
       * @param base The encryption implementation which called this function.
       * @param data The encrypted data (e.g. to send over the wire).
       */
      void handleEncryptedData( const TLSBase* base, const std::string& data );

      /**
       * Reimplement this function to receive decrypted data from a TLSBase implementation.
       * @param base The encryption implementation which called this function.
       * @param data The decrypted data (e.g. to parse).
       */
      void handleDecryptedData( const TLSBase* base, const std::string& data );

      /**
       * Reimplement this function to receive the result of a TLS handshake.
       * @param base The encryption implementation which called this function.
       * @param success Whether or not the handshake was successful.
       * @param certinfo Information about the server's certificate.
       */
      void handleHandshakeResult( const TLSBase* base, bool success, CertInfo &certinfo );

      // reimplemented from ConnectionBase
      void handleReceivedData( const ConnectionBase* connection, const std::string& data );

      // reimplemented from ConnectionBase
      void handleConnect( const ConnectionBase* connection );

      // reimplemented from ConnectionBase
      void handleDisconnect( const ConnectionBase* connection, ConnectionError reason );

      // reimplemented from ConnectionBase
      virtual ConnectionBase* newInstance() const;

      /**
       * This signal is emitted when the TLS handshake finidshed.
       * @param success Whether or not the handshake was successful.
       * @param certinfo Information about the server's certificate.
       */
      signal2<bool, CertInfo&> handshakeFinished;

    protected:
      /**
       * Returns a TLS object (client). Reimplement to change the
       * type of the object.
       * @return A TLS object.
       */
      virtual TLSBase* getTLSBase( const std::string server )
      {
        return new TLSDefault( server, TLSDefault::VerifyingClient );
      }

      ConnectionBase* m_connection;
      TLSBase* m_tls;
      CertInfo m_certInfo;
      const LogSink& m_log;
      StringList m_cacerts;
      std::string m_clientCerts;
      std::string m_clientKey;

    private:
      ConnectionTLS& operator=( const ConnectionTLS& );

  };

}

#endif // CONNECTIONTLS_H__
