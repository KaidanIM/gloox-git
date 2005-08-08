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



#include "jthread.h"

#include "clientbase.h"
#include "dns.h"

#include <unistd.h>
#include <errno.h>

// temp
#include <netinet/in.h>
#include <resolv.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
// temp

namespace gloox
{
  JThread::JThread( ClientBase* parent )
    : m_parent( parent ),
      m_cancel( false ), m_over( false ),
      m_parser( m_parent->parser() )
  {
  }

  JThread::~JThread()
  {
  }

  void JThread::run()
  {
    int fd;
    int ret;

    if( m_parent->port() != XMPP_PORT )
      fd = DNS::connect( m_parent->serverIdn(), m_parent->port() );
    else
      fd = DNS::connect( m_parent->serverIdn() );

    if( fd <= 2 )
    {
      switch( fd )
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
      m_parent->disconnect();
      return;
    }
    printf( "created socket %d\n", fd );

    if( (ret = m_parent->Stream::connect( fd ) ) == IKS_NOMEM )
    {
      printf( "memory allocation error. exiting. %d\n", ret );
      m_parent->disconnect();
      return;
    }
    int t = m_parent->header( m_parent->streamTo() );

    m_parent->setState( STATE_CONNECTED );

    while( ( m_parent->state() >= STATE_CONNECTED ) && !m_cancel ) {
      ret = iks_recv( m_parser, 0 );
      if( ret != IKS_OK ) {
        switch( ret ) {
          case IKS_HOOK:
            if( m_parent->debug() ) printf( "IKS_HOOK. what happened???\n" );
            m_parent->disconnect();
            return;

          case IKS_NET_TLSFAIL:
            if( m_parent->debug() ) printf( "tls failed\n" );
            m_parent->setState( STATE_TLS_FAILED );
            m_parent->disconnect();
            return;

          case IKS_NET_RWERR:
            if( m_parent->debug() ) printf( "read/write error. connection was closed unexpectedly.\n" );
            m_parent->setState( STATE_IO_ERROR );
            m_parent->disconnect();
            return;

          case IKS_NET_NOCONN:
            if( m_parent->debug() ) printf( "connection was closed\n" );
            m_parent->setState( STATE_DISCONNECTED );
            return;

          case IKS_BADXML:
            if( m_parent->debug() ) printf( "parse error\n" );
            m_parent->setState( STATE_PARSE_ERROR );
            m_parent->disconnect();
            return;

          case IKS_NOMEM:
            if( m_parent->debug() ) printf( "out of memory\n" );
            m_parent->setState( STATE_OUT_OF_MEMORY );
            m_parent->disconnect();
            return;

          default:
            if( m_parent->debug() ) printf( "something bad happend. I don't know what.\n" );
            m_parent->disconnect();
            return;
        }
      }
      sleep( 200 );
    }
  }

  void JThread::cancel()
  {
    m_cancel = true;
  }

  void JThread::over()
  {
    m_over = true;
  }

};
