/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "messageeventfilter.h"
#include "messageeventhandler.h"
#include "messagesession.h"
#include "stanza.h"

namespace gloox
{

  MessageEventFilter::MessageEventFilter( MessageSession *parent, int defaultEvents )
    : MessageFilter( parent ), m_messageEventHandler( 0 ), m_requestedEvents( 0 ),
      m_defaultEvents( defaultEvents ), m_lastSent( MESSAGE_EVENT_CANCEL )
  {
  }

  MessageEventFilter::~MessageEventFilter()
  {
  }

  void MessageEventFilter::filter( Stanza *stanza )
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
  }

  void MessageEventFilter::raiseMessageEvent( MessageEventType event )
  {
    if( ( m_requestedEvents & event ) ||
          ( ( m_lastSent == MESSAGE_EVENT_COMPOSING ) && ( event == MESSAGE_EVENT_CANCEL ) ) )
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
          m_requestedEvents ^= event;
          break;
        case MESSAGE_EVENT_DELIVERED:
          new Tag( x, "delivered" );
          m_requestedEvents ^= event;
          break;
        case MESSAGE_EVENT_DISPLAYED:
          new Tag( x, "displayed" );
          m_requestedEvents ^= event;
          break;
        case MESSAGE_EVENT_COMPOSING:
          if( m_lastSent != MESSAGE_EVENT_COMPOSING )
          {
            new Tag( x, "composing" );
          }
          break;
        case MESSAGE_EVENT_CANCEL:
          break;
      }

      m_lastSent = event;

      m_parent->send( m );
    }
  }

  void MessageEventFilter::decorate( Tag *tag )
  {
    if( m_defaultEvents != 0 )
    {
      Tag *x = new Tag( tag, "x" );
      x->addAttribute( "xmlns", XMLNS_X_EVENT );

      if( m_defaultEvents & MESSAGE_EVENT_OFFLINE )
        new Tag( x, "offline" );
      if( m_defaultEvents & MESSAGE_EVENT_DELIVERED )
        new Tag( x, "delivered" );
      if( m_defaultEvents & MESSAGE_EVENT_DISPLAYED )
        new Tag( x, "displayed" );
      if( m_defaultEvents & MESSAGE_EVENT_COMPOSING )
        new Tag( x, "composing" );
    }
  }

  void MessageEventFilter::registerMessageEventHandler( MessageEventHandler *meh )
  {
    m_messageEventHandler = meh;
  }

  void MessageEventFilter::removeMessageEventHandler()
  {
    m_messageEventHandler = 0;
  }

}
