/*
 * Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
 * This file is part of the gloox library. http://camaya.net/gloox
 *
 * This software is distributed under a license. The full license
 * agreement can be found in the file LICENSE in this distribution.
 * This software may not be copied, modified, sold or distributed
 * other than expressed in the named license agreement.
 *
 * This software is distributed without any warranty.
 */

#include "tlsdefault.h"

#include "tlshandler.h"

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
# include "tlsopenssl.h"
#elif defined( HAVE_GNUTLS )
# define USE_GNUTLS
# define HAVE_TLS
# include "tlsgnutls.h"
#elif defined( HAVE_WINTLS )
# define USE_WINTLS
# define HAVE_TLS
# include "tlsschannel.h"
#endif

namespace gloox
{

  TLSDefault::TLSDefault( TLSHandler *th, const std::string server )
    : TLSBase( th, server ), m_impl( 0 )
  {
#ifdef USE_GNUTLS
    m_impl = new GnuTLSClient( th, server );
#elif defined( USE_OPENSSL )
    m_impl = new OpenSSL( th, server );
#elif defined( USE_WINTLS )
    m_impl = new SChannel( th, server );
#else
    m_impl = 0;
#endif
  }

  TLSDefault::~TLSDefault()
  {
    delete m_impl;
  }

  bool TLSDefault::encrypt( const std::string& data )
  {
    if( m_impl )
      return m_impl->encrypt( data );

    return false;
  }

  int TLSDefault::decrypt( const std::string& data )
  {
    if( m_impl )
      return m_impl->decrypt( data );

    return 0;
  }

  void TLSDefault::cleanup()
  {
    if( m_impl )
      m_impl->cleanup();
  }

  bool TLSDefault::handshake()
  {
    if( m_impl )
      return m_impl->handshake();

    return false;
  }

  bool TLSDefault::isSecure() const
  {
    if( m_impl )
      return m_impl->isSecure();

    return false;
  }

  void TLSDefault::setCACerts( const StringList& cacerts )
  {
    if( m_impl )
      m_impl->setCACerts( cacerts );
  }

  const CertInfo& TLSDefault::fetchTLSInfo() const
  {
    if( m_impl )
      return m_impl->fetchTLSInfo();

    return m_certInfo;
  }

  void TLSDefault::setClientCert( const std::string& clientKey, const std::string& clientCerts )
  {
    if( m_impl )
      m_impl->setClientCert( clientKey, clientCerts );
  }

}
