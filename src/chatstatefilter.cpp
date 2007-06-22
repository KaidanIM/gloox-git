/*
  Copyright (c) 2005-2007 by Jakob Schroeter <js@camaya.net>
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
#include "message.h"

namespace gloox
{

  ChatStateFilter::ChatStateFilter( MessageSession *parent )
  : MessageFilter( parent ), m_chatStateHandler( 0 ), m_lastSent( ChatStateGone ),
      m_enableChatStates( true )
  {
  }

  ChatStateFilter::~ChatStateFilter()
  {
  }

  void ChatStateFilter::filter( Message* msg )
  {
    if( m_chatStateHandler )
    {
      if( msg->body().empty() )
      {
        m_enableChatStates = true;
        if( msg->hasChild( "active" ) )
          m_chatStateHandler->handleChatState( msg->from(), ChatStateActive );
        else if( msg->hasChild( "composing" ) )
          m_chatStateHandler->handleChatState( msg->from(), ChatStateComposing );
        else if( msg->hasChild( "paused" ) )
          m_chatStateHandler->handleChatState( msg->from(), ChatStatePaused );
        else if( msg->hasChild( "inactive" ) )
          m_chatStateHandler->handleChatState( msg->from(), ChatStateInactive );
        else if( msg->hasChild( "gone" ) )
          m_chatStateHandler->handleChatState( msg->from(), ChatStateGone );
        else
          m_enableChatStates = false;
      }
      else
      {
        if( msg->hasChild( "active", "xmlns", XMLNS_CHAT_STATES )
            || msg->hasChild( "composing", "xmlns", XMLNS_CHAT_STATES )
            || msg->hasChild( "paused", "xmlns", XMLNS_CHAT_STATES )
            || msg->hasChild( "inactive", "xmlns", XMLNS_CHAT_STATES )
            || msg->hasChild( "gone", "xmlns", XMLNS_CHAT_STATES ) )
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

    Message* m = new Message( Message::MessageChat, m_parent->target() );

    Tag *s = 0;
    switch( state )
    {
      case ChatStateActive:
        s = new Tag( m, "active" );
        s->addAttribute( "xmlns", XMLNS_CHAT_STATES );
        break;
      case ChatStateComposing:
        s = new Tag( m, "composing" );
        s->addAttribute( "xmlns", XMLNS_CHAT_STATES );
        break;
      case ChatStatePaused:
        s = new Tag( m, "paused" );
        s->addAttribute( "xmlns", XMLNS_CHAT_STATES );
        break;
      case ChatStateInactive:
        s = new Tag( m, "inactive" );
        s->addAttribute( "xmlns", XMLNS_CHAT_STATES );
        break;
      case ChatStateGone:
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
