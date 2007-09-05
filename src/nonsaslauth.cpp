/*
  Copyright (c) 2005-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "nonsaslauth.h"
#include "client.h"
#include "error.h"
#include "sha.h"

#include <string>

namespace gloox
{

  NonSaslAuth::NonSaslAuth( Client* parent )
    : m_parent( parent )
  {
    if( m_parent )
      m_parent->registerIqHandler( this, XMLNS_AUTH );
  }

  NonSaslAuth::~NonSaslAuth()
  {
    if( m_parent )
      m_parent->removeIqHandler( this, XMLNS_AUTH );
  }

  void NonSaslAuth::doAuth( const std::string& sid )
  {
    m_sid = sid;
    const std::string& id = m_parent->getID();

    IQ* iq = new IQ( IQ::Get, m_parent->jid().server(), id, XMLNS_AUTH );
    new Tag( iq->query(), "username", m_parent->username() );

    m_parent->trackID( this, id, TRACK_REQUEST_AUTH_FIELDS );
    m_parent->send( iq );
  }

  void NonSaslAuth::handleIqID( IQ* iq, int context )
  {
    switch( iq->subtype() )
    {
      case IQ::Error:
      {
        m_parent->setAuthed( false );
        m_parent->disconnect( ConnAuthenticationFailed );

        const Error* e = iq->error();
        if( e )
        {
          switch( e->error() )
          {
            case StanzaErrorConflict:
              m_parent->setAuthFailure( NonSaslConflict );
              break;
            case StanzaErrorNotAcceptable:
              m_parent->setAuthFailure( NonSaslNotAcceptable );
              break;
            case StanzaErrorNotAuthorized:
              m_parent->setAuthFailure( NonSaslNotAuthorized );
              break;
            default:
              break;
          }
        }
        break;
      }
      case IQ::Result:
        switch( context )
        {
          case TRACK_REQUEST_AUTH_FIELDS:
          {
            const std::string& id = m_parent->getID();

            IQ* re = new IQ( IQ::Set, JID(), id, XMLNS_AUTH );
            Tag* query = re->query();
            new Tag( query, "username", m_parent->jid().username() );
            new Tag( query, "resource", m_parent->jid().resource() );

            Tag* q = iq->query();
            if( q && q->hasChild( "digest" ) && !m_sid.empty() )
            {
              SHA sha;
              sha.feed( m_sid );
              sha.feed( m_parent->password() );
              sha.finalize();
              new Tag( query, "digest", sha.hex() );
            }
            else
            {
              new Tag( query, "password", m_parent->password() );
            }

            m_parent->trackID( this, id, TRACK_SEND_AUTH );
            m_parent->send( re );
            break;
          }
          case TRACK_SEND_AUTH:
            m_parent->setAuthed( true );
            m_parent->connected();
            break;
        }
        break;

      default:
        break;
    }
  }

  bool NonSaslAuth::handleIq( IQ* /*iq*/ )
  {
    return false;
  }

}
