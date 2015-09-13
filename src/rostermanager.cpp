/*
  Copyright (c) 2004-2015 by Jakob Schröter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "clientbase.h"
#include "rostermanager.h"
#include "disco.h"
#include "rosteritem.h"
#include "rosteritemdata.h"
#include "rosterlistener.h"
#include "privatexml.h"
#include "util.h"
#include "stanzaextension.h"
#include "capabilities.h"
#include "rosterx.h"


namespace gloox
{

  // ---- RosterManager::Query ----
  RosterManager::Query::Query( const JID& jid, const std::string& name, const StringList& groups )
    : StanzaExtension( ExtRoster )
  {
    m_roster.push_back( new RosterItemData( jid, name, groups ) );
  }

  RosterManager::Query::Query( const JID& jid )
    : StanzaExtension( ExtRoster )
  {
    m_roster.push_back( new RosterItemData( jid ) );
  }

  RosterManager::Query::Query( const Tag* tag )
    : StanzaExtension( ExtRoster )
  {
    if( !tag || tag->name() != "query" || tag->xmlns() != XMLNS_ROSTER )
      return;

    const ConstTagList& l = tag->findTagList( "query/item" );
    ConstTagList::const_iterator it = l.begin();
    for( ; it != l.end(); ++it )
    {
      StringList groups;
      const ConstTagList& g = (*it)->findTagList( "item/group" );
      ConstTagList::const_iterator it_g = g.begin();
      for( ; it_g != g.end(); ++it_g )
        groups.push_back( (*it_g)->cdata() );

      const std::string sub = (*it)->findAttribute( "subscription" );
      if( sub == "remove" )
        m_roster.push_back( new RosterItemData( JID( (*it)->findAttribute( "jid" ) ) ) );
      else
      {
        RosterItemData* rid = new RosterItemData( JID( (*it)->findAttribute( "jid" ) ),
                                                  (*it)->findAttribute( "name" ),
                                                  groups );
        rid->setSubscription( sub, (*it)->findAttribute( "ask" ) );
        m_roster.push_back( rid );
      }
    }
  }

  RosterManager::Query::~Query()
  {
    util::clearList( m_roster );
  }

  const std::string& RosterManager::Query::filterString() const
  {
    static const std::string filter = "/iq/query[@xmlns='" + XMLNS_ROSTER + "']";
    return filter;
  }

  Tag* RosterManager::Query::tag() const
  {
    Tag* t = new Tag( "query" );
    t->setXmlns( XMLNS_ROSTER );

    RosterData::const_iterator it = m_roster.begin();
    for( ; it != m_roster.end(); ++it )
      t->addChild( (*it)->tag() );

    return t;
  }

  StanzaExtension* RosterManager::Query::clone() const
  {
    Query* q = new Query();
    RosterData::const_iterator it = m_roster.begin();
    for( ; it != m_roster.end(); ++it )
    {
      q->m_roster.push_back( new RosterItemData( *(*it) ) );
    }
    return q;
  }
  // ---- ~RosterManager::Query ----

  // ---- RosterManager ----
  RosterManager::RosterManager( ClientBase* parent )
    : m_rosterListener( 0 ), m_parent( parent ),
#if !defined( GLOOX_MINIMAL ) || defined( WANT_PRIVATEXML )
    m_privateXML( 0 ),
#endif // GLOOX_MINIMAL
    m_syncSubscribeReq( false )
  {
    if( m_parent )
    {
      m_parent->registerIqHandler( this, ExtRoster );
      m_parent->registerPresenceHandler( this );
      m_parent->registerSubscriptionHandler( this );
      m_parent->registerStanzaExtension( new Query() );
#if !defined( GLOOX_MINIMAL ) || defined( WANT_ROSTER_ITEM_EXCHANGE )
      m_parent->registerStanzaExtension( new RosterX() );
      m_parent->registerIqHandler( this, ExtRosterX );
      m_parent->disco()->addFeature( XMLNS_ROSTER_X );
      #endif // GLOOX_MINIMAL

      m_self = new RosterItem( m_parent->jid().bare() );
#if !defined( GLOOX_MINIMAL ) || defined( WANT_PRIVATEXML )
      m_privateXML = new PrivateXML( m_parent );
#endif // GLOOX_MINIMAL
    }
  }

  RosterManager::~RosterManager()
  {
    if( m_parent )
    {
      m_parent->removeIqHandler( this, ExtRoster );
      m_parent->removeIDHandler( this );
      m_parent->removePresenceHandler( this );
      m_parent->removeSubscriptionHandler( this );
      m_parent->removeStanzaExtension( ExtRoster );
#if !defined( GLOOX_MINIMAL ) || defined( WANT_ROSTER_ITEM_EXCHANGE )
      m_parent->disco()->removeFeature( XMLNS_ROSTER_X );
      m_parent->removeStanzaExtension( ExtRosterX );
      m_parent->removeIqHandler( this, ExtRosterX );
      #endif // GLOOX_MINIMAL
      delete m_self;
#if !defined( GLOOX_MINIMAL ) || defined( WANT_PRIVATEXML )
      delete m_privateXML;
#endif // GLOOX_MINIMAL
    }

    util::clearMap( m_roster );
  }

  Roster* RosterManager::roster()
  {
    return &m_roster;
  }

  void RosterManager::fill()
  {
    if( !m_parent )
      return;

    util::clearMap( m_roster );
#if !defined( GLOOX_MINIMAL ) || defined( WANT_PRIVATEXML )
    m_privateXML->requestXML( "roster", XMLNS_ROSTER_DELIMITER, this );
#endif // GLOOX_MINIMAL
    IQ iq( IQ::Get, JID(), m_parent->getID() );
    iq.addExtension( new Query() );
    m_parent->send( iq, this, RequestRoster );
  }

  bool RosterManager::handleIq( const IQ& iq )
  {
    if( iq.subtype() != IQ::Set ) // FIXME add checks for 'from' attribute (empty or bare self jid?)
      return false;

    // single roster item push
    const Query* q = iq.findExtension<Query>( ExtRoster );
    if( q && q->roster().size() )
      mergePush( q->roster() );

    // Roster Item Exchange
    const RosterX* r = iq.findExtension<RosterX>( ExtRosterX );
    if( r && m_rosterListener )
      m_rosterListener->handleRosterItemExchange( iq.from(), r );

    IQ re( IQ::Result, JID(), iq.id() );
    m_parent->send( re );
    return true;
  }

#if !defined( GLOOX_MINIMAL ) || defined( WANT_ROSTER_ITEM_EXCHANGE )
  void RosterManager::handleMessage( const Message& msg, MessageSession* )
  {
    // Roster Item Exchange
    const RosterX* r = msg.findExtension<RosterX>( ExtRosterX );
    if( r && m_rosterListener )
      m_rosterListener->handleRosterItemExchange( msg.from(), r );
  }
#endif

  void RosterManager::handleIqID( const IQ& iq, int context )
  {
    if( iq.subtype() == IQ::Result ) // initial roster
    {
      const Query* q = iq.findExtension<Query>( ExtRoster );
      if( q )
        mergeRoster( q->roster() );

      if( context == RequestRoster )
      {
        if( m_parent )
          m_parent->rosterFilled();

        if( m_rosterListener )
          m_rosterListener->handleRoster( m_roster );
      }
    }
    else if( iq.subtype() == IQ::Error )
    {
      if( context == RequestRoster && m_parent )
        m_parent->rosterFilled();

      if( m_rosterListener )
        m_rosterListener->handleRosterError( iq );
    }
  }

  void RosterManager::handlePresence( const Presence& presence )
  {
    if( presence.subtype() == Presence::Error )
      return;

    bool self = false;
    Roster::iterator it = m_roster.find( presence.from().bare() );
    if( it != m_roster.end() || ( self = ( presence.from().bareJID() == m_self->jid() ) ) )
    {
      RosterItem* ri = self ? m_self : (*it).second;
      const std::string& resource = presence.from().resource();

      if( presence.presence() == Presence::Unavailable )
        ri->removeResource( resource );
      else
      {
        ri->setPresence( resource, presence.presence() );
        ri->setStatus( resource, presence.status() );
        ri->setPriority( resource, presence.priority() );
        ri->setExtensions( resource, presence.extensions() );
      }

      if( m_rosterListener && !self )
        m_rosterListener->handleRosterPresence( *ri, resource,
                                                presence.presence(), presence.status() );
      else if( m_rosterListener && self )
        m_rosterListener->handleSelfPresence( *ri, resource,
                                              presence.presence(), presence.status() );
    }
    else
    {
      if( m_rosterListener )
        m_rosterListener->handleNonrosterPresence( presence );
    }
  }

  void RosterManager::subscribe( const JID& jid, const std::string& name,
                                 const StringList& groups, const std::string& msg )
  {
    if( !jid )
      return;

    add( jid, name, groups );

    Subscription s( Subscription::Subscribe, jid.bareJID(), msg );
    m_parent->send( s );
  }


  void RosterManager::add( const JID& jid, const std::string& name, const StringList& groups )
  {
    if( !jid )
      return;

    IQ iq( IQ::Set, JID(), m_parent->getID() );
    iq.addExtension( new Query( jid, name, groups) );

    m_parent->send( iq, this, AddRosterItem );
  }

  void RosterManager::unsubscribe( const JID& jid, const std::string& msg )
  {
    Subscription p( Subscription::Unsubscribe, jid.bareJID(), msg );
    m_parent->send( p );
  }

  void RosterManager::cancel( const JID& jid, const std::string& msg )
  {
    Subscription p( Subscription::Unsubscribed, jid.bareJID(), msg );
    m_parent->send( p );
  }

  void RosterManager::remove( const JID& jid )
  {
    if( !jid )
      return;

    IQ iq( IQ::Set, JID(), m_parent->getID() );
    iq.addExtension( new Query( jid ) );

    m_parent->send( iq, this, RemoveRosterItem );
  }

  void RosterManager::synchronize()
  {
    Roster::const_iterator it = m_roster.begin();
    for( ; it != m_roster.end(); ++it )
    {
      if( !(*it).second->changed() )
        continue;

      IQ iq( IQ::Set, JID(), m_parent->getID() );
      iq.addExtension( new Query( (*it).second->jid(), (*it).second->name(), (*it).second->groups() ) );
      m_parent->send( iq, this, SynchronizeRoster );
    }
  }

  void RosterManager::ackSubscriptionRequest( const JID& to, bool ack )
  {
    Subscription p( ack ? Subscription::Subscribed
                                            : Subscription::Unsubscribed, to.bareJID() );
    m_parent->send( p );
  }

  void RosterManager::handleSubscription( const Subscription& s10n )
  {
    if( !m_rosterListener )
      return;

    switch( s10n.subtype() )
    {
      case Subscription::Subscribe:
      {
        bool answer = m_rosterListener->handleSubscriptionRequest( s10n.from(), s10n.status() );
        if( m_syncSubscribeReq )
        {
          ackSubscriptionRequest( s10n.from(), answer );
        }
        break;
      }
      case Subscription::Subscribed:
      {
        m_rosterListener->handleItemSubscribed( s10n.from() );
        break;
      }

      case Subscription::Unsubscribe:
      {
        Subscription p( Subscription::Unsubscribed, s10n.from().bareJID() );
        m_parent->send( p );

        bool answer = m_rosterListener->handleUnsubscriptionRequest( s10n.from(), s10n.status() );
        if( m_syncSubscribeReq && answer )
          remove( s10n.from().bare() );
        break;
      }

      case Subscription::Unsubscribed:
      {
        m_rosterListener->handleItemUnsubscribed( s10n.from() );
        break;
      }

      default:
        break;
    }
  }

  void RosterManager::registerRosterListener( RosterListener* rl, bool syncSubscribeReq )
  {
    m_syncSubscribeReq = syncSubscribeReq;
    m_rosterListener = rl;
  }

  void RosterManager::removeRosterListener()
  {
    m_syncSubscribeReq = false;
    m_rosterListener = 0;
  }

#if !defined( GLOOX_MINIMAL ) || defined( WANT_PRIVATEXML )
  void RosterManager::setDelimiter( const std::string& delimiter )
  {
    m_delimiter = delimiter;
    Tag* t = new Tag( "roster", m_delimiter );
    t->addAttribute( XMLNS, XMLNS_ROSTER_DELIMITER );
    m_privateXML->storeXML( t, this );
  }

  void RosterManager::handlePrivateXML( const Tag* xml )
  {
    if( xml )
      m_delimiter = xml->cdata();
  }

  void RosterManager::handlePrivateXMLResult( const std::string& /*uid*/, PrivateXMLResult /*result*/ )
  {
  }
#endif // GLOOX_MINIMAL

  RosterItem* RosterManager::getRosterItem( const JID& jid )
  {
    Roster::const_iterator it = m_roster.find( jid.bare() );
    return it != m_roster.end() ? (*it).second : 0;
  }

  void RosterManager::mergePush( const RosterData& data )
  {
    RosterData::const_iterator it = data.begin();
    for( ; it != data.end(); ++it )
    {
      Roster::iterator itr = m_roster.find( (*it)->jid().full() );
      if( itr != m_roster.end() )
      {
        if( (*it)->remove() )
        {
          if( m_rosterListener )
            m_rosterListener->handleItemRemoved( (*it)->jid().full() );
          delete (*itr).second;
          m_roster.erase( itr );
        }
        else
        {
          (*itr).second->setData( *(*it) );
          if( m_rosterListener )
            m_rosterListener->handleItemUpdated( (*it)->jid().full() );
        }
      }
      else if( !(*it)->remove() )
      {
        m_roster.insert( std::make_pair( (*it)->jid().full(), new RosterItem( *(*it) ) ) );
        if( m_rosterListener )
          m_rosterListener->handleItemAdded( (*it)->jid().full() );
      }
    }
  }

  void RosterManager::mergeRoster( const RosterData& data )
  {
    RosterData::const_iterator it = data.begin();
    for( ; it != data.end(); ++it )
      m_roster.insert( std::make_pair( (*it)->jid().full(), new RosterItem( *(*it) ) ) );
  }

}
