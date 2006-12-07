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
#include "dataform.h"
#include "stanza.h"
#include "disco.h"
#include "mucroomlistener.h"
#include "mucmessagesession.h"

namespace gloox
{

  MUCRoom::MUCRoom( ClientBase *parent, const JID& nick, MUCRoomListener *mrl )
    : m_parent( parent ), m_nick( nick ), m_roomListener( mrl ), m_affiliation( AffiliationNone ),
      m_role( RoleNone ), m_type( TypeUnknown ), m_features( 0 ), m_flags( 0 ),
      m_joined( false ), m_unique( false )
  {
#ifndef _MSC_VER
#warning TODO: discover reserved room nickname: http://www.xmpp.org/extensions/xep-0045.html#reservednick
#endif
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
#ifndef _MSC_VER
#warning TODO: add history
#endif
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

  void MUCRoom::getRoomInfo()
  {
    if( m_parent )
    {
      JID j( m_nick.bare() );
      m_parent->disco()->getDiscoInfo( j, "", this, GetRoomInfo );
    }
  }

  void MUCRoom::getRoomItems()
  {
    if( m_parent )
    {
      JID j( m_nick.bare() );
      m_parent->disco()->getDiscoItems( j, "", this, GetRoomItems );
    }
  }

  void MUCRoom::handlePresence( Stanza *stanza )
  {
    if( stanza->from().bare() != m_nick.bare() )
      return;

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
          else if( role == "visitor" )
            party.role = RoleVisitor;
          else
            party.role = RoleNone;
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

        m_roomListener->handleMUCParticipantPresence( this, party, stanza->show() );
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
#ifndef _MSC_VER
#warning FIXME: use local tracking, delete iterator here
#endif
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
    switch( context )
    {
      case GetRoomInfo:
      {
        int oldfeatures = m_features;
        m_features = 0;
        if( oldfeatures & FlagPublicLogging )
          m_features |= FlagPublicLogging;

        std::string name;
        DataForm *df = 0;
        Tag *q = stanza->findChild( "query" );
        if( q )
        {
          Tag::TagList l = q->children();
          Tag::TagList::const_iterator it = l.begin();
          for( ; it != l.end(); ++it )
          {
            if( (*it)->name() == "feature" )
            {
              if( (*it)->findAttribute( "var" ) == "muc_hidden" )
                m_features |= FlagHidden;
              else if( (*it)->findAttribute( "var" ) == "muc_membersonly" )
                m_features |= FlagMembersOnly;
              else if( (*it)->findAttribute( "var" ) == "muc_moderated" )
                m_features |= FlagModerated;
              else if( (*it)->findAttribute( "var" ) == "muc_nonanonymous" )
                m_features |= FlagNonAnonymous;
              else if( (*it)->findAttribute( "var" ) == "muc_open" )
                m_features |= FlagOpen;
              else if( (*it)->findAttribute( "var" ) == "muc_passwordprotected" )
                m_features |= FlagPasswordProtected;
              else if( (*it)->findAttribute( "var" ) == "muc_persistent" )
                m_features |= FlagPersistent;
              else if( (*it)->findAttribute( "var" ) == "muc_public" )
                m_features |= FlagPublic;
              else if( (*it)->findAttribute( "var" ) == "muc_semianonymous" )
                m_features |= FlagSemiAnonymous;
              else if( (*it)->findAttribute( "var" ) == "muc_temporary" )
                m_features |= FlagTemporary;
              else if( (*it)->findAttribute( "var" ) == "muc_unmoderated" )
                m_features |= FlagUnmoderated;
              else if( (*it)->findAttribute( "var" ) == "muc_unsecured" )
                m_features |= FlagUnsecured;
            }
            else if( (*it)->name() == "identity" )
            {
              name = (*it)->findAttribute( "name" );
            }
            else if( (*it)->name() == "x" && (*it)->hasAttribute( "xmlns", XMLNS_X_DATA ) )
            {
              df = new DataForm( (*it) );
            }
          }
        }
        if( m_roomListener )
          m_roomListener->handleMUCInfo( this, m_features, name, df );
        break;
      }
      default:
        break;
    }
  }

  void MUCRoom::handleDiscoItemsResult( Stanza *stanza, int context )
  {
    switch( context )
    {
      case GetRoomItems:
      {
        Tag *q = stanza->findChild( "query" );
        if( q )
        {
          StringMap items;
          Tag::TagList l = q->children();
          Tag::TagList::const_iterator it = l.begin();
          for( ; it != l.end(); ++it )
          {
            if( (*it)->name() == "item" && (*it)->hasAttribute( "jid" ) )
            {
              items[(*it)->findAttribute( "name" )] = (*it)->findAttribute( "jid" );
            }
          }
          if( m_roomListener )
            m_roomListener->handleMUCItems( this, items );
        }
        break;
      }
      default:
        break;
    }
  }

  void MUCRoom::handleDiscoError( Stanza *stanza, int context )
  {
    if( !m_roomListener )
      return;

    switch( context )
    {
      case GetRoomInfo:
        m_roomListener->handleMUCInfo( this, 0, "", 0 );
        break;
      case GetRoomItems:
      {
        StringMap items;
        m_roomListener->handleMUCItems( this, items );
        break;
      }
      default:
        break;
    }
  }

}
