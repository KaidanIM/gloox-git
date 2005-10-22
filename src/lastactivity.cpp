/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/



#include "lastactivity.h"
#include "disco.h"
#include "discohandler.h"
#include "client.h"
#include "lastactivityhandler.h"

#include <math.h>

namespace gloox
{

  LastActivity::LastActivity( ClientBase *parent, Disco *disco )
    : m_lastActivityHandler( 0 ), m_parent( parent ), m_disco( disco )
  {
    m_disco->addFeature( XMLNS_LAST );
    m_active = time( 0 );
  }

  LastActivity::~LastActivity()
  {
  }

  void LastActivity::query( const JID& jid )
  {
    const std::string id = m_parent->getID();

    Tag *t = new Tag( "iq" );
    t->addAttrib( "type", "get" );
    t->addAttrib( "id", id );
    t->addAttrib( "to", jid.full() );
    Tag *q = new Tag( t, "query" );
    q->addAttrib( "xmlns", XMLNS_LAST );

    m_parent->trackID( this, id, 0 );
    m_parent->send( t );
  }

  bool LastActivity::handleIq( Stanza *stanza )
  {
    switch( stanza->subtype() )
    {
      case STANZA_IQ_GET:
      {
        time_t now = time( 0 );
        char *tmp = new char[4+(int)log10( (int)(now - m_active) )+1];
        sprintf( tmp, "%d", (int)m_active );
        delete[] tmp;

        Tag *t = new Tag( "iq" );
        t->addAttrib( "type", "result" );
        t->addAttrib( "id", stanza->id() );
        t->addAttrib( "to", stanza->from().full() );
        Tag *q = new Tag( t, "query" );
        q->addAttrib( "seconds", tmp );
        q->addAttrib( "xmlns", XMLNS_LAST );

        m_parent->send( t);
        break;
      }

      case STANZA_IQ_SET:
      {
        Tag *t = new Tag( "iq" );
        t->addAttrib( "id", stanza->id() );
        t->addAttrib( "to", stanza->from().full() );
        t->addAttrib( "type", "error" );
        Tag *e = new Tag( t, "error" );
        e->addAttrib( "type", "cancel" );
        Tag *f = new Tag( e, "feature-not-implemented" );
        f->addAttrib( "xmlns", XMLNS_XMPP_STANZAS );
        break;
      }

      default:
        break;
    }

    return true;
  }

  bool LastActivity::handleIqID( Stanza *stanza, int /*context*/ )
  {
    if( !m_lastActivityHandler )
      return false;

    int secs = 0;
    const std::string seconds = stanza->findChild( "query" )->findAttribute( "seconds" );
    if( !seconds.empty() )
      secs = atoi( seconds.c_str() );

    switch( stanza->subtype() )
    {
      case STANZA_IQ_RESULT:
        m_lastActivityHandler->handleLastActivityResult( stanza->from(), secs );
        break;
      case STANZA_IQ_ERROR:
        m_lastActivityHandler->handleLastActivityError( stanza->from(), stanza->error() );
        break;
      default:
        break;
    }

    return false;
  }

  void LastActivity::resetIdleTimer()
  {
    m_active = time( 0 );
  }

}
