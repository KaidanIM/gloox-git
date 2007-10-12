/*
  Copyright (c) 2006-2007 by Jakob Schroeter <js@camaya.net>
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
#include "presence.h"
#include "disco.h"
#include "mucmessagesession.h"
#include "message.h"
#include "error.h"
#include "util.h"

namespace gloox
{

  MUCRoom::MUCRoom( ClientBase* parent, const JID& nick, MUCRoomHandler* mrh,
                    MUCRoomConfigHandler* mrch )
    : m_parent( parent ), m_nick( nick ), m_joined( false ), m_roomHandler( mrh ),
      m_roomConfigHandler( mrch ), m_affiliation( AffiliationNone ), m_role( RoleNone ),
      m_historyType( HistoryUnknown ), m_historyValue( 0 ), m_flags( 0 ),
      m_creationInProgress( false ), m_configChanged( false ),
      m_publishNick( false ), m_publish( false ), m_unique( false )
  {
  }

  MUCRoom::~MUCRoom()
  {
    if( m_joined )
      leave();

    if( m_parent && m_publish )
      m_parent->disco()->removeNodeHandler( this, XMLNS_MUC_ROOMS );
  }

  const char * historyTypeValues[] =
  {
    "maxchars", "maxstanzas", "seconds", "since"
  };

  void MUCRoom::join()
  {
    if( m_joined || !m_parent )
      return;

    m_parent->registerPresenceHandler( m_nick.bareJID(), this );

    m_session = new MUCMessageSession( m_parent, m_nick.bareJID() );
    m_session->registerMessageHandler( this );

    Presence* s = new Presence( Presence::Available, m_nick.full() );
    Tag* x = new Tag( s, "x", XMLNS, XMLNS_MUC );
    if( !m_password.empty() )
      new Tag( x, "password",  m_password );

    if( m_historyType != HistoryUnknown )
    {
      const std::string& histStr = util::lookup( m_historyType, historyTypeValues );
      Tag* h = new Tag( x, "history" );
      if( m_historyType == HistorySince )
        h->addAttribute( histStr, m_historySince );
      else
        h->addAttribute( histStr, m_historyValue );
    }

    if( m_parent )
      m_parent->send( s );

    m_joined = true;
  }

  void MUCRoom::leave( const std::string& msg )
  {
    if( !m_joined )
      return;

    if( m_parent )
    {
      Presence* s = new Presence( Presence::Unavailable, m_nick.full(), msg );
      new Tag( s, "x", XMLNS, XMLNS_MUC );
      m_parent->send( s );
    }

    m_parent->removePresenceHandler( m_nick.bareJID(), this );
    m_parent->disposeMessageSession( m_session );
    m_session = 0;
    m_joined = false;
  }

  void MUCRoom::destroy( const std::string& reason, const JID* alternate, const std::string& password )
  {
    if( !m_parent || !m_joined )
      return;

    const std::string& id = m_parent->getID();
    IQ* iq = new IQ( IQ::Set, m_nick.bareJID(), id, XMLNS_MUC_OWNER );
    Tag* d = new Tag( iq->query(), "destroy" );
    if( alternate )
      d->addAttribute( "jid", alternate->bare() );

    if( !reason.empty() )
      new Tag( d, "reason", reason );

    if( !password.empty() )
      new Tag( d, "password", password );

    m_parent->trackID( this, id, DestroyRoom );
    m_parent->send( iq );
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
    if( m_parent && m_joined )
    {
      m_newNick = nick;

      Presence* p = new Presence( Presence::Available, m_nick.bare() + "/" + m_newNick );
      m_parent->send( p );
    }
    else
      m_nick.setResource( nick );
  }

  void MUCRoom::getRoomInfo()
  {
    if( m_parent )
      m_parent->disco()->getDiscoInfo( m_nick.bare(), "", this, GetRoomInfo );
  }

  void MUCRoom::getRoomItems()
  {
    if( m_parent )
      m_parent->disco()->getDiscoItems( m_nick.bare(), "", this, GetRoomItems );
  }

  void MUCRoom::setPresence( Presence::PresenceType presence, const std::string& msg )
  {
    if( m_parent && presence != Presence::Unavailable && m_joined )
    {
      Presence* p = new Presence( presence, m_nick.full(), msg );
      m_parent->send( p );
    }
  }

  void MUCRoom::invite( const JID& invitee, const std::string& reason, bool cont )
  {
    if( !m_parent || !m_joined )
      return;

    Message* m = new Message( Message::Normal, m_nick.bareJID() );
    Tag* x = new Tag( m, "x", XMLNS, XMLNS_MUC_USER );
    Tag* i = new Tag( x, "invite", "to", invitee.bare() );
    if( !reason.empty() )
      new Tag( i, "reason", reason );
    if( cont )
      new Tag( i, "continue" );

    m_parent->send( m );
  }

  Message* MUCRoom::declineInvitation( const JID& room, const JID& invitor, const std::string& reason )
  {
    Message* m = new Message( Message::Normal, room.bare() );
    Tag* x = new Tag( m, "x", XMLNS, XMLNS_MUC_USER );
    Tag* d = new Tag( x, "decline", "to", invitor.bare() );
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

    Message* m = new Message( Message::Normal, m_nick.bareJID(), message );
    Tag* x = new Tag( m, "x", XMLNS, XMLNS_X_DELAY );
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

  Stanza* MUCRoom::createDataForm( const JID& room, const DataForm& df )
  {
    Message* m = new Message( Message::Normal, room.bare() );
    m->addChild( df.tag() );
    return m;
  }

  void MUCRoom::requestVoice()
  {
    if( !m_parent || !m_joined )
      return;

    DataForm df( TypeSubmit );
    df.addField( DataFormField::TypeNone, "FORM_TYPE", XMLNS_MUC_REQUEST );
    df.addField( DataFormField::TypeTextSingle, "muc#role", "participant", "Requested role" );

    Tag* m = new Tag( "messsage", "to", m_nick.bare() );
    m->addChild( df.tag() );

    m_parent->send( m );
  }

  void MUCRoom::modifyOccupant( const std::string& nick, int state, const std::string& roa,
                                const std::string& reason )
  {
    if( !m_parent || !m_joined || nick.empty() || roa.empty() )
      return;

    std::string newRoA;
    MUCOperation action = SetRNone;
    if( roa == "role" )
    {
      switch( state )
      {
        case RoleNone:
          newRoA = "none";
          action = SetRNone;
          break;
        case RoleVisitor:
          newRoA = "visitor";
          action = SetVisitor;
          break;
        case RoleParticipant:
          newRoA = "participant";
          action = SetParticipant;
          break;
        case RoleModerator:
          newRoA = "moderator";
          action = SetModerator;
          break;
      }
    }
    else
    {
      switch( state )
      {
        case AffiliationOutcast:
          newRoA = "outcast";
          action = SetOutcast;
          break;
        case AffiliationNone:
          newRoA = "none";
          action = SetANone;
          break;
        case AffiliationMember:
          newRoA = "member";
          action = SetMember;
          break;
        case AffiliationAdmin:
          newRoA = "admin";
          action = SetAdmin;
          break;
        case AffiliationOwner:
          newRoA = "owner";
          action = SetOwner;
          break;
      }
    }

    const std::string& id = m_parent->getID();
    IQ* k = new IQ( IQ::Set, m_nick.bareJID(), id, XMLNS_MUC_ADMIN );
    Tag* i = new Tag( k->query(), "item", "nick", nick );
    i->addAttribute( roa, newRoA );
    if( !reason.empty() )
      new Tag( i, "reason", reason );

    m_parent->trackID( this, id, action );
    m_parent->send( k );
  }

  void MUCRoom::requestList( MUCOperation operation )
  {
    if( !m_parent || !m_joined || !m_roomConfigHandler )
      return;

    const std::string& id = m_parent->getID();
    IQ* iq = new IQ( IQ::Get, m_nick.bareJID(), id, XMLNS_MUC_ADMIN );
    Tag* i = new Tag( iq->query(), "item" );

    switch( operation )
    {
      case RequestVoiceList:
        i->addAttribute( "role", "participant" );
        break;
      case RequestBanList:
        i->addAttribute( "affiliation", "outcast" );
        break;
      case RequestMemberList:
        i->addAttribute( "affiliation", "member" );
        break;
      case RequestModeratorList:
        i->addAttribute( "role", "moderator" );
        break;
      case RequestOwnerList:
        i->addAttribute( "affiliation", "owner" );
        break;
      case RequestAdminList:
        i->addAttribute( "affiliation", "admin" );
        break;
      default:
        delete i;
        return;
        break;
    }

    m_parent->trackID( this, id, operation );
    m_parent->send( iq );
  }

  void MUCRoom::storeList( const MUCListItemList items, MUCOperation operation )
  {
    if( !m_parent || !m_joined )
      return;

    std::string roa;
    std::string value;
    switch( operation )
    {
      case RequestVoiceList:
        roa = "role";
        value = "participant";
        break;
      case RequestBanList:
        roa = "affiliation";
        value = "outcast";
        break;
      case RequestMemberList:
        roa = "affiliation";
        value = "member";
        break;
      case RequestModeratorList:
        roa = "role";
        value = "moderator";
        break;
      case RequestOwnerList:
        roa = "affiliation";
        value = "owner";
        break;
      case RequestAdminList:
        roa = "affiliation";
        value = "admin";
        break;
      default:
        return;
        break;
    }

    const std::string& id = m_parent->getID();
    IQ* iq = new IQ( IQ::Set, m_nick.bareJID(), id, XMLNS_MUC_ADMIN );
    Tag* q = iq->query();
    q->addAttribute( XMLNS, XMLNS_MUC_ADMIN );

    MUCListItemList::const_iterator it = items.begin();
    for( ; it != items.end(); ++it )
    {
      if( (*it).nick.empty() )
        continue;

      Tag* i = new Tag( q, "item", "nick", (*it).nick );
      i->addAttribute( roa, value );
      if( !(*it).reason.empty() )
        new Tag( i, "reason", (*it).reason );
    }

    m_parent->trackID( this, id, operation );
    m_parent->send( iq );
  }

  void MUCRoom::handlePresence( Presence* presence )
  {
    if( ( presence->from().bare() != m_nick.bare() ) || !m_roomHandler )
      return;

    if( presence->subtype() == Presence::Error )
    {
      m_joined = false;
      m_roomHandler->handleMUCError( this, presence->error()->error() );
    }
    else
    {
      Tag* x = 0;
      if( m_roomHandler && ( x = presence->findChild( "x", XMLNS, XMLNS_MUC_USER ) ) != 0 )
      {
        MUCRoomParticipant party;
        party.flags = 0;
        party.nick = new JID( presence->from() );
        party.jid = 0;
        party.actor = 0;
        party.alternate = 0;
        const Tag* tag;
        const TagList& l = x->children();
        TagList::const_iterator it = l.begin();
        for( ; it != l.end(); ++it )
        {
          if( (*it)->name() == "item" )
          {
            party.affiliation = getEnumAffiliation( (*it)->findAttribute( "affiliation" ) );
            party.role = getEnumRole( (*it)->findAttribute( "role" ) );

            const std::string& jid = (*it)->findAttribute( "jid" );
            if( !jid.empty() )
              party.jid = new JID( jid );

            if( ( tag = (*it)->findChild( "actor" ) ) )
            {
              const std::string& actor = tag->findAttribute( "jid" );
              if( !actor.empty() )
                party.actor = new JID( actor );
            }

            if( ( tag = (*it)->findChild( "reason" ) ) )
              party.reason = tag->cdata();

            party.newNick = (*it)->findAttribute( "nick" );
          }
          else if( (*it)->name() == "status" )
          {
            const std::string& code = (*it)->findAttribute( "code" );
            if( code == "100" )
              setNonAnonymous();
            else if( code == "101" )
            {
              // affiliation changed while not in the room. not to be handled here, I guess
            }
            else if( code == "110" )
            {
              party.flags |= UserSelf;
              m_role = party.role;
              m_affiliation = party.affiliation;
            }
            else if( code == "201" )
            {
              m_creationInProgress = true;
              if( instantRoomHook() || m_roomHandler->handleMUCRoomCreation( this ) )
                acknowledgeInstantRoom();
            }
            else if( code == "210" )
              m_nick.setResource( presence->from().resource() );
            else if( code == "301" )
              party.flags |= UserBanned;
            else if( code == "303" )
              party.flags |= UserNickChanged;
            else if( code == "307" )
              party.flags |= UserKicked;
            else if( code == "321" )
              party.flags |= UserAffiliationChanged;
          }
          else if( (*it)->name() == "destroy" )
          {
            if( (*it)->hasAttribute( "jid" ) )
              party.alternate = new JID( (*it)->findAttribute( "jid" ) );

            if( ( tag = (*it)->findChild( "reason" ) ) )
              party.reason = tag->cdata();

            party.flags |= UserRoomDestroyed;
          }
        }

        if( party.flags & UserNickChanged && !party.newNick.empty()
            && m_nick.resource() == presence->from().resource()
            && party.newNick == m_newNick )
          party.flags |= UserSelf;

        if( party.flags & UserNickChanged && party.flags & UserSelf && !party.newNick.empty() )
          m_nick.setResource( party.newNick );

        party.status = presence->status();

        m_roomHandler->handleMUCParticipantPresence( this, party, presence->presence() );
        delete party.jid;
        delete party.nick;
        delete party.actor;
        delete party.alternate;
      }
    }
  }

  void MUCRoom::instantRoom( int context )
  {
    if( !m_creationInProgress || !m_parent || !m_joined )
      return;

    const std::string& id = m_parent->getID();
    IQ* iq = new IQ( IQ::Set, m_nick.bareJID(), id, XMLNS_MUC_OWNER );
    Tag* x = new Tag( iq->query(), "x", XMLNS, XMLNS_X_DATA );
    x->addAttribute( TYPE, context == CreateInstantRoom ? "submit" :"cancel" );

    m_parent->trackID( this, id, context );
    m_parent->send( iq );

    m_creationInProgress = false;
  }

  void MUCRoom::requestRoomConfig()
  {
    if( !m_parent || !m_joined )
      return;

    const std::string& id = m_parent->getID();
    IQ* iq = new IQ( IQ::Get, m_nick.bareJID(), id, XMLNS_MUC_OWNER );

    m_parent->trackID( this, id, RequestRoomConfig );
    m_parent->send( iq );

    if( m_creationInProgress )
      m_creationInProgress = false;
  }

  void MUCRoom::setNonAnonymous()
  {
    m_flags |= FlagNonAnonymous;
    m_flags ^= FlagSemiAnonymous | FlagFullyAnonymous;
  }

  void MUCRoom::setSemiAnonymous()
  {
    m_flags ^= FlagNonAnonymous | FlagFullyAnonymous;
    m_flags |= FlagSemiAnonymous;
  }

  void MUCRoom::setFullyAnonymous()
  {
    m_flags ^= FlagNonAnonymous | FlagSemiAnonymous;
    m_flags |= FlagFullyAnonymous;
  }

  void MUCRoom::handleMessage( Message* msg, MessageSession * /*session*/ )
  {
    if( !m_roomHandler )
      return;

    if( msg->subtype() == Message::Error )
    {
      m_roomHandler->handleMUCError( this, msg->error()->error() );
    }
    else
    {
      Tag* x;
      if( ( x = msg->findChild( "x", XMLNS, XMLNS_MUC_USER ) ) != 0 )
      {
        const TagList& l = x->children();
        TagList::const_iterator it = l.begin();
        for( ; it != l.end(); ++it )
        {
          if( (*it)->name() == "status" )
          {
            const std::string& code = (*it)->findAttribute( "code" );
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
            m_roomHandler->handleMUCInviteDecline( this, invitee, reason );
            return;
          }
          // call some handler?
        }
      }
      else if( m_roomConfigHandler && ( x = msg->findChild( "x", XMLNS, XMLNS_X_DATA ) ) != 0 )
      {
        m_roomConfigHandler->handleMUCRequest( this, DataForm( x ) );
        return;
      }

      if( !msg->subject().empty() )
      {
        m_roomHandler->handleMUCSubject( this, msg->from().resource(), msg->subject() );
      }
      else if( !msg->body().empty() )
      {
        JID from;
        std::string when;
        bool privMsg = false;
        bool history = false;
        if( ( x = msg->findChild( "x", XMLNS, XMLNS_X_DELAY ) ) != 0 )
        {
          from.setJID( x->findAttribute( "from" ) );
          when = x->findAttribute( "when" );
          history = true;
        }
        if( msg->subtype() & ( Message::Chat | Message::Normal ) )
          privMsg = true;

        m_roomHandler->handleMUCMessage( this, msg->from().resource(), msg->body(),
                                          history, when, privMsg );
        m_roomHandler->handleMUCMessage( this, *msg, privMsg );
      }
    }
  }

  void MUCRoom::handleIqID( IQ* iq, int context )
  {
    if( !m_roomConfigHandler )
      return;

    switch( iq->subtype() )
    {
      case IQ::Result:
        handleIqResult( iq, context );
        break;
      case IQ::Error:
        handleIqError( iq, context );
        break;
      default:
        break;
    }
  }

  void MUCRoom::handleIqResult( IQ* iq, int context )
  {
    switch( context )
    {
      case SetRNone:
      case SetVisitor:
      case SetParticipant:
      case SetModerator:
      case SetANone:
      case SetOutcast:
      case SetMember:
      case SetAdmin:
      case SetOwner:
      case CreateInstantRoom:
      case CancelRoomCreation:
      case DestroyRoom:
      case StoreVoiceList:
      case StoreBanList:
      case StoreMemberList:
      case StoreModeratorList:
      case StoreAdminList:
        m_roomConfigHandler->handleMUCConfigResult( this, true, (MUCOperation)context );
        break;
      case RequestRoomConfig:
      {
        const Tag* q = iq->findChild( "query", XMLNS, XMLNS_MUC_OWNER );
        if( q )
        {
          const Tag* x = q->findChild( "x", XMLNS, XMLNS_X_DATA );
          if( x )
          {
            const DataForm df( x );
            m_roomConfigHandler->handleMUCConfigForm( this, df );
          }
        }
        break;
      }
      case RequestVoiceList:
      case RequestBanList:
      case RequestMemberList:
      case RequestModeratorList:
      case RequestOwnerList:
      case RequestAdminList:
      {
        const Tag* q = iq->findChild( "query", XMLNS, XMLNS_MUC_OWNER );
        if( q )
        {
          const Tag* x = q->findChild( "x", XMLNS, XMLNS_X_DATA );
          if( x )
          {
            MUCListItemList itemList;
            const TagList& items = x->findChildren( "item" );
            TagList::const_iterator it = items.begin();
            for( ; it != items.end(); ++it )
            {
              MUCListItem item;
              item.jid = 0;
              item.role = getEnumRole( (*it)->findAttribute( "role" ) );
              item.affiliation = getEnumAffiliation( (*it)->findAttribute( "affiliation" ) );
              if( (*it)->hasAttribute( "jid" ) )
                item.jid = new JID( (*it)->findAttribute( "jid" ) );
              item.nick = (*it)->findAttribute( "nick" );
              itemList.push_back( item );
            }
            m_roomConfigHandler->handleMUCConfigList( this, itemList, (MUCOperation)context );

            MUCListItemList::iterator itl = itemList.begin();
            for( ; itl != itemList.end(); ++itl )
              delete (*itl).jid;
          }
        }
        break;
      }
      default:
        break;
    }
  }

  void MUCRoom::handleIqError( IQ* /*iq*/, int context )
  {
    switch( context )
    {
      case SetRNone:
      case SetVisitor:
      case SetParticipant:
      case SetModerator:
      case SetANone:
      case SetOutcast:
      case SetMember:
      case SetAdmin:
      case SetOwner:
      case CreateInstantRoom:
      case CancelRoomCreation:
      case RequestRoomConfig:
      case DestroyRoom:
      case RequestVoiceList:
      case StoreVoiceList:
      case RequestBanList:
      case StoreBanList:
      case RequestMemberList:
      case StoreMemberList:
      case RequestModeratorList:
      case StoreModeratorList:
      case RequestOwnerList:
      case StoreOwnerList:
      case RequestAdminList:
      case StoreAdminList:
        m_roomConfigHandler->handleMUCConfigResult( this, false, (MUCOperation)context );
        break;
    }
  }

  void MUCRoom::handleDiscoInfoResult( IQ* iq, int context )
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
        const DataForm* df = 0;
        const Tag* q = iq->query();
        if( q )
        {
          const TagList& l = q->children();
          TagList::const_iterator it = l.begin();
          for( ; it != l.end(); ++it )
          {
            if( (*it)->name() == "feature" )
            {
              const std::string& var = (*it)->findAttribute( "var" );
              if( var == "muc_hidden" )
                m_flags |= FlagHidden;
              else if( var == "muc_membersonly" )
                m_flags |= FlagMembersOnly;
              else if( var == "muc_moderated" )
                m_flags |= FlagModerated;
              else if( var == "muc_nonanonymous" )
                setNonAnonymous();
              else if( var == "muc_open" )
                m_flags |= FlagOpen;
              else if( var == "muc_passwordprotected" )
                m_flags |= FlagPasswordProtected;
              else if( var == "muc_persistent" )
                m_flags |= FlagPersistent;
              else if( var == "muc_public" )
                m_flags |= FlagPublic;
              else if( var == "muc_semianonymous" )
                setSemiAnonymous();
              else if( var == "muc_temporary" )
                m_flags |= FlagTemporary;
              else if( var == "muc_fullyanonymous" )
                setFullyAnonymous();
              else if( var == "muc_unmoderated" )
                m_flags |= FlagUnmoderated;
              else if( var == "muc_unsecured" )
                m_flags |= FlagUnsecured;
            }
            else if( (*it)->name() == "identity" )
            {
              name = (*it)->findAttribute( "name" );
            }
            else if( (*it)->name() == "x" && (*it)->hasAttribute( XMLNS, XMLNS_X_DATA ) )
            {
              df = new DataForm( (*it) );
            }
          }
        }
        if( m_roomHandler )
          m_roomHandler->handleMUCInfo( this, m_flags, name, df );
        break;
      }
      default:
        break;
    }
  }

  void MUCRoom::handleDiscoItemsResult( IQ* iq, int context )
  {
    if( !m_roomHandler )
      return;

    switch( context )
    {
      case GetRoomItems:
      {
        const Tag* q = iq->query();
        if( q )
        {
          StringMap items;
          const TagList& l = q->children();
          TagList::const_iterator it = l.begin();
          for( ; it != l.end(); ++it )
          {
            if( (*it)->name() == "item" && (*it)->hasAttribute( "jid" ) )
              items[(*it)->findAttribute( "name" )] = (*it)->findAttribute( "jid" );
          }
          m_roomHandler->handleMUCItems( this, items );
        }
        break;
      }
      default:
        break;
    }
  }

  void MUCRoom::handleDiscoError( IQ * /*iq*/, int context )
  {
    if( !m_roomHandler )
      return;

    switch( context )
    {
      case GetRoomInfo:
        m_roomHandler->handleMUCInfo( this, 0, "", 0 );
        break;
      case GetRoomItems:
        m_roomHandler->handleMUCItems( this, StringMap() );
        break;
      default:
        break;
    }
  }

  StringList MUCRoom::handleDiscoNodeFeatures( const std::string& /*node*/ )
  {
    return StringList();
  }

  StringMap MUCRoom::handleDiscoNodeIdentities( const std::string& /*node*/, std::string& /*name*/ )
  {
    return StringMap();
  }

  DiscoNodeItemList MUCRoom::handleDiscoNodeItems( const std::string& node )
  {
    DiscoNodeItemList l;

    if( node == XMLNS_MUC_ROOMS && m_publish )
    {
      DiscoNodeItem item;
      item.jid = m_nick.bare();
      if( m_publishNick )
        item.name = m_nick.resource();
      l.push_back( item );
    }
    return l;
  }

  MUCRoomRole MUCRoom::getEnumRole( const std::string& role )
  {
    if( role == "moderator" )
      return RoleModerator;
    if( role == "participant" )
      return RoleParticipant;
    if( role == "visitor" )
      return RoleVisitor;
    return RoleNone;
  }

  MUCRoomAffiliation MUCRoom::getEnumAffiliation( const std::string& affiliation )
  {
    if( affiliation == "owner" )
      return AffiliationOwner;
    if( affiliation == "admin" )
      return AffiliationAdmin;
    if( affiliation == "member" )
      return AffiliationMember;
    if( affiliation == "outcast" )
      return AffiliationOutcast;
    return AffiliationNone;
  }

}
