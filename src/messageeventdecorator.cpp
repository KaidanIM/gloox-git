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

  MessageEventDecorator::MessageEventDecorator( MessageSession *ms, int defaultEvents )
    : SessionDecorator( ms ), m_parent( ms ), m_messageEventHandler( 0 ), m_requestedEvents( 0 ),
      m_defaultEvents( defaultEvents )
  {
  }

  MessageEventDecorator::~MessageEventDecorator()
  {
  }

  void MessageEventDecorator::handleMessage( Stanza *stanza )
  {
    if( ( m_messageEventHandler ) && stanza->hasChild( "x", "xmlns", XMLNS_X_EVENT ) )
    {
      if( stanza->body().empty() )
      {
        Tag *x = stanza->findChild( "x" );
        if( x->hasChild( "offline" ) )
          m_messageEventHandler->handleMessageEvent( stanza->from(), MESSAGE_EVENT_OFFLINE );
        else if( x->hasChild( "delivered" ) )
          m_messageEventHandler->handleMessageEvent( stanza->from(), MESSAGE_EVENT_DELIVERED );
        else if( x->hasChild( "displayed" ) )
          m_messageEventHandler->handleMessageEvent( stanza->from(), MESSAGE_EVENT_DISPLAYED );
        else if( x->hasChild( "composing" ) )
          m_messageEventHandler->handleMessageEvent( stanza->from(), MESSAGE_EVENT_COMPOSING );
        else
          m_messageEventHandler->handleMessageEvent( stanza->from(), MESSAGE_EVENT_CANCEL );
      }
      else
      {
        m_requestedEvents = 0;
        Tag *x = stanza->findChild( "x" );
        if( x->hasChild( "offline" ) )
          m_requestedEvents |= MESSAGE_EVENT_OFFLINE;
        if( x->hasChild( "delivered" ) )
          m_requestedEvents |= MESSAGE_EVENT_DELIVERED;
        if( x->hasChild( "displayed" ) )
          m_requestedEvents |= MESSAGE_EVENT_DISPLAYED;
        if( x->hasChild( "composing" ) )
          m_requestedEvents |= MESSAGE_EVENT_COMPOSING;
      }
    }
    else
    {
      m_requestedEvents = 0;
      m_lastID.clear();
    }

    m_parent->handleMessage( stanza );
  }

  void MessageEventDecorator::raiseEvent( MessageEventType event )
  {
    if( event & m_requestedEvents )
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
        case MESSAGE_EVENT_CANCEL:
          break;
      }

      send( m );
    }
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
