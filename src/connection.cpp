/*
  Copyright (c) 2004-2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#include "gloox.h"

#include "compression.h"
#include "connection.h"
#include "dns.h"
#include "logsink.h"
#include "prep.h"
#include "parser.h"

#ifdef __MINGW32__
#include <winsock.h>
#endif

#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#else
#include <winsock.h>
#endif

#if defined( _MSC_VER ) && ( _MSC_VER >= 1300 )
#define strcasecmp stricmp
#endif

#include <time.h>

#include <string>

namespace gloox
{

  Connection::Connection( Parser *parser, const LogSink& logInstance, const std::string& server,
                          int port )
    : m_parser( parser ), m_state ( StateDisconnected ), m_disconnect ( ConnNoError ),
      m_logInstance( logInstance ), m_compression( 0 ), m_buf( 0 ),
      m_server( Prep::idna( server ) ), m_port( port ), m_socket( -1 ), m_bufsize( 17000 ),
      m_cancel( true ), m_secure( false ), m_fdRequested( false ), m_enableCompression( false )
  {
    m_buf = (char*)calloc( m_bufsize + 1, sizeof( char ) );
#ifdef USE_OPENSSL
    m_ssl = 0;
#endif
  }

  Connection::~Connection()
  {
    cleanup();
    free( m_buf );
    m_buf = 0;
  }

#ifdef HAVE_TLS
  void Connection::setClientCert( const std::string& clientKey, const std::string& clientCerts )
  {
    m_clientKey = clientKey;
    m_clientCerts = clientCerts;
  }
#endif

#if defined( USE_OPENSSL )
  bool Connection::tlsHandshake()
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
      if( strcasecmp( peer_CN, m_server.c_str() ) )
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

#elif defined( USE_GNUTLS )
  bool Connection::tlsHandshake()
  {
    const int protocolPriority[] = { GNUTLS_TLS1, GNUTLS_SSL3, 0 };
    const int kxPriority[]       = { GNUTLS_KX_RSA, 0 };
    const int cipherPriority[]   = { GNUTLS_CIPHER_AES_256_CBC, GNUTLS_CIPHER_AES_128_CBC,
                                             GNUTLS_CIPHER_3DES_CBC, GNUTLS_CIPHER_ARCFOUR, 0 };
    const int compPriority[]     = { GNUTLS_COMP_ZLIB, GNUTLS_COMP_NULL, 0 };
    const int macPriority[]      = { GNUTLS_MAC_SHA, GNUTLS_MAC_MD5, 0 };

    if( gnutls_global_init() != 0 )
      return false;

    if( gnutls_certificate_allocate_credentials( &m_credentials ) < 0 )
      return false;

    StringList::const_iterator it = m_cacerts.begin();
    for( ; it != m_cacerts.end(); ++it )
      gnutls_certificate_set_x509_trust_file( m_credentials, (*it).c_str(), GNUTLS_X509_FMT_PEM );

    if( !m_clientKey.empty() && !m_clientCerts.empty() )
    {
      gnutls_certificate_set_x509_key_file( m_credentials, m_clientKey.c_str(),
                                            m_clientCerts.c_str(), GNUTLS_X509_FMT_PEM );
    }

    if( gnutls_init( &m_session, GNUTLS_CLIENT ) != 0 )
    {
      gnutls_certificate_free_credentials( m_credentials );
      return false;
    }

    gnutls_protocol_set_priority( m_session, protocolPriority );
    gnutls_cipher_set_priority( m_session, cipherPriority );
    gnutls_compression_set_priority( m_session, compPriority );
    gnutls_kx_set_priority( m_session, kxPriority );
    gnutls_mac_set_priority( m_session, macPriority );
    gnutls_credentials_set( m_session, GNUTLS_CRD_CERTIFICATE, m_credentials );

    gnutls_transport_set_ptr( m_session, (gnutls_transport_ptr_t)m_socket );
    if( gnutls_handshake( m_session ) != 0 )
    {
      gnutls_deinit( m_session );
      gnutls_certificate_free_credentials( m_credentials );
      return false;
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

    return true;
  }

  bool Connection::verifyAgainst( gnutls_x509_crt_t cert, gnutls_x509_crt_t issuer )
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

  bool Connection::verifyAgainstCAs( gnutls_x509_crt_t cert, gnutls_x509_crt_t *CAList, int CAListSize )
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
#endif

#ifdef HAVE_ZLIB
  bool Connection::initCompression( StreamFeature method )
  {
    delete m_compression;
    m_compression = 0;
    m_compression = new Compression( method );
    return true;
  }

  void Connection::enableCompression()
  {
    if( !m_compression )
      return;

    m_enableCompression = true;
  }
#endif

  ConnectionState Connection::connect()
  {
    if( m_socket != -1 && m_state >= StateConnecting )
    {
      return m_state;
    }

    m_state = StateConnecting;

    if( m_port == -1 )
      m_socket = DNS::connect( m_server, m_logInstance );
    else
      m_socket = DNS::connect( m_server, m_port, m_logInstance );

    if( m_socket < 0 )
    {
      switch( m_socket )
      {
        case -DNS::DNS_COULD_NOT_CONNECT:
          m_logInstance.log( LogLevelError, LogAreaClassConnection, "connection error: could not connect" );
          break;
        case -DNS::DNS_NO_HOSTS_FOUND:
          m_logInstance.log( LogLevelError, LogAreaClassConnection, "connection error: no hosts found" );
          break;
        case -DNS::DNS_COULD_NOT_RESOLVE:
          m_logInstance.log( LogLevelError, LogAreaClassConnection, "connection error: could not resolve" );
          break;
      }
      cleanup();
    }
    else
      m_state = StateConnected;

    m_cancel = false;
    return m_state;
  }

  void Connection::disconnect( ConnectionError e )
  {
    m_disconnect = e;
    m_cancel = true;

    if( m_fdRequested )
      cleanup();
  }

  int Connection::fileDescriptor()
  {
    m_fdRequested = true;
    return m_socket;
  }

  ConnectionError Connection::recv( int timeout )
  {
    if( m_cancel )
    {
      ConnectionError e = m_disconnect;
      cleanup();
      return e;
    }

    if( m_socket == -1 )
      return ConnNotConnected;

    if( !m_fdRequested )
    {
      fd_set fds;
      struct timeval tv;

      FD_ZERO( &fds );
      FD_SET( m_socket, &fds );

      tv.tv_sec = timeout;
      tv.tv_usec = 0;

      if( select( m_socket + 1, &fds, 0, 0, timeout == -1 ? 0 : &tv ) < 0 )
        return ConnIoError;

      if( !FD_ISSET( m_socket, &fds ) )
        return ConnNoError;
    }

    // optimize(?): recv returns the size. set size+1 = \0
    memset( m_buf, '\0', m_bufsize + 1 );
    int size = 0;
#if defined( USE_GNUTLS )
    if( m_secure )
    {
      size = gnutls_record_recv( m_session, m_buf, m_bufsize );
    }
    else
#elif defined( USE_OPENSSL )
    if( m_secure )
    {
      size = SSL_read( m_ssl, m_buf, m_bufsize );
    }
    else
#endif
    {
#ifdef SKYOS
      size = ::recv( m_socket, (unsigned char*)m_buf, m_bufsize, 0 );
#else
      size = ::recv( m_socket, m_buf, m_bufsize, 0 );
#endif
    }

    if( size < 0 )
    {
      // error
      return ConnIoError;
    }
    else if( size == 0 )
    {
      // connection closed
      return ConnUserDisconnected;
    }
    else
    {
      std::string buf;
      if( m_compression && m_enableCompression )
      {
        buf.assign( m_buf, size );
        buf = m_compression->decompress( buf );
      }
      else
        buf.assign( m_buf, strlen( m_buf ) );

      Parser::ParserState ret = m_parser->feed( buf );
      if( ret != Parser::PARSER_OK )
      {
        cleanup();
        switch( ret )
        {
          case Parser::PARSER_BADXML:
            m_logInstance.log( LogLevelError, LogAreaClassConnection, "XML parse error" );
            break;
          case Parser::PARSER_NOMEM:
            m_logInstance.log( LogLevelError, LogAreaClassConnection, "memory allocation error" );
            break;
          default:
            break;
        }
        return ConnIoError;
      }
    }

    return ConnNoError;
  }

  ConnectionError Connection::receive()
  {
    if( m_socket == -1 || !m_parser )
      return ConnNotConnected;

    while( !m_cancel )
    {
      ConnectionError r = recv();
      if( r != ConnNoError )
        return r;
    }
    cleanup();

    return m_disconnect;
  }

  void Connection::send( const std::string& data )
  {
    if( data.empty() || ( m_socket == -1 ) )
      return;

    std::string xml;
    if( m_compression && m_enableCompression )
      xml = m_compression->compress( data );
    else
      xml = data;

#if defined( USE_GNUTLS )
    if( m_secure )
    {
      int ret;
      size_t len = xml.length();
      do
      {
        ret = gnutls_record_send( m_session, xml.c_str(), len );
      }
      while( ( ret == GNUTLS_E_AGAIN ) || ( ret == GNUTLS_E_INTERRUPTED ) );
    }
    else
#elif defined( USE_OPENSSL )
    if( m_secure )
    {
      int ret;
      size_t len = xml.length();
      ret = SSL_write( m_ssl, xml.c_str(), len );
    }
    else
#endif
    {
      size_t num = 0;
      size_t len = xml.length();
      while( num < len )
      {
#ifdef SKYOS
        num += ::send( m_socket, (unsigned char*)(xml.c_str()+num), len - num, 0 );
#else
        num += ::send( m_socket, (xml.c_str()+num), len - num, 0 );
#endif
      }
    }
  }

  void Connection::cleanup()
  {
#if defined( USE_GNUTLS )
    if( m_secure )
    {
      gnutls_bye( m_session, GNUTLS_SHUT_RDWR );
      gnutls_deinit( m_session );
      gnutls_certificate_free_credentials( m_credentials );
      gnutls_global_deinit();
    }
#elif defined( USE_OPENSSL )
    if( m_secure )
    {
      SSL_shutdown( m_ssl );
      SSL_free( m_ssl );
    }
#endif

    if( m_socket != -1 )
    {
#ifdef WIN32
      closesocket( m_socket );
#else
      close( m_socket );
#endif
      m_socket = -1;
    }
    m_state = StateDisconnected;
    m_disconnect = ConnNoError;
    m_enableCompression = false;
    m_secure = false;
    m_cancel = true;
    m_fdRequested = false;
  }

}
