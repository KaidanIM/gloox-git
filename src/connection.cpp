/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/



#include "gloox.h"

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
#define strcasecmp stricmp
#endif

#include <time.h>

#include <string>

namespace gloox
{

  static const int BUFSIZE = 1024;

  Connection::Connection( Parser *parser, const std::string& server, int port )
    : m_parser( parser ), m_buf( 0 ), m_server( Prep::idna( server ) ), m_port( port ),
      m_socket( -1 ), m_compCount( 0 ), m_decompCount( 0 ), m_dataOutCount( 0 ),
      m_dataInCount( 0 ), m_cancel( true ), m_secure( false ), m_compression( false ),
      m_fdRequested( false ), m_compInited( false )
  {
    m_buf = (char*)calloc( BUFSIZE, sizeof( char ) );
  }

  Connection::~Connection()
  {
    cleanup();
    free( m_buf );
    m_buf = 0;
#ifdef HAVE_ZLIB
    initCompression( false );
#endif
  }

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
      m_certInfo.status = CERT_INVALID;
    else
      m_certInfo.status = CERT_OK;

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
        m_certInfo.status |= CERT_WRONG_PEER;
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
      m_certInfo.status |= CERT_INVALID;
    if( status & GNUTLS_CERT_SIGNER_NOT_FOUND )
      m_certInfo.status |= CERT_SIGNER_UNKNOWN;
    if( status & GNUTLS_CERT_REVOKED )
      m_certInfo.status |= CERT_REVOKED;
    if( status & GNUTLS_CERT_SIGNER_NOT_CA )
      m_certInfo.status |= CERT_SIGNER_NOT_CA;
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
      m_certInfo.status |= CERT_INVALID;
    m_certInfo.chain = chain;

    m_certInfo.chain = verifyAgainstCAs( cert[certListSize], 0 /*CAList*/, 0 /*CAListSize*/ );

    int t = (int)gnutls_x509_crt_get_expiration_time( cert[0] );
    if( t == -1 )
      error = true;
    else if( t < time( 0 ) )
      m_certInfo.status |= CERT_EXPIRED;
    m_certInfo.date_from = t;

    t = (int)gnutls_x509_crt_get_activation_time( cert[0] );
    if( t == -1 )
      error = true;
    else if( t > time( 0 ) )
      m_certInfo.status |= CERT_NOT_ACTIVE;
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
      m_certInfo.status |= CERT_WRONG_PEER;

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
  bool Connection::initCompression( bool init )
  {
    int ret = Z_OK;

    if( init )
    {
      m_zinflate.zalloc = Z_NULL;
      m_zinflate.zfree = Z_NULL;
      m_zinflate.opaque = Z_NULL;
      ret = inflateInit( &m_zinflate );
    }
    else if( m_compInited && !init )
      inflateEnd( &m_zinflate );

    if( ret == Z_OK )
    {
      m_compInited = init;
      return true;
    }
    else
    {
      m_compInited = false;
      return false;
    }
  }

  void Connection::setCompression( bool compression )
  {
    if( m_compInited )
      m_compression = compression;
  }

  std::string Connection::compress( const std::string& data )
  {
    if( data.empty() )
      return "";

    int CHUNK = data.length() + ( data.length() / 100 ) + 13;
    Bytef *out = new Bytef[CHUNK];
    char *in = const_cast<char*>( data.c_str() );

    ::compress( out, (uLongf*)CHUNK, (Bytef*)in, data.length() );
    std::string result;
    result.assign( (char*)out, CHUNK );
    m_compCount += result.length();
    m_dataOutCount += data.length();
    delete[] out;

    return result;
  }

  std::string Connection::decompress( const std::string& data )
  {
    if( data.empty() )
      return "";

    int CHUNK = data.length() * 10;
    char *out = new char[CHUNK];
    char *in = const_cast<char*>( data.c_str() );

    m_zinflate.avail_in = data.length();
    m_zinflate.next_in = (Bytef*)in;

    int ret;
    std::string result, tmp;
    do {
      m_zinflate.avail_out = CHUNK;
      m_zinflate.next_out = (Bytef*)out;

      ret = inflate( &m_zinflate, Z_FINISH );
      tmp.assign( out, CHUNK - m_zinflate.avail_out );
      result += tmp;
    } while( m_zinflate.avail_out == 0 );

    m_decompCount += result.length();
    m_dataInCount += data.length();
    delete[] out;

    return result;
  }
#endif

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
      cleanup();
      return CONN_USER_DISCONNECTED;
    }

    if( !m_fdRequested )
    {
      fd_set fds;
      struct timeval tv;

      FD_ZERO( &fds );
      FD_SET( m_socket, &fds );

      tv.tv_sec = timeout;
      tv.tv_usec = 0;

      if( select( m_socket + 1, &fds, 0, 0, timeout == -1 ? 0 : &tv ) < 0 )
        return CONN_IO_ERROR;

      if( !FD_ISSET( m_socket, &fds ) )
        return CONN_OK;
    }

    // optimize(?): recv returns the size. set size+1 = \0
    memset( m_buf, '\0', BUFSIZE );
    int size;
#if defined( USE_GNUTLS )
    if( m_secure )
    {
      size = gnutls_record_recv( m_session, m_buf, BUFSIZE );
    }
    else
#elif defined( USE_OPENSSL )
    if( m_secure )
    {
      size = SSL_read( m_ssl, m_buf, BUFSIZE );
    }
    else
#endif
    {
#ifdef SKYOS
      size = ::recv( m_socket, (unsigned char*)m_buf, BUFSIZE - 1, 0 );
#else
      size = ::recv( m_socket, m_buf, BUFSIZE - 1, 0 );
#endif
    }

    if( size < 0 )
    {
      // error
      return CONN_IO_ERROR;
    }
    else if( size == 0 )
    {
      // connection closed
      return CONN_USER_DISCONNECTED;
    }
    else
    {
      std::string buf;
#ifdef HAVE_ZLIB
      if( m_compression )
        buf = decompress( m_buf );
      else
#endif
        buf = m_buf;

      Parser::ParserState ret = m_parser->feed( buf );
      if( ret != Parser::PARSER_OK )
      {
        cleanup();
        switch( ret )
        {
          case Parser::PARSER_BADXML:
            LogSink::instance().log( LOG_ERROR, LOG_CLASS_CONNECTION, "XML parse error" );
            break;
          case Parser::PARSER_NOMEM:
            LogSink::instance().log( LOG_ERROR, LOG_CLASS_CONNECTION, "memory allocation error" );
            break;
          default:
            break;
        }
        return CONN_IO_ERROR;
      }
    }

    return CONN_OK;
  }

  ConnectionError Connection::receive()
  {
    if( m_socket == -1 || !m_parser )
      return CONN_IO_ERROR;

    while( !m_cancel )
    {
      ConnectionError r = recv();
      if( r != CONN_OK )
        return r;
    }

    return m_disconnect;
  }

  void Connection::send( const std::string& data )
  {
    if( data.empty() )
      return;

    char *xml;
#ifdef HAVE_ZLIB
    if( m_compression )
      xml = strdup( compress( data ).c_str() );
    else
#endif
      xml = strdup( data.c_str() );

    if( !xml )
      return;

#if defined( USE_GNUTLS )
    if( m_secure )
    {
      int ret;
      int len = strlen( xml );
      do
      {
        ret = gnutls_record_send( m_session, xml, len );
      }
      while( ( ret == GNUTLS_E_AGAIN ) || ( ret == GNUTLS_E_INTERRUPTED ) );
    }
    else
#elif defined( USE_OPENSSL )
    if( m_secure )
    {
      int ret;
      int len = strlen( xml );
      ret = SSL_write( m_ssl, xml, len );
    }
    else
#endif
    {
      int num = 0;
      int len = strlen( xml );
      while( num < len )
#ifdef SKYOS
        num += ::send( m_socket, (unsigned char*)(xml+num), len - num, 0 );
#else
        num += ::send( m_socket, (xml+num), len - num, 0 );
#endif
    }

    free( xml );
  }

  ConnectionState Connection::connect()
  {
    if( m_socket != -1 && m_state >= STATE_CONNECTING )
      return m_state;

    m_state = STATE_CONNECTING;

    if( m_port == -1 )
      m_socket = DNS::connect( m_server );
    else
      m_socket = DNS::connect( m_server, m_port );

    if( m_socket < 0 )
    {
      switch( m_socket )
      {
        case -DNS::DNS_COULD_NOT_CONNECT:
          LogSink::instance().log( LOG_ERROR, LOG_CLASS_CONNECTION, "connection error: could not connect" );
          break;
        case -DNS::DNS_NO_HOSTS_FOUND:
          LogSink::instance().log( LOG_ERROR, LOG_CLASS_CONNECTION, "connection error: no hosts found" );
          break;
        case -DNS::DNS_COULD_NOT_RESOLVE:
          LogSink::instance().log( LOG_ERROR, LOG_CLASS_CONNECTION, "connection error: could not resolve" );
          break;
      }
      cleanup();
    }
    else
      m_state = STATE_CONNECTED;

    m_cancel = false;
    return m_state;
  }

  void Connection::cleanup()
  {
    if( m_socket != -1 )
    {
#ifdef WIN32
      closesocket( m_socket );
#else
      close( m_socket );
#endif
      m_socket = -1;
    }
    m_state = STATE_DISCONNECTED;
    m_disconnect = CONN_OK;

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
    m_secure = false;
    m_cancel = true;
    m_fdRequested = false;
  }

}
