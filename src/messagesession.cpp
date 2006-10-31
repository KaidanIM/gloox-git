/*
  Copyright (c) 2005-2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "messagesession.h"

#include "messagefilter.h"
#include "clientbase.h"
#include "client.h"
#include "disco.h"
#include "stanza.h"
#include "tag.h"

namespace gloox
{

  MessageSession::MessageSession( ClientBase *parent, const JID& jid, bool wantUpgrade )
    : m_parent( parent ), m_target( jid ), m_messageHandler( 0 ), m_wantUpgrade( wantUpgrade )
  {
    if( m_parent )
      m_parent->registerMessageHandler( m_target.full(), this, wantUpgrade );

    m_thread = "gloox" + m_parent->getID();
  }

  MessageSession::~MessageSession()
  {
    if( m_parent )
      m_parent->removeMessageHandler( m_target.full() );
  }

  void MessageSession::handleMessage( Stanza *stanza )
  {
    if( m_wantUpgrade && stanza->from().bare() == m_target.full() )
      setResource( stanza->from().resource() );

    MessageFilterList::const_iterator it = m_messageFilterList.begin();
    for( ; it != m_messageFilterList.end(); ++it )
    {
      (*it)->filter( stanza );
    }

    if( !m_messageHandler || stanza->body().empty() )
      return;
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

    MessageFilterList::const_iterator it = m_messageFilterList.begin();
    for( ; it != m_messageFilterList.end(); ++it )
    {
      (*it)->decorate( m );
    }

    m_parent->send( m );
  }

  void MessageSession::send( Tag *tag )
  {
    m_parent->send( tag );
  }

  void MessageSession::setResource( const std::string& resource )
  {
    m_parent->removeMessageHandler( m_target.full() );
    m_target.setResource( resource );
    m_parent->registerMessageHandler( m_target.full(), this, false );
  }

  void MessageSession::registerMessageHandler( MessageHandler *mh )
  {
    m_messageHandler = mh;
  }

  void MessageSession::removeMessageHandler()
  {
    m_messageHandler = 0;
  }

  void MessageSession::registerMessageFilter( MessageFilter *mf )
  {
    m_messageFilterList.push_back( mf );
  }

  void MessageSession::removeMessageFilter( MessageFilter *mf )
  {
    m_messageFilterList.remove( mf );
  }

}
