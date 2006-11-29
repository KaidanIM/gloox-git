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
#include "mucmessagesession.h"

namespace gloox
{

  MUCRoom::MUCRoom( ClientBase *parent, const JID& nick, MUCRoomListener *mrl )
    : m_parent( parent ), m_nick( nick ), m_roomListener( mrl ), m_affiliation( AffiliationNone ),
      m_role( RoleNone ), m_type( TypeUnknown ), m_flags( 0 ), m_joined( false ), m_unique( false )
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
    if( m_joined )
      return;

    JID j( m_nick.bare() );
    m_session = new MUCMessageSession( m_parent, j );
    m_session->registerMessageHandler( this );

    Stanza *s = Stanza::createPresenceStanza( m_nick );
    Tag *x = new Tag( s, "x" );
    x->addAttribute( "xmlns", XMLNS_MUC );
    if( !m_password.empty() )
      new Tag( x, "password",  m_password );
#warning TODO: add history
    if( m_parent )
      m_parent->send( s );

    m_joined = true;
  }

  void MUCRoom::leave( const std::string& msg )
  {
    if( !m_joined )
      return;

    Stanza *s = Stanza::createPresenceStanza( m_nick, msg, PresenceUnavailable );
    Tag *x = new Tag( s, "x" );
    x->addAttribute( "xmlns", XMLNS_MUC );

    if( m_parent )
      m_parent->send( s );

    m_joined = false;
  }

  void MUCRoom::send( const std::string& message )
  {
    if( m_session && m_joined )
      m_session->send( message );
  }

  void MUCRoom::setSubject( const std::string& subject )
  {
    if( m_session && m_joined )
      m_session->setSubject( subject );
  }

//   void MUCRoom::setUnique( bool unique )
//   {
//     if( !m_parent )
//       return;
//
//     const std::string id = m_parent->getID();
//     Tag *iq = new Tag( "iq" );
//     iq->addAttribute( "id", id );
//     iq->addAttribute( "to", m_nick.server() );
//     iq->addAttribute( "type", "get" );
//     Tag *u = new Tag( iq, "unique" );
//     u->addAttribute( "xmlns", XMLNS_MUC_UNIQUE );
//
//     m_parent->trackID( this, id, RequestUniqueName );
//     m_parent->send( iq );
//   }

  void MUCRoom::handlePresence( Stanza *stanza )
  {
    if( !m_roomListener )
      return;

    if( stanza->subtype() == StanzaPresenceError )
    {
       m_joined = false;
       m_roomListener->handleMUCError( this, stanza->error() );
    }
    else
    {
      Tag *t;
      if( m_roomListener && ( t = stanza->findChild( "x", "xmlns", XMLNS_MUC_USER ) ) != 0 )
      {
        Presence presence = stanza->show();
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

        Tag::TagList l = stanza->children();
        Tag::TagList::const_iterator it = l.begin();
        for( ; it != l.end(); ++it )
        {
          if( (*it)->name() == "status" )
          {
            const std::string code = i->findAttribute( "code" );
            if( code == "100" )
            {
              party.self = true;
              m_role = party.role;
              m_affiliation = party.affiliation;
            }
            else if( code == "101" )
            {
              // affiliation changed while not in the room. not to be handled here, I guess
            }
            else if( code == "110" )
            {
              m_type = TypeNonAnonymous;
//               m_roomListener->handleRoomConfig( m_type );
            }
            else if( code == "210" )
            {
              m_nick.setResource( stanza->from().resource() );
            }
          }
        }

        m_participants.push_back( party );

        m_roomListener->handleMUCParticipantPresence( this, party, presence );
      }
    }
  }

  void MUCRoom::handleMessage( Stanza *stanza )
  {
    if( !m_roomListener )
      return;

    Tag *x;
    if( ( x = stanza->findChild( "x", "xmlns", XMLNS_MUC_USER ) ) != 0 )
    {
      Tag::TagList l = x->children();
      Tag::TagList::const_iterator it = l.begin();
      for( ; it != l.end(); ++it )
      {
        if( (*it)->name() == "status" )
        {
          const std::string code = (*it)->findAttribute( "code" );
          if( code == "104" )
            /*m_configChanged =*/ (void)true;
          if( code == "170" )
            m_flags |= FlagPublicLogging;
          else if( code == "171" )
            m_flags ^= FlagPublicLogging;
          else if( code == "172" )
            m_type = TypeNonAnonymous;
          else if( code == "173" )
            m_type = TypeSemiAnonymous;
          else if( code == "174" )
            m_type = TypeFullyAnonymous;
        }
        // call some handler?
      }
    }

    if( !stanza->subject().empty() )
    {
      m_roomListener->handleMUCSubject( this, stanza->from().resource(), stanza->subject() );
    }
    else if( !stanza->body().empty() )
    {
      std::string from;
      std::string when;
      bool history = false;
      if( ( x = stanza->findChild( "x", "xmlns", XMLNS_X_DELAY ) ) != 0 )
      {
        JID j( x->findAttribute( "from" ) );
        from = j.resource();
        when = x->findAttribute( "when" );
        history = true;
      }
      else
      {
        from = stanza->from().resource();
      }
      m_roomListener->handleMUCMessage( this, from, stanza->body(), history, when );
    }
  }

  bool MUCRoom::handleIqID( Stanza *stanza, int context )
  {
    switch( stanza->subtype() )
    {
      case StanzaIqGet:
        return handleIqGet( stanza, context );
        break;
      case StanzaIqSet:
        return handleIqSet( stanza, context );
        break;
      case StanzaIqResult:
        return handleIqResult( stanza, context );
        break;
      case StanzaIqError:
        return handleIqError( stanza, context );
        break;
      default:
        break;
    }
    return false;
  }

  bool MUCRoom::handleIqGet( Stanza * /*stanza*/, int /*context*/ )
  {
    return false;
  }

  bool MUCRoom::handleIqSet( Stanza * /*stanza*/, int /*context*/ )
  {
    return false;
  }

  bool MUCRoom::handleIqResult( Stanza *stanza, int context )
  {
    switch( context )
    {
      case RequestUniqueName:
        break;
    }
    return false;
  }

  bool MUCRoom::handleIqError( Stanza *stanza, int context )
  {
    switch( context )
    {
      case RequestUniqueName:
        break;
    }
    return false;
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
