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

#ifdef HAVE_OPENSSL

namespace gloox
{

  OpenSSL::OpenSSL( TLSHandler *th, const std::string& server )
    : TLSBase( th, server ), m_ssl( 0 ), m_ctx( 0 ), m_buf( 0 ), m_bufsize( 17000 )
  {
    m_buf = (char*)calloc( m_bufsize + 1, sizeof( char ) );

    SSL_library_init();
    SSL_CTX *m_ctx = SSL_CTX_new( TLSv1_client_method() );
    if( !m_ctx )
      return;

    if( !SSL_CTX_set_cipher_list( m_ctx, "HIGH:MEDIUM:AES:@STRENGTH" ) )
      return;

    m_ssl = SSL_new( m_ctx );
    SSL_set_connect_state( m_ssl );

    if( !BIO_new_bio_pair( &m_ibio, 0, &m_nbio, 0 ) )
    {
      printf( "BIO_new_bio_pair() failed\n" );
      return;
    }
    SSL_set_bio( m_ssl, m_ibio, m_ibio );
    SSL_set_mode( m_ssl, SSL_MODE_AUTO_RETRY );
  }

  OpenSSL::~OpenSSL()
  {
  }

  bool OpenSSL::encrypt( const std::string& data )
  {
    if( !m_secure )
    {
      handshake();
      return 0;
    }

    printf( "encrypting: %d\n", data.length() );
    int ret;
    ret = SSL_write( m_ssl, data.c_str(), data.length() );
    return true;
  }

  int OpenSSL::decrypt( const std::string& data )
  {
    printf( "decrypting: %d\n", data.length() );

    m_recvBuffer += data;

    if( !m_secure )
    {
      handshake();
      return 0;
    }

    return SSL_read( m_ssl, reinterpret_cast<void*>( const_cast<char*>( data.c_str() ) ), data.length() );
  }

  void OpenSSL::setCACerts( const StringList& cacerts )
  {
    m_cacerts = cacerts;

    StringList::const_iterator it = m_cacerts.begin();
    for( ; it != m_cacerts.end(); ++it )
      SSL_CTX_load_verify_locations( m_ctx, (*it).c_str(), NULL );
  }

  void OpenSSL::setClientCert( const std::string& clientKey, const std::string& clientCerts )
  {
    m_clientKey = clientKey;
    m_clientCerts = clientCerts;

    if( !m_clientKey.empty() && !m_clientCerts.empty() )
    {
      SSL_CTX_use_certificate_chain_file( m_ctx, m_clientCerts.c_str() );
      SSL_CTX_use_PrivateKey_file( m_ctx, m_clientKey.c_str(), SSL_FILETYPE_PEM );
    }
  }

  void OpenSSL::cleanup()
  {
    SSL_shutdown( m_ssl );
    SSL_free( m_ssl );
    BIO_free( m_nbio );
  }

  bool OpenSSL::handshake()
  {
    if( !m_handler )
      return false;

    int ret;
    do
    {
      ret = SSL_connect( m_ssl );
      if( ret == -1 )
      {
        switch( SSL_get_error( m_ssl, ret ) )
        {
          case SSL_ERROR_WANT_READ:
            printf( "want read\n" );
          case SSL_ERROR_WANT_WRITE:
            printf( "want write\n" );
            pushFunc();
            break;
          default:
            m_handler->handleHandshakeResult( false, m_certInfo );
            return false;
            break;
        }
//         return true;
      }
    }
    while( ret != 1 || m_recvBuffer.length() );

    m_secure = true;

    int res = SSL_get_verify_result( m_ssl );
    if( res != X509_V_OK )
      m_certInfo.status = CertInvalid;
    else
      m_certInfo.status = CertOk;

    X509 *peer = SSL_get_peer_certificate( m_ssl );
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

  int OpenSSL::pullFunc()
  {
    return 0;
  }

  int OpenSSL::pushFunc()
  {
    printf( "in pushFunc\n" );
    int wantwrite;
    size_t wantread;
    int frombio;
    int tobio;

    while( ( wantwrite = BIO_ctrl_pending( m_nbio ) ) > 0 )
    {
      if( wantwrite > m_bufsize )
        wantwrite = m_bufsize;

      if( !wantwrite )
        break;

      printf( "pushing: %d\n", wantwrite );
      frombio = BIO_read( m_nbio, m_buf, wantwrite );
      printf( "read from bio: %d\n", frombio );

      if( m_handler )
        m_handler->handleEncryptedData( std::string( m_buf, frombio ) );

      if( BIO_should_retry( m_nbio ) )
      {
        printf( "after BIO_read: BIO_should_retry() is true\n" );
//         break;
      }
      else
      {
        printf( "after BIO_read: BIO_should_retry() is false\n" );
//         break;
      }

    }

    if( BIO_should_read( m_nbio ) )
      printf( "should read\n" );

    if( BIO_should_write( m_nbio ) )
      printf( "should write\n" );

    while( ( wantread = BIO_ctrl_get_read_request( m_nbio ) ) > 0 )
    {
      if( wantread > m_recvBuffer.length() )
        wantread = m_recvBuffer.length();

      if( !wantread )
        break;

      printf( "pulling: %d\n", wantread );
      tobio = BIO_write( m_nbio, m_recvBuffer.c_str(), wantread );
      printf( "wrote to bio: %d of %d\n", tobio, m_recvBuffer.length() );
      m_recvBuffer.erase( 0, tobio );

     if( BIO_should_retry( m_nbio ) )
      {
        printf( "after BIO_write: BIO_should_retry() is true\n" );
//         break;
      }
      else
      {
        printf( "after BIO_write: BIO_should_retry() is false\n" );
//         break;
      }

    }

    printf( "leaving pushFunc\n" );
    return 0;
  }

}

#endif // HAVE_OPENSSL
