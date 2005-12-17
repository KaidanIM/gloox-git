/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#include "messageeventdecorator.h"
#include "messageeventhandler.h"

namespace gloox
{

  MessageEventDecorator::MessageEventDecorator( MessageSession *ms )
    : SessionDecorator( ms ), m_parent( ms )
  {
  }

  MessageEventDecorator::~MessageEventDecorator()
  {
  }

  void MessageEventDecorator::handleMessage( Stanza *stanza )
  {
    if( !m_messageEventHandler )
      return;

    if( stanza->hasChild( "x", "xmlns", XMLNS_X_EVENT ) )
    {
      if( stanza->body().empty() )
      {
        Tag *x = stanza->findChild( "x" );
        if( x->hasChild( "offline" ) )
          m_messageEventHandler->handleOfflineEvent( stanza->from() );
        else if( x->hasChild( "delivered" ) )
          m_messageEventHandler->handleDeliveredEvent( stanza->from() );
        else if( x->hasChild( "displayed" ) )
          m_messageEventHandler->handleDisplayedEvent( stanza->from() );
        else if( x->hasChild( "composing" ) )
          m_messageEventHandler->handleComposingEvent( stanza->from() );
      }
      else
      {
        int events = 0;
        Tag *x = stanza->findChild( "x" );
        if( x->hasChild( "offline" ) )
          events |= MESSAGE_EVENT_OFFLINE;
        else if( x->hasChild( "delivered" ) )
          events |= MESSAGE_EVENT_DELIVERED;
        else if( x->hasChild( "displayed" ) )
          events |= MESSAGE_EVENT_DISPLAYED;
        else if( x->hasChild( "composing" ) )
          events |= MESSAGE_EVENT_COMPOSING;
      }
    }

    m_parent->handleMessage( stanza );
  }

  void MessageEventDecorator::raiseEvent( MessageEventType event )
  {
    Tag *m = new Tag( "message" );
    m->addAttribute( "to", m_parent->target().full() );
    Tag *x = new Tag( m, "x" );
    x->addAttribute( "xmlns", XMLNS_X_EVENT );
    new Tag( x, "id", m_lastID );

    switch( event )
    {
      case MESSAGE_EVENT_OFFLINE:
        new Tag( x, "offline" );
        break;
      case MESSAGE_EVENT_DELIVERED:
        new Tag( x, "delivered" );
        break;
      case MESSAGE_EVENT_DISPLAYED:
        new Tag( x, "displayed" );
        break;
      case MESSAGE_EVENT_COMPOSING:
        new Tag( x, "composing" );
        break;
    }

    send( m );
  }

  void MessageEventDecorator::send( Tag *tag )
  {
    m_parent->send( tag );
  }

  void MessageEventDecorator::registerMessageHandler( MessageHandler *meh )
  {
    m_parent->registerMessageHandler( meh );
  }

  void MessageEventDecorator::removeMessageHandler()
  {
    m_parent->removeMessageHandler();
  }

  void MessageEventDecorator::registerMessageEventHandler( MessageEventHandler *meh )
  {
    m_messageEventHandler = meh;
  }

  void MessageEventDecorator::removeMessageEventHandler()
  {
    m_messageEventHandler = 0;
  }

}
