/*
 * (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
 */




#include "jthread.h"
#include "jclient.h"

#include <unistd.h>


JThread::JThread( JClient* parent )
  : m_parent( parent ), m_cancel( false ), m_parser( m_parent->parser() )
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
    // check for data
    ret = iks_recv( m_parser, 0 );
    // check for error
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
    // Allow an other thread to execute (cancel-point)
    sleep( 200 );
  }
}

void JThread::cancel()
{
  if( m_parent->debug() ) printf("canceling...\n");
  m_cancel = true;
}
