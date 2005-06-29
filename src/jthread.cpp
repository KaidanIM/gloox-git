/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>

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
#include "jclient.h"

#include <unistd.h>


JThread::JThread( JClient* parent )
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
  if( m_parent->debug() ) printf("thread run()ing\n");
  int ret;
  while( ( m_parent->clientState() >= JClient::STATE_CONNECTED ) && !m_cancel ) {
    ret = iks_recv( m_parser, 0 );
    if( ret != IKS_OK ) {
      switch( ret ) {
        case IKS_HOOK:
          if( m_parent->debug() ) printf("IKS_HOOK\n");
          m_parent->disconnect();
          return;

        case IKS_NET_TLSFAIL:
          if( m_parent->debug() ) printf("tls failed\n");
          m_parent->setClientState( JClient::STATE_IO_ERROR );
          m_parent->disconnect();
          return;

        default:
          if( m_parent->debug() ) printf("everything's alright\n");
          m_parent->setClientState( JClient::STATE_IO_ERROR );
          m_parent->disconnect();
          return;
      }
    }
    // Allow another thread to execute (cancel-point)
    sleep( 200 );
  }
}

void JThread::cancel()
{
  if( m_parent->debug() ) printf("canceling...\n");
  m_cancel = true;
}

void JThread::over()
{
  m_over = true;
}
