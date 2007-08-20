/*
  Copyright (c) 2004-2007 by Jakob Schroeter <js@camaya.net>
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
#include "rosterlistener.h"
#include "privatexml.h"
#include "util.h"
#include "stanzaextension.h"
#include "capabilities.h"


namespace gloox
{

  RosterManager::RosterManager( ClientBase* parent )
    : m_rosterListener( 0 ), m_parent( parent ), m_privateXML( 0 ),
      m_syncSubscribeReq( false )
  {
    if( m_parent )
    {
      m_parent->registerIqHandler( this, XMLNS_ROSTER );
      m_parent->registerPresenceHandler( this );
      m_parent->registerSubscriptionHandler( this );

      m_self = new RosterItem( m_parent->jid().bare() );
      m_privateXML = new PrivateXML( m_parent );
    }
  }

  RosterManager::~RosterManager()
  {
    if( m_parent )
    {
      m_parent->removeIqHandler( this, XMLNS_ROSTER );
      m_parent->removePresenceHandler( this );
      m_parent->removeSubscriptionHandler( this );
      delete m_self;
      delete m_privateXML;
    }

    util::clear( m_roster );
  }

  Roster* RosterManager::roster()
  {
    return &m_roster;
  }

  void RosterManager::fill()
  {
    m_privateXML->requestXML( "roster", XMLNS_ROSTER_DELIMITER, this );
    m_parent->send( new IQ( IQ::Get, JID(), m_parent->getID(), XMLNS_ROSTER ) );
  }

  bool RosterManager::handleIq( IQ* iq )
  {
    if( iq->subtype() == IQ::Result ) // initial roster
    {
      extractItems( iq, false );

      if( m_rosterListener )
        m_rosterListener->handleRoster( m_roster );

      m_parent->rosterFilled();
    }
    else if( iq->subtype() == IQ::Set ) // roster item push
    {
      extractItems( iq, true );
      m_parent->send( new IQ( IQ::Result, JID(), iq->id() ) );
    }
    else if( iq->subtype() == IQ::Error )
    {
      if( m_rosterListener )
        m_rosterListener->handleRosterError( iq );
      return false;
    }

    return true;
  }

  void RosterManager::handleIqID( IQ* /*iq*/, int /*context*/ )
  {
  }

  void RosterManager::handlePresence( Presence* presence )
  {
    if( presence->subtype() == Presence::Error )
      return;

    std::string capsNode;
    std::string capsVer;
    const StanzaExtension* caps = presence->findExtension( ExtCaps );
    if( caps )
    {
      capsNode = static_cast<const Capabilities*>( caps )->node();
      capsVer = static_cast<const Capabilities*>( caps )->ver();
    }

    Roster::iterator it = m_roster.find( presence->from().bare() );
    if( it != m_roster.end() )
    {
      const std::string& resource = presence->from().resource();
      RosterItem* ri = (*it).second;
      if( presence->presence() == Presence::Unavailable )
        ri->removeResource( resource );
      else
      {
        ri->setPresence( resource, presence->presence() );
        ri->setStatus( resource, presence->status() );
        ri->setPriority( resource, presence->priority() );
        ri->setCaps( resource, capsNode, capsVer );
      }

      if( m_rosterListener )
        m_rosterListener->handleRosterPresence( *ri, resource,
                                                presence->presence(), presence->status() );
    }
    else if( presence->from().bare() == m_self->jid() )
    {
      if( presence->presence() == Presence::Unavailable )
        m_self->removeResource( presence->from().resource() );
      else
      {
        m_self->setPresence( presence->from().resource(), presence->presence() );
        m_self->setStatus( presence->from().resource(), presence->status() );
        m_self->setPriority( presence->from().resource(), presence->priority() );
        m_self->setCaps( presence->from().resource(), capsNode, capsVer );
      }

      if( m_rosterListener )
        m_rosterListener->handleSelfPresence( *m_self, presence->from().resource(),
                                               presence->presence(), presence->status() );
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
    if( jid.empty() )
      return;

    add( jid, name, groups );

    Subscription* s = new Subscription( Subscription::Subscribe, jid.bareJID(), msg );
    m_parent->send( s );
  }


  void RosterManager::add( const JID& jid, const std::string& name, const StringList& groups )
  {
    if( jid.empty() )
      return;

    const std::string& id = m_parent->getID();

    IQ* iq = new IQ( IQ::Set, JID(), id, XMLNS_ROSTER );
    Tag* i = new Tag( iq->query(), "item", "jid", jid.bare() );
    if( !name.empty() )
      i->addAttribute( "name", name );

    if( groups.size() != 0 )
    {
      StringList::const_iterator it = groups.begin();
      for( ; it != groups.end(); ++it )
        new Tag( i, "group", (*it) );
    }

    m_parent->send( iq );
  }

  void RosterManager::unsubscribe( const JID& jid, const std::string& msg )
  {
    Subscription* p = new Subscription( Subscription::Unsubscribe, jid.bareJID(), msg );
    m_parent->send( p );
  }

  void RosterManager::cancel( const JID& jid, const std::string& msg )
  {
    Subscription* p = new Subscription( Subscription::Unsubscribed, jid.bareJID(), msg );
    m_parent->send( p );
  }

  void RosterManager::remove( const JID& jid )
  {
    const std::string& id = m_parent->getID();

    IQ* iq = new IQ( IQ::Set, JID(), id, XMLNS_ROSTER );
    Tag* i = new Tag( iq->query(), "item", "jid", jid.bare() );
    i->addAttribute( "subscription", "remove" );

    m_parent->send( iq );
  }

  void RosterManager::synchronize()
  {
    Roster::const_iterator it = m_roster.begin();
    for( ; it != m_roster.end(); ++it )
    {
      if( (*it).second->changed() )
      {
        const std::string& id = m_parent->getID();

        IQ* iq = new IQ( IQ::Set, JID(), id, XMLNS_ROSTER );
        Tag* i = new Tag( iq->query(), "item", "jid", (*it).second->jid() );
        if( !(*it).second->name().empty() )
          i->addAttribute( "name", (*it).second->name() );

        if( (*it).second->groups().size() != 0 )
        {
          StringList::const_iterator g_it = (*it).second->groups().begin();
          for( ; g_it != (*it).second->groups().end(); ++g_it )
            new Tag( i, "group", (*g_it) );
        }

        m_parent->send( iq );
      }
    }
  }

  void RosterManager::ackSubscriptionRequest( const JID& to, bool ack )
  {
    Subscription* p = new Subscription( ack ? Subscription::Subscribed : Subscription::Unsubscribed,
                                        to.bareJID() );
    m_parent->send( p );
  }

  void RosterManager::handleSubscription( Subscription* s10n )
  {
    if( !m_rosterListener )
      return;

    switch( s10n->subtype() )
    {
      case Subscription::Subscribe:
      {
        bool answer = m_rosterListener->handleSubscriptionRequest( s10n->from(), s10n->status() );
        if( m_syncSubscribeReq )
        {
          ackSubscriptionRequest( s10n->from(), answer );
        }
        break;
      }
      case Subscription::Subscribed:
      {
//         Subscription* p = new Subscription( Subscription::Subscribe, s10n->from().bareJID() );
//         m_parent->send( p );

        m_rosterListener->handleItemSubscribed( s10n->from() );
        break;
      }

      case Subscription::Unsubscribe:
      {
        Subscription* p = new Subscription( Subscription::Unsubscribed, s10n->from().bareJID() );
        m_parent->send( p );

        bool answer = m_rosterListener->handleUnsubscriptionRequest( s10n->from(), s10n->status() );
        if( m_syncSubscribeReq && answer )
          remove( s10n->from().bare() );
        break;
      }

      case Subscription::Unsubscribed:
      {
//         Subscription* p = new Subscription( Subscription::Unsubscribe, s10n->from().bareJID() );
//         m_parent->send( p );

        m_rosterListener->handleItemUnsubscribed( s10n->from() );
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

  void RosterManager::extractItems( Tag* tag, bool isPush )
  {
    Tag* t = tag->findChild( "query" );
    const TagList& l = t->children();
    TagList::const_iterator it = l.begin();
    for( ; it != l.end(); ++it )
    {
      if( (*it)->name() == "item" )
      {
        StringList gl;
        if( (*it)->hasChild( "group" ) )
        {
          const TagList& g = (*it)->children();
          TagList::const_iterator it_g = g.begin();
          for( ; it_g != g.end(); ++it_g )
          {
            gl.push_back( (*it_g)->cdata() );
          }
        }

        const JID& jid = (*it)->findAttribute( "jid" );
        Roster::iterator it_d = m_roster.find( jid.bare() );
        if( it_d != m_roster.end() )
        {
          (*it_d).second->setName( (*it)->findAttribute( "name" ) );
          const std::string& sub = (*it)->findAttribute( "subscription" );
          if( sub == "remove" )
          {
            delete (*it_d).second;
            m_roster.erase( it_d );
            if( m_rosterListener )
              m_rosterListener->handleItemRemoved( jid );
            continue;
          }
          const std::string& ask = (*it)->findAttribute( "ask" );
          (*it_d).second->setSubscription( sub, !ask.empty() );
          (*it_d).second->setGroups( gl );
          (*it_d).second->setSynchronized();
        }
        else
        {
          const std::string& sub = (*it)->findAttribute( "subscription" );
          if( sub == "remove" )
            continue;
          const std::string& name = (*it)->findAttribute( "name" );
          const std::string& ask = (*it)->findAttribute( "ask" );

          add( jid.bare(), name, gl, sub, !ask.empty() );
        }

        if( isPush && m_rosterListener )
          m_rosterListener->handleItemAdded( jid );
      }
    }
  }

  void RosterManager::add( const std::string& jid, const std::string& name,
                           const StringList& groups, const std::string& sub, bool ask )
  {
    if( m_roster.find( jid ) == m_roster.end() )
      m_roster[jid] = new RosterItem( jid, name );

    m_roster[jid]->setSubscription( sub, ask );
    m_roster[jid]->setGroups( groups );
    m_roster[jid]->setSynchronized();
  }

  void RosterManager::setDelimiter( const std::string& delimiter )
  {
    m_delimiter = delimiter;
    Tag* t = new Tag( "roster", m_delimiter );
    t->addAttribute( XMLNS, XMLNS_ROSTER_DELIMITER );
    m_privateXML->storeXML( t, this );
  }

  void RosterManager::handlePrivateXML( const std::string& /*tag*/, Tag* xml )
  {
    m_delimiter = xml->cdata();
  }

  void RosterManager::handlePrivateXMLResult( const std::string& /*uid*/, PrivateXMLResult /*result*/ )
  {
  }

  RosterItem* RosterManager::getRosterItem( const JID& jid )
  {
    Roster::const_iterator it = m_roster.find( jid.bare() );
    return it != m_roster.end() ? (*it).second : 0;
  }

}
