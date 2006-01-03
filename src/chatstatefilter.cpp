/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "chatstatefilter.h"
#include "chatstatehandler.h"
#include "messageeventhandler.h"
#include "messagesession.h"
// #include "tag.h"

namespace gloox
{

  ChatStateFilter::ChatStateFilter( MessageSession *parent )
    : m_parent( parent ), m_chatStateHandler( 0 ), m_lastSent( CHAT_STATE_GONE ),
      m_enableChatStates( true )
  {
  }

  ChatStateFilter::~ChatStateFilter()
  {
  }

  void ChatStateFilter::handleMessage( Stanza *stanza )
  {
    if( m_chatStateHandler )
    {
      if( stanza->body().empty() )
      {
        if( stanza->hasChild( "active" ) )
          m_chatStateHandler->handleChatState( stanza->from(), CHAT_STATE_ACTIVE );
        else if( stanza->hasChild( "composing" ) )
          m_chatStateHandler->handleChatState( stanza->from(), CHAT_STATE_COMPOSING );
        else if( stanza->hasChild( "paused" ) )
          m_chatStateHandler->handleChatState( stanza->from(), CHAT_STATE_PAUSED );
        else if( stanza->hasChild( "inactive" ) )
          m_chatStateHandler->handleChatState( stanza->from(), CHAT_STATE_INACTIVE );
        else if( stanza->hasChild( "gone" ) )
          m_chatStateHandler->handleChatState( stanza->from(), CHAT_STATE_GONE );
        else
          m_enableChatStates = false;
      }
      else
      {
        if( stanza->hasChild( "active", "xmlns", XMLNS_CHAT_STATES )
            || stanza->hasChild( "composing", "xmlns", XMLNS_CHAT_STATES )
            || stanza->hasChild( "paused", "xmlns", XMLNS_CHAT_STATES )
            || stanza->hasChild( "inactive", "xmlns", XMLNS_CHAT_STATES )
            || stanza->hasChild( "gone", "xmlns", XMLNS_CHAT_STATES ) )
          m_enableChatStates = true;
        else
          m_enableChatStates = false;
      }
    }
    else
    {
      m_enableChatStates = false;
    }
  }

  void ChatStateFilter::setChatState( ChatStateType state )
  {
    if( !m_enableChatStates )
      return;

    Tag *m = new Tag( "message" );
    m->addAttribute( "to", m_parent->target().full() );

    Tag *s = 0;
    switch( state )
    {
      case CHAT_STATE_ACTIVE:
        s = new Tag( m, "active" );
        s->addAttribute( "xmlns", XMLNS_CHAT_STATES );
        break;
      case CHAT_STATE_COMPOSING:
        s = new Tag( m, "composing" );
        s->addAttribute( "xmlns", XMLNS_CHAT_STATES );
        break;
      case CHAT_STATE_PAUSED:
        s = new Tag( m, "paused" );
        s->addAttribute( "xmlns", XMLNS_CHAT_STATES );
        break;
      case CHAT_STATE_INACTIVE:
        s = new Tag( m, "inactive" );
        s->addAttribute( "xmlns", XMLNS_CHAT_STATES );
        break;
      case CHAT_STATE_GONE:
        s = new Tag( m, "gone" );
        s->addAttribute( "xmlns", XMLNS_CHAT_STATES );
        break;
    }

    m_lastSent = state;

    m_parent->send( m );
  }

  void ChatStateFilter::decorate( Tag *tag )
  {
    if( !m_enableChatStates )
      return;

    Tag *s = new Tag( tag, "active" );
    s->addAttribute( "xmlns", XMLNS_CHAT_STATES );
  }

  void ChatStateFilter::registerChatStateHandler( ChatStateHandler *csh )
  {
    m_chatStateHandler = csh;
  }

  void ChatStateFilter::removeChatStateHandler()
  {
    m_chatStateHandler = 0;
  }

}
