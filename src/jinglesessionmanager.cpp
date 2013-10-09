/*
  Copyright (c) 2013 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "jinglesessionmanager.h"

#include "clientbase.h"
#include "jinglesession.h"
#include "jinglesessionhandler.h"
#include "disco.h"
#include "util.h"

namespace gloox
{

  namespace Jingle
  {

    SessionManager::SessionManager( ClientBase* parent, SessionHandler* sh )
      : m_parent( parent ), m_handler( sh )
    {
      if( m_parent )
      {
        m_parent->registerStanzaExtension( new Session::Jingle() );
        m_parent->registerIqHandler( this, ExtJingle );
        m_parent->disco()->addFeature( XMLNS_JINGLE );
      }
    }

    SessionManager::~SessionManager()
    {
      util::clearList( m_sessions );
    }

    Session* SessionManager::createSession( const JID& callee, SessionHandler* handler )
    {
      if( !( handler || m_handler ) || !callee )
        return 0;

      Session* sess = new Session( m_parent, callee, handler ? handler : m_handler );
      m_sessions.push_back( sess );
      return sess;
    }

    bool SessionManager::handleIq( const IQ& iq )
    {
      const Session::Jingle* j = iq.findExtension<Session::Jingle>( ExtJingle );
      if( !j )
        return false;

      SessionList::iterator it = m_sessions.begin();
      for( ; it != m_sessions.end() && (*it)->sid() != j->sid(); ++it ) ;
      if( it == m_sessions.end() )
      {
        Session* s = new Session( m_parent, j, m_handler );
        m_handler->handleIncomingSession( s );
        s->handleIq( iq );
      }
      else
      {
        (*it)->handleIq( iq );
      }
      return true;
    }

  }

}
