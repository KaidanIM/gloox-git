/*
  Copyright (c) 2006 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#include "mucroom.h"
#include "clientbase.h"
#include "stanza.h"
#include "disco.h"
#include "mucroomlistener.h"

namespace gloox
{

  MUCRoom::MUCRoom( ClientBase *parent, const JID& nick, MUCRoomListener *mrl )
    : m_parent( parent ), m_nick( nick ), m_roomListener( mrl ), m_joined( false )
  {
#warning TODO: discover reserved room nickname: http://www.xmpp.org/extensions/xep-0045.html#reservednick
    // here or in join()
    if( m_parent )
      m_parent->registerPresenceHandler( this );
  }

  MUCRoom::~MUCRoom()
  {
    if( m_joined )
      leave();
  }

  void MUCRoom::join()
  {
    Stanza *s = Stanza::createPresenceStanza( m_nick );
    Tag *x = new Tag( s, "x" );
    x->addAttribute( "xmlns", XMLNS_MUC );
    if( m_parent )
      m_parent->send( s );

    m_joined = true;
  }

  void MUCRoom::leave()
  {
    Stanza *s = Stanza::createPresenceStanza( m_nick, "", PresenceUnavailable );
    Tag *x = new Tag( s, "x" );
    x->addAttribute( "xmlns", XMLNS_MUC );
    if( m_parent )
      m_parent->send( s );

    m_joined = false;
  }

  void MUCRoom::handlePresence( Stanza *stanza )
  {
    Tag *t;
    if( m_roomListener && ( t = stanza->findChild( "x", "xmlns", XMLNS_MUC_USER ) ) != 0 )
    {
      Presence presence = PresenceAvailable;
      MUCRoomParticipant party;
      party.self = false;
      party.nick = new JID( stanza->from() );
      Tag *i;
      if( ( i = t->findChild( "item" ) ) != 0 )
      {
        const std::string affiliation = i->findAttribute( "affiliation" );
        if( affiliation == "owner" )
          party.affiliation = AffiliationOwner;
        else if( affiliation == "admin" )
          party.affiliation = AffiliationAdmin;
        else if( affiliation == "member" )
          party.affiliation = AffiliationMember;
        else
          party.affiliation = AffiliationNone;

        const std::string role = i->findAttribute( "role" );
        if( role == "moderator" )
          party.role = RoleModerator;
        else if( role == "participant" )
          party.role = RoleParticipant;
        else
          party.role = RoleVisitor;
      }
      if( ( i = t->findChild( "status" ) ) != 0 )
      {
        const std::string code = i->findAttribute( "code" );
        if( code == "110" or code == "210" )
          party.self = true;
      }
      m_participants.push_back( party );

      if( m_roomListener )
        m_roomListener->handleParticipantPresence( party, presence );
    }
  }

  void MUCRoom::handleDiscoInfoResult( Stanza *stanza, int context )
  {
  }

  void MUCRoom::handleDiscoItemsResult( Stanza *stanza, int context )
  {
  }

  void MUCRoom::handleDiscoError( Stanza *stanza, int context )
  {
  }

}
