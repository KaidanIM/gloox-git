/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "simanager.h"

#include "siprofilehandler.h"
#include "sihandler.h"
#include "clientbase.h"
#include "disco.h"

namespace gloox
{

  SIManager::SIManager( ClientBase* parent, bool advertise )
    : m_parent( parent ), m_advertise( advertise )
  {
    if( m_parent && m_advertise )
    {
      m_parent->registerIqHandler( this, XMLNS_SI );
      if( m_parent->disco() )
        m_parent->disco()->addFeature( XMLNS_SI );
    }
  }

  SIManager::~SIManager()
  {
    if( m_parent && m_advertise )
    {
      m_parent->removeIqHandler( XMLNS_SI );
      if( m_parent->disco() )
        m_parent->disco()->removeFeature( XMLNS_SI );
    }
  }

  const std::string SIManager::requestSI( SIHandler* sih, const JID& to, const std::string& profile,
                                          Tag* child1, Tag* child2, const std::string& mimetype )
  {
    if( !m_parent || !sih )
      return std::string();

    const std::string& id = m_parent->getID();
    const std::string& id2 = m_parent->getID();

    IQ* iq = new IQ( IQ::Set, to, id, XMLNS_SI, "si" );
    Tag* si = iq->query();
    si->addAttribute( "id", id2 );
    si->addAttribute( "mime-type", mimetype.empty() ? "binary/octet-stream" : mimetype );
    si->addAttribute( "profile", profile );

    si->addChild( child1 );
    si->addChild( child2 );

    TrackStruct t;
    t.sid = id2;
    t.profile = profile;
    t.sih = sih;
    m_track[id] = t;
    m_parent->trackID( this, id, OfferSI );
    m_parent->send( iq );

    return id2;
  }

  void SIManager::acceptSI( const JID& to, const std::string& id, Tag* child1, Tag* child2 )
  {
    IQ* iq = new IQ( IQ::Result, to, id, XMLNS_SI, "si" );
    iq->query()->addChild( child1 );
    iq->query()->addChild( child2 );

    m_parent->send( iq );
  }

  void SIManager::declineSI( const JID& to, const std::string& id, SIError reason, const std::string& text )
  {
    IQ* iq = new IQ( IQ::Error, to, id );
    Tag* error = new Tag( iq, "error" );
    if( reason == NoValidStreams || reason == BadProfile )
    {
      error->addAttribute( "error", "400" );
      error->addAttribute( "type", "cancel" );
      new Tag( error, "bad-request", "xmlns", XMLNS_XMPP_STANZAS );
      if( reason == NoValidStreams )
        new Tag( error, "no-valid-streams", "xmlns", XMLNS_SI );
      else if( reason == BadProfile )
        new Tag( error, "bad-profile", "xmlns", XMLNS_SI );
    }
    else
    {
      error->addAttribute( "error", "403" );
      error->addAttribute( "type", "cancel" );
      new Tag( error, "forbidden", "xmlns", XMLNS_XMPP_STANZAS );
      if( !text.empty() )
      {
        Tag* t = new Tag( error, "text", "xmlns", XMLNS_XMPP_STANZAS );
        t->setCData( text );
      }
    }

    m_parent->send( iq );
  }

  void SIManager::registerProfile( const std::string& profile, SIProfileHandler* sih )
  {
    if( !sih || profile.empty() )
      return;

    m_handlers[profile] = sih;

    if( m_parent && m_advertise && m_parent->disco() )
      m_parent->disco()->addFeature( profile );
  }

  void SIManager::removeProfile( const std::string& profile )
  {
    if( profile.empty() )
      return;

    m_handlers.erase( profile );

    if( m_parent && m_advertise && m_parent->disco() )
      m_parent->disco()->removeFeature( profile );
  }

  bool SIManager::handleIq( IQ* iq )
  {
    TrackMap::iterator it = m_track.find( iq->id() );
    if( it != m_track.end() )
      return false;

    Tag *si = iq->findChild( "si", "xmlns", XMLNS_SI );
    if( si && si->hasAttribute( "profile" ) )
    {
      const std::string& profile = si->findAttribute( "profile" );
      HandlerMap::const_iterator it = m_handlers.find( profile );
      if( it != m_handlers.end() && (*it).second )
      {
        Tag* p = si->findChildWithAttrib( "xmlns", profile );
        Tag* f = si->findChild( "feature", "xmlns", XMLNS_FEATURE_NEG );
        (*it).second->handleSIRequest( iq->from(), iq->id(), profile, si, p, f );
        return true;
      }
    }

    return false;
  }

  void SIManager::handleIqID( IQ* iq, int context )
  {
    switch( iq->subtype() )
    {
      case IQ::Result:
        if( context == OfferSI )
        {
          TrackMap::iterator it = m_track.find( iq->id() );
          if( it != m_track.end() )
          {
            Tag* si = iq->findChild( "si", "xmlns", XMLNS_SI );
            Tag* ptag = 0;
            Tag* fneg = 0;
            if( si )
            {
              ptag = si->findChildWithAttrib( "xmlns", (*it).second.profile );
              fneg = si->findChild( "feature", "xmlns", XMLNS_FEATURE_NEG );
            }
            (*it).second.sih->handleSIRequestResult( iq->from(), (*it).second.sid, si, ptag, fneg );
          }
        }
        break;
      case IQ::Error:
        if( context == OfferSI )
        {
          TrackMap::iterator it = m_track.find( iq->id() );
          if( it != m_track.end() )
          {
            (*it).second.sih->handleSIRequestError( iq );
          }
        }
        break;
      case IQ::Set:
      case IQ::Get:
      default:
        break;
    }
  }

}
