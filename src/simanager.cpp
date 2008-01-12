/*
  Copyright (c) 2007-2008 by Jakob Schroeter <js@camaya.net>
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
#include "error.h"

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
      m_parent->removeIqHandler( this, XMLNS_SI );
      m_parent->removeIDHandler( this );
      if( m_parent->disco() )
        m_parent->disco()->removeFeature( XMLNS_SI );
    }
  }

  const std::string SIManager::requestSI( SIHandler* sih, const JID& to, const std::string& profile,
                                          Tag* child1, Tag* child2, const std::string& mimetype )
  {
    if( !m_parent || !sih )
      return EmptyString;

    const std::string& id = m_parent->getID();
    const std::string& id2 = m_parent->getID();

    IQ iq( IQ::Set, to, id, XMLNS_SI, "si" );
    Tag* si = iq.query();
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
    m_parent->send( iq, this, OfferSI );

    return id2;
  }

  void SIManager::acceptSI( const JID& to, const std::string& id, Tag* child1, Tag* child2 )
  {
    IQ iq( IQ::Result, to, id, XMLNS_SI, "si" );
    iq.query()->addChild( child1 );
    iq.query()->addChild( child2 );

    m_parent->send( iq );
  }

  void SIManager::declineSI( const JID& to, const std::string& id, SIError reason, const std::string& text )
  {
    IQ iq( IQ::Error, to, id );
    Error* error;
    if( reason == NoValidStreams || reason == BadProfile )
    {
      Tag* appError = 0;
      if( reason == NoValidStreams )
        appError = new Tag( "no-valid-streams", XMLNS, XMLNS_SI );
      else if( reason == BadProfile )
        appError = new Tag( "bad-profile", XMLNS, XMLNS_SI );
      error = new Error( StanzaErrorTypeCancel, StanzaErrorBadRequest, appError );
    }
    else
    {
      error = new Error( StanzaErrorTypeCancel, StanzaErrorForbidden );
      if( !text.empty() )
        error->text( text );
    }

    iq.addExtension( error );
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

  bool SIManager::handleIq( const IQ& iq )
  {
    TrackMap::iterator it = m_track.find( iq.id() );
    if( it != m_track.end() )
      return false;

    Tag* si = iq.query();
    if( si && si->name() == "si" && si->xmlns() == XMLNS_SI && si->hasAttribute( "profile" ) )
    {
      const std::string& profile = si->findAttribute( "profile" );
      HandlerMap::const_iterator it = m_handlers.find( profile );
      if( it != m_handlers.end() && (*it).second )
      {
        Tag* p = si->findChildWithAttrib( XMLNS, profile );
        Tag* f = si->findChild( "feature", XMLNS, XMLNS_FEATURE_NEG );
        (*it).second->handleSIRequest( iq.from(), iq.id(), profile, si, p, f );
        return true;
      }
    }

    return false;
  }

  void SIManager::handleIqID( const IQ& iq, int context )
  {
    switch( iq.subtype() )
    {
      case IQ::Result:
        if( context == OfferSI )
        {
          TrackMap::iterator it = m_track.find( iq.id() );
          if( it != m_track.end() )
          {
            Tag* si = iq.query();
            Tag* ptag = 0;
            Tag* fneg = 0;
            if( si && si->name() == "si" && si->xmlns() == XMLNS_SI )
            {
              ptag = si->findChildWithAttrib( XMLNS, (*it).second.profile );
              fneg = si->findChild( "feature", XMLNS, XMLNS_FEATURE_NEG );
            }
            (*it).second.sih->handleSIRequestResult( iq.from(), (*it).second.sid, si, ptag, fneg );
            m_track.erase( it );
          }
        }
        break;
      case IQ::Error:
        if( context == OfferSI )
        {
          TrackMap::iterator it = m_track.find( iq.id() );
          if( it != m_track.end() )
          {
            (*it).second.sih->handleSIRequestError( iq, (*it).second.sid );
            m_track.erase( it );
          }
        }
        break;
      default:
        break;
    }
  }

}
