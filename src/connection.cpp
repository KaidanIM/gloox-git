/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*/



#include "gloox.h"

#include "connection.h"
#include "dns.h"
#include "prep.h"
#include "parser.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>

#include <string>

namespace gloox
{

  Connection::Connection( Parser *parser, const std::string& server, int port )
    : m_parser( parser ), m_server( Prep::idna( server ) ), m_port( port ),
      m_cancel( false ), m_socket( 0 ), m_buf( 0 ), m_secure( false )
  {
    m_buf = (char*)calloc( BUFSIZE, sizeof( char ) );
  }

  Connection::~Connection()
  {
    cleanup();
    free( m_buf );
    m_buf = 0;
  }

#ifdef HAVE_GNUTLS
  bool Connection::tlsHandshake()
  {
    const int protocol_priority[] = { GNUTLS_TLS1, GNUTLS_SSL3, 0 };
    const int kx_priority[]       = { GNUTLS_KX_RSA, 0 };
    const int cipher_priority[]   = { GNUTLS_CIPHER_AES_256_CBC, GNUTLS_CIPHER_AES_128_CBC,
                                             GNUTLS_CIPHER_3DES_CBC, GNUTLS_CIPHER_ARCFOUR, 0 };
    const int comp_priority[]     = { GNUTLS_COMP_ZLIB, GNUTLS_COMP_NULL, 0 };
    const int mac_priority[]      = { GNUTLS_MAC_SHA, GNUTLS_MAC_MD5, 0 };

    if( gnutls_global_init() != 0 )
      return false;

    if( gnutls_certificate_allocate_credentials( &m_credentials ) < 0 )
      return false;

    StringList::const_iterator it = m_cacerts.begin();
    for( it; it != m_cacerts.end(); it++ )
      gnutls_certificate_set_x509_trust_file( m_credentials, (*it).c_str(), GNUTLS_X509_FMT_PEM );

    if( gnutls_init( &m_session, GNUTLS_CLIENT ) != 0 )
    {
      gnutls_certificate_free_credentials( m_credentials );
      return false;
    }

    gnutls_protocol_set_priority( m_session, protocol_priority );
    gnutls_cipher_set_priority( m_session, cipher_priority );
    gnutls_compression_set_priority( m_session, comp_priority );
    gnutls_kx_set_priority( m_session, kx_priority );
    gnutls_mac_set_priority( m_session, mac_priority );
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
    const gnutls_datum_t* certList;
    unsigned int certListSize;
    if( !error && ( ( certList = gnutls_certificate_get_peers( m_session, &certListSize ) ) == 0 ) )
      error = true;

    gnutls_x509_crt_t cert[certListSize];
    for( int i=0; !error && ( i<certListSize ); ++i )
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
    for( int i=1; !error && ( i<certListSize ); i++ )
    {
      chain = error = !verifyAgainst( cert[i-1], cert[i] );
    }
    if( !chain )
      m_certInfo.status |= CERT_INVALID;
    m_certInfo.chain = chain;

    m_certInfo.chain = verifyAgainstCAs( cert[certListSize], 0 /*CAList*/, 0 /*CAListSize*/ );

    int t = (int)gnutls_x509_crt_get_expiration_time( cert[0] );
    if( t = -1 )
      error = true;
    else if( t < time( 0 ) )
      m_certInfo.status |= CERT_EXPIRED;
    m_certInfo.date_from = t;

    t = (int)gnutls_x509_crt_get_activation_time( cert[0] );
    if( t = -1 )
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

    for( int i=0; i<certListSize; i++ )
      gnutls_x509_crt_deinit( cert[i] );

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

  void Connection::disconnect()
  {
    m_cancel = true;
  }

  int Connection::receive()
  {
    printf( "in receive(). socket: %d\n", m_socket );
    if( !m_socket || !m_parser )
      return STATE_DISCONNECTED;

    int size;
    printf( "starting loop\n" );
    while( !m_cancel )
    {
      // optimize(?): recv returns the size. set size+1 = \0
      memset( m_buf, '\0', BUFSIZE );
#ifdef HAVE_GNUTLS
      if( m_secure )
      {
        size = gnutls_record_recv( m_session, m_buf, BUFSIZE );
      }
      else
#endif
      {
        size = recv( m_socket, m_buf, BUFSIZE - 1, 0 );
      }

      if( size < 0 )
      {
        // error
        return STATE_ERROR;
      }
      else if( size == 0 )
      {
        // connection closed
        return STATE_DISCONNECTED;
      }
      else
      {
        // data received
        Parser::ParserState ret = m_parser->feed( m_buf );
        if( ret != Parser::PARSER_OK )
        {
#ifdef DEBUG
          switch( ret )
          {
            case Parser::PARSER_BADXML:
              printf( "XML parse error\n" );
              break;
            case Parser::PARSER_NOMEM:
              printf( "memory allocation error\n" );
              break;
          }
#endif
        cleanup();
        }
      }
    }
  }

  void Connection::send( const std::string& data )
  {
    if( data.empty() )
      return;

    char *xml = strdup( data.c_str() );
    if( !xml )
      return;
    int len = strlen( xml );

#ifdef HAVE_GNUTLS
    if( m_secure )
    {
      int ret;
      do
      {
        ret = gnutls_record_send( m_session, xml, len );
      }
      while( ( ret == GNUTLS_E_AGAIN ) || ( ret == GNUTLS_E_INTERRUPTED ) );
    }
    else
#endif
    {
      int num = 0;
      int len = strlen( xml );
      while( num < len )
        num += ::send( m_socket, (void*)(xml+num), len - num, 0 );
    }

    free( xml );
  }

  int Connection::connect()
  {
    printf( "in connect()\n" );
    if( m_socket && m_state >= STATE_CONNECTING )
      return m_state;

    m_state = STATE_CONNECTING;

    if( m_port == -1 )
      m_socket = DNS::connect( m_server );
    else
      m_socket = DNS::connect( m_server, m_port );

    if( m_socket <= 2 )
    {
#ifdef DEBUG
      switch( m_socket )
      {
        case -DNS::DNS_COULD_NOT_CONNECT:
          printf( "could not connect\n" );
          break;
        case -DNS::DNS_NO_HOSTS_FOUND:
          printf( "no hosts found\n" );
          break;
        case -DNS::DNS_COULD_NOT_RESOLVE:
          printf( "could not resolve\n" );
          break;
      }
      printf( "connection error\n" );
#endif
      cleanup();
    }
    else
      m_state = STATE_CONNECTED;

    return m_state;
  }

  void Connection::cleanup()
  {
    if( m_socket )
    {
      close( m_socket );
      m_socket = 0;
    }
    m_state = STATE_DISCONNECTED;

#ifdef HAVE_GNUTLS
    if( m_secure )
    {
      gnutls_bye( m_session, GNUTLS_SHUT_RDWR );
      gnutls_deinit( m_session );
      gnutls_certificate_free_credentials( m_credentials );
      gnutls_global_deinit();
    }
#endif
    m_secure = false;
  }

};
