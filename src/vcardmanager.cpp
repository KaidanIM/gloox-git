/*
  Copyright (c) 2006-2008 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "vcardmanager.h"
#include "vcardhandler.h"
#include "vcard.h"
#include "clientbase.h"
#include "disco.h"
#include "error.h"

namespace gloox
{

  VCardManager::VCardManager( ClientBase* parent )
    : m_parent( parent )
  {
    if( m_parent )
    {
      m_parent->registerIqHandler( this, XMLNS_VCARD_TEMP );
      m_parent->disco()->addFeature( XMLNS_VCARD_TEMP );
    }
  }

  VCardManager::~VCardManager()
  {
    if( m_parent )
    {
      m_parent->disco()->removeFeature( XMLNS_VCARD_TEMP );
      m_parent->removeIqHandler( this, XMLNS_VCARD_TEMP );
      m_parent->removeIDHandler( this );
    }
  }

  void VCardManager::fetchVCard( const JID& jid, VCardHandler* vch )
  {
    if( !m_parent || !vch )
      return;

    TrackMap::const_iterator it = m_trackMap.find( jid.bare() );
    if( it != m_trackMap.end() )
      return;

    const std::string& id = m_parent->getID();
    IQ iq ( IQ::Get, jid.bareJID(), id, XMLNS_VCARD_TEMP, "vCard" );

    m_trackMap[id] = vch;
    m_parent->send( iq, this,VCardHandler::FetchVCard  );
  }

  void VCardManager::cancelVCardOperations( VCardHandler* vch )
  {
    TrackMap::iterator t;
    TrackMap::iterator it = m_trackMap.begin();
    while( it != m_trackMap.end() )
    {
      t = it;
      ++it;
      if( (*t).second == vch )
        m_trackMap.erase( t );
    }
  }

  void VCardManager::storeVCard( const VCard* vcard, VCardHandler* vch )
  {
    if( !m_parent || !vch )
      return;

    const std::string& id = m_parent->getID();
    IQ iq( IQ::Set, JID(), id );
    Tag* q = iq.query();
    delete q; // FIXME somewhat dirty...
    q = vcard->tag();

    m_trackMap[id] = vch;
    m_parent->send( iq, this, VCardHandler::StoreVCard );
  }

  void VCardManager::handleIqID( const IQ& iq, int context )
  {
    TrackMap::iterator it = m_trackMap.find( iq.id() );
    if( it != m_trackMap.end() )
    {
      switch( iq.subtype() )
      {
        case IQ::Result:
        {
          switch( context )
          {
            case VCardHandler::FetchVCard:
            {
              Tag* v = iq.query();
              if( v && v->name() == "vCard" && v->xmlns() == XMLNS_VCARD_TEMP )
                (*it).second->handleVCard( iq.from(), new VCard( v ) );
              else
                (*it).second->handleVCard( iq.from(), 0 );
              break;
            }
            case VCardHandler::StoreVCard:
              (*it).second->handleVCardResult( VCardHandler::StoreVCard, iq.from() );
              break;
          }
        }
        break;
        case IQ::Error:
        {
          (*it).second->handleVCardResult( (VCardHandler::VCardContext)context,
                                           iq.from(),
                                           iq.error() ? iq.error()->error()
                                                       : StanzaErrorUndefined );
          break;
        }
        default:
          break;
      }

      m_trackMap.erase( it );
    }
  }

}
