/*
  Copyright (c) 2005-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "messagesession.h"

#include "messagefilter.h"
#include "messagehandler.h"
#include "clientbase.h"
#include "disco.h"
#include "message.h"
#include "util.h"

namespace gloox
{

  MessageSession::MessageSession( ClientBase *parent, const JID& jid, bool wantUpgrade, int types )
    : m_parent( parent ), m_target( jid ), m_messageHandler( 0 ),
      m_types( types ), m_wantUpgrade( wantUpgrade ), m_hadMessages( false )
  {
    if( m_parent )
      m_parent->registerMessageSession( this );
  }

  MessageSession::~MessageSession()
  {
    util::clear( m_messageFilterList );
  }

  void MessageSession::handleMessage( Message* msg )
  {
    if( m_wantUpgrade && msg->from().bare() == m_target.full() )
      setResource( msg->from().resource() );

    if( !m_hadMessages )
    {
      m_hadMessages = true;
      if( msg->thread().empty() )
      {
        m_thread = "gloox" + m_parent->getID();
        msg->setThread( m_thread );
      }
      else
        m_thread = msg->thread();
    }

    MessageFilterList::const_iterator it = m_messageFilterList.begin();
    for( ; it != m_messageFilterList.end(); ++it )
    {
      (*it)->filter( msg );
    }

    if( m_messageHandler && !msg->body().empty() )
      m_messageHandler->handleMessage( msg, this );
  }

  void MessageSession::send( const std::string& message, const std::string& subject )
  {
    if( !m_hadMessages )
    {
      m_thread = "gloox" + m_parent->getID();
      m_hadMessages = true;
    }

    Message* m = new Message( Message::Chat, m_target.full(), message, subject, m_thread );
    m->addAttribute( "id", m_parent->getID() );

    decorate( m );

    m_parent->send( m );
  }

  void MessageSession::send( Tag *tag )
  {
    m_parent->send( tag );
  }

  void MessageSession::decorate( Tag *tag )
  {
    MessageFilterList::const_iterator it = m_messageFilterList.begin();
    for( ; it != m_messageFilterList.end(); ++it )
    {
      (*it)->decorate( tag );
    }
  }

  void MessageSession::resetResource()
  {
    m_wantUpgrade = true;
    m_target.setResource( "" );
  }

  void MessageSession::setResource( const std::string& resource )
  {
    m_target.setResource( resource );
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

  void MessageSession::disposeMessageFilter( MessageFilter *mf )
  {
    removeMessageFilter( mf );
    delete mf;
  }

}
