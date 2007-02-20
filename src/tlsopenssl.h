/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#ifndef TLSOPENSSL_H__
#define TLSOPENSSL_H__

#include "tlsbase.h"

#ifdef HAVE_OPENSSL

#include <openssl/ssl.h>

namespace gloox
{

  /**
   *
   * @author Jakob Schr�ter <js@camaya.net>
   * @since 0.9
   */
  class OpenSSL : public TLSBase
  {
    public:
      /**
       * Constructor.
       */
      OpenSSL( TLSHandler *th, const std::string& server );

      /**
       * Virtual destructor.
       */
      virtual ~OpenSSL();

      // reimplemented from TLSBase
      virtual bool encrypt( const std::string& data );

      // reimplemented from TLSBase
      virtual int decrypt( const std::string& data );

      // reimplemented from TLSBase
      virtual void cleanup();

      // reimplemented from TLSBase
      virtual bool handshake();

      // reimplemented from TLSBase
      virtual void setCACerts( const StringList& cacerts );

      // reimplemented from TLSBase
      virtual void setClientCert( const std::string& clientKey, const std::string& clientCerts );

    private:
      SSL *m_ssl;

  };

}

#endif // HAVE_OPENSSL

#endif // TLSOPENSSL_H__
