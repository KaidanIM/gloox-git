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

  ChatStateFilter::ChatStateFilter( MessageSession* parent )
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
        if( msg->hasChild( "active", XMLNS, XMLNS_CHAT_STATES )
            || msg->hasChild( "composing", XMLNS, XMLNS_CHAT_STATES )
            || msg->hasChild( "paused", XMLNS, XMLNS_CHAT_STATES )
            || msg->hasChild( "inactive", XMLNS, XMLNS_CHAT_STATES )
            || msg->hasChild( "gone", XMLNS, XMLNS_CHAT_STATES ) )
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
    if( !m_enableChatStates || state == m_lastSent )
      return;

    Message* m = new Message( Message::Chat, m_parent->target() );

    switch( state )
    {
      case ChatStateActive:
        new Tag( m, "active", XMLNS, XMLNS_CHAT_STATES );
        break;
      case ChatStateComposing:
        new Tag( m, "composing", XMLNS, XMLNS_CHAT_STATES );
        break;
      case ChatStatePaused:
        new Tag( m, "paused", XMLNS, XMLNS_CHAT_STATES );
        break;
      case ChatStateInactive:
        new Tag( m, "inactive", XMLNS, XMLNS_CHAT_STATES );
        break;
      case ChatStateGone:
        new Tag( m, "gone", XMLNS, XMLNS_CHAT_STATES );
        break;
    }

    m_lastSent = state;

    send( m );
  }

  void ChatStateFilter::decorate( Tag* tag )
  {
    if( m_enableChatStates )
      new Tag( tag, "active", XMLNS, XMLNS_CHAT_STATES );
  }

}
