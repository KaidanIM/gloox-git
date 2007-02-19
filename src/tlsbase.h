/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#ifndef TLSBASE_H__
#define TLSBASE_H__

#ifdef WIN32
# include "../config.h.win"
#elif defined( _WIN32_WCE )
# include "../config.h.win"
#else
# include "config.h"
#endif

#if defined( HAVE_OPENSSL )
# define USE_OPENSSL
# define HAVE_TLS
#elif defined( HAVE_GNUTLS )
# define USE_GNUTLS
# define HAVE_TLS
#elif defined( HAVE_WINTLS )
# define USE_WINTLS
# define HAVE_TLS
#endif

#include "gloox.h"

#include <sys/types.h>

namespace gloox
{

  class TLSHandler;

  /**
   * @brief An abstract base class for TLS implementations.
   *
   * @author Jakob Schröter <js@camaya.net>
   * @since 0.9
   */
  class TLSBase
  {
    public:
      /**
       * Constructor.
       */
      TLSBase( TLSHandler *th ) : m_handler( th ), m_secure( false ) {};

      /**
       * Virtual destructor.
       */
      virtual ~TLSBase() { if( m_secure ) cleanup(); };

      /**
       *
       */
      virtual bool encrypt( const std::string& data ) = 0;

      /**
       *
       */
      virtual int decrypt( const std::string& data ) = 0;

      /**
       *
       */
      virtual void cleanup() {};

      /**
       *
       */
      virtual bool handshake() = 0;

      /**
       * Use this function to set a number of trusted root CA certificates which shall be
       * used to verify a servers certificate.
       * @param cacerts A list of absolute paths to CA root certificate files in PEM format.
       */
      void setCACerts( const StringList& cacerts ) { m_cacerts = cacerts; };

      /**
       * This function is used to retrieve certificate and connection info of a encrypted connection.
       * @return Certificate information.
       */
      const CertInfo& fetchTLSInfo() const { return m_certInfo; };

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
       */
      void setClientCert( const std::string& clientKey, const std::string& clientCerts )
      {
        m_clientKey = clientKey;
        m_clientCerts = clientCerts;
      };

    protected:
      TLSHandler *m_handler;
      StringList m_cacerts;
      std::string m_clientKey;
      std::string m_clientCerts;
      CertInfo m_certInfo;
      bool m_secure;

  };

}

#endif // TLSBASE_H__
