/*
  Copyright (c) 2005-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#include "lastactivity.h"
#include "disco.h"
#include "discohandler.h"
#include "client.h"
#include "lastactivityhandler.h"

#include <cstdlib>

namespace gloox
{

  LastActivity::LastActivity( ClientBase *parent )
    : m_lastActivityHandler( 0 ), m_parent( parent ),
      m_active( time ( 0 ) )
  {
    if( m_parent )
      m_parent->disco()->addFeature( XMLNS_LAST );
  }

  LastActivity::~LastActivity()
  {
    if( m_parent )
      m_parent->disco()->removeFeature( XMLNS_LAST );
  }

  void LastActivity::query( const JID& jid )
  {
    const std::string& id = m_parent->getID();

    IQ* t = new IQ( IQ::Get, jid, id, XMLNS_LAST );

    m_parent->trackID( this, id, 0 );
    m_parent->send( t );
  }

  bool LastActivity::handleIq( IQ* iq )
  {
    switch( iq->subtype() )
    {
      case IQ::Get:
      {
        time_t now = time( 0 );

        IQ* t = new IQ( IQ::Result, iq->from(), iq->id(), XMLNS_LAST );
        t->query()->addAttribute( "seconds", (long)( now - m_active ) );

        m_parent->send( t );
        break;
      }

      case IQ::Set:
      {
        IQ* t = new IQ( IQ::Error, iq->from(), iq->id() );
        Tag *e = new Tag( t, "error" );
        e->addAttribute( TYPE, "cancel" );
        Tag *f = new Tag( e, "feature-not-implemented" );
        f->addAttribute( XMLNS, XMLNS_XMPP_STANZAS );

        m_parent->send( t );
        break;
      }

      default:
        break;
    }

    return true;
  }

  void LastActivity::handleIqID( IQ* iq, int /*context*/ )
  {
    if( !m_lastActivityHandler )
      return;

    switch( iq->subtype() )
    {
      case IQ::Result:
      {
        Tag *q = iq->query();
        if( q )
        {
          const std::string& seconds = q->findAttribute( "seconds" );
          if( !seconds.empty() )
          {
            int secs = atoi( seconds.c_str() );
            m_lastActivityHandler->handleLastActivityResult( iq->from(), secs );
          }
        }
        break;
      }
      case IQ::Error:
        m_lastActivityHandler->handleLastActivityError( iq->from(), iq->error() );
        break;
      default:
        break;
    }
  }

  void LastActivity::resetIdleTimer()
  {
    m_active = time( 0 );
  }

}
