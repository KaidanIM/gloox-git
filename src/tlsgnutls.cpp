/*
  Copyright (c) 2005-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#include "tlsgnutls.h"

#ifdef HAVE_GNUTLS

#include <errno.h>

namespace gloox
{

  GnuTLS::GnuTLS( TLSHandler *th, const std::string& server )
    : TLSBase( th, server ), m_buf( 0 ), m_bufsize( 17000 )
  {
    m_buf = (char*)calloc( m_bufsize + 1, sizeof( char ) );

    const int protocolPriority[] = { GNUTLS_TLS1, GNUTLS_SSL3, 0 };
    const int kxPriority[]       = { GNUTLS_KX_RSA, 0 };
    const int cipherPriority[]   = { GNUTLS_CIPHER_AES_256_CBC, GNUTLS_CIPHER_AES_128_CBC,
                                     GNUTLS_CIPHER_3DES_CBC, GNUTLS_CIPHER_ARCFOUR, 0 };
    const int compPriority[]     = { GNUTLS_COMP_ZLIB, GNUTLS_COMP_NULL, 0 };
    const int macPriority[]      = { GNUTLS_MAC_SHA, GNUTLS_MAC_MD5, 0 };

    if( gnutls_global_init() != 0 )
    {
      m_valid = false;
      return;
    }

    if( gnutls_certificate_allocate_credentials( &m_credentials ) < 0 )
    {
      m_valid = false;
      return;
    }

    if( gnutls_init( &m_session, GNUTLS_CLIENT ) != 0 )
    {
      gnutls_certificate_free_credentials( m_credentials );
      m_valid = false;
      return;
    }

    gnutls_protocol_set_priority( m_session, protocolPriority );
    gnutls_cipher_set_priority( m_session, cipherPriority );
    gnutls_compression_set_priority( m_session, compPriority );
    gnutls_kx_set_priority( m_session, kxPriority );
    gnutls_mac_set_priority( m_session, macPriority );
    gnutls_credentials_set( m_session, GNUTLS_CRD_CERTIFICATE, m_credentials );

    gnutls_transport_set_ptr( m_session, (gnutls_transport_ptr_t)this );
    gnutls_transport_set_push_function( m_session, pushFunc );
    gnutls_transport_set_pull_function( m_session, pullFunc );

  }

  GnuTLS::~GnuTLS()
  {
    free( m_buf );
    m_buf = 0;
  }

  bool GnuTLS::encrypt( const std::string& data )
  {
    if( !m_secure )
    {
      handshake();
      return 0;
    }

    int ret;
    do
    {
      ret = gnutls_record_send( m_session, data.c_str(), data.length() );
    }
    while( ( ret == GNUTLS_E_AGAIN ) || ( ret == GNUTLS_E_INTERRUPTED ) );
    return true;
  }

  int GnuTLS::decrypt( const std::string& data )
  {
    m_recvBuffer += data;

    if( !m_secure )
    {
      handshake();
      return 0;
    }

    int ret = gnutls_record_recv( m_session, m_buf, m_bufsize );
    if( ret > 0  && m_handler )
    {
      std::string temp;
      temp.assign( m_buf, ret );
      m_handler->handleDecryptedData( temp );
    }
    return ret;
  }

  void GnuTLS::setCACerts( const StringList& cacerts )
  {
    m_cacerts = cacerts;

    StringList::const_iterator it = m_cacerts.begin();
    for( ; it != m_cacerts.end(); ++it )
      gnutls_certificate_set_x509_trust_file( m_credentials, (*it).c_str(), GNUTLS_X509_FMT_PEM );
  }

  void GnuTLS::setClientCert( const std::string& clientKey, const std::string& clientCerts )
  {
    m_clientKey = clientKey;
    m_clientCerts = clientCerts;

    if( !m_clientKey.empty() && !m_clientCerts.empty() )
    {
      gnutls_certificate_set_x509_key_file( m_credentials, m_clientKey.c_str(),
                                            m_clientCerts.c_str(), GNUTLS_X509_FMT_PEM );
    }
  }

  void GnuTLS::cleanup()
  {
    gnutls_bye( m_session, GNUTLS_SHUT_RDWR );
    gnutls_deinit( m_session );
    gnutls_certificate_free_credentials( m_credentials );
    gnutls_global_deinit();
  }

  bool GnuTLS::handshake()
  {
    if( !m_handler )
      return false;

    int ret = gnutls_handshake( m_session );
    if( ret < 0 && gnutls_error_is_fatal( ret ) )
    {
      gnutls_deinit( m_session );
      gnutls_certificate_free_credentials( m_credentials );
      m_valid = false;

      m_handler->handleHandshakeResult( false, m_certInfo );

      return false;
    }
    else if( ret == GNUTLS_E_AGAIN )
    {
      return true;
    }

    gnutls_certificate_free_ca_names( m_credentials );

    m_secure = true;

    unsigned int status;
    bool error = false;

    if( gnutls_certificate_verify_peers2( m_session, &status ) < 0 )
      error = true;

    m_certInfo.status = 0;
    if( status & GNUTLS_CERT_INVALID )
      m_certInfo.status |= CertInvalid;
    if( status & GNUTLS_CERT_SIGNER_NOT_FOUND )
      m_certInfo.status |= CertSignerUnknown;
    if( status & GNUTLS_CERT_REVOKED )
      m_certInfo.status |= CertRevoked;
    if( status & GNUTLS_CERT_SIGNER_NOT_CA )
      m_certInfo.status |= CertSignerNotCa;
    const gnutls_datum_t* certList = 0;
    unsigned int certListSize;
    if( !error && ( ( certList = gnutls_certificate_get_peers( m_session, &certListSize ) ) == 0 ) )
      error = true;

    gnutls_x509_crt_t *cert = new gnutls_x509_crt_t[certListSize+1];
    for( unsigned int i=0; !error && ( i<certListSize ); ++i )
    {
      if( !error && ( gnutls_x509_crt_init( &cert[i] ) < 0 ) )
        error = true;
      if( !error && ( gnutls_x509_crt_import( cert[i], &certList[i], GNUTLS_X509_FMT_DER ) < 0 ) )
        error = true;
    }

    if( ( gnutls_x509_crt_check_issuer( cert[certListSize-1], cert[certListSize-1] ) > 0 )
         && certListSize > 0 )
      certListSize--;

    bool chain = true;
    for( unsigned int i=1; !error && ( i<certListSize ); ++i )
    {
      chain = error = !verifyAgainst( cert[i-1], cert[i] );
    }
    if( !chain )
      m_certInfo.status |= CertInvalid;
    m_certInfo.chain = chain;

    m_certInfo.chain = verifyAgainstCAs( cert[certListSize], 0 /*CAList*/, 0 /*CAListSize*/ );

    int t = (int)gnutls_x509_crt_get_expiration_time( cert[0] );
    if( t == -1 )
      error = true;
    else if( t < time( 0 ) )
      m_certInfo.status |= CertExpired;
    m_certInfo.date_from = t;

    t = (int)gnutls_x509_crt_get_activation_time( cert[0] );
    if( t == -1 )
      error = true;
    else if( t > time( 0 ) )
      m_certInfo.status |= CertNotActive;
    m_certInfo.date_to = t;

    char name[64];
    size_t nameSize = sizeof( name );
    gnutls_x509_crt_get_issuer_dn( cert[0], name, &nameSize );
    m_certInfo.issuer = name;

    nameSize = sizeof( name );
    gnutls_x509_crt_get_dn( cert[0], name, &nameSize );
    m_certInfo.server = name;

    const char* info;
    info = gnutls_compression_get_name( gnutls_compression_get( m_session ) );
    if( info )
      m_certInfo.compression = info;

    info = gnutls_mac_get_name( gnutls_mac_get( m_session ) );
    if( info )
      m_certInfo.mac = info;

    info = gnutls_cipher_get_name( gnutls_cipher_get( m_session ) );
    if( info )
      m_certInfo.cipher = info;

    info = gnutls_protocol_get_name( gnutls_protocol_get_version( m_session ) );
    if( info )
      m_certInfo.protocol = info;

    if( !gnutls_x509_crt_check_hostname( cert[0], m_server.c_str() ) )
      m_certInfo.status |= CertWrongPeer;

    for( unsigned int i=0; i<certListSize; ++i )
      gnutls_x509_crt_deinit( cert[i] );

    delete[] cert;

    m_valid = true;

    m_handler->handleHandshakeResult( true, m_certInfo );
    return true;
  }

  bool GnuTLS::verifyAgainst( gnutls_x509_crt_t cert, gnutls_x509_crt_t issuer )
  {
    unsigned int result;
    gnutls_x509_crt_verify( cert, &issuer, 1, 0, &result );
    if( result & GNUTLS_CERT_INVALID )
      return false;

    if( gnutls_x509_crt_get_expiration_time( cert ) < time( 0 ) )
      return false;

    if( gnutls_x509_crt_get_activation_time( cert ) > time( 0 ) )
      return false;

    return true;
  }

  bool GnuTLS::verifyAgainstCAs( gnutls_x509_crt_t cert, gnutls_x509_crt_t *CAList, int CAListSize )
  {
    unsigned int result;
    gnutls_x509_crt_verify( cert, CAList, CAListSize, GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT, &result );
    if( result & GNUTLS_CERT_INVALID )
      return false;

    if( gnutls_x509_crt_get_expiration_time( cert ) < time( 0 ) )
      return false;

    if( gnutls_x509_crt_get_activation_time( cert ) > time( 0 ) )
      return false;

    return true;
  }

  ssize_t GnuTLS::pullFunc( void *data, size_t len )
  {
    ssize_t cpy = ( len > m_recvBuffer.length() ) ? ( m_recvBuffer.length() ) : ( len );
    if( cpy > 0 )
    {
      memcpy( data, (const void*)m_recvBuffer.c_str(), cpy );
      m_recvBuffer.erase( 0, cpy );
      return cpy;
    }
    else
    {
      errno = EAGAIN;
      return GNUTLS_E_AGAIN;
    }
  }

  ssize_t GnuTLS::pullFunc( gnutls_transport_ptr_t ptr, void *data, size_t len )
  {
    return static_cast<GnuTLS*>( ptr )->pullFunc( data, len );
  }

  ssize_t GnuTLS::pushFunc( const void *data, size_t len )
  {
    m_sendBuffer.assign( (const char*)data, len );
    if( m_handler )
    {
      m_handler->handleEncryptedData( m_sendBuffer );
      m_sendBuffer = "";
    }
    return len;
  }

  ssize_t GnuTLS::pushFunc( gnutls_transport_ptr_t ptr, const void *data, size_t len )
  {
    return static_cast<GnuTLS*>( ptr )->pushFunc( data, len );
  }

}

#endif // HAVE_GNUTLS
