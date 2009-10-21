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

#include "tlsdefault.h"

#include "config.h"

#if defined( HAVE_GNUTLS )
# define HAVE_TLS
# include "tlsgnutlsclient.h"
# include "tlsgnutlsclientanon.h"
# include "tlsgnutlsserver.h"
# include "tlsgnutlsserveranon.h"
#elif defined( HAVE_OPENSSL )
# define HAVE_TLS
# include "tlsopensslclient.h"
#ifndef __SYMBIAN32__
# include "tlsopensslserver.h"
#endif
#elif defined( HAVE_WINTLS )
# define HAVE_TLS
# include "tlsschannel.h"
#endif

namespace gloox
{

  TLSDefault::TLSDefault( const std::string server, Type type )
    : TLSBase( server ), m_impl( 0 )
  {
    switch( type )
    {
      case VerifyingClient:
#ifdef HAVE_GNUTLS
        m_impl = new GnuTLSClient( server );
#elif defined( HAVE_OPENSSL )
        m_impl = new OpenSSLClient( server );
#elif defined( HAVE_WINTLS )
        m_impl = new SChannel( server );
#endif
        break;
      case AnonymousClient:
#ifdef HAVE_GNUTLS
        m_impl = new GnuTLSClientAnon();
#endif
        break;
      case AnonymousServer:
#ifdef HAVE_GNUTLS
        m_impl = new GnuTLSServerAnon();
#endif
        break;
      case VerifyingServer:
#ifdef HAVE_GNUTLS
        m_impl = new GnuTLSServer();
#elif defined( HAVE_OPENSSL )
#ifndef __SYMBIAN32__
        m_impl = new OpenSSLServer();
#endif
#endif
        break;
      default:
        break;
    }

    if( m_impl )
    {
      m_impl->dataDecrypted.Connect( this, &TLSDefault::handleDecryptedData );
      m_impl->dataEncrypted.Connect( this, &TLSDefault::handleEncryptedData );
      m_impl->handshakeFinished.Connect( this, &TLSDefault::handleHandshakeResult );
    }
  }

  TLSDefault::~TLSDefault()
  {
    delete m_impl;
  }

  bool TLSDefault::init( const std::string& clientKey,
                         const std::string& clientCerts,
                         const StringList& cacerts )
  {
    return m_impl ? m_impl->init( clientKey, clientCerts,
                                  cacerts ) : false;
  }

  int TLSDefault::types()
  {
    int types = 0;
#ifdef HAVE_GNUTLS
    types |= VerifyingClient;
    types |= AnonymousClient;
    types |= AnonymousServer;
#elif defined( HAVE_OPENSSL )
    types |= VerifyingClient;
    types |= VerifyingServer;
#elif defined( HAVE_WINTLS )
    types |= VerifyingClient;
#endif
    return types;
  }

  bool TLSDefault::encrypt( const std::string& data )
  {
    return m_impl ? m_impl->encrypt( data ) : false;
  }

  int TLSDefault::decrypt( const std::string& data )
  {
    return m_impl ? m_impl->decrypt( data ) : 0;
  }

  void TLSDefault::cleanup()
  {
    if( m_impl )
      m_impl->cleanup();
  }

  bool TLSDefault::handshake()
  {
    return m_impl ? m_impl->handshake() : false;
  }

  bool TLSDefault::isSecure() const
  {
    return m_impl ? m_impl->isSecure() : false;
  }

  void TLSDefault::setCACerts( const StringList& cacerts )
  {
    if( m_impl )
      m_impl->setCACerts( cacerts );
  }

  const CertInfo& TLSDefault::fetchTLSInfo() const
  {
    return m_impl ? m_impl->fetchTLSInfo() : m_certInfo;
  }

  void TLSDefault::setClientCert( const std::string& clientKey, const std::string& clientCerts )
  {
    if( m_impl )
      m_impl->setClientCert( clientKey, clientCerts );
  }

  void TLSDefault::handleEncryptedData( const TLSBase* base, const std::string& data )
  {
    dataEncrypted( base, data );
  }

  void TLSDefault::handleDecryptedData( const TLSBase* base, const std::string& data )
  {
    dataDecrypted( base, data );
  }

  void TLSDefault::handleHandshakeResult( const TLSBase* base, bool success,
                                          CertInfo &certinfo )
  {
    handshakeFinished( base, success, certinfo );
  }


}
