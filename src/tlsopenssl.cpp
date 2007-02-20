/*
  Copyright (c) 2005-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#include "tlsopenssl.h"

namespace gloox
{

  OpenSSL::OpenSSL( TLSHandler *th, const std::string& server )
    : TLSBase( th, server )
  {
  }

  OpenSSL::~OpenSSL()
  {
  }

  bool OpenSSL::encrypt( const std::string& data )
  {
    int ret;
    ret = SSL_write( m_ssl, data, len );
    return true;
  }

  int OpenSSL::decrypt( const std::string& data )
  {
    return SSL_read( m_ssl, data, len );
  }

  void OpenSSL::cleanup()
  {
    SSL_shutdown( m_ssl );
    SSL_free( m_ssl );
  }

  bool OpenSSL::handshake()
  {
    SSL_library_init();
    SSL_CTX *sslCTX = SSL_CTX_new( TLSv1_client_method() );
    if( !sslCTX )
      return false;

    if( !SSL_CTX_set_cipher_list( sslCTX, "HIGH:MEDIUM:AES:@STRENGTH" ) )
      return false;

    StringList::const_iterator it = m_cacerts.begin();
    for( ; it != m_cacerts.end(); ++it )
      SSL_CTX_load_verify_locations( sslCTX, (*it).c_str(), NULL );

    if( !m_clientKey.empty() && !m_clientCerts.empty() )
    {
      SSL_CTX_use_certificate_chain_file( sslCTX, m_clientCerts.c_str() );
      SSL_CTX_use_PrivateKey_file( sslCTX, m_clientKey.c_str(), SSL_FILETYPE_PEM );
    }

    m_ssl = SSL_new( sslCTX );
    SSL_set_connect_state( m_ssl );

    BIO *socketBio = BIO_new_socket( m_socket, BIO_NOCLOSE );
    if( !socketBio )
      return false;

    SSL_set_bio( m_ssl, socketBio, socketBio );
    SSL_set_mode( m_ssl, SSL_MODE_AUTO_RETRY );

    if( !SSL_connect( m_ssl ) )
      return false;

    m_secure = true;

    int res = SSL_get_verify_result( m_ssl );
    if( res != X509_V_OK )
      m_certInfo.status = CertInvalid;
    else
      m_certInfo.status = CertOk;

    X509 *peer;
    peer = SSL_get_peer_certificate( m_ssl );
    if( peer )
    {
      char peer_CN[256];
      X509_NAME_get_text_by_NID( X509_get_issuer_name( peer ), NID_commonName, peer_CN, sizeof( peer_CN ) );
      m_certInfo.issuer = peer_CN;
      X509_NAME_get_text_by_NID( X509_get_subject_name( peer ), NID_commonName, peer_CN, sizeof( peer_CN ) );
      m_certInfo.server = peer_CN;
      std::string p;
      p.assign( peer_CN );
      int (*pf)( int ) = tolower;
      transform( p.begin(), p.end(), p.begin(), pf );
      if( p != m_server )
        m_certInfo.status |= CertWrongPeer;
    }
    else
    {
      m_certInfo.status = CertInvalid;
    }

    const char *tmp;
    tmp = SSL_get_cipher_name( m_ssl );
    if( tmp )
      m_certInfo.cipher = tmp;

    tmp = SSL_get_cipher_version( m_ssl );
    if( tmp )
      m_certInfo.protocol = tmp;

    return true;
  }

}
