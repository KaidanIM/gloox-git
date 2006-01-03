/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "messagesession.h"

#include "messageeventhandler.h"
#include "messageeventfilter.h"
#include "clientbase.h"
#include "tag.h"

namespace gloox
{

  MessageSession::MessageSession( ClientBase *parent, const JID& jid )
  : m_parent( parent ), m_eventFilter( 0 ), m_target( jid ), m_messageHandler( 0 )
  {
    if( m_parent )
      m_parent->registerMessageHandler( m_target.full(), this );

    m_thread = "gloox" + m_parent->getID();

    m_eventFilter = new MessageEventFilter( this );
  }

  MessageSession::~MessageSession()
  {
    if( m_parent )
      m_parent->removeMessageHandler( m_target.full() );
  }

  void MessageSession::handleMessage( Stanza *stanza )
  {
    if( m_eventFilter )
    {
      printf( "filtering\n");
      m_eventFilter->handleMessage( stanza );
    }

    if( !m_messageHandler )
    {
      printf( "returning, !mh\n");
      return;
    }

    if( stanza->body().empty() )
    {
      printf( "returning, empty body\n");
      return;
    }
    else
      m_messageHandler->handleMessage( stanza );
  }

  void MessageSession::send( const std::string& message, const std::string& subject )
  {
    Tag *m = new Tag( "message" );
    m->addAttribute( "type", "chat" );
    new Tag( m, "body", message );
    if( !subject.empty() )
      new Tag( m, "subject", subject );

    m->addAttribute( "from", m_parent->jid().full() );
    m->addAttribute( "to", m_target.full() );
    new Tag( m, "thread", m_thread );

    if( m_eventFilter )
      m_eventFilter->decorate( m );

    m_parent->send( m );
  }

  void MessageSession::send( Tag *tag )
  {
    m_parent->send( tag );
  }

  void MessageSession::raiseEvent( MessageEventType event )
  {
    if( m_eventFilter )
    {
      printf( "raising event %d\n", event );
      m_eventFilter->raiseEvent( event );
    }
  }

  void MessageSession::registerMessageHandler( MessageHandler *mh )
  {
    printf( "registering mh\n");
    m_messageHandler = mh;
  }

  void MessageSession::removeMessageHandler()
  {
    m_messageHandler = 0;
  }

  void MessageSession::registerMessageEventHandler( MessageEventHandler *meh )
  {
    if( m_eventFilter )
      m_eventFilter->registerMessageEventHandler( meh );
  }

  void MessageSession::removeMessageEventHandler()
  {
    if( m_eventFilter )
      m_eventFilter->removeMessageEventHandler();
  }
}
