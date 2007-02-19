/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#ifndef GNUTLS_H__
#define GNUTLS_H__

#include "tlsbase.h"

#ifdef USE_GNUTLS

#include <gnutls/gnutls.h>
#include <gnutls/x509.h>

namespace gloox
{

  /**
   *
   * @author Jakob Schröter <js@camaya.net>
   * @since 0.9
   */
  class GnuTLS : public TLSBase
  {
    public:
      /**
       * Constructor.
       */
      GnuTLS( TLSHandler *th );

      /**
       * Virtual destructor.
       */
      virtual ~GnuTLS();

      // reimplemented from TLSBase
      virtual bool encrypt( const std::string& data );

      // reimplemented from TLSBase
      virtual int decrypt( const std::string& data );

      // reimplemented from TLSBase
      virtual void cleanup();

      // reimplemented from TLSBase
      virtual bool handshake();

    private:
      bool verifyAgainst( gnutls_x509_crt_t cert, gnutls_x509_crt_t issuer );
      bool verifyAgainstCAs( gnutls_x509_crt_t cert, gnutls_x509_crt_t *CAList, int CAListSize );

      gnutls_session_t m_session;
      gnutls_certificate_credentials m_credentials;

  };

}

#endif // USE_GNUTLS

#endif // GNUTLS_H__
