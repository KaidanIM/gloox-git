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
      m_role( RoleNone ), m_historyType( HistoryUnknown ), m_historyValue( 0 ), m_flags( 0 ),
      m_configChanged( false ), m_publishNick( false ), m_publish( false ), m_joined( false ),
      m_unique( false )
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

    if( m_parent )
    {
      m_parent->removePresenceHandler( this );
      m_parent->disco()->removeNodeHandler( this, XMLNS_MUC_ROOMS );
    }
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
    if( m_historyType != HistoryUnknown )
    {
      switch( m_historyType )
      {
        case HistoryMaxChars:
        {
          Tag *h = new Tag( x, "history" );
          h->addAttribute( "maxchars", m_historyValue );
          break;
        }
        case HistoryMaxStanzas:
        {
          Tag *h = new Tag( x, "history" );
          h->addAttribute( "maxstanzas", m_historyValue );
          break;
        }
        case HistorySeconds:
        {
          Tag *h = new Tag( x, "history" );
          h->addAttribute( "seconds", m_historyValue );
          break;
        }
        case HistorySince:
        {
          Tag *h = new Tag( x, "history" );
          h->addAttribute( "since", m_historySince );
          break;
        }
        default:
          break;
      }
    }

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

  void MUCRoom::setNick( const std::string& nick )
  {
    if( m_parent )
    {
      m_newNick = nick;

      Tag *p = new Tag( "presence" );
      p->addAttribute( "to", m_nick.bare() + "/" + m_newNick );
      m_parent->send( p );
    }
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

  void MUCRoom::setPresence( Presence presence, const std::string& msg )
  {
    if( m_parent && presence != PresenceUnavailable && m_joined )
    {
      Stanza *p = Stanza::createPresenceStanza( m_nick, msg, presence );
      m_parent->send( p );
    }
  }

  void MUCRoom::invite( const JID& invitee, const std::string& reason, bool cont )
  {
    if( !m_parent )
      return;

    Tag *m = new Tag( "message" );
    m->addAttribute( "to", m_nick.bare() );
    Tag *x = new Tag( m, "x" );
    x->addAttribute( "xmlns", XMLNS_MUC_USER );
    Tag *i = new Tag( x, "invite" );
    i->addAttribute( "to", invitee.bare() );
    if( !reason.empty() )
      new Tag( i, "reason", reason );
    if( cont )
      new Tag( i, "continue" );

    m_parent->send( m );
  }

  Stanza* MUCRoom::declineInvitation( const JID& room, const JID& invitee, const std::string& reason )
  {
    Stanza *m = new Stanza( "message" );
    m->addAttribute( "to", room.bare() );
    Tag *x = new Tag( m, "x" );
    x->addAttribute( "xmlns", XMLNS_MUC_USER );
    Tag *d = new Tag( x, "decline" );
    d->addAttribute( "to", invitee.bare() );
    if( !reason.empty() )
      new Tag( d, "reason", reason );

    return m;
  }

  void MUCRoom::setPublish( bool publish, bool publishNick )
  {
    m_publish = publish;
    m_publishNick = publishNick;

    if( !m_parent )
      return;

    if( m_publish )
      m_parent->disco()->registerNodeHandler( this, XMLNS_MUC_ROOMS );
    else
      m_parent->disco()->removeNodeHandler( this, XMLNS_MUC_ROOMS );
  }

  void MUCRoom::addHistory( const std::string& message, const JID& from, const std::string& stamp )
  {
    if( !m_joined || !m_parent )
      return;

    Tag *m = new Tag( "message" );
    m->addAttribute( "to", m_nick.bare() );
    new Tag( m, "body", message );
    Tag *x = new Tag( m, "x" );
    x->addAttribute( "xmlns", XMLNS_X_DELAY );
    x->addAttribute( "from", from.full() );
    x->addAttribute( "stamp", stamp );

    m_parent->send( m );
  }

  void MUCRoom::setRequestHistory( int value, MUCRoom::HistoryRequestType type )
  {
    m_historyType = type;
    m_historySince = "";
    m_historyValue = value;
  }

  void MUCRoom::setRequestHistory( const std::string& since )
  {
    m_historyType = HistorySince;
    m_historySince = since;
    m_historyValue = 0;
  }

  void MUCRoom::requestVoice()
  {
    if( !m_parent )
      return;

    DataForm df;
    DataFormField *field = new DataFormField( DataFormField::FieldTypeNone );
    field->setName( "FORM_TYPE" );
    field->setValue( XMLNS_MUC_REQUEST );
    df.addField( field );
    field = new DataFormField( DataFormField::FieldTypeTextSingle );
    field->setName( "muc#role" );
    field->setLabel( "Requested role" );
    field->setValue( "participant" );
    df.addField( field );

    Tag *m = new Tag( "messsage" );
    m->addAttribute( "to", m_nick.bare() );
    m->addChild( df.tag() );

    m_parent->send( m );
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
        party.jid = 0;
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

          std::string jid = i->findAttribute( "jid" );
          if( !jid.empty() )
            party.jid = new JID( jid );
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
              setNonAnonymous();
            }
            else if( code == "101" )
            {
              // affiliation changed while not in the room. not to be handled here, I guess
            }
            else if( code == "110" )
            {
              party.self = true;
              m_role = party.role;
              m_affiliation = party.affiliation;
            }
            else if( code == "210" )
            {
              m_nick.setResource( stanza->from().resource() );
            }
            else if( code == "303" )
            {
              if( stanza->from().resource() == m_nick.resource()
                  && stanza->show() == PresenceUnavailable
                  && !m_newNick.empty() )
              {
#warning TODO: implement nick change
                // myself requested nick change
              }
              else if( stanza->show() == PresenceUnavailable )
              {
                // somebody else changed nicks
              }
            }
          }
        }

//         m_participants.push_back( party );

        m_roomListener->handleMUCParticipantPresence( this, party, stanza->show() );
        delete party.jid;
        delete party.nick;
      }
    }
  }

  void MUCRoom::setNonAnonymous()
  {
    m_flags ^= FlagSemiAnonymous;
    m_flags ^= FlagFullyAnonymous;
    m_flags |= FlagNonAnonymous;
  }

  void MUCRoom::setSemiAnonymous()
  {
    m_flags ^= FlagNonAnonymous;
    m_flags ^= FlagFullyAnonymous;
    m_flags |= FlagSemiAnonymous;
  }

  void MUCRoom::setFullyAnonymous()
  {
    m_flags ^= FlagNonAnonymous;
    m_flags ^= FlagSemiAnonymous;
    m_flags |= FlagFullyAnonymous;
  }

  void MUCRoom::handleMessage( Stanza *stanza )
  {
    if( !m_roomListener )
      return;

    if( stanza->subtype() == StanzaMessageError )
    {
    }
    else
    {
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
            if( code == "100" )
            {
              setNonAnonymous();
            }
            else if( code == "104" )
              /*m_configChanged =*/ (void)true;
            else if( code == "170" )
              m_flags |= FlagPublicLogging;
            else if( code == "171" )
              m_flags ^= FlagPublicLogging;
            else if( code == "172" )
            {
              setNonAnonymous();
            }
            else if( code == "173" )
            {
              setSemiAnonymous();
            }
            else if( code == "174" )
            {
              setFullyAnonymous();
            }
          }
          else if( (*it)->name() == "decline" )
          {
            std::string reason;
            JID invitee( (*it)->findAttribute( "from" ) );
            if( (*it)->hasChild( "reason" ) )
              reason = (*it)->findChild( "reason" )->cdata();
            m_roomListener->handleMUCInviteDecline( this, invitee, reason );
            return;
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
        m_roomListener->handleMUCMessage( this, from, stanza->body(), history, when, false );
      }
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
        int oldflags = m_flags;
        m_flags = 0;
        if( oldflags & FlagPublicLogging )
          m_flags |= FlagPublicLogging;

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
                m_flags |= FlagHidden;
              else if( (*it)->findAttribute( "var" ) == "muc_membersonly" )
                m_flags |= FlagMembersOnly;
              else if( (*it)->findAttribute( "var" ) == "muc_moderated" )
                m_flags |= FlagModerated;
              else if( (*it)->findAttribute( "var" ) == "muc_nonanonymous" )
                setNonAnonymous();
              else if( (*it)->findAttribute( "var" ) == "muc_open" )
                m_flags |= FlagOpen;
              else if( (*it)->findAttribute( "var" ) == "muc_passwordprotected" )
                m_flags |= FlagPasswordProtected;
              else if( (*it)->findAttribute( "var" ) == "muc_persistent" )
                m_flags |= FlagPersistent;
              else if( (*it)->findAttribute( "var" ) == "muc_public" )
                m_flags |= FlagPublic;
              else if( (*it)->findAttribute( "var" ) == "muc_semianonymous" )
                setSemiAnonymous();
              else if( (*it)->findAttribute( "var" ) == "muc_temporary" )
                m_flags |= FlagTemporary;
              else if( (*it)->findAttribute( "var" ) == "muc_fullyanonymous" )
                setFullyAnonymous();
              else if( (*it)->findAttribute( "var" ) == "muc_unmoderated" )
                m_flags |= FlagUnmoderated;
              else if( (*it)->findAttribute( "var" ) == "muc_unsecured" )
                m_flags |= FlagUnsecured;
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
          m_roomListener->handleMUCInfo( this, m_flags, name, df );
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

  StringList MUCRoom::handleDiscoNodeFeatures( const std::string& node )
  {
    return StringList();
  }

  StringMap MUCRoom::handleDiscoNodeIdentities( const std::string& node, std::string& name )
  {
    return StringMap();
  }

  StringMap MUCRoom::handleDiscoNodeItems( const std::string& node )
  {
    StringMap m;
    if( m_publish )
    {
      m[m_nick.bare()] = m_publishNick ? m_nick.username() : "";
    }
    return m;
  }

}
