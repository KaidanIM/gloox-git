/*
  Copyright (c) 2005-2008 by Jakob Schroeter <js@camaya.net>
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
#include "clientbase.h"
#include "error.h"
#include "lastactivityhandler.h"

#include <cstdlib>

namespace gloox
{

  LastActivity::LastActivity( ClientBase* parent )
    : m_lastActivityHandler( 0 ), m_parent( parent ),
      m_active( time ( 0 ) )
  {
    if( m_parent )
      m_parent->disco()->addFeature( XMLNS_LAST );
  }

  LastActivity::~LastActivity()
  {
    if( m_parent )
    {
      m_parent->disco()->removeFeature( XMLNS_LAST );
      m_parent->removeIDHandler( this );
    }
  }

  void LastActivity::query( const JID& jid )
  {
    IQ iq( IQ::Get, jid, m_parent->getID(), XMLNS_LAST );
    m_parent->send( iq, this, 0 );
  }

  bool LastActivity::handleIq( const IQ& iq )
  {
    switch( iq.subtype() )
    {
      case IQ::Get:
      {
        time_t now = time( 0 );

        IQ t( IQ::Result, iq.from(), iq.id(), XMLNS_LAST );
        t.query()->addAttribute( "seconds", (long)( now - m_active ) );

        m_parent->send( t );
        break;
      }

      case IQ::Set:
      {
        IQ t( IQ::Error, iq.from(), iq.id() );
        t.addExtension( new Error( StanzaErrorTypeCancel, StanzaErrorFeatureNotImplemented ) );
        m_parent->send( t );
        break;
      }

      default:
        break;
    }

    return true;
  }

  void LastActivity::handleIqID( const IQ& iq, int /*context*/ )
  {
    if( !m_lastActivityHandler )
      return;

    switch( iq.subtype() )
    {
      case IQ::Result:
      {
        Tag* q = iq.query();
        if( q )
        {
          const std::string& seconds = q->findAttribute( "seconds" );
          if( !seconds.empty() )
          {
            int secs = atoi( seconds.c_str() );
            m_lastActivityHandler->handleLastActivityResult( iq.from(), secs );
          }
        }
        break;
      }
      case IQ::Error:
        m_lastActivityHandler->handleLastActivityError( iq.from(), iq.error()->error() );
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
