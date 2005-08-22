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

#include <gnutls/gnutls.h>

#include <string>

namespace gloox
{

  Connection::Connection( Parser *parser, const std::string& server, int port )
    : m_parser( parser ), m_server( Prep::idna( server ) ), m_port( port ),
      m_cancel( false ), m_socket( 0 ), m_buf( 0 ), m_secure( false )
  {
    m_buf = (char*)calloc( BUFSIZE, sizeof( char ) );
    printf( "new connection\n" );
  }

  Connection::~Connection()
  {
    cleanup();
    free( m_buf );
    m_buf = 0;
    printf( "~Connection(): connection deleted\n" );
  }

  void Connection::tlsHandshake()
  {
#ifdef HAVE_GNUTLS


#endif
  }

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
//         size = gnutls_record_recv();
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
            case Parser::PARSER_HOOK:
              printf( "PARSER_HOOK (from IKS_HOOK). what does that mean?\n" );
              break;
          }
#endif
        disconnect();
        }
      }
    }
    m_secure = false;
  }

  void Connection::send( const std::string& data )
  {
    if( data.empty() )
      return;

    char *xml = strdup( data.c_str() );
    if( !xml )
      return;

#ifdef HAVE_GNUTLS
    if( m_secure )
    {
//       if( gnutls_record_send( m_session, xml.c_str(), xml.length() ) < 0 )
//         return IKS_NET_RWERR;
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
  }

};
